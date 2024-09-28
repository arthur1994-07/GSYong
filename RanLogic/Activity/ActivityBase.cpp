//
//mjeon.activities
//
#include "../pch.h"
#include <boost/lexical_cast.hpp>

#include "./ActivityBase.h"
#include "./ActivityVerifier.h"
#include "./TitleManager.h"
#include "../GLUseFeatures.h"

#include "../../SigmaCore/Compress/Unzipper.h"
#include "../../SigmaCore/Compress/Zipper.h"
#include "../../SigmaCore/Compress/ZipMan.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/Encrypt/DbEncrypt.h"
#include "../../SigmaCore/Xml/2.5.3/ticpp.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/Json/json_spirit_reader.h"
#include "../../SigmaCore/Loading/NSLoadingDirect.h"

#include "../Msg/GLContrlMsg.h"
#include "../GLogicDataMan.h"

#include "../../EngineLib/G-Logic/GLogic.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


ActivityBaseData* ActivityBaseData::SelfInstance;


ActivityBaseData* ActivityBaseData::GetInstance()
{
	static sc::DefaultLock instanceLock;

	if (SelfInstance == NULL)
	{
		instanceLock.lockOn();

		if (SelfInstance == NULL)
		{
			SelfInstance = new ActivityBaseData();
		}

		instanceLock.lockOff();
	}
	return SelfInstance;	
}

void ActivityBaseData::ReleaseInstance()
{
	static sc::DefaultLock instanceLock;	
	
	instanceLock.lockOn();

	if (SelfInstance != NULL)
	{
		SAFE_DELETE(SelfInstance);
	}

	instanceLock.lockOff();
}


ActivityBaseData::ActivityBaseData()
:m_pBuffer(NULL)
,m_bSync(false)
{
	m_pVerifier = new ActivityVerifier();
}


ActivityBaseData::~ActivityBaseData()
{
	SAFE_DELETE(m_pBuffer);
	
	SAFE_DELETE(m_pVerifier);
}


//
// plain .XML --> encrypted .BIN --> zip to .RCC
//
BOOL ActivityBaseData::xml2rcc(std::string &pathXML, std::string &pathZip, std::string &pathBin)
{	
	std::string xml;

	//
	//1.Load an XML to buffer
	//
	LoadXML(pathXML, xml);


	//
	//2. Encrypt the XML
	//
	std::string strEncrypted;
	BOOL bEncrypted = FALSE;

	bEncrypted = Encrypt(xml, strEncrypted);

	if (!bEncrypted || strEncrypted.empty())
	{
		sc::writeLogError(std::string("Encryption failure."));

		return FALSE;
	}


	//
	//3. Save to .BIN
	//
	BOOL bSaved = FALSE;

	bSaved = WriteFile(pathBin, strEncrypted);

	if (!bSaved)
	{
		sc::writeLogError(std::string("Could not saved to XML."));

		return FALSE;
	}


	//
	//4. zip to .RCC
	//
	CTime	ctCurrent;
	ctCurrent = CTime::GetCurrentTime();
	
	CString csFileName;
	//
	// TEMPORARY PATH
	//
	csFileName.Format(_T("%s-%s"), pathZip.c_str(), ctCurrent.Format(_T("%y%m%d_%H%M%S")) );
	

	CZipper zipper;

	if ( !zipper.OpenZip(csFileName.GetString()) )
	{
		sc::writeLogError(std::string("Could not create a .RCC file."));
		
		return FALSE;
	}

	if ( !zipper.AddFileToZip(pathBin.c_str(), false, NULL) )
	{
		sc::writeLogError(std::string("Could not add .BIN into .RCC file."));
		
		return FALSE;
	}

	zipper.CloseZip();	

	return TRUE;	
}


//
// to .RCC from m_mmActivities.
//
BOOL ActivityBaseData::SaveRCC(std::string &pathZip, std::string &pathBin)
{
	//
	//1. Save to .BIN
	//
	SaveBIN(pathBin);


	//
	//2. zip to .RCC
	//
	CTime	ctCurrent;
	ctCurrent = CTime::GetCurrentTime();
	
    CString csFileName = pathZip.c_str();


	CZipper zipper;

	if ( !zipper.OpenZip(csFileName.GetString()) )
	{
		sc::writeLogError(std::string("Could not create a .RCC file."));
		
		return FALSE;
	}

	if ( !zipper.AddFileToZip(pathBin.c_str(), false, NULL) )
	{
		sc::writeLogError(std::string("Could not add .BIN into .RCC file."));
		
		return FALSE;
	}

	zipper.CloseZip();
	

	return TRUE;
}


