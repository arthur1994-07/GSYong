#include <afxwin.h>
#include <afxext.h>
#include <afxole.h>
#include <atlbase.h>
#include <atlwin.h>
#include <afxcoll.h>

#include <windows.h>
#include <math.h>
#include <time.h>
#include <strsafe.h>

#include <stdio.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <boost/lexical_cast.hpp>

// 링커에 넣어줘도됨;
#pragma comment( lib, "winmm.lib" )
#include "../SigmaCore/Xml/2.5.2/tinyxml.h"

#include "../SigmaCore/String/StringUtils.h"
#include "../SigmaCore/String/StringUtil.h"
#include "../SigmaCore/File/StringFile.h"
#include "../SigmaCore/File/SerialFile.h"
#include "../SigmaCore/Encrypt/Rijndael.h"
#include "../SigmaCore/String/basestring.h"
#include "../../SigmaCore/File/TxtFile.h"
#include "../../SigmaCore/Lua/MinLua.h"
#include "../SigmaCore/String/StringFormat.h"

#include "../EngineLib/G-Logic/GLogic.h"
#include "../InternalCommonLib/dxstdafx.h"

#include "../SigmaCore/Xml/2.5.3/tinystr.h"
#include "XmlConverter.h"

#include "../RanLogic/GLogicDataMan.h"
#include "../RanLogic/Item/GLItem.h"
#include "../RanLogic/GLLevelFile.h"
#include "../RanLogic/Crow/GLMobSchedule.h"
#include "../RanLogic/Skill/GLSkill.h"
#include "../RanLogic/Quest/GLQuest.h"
#include "../RanLogic/Post/GLPostData.h"
#include "../RanLogic/Activity/Activity.h"
#include "../RanLogic/Product/GLProductRecipe.h"
#include "../enginelib/DxTools/DxFontMan.h"
#include "../RanLogic/GLCommentFile.h"

#include "../RanLogic/Pet/GLPetDefine.h"
#include "../RanLogic/Pet/GLPet.h"

int g_nCodePage_luaCom;

void ClearString(std::string* strArray, int nsize)
{
	ZeroMemory(strArray, sizeof(std::string)*nsize);
}

void XmlConverter::ClearLuaDefaultString()
{
	ClearString(COMMENT::SKILL_ROLE,			SKILL::EMROLE_NSIZE);
	ClearString(COMMENT::SKILL_APPLY,			SKILL::EMAPPLY_NSIZE);
	ClearString(COMMENT::SKILL_ACTION_TYPE,		SKILL::EMACTION_TYPE_NSIZE);
	ClearString(COMMENT::SKILL_EFFECT_TYPE,		SKILL::EMEFFECT_TYPE_NSIZE);
	ClearString(COMMENT::SKILL_TYPES,			SKILL::FOR_TYPE_SIZE);
	ClearString(COMMENT::SKILL_LEVEL,			SKILL::MAX_LEVEL);
	ClearString(COMMENT::SKILL_EFFTIME,			SKILL::EMTIME_NSIZE);
	ClearString(COMMENT::SKILL_EFFPOS,			SKILL::EMPOS_NSIZE);
	ClearString(COMMENT::SPECIAL_SKILL_TYPE,	SKILL::EMSSTYPE_NSIZE);
	ClearString(COMMENT::SKILLATTACK,			SKILL::SKILLATT_NSIZE + 1 );
	ClearString(COMMENT::CDT_SIGN,				EMSIGN_SIZE);
	ClearString(COMMENT::CDT_SIGN_ID,			EMSIGN_SIZE);
	ClearString(COMMENT::ADDON_NO,				ITEM::SSUIT::ADDON_SIZE);
	ClearString(COMMENT::szCROWACT_UP,			EMCROWACT_UP_NSIZE);
	ClearString(COMMENT::szCROWACT_DN,			EMCROWACT_DN_NSIZE);
	ClearString(COMMENT::NPC_TYPE,				NPC_TYPE_SIZE);
	ClearString(COMMENT::szEMREACTION,			REACT_SIZE + 1);
	ClearString(COMMENT::MOBACTIONS,			EMACTION_SIZE);
	ClearString(COMMENT::ITEMTYPE,				ITEM_NSIZE);
	ClearString(COMMENT::ITEMSUIT,				SUIT_NSIZE);
	ClearString(COMMENT::ITEMDRUG,				ITEM_DRUG_SIZE);
	ClearString(COMMENT::ITEMDRUG_VAR,			ITEM_DRUG_SIZE);
	ClearString(COMMENT::ITEMATTACK,			ITEMATT_NSIZE + 1);
	ClearString(COMMENT::ITEMADDON,				EMADD_SIZE);
	ClearString(COMMENT::ITEMVAR,				EMVAR_SIZE);
	ClearString(COMMENT::ITEMVOL,				EMVAR_SIZE);
	ClearString(COMMENT::ITEM_QUE_TYPE,			QUESTION_SIZE);
	ClearString(COMMENT::ITEM_QUE_VAR1,			QUESTION_SIZE);
	ClearString(COMMENT::ITEM_QUE_VAR2,			QUESTION_SIZE);
	ClearString(COMMENT::GRINDING_RESIST,		EMGRINDING_RS_NSIZE);
	ClearString(COMMENT::ITEM_RANDOM_OPT,		EMR_OPT_SIZE);
	ClearString(COMMENT::GRINDING_LEVEL,		EMGRINDER_SIZE);
	ClearString(COMMENT::COOLTYPE,					EMCOOL_SIZE);
	ClearString(COMMENT::BIKETYPE,					EMBIKE_NSIZE);
	ClearString(COMMENT::BOARDTYPE,					EMBIKE_NSIZE);
	ClearString(COMMENT::CARTYPE,					EMBIKE_NSIZE);
	ClearString(COMMENT::REARCARTYPE,				EMBIKE_NSIZE);
	ClearString(COMMENT::TRIBE,						TRIBE_NSIZE);
	ClearString(COMMENT::MOVETYPE,					MOVETYPE_NSIZE);
	ClearString(COMMENT::ELEMENT,					EMELEMENT_MAXNUM2);
	ClearString(COMMENT::BRIGHT,					BRIGHT_SIZE);
	ClearString(COMMENT::CHARCLASS,					GLCI_NUM_GUITAR);
	ClearString(COMMENT::CHARCOMMENT,				GLCI_NUM_GUITAR);
	ClearString(COMMENT::BLOW,						EMBLOW_SIZE);
	ClearString(COMMENT::BLOW_VAR1,					EMBLOW_SIZE);
	ClearString(COMMENT::BLOW_VAR2,					EMBLOW_SIZE);
	ClearString(COMMENT::IMPACT_ADDON,				EIMPACTA_SIZE);
	ClearString(COMMENT::IMPACT_TAR,				EMIMPACT_TARGET_NSIZE);
	ClearString(COMMENT::IMPACT_REALM,				EMIMPACT_REALM_NSIZE);
	ClearString(COMMENT::IMPACT_SIDE,				EMIMPACT_SIDE_NSIZE);
	ClearString(COMMENT::SPEC_ADDON,				EMSPECA_NSIZE);
	ClearString(COMMENT::SPEC_ADDON_VAR1,			EMSPECA_NSIZE);
	ClearString(COMMENT::SPEC_ADDON_VAR2,			EMSPECA_NSIZE);
	ClearString(COMMENT::ITEMLEVEL,					LEVEL_NSIZE);
	ClearString(COMMENT::ITEMLEVEL,					LEVEL_NSIZE);
	ClearString(COMMENT::ITEMLEVEL,					LEVEL_NSIZE);
	ClearString(COMMENT::ITEMLEVEL,					LEVEL_NSIZE);
	ClearString(COMMENT::ITEMLEVEL,					LEVEL_NSIZE);
	ClearString(COMMENT::ITEMLEVEL,					LEVEL_NSIZE);
	ClearString(COMMENT::ITEMLEVEL,					LEVEL_NSIZE);
	ClearString(COMMENT::ITEMLEVEL,					LEVEL_NSIZE);
	ClearString(COMMENT::ITEMLEVEL,					LEVEL_NSIZE);
	ClearString(COMMENT::ITEMLEVEL,					LEVEL_NSIZE);
	ClearString(COMMENT::ITEMLEVEL,					LEVEL_NSIZE);
	ClearString(COMMENT::ITEMLEVEL,					LEVEL_NSIZE);
	ClearString(COMMENT::SPEC_STIGMA_TYPE,			EMSPEC_STIGMA_TYPE_NSIZE);
	ClearString(COMMENT::SPEC_HALLUCINATE_TYPE,		EMSPEC_HALLUCINATE_TYPE_NSIZE);
	ClearString(COMMENT::SPEC_DOMINATE_TYPE,		EMSPEC_DOMINATE_TYPE_NSIZE);
	ClearString(COMMENT::SPEC_ASIGN_TYPE,			EMSPEC_SKILL_ASING_NSIZE);
	ClearString(COMMENT::SPEC_INC_TYPE,				EMSPEC_INC_TYPE_NSIZE);
	ClearString(&COMMENT::SPEC_INVISIBLE_USE_TYPE,	1);
	ClearString(COMMENT::SPEC_INVISIBLE_ANI_TYPE,	EMSPEC_INVISIBLE_ANI_TYPE_NSIZE);
	ClearString(COMMENT::SPEC_SWAPPOS_DIR_TYPE,		EMSPEC_SWAPPOS_DIR_NSIZE);

	ClearString(COMMENT::SKILLCLASS,				EMSKILLCLASS_NSIZE );
	ClearString(COMMENT::LANDEFFECT_TYPE,			EMLANDEFFECT_SIZE );

	ClearString(COMMENT::CROW, CROW_NUM );
	ClearString(COMMENT::ATTACK_RGTYPE, EMATT_SIZE );
	ClearString(COMMENT::FONTSYS, 2 );
	ClearString(COMMENT::ANI_MAINTYPE, AN_TYPE_SIZE );
	ClearString(COMMENT::ANI_SUBTYPE, AN_SUB_00_SIZE );
	ClearString(COMMENT::ANI_SUBTYPE_00, AN_SUB_00_SIZE );
	ClearString(COMMENT::ANI_SUBTYPE_01, AN_SUB_00_SIZE );
	ClearString(COMMENT::ANI_SUBTYPE_03, AN_SUB_00_SIZE );

	ClearString(COMMENT::ANI_VEHICLE_MAINTYPE, AN_VEHICLE_SIZE );
	ClearString(COMMENT::STRIKE_EFF, EMSF_SIZE );
	ClearString(SNpcTalk::strACTIONTYPE, SNpcTalk::EM_TALK_SIZE );
	ClearString(SNpcTalk::szBASICDESC, SNpcTalk::EM_BASIC_SIZE );
	ClearString(COMMENT::QUEST_PROGRESS, EMQP_SIZE );
	ClearString(GLCONST_CHAR::strSCHOOLNAME, MAX_SCHOOL );
	ClearString(COMMENT::VEHICLE_TYPE, VEHICLE_TYPE_SIZE );
	ClearString(COMMENT::POSTBOX_DESIGN_TYPE, EMPOSTBOX_DESIGN_TYPE_SIZE );
	for ( int i=0; i<COMMENT::QUEST_AREA_NAME.size(); ++i )
	{
		COMMENT::QUEST_AREA_NAME[i].clear();
	}


	ClearString(COMMENT::ACTIVITY_CLASS, ACTIVITY_CLASS_SIZE);
	ClearString(COMMENT::ACTIVITY_CATEGORY, ACTIVITY_CATEGORY_SIZE);
	ClearString(COMMENT::ACTIVITY_NOTIFY, ACTIVITY_NOTIFY_SIZE);
	ClearString(COMMENT::ACTIVITY_MID, ACTIVITY_MID_SIZE);
	ClearString(COMMENT::PRODUCT_TYPE_NAME, Product::ETypeTotal);
}

