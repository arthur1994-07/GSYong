#include "pch.h"
#include "../RanLogicClient/Char/SkinCharMulti.h"
#include "../SigmaCore/Log/LogMan.h"
#include "../SigmaCore/String/StringUtils.h"
#include "../SigmaCore/String/StringFormat.h"
#include "../SigmaCore/Compress/Unzipper.h"
#include "../SigmaCore/File/StringFile.h"
#include "../SigmaCore/Loading/NSLoadingDirect.h"

#include "../EngineLib/G-Logic/GLogic.h"
#include "../EngineLib/G-Logic/GLPeriod.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinDataDummy.h"
#include "../EngineLib/Common/gltexfile.h"
#include "../EngineLib/Common/SUBPATH.h"
#include "../EngineLib/LoadingTimeCheck.h"

#include "./RANPARAM.h"
#include "./Skill/GLRnSkillLearnSetting.h"
#include "./Activity/ActivityBase.h"
#include "./Character/GLCharData.h"
#include "./Club/ClubAuthData.h"
#include "./Crow/GLCrowData.h"
#include "./Crow/GLCrowDataMan.h"
#include "./GLCommentFile.h"
#include "./GLPetCommentFile.h"
#include "./GLStringTable.h"
#include "../enginelib/GUInterface/GameTextControl.h"
#include "./GLColorTable.h"
#include "./GLUseFeatures.h"
#include "./HideSet/GLHideSet.h"
#include "./TransformSet/GLTransformSet.h"
#include "./MiniGame/OddEven/GLOddEvenSettings.h"
#include "./TexasHoldem/GLTexasHoldemSetting.h"
#include "./Product/GLProductRecipeMan.h"
#include "./Item/GLItemMan.h"
#include "./NpcTalk/NpcTalkMan.h"
#include "./Post/GLPostData.h"
#include "./Quest/GLQuestMan.h"
#include "./SNS/GLSNSData.h"
#include "./Transport/GLBusStation.h"
#include "./Transport/GLTaxiStation.h"
#include "./WorldBattle/WorldBattleCountry.h"
#include "./Market/MarketDefine.h"
#include "./Skill/SpecialEffect/GLSpecExperiencePoint.h"
#include "./InstanceSystem/GLInstanceSystem.h"
#include "./GLogicData.h"
#include "./GLogicFile.h"
#include "./Item/ItemSet/GLItemSet.h"
#include "./Item/ItemLevel/GLItemLevel.h"
#include "./Item/ItemBasicStat/GLItemBasicStatMan.h"
#include "./Item/ItemSkill/GLItemLinkSkillMan.h"
#include "./Character/CharacterSlot/GLCharSlotMan.h"
#include "./GenItemTable/GenItemTable.h"
#include "./CountryCode/CountryCode.h"
#include "./Item/GLItemCostumeAttach.h"
#include "./Skill/GLSkillChangeList.h"
#include "./ItemPeriodExtension/ItemPeriodExtension.h"
#include "./RandomAddon/RandomAddon.h"
#include "./RanMobile/GLRanMobile.h"
#include "./Skill/GLSkillReset.h"