//
// to .BIN from m_mmActivities.
//
BOOL ActivityBaseData::SaveBIN(std::string &pathBin)
{
	size_t nSize = m_mmActivities.size();

	if (nSize <= 0)
	{
		sc::writeLogError(std::string("mmActivities is empty."));
		return FALSE;
	}


	//
	//1.Compose XML  from  m_mmActivities
	//
	std::string xml;

	ComposeXML(xml, TRUE);


	//
	//3. Encrypt()
	//
	std::string strEncrypted;
	BOOL bEncrypted = FALSE;

	bEncrypted = Encrypt(xml, strEncrypted);

	if (!bEncrypted || strEncrypted.empty())
	{
		sc::writeLogError(std::string("Encryption failure."));

		return FALSE;
	}


	//
	//4. Save to .BIN
	//
	BOOL bSaved = FALSE;

	bSaved = WriteFile(pathBin, strEncrypted);

	if (!bSaved)
	{
		sc::writeLogError(std::string("Could not saved to XML."));

		return FALSE;
	}

    return TRUE;
}


//
//to raw(decrypted) .XML from m_mmActivities.
//
BOOL ActivityBaseData::SaveXML(const std::string& pathXML)
{   
    std::string xml;

    if ( !ComposeXML(xml, TRUE) )
    {
        sc::writeLogError(std::string("SaveXML couldn't compose the XML."));

        return FALSE;
    }

    return WriteFile(pathXML, xml);
}


//
//to raw(decrypted) .XML from buffer.
//
BOOL ActivityBaseData::SaveXML(std::string &pathXML, IN std::string &xml)
{
	//
	// the buffer "xml" MUST be set UNICODE-tag at the header.
	//

	return WriteFile(pathXML, xml);
}

//
//to .csv   from m_mmActivities.
//
BOOL ActivityBaseData::SaveCSV(std::string &pathCSV)
{
    std::string strCSV;

    ITER_MMACTIVITIES iter;
    for (iter = m_mmActivities.begin(); iter != m_mmActivities.end(); ++iter)
    {
        Activity &actv = iter->second;

        if ( !actv.GetRewardTitleID().empty() && !actv.GetRewardTitle().empty() )
        {
            strCSV += sc::string::format( "%1%,\"%2%\"\r\n", actv.GetRewardTitleID(), actv.GetRewardTitle() );
        }

        if ( actv.GetTitleID().empty() || actv.GetTitle().empty() )
        {
            std::string strErr = sc::string::format( "Activity mid=%1% sid=%2% : Title or Title ID is Empty", actv.m_id.Mid(), actv.m_id.Sid() );
            GASSERT(!"Activity Title or Title ID is Empty");
            sc::writeLogError(strErr);
        }

        strCSV += sc::string::format( "%1%,\"%2%\"\r\n", actv.GetTitleID(), actv.GetTitle() );

        if ( actv.GetDescID().empty() || actv.GetDesc().empty() )
        {
            std::string strErr = sc::string::format( "Activity mid=%1% sid=%2% : Desc or Desc ID is Empty", actv.m_id.Mid(), actv.m_id.Sid() );
            GASSERT(!"Activity Desc or Desc ID is Empty");
            sc::writeLogError(strErr);
        }

        strCSV += sc::string::format( "%1%,\"%2%\"\r\n", actv.GetDescID(), actv.GetDesc() );
    }

    return WriteFile(pathCSV, strCSV);
}

//
// write null-terminated buffer into the file
//
BOOL ActivityBaseData::WriteFile(const std::string& strPath, const std::string& strWhat)
{
	CFile cf;	
	if ( !cf.Open(strPath.c_str(), CFile::typeBinary | CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate) )
	{
		sc::writeLogError(std::string("Could not open the file to write."));
		
		return FALSE;
	}

	cf.Write(strWhat.c_str(), (UINT) strWhat.size());
	cf.Close();

	return TRUE;
}


