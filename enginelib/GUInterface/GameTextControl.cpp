#include "pch.h"
#include "../GlobalParam.h"
#include "../DxTools/RENDERPARAM.h"
#include "../../RanLogic/RANPARAM.h"
#include "./GameTextControl.h"

#include "../DxTools/DxFontMan.h"
#include "../../RanLogic/Item/GLItem.h"
#include "../../RanLogic/GLLevelFile.h"
#include "../../RanLogic/Crow/GLMobSchedule.h"
#include "../../RanLogic/Skill/GLSkill.h"
#include "../../RanLogic/Quest/GLQuest.h"
#include "../../RanLogic/Post/GLPostData.h"
#include "../../RanLogic/Activity/Activity.h"
#include "../../RanLogic/GLUseFeatures.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CGameTextMan& CGameTextMan::GetInstance()
{
	static CGameTextMan Instance;
	return Instance;
}


CGameTextMan::CGameTextMan ()
{
	memset( m_szPath, 0, sizeof( m_szPath ) );
}

CGameTextMan::~CGameTextMan ()
{
    Clear ( EM_GAME_WORD );
	Clear ( EM_GAME_IN_TEXT );
	Clear ( EM_GAME_EX_TEXT );
	Clear ( EM_SERVER_TEXT );
	Clear ( EM_SKILL_TEXT );
	Clear ( EM_ITEM_TEXT );
	Clear ( EM_TIP_TEXT );
	Clear ( EM_CROW_TEXT );
	Clear ( EM_COMMENT_TEXT );
	Clear ( EM_QUEST_TEXT );
	Clear ( EM_NPCTALK_TEXT );
	Clear ( EM_AUTOLEVELSET_TEXT );
}

void CGameTextMan::Clear ( int nType )
{
	switch ( nType )
	{	
	case EM_GAME_WORD:		NS_TEXT_LOADER::ClearText ( m_GameWord.mapFlags );		return;
	case EM_GAME_IN_TEXT:	NS_TEXT_LOADER::ClearText ( m_GameInText.mapFlags );	return;
	case EM_GAME_EX_TEXT:	NS_TEXT_LOADER::ClearText ( m_GameExText.mapFlags );	return;
	case EM_SERVER_TEXT:	NS_TEXT_LOADER::ClearText ( m_ServerText.mapFlags );	return;
	case EM_SKILL_TEXT:		NS_TEXT_LOADER::ClearText ( m_SkillText.mapFlags );	return;
	case EM_ITEM_TEXT:		NS_TEXT_LOADER::ClearText ( m_ItemText.mapFlags );	return;
	case EM_CROW_TEXT:		NS_TEXT_LOADER::ClearText ( m_CrowText.mapFlags );	return;
	case EM_TIP_TEXT:		NS_TEXT_LOADER::ClearText ( m_TipText.mapFlags );	return;
	case EM_COMMENT_TEXT:	NS_TEXT_LOADER::ClearText ( m_CommentText.mapFlags );	return;
	case EM_QUEST_TEXT:		NS_TEXT_LOADER::ClearText ( m_QuestText.mapFlags );	return;
	case EM_NPCTALK_TEXT:		NS_TEXT_LOADER::ClearText ( m_NPCTalkText.mapFlags );	return;
	case EM_AUTOLEVELSET_TEXT:	NS_TEXT_LOADER::ClearText ( m_AutoLevelText.mapFlags ); return;
	}

	GASSERT ( 0 && "CGameTextMan::Clear() : 정의되지 않은 타입" );
}

// xml id로 영어가 아닌 한글이 들어가는 경우가있다(autolevel);
// 해당 키워드는 MultitoUTF8이 필요하다; 전체다 적용시킬필요는없다;
BOOL CGameTextMan::LoadText ( const char* szFileName, int nType,const CString& country, DWORD dwLoadOP )
{
	CString strFileName(szFileName);
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate())
	{		
		const DWORD dwFind(strFileName.ReverseFind(L'.'));
		if ( dwFind != -1 )
		{
			// 확장자는 .xml이 확실하지만 혹시모르니깐;
			CString strFile(strFileName.Left(dwFind));
			CString strExt(strFileName.Mid(dwFind, strFileName.GetLength()-dwFind));
			strFileName.Format("%s_%s%s", strFile.GetString(), RANPARAM::strCountry.GetString(), strExt.GetString());
		}
		else
		{
			GASSERT ( 0 && "CGameTextMan::LoadText, 확장자가 없습니다" );
			return FALSE;
		}
	}

	DWORD dwTexLoadOP = 0;
	dwTexLoadOP |= dwLoadOP & GAMETEXTMAN_LOADOP_REFACT		?	NS_TEXT_LOADER::NS_TEXT_LOADOP_REFACT:0;
	dwTexLoadOP |= dwLoadOP & GAMETEXTMAN_LOADOP_SERVEREXE	?	NS_TEXT_LOADER::NS_TEXT_LOADOP_SERVEREXE:0;
	dwTexLoadOP |= dwLoadOP & GAMETEXTMAN_LOADOP_ALLLANG	?	NS_TEXT_LOADER::NS_TEXT_LOADOP_ALLLANG:0;	
	dwTexLoadOP |= dwLoadOP & GAMETEXTMAN_LOADOP_CONVUTF8	?	NS_TEXT_LOADER::NS_TEXT_LOADOP_CONVUTF8:0;

	CString strPath(m_szPath);
	strPath += strFileName;

	BOOL bResult(FALSE);
	switch ( nType )
	{
	case EM_GAME_WORD:		bResult =  NS_TEXT_LOADER::LoadText( strPath.GetString(), SGAMEWORD::progVersion,   m_GameWord.mapFlags,   country, dwTexLoadOP ); break;
	case EM_GAME_IN_TEXT:	bResult =  NS_TEXT_LOADER::LoadText( strPath.GetString(), SGAMEINTEXT::progVersion, m_GameInText.mapFlags, country, dwTexLoadOP ); break;
	case EM_GAME_EX_TEXT:	bResult =  NS_TEXT_LOADER::LoadText( strPath.GetString(), SGAMEEXTEXT::progVersion, m_GameExText.mapFlags, country, dwTexLoadOP ); break;
	case EM_SERVER_TEXT:	bResult =  NS_TEXT_LOADER::LoadText( strPath.GetString(), SSERVERTEXT::progVersion, m_ServerText.mapFlags, country, dwTexLoadOP ); break;
	case EM_SKILL_TEXT:		bResult =  NS_TEXT_LOADER::LoadText( strPath.GetString(), SSKILLTEXT::progVersion,  m_SkillText.mapFlags,  country, dwTexLoadOP ); break;
	case EM_ITEM_TEXT:		bResult =  NS_TEXT_LOADER::LoadText( strPath.GetString(), SITEMTEXT::progVersion,   m_ItemText.mapFlags,   country, dwTexLoadOP ); break;
	case EM_CROW_TEXT:		bResult =  NS_TEXT_LOADER::LoadText( strPath.GetString(), SCROWTEXT::progVersion,   m_CrowText.mapFlags,   country, dwTexLoadOP ); break;
	case EM_TIP_TEXT:		bResult =  NS_TEXT_LOADER::LoadText( strPath.GetString(), STIPTEXT::progVersion,    m_TipText.mapFlags,    country, dwTexLoadOP ); break;
	case EM_COMMENT_TEXT:	bResult =  NS_TEXT_LOADER::LoadText( strPath.GetString(), SCOMMENTTEXT::progVersion,    m_CommentText.mapFlags,		country, dwTexLoadOP );	break;
	case EM_NPCTALK_TEXT:		bResult =   NS_TEXT_LOADER::LoadText( strPath.GetString(), SNPCTALKTEXT::progVersion,	m_NPCTalkText.mapFlags,			country, dwTexLoadOP ); break;
	case EM_QUEST_TEXT:			bResult =   NS_TEXT_LOADER::LoadText( strPath.GetString(), SQUESTTEXT::progVersion,	m_QuestText.mapFlags,				country, dwTexLoadOP );	break;
	case EM_AUTOLEVELSET_TEXT:	bResult =   NS_TEXT_LOADER::LoadText( strPath.GetString(), SAUTOLEVELSETTEXT::progVersion,    m_AutoLevelText.mapFlags,	country, dwTexLoadOP );	break;
	default:
		GASSERT ( 0 && "CGameTextMan::LoadText() : 정의되지 않은 타입" );
		return FALSE;
	}

	if ( bResult == FALSE )
	{
		CString strMsg("not exist ");
		strMsg += strPath;
		AfxMessageBox( strMsg, MB_OK|MB_ICONSTOP );
	}
	return bResult;
}

