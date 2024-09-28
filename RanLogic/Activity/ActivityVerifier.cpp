#include "../pch.h"
#include <boost/lexical_cast.hpp>

#include "./ActivityVerifier.h"

#include "../../SigmaCore/Util/Lock.h"

#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/Xml/2.5.3/ticpp.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringUtil.h"

#include "../../EngineLib/GUInterface/GameTextControl.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


ActivityVerifier::ActivityVerifier()
:m_bVerifiedOK(TRUE)
,m_sActID(false)
{
}


ActivityVerifier::~ActivityVerifier()
{
}


BOOL ActivityVerifier::Verify(std::string &xml, ENUM_XML_ENCODING_TYPE encoded)
{
	if (xml.empty())
	{
		sc::writeLogError(std::string("xml is empty"));

		return FALSE;
	}

    Clear();

	try
	{
		ticpp::Document doc;

		//
		//even though, the xml header includes "encoding utf-8" in it,
		// at this time it MUST be parsed as ansi because we already converted it to ansi before parsing.
		//
		
		//doc.Parse(xml, true ,TIXML_ENCODING_LEGACY);
		doc.Parse(xml, true);	// ,TIXML_ENCODING_LEGACY);

        ticpp::Element* pTop	= NULL;		
		ticpp::Element* pHead	= NULL;		
		ticpp::Element* pElem	= NULL;
		ticpp::Element* pCond	= NULL;

		float			fVersion= 0.0;		
		int				nValue	= 0;
		std::string		strValue;
		std::string		strTemp;

        pTop = doc.FirstChildElement("Activities");

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
            pElem = pHead->FirstChildElement( "Class" );
            strValue = pElem->GetTextOrDefault("0");
            nValue = boost::lexical_cast<int> (strValue);
            VerifyClass(nValue);

            pElem    = pHead->FirstChildElement( "Filename" );
            strValue = pElem->GetTextOrDefault("");

            SACTIVITYSALESITEM sSalesItem;
            if ( !sSalesItem.Load( strValue ) )
            {
                Issue ( sc::string::format( "Verify Failed - Supply file '%1%', load to failed", strValue) );
            }

        } while ( pHead = pHead->NextSiblingElement(false) );

        //
        // Activity
        //
        pHead = pTop->FirstChildElement("Activity");

		do
		{
			nValue = 0;
			
			{
				WORD wMID = 0;
				WORD wSID = 0;
				//
				// mandatory
				//
				pElem = pHead->FirstChildElement( "MID" );
				strValue = pElem->GetText();
				nValue = boost::lexical_cast<int> (strValue);
				VerifyMID(nValue);

				wMID = (WORD)nValue;
				

				pElem = pHead->FirstChildElement( "SID" );
				strValue = pElem->GetText();
				nValue = boost::lexical_cast<int> (strValue);
				VerifySID(nValue);

				wSID = (WORD)nValue;


				VerifyDuplicatedID(SNATIVEID(wMID, wSID).dwID);
                m_sActID = SNATIVEID(wMID, wSID);
				
				
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
                            VerifyTargetMID(targetMID);

                            pElem = pTarget->FirstChildElement( "TargetSID" );
                            strValue = pElem->GetTextOrDefault("0");	//some condition such like "Level" does not require SID.
                            const int targetSID =boost::lexical_cast<int> (strValue);
                            VerifyTargetMID(targetSID);
                        }
                        while( pTarget = pTarget->NextSiblingElement("Target", false) );
                    }
                    else
                    {
                        // 이 코드는 예전 TargetID가 1개이던 버전을 호환하기위해 남겨둔 코드;
					    pElem = pCond->FirstChildElement( "TargetMID" );
					    strValue = pElem->GetText();
					    nValue = boost::lexical_cast<int> (strValue);
					    VerifyTargetMID(nValue);

					    pElem = pCond->FirstChildElement( "TargetSID" );
					    strValue = pElem->GetTextOrDefault("0");	//some condition such like "Level" does not require SID.
					    nValue = boost::lexical_cast<int> (strValue);
					    VerifyTargetSID(nValue);
                    }

					pElem = pCond->FirstChildElement( "Required" );
					strValue = pElem->GetText();
					nValue = boost::lexical_cast<int> (strValue);
					VerifyRequired(nValue);
				}

				
				pElem = pHead->FirstChildElement( "Class" );
				strValue = pElem->GetTextOrDefault("1");
				nValue = boost::lexical_cast<int> (strValue);
				VerifyClass(nValue);


				pElem = pHead->FirstChildElement( "Notify" );
				strValue = pElem->GetTextOrDefault("1");
				nValue = boost::lexical_cast<int> (strValue);
				VerifyNotify(nValue);


				pElem = pHead->FirstChildElement( "ByOrder" );
				strValue = pElem->GetTextOrDefault("0");
				nValue = boost::lexical_cast<int> (strValue);
				VerifyByOrder(nValue);


				//
				// optional for run BUT REQUIRED for play!
				//
				pElem = pHead->FirstChildElement( "RewardPoint" );
				strValue = pElem->GetTextOrDefault("0");
				nValue = boost::lexical_cast<UINT> (strValue);
				VerifyRewardPoint(nValue);
								
				pElem = pHead->FirstChildElement( "RewardTitle" );
				strTemp = pElem->GetTextOrDefault("");
				VerifyRewardTitleID(strTemp);

                if ( !strTemp.empty() )
                {
                    strTemp = ID2GAMEINTEXT( strTemp.c_str() );
                    VerifyRewardTitle(strTemp);
                }

				pElem = pHead->FirstChildElement( "Title" );
				strTemp = pElem->GetTextOrDefault("");
				VerifyTitleID(strTemp);
                VerifyDuplicatedTitle(strTemp);
                strTemp = ID2GAMEINTEXT( strTemp.c_str() );
                VerifyTitle(strTemp);

				pElem = pHead->FirstChildElement( "Desc" );
				strTemp = pElem->GetTextOrDefault("");
                VerifyDescID(strTemp);
                strTemp = ID2GAMEINTEXT( strTemp.c_str() );
                VerifyDesc(strTemp);

				pElem = pHead->FirstChildElement( "Validate" );
				strValue = pElem->GetTextOrDefault("0");
				nValue = boost::lexical_cast<BOOL> (strValue);
                VerifyValidate(nValue);
			}			
		}
		while ( pHead = pHead->NextSiblingElement(false) );	

	}
	catch ( ticpp::Exception& ex )
	{	
		sc::writeLogError(ex.m_details);

		return FALSE;
	}

    if ( !GetIssues().empty() )
    {
        std::for_each(
            GetIssues().begin(),
            GetIssues().end(),
            &sc::writeLogError);
        return FALSE;
    }

	return TRUE;
}