bool XmlConverter::RanTextConvertToXml_CommentLua_OnlyText(const char* szFile, const char* szSave, const char* szNational, bool bSave)
{
	TIXMLOLD::TiXmlDocument doc;
	if ( doc.LoadFile( szSave, TiXmlEncoding::TIXML_ENCODING_UTF8 ) )
	{
		m_XmlDocument = doc;
		m_pXmlRootElem = m_XmlDocument.RootElement();
		doc.Clear();
	}
	ClearLuaDefaultString();
	if ( LOADFILE_LUA(szFile) == false )
		return false;
	
	const language::SERVICENATION sNation = language::StrNationToEMNation(szNational);
	const int nLang = (int)language::NationToLanguage(sNation);
	g_nCodePage_luaCom = CD3DFontPar::nCodePage[nLang];

// 	CGLCommentFile CommentFile;
// 	if ( CommentFile.LOADFILE(szFile, true) == false )
// 		return false;

	_Lua_2_Composition("SKILL_ROLE",					COMMENT::SKILL_ROLE,			SKILL::EMROLE_NSIZE, szNational);
	_Lua_2_Composition("SKILL_APPLY",					COMMENT::SKILL_APPLY,			SKILL::EMAPPLY_NSIZE, szNational);
	_Lua_2_Composition("SKILL_ACTION_TYPE",				COMMENT::SKILL_ACTION_TYPE,		SKILL::EMACTION_TYPE_NSIZE, szNational);
	_Lua_2_Composition("SKILL_EFFECT_TYPE",				COMMENT::SKILL_EFFECT_TYPE,		SKILL::EMEFFECT_TYPE_NSIZE, szNational);
	_Lua_2_Composition("SKILL_TYPES",					COMMENT::SKILL_TYPES,			SKILL::FOR_TYPE_SIZE, szNational);
	_Lua_2_Composition("SKILL_LEVEL",					COMMENT::SKILL_LEVEL,			SKILL::MAX_LEVEL, szNational);
	_Lua_2_Composition("SKILL_EFFTIME",					COMMENT::SKILL_EFFTIME,			SKILL::EMTIME_NSIZE, szNational);
	_Lua_2_Composition("SKILL_EFFPOS",					COMMENT::SKILL_EFFPOS,			SKILL::EMPOS_NSIZE, szNational);
	_Lua_2_Composition("SPECIAL_SKILL_TYPE",			COMMENT::SPECIAL_SKILL_TYPE,	SKILL::EMSSTYPE_NSIZE, szNational);
	_Lua_2_Composition("SKILLATTACK",					COMMENT::SKILLATTACK,			SKILL::SKILLATT_NSIZE + 1 , szNational);
	_Lua_2_Composition("CDT_SIGN",						COMMENT::CDT_SIGN,				EMSIGN_SIZE, szNational);
	_Lua_2_Composition("CDT_SIGN_ID",					COMMENT::CDT_SIGN_ID,			EMSIGN_SIZE, szNational);
	_Lua_2_Composition("ADDON_NO",						COMMENT::ADDON_NO,				ITEM::SSUIT::ADDON_SIZE, szNational);
	_Lua_2_Composition("CROWACT_UP",					COMMENT::szCROWACT_UP,			EMCROWACT_UP_NSIZE, szNational);
	_Lua_2_Composition("CROWACT_DN",					COMMENT::szCROWACT_DN,			EMCROWACT_DN_NSIZE, szNational);
	_Lua_2_Composition("NPC_TYPE",						COMMENT::NPC_TYPE,				NPC_TYPE_SIZE, szNational);
	_Lua_2_Composition("EMREACTION",					COMMENT::szEMREACTION,			REACT_SIZE + 1, szNational);
	_Lua_2_Composition("MOBACTIONS",					COMMENT::MOBACTIONS,			EMACTION_SIZE, szNational);
	_Lua_2_Composition("ITEMTYPE",						COMMENT::ITEMTYPE,				ITEM_NSIZE, szNational);
	_Lua_2_Composition("ITEMSUIT",						COMMENT::ITEMSUIT,				SUIT_NSIZE, szNational);
	_Lua_2_Composition("ITEMDRUG",						COMMENT::ITEMDRUG,				ITEM_DRUG_SIZE, szNational);
	_Lua_2_Composition("ITEMDRUG_VAR",					COMMENT::ITEMDRUG_VAR,			ITEM_DRUG_SIZE, szNational);
	_Lua_2_Composition("ITEMATTACK",					COMMENT::ITEMATTACK,			ITEMATT_NSIZE + 1, szNational);
	_Lua_2_Composition("ITEMADDON",						COMMENT::ITEMADDON,				EMADD_SIZE, szNational);
	_Lua_2_Composition("ITEMVAR",						COMMENT::ITEMVAR,				EMVAR_SIZE, szNational);
	_Lua_2_Composition("ITEMVOL",						COMMENT::ITEMVOL,				EMVAR_SIZE, szNational);
	_Lua_2_Composition("ITEM_QUE_TYPE",					COMMENT::ITEM_QUE_TYPE,			QUESTION_SIZE, szNational);
	_Lua_2_Composition("ITEM_QUE_VAR1",					COMMENT::ITEM_QUE_VAR1,			QUESTION_SIZE, szNational);
	_Lua_2_Composition("ITEM_QUE_VAR2",					COMMENT::ITEM_QUE_VAR2,			QUESTION_SIZE, szNational);
	_Lua_2_Composition("GRINDING_RESIST",				COMMENT::GRINDING_RESIST,		EMGRINDING_RS_NSIZE, szNational);
	_Lua_2_Composition("ITEM_RANDOM_OPT",				COMMENT::ITEM_RANDOM_OPT,		EMR_OPT_SIZE, szNational);
	_Lua_2_Composition("GRINDING_LEVEL",				COMMENT::GRINDING_LEVEL,		EMGRINDER_SIZE, szNational);
	_Lua_2_Composition("COOLTYPE",						COMMENT::COOLTYPE,					EMCOOL_SIZE, szNational);
	_Lua_2_Composition("BIKETYPE",						COMMENT::BIKETYPE,					EMBIKE_NSIZE, szNational);
	_Lua_2_Composition("BOARDTYPE",						COMMENT::BOARDTYPE,					EMBIKE_NSIZE, szNational);
	_Lua_2_Composition("CARTYPE", 						COMMENT::CARTYPE,					EMBIKE_NSIZE, szNational);
	_Lua_2_Composition("REARCARTYPE",					COMMENT::REARCARTYPE,				EMBIKE_NSIZE, szNational);
	_Lua_2_Composition("TRIBE",							COMMENT::TRIBE,						TRIBE_NSIZE, szNational);
	_Lua_2_Composition("MOVETYPE",						COMMENT::MOVETYPE,					MOVETYPE_NSIZE, szNational);
	_Lua_2_Composition("ELEMENT",						COMMENT::ELEMENT,					EMELEMENT_MAXNUM2, szNational);
	_Lua_2_Composition("BRIGHT",						COMMENT::BRIGHT,					BRIGHT_SIZE, szNational);
	_Lua_2_Composition("CHARCLASS",						COMMENT::CHARCLASS,					GLCI_NUM_GUITAR, szNational);
	_Lua_2_Composition("CHARCOMMENT",					COMMENT::CHARCOMMENT,				GLCI_NUM_GUITAR, szNational);
	_Lua_2_Composition("BLOW",							COMMENT::BLOW,						EMBLOW_SIZE, szNational);
	_Lua_2_Composition("BLOW_VAR1",						COMMENT::BLOW_VAR1,					EMBLOW_SIZE, szNational);
	_Lua_2_Composition("BLOW_VAR2",						COMMENT::BLOW_VAR2,					EMBLOW_SIZE, szNational);
	_Lua_2_Composition("IMPACT_ADDON",					COMMENT::IMPACT_ADDON,				EIMPACTA_SIZE, szNational);
	_Lua_2_Composition("IMPACT_TAR",					COMMENT::IMPACT_TAR,				EMIMPACT_TARGET_NSIZE, szNational);
	_Lua_2_Composition("IMPACT_REALM",					COMMENT::IMPACT_REALM,				EMIMPACT_REALM_NSIZE, szNational);
	_Lua_2_Composition("IMPACT_SIDE",					COMMENT::IMPACT_SIDE,				EMIMPACT_SIDE_NSIZE, szNational);
	_Lua_2_Composition("SPEC_ADDON",					COMMENT::SPEC_ADDON,				EMSPECA_NSIZE, szNational);
	_Lua_2_Composition("SPEC_ADDON_VAR1",				COMMENT::SPEC_ADDON_VAR1,			EMSPECA_NSIZE, szNational);
	_Lua_2_Composition("SPEC_ADDON_VAR2",				COMMENT::SPEC_ADDON_VAR2,			EMSPECA_NSIZE, szNational);
	_Lua_2_Composition("ITEMLEVEL",						COMMENT::ITEMLEVEL,					LEVEL_NSIZE, szNational);
	_Lua_2_Composition("ITEMLEVEL",						COMMENT::ITEMLEVEL,					LEVEL_NSIZE, szNational);
	_Lua_2_Composition("ITEMLEVEL",						COMMENT::ITEMLEVEL,					LEVEL_NSIZE, szNational);
	_Lua_2_Composition("ITEMLEVEL",						COMMENT::ITEMLEVEL,					LEVEL_NSIZE, szNational);
	_Lua_2_Composition("ITEMLEVEL",						COMMENT::ITEMLEVEL,					LEVEL_NSIZE, szNational);
	_Lua_2_Composition("ITEMLEVEL",						COMMENT::ITEMLEVEL,					LEVEL_NSIZE, szNational);
	_Lua_2_Composition("ITEMLEVEL",						COMMENT::ITEMLEVEL,					LEVEL_NSIZE, szNational);
	_Lua_2_Composition("ITEMLEVEL",						COMMENT::ITEMLEVEL,					LEVEL_NSIZE, szNational);
	_Lua_2_Composition("ITEMLEVEL",						COMMENT::ITEMLEVEL,					LEVEL_NSIZE, szNational);
	_Lua_2_Composition("ITEMLEVEL",						COMMENT::ITEMLEVEL,					LEVEL_NSIZE, szNational);
	_Lua_2_Composition("ITEMLEVEL",						COMMENT::ITEMLEVEL,					LEVEL_NSIZE, szNational);
	_Lua_2_Composition("ITEMLEVEL",						COMMENT::ITEMLEVEL,					LEVEL_NSIZE, szNational);
	_Lua_2_Composition(_T( "SPEC_STIGMA_TYPE" ),		COMMENT::SPEC_STIGMA_TYPE,			EMSPEC_STIGMA_TYPE_NSIZE, szNational);
	_Lua_2_Composition(_T( "SPEC_HALLUCINATE_TYPE" ),	COMMENT::SPEC_HALLUCINATE_TYPE,		EMSPEC_HALLUCINATE_TYPE_NSIZE, szNational);
	_Lua_2_Composition(_T( "SPEC_DOMINATE_TYPE" ),		COMMENT::SPEC_DOMINATE_TYPE,		EMSPEC_DOMINATE_TYPE_NSIZE, szNational);
	_Lua_2_Composition(_T( "SPEC_ASIGN_TYPE" ),			COMMENT::SPEC_ASIGN_TYPE,			EMSPEC_SKILL_ASING_NSIZE, szNational);
	_Lua_2_Composition(_T( "SPEC_INC_TYPE" ),			COMMENT::SPEC_INC_TYPE,				EMSPEC_INC_TYPE_NSIZE, szNational);
	_Lua_2_Composition(_T( "SPEC_INVISIBLE_USE_TYPE" ),&COMMENT::SPEC_INVISIBLE_USE_TYPE,	1, szNational);
	_Lua_2_Composition(_T( "SPEC_INVISIBLE_ANI_TYPE" ), COMMENT::SPEC_INVISIBLE_ANI_TYPE,	EMSPEC_INVISIBLE_ANI_TYPE_NSIZE, szNational);
	_Lua_2_Composition(_T( "SPEC_SWAPPOS_DIR_TYPE" ),	COMMENT::SPEC_SWAPPOS_DIR_TYPE,		EMSPEC_SWAPPOS_DIR_NSIZE, szNational);

	_Lua_2_Composition( _T( "SKILLCLASS" ),				COMMENT::SKILLCLASS,				EMSKILLCLASS_NSIZE , szNational);
	_Lua_2_Composition( _T( "LANDEFFECT_TYPE" ),		COMMENT::LANDEFFECT_TYPE,			EMLANDEFFECT_SIZE , szNational);

	_Lua_2_Composition( _T( "CROW" ),					COMMENT::CROW, CROW_NUM , szNational);
	_Lua_2_Composition( _T( "ATTACK_RGTYPE" ),			COMMENT::ATTACK_RGTYPE, EMATT_SIZE , szNational);
	_Lua_2_Composition( _T( "FONTSYS" ),				COMMENT::FONTSYS, 2 , szNational);
	_Lua_2_Composition( _T( "ANI_MAINTYPE" ),			COMMENT::ANI_MAINTYPE, AN_TYPE_SIZE , szNational);
	_Lua_2_Composition( _T( "ANI_SUBTYPE" ),			COMMENT::ANI_SUBTYPE, AN_SUB_00_SIZE , szNational);
	_Lua_2_Composition( _T( "ANI_SUBTYPE_00" ),			COMMENT::ANI_SUBTYPE_00, AN_SUB_00_SIZE , szNational);
	_Lua_2_Composition( _T( "ANI_SUBTYPE_01" ),			COMMENT::ANI_SUBTYPE_01, AN_SUB_00_SIZE , szNational);
	_Lua_2_Composition( _T( "ANI_SUBTYPE_03" ),			COMMENT::ANI_SUBTYPE_03, AN_SUB_00_SIZE , szNational);

	_Lua_2_Composition( _T( "ANI_VEHICLE_MAINTYPE" ),	COMMENT::ANI_VEHICLE_MAINTYPE, AN_VEHICLE_SIZE , szNational);
	_Lua_2_Composition( _T( "STRIKE_EFF" ),				COMMENT::STRIKE_EFF, EMSF_SIZE , szNational);
	_Lua_2_Composition( _T( "NPC_ACTIONTYPE" ),			SNpcTalk::strACTIONTYPE, SNpcTalk::EM_TALK_SIZE , szNational);
	_Lua_2_Composition( _T( "NPC_BASICDESC" ),			SNpcTalk::szBASICDESC, SNpcTalk::EM_BASIC_SIZE , szNational);
	_Lua_2_Composition( _T( "QUEST_PROGRESS" ),			COMMENT::QUEST_PROGRESS, EMQP_SIZE , szNational);
	_Lua_2_Composition( _T( "SCHOOLNAME" ),				GLCONST_CHAR::strSCHOOLNAME, MAX_SCHOOL , szNational);
	_Lua_2_Composition( _T( "VEHICLE_TYPE" ),			COMMENT::VEHICLE_TYPE, VEHICLE_TYPE_SIZE , szNational);
	_Lua_2_Composition( _T( "POSTBOX_DESIGN_TYPE" ),	COMMENT::POSTBOX_DESIGN_TYPE, EMPOSTBOX_DESIGN_TYPE_SIZE , szNational);
	TiXmlElement* pElem;
	if (OK != GetChildElement(&pElem, m_pXmlRootElem, "Id", "QUEST_AREA"))
	{
		pElem = new TIXMLOLD::TiXmlElement("WORD");
		pElem->SetAttribute( "Ver", 1 );
		pElem->SetAttribute( "Id", "QUEST_AREA" );
		m_XmlDocument.RootElement()->LinkEndChild( pElem );
	}


	for ( int i=0; i<COMMENT::QUEST_AREA_NAME.size(); ++i )
	{
		TiXmlElement* pChildElem = new TiXmlElement("VALUE");
		pChildElem->SetAttribute( "Lang", szNational );
		pChildElem->SetAttribute( "Index", i );
		char szTemp[512];
		WCHAR wszTemp[512];
		strcpy(szTemp, COMMENT::QUEST_AREA_NAME[i].c_str());

		//MultiByteToWideChar(g_nCodePage_luaCom, 0, szTemp, strlen(szTemp)+1,wszTemp,sizeof(wszTemp)/sizeof(wszTemp[0]));			
		//WideCharToMultiByte(CP_UTF8, 0, wszTemp, -1, szTemp, sizeof(szTemp)/sizeof(szTemp[0]), NULL,NULL );

		TiXmlText* pText = new TiXmlText( strlen(szTemp) == 0 ? "--TEMP--":szTemp );
		pChildElem->LinkEndChild(pText);
		pElem->LinkEndChild(pChildElem);	
	}
//	m_pXmlRootElem->LinkEndChild(pElem);
	

	_Lua_2_Composition( _T( "ACTIVITY_CLASS" ), COMMENT::ACTIVITY_CLASS, ACTIVITY_CLASS_SIZE, szNational);
	_Lua_2_Composition( _T( "ACTIVITY_CATEGORY" ), COMMENT::ACTIVITY_CATEGORY, ACTIVITY_CATEGORY_SIZE, szNational);
	_Lua_2_Composition( _T( "ACTIVITY_NOTIFY" ), COMMENT::ACTIVITY_NOTIFY, ACTIVITY_NOTIFY_SIZE, szNational);
	_Lua_2_Composition( _T( "ACTIVITY_MID" ), COMMENT::ACTIVITY_MID, ACTIVITY_MID_SIZE, szNational);
	_Lua_2_Composition( _T( "PRODUCT_TYPE_NAME" ), COMMENT::PRODUCT_TYPE_NAME, Product::ETypeTotal, szNational);
/**/
	m_XmlDocument.SaveFile(szSave);
	return true;
}