const BOOL CGameTextMan::SaveXML(const char* const szFileName, const int nType, const BOOL bAllLang)
{
	CString strPath ( m_szPath );
	strPath += szFileName;

	switch ( nType )
	{
	case EM_GAME_WORD:		return NS_TEXT_LOADER::SaveStringTableXML( strPath.GetString(), m_GameWord.mapFlags,    bAllLang );
	case EM_GAME_IN_TEXT:	return NS_TEXT_LOADER::SaveStringTableXML( strPath.GetString(), m_GameInText.mapFlags,  bAllLang );
	case EM_GAME_EX_TEXT:	return NS_TEXT_LOADER::SaveStringTableXML( strPath.GetString(), m_GameExText.mapFlags,  bAllLang );
	case EM_SERVER_TEXT:	return NS_TEXT_LOADER::SaveStringTableXML( strPath.GetString(), m_ServerText.mapFlags,  bAllLang );
	case EM_SKILL_TEXT:		return NS_TEXT_LOADER::SaveStringTableXML( strPath.GetString(), m_SkillText.mapFlags,   bAllLang );
	case EM_ITEM_TEXT:		return NS_TEXT_LOADER::SaveStringTableXML( strPath.GetString(), m_ItemText.mapFlags,    bAllLang );
	case EM_CROW_TEXT:		return NS_TEXT_LOADER::SaveStringTableXML( strPath.GetString(), m_CrowText.mapFlags,    bAllLang );
	case EM_TIP_TEXT:		return NS_TEXT_LOADER::SaveStringTableXML( strPath.GetString(), m_TipText.mapFlags,     bAllLang );
	case EM_QUEST_TEXT:		return NS_TEXT_LOADER::SaveStringTableXML( strPath.GetString(), m_QuestText.mapFlags,	bAllLang );
	case EM_NPCTALK_TEXT:	return NS_TEXT_LOADER::SaveStringTableXML(strPath.GetString(), m_NPCTalkText.mapFlags, bAllLang);
	case EM_AUTOLEVELSET_TEXT:	return NS_TEXT_LOADER::SaveStringTableXML( strPath.GetString(), m_AutoLevelText.mapFlags,     bAllLang );
	default:
		GASSERT ( 0 && "CGameTextMan::SaveXML() : 정의되지 않은 타입" );
		return FALSE;
	}
	return TRUE;
}

const CString& CGameTextMan::GetGameWord(const std::string& strKey, const int Index)
{
	return GetText(strKey, Index, EM_GAME_WORD);
}

const CString& CGameTextMan::GetGameInText(const std::string& strKey, const int Index)
{
	return GetText(strKey, Index, EM_GAME_IN_TEXT);
}

const CString& CGameTextMan::GetGameExText(const std::string& strKey, const int Index)
{
	return GetText(strKey, Index, EM_GAME_EX_TEXT);
}

const CString& CGameTextMan::GetServerText(const std::string& strKey, const int nIndex)
{
	return GetText(strKey, nIndex, EM_SERVER_TEXT);
}

const CString& CGameTextMan::GetSkillText(const std::string& strKey, const int nIndex)
{
	return GetText(strKey, nIndex, EM_SKILL_TEXT);
}

const CString& CGameTextMan::GetItemText(const std::string& strKey, const int nIndex)
{
	return GetText(strKey, nIndex, EM_ITEM_TEXT);
}

const CString& CGameTextMan::GetCrowText(const std::string& strKey, const int nIndex)
{
	return GetText(strKey, nIndex, EM_CROW_TEXT);
}

const CString& CGameTextMan::GetTipText(const std::string& strKey, const int nIndex)
{
	return GetText(strKey, nIndex, EM_TIP_TEXT);
}