BOOL ActivityBaseData::Load(std::string &nameBin)
{
	if (!GLUseFeatures::GetInstance().IsUsingActivity())
	{		
		return TRUE;	//if here returns FALSE, entire GLogic loading routines will be exited.
	}

    if ( GLOGIC::UseLogicZipFile() )
    {
        return LoadRCC(GLogicData::GetInstance().GetActivityRccPath(), nameBin);
    }
    else
    {
		std::string pathBin(GLogicData::GetInstance().GetActivityRccPath());
        pathBin = pathBin.substr(0, pathBin.find_last_of("\\") + 1) + nameBin;
        return LoadBIN(pathBin);
    }
}


//
//load activities from data-file into m_mmActivities.
//
BOOL ActivityBaseData::LoadRCC(std::string &pathZip, std::string &nameBin)
{
/*
	std::string pathXML = "d:\\workshop\\ran_2005\\_bin\\data\\glogic\\activity\\export\\activity.xml";
	std::string pathRcc = "d:\\workshop\\ran_2005\\_bin\\data\\glogic\\activity\\export\\activity.rcc";
	std::string pathBin = "d:\\workshop\\ran_2005\\_bin\\data\\glogic\\activity\\export\\activity.bin";

	xml2rcc(pathXML, pathRcc, pathBin);
*/


	//
	//1. Load activity file - unzip from .RCC to memory
	//
	//CUnzipper unzipper;
	UINT size = 0;

    SAFE_DELETE( m_pBuffer );

    size = ZipMan::GetInstance().UnzipToMemory(pathZip, nameBin, m_pBuffer);

	if (size <= 0 || m_pBuffer == NULL)
	{
		sc::writeLogError(std::string("UnzipToMemory() weird."));

        SAFE_DELETE( m_pBuffer );
		return FALSE;
	}

	std::string strEncrypted;	
	strEncrypted.append((char *)m_pBuffer, size);	
	strEncrypted.append("\0");
	

	//
	//2. Decrypt()
	//
	std::string strDecrypted;
	BOOL bDecrypted = FALSE;

	bDecrypted = Decrypt(strEncrypted, strDecrypted);

	if (!bDecrypted || strDecrypted.empty())
	{
		sc::writeLogError(std::string("Decryption failure."));
        SAFE_DELETE( m_pBuffer );

		return FALSE;
	}

	//
	//3. ParseXML()
	//
	return ParseXML(strDecrypted);
}


//
//from .BIN	to m_mmActivities.
//
BOOL ActivityBaseData::LoadBIN(std::string &pathBin)
{
	std::string strEncrypted;

	//
	//1. Read the file into m_pBuffer
	//
	ReadFile(pathBin, strEncrypted);

	
	//
	//2. Decrypt
	//
	std::string strDecrypted;
	BOOL bDecrypted = FALSE;

	bDecrypted = Decrypt(strEncrypted, strDecrypted);

	if (!bDecrypted || strDecrypted.empty())
	{
		sc::writeLogError(std::string("Decryption failure."));
        SAFE_DELETE( m_pBuffer );

		return FALSE;
	}

	//
	//3. ParseXML()
	//
	return ParseXML(strDecrypted);
}


//
//from .XML to m_mmActivities.
//
BOOL ActivityBaseData::LoadXML(IN const std::string& pathXML)
{
    try
    {
		ticpp::Document doc(pathXML);
        doc.LoadFile();//, TIXML_ENCODING_UTF8);

        return ParseData(doc);
    }
    catch (ticpp::Exception& ex)
    {
		sc::writeLogError(ex.m_details);

        return FALSE;
    }
}


//
//from .XML to buffer.
//
BOOL ActivityBaseData::LoadXML(IN const std::string &pathXML, OUT std::string &xml)
{	
	if (!ReadFile(pathXML, xml))
		return FALSE;


	// Bunch of unicode info at:
    // http://www.unicode.org/faq/utf_bom.html
    // 00 00 FE FF  UTF-32, big-endian
    // FF FE 00 00 	UTF-32, little-endian
    // FE FF 		UTF-16, big-endian
    // FF FE 		UTF-16, little-endian
    // EF BB BF 	UTF-8

	if (!(xml[0] == (char)0xEF && xml[1] == (char)0xBB && xml[2] == (char)0xBF))
	{
		//ActivityBaseData only supports UTF-8.

		sc::writeLogError(std::string("LoadXML(): There is no UTF-8 tag in the file-header."));

		xml.clear();

		return FALSE;
	}

	return TRUE;
}