bool XmlConverter::_Lua_2_Composition( const char* atrbData
									  , std::string* strArray
									  , const int nSize
									  , const char* szNation )
{
	TiXmlElement* pElem;
	if (OK != GetChildElement(&pElem, m_pXmlRootElem, "Id", atrbData))
	{
		pElem = new TIXMLOLD::TiXmlElement("WORD");
		pElem->SetAttribute( "Ver", 1 );
		pElem->SetAttribute( "Id", atrbData );
		m_XmlDocument.RootElement()->LinkEndChild( pElem );
	}

	const language::SERVICENATION sNation = language::StrNationToEMNation(szNation);
	const int nLang = (int)language::NationToLanguage(sNation);
	const int nCodePage = CD3DFontPar::nCodePage[nLang];

	for ( int i=0; i<nSize; ++i )
	{
		TiXmlElement* pChildElem = new TiXmlElement("VALUE");
		pChildElem->SetAttribute( "Lang", szNation );
		pChildElem->SetAttribute( "Index", i );

		char szTemp[512];
		WCHAR wszTemp[512];
		strcpy(szTemp, strArray[i].c_str());

//		MultiByteToWideChar(nCodePage, 0, szTemp, strlen(szTemp)+1,wszTemp,sizeof(wszTemp)/sizeof(wszTemp[0]));			
//		WideCharToMultiByte(CP_UTF8, 0, wszTemp, -1, szTemp, sizeof(szTemp)/sizeof(szTemp[0]), NULL,NULL );

		TiXmlText* pText = new TiXmlText( strlen(szTemp) == 0 ? "--TEMP--":szTemp );//strArray[i].c_str());
		pChildElem->LinkEndChild(pText);
		pElem->LinkEndChild(pChildElem);
	}
//	m_pXmlRootElem->LinkEndChild(pElem);

	return true;
}





