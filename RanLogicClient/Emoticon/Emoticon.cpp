#include "pch.h"
#include "../../SigmaCore/Log/LogMan.h"

#include "./Emoticon.h"

#include "../../RanLogic/GLogicData.h"
#include "../../RanLogic/GLogicDataMan.h"

#include "../../RanLogic/RANPARAM.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../EngineLib/GUInterface/GameTextLoader.h"
#include "../../EngineLib/DxTools/DxFontDefine.h"
#include "../../EngineLib/Common/SUBPATH.h"
#include "../../EngineLib/G-Logic/GLogic.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"

#include "../../EngineLib/Common/IniLoader.h"
#include "../../EngineLib/G-Logic/GLogic.h"

#include "../../SigmaCore/DebugInclude.h"

CEmoticon::CEmoticon(void)
{
}

CEmoticon::~CEmoticon(void)
{
}

CEmoticon& CEmoticon::GetInstance ()
{
	static CEmoticon Instance;
	return Instance;
}

bool CEmoticon::LOAD(bool bToolMode)
{
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() == TRUE )
	{
// 		std::string strPATH( GLogicData::GetInstance().GetPath()->GuiFileRoot().c_str() );
// 		strPATH += "emoticon.xml";
//
// 		NS_TEXT_LOADER::STRFLAG	mapFlag[language::NATION_SIZE];
// 		NS_TEXT_LOADER::LoadText( strPATH.c_str(), 1, mapFlag, TRUE, FALSE, RANPARAM::strCountry, FALSE );
// 
// 		NS_TEXT_LOADER::STRFLAG_IT itrLIST = mapFlag[RANPARAM::emNation].find("EMOTICON_LIST");
// 		NS_TEXT_LOADER::STRFLAG_IT itrEND = mapFlag[RANPARAM::emNation].end();
// 		if ( itrLIST == itrEND )
// 		{
// 			std::string ErrorMsg = "EMOTION_LIST Load Failed";
// 			sc::writeLogError(ErrorMsg);
// 			AfxMessageBox(ErrorMsg.c_str(), MB_OK|MB_ICONSTOP);
// 			NS_TEXT_LOADER::ClearText(mapFlag);
// 			return false;
// 		}
		std::string strPATH( GLOGIC::GetPath() );
//		strPATH += "emoticon.xml";
		CString strXmlFile;
		strXmlFile.Format( "emoticon_%s.xml", RANPARAM::strCountry.GetString() );
		strPATH += strXmlFile.GetString();

		NS_TEXT_LOADER::STRFLAG mapFlag[language::LANGFLAG_TOTAL_NUM];
		NS_TEXT_LOADER::LoadTextInZip( GLogicData::GetInstance().GetGlogicZipFile().c_str()
			, strPATH.c_str()
			, strXmlFile.GetString()//"emoticon.xml"
			, GLOGIC::UseLogicPackFile()
			, mapFlag
			, FALSE, RANPARAM::strCountry );

		NS_TEXT_LOADER::STRFLAG_IT itrLIST = mapFlag[RANPARAM::emProvideLangFlag].find("EMOTICON_LIST");
		NS_TEXT_LOADER::STRFLAG_IT itrEND = mapFlag[RANPARAM::emProvideLangFlag].end();
		if ( itrLIST == itrEND )
		{
			std::string ErrorMsg = "Emoticon Load Error";
			sc::writeLogError(ErrorMsg);
			AfxMessageBox(ErrorMsg.c_str(), MB_OK|MB_ICONSTOP);
			NS_TEXT_LOADER::ClearText(mapFlag);
			return false;
		}

		CStringArray* pTextArray = itrLIST->second;		
		for ( INT i = 0; i < pTextArray->GetSize(); ++i )
		{
			const CString strEmotion = pTextArray->GetAt(i);
			if ( strEmotion.IsEmpty() )
				continue;

			CString strMotion = strEmotion, strTerm = strEmotion;
			strMotion.Append("_MOTION");		
			strTerm.Append("_TERM");
			
			NS_TEXT_LOADER::STRFLAG_IT itrMOTION = mapFlag[RANPARAM::emProvideLangFlag].find(strMotion.GetString());
			NS_TEXT_LOADER::STRFLAG_IT itrTERM = mapFlag[RANPARAM::emProvideLangFlag].find(strTerm.GetString());
			if ( itrMOTION == itrEND || itrTERM == itrEND )
			{
				std::string ErrorMsg(sc::string::format("\"%1%\", \"%2%\" not exit", strMotion.GetString(), strTerm.GetString()) );
				sc::writeLogError(ErrorMsg);
				AfxMessageBox(ErrorMsg.c_str(), MB_OK|MB_ICONSTOP);
				NS_TEXT_LOADER::ClearText(mapFlag);
				continue;
			}

			SEMOTICON sEmoticon;
			sEmoticon.nID = i;
			sEmoticon.strMotion = static_cast<CStringArray*>(itrMOTION->second)->GetAt(0).GetString();
			CStringArray* strTextArr = itrTERM->second;
			for ( int j = 0; j < strTextArr->GetSize(); ++j )
			{
				sEmoticon.listTerm.push_back( strTextArr->GetAt(j).GetString() );
			}
			m_listEmoticon.push_back( sEmoticon );
		}

		NS_TEXT_LOADER::ClearText(mapFlag);
	}
	else
	{
		std::string strPATH( GLOGIC::GetPath() );
		strPATH += "emoticon.ini";

		CIniLoader cFILE(",", bToolMode);

		if (GLOGIC::UseLogicZipFile())
			cFILE.SetZipFile(GLogicData::GetInstance().GetGlogicZipFile());

		if (!cFILE.open(strPATH, true, GLOGIC::UseLogicPackFile()))
		{
			sc::writeLogError(
				std::string("CEmoticon::LOAD()"));
			return false;
		}

		UINT nNumber(0);
		cFILE.getflag( "FILE_INFO", "DataNumber", 0, 1, nNumber );

		CString strSection;
		for( UINT i=0; i<nNumber; ++i )
		{
			strSection.Format( "EMOTICON_%04d", i+1 );

			bool bUsed(false);
			cFILE.getflag( strSection.GetString(), "Used", 0, 1, bUsed );

			if( !bUsed ) continue;

			SEMOTICON sEmoticon;

			cFILE.getflag( strSection.GetString(), "Id", 0, 1, sEmoticon.nID );
			cFILE.getflag( strSection.GetString(), "Motion", 0, 1, sEmoticon.strMotion );

			UINT nSize = cFILE.GetKeyDataSize( strSection.GetString(), "TermList" );

			std::string strTerm;
			for( UINT j=0; j<nSize; ++j )
			{
				cFILE.getflag( strSection.GetString(), "TermList", j, nSize, strTerm );
				sEmoticon.listTerm.push_back( strTerm );
			}

			m_listEmoticon.push_back( sEmoticon );
		}
	}
	return true;
}