//
// read the file
//
BOOL ActivityBaseData::ReadFile(const std::string& strPath, OUT std::string& strTarget)
{
	CFile cf;

	if ( !cf.Open(strPath.c_str(), CFile::typeBinary | CFile::modeRead | CFile::shareExclusive) )
	{
		sc::writeLogError(std::string("Could not open the file to Read"));
		
		return FALSE;
	}

	UINT size = (UINT)cf.GetLength();
    
	m_pBuffer = new BYTE[size+1];	

	UINT nBytesRead = cf.Read(m_pBuffer, (UINT)size);

	cf.Close();

	if (nBytesRead <= 0 || nBytesRead > (UINT)size)
	{
		sc::writeLogError(std::string("Could not Read the file."));
		
		SAFE_DELETE(m_pBuffer);

		return FALSE;
	}

	strTarget.append((char *)m_pBuffer, size);
	strTarget.append("\0");

	SAFE_DELETE(m_pBuffer);

	return TRUE;
}


//
//from XML-buffer to m_mmActivities.
//
BOOL ActivityBaseData::ParseXML(std::string &xml, BOOL bVerify, ENUM_XML_ENCODING_TYPE encoded)
{
	if (xml.empty())
	{
		sc::writeLogError(std::string("xml is empty"));

		return FALSE;
	}

	//
	// Verify the XML
	//
	if (bVerify)
	{
		if (!m_pVerifier->Verify(xml, encoded))
			return FALSE;
	}

	try
	{
		ticpp::Document doc;
		
		doc.Parse(xml, true);

		return ParseData(doc);

	}
	catch ( ticpp::Exception& ex )
	{	
		sc::writeLogError(ex.m_details);

		return FALSE;
	}
}

//
// Verifier Check
//
BOOL ActivityBaseData::CheckVerifier ()
{
    std::string xml;

    if ( !ComposeXML(xml, TRUE) )
    {
        sc::writeLogError(std::string("couldn't compose the XML."));

        return FALSE;
    }

    if ( !m_pVerifier )
    {
        sc::writeLogError(std::string("m_pVerifier is NULL"));
        return FALSE;
    }

    return m_pVerifier->Verify( xml );
}

