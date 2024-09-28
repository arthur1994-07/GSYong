#include "../pch.h"
#include "../../SigmaCore/Util/SystemInfo.h"
#include "../../SigmaCore/File/FileUtil.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/String/StringUtils.h"
#include "../../SigmaCore/Loading/NSLoadingDirect.h"
#include "../../SigmaCore/Xml/2.5.2/tinyxml.h"

#include "../../RanLogic/RANPARAM.h"
#include "../../RanLogic/GLUseFeatures.h"

#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/Common/SUBPATH.h"
#include "../../EngineLib/G-Logic/GLogic.h"
#include "../../EngineLib/DxTools/DxFontDefine.h"
#include "../../EngineLib/DxTools/DxFontMan.h"

#include "../GLogicData.h"
#include "../GLogicDataMan.h"

#include "./NpcDialogueSet.h"
#include "./NpcDialogue.h"
#include "./NpcTalkControl.h"
#include "./NpcTalkMan.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

NpcTalkMan::NpcTalkMan()
{
}

NpcTalkMan::~NpcTalkMan()
{
    m_DlgData.clear();
}

const std::tr1::shared_ptr<CNpcDialogueSet> NpcTalkMan::Find(const std::string& NtkFile)
{
    // 소문자로 변경
    std::string LowerNtkFile(NtkFile);
    sc::string::lowerString(LowerNtkFile);
    NPC_DLG_DATA_ITER iter = m_DlgData.find(LowerNtkFile);
    if (iter != m_DlgData.end())
    {
        return iter->second;
    }
    else
    {
        static std::tr1::shared_ptr<CNpcDialogueSet> TempData;
        return TempData;
    }
}

bool NpcTalkMan::AddDlgSet(const std::string& DlgSetName, std::tr1::shared_ptr<CNpcDialogueSet> spDlg)
{
    std::string LowerNtkFile(DlgSetName);
    sc::string::lowerString(LowerNtkFile);
    std::tr1::shared_ptr<CNpcDialogueSet> spExist = Find(LowerNtkFile);
    if (spExist)
    {
        return false;
    }
    else
    {
        m_DlgData.insert(NPC_DLG_DATA_VALUE(LowerNtkFile, spDlg));
        return true;
    }
}

bool NpcTalkMan::DeleteDlgSet(const std::string& DlgSetName)
{
    std::string LowerNtkFile(DlgSetName);
    sc::string::lowerString(LowerNtkFile);
    NPC_DLG_DATA_ITER iter = m_DlgData.find(LowerNtkFile);
    if (iter != m_DlgData.end())
    {
        m_DlgData.erase(iter);
        return true;
    }
    else
    {
        return false;
    }
}

//! 분리된 Ntk 파일을 하나로 통합하기 위해서 임시로 만듬
//! 통합된 이후에는 삭제를 합시다.
void NpcTalkMan::MergeNtkFiles(const std::vector<std::string>& vFileList)
{
    m_DlgData.clear();
    for (size_t i=0; i<vFileList.size(); ++i)
    {
        std::string LowerNtkFile(vFileList[i]);
        sc::string::lowerString(LowerNtkFile);
        std::tr1::shared_ptr<CNpcDialogueSet> spDlgSet(new CNpcDialogueSet);
        if (!spDlgSet->Load(LowerNtkFile))
            continue;
        m_DlgData.insert(NPC_DLG_DATA_VALUE(LowerNtkFile, spDlgSet));
    }
}