const CString CGameTextMan::GetTipRandomText()
{
	const static CString strNotExist = "Not exist TipText";
	const int nTipSize = static_cast<int>(m_TipText.mapFlags[RANPARAM::emProvideLangFlag].size());
	if ( nTipSize == 0 )
		return strNotExist;

	int nIndex = rand () % nTipSize;
	char szIndex[8];
	itoa(nIndex, szIndex, 10);
	std::string strKey(szIndex);
	CString strRes = NS_TEXT_LOADER::GetText(strKey, 0, m_TipText.mapFlags[RANPARAM::emProvideLangFlag]);
	if ( strRes == "skip-empty" || strRes == "skip" || strRes == "Skip" )
		return "";

	return strRes;
}

const CString& CGameTextMan::GetQuestText(const std::string& strKey, const int nIndex)
{
	return GetText(strKey, nIndex, EM_QUEST_TEXT);
}

const CString& CGameTextMan::GetNPCTalkText(const std::string& strKey, const int nIndex)
{
	return GetText(strKey, nIndex, EM_NPCTALK_TEXT);
}

const CString& CGameTextMan::GetCommentText(const std::string& strKey, const int nIndex)
{
	return GetText(strKey, nIndex, EM_COMMENT_TEXT);
}

const CString& CGameTextMan::GetAutoLevelSetText(const std::string& strKey, const int nIndex)
{
	return GetText(strKey, nIndex, EM_AUTOLEVELSET_TEXT);
}

const CString &	CGameTextMan::GetText(const std::string& strKey, const int Index, const int nType )
{
    if (GLOBAL_PARAM::bShowContentID)
    {
        static CString strTemp;
        strTemp.Format("%s %d %d", strKey.c_str(), Index, nType);
        return strTemp;
    }

	switch (nType)
	{
	case EM_GAME_WORD:		
		return NS_TEXT_LOADER::GetText(strKey, Index, m_GameWord.mapFlags[RANPARAM::emProvideLangFlag]);
	case EM_GAME_IN_TEXT:
        return NS_TEXT_LOADER::GetText(strKey, Index, m_GameInText.mapFlags[RANPARAM::emProvideLangFlag]);
	case EM_GAME_EX_TEXT:
        return NS_TEXT_LOADER::GetText(strKey, Index, m_GameExText.mapFlags[RANPARAM::emProvideLangFlag]);
	case EM_SERVER_TEXT:
        return NS_TEXT_LOADER::GetText(strKey, Index, m_ServerText.mapFlags[RANPARAM::emProvideLangFlag]);
	case EM_SKILL_TEXT:
		return NS_TEXT_LOADER::GetText(strKey, Index, m_SkillText.mapFlags[RANPARAM::emProvideLangFlag]);
	case EM_ITEM_TEXT:
		return NS_TEXT_LOADER::GetText(strKey, Index, m_ItemText.mapFlags[RANPARAM::emProvideLangFlag]);
	case EM_CROW_TEXT:
		return NS_TEXT_LOADER::GetText(strKey, Index, m_CrowText.mapFlags[RANPARAM::emProvideLangFlag]);
	case EM_TIP_TEXT:
		return NS_TEXT_LOADER::GetText(strKey, Index, m_TipText.mapFlags[RANPARAM::emProvideLangFlag]);
	case EM_COMMENT_TEXT:
		return NS_TEXT_LOADER::GetText(strKey, Index, m_CommentText.mapFlags[RANPARAM::emProvideLangFlag]);
	case EM_QUEST_TEXT:
		return NS_TEXT_LOADER::GetText(strKey, Index, m_QuestText.mapFlags[RANPARAM::emProvideLangFlag]);
	case EM_NPCTALK_TEXT:
		return NS_TEXT_LOADER::GetText(strKey, Index, m_NPCTalkText.mapFlags[RANPARAM::emProvideLangFlag]);
	case EM_AUTOLEVELSET_TEXT:
		return NS_TEXT_LOADER::GetText(strKey, Index, m_AutoLevelText.mapFlags[RANPARAM::emProvideLangFlag]);
	}

	GASSERT(0 && "CGameTextMan::GetText() : 정의되지 않은 타입");
	static CString strEmpty; // MEMO : 에러시 반환되는 빈 스트링
	return strEmpty;
}

int	CGameTextMan::GetNumber(const std::string& strKey, int Index, int nType)
{
	switch (nType)
	{
	case EM_GAME_WORD:
        return NS_TEXT_LOADER::GetNumber(strKey, Index, m_GameWord.mapFlags[RANPARAM::emProvideLangFlag]);
	case EM_GAME_IN_TEXT:
        return NS_TEXT_LOADER::GetNumber(strKey, Index, m_GameInText.mapFlags[RANPARAM::emProvideLangFlag]);
	case EM_GAME_EX_TEXT:
        return NS_TEXT_LOADER::GetNumber(strKey, Index, m_GameExText.mapFlags[RANPARAM::emProvideLangFlag]);
	case EM_SERVER_TEXT:
        return NS_TEXT_LOADER::GetNumber(strKey, Index, m_ServerText.mapFlags[RANPARAM::emProvideLangFlag]);
	case EM_SKILL_TEXT:
		return NS_TEXT_LOADER::GetNumber(strKey, Index, m_SkillText.mapFlags[RANPARAM::emProvideLangFlag]);
	case EM_ITEM_TEXT:
		return NS_TEXT_LOADER::GetNumber(strKey, Index, m_ItemText.mapFlags[RANPARAM::emProvideLangFlag]);
	case EM_CROW_TEXT:
		return NS_TEXT_LOADER::GetNumber(strKey, Index, m_CrowText.mapFlags[RANPARAM::emProvideLangFlag]);
	case EM_TIP_TEXT:
		return NS_TEXT_LOADER::GetNumber(strKey, Index, m_TipText.mapFlags[RANPARAM::emProvideLangFlag]);
	case EM_QUEST_TEXT:
		return NS_TEXT_LOADER::GetNumber(strKey, Index, m_QuestText.mapFlags[RANPARAM::emProvideLangFlag]);
//	case EM_NPCTALK_TEXT:
//		return NS_TEXT_LOADER::GetNumber(strKey, Index, m_NPCTalkText.mapFlags[RANPARAM::emProvideLangFlag]);
	case EM_AUTOLEVELSET_TEXT:
		return NS_TEXT_LOADER::GetNumber(strKey, Index, m_AutoLevelText.mapFlags[RANPARAM::emProvideLangFlag]);
	}

	GASSERT(0 && "CGameTextMan::GetNumber() : 정의되지 않은 타입");
	return 0;
}