//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
std::string XmlConverter::unicodeToUtf8(std::wstring& szW)
{
	std::string str;

	char szTEMP[512] = { 0 };
	WideCharToMultiByte(
		CP_UTF8,
		0,
		szW.c_str(),
		-1,
		szTEMP,
		512,
		NULL,
		NULL );
	str = szTEMP;
	return str;
}

void XmlConverter::LoadHelp(
			  const std::string& strTitle,
			  std::string* pList,
			  int nCount )
{
	std::wstring ws;

	try
	{
		int i = 0;
		LuaPlus::LuaObject ob = sc::lua_getGlobalFromName( strTitle.c_str() );
		for( LuaPlus::LuaTableIterator it( ob ); it.IsValid(); it.Next(), ++i )
		{
			if( i >= nCount )
				break;

			ws = reinterpret_cast< const wchar_t* >( it.GetValue().GetWString() );

			//char showsz[255];
			//WideCharToMultiByte(g_nCodePage_luaCom, 0, ws.c_str(), -1, showsz, sizeof(showsz)/sizeof(showsz[0]), NULL,NULL );
			pList[ i ] = unicodeToUtf8(ws).c_str();//showsz;
		}
		for ( ; i < nCount; ++i )
		{
			pList[ i ] = "-----TEMP-----";
		}
	}
	catch( const LuaPlus::LuaException& e )
	{        
		MessageBox( NULL, e.GetErrorMessage(), strTitle.c_str(), MB_OK );
	}
}