bool NpcTalkMan::Save(std::string& SaveFileName OUT)
{
    std::string FullPath(sc::getAppPath());
    SUBPATH* pPath = GLogicData::GetInstance().GetPath();
    if (pPath)
        FullPath.append(pPath->NtkFileRoot());

    FullPath.append(GLogicData::GetInstance().GetNewNpcTalkFileName());

    // 기존파일이 있으면 백업해 둔다.
    sc::file::autoRename(FullPath);

    SaveFileName = FullPath;

    sc::SerialFile SFile;
    if (!SFile.OpenFile(FOT_WRITE, FullPath.c_str()))
    {
        GASSERT(0 && "NpcTalkMan::Save OpenFile FOT_WRITE");
        return false;
    }

    SFile << (WORD) VERSION;
    SFile << (WORD) m_DlgData.size();

	CString strTemp1, strTemp2;
    for (NPC_DLG_DATA_ITER iter=m_DlgData.begin(); iter!=m_DlgData.end(); ++iter)
    {
        std::tr1::shared_ptr<CNpcDialogueSet> spDlgSet = iter->second;
        spDlgSet->Save(SFile, strTemp1, strTemp2);
    }

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() == TRUE )
	{
		FullPath = sc::getAppPath();
		FullPath.append(pPath->NtkFileRoot());
		FullPath.append("NpcTalk.xml");
		CGameTextMan::GetInstance().SaveXML(FullPath.c_str(), CGameTextMan::EM_NPCTALK_TEXT, FALSE);
	}

    return true;
}

bool NpcTalkMan::Load(const std::string& LoadFileName)
{
    m_DlgData.clear();

    std::string FullPath;
    if (LoadFileName.empty())
    {
        FullPath = sc::getAppPath();
        SUBPATH* pPath = GLogicData::GetInstance().GetPath();
        if (pPath)
            FullPath.append(pPath->NtkFileRoot());
        FullPath.append(GLogicData::GetInstance().GetNewNpcTalkFileName());
    }
    else
    {
        FullPath = LoadFileName;
    }

    std::auto_ptr<sc::BaseStream> pBStream(        
        GLOGIC::openfile_basestream(
            FALSE,
            "", 
            FullPath.c_str(),
            GLogicData::GetInstance().GetNewNpcTalkFileName(),
            false,
            FALSE));
    if (!pBStream.get())
    {
        std::string Msg(
            sc::string::format(
                "%1% load failed",
                FullPath));
        AfxMessageBox(Msg.c_str(), MB_OK | MB_ICONEXCLAMATION);
        return false;
    }

    sc::BaseStream& SFile = *pBStream;

    WORD Version = 0;
    SFile >> Version;
    WORD DlgSize = 0;
    SFile >> DlgSize;

    for (WORD i=0; i<DlgSize; ++i)
    {
        //---------------------------------------------------//
        NSLoadingDirect::FrameMoveRender( FALSE );
        //---------------------------------------------------//

        std::tr1::shared_ptr<CNpcDialogueSet> spDlgSet(new CNpcDialogueSet);
        if (spDlgSet->Load(SFile))
        {
            std::string LowerNtkFile(spDlgSet->GetDlgSetName());
            sc::string::lowerString(LowerNtkFile);
            m_DlgData.insert(NPC_DLG_DATA_VALUE(LowerNtkFile, spDlgSet));
        }
    }

    return true;
}

void NpcTalkMan::ExportTextAll()
{
    for (NPC_DLG_DATA_CITER iter=m_DlgData.begin(); iter!=m_DlgData.end(); ++iter)
    {
        std::string strFileName = iter->first;
        std::tr1::shared_ptr<CNpcDialogueSet> spDlgSet = iter->second;

        std::string FullPath = sc::getAppPath();
        SUBPATH* pPath = GLogicData::GetInstance().GetPath();
        if (pPath)
            FullPath.append(pPath->NtkFileRoot());
        FullPath.append( strFileName );

        CString strPath;
        STRUTIL::ChangeExt( strFileName.c_str(), strPath, ".ntt" );

        spDlgSet->ExportText( strPath.GetString() );
    }
}

void NpcTalkMan::GetDlgList(std::vector<std::string>& vFileList)
{
    for (NPC_DLG_DATA_CITER iter=m_DlgData.begin(); iter!=m_DlgData.end(); ++iter)
    {
        std::tr1::shared_ptr<CNpcDialogueSet> spDlgSet = iter->second;
        vFileList.push_back(spDlgSet->GetDlgSetName());
    }

    std::sort(vFileList.begin(), vFileList.end());
}