//
// parse XML document object
//
BOOL ActivityBaseData::ParseData(ticpp::Document &doc, ENUM_XML_ENCODING_TYPE encoded)
{
	CleanUp();

	try
	{
		ticpp::Element* pTop	= NULL;		
		ticpp::Element* pHead	= NULL;		
		ticpp::Element* pElem	= NULL;
		ticpp::Element* pCond	= NULL;

		std::string		strValue;	//for Numeric values.		
		WORD			wMID = 0;
		WORD			wSID = 0;
		int				nClass = 0;	
        std::vector<DWORD> vecTargetID;
		int				nRequired = 0;
		float			fVersion = 0.0;
		
        vecTargetID.reserve(4);

        // Activities
		pTop = doc.FirstChildElement("Activities");

		//
		// Version
		//
		pHead = pTop->FirstChildElement("Version");		
		strValue = pHead->GetTextOrDefault("1.0");
		fVersion = boost::lexical_cast<float> (strValue);

		//
        // Supplies
		//
		pHead = pTop->FirstChildElement("Supplies");		        
        pHead = pHead->FirstChildElement( "Supply" );

        do 
        {
            //---------------------------------------------------//
            NSLoadingDirect::FrameMoveRender( FALSE );
            //---------------------------------------------------//

            pElem = pHead->FirstChildElement( "Class" );
            strValue = pElem->GetTextOrDefault("0");
            ENUM_ACTIVITY_CLASS emClass = (ENUM_ACTIVITY_CLASS)boost::lexical_cast<int> (strValue);

            ITER_MMACTIVITYSALESITEM insertIter = m_mmActivitySalesItem.insert( std::make_pair(emClass, SACTIVITYSALESITEM()) );
            SACTIVITYSALESITEM&      sSalesItem = insertIter->second;

            pElem    = pHead->FirstChildElement( "Filename" );
            strValue = pElem->GetTextOrDefault("");
            
            if ( !sSalesItem.Load( strValue ) )
            {
                sc::writeLogError( sc::string::format( "Supply file : '%1%', load to failed", strValue) );
            }

        } while ( pHead = pHead->NextSiblingElement(false) );


		pHead = pTop->FirstChildElement("Activity");
		
		do
		{
            //---------------------------------------------------//
            NSLoadingDirect::FrameMoveRender( FALSE );
            //---------------------------------------------------//

			wMID = 0;
			wSID = 0;
            vecTargetID.clear();
			nClass = 0;
			nRequired = 0;
			
			{
				//
				// mandatory
				//
				pElem = pHead->FirstChildElement( "MID" );
				strValue = pElem->GetText();
				wMID = boost::lexical_cast<int> (strValue);

				
				pElem = pHead->FirstChildElement( "SID" );
				strValue = pElem->GetText();
				wSID = boost::lexical_cast<int> (strValue);	
				
				
				pCond = pHead->FirstChildElement( "Condition" );
				{
                    ticpp::Element* pTarget = pCond->FirstChildElement( "Target", false );
                    if ( pTarget != NULL )
                    {
                        do
                        {
                            pElem = pTarget->FirstChildElement( "TargetMID" );
                            strValue = pElem->GetTextOrDefault("0");	//some condition such like "Level" does not require SID.
                            const int targetMID =boost::lexical_cast<int> (strValue);

                            pElem = pTarget->FirstChildElement( "TargetSID" );
                            strValue = pElem->GetTextOrDefault("0");	//some condition such like "Level" does not require SID.
                            const int targetSID =boost::lexical_cast<int> (strValue);

                            vecTargetID.push_back(SNATIVEID((WORD)targetMID, (WORD)targetSID).dwID);
                        }
                        while( pTarget = pTarget->NextSiblingElement("Target", false) );
                    }
                    else
                    {
                        // 이 코드는 예전 TargetID가 1개이던 버전을 호환하기위해 남겨둔 코드;
                        pElem = pCond->FirstChildElement( "TargetMID" );
                        strValue = pElem->GetText();
                        const int targetMID =boost::lexical_cast<int> (strValue);

                        pElem = pCond->FirstChildElement( "TargetSID" );
                        strValue = pElem->GetTextOrDefault("0");	//some condition such like "Level" does not require SID.
                        const int targetSID =boost::lexical_cast<int> (strValue);

                        vecTargetID.push_back(SNATIVEID((WORD)targetMID, (WORD)targetSID).dwID);
                    }

					pElem = pCond->FirstChildElement( "Required" );
					strValue = pElem->GetText();
					nRequired = boost::lexical_cast<int> (strValue);
				}

				//SNATIVEID id((WORD)nTargetMID, (WORD)nTargetSID);

				//
				// construct a Activity-object with mandatory informations.
				//
				Activity actv(wMID, wSID, nRequired, fVersion);

                actv.m_vecTargetID = vecTargetID;

				pElem = pHead->FirstChildElement( "Class" );
				strValue = pElem->GetTextOrDefault("1");
				actv.m_nClass = (ENUM_ACTIVITY_CLASS)boost::lexical_cast<int> (strValue);

				pElem = pHead->FirstChildElement( "Notify" );
				strValue = pElem->GetTextOrDefault("1");
				actv.m_nNotifyLevel = (ENUM_ACTIVITY_NOTIFY_LEVEL)boost::lexical_cast<int> (strValue);

				pElem = pHead->FirstChildElement( "ByOrder" );
				strValue = pElem->GetTextOrDefault("0");
				actv.m_nByOrder = boost::lexical_cast<int> (strValue);

				//
				// optional for run BUT REQUIRED for play!
				//
				pElem = pHead->FirstChildElement( "RewardPoint" );
				strValue = pElem->GetTextOrDefault("0");
				actv.SetRewardPoint(boost::lexical_cast<UINT> (strValue));
				

				pElem = pHead->FirstChildElement( "RewardTitle" );
                actv.SetRewardTitleID(pElem->GetTextOrDefault(""));
				if ( !actv.GetRewardTitleID().empty() )
				{
					actv.SetRewardTitle(ID2GAMEINTEXT(actv.GetRewardTitleID().c_str()));
				}

                pElem = pHead->FirstChildElement( "Title" );			
                actv.SetTitleID(pElem->GetTextOrDefault(""));
				if ( !actv.GetTitleID().empty() )
				{
					actv.SetTitle(ID2GAMEINTEXT(actv.GetTitleID().c_str()));
				}

                pElem = pHead->FirstChildElement( "Desc" );
				actv.SetDescID(pElem->GetTextOrDefault(""));
				if ( !actv.GetDescID().empty() )
				{
					actv.SetDesc(ID2GAMEINTEXT(actv.GetDescID().c_str()));
				}


				pElem = pHead->FirstChildElement( "Validate" );
				strValue = pElem->GetTextOrDefault("0");
				actv.m_bValidate = boost::lexical_cast<BOOL> (strValue);


				//
				// insert into multi-map
				//
				m_mmActivities.insert(std::make_pair(wMID, actv));

                for( size_t i = 0; i < vecTargetID.size(); i++ )
                {
                    // set of Targets: for quick-check & fast-iteration
                    //   prepare some sets for fast-search & iterations.
                    //
                    switch(wMID)
                    {
                    case ACTIVITY_MID_KILLMOB:					
                        {
                            m_setMob.insert(vecTargetID[i]);

                            break;
                        }
                    case ACTIVITY_MID_MAP:
                        {
                            m_setMap.insert(vecTargetID[i]);

                            break;
                        }
                    case ACTIVITY_MID_KILLPC:
                        {
                            m_setMapPK.insert(vecTargetID[i]);

                            break;
                        }
                    case ACTIVITY_MID_LEVEL:
                        {
                            //m_setLevel.insert(nRequired);
                            break;
                        }
                    case ACTIVITY_MID_USEITEM:					
                        {
                            m_setItemUse.insert(vecTargetID[i]);
                            break;
                        }
                    case ACTIVITY_MID_TAKE:
                        {
                            m_setItemTake.insert(vecTargetID[i]);
                            break;
                        }

                    case ACTIVITY_MID_ACTIVITY:
                    case ACTIVITY_MID_QUEST:
                    case ACTIVITY_MID_PONIT:
                        break;

                    default:
                        break;
                    }
                }
			}			
		}
		while ( pHead = pHead->NextSiblingElement(false) );	

	}
	catch ( ticpp::Exception& ex )
	{	
		sc::writeLogError(ex.m_details);

		return FALSE;
	}

	return TRUE;
}



