#include "pch.h"

#include "../SigmaCore/File/DepthWithTextToFile.h"
#include "../SigmaCore/String/StringFormat.h"
#include "../SigmaCore/Log/LogMan.h"
#include "../SigmaCore/Lua/MinLua.h"
#include "../EngineLib/Common/IniLoader.h"
#include "../EngineLib/DxTools/DxFontMan.h"
#include "../EngineLib/DxMeshs/DxAniKeys.h"
#include "../EngineLib/G-Logic/GLogic.h"
#include "../EngineLib/GUInterface/GameTextControl.h"

#include "./NpcTalk/NpcTalk.h"
#include "./GLUseFeatures.h"

#include "./Item/GLItem.h"
#include "./GLLevelFile.h"
#include "./Crow/GLMobSchedule.h"
#include "./Skill/GLSkill.h"
#include "./Quest/GLQuest.h"
#include "./Post/GLPostData.h"
#include "./Activity/Activity.h"
#include "./GLCommentFile.h"
#include "./Product/GLProductRecipe.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

bool CGLCommentFile::LOADFILE( const std::string& strName, bool bToolMode )
{
	std::string strPath( GLOGIC::GetPath() );
	strPath.append( strName );

	return LOADFILE_LUA( strPath );
}

bool CGLCommentFile::LOADFILE_INI( const std::string& strPath, bool bToolMode )
{
	if( strPath.empty() )
        return false;	

	CIniLoader cFILE( ",", bToolMode );

	if( GLOGIC::UseLogicZipFile() )
		cFILE.SetZipFile(GLogicData::GetInstance().GetGlogicZipFile());

	if( !cFILE.open( strPath, true, GLOGIC::UseLogicPackFile() ) )
	{
        sc::writeLogError(
            sc::string::format(
                "CGLCommentFile::LOADFILE_INI(), File Open %1%", strPath ) );

		return false;
	}

	int i = 0;

	//==========================================================================

	for ( i=0; i<SKILL::EMROLE_NSIZE; ++i )
		cFILE.getflag( "Skill", "SKILL_ROLE", i, SKILL::EMROLE_NSIZE, COMMENT::SKILL_ROLE[i] );

	for ( i=0; i<SKILL::EMAPPLY_NSIZE; ++i )
		cFILE.getflag( "Skill", "SKILL_APPLY", i, SKILL::EMAPPLY_NSIZE, COMMENT::SKILL_APPLY[i] );

	for ( i=0; i<SKILL::EMACTION_TYPE_NSIZE; ++i )
		cFILE.getflag( "Skill", "SKILL_ACTION_TYPE", i, SKILL::EMACTION_TYPE_NSIZE, COMMENT::SKILL_ACTION_TYPE[i]);

	for ( i=0; i<SKILL::FOR_TYPE_SIZE; ++i )
	{
		cFILE.getflag( "Skill", "SKILL_TYPES", i, SKILL::FOR_TYPE_SIZE, COMMENT::SKILL_TYPES[i]);
		cFILE.getflag( "Skill", "SKILL_TYPES_SCALE", i, SKILL::FOR_TYPE_SIZE, COMMENT::SKILL_TYPES_SCALE[i]);
	}

	for ( i=0; i<SKILL::MAX_LEVEL; ++i )
		cFILE.getflag( "Skill", "SKILL_LEVEL", i, SKILL::MAX_LEVEL, COMMENT::SKILL_LEVEL[i]);

	for ( i=0; i<SKILL::EMTIME_NSIZE; ++i )
		cFILE.getflag( "Skill", "SKILL_EFFTIME", i, SKILL::EMTIME_NSIZE, COMMENT::SKILL_EFFTIME[i]);

	for ( i=0; i<SKILL::EMPOS_NSIZE; ++i )
		cFILE.getflag( "Skill", "SKILL_EFFPOS", i, SKILL::EMPOS_NSIZE, COMMENT::SKILL_EFFPOS[i]);

	for ( i=0; i<SKILL::EMSSTYPE_NSIZE; ++i )
		cFILE.getflag( "Skill", "SPECIAL_SKILL_TYPE", i, SKILL::EMSSTYPE_NSIZE, COMMENT::SPECIAL_SKILL_TYPE[i]);

	for ( i = 0; i<SKILL::EMEFFECT_TYPE_NSIZE; ++i )
		cFILE.getflag( "Skill", "SKILL_EFFECT_TYPE", i, SKILL::EMEFFECT_TYPE_NSIZE, COMMENT::SKILL_EFFECT_TYPE[i]);

	for ( i = 0; i<SKILL::EMKEYWORD_NSIZE; ++i )
		cFILE.getflag( "Skill", "SKILL_KEYWORD", i, SKILL::EMKEYWORD_NSIZE, COMMENT::SKILL_KEYWORD[i]);
	
	for ( i=0; i<SKILL::SKILLATT_NSIZE+1; ++i )
		cFILE.getflag( "Skill", "SKILLATTACK", i, SKILL::SKILLATT_NSIZE+1, COMMENT::SKILLATTACK[i]);	

	//==========================================================================

	for ( i=0; i<EMSIGN_SIZE; ++i )
	{
		cFILE.getflag( "Level", "CDT_SIGN", i, EMSIGN_SIZE, COMMENT::CDT_SIGN[i]);
		cFILE.getflag( "Level", "CDT_SIGN_ID", i, EMSIGN_SIZE, COMMENT::CDT_SIGN_ID[i]);
	}

	//==========================================================================

	for ( i=0; i<ITEM::SSUIT::ADDON_SIZE; ++i )
		cFILE.getflag( "Item", "ADDON_NO", i, ITEM::SSUIT::ADDON_SIZE, COMMENT::ADDON_NO[i]);

	//==========================================================================

	for ( i=0; i<EMCROWACT_UP_NSIZE; ++i )
		cFILE.getflag( "Crow", "CROWACT_UP", i, EMCROWACT_UP_NSIZE, COMMENT::szCROWACT_UP[i]);

	for ( i=0; i<EMCROWACT_DN_NSIZE; ++i )
		cFILE.getflag( "Crow", "CROWACT_DN", i, EMCROWACT_DN_NSIZE, COMMENT::szCROWACT_DN[i]);

	for ( i=0; i<NPC_TYPE_SIZE; ++i )
		cFILE.getflag( "Crow", "NPC_TYPE", i, NPC_TYPE_SIZE, COMMENT::NPC_TYPE[i]);

	//==========================================================================

	for ( i=0; i<REACT_SIZE+1; ++i )
		cFILE.getflag( "Character", "EMREACTION", i, REACT_SIZE+1, COMMENT::szEMREACTION[i]);
	
	//==========================================================================

	for ( i=0; i<EMACTION_SIZE; ++i )
		cFILE.getflag( "MobSchedule", "MOBACTIONS", i, EMACTION_SIZE, COMMENT::MOBACTIONS[i]);

	//==========================================================================

	for ( i=0; i<EMITEM_LEVEL_NSIZE; ++i )
		cFILE.getflag( "ItemDef", "ITEMLEVEL", i, EMITEM_LEVEL_NSIZE, COMMENT::ITEMLEVEL[i]);

	DWORD dwColor_R;
	DWORD dwColor_G;
	DWORD dwColor_B;

	for ( i=0; i<EMITEM_LEVEL_NSIZE; ++i )
	{
		cFILE.getflag( "ItemDef", "ITEMCOLOR", i*3, EMITEM_LEVEL_NSIZE * 3, dwColor_R);
		cFILE.getflag( "ItemDef", "ITEMCOLOR", i*3+1, EMITEM_LEVEL_NSIZE * 3, dwColor_G);
		cFILE.getflag( "ItemDef", "ITEMCOLOR", i*3+2, EMITEM_LEVEL_NSIZE * 3, dwColor_B);
		
		COMMENT::ITEMCOLOR[i] = D3DCOLOR_ARGB(0xff,dwColor_R,dwColor_G,dwColor_B);
	}

    int nItemTypesize = cFILE.GetKeyDataSize( "ItemDef", "ITEMTYPE" );
	for ( i=0; i<ITEM_NSIZE; ++i )
		cFILE.getflag( "ItemDef", "ITEMTYPE", i, ITEM_NSIZE, COMMENT::ITEMTYPE[i]);

	for ( i=0; i<SUIT_NSIZE; ++i )
		cFILE.getflag( "ItemDef", "ITEMSUIT", i, SUIT_NSIZE, COMMENT::ITEMSUIT[i]);

	for ( i=0; i<ITEM_DRUG_SIZE; ++i )
		cFILE.getflag( "ItemDef", "ITEMDRUG", i, ITEM_DRUG_SIZE, COMMENT::ITEMDRUG[i]);

	//for ( i=0; i<SLOT_NSIZE_S; ++i )
	//	cFILE.getflag( "ItemDef", "ITEMSLOT", i, SLOT_NSIZE_S, COMMENT::ITEMSLOT[i]);

	for ( i=0; i<ITEMATT_NSIZE+1; ++i )
		cFILE.getflag( "ItemDef", "ITEMATTACK", i, ITEMATT_NSIZE+1, COMMENT::ITEMATTACK[i]);


	for ( i=0; i<EMADD_SIZE; ++i )
		cFILE.getflag( "ItemDef", "ITEMADDON", i, EMADD_SIZE, COMMENT::ITEMADDON[i]);

	for ( i=0; i<EMVAR_SIZE; ++i )
		cFILE.getflag( "ItemDef", "ITEMVAR", i, EMVAR_SIZE, COMMENT::ITEMVAR[i]);

	for ( i=0; i<EMVAR_SIZE; ++i )
		cFILE.getflag( "ItemDef", "ITEMVOL", i, EMVAR_SIZE, COMMENT::ITEMVOL[i]);

	for ( i=0; i<QUESTION_SIZE; ++i )
	{
		cFILE.getflag( "ItemDef", "ITEM_QUE_TYPE", i, QUESTION_SIZE, COMMENT::ITEM_QUE_TYPE[i] );
		cFILE.getflag( "ItemDef", "ITEM_QUE_VAR1", i, QUESTION_SIZE, COMMENT::ITEM_QUE_VAR1[i] );
		cFILE.getflag( "ItemDef", "ITEM_QUE_VAR2", i, QUESTION_SIZE, COMMENT::ITEM_QUE_VAR2[i] );
	}

	for ( i=0; i<EMVAR_SIZE; ++i )
		cFILE.getflag( "ItemDef", "ITEMVAR_SCALE", i, EMVAR_SIZE, COMMENT::ITEMVAR_SCALE[i]);

	for ( i=0; i<EMGRINDING_NSIZE; ++i )
		cFILE.getflag( "ItemDef", "GRINDING_TYPE", i, EMGRINDING_NSIZE, COMMENT::GRINDING_TYPE[i]);

	for ( i=0; i<EMGRINDING_RS_NSIZE; ++i )
		cFILE.getflag( "ItemDef", "GRINDING_RESIST", i, EMGRINDING_RS_NSIZE, COMMENT::GRINDING_RESIST[i]);

    for ( i=0; i<RandomOption::RANDOM_OPTION_NSIZE; ++i )
        cFILE.getflag( "ItemDef", "ITEM_RANDOM_OPT", i, RandomOption::RANDOM_OPTION_NSIZE, COMMENT::ITEM_RANDOM_OPT[i]);

    for ( i=0; i<RandomOption::RANDOM_OPTION_NSIZE; ++i )
        cFILE.getflag( "ItemDef", "ITEM_RANDOM_OPT_SCALE", i, RandomOption::RANDOM_OPTION_NSIZE, COMMENT::ITEM_RANDOM_OPT_SCALE[i]);

	for ( i=0; i<EMGRINDER_SIZE; ++i )
		cFILE.getflag( "ItemDef", "GRINDING_LEVEL", i, EMGRINDER_SIZE, COMMENT::GRINDING_LEVEL[i]);

	for ( i=0; i<EMCOOL_SIZE; ++i )
		cFILE.getflag( "ItemDef", "COOLTYPE", i, EMCOOL_SIZE, COMMENT::COOLTYPE[i]);

	for ( i=0; i<EMBIKE_NSIZE; ++i )
		cFILE.getflag( "ItemDef", "BIKETYPE", i, EMBIKE_NSIZE, COMMENT::BIKETYPE[i]);

	for ( i=0; i<EMBIKE_NSIZE; ++i )
		cFILE.getflag( "ItemDef", "BOARDTYPE", i, EMBIKE_NSIZE, COMMENT::BOARDTYPE[i]);

	for ( i=0; i<EMBIKE_NSIZE; ++i )
		cFILE.getflag( "ItemDef", "CARTYPE", i, EMBIKE_NSIZE, COMMENT::CARTYPE[i]);

	for ( i=0; i<EMBIKE_NSIZE; ++i )
		cFILE.getflag( "ItemDef", "REARCARTYPE", i, EMBIKE_NSIZE, COMMENT::REARCARTYPE[i]);

	//==========================================================================

	for ( i=0; i<TRIBE_NSIZE; ++i )
		cFILE.getflag( "CharDefine", "TRIBE", i, TRIBE_NSIZE, COMMENT::TRIBE[i]);

	for ( i=0; i<MOVETYPE_NSIZE; ++i )
		cFILE.getflag( "CharDefine", "MOVETYPE", i, MOVETYPE_NSIZE, COMMENT::MOVETYPE[i]);

	for ( i=0; i<EMELEMENT_MAXNUM2; ++i )
		cFILE.getflag( "CharDefine", "ELEMENT", i, EMELEMENT_MAXNUM2, COMMENT::ELEMENT[i]);

	for ( i=0; i<BRIGHT_SIZE; ++i )
		cFILE.getflag( "CharDefine", "BRIGHT", i, BRIGHT_SIZE, COMMENT::BRIGHT[i]);

	for ( i=0; i<GLCI_NUM_ACTOR; ++i )
	{
		cFILE.getflag( "CharDefine", "CHARCLASS", i, GLCI_NUM_ACTOR, COMMENT::CHARCLASS[i]);
		cFILE.getflag( "CharDefine", "CHARCOMMENT", i, GLCI_NUM_ACTOR, COMMENT::CHARCOMMENT[i]);
	}

	for ( i=0; i<EMBLOW_SIZE; ++i )
	{
		cFILE.getflag( "CharDefine", "BLOW", i, EMBLOW_SIZE, COMMENT::BLOW[i]);
		cFILE.getflag( "CharDefine", "BLOW_VAR1", i, EMBLOW_SIZE, COMMENT::BLOW_VAR1[i]);
		cFILE.getflag( "CharDefine", "BLOW_VAR1_SCALE", i, EMBLOW_SIZE, COMMENT::BLOW_VAR1_SCALE[i]);
		cFILE.getflag( "CharDefine", "BLOW_VAR2", i, EMBLOW_SIZE, COMMENT::BLOW_VAR2[i]);
		cFILE.getflag( "CharDefine", "BLOW_VAR2_SCALE", i, EMBLOW_SIZE, COMMENT::BLOW_VAR2_SCALE[i]);
	}

	for ( i=0; i<EIMPACTA_SIZE; ++i )
	{
		cFILE.getflag( "CharDefine", "IMPACT_ADDON", i, EIMPACTA_SIZE, COMMENT::IMPACT_ADDON[i]);
		cFILE.getflag( "CharDefine", "IMPACT_ADDON_SCALE", i, EIMPACTA_SIZE, COMMENT::IMPACT_ADDON_SCALE[i]);
	}

	for ( i=0; i<EMIMPACT_TARGET_NSIZE; ++i )
		cFILE.getflag( "CharDefine", "IMPACT_TAR", i, EMIMPACT_TARGET_NSIZE, COMMENT::IMPACT_TAR[i]);

	for ( i=0; i<EMIMPACT_REALM_NSIZE; ++i )
		cFILE.getflag( "CharDefine", "IMPACT_REALM", i, EMIMPACT_REALM_NSIZE, COMMENT::IMPACT_REALM[i]);

	for ( i=0; i<EMIMPACT_SIDE_NSIZE; ++i )
		cFILE.getflag( "CharDefine", "IMPACT_SIDE", i, EMIMPACT_SIDE_NSIZE, COMMENT::IMPACT_SIDE[i]);

	for ( i=0; i<EMSPECA_NSIZE; ++i )
	{
		cFILE.getflag( "CharDefine", "SPEC_ADDON", i, EMSPECA_NSIZE, COMMENT::SPEC_ADDON[i]);
		cFILE.getflag( "CharDefine", "SPEC_ADDON_VAR1", i, EMSPECA_NSIZE, COMMENT::SPEC_ADDON_VAR1[i]);
		cFILE.getflag( "CharDefine", "SPEC_ADDON_VAR1_SCALE", i, EMSPECA_NSIZE, COMMENT::SPEC_ADDON_VAR1_SCALE[i]);
		cFILE.getflag( "CharDefine", "SPEC_ADDON_VAR2", i, EMSPECA_NSIZE, COMMENT::SPEC_ADDON_VAR2[i]);
		cFILE.getflag( "CharDefine", "SPEC_ADDON_VAR2_SCALE", i, EMSPECA_NSIZE, COMMENT::SPEC_ADDON_VAR2_SCALE[i]);
	}

	for ( i = 0; i<EMSPEC_STIGMA_TYPE_NSIZE; ++i )
		cFILE.getflag( "CharDefine", "SPEC_STIGMA_TYPE", i, EMSPEC_STIGMA_TYPE_NSIZE, COMMENT::SPEC_STIGMA_TYPE[i]);

	for ( i = 0; i<EMSPEC_HALLUCINATE_TYPE_NSIZE; ++i )
		cFILE.getflag( "CharDefine", "SPEC_HALLUCINATE_TYPE", i, EMSPEC_HALLUCINATE_TYPE_NSIZE, COMMENT::SPEC_HALLUCINATE_TYPE[i]);

	for ( i = 0; i<EMSPEC_DOMINATE_TYPE_NSIZE; ++i )
		cFILE.getflag( "CharDefine", "SPEC_DOMINATE_TYPE", i, EMSPEC_DOMINATE_TYPE_NSIZE, COMMENT::SPEC_DOMINATE_TYPE[i]);

	for ( i = 0; i<EMSPEC_SKILL_ASING_NSIZE; ++i )
		cFILE.getflag( "CharDefine", "SPEC_ASIGN_TYPE", i, EMSPEC_SKILL_ASING_NSIZE, COMMENT::SPEC_ASIGN_TYPE[i]);

	for ( i = 0; i<EMSPEC_INC_TYPE_NSIZE; ++i )
		cFILE.getflag( "CharDefine", "SPEC_INC_TYPE", i, EMSPEC_INC_TYPE_NSIZE, COMMENT::SPEC_INC_TYPE[i]);
	
	cFILE.getflag( "CharDefine", "SPEC_INVISIBLE_USE_TYPE", 0, 1, COMMENT::SPEC_INVISIBLE_USE_TYPE);

	for ( i = 0; i<EMSPEC_INVISIBLE_ANI_TYPE_NSIZE; ++i )
		cFILE.getflag( "CharDefine", "SPEC_INVISIBLE_ANI_TYPE", i, EMSPEC_INVISIBLE_ANI_TYPE_NSIZE, COMMENT::SPEC_INVISIBLE_ANI_TYPE[i]);

	for ( i = 0; i<EMSPEC_SWAPPOS_DIR_NSIZE; ++i )
		cFILE.getflag( "CharDefine", "SPEC_SWAPPOS_DIR_TYPE", i, EMSPEC_SWAPPOS_DIR_NSIZE, COMMENT::SPEC_SWAPPOS_DIR_TYPE[i]);

	for ( i=0; i<EMSKILLCLASS_NSIZE; ++i )
		cFILE.getflag( "CharDefine", "SKILLCLASS", i, EMSKILLCLASS_NSIZE, COMMENT::SKILLCLASS[i]);

	for ( i=0; i<EMLANDEFFECT_SIZE; ++i )
		cFILE.getflag( "CharDefine", "LANDEFFECT_TYPE", i, EMLANDEFFECT_SIZE, COMMENT::LANDEFFECT_TYPE[i]);

	//==========================================================================

	for ( i=0; i<CROW_NUM; ++i )
		cFILE.getflag( "Define", "CROW", i, CROW_NUM, COMMENT::CROW[i]);

	for ( i=0; i<EMATT_SIZE; ++i )
		cFILE.getflag( "Define", "ATTACK_RGTYPE", i, EMATT_SIZE, COMMENT::ATTACK_RGTYPE[i]);

	//==========================================================================

	for ( i=0; i<2; ++i )
		cFILE.getflag( "FontMan", "FONTSYS", i, 2, COMMENT::FONTSYS[i]);

	//==========================================================================

	for ( i=0; i<AN_TYPE_SIZE; ++i )
		cFILE.getflag( "SkinAniMan", "ANI_MAINTYPE", i, AN_TYPE_SIZE, COMMENT::ANI_MAINTYPE[i]);

	for ( i=0; i<AN_SUB_00_SIZE; ++i )
	{
		cFILE.getflag( "SkinAniMan", "ANI_SUBTYPE", i, AN_SUB_00_SIZE, COMMENT::ANI_SUBTYPE[i]);
		cFILE.getflag( "SkinAniMan", "ANI_SUBTYPE_00", i, AN_SUB_00_SIZE, COMMENT::ANI_SUBTYPE_00[i]);
		cFILE.getflag( "SkinAniMan", "ANI_SUBTYPE_01", i, AN_SUB_00_SIZE, COMMENT::ANI_SUBTYPE_01[i]);
		cFILE.getflag( "SkinAniMan", "ANI_SUBTYPE_03", i, AN_SUB_00_SIZE, COMMENT::ANI_SUBTYPE_03[i]);
	}

	for ( i=0; i<AN_VEHICLE_SIZE; ++i )
		cFILE.getflag( "SkinAniMan", "ANI_VEHICLE_MAINTYPE", i, AN_VEHICLE_SIZE, COMMENT::ANI_VEHICLE_MAINTYPE[i]);

	for ( i=0; i<EMSF_SIZE; ++i )
		cFILE.getflag( "SkinAniMan", "STRIKE_EFF", i, EMSF_SIZE, COMMENT::STRIKE_EFF[i]);

	//==========================================================================

	for ( i=0; i<SNpcTalk::EM_TALK_SIZE; ++i )
		cFILE.getflag( "NpcTalk", "NPC_ACTIONTYPE", i, SNpcTalk::EM_TALK_SIZE, SNpcTalk::strACTIONTYPE[i]);

	for ( i=0; i<SNpcTalk::EM_BASIC_SIZE; ++i )
		cFILE.getflag( "NpcTalk", "NPC_BASICDESC", i, SNpcTalk::EM_BASIC_SIZE, SNpcTalk::szBASICDESC[i]);

	for ( i=0; i < EMQP_SIZE; ++i )
		cFILE.getflag( "QuestDef", "QUEST_PROGRESS", i, EMQP_SIZE, COMMENT::QUEST_PROGRESS[i]);

	CString strFLAG;
	// 현재 comment.ini에 키값이 3개뿐이어서 MAX_SCHOOL일때 에러가 난다.
	//for ( i=0; i<MAX_SCHOOL; ++i )
	for ( i=0; i<3; ++i )
	{
		strFLAG.Format ( "strSCHOOLNAME%02d", i );
		cFILE.getflag( "School", strFLAG.GetString(), 0, 1, GLCONST_CHAR::strSCHOOLNAME[i]);
	}

	for ( i=0; i<VEHICLE_TYPE_SIZE; ++i )
		cFILE.getflag( "VehicleDefine", "VEHICLE_TYPE", i, VEHICLE_TYPE_SIZE, COMMENT::VEHICLE_TYPE[i] );

	//==========================================================================

	for ( i=0; i<EMPOSTBOX_DESIGN_TYPE_SIZE; ++i )
		cFILE.getflag( "PostBox", "POSTBOX_DESIGN_TYPE", i, EMPOSTBOX_DESIGN_TYPE_SIZE, COMMENT::POSTBOX_DESIGN_TYPE[i] );

	//==========================================================================

    cFILE.getflag( "QuestArea", "QUEST_AREA_NUM", 0, 1, COMMENT::QUEST_AREA_NUM );
    const int nQuestAreaNum = COMMENT::QUEST_AREA_NUM;
    COMMENT::QUEST_AREA_NAME.clear();
    COMMENT::QUEST_AREA_NAME.reserve( nQuestAreaNum );
    for ( i = 0; i < nQuestAreaNum; i++ )
    {
        std::string strTemp;
        cFILE.getflag( "QuestArea", "QUEST_AREA_NAME", i, nQuestAreaNum, strTemp );
        COMMENT::QUEST_AREA_NAME.push_back( strTemp );
    }

	//==========================================================================

	for ( i=0; i<ACTIVITY_CLASS_SIZE; ++i )
		cFILE.getflag( "Activity", "ACTIVITY_CLASS", i, ACTIVITY_CLASS_SIZE, COMMENT::ACTIVITY_CLASS[i]);

	for ( i=0; i<ACTIVITY_CATEGORY_SIZE; ++i )
		cFILE.getflag( "Activity", "ACTIVITY_CATEGORY", i, ACTIVITY_CATEGORY_SIZE, COMMENT::ACTIVITY_CATEGORY[i]);

	for ( i=0; i<ACTIVITY_NOTIFY_SIZE; ++i )
		cFILE.getflag( "Activity", "ACTIVITY_NOTIFY", i, ACTIVITY_NOTIFY_SIZE, COMMENT::ACTIVITY_NOTIFY[i]);

	for ( i=0; i<ACTIVITY_MID_SIZE; ++i )
		cFILE.getflag( "Activity", "ACTIVITY_MID", i, ACTIVITY_MID_SIZE, COMMENT::ACTIVITY_MID[i]);

	//==========================================================================

    //  [2/14/2013 hsshin]
    for ( i = 0; i < Product::ETypeTotal; i++ )
    {
        cFILE.getflag( "Product", "PRODUCT_TYPE_NAME", i, Product::ETypeTotal, COMMENT::PRODUCT_TYPE_NAME[i] );
    }

    //=========================================================================

	return true;
}

