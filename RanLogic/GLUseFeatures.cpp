#include "pch.h"

#include "../SigmaCore/Xml/2.5.3/ticpp.h"
#include "../SigmaCore/String/StringFormat.h"
#include "../SigmaCore/String/StringUtil.h"
#include "../SigmaCore/Util/SystemInfo.h"
#include "../SigmaCore/Util/MinMessageBox.h"
#include "../SigmaCore/Log/LogMan.h"
#include "../SigmaCore/gassert.h"

#include "../=LuaPlus/src/LuaPlus/LuaPlus.h"

#include "../EngineLib/Common/SUBPATH.h"

#include "./GLUseFeatures.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

#define IMPLEMENT_FEATURES( var )															\
    std::string str##var ( #var );															\
    std::transform( str##var.begin(), str##var.end(), str##var.begin(), std::toupper );		\
    m_mFeatures.insert ( std::make_pair(str##var, boost::ref(m_s##var) ) );					\
	m_vecFeatures.push_back ( boost::ref(m_s##var) );

#define IMPLEMENT_FEATURES_DEFAULT( var, default )											\
	m_s##var.bUse = default;																\
	IMPLEMENT_FEATURES( var )

#define IMPLEMENT_FEATURES_PRIVATE( var, default )											\
	m_s##var.bUse = default;																\
	m_s##var.bInit = true;																	\
	IMPLEMENT_FEATURES( var )

// ----------------------------------------------------------------------------

GLUseFeatures& GLUseFeatures::GetInstance()
{
    static GLUseFeatures sInstance;
    return sInstance;
}

GLUseFeatures::GLUseFeatures()
    : m_pLuaState(NULL)
	, m_bUseFeaturesLoad(FALSE)
{
	IMPLEMENT_FEATURES( Post );
	IMPLEMENT_FEATURES( SNS );	
    IMPLEMENT_FEATURES( PointShop );
    IMPLEMENT_FEATURES( Attendance );
    IMPLEMENT_FEATURES( Activity );
    IMPLEMENT_FEATURES( CaptureTheField );
    IMPLEMENT_FEATURES( RebirthRecoveryExp );
    IMPLEMENT_FEATURES( RebirthRecoveryExpNpc );
    IMPLEMENT_FEATURES( ItemTooltip );
	IMPLEMENT_FEATURES( SkillTooltip );
	IMPLEMENT_FEATURES_DEFAULT( ExtremeClass, true );
	IMPLEMENT_FEATURES( NewControlType );
	IMPLEMENT_FEATURES( RenewProduct );
	IMPLEMENT_FEATURES( RenewCharWindow); // 새캐릭터창
    IMPLEMENT_FEATURES_DEFAULT( WorldBattleClub, false );
    IMPLEMENT_FEATURES_DEFAULT( EssentialSelectClub, false );
	IMPLEMENT_FEATURES( RenewInvenWindow );
	IMPLEMENT_FEATURES( CursorTargetInfo );
	IMPLEMENT_FEATURES( ReferChar );
    IMPLEMENT_FEATURES( RenewSkillWindow );
	IMPLEMENT_FEATURES( GroupChat );
	IMPLEMENT_FEATURES( RenewMapWindow ); // 새맵
    IMPLEMENT_FEATURES_DEFAULT( NewDefenseRate, true );
    IMPLEMENT_FEATURES( RenewFriendWindow );
	IMPLEMENT_FEATURES_PRIVATE( PartyWindowRenewal, false );
	IMPLEMENT_FEATURES_PRIVATE( ExpeditionWindowRenewal, false );
	IMPLEMENT_FEATURES_DEFAULT( MatchSystem, false );
    IMPLEMENT_FEATURES( NotifyWindow );
	IMPLEMENT_FEATURES( PrivateMarketSearchBuy ); // 개인상점
    IMPLEMENT_FEATURES( PrivateMarketConsignmentSale ); // 위탁판매 ( 개인상점에 종속적 )
    IMPLEMENT_FEATURES( ItemDurability );
    IMPLEMENT_FEATURES( InfinityStairs );
	IMPLEMENT_FEATURES( RnattendancePage );
	IMPLEMENT_FEATURES( StampCard );
	IMPLEMENT_FEATURES_DEFAULT( PostSendBlock, false );
	IMPLEMENT_FEATURES( ExtensionLunchSlot );
	IMPLEMENT_FEATURES( NewInstance );	
	IMPLEMENT_FEATURES( Tournament );
	IMPLEMENT_FEATURES( afkBanCTF );	
	IMPLEMENT_FEATURES( DataIntegrate, false );
	IMPLEMENT_FEATURES( MultiGate );
	IMPLEMENT_FEATURES( MiniGameTexasHoldem );
	IMPLEMENT_FEATURES( LottoSystem );
	IMPLEMENT_FEATURES_PRIVATE( LottoSystemDevelopMode, false );
	IMPLEMENT_FEATURES( ColorProperty3D );
	IMPLEMENT_FEATURES_PRIVATE( MegaphoneControl, false );
	IMPLEMENT_FEATURES(SetItemUse); //세트아이템
	IMPLEMENT_FEATURES_PRIVATE( PostRenew, false );
	IMPLEMENT_FEATURES( RandomOptionClientView );
	IMPLEMENT_FEATURES( SelectCountry );

	IMPLEMENT_FEATURES( FactionInfo );

	// 두가지가 동시에 켜져서는 안된다;
	IMPLEMENT_FEATURES( WorldBattle );
	IMPLEMENT_FEATURES( JoinWorldBattle );

	IMPLEMENT_FEATURES_PRIVATE( ItemDecompose, false );
	IMPLEMENT_FEATURES_PRIVATE( AttackSuccessRate_Renew, false );
	IMPLEMENT_FEATURES_PRIVATE( Macro, false );
	IMPLEMENT_FEATURES_PRIVATE( Macro_LimitTime, false );

	IMPLEMENT_FEATURES( CheckOverStat );
	IMPLEMENT_FEATURES( ValidationSkill );

    /*
     * redmine : #1161 Randombox 확률제어 시스템 처리
     * created : sckim, 2015.12.17, 5% 미만 아이템 당첨시 전체 공지 설정
     * modified : 
     */
	IMPLEMENT_FEATURES_PRIVATE(RandomboxChanceWin_notify, false);
	IMPLEMENT_FEATURES_PRIVATE(OPItemCheck, false);

	IMPLEMENT_FEATURES( RanMobile );

	// 캐릭터 슬롯 선택 기능;
	IMPLEMENT_FEATURES( SelectCharSlot );
}

GLUseFeatures::~GLUseFeatures()
{
    Clear();
}

const bool GLUseFeatures::LoadFile(const std::string& strFileName, const SUBPATH* pPath, bool bToolMode)
{
	if ( m_bUseFeaturesLoad )
		return true;

    std::string strFilePath(sc::getAppPath());
    strFilePath.append(pPath->GLogicPath()); //SUBPATH::GLogicPath());
    strFilePath.append(strFileName);
    m_strFilePath = strFilePath;

    try 
    {
        Clear();

        m_pLuaState = LuaPlus::LuaState::Create(true);
        if (!m_pLuaState)
        {
            std::string ErrorMsg(
                sc::string::format(
                    "GLUseFeatures::Load %1% %2%",
                    m_strFilePath,
                    LuaPlus::LuaStackObject(m_pLuaState, -1).GetString()));
            sc::ErrorMessageBox(ErrorMsg, bToolMode);
            return false;
        }

        if (0 != m_pLuaState->DoFile(m_strFilePath.c_str()))
        {
            std::string ErrorMsg(
                sc::string::format(
                    "GLUseFeatures::Load %1% %2%",
                    strFileName,
                    LuaPlus::LuaStackObject(m_pLuaState, -1).GetString()));
            sc::ErrorMessageBox(ErrorMsg, bToolMode);
            Clear();
            return false;
        }

        LuaPlus::LuaObject FeaturesList = m_pLuaState->GetGlobals()["UseFeatures"];
        for (LuaPlus::LuaTableIterator it(FeaturesList); it; it.Next())
        {
            std::string strKey = it.GetKey().GetString();
            std::transform( strKey.begin(), strKey.end(), strKey.begin(), std::toupper );

            bool bUse = it.GetValue().GetBoolean();

            MAPFEATURES_ITER findIter = m_mFeatures.find( strKey ); 
            if ( findIter != m_mFeatures.end() )
            {
                SFEATURE& ref_sFeature = findIter->second;
                ref_sFeature.bUse  = bUse;
                ref_sFeature.bInit = true;
            }
            else
            {
                std::string ErrorMsg(
                    sc::string::format(
                        "feature %1% %2% is not declared",
                        strFileName,
                        strKey));
                sc::ErrorMessageBox(ErrorMsg, bToolMode);
            }
        }
    }
    catch (const LuaPlus::LuaException& e)
    {
        std::string ErrorMsg(
            sc::string::format(
                "%1% %2%",
                m_strFilePath,
                e.GetErrorMessage()));
        sc::ErrorMessageBox(ErrorMsg, bToolMode);
        Clear();
        return false;
    }

    MAPFEATURES_ITER it = m_mFeatures.begin();
    for ( ; it != m_mFeatures.end(); ++it )
    {
        SFEATURE& ref_sFeature = it->second;

        if ( !ref_sFeature.bInit )
        {
            std::string ErrorMsg(
                sc::string::format(
                    "%1% Key='%2%' does not exist",
                    strFileName,
                    it->first));
            sc::ErrorMessageBox(ErrorMsg, bToolMode);
        }
    }

	m_bUseFeaturesLoad = TRUE;

    return true;
}

void GLUseFeatures::Clear()
{
    if ( m_pLuaState )
    {
        LuaPlus::LuaState::Destroy(m_pLuaState);
        m_pLuaState = NULL;
    }
}

const bool  GLUseFeatures::IsUsingUseFeature ( const int nKey ) const
{
	if ( m_vecFeatures.size() <= nKey ||
		0 > nKey )
		return false;

	SFEATURE& refFeature = m_vecFeatures.at( nKey );
	return refFeature.bUse;
}

const bool  GLUseFeatures::SetUseFeature ( const int nKey, const bool bUse )
{
	if ( m_vecFeatures.size() <= nKey ||
		0 > nKey )
		return false;

	SFEATURE& refFeature = m_vecFeatures.at( nKey );
	refFeature.bUse  = bUse;
	refFeature.bInit = true;

	return true;
}

const int	GLUseFeatures::GetSizeUseFeatures () const
{
	return m_vecFeatures.size();
}