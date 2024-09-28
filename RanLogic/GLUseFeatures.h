#pragma once

#include <vector>
#include <map>
#include <string>
#include <boost/noncopyable.hpp>
#include <boost/ref.hpp>

//--------------------------------------------------------------------------------------------//

#define DECLARE_FEATURES( var )                                   \
	private : SFEATURE m_s##var;                                  \
	public  : const bool IsUsing##var () { return m_s##var.bUse; }

//--------------------------------------------------------------------------------------------//

namespace LuaPlus
{
    class LuaState;
}

class SUBPATH;

//--------------------------------------------------------------------------------------------//

class GLUseFeatures : private boost::noncopyable
{
public:
	enum EMUSE_FEATURES
	{
		EMUSE_FEATURE_POST,
		EMUSE_FEATURE_SNS,
		EMUSE_FEATURE_POINTSHOP,
		EMUSE_FEATURE_ATTENDANCE,
		EMUSE_FEATURE_ACTIVITY,
		EMUSE_FEATURE_CAPTURETHEFIELD,
		EMUSE_FEATURE_REVIRTHRECOVERYEXP,
		EMUSE_FEATURE_REVIRTHRECOVERYEXPNPC,
		EMUSE_FEATURE_ITEMTOOLTIP,
		EMUSE_FEATURE_SKILLTOOLTIP,
		EMUSE_FEATURE_EXTREMECLASS,
		EMUSE_FEATURE_NEWCONTROLTYPE,
		EMUSE_FEATURE_RENEWPRODUCT,
		EMUSE_FEATURE_RENEWCHARWINDOW,
		EMUSE_FEATURE_RENEWINVENWINDOW,
		EMUSE_FEATURE_WORLDBATTLECLUB,
		EMUSE_FEATURE_ESSENTIALSELECTCLUB,
		EMUSE_FEATURE_CURSORTARGETINFO,
		EMUSE_FEATURE_REFERCHAR,
		EMUSE_FEATURE_RENEWSKILLWINDOW,
		EMUSE_FEATURE_RENEWMAPWINDOW,
		EMUSE_FEATURE_NEWDEFENSEREATE,
		EMUSE_FEATURE_PARTYWINDOWRENEWAL,
		EMUSE_FEATURE_EXPEDITIONWINDOWRENEWAL,
		EMUSE_FEATURE_RENEWFRIENDWINDOW,
		EMUSE_FEATURE_MATCHSYSTEM,
		EMUSE_FEATURE_GROUPCHAT,
		EMUSE_FEATURE_NOTIFYWINDOW,
		EMUSE_FEATURE_PRIVATEMARKETSEARCHBUY,
		EMUSE_FEATURE_PRIVATEMARKETCONSIGNMENTSALE,
		EMUSE_FEATURE_ITEMDURABILITY,
		EMUSE_FEATURE_INFINITYSTAIRS,
		EMUSE_FEATURE_RNATTENDANCEPAGE,
		EMUSE_FEATURE_STAMPCARD,
		EMUSE_FEATURE_POSTSENDBLOCK,
		EMUSE_FEATURE_NEWINSTANCE,
		EMUSE_FEATURE_TOURNAMENT,
		EMUSE_FEATURE_EXTENSIONLUNCHSLOT,
		EMUSE_FEATURE_AFKBANCTF,
		EMUSE_FEATURE_DATAINTEGRATE,
		EMUSE_FEATURE_MULTIGATE,
		EMUSE_FEATURE_MINIGAMETEXASHOLDEM,
		EMUSE_FEATURE_LOTTOSYSTEM,
		EMUSE_FEATURE_LOTTOSYSTEMDEVELOPMODE,
		EMUSE_FEATURE_COLORPROPERTY3D,
		EMUSE_FEATURE_MEGAPHONECONTROL,
		EMUSE_FEATURE_SETITEMUSE,
		EMUSE_FEATURE_POSTRENEW,
		EMUSE_FEATURE_RANDOMOPTIONCLIENTVIEW,
		EMUSE_FEATURE_SELECTCOUNTRY,
		EMUSE_FEATURE_FACTIONINFO,
		EMUSE_FEATURE_WORLDBATTLE,
		EMUSE_FEATURE_JOINWORLDBATTLE,
		EMUSE_FEATURE_ITEMDECOMPOSE,
		EMUSE_FEATURE_ATTACKSUCCESSRATE_RENEW,
		EMUSE_FEATURE_MACRO,
		EMUSE_FEATURE_MACRO_LIMITTIME,

		// �������� üũ ���;
		EMUSE_FEATURN_CHECK_OVERSTAT,
		EMUSE_FEATURE_VALIDATION_SKILL,

        /*
         * redmine : #1161 Randombox Ȯ������ �ý��� ó��
         * created : sckim, 2015.12.17, 5% �̸� ������ ��÷�� ��ü ���� ����
         * modified : 
         */
		 EMUSE_FEATURE_RANDOMBOXCHANCE_WIN_NOTIFY,
		 EMUSE_FEATURE_OPITEMCHECK,

		 EMUSE_FEATURE_RANMOBILE,

		// ĳ���� ���� ���ñ��;
		EMUSE_FEATURE_SELECT_CHARSLOT,

		// ������;
		EMUSE_FEATURE_SIZE,
	};

private:
    struct SFEATURE
    {
        bool bUse;
        bool bInit;

        SFEATURE()
            : bUse( false )
            , bInit( false )
        {
        }
    };