const BOOL CGameTextMan::FindData(const std::string& strKey, const INT nType)
{
	NS_TEXT_LOADER::STRFLAG* hmMapFlag = NULL;
	switch (nType)
	{
	case EM_SKILL_TEXT:
		{
			hmMapFlag = &m_SkillText.mapFlags[RANPARAM::emProvideLangFlag];
		}
		break;
	case EM_ITEM_TEXT:
		{
			hmMapFlag = &m_ItemText.mapFlags[RANPARAM::emProvideLangFlag];
		}
		break;
	case EM_CROW_TEXT:
		{
			hmMapFlag = &m_CrowText.mapFlags[RANPARAM::emProvideLangFlag];
		}
		break;
	default:
		return FALSE;
	}

	NS_TEXT_LOADER::STRFLAG_IT found = (*hmMapFlag).find(strKey);
	return (( (*hmMapFlag).end() == found )? FALSE : TRUE);
}

const BOOL CGameTextMan::InsertString( const char* const szKeyName, const char* const szDesc, const int nType, const int nIndex )
{
	NS_TEXT_LOADER::STRFLAG* hmMapFlag = NULL;
	CStringArray* pStrArr;
	switch (nType)
	{
	case EM_SKILL_TEXT:			hmMapFlag = &m_SkillText.mapFlags[RANPARAM::emProvideLangFlag];		break;
	case EM_ITEM_TEXT:			hmMapFlag = &m_ItemText.mapFlags[RANPARAM::emProvideLangFlag];		break;
	case EM_CROW_TEXT:			hmMapFlag = &m_CrowText.mapFlags[RANPARAM::emProvideLangFlag];		break;
	case EM_AUTOLEVELSET_TEXT:	hmMapFlag = &m_AutoLevelText.mapFlags[RANPARAM::emProvideLangFlag];	break;
	case EM_NPCTALK_TEXT:		hmMapFlag = &m_NPCTalkText.mapFlags[RANPARAM::emProvideLangFlag];	break;
	case EM_QUEST_TEXT:			hmMapFlag = &m_QuestText.mapFlags[RANPARAM::emProvideLangFlag];	break;
	default:
		GASSERT(0 && "CGameTextMan::InsertString() : 정의되지 않은 타입");
		return FALSE;
	}

	if ( (*hmMapFlag)[szKeyName] == NULL )
	{
		pStrArr = new CStringArray();
		(*hmMapFlag)[szKeyName] = pStrArr;
	}
	else
	{
		pStrArr = (*hmMapFlag)[szKeyName];
	}

	if ( nIndex == -1 )
	{
		pStrArr->RemoveAll();
		pStrArr->Add(szDesc);
	}
	else
	{
		pStrArr->InsertAt(nIndex, szDesc);
	}

	return TRUE;
}

const BOOL CGameTextMan::DeleteString( const char* const szKeyName, const int nType )
{
	NS_TEXT_LOADER::STRFLAG* hmMapFlag = NULL;

	switch (nType)
	{
	case EM_GAME_WORD:
	case EM_GAME_IN_TEXT:
	case EM_GAME_EX_TEXT:
	case EM_SERVER_TEXT:
		return TRUE;
	case EM_SKILL_TEXT:			hmMapFlag = &m_SkillText.mapFlags[RANPARAM::emProvideLangFlag];		break;
	case EM_ITEM_TEXT:			hmMapFlag = &m_ItemText.mapFlags[RANPARAM::emProvideLangFlag];		break;
	case EM_CROW_TEXT:			hmMapFlag = &m_CrowText.mapFlags[RANPARAM::emProvideLangFlag];		break;
	case EM_NPCTALK_TEXT:		hmMapFlag = &m_NPCTalkText.mapFlags[RANPARAM::emProvideLangFlag];	break;
	case EM_QUEST_TEXT:			hmMapFlag = &m_QuestText.mapFlags[RANPARAM::emProvideLangFlag];		break;
	default:
		return FALSE;
	}

	if ( hmMapFlag == NULL )
		return TRUE;
	else
	{
		CStringArray* pStrArr = (*hmMapFlag)[szKeyName];
		pStrArr->RemoveAll();
	}
	return TRUE;
}

const char* ID2GAMEWORD(const char* szKeyword, const int nIndex)
{
	return CGameTextMan::GetInstance().GetText(szKeyword, nIndex, CGameTextMan::EM_GAME_WORD);
}

const char* ID2GAMEINTEXT(const char* szKeyword, const int nIndex)
{
	return CGameTextMan::GetInstance().GetText(szKeyword, nIndex, CGameTextMan::EM_GAME_IN_TEXT);
}

const char* ID2GAMEEXTEXT(const char* szKeyword, int nIndex)
{
	return CGameTextMan::GetInstance().GetText(szKeyword, nIndex, CGameTextMan::EM_GAME_EX_TEXT);
}

const char* ID2SERVERTEXT(const char* szKeyword, const int nIndex)
{
	return CGameTextMan::GetInstance().GetText(szKeyword, nIndex, CGameTextMan::EM_SERVER_TEXT);
}

const char* ID2SKILLTEXT(const char* szKeyword, const int nIndex)
{
	return CGameTextMan::GetInstance().GetText(szKeyword, nIndex, CGameTextMan::EM_SKILL_TEXT);
}

const char* ID2ITEMTEXT(const char* szKeyword, const int nIndex)
{
	return CGameTextMan::GetInstance().GetText(szKeyword, nIndex, CGameTextMan::EM_ITEM_TEXT);
}

const char * ID2CROWTEXT(const char* szKeyword, const int nIndex)
{
	return CGameTextMan::GetInstance().GetText(szKeyword, nIndex, CGameTextMan::EM_CROW_TEXT);
}