#include "./GLogicDataMan.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace GLCONST_CHAR
{
    PCHARDATA2	pCharData2[MAX_SCHOOL][GLCI_NUM_ACTOR] =
    {
        { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
        { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
        { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
        { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
        { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
        { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
        { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
        { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
        { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
        { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    };

    PCHARDATA2	pJumpingCharData2[MAX_SCHOOL][GLCI_NUM_ACTOR] =
    {
        { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
        { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
        { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
        { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
        { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
        { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
        { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
        { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
        { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
        { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    };

    BOOL VAID_CHAR_DATA2 ( WORD wSchool, EMCHARINDEX emIndex, const BOOL bJumping )	
	{ 
		if( bJumping == FALSE )
			return pCharData2[wSchool][emIndex] != NULL; 
		else
			return pJumpingCharData2[wSchool][emIndex] != NULL; 
	}

    SCHARDATA2& GET_CHAR_DATA2 ( WORD wSchool, EMCHARINDEX emIndex, const BOOL bJumping )
    {
        GASSERT(wSchool<MAX_SCHOOL);
        GASSERT(emIndex<GLCI_NUM_ACTOR);
	
		if( bJumping == FALSE )
			return *pCharData2[wSchool][emIndex];
		else
			return *pJumpingCharData2[wSchool][emIndex];
    }
} // namespace GLCONST_CHAR

GLogicData& GLogicData::GetInstance()
{
    static GLogicData Instance;
    return Instance;
}

GLogicData::GLogicData()
    : m_pPath(NULL)
{
    m_spNpcTalkMan = std::tr1::shared_ptr<NpcTalkMan> (new NpcTalkMan);
    m_spRnSkillLearnSetting = std::tr1::shared_ptr<GLRnSkillLearnSetting> (new GLRnSkillLearnSetting);
    m_pLogicFile = new GLogicFile;
    m_pWbCountry = new wb::Country;
    m_pClubBaseData = new club::ClubDefineData;
}

GLogicData::~GLogicData()
{
    delete m_pClubBaseData;
    m_pClubBaseData = NULL;

    delete m_pWbCountry;
    m_pWbCountry = NULL;

    delete m_pLogicFile;
    m_pLogicFile = NULL;
    ClearData();
}

#ifdef LOADING_TIME_CHECK
#include <boost/timer.hpp>
/*
                        2011-08-16
    --------------------------------------------------
    GLCommentFile       0.01  0.01  sec
    GLStringTable       0.323 0.399 sec
    GLUseFeatures       0.002 0.401 sec
    GLItemMan           0.578 0.979 sec
    Activity            1.797 2.776 sec
    NpcTalk             2.041 4.817 sec
    GLCrowDataMan       1.204 6.021 sec
    GLSkillMan          0.164 6.185 sec
    GLQuestMan          0.224 6.409 sec
    GLCONST_CHAR        0.21  6.619 sec
    GLCONST_ATTENDANCE  0     6.674 sec
    GLCONST_POST        0.001 6.675 sec
    GLCONST_SNS         0     6.675 sec
    GLCONST_CLOLORTABLE 0.003 6.678 sec
    GLBusStation        0.01  6.68  sec
    GLTaxiStation       0     6.68  sec
    GLItemMixMan        0.002 6.682 sec
    GLCONST_CHAR class  0.378 7.06  sec
    GLCONST_CHAR server 0     7.06  sec
    GLPET               0.037 7.097 sec
    VEHICLE             0.001 7.133 sec
    SUMMON server       0.005 7.138 sec
*/
#endif

HRESULT GLogicData::LoadData(
    BOOL bServer,
    SUBPATH* pPath,
    bool bToolMode,
    boost::function<void (std::string)> PercentCallbackFunc,
	boost::function<void (unsigned int)> PercentCallbackFunc2)
{

#ifdef LOADING_TIME_CHECK
    boost::timer LoadingTotalTimer;
    LoadingTotalTimer.restart();

    boost::timer LoadingSectionTimer;
    LoadingSectionTimer.restart();
#endif

    m_pPath = pPath;

	// ------------------------------------------------------------------------
	// bjju.UseFeatures load
	if (PercentCallbackFunc)
		PercentCallbackFunc(m_pLogicFile->GetUseFeaturesFileName());
	if (GLUseFeatures::GetInstance().LoadFile(m_pLogicFile->GetUseFeaturesFileName(), pPath, bToolMode) == false)
	{
		std::string ErrMsg(sc::string::format("%1% load failed", m_pLogicFile->GetUseFeaturesFileName()));
		sc::writeLogError(ErrMsg);
		AfxMessageBox(ErrMsg.c_str(), MB_OK|MB_ICONSTOP);
	}

	// 툴인 경우에는 데이터통합 기능 무조건 끈다;
	if ( bToolMode == true )
		GLUseFeatures::GetInstance().SetUseFeature(GLUseFeatures::EMUSE_FEATURE_DATAINTEGRATE, false);

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

    // ------------------------------------------------------------------------
    // comment file load
    if (PercentCallbackFunc)
        PercentCallbackFunc(m_pLogicFile->GetCommentFileName());	

	BOOL bCommentLoad;
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		bCommentLoad = CGameTextMan::GetInstance().LoadText(RANPARAM::strCommentText, CGameTextMan::EM_COMMENT_TEXT, RANPARAM::strCountry, CGameTextMan::GAMETEXTMAN_LOADOP_REFACT);
	}

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

// 	else
// 	{
	CGLCommentFile glCommentFile;
	bCommentLoad = glCommentFile.LOADFILE(m_pLogicFile->GetCommentFileName(), bToolMode );
//	}
    if( !bCommentLoad )
	{
		std::string ErrMsg(sc::string::format("%1% load failed", m_pLogicFile->GetCommentFileName()));
		sc::writeLogError(ErrMsg);
		AfxMessageBox(ErrMsg.c_str(), MB_OK|MB_ICONSTOP);
		return E_FAIL;
	}
 
   // ------------------------------------------------------------------------
    // country.lua country.luc
    if (!m_pLogicFile->IsLoad())
        m_pLogicFile->LoadCountryFile();

    // Loading ----------------------------------------------------------------
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

    // ------------------------------------------------------------------------
    // pet comment file load
    if (PercentCallbackFunc)
        PercentCallbackFunc(m_pLogicFile->GetPetCommentFileName());	

	BOOL bPETCommentLoad;
	//if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() == FALSE)
	{
		CGLPetCommentFile glPetCommentFile;
		bPETCommentLoad = glPetCommentFile.LOADFILE(m_pLogicFile->GetPetCommentFileName(), bToolMode );

		if( !bPETCommentLoad )
		{
			std::string ErrMsg(sc::string::format("%1% load failed", m_pLogicFile->GetPetCommentFileName()));
			sc::writeLogError(ErrMsg);
			AfxMessageBox(ErrMsg.c_str(), MB_OK|MB_ICONSTOP);
			return E_FAIL;
		}
	}    

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//
    
	if ( PercentCallbackFunc2 )
        PercentCallbackFunc2(3);

#ifdef LOADING_TIME_CHECK
    sc::writeLogInfo(sc::string::format("Loading GLogicData GLCommentFile %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
    LoadingSectionTimer.restart();
#endif
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() == FALSE)
		GLStringTable::GetInstance().CLEAR();

    // ------------------------------------------------------------------------
    // 스트링테이블을 로드한다. by 경대
    // Loading ----------------------------------------------------------------
    NSLoadingDirect::FrameMoveRender( FALSE );
	if (PercentCallbackFunc)
		PercentCallbackFunc("String table load ( Skill Manager )");

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		CGameTextMan::GetInstance().LoadText(RANPARAM::strSkillText, CGameTextMan::EM_SKILL_TEXT, RANPARAM::strCountry, CGameTextMan::GAMETEXTMAN_LOADOP_REFACT );
	else
		GLStringTable::GetInstance().LOADFILE(GLSkillMan::GetInstance()._STRINGTABLE, GLStringTable::SKILL);

    // Loading ----------------------------------------------------------------
    NSLoadingDirect::FrameMoveRender( FALSE );
	if (PercentCallbackFunc)
		PercentCallbackFunc("String table load ( CrowData Manager");

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		CGameTextMan::GetInstance().LoadText(RANPARAM::strCrowText, CGameTextMan::EM_CROW_TEXT, RANPARAM::strCountry, CGameTextMan::GAMETEXTMAN_LOADOP_REFACT );
	else
		GLStringTable::GetInstance().LOADFILE(GLCrowDataMan::GetInstance()._STRINGTABLE, GLStringTable::CROW);

#ifdef LOADING_TIME_CHECK
    sc::writeLogInfo(sc::string::format("Loading GLogicData GLStringTable %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
    LoadingSectionTimer.restart();
#endif
	
	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	// ------------------------------------------------------------------------
	// randomOptionServer/Client.rsf loading
	const std::string& _stringNameFileRandomOption(bServer == TRUE ? m_pLogicFile->GetRandomOptionServerFileName() : m_pLogicFile->GetRandomOptionClientFileName());
	if (PercentCallbackFunc)
		PercentCallbackFunc(_stringNameFileRandomOption);

	// if ( RandomOption::LoadFile(bServer == TRUE ? RandomOption::LOAD_SERVER : RandomOption::LOAD_CLIENT, m_pLogicFile, bToolMode) == E_FAIL )
	if ( RandomOption::LoadFile(RandomOption::LOAD_SERVER, m_pLogicFile, bToolMode) == E_FAIL )
	{
		const std::string ErrMsg(sc::string::format("%1% load failed", _stringNameFileRandomOption));
		sc::writeLogError(ErrMsg);
		AfxMessageBox(ErrMsg.c_str(), MB_OK|MB_ICONSTOP);
		return E_FAIL;
	}
	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//
	if (PercentCallbackFunc2)
		PercentCallbackFunc2(78);

#ifdef LOADING_TIME_CHECK
	sc::writeLogInfo(sc::string::format("Loading RandomOption %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
	LoadingSectionTimer.restart();
#endif



    // Loading ----------------------------------------------------------------
    NSLoadingDirect::FrameMoveRender( FALSE );
    if (PercentCallbackFunc2)
        PercentCallbackFunc2(5);

	if(GLUseFeatures::GetInstance().IsUsingSetItemUse())
	{	
		if(!GLItemSet::GetInstance().LoadFile(m_pLogicFile->GetSetListFileName(), bToolMode))
		{
			std::string ErrMsg(sc::string::format("%1% load failed", m_pLogicFile->GetSetListFileName()));
			sc::writeLogError(ErrMsg);
			AfxMessageBox(ErrMsg.c_str(), MB_OK|MB_ICONSTOP);
			return E_FAIL;
		}
	}

	// 아이템 등급별 연마설정 로딩;
	GLItemLevelMan::Instance()->LoadData();

	// 아이템 기본 능력치 랜덤 설정 로딩;
	GLItemBasicStatMan::Instance()->LoadData();

	// 아이템 링크 스킬 랜덤 설정 로딩;
	GLItemLinkSkillMan::Instance()->LoadData();

	// 캐릭터 슬롯 설정 로딩;
	GLCharSlotMan::Instance()->LoadData();

    // Loading ----------------------------------------------------------------
    NSLoadingDirect::FrameMoveRender( FALSE );    
	if (PercentCallbackFunc2)
        PercentCallbackFunc2(7);    

#ifdef LOADING_TIME_CHECK
    sc::writeLogInfo(sc::string::format("Loading GLogicData GLUseFeatures %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
    LoadingSectionTimer.restart();
#endif



	{ // 경험치 테이블 로드;
		// ------------------------------------------------------------------------
		// SkillSpecial_ExperiencePoint.lua loading
		if (PercentCallbackFunc)
			PercentCallbackFunc(m_pLogicFile->GetSkillSpecExperiencePointFileName());

		if ( SKILL::ExperiencePoint::LoadFile(m_pLogicFile->GetSkillSpecExperiencePointFileName(), bServer==TRUE) == E_FAIL )
		{
			std::string ErrMsg(sc::string::format("%1% load failed", m_pLogicFile->GetSkillSpecExperiencePointFileName()));
			sc::writeLogError(ErrMsg);
			AfxMessageBox(ErrMsg.c_str(), MB_OK|MB_ICONSTOP);
			//return E_FAIL;
		}
		//---------------------------------------------------//
		NSLoadingDirect::FrameMoveRender( FALSE );
		//---------------------------------------------------//
		if (PercentCallbackFunc2)
			PercentCallbackFunc2(8);
	}

    // ------------------------------------------------------------------------
    // item file load
	if (PercentCallbackFunc)
		PercentCallbackFunc("String table load ( Item Manager )");

	CGameTextMan::GetInstance().Clear(CGameTextMan::EM_ITEM_TEXT);
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		CGameTextMan::GetInstance().LoadText(RANPARAM::strItemText, CGameTextMan::EM_ITEM_TEXT, RANPARAM::strCountry, CGameTextMan::GAMETEXTMAN_LOADOP_REFACT );
	else
		GLStringTable::GetInstance().LOADFILE(GLItemMan::GetInstance()._STRINGTABLE, GLStringTable::ITEM, PercentCallbackFunc);

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

    if (PercentCallbackFunc)
        PercentCallbackFunc(m_pLogicFile->GetItemFileName());
    if (GLItemMan::GetInstance().LoadFile(m_pLogicFile->GetItemFileName(), PercentCallbackFunc) == E_FAIL)
    {
        std::string ErrMsg(sc::string::format("%1% load failed", m_pLogicFile->GetItemFileName()));
        sc::writeLogError(ErrMsg);
        AfxMessageBox(ErrMsg.c_str(), MB_OK|MB_ICONSTOP);
        return E_FAIL;
    }

    // Loading ----------------------------------------------------------------
    NSLoadingDirect::FrameMoveRender( FALSE );
	if (PercentCallbackFunc2)
        PercentCallbackFunc2(35);

#ifdef LOADING_TIME_CHECK
    sc::writeLogInfo(sc::string::format("Loading GLogicData GLItemMan %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
    LoadingSectionTimer.restart();
#endif

    // ------------------------------------------------------------------------
    // mjeon.Activity load
    if (PercentCallbackFunc)
        PercentCallbackFunc(m_pLogicFile->GetActivityFileName());
    if (ActivityBaseData::GetInstance()->Load(m_pLogicFile->GetActivityFileName()) == FALSE)
    {
        std::string ErrMsg(sc::string::format("%1% load failed", m_pLogicFile->GetActivityFileName()));
        sc::writeLogError(ErrMsg);
        AfxMessageBox(ErrMsg.c_str(), MB_OK|MB_ICONSTOP);
        return E_FAIL;
    }

    // Loading ----------------------------------------------------------------
    NSLoadingDirect::FrameMoveRender( FALSE );
	if (PercentCallbackFunc2)
        PercentCallbackFunc2(38);

#ifdef LOADING_TIME_CHECK
    sc::writeLogInfo(sc::string::format("Loading GLogicData Activity %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
    LoadingSectionTimer.restart();
#endif

    // ------------------------------------------------------------------------
    // gametalk.dat file loading
    if (PercentCallbackFunc)
        PercentCallbackFunc(m_pLogicFile->GetNewNpcTalkFileName());
    m_spNpcTalkMan->Load(std::string(""));

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		CGameTextMan::GetInstance().LoadText( RANPARAM::strNPCTalkText, CGameTextMan::EM_NPCTALK_TEXT, RANPARAM::strCountry, CGameTextMan::GAMETEXTMAN_LOADOP_REFACT );
	}

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//
	if (PercentCallbackFunc2)
        PercentCallbackFunc2(43);

#ifdef LOADING_TIME_CHECK
    sc::writeLogInfo(sc::string::format("Loading GLogicData NpcTalk %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
    LoadingSectionTimer.restart();
#endif

    // ------------------------------------------------------------------------
    // crow file loading
    if (PercentCallbackFunc)
        PercentCallbackFunc(m_pLogicFile->GetCrowFileName());
    if (GLCrowDataMan::GetInstance().LoadFile(m_pLogicFile->GetCrowFileName(), bServer, TRUE, PercentCallbackFunc) == E_FAIL)
    {
        std::string ErrMsg(sc::string::format("%1% load failed", m_pLogicFile->GetCrowFileName()));
        sc::writeLogError(ErrMsg);
        AfxMessageBox(ErrMsg.c_str(), MB_OK|MB_ICONSTOP);
        return E_FAIL;
    }

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//
	if (PercentCallbackFunc2)
        PercentCallbackFunc2(75);


    // Club Define ------------------------------------------------------------
    std::string ClubBaseDefineFile(sc::getAppPath());
    ClubBaseDefineFile.append(m_pPath->GLogicPath());
    ClubBaseDefineFile.append(m_pLogicFile->GetClubDefineFile());
    if (!m_pClubBaseData->Load(ClubBaseDefineFile))
    {
        std::string ErrMsg(
            sc::string::format(
            "%1% load failed.", ClubBaseDefineFile));
        AfxMessageBox(ErrMsg.c_str(), MB_OK|MB_ICONSTOP);
    }

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

    // ------------------------------------------------------------------------
    // World battle country info load
    std::string WorldBattleCountyFile(sc::getAppPath());
    WorldBattleCountyFile.append(m_pPath->GLogicPath());
    WorldBattleCountyFile.append(m_pLogicFile->GetWorldBattleCountryFile());
    if (!m_pWbCountry->Load(GetServiceProvider(), WorldBattleCountyFile))
    {
        std::string ErrMsg(
            sc::string::format(
            "%1% load failed.", WorldBattleCountyFile));
        AfxMessageBox(ErrMsg.c_str(), MB_OK|MB_ICONSTOP);
    }

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

#ifdef LOADING_TIME_CHECK
    sc::writeLogInfo(sc::string::format("Loading GLogicData GLCrowDataMan %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
    LoadingSectionTimer.restart();
#endif

    // ------------------------------------------------------------------------
    // 스킬 창 개선의 데이터 파일 로드
    if( GLUseFeatures::GetInstance().IsUsingRenewSkillWindow() )
    {
        if (PercentCallbackFunc)
            PercentCallbackFunc(m_pLogicFile->GetSkillFileName());

        // 파일을 읽지 못하여도 기본 설정으로 계속 진행합니다.
        m_spRnSkillLearnSetting->LoadFile(m_pLogicFile->GetSkillLearnPrereqFileName());

//         if( !m_spRnSkillLearnSetting->LoadFile( GLogicFile::GetInstance().GetSkillLearnPrereqFileName() ) )
//         {
//             std::string ErrMsg(sc::string::format("%1% load failed", GLogicFile::GetInstance().GetSkillLearnPrereqFileName()));
//             sc::writeLogError(ErrMsg);
//             AfxMessageBox(ErrMsg.c_str(), MB_OK|MB_ICONSTOP);
//             return E_FAIL;
//         }

        //---------------------------------------------------//
        NSLoadingDirect::FrameMoveRender( FALSE );
        //---------------------------------------------------//
        if ( PercentCallbackFunc2 )
            PercentCallbackFunc2(76);
    }
    

#ifdef LOADING_TIME_CHECK
    sc::writeLogInfo(sc::string::format("Loading GLogicData RnSkillSetting %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
    LoadingSectionTimer.restart();
#endif

    // ------------------------------------------------------------------------
    // skill file loading
    if (PercentCallbackFunc)
        PercentCallbackFunc(m_pLogicFile->GetSkillFileName());
    if (GLSkillMan::GetInstance().LoadFile(m_pLogicFile->GetSkillFileName(), TRUE) == E_FAIL)
    {
        std::string ErrMsg(sc::string::format("%1% load failed", m_pLogicFile->GetSkillFileName()));
        sc::writeLogError(ErrMsg);
        AfxMessageBox(ErrMsg.c_str(), MB_OK|MB_ICONSTOP);
        return E_FAIL;
    }

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//
	if (PercentCallbackFunc2)
        PercentCallbackFunc2(77);


#ifdef LOADING_TIME_CHECK
    sc::writeLogInfo(sc::string::format("Loading GLogicData GLSkillMan %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
    LoadingSectionTimer.restart();
#endif

	

	// ------------------------------------------------------------------------
	// HideSet.lua loading
	if (PercentCallbackFunc)
		PercentCallbackFunc(m_pLogicFile->GetHideSetFileName());
	
	if (GLHIDESET::LoadFile(m_pLogicFile->GetHideSetFileName(), (bToolMode == TRUE ? false : (bServer==TRUE))))
	{
		std::string ErrMsg(sc::string::format("%1% load failed", m_pLogicFile->GetHideSetFileName()));
		sc::writeLogError(ErrMsg);
		AfxMessageBox(ErrMsg.c_str(), MB_OK|MB_ICONSTOP);
		return E_FAIL;
	}
	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//
	if (PercentCallbackFunc2)
        PercentCallbackFunc2(78);

#ifdef LOADING_TIME_CHECK
	sc::writeLogInfo(sc::string::format("Loading GLogicData GLHIDESET %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
	LoadingSectionTimer.restart();
#endif

	// ------------------------------------------------------------------------
	// TransformSet.lua loading
	if (PercentCallbackFunc)
		PercentCallbackFunc(m_pLogicFile->GetTransformSkinSetFileName());

	if (GLTRANSFORMSET::LoadFile(m_pLogicFile->GetTransformSkinSetFileName(), bServer==TRUE))
	{
		std::string ErrMsg(sc::string::format("%1% load failed", m_pLogicFile->GetTransformSkinSetFileName()));
		sc::writeLogError(ErrMsg);
		AfxMessageBox(ErrMsg.c_str(), MB_OK|MB_ICONSTOP);
	}
	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//
	if (PercentCallbackFunc2)
        PercentCallbackFunc2(79);

#ifdef LOADING_TIME_CHECK
	sc::writeLogInfo(sc::string::format("Loading GLogicData GLTRANSFORMSET %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
	LoadingSectionTimer.restart();
#endif

	// ------------------------------------------------------------------------
	// MiniGame_OddEven.lua loading
	if (PercentCallbackFunc)
		PercentCallbackFunc(m_pLogicFile->GetMiniGameOddEvenFileName());

	if ( MiniGame::OddEven::LoadFile(m_pLogicFile->GetMiniGameOddEvenFileName(), (bToolMode == TRUE ? false : (bServer==TRUE))) == E_FAIL )
	{
		std::string ErrMsg(sc::string::format("%1% load failed", m_pLogicFile->GetMiniGameOddEvenFileName()));
		sc::writeLogError(ErrMsg);
		AfxMessageBox(ErrMsg.c_str(), MB_OK|MB_ICONSTOP);
	}
	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//
	if (PercentCallbackFunc2)
		PercentCallbackFunc2(79);

#ifdef LOADING_TIME_CHECK
	sc::writeLogInfo(sc::string::format("Loading GLogicData GLTRANSFORMSET %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
	LoadingSectionTimer.restart();
#endif

	// ------------------------------------------------------------------------
	// GeoIP.dat loading
	if (PercentCallbackFunc)
		PercentCallbackFunc(m_pLogicFile->GetCountryCodeFileName());

	if ( CountryCode::LoadFile(m_pLogicFile->GetCountryCodeFileName(), (bToolMode == TRUE ? false : (bServer==TRUE))) == E_FAIL )
	{
		std::string ErrMsg(sc::string::format("%1% load failed", m_pLogicFile->GetCountryCodeFileName()));
		sc::writeLogError(ErrMsg);
		AfxMessageBox(ErrMsg.c_str(), MB_OK|MB_ICONSTOP);
	}
	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//
	if (PercentCallbackFunc2)
		PercentCallbackFunc2(79);

#ifdef LOADING_TIME_CHECK
	sc::writeLogInfo(sc::string::format("Loading GLogicData CountryCode %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
	LoadingSectionTimer.restart();
#endif
	
    // ------------------------------------------------------------------------
    // quest file loading
	
	if (PercentCallbackFunc)
        PercentCallbackFunc(m_pLogicFile->GetQuestFileName());

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		CGameTextMan::GetInstance().LoadText(RANPARAM::strQuestText, CGameTextMan::EM_QUEST_TEXT, RANPARAM::strCountry, CGameTextMan::GAMETEXTMAN_LOADOP_REFACT);
	}

    if (!GLQuestMan::GetInstance().LoadFile(m_pLogicFile->GetQuestFileName(), bServer==TRUE))
    {
        std::string ErrMsg(sc::string::format("%1% load failed", m_pLogicFile->GetQuestFileName()));
        sc::writeLogError(ErrMsg);
        AfxMessageBox(ErrMsg.c_str(), MB_OK|MB_ICONSTOP);
    }

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	//LoadRnattenadenceReward
	if ( GLUseFeatures::GetInstance().IsUsingRnattendancePage() )
	{
		std::string RnattenadenceRewardFile(sc::getAppPath());
		RnattenadenceRewardFile.append(pPath->GLogicPath()); //SUBPATH::GLogicPath());
		RnattenadenceRewardFile.append(GLogicData::GetInstance().GetRnAttendanceFileName());

		if (!GLQuestMan::GetInstance().LoadRnattenadenceReward(RnattenadenceRewardFile))
		{
			std::string ErrMsg(sc::string::format("%1% load failed", m_pLogicFile->GetRnAttendanceFileName()));
			sc::writeLogError(ErrMsg);
			AfxMessageBox(ErrMsg.c_str(), MB_OK|MB_ICONSTOP);
		}
	}
    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//
	if (PercentCallbackFunc2)
        PercentCallbackFunc2(80);

#ifdef LOADING_TIME_CHECK
    sc::writeLogInfo(sc::string::format("Loading GLogicData GLQuestMan %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
    LoadingSectionTimer.restart();
#endif

    // ------------------------------------------------------------------------
    // default.charclass
    if (PercentCallbackFunc)
        PercentCallbackFunc(m_pLogicFile->GetDefaultCharClassFileName());
    GLCONST_CHAR::LOADFILE(m_pLogicFile->GetDefaultCharClassFileName(), bServer);

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//
	if (PercentCallbackFunc2)
        PercentCallbackFunc2(81);


#ifdef LOADING_TIME_CHECK
    sc::writeLogInfo(sc::string::format("Loading GLogicData GLCONST_CHAR %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
    LoadingSectionTimer.restart();
#endif

    // ------------------------------------------------------------------------
    // attendance.ini
    if (PercentCallbackFunc)
        PercentCallbackFunc(m_pLogicFile->GetAttendanceFileName());
    GLCONST_ATTENDANCE::LoadFile(m_pLogicFile->GetAttendanceFileName(), bServer); // (구) 출석부

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//
	if (PercentCallbackFunc2)
        PercentCallbackFunc2(82);

#ifdef LOADING_TIME_CHECK
    sc::writeLogInfo(sc::string::format("Loading GLogicData GLCONST_ATTENDANCE %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
    LoadingSectionTimer.restart();
#endif

    // ------------------------------------------------------------------------
    // post.ini
    if (PercentCallbackFunc)
        PercentCallbackFunc(m_pLogicFile->GetPostFileName());
    GLCONST_POST::LOADFILE(m_pLogicFile->GetPostFileName(), bServer);

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

#ifdef LOADING_TIME_CHECK
    sc::writeLogInfo(sc::string::format("Loading GLogicData GLCONST_POST %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
    LoadingSectionTimer.restart();
#endif

    // ------------------------------------------------------------------------
    // sns.ini
    if (PercentCallbackFunc)
        PercentCallbackFunc(m_pLogicFile->GetSNSFileName());
    GLCONST_SNS::LOADFILE(m_pLogicFile->GetSNSFileName(), bToolMode, bServer);

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

#ifdef LOADING_TIME_CHECK
    sc::writeLogInfo(sc::string::format("Loading GLogicData GLCONST_SNS %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
    LoadingSectionTimer.restart();
#endif

    // ------------------------------------------------------------------------
    // colortable.ini
    if (PercentCallbackFunc)
        PercentCallbackFunc(m_pLogicFile->GetColorTableFileName());
    GLCONST_COLORTABLE::LOADFILE(m_pLogicFile->GetColorTableFileName(), bServer);

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

#ifdef LOADING_TIME_CHECK
    sc::writeLogInfo(sc::string::format("Loading GLogicData GLCONST_CLOLORTABLE %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
    LoadingSectionTimer.restart();
#endif

    // ------------------------------------------------------------------------
    // busstation.ini
    if (PercentCallbackFunc)
        PercentCallbackFunc(m_pLogicFile->GetBusStationFileName());
    GLBusStation::GetInstance().LOAD(m_pLogicFile->GetBusStationFileName(), bToolMode);

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

#ifdef LOADING_TIME_CHECK
    sc::writeLogInfo(sc::string::format("Loading GLogicData GLBusStation %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
    LoadingSectionTimer.restart();
#endif

    // ------------------------------------------------------------------------
    // taxistation.tsf
    if (PercentCallbackFunc)
        PercentCallbackFunc(m_pLogicFile->GetTaxiStationFileName());
    GLTaxiStation::GetInstance().LoadFile(m_pLogicFile->GetTaxiStationFileName());

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//
	if (PercentCallbackFunc2)
        PercentCallbackFunc2(84);

#ifdef LOADING_TIME_CHECK
    sc::writeLogInfo(sc::string::format("Loading GLogicData GLTaxiStation %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
    LoadingSectionTimer.restart();
#endif

    // ------------------------------------------------------------------------
    // itemmix.ims
    if (PercentCallbackFunc)
        PercentCallbackFunc(m_pLogicFile->GetProductRecipeFileName());
    GLProductRecipeMan::GetInstance().LoadFile(m_pLogicFile->GetProductRecipeFileName(), bServer);

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//
	
	if (GenItem::LoadFile("", bToolMode) == E_FAIL)
	{
		std::string ErrMsg(".genitem  load failed");
		sc::writeLogError(ErrMsg);
		AfxMessageBox(ErrMsg.c_str(), MB_OK|MB_ICONSTOP);
		return E_FAIL;
	}

#ifdef LOADING_TIME_CHECK
    sc::writeLogInfo(sc::string::format("Loading GLogicData GLItemMixMan %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
    LoadingSectionTimer.restart();
#endif

    // ------------------------------------------------------------------------
    if (PercentCallbackFunc)
        PercentCallbackFunc("class data load");
    for (WORD i=0; i<MAX_SCHOOL; i++)
    {
        for (WORD j=0; j<GLCI_NUM_ACTOR; j++)
        {
            //---------------------------------------------------//
            NSLoadingDirect::FrameMoveRender( FALSE );
            //---------------------------------------------------//

            SCHARDATA2* pNEW_CHARDATA = new SCHARDATA2;
            pNEW_CHARDATA->LOADFILE(GLCONST_CHAR::strCLASS_INIT[i][j]);
			
            SAFE_DELETE(GLCONST_CHAR::pCharData2[i][j]);
            GLCONST_CHAR::pCharData2[i][j] = pNEW_CHARDATA;

			SCHARDATA2* pNEW_JUMPINGCHARDATA = new SCHARDATA2;
			pNEW_JUMPINGCHARDATA->LOADFILE(GLCONST_CHAR::strJUMPING_CLASS_INIT[i][j]);

			SAFE_DELETE(GLCONST_CHAR::pJumpingCharData2[i][j]);
			GLCONST_CHAR::pJumpingCharData2[i][j] = pNEW_JUMPINGCHARDATA;

			if (PercentCallbackFunc){				
				TCHAR _szOutString[256];
				::wsprintf(_szOutString, "%s\n%s", 
					GLCONST_CHAR::strCLASS_INIT[i][j].c_str(), pNEW_CHARDATA->m_szName);
				PercentCallbackFunc(_szOutString);
			}			
        }
    }

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//
	if ( PercentCallbackFunc2 ) PercentCallbackFunc2(90);

#ifdef LOADING_TIME_CHECK
    sc::writeLogInfo(sc::string::format("Loading GLogicData GLCONST_CHAR class %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
    LoadingSectionTimer.restart();
#endif

	// ------------------------------------------------------------------------
	// PrivateMarket.lua
	if ( PercentCallbackFunc )
		PercentCallbackFunc( m_pLogicFile->GetPrivateMarketFileName() );

	std::string strPrivateMarketDefineFile( sc::getAppPath() );
	strPrivateMarketDefineFile.append( m_pPath->GLogicPath() );
	strPrivateMarketDefineFile.append( m_pLogicFile->GetPrivateMarketFileName() );

	private_market::GLPrivateMarketDefine::Instance()->Load( strPrivateMarketDefineFile, bServer );

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

#ifdef LOADING_TIME_CHECK
	sc::writeLogInfo(sc::string::format("Loading GLogicData GLItemMixMan %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
	LoadingSectionTimer.restart();
#endif

	// ------------------------------------------------------------------------
	// instanceTable.lua loading
    if ( GLUseFeatures::GetInstance().IsUsingNewInstance() == true )
    {
        if (PercentCallbackFunc)
            PercentCallbackFunc(m_pLogicFile->GetInstanceTableFileName());

        if (InstanceSystem::LoadFile(m_pLogicFile->GetInstanceTableFileName(), (bToolMode == TRUE ? false : (bServer==TRUE))))
        {
            std::string ErrMsg(sc::string::format("%1% load failed", m_pLogicFile->GetInstanceTableFileName()));
            sc::writeLogError(ErrMsg);
            AfxMessageBox(ErrMsg.c_str(), MB_OK|MB_ICONSTOP);
            return E_FAIL;
        }
    }
	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//
	if (PercentCallbackFunc2)
		PercentCallbackFunc2(92);

#ifdef LOADING_TIME_CHECK
	sc::writeLogInfo(sc::string::format("Loading Instance ScriptManager %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
	LoadingSectionTimer.restart();
#endif

	// ------------------------------------------------------------------------
	// MiniGameTexasHoldem.lua loading
	if ( GLUseFeatures::GetInstance().IsUsingMiniGameTexasHoldem() == true )
	{
		if (PercentCallbackFunc)
			PercentCallbackFunc(m_pLogicFile->GetMiniGameTexasHoldemFileName());

		std::string TexasHoldemDefineFile(sc::getAppPath());
		TexasHoldemDefineFile.append(m_pPath->GLogicPath());
		TexasHoldemDefineFile.append(m_pLogicFile->GetMiniGameTexasHoldemFileName());

		if ( !TexasHoldem::GLTexasHoldem::LoadFile(TexasHoldemDefineFile) )
		{
			std::string ErrMsg(sc::string::format("%1% load failed", m_pLogicFile->GetMiniGameTexasHoldemFileName()));
			sc::writeLogError(ErrMsg);
			AfxMessageBox(ErrMsg.c_str(), MB_OK|MB_ICONSTOP);
			return E_FAIL;
		}
	}
	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//
	if (PercentCallbackFunc2)
		PercentCallbackFunc2(92);

#ifdef LOADING_TIME_CHECK
	sc::writeLogInfo(sc::string::format("Loading Instance ScriptManager %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
	LoadingSectionTimer.restart();
#endif

    // ------------------------------------------------------------------------
    if (bServer)
    {
        if (PercentCallbackFunc)
            PercentCallbackFunc(TEXT("load ani set | GLCI_FIGHTER_M"));
        GLCONST_CHAR::cCONSTCLASS[GLCI_FIGHTER_M  ].LoadAniSet(GLCONST_CHAR::szCharSkin[GLCI_FIGHTER_M], bToolMode);
		if (PercentCallbackFunc)
			PercentCallbackFunc(TEXT("load ani set | GLCI_FIGHTER_W"));
        GLCONST_CHAR::cCONSTCLASS[GLCI_FIGHTER_W  ].LoadAniSet(GLCONST_CHAR::szCharSkin[GLCI_FIGHTER_W], bToolMode);
		if (PercentCallbackFunc)
			PercentCallbackFunc(TEXT("load ani set | GLCI_ARMS_M"));
        GLCONST_CHAR::cCONSTCLASS[GLCI_ARMS_M     ].LoadAniSet(GLCONST_CHAR::szCharSkin[GLCI_ARMS_M], bToolMode);
		if (PercentCallbackFunc)
			PercentCallbackFunc(TEXT("load ani set | GLCI_ARMS_W"));
        GLCONST_CHAR::cCONSTCLASS[GLCI_ARMS_W     ].LoadAniSet(GLCONST_CHAR::szCharSkin[GLCI_ARMS_W], bToolMode);
		if (PercentCallbackFunc)
			PercentCallbackFunc(TEXT("load ani set | GLCI_ARCHER_M"));
        GLCONST_CHAR::cCONSTCLASS[GLCI_ARCHER_M   ].LoadAniSet(GLCONST_CHAR::szCharSkin[GLCI_ARCHER_M], bToolMode);
		if (PercentCallbackFunc)
			PercentCallbackFunc(TEXT("load ani set | GLCI_ARCHER_W"));
        GLCONST_CHAR::cCONSTCLASS[GLCI_ARCHER_W   ].LoadAniSet(GLCONST_CHAR::szCharSkin[GLCI_ARCHER_W], bToolMode);
		if (PercentCallbackFunc)
			PercentCallbackFunc(TEXT("load ani set | GLCI_SPIRIT_M"));
        GLCONST_CHAR::cCONSTCLASS[GLCI_SPIRIT_M   ].LoadAniSet(GLCONST_CHAR::szCharSkin[GLCI_SPIRIT_M], bToolMode);
		if (PercentCallbackFunc)
			PercentCallbackFunc(TEXT("load ani set | GLCI_SPIRIT_W"));
        GLCONST_CHAR::cCONSTCLASS[GLCI_SPIRIT_W   ].LoadAniSet(GLCONST_CHAR::szCharSkin[GLCI_SPIRIT_W], bToolMode);
		if (PercentCallbackFunc)
			PercentCallbackFunc(TEXT("load ani set | GLCI_EXTREME_M"));
        GLCONST_CHAR::cCONSTCLASS[GLCI_EXTREME_M  ].LoadAniSet(GLCONST_CHAR::szCharSkin[GLCI_EXTREME_M], bToolMode);
		if (PercentCallbackFunc)
			PercentCallbackFunc(TEXT("load ani set | GLCI_EXTREME_W"));
        GLCONST_CHAR::cCONSTCLASS[GLCI_EXTREME_W  ].LoadAniSet(GLCONST_CHAR::szCharSkin[GLCI_EXTREME_W], bToolMode);
		if (PercentCallbackFunc)
			PercentCallbackFunc(TEXT("load ani set | GLCI_SCIENTIST_M"));
        GLCONST_CHAR::cCONSTCLASS[GLCI_SCIENTIST_M].LoadAniSet(GLCONST_CHAR::szCharSkin[GLCI_SCIENTIST_M], bToolMode);
		if (PercentCallbackFunc)
			PercentCallbackFunc(TEXT("load ani set | GLCI_SCIENTIST_W"));
        GLCONST_CHAR::cCONSTCLASS[GLCI_SCIENTIST_W].LoadAniSet(GLCONST_CHAR::szCharSkin[GLCI_SCIENTIST_W], bToolMode);
		if (PercentCallbackFunc)
			PercentCallbackFunc(TEXT("load ani set | GLCI_ASSASSIN_M"));
        GLCONST_CHAR::cCONSTCLASS[GLCI_ASSASSIN_M].LoadAniSet(GLCONST_CHAR::szCharSkin[GLCI_ASSASSIN_M], bToolMode);
		if (PercentCallbackFunc)
			PercentCallbackFunc(TEXT("load ani set | GLCI_ASSASSIN_W"));
        GLCONST_CHAR::cCONSTCLASS[GLCI_ASSASSIN_W].LoadAniSet(GLCONST_CHAR::szCharSkin[GLCI_ASSASSIN_W], bToolMode);
		if (PercentCallbackFunc)
			PercentCallbackFunc(TEXT("load ani set | GLCI_TRICKER_M"));
		GLCONST_CHAR::cCONSTCLASS[GLCI_TRICKER_M].LoadAniSet(GLCONST_CHAR::szCharSkin[GLCI_TRICKER_M], bToolMode);
		if (PercentCallbackFunc)
			PercentCallbackFunc(TEXT("load ani set | GLCI_TRICKER_W"));
		GLCONST_CHAR::cCONSTCLASS[GLCI_TRICKER_W].LoadAniSet(GLCONST_CHAR::szCharSkin[GLCI_TRICKER_W], bToolMode);
		if (PercentCallbackFunc)
			PercentCallbackFunc(TEXT("load ani set | GLCI_ETC_M"));
        GLCONST_CHAR::cCONSTCLASS[GLCI_ETC_M].LoadAniSet(GLCONST_CHAR::szCharSkin[GLCI_ETC_M], bToolMode);
		if (PercentCallbackFunc)
			PercentCallbackFunc(TEXT("load ani set | GLCI_ETC_W"));
        GLCONST_CHAR::cCONSTCLASS[GLCI_ETC_W].LoadAniSet(GLCONST_CHAR::szCharSkin[GLCI_ETC_W], bToolMode);
		if (PercentCallbackFunc)
			PercentCallbackFunc(TEXT("load ani set | GLCI_ACTOR_M"));
		GLCONST_CHAR::cCONSTCLASS[GLCI_ACTOR_M].LoadAniSet(GLCONST_CHAR::szCharSkin[GLCI_ACTOR_M], bToolMode);
		if (PercentCallbackFunc)
			PercentCallbackFunc(TEXT("load ani set | GLCI_ACTOR_W"));
		GLCONST_CHAR::cCONSTCLASS[GLCI_ACTOR_W].LoadAniSet(GLCONST_CHAR::szCharSkin[GLCI_ACTOR_W], bToolMode);
    }

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	if ( PercentCallbackFunc2 ) PercentCallbackFunc2(98);

#ifdef LOADING_TIME_CHECK
    sc::writeLogInfo(sc::string::format("Loading GLogicData GLCONST_CHAR server %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
    LoadingSectionTimer.restart();
#endif

    // ------------------------------------------------------------------------
    // PET Data 로딩 (안에 대입 있음)/이런 구조는 고쳐야 한다.
    if (PercentCallbackFunc)
        PercentCallbackFunc("pet data load");
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		std::vector<std::string> vecString;
		CGameTextMan::GetInstance().LoadHelpUnknownCommentString("PET_INITFILE", vecString, CGameTextMan::EM_COMMENT_TEXT);
		WORD petInitFileSize = static_cast<WORD>(vecString.size()); 
		GLCONST_PET::pGLPET.resize(petInitFileSize);
		for (WORD i = 0; i < petInitFileSize; ++i)
			GLCONST_PET::pGLPET[i] = NULL;

		GLCONST_PET::sPETSTYLE.resize(petInitFileSize);
		for (WORD i = 0; i < petInitFileSize; ++i)
			GLCONST_PET::sPETSTYLE[i] = PETSTYLE();

		for (WORD i = 0; i < petInitFileSize; ++i)
		{
			PGLPET pPet = new GLPET;
			if (!pPet->LoadFile(vecString[i], (int)i))
				return E_FAIL;
		}
	}
	else
	{
		WORD petInitFileSize = static_cast<WORD>(COMMENT::szPetInitFile.size());

		GLCONST_PET::pGLPET.resize(petInitFileSize);
		for (WORD i = 0; i < petInitFileSize; ++i)
			GLCONST_PET::pGLPET[i] = NULL;

		GLCONST_PET::sPETSTYLE.resize(petInitFileSize);
		for (WORD i = 0; i < petInitFileSize; ++i)
			GLCONST_PET::sPETSTYLE[i] = PETSTYLE();

		for (WORD i = 0; i < petInitFileSize; ++i)
		{
			PGLPET pPet = new GLPET;
			if (!pPet->LoadFile(COMMENT::szPetInitFile[i], (int)i))
				return E_FAIL;
		}
	}


	if (PercentCallbackFunc)
		PercentCallbackFunc("Item Period Extension data load");

	if ( ItemPeriodExtension::LoadFile(m_pLogicFile, bToolMode) != S_OK )
		return E_FAIL;

	if (PercentCallbackFunc)
		PercentCallbackFunc("Item Addon Random data load");

	if ( RandomAddon::Manager::GetInstance().LoadFile(m_pLogicFile) != S_OK )
		return E_FAIL;

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

#ifdef LOADING_TIME_CHECK
    sc::writeLogInfo(sc::string::format("Loading GLogicData GLPET %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
    LoadingSectionTimer.restart();
#endif

    // ------------------------------------------------------------------------
    // 탈것 data 로딩 (안에 대입 있음)/이런 구조는 고쳐야 한다.
    if (PercentCallbackFunc)
        PercentCallbackFunc("Vehicle data load");
    for (WORD i = VEHICLE_TYPE_BOARD; i < VEHICLE_TYPE_SIZE; ++i)
    {
        GLVEHICLE* pVehicle = new GLVEHICLE;
        if (!pVehicle->LoadFile(GLCONST_VEHICLE::szVehicleInit[i], (VEHICLE_TYPE) i))
            return E_FAIL;
    }

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

#ifdef LOADING_TIME_CHECK
    sc::writeLogInfo(sc::string::format("Loading GLogicData VEHICLE %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
    LoadingSectionTimer.restart();
#endif

    // ------------------------------------------------------------------------
    // 소환수 data 로딩(안에 대입 있음)/이런 구조는 고쳐야 한다.
    if (PercentCallbackFunc)
        PercentCallbackFunc("summon data load");
    for (WORD i = SUMMON_TYPE_NORMAL; i < SUMMON_TYPE_SIZE; ++i)
    {
        PGLSUMMON pSummon = new GLSUMMON;
        if (!pSummon->LoadFile(std::string(GLCONST_SUMMON::szSummonInit[i]), (SUMMON_TYPE) i))
            return E_FAIL;
    }

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	if ( PercentCallbackFunc2 ) PercentCallbackFunc2(100);

#ifdef LOADING_TIME_CHECK
    sc::writeLogInfo(sc::string::format("Loading GLogicData SUMMON server %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
    LoadingSectionTimer.restart();
#endif

	if (!bServer)
	{
		if (PercentCallbackFunc)
			PercentCallbackFunc(m_pLogicFile->GetClonSkillPosFilename());
		SkinCharMultiData::GetInstance().LoadFile(m_pLogicFile->GetClonSkillPosFilename());
		//---------------------------------------------------//
		NSLoadingDirect::FrameMoveRender( FALSE );
		//---------------------------------------------------//

		if ( PercentCallbackFunc2 ) PercentCallbackFunc2(100);
	}

	if( !bToolMode )
	{
		if ( PercentCallbackFunc ) PercentCallbackFunc("costume stats data Load");

		COSSDATA.LoadData( GetCostumeStatFileName() );

		if ( PercentCallbackFunc ) PercentCallbackFunc("Skill Change List Load"); 

		GSCL_INST.Load_Data(GetSkillChangeFileName());

		if( bServer )
		{
			if ( PercentCallbackFunc ) PercentCallbackFunc("Distinguish Skin data Load");

			GLDistinguishSkinManager::GetInstance().LoadAniSet();
		}

		NSLoadingDirect::FrameMoveRender( FALSE );		
	}


#ifdef LOADING_TIME_CHECK
	sc::writeLogInfo(sc::string::format("Loading GLogicData ClonSkill  %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
	LoadingSectionTimer.restart();
#endif

	if( false == NSRanMobile::GLRanMobile::GetInstance().Load() )
		return E_FAIL;

#ifdef LOADING_TIME_CHECK
	sc::writeLogInfo(sc::string::format("Loading GLogicData RanMobile  %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
	LoadingSectionTimer.restart();
#endif

	std::string strSkillResetDefineFile( sc::getAppPath() );
	strSkillResetDefineFile.append( m_pPath->GLogicPath() );
	strSkillResetDefineFile.append( "SkillReset.lua" );

	GLSkillResetDefine::Instance()->Load( strSkillResetDefineFile, bServer );

#ifdef LOADING_TIME_CHECK
	sc::writeLogInfo(sc::string::format("Loading GLogicData SkillReset  %1% %2% sec", LoadingSectionTimer.elapsed(), LoadingTotalTimer.elapsed()));
	LoadingSectionTimer.restart();
#endif

	return S_OK;
}

HRESULT GLogicData::ClearData()
{
	ActivityBaseData::GetInstance()->ReleaseInstance();

    GLCrowDataMan::GetInstance().FinalCleanup();
    GLSkillMan::GetInstance().FinalCleanup();

    for (int i=0; i<GLCI_NUM_ACTOR; i++)
        GLCONST_CHAR::cCONSTCLASS[i].ClearAniSet();

    for (int i=0; i<MAX_SCHOOL; i++)
	{
        for ( int j=0; j<GLCI_NUM_ACTOR; j++ )
		{
            SAFE_DELETE(GLCONST_CHAR::pCharData2[i][j]);
			SAFE_DELETE(GLCONST_CHAR::pJumpingCharData2[i][j]);
		}
	}

// 	if ( GLUseFeatures::GetInstance().IsUsingPrivateDataIntegrate() )
// 	{
// 		std::vector<std::string> vecPET_DEFINE;
// 		CGameTextMan::GetInstance().LoadHelpUnknownCommentString("PET_DEFINE", vecPET_DEFINE, CGameTextMan::EM_PETCOMMENT_TEXT);
// 		for (size_t i = 0; i < vecPET_DEFINE.size()/3; ++i)
// 			SAFE_DELETE(GLCONST_PET::pGLPET[i]);
// 	}
// 	else
// 	{
		// [shhan][2015.01.08] 알수없는 이유로 GLogicData::LoadData 작업중 실패가 된다면 COMMENT::pGLPET 에 값이 셋팅되지 않을 수 있어서
		//						COMMENT::pGLPET.size() 로 지우도록 함.
		for (size_t i = 0; i < GLCONST_PET::pGLPET.size(); ++i)
			SAFE_DELETE(GLCONST_PET::pGLPET[i]);
//	}

    for (WORD i = VEHICLE_TYPE_BOARD; i < VEHICLE_TYPE_SIZE; ++i)
        SAFE_DELETE( GLCONST_VEHICLE::pGLVEHICLE[i] );

    for (WORD i = SUMMON_TYPE_NORMAL; i < SUMMON_TYPE_SIZE; ++i)
        SAFE_DELETE( GLCONST_SUMMON::pGLSUMMON[i] );

    return S_OK;
}

void GLogicData::ReleaseInstance()
{
	GLQuestMan::ReleaseInstance();
	GLItemMan::ReleaseInstance();
}

HRESULT GLogicData::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
    HRESULT hr=S_OK;
    hr = GLItemMan::GetInstance().InitDeviceObjects(pd3dDevice);
    if (FAILED(hr))
        return hr;
    else
        return S_OK;
}

HRESULT GLogicData::DeleteDeviceObjects ()
{
    HRESULT hr=S_OK;

    hr = GLItemMan::GetInstance().DeleteDeviceObjects ();
    if ( FAILED(hr) )	return hr;

    return S_OK;
}


const DWORD GLogicData::GetRnSkillUseMoney( const SNATIVEID& sNativeID )
{
    return m_spRnSkillLearnSetting->GetUseMoney( sNativeID.Mid(), sNativeID.Sid() );
}

const bool GLogicData::GetRnSkillUseScrool( const SNATIVEID& sNativeID )
{
    return m_spRnSkillLearnSetting->GetUseScrool( sNativeID.Mid(), sNativeID.Sid() );
}

const std::string& GLogicData::GetQuestName(const SNATIVEID& QuestId)
{
    return GLQuestMan::GetInstance().GetQuestName(QuestId);
}

const std::string& GLogicData::GetQuestName(WORD MainId, WORD SubId)
{
    return GLQuestMan::GetInstance().GetQuestName(MainId, SubId);
}

const std::string& GLogicData::GetQuestName(DWORD QuestId)
{
    return GLQuestMan::GetInstance().GetQuestName(QuestId);
}

void GLogicData::SaveQuestDataSimple(sc::Excel& Xls, int Sheet, const std::string& SheetName)
{
    GLQuestMan::GetInstance().SaveQuestDataSimple(Xls,Sheet, SheetName);
}

bool GLogicData::IsExistQuest(const SNATIVEID& QuestId)
{
    return IsExistQuest(QuestId.Id());
}

bool GLogicData::IsExistQuest(DWORD QuestId)
{
    if (GLQuestMan::GetInstance().Find(QuestId))
        return true;
    else
        return false;
}

const std::string& GLogicData::GetItemName(const SNATIVEID& sNativeID)
{
    return GLItemMan::GetInstance().GetItemName(sNativeID);
}

const std::string& GLogicData::GetItemName(WORD MainId, WORD SubId)
{
    SNATIVEID ItemId(MainId, SubId);
    return GLItemMan::GetInstance().GetItemName(ItemId);
}

const std::string& GLogicData::GetItemName(DWORD ItemId)
{
    SNATIVEID sItemId(ItemId);
    return GLItemMan::GetInstance().GetItemName(sItemId);
}

SITEM* GLogicData::GetItem(WORD wMID, WORD wSID)
{
    return GLItemMan::GetInstance().GetItem(wMID, wSID);
}

SITEM* GLogicData::GetItem(const SNATIVEID& sNativeID)
{
    return GLItemMan::GetInstance().GetItem(sNativeID);
}

SITEM* GLogicData::GetItem(DWORD ItemId)
{
    return GLItemMan::GetInstance().GetItem(ItemId);
}

size_t GLogicData::GetItemMaxMid() const
{
    return GLItemMan::GetInstance().GetMaxMid();
}

size_t GLogicData::GetItemMaxSid() const
{
    return GLItemMan::GetInstance().GetMaxSid();
}

void GLogicData::SaveItemDataSimple(sc::Excel& Xls, int Sheet, const std::string& SheetName)
{
    GLItemMan::GetInstance().SaveItemDataSimple(Xls, Sheet, SheetName);
}

const std::string GLogicData::GetSkillName(const SNATIVEID& SkillId) const
{
    return GLSkillMan::GetInstance().GetSkillName(SkillId);
}

const std::string GLogicData::GetSkillName(WORD MainId, WORD SubId) const
{
    return GLSkillMan::GetInstance().GetSkillName(MainId, SubId);
}

const std::string GLogicData::GetSkillName(DWORD SkillId) const
{
    return GLSkillMan::GetInstance().GetSkillName(SkillId);
}

GLSKILL* GLogicData::GetSkillData(const SNATIVEID& SkillId) const
{
    return GLSkillMan::GetInstance().GetData(SkillId);
}

void GLogicData::SaveSkillDataSimple(sc::Excel& Xls, int Sheet, const std::string& SheetName)
{
    GLSkillMan::GetInstance().SaveSkillDataSimple(Xls, Sheet, SheetName);
}

const std::string GLogicData::GetCrowName(WORD wMID, WORD wSID)
{
    return GLCrowDataMan::GetInstance().GetCrowName(wMID, wSID);
}

const std::string GLogicData::GetCrowName(SNATIVEID sNativeID)
{
    return GLCrowDataMan::GetInstance().GetCrowName(sNativeID);
}

const std::string GLogicData::GetCrowName(DWORD NpcId)
{
    return GLCrowDataMan::GetInstance().GetCrowName(NpcId);
}

void GLogicData::SaveCrowDataSimple(sc::Excel& Xls, int Sheet, const std::string& SheetName)
{
    GLCrowDataMan::GetInstance().SaveCrowDataSimple(Xls, Sheet, SheetName);
}

bool GLogicData::SaveNpcTalkExcel(std::string& SaveFileName OUT, boost::function<void (int)> PercentCallbackFunc)
{
    return m_spNpcTalkMan->SaveExcel(SaveFileName, PercentCallbackFunc);
}

std::string GLogicData::GetNewNpcTalkFileName() const
{
    return m_pLogicFile->GetNewNpcTalkFileName();
}

bool GLogicData::LoadCountryFile()
{
    return m_pLogicFile->LoadCountryFile();
}

EMSERVICE_PROVIDER GLogicData::GetServiceProvider() const
{
    return m_pLogicFile->GetServiceProvider();
}

RENDERPARAM::EMSERVICE_TYPE GLogicData::GetServiceType(EMSERVICE_PROVIDER emType) const
{
    return m_pLogicFile->GetServiceType(emType);
}

void GLogicData::SetLogicFileFullPath(BOOL bPack)
{
    m_pLogicFile->SetFullPath(bPack);
}

std::string GLogicData::GetGlogicZipFile() const
{
    return m_pLogicFile->GetGlogicZipFile();
}

std::string GLogicData::GetQuestZipFile() const
{
    return m_pLogicFile->GetQuestZipFile();
}

std::string GLogicData::GetLevelZipFile() const
{
    return m_pLogicFile->GetLevelZipFile();
}

std::string GLogicData::GetNpcTalkZipFile() const
{
    return m_pLogicFile->GetNpcTalkZipFile();
}

std::string GLogicData::GetCommentFileName() const
{
    return m_pLogicFile->GetCommentFileName();
}

std::string GLogicData::GetItemFileName() const
{
    return m_pLogicFile->GetItemFileName();
}

std::string GLogicData::GetCrowFileName() const
{
    return m_pLogicFile->GetCrowFileName();
}

std::string GLogicData::GetSkillFileName() const
{
    return m_pLogicFile->GetSkillFileName();
}

std::string GLogicData::GetQuestFileName() const
{
    return m_pLogicFile->GetQuestFileName();
}

std::string GLogicData::GetDefaultCharClassFileName() const
{
    return m_pLogicFile->GetDefaultCharClassFileName();
}

std::string GLogicData::GetAttendanceFileName() const
{
    return m_pLogicFile->GetAttendanceFileName();
}

std::string GLogicData::GetPostFileName() const
{
    return m_pLogicFile->GetPostFileName();
}

std::string GLogicData::GetSNSFileName() const
{
    return m_pLogicFile->GetSNSFileName();
}

std::string GLogicData::GetColorTableFileName() const
{
    return m_pLogicFile->GetColorTableFileName();
}

std::string GLogicData::GetBusStationFileName() const
{
    return m_pLogicFile->GetBusStationFileName();
}

std::string GLogicData::GetTaxiStationFileName() const
{
    return m_pLogicFile->GetTaxiStationFileName();
}

std::string GLogicData::GetItemMixFileName() const
{
    return m_pLogicFile->GetProductRecipeFileName();
}

std::string GLogicData::GetUseFeaturesFileName() const
{
    return m_pLogicFile->GetUseFeaturesFileName();
}

std::string GLogicData::GetPointShopUiOptionFileName() const
{
    return m_pLogicFile->GetPointShopUiOptionFileName();
}

std::string GLogicData::GetAttendanceUiOptionFileName() const
{
    return m_pLogicFile->GetAttendanceUiOptionFileName();
}

std::string GLogicData::GetControlTypeAFileName() const
{
    return m_pLogicFile->GetControlTypeAFileName();
}

std::string GLogicData::GetControlTypeBFileName() const
{
    return m_pLogicFile->GetControlTypeBFileName();
}

std::string GLogicData::GetControlTypeFlyCameraFileName() const
{
	return m_pLogicFile->GetControlTypeFlyCameraFileName();
}

std::string GLogicData::GetAttendanceScriptName() const
{
    return m_pLogicFile->GetAttendanceScriptName();
}

std::string GLogicData::GetCaptureTheFieldScriptName() const
{
    return m_pLogicFile->GetCaptureTheFieldScriptName();
}

std::string GLogicData::GetBuffManagerScriptName() const
{
    return m_pLogicFile->GetBuffManagerScriptName();
}

std::string GLogicData::GetActivityRccPath() const
{
    return m_pLogicFile->GetActivityRccPath();
}

std::string GLogicData::GetMatchSystemScriptName() const
{
    return m_pLogicFile->GetMatchSystemScriptName();
}

std::string GLogicData::GetGroupChatScriptName() const
{
    return m_pLogicFile->GetGroupChatScriptName();
}

std::string GLogicData::GetMapListFileName() const
{
    return m_pLogicFile->GetMapListFile();
}

std::string GLogicData::GetMapListCompileFileName() const
{
	return m_pLogicFile->GetMapListCompileFile();
}

std::string GLogicData::GetMapListWBFileName() const
{
	return m_pLogicFile->GetMapListWBFile();
}

std::string GLogicData::GetMapListWBCompileFileName() const
{
	return m_pLogicFile->GetMapListWBCompileFile();
}

std::string GLogicData::GetCostumeStatFileName() const
{
	return m_pLogicFile->GetCostumeStatFileName();
}

std::string GLogicData::GetSkillChangeFileName() const
{
	return m_pLogicFile->GetSkillChangeFileName();
}

std::string GLogicData::GetMapListFileNameOld() const
{
    return m_pLogicFile->GetMapListFileOld();
}

std::string GLogicData::GetPrivateMarketFileName() const
{
	return m_pLogicFile->GetPrivateMarketFileName();
}

std::string GLogicData::GetTriggerListFileName() const
{
	std::string file;

	if (m_pLogicFile)
	{
		file = m_pLogicFile->GetTriggerListFileName();
	}

	return file;
}

std::string GLogicData::GetRnAttendanceFileName() const
{
	return m_pLogicFile->GetRnAttendanceFileName();
}

std::string GLogicData::GetLottoSystemFileName() const
{
	std::string file;

	if( m_pLogicFile )
		file = m_pLogicFile->GetLottoSystemFilename();

	return file;
}

std::string GLogicData::GetFSMContainerFileName() const
{
	std::string file;

	if( m_pLogicFile )
		file = m_pLogicFile->GetFSMContainerFileName();

	return file;
}

std::string GLogicData::GetClonSkillPosFileName() const
{
	std::string file;

	if( m_pLogicFile )
		file = m_pLogicFile->GetClonSkillPosFilename();

	return file;
}

std::string GLogicData::GetCountrySystemFileName() const
{
	std::string file;

	if( m_pLogicFile )
		file = m_pLogicFile->GetCountrySystemFileName();

	return file;
}

//std::string GLogicData::GetSetItemListFileName() const
//{
//	return m_pLogicFile->GetSetListFileName();
//}

size_t GLogicData::GetClubAuthDescSize() const
{
    return m_pClubBaseData->GetDescSize();
}

std::string GLogicData::GetClubAuthDesc(size_t Index) const
{
    return m_pClubBaseData->GetDesc(Index);
}

size_t GLogicData::GetClubGradeSize() const
{
    return m_pClubBaseData->GetGradeSize();
}

club::AUTH_GRADE GLogicData::GetClubBaseGrade(size_t Index) const
{
    return m_pClubBaseData->GetGrade(Index);
}

const GLCLUBRANK& GLogicData::GetClubRankData(size_t Rank) const
{
    if (Rank >= MAX_CLUBRANK)
        return GLCONST_CHAR::sCLUBRANK[0];
    else
        return GLCONST_CHAR::sCLUBRANK[Rank];
}

bool GLogicData::IsClubLogDeadByMonster(const SNATIVEID& MonsterID) const
{
    return m_pClubBaseData->IsLogDeadByMonster(MonsterID);
}

bool GLogicData::IsClubLogKillMonster(const SNATIVEID& MonsterID) const
{
    return m_pClubBaseData->IsLogKillMonster(MonsterID);
}

bool GLogicData::IsClubLogLevelUp(int ChaLevel) const
{
    return m_pClubBaseData->IsLogMemberLevel(ChaLevel);
}

LONGLONG GLogicData::MultiplyBigValueByPoint( LONGLONG Value, double Point )
{
    double TempPoint = floorl( Point * LIMIT_POINT_DIGIT );
    return static_cast<LONGLONG>( Value * TempPoint / LIMIT_POINT_DIGIT );
}

LONGLONG GLogicData::MultiplyBigValueByPoint( LONGLONG Value, float Point )
{
    double TempPoint = floorl( Point * LIMIT_POINT_DIGIT );
    return static_cast<LONGLONG>( Value * TempPoint / LIMIT_POINT_DIGIT );
}

//! 확률/아이템 리스트 제어 기능 : sckim, Redmine #4544
std::string GLogicData::GetRandomChanceFileName() const
{
	return m_pLogicFile->GetRandomChanceFileName();
}

std::string GLogicData::GetItemLevelFileName() const
{
	return m_pLogicFile->GetItemLevelFileName();
}

std::string GLogicData::GetItemBasicStatFileName() const
{
	return m_pLogicFile->GetItemBasicStatFileName();
}

std::string GLogicData::GetItemLinkSkillFileName() const
{
	return m_pLogicFile->GetItemLinkSkillFileName();
}

std::string GLogicData::GetCharacterSlotFileName() const
{
	return m_pLogicFile->GetCharacterSlotFileName();
}

std::string GLogicData::GetRanMobileFileName() const
{
	return m_pLogicFile->GetRanMobileFileName();
}