void ActivityVerifier::Issue(std::string &strIssue)
{
	m_vecIssues.push_back(strIssue);
}


void ActivityVerifier::GetIssues(std::string &strIssues)
{
	if (m_vecIssues.empty())
		return;

	ITER_VECISSUES iter;

	for (iter = m_vecIssues.begin(); iter != m_vecIssues.end(); ++iter)
	{
		strIssues += *iter;
	}
}


void ActivityVerifier::Clear()
{
    m_bVerifiedOK = FALSE;
    m_sActID = NATIVEID_NULL();

    m_vecIssues.clear();
    m_setIDs.clear();
    m_setTitles.clear();
}

BOOL ActivityVerifier::VerifyDuplicatedID(DWORD dwID)
{
	BOOL bUnique = m_setIDs.insert(dwID).second;
	if (!bUnique)
	{
		m_bVerifiedOK &= FALSE;

		Issue(
			sc::string::format(
			"Verify Failed - MID:%d, SID:%d -> (Duplicated!)\n"
					,(WORD)(0xffff & dwID), (WORD)(0xffff & (dwID >> 16)) ) );
	}

	return bUnique;
}


BOOL ActivityVerifier::VerifyDuplicatedTitle(std::string strTitle)
{
	BOOL bUnique = m_setTitles.insert(strTitle).second;
	if (!bUnique)
	{
		m_bVerifiedOK &= FALSE;

		Issue(
			sc::string::format(
			"Verify Failed - ID(%d,%d) Title:%s -> (Duplicated!)\n"
					,m_sActID.wMainID, m_sActID.wSubID ,strTitle.c_str() ) );
	}

	return bUnique;
}


BOOL ActivityVerifier::VerifyNumeric(int nValue, int nMin, int nMax, std::string &hint)
{
	BOOL bResult = (nValue >= nMin) && (nValue <= nMax);

	if (!bResult)
	{
		m_bVerifiedOK &= FALSE;

		Issue(
			sc::string::format(
            "Verify Failed - ID(%d,%d) %s:%d -> (Min:%d, Max:%d)\n"
					,m_sActID.wMainID, m_sActID.wSubID, hint.c_str(), nValue, nMin, nMax) );
	}

	return bResult;
}


BOOL ActivityVerifier::VerifyStringLength(std::string &strValue, int nMaxLength, std::string &hint)
{
	BOOL bResult = ( strValue.length() <= (UINT)nMaxLength );
	
	if (!bResult)
	{
		m_bVerifiedOK &= FALSE;

		Issue(
			sc::string::format(
				"Verify Failed - ID(%d,%d) %s:%s -> (MaxLength:%d)\n"
					,m_sActID.wMainID, m_sActID.wSubID, hint.c_str(), strValue.c_str(), nMaxLength) );
	}

	return bResult;
}