	BOOL m_bUseFeaturesLoad;
public:
    typedef std::map<std::string, boost::reference_wrapper<SFEATURE> > MAPFEATURES;
    typedef MAPFEATURES::iterator                                      MAPFEATURES_ITER;

	typedef std::vector< boost::reference_wrapper<SFEATURE> >			VEC_FEATURES;
	typedef VEC_FEATURES::iterator										VEC_FEATURES_ITER;
	typedef VEC_FEATURES::const_iterator								VEC_FEATURES_CITER;

public:
    const bool LoadFile(const std::string& strFileName, const SUBPATH* pPath, bool bToolMode);
    void Clear();

public:
	const bool	IsUsingUseFeature ( const int nKey ) const;
	const bool	SetUseFeature ( const int nKey, const bool bUse );
	const int	GetSizeUseFeatures () const;

private:
    LuaPlus::LuaState* m_pLuaState;
    MAPFEATURES        m_mFeatures;
	VEC_FEATURES		m_vecFeatures;

private:
    std::string m_strFilePath;

public:
    static GLUseFeatures& GetInstance();

public:
    GLUseFeatures();
    ~GLUseFeatures();

public:
    DECLARE_FEATURES( Post )
    DECLARE_FEATURES( SNS )
    DECLARE_FEATURES( PointShop )
    DECLARE_FEATURES( Attendance )
    DECLARE_FEATURES( Activity )
    DECLARE_FEATURES( CaptureTheField )
    DECLARE_FEATURES( RebirthRecoveryExp )
    DECLARE_FEATURES( RebirthRecoveryExpNpc )
    DECLARE_FEATURES( ItemTooltip )
	DECLARE_FEATURES( SkillTooltip )
	DECLARE_FEATURES( ExtremeClass )
	DECLARE_FEATURES( NewControlType )
	DECLARE_FEATURES( RenewProduct )
	DECLARE_FEATURES( RenewCharWindow )
	DECLARE_FEATURES( RenewInvenWindow )
    DECLARE_FEATURES( WorldBattleClub )
    DECLARE_FEATURES( EssentialSelectClub )
	DECLARE_FEATURES( CursorTargetInfo )
	DECLARE_FEATURES( ReferChar )
    DECLARE_FEATURES( RenewSkillWindow )
	DECLARE_FEATURES( RenewMapWindow )
    DECLARE_FEATURES( NewDefenseRate )
	DECLARE_FEATURES( PartyWindowRenewal )	
	DECLARE_FEATURES( ExpeditionWindowRenewal )
    DECLARE_FEATURES( RenewFriendWindow )
	DECLARE_FEATURES( MatchSystem )
	DECLARE_FEATURES( GroupChat )
    DECLARE_FEATURES( NotifyWindow )
	DECLARE_FEATURES( PrivateMarketSearchBuy )          // ���λ���
    DECLARE_FEATURES( PrivateMarketConsignmentSale )    // ��Ź�ǸŴ� ���λ����� �������̴�.( DependencySetting )
    DECLARE_FEATURES( ItemDurability )
    DECLARE_FEATURES( InfinityStairs )
	DECLARE_FEATURES( RnattendancePage )
	DECLARE_FEATURES( StampCard )
	DECLARE_FEATURES( PostSendBlock )
	DECLARE_FEATURES( NewInstance )
	DECLARE_FEATURES( Tournament )

	//!! ���� ������ �� �߰� �Ǿ� �ϴ� ��Ȳ�� �� ���,
	//!! �ܼ� ���� ���� �þ�� �ƴ�, ������ ���ݱ��� �ٲ�� ���ȱ� ������,
	//!! ���ö� ������ ���� ������ �ƿ� ���� ������ �Ѵ�;	
	DECLARE_FEATURES( ExtensionLunchSlot )

	DECLARE_FEATURES( afkBanCTF )
	DECLARE_FEATURES( DataIntegrate )
	DECLARE_FEATURES( MultiGate )

	DECLARE_FEATURES( MiniGameTexasHoldem )

    
	DECLARE_FEATURES( LottoSystem )
	DECLARE_FEATURES( LottoSystemDevelopMode )
	DECLARE_FEATURES( ColorProperty3D )

	DECLARE_FEATURES( MegaphoneControl )
	DECLARE_FEATURES( SetItemUse ) //��Ʈ������

	DECLARE_FEATURES( PostRenew ) // ���� �ڵ� ����

	DECLARE_FEATURES( RandomOptionClientView ) // ���� �ɼ� ���� ǥ��;
	DECLARE_FEATURES( SelectCountry )			// ���� ���� ����;

	DECLARE_FEATURES( FactionInfo )

	// �ΰ����� ���ÿ� �������� �ȵȴ�;
	DECLARE_FEATURES( WorldBattle )
	DECLARE_FEATURES( JoinWorldBattle )

	DECLARE_FEATURES( ItemDecompose );	// ���� ���;
	DECLARE_FEATURES( AttackSuccessRate_Renew );
	DECLARE_FEATURES( Macro );
	DECLARE_FEATURES( Macro_LimitTime);

	DECLARE_FEATURES( CheckOverStat );
	DECLARE_FEATURES( ValidationSkill );

    /*
     * redmine : #1161 Randombox Ȯ������ �ý��� ó��
     * created : sckim, 2015.12.17, 5% �̸� ������ ��÷�� ��ü ���� ����
     * modified : 
     */
	DECLARE_FEATURES(RandomboxChanceWin_notify);
	DECLARE_FEATURES(OPItemCheck);

	DECLARE_FEATURES( RanMobile );

	// ĳ���� ���� ���� ���;
	DECLARE_FEATURES( SelectCharSlot );
};