const char * ID2TIPTEXT(const char* szKeyword, const int nIndex)
{
	return CGameTextMan::GetInstance().GetText(szKeyword, nIndex, CGameTextMan::EM_TIP_TEXT);
}

const char * ID2COMMENTTEXT(const char* szKeyword, const int nIndex)
{
	return CGameTextMan::GetInstance().GetText(szKeyword, nIndex, CGameTextMan::EM_COMMENT_TEXT);
}

const char * ID2QUESTTEXT(const char* szKeyword, const int nIndex)
{
	return CGameTextMan::GetInstance().GetText(szKeyword, nIndex, CGameTextMan::EM_QUEST_TEXT);
}

const char * ID2NPCTALKTEXT(const std::string& strKey, const int nIndex)
{
	return CGameTextMan::GetInstance().GetText(strKey, nIndex, CGameTextMan::EM_NPCTALK_TEXT);
}

void CGameTextMan::LoadHelpCommentString(const std::string& strID, std::string* pOutList, NS_TEXT_LOADER::STRFLAG& mapFlags, const int nCount)
{
	for ( INT i = 0; i < nCount; ++i )
	{
		pOutList[i] = NS_TEXT_LOADER::GetText(strID, i, mapFlags);
	}	
}

bool CGameTextMan::LoadHelpCommentString(const std::string& strID, std::string* pOutList, INT nType, const int nCount)
{
	NS_TEXT_LOADER::STRFLAG* mapFlags;
	switch (nType)
	{
	case EM_GAME_WORD:		mapFlags = &m_GameWord.mapFlags[RANPARAM::emProvideLangFlag];		break;
	case EM_GAME_IN_TEXT:	mapFlags = &m_GameInText.mapFlags[RANPARAM::emProvideLangFlag];		break;
	case EM_GAME_EX_TEXT:	mapFlags = &m_GameExText.mapFlags[RANPARAM::emProvideLangFlag];		break;
	case EM_SERVER_TEXT:	mapFlags = &m_ServerText.mapFlags[RANPARAM::emProvideLangFlag];		break;
	case EM_SKILL_TEXT:	mapFlags = &m_SkillText.mapFlags[RANPARAM::emProvideLangFlag];		break;
	case EM_ITEM_TEXT:	mapFlags = &m_ItemText.mapFlags[RANPARAM::emProvideLangFlag];		break;
	case EM_CROW_TEXT:	mapFlags = &m_CrowText.mapFlags[RANPARAM::emProvideLangFlag];		break;
	case EM_TIP_TEXT:	mapFlags = &m_TipText.mapFlags[RANPARAM::emProvideLangFlag];		break;
	case EM_COMMENT_TEXT:	mapFlags = &m_CommentText.mapFlags[RANPARAM::emProvideLangFlag];		break;
	case EM_QUEST_TEXT:	mapFlags = &m_QuestText.mapFlags[RANPARAM::emProvideLangFlag];		break;
	default:	return false;
	}
	for ( INT i = 0; i < nCount; ++i )
	{
		pOutList[i] = NS_TEXT_LOADER::GetText(strID, i, *mapFlags);
	}	
	return true;
}

bool CGameTextMan::LoadHelpUnknownCommentString(const std::string& strID, std::vector<std::string>& pOutList, INT nType)
{
	NS_TEXT_LOADER::STRFLAG* mapFlags;
	switch (nType)
	{
	case EM_GAME_WORD:		mapFlags = &m_GameWord.mapFlags[RANPARAM::emProvideLangFlag];		break;
	case EM_GAME_IN_TEXT:	mapFlags = &m_GameInText.mapFlags[RANPARAM::emProvideLangFlag];		break;
	case EM_GAME_EX_TEXT:	mapFlags = &m_GameExText.mapFlags[RANPARAM::emProvideLangFlag];		break;
	case EM_SERVER_TEXT:	mapFlags = &m_ServerText.mapFlags[RANPARAM::emProvideLangFlag];		break;
	case EM_SKILL_TEXT:	mapFlags = &m_SkillText.mapFlags[RANPARAM::emProvideLangFlag];		break;
	case EM_ITEM_TEXT:	mapFlags = &m_ItemText.mapFlags[RANPARAM::emProvideLangFlag];		break;
	case EM_CROW_TEXT:	mapFlags = &m_CrowText.mapFlags[RANPARAM::emProvideLangFlag];		break;
	case EM_TIP_TEXT:	mapFlags = &m_TipText.mapFlags[RANPARAM::emProvideLangFlag];		break;
	case EM_COMMENT_TEXT:	mapFlags = &m_CommentText.mapFlags[RANPARAM::emProvideLangFlag];		break;
	case EM_QUEST_TEXT:	mapFlags = &m_QuestText.mapFlags[RANPARAM::emProvideLangFlag];		break;
	default:	return false;
	}






	NS_TEXT_LOADER::STRFLAG_IT FlagItr = mapFlags->find(strID.c_str());
	if ( FlagItr == mapFlags->end() )
	{
// 		CString strTemp;
// 		strTemp.Format("[GAME TEXT ERROR]Not exist text keyword : %s", strID.c_str());
// 		AfxMessageBox( strTemp, MB_OK|MB_ICONSTOP );
		return false;
	}
	else
	{
		CStringArray *pStrArray = (*FlagItr).second;
		if ( pStrArray->GetCount () == 0 )
		{
			CString strTemp;
			strTemp.Format("[GAME TEXT ERROR]Index Range is 0 : %s", strID.c_str());
			AfxMessageBox( strTemp, MB_OK|MB_ICONSTOP );


		}
		for ( INT j = 0; j < pStrArray->GetCount(); ++j )
		{
			pOutList.push_back(pStrArray->GetAt(j).GetString());
		}
	}
	return true;
}