CDepthWithTextToFile* g_pFileHelp = NULL;

void SaveHelp(
	const std::string& strTitle,
	const std::string* pList,
	int nCount )
{
	g_pFileHelp->Write( _T( "%s =" ), strTitle.c_str() );
	g_pFileHelp->Begin( _T( "{" ) );
	{
		for( int i=0; i<nCount; ++i )
			g_pFileHelp->Write( _T( "L\"%s\"," ), pList[ i ].c_str() );
	}
	g_pFileHelp->End( _T( "}\n" ) );
}

bool CGLCommentFile::SAVEFILE_LUA( const std::string& strPath )
{
	if( strPath.empty() )
		return false;

	CDepthWithTextToFile cWriter;

	if( !cWriter.Open( strPath, _T( "wt, ccs=UTF-16LE" ), _T( "  " ), false ) )
		return false;

	g_pFileHelp = &cWriter;

	cWriter.Write( _T( "-------------------------------------------------------------------------------" ) );
	cWriter.Write( _T( "-- 1) 대/소 문자를 확실하게 구분해야 합니다." ) );
	cWriter.Write( _T( "-- 2) 1, 0, true, false, null 을 확실하게 구분해야 합니다." ) );
	cWriter.Write( _T( "-- 3) UCS-2 Little Endian 로 저장해야 합니다." ) );
	cWriter.Write( _T( "-- 4) 문자열을 작성할때는 L\"문자열\" 로 작성하십시오. (unicode)" ) );
	cWriter.Write( _T( "-------------------------------------------------------------------------------\n" ) );

	//==========================================================================

	SaveHelp( _T( "SKILL_ROLE" ), COMMENT::SKILL_ROLE, SKILL::EMROLE_NSIZE );
	SaveHelp( _T( "SKILL_APPLY" ), COMMENT::SKILL_APPLY, SKILL::EMAPPLY_NSIZE );
	SaveHelp( _T( "SKILL_ACTION_TYPE" ), COMMENT::SKILL_ACTION_TYPE, SKILL::EMACTION_TYPE_NSIZE );
	SaveHelp( _T( "SKILL_EFFECT_TYPE" ), COMMENT::SKILL_EFFECT_TYPE, SKILL::EMEFFECT_TYPE_NSIZE );

	cWriter.Write( _T( "SKILL_TYPES =" ) );
	cWriter.Begin( _T( "{" ) );
	{
		for( int i=0; i<SKILL::FOR_TYPE_SIZE; ++i )
		{
			cWriter.Write(
				_T( "L\"%s\", %.2f," ),
				COMMENT::SKILL_TYPES[ i ].c_str(),
				COMMENT::SKILL_TYPES_SCALE[ i ] );
		}
	}
	cWriter.End( _T( "}\n" ) );

	SaveHelp( _T( "SKILL_LEVEL" ), COMMENT::SKILL_LEVEL, SKILL::MAX_LEVEL );
	SaveHelp( _T( "SKILL_EFFTIME" ), COMMENT::SKILL_EFFTIME, SKILL::EMTIME_NSIZE );
	SaveHelp( _T( "SKILL_EFFPOS" ), COMMENT::SKILL_EFFPOS, SKILL::EMPOS_NSIZE );
	SaveHelp( _T( "SPECIAL_SKILL_TYPE" ), COMMENT::SPECIAL_SKILL_TYPE, SKILL::EMSSTYPE_NSIZE );
	SaveHelp( _T( "SKILLATTACK" ), COMMENT::SKILLATTACK, SKILL::SKILLATT_NSIZE + 1 );

	//==========================================================================

	cWriter.Write( _T( "CDT_SIGN =" ) );
	cWriter.Begin( _T( "{" ) );
	{
		for( int i=0; i<EMSIGN_SIZE; ++i )
		{
			cWriter.Write(
				_T( "L\"%s\", L\"%s\"," ),
				COMMENT::CDT_SIGN[ i ].c_str(),
				COMMENT::CDT_SIGN_ID[ i ].c_str() );
		}
	}
	cWriter.End( _T( "}\n" ) );

	//==========================================================================

	SaveHelp( _T( "ADDON_NO" ), COMMENT::ADDON_NO, ITEM::SSUIT::ADDON_SIZE );

	//==========================================================================

	SaveHelp( _T( "CROWACT_UP" ), COMMENT::szCROWACT_UP, EMCROWACT_UP_NSIZE );
	SaveHelp( _T( "CROWACT_DN" ), COMMENT::szCROWACT_DN, EMCROWACT_DN_NSIZE );
	SaveHelp( _T( "NPC_TYPE" ), COMMENT::NPC_TYPE, NPC_TYPE_SIZE );

	//==========================================================================

	SaveHelp( _T( "EMREACTION" ), COMMENT::szEMREACTION, REACT_SIZE + 1 );

	//==========================================================================

	SaveHelp( _T( "MOBACTIONS" ), COMMENT::MOBACTIONS, EMACTION_SIZE );

	//==========================================================================

	cWriter.Write( _T( "ITEMLEVEL =" ) );
	cWriter.Begin( _T( "{" ) );
	{
		for( int i=0; i<EMITEM_LEVEL_NSIZE; ++i )
		{
			D3DXCOLOR sColor = COMMENT::ITEMCOLOR[ i ];

			cWriter.Write(
				_T( "L\"%s\", %d, %d, %d," ),
				COMMENT::ITEMLEVEL[ i ].c_str(),
				static_cast< WORD >( sColor.r * 255.0f ),
				static_cast< WORD >( sColor.g * 255.0f ),
				static_cast< WORD >( sColor.b * 255.0f ) );
		}
	}
	cWriter.End( _T( "}\n" ) );

	SaveHelp( _T( "ITEMTYPE" ), COMMENT::ITEMTYPE, ITEM_NSIZE );
	SaveHelp( _T( "ITEMSUIT" ), COMMENT::ITEMSUIT, SUIT_NSIZE );
	//SaveHelp( _T( "ITEMDRUG" ), COMMENT::ITEMDRUG, ITEM_DRUG_SIZE );

	cWriter.Write( _T( "ITEMDRUG =" ) );
	cWriter.Begin( _T( "{" ) );
	{
		for( int i=0; i<EIMPACTA_SIZE; ++i )
		{
			cWriter.Write(
				_T( "L\"%s\", L\"%s\"," ),
				COMMENT::ITEMDRUG[ i ].c_str(),
				COMMENT::ITEMDRUG_VAR[ i ].c_str() );
		}
	}
	cWriter.End( _T( "}\n" ) );

	SaveHelp( _T( "ITEMATTACK" ), COMMENT::ITEMATTACK, ITEMATT_NSIZE + 1 );
	SaveHelp( _T( "ITEMADDON" ), COMMENT::ITEMADDON, EMADD_SIZE );

	cWriter.Write( _T( "ITEMVAR =" ) );
	cWriter.Begin( _T( "{" ) );
	{
		for( int i=0; i<EMVAR_SIZE; ++i )
		{
			cWriter.Write(
				_T( "L\"%s\", %.2f," ),
				COMMENT::ITEMVAR[ i ].c_str(),
				COMMENT::ITEMVAR_SCALE[ i ] );
		}
	}
	cWriter.End( _T( "}\n" ) );

	SaveHelp( _T( "ITEMVOL" ), COMMENT::ITEMVOL, EMVAR_SIZE );

	cWriter.Write( _T( "ITEM_QUE_TYPE =" ) );
	cWriter.Begin( _T( "{" ) );
	{
		for( int i=0; i<QUESTION_SIZE; ++i )
		{
			cWriter.Write(
				_T( "L\"%s\", L\"%s\", L\"%s\"," ),
				COMMENT::ITEM_QUE_TYPE[ i ].c_str(),
				COMMENT::ITEM_QUE_VAR1[ i ].c_str(),
				COMMENT::ITEM_QUE_VAR2[ i ].c_str() );
		}
	}
	cWriter.End( _T( "}\n" ) );

	SaveHelp( _T( "GRINDING_TYPE" ), COMMENT::GRINDING_TYPE, EMGRINDING_NSIZE );
	SaveHelp( _T( "GRINDING_RESIST" ), COMMENT::GRINDING_RESIST, EMGRINDING_RS_NSIZE );

	cWriter.Write( _T( "ITEM_RANDOM_OPT =" ) );
	cWriter.Begin( _T( "{" ) );
	{
		for( int i=0; i<RandomOption::RANDOM_OPTION_NSIZE; ++i )
		{
			cWriter.Write(
				_T( "L\"%s\", %.2f," ),
				COMMENT::ITEM_RANDOM_OPT[ i ].c_str(),
				COMMENT::ITEM_RANDOM_OPT_SCALE[ i ] );
		}
	}
	cWriter.End( _T( "}\n" ) );

	SaveHelp( _T( "GRINDING_LEVEL" ), COMMENT::GRINDING_LEVEL, EMGRINDER_SIZE );
	SaveHelp( _T( "COOLTYPE" ), COMMENT::COOLTYPE, EMCOOL_SIZE );
	SaveHelp( _T( "BIKETYPE" ), COMMENT::BIKETYPE, EMBIKE_NSIZE );
	SaveHelp( _T( "BOARDTYPE" ), COMMENT::BOARDTYPE, EMBIKE_NSIZE );
	SaveHelp( _T( "CARTYPE" ), COMMENT::CARTYPE, EMBIKE_NSIZE );
	SaveHelp( _T( "REARCARTYPE" ), COMMENT::REARCARTYPE, EMBIKE_NSIZE );

	//==========================================================================

	SaveHelp( _T( "TRIBE" ), COMMENT::TRIBE, TRIBE_NSIZE );
	SaveHelp( _T( "MOVETYPE" ), COMMENT::MOVETYPE, MOVETYPE_NSIZE );
	SaveHelp( _T( "ELEMENT" ), COMMENT::ELEMENT, EMELEMENT_MAXNUM2 );
	SaveHelp( _T( "BRIGHT" ), COMMENT::BRIGHT, BRIGHT_SIZE );

	cWriter.Write( _T( "CHARCLASS =" ) );
	cWriter.Begin( _T( "{" ) );
	{
		for( int i=0; i<GLCI_NUM_ACTOR; ++i )
		{
			cWriter.Write(
				_T( "L\"%s\", L\"%s\"," ),
				COMMENT::CHARCLASS[ i ].c_str(),
				COMMENT::CHARCOMMENT[ i ].c_str() );
		}
	}
	cWriter.End( _T( "}\n" ) );
	
	cWriter.Write( _T( "BLOW =" ) );
	cWriter.Begin( _T( "{" ) );
	{
		for( int i=0; i<EMBLOW_SIZE; ++i )
		{
			cWriter.Write(
				_T( "L\"%s\", L\"%s\", %.2f, L\"%s\", %.2f," ),
				COMMENT::BLOW[ i ].c_str(),
				COMMENT::BLOW_VAR1[ i ].c_str(),
				COMMENT::BLOW_VAR1_SCALE[ i ],
				COMMENT::BLOW_VAR2[ i ].c_str(),
				COMMENT::BLOW_VAR2_SCALE[ i ] );
		}
	}
	cWriter.End( _T( "}\n" ) );

	cWriter.Write( _T( "IMPACT_ADDON =" ) );
	cWriter.Begin( _T( "{" ) );
	{
		for( int i=0; i<EIMPACTA_SIZE; ++i )
		{
			cWriter.Write(
				_T( "L\"%s\", %.2f," ),
				COMMENT::IMPACT_ADDON[ i ].c_str(),
				COMMENT::IMPACT_ADDON_SCALE[ i ] );
		}
	}
	cWriter.End( _T( "}\n" ) );

	SaveHelp( _T( "IMPACT_TAR" ), COMMENT::IMPACT_TAR, EMIMPACT_TARGET_NSIZE );
	SaveHelp( _T( "IMPACT_REALM" ), COMMENT::IMPACT_REALM, EMIMPACT_REALM_NSIZE );
	SaveHelp( _T( "IMPACT_SIDE" ), COMMENT::IMPACT_SIDE, EMIMPACT_SIDE_NSIZE );

	cWriter.Write( _T( "SPEC_ADDON =" ) );
	cWriter.Begin( _T( "{" ) );
	{
		for( int i=0; i<EMSPECA_NSIZE; ++i )
		{
			cWriter.Write(
				_T( "L\"%s\", L\"%s\", %.2f, L\"%s\", %.2f," ),
				COMMENT::SPEC_ADDON[ i ].c_str(),
				COMMENT::SPEC_ADDON_VAR1[ i ].c_str(),
				COMMENT::SPEC_ADDON_VAR1_SCALE[ i ],
				COMMENT::SPEC_ADDON_VAR2[ i ].c_str(),
				COMMENT::SPEC_ADDON_VAR2_SCALE[ i ] );
		}
	}
	cWriter.End( _T( "}\n" ) );

	SaveHelp( _T( "SPEC_STIGMA_TYPE" ), COMMENT::SPEC_STIGMA_TYPE, EMSPEC_STIGMA_TYPE_NSIZE);
	SaveHelp( _T( "SPEC_HALLUCINATE_TYPE" ), COMMENT::SPEC_HALLUCINATE_TYPE, EMSPEC_HALLUCINATE_TYPE_NSIZE);
	SaveHelp( _T( "SPEC_DOMINATE_TYPE" ), COMMENT::SPEC_DOMINATE_TYPE, EMSPEC_DOMINATE_TYPE_NSIZE);
	SaveHelp( _T( "SPEC_ASIGN_TYPE" ), COMMENT::SPEC_ASIGN_TYPE, EMSPEC_SKILL_ASING_NSIZE);
	SaveHelp( _T( "SPEC_INC_TYPE" ), COMMENT::SPEC_INC_TYPE, EMSPEC_INC_TYPE_NSIZE);
	SaveHelp( _T( "SPEC_INVISIBLE_USE_TYPE" ), &COMMENT::SPEC_INVISIBLE_USE_TYPE, 1);
	SaveHelp( _T( "SPEC_INVISIBLE_ANI_TYPE" ), COMMENT::SPEC_INVISIBLE_ANI_TYPE, EMSPEC_INVISIBLE_ANI_TYPE_NSIZE);
	SaveHelp( _T( "SPEC_SWAPPOS_DIR_TYPE" ), COMMENT::SPEC_SWAPPOS_DIR_TYPE, EMSPEC_SWAPPOS_DIR_NSIZE);

	SaveHelp( _T( "SKILLCLASS" ), COMMENT::SKILLCLASS, EMSKILLCLASS_NSIZE );
	SaveHelp( _T( "LANDEFFECT_TYPE" ), COMMENT::LANDEFFECT_TYPE, EMLANDEFFECT_SIZE );

	//==========================================================================

	SaveHelp( _T( "CROW" ), COMMENT::CROW, CROW_NUM );
	SaveHelp( _T( "ATTACK_RGTYPE" ), COMMENT::ATTACK_RGTYPE, EMATT_SIZE );

	//==========================================================================

	SaveHelp( _T( "FONTSYS" ), COMMENT::FONTSYS, 2 );

	//==========================================================================

	SaveHelp( _T( "ANI_MAINTYPE" ), COMMENT::ANI_MAINTYPE, AN_TYPE_SIZE );

	cWriter.Write( _T( "ANI_SUBTYPE =" ) );
	cWriter.Begin( _T( "{" ) );
	{
		for( int i=0; i<AN_SUB_00_SIZE; ++i )
		{
			cWriter.Write(
				_T( "L\"%s\", L\"%s\", L\"%s\", L\"%s\"," ),
				COMMENT::ANI_SUBTYPE[ i ].c_str(),
				COMMENT::ANI_SUBTYPE_00[ i ].c_str(),
				COMMENT::ANI_SUBTYPE_01[ i ].c_str(),
				COMMENT::ANI_SUBTYPE_03[ i ].c_str() );
		}
	}
	cWriter.End( _T( "}\n" ) );

	SaveHelp( _T( "ANI_VEHICLE_MAINTYPE" ), COMMENT::ANI_VEHICLE_MAINTYPE, AN_VEHICLE_SIZE );
	SaveHelp( _T( "STRIKE_EFF" ), COMMENT::STRIKE_EFF, EMSF_SIZE );

	//==========================================================================

	SaveHelp( _T( "NPC_ACTIONTYPE" ), SNpcTalk::strACTIONTYPE, SNpcTalk::EM_TALK_SIZE );
	SaveHelp( _T( "NPC_BASICDESC" ), SNpcTalk::szBASICDESC, SNpcTalk::EM_BASIC_SIZE );
	SaveHelp( _T( "QUEST_PROGRESS" ), COMMENT::QUEST_PROGRESS, EMQP_SIZE );
	SaveHelp( _T( "SCHOOLNAME" ), GLCONST_CHAR::strSCHOOLNAME, MAX_SCHOOL );
	SaveHelp( _T( "VEHICLE_TYPE" ), COMMENT::VEHICLE_TYPE, VEHICLE_TYPE_SIZE );

	//==========================================================================

	SaveHelp( _T( "POSTBOX_DESIGN_TYPE" ), COMMENT::POSTBOX_DESIGN_TYPE, EMPOSTBOX_DESIGN_TYPE_SIZE );

	//==========================================================================

	cWriter.Write( _T( "QUEST_AREA =" ) );
	cWriter.Begin( _T( "{" ) );
	{
		for( size_t i=0; i<COMMENT::QUEST_AREA_NAME.size(); ++i )
			cWriter.Write( _T( "L\"%s\"," ), COMMENT::QUEST_AREA_NAME[ i ].c_str() );
	}
	cWriter.End( _T( "}\n" ) );

	//==========================================================================

	SaveHelp( _T( "ACTIVITY_CLASS" ), COMMENT::ACTIVITY_CLASS, ACTIVITY_CLASS_SIZE );
	SaveHelp( _T( "ACTIVITY_CATEGORY" ), COMMENT::ACTIVITY_CATEGORY, ACTIVITY_CATEGORY_SIZE );
	SaveHelp( _T( "ACTIVITY_NOTIFY" ), COMMENT::ACTIVITY_NOTIFY, ACTIVITY_NOTIFY_SIZE );
	SaveHelp( _T( "ACTIVITY_MID" ), COMMENT::ACTIVITY_MID, ACTIVITY_MID_SIZE );

	//==========================================================================

    //  [2/14/2013 hsshin]    
    SaveHelp( _T( "PRODUCT_TYPE_NAME" ), COMMENT::PRODUCT_TYPE_NAME, Product::ETypeTotal );

    //==========================================================================

	cWriter.Close();

	return true;
}