//
// bTagUnicode: in case of exporting to a file, Unicode Tag MUST be written first in the file.
//
BOOL ActivityBaseData::ComposeXML(OUT std::string &xml, BOOL bTagUnicode)
{
	if (bTagUnicode)
	{
		// Bunch of unicode info at:
        // http://www.unicode.org/faq/utf_bom.html    
	    // 00 00 FE FF  UTF-32, big-endian
	    // FF FE 00 00 	UTF-32, little-endian
	    // FE FF 		UTF-16, big-endian
	    // FF FE 		UTF-16, little-endian
	    // EF BB BF 	UTF-8
		
		xml += (char)0xEF;
		xml += (char)0xBB;
		xml += (char)0xBF;	
	}

	xml += std::string(			"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"\
								"\r\n");

    // Activities
    {
        xml.append(	
								"<Activities>\r\n"		                        \
                                "\r\n"
                                );
		{
			//
			// Version
			//
			xml += sc::string::format(
                                "	<Version>\r\n"								\
								"		%1.1f\r\n"								\
								"	</Version>\r\n"								\
								"\r\n"											\
								, ACTIVITY_VERSION
								);

			//
			// Supplies
			//
			xml.append(			"	<Supplies>\r\n"		                        \
								"\r\n"
								);

			std::string strSalesItem;
			ITER_MMACTIVITYSALESITEM iter;
			for (iter = m_mmActivitySalesItem.begin(); iter != m_mmActivitySalesItem.end(); ++iter)
			{
				SACTIVITYSALESITEM& sSalesItem = iter->second;

				strSalesItem = sc::string::format(
								"		<Supply>\r\n"							\
								"			<Class>%1%</Class>\r\n"				\
								"\r\n"                                          \
								"			<Filename>%2%</Filename>\r\n"		\
								"		</Supply>\r\n"							\
								"\r\n"
								, ENUM_ACTIVITY_CLASS( iter->first )
								, sSalesItem.strFILENAME
								);

				xml += strSalesItem;
			}

			xml.append(			"	</Supplies>\r\n"							\
								"\r\n"
								);
		}


        std::string strActv;
        ITER_MMACTIVITIES iter;
	    for (iter = m_mmActivities.begin(); iter != m_mmActivities.end(); ++iter)
	    {
		    Activity &actv = iter->second;

		    xml += sc::string::format(
							    "	<Activity>\r\n"								\
							    "		<MID>%d</MID>\r\n"						\
							    "\r\n"											\
							    "		<SID>%d</SID>\r\n"						\
							    "\r\n"											\
							    "		<Class>%d</Class>\r\n"					\
							    "\r\n"											\
							    "		<Notify>%d</Notify>\r\n"				\
							    "\r\n"											\
							    "		<ByOrder>%d</ByOrder>\r\n"				\
							    "\r\n"                                          \
                                "		<Condition>\r\n"
                                ,(UINT)actv.m_id.wMainID
                                ,(UINT)actv.m_id.wSubID
                                ,(UINT)actv.m_nClass
                                ,(UINT)actv.m_nNotifyLevel
                                ,(UINT)actv.m_nByOrder
                                );

            for ( size_t i = 0; i < actv.m_vecTargetID.size(); i++ )
            {
                SNATIVEID _targetID(actv.m_vecTargetID[i]);
                xml += sc::string::format(
                    "		    <Target>\r\n"                       \
                    "			    <TargetMID>%d</TargetMID>\r\n"	\
                    "			    <TargetSID>%d</TargetSID>\r\n"  \
                    "		    </Target>\r\n"						\
                    "\r\n"
                    ,(UINT)_targetID.wMainID
                    ,(UINT)_targetID.wSubID
                    );
            }

            xml += sc::string::format(
                                "			<Required>%d</Required>\r\n"		\
                                "\r\n"											\
                                "		</Condition>\r\n"						\
                                "\r\n"                                          \
							    "		<RewardPoint>%d</RewardPoint>\r\n"		\
							    "\r\n"											\
							    "		<RewardTitle>%s</RewardTitle>\r\n"		\
							    "\r\n"											\
							    "		<Title>%s</Title>\r\n"					\
							    "\r\n"											\
							    "		<Desc>%s</Desc>\r\n"					\
							    "\r\n"											\
							    "		<Validate>%d</Validate>\r\n"			\
							    "	</Activity>\r\n"							\
							    "\r\n"
                                ,actv.m_nValueBase
							    ,actv.GetRewardPoint()
							    ,actv.GetRewardTitleID().c_str()
							    ,actv.GetTitleID().c_str()
							    ,actv.GetDescID().c_str()
							    ,(UINT)actv.m_bValidate
							    );
	    }

        xml.append(				"</Activities>\r\n" );
    }
	
	if(xml.empty())
	{
		sc::writeLogError(std::string("Could not create XML."));
		
		return FALSE;
	}
	
	//utf8 = sc::string::ansi_to_utf8(xml);
	
	return TRUE;
}