bool CGameTextMan::ParseComment()
{
	NS_TEXT_LOADER::STRFLAG& mapFlag = m_CommentText.mapFlags[RANPARAM::emProvideLangFlag];
	LoadHelpCommentString("SKILL_ROLE",		COMMENT::SKILL_ROLE, mapFlag, SKILL::EMROLE_NSIZE);
	LoadHelpCommentString("SKILL_APPLY",	COMMENT::SKILL_APPLY, mapFlag, SKILL::EMAPPLY_NSIZE);
	LoadHelpCommentString("SKILL_ACTION_TYPE", COMMENT::SKILL_ACTION_TYPE, mapFlag, SKILL::EMACTION_TYPE_NSIZE);
	LoadHelpCommentString("SKILL_EFFECT_TYPE", COMMENT::SKILL_EFFECT_TYPE, mapFlag, SKILL::EMEFFECT_TYPE_NSIZE);
	LoadHelpCommentString("SKILL_LEVEL",	COMMENT::SKILL_LEVEL,	mapFlag, SKILL::MAX_LEVEL);
	LoadHelpCommentString("SKILL_EFFTIME",	COMMENT::SKILL_EFFTIME, mapFlag, SKILL::EMTIME_NSIZE);
	LoadHelpCommentString("SKILL_EFFPOS",	COMMENT::SKILL_EFFPOS,	mapFlag, SKILL::EMROLE_NSIZE);
	LoadHelpCommentString("SPECIAL_SKILL_TYPE",	COMMENT::SPECIAL_SKILL_TYPE, mapFlag, SKILL::EMSSTYPE_NSIZE);
	LoadHelpCommentString("SKILLATTACK",	COMMENT::SKILLATTACK,	mapFlag, SKILL::SKILLATT_NSIZE+1);
	LoadHelpCommentString("ADDON_NO",	COMMENT::ADDON_NO,		mapFlag, ITEM::SSUIT::ADDON_SIZE);
	LoadHelpCommentString("CROWACT_UP",	COMMENT::szCROWACT_UP,	mapFlag, EMCROWACT_UP_NSIZE);
	LoadHelpCommentString("CROWACT_DN",	COMMENT::szCROWACT_DN,	mapFlag, EMCROWACT_DN_NSIZE);
	LoadHelpCommentString("NPC_TYPE",	COMMENT::NPC_TYPE,		mapFlag, NPC_TYPE_SIZE);
	LoadHelpCommentString("EMREACTION",	COMMENT::szEMREACTION,	mapFlag, REACT_SIZE+1);
	LoadHelpCommentString("MOBACTIONS",	COMMENT::MOBACTIONS,	mapFlag, EMACTION_SIZE);
	LoadHelpCommentString("ITEMTYPE",	COMMENT::ITEMTYPE,	mapFlag, ITEM_NSIZE);
	LoadHelpCommentString("ITEMSUIT",	COMMENT::ITEMSUIT,	mapFlag, SUIT_NSIZE);
	LoadHelpCommentString("ITEMATTACK",	COMMENT::ITEMATTACK,	mapFlag, ITEMATT_NSIZE+1);
	LoadHelpCommentString("ITEMADDON",	COMMENT::ITEMADDON,		mapFlag, EMADD_SIZE);
	LoadHelpCommentString("ITEMVOL",	COMMENT::ITEMVOL,		mapFlag, EMVAR_SIZE);
	LoadHelpCommentString( _T( "GRINDING_TYPE" ), COMMENT::GRINDING_TYPE, mapFlag, EMGRINDING_NSIZE );
	LoadHelpCommentString( _T( "GRINDING_RESIST" ), COMMENT::GRINDING_RESIST, mapFlag ,EMGRINDING_RS_NSIZE );
	LoadHelpCommentString( _T( "GRINDING_LEVEL" ), COMMENT::GRINDING_LEVEL, mapFlag, EMGRINDER_SIZE );
	LoadHelpCommentString( _T( "COOLTYPE" ),	COMMENT::COOLTYPE, mapFlag, EMCOOL_SIZE );
	LoadHelpCommentString( _T( "BIKETYPE" ),	COMMENT::BIKETYPE, mapFlag, EMBIKE_NSIZE );
	LoadHelpCommentString( _T( "BOARDTYPE" ),	COMMENT::BOARDTYPE, mapFlag, EMBIKE_NSIZE );
	LoadHelpCommentString( _T( "CARTYPE" ),		COMMENT::CARTYPE, mapFlag, EMBIKE_NSIZE );
	LoadHelpCommentString( _T( "REARCARTYPE" ), COMMENT::REARCARTYPE, mapFlag, EMBIKE_NSIZE );
	LoadHelpCommentString( _T( "TRIBE" ), COMMENT::TRIBE, mapFlag, TRIBE_NSIZE );
	LoadHelpCommentString( _T( "MOVETYPE" ), COMMENT::MOVETYPE, mapFlag, MOVETYPE_NSIZE );
	LoadHelpCommentString( _T( "ELEMENT" ), COMMENT::ELEMENT, mapFlag, EMELEMENT_MAXNUM2 );
	LoadHelpCommentString( _T( "BRIGHT" ), COMMENT::BRIGHT, mapFlag, BRIGHT_SIZE );
	LoadHelpCommentString( "IMPACT_TAR", COMMENT::IMPACT_TAR, mapFlag, EMIMPACT_TARGET_NSIZE );
	LoadHelpCommentString( "IMPACT_REALM", COMMENT::IMPACT_REALM, mapFlag, EMIMPACT_REALM_NSIZE );
	LoadHelpCommentString( "IMPACT_SIDE", COMMENT::IMPACT_SIDE, mapFlag, EMIMPACT_SIDE_NSIZE );
	LoadHelpCommentString(_T( "SPEC_STIGMA_TYPE" ),			COMMENT::SPEC_STIGMA_TYPE,		mapFlag, EMSPEC_STIGMA_TYPE_NSIZE);
	LoadHelpCommentString(_T( "SPEC_HALLUCINATE_TYPE" ),	COMMENT::SPEC_HALLUCINATE_TYPE, mapFlag, EMSPEC_HALLUCINATE_TYPE_NSIZE);
	LoadHelpCommentString(_T( "SPEC_DOMINATE_TYPE" ),	COMMENT::SPEC_DOMINATE_TYPE, mapFlag, EMSPEC_DOMINATE_TYPE_NSIZE);
	LoadHelpCommentString(_T( "SPEC_ASIGN_TYPE" ),		COMMENT::SPEC_ASIGN_TYPE,	mapFlag, EMSPEC_SKILL_ASING_NSIZE);
	LoadHelpCommentString(_T( "SPEC_INC_TYPE" ),		COMMENT::SPEC_INC_TYPE,		mapFlag, EMSPEC_INC_TYPE_NSIZE);
	LoadHelpCommentString(_T( "SPEC_INVISIBLE_USE_TYPE" ), &COMMENT::SPEC_INVISIBLE_USE_TYPE,	mapFlag, 1);
	LoadHelpCommentString(_T( "SPEC_INVISIBLE_ANI_TYPE" ),	COMMENT::SPEC_INVISIBLE_ANI_TYPE,	mapFlag, EMSPEC_INVISIBLE_ANI_TYPE_NSIZE);
	LoadHelpCommentString(_T( "SPEC_SWAPPOS_DIR_TYPE" ),	COMMENT::SPEC_SWAPPOS_DIR_TYPE,		mapFlag, EMSPEC_SWAPPOS_DIR_NSIZE);
	LoadHelpCommentString(_T( "SKILLCLASS" ),		COMMENT::SKILLCLASS,		mapFlag, EMSKILLCLASS_NSIZE );
	LoadHelpCommentString(_T( "LANDEFFECT_TYPE" ),	COMMENT::LANDEFFECT_TYPE,	mapFlag, EMLANDEFFECT_SIZE );
	LoadHelpCommentString(_T( "CROW" ),			COMMENT::CROW,			mapFlag, CROW_NUM );
	LoadHelpCommentString(_T( "ATTACK_RGTYPE" ),	COMMENT::ATTACK_RGTYPE,	mapFlag, EMATT_SIZE );
	LoadHelpCommentString(_T( "FONTSYS" ),			COMMENT::FONTSYS,		mapFlag, 2 );
	LoadHelpCommentString(_T( "ANI_MAINTYPE" ),	COMMENT::ANI_MAINTYPE,	mapFlag, AN_TYPE_SIZE );
	LoadHelpCommentString( _T( "ANI_VEHICLE_MAINTYPE" ), COMMENT::ANI_VEHICLE_MAINTYPE, mapFlag, AN_VEHICLE_SIZE );
	LoadHelpCommentString( _T( "STRIKE_EFF" ), COMMENT::STRIKE_EFF, mapFlag, EMSF_SIZE );
	LoadHelpCommentString( _T( "NPC_ACTIONTYPE" ), SNpcTalk::strACTIONTYPE, mapFlag, SNpcTalk::EM_TALK_SIZE );
	LoadHelpCommentString( _T( "NPC_BASICDESC" ), SNpcTalk::szBASICDESC, mapFlag, SNpcTalk::EM_BASIC_SIZE );
	LoadHelpCommentString( _T( "QUEST_PROGRESS" ), COMMENT::QUEST_PROGRESS, mapFlag, EMQP_SIZE );
	LoadHelpCommentString( _T( "SCHOOLNAME" ), GLCONST_CHAR::strSCHOOLNAME, mapFlag, MAX_SCHOOL );
	LoadHelpCommentString( _T( "VEHICLE_TYPE" ), COMMENT::VEHICLE_TYPE, mapFlag, VEHICLE_TYPE_SIZE );
	LoadHelpCommentString( _T( "POSTBOX_DESIGN_TYPE" ), COMMENT::POSTBOX_DESIGN_TYPE, mapFlag, EMPOSTBOX_DESIGN_TYPE_SIZE );
	LoadHelpCommentString( _T( "ACTIVITY_CLASS" ), COMMENT::ACTIVITY_CLASS, mapFlag, ACTIVITY_CLASS_SIZE );
	LoadHelpCommentString( _T( "ACTIVITY_CATEGORY" ), COMMENT::ACTIVITY_CATEGORY, mapFlag, ACTIVITY_CATEGORY_SIZE );
	LoadHelpCommentString( _T( "ACTIVITY_NOTIFY" ), COMMENT::ACTIVITY_NOTIFY, mapFlag, ACTIVITY_NOTIFY_SIZE );
	LoadHelpCommentString( _T( "ACTIVITY_MID" ), COMMENT::ACTIVITY_MID, mapFlag, ACTIVITY_MID_SIZE );
	LoadHelpCommentString( _T( "PRODUCT_TYPE_NAME" ), COMMENT::PRODUCT_TYPE_NAME, mapFlag, Product::ETypeTotal );
	LoadHelpCommentString( _T( "SKILL_LINK_APPLY_TYPE" ), COMMENT::SKILL_LINK_TYPE, mapFlag, EMITEM_SKILL_SIZE );
/*
	for ( INT i=0; i < SKILL::FOR_TYPE_SIZE; ++i )
	{
		COMMENT::SKILL_TYPES[i] = NS_TEXT_LOADER::GetText("SKILL_TYPES", i*2, mapFlag);
		COMMENT::SKILL_TYPES_SCALE[i] = atof(NS_TEXT_LOADER::GetText("SKILL_TYPES", i*2+1, mapFlag));
	}
	
	for ( INT i=0; i < EMSIGN_SIZE; ++i )
	{
		COMMENT::CDT_SIGN[i] = NS_TEXT_LOADER::GetText("CDT_SIGN", i*2, mapFlag);
		COMMENT::CDT_SIGN_ID[i] = NS_TEXT_LOADER::GetText("CDT_SIGN", i*2+1, mapFlag);
	}

	for ( INT i = 0; i < LEVEL_NSIZE; ++i )
	{
		COMMENT::ITEMLEVEL[i] = NS_TEXT_LOADER::GetText("ITEMLEVEL", i*4, mapFlag);
		COMMENT::ITEMCOLOR[i] = D3DCOLOR_ARGB(0xff,
			static_cast<WORD>(atoi(NS_TEXT_LOADER::GetText("ITEMLEVEL", i*4+1, mapFlag))),
			static_cast<WORD>(atoi(NS_TEXT_LOADER::GetText("ITEMLEVEL", i*4+2, mapFlag))),
			static_cast<WORD>(atoi(NS_TEXT_LOADER::GetText("ITEMLEVEL", i*4+3, mapFlag))));
	}
	
	for ( INT i = 0; i < EMITEM_DRUG::ITEM_DRUG_SIZE; ++i )
	{
		COMMENT::ITEMDRUG[i] = NS_TEXT_LOADER::GetText("ITEMDRUG", i*2, mapFlag);
		COMMENT::ITEMDRUG_VAR[i] = NS_TEXT_LOADER::GetText("ITEMDRUG", i*2+1, mapFlag);
	}
	
	for ( INT i = 0; i < EMVAR_SIZE; ++i )
	{
		COMMENT::ITEMVAR[i] = NS_TEXT_LOADER::GetText("ITEMVAR", i*2, mapFlag);
		COMMENT::ITEMVAR_SCALE[i] = atof(NS_TEXT_LOADER::GetText("ITEMVAR", i*2+1, mapFlag));
	}

	for ( INT i = 0; i < EMVAR_SIZE; ++i )
	{
		COMMENT::ITEM_QUE_TYPE[i] = NS_TEXT_LOADER::GetText("ITEM_QUE_TYPE", i*3, mapFlag);
		COMMENT::ITEM_QUE_VAR1[i] = NS_TEXT_LOADER::GetText("ITEM_QUE_TYPE", i*3+1, mapFlag);
		COMMENT::ITEM_QUE_VAR2[i] = NS_TEXT_LOADER::GetText("ITEM_QUE_TYPE", i*3+2, mapFlag);
	}

	for ( INT i = 0; i < RandomOption::RANDOM_OPTION_NSIZE; ++i )
	{
		COMMENT::ITEM_RANDOM_OPT[i] = NS_TEXT_LOADER::GetText("ITEM_RANDOM_OPT", i*2, mapFlag);
		COMMENT::ITEM_RANDOM_OPT_SCALE[i] = atof(NS_TEXT_LOADER::GetText("ITEM_RANDOM_OPT", i*2+1, mapFlag));
		++i;
	}

	for ( INT i = 0; i < GLCI_NUM_GUITAR; ++i )
	{
		COMMENT::CHARCLASS[i] = NS_TEXT_LOADER::GetText("CHARCLASS", i*2, mapFlag);
		COMMENT::CHARCOMMENT[i] = NS_TEXT_LOADER::GetText("CHARCLASS", i*2+1, mapFlag);
	}

	for ( INT i = 0; i < EMBLOW_SIZE; ++i )
	{
		COMMENT::BLOW[i] = NS_TEXT_LOADER::GetText("BLOW", i*5, mapFlag);
		COMMENT::BLOW_VAR1[i] = NS_TEXT_LOADER::GetText("BLOW", i*5+1, mapFlag);
		COMMENT::BLOW_VAR1_SCALE[i] = atof(NS_TEXT_LOADER::GetText("BLOW", i*5+2, mapFlag));
		COMMENT::BLOW_VAR2[i] = NS_TEXT_LOADER::GetText("BLOW", i*5+3, mapFlag);
		COMMENT::BLOW_VAR2_SCALE[i] = atof(NS_TEXT_LOADER::GetText("BLOW", i*5+4, mapFlag));
		++i;
	}

	for ( INT i = 0; i < EIMPACTA_SIZE; ++i )
	{
		COMMENT::IMPACT_ADDON[i] = NS_TEXT_LOADER::GetText("IMPACT_ADDON", i*2, mapFlag);
		COMMENT::IMPACT_ADDON_SCALE[i] = atof(NS_TEXT_LOADER::GetText("IMPACT_ADDON", i*2+1, mapFlag));
	}
	
	for ( INT i = 0; i < EMSPECA_NSIZE; ++i )
	{
		COMMENT::SPEC_ADDON[i]				= NS_TEXT_LOADER::GetText("SPEC_ADDON", i*5, mapFlag);
		COMMENT::SPEC_ADDON_VAR1[i]			= NS_TEXT_LOADER::GetText("SPEC_ADDON", i*5+1, mapFlag);
		COMMENT::SPEC_ADDON_VAR1_SCALE[i]	= atof(NS_TEXT_LOADER::GetText("SPEC_ADDON", i*5+2, mapFlag));
		COMMENT::SPEC_ADDON_VAR2[i]			= NS_TEXT_LOADER::GetText("SPEC_ADDON", i*5+3, mapFlag);
		COMMENT::SPEC_ADDON_VAR2_SCALE[i]	= atof(NS_TEXT_LOADER::GetText("SPEC_ADDON", i*5+4, mapFlag));
	}
	
	for ( INT i = 0; i < EMSPECA_NSIZE; ++i )
	{
		COMMENT::ANI_SUBTYPE[i]				= NS_TEXT_LOADER::GetText("ANI_SUBTYPE", i*4, mapFlag);
		COMMENT::ANI_SUBTYPE_00[i]			= NS_TEXT_LOADER::GetText("ANI_SUBTYPE", i*4+1, mapFlag);
		COMMENT::ANI_SUBTYPE_01[i]			= NS_TEXT_LOADER::GetText("ANI_SUBTYPE", i*4+2, mapFlag);
		COMMENT::ANI_SUBTYPE_03[i]			= NS_TEXT_LOADER::GetText("ANI_SUBTYPE", i*4+3, mapFlag);
	}
	
	NS_TEXT_LOADER::STRFLAG_IT FlagItr = mapFlag.find("QUEST_AREA");
	if ( FlagItr == mapFlag.end() )
	{
		CString strTemp;
		strTemp.Format("[GAME TEXT ERROR]Not exist text keyword : %1%", "QUEST_AREA");
		AfxMessageBox( strTemp, MB_OK|MB_ICONSTOP );
	}
	else
	{
		CStringArray *pStrArray = (*FlagItr).second;
		if ( pStrArray->GetCount () == 0 )
		{
			CString strTemp;
			strTemp.Format("[GAME TEXT ERROR]Index Range is 0 : %1%", "QUEST_AREA");
			AfxMessageBox( strTemp, MB_OK|MB_ICONSTOP );
		}
		for ( INT j = 0; j < pStrArray->GetCount(); ++j )
		{
			COMMENT::QUEST_AREA_NAME.push_back(pStrArray->GetAt(j).GetString());
		}
	}
	*/
	return true;
}