bool XmlConverter::ParseCommentLua()
{
	std::wstring ws;

	//==========================================================================

	LoadHelp( _T( "SKILL_ROLE" ), COMMENT::SKILL_ROLE, SKILL::EMROLE_NSIZE );
	LoadHelp( _T( "SKILL_APPLY" ), COMMENT::SKILL_APPLY, SKILL::EMAPPLY_NSIZE );
	LoadHelp( _T( "SKILL_ACTION_TYPE" ), COMMENT::SKILL_ACTION_TYPE, SKILL::EMACTION_TYPE_NSIZE );
	LoadHelp( _T( "SKILL_EFFECT_TYPE" ), COMMENT::SKILL_EFFECT_TYPE, SKILL::EMEFFECT_TYPE_NSIZE );	

	try
	{
		int i = 0;
		LuaPlus::LuaObject ob = sc::lua_getGlobalFromName( _T( "SKILL_TYPES" ) );
		for( LuaPlus::LuaTableIterator it( ob ); it.IsValid(); it.Next(), ++i )
		{
			if( i >= SKILL::FOR_TYPE_SIZE )
				break;

			ws = reinterpret_cast< const wchar_t* >( it.GetValue().GetWString() );
			COMMENT::SKILL_TYPES[ i ] = unicodeToUtf8(ws).c_str();

			it.Next();
			COMMENT::SKILL_TYPES_SCALE[ i ] = it.GetValue().GetFloat();
		}
	}
	catch( const LuaPlus::LuaException& e )
	{        
		MessageBox( NULL, e.GetErrorMessage(), _T( "SKILL_TYPES" ), MB_OK );
	}

	LoadHelp( _T( "SKILL_LEVEL" ), COMMENT::SKILL_LEVEL, SKILL::MAX_LEVEL );
	LoadHelp( _T( "SKILL_EFFTIME" ), COMMENT::SKILL_EFFTIME, SKILL::EMTIME_NSIZE );
	LoadHelp( _T( "SKILL_EFFPOS" ), COMMENT::SKILL_EFFPOS, SKILL::EMPOS_NSIZE );
	LoadHelp( _T( "SPECIAL_SKILL_TYPE" ), COMMENT::SPECIAL_SKILL_TYPE, SKILL::EMSSTYPE_NSIZE );
	LoadHelp( _T( "SKILLATTACK" ), COMMENT::SKILLATTACK, SKILL::SKILLATT_NSIZE + 1 );

	try
	{
		int i = 0;
		LuaPlus::LuaObject ob = sc::lua_getGlobalFromName( _T( "CDT_SIGN" ) );
		for( LuaPlus::LuaTableIterator it( ob ); it.IsValid(); it.Next(), ++i )
		{
			if( i >= EMSIGN_SIZE )
				break;

			ws = reinterpret_cast< const wchar_t* >( it.GetValue().GetWString() );
			COMMENT::CDT_SIGN[ i ] = unicodeToUtf8(ws).c_str();

			it.Next();
			ws = reinterpret_cast< const wchar_t* >( it.GetValue().GetWString() );
			COMMENT::CDT_SIGN_ID[ i ] = unicodeToUtf8(ws).c_str();
		}
	}
	catch( const LuaPlus::LuaException& e )
	{        
		MessageBox( NULL, e.GetErrorMessage(), _T( "CDT_SIGN" ), MB_OK );
	}

	//==========================================================================

	LoadHelp( _T( "ADDON_NO" ), COMMENT::ADDON_NO, ITEM::SSUIT::ADDON_SIZE );

	//==========================================================================

	LoadHelp( _T( "CROWACT_UP" ), COMMENT::szCROWACT_UP, EMCROWACT_UP_NSIZE );
	LoadHelp( _T( "CROWACT_DN" ), COMMENT::szCROWACT_DN, EMCROWACT_DN_NSIZE );
	LoadHelp( _T( "NPC_TYPE" ), COMMENT::NPC_TYPE, NPC_TYPE_SIZE );

	//==========================================================================

	LoadHelp( _T( "EMREACTION" ), COMMENT::szEMREACTION, REACT_SIZE + 1 );

	//==========================================================================

	LoadHelp( _T( "MOBACTIONS" ), COMMENT::MOBACTIONS, EMACTION_SIZE );

	//==========================================================================

	try
	{
		int i = 0;
		LuaPlus::LuaObject ob = sc::lua_getGlobalFromName( _T( "ITEMLEVEL" ) );
		for( LuaPlus::LuaTableIterator it( ob ); it.IsValid(); it.Next(), ++i )
		{
			if( i >= LEVEL_NSIZE )
				break;

			ws = reinterpret_cast< const wchar_t* >( it.GetValue().GetWString() );
			COMMENT::ITEMLEVEL[ i ] = unicodeToUtf8(ws).c_str();

			it.Next();
			WORD nR = static_cast< WORD >( it.GetValue().GetInteger() );
			it.Next();
			WORD nG = static_cast< WORD >( it.GetValue().GetInteger() );
			it.Next();
			WORD nB = static_cast< WORD >( it.GetValue().GetInteger() );
			COMMENT::ITEMCOLOR[ i ] = D3DCOLOR_ARGB( 0xff, nR, nG, nB );
		}
	}
	catch( const LuaPlus::LuaException& e )
	{        
		MessageBox( NULL, e.GetErrorMessage(), _T( "ITEMLEVEL" ), MB_OK );
	}

	LoadHelp( _T( "ITEMTYPE" ), COMMENT::ITEMTYPE, ITEM_NSIZE );
	LoadHelp( _T( "ITEMSUIT" ), COMMENT::ITEMSUIT, SUIT_NSIZE );
	//LoadHelp( _T( "ITEMDRUG" ), COMMENT::ITEMDRUG, ITEM_DRUG_SIZE );
	try
	{
		unsigned int i = 0;
		LuaPlus::LuaObject ob = sc::lua_getGlobalFromName( _T( "ITEMDRUG" ) );
		for( LuaPlus::LuaTableIterator it( ob ); it.IsValid(); it.Next(), ++i )
		{
			// 비표준 확장 사용문제로 Warning 수정;
			//if( i >= EMITEM_DRUG::ITEM_DRUG_SIZE )
			if( i >= static_cast<EMITEM_DRUG>( ITEM_DRUG_SIZE ) )
				break;

			ws = reinterpret_cast< const wchar_t* >( it.GetValue().GetWString() );
			COMMENT::ITEMDRUG[ i ] = unicodeToUtf8(ws).c_str();

			it.Next();
			ws = reinterpret_cast< const wchar_t* >( it.GetValue().GetWString() );
			COMMENT::ITEMDRUG_VAR[ i ] = unicodeToUtf8(ws).c_str();
		}
	}
	catch( const LuaPlus::LuaException& e )
	{        
		MessageBox( NULL, e.GetErrorMessage(), _T( "SKILL_TYPES" ), MB_OK );
	}

	LoadHelp( _T( "ITEMATTACK" ), COMMENT::ITEMATTACK, ITEMATT_NSIZE + 1 );
	LoadHelp( _T( "ITEMADDON" ), COMMENT::ITEMADDON, EMADD_SIZE );

	try
	{
		int i = 0;
		LuaPlus::LuaObject ob = sc::lua_getGlobalFromName( _T( "ITEMVAR" ) );
		for( LuaPlus::LuaTableIterator it( ob ); it.IsValid(); it.Next(), ++i )
		{
			if( i >= EMVAR_SIZE )
				break;

			ws = reinterpret_cast< const wchar_t* >( it.GetValue().GetWString() );
			COMMENT::ITEMVAR[ i ] = unicodeToUtf8(ws).c_str();

			it.Next();
			COMMENT::ITEMVAR_SCALE[ i ] = it.GetValue().GetFloat();
		}
	}
	catch( const LuaPlus::LuaException& e )
	{        
		MessageBox( NULL, e.GetErrorMessage(), _T( "ITEMVAR" ), MB_OK );
	}

	LoadHelp( _T( "ITEMVOL" ), COMMENT::ITEMVOL, EMVAR_SIZE );

	try
	{
		int i = 0;
		LuaPlus::LuaObject ob = sc::lua_getGlobalFromName( _T( "ITEM_QUE_TYPE" ) );
		for( LuaPlus::LuaTableIterator it( ob ); it.IsValid(); it.Next(), ++i )
		{
			if( i >= QUESTION_SIZE )
				break;

			ws = reinterpret_cast< const wchar_t* >( it.GetValue().GetWString() );
			COMMENT::ITEM_QUE_TYPE[ i ] = unicodeToUtf8(ws).c_str();

			it.Next();
			ws = reinterpret_cast< const wchar_t* >( it.GetValue().GetWString() );
			COMMENT::ITEM_QUE_VAR1[ i ] = unicodeToUtf8(ws).c_str();

			it.Next();
			ws = reinterpret_cast< const wchar_t* >( it.GetValue().GetWString() );
			COMMENT::ITEM_QUE_VAR2[ i ] = unicodeToUtf8(ws).c_str();
		}
	}
	catch( const LuaPlus::LuaException& e )
	{        
		MessageBox( NULL, e.GetErrorMessage(), _T( "ITEM_QUE_TYPE" ), MB_OK );
	}

	LoadHelp( _T( "GRINDING_TYPE" ), COMMENT::GRINDING_TYPE, EMGRINDING_NSIZE );
	LoadHelp( _T( "GRINDING_RESIST" ), COMMENT::GRINDING_RESIST, EMGRINDING_RS_NSIZE );

	try
	{
		int i = 0;
		LuaPlus::LuaObject ob = sc::lua_getGlobalFromName( _T( "ITEM_RANDOM_OPT" ) );
		for( LuaPlus::LuaTableIterator it( ob ); it.IsValid(); it.Next(), ++i )
		{
			if( i >= EMR_OPT_SIZE )
				break;

			ws = reinterpret_cast< const wchar_t* >( it.GetValue().GetWString() );
			COMMENT::ITEM_RANDOM_OPT[ i ] = unicodeToUtf8(ws).c_str();

			it.Next();
			COMMENT::ITEM_RANDOM_OPT_SCALE[ i ] = it.GetValue().GetFloat();
		}
	}
	catch( const LuaPlus::LuaException& e )
	{        
		MessageBox( NULL, e.GetErrorMessage(), _T( "ITEM_RANDOM_OPT" ), MB_OK );
	}

	LoadHelp( _T( "GRINDING_LEVEL" ), COMMENT::GRINDING_LEVEL, EMGRINDER_SIZE );
	LoadHelp( _T( "COOLTYPE" ), COMMENT::COOLTYPE, EMCOOL_SIZE );
	LoadHelp( _T( "BIKETYPE" ), COMMENT::BIKETYPE, EMBIKE_NSIZE );
	LoadHelp( _T( "BOARDTYPE" ), COMMENT::BOARDTYPE, EMBIKE_NSIZE );
	LoadHelp( _T( "CARTYPE" ), COMMENT::CARTYPE, EMBIKE_NSIZE );
	LoadHelp( _T( "REARCARTYPE" ), COMMENT::REARCARTYPE, EMBIKE_NSIZE );

	//==========================================================================

	LoadHelp( _T( "TRIBE" ), COMMENT::TRIBE, TRIBE_NSIZE );
	LoadHelp( _T( "MOVETYPE" ), COMMENT::MOVETYPE, MOVETYPE_NSIZE );
	LoadHelp( _T( "ELEMENT" ), COMMENT::ELEMENT, EMELEMENT_MAXNUM2 );
	LoadHelp( _T( "BRIGHT" ), COMMENT::BRIGHT, BRIGHT_SIZE );

	try
	{
		int i = 0;
		LuaPlus::LuaObject ob = sc::lua_getGlobalFromName( _T( "CHARCLASS" ) );
		for( LuaPlus::LuaTableIterator it( ob ); it.IsValid(); it.Next(), ++i )
		{
			if( i >= GLCI_NUM_GUITAR )
				break;

			ws = reinterpret_cast< const wchar_t* >( it.GetValue().GetWString() );
			COMMENT::CHARCLASS[ i ] = unicodeToUtf8(ws).c_str();

			it.Next();
			ws = reinterpret_cast< const wchar_t* >( it.GetValue().GetWString() );
			COMMENT::CHARCOMMENT[ i ] = unicodeToUtf8(ws).c_str();
		}
	}
	catch( const LuaPlus::LuaException& e )
	{        
		MessageBox( NULL, e.GetErrorMessage(), _T( "CHARCLASS" ), MB_OK );
	}

	try
	{
		int i = 0;
		LuaPlus::LuaObject ob = sc::lua_getGlobalFromName( _T( "BLOW" ) );
		for( LuaPlus::LuaTableIterator it( ob ); it.IsValid(); it.Next(), ++i )
		{
			if( i >= EMBLOW_SIZE )
				break;

			ws = reinterpret_cast< const wchar_t* >( it.GetValue().GetWString() );
			COMMENT::BLOW[ i ] = unicodeToUtf8(ws).c_str();

			it.Next();
			ws = reinterpret_cast< const wchar_t* >( it.GetValue().GetWString() );
			COMMENT::BLOW_VAR1[ i ] = unicodeToUtf8(ws).c_str();

			it.Next();
			COMMENT::BLOW_VAR1_SCALE[ i ] = it.GetValue().GetFloat();

			it.Next();
			ws = reinterpret_cast< const wchar_t* >( it.GetValue().GetWString() );
			COMMENT::BLOW_VAR2[ i ] = unicodeToUtf8(ws).c_str();

			it.Next();
			COMMENT::BLOW_VAR2_SCALE[ i ] = it.GetValue().GetFloat();
		}
	}
	catch( const LuaPlus::LuaException& e )
	{        
		MessageBox( NULL, e.GetErrorMessage(), _T( "BLOW" ), MB_OK );
	}

	try
	{
		int i = 0;
		LuaPlus::LuaObject ob = sc::lua_getGlobalFromName( _T( "IMPACT_ADDON" ) );
		for( LuaPlus::LuaTableIterator it( ob ); it.IsValid(); it.Next(), ++i )
		{
			if( i >= EIMPACTA_SIZE )
				break;

			ws = reinterpret_cast< const wchar_t* >( it.GetValue().GetWString() );
			COMMENT::IMPACT_ADDON[ i ] = unicodeToUtf8(ws).c_str();

			it.Next();
			COMMENT::IMPACT_ADDON_SCALE[ i ] = it.GetValue().GetFloat();
		}
	}
	catch( const LuaPlus::LuaException& e )
	{        
		MessageBox( NULL, e.GetErrorMessage(), _T( "IMPACT_ADDON" ), MB_OK );
	}

	LoadHelp( _T( "IMPACT_TAR" ), COMMENT::IMPACT_TAR, EMIMPACT_TARGET_NSIZE );
	LoadHelp( _T( "IMPACT_REALM" ), COMMENT::IMPACT_REALM, EMIMPACT_REALM_NSIZE );
	LoadHelp( _T( "IMPACT_SIDE" ), COMMENT::IMPACT_SIDE, EMIMPACT_SIDE_NSIZE );

	try
	{
		int i = 0;
		LuaPlus::LuaObject ob = sc::lua_getGlobalFromName( _T( "SPEC_ADDON" ) );
		for( LuaPlus::LuaTableIterator it( ob ); it.IsValid(); it.Next(), ++i )
		{
			if( i >= EMSPECA_NSIZE )
				break;

			ws = reinterpret_cast< const wchar_t* >( it.GetValue().GetWString() );
			COMMENT::SPEC_ADDON[ i ] = unicodeToUtf8(ws).c_str();

			it.Next();
			ws = reinterpret_cast< const wchar_t* >( it.GetValue().GetWString() );
			COMMENT::SPEC_ADDON_VAR1[ i ] = unicodeToUtf8(ws).c_str();

			it.Next();
			COMMENT::SPEC_ADDON_VAR1_SCALE[ i ] = it.GetValue().GetFloat();

			it.Next();
			ws = reinterpret_cast< const wchar_t* >( it.GetValue().GetWString() );
			COMMENT::SPEC_ADDON_VAR2[ i ] = unicodeToUtf8(ws).c_str();

			it.Next();
			COMMENT::SPEC_ADDON_VAR2_SCALE[ i ] = it.GetValue().GetFloat();
		}
	}
	catch( const LuaPlus::LuaException& e )
	{        
		MessageBox( NULL, e.GetErrorMessage(), _T( "SPEC_ADDON" ), MB_OK );
	}

	LoadHelp(_T( "SPEC_STIGMA_TYPE" ), COMMENT::SPEC_STIGMA_TYPE, EMSPEC_STIGMA_TYPE_NSIZE);
	LoadHelp(_T( "SPEC_HALLUCINATE_TYPE" ), COMMENT::SPEC_HALLUCINATE_TYPE, EMSPEC_HALLUCINATE_TYPE_NSIZE);
	LoadHelp(_T( "SPEC_DOMINATE_TYPE" ), COMMENT::SPEC_DOMINATE_TYPE, EMSPEC_DOMINATE_TYPE_NSIZE);
	LoadHelp(_T( "SPEC_ASIGN_TYPE" ), COMMENT::SPEC_ASIGN_TYPE, EMSPEC_SKILL_ASING_NSIZE);
	LoadHelp(_T( "SPEC_INC_TYPE" ), COMMENT::SPEC_INC_TYPE, EMSPEC_INC_TYPE_NSIZE);
	LoadHelp(_T( "SPEC_INVISIBLE_USE_TYPE" ), &COMMENT::SPEC_INVISIBLE_USE_TYPE, 1);
	LoadHelp(_T( "SPEC_INVISIBLE_ANI_TYPE" ), COMMENT::SPEC_INVISIBLE_ANI_TYPE, EMSPEC_INVISIBLE_ANI_TYPE_NSIZE);
	LoadHelp(_T( "SPEC_SWAPPOS_DIR_TYPE" ), COMMENT::SPEC_SWAPPOS_DIR_TYPE, EMSPEC_SWAPPOS_DIR_NSIZE);

	LoadHelp( _T( "SKILLCLASS" ), COMMENT::SKILLCLASS, EMSKILLCLASS_NSIZE );
	LoadHelp( _T( "LANDEFFECT_TYPE" ), COMMENT::LANDEFFECT_TYPE, EMLANDEFFECT_SIZE );

	//==========================================================================

	LoadHelp( _T( "CROW" ), COMMENT::CROW, CROW_NUM );
	LoadHelp( _T( "ATTACK_RGTYPE" ), COMMENT::ATTACK_RGTYPE, EMATT_SIZE );

	//==========================================================================

	LoadHelp( _T( "FONTSYS" ), COMMENT::FONTSYS, 2 );

	//==========================================================================

	LoadHelp( _T( "ANI_MAINTYPE" ), COMMENT::ANI_MAINTYPE, AN_TYPE_SIZE );

	try
	{
		int i = 0;
		LuaPlus::LuaObject ob = sc::lua_getGlobalFromName( _T( "ANI_SUBTYPE" ) );
		for( LuaPlus::LuaTableIterator it( ob ); it.IsValid(); it.Next(), ++i )
		{
			if( i >= AN_SUB_00_SIZE )
				break;

			ws = reinterpret_cast< const wchar_t* >( it.GetValue().GetWString() );
			COMMENT::ANI_SUBTYPE[ i ] = unicodeToUtf8(ws).c_str();

			it.Next();
			ws = reinterpret_cast< const wchar_t* >( it.GetValue().GetWString() );
			COMMENT::ANI_SUBTYPE_00[ i ] = unicodeToUtf8(ws).c_str();

			it.Next();
			ws = reinterpret_cast< const wchar_t* >( it.GetValue().GetWString() );
			COMMENT::ANI_SUBTYPE_01[ i ] = unicodeToUtf8(ws).c_str();

			it.Next();
			ws = reinterpret_cast< const wchar_t* >( it.GetValue().GetWString() );
			COMMENT::ANI_SUBTYPE_03[ i ] = unicodeToUtf8(ws).c_str();
		}
	}
	catch( const LuaPlus::LuaException& e )
	{        
		MessageBox( NULL, e.GetErrorMessage(), _T( "ANI_SUBTYPE" ), MB_OK );
	}

	LoadHelp( _T( "ANI_VEHICLE_MAINTYPE" ), COMMENT::ANI_VEHICLE_MAINTYPE, AN_VEHICLE_SIZE );
	LoadHelp( _T( "STRIKE_EFF" ), COMMENT::STRIKE_EFF, EMSF_SIZE );

	//==========================================================================

	LoadHelp( _T( "NPC_ACTIONTYPE" ), SNpcTalk::strACTIONTYPE, SNpcTalk::EM_TALK_SIZE );
	LoadHelp( _T( "NPC_BASICDESC" ), SNpcTalk::szBASICDESC, SNpcTalk::EM_BASIC_SIZE );
	LoadHelp( _T( "QUEST_PROGRESS" ), COMMENT::QUEST_PROGRESS, EMQP_SIZE );
	LoadHelp( _T( "SCHOOLNAME" ), GLCONST_CHAR::strSCHOOLNAME, MAX_SCHOOL );
	LoadHelp( _T( "VEHICLE_TYPE" ), COMMENT::VEHICLE_TYPE, VEHICLE_TYPE_SIZE );

	//==========================================================================

	LoadHelp( _T( "POSTBOX_DESIGN_TYPE" ), COMMENT::POSTBOX_DESIGN_TYPE, EMPOSTBOX_DESIGN_TYPE_SIZE );

	//==========================================================================

	try
	{
		LuaPlus::LuaObject ob = sc::lua_getGlobalFromName( _T( "QUEST_AREA" ) );
		for( LuaPlus::LuaTableIterator it( ob ); it.IsValid(); it.Next() )
		{
			ws = reinterpret_cast< const wchar_t* >( it.GetValue().GetWString() );
			COMMENT::QUEST_AREA_NAME.push_back( sc::string::unicodeToUtf8( ws ) );
		}
	}
	catch( const LuaPlus::LuaException& e )
	{        
		MessageBox( NULL, e.GetErrorMessage(), _T( "QUEST_AREA" ), MB_OK );
	}

	//==========================================================================

	LoadHelp( _T( "ACTIVITY_CLASS" ), COMMENT::ACTIVITY_CLASS, ACTIVITY_CLASS_SIZE );
	LoadHelp( _T( "ACTIVITY_CATEGORY" ), COMMENT::ACTIVITY_CATEGORY, ACTIVITY_CATEGORY_SIZE );
	LoadHelp( _T( "ACTIVITY_NOTIFY" ), COMMENT::ACTIVITY_NOTIFY, ACTIVITY_NOTIFY_SIZE );
	LoadHelp( _T( "ACTIVITY_MID" ), COMMENT::ACTIVITY_MID, ACTIVITY_MID_SIZE );

	//==========================================================================

	//  [2/14/2013 hsshin]
	LoadHelp( _T( "PRODUCT_TYPE_NAME" ), COMMENT::PRODUCT_TYPE_NAME, Product::ETypeTotal );

	//==========================================================================

	return true;
}