BOOL ActivityBaseData::Encrypt(LPBYTE pPlain, std::string &strEncrypted)
{
	if(pPlain == NULL)
	{
		sc::writeLogError(std::string("Encryption failure: Byte-buffer is empty."));
		
		return FALSE;
	}

	return Encrypt(std::string((char *)pPlain), strEncrypted);
}


BOOL ActivityBaseData::Encrypt(std::string &strPlain, std::string &strEncrypted)
{
    sc::DbEncrypt Encrypt;
    return Encrypt.encrypt(strPlain, strEncrypted);
}


BOOL ActivityBaseData::Encrypt(CString &csPlain, CString &csEncrypted)
{
	std::string strEncrypted;
	BOOL bRet = FALSE;

	bRet = Encrypt(std::string(csPlain.GetString()), strEncrypted);

	csEncrypted.SetString(strEncrypted.c_str());

	return bRet;
}


BOOL ActivityBaseData::Decrypt(LPBYTE pEncrypted, std::string &strPlain)
{
	if(pEncrypted == NULL)
	{
		sc::writeLogError(std::string("Decryption failure: Byte-buffer is empty."));
		
		return FALSE;
	}

	return Decrypt(std::string((char *)pEncrypted), strPlain);
}


BOOL ActivityBaseData::Decrypt(std::string &strEncrypted, std::string &strPlain)
{	
    sc::DbEncrypt Decrypt;
    return Decrypt.decrypt(strEncrypted, strPlain);
}