const char* NpcTalkMan::GetNpcTalkString( const std::string& strKey, const WORD wNID, const WORD wSubID )
{
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		CString strTalkKey;
		strTalkKey.Format("%s_%d_%d", strKey.c_str(), wNID, wSubID);
		const char* szTalkText = CGameTextMan::GetInstance().GetText(strTalkKey.GetString(), 0, CGameTextMan::EM_NPCTALK_TEXT).GetString();
		return szTalkText;
	}
	return strKey.c_str();
}

void NpcTalkMan::Clear(void)
{
	m_DlgData.clear();
}

// 데이터 통합 이후에는 지우자;
const bool NpcTalkMan::GetChildElement( TIXMLOLD::TiXmlElement** outXmlElem, 
						   TIXMLOLD::TiXmlElement* rootElem,
						   const char* atrbName,
						   const char* atrbData)
{
	CString resTokenChild;
	CString strCompare;
	CString strChild, strElem, strValue, strAtrbData;

	int nPosChild = 0;
	int nPosFormat = 0;

	if ( !rootElem ) return false;

	TIXMLOLD::TiXmlElement* nodeElem = rootElem; 
	TIXMLOLD::TiXmlElement* nodeElemChild = rootElem->FirstChildElement(); 	

	while ( nodeElemChild )
	{
		TIXMLOLD::TiXmlAttribute* atrb = nodeElemChild->FirstAttribute();
		while ( atrb )
		{
			if ( strcmp( atrb->Name(),  atrbName ) == 0 && 
				strcmp( atrb->Value(), atrbData ) == 0)
			{
				(*outXmlElem) = nodeElemChild;
				return true;
			}

			strAtrbData = atrb->Value();
			atrb = atrb->Next();
		}
		if ( nodeElemChild == nodeElemChild->NextSiblingElement() )
			break;

		nodeElemChild = nodeElemChild->NextSiblingElement();
	}
	return false;
}