int	CEmoticon::GetEmoticonOfMotion(const std::string & strSrc)
{
//	return UINT_MAX;

	std::string strTemp( strSrc.substr( 1 ) );

	LISTEMOTICON_ITER iter = m_listEmoticon.begin();
	LISTEMOTICON_ITER iter_end = m_listEmoticon.end();

	for( ;iter != iter_end ;++iter )
	{
		if( iter->strMotion == strTemp ) return iter->nID;
	}
	
	return UINT_MAX;
}

int	CEmoticon::GetEmoticonOfTerm(const std::string & strSrc)
{
//	return UINT_MAX;

	LISTEMOTICON_ITER iter = m_listEmoticon.begin();
	LISTEMOTICON_ITER iter_end = m_listEmoticon.end();

	for( ; iter != iter_end; ++iter )
	{
		LISTSTRING_ITER iter_in = iter->listTerm.begin();
		LISTSTRING_ITER iter_end_in = iter->listTerm.end();

		for( ; iter_in != iter_end_in; ++iter_in )
		{
			// 대문자 변경에서 뻑남;
 			const std::string strChat = _strlwr( const_cast<char*>(strSrc.c_str()) );// CString(strSrc.c_str()).MakeLower().GetString();
 
 			const std::string::size_type idx = strChat.find( *iter_in );
			if ( idx != std::string::npos )
			{
				return iter->nID;
			}
		}
	}

	return UINT_MAX;
}