BOOL ActivityBaseData::Decrypt(CString &csEncrypted, CString &csPlain)
{
	std::string strPlain;
	BOOL bRet = FALSE;

	bRet = Decrypt(std::string(csEncrypted.GetString()), strPlain);

	csPlain.SetString(strPlain.c_str());

	return bRet;
}

Activity* ActivityBaseData::GetActivity( SNATIVEID sID )
{
	ITER_MMACTIVITIES pos;

    for ( pos = m_mmActivities.lower_bound(sID.wMainID); pos != m_mmActivities.upper_bound(sID.wMainID); ++pos )
    {
        Activity* pActivity = &pos->second;
        if ( pActivity->m_id == sID ) 
            return pActivity;
    }

    return NULL;
}

Activity* ActivityBaseData::GetActivity( DWORD dwID )
{
	SNATIVEID sID(dwID);

	ITER_MMACTIVITIES pos;

    for ( pos = m_mmActivities.lower_bound(sID.wMainID); pos != m_mmActivities.upper_bound(sID.wMainID); ++pos )
    {
        Activity* pActivity = &pos->second;
        if ( pActivity->m_id == sID ) 
            return pActivity;
    }

    return NULL;
}


bool ActivityBaseData::InsertActivity( const Activity& cActivity )
{
    //  이미 존재하는 ID
    if ( GetActivity( cActivity.m_id ) )
    {
        return false;
    }

    m_mmActivities.insert( std::make_pair( cActivity.m_id.wMainID, cActivity ) );

    return true;
}


bool ActivityBaseData::DeleteActivity( SNATIVEID sID )
{
    ITER_MMACTIVITIES pos;

    for ( pos = m_mmActivities.lower_bound(sID.wMainID); pos != m_mmActivities.upper_bound(sID.wMainID); ++pos )
    {
        Activity* pActivity = &pos->second;
        if ( pActivity->m_id == sID ) 
        {
            m_mmActivities.erase( pos );
            return true;
        }
    }

    return false;
}


void ActivityBaseData::CleanUp()
{
    m_mmActivities.clear();
    m_mmActivitySalesItem.clear();
}


void ActivityBaseData::ResetData()
{
    ITER_MMACTIVITIES pos = m_mmActivities.begin();
    ITER_MMACTIVITIES pos_end = m_mmActivities.end();

    DWORD dwCount = 0;

    for ( ; pos != pos_end; ++pos )
    {
        Activity* pActivity = &pos->second;
        
        pActivity->ResetData();
    } 

    m_bSync = false;
}


BOOL ActivityBaseData::SyncActivityStatus(GLMSG::SNET_ACTIVITY_SYNC *pMsg)
{
	if (!pMsg)
		return FALSE;

	UINT			nSync	= pMsg->nSync;
	ACTIVITY_SYNC	*pData	= pMsg->GetData();
	Activity		*pa		= NULL;

	UINT			idx		= 0;

	for (idx = 0; idx < nSync; idx++)
	{
		pa = GetActivity(pData[idx].dwID);

		if (pa)
		{
			pa->SetStatus((ENUM_ACTIVITY_STATUS)pData[idx].dwStatus, FALSE);	//status
			pa->SetValue(pData[idx].dwValue, FALSE);							//value

			pa = NULL;
		}
		else
		{
			sc::writeLogError(
				sc::string::format("SyncActivityStatus(): %th object has wrong ActivityID.", idx+1));
			continue;
		}
	}

    if ( pMsg->wSeq == pMsg->wLastSeq )
    {
        m_bSync = true;
    }

	return (idx == nSync);
}


DWORD ActivityBaseData::GetComplateCount( ENUM_ACTIVITY_CLASS emClass )
{
    ITER_MMACTIVITIES pos = m_mmActivities.begin();
    ITER_MMACTIVITIES pos_end = m_mmActivities.end();

    DWORD dwCount = 0;

    for ( ; pos != pos_end; ++pos )
    {
        Activity* pActivity = &pos->second;
        
        if ( pActivity->m_nClass == emClass &&
            pActivity->GetStatus() == ACTIVITY_STATUS_COMPLETED ) 
        {
            dwCount++;
        }
    }   

    return dwCount;
}