bool XmlConverter::LOADFILE_LUA( const std::string& strPath )
{
	if( strPath.empty() )
		return false;

	char szName[ _MAX_FNAME ] = _T( "" );
	char szExt[ _MAX_EXT ] = _T( "" );
	_splitpath( strPath.c_str(), NULL, NULL, szName, szExt );
	StringCchCat( szName, _MAX_FNAME, szExt );

	std::auto_ptr< sc::BaseStream > pStream(
		GLOGIC::openfile_basestream(
		GLOGIC::UseLogicZipFile(),
		GLogicData::GetInstance().GetGlogicZipFile(),
		strPath.c_str(), 
		szName, 
		false,
		GLOGIC::UseLogicPackFile() ) );

	if( !pStream.get() )
	{
		sc::writeLogError(
			sc::string::format(
			"CGLCommentFile::LOADFILE_LUA(), File Open %1%", strPath ) );

		return false;
	}

	size_t nSize = static_cast< size_t >( pStream->GetFileSize() );
	void* pBuffer = malloc( nSize );
	pStream->ReadBuffer( pBuffer, 0 );
	CTypeSmartPtr cPtrData( pBuffer );

	try
	{
		sc::lua_init();

		if( !sc::lua_doBuffer( static_cast< char* >( pBuffer ), nSize, NULL ) )
		{
			MessageBox( NULL, sc::lua_getError().c_str(), strPath.c_str(), MB_OK );

			return false;
		}

		return ParseCommentLua();
	}
	catch( const LuaPlus::LuaException& e )
	{        
		MessageBox( NULL, e.GetErrorMessage(), strPath.c_str(), MB_OK );

		return false;
	}

	return false;
}