const bool NpcTalkMan::ConvertXml(const std::string& strPath)
{
	const int iLangFlag = (int)language::ProvidelangToLangflag(RANPARAM::emProvideLangFlag);

	TIXMLOLD::TiXmlDocument XmlDoc;
	TIXMLOLD::TiXmlElement* pRootElem;
	if ( XmlDoc.LoadFile(strPath.c_str(), TIXMLOLD::TIXML_ENCODING_UTF8) )
	{	// Root Element 삭제;
		// XmlDoc.RemoveChild(XmlDoc.FirstChild()->NextSiblingElement("TEXT"));
		pRootElem = XmlDoc.RootElement();
	}
	else
	{
		TIXMLOLD::TiXmlDeclaration*	XmlDecl = new TIXMLOLD::TiXmlDeclaration( "1.0", "utf-8", "");
		XmlDoc.LinkEndChild( XmlDecl );

		pRootElem = new TIXMLOLD::TiXmlElement("TEXT");
		XmlDoc.LinkEndChild(pRootElem);
	}

// 	TIXMLOLD::TiXmlElement* pRootElem = new TIXMLOLD::TiXmlElement("TEXT");
// 	XmlDoc.LinkEndChild(pRootElem);

	char	szConv[1024] = {0};
	wchar_t	wsConv[1024] = {0};

	NPC_DLG_DATA_ITER iter, end = m_DlgData.end();
	for (iter=m_DlgData.begin(); iter!=end; ++iter)
	{
		std::tr1::shared_ptr<CNpcDialogueSet> spDlgSet = iter->second;
		
		// talk
		std::string strTalk(spDlgSet->GetDlgSetName().c_str());
		CNpcDialogueSet::DIALOGMAP_ITER dlgSetIter, dlgSetEnd = spDlgSet->GetDialogueMap()->end();
		for ( dlgSetIter = spDlgSet->GetDialogueMap()->begin(); dlgSetIter!=dlgSetEnd; ++dlgSetIter )
		{
			INT iCount = 0;
			std::tr1::shared_ptr<CNpcDialogue> spDialogue = dlgSetIter->second;
			CNpcDialogue::DLGCASE_ITER dlgCaseIter, dlgCaseEnd = spDialogue->m_vecCase.end();
			for ( dlgCaseIter = spDialogue->m_vecCase.begin(); dlgCaseIter!=dlgCaseEnd; ++dlgCaseIter )
			{
				std::tr1::shared_ptr<CNpcDialogueCase> spDlgCase = *dlgCaseIter;
				
				CString strElemName;
				strElemName.Format("%s_%d_%d", spDlgSet->GetDlgSetName().c_str(), spDialogue->GetDlgNID(), iCount);

//				TIXMLOLD::TiXmlElement* pTalkElem = new TIXMLOLD::TiXmlElement("SENTENSE");
//				pTalkElem->SetAttribute("Ver",1);
//				pTalkElem->SetAttribute("Id",strElemName.GetString());
				TIXMLOLD::TiXmlElement* pTalkElem;
				if ( !GetChildElement(&pTalkElem, pRootElem, "Id", strElemName.GetString()) )
				{
					pTalkElem = new TIXMLOLD::TiXmlElement("SENTENSE");
					pTalkElem->SetAttribute("Ver",1);
					pTalkElem->SetAttribute("Id",strElemName.GetString());
					pRootElem->LinkEndChild(pTalkElem);
				}

				// test
				// pRootElem->LinkEndChild(pTalkElem);
// 				if ( iCount == 0)
// 					XmlDoc.SaveFile(strPath);

//				TIXMLOLD::TiXmlElement* pTalkChildElem = new TIXMLOLD::TiXmlElement("VALUE");
//				pTalkChildElem->SetAttribute("Lang",RANPARAM::strCountry.GetString());

				TIXMLOLD::TiXmlElement* pTalkChildElem;
				if ( !GetChildElement(&pTalkChildElem, pTalkElem, "Lang", language::strProvideLanguage[RANPARAM::emProvideLangFlag]) )
				{
					pTalkChildElem = new TIXMLOLD::TiXmlElement("VALUE");
					pTalkChildElem->SetAttribute("Lang", language::strProvideLanguage[RANPARAM::emProvideLangFlag]);
					pTalkElem->LinkEndChild(pTalkChildElem);
				}
				else
				{
					if ( pTalkChildElem->FirstChild() )
						pTalkChildElem->RemoveChild(pTalkChildElem->FirstChild());
				}

				// \r\n은 xml 특수 기호로 전환되기 때문에 개행 문자 정리 필요없음;
// 				CString strFind("\r\n"), strText = spDlgCase->GetBasicTalk().c_str();
// 				INT iFind;
// 				while ( (iFind=strText.Find(strFind)) != -1 )
// 				{
// 					strText.Delete(iFind, strFind.GetLength());
// 					strText.Insert(iFind, "\\r\\n");
// 				}
				//  [6/17/2014 gbgim]
				// - GS에서 2000넘게 사용하는게 있다;
				char	szConv_talk[4096] = {0};
				wchar_t	wsConv_talk[4096] = {0};
				if ( strlen(spDlgCase->GetBasicTalk().c_str()) != 0 )
				{
					CString strCheck = spDlgCase->GetBasicTalk().c_str();
					strCheck.Replace(" ", "");
					if ( strCheck.GetLength() == 0 )
						_tcscpy(szConv_talk, "skip");
					else
						_tcscpy(szConv_talk, spDlgCase->GetBasicTalk().c_str());
				}
				else
				{
					_tcscpy(szConv_talk, "skip");
				}

				MultiByteToWideChar(CD3DFontPar::nCodePage[iLangFlag],
					0,szConv_talk, 
					strlen(szConv_talk)
					,wsConv_talk,sizeof(wsConv_talk)/sizeof(wsConv_talk[0]));			
				WideCharToMultiByte(CP_UTF8, 0, wsConv_talk, -1, szConv_talk,
					sizeof(szConv_talk)/sizeof(szConv_talk[0]), NULL,NULL );

				TIXMLOLD::TiXmlText* pXmlText = new TIXMLOLD::TiXmlText(szConv_talk);
				pTalkChildElem->LinkEndChild(pXmlText);
//				pTalkElem->LinkEndChild(pTalkChildElem);
				// pRootElem->LinkEndChild(pTalkElem);

				// answer
				if ( spDlgCase->GetTalkControl() == NULL )
				{
					iCount++;
					continue;
				}

				CNpcTalkControl::NPCMAP talkMapConst = spDlgCase->GetTalkControl()->GetTalkMapConst();
				for (CNpcTalkControl::NPCMAP_IT npciter=talkMapConst.begin(); npciter!=talkMapConst.end(); ++npciter)
				{
					std::tr1::shared_ptr<SNpcTalk> spNpcTalk = npciter->second;

					CString strAnswerName;
					strAnswerName.Format("%s_%d_%d_%d", spDlgSet->GetDlgSetName().c_str(), spDialogue->GetDlgNID(), iCount, spNpcTalk->m_dwNID );

// 					TIXMLOLD::TiXmlElement* pAnswerElem = new TIXMLOLD::TiXmlElement("SENTENSE");
// 					pAnswerElem->SetAttribute("Ver",1);
// 					pAnswerElem->SetAttribute("Id",strAnswerName.GetString());
					TIXMLOLD::TiXmlElement* pAnswerElem;
					if ( !GetChildElement(&pAnswerElem, pRootElem, "Id", strAnswerName.GetString()) )
					{
						pAnswerElem = new TIXMLOLD::TiXmlElement("SENTENSE");
						pAnswerElem->SetAttribute("Ver",1);
						pAnswerElem->SetAttribute("Id", strAnswerName.GetString());
						pRootElem->LinkEndChild(pAnswerElem);
					}

//					TIXMLOLD::TiXmlElement* pAnswerChildElem = new TIXMLOLD::TiXmlElement("VALUE");
//					pAnswerChildElem->SetAttribute("Lang",RANPARAM::strCountry.GetString());
					TIXMLOLD::TiXmlElement* pAnswerChildElem;
					if ( !GetChildElement(&pAnswerChildElem, pAnswerElem, "Lang", language::strProvideLanguage[RANPARAM::emProvideLangFlag]) )
					{
						pAnswerChildElem = new TIXMLOLD::TiXmlElement("VALUE");
						pAnswerChildElem->SetAttribute("Lang", language::strProvideLanguage[RANPARAM::emProvideLangFlag]);
						pAnswerElem->LinkEndChild(pAnswerChildElem);
					}
					else
					{
						if ( pAnswerChildElem->FirstChild() )
							pAnswerChildElem->RemoveChild(pAnswerChildElem->FirstChild());
					}

					char	szConv_answer[4096] = {0};
					wchar_t	wsConv_answer[4096] = {0};
					if ( strlen(spNpcTalk->m_strTalk.c_str()) != 0 )
					{
						CString strCheck = spNpcTalk->m_strTalk.c_str();
						strCheck.Replace(" ", "");
						if ( strCheck.GetLength() == 0 )
							_tcscpy(szConv_answer, "skip");
						else
							_tcscpy(szConv_answer, spNpcTalk->m_strTalk.c_str());
					}
					else
					{
						_tcscpy(szConv_answer, "skip");
					}

					MultiByteToWideChar(CD3DFontPar::nCodePage[iLangFlag],
						0,szConv_answer, 
						strlen(szConv_answer)
						,wsConv_answer,sizeof(wsConv_answer)/sizeof(wsConv_answer[0]));			
					WideCharToMultiByte(CP_UTF8, 0, wsConv_answer, -1, szConv_answer,
						sizeof(szConv_answer)/sizeof(szConv_answer[0]), NULL,NULL );

					TIXMLOLD::TiXmlText* pXmlText = new TIXMLOLD::TiXmlText(szConv_answer);
 					pAnswerChildElem->LinkEndChild(pXmlText);
// 					pAnswerElem->LinkEndChild(pAnswerChildElem);
// 					pRootElem->LinkEndChild(pAnswerElem);					
				}
				iCount++;
			}
		}
	}

	XmlDoc.SaveFile(strPath);
	return true;
}