void LoadHelp(
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
			pList[ i ] = sc::string::unicodeToAnsi( ws );
		}
	}
	catch( const LuaPlus::LuaException& e )
    {        
        MessageBox( NULL, e.GetErrorMessage(), strTitle.c_str(), MB_OK );
    }
}

bool ParseCommentLua()
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
			COMMENT::SKILL_TYPES[ i ] = sc::string::unicodeToAnsi( ws );

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
			COMMENT::CDT_SIGN[ i ] = sc::string::unicodeToAnsi( ws );

			it.Next();
			ws = reinterpret_cast< const wchar_t* >( it.GetValue().GetWString() );
			COMMENT::CDT_SIGN_ID[ i ] = sc::string::unicodeToAnsi( ws );
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
			if( i >= EMITEM_LEVEL_NSIZE )
				break;

			ws = reinterpret_cast< const wchar_t* >( it.GetValue().GetWString() );
			COMMENT::ITEMLEVEL[ i ] = sc::string::unicodeToAnsi( ws );

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
			COMMENT::ITEMDRUG[ i ] = sc::string::unicodeToAnsi( ws );

			it.Next();
			ws = reinterpret_cast< const wchar_t* >( it.GetValue().GetWString() );
			COMMENT::ITEMDRUG_VAR[ i ] = sc::string::unicodeToAnsi( ws );
		}
	}
	catch( const LuaPlus::LuaException& e )
	{        
		MessageBox( NULL, e.GetErrorMessage(), _T( "SKILL_TYPES" ), MB_OK );
	}

	LoadHelp( _T( "ITEMATTACK" ), COMMENT::ITEMATTACK, ITEMATT_NSIZE + 1 );
	//LoadHelp( _T( "ITEMADDON" ), COMMENT::ITEMADDON, EMADD_SIZE );

	INT nAddonIdx = 0;
	try
	{
		LuaPlus::LuaObject ob = sc::lua_getGlobalFromName( _T( "ITEMADDON" ) );
		for( LuaPlus::LuaTableIterator it( ob ); it.IsValid(); it.Next(), ++nAddonIdx )
		{
			if( nAddonIdx >= EMADD_SIZE )
				break;

			ws = reinterpret_cast< const wchar_t* >( it.GetValue().GetWString() );
			COMMENT::ITEMADDON[ nAddonIdx ] = sc::string::unicodeToAnsi( ws );

			it.Next();
			COMMENT::ITEMADDON_VAR_SCALE[ nAddonIdx ] = it.GetValue().GetFloat();
		}
	}
	catch( const LuaPlus::LuaException& e )
	{        
		MessageBox( NULL, e.GetErrorMessage(), _T( "ITEMADDON" ), MB_OK );
	}

	try
	{
		int i = 0;
		LuaPlus::LuaObject ob = sc::lua_getGlobalFromName( _T( "ITEMVAR" ) );
		for( LuaPlus::LuaTableIterator it( ob ); it.IsValid(); it.Next(), ++i )
		{
			if( i >= EMVAR_SIZE )
				break;

			ws = reinterpret_cast< const wchar_t* >( it.GetValue().GetWString() );
			COMMENT::ITEMVAR[ i ] = sc::string::unicodeToAnsi( ws );

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
			COMMENT::ITEM_QUE_TYPE[ i ] = sc::string::unicodeToAnsi( ws );

			it.Next();
			ws = reinterpret_cast< const wchar_t* >( it.GetValue().GetWString() );
			COMMENT::ITEM_QUE_VAR1[ i ] = sc::string::unicodeToAnsi( ws );

			it.Next();
			ws = reinterpret_cast< const wchar_t* >( it.GetValue().GetWString() );
			COMMENT::ITEM_QUE_VAR2[ i ] = sc::string::unicodeToAnsi( ws );
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
			if( i >= RandomOption::RANDOM_OPTION_NSIZE )
				break;

			ws = reinterpret_cast< const wchar_t* >( it.GetValue().GetWString() );
			COMMENT::ITEM_RANDOM_OPT[ i ] = sc::string::unicodeToAnsi( ws );

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
		const bool isDataIntegration(GLUseFeatures::GetInstance().IsUsingDataIntegrate() == true);
		int i = 0;
		LuaPlus::LuaObject ob = sc::lua_getGlobalFromName( _T( "CHARCLASS" ) );
		for( LuaPlus::LuaTableIterator it( ob ); it.IsValid(); it.Next(), ++i )
		{
			if( i >= GLCI_NUM_ACTOR )
				break;

			if ( isDataIntegration )
			{
				std::string strCharClass(CGameTextMan::GetInstance().GetText("CHARCLASS", i, CGameTextMan::EM_COMMENT_TEXT))
					, strCharComment(CGameTextMan::GetInstance().GetText("CHARCOMMENT", i, CGameTextMan::EM_COMMENT_TEXT));
				COMMENT::CHARCLASS[ i ] = strCharClass;
				COMMENT::CHARCOMMENT[ i ] = strCharComment;
			}
			else
			{
				ws = reinterpret_cast< const wchar_t* >( it.GetValue().GetWString() );
				COMMENT::CHARCLASS[ i ] = sc::string::unicodeToAnsi( ws );
			}

			it.Next();
			if ( !isDataIntegration )
			{
				ws = reinterpret_cast< const wchar_t* >( it.GetValue().GetWString() );
				COMMENT::CHARCOMMENT[ i ] = sc::string::unicodeToAnsi( ws );
			}
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
			COMMENT::BLOW[ i ] = sc::string::unicodeToAnsi( ws );

			it.Next();
			ws = reinterpret_cast< const wchar_t* >( it.GetValue().GetWString() );
			COMMENT::BLOW_VAR1[ i ] = sc::string::unicodeToAnsi( ws );

			it.Next();
			COMMENT::BLOW_VAR1_SCALE[ i ] = it.GetValue().GetFloat();

			it.Next();
			ws = reinterpret_cast< const wchar_t* >( it.GetValue().GetWString() );
			COMMENT::BLOW_VAR2[ i ] = sc::string::unicodeToAnsi( ws );

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
			COMMENT::IMPACT_ADDON[ i ] = sc::string::unicodeToAnsi( ws );

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
			COMMENT::SPEC_ADDON[ i ] = sc::string::unicodeToAnsi( ws );

			it.Next();
			ws = reinterpret_cast< const wchar_t* >( it.GetValue().GetWString() );
			COMMENT::SPEC_ADDON_VAR1[ i ] = sc::string::unicodeToAnsi( ws );

			it.Next();
			COMMENT::SPEC_ADDON_VAR1_SCALE[ i ] = it.GetValue().GetFloat();

			it.Next();
			ws = reinterpret_cast< const wchar_t* >( it.GetValue().GetWString() );
			COMMENT::SPEC_ADDON_VAR2[ i ] = sc::string::unicodeToAnsi( ws );

			it.Next();
			COMMENT::SPEC_ADDON_VAR2_SCALE[ i ] = it.GetValue().GetFloat();
		}
	}
	catch( const LuaPlus::LuaException& e )
    {        
        MessageBox( NULL, e.GetErrorMessage(), _T( "SPEC_ADDON" ), MB_OK );
    }

	try
	{
		int i = 0;
		LuaPlus::LuaObject ob = sc::lua_getGlobalFromName( _T( "SPEC_CA" ) );
		for( LuaPlus::LuaTableIterator it( ob ); it.IsValid(); it.Next(), ++i )
		{
			if( i >= EMSPECA_CAK_MAX )
				break;

			COMMENT::SPEC_CA[i] = it.GetValue().GetInteger();
		}
	}
	catch( const LuaPlus::LuaException& e )
	{        
		MessageBox( NULL, e.GetErrorMessage(), _T( "SPEC_CA" ), MB_OK );
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
			COMMENT::ANI_SUBTYPE[ i ] = sc::string::unicodeToAnsi( ws );

			it.Next();
			ws = reinterpret_cast< const wchar_t* >( it.GetValue().GetWString() );
			COMMENT::ANI_SUBTYPE_00[ i ] = sc::string::unicodeToAnsi( ws );

			it.Next();
			ws = reinterpret_cast< const wchar_t* >( it.GetValue().GetWString() );
			COMMENT::ANI_SUBTYPE_01[ i ] = sc::string::unicodeToAnsi( ws );

			it.Next();
			ws = reinterpret_cast< const wchar_t* >( it.GetValue().GetWString() );
			COMMENT::ANI_SUBTYPE_03[ i ] = sc::string::unicodeToAnsi( ws );
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
			COMMENT::QUEST_AREA_NAME.push_back( sc::string::unicodeToAnsi( ws ) );
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
	LoadHelp( _T( "SKILL_LINK_APPLY_TYPE" ), COMMENT::SKILL_LINK_TYPE, EMITEM_SKILL_SIZE );

    //==========================================================================

	return true;
}

bool CGLCommentFile::LOADFILE_LUA( const std::string& strPath )
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
