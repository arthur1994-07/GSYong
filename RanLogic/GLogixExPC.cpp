#include "pch.h"

#include <boost/bind.hpp>
#include <boost/mem_fn.hpp>

#include "../SigmaCore/Math/Random.h"
#include "../SigmaCore/Log/LogMan.h"
#include "../EngineLib/DxTools/DebugSet.h"

#include "./Character/CharacterSlot/GLCharSlotMan.h"
#include "./Item/GLItemMan.h"
#include "./Item/GLItemGrindingScript.h"
#include "./Item/ItemDurability.h"
//#include "./Item/GLItemMix.h"
#include "./Product/GLProductRecipe.h"
#include "./Quest/GLQuest.h"
#include "./Quest/GLQuestMan.h"
#include "./GLogicDataMan.h"
#include "./GLogicEx.h"
#include "./Event/GLIPEvent.h"

//#include "../../RanLogic/Item/ItemSet/GLItemSet.h"

#include <boost/lambda/lambda.hpp>
#include <boost/bind.hpp>

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

GLCHARLOGIC::GLCHARLOGIC ()
: m_dwGlobalID(0)

, m_CHARINDEX(GLCI_FIGHTER_M)
, m_dwHOLDBLOW(NULL)
, m_bSTATEBLOW(FALSE)
, m_fSTATE_MOVE_RATE(1.0f)
, m_fSTATE_MOVE_VALUE(0)
, m_fSTATE_DELAY(1.0f)
, m_fSTATE_DAMAGE(1.0f)

, m_bSTATE_PANT(false)
, m_bSTATE_STUN(false)
, m_bSTATE_CONTROL(true)

, m_fHP_TOTALRATE(0)
, m_fMP_TOTALRATE(0)
, m_fSP_TOTALRATE(0)
, m_fHP_RATE(0)
, m_fMP_RATE(0)
, m_fSP_RATE(0)

, m_fVariationApplyRateHP(1.0f)
, m_fVariationApplyRateMP(1.0f)
, m_fVariationApplyRateSP(1.0f)

, m_nHP_ADD(0)
, m_nMP_ADD(0)
, m_nSP_ADD(0)
, m_nCP_ADD(0)

, m_nHP_ADD_SKILL(0)
, m_nMP_ADD_SKILL(0)
, m_nSP_ADD_SKILL(0)
, m_nCP_ADD_SKILL(0)

, m_nPotion_HP_Inc_SKILL(0)
, m_nPotion_MP_Inc_SKILL(0)
, m_nPotion_SP_Inc_SKILL(0)

, m_nGET_CP_VALUE(0)

, m_nPA_ADD(0)
, m_nMA_ADD(0)
, m_nSA_ADD(0)

, m_fSKILL_MOVE(0.0f)
, m_fOPTION_MOVE(0.0f)

, m_powerAttack(0)
, m_powerDefence(0)

, m_nSUM_PIERCE(0)
, m_fSUM_RANGE_PSY_LONG(0)

, m_fSUM_TARRANGE_ALL(0)
, m_fSUM_APPLYRANGE_ALL(0)

, m_nHIT(0)
, m_nSUM_HIT(0)
, m_nAVOID(0)
, m_nSUM_AVOID(0)

, m_wACCEPTP(0)
, m_wACCEPTP_SKILL(0)
, m_wSUM_DisSP(0)

, m_nDEFENSE(0)

, m_fDamageRate(1.0f)
, m_fDefenseRate(1.0f)
, m_fHitRate(1.0f)
, m_fAvoidRate(1.0f)

, m_fBasicRateHP(0.0f)
, m_fVariationRateHP(0.0f)
, m_fVariationRateMP(0.0f)
, m_fVariationRateSP(0.0f)

, m_fTempVarHP(0.0f)
, m_fTempVarMP(0.0f)
, m_fTempVarSP(0.0f)
, m_fTempVarCP(0.0f)
, m_fCP_Timer(0.0f)

, m_fATTVELO(0.0f)
, m_wATTRANGE(4)
, m_wATTRANGE_SKILL(0)
, m_wSUM_ATTRANGE(4)

, m_fSKILLDELAY(0.0f)

, m_fEXP_RATE(1)
, m_fQItem_EXP_RATE(0)
, m_fSkill_EXP_RATE(0)
, m_fRANDOM_BOX_EXP_RATE(0)
, m_fGENITEM_RATE(1)
, m_fGENMONEY_RATE(1)

, m_emITEM_ATT(ITEMATT_NOTHING)
, m_emANISUBTYPE(AN_SUB_NONE)

, m_idACTIVESKILL(SNATIVEID(false))
, m_dwTransformSkill(SNATIVEID::ID_NULL)
, m_emANIMAINSKILL(AN_NONE)
, m_emANISUBSKILL(AN_SUB_NONE)
, m_emANIVehicleSKILL(AN_VEHICLE_STAY)

, m_bUseArmSub(FALSE)
, m_emVehicle(EMVEHICLE_OFF)
, m_fVehicleSpeedRate( 0.0f )
, m_fVehicleSpeedVol( 0.0f )
, m_bSafeZone(false)

, m_fSUMMONACTIVETIME(0.0f)

, m_sPOSTBOXID(SNATIVEID(false))
, m_emPOSTBOXIDTYPE(EMPOSTBOXID_TYPE_NONE)

, m_ItemGarbagePosX( USHRT_MAX )
, m_ItemGarbagePosY( USHRT_MAX )

, m_fMoveVeloLimit_Min(-1.0f)
, m_fMoveVeloLimit_Max(1000.0f)
, m_IdSkillTransform(NATIVEID_NULL())
{
	memset( m_sumPowerAttribute, 0x00, sizeof(int) * SKILL::EMAPPLY_NSIZE);
	memset ( m_pITEMS, 0x00, sizeof(SITEM*)*SLOT_TSIZE );

	for ( WORD i=0; i<MAX_PETSKILLSLOT; ++i ) m_sPETSKILLFACT[i].RESET ();

	memset ( m_bActionType, false, sizeof( bool ) * SKILL::EMACTION_TYPE_NSIZE );

	memset( m_szTitle, 0, sizeof( char ) * EM_TITLEID_MAXLENGTH );

	memset(m_bSkillFactsSpecial, false, sizeof(bool) * EMSPECA_NSIZE);

    for( int i=0; i<SLOT_TSIZE; i++)
         m_DurabilitySlotItemPerformance[i] = 1;

	m_bParty = false;
}

GLCHARLOGIC::~GLCHARLOGIC ()
{
	DEL_PLAYHOSTILE_ALL();
}

void GLCHARLOGIC::RESET_SKILL_DATA()
{
	RF_FEIGN_DEATH( m_EffSkillVarSet ).ResetValue();
}

void GLCHARLOGIC::RESET_DATA ()
{	
	SCHARDATA2::Assign ( SCHARDATA2() );

	m_CHARINDEX = GLCI_FIGHTER_M;
	m_SKILLDELAY.clear();

	for (size_t i=0; i<SKILLFACT_SIZE; ++i)
		SetSkillFact(i, SSKILLFACT());

	for (size_t i=0; i<MAX_PETSKILLSLOT; ++i)
		m_sPETSKILLFACT[i].RESET ();

	m_dwHOLDBLOW = NULL;
	m_bSTATEBLOW = false;

	for (size_t i=0; i<EMBLOW_MULTI; ++i)
		m_sSTATEBLOWS[i] = SSTATEBLOW();

	for ( size_t _i(SKILL::EMAPPLY_NSIZE); _i--; )
		m_sumPowerAttribute[_i] = 0;

	m_fSTATE_MOVE_RATE = 1.0f;
	m_fSTATE_MOVE_VALUE = 0;
	m_fSTATE_DELAY = 1.0f;

	m_fSTATE_DAMAGE = 1.0f;

	m_bSTATE_PANT = false;
	m_bSTATE_STUN = false;	
	m_bSTATE_CONTROL = true;	
    
    m_sLINKHP.fTake = 1.0f;
    m_sLINKHP.fRef = 0.0f;

	m_fSKILL_MOVE = 0.0f;
	m_fOPTION_MOVE = 0.0f;	

	m_sDamageSpec.RESET();	
	m_sDefenseSkill.RESET();
	m_sINVISIBLE.RESET();	
	m_sRECVISIBLE.RESET();
	m_sRELEASE_ENDURE.RESET();
	m_sHALLUCINATE.RESET();
	m_sDOMINATE.RESET();
	m_sSTIGMA_TAKER.RESET();	
	m_sIncreaseEff.RESET();
	m_ExperienceRate.clear();
	m_sRateViewRange.RESET();

	m_sSUMSTATS = SCHARSTATS();
	m_sSUMSTATS_SKILL = SCHARSTATS();

	m_powerAttack = 0;
	m_powerDefence = 0;

	m_sSUMRESIST = SRESIST();
	m_sSUMRESIST_SKILL = SRESIST();
	m_sSUMITEM = SSUM_ITEM();
	m_sSUM_PASSIVE = SPASSIVE_SKILL_DATA();

	m_nSUM_PIERCE = 0;
	m_fSUM_RANGE_PSY_LONG = 0;

	m_fSUM_TARRANGE_ALL = 0;
	m_fSUM_APPLYRANGE_ALL = 0;

	m_wACCEPTP = 0;
	m_wACCEPTP_SKILL = 0;
	m_wSUM_DisSP = 0;

	m_nHIT = 0;
	m_nSUM_HIT = 0;
	m_nAVOID = 0;
	m_nSUM_AVOID = 0;

	m_nDEFENSE_BODY = 0;
	m_nDEFENSE = 0;
	m_nDEFENSE_SKILL = 0;

	m_gdDAMAGE = GLDWDATA();
	m_gdDAMAGE_SKILL = GLDWDATA();
	m_gdDAMAGE_BASIC = GLDWDATA();

	m_fDamageRate = 1.0f;
	m_fDefenseRate = 1.0f;
	m_fHitRate = 1.0f;
	m_fAvoidRate = 1.0f;

	m_fBasicRateHP = 0.0f;
	m_fVariationRateHP = 0.0f;
	m_fVariationRateMP = 0.0f;
	m_fVariationRateSP = 0.0f;
	
	m_fVariationApplyRateHP = 1.0f;
	m_fVariationApplyRateMP = 1.0f;
	m_fVariationApplyRateSP = 1.0f;

    m_nHP_ADD = 0;
    m_nMP_ADD = 0;
    m_nSP_ADD = 0;
    m_nCP_ADD = 0;

	m_nHP_ADD_SKILL = 0;
	m_nMP_ADD_SKILL = 0;
	m_nSP_ADD_SKILL = 0;
	m_nCP_ADD_SKILL = 0;


	m_nPotion_HP_Inc_SKILL = 0;
	m_nPotion_MP_Inc_SKILL = 0;
	m_nPotion_SP_Inc_SKILL = 0;
	
	m_nPA_ADD = 0;
	m_nMA_ADD = 0;
	m_nSA_ADD = 0;

	m_fATTVELO = 0.0f;
	m_wATTRANGE = 0;
	m_wATTRANGE_SKILL = 0;
	m_wSUM_ATTRANGE = 0;
	m_fSKILLDELAY = 0.0f;
	
	m_nGET_CP_VALUE = 0;

	m_emITEM_ATT = ITEMATT_NOTHING;

	m_emANISUBTYPE = AN_SUB_NONE;

	m_idACTIVESKILL = SNATIVEID(false);
	m_emANIMAINSKILL = AN_GUARD_N;
	m_emANISUBSKILL = AN_SUB_NONE;
	m_emANIVehicleSKILL = AN_VEHICLE_STAY;

	m_bUseArmSub = FALSE;

	for (size_t i=0; i<SLOT_TSIZE; ++i)
		m_pITEMS[i] = NULL;

	m_fTempVarHP = 0.0f;
	m_fTempVarMP = 0.0f;
    
    m_fCP_Timer = 0.0f;	

	DEL_PLAYHOSTILE_ALL();	

	m_emVehicle = EMVEHICLE_OFF;
	m_fVehicleSpeedRate = 0.0f;
	m_fVehicleSpeedVol = 0.0f;

	m_sDamageSpec.RESET();

	m_ExpSkills.clear();
	m_ExpChangeSkills.clear();

	m_bSafeZone = false;

	//m_dwSummonGUIDList.clear(); // ��ȯ�� GUID ����Ʈ
    m_dwLandEventList.clear();

	m_fSUMMONACTIVETIME = 0.0f;

	m_sPOSTBOXID	  = NATIVEID_NULL();
	m_emPOSTBOXIDTYPE = EMPOSTBOXID_TYPE_NONE;

	DISABLEALLLANDEFF();
	DISABLEALLMAPEFF();

	memset ( m_bActionType, false, sizeof( bool ) * SKILL::EMACTION_TYPE_NSIZE );

	memset( m_szTitle, 0, sizeof( char ) * EM_TITLEID_MAXLENGTH );

	memset(m_bSkillFactsSpecial, false, sizeof(bool) * EMSPECA_NSIZE);

	m_ItemGarbagePosX = USHRT_MAX;
	m_ItemGarbagePosY = USHRT_MAX;

    m_fMoveVeloLimit_Min = -1.0f;
    m_fMoveVeloLimit_Max = 1000.0f;

	m_sSummonable.RESET();

	RF_SCREEN_THROW( m_EffSkillVarSet ).Reset();
	RF_CAMERA_HIT( m_EffSkillVarSet ).ResetValue();
	RF_FEIGN_DEATH( m_EffSkillVarSet ).ResetValue();
	RF_DISGUISE( m_EffSkillVarSet ).ResetValue();
	RF_PROVOKE( m_EffSkillVarSet ).ResetValue();
	RF_IMMUNE( m_EffSkillVarSet ).ResetValue();
	RF_ACTIVE( m_EffSkillVarSet ).ResetValue( TRUE);

	m_bParty = false;
}

VOID GLCHARLOGIC::RegistProperty()
{
    //Super::RegistProperty();

    //{
    //    ic::CPropertyWord* pProperty = ic::CPropertyFactory::New< ic::CPropertyWord >();
    //    pProperty->SetPtr( &m_sHP.wNow );
    //    InsertProperty( _T( "HP Now" ), pProperty );
    //}
    //{
    //    ic::CPropertyWord* pProperty = ic::CPropertyFactory::New< ic::CPropertyWord >();
    //    pProperty->SetPtr( &m_sHP.wMax );
    //    InsertProperty( _T( "HP Max" ), pProperty );
    //}
    //{
    //    ic::CPropertyWord* pProperty = ic::CPropertyFactory::New< ic::CPropertyWord >();
    //    pProperty->SetPtr( &m_sMP.wNow );
    //    InsertProperty( _T( "MP Now" ), pProperty );
    //}
    //{
    //    ic::CPropertyWord* pProperty = ic::CPropertyFactory::New< ic::CPropertyWord >();
    //    pProperty->SetPtr( &m_sMP.wMax );
    //    InsertProperty( _T( "MP Max" ), pProperty );
    //}
    //{
    //    ic::CPropertyWord* pProperty = ic::CPropertyFactory::New< ic::CPropertyWord >();
    //    pProperty->SetPtr( &m_sSP.wNow );
    //    InsertProperty( _T( "SP Now" ), pProperty );
    //}
    //{
    //    ic::CPropertyWord* pProperty = ic::CPropertyFactory::New< ic::CPropertyWord >();
    //    pProperty->SetPtr( &m_sSP.wMax );
    //    InsertProperty( _T( "SP Max" ), pProperty );
    //}
    //{
    //    ic::CPropertyFnVoid* pProperty = ic::CPropertyFactory::New< ic::CPropertyFnVoid >();
    //    pProperty->SetFunction( boost::bind( boost::mem_fn( &GLCHARLOGIC::RESET_SKILL ), this ) );
    //    InsertProperty( _T( "Reset Skill" ), pProperty );
    //}
}

void GLCHARLOGIC::INIT_NEW_CHAR (
    const EMCHARINDEX _emCIndex,
    const DWORD dwUserID,
    const DWORD dwServerID,
    const char* szCharName,
    const WORD _wSchool,
    const WORD wHair,
    const WORD wFace,
    const WORD _wHairColor,
    const WORD _wSex,
	const BOOL	bJumping
    )
{
	EMCHARINDEX emCIndex = _emCIndex;
	WORD wSchool = _wSchool;

	if ( emCIndex>= GLCI_NUM_ACTOR )
        emCIndex = GLCI_FIGHTER_M;
	if ( wSchool>=GLCONST_CHAR::wSCHOOLNUM )
        wSchool = 0;
	
	SCHARDATA2 &CharData2 = GLCONST_CHAR::GET_CHAR_DATA2 ( wSchool, emCIndex, bJumping );

	SCHARDATA2::Assign ( CharData2 );
	m_cInventory.SetItemGenTime();

	m_dwUserID		= dwUserID;			// ������ȣ.
	m_dwServerID	= dwServerID;		// �����׷�.
	SetName(szCharName);	// ĳ���͸�.

	m_wSchool		= wSchool;
	m_wHair			= wHair;
	m_wFace			= wFace;
	m_wHairColor	= _wHairColor;
	m_wSex			= _wSex;

	// �κ��丮 �⺻ Ȯ�� �ټ� ����;
	m_wINVENLINE += GLCONST_CHAR::wInvenExtendEventLine;
	m_cInventory.SetAddLine ( GetOnINVENLINE(), true );

	INIT_DATA ( TRUE, TRUE );

	const GLCONST_CHARCLASS &cCONST = GLCONST_CHAR::cCONSTCLASS[emCIndex];

	if ( cCONST.dwHAIRNUM <= m_wHair )				m_wHair = 0;
	if ( cCONST.dwHEADNUM <= m_wFace )				m_wFace = 0;

	//	Note : �ʱ� ���� ��ġ ����. ( �б��� ���� Ʋ��. )
	//
    if ( GLCONST_CHAR::wSCHOOLNUM <= m_wSchool )
        m_wSchool = 0;

    //GLCONST_CHAR::nidSTARTMAP[m_wSchool];
    //GLCONST_CHAR::dwSTARTGATE[m_wSchool];
}

const SCHARSTATS& GLCHARLOGIC::GETSTATS_ADD () const
{
	static SCHARSTATS sSTATS_SUM;
	sSTATS_SUM = m_sStats + m_sSUMITEM.sStats;

	return sSTATS_SUM;
}

const SCHARSTATS& GLCHARLOGIC::GETSTATS_ITEM () const
{
	return m_sSUMITEM.sStats;
}

void GLCHARLOGIC::OptionMoveUp()
{
	m_fOPTION_MOVE += 0.1f;

	if( m_fOPTION_MOVE >= 3.0f )
		m_fOPTION_MOVE = 3.0f;
}

void GLCHARLOGIC::OptionMoveDown()
{
	m_fOPTION_MOVE -= 0.1f;

	if( m_fOPTION_MOVE <= 0.0f )
		m_fOPTION_MOVE = 0.0f;
}

void GLCHARLOGIC::SUM_ADDITION ( float fCONFT_POINT_RATE )
{
	int i=0;
	int nLEVEL = GETLEVEL();												//	�ɸ����� ����.
	EMCHARINDEX emCI = GETCHARINDEX ();										//	�ɸ����� ���� �ε���.
	const GLCONST_CHARCLASS &cCHARCONST = GLCONST_CHAR::cCONSTCLASS[emCI];	//	�ɸ����� �����.
	const float ZBLEVEL(float(nLEVEL-1));												//	Zero base Level.

	//	Note : ��ú� ��ų ���갪 �ջ�.
	//
	RESET_PASSIVE();
	SUM_PASSIVE ();

	//	Note : �����ۿ� ���� �ΰ� ȿ���� �ջ�.
	//
	SUM_ITEM ();

	//	Note : �⺻ ���� �� ���.
	//		STATS = �ʱ��ġ + ������ ���� �ڵ�������ġ + ĳ���� ��������Ʈ�� �к��� ��ġ + ������ �ɼ�.
	//
	m_sSUMSTATS.RESET();
	m_sSUMSTATS = cCHARCONST.sBEGIN_STATS + cCHARCONST.sLVLUP_STATS*ZBLEVEL + m_sStats + m_sSUMITEM.sStats;
	m_sSUMSTATS_SKILL = m_sSUMSTATS;

	//	Note : ���, ��� ���.
	m_powerAttack = int ( ( cCHARCONST.wBEGIN_AP + int(cCHARCONST.fLVLUP_AP*ZBLEVEL) ) * cCHARCONST.fCONV_AP );
	m_powerDefence = int ( ( cCHARCONST.wBEGIN_DP + int(cCHARCONST.fLVLUP_DP*ZBLEVEL) ) * cCHARCONST.fCONV_DP );

	//	Note : ����ġ, ���ġ.
	int powerAttribute[SKILL::EMAPPLY_NSIZE] = {0, 0, 0};
	powerAttribute[SKILL::EMAPPLY_MELEE] = int(( cCHARCONST.wBEGIN_PA + int(cCHARCONST.fLVLUP_PA*ZBLEVEL) ) * cCHARCONST.fCONV_PA );
	powerAttribute[SKILL::EMAPPLY_RANGE] = int(( cCHARCONST.wBEGIN_SA + int(cCHARCONST.fLVLUP_SA*ZBLEVEL) ) * cCHARCONST.fCONV_SA );
	powerAttribute[SKILL::EMAPPLY_MAGIC] = 0;

	//	Note : ����ġ, ���ġ, ����ġ�� STATS �� �ݿ�.
	powerAttribute[SKILL::EMAPPLY_MELEE] += int( m_sSUMSTATS.wPow * cCHARCONST.fPA_POW + m_sSUMSTATS.wDex * cCHARCONST.fPA_DEX );
	powerAttribute[SKILL::EMAPPLY_RANGE] += int( m_sSUMSTATS.wPow * cCHARCONST.fSA_POW + m_sSUMSTATS.wDex * cCHARCONST.fSA_DEX );
	powerAttribute[SKILL::EMAPPLY_MAGIC] += int( m_sSUMSTATS.wDex * cCHARCONST.fMA_DEX + m_sSUMSTATS.wSpi * cCHARCONST.fMA_SPI + m_sSUMSTATS.wInt * cCHARCONST.fMA_INT );

	//	Note : ����ġ, ���ġ, ����ġ�� SUM_ITEM �� �ݿ�.
	const int sumMelee(m_sSUMITEM.nPA + m_sSUM_PASSIVE.m_nPA);
	GLOGICEX::VARIATION ( powerAttribute[SKILL::EMAPPLY_MELEE], INT_MAX, sumMelee );

	const int sumRange(m_sSUMITEM.nSA + m_sSUM_PASSIVE.m_nSA);
	GLOGICEX::VARIATION ( powerAttribute[SKILL::EMAPPLY_RANGE], INT_MAX, sumRange );

	const int sumMagic(m_sSUMITEM.nMA + m_sSUM_PASSIVE.m_nMA);
	GLOGICEX::VARIATION ( powerAttribute[SKILL::EMAPPLY_MAGIC], INT_MAX, sumMagic );

	// �Ʒ� ȿ������ ������ �ջ� ���� ��ų�нú� �ջ� ���� ȯ��Ǿ� ó���Ǿ������Ѵ�;
	/*const INT nAllHitRatioToMelee(m_nSUM_HIT*GetIncR_AllHitRatioToMelee())
		, nAllHitRatioToRange(m_nSUM_HIT*GetIncR_AllHitRatioToRange())
		, nAllHitRatioToMagic(m_nSUM_HIT*GetIncR_AllHitRatioToMagic());
	GLOGICEX::VARIATION ( powerAttribute[SKILL::EMAPPLY_MELEE], INT_MAX, nAllHitRatioToMelee );
	GLOGICEX::VARIATION ( powerAttribute[SKILL::EMAPPLY_RANGE], INT_MAX, nAllHitRatioToRange );
	GLOGICEX::VARIATION ( powerAttribute[SKILL::EMAPPLY_MAGIC], INT_MAX, nAllHitRatioToMagic );*/

	//	Note : �ջ� ��ġ ����.
	//
	m_powerAttribute[SKILL::EMAPPLY_MELEE] = m_sumPowerAttribute[SKILL::EMAPPLY_MELEE] = powerAttribute[SKILL::EMAPPLY_MELEE];
	m_powerAttribute[SKILL::EMAPPLY_RANGE] = m_sumPowerAttribute[SKILL::EMAPPLY_RANGE] = powerAttribute[SKILL::EMAPPLY_RANGE];
	m_powerAttribute[SKILL::EMAPPLY_MAGIC] = m_sumPowerAttribute[SKILL::EMAPPLY_MAGIC] = powerAttribute[SKILL::EMAPPLY_MAGIC];

	//	Note : HP, MP, SP �ѷ� Ȯ��.
    //
	const float fTotalHPRate = (1.0f + m_sSUM_PASSIVE.m_fHP_RATE + m_fHP_RATE) * fCONFT_POINT_RATE;
    const int  nHPMax(m_sHP.nMax);
	m_sHP.nMax = 0;
	int nHPTemp(0);
	GLOGICEX::VARIATION( nHPTemp, INT_MAX, int ( m_sSUMSTATS.wStr*cCHARCONST.fHP_STR + m_sSUMITEM.nHP + m_sSUM_PASSIVE.m_nHP));
	GLOGICEX::VARIATION( m_sHP.nMax, INT_MAX, int(float(nHPTemp) * fTotalHPRate));

	// HP�������� �ƹ��� �����ص� MaxHP�� 1���Ϸδ� ������ �� ����. [7/16/2014 hsshin];
	if ( m_sHP.nMax <= 0 )
		m_sHP.nMax = 1;
	
    const float fTotalMPRate((1.0f + m_sSUM_PASSIVE.m_fMP_RATE + m_fMP_RATE) * fCONFT_POINT_RATE);
	const int nMPMax(m_sMP.nMax);
	m_sMP.nMax = 0;
	int nMPTemp(0);
	GLOGICEX::VARIATION( nMPTemp, USHRT_MAX, int(m_sSUMSTATS.wSpi*cCHARCONST.fMP_SPI + m_sSUMITEM.nMP + m_sSUM_PASSIVE.m_nMP));
	GLOGICEX::VARIATION( m_sMP.nMax, USHRT_MAX, int(float(nMPTemp) * fTotalMPRate));
	
    const float fTotalSPRate((1.0f + m_sSUM_PASSIVE.m_fSP_RATE+m_fSP_RATE) * fCONFT_POINT_RATE);
    const int nSPMax(m_sSP.nMax);
	m_sSP.nMax = 0;
	int nSPTemp(0);
	GLOGICEX::VARIATION( nSPTemp, USHRT_MAX, int ( m_sSUMSTATS.wSta*cCHARCONST.fSP_STA + m_sSUMITEM.nSP + m_sSUM_PASSIVE.m_nSP));
	GLOGICEX::VARIATION( m_sSP.nMax, USHRT_MAX, int(float(nSPTemp) * fTotalSPRate));

    //! �������� �޶������� ���ʿ��� Now���� ���� ��ȭ��Ű�� ���Ͽ�
     if ( m_fHP_TOTALRATE != 0.0f )
	 { // float ������ ���� 0.000001 ���̰� ����;
		 if ( static_cast<int>((m_fHP_TOTALRATE - fTotalHPRate) * 1000.0f) != 0 )
			m_sHP.nNow = int(float(m_sHP.nNow) * (float(m_sHP.nMax) / float(nHPMax)));
	 }

    if ( m_fMP_TOTALRATE != 0.0f && m_fMP_TOTALRATE != fTotalMPRate )
        m_sMP.nNow = int(float(m_sMP.nNow) * (float(m_sMP.nMax) / float(nMPMax)));

    if ( m_fSP_TOTALRATE != 0.0f && m_fSP_TOTALRATE != fTotalSPRate )
        m_sSP.nNow = int(float(m_sSP.nNow) * (float(m_sSP.nMax) / float(nSPMax)));

	m_fHP_TOTALRATE = fTotalHPRate;
	m_fMP_TOTALRATE = fTotalMPRate;
    m_fSP_TOTALRATE = fTotalSPRate;

    m_sHP.LIMIT();
    m_sMP.LIMIT();
    m_sSP.LIMIT();

	//	Note : ������, ȸ���� ���.
	m_nHIT = int ( m_sSUMSTATS.wDex*cCHARCONST.fHIT_DEX + m_sSUMITEM.nHitRate + m_sSUM_PASSIVE.m_nHIT );
	m_nAVOID = int ( m_sSUMSTATS.wDex*cCHARCONST.fAVOID_DEX + m_sSUMITEM.nAvoidRate + m_sSUM_PASSIVE.m_nAVOID );

	m_nDEFENSE_BODY = int ( m_powerDefence + m_sSUMSTATS.wDex*cCHARCONST.fDEFENSE_DEX );

	//	Note : ���� ( DP + POW*��� + ITEM_DEF ) * �������� ����.
	m_nDEFENSE_SKILL = m_nDEFENSE = int ( m_nDEFENSE_BODY + m_sSUMITEM.nDefense + m_sSUM_PASSIVE.m_nDEFENSE );
	
	//	Note : ���ݷ�.
	m_gdDAMAGE.nMax = m_gdDAMAGE.nNow = int ( m_powerAttack + m_sSUM_PASSIVE.m_nDAMAGE );

	m_gdDAMAGE_SKILL = m_gdDAMAGE;

	m_gdDAMAGE_BASIC = m_gdDAMAGE_SKILL;

	const INT nAllMPRatioToAttack(m_sMP.nMax*GetIncR_AllMPRatioToAttack())
		, nPhysicsDamageNow(m_gdDAMAGE_BASIC.nNow + m_sSUMITEM.gdDamage.nNow + nAllMPRatioToAttack)
		, nPhysicsDamageMax(m_gdDAMAGE_BASIC.nMax + m_sSUMITEM.gdDamage.nMax + nAllMPRatioToAttack);

	m_gdDAMAGE_BASIC.nNow = nPhysicsDamageNow < 0 ? INT_MAX : nPhysicsDamageNow;
	m_gdDAMAGE_BASIC.nMax = nPhysicsDamageMax < 0 ? INT_MAX : nPhysicsDamageMax;

	if ( ISLONGRANGE_ARMS() )
		m_gdDAMAGE_BASIC.VAR_PARAM ( m_sumPowerAttribute[SKILL::EMAPPLY_RANGE] );	//	���.
	else
		m_gdDAMAGE_BASIC.VAR_PARAM ( m_sumPowerAttribute[SKILL::EMAPPLY_MELEE] );	//	����.
	// ���� ���ݵ� �־�� ��;

	
	//	Note : ���׷�. ( ������ �����ƿ��� ������ ����. )
	m_sSUMRESIST.RESET();
	m_sSUMRESIST = m_sSUM_PASSIVE.m_sSUMRESIST + m_sSUMITEM.sResist;

	EMSLOT emRHand = GetCurRHand();
	EMSLOT emLHand = GetCurLHand();

	if ( m_pITEMS[emRHand] )
	{
		m_emITEM_ATT = CONVERT_ITEMATT( m_pITEMS[emRHand]->sSuitOp.emAttack );
		m_wATTRANGE = GET_ATTACKDISTANCE_USING_ITEMATT( m_emITEM_ATT );

		//m_emITEM_ATT = m_pITEMS[emRHand]->sSuitOp.emAttack;
		//m_wATTRANGE = m_pITEMS[emRHand]->sSuitOp.wAttRange;
	}
	else
	{
		m_emITEM_ATT = ITEMATT_NOTHING;
		m_wATTRANGE = GLCONST_CHAR::wMAXATRANGE_SHORT;
	}

	//	Note : ���ڶ�� ��ġ�� �ջ�. ( STATS, LEVEL )
	//
	m_wACCEPTP = 0;
	m_wACCEPTP += CALC_ACCEPTP ( GET_SLOT_ITEM(emLHand) );
	m_wACCEPTP += CALC_ACCEPTP ( GET_SLOT_ITEM(emRHand) );

	//	Note : "SP�Һ�� ���Ǵ� �䱸��ġ" + "������ ���� �Һ� SP �ջ�"
	//
	m_wSUM_DisSP = m_wACCEPTP;

	SITEM* pRHAND = GET_SLOT_ITEMDATA ( emRHand );
	SITEM* pLHAND = GET_SLOT_ITEMDATA ( emLHand );

	if ( pRHAND )	m_wSUM_DisSP += pRHAND->sSuitOp.wReqSP;
	if ( pLHAND )	m_wSUM_DisSP += pLHAND->sSuitOp.wReqSP;

	m_sHP.LIMIT ();
	m_sMP.LIMIT ();
	m_sSP.LIMIT ();
}

void GLCHARLOGIC::SUM_ITEM ()
{
	m_sSUMITEM.RESET();

	SUM_ITEM_BASIC();
	SUM_ITEM_COSTUM ();
	SUM_ITEM_SET();
	SUM_ITEM_ADDOP_LIMIT();
}

void GLCHARLOGIC::SUM_ITEM_ADDOP_LIMIT()
{
	if ( GLCONST_CHAR::nBossFixedDamageMax < m_sSUMITEM.nInc_BossFixedDamage )
		m_sSUMITEM.nInc_BossFixedDamage = GLCONST_CHAR::nBossFixedDamageMax;

	if ( GLCONST_CHAR::fIgnore_BossDamageMax < m_sSUMITEM.fIncR_Ignore_BossDamage )
		m_sSUMITEM.fIncR_Ignore_BossDamage = GLCONST_CHAR::fIgnore_BossDamageMax;

	if ( GLCONST_CHAR::fIncRCriticalMax_EmergencyMob < m_sSUMITEM.fIncR_Critical_EmergencyMob )
		m_sSUMITEM.fIncR_Critical_EmergencyMob = GLCONST_CHAR::fIncRCriticalMax_EmergencyMob;

	if ( GLCONST_CHAR::fIncRCriticalMax_EmergencyBoss < m_sSUMITEM.fIncR_Critical_EmergencyBoss )
		m_sSUMITEM.fIncR_Critical_EmergencyBoss = GLCONST_CHAR::fIncRCriticalMax_EmergencyBoss;

	if ( GLCONST_CHAR::fDecRDamageMax_EmergencyState < m_sSUMITEM.fDecR_Damage_EmergencyState )
		m_sSUMITEM.fDecR_Damage_EmergencyState = GLCONST_CHAR::fDecRDamageMax_EmergencyState;	

	if ( GLCONST_CHAR::fIncRExpAddEffectMax < m_sSUMITEM.fIncR_ExpAddEffect )
		m_sSUMITEM.fIncR_ExpAddEffect = GLCONST_CHAR::fIncRExpAddEffectMax;

	if ( GLCONST_CHAR::fIncRDropGenMoneyMax < m_sSUMITEM.fIncR_DropGenMoney )
		m_sSUMITEM.fIncR_DropGenMoney = GLCONST_CHAR::fIncRDropGenMoneyMax;

	if ( GLCONST_CHAR::fDecRDamageMax_MeleeSkill < m_sSUMITEM.fDecR_Damage_MeleeSkill )
		m_sSUMITEM.fDecR_Damage_MeleeSkill = GLCONST_CHAR::fDecRDamageMax_MeleeSkill;

	if ( GLCONST_CHAR::fDecRDamageMax_RangeSkill < m_sSUMITEM.fDecR_Damage_RangeSkill )
		m_sSUMITEM.fDecR_Damage_RangeSkill = GLCONST_CHAR::fDecRDamageMax_RangeSkill;

	if ( GLCONST_CHAR::fDecRDamageMax_MagicSkill < m_sSUMITEM.fDecR_Damage_MagicSkill )
		m_sSUMITEM.fDecR_Damage_MagicSkill = GLCONST_CHAR::fDecRDamageMax_MagicSkill;

	if ( GLCONST_CHAR::nInc_HPMax_SoloPlay < m_sSUMITEM.nInc_HP_SoloPlay )
		m_sSUMITEM.nInc_HP_SoloPlay = GLCONST_CHAR::nInc_HPMax_SoloPlay;

	if ( GLCONST_CHAR::nInc_HPMax_PartyPlay < m_sSUMITEM.nInc_HP_PartyPlay )
		m_sSUMITEM.nInc_HP_PartyPlay = GLCONST_CHAR::nInc_HPMax_PartyPlay;

	if ( GLCONST_CHAR::fIncR_AtkSpeedMax_SoloPlay < m_sSUMITEM.fIncR_AtkSpeed_SoloPlay )
		m_sSUMITEM.fIncR_AtkSpeed_SoloPlay = GLCONST_CHAR::fIncR_AtkSpeedMax_SoloPlay;

	if ( GLCONST_CHAR::fIncR_AtkSpeedMax_PartyPlay < m_sSUMITEM.fIncR_AtkSpeed_PartyPlay )
		m_sSUMITEM.fIncR_AtkSpeed_PartyPlay = GLCONST_CHAR::fIncR_AtkSpeedMax_PartyPlay;

	for ( INT i = 0; i < SKILL::MAX_GRADE; ++i )
	{
		if ( GLCONST_CHAR::fInc_RandomOp_SkillDurationAddTimeMax < m_sSUMITEM.fInc_SkillDurationTAdd[i] )
			m_sSUMITEM.fInc_SkillDurationTAdd[i] = GLCONST_CHAR::fInc_RandomOp_SkillDurationAddTimeMax;
	}

	for ( INT i = 0; i < SKILL::MAX_GRADE; ++i )
	{
		if ( GLCONST_CHAR::fIncR_RandomOp_SkillDamageAddMax < m_sSUMITEM.fIncR_SkillDamageAdd[i] )
			m_sSUMITEM.fIncR_SkillDamageAdd[i] = GLCONST_CHAR::fIncR_RandomOp_SkillDamageAddMax;
	}

	for ( INT i = 0; i < SKILL::MAX_GRADE; ++i )
	{
		if ( GLCONST_CHAR::fDecR_SkillDelayTMax < m_sSUMITEM.fDecR_SkillDelayT[i] )
			m_sSUMITEM.fDecR_SkillDelayT[i] = GLCONST_CHAR::fDecR_SkillDelayTMax;
	}

	if ( GLCONST_CHAR::fIncR_CriticalDamageMax < m_sSUMITEM.fIncR_CriticalDamage )
		m_sSUMITEM.fIncR_CriticalDamage = GLCONST_CHAR::fIncR_CriticalDamageMax;

	if ( GLCONST_CHAR::fIncR_CrushingBlowMax < m_sSUMITEM.fIncR_CrushingBlowDamage )
		m_sSUMITEM.fIncR_CrushingBlowDamage = GLCONST_CHAR::fIncR_CrushingBlowMax;

	if ( GLCONST_CHAR::fIncR_LifePerKillMax < m_sSUMITEM.fIncR_LifePerKill )
		m_sSUMITEM.fIncR_LifePerKill = GLCONST_CHAR::fIncR_LifePerKillMax;
	
	if ( GLCONST_CHAR::fIncR_MPPerKillMax < m_sSUMITEM.fIncR_MPPerKill )
		m_sSUMITEM.fIncR_MPPerKill = GLCONST_CHAR::fIncR_MPPerKillMax;

	if ( GLCONST_CHAR::fIncR_SPPerKillMax < m_sSUMITEM.fIncR_SPPerKill )
		m_sSUMITEM.fIncR_SPPerKill = GLCONST_CHAR::fIncR_SPPerKillMax;

	if ( GLCONST_CHAR::fIncR_HPMPSPPerKillMax < m_sSUMITEM.fIncR_HPMPSPPerKill )
		m_sSUMITEM.fIncR_MPPerKill = GLCONST_CHAR::fIncR_HPMPSPPerKillMax;

	if ( GLCONST_CHAR::fIncR_AllMPRatioToAttackMax < m_sSUMITEM.fIncR_AllMPRatioToAttack )
		m_sSUMITEM.fIncR_AllMPRatioToAttack = GLCONST_CHAR::fIncR_AllMPRatioToAttackMax;

	if ( GLCONST_CHAR::fIncR_AllSPRatioToHpMax < m_sSUMITEM.fIncR_AllSPRatioToHp )
		m_sSUMITEM.fIncR_AllSPRatioToHp = GLCONST_CHAR::fIncR_AllSPRatioToHpMax;

	if ( GLCONST_CHAR::fIncR_AllHitRatioToMeleeMax < m_sSUMITEM.fIncR_AllHitRatioToMelee )
		m_sSUMITEM.fIncR_AllHitRatioToMelee = GLCONST_CHAR::fIncR_AllHitRatioToMeleeMax;

	if ( GLCONST_CHAR::fIncR_AllHitRatioToRangeMax < m_sSUMITEM.fIncR_AllHitRatioToRange )
		m_sSUMITEM.fIncR_AllHitRatioToRange = GLCONST_CHAR::fIncR_AllHitRatioToRangeMax;

	if ( GLCONST_CHAR::fIncR_AllHitRatioToMagicMax < m_sSUMITEM.fIncR_AllHitRatioToMagic )
		m_sSUMITEM.fIncR_AllHitRatioToMagic = GLCONST_CHAR::fIncR_AllHitRatioToMagicMax;
}

bool GLCHARLOGIC::SUM_ITEM_BASIC_CS( const SITEMCUSTOM& rItemCustom, float fDurability, EMSLOT eSlot)
{
	if( !rItemCustom.IsCSApplied() ) return false;

	int iValue          = 0;
	double fValue       = 0;
	WORD iInvestPt      = 0;
	COSTUME_STATS eStat = E_NONE;
	COSTUMEUSERVALUE::sCostumeUser::sInvest sData;

	SITEM &sItem = *m_pITEMS[eSlot];

	bool bDistingush = false;

	if( rItemCustom.nidDISGUISE != NATIVEID_NULL() )
	{
		bDistingush = true;
	}

	for ( WORD wRef = 0; wRef < COSTUMEUSERVALUE::sCostumeUser::MAX_COS_SIZE; ++wRef )
	{
		eStat     = (COSTUME_STATS)rItemCustom.costumeUser.Invest[wRef].cStatType;

		iValue = 0; fValue = 0; iInvestPt = 0;
		
		if( eStat != E_NONE )
		{
			iInvestPt = rItemCustom.costumeUser.Invest[wRef].wInvestPt;	
			fValue    = COSSDATA.GetStatInc(sItem.sSuitOp.emSuit, eStat);
			fValue    *= iInvestPt;
			
			iValue = static_cast<int>(fValue);
			iValue = static_cast<int>(iValue*fDurability);
			fValue = static_cast<float>( fValue * fDurability );
		}
		
		switch( eStat )
		{
		// �Ϲ�
		case E_EXPMULTIPLE   : 
			{
				float fExpMultiple = 1.0f + fValue;

				if( (fExpMultiple > 1.0f) || (fDurability < 1.0f) )
				{
					fExpMultiple = ( (fExpMultiple - 1.0f) * fDurability ) + 1.0f;
				}

				if( fExpMultiple > 1.0f )
				{
					m_sSUMITEM.fExpMultiple += fExpMultiple - 1.0f;
					//m_sSUMITEM.fExpMultiple = max( m_sSUMITEM.fExpMultiple, 1.0f );
				}

			}
			break;
		case E_DEFENCE       : 
			{
				if( !bDistingush ) m_sSUMITEM.nDefense += iValue; 
			}
			break;
		case E_RESIST        : 
			{
				if( !bDistingush ) {
					m_sSUMITEM.sResist.nElectric	+= iValue;
					m_sSUMITEM.sResist.nFire		+= iValue;
					m_sSUMITEM.sResist.nIce			+= iValue;
					m_sSUMITEM.sResist.nPoison		+= iValue;
					m_sSUMITEM.sResist.nSpirit		+= iValue;
				}
			}
			break;
		// ����
		case E_ADD_HITRATE	 : m_sSUMITEM.nHitRate += iValue;	  break;
		case E_ADD_AVOIDRATE : m_sSUMITEM.nAvoidRate += iValue; break;
		case E_ADD_DAMAGE    :
			{
				m_sSUMITEM.gdDamage.nNow += iValue;
				m_sSUMITEM.gdDamage.nMax += iValue;
			}
			break;
		case E_ADD_DEFENSE   : m_sSUMITEM.nDefense    += iValue; break;
		case E_ADD_HP        : m_sSUMITEM.nHP         += iValue; break;
		case E_ADD_STATS_POW : m_sSUMITEM.sStats.wPow += iValue; break;
		case E_ADD_STATS_STA : m_sSUMITEM.sStats.wStr += iValue; break;
		case E_ADD_STATS_SPI : m_sSUMITEM.sStats.wSpi += iValue; break;
		case E_ADD_STATS_DEX : m_sSUMITEM.sStats.wDex += iValue; break;
		case E_ADD_ENERGY    : m_sSUMITEM.nMA	      += iValue; break;
		case E_ADD_SHOOTING  : m_sSUMITEM.nSA	      += iValue; break;
		case E_ADD_MELEE     : m_sSUMITEM.nPA	      += iValue; break;
		// ��ȭ��
		case E_R_HP_INC      : m_sSUMITEM.fIncR_HP    += fValue; break;
		case E_R_MP_INC      : m_sSUMITEM.fIncR_MP    += fValue; break;
		case E_R_SP_INC      : m_sSUMITEM.fIncR_SP    += fValue; break;
		case E_R_HMSP_INC    :
			{
				m_sSUMITEM.fIncR_HP    += fValue;
				m_sSUMITEM.fIncR_MP    += fValue;
				m_sSUMITEM.fIncR_SP    += fValue;
			}
			break;
		case E_R_MOVESPEED   :
			{
				if (eSlot != SLOT_VEHICLE)
					m_sSUMITEM.fIncR_MoveSpeed += fValue;
				else if (m_emVehicle != EMVEHICLE_OFF)
					m_sSUMITEM.fIncR_MoveSpeed += m_fVehicleSpeedRate + fValue;
			}
			break;
		case E_R_ATTACKSPEED : m_sSUMITEM.fIncR_AtkSpeed     += fValue; break;
		case E_R_CRITICALHIT : m_sSUMITEM.fIncR_Critical     += fValue; break;
		case E_R_STRIKEHIT   : m_sSUMITEM.fIncR_CrushingBlow += fValue; break;
		}
	}

	return true;
}

bool GLCHARLOGIC::SUM_ITEM_BASIC_WEAPON( EMSLOT emSLOT, float fDurability, const SITEMCUSTOM& sItemCustom )
{
//#ifdef _RELEASED
	/*
	if( emSLOT != SLOT_RHAND   && emSLOT != SLOT_LHAND &&
		emSLOT != SLOT_LHAND_S && emSLOT != SLOT_RHAND_S  )
		return false;
	*/

	SITEM &sItem = *m_pITEMS[emSLOT];

	if( sItem.sSuitOp.emSuit != SUIT_HANDHELD ) return false;

	// RM #4695 - [����������]�ڽ�Ƭ ����� ĳ��������â ������ ���� ���� ��û
	// fDurability - float ���� ���� ���·� ���� �Ѿ���µ�
	// �Ʒ��� int�� �� ��ȯ�� �ϸ鼭 ���� ���� 1�̻� ���̳��� �ȴ�.
	// �� ������ �� ������.
	float fApplyValue = fDurability + 0.000001f;

	/* ==  �⺻ �ɷ�ġ ============================================================ */
	if( !GLCONST_CHAR::nDurabilityApply[DURABILITY_APPLY_BASIC] )
		fApplyValue = 1.f;

	GLPADATA& sDamage = sItemCustom.getdamage( 0.0f, m_CHARINDEX, m_wLevel);

	// �⺻ ���ݷ�
	//int nMinDmg = static_cast<int>( sItem.sSuitOp.gdDamage.wLow ); 
	//int nMaxDmg = static_cast<int>( sItem.sSuitOp.gdDamage.wHigh );
	int nMinDmg = static_cast<int>( sItemCustom.GetBasicAttackDamage().GetLow() ); 
	int nMaxDmg = static_cast<int>( sItemCustom.GetBasicAttackDamage().GetHigh() );

	int nExtraValue  = static_cast<int>( sItemCustom.GETGRADE_DAMAGE() );
	int nGradeDamage = static_cast<int>( sItemCustom.GETGRADE_DAMAGE() * fApplyValue );

	int iMin = nMinDmg + nExtraValue;
	int iMax = nMaxDmg + nExtraValue;

	GLDWDATA tempDamage = GLDWDATA( iMin, iMax ) * fApplyValue;

	// �⺻ ����
	//int nBasicDef = static_cast<int>( sItem.sSuitOp.nDefense);
	int nBasicDef = static_cast<int>( sItemCustom.GetBasicDefence() );
	nExtraValue = sItemCustom.GETGRADE_DEFENSE();

	int iDefense = static_cast<int>( (nBasicDef + nExtraValue) * fApplyValue );
	// �⺻ ���߷� 
	int nBasicHitRate = static_cast<int>( sItem.sSuitOp.nHitRate * fApplyValue );
	// �⺻ ȸ�Ƿ�
	int nBasicAvoidRate = static_cast<int>( sItem.sSuitOp.nAvoidRate * fApplyValue );

	m_sSUMITEM.gdDamage			+= tempDamage;
	m_sSUMITEM.nDefense			+= iDefense;
	m_sSUMITEM.nAvoidRate		+= nBasicAvoidRate;
	m_sSUMITEM.nHitRate			+= nBasicHitRate;

	WORD wMaDAMAGE = 0;

	/* == ���� �ɷ�ġ ===================================================================== */

	fApplyValue = fDurability;

	if( !GLCONST_CHAR::nDurabilityApply[DURABILITY_APPLY_ADD] ) fApplyValue = 1.f;

	for ( DWORD addon=0; addon<SITEMCUSTOM_ADDON::ADDON_SIZE; ++addon )
	{
		const DWORD dwType = static_cast< DWORD >( sItemCustom.sAddonInfo.m_sADDON[addon].emTYPE );

		// ����ɼ� ���� �����´�;
		const int nValue = static_cast< int >( sItemCustom.sAddonInfo.GetValue< int >( addon, ITEM::bAddonValueInteger[ dwType ] ) * fApplyValue );
		const WORD wValue = static_cast< WORD >( sItemCustom.sAddonInfo.GetValue< WORD >( addon, ITEM::bAddonValueInteger[ dwType ] ) * fApplyValue );
		const float fValue = static_cast< float >( sItemCustom.sAddonInfo.GetValue< float >( addon, ITEM::bAddonValueInteger[ dwType ] ) * fApplyValue );

		// ����ɼ��� �����Ѵ�;
		switch ( dwType )
		{
		case EMADD_HITRATE:
			m_sSUMITEM.nHitRate	+= nValue;
			break;

		case EMADD_AVOIDRATE:
			m_sSUMITEM.nAvoidRate += nValue;
			break;

		case EMADD_DAMAGE:
			m_sSUMITEM.gdDamage.nNow += nValue;
			m_sSUMITEM.gdDamage.nMax += nValue;
			break;

		case EMADD_DEFENSE:
			m_sSUMITEM.nDefense += nValue;
			break;

		case EMADD_HP:
			m_sSUMITEM.nHP += nValue;
			break;

		case EMADD_MP:
			m_sSUMITEM.nMP += nValue;
			break;

		case EMADD_SP:
			m_sSUMITEM.nSP += nValue;
			break;

		case EMADD_STATS_POW:
			m_sSUMITEM.sStats.wPow += wValue;
			break;

		case EMADD_STATS_STR:
			m_sSUMITEM.sStats.wStr += wValue;
			break;

		case EMADD_STATS_SPI:
			m_sSUMITEM.sStats.wSpi += wValue;
			break;

		case EMADD_STATS_DEX:
			m_sSUMITEM.sStats.wDex += wValue;
			break;

		case EMADD_STATS_INT:
			m_sSUMITEM.sStats.wInt += wValue;
			break;

		case EMADD_STATS_STA:
			m_sSUMITEM.sStats.wSta += wValue;
			break;

		case EMADD_PA:
			m_sSUMITEM.nPA += nValue;
			break;

		case EMADD_SA:
			m_sSUMITEM.nSA += nValue;
			break;

		case EMADD_MA:
			wMaDAMAGE += wValue;
			break;

		case EMADDEX_INCR_HP:
			m_sSUMITEM.fIncR_HP += fValue;
			break;

		case EMADDEX_INCR_MP:
			m_sSUMITEM.fIncR_MP += fValue;
			break;

		case EMADDEX_INCR_SP:
			m_sSUMITEM.fIncR_SP += fValue;
			break;

		case EMADDEX_INCR_AP:
			m_sSUMITEM.fIncR_HP += fValue;
			m_sSUMITEM.fIncR_MP += fValue;
			m_sSUMITEM.fIncR_SP += fValue;
			break;

		case EMADDEX_INCR_MOVE_SPEED:
			if (emSLOT != SLOT_VEHICLE)
				m_sSUMITEM.fIncR_MoveSpeed += sItemCustom.GETMOVESPEEDR();
			else if (m_emVehicle != EMVEHICLE_OFF)
				m_sSUMITEM.fIncR_MoveSpeed += m_fVehicleSpeedRate + sItemCustom.GETMOVESPEEDR();
			break;

		case EMADDEX_INCR_ATTACK_SPEED:
			m_sSUMITEM.fIncR_AtkSpeed += fValue;
			break;

		case EMADDEX_INCR_CRITICAL_RATE:
			m_sSUMITEM.fIncR_Critical += fValue;
			break;

		case EMADDEX_INCR_CRUSHING_BLOW:
			m_sSUMITEM.fIncR_CrushingBlow += fValue;
			break;

		case EMADDEX_INCR_IGNORE_BOSSDAMAGE:
			m_sSUMITEM.fIncR_Ignore_BossDamage += fValue;
			break;

		case EMADDEX_INCR_CRITICAL_EMERGENCYMOB:
			m_sSUMITEM.fIncR_Critical_EmergencyMob += fValue;
			break;

		case EMADDEX_INCR_CRITICAL_EMERGENCYBOSS:
			m_sSUMITEM.fIncR_Critical_EmergencyBoss += fValue;
			break;

		case EMADDEX_INCR_EXPADDEFFECT:
			m_sSUMITEM.fIncR_ExpAddEffect += fValue;
			break;

		case EMADDEX_INCR_DROPGENMONEY:
			m_sSUMITEM.fIncR_DropGenMoney += fValue;
			break;

		case EMADDEX_DECR_DAMAGE_MELEESKILL:
			m_sSUMITEM.fDecR_Damage_MeleeSkill += fValue;
			break;

		case EMADDEX_DECR_DAMAGE_RANGESKILL:
			m_sSUMITEM.fDecR_Damage_RangeSkill += fValue;
			break;

		case EMADDEX_DECR_DAMAGE_MAGICSKILL:
			m_sSUMITEM.fDecR_Damage_MagicSkill += fValue;
			break;

		case EMADDEX_INCR_ATKSPEED_SOLOPLAY:
			m_sSUMITEM.fIncR_AtkSpeed_SoloPlay += fValue;
			break;

		case EMADDEX_INCR_ATKSPEED_PARTYPLAY:
			m_sSUMITEM.fIncR_AtkSpeed_PartyPlay += fValue;
			break;

		case EMADDEX_INC_HP:
			m_sSUMITEM.fInc_HP += fValue;
			break;

		case EMADDEX_INC_MP:
			m_sSUMITEM.fInc_MP += fValue;
			break;

		case EMADDEX_INC_SP:
			m_sSUMITEM.fInc_SP += fValue; 
			break;

		case EMADDEX_INC_AP:
			m_sSUMITEM.fInc_HP += fValue;
			m_sSUMITEM.fInc_MP += fValue;
			m_sSUMITEM.fInc_SP += fValue;
			break;

		case EMADDEX_INC_MOVE_SPEED :
			if (emSLOT != SLOT_VEHICLE)
				m_sSUMITEM.fInc_MoveSpeed += sItemCustom.GETMOVESPEED_EXCEPTDISGUISE();
			else if	(m_emVehicle != EMVEHICLE_OFF)
				m_sSUMITEM.fInc_MoveSpeed += m_fVehicleSpeedVol + sItemCustom.GETMOVESPEED_EXCEPTDISGUISE();
			break;

		case EMADDEX_INC_ATTACK_SPEED:
			m_sSUMITEM.fInc_AtkSpeed += fValue;
			break;

		case EMADDEX_INC_CRITICAL_RATE:
			m_sSUMITEM.fInc_Critical += fValue;
			break;

		case EMADDEX_INC_CRUSHING_BLOW:
			m_sSUMITEM.fInc_CrushingBlow += fValue;
			break;

		case EMADDEX_BOSSFIXEDDAMAGE:
			m_sSUMITEM.nInc_BossFixedDamage += nValue;
			break;

		case EMADDEX_DEC_DAMAGE_EMERGENCYSTATE :
			m_sSUMITEM.fDecR_Damage_EmergencyState += fValue;
			break;

		case EMADDEX_INC_HP_SOLOPLAY :
			m_sSUMITEM.nInc_HP_SoloPlay += nValue;
			break;

		case EMADDEX_INC_HP_PARTYPLAY:
			m_sSUMITEM.nInc_HP_PartyPlay += nValue;
			break;
		};
	}

	m_sSUMITEM.fIncR_HP += (sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_RATE_RECOVERY_HP) + sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_RATE_RECOVERY_HP_MP_SP)) * fApplyValue;
	m_sSUMITEM.fIncR_MP += (sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_RATE_RECOVERY_MP) + sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_RATE_RECOVERY_HP_MP_SP)) * fApplyValue;
	m_sSUMITEM.fIncR_SP += (sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_RATE_RECOVERY_SP) + sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_RATE_RECOVERY_HP_MP_SP)) * fApplyValue;

	m_sSUMITEM.sResist.nElectric	+= static_cast<int>( ( sItem.sSuitOp.sResist.nElectric + sItemCustom.GETGRADE_RESIST_ELEC()  ) * fApplyValue );
	m_sSUMITEM.sResist.nFire		+= static_cast<int>( ( sItem.sSuitOp.sResist.nFire     + sItemCustom.GETGRADE_RESIST_FIRE()  ) * fApplyValue );
	m_sSUMITEM.sResist.nIce			+= static_cast<int>( ( sItem.sSuitOp.sResist.nIce      + sItemCustom.GETGRADE_RESIST_ICE()   ) * fApplyValue );
	m_sSUMITEM.sResist.nPoison		+= static_cast<int>( ( sItem.sSuitOp.sResist.nPoison   + sItemCustom.GETGRADE_RESIST_POISON()) * fApplyValue );
	m_sSUMITEM.sResist.nSpirit		+= static_cast<int>( ( sItem.sSuitOp.sResist.nSpirit   + sItemCustom.GETGRADE_RESIST_SPIRIT()) * fApplyValue );
	/* == �����ɼ� �ɷ�ġ ===================================================================== */

	fApplyValue = fDurability;

	if( !GLCONST_CHAR::nDurabilityApply[DURABILITY_APPLY_RANDOM_OPTION] ) fApplyValue = 1.f;

	GLPADATA glDamage;

	float fRATE  = sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_RATE_DAMAGE)  * fApplyValue;
	float fVALUE = sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_DAMAGE) * fApplyValue;

	if ( fRATE!=0.0f && (m_CHARINDEX != GLCI_NUM_ACTOR && m_wLevel != -1) )
	{
		glDamage.wLow  = GLCONST_CHAR::cCONSTCLASS[m_CHARINDEX].fLVLUP_AP * m_wLevel * (fRATE*0.01f);
		glDamage.wHigh = GLCONST_CHAR::cCONSTCLASS[m_CHARINDEX].fLVLUP_AP * m_wLevel * (fRATE*0.01f);
	}
	if ( fVALUE != 0.0f ) glDamage.VAR_PARAM(int(fVALUE));

	fRATE  = sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_RATE_DEFENCE)  * fApplyValue;;
	fVALUE = sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_DEFENCE) * fApplyValue;;

	int nTempDefense = 0;

	if ( fRATE!=0.0f || fVALUE!=0.0f ) nTempDefense = fVALUE + (GLCONST_CHAR::cCONSTCLASS[m_CHARINDEX].fLVLUP_DP * m_wLevel * (fRATE*0.01f));

	m_sSUMITEM.gdDamage += ( GLDWDATA(glDamage.wLow, glDamage.wHigh ));
	m_sSUMITEM.nDefense += static_cast<int>( nTempDefense );

	m_sSUMITEM.nHP += static_cast<int>( sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_HP) * fApplyValue );
	m_sSUMITEM.nMP += static_cast<int>( sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_MP) * fApplyValue );
	m_sSUMITEM.nSP += static_cast<int>( sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_SP) * fApplyValue );

	m_sSUMITEM.nPA += static_cast<int>( sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_MELEE) * fApplyValue );
	m_sSUMITEM.nSA += static_cast<int>( sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_RANGE) * fApplyValue );

	m_sSUMITEM.sStats.wPow	+= static_cast<int>( sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_POW) * fApplyValue );
	m_sSUMITEM.sStats.wStr	+= static_cast<int>( sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_STR) * fApplyValue );
	m_sSUMITEM.sStats.wSpi	+= static_cast<int>( sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_SPI) * fApplyValue );
	m_sSUMITEM.sStats.wDex	+= static_cast<int>( sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_DEX) * fApplyValue );
	m_sSUMITEM.sStats.wSta	+= static_cast<int>( sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_STA) * fApplyValue );

	m_sSUMITEM.nPotion_HP_Inc  += static_cast<int>( sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_POTION_HP) * fApplyValue );
	m_sSUMITEM.nPotion_MP_Inc  += static_cast<int>( sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_POTION_MP) * fApplyValue );
	m_sSUMITEM.nPotion_SP_Inc  += static_cast<int>( sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_POTION_SP) * fApplyValue );
	m_sSUMITEM.nReceive_CP_Inc += static_cast<int>( sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_KILL_CP)   * fApplyValue );

	m_sSUMITEM.nHitRate += static_cast<int>( sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_HIT)   * fApplyValue );
	m_sSUMITEM.nAvoidRate += static_cast<int>( sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_AVOID)   * fApplyValue );

	fRATE  = sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_RATE_DAMAGE) * fApplyValue;
	fVALUE = sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_DAMAGE) * fApplyValue;

	fVALUE += sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_MAGIC) * fApplyValue;

	if( fRATE != 0.0f || fVALUE != 0.0f )
	{
		wMaDAMAGE = WORD(wMaDAMAGE*(100.0f+fRATE)*0.01f);
		GLOGICEX::VARIATION( wMaDAMAGE, USHRT_MAX, int(fVALUE) );
	}

	wMaDAMAGE += nGradeDamage;
		
	m_sSUMITEM.nMA	+= static_cast<int>( wMaDAMAGE );

	float fTempValue = sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_RESIST);

	int iRegistRandom = static_cast<int>( fTempValue * fApplyValue );
	
	m_sSUMITEM.sResist.nElectric	+= iRegistRandom;
	m_sSUMITEM.sResist.nFire		+= iRegistRandom;
	m_sSUMITEM.sResist.nIce			+= iRegistRandom;
	m_sSUMITEM.sResist.nPoison		+= iRegistRandom;
	m_sSUMITEM.sResist.nSpirit		+= iRegistRandom; 

	// �������� ����ġ ���� ���ϱ�
	float fExpMultiple = sItem.GetExpMultiple();

	//������ ����
	if( (fExpMultiple > 1.0f) /*|| (fDurability < 1.0f) */)
	{
		float fValue = 1.0f;

		if( fDurability < 1.0f)
			fValue = fDurability;

		fExpMultiple = ( (fExpMultiple - 1.0f) * fValue ) + 1.0f;
	}

	if( fExpMultiple > 1.0f )
	{
		m_sSUMITEM.fExpMultiple += fExpMultiple - 1.0f;
		m_sSUMITEM.fExpMultiple = max( m_sSUMITEM.fExpMultiple, 1.0f );
	}

	for ( INT i = 0; i < SKILL::MAX_GRADE; ++i )
	{
		INT nDurIdx(RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_1_DURATIONTIME_ADD+i),
			nDamIdx(RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_1_DAMAGE_ADD+i),
			nDelIdx(RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_1_SKILLDILAY_DEC+i);

		m_sSUMITEM.fInc_SkillDurationTAdd[i] += sItemCustom.GETOptVALUE(static_cast<RandomOption::RANDOM_OPTION>(nDurIdx)) * fApplyValue;
		m_sSUMITEM.fIncR_SkillDamageAdd[i] += sItemCustom.GETOptVALUE(static_cast<RandomOption::RANDOM_OPTION>(nDamIdx)) * fApplyValue;
		m_sSUMITEM.fDecR_SkillDelayT[i] += sItemCustom.GETOptVALUE(static_cast<RandomOption::RANDOM_OPTION>(nDelIdx)) * fApplyValue;
	}

	m_sSUMITEM.fIncR_CriticalDamage += sItemCustom.GETOptVALUE( RandomOption::RANDOM_OPTION_VALUE_CRITICALDAMAGE_ADD ) * fApplyValue;
	m_sSUMITEM.fIncR_CrushingBlowDamage	+= sItemCustom.GETOptVALUE( RandomOption::RANDOM_OPTION_VALUE_CRUSHINGBLOW_ADD ) * fApplyValue;
	m_sSUMITEM.fIncR_LifePerKill	+= sItemCustom.GETOptVALUE( RandomOption::RANDOM_OPTION_VALUE_LIFE_PER_KILL ) * fApplyValue;
	m_sSUMITEM.fIncR_MPPerKill		+= sItemCustom.GETOptVALUE( RandomOption::RANDOM_OPTION_VALUE_MP_PER_KILL ) * fApplyValue;
	m_sSUMITEM.fIncR_SPPerKill			+= sItemCustom.GETOptVALUE( RandomOption::RANDOM_OPTION_VALUE_SP_PER_KILL ) * fApplyValue;
	m_sSUMITEM.fIncR_HPMPSPPerKill		+= sItemCustom.GETOptVALUE( RandomOption::RANDOM_OPTION_VALUE_HPMPSP_PER_KILL ) * fApplyValue;

	m_sSUMITEM.fIncR_AllMPRatioToAttack	+= sItemCustom.GETOptVALUE( RandomOption::RANDOM_OPTION_VALUE_ALLMPRATIO_TO_ATTACK ) * fApplyValue;
	m_sSUMITEM.fIncR_AllSPRatioToHp		+= sItemCustom.GETOptVALUE( RandomOption::RANDOM_OPTION_VALUE_ALLSPRATIO_TO_HP );
	m_sSUMITEM.fIncR_AllHitRatioToMelee	+= sItemCustom.GETOptVALUE( RandomOption::RANDOM_OPTION_VALUE_ALLHITRATIO_TO_MELEE ) * fApplyValue;
	m_sSUMITEM.fIncR_AllHitRatioToRange	+= sItemCustom.GETOptVALUE( RandomOption::RANDOM_OPTION_VALUE_ALLHITRATIO_TO_RANGE ) * fApplyValue;
	m_sSUMITEM.fIncR_AllHitRatioToMagic	+= sItemCustom.GETOptVALUE( RandomOption::RANDOM_OPTION_VALUE_ALLHITRATIO_TO_MAGIC ) * fApplyValue;

	return true;
//#else
//	return false;
//#endif
}

void GLCHARLOGIC::SUM_ITEM_BASIC ()
{
	for ( int i=0; i<SLOT_NSIZE_S_2; i++ )
	{
		EMSLOT emSLOT = static_cast<EMSLOT>(i);
		if ( !VALID_SLOT_ITEM(emSLOT) )
			continue;

		const SITEMCUSTOM& sItemCustom = GET_SLOT_ITEM ( emSLOT );
		SITEM &sItem = *m_pITEMS[emSLOT];

        // �������� ���� ���� ����
        float fDurabilityPerformance = 1.0f;   // �⺻������ 100%

        // ������ ��� ON & OFF
        if ( GLUseFeatures::GetInstance().IsUsingItemDurability() )
        {
            // ���� ����ϰ��ִ� ����� ����
            EMSUIT emSuit = SLOT_2_SUIT( emSLOT );
            if( emSuit < SUIT_NSIZE )
            {
                // ����������� ������� ����
                if(  ItemDurability::GetInstance().IsUse(emSuit) )
				{
                    fDurabilityPerformance = ItemDurability::GetInstance().GetPerformance( sItemCustom );
				}
            }
        }
		
		if( SUM_ITEM_BASIC_CS( sItemCustom, fDurabilityPerformance, emSLOT ) && sItemCustom.nidDISGUISE == NATIVEID_NULL() )
		{
			continue;
		}

		if( SUM_ITEM_BASIC_WEAPON( emSLOT, fDurabilityPerformance, sItemCustom ) ) continue;
				
		//	������ ���� ���ݿ��� ������ ���� ����. => ��ȹ�� ��û���� ����. 2012-08-01.
		/*if ( sItem.sBasicOp.emItemType==ITEM_CHARM )
			continue;*/
		// ����ȿ���� �������� ������ �ȵȴ� => ��ȹ�� Ȯ�� 2014-06-05
		for ( DWORD addon=0; addon<SITEMCUSTOM_ADDON::ADDON_SIZE; ++addon )
		{
			const DWORD dwType = static_cast< DWORD >( sItemCustom.sAddonInfo.m_sADDON[addon].emTYPE );

			// ����ɼ� ���� �����´�;
			const int nValue = static_cast< int >( sItemCustom.sAddonInfo.GetValue< int >( addon, ITEM::bAddonValueInteger[ dwType ] ) * fDurabilityPerformance );
			const WORD wValue = static_cast< WORD >( sItemCustom.sAddonInfo.GetValue< WORD >( addon, ITEM::bAddonValueInteger[ dwType ] ) * fDurabilityPerformance );
			const float fValue = static_cast< float >( sItemCustom.sAddonInfo.GetValue< float >( addon, ITEM::bAddonValueInteger[ dwType ] ) * fDurabilityPerformance );

			// ����ɼ��� �����Ѵ�;
			switch ( dwType )
			{
			case EMADD_NONE:
				break;

			case EMADD_HITRATE:
				m_sSUMITEM.nHitRate += nValue;
				break;

			case EMADD_AVOIDRATE:
				m_sSUMITEM.nAvoidRate += nValue;
				break;

			case EMADD_DAMAGE:
				m_sSUMITEM.gdDamage.nNow += nValue;
				m_sSUMITEM.gdDamage.nMax += nValue;
				break;

			case EMADD_DEFENSE:
				m_sSUMITEM.nDefense += nValue;
				break;

			case EMADD_HP:
				m_sSUMITEM.nHP += nValue;
				break;

			case EMADD_MP:
				m_sSUMITEM.nMP += nValue;
				break;

			case EMADD_SP:
				m_sSUMITEM.nSP += nValue;
				break;

			case EMADD_STATS_POW:
				m_sSUMITEM.sStats.wPow += wValue;
				break;

			case EMADD_STATS_STR:
				m_sSUMITEM.sStats.wStr += wValue;
				break;

			case EMADD_STATS_SPI:
				m_sSUMITEM.sStats.wSpi += wValue;
				break;

			case EMADD_STATS_DEX:
				m_sSUMITEM.sStats.wDex += wValue;
				break;

			case EMADD_STATS_INT:
				m_sSUMITEM.sStats.wInt += wValue;
				break;

			case EMADD_STATS_STA:
				m_sSUMITEM.sStats.wSta += wValue;
				break;

			case EMADD_PA:
				m_sSUMITEM.nPA += nValue;
				break;

			case EMADD_SA:
				m_sSUMITEM.nSA += nValue;
				break;
				//case EMADD_MA:	//	�� �ɼ��� GETMaDAMAGE() ���� ������ ���ѵ� ó��.
				//	m_sSUMITEM.nMA	+= sItemCustom.sAddonInfo.m_sADDON[addon].nVALUE;
				//	break;

			case EMADDEX_INCR_HP:
				m_sSUMITEM.fIncR_HP += fValue;
				break;

			case EMADDEX_INCR_MP:
				m_sSUMITEM.fIncR_MP += fValue;
				break;

			case EMADDEX_INCR_SP:
				m_sSUMITEM.fIncR_SP += fValue;
				break;

			case EMADDEX_INCR_AP:
				m_sSUMITEM.fIncR_HP += fValue;
				m_sSUMITEM.fIncR_MP += fValue;
				m_sSUMITEM.fIncR_SP += fValue;
				break;

			case EMADDEX_INCR_MOVE_SPEED:
				// ���߿� �ջ����� ����
				if (emSLOT != SLOT_VEHICLE)
					m_sSUMITEM.fIncR_MoveSpeed += sItemCustom.GETMOVESPEEDR() * fDurabilityPerformance;
				else if (m_emVehicle != EMVEHICLE_OFF)
					m_sSUMITEM.fIncR_MoveSpeed += m_fVehicleSpeedRate + sItemCustom.GETMOVESPEEDR() * fDurabilityPerformance;
				break;

			case EMADDEX_INCR_ATTACK_SPEED:
				m_sSUMITEM.fIncR_AtkSpeed += fValue;
				break;

			case EMADDEX_INCR_CRITICAL_RATE:
				m_sSUMITEM.fIncR_Critical += fValue;
				break;

			case EMADDEX_INCR_CRUSHING_BLOW:
				m_sSUMITEM.fIncR_CrushingBlow += fValue;
				break;

			case EMADDEX_INCR_IGNORE_BOSSDAMAGE: 
				m_sSUMITEM.fIncR_Ignore_BossDamage += fValue;
				break;

			case EMADDEX_INCR_CRITICAL_EMERGENCYMOB: 
				m_sSUMITEM.fIncR_Critical_EmergencyMob += fValue;
				break;

			case EMADDEX_INCR_CRITICAL_EMERGENCYBOSS:
				m_sSUMITEM.fIncR_Critical_EmergencyBoss += fValue;
				break;

			case EMADDEX_INCR_EXPADDEFFECT: 
				m_sSUMITEM.fIncR_ExpAddEffect += fValue;
				break;

			case EMADDEX_INCR_DROPGENMONEY:
				m_sSUMITEM.fIncR_DropGenMoney += fValue;
				break;

			case EMADDEX_DECR_DAMAGE_MELEESKILL:
				m_sSUMITEM.fDecR_Damage_MeleeSkill += fValue;
				break;

			case EMADDEX_DECR_DAMAGE_RANGESKILL:
				m_sSUMITEM.fDecR_Damage_RangeSkill += fValue;
				break;

			case EMADDEX_DECR_DAMAGE_MAGICSKILL:
				m_sSUMITEM.fDecR_Damage_MagicSkill += fValue;
				break;

			case EMADDEX_INCR_ATKSPEED_SOLOPLAY:
				m_sSUMITEM.fIncR_AtkSpeed_SoloPlay += fValue;
				break;

			case EMADDEX_INCR_ATKSPEED_PARTYPLAY :
				m_sSUMITEM.fIncR_AtkSpeed_PartyPlay += fValue;
				break;

			case EMADDEX_INC_HP:
				m_sSUMITEM.fInc_HP += fValue;
				break;

			case EMADDEX_INC_MP:
				m_sSUMITEM.fInc_MP += fValue;
				break;

			case EMADDEX_INC_SP:
				m_sSUMITEM.fInc_SP += fValue;
				break;

			case EMADDEX_INC_AP:
				m_sSUMITEM.fInc_HP += fValue;
				m_sSUMITEM.fInc_MP += fValue;
				m_sSUMITEM.fInc_SP += fValue;
				break;

			case EMADDEX_INC_MOVE_SPEED:
				// [shhan][2015.01.27] rm #763
				//						[MY]�ڽ�Ƭ �̵��ӵ� ȿ�� ������û
				//						SUM_ITEM_BASIC(), GETMOVESPEED() �� �ڽ�Ƭ ��ġ�� �ջ�ǰ�
				//						SUM_ITEM_COSTUM(), ���� sVOLUME ���� �� ���ؼ� ������ �ȴ�.
				//						SUM_ITEM_BASIC() �۾��� GETMOVESPEED_EXCEPTDISGUISE() ���� ����ϵ��� �Ѵ�.
				//
				// ���߿� �ջ����� ����
				if (emSLOT != SLOT_VEHICLE)
					m_sSUMITEM.fInc_MoveSpeed += sItemCustom.GETMOVESPEED_EXCEPTDISGUISE() * fDurabilityPerformance;
				else if	(m_emVehicle != EMVEHICLE_OFF)
					m_sSUMITEM.fInc_MoveSpeed += m_fVehicleSpeedVol + sItemCustom.GETMOVESPEED_EXCEPTDISGUISE() * fDurabilityPerformance;
				break;

			case EMADDEX_INC_ATTACK_SPEED:
				m_sSUMITEM.fInc_AtkSpeed += fValue;
				break;

			case EMADDEX_INC_CRITICAL_RATE:
				m_sSUMITEM.fInc_Critical += fValue;
				break;

			case EMADDEX_INC_CRUSHING_BLOW:
				m_sSUMITEM.fInc_CrushingBlow += fValue;
				break;

			case EMADDEX_BOSSFIXEDDAMAGE : 
				m_sSUMITEM.nInc_BossFixedDamage += nValue;
				break;

			case EMADDEX_DEC_DAMAGE_EMERGENCYSTATE : 
				m_sSUMITEM.fDecR_Damage_EmergencyState += fValue;
				break;

			case EMADDEX_INC_HP_SOLOPLAY :
				m_sSUMITEM.nInc_HP_SoloPlay += nValue;
				break;

			case EMADDEX_INC_HP_PARTYPLAY:
				m_sSUMITEM.nInc_HP_PartyPlay += nValue;
				break;
			};
		}

		m_sSUMITEM.nHP += static_cast<int>( sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_HP) * fDurabilityPerformance );
		m_sSUMITEM.nMP += static_cast<int>( sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_MP) * fDurabilityPerformance );
		m_sSUMITEM.nSP += static_cast<int>( sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_SP) * fDurabilityPerformance );

		m_sSUMITEM.nPA += static_cast<int>( sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_MELEE) * fDurabilityPerformance );
		m_sSUMITEM.nSA += static_cast<int>( sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_RANGE) * fDurabilityPerformance );

		m_sSUMITEM.sStats.wPow	+= static_cast<int>( sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_POW) * fDurabilityPerformance );
		m_sSUMITEM.sStats.wStr	+= static_cast<int>( sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_STR) * fDurabilityPerformance );
		m_sSUMITEM.sStats.wSpi	+= static_cast<int>( sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_SPI) * fDurabilityPerformance );
		m_sSUMITEM.sStats.wDex	+= static_cast<int>( sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_DEX) * fDurabilityPerformance );
		m_sSUMITEM.sStats.wSta	+= static_cast<int>( sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_STA) * fDurabilityPerformance );

		m_sSUMITEM.nPotion_HP_Inc  += static_cast<int>( sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_POTION_HP) * fDurabilityPerformance );
		m_sSUMITEM.nPotion_MP_Inc  += static_cast<int>( sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_POTION_MP) * fDurabilityPerformance );
		m_sSUMITEM.nPotion_SP_Inc  += static_cast<int>( sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_POTION_SP) * fDurabilityPerformance );
		m_sSUMITEM.nReceive_CP_Inc += static_cast<int>( sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_KILL_CP) * fDurabilityPerformance );
		
		m_sSUMITEM.fIncR_HP += (sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_RATE_RECOVERY_HP) + sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_RATE_RECOVERY_HP_MP_SP)) * fDurabilityPerformance;
		m_sSUMITEM.fIncR_MP += (sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_RATE_RECOVERY_MP) + sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_RATE_RECOVERY_HP_MP_SP)) * fDurabilityPerformance;
		m_sSUMITEM.fIncR_SP += (sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_RATE_RECOVERY_SP) + sItemCustom.GETOptVALUE(RandomOption::RANDOM_OPTION_RATE_RECOVERY_HP_MP_SP)) * fDurabilityPerformance;

		// �̵��ӵ� �����ɼ� ����( ������ ������ ���� m_fVehicleSpeedVol ) 
		//		if ( emSLOT != SLOT_VEHICLE ) m_sSUMITEM.fInc_MoveSpeed += sItemCustom.GETOptVALUE( RandomOption::RANDOM_OPTION_RATE_SPEED_MOVE );
		//		else if ( m_bVehicle ) m_sSUMITEM.fInc_MoveSpeed += sItemCustom.GETOptVALUE( RandomOption::RANDOM_OPTION_RATE_SPEED_MOVE );

		//	Note : ������ �⺻ �ɷ� ADD ��ġ.
		//
		m_sSUMITEM.gdDamage			+= (sItemCustom.GETDAMAGE(m_CHARINDEX, m_wLevel) * fDurabilityPerformance);
		m_sSUMITEM.nDefense			+= static_cast<int>( sItemCustom.GETDEFENSE(m_CHARINDEX, m_wLevel) * fDurabilityPerformance );

		m_sSUMITEM.nAvoidRate		+= static_cast<int>( sItemCustom.GETAVOIDRATE() * fDurabilityPerformance );
		m_sSUMITEM.nHitRate			+= static_cast<int>( sItemCustom.GETHITRATE() * fDurabilityPerformance );

		
		// Note : ������ ���� [+] ��ġ �ջ�.
		// 
		m_sSUMITEM.sResist.nElectric	+= static_cast<int>( sItemCustom.GETRESIST_ELEC() * fDurabilityPerformance );
		m_sSUMITEM.sResist.nFire		+= static_cast<int>( sItemCustom.GETRESIST_FIRE() * fDurabilityPerformance );
		m_sSUMITEM.sResist.nIce			+= static_cast<int>( sItemCustom.GETRESIST_ICE() * fDurabilityPerformance );
		m_sSUMITEM.sResist.nPoison		+= static_cast<int>( sItemCustom.GETRESIST_POISON() * fDurabilityPerformance );
		m_sSUMITEM.sResist.nSpirit		+= static_cast<int>( sItemCustom.GETRESIST_SPIRIT() * fDurabilityPerformance ); 
			
		//	Note : ���ġ�� ���ݷ� ����.
		//
		m_sSUMITEM.nMA	+= static_cast<int>( sItemCustom.GETMaDAMAGE() * fDurabilityPerformance );

		// �������� ����ġ ���� ���ϱ�
		float fExpMultiple = sItem.GetExpMultiple();

		//������ ����
		if( (fExpMultiple > 1.0f) || (fDurabilityPerformance < 1.0f ) || (fDurabilityPerformance > 1.0f ))
		{
			fExpMultiple = ( (fExpMultiple - 1.0f) * fDurabilityPerformance ) + 1.0f;
		}

		if( fExpMultiple > 1.0f )
		{
			m_sSUMITEM.fExpMultiple += fExpMultiple - 1.0f;
			m_sSUMITEM.fExpMultiple = max( m_sSUMITEM.fExpMultiple, 1.0f );
		}

		for ( INT i = 0; i < SKILL::MAX_GRADE; ++i )
		{
			INT nDurIdx(RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_1_DURATIONTIME_ADD+i),
				nDamIdx(RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_1_DAMAGE_ADD+i),
				nDelIdx(RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_1_SKILLDILAY_DEC+i);

			m_sSUMITEM.fInc_SkillDurationTAdd[i] += sItemCustom.GETOptVALUE(static_cast<RandomOption::RANDOM_OPTION>(nDurIdx)) * fDurabilityPerformance;
			m_sSUMITEM.fIncR_SkillDamageAdd[i] += sItemCustom.GETOptVALUE(static_cast<RandomOption::RANDOM_OPTION>(nDamIdx)) * fDurabilityPerformance;
			m_sSUMITEM.fDecR_SkillDelayT[i] += sItemCustom.GETOptVALUE(static_cast<RandomOption::RANDOM_OPTION>(nDelIdx)) * fDurabilityPerformance;
		}

		m_sSUMITEM.fIncR_CriticalDamage += sItemCustom.GETOptVALUE( RandomOption::RANDOM_OPTION_VALUE_CRITICALDAMAGE_ADD ) * fDurabilityPerformance;
		m_sSUMITEM.fIncR_CrushingBlowDamage	+= sItemCustom.GETOptVALUE( RandomOption::RANDOM_OPTION_VALUE_CRUSHINGBLOW_ADD ) * fDurabilityPerformance;
		m_sSUMITEM.fIncR_LifePerKill	+= sItemCustom.GETOptVALUE( RandomOption::RANDOM_OPTION_VALUE_LIFE_PER_KILL ) * fDurabilityPerformance;
		m_sSUMITEM.fIncR_MPPerKill		+= sItemCustom.GETOptVALUE( RandomOption::RANDOM_OPTION_VALUE_MP_PER_KILL ) * fDurabilityPerformance;
		m_sSUMITEM.fIncR_SPPerKill			+= sItemCustom.GETOptVALUE( RandomOption::RANDOM_OPTION_VALUE_SP_PER_KILL ) * fDurabilityPerformance;
		m_sSUMITEM.fIncR_HPMPSPPerKill		+= sItemCustom.GETOptVALUE( RandomOption::RANDOM_OPTION_VALUE_HPMPSP_PER_KILL ) * fDurabilityPerformance;

		m_sSUMITEM.fIncR_AllMPRatioToAttack	+= sItemCustom.GETOptVALUE( RandomOption::RANDOM_OPTION_VALUE_ALLMPRATIO_TO_ATTACK ) * fDurabilityPerformance;
		m_sSUMITEM.fIncR_AllSPRatioToHp		+= sItemCustom.GETOptVALUE( RandomOption::RANDOM_OPTION_VALUE_ALLSPRATIO_TO_HP );
		m_sSUMITEM.fIncR_AllHitRatioToMelee	+= sItemCustom.GETOptVALUE( RandomOption::RANDOM_OPTION_VALUE_ALLHITRATIO_TO_MELEE ) * fDurabilityPerformance;
		m_sSUMITEM.fIncR_AllHitRatioToRange	+= sItemCustom.GETOptVALUE( RandomOption::RANDOM_OPTION_VALUE_ALLHITRATIO_TO_RANGE ) * fDurabilityPerformance;
		m_sSUMITEM.fIncR_AllHitRatioToMagic	+= sItemCustom.GETOptVALUE( RandomOption::RANDOM_OPTION_VALUE_ALLHITRATIO_TO_MAGIC ) * fDurabilityPerformance;
	}
}

void GLCHARLOGIC::SUM_ITEM_COSTUM ()
{
	for ( int i=0; i<SLOT_NSIZE_S_2; i++ )
	{
		EMSLOT emSLOT = static_cast<EMSLOT>(i);
		if ( !VALID_SLOT_ITEM(emSLOT) )
			continue;

		const SITEMCUSTOM& sItemCustom = GET_SLOT_ITEM ( emSLOT );
		if ( sItemCustom.nidDISGUISE == NATIVEID_NULL() )
			continue;

		if( sItemCustom.IsCSApplied() ) continue;
        
        //if ( pItem && pItem->sBasicOp.IsDISGUISE() )
        const SITEM* pItem = GLogicData::GetInstance().GetItem(sItemCustom.nidDISGUISE);
        if ( !pItem )
            continue;

        SITEMCUSTOM sCustomeItem(sItemCustom.nidDISGUISE);
        if (sCustomeItem.IsDisguise()==false)
            continue;

        // ��ȹ ����( ����� : ���ͼ�, ��õ�� ) 140619 cwback
        // �ڽ�Ƭ�� ��ġ�� �������� �ʴ´�.

		for ( DWORD addon=0; addon<SITEMCUSTOM_ADDON::ADDON_SIZE; ++addon )
		{
			const DWORD dwType = static_cast< DWORD >( sCustomeItem.sAddonInfo.m_sADDON[addon].emTYPE );

			// ����ɼ� ���� �����´�;
			const int nValue = static_cast< int >( sCustomeItem.sAddonInfo.GetValue< int >( addon, ITEM::bAddonValueInteger[ dwType ] ) );
			const WORD wValue = static_cast< WORD >( sCustomeItem.sAddonInfo.GetValue< WORD >( addon, ITEM::bAddonValueInteger[ dwType ] ) );
			const float fValue = static_cast< float >( sCustomeItem.sAddonInfo.GetValue< float >( addon, ITEM::bAddonValueInteger[ dwType ] ) );

			// ����ɼ��� �����Ѵ�;
			switch ( dwType )
			{
			case EMADD_NONE:
				break;

			case EMADD_HITRATE:
				m_sSUMITEM.nHitRate += nValue;
				break;

			case EMADD_AVOIDRATE:
				m_sSUMITEM.nAvoidRate += nValue;
				break;

			case EMADD_DAMAGE:
				m_sSUMITEM.gdDamage.nNow += nValue;
				m_sSUMITEM.gdDamage.nMax += nValue;
				break;

			case EMADD_DEFENSE:
				m_sSUMITEM.nDefense += nValue;
				break;

			case EMADD_HP:
				m_sSUMITEM.nHP += nValue;
				break;

			case EMADD_MP:
				m_sSUMITEM.nMP += nValue;
				break;

			case EMADD_SP:
				m_sSUMITEM.nSP += nValue;
				break;

			case EMADD_STATS_POW:
				m_sSUMITEM.sStats.wPow += wValue;
				break;

			case EMADD_STATS_STR:
				m_sSUMITEM.sStats.wStr += wValue;
				break;

			case EMADD_STATS_SPI:
				m_sSUMITEM.sStats.wSpi += wValue;
				break;

			case EMADD_STATS_DEX:
				m_sSUMITEM.sStats.wDex += wValue;
				break;

			case EMADD_STATS_INT:
				m_sSUMITEM.sStats.wInt += wValue;
				break;

			case EMADD_STATS_STA:
				m_sSUMITEM.sStats.wSta += wValue;
				break;

			case EMADD_PA:
				m_sSUMITEM.nPA += nValue;
				break;

			case EMADD_SA:
				m_sSUMITEM.nSA += nValue;
				break;

			case EMADD_MA:	
				m_sSUMITEM.nMA += nValue;
				break;

			case EMADDEX_INCR_HP:
				m_sSUMITEM.fIncR_HP += fValue;
				break;

			case EMADDEX_INCR_MP:
				m_sSUMITEM.fIncR_MP += fValue;
				break;

			case EMADDEX_INCR_SP:
				m_sSUMITEM.fIncR_SP += fValue;
				break;

			case EMADDEX_INCR_AP:
				m_sSUMITEM.fIncR_HP += fValue;
				m_sSUMITEM.fIncR_MP += fValue;
				m_sSUMITEM.fIncR_SP += fValue;
				break;

			case EMADDEX_INCR_MOVE_SPEED:
				m_sSUMITEM.fIncR_MoveSpeed += fValue;
				break;

			case EMADDEX_INCR_ATTACK_SPEED:
				m_sSUMITEM.fIncR_AtkSpeed += fValue;
				break;

			case EMADDEX_INCR_CRITICAL_RATE:
				m_sSUMITEM.fIncR_Critical += fValue;
				break;

			case EMADDEX_INCR_CRUSHING_BLOW:
				m_sSUMITEM.fIncR_CrushingBlow += fValue;
				break;

			case EMADDEX_INCR_IGNORE_BOSSDAMAGE: 
				m_sSUMITEM.fIncR_Ignore_BossDamage += fValue;
				break;

			case EMADDEX_INCR_CRITICAL_EMERGENCYMOB: 
				m_sSUMITEM.fIncR_Critical_EmergencyMob += fValue;
				break;

			case EMADDEX_INCR_CRITICAL_EMERGENCYBOSS:
				m_sSUMITEM.fIncR_Critical_EmergencyBoss += fValue;
				break;

			case EMADDEX_INCR_EXPADDEFFECT: 
				m_sSUMITEM.fIncR_ExpAddEffect += fValue;
				break;

			case EMADDEX_INCR_DROPGENMONEY:
				m_sSUMITEM.fIncR_DropGenMoney += fValue;
				break;

			case EMADDEX_DECR_DAMAGE_MELEESKILL:
				m_sSUMITEM.fDecR_Damage_MeleeSkill += fValue;
				break;

			case EMADDEX_DECR_DAMAGE_RANGESKILL:
				m_sSUMITEM.fDecR_Damage_RangeSkill += fValue;
				break;

			case EMADDEX_DECR_DAMAGE_MAGICSKILL:
				m_sSUMITEM.fDecR_Damage_MagicSkill += fValue;
				break;

			case EMADDEX_INCR_ATKSPEED_SOLOPLAY:
				m_sSUMITEM.fIncR_AtkSpeed_SoloPlay += fValue;
				break;

			case EMADDEX_INCR_ATKSPEED_PARTYPLAY :
				m_sSUMITEM.fIncR_AtkSpeed_PartyPlay += fValue;
				break;

			case EMADDEX_INC_HP:
				m_sSUMITEM.fInc_HP += fValue;
				break;

			case EMADDEX_INC_MP:
				m_sSUMITEM.fInc_MP += fValue;
				break;

			case EMADDEX_INC_SP:
				m_sSUMITEM.fInc_SP += fValue;
				break;

			case EMADDEX_INC_AP:
				m_sSUMITEM.fInc_HP += fValue;
				m_sSUMITEM.fInc_MP += fValue;
				m_sSUMITEM.fInc_SP += fValue;
				break;

			case EMADDEX_INC_MOVE_SPEED:
				// ���߿� �ջ����� ����
				m_sSUMITEM.fInc_MoveSpeed += fValue;
				break;

			case EMADDEX_INC_ATTACK_SPEED:
				m_sSUMITEM.fInc_AtkSpeed += fValue;
				break;

			case EMADDEX_INC_CRITICAL_RATE:
				m_sSUMITEM.fInc_Critical += fValue;
				break;

			case EMADDEX_INC_CRUSHING_BLOW:
				m_sSUMITEM.fInc_CrushingBlow += fValue;
				break;

			case EMADDEX_BOSSFIXEDDAMAGE : 
				m_sSUMITEM.nInc_BossFixedDamage += nValue;
				break;

			case EMADDEX_DEC_DAMAGE_EMERGENCYSTATE : 
				m_sSUMITEM.fDecR_Damage_EmergencyState += fValue;
				break;

			case EMADDEX_INC_HP_SOLOPLAY :
				m_sSUMITEM.nInc_HP_SoloPlay += nValue;
				break;

			case EMADDEX_INC_HP_PARTYPLAY:
				m_sSUMITEM.nInc_HP_PartyPlay += nValue;
				break;
			};
		}

        {
            //! �ڽ�Ƭ�� ����� ���� ��ġ�� �ջ�ǵ��� ���� �Ǿ���. - 2012.11.06 cwBack;
            //! ��ȹ�� [ ������ ���� ���� �߰����� ] - 2012.11.01
            m_sSUMITEM.sResist.nElectric	+= static_cast<int>( pItem->sSuitOp.sResist.nElectric );
            m_sSUMITEM.sResist.nFire		+= static_cast<int>( pItem->sSuitOp.sResist.nFire );
            m_sSUMITEM.sResist.nIce			+= static_cast<int>( pItem->sSuitOp.sResist.nIce );
            m_sSUMITEM.sResist.nPoison		+= static_cast<int>( pItem->sSuitOp.sResist.nPoison );
            m_sSUMITEM.sResist.nSpirit		+= static_cast<int>( pItem->sSuitOp.sResist.nSpirit );
        }
        

		// �������� ����ġ ���� ���ϱ�
		float fExpMultiple = pItem->GetExpMultiple();
		if( fExpMultiple > 1.0f )
		{
			m_sSUMITEM.fExpMultiple += fExpMultiple - 1.0f;
			m_sSUMITEM.fExpMultiple = max( m_sSUMITEM.fExpMultiple, 1.0f );
		}
	}
}

void GLCHARLOGIC::SUM_ITEM_SET ()
{
	if ( !GLUseFeatures::GetInstance().IsUsingSetItemUse() ) return;

	//��Ʈ������ �˻�
	GLItemSet::GetInstance().SearchSetItem(m_vInfoPutOnSetItem, m_PutOnItems, SLOT_TSIZE, m_bUseArmSub );
}

void GLCHARLOGIC::SUM_PASSIVE_BASIC( PGLSKILL pSkill, WORD wLev )
{
	GASSERT( pSkill );

	const SKILL::CDATA_LVL &sDATA_LVL = pSkill->m_sAPPLY.sDATA_LVL[wLev];
	switch( pSkill->m_sAPPLY.emBASIC_TYPE )
	{
	case SKILL::EMFOR_HP:
		m_sSUM_PASSIVE.m_nHP += int( sDATA_LVL.fBASIC_VAR );
		break;

	case SKILL::EMFOR_MP:
		m_sSUM_PASSIVE.m_nMP += WORD ( sDATA_LVL.fBASIC_VAR );
		break;

	case SKILL::EMFOR_SP:
		m_sSUM_PASSIVE.m_nSP += WORD ( sDATA_LVL.fBASIC_VAR );
		break;

	case SKILL::EMFOR_VARHP:
		m_sSUM_PASSIVE.m_fINCR_HP += sDATA_LVL.fBASIC_VAR;
		break;

	case SKILL::EMFOR_VARMP:
		m_sSUM_PASSIVE.m_fINCR_MP += sDATA_LVL.fBASIC_VAR;
		break;

	case SKILL::EMFOR_VARSP:
		m_sSUM_PASSIVE.m_fINCR_SP += sDATA_LVL.fBASIC_VAR;
		break;

	case SKILL::EMFOR_DEFENSE:
		m_sSUM_PASSIVE.m_nDEFENSE += int ( sDATA_LVL.fBASIC_VAR );
		break;

	case SKILL::EMFOR_HITRATE:
		m_sSUM_PASSIVE.m_nHIT += int ( sDATA_LVL.fBASIC_VAR );
		break;

	case SKILL::EMFOR_AVOIDRATE:
		m_sSUM_PASSIVE.m_nAVOID += int ( sDATA_LVL.fBASIC_VAR );
		break;

	case SKILL::EMFOR_VARAP:
		m_sSUM_PASSIVE.m_fINCR_HP += sDATA_LVL.fBASIC_VAR;
		m_sSUM_PASSIVE.m_fINCR_MP += sDATA_LVL.fBASIC_VAR;
		m_sSUM_PASSIVE.m_fINCR_SP += sDATA_LVL.fBASIC_VAR;
		break;

	case SKILL::EMFOR_VARDAMAGE:
		m_sSUM_PASSIVE.m_nDAMAGE += int(sDATA_LVL.fBASIC_VAR);
		break;

	case SKILL::EMFOR_VARDEFENSE:
		m_sSUM_PASSIVE.m_nDEFENSE += int(sDATA_LVL.fBASIC_VAR);
		break;

	case SKILL::EMFOR_PA:
		m_sSUM_PASSIVE.m_nPA += int(sDATA_LVL.fBASIC_VAR);
		break;

	case SKILL::EMFOR_SA:
		m_sSUM_PASSIVE.m_nSA += int(sDATA_LVL.fBASIC_VAR);
		break;

	case SKILL::EMFOR_MA:
		m_sSUM_PASSIVE.m_nMA += int(sDATA_LVL.fBASIC_VAR);
		break;

	case SKILL::EMFOR_HP_RATE:
		m_sSUM_PASSIVE.m_fHP_RATE += sDATA_LVL.fBASIC_VAR;
		break;

	case SKILL::EMFOR_MP_RATE:
		m_sSUM_PASSIVE.m_fMP_RATE += sDATA_LVL.fBASIC_VAR;
		break;

	case SKILL::EMFOR_SP_RATE:
		m_sSUM_PASSIVE.m_fSP_RATE += sDATA_LVL.fBASIC_VAR;
		break;

	case SKILL::EMFOR_RESIST:
		m_sSUM_PASSIVE.m_sSUMRESIST += (int)(sDATA_LVL.fBASIC_VAR);
		break;

	case SKILL::EMFOR_SUMMON_ACTIVETIME:
		m_sSUM_PASSIVE.m_fSUMMONACTIVETIME += sDATA_LVL.fBASIC_VAR;
		break;

	case SKILL::EMFOR_SUMMON_HIRE:
		// ��ȯ �нú갡 �־�߸� ���� ��ȯ���� �̿��� �� �ִ�;
		m_sSummonable.bSummonable = true;
		m_sSummonable.dwSummonLevel = wLev;
		SummonStateVisibleUpdate();
		break;
	};

} //GLCHARLOGIC::SUM_PASSIVE_BASIC

void GLCHARLOGIC::SUM_PASSIVE_ADDON( PGLSKILL pSkill, WORD wLev )
{
	GASSERT( pSkill );

	for ( DWORD i = 0; i <pSkill->m_sAPPLY.vecADDON.size(); ++i )
	{
		const float &fADDON = pSkill->m_sAPPLY.vecADDON[i].fADDON_VAR[wLev];
		switch ( pSkill->m_sAPPLY.vecADDON[i].emADDON )
		{
		case EMIMPACTA_HITRATE:
			m_sSUM_PASSIVE.m_nHIT += int ( fADDON );
			break;
		case EMIMPACTA_AVOIDRATE:
			m_sSUM_PASSIVE.m_nAVOID += int ( fADDON );
			break;

		case EMIMPACTA_DAMAGE:
			m_sSUM_PASSIVE.m_nDAMAGE += int ( fADDON );
			break;

		case EMIMPACTA_DEFENSE:
			m_sSUM_PASSIVE.m_nDEFENSE += int ( fADDON );
			break;

		case EMIMPACTA_VARHP:
			m_sSUM_PASSIVE.m_fINCR_HP += fADDON;
			break;
		case EMIMPACTA_VARMP:
			m_sSUM_PASSIVE.m_fINCR_MP += fADDON;
			break;
		case EMIMPACTA_VARSP:
			m_sSUM_PASSIVE.m_fINCR_SP += fADDON;
			break;

		case EMIMPACTA_VARAP:
			m_sSUM_PASSIVE.m_fINCR_HP += fADDON;
			m_sSUM_PASSIVE.m_fINCR_MP += fADDON;
			m_sSUM_PASSIVE.m_fINCR_SP += fADDON;
			break;

		case EMIMPACTA_DAMAGE_RATE:
			m_sSUM_PASSIVE.m_fDAMAGE_RATE += fADDON;
			break;

		case EMIMPACTA_DEFENSE_RATE:
			m_sSUM_PASSIVE.m_fDEFENSE_RATE += fADDON;
			break;

		case EMIMPACTA_PA:
			m_sSUM_PASSIVE.m_nPA += int(fADDON);
			break;

		case EMIMPACTA_SA:
			m_sSUM_PASSIVE.m_nSA += int(fADDON);
			break;

		case EMIMPACTA_MA:
			m_sSUM_PASSIVE.m_nMA += int(fADDON);
			break;

		case EMIMPACTA_HP_RATE:
			m_sSUM_PASSIVE.m_fHP_RATE += fADDON;
			break;

		case EMIMPACTA_MP_RATE:
			m_sSUM_PASSIVE.m_fMP_RATE += fADDON;
			break;

		case EMIMPACTA_SP_RATE:
			m_sSUM_PASSIVE.m_fSP_RATE += fADDON;
			break;

		case EMIMPACTA_RESIST:
			m_sSUM_PASSIVE.m_sSUMRESIST += (int)fADDON;
			break;
		case EMIMPACTA_STATS:
			break;
		case EMIMPACTA_CP_ADD:
			m_sSUM_PASSIVE.m_nCP_ADD += (int)fADDON;
			break;
		case EMIMPACTA_CP_DUR:
			m_sSUM_PASSIVE.m_fDurationCPAdd += fADDON;
			break;
		};		
	}
} //GLCHARLOGIC::SUM_PASSIVE_ADDON

void GLCHARLOGIC::SUM_PASSIVE_SPEC( PGLSKILL pSkill, WORD wLev )
{
	GASSERT( pSkill );

	for ( DWORD i = 0; i <pSkill->m_sAPPLY.vecSPEC.size(); ++i )
	{
		const SKILL::SSPEC &sSPEC = pSkill->m_sAPPLY.vecSPEC[i].sSPEC[wLev];
		switch ( pSkill->m_sAPPLY.vecSPEC[i].emSPEC )
		{
		case EMSPECA_PIERCE:
			m_sSUM_PASSIVE.m_nPIERCE += int ( sSPEC.fVAR1 );
			break;

		case EMSPECA_RANGE_PSY_LONG:
			m_sSUM_PASSIVE.m_fRANGE_PSY_LONG += sSPEC.fVAR1;
			break;

		case EMSPECA_MOVEVELO:
			m_sSUM_PASSIVE.m_fMOVEVELO += sSPEC.fVAR1;
			break;

		case EMSPECA_ATTACKVELO:
			//	���� �ӵ� ����̹Ƿ� ( ���� �ð��� -0.1 (-10%) ���� ��Ű�� ���ؼ��� ��ȣ ������ �Ͽ�����. )
			m_sSUM_PASSIVE.m_fATTVELO -= sSPEC.fVAR1;
			break;

		case EMSPECA_SKILLDELAY:
            if ( CHECK_FLAG_SKILL(sSPEC.dwFLAG, EMSPEC_SKILL_ASING_AFTER))
            {
                if ( sc::Random::RANDOM_POS()*100.0f < sSPEC.fVAR2 )
                    m_sSUM_PASSIVE.m_fSKILLDELAY += sSPEC.fVAR1;
            }
            break;

		case EMSPECA_PSY_DAMAGE_REDUCE:
			if ( m_sSUM_PASSIVE.m_sDamageSpec.m_fPsyDamageReduce < sSPEC.fVAR1 )
				m_sSUM_PASSIVE.m_sDamageSpec.m_fPsyDamageReduce = sSPEC.fVAR1;
			break;

		case EMSPECA_MAGIC_DAMAGE_REDUCE:
			if ( m_sSUM_PASSIVE.m_sDamageSpec.m_fMagicDamageReduce < sSPEC.fVAR1 )
				m_sSUM_PASSIVE.m_sDamageSpec.m_fMagicDamageReduce = sSPEC.fVAR1;
			break;

		case EMSPECA_PSY_DAMAGE_REFLECTION:
			if ( m_sSUM_PASSIVE.m_sDamageSpec.m_fPsyDamageReflection < sSPEC.fVAR1 )
			{
				m_sSUM_PASSIVE.m_sDamageSpec.m_fPsyDamageReflection = sSPEC.fVAR1;
				m_sSUM_PASSIVE.m_sDamageSpec.m_fPsyDamageReflectionRate = sSPEC.fVAR2;
			}
			break;

		case EMSPECA_MAGIC_DAMAGE_REFLECTION:
			if ( m_sSUM_PASSIVE.m_sDamageSpec.m_fMagicDamageReflection < sSPEC.fVAR1 )
			{
				m_sSUM_PASSIVE.m_sDamageSpec.m_fMagicDamageReflection = sSPEC.fVAR1;
				m_sSUM_PASSIVE.m_sDamageSpec.m_fMagicDamageReflectionRate = sSPEC.fVAR2;
			}
			break;
		case EMSPECA_TARRANGE_ALL:
			{
				m_sSUM_PASSIVE.m_fTARRANGE_ALL += sSPEC.fVAR1;
			}
			break;
		case EMSPECA_APPLYRANGE_ALL:
			{
				m_sSUM_PASSIVE.m_fAPPLYRANGE_ALL += sSPEC.fVAR1;
			}
			break;	
		case EMSPECA_DEFENSE_SKILL_ACTIVE:
		case EMSPECA_DURATION_DAMAGE:
		case EMSPECA_CURSE:
			break;
		};
	}
} //GLCHARLOGIC::SUM_PASSIVE_SPEC

void GLCHARLOGIC::RESET_PASSIVE ()
{
	m_sSummonable.RESET();
	SummonStateVisibleUpdate();
}

void GLCHARLOGIC::SUM_PASSIVE ()
{
	m_sSUM_PASSIVE = SPASSIVE_SKILL_DATA();
	
//	if (m_emVehicle != EMVEHICLE_OFF)
//      return;

	if ( m_ExpSkills.empty() )
        return;

	SCHARSKILL sTempCharSkill;

	for( SKILL_MAP_ITER iter = m_ExpSkills.begin(); iter != m_ExpSkills.end(); ++iter )
	{
		sTempCharSkill = SCHARSKILL(NATIVEID_NULL(), 0 );

		const SCHARSKILL	&sCharSkill = (*iter).second;

		// ��� ��ų�� ��ȿ������ �˻��Ѵ�;
		const EMSKILL_VALIDATION emSkillValidation = VALIDATION_EXPSKILL( sCharSkill.sNativeID.dwID );
		if ( EMSKILL_VALIDATION_OK != emSkillValidation )
		{
			continue;
		}

		CHANGE_CHARSKILL( sCharSkill, sTempCharSkill );

		PGLSKILL			pSkill;
		//for( SNATIVEID sSkillID	= sCharSkill.sNativeID; 
		//	sSkillID != NATIVEID_NULL(); 
		//	sSkillID = pSkill->m_sEXT_DATA.idMultiTargetSkill )
		for( SNATIVEID sSkillID	= sTempCharSkill.sNativeID; 
			 sSkillID != NATIVEID_NULL(); 
			 sSkillID = pSkill->m_sEXT_DATA.idMultiTargetSkill )
		{
			pSkill = GLSkillMan::GetInstance().GetData( sSkillID.wMainID, sSkillID.wSubID );
			if( !pSkill )
				break;

			if( pSkill->m_sBASIC.emROLE != SKILL::EMROLE_PASSIVE )
				continue;

			//	Note : ������ ���� ���� �˻�.
			//
			bool				bvalid_left		= true;
			bool				bvalid_right	= true;
			const bool			bHiddenWeapon	= pSkill->m_sLEARN.bHiddenWeapon;
			GLITEM_ATT			emITEM_LEFT		= ITEMATT_NOTHING;
			GLITEM_ATT			emITEM_RIGHT	= ITEMATT_NOTHING;
			SKILL::GLSKILL_ATT	emSKILL_LEFT	= pSkill->m_sBASIC.emUSE_LITEM;
			SKILL::GLSKILL_ATT	emSKILL_RIGHT	= pSkill->m_sBASIC.emUSE_RITEM;
			EMSLOT				emRHand			= GetCurRHand();
			EMSLOT				emLHand			= GetCurLHand();

			if ( emSKILL_LEFT!=SKILL::SKILLATT_NOCARE )
			{
				bvalid_left = false;
				SITEM* pItem = GET_SLOT_ITEMDATA(emLHand);
				if ( pItem )
				{
					emITEM_LEFT = pItem->sSuitOp.emAttack;
					bvalid_left = CHECHSKILL_ITEM(emSKILL_LEFT,CONVERT_ITEMATT( emITEM_LEFT ),bHiddenWeapon);
				}
			}

			if ( emSKILL_RIGHT!=SKILL::SKILLATT_NOCARE )
			{
				bvalid_right = false;
				SITEM* pItem = GET_SLOT_ITEMDATA(emRHand);
				if ( pItem )
				{
					emITEM_RIGHT = pItem->sSuitOp.emAttack;
					bvalid_right = CHECHSKILL_ITEM(emSKILL_RIGHT,CONVERT_ITEMATT( emITEM_RIGHT ),bHiddenWeapon);
				}
			}

			if ( !(bvalid_left&&bvalid_right) )
				continue;

			SUM_PASSIVE_BASIC( pSkill, sTempCharSkill.wLevel );
			SUM_PASSIVE_ADDON( pSkill, sTempCharSkill.wLevel );
			SUM_PASSIVE_SPEC( pSkill, sTempCharSkill.wLevel );
		} //while
	}
}

void GLCHARLOGIC::INIT_RECOVER ( int nRECOVER )
{
	m_sHP.CHECKMIN ( (m_sHP.nMax * nRECOVER) / 100 + 1 );
	m_sMP.CHECKMIN ( (m_sMP.nMax * nRECOVER) / 100 + 1 );
	m_sSP.CHECKMIN ( (m_sSP.nMax * nRECOVER) / 100 + 1 );

	// CP �ʱ�ȭ
//	m_sCP.SetData( 0, MAX_CP );

	for ( unsigned int _i(EMBLOW_MULTI); _i--; )
		DISABLEBLOW ( _i );

	for ( unsigned int _i(SKILLFACT_INDEX_NORMAL_START); _i<SKILLFACT_INDEX_NORMAL_END; ++_i )
		RESETSKEFF  ( _i );
	
	for ( unsigned int _i(MAX_PETSKILLSLOT); _i--; )
		m_sPETSKILLFACT[_i].RESET ();
}

void GLCHARLOGIC::RELEASE_SLOT_ITEM ( EMSLOT _slot )
{
	m_PutOnItems[_slot] = SITEMCUSTOM ( SNATIVEID(false) );

	//	Note : ������ ������ �ʱ�ȭ.
	//
	m_pITEMS[_slot] = NULL;
}

void GLCHARLOGIC::SLOT_ITEM ( const SITEMCUSTOM &sItemCustom, EMSLOT _slot )
{
	m_PutOnItems[_slot] = sItemCustom;

	//	Note : ������ ������ �ʱ�ȭ.
	//
	if ( VALID_SLOT_ITEM(_slot) )
		m_pITEMS[_slot] =  GLogicData::GetInstance().GetItem ( m_PutOnItems[_slot].GetNativeID() );
}

void GLCHARLOGIC::HOLD_ITEM( const SITEMCUSTOM& sItemCustom )
{
	m_PutOnItems[SLOT_HOLD] = sItemCustom;

	//	Note : ������ ������ �ʱ�ȭ.
	//
	if ( VALID_SLOT_ITEM( SLOT_HOLD ) )
		m_pITEMS[SLOT_HOLD] = GLogicData::GetInstance().GetItem( m_PutOnItems[SLOT_HOLD].GetNativeID() );
}

void GLCHARLOGIC::RELEASE_HOLD_ITEM ()
{
	m_PutOnItems[SLOT_HOLD] = SITEMCUSTOM ( SNATIVEID(false) );

	//	Note : ������ ������ �ʱ�ȭ.
	//
	m_pITEMS[SLOT_HOLD] = NULL;
}


//	�޼�, ������ �������߿��� 'BLOW' �Ӽ��� �ִ� �������� ��ȯ.
SITEM* GLCHARLOGIC::GET_ELMT_ITEM ()
{
	EMSLOT emRHand = GetCurRHand();
	EMSLOT emLHand = GetCurLHand();

	SITEM* pRHandItem = GET_SLOT_ITEMDATA(emRHand);
	SITEM* pLHandItem = GET_SLOT_ITEMDATA(emLHand);

	if ( pRHandItem && pRHandItem->sSuitOp.sBLOW.emTYPE!=EMBLOW_NONE )
	{
		return pRHandItem;
	}
	else if ( pLHandItem && pLHandItem->sSuitOp.sBLOW.emTYPE!=EMBLOW_NONE )
	{
		return pLHandItem;
	}

	return NULL;
}

BOOL GLCHARLOGIC::INIT_DATA ( BOOL bNEW, BOOL bReGen, float fCONFT_POINT_RATE, bool bInitNowExp, bool bJumping )
{
	int i = 0;
	m_CHARINDEX = GETCHARINDEX ();
	const GLCONST_CHARCLASS &cCHARCONST = GLCONST_CHAR::cCONSTCLASS[m_CHARINDEX];

	//	Note : ������ ������ ��������.
	//
	memset ( m_pITEMS, 0x00, sizeof(SITEM*)*SLOT_TSIZE );
	for( int i = 0; i < SLOT_TSIZE; i++ )
	{
		if ( m_PutOnItems[i].GetNativeID() != NATIVEID_NULL() )
		{
			m_pITEMS[i] = GLogicData::GetInstance().GetItem ( m_PutOnItems[i].GetNativeID() );

			//	������ ���̺��� ���� �������� ����.
			if ( !m_pITEMS[i] )
				RELEASE_SLOT_ITEM ( EMSLOT(i) );
		}
	}

	//	Note : ADD �� ���.
	SUM_ADDITION( fCONFT_POINT_RATE );

	if ( bNEW )
	{
		m_bServerStorage = TRUE;

		//m_wStatsPoint = GLCONST_CHAR::wLVL_STATS_P;

		m_sHP.TO_FULL ();
		m_sMP.TO_FULL ();
		m_sSP.TO_FULL ();
		m_sCP.SetData( 0, MAX_CP );

	}
	else if ( bReGen )
	{
		INIT_RECOVER ();	
		m_sCP.SetData( m_sCP.wNow, MAX_CP );
	}

	EMSLOT emRHand = GetCurRHand();
	EMSLOT emLHand = GetCurLHand();

	m_emANISUBTYPE = CHECK_ANISUB ( m_pITEMS[emRHand], m_pITEMS[emLHand], GETCHARINDEX() );

	GLITEM_ATT emRHAtt = ITEMATT_NOTHING;
	GLITEM_ATT emLHAtt = ITEMATT_NOTHING;

	if ( m_pITEMS[emRHand] )
        emRHAtt = m_pITEMS[emRHand]->sSuitOp.emAttack;
	if ( m_pITEMS[emLHand] )
        emLHAtt = m_pITEMS[emLHand]->sSuitOp.emAttack;

	if ( emRHAtt==ITEMATT_NOTHING )
	{
		m_emITEM_ATT = ITEMATT_NOTHING;
        m_wATTRANGE = GLCONST_CHAR::wMAXATRANGE_SHORT;
	}
	else
	{
		m_emITEM_ATT = CONVERT_ITEMATT( emRHAtt );
		m_wATTRANGE = GET_ATTACKDISTANCE_USING_ITEMATT( m_emITEM_ATT );

		//m_wATTRANGE = m_pITEMS[emRHand]->sSuitOp.wAttRange;
	}   

	bool bInitRemainExp = bInitNowExp;

	if (bInitRemainExp)
	{
        if ( GLCONST_CHAR::GET_EXCESS_EXP_TYPE() == 1)
		{
			if (m_CHARINDEX == GLCI_EXTREME_M || m_CHARINDEX == GLCI_EXTREME_W)
                bInitRemainExp = FALSE;
		}
        else if ( GLCONST_CHAR::GET_EXCESS_EXP_TYPE() == 2 || m_bVietnamLevelUp)
		{
			bInitRemainExp = FALSE;
		}
	}
	
	//	���� �������� ���������� �ִ� ����ġ ���.
	m_sExperience.lnMax = GET_LEVELUP_EXP();
	
	if( bInitRemainExp )
		m_sExperience.LIMIT();

	return TRUE;
}

// ��ȯ���� ������ ���
const int GLCHARLOGIC::GETSUMMONDAMGE ()
{
	// �� �տ� ���� �ִ� ������;
	EMSLOT emRHand = GetCurRHand();
	EMSLOT emLHand = GetCurLHand();

	// �⺻ �����;
	GLDWDATA gdDamage(m_gdDAMAGE_SKILL);

	// ����, �Ѿ� ���� ����� �ջ�;
	/*SITEM* pLItem = GET_SLOT_ITEMDATA(emLHand);
	if ( pLItem )
	{
		GLPADATA &sDATA = pLItem->sSuitOp.gdDamage;
		gdDamage.nNow  += int(sDATA.wLow);
		gdDamage.nMax += int(sDATA.wHigh);
	}*/
	SITEMCUSTOM sLItemCustom = GET_SLOT_ITEM(emLHand);
	const GLPADATA& sDATA = sLItemCustom.GetBasicAttackDamage();
	gdDamage.nNow  += int(sDATA.wLow);
	gdDamage.nMax += int(sDATA.wHigh);

	// Stat Ÿ�Ժ� ���� ��ȭ;
	SKILL::EMAPPLY emApply = GetStatType();
	switch ( emApply )
	{
	case SKILL::EMAPPLY_MELEE:
		gdDamage.nNow += m_sSUMITEM.gdDamage.nNow;
		gdDamage.nMax += m_sSUMITEM.gdDamage.nMax;
		break;
	case SKILL::EMAPPLY_RANGE:
		gdDamage.nNow += m_sSUMITEM.gdDamage.nNow;
		gdDamage.nMax += m_sSUMITEM.gdDamage.nMax;
		break;
	case SKILL::EMAPPLY_MAGIC:
		// ����ġ�� ��� ����ġ�� �� ���ԵǾ� �ִ�;
		break;
	}

	// ������� ������ ����� ����ó��;
	gdDamage.nNow = gdDamage.nNow < 0 ? 0 : gdDamage.nNow;
	gdDamage.nMax = gdDamage.nMax < 0 ? 0 : gdDamage.nMax;

	// Stat Ÿ�� �� ����ġ, ���ġ, ����ġ �ݿ�;
	gdDamage.VAR_PARAM( m_sumPowerAttribute[ emApply ] );

	// ���ݷ�, ��ȭ�� �ݿ�;
	gdDamage.nNow  = int(float(gdDamage.nNow)  * m_fDamageRate);
	gdDamage.nMax = int(float(gdDamage.nMax) * m_fDamageRate);

	// ������� ������ ����� ����ó��;
	gdDamage.nNow = gdDamage.nNow < 0 ? 0 : gdDamage.nNow;
	gdDamage.nMax = gdDamage.nMax < 0 ? 0 : gdDamage.nMax;

	// ����� �ִ� �ּҰ� ������ ���� ����� ����;
	const int nDAMAGE_NOW(int(gdDamage.nNow + (gdDamage.nMax-gdDamage.nNow) * sc::Random::RANDOM_POS()));

	return nDAMAGE_NOW;
}

WORD GLCHARLOGIC::RECEIVE_DAMAGE(const int nDamage)
{
	const int nOld(m_sHP.nNow);
	m_sHP.DECREASE(nDamage);	

	RECEIVE_DAMAGE_AFTER();	
	GLCHARLOGIC::RELEASE_ENDURE_PROC();

	return (nOld > m_sHP.nNow) ? (nOld - m_sHP.nNow) : 0;
}

void GLCHARLOGIC::RELEASE_ENDURE_PROC(void)
{
	const DWORD nRELEASE_ENDURE = m_sRELEASE_ENDURE.nRELEASE_ENDURE;
	if ( nRELEASE_ENDURE )
	{ // �ǰݽ� ��ų ȿ�� ���� ��ų�� �ɷ��־��ٸ� ó��;  // �Ϲ� ���� ����;
		const DWORD dwPercentage = sc::Random::getInstance().GetPercent();
		for ( unsigned int _i = 0; _i < nRELEASE_ENDURE; ++_i )
		{			
			if ( dwPercentage < m_sRELEASE_ENDURE.dwActivePercentage[_i] )
			{	
				UPDATE_DATA_END(m_sRELEASE_ENDURE.dwSkillFactIndex[_i]);
				continue;
			}			
		}
	}
}

WORD GLCHARLOGIC::RECEIVE_HEAL(const int nHeal)
{
	const int nOld(m_sHP.nNow);
	m_sHP.INCREASE(nHeal);

	return (nOld < m_sHP.nNow) ? (m_sHP.nNow - nOld) : 0;
}

BOOL GLCHARLOGIC::RECEIVE_QITEMFACT ( const SNATIVEID &nidITEM )
{
	const SITEM *pITEM = GLogicData::GetInstance().GetItem ( nidITEM );
	if ( !pITEM )
		return FALSE;
	if ( pITEM->sBasicOp.emItemType!=ITEM_QITEM )
		return FALSE;
	if ( pITEM->sQuestionItem.emType==QUESTION_NONE )
		return FALSE;

	const ITEM::SQUESTIONITEM &sQUESTIONITEM = pITEM->sQuestionItem;

	switch ( sQUESTIONITEM.emType )
	{
	case QUESTION_SPEED_UP:
	case QUESTION_CRAZY:
	case QUESTION_ATTACK_UP:
	case QUESTION_EXP_UP:
	case QUESTION_LUCKY:
	case QUESTION_SPEED_UP_M:
	case QUESTION_MADNESS:
	case QUESTION_ATTACK_UP_M:
		m_sQITEMFACT.emType = sQUESTIONITEM.emType;
		m_sQITEMFACT.fTime = sQUESTIONITEM.fTime;
		m_sQITEMFACT.wParam1 = sQUESTIONITEM.wParam1;
		m_sQITEMFACT.wParam2 = sQUESTIONITEM.wParam2;
		break;
	};

	return TRUE;
}

void GLCHARLOGIC::RECEIVE_EVENTFACT_BEGIN ( const EMGM_EVENT_TYPE emType, const WORD wValue )
{
	m_sEVENTFACT.SetEVENT( emType, wValue );
}

void GLCHARLOGIC::RECEIVE_EVENTFACT_END ( const EMGM_EVENT_TYPE emType )
{
	m_sEVENTFACT.ResetEVENT( emType );
}

//	Note : ? �������� ��ȿ �ð��� ���Ž�Ų��.
//			���� �ڵ�� ��������� false, �׿� ���׿� true
//
bool GLCHARLOGIC::UPDATE_QITEMFACT ( float fElapsedTime )
{
	if( !m_sQITEMFACT.IsACTIVE() )
        return true;

	m_sQITEMFACT.fTime -= fElapsedTime;
	if ( m_sQITEMFACT.fTime <= 0 )
	{
		m_sQITEMFACT.RESET();
		return false;
	}
    
    return true;
}

void GLCHARLOGIC::UPDATE_MAX_POINT ( float fCONFT_POINT_RATE, const bool bRATE_MAINTENANCE )
{
	EMCHARINDEX emCI = GETCHARINDEX ();										//	�ɸ����� ���� �ε���.
	const GLCONST_CHARCLASS &cCHARCONST = GLCONST_CHAR::cCONSTCLASS[emCI];	//	�ɸ����� �����.

    float fTotalHPRate = 0.0f;
    float fTotalMPRate = 0.0f;
    float fTotalSPRate = 0.0f;
	
    const int nHPMax(m_sHP.nMax);
	m_sHP.nMax = 0;
    fTotalHPRate = ((1+m_sSUM_PASSIVE.m_fHP_RATE+m_fHP_RATE ) * fCONFT_POINT_RATE);

	int nHPTemp(0);
	
	// �����ɼ� ȿ��;
	const INT nSPRatioToHP(GetIncR_AllSPRatioToHp()*m_sSP.dwMax);
//	GLOGICEX::VARIATION( nHPTemp, INT_MAX, int (float(m_sSUMSTATS_SKILL.wStr) * cCHARCONST.fHP_STR) + m_sSUMITEM.nHP + m_sSUM_PASSIVE.m_nHP + m_nHP_ADD_SKILL);
	GLOGICEX::VARIATION( nHPTemp, INT_MAX, int (float(m_sSUMSTATS_SKILL.wStr * cCHARCONST.fHP_STR)) + m_sSUMITEM.nHP + m_sSUM_PASSIVE.m_nHP + m_nHP_ADD_SKILL + nSPRatioToHP);
	GLOGICEX::VARIATION( m_sHP.nMax, INT_MAX, int ( float(nHPTemp) * fTotalHPRate ) );
    m_sHP.nMax = static_cast<int>(static_cast<float>(m_sHP.nMax)) * m_sLINKHP.fTake;
	if ( m_bParty == FALSE )
	{
		if ( m_sSUMITEM.nInc_HP_SoloPlay != 0 )
			GLOGICEX::VARIATION( m_sHP.nMax, INT_MAX, m_sSUMITEM.nInc_HP_SoloPlay);
	}
	else
	{
		if ( m_sSUMITEM.nInc_HP_PartyPlay != 0 )
			GLOGICEX::VARIATION( m_sHP.nMax, INT_MAX, m_sSUMITEM.nInc_HP_PartyPlay);
	}
	// HP�������� �ƹ��� �����ص� MaxHP�� 1���Ϸδ� ������ �� ����. [7/16/2014 hsshin];
	if ( m_sHP.nMax <= 0 )
		m_sHP.nMax = 1;

    const int nMPMax(m_sMP.nMax);
    m_sMP.nMax = 0;
    fTotalMPRate = ((1+m_sSUM_PASSIVE.m_fMP_RATE+m_fMP_RATE ) * fCONFT_POINT_RATE);

	int nMPTemp(0);
	GLOGICEX::VARIATION(nMPTemp, USHRT_MAX, int(float(m_sSUMSTATS_SKILL.wSpi) * cCHARCONST.fMP_SPI + m_sSUMITEM.nMP + m_sSUM_PASSIVE.m_nMP + m_nMP_ADD_SKILL));
	GLOGICEX::VARIATION(m_sMP.nMax, USHRT_MAX, int (float(nMPTemp) * fTotalMPRate) );


    const int nSPMax(m_sSP.nMax);
    m_sSP.nMax = 0;
    fTotalSPRate = ((1+m_sSUM_PASSIVE.m_fSP_RATE+m_fSP_RATE ) * fCONFT_POINT_RATE);

	int nSPTemp(0);
	GLOGICEX::VARIATION(nSPTemp, USHRT_MAX, int(float(m_sSUMSTATS_SKILL.wSta) * cCHARCONST.fSP_STA + m_sSUMITEM.nSP + m_sSUM_PASSIVE.m_nSP + m_nSP_ADD_SKILL));
	GLOGICEX::VARIATION(m_sSP.nMax, USHRT_MAX, int(float(nSPTemp) * fTotalSPRate ) );


    //! �������� �޶������� ���ʿ��� Now���� ���� ��ȭ��Ű�� ���Ͽ�
    if ( bRATE_MAINTENANCE && m_fHP_TOTALRATE != 0.0f )
	{		
		if ( static_cast<int>((m_fHP_TOTALRATE - fTotalHPRate) * 1000.0f) != 0 )
		{ // float ������ ���� 0.000001 ���̰� ����;
			if ( nHPMax > 0 )
				m_sHP.nNow = int(float(m_sHP.nNow) * (float(m_sHP.nMax) / float(nHPMax)));
			else
				m_sHP.nNow = 1;
		}
	}

    m_fHP_TOTALRATE = fTotalHPRate;

    if ( bRATE_MAINTENANCE && m_fMP_TOTALRATE != 0.0f && m_fMP_TOTALRATE != fTotalMPRate )
	{
		if ( nMPMax > 0)
			m_sMP.nNow = int(float(m_sMP.nNow) * (float(m_sMP.nMax) / float(nMPMax)));			
		else
			m_sMP.nNow = 0;
	}

    m_fMP_TOTALRATE = fTotalMPRate;

    if ( bRATE_MAINTENANCE && m_fSP_TOTALRATE != 0.0f && m_fSP_TOTALRATE != fTotalSPRate )
	{
		if ( nSPMax > 0)
			m_sSP.nNow = int(float(m_sSP.nNow) * (float(m_sSP.nMax) / float(nSPMax)));
		else
			m_sSP.nNow = 0;
	}

    m_fSP_TOTALRATE = fTotalSPRate;

    m_sHP.LIMIT();
    m_sMP.LIMIT();
    m_sSP.LIMIT();
}

void GLCHARLOGIC::PrevUPDATE_DATA(const float fElapsedTime)
{
	memset(m_bSkillFactsSpecial, false, sizeof(bool) * EMSPECA_NSIZE);
	m_nDEFENSE_SKILL = m_nDEFENSE;
	m_gdDAMAGE_SKILL = m_gdDAMAGE;

	m_nSUM_HIT = m_nHIT;
	m_nSUM_AVOID = m_nAVOID;

	m_nSUM_PIERCE = m_sSUM_PASSIVE.m_nPIERCE;
	m_fSUM_RANGE_PSY_LONG = m_sSUM_PASSIVE.m_fRANGE_PSY_LONG;

	m_fSUM_TARRANGE_ALL = m_sSUM_PASSIVE.m_fTARRANGE_ALL;
	m_fSUM_APPLYRANGE_ALL = m_sSUM_PASSIVE.m_fAPPLYRANGE_ALL;

	m_bSTATEBLOW = FALSE;	

	m_bSTATE_PANT = false;
	m_bSTATE_STUN = false;
	m_bSTATE_CONTROL = true;

	m_dwHOLDBLOW = NULL;

	m_fDamageRate = 1.0f + m_sSUM_PASSIVE.m_fDAMAGE_RATE;
	m_fDefenseRate = 1.0f + m_sSUM_PASSIVE.m_fDEFENSE_RATE;
	m_fHitRate = 1.0f;		//�нú� �߰� �ȵ�
	m_fAvoidRate = 1.0f;	//�нú� �߰� �ȵ�

	m_sSUMRESIST_SKILL = m_sSUMRESIST;
	m_sSUMSTATS_SKILL = m_sSUMSTATS;	//������ ���� ������ Ȯ���Ҷ� ���� �ش� �ʱⰪ�� ����ϰ� �ִ� �����Ҷ� ���� ����

	m_sDamageSpec.RESET();
	m_sDamageSpec = m_sSUM_PASSIVE.m_sDamageSpec;
	m_sDefenseSkill.RESET();
	m_sINVISIBLE.RESET();	
	m_sRECVISIBLE.RESET();
	m_sRELEASE_ENDURE.RESET();
	m_sHALLUCINATE.RESET();
	m_sDOMINATE.RESET();
	m_sIncreaseEff.RESET();
	m_sSTIGMA_TAKER.RESET();
	m_sLINKHP.RESET();	
	m_sRateViewRange.RESET();

	m_fEXP_RATE = 1;
	m_fQItem_EXP_RATE = 0;
	m_fSkill_EXP_RATE = 0;
	m_fRANDOM_BOX_EXP_RATE = 0;
    m_fGENITEM_RATE = 1;
    m_fGENMONEY_RATE = 1;

	m_fHP_RATE = 0.0f;
	m_fMP_RATE = 0.0f;
	m_fSP_RATE = 0.0f;
	
	m_fVariationApplyRateHP = 1.0f;
	m_fVariationApplyRateMP = 1.0f;
	m_fVariationApplyRateSP = 1.0f;

	m_fATTVELO = 0.0f;
	m_fSKILLDELAY = 0.0f;

	m_fSTATE_MOVE_RATE = 1.0f;
	m_fSTATE_MOVE_VALUE = 0;
	m_fSTATE_DELAY = 1.0f;
	m_fSTATE_DAMAGE = 1.0f;
	m_fSKILL_MOVE = 0.0f + m_sSUM_PASSIVE.m_fMOVEVELO;

    m_nHP_ADD = 0;
    m_nMP_ADD = 0;
    m_nSP_ADD = 0;
    m_nCP_ADD = 0;
    m_fTempVarCP = 0.0f;
	m_nGET_CP_VALUE = 0;

	m_nHP_ADD_SKILL = 0;
	m_nMP_ADD_SKILL = 0;
	m_nSP_ADD_SKILL = 0;
	m_nCP_ADD_SKILL = 0;

	m_nPotion_HP_Inc_SKILL = 0;
	m_nPotion_MP_Inc_SKILL = 0;
	m_nPotion_SP_Inc_SKILL = 0;

	m_nPA_ADD = 0;
	m_nMA_ADD = 0;
	m_nSA_ADD = 0;

	m_fSUMMONACTIVETIME  = 0.0f;
	m_fSUMMONACTIVETIME += m_sSUM_PASSIVE.m_fSUMMONACTIVETIME;

	m_bSafeZone = false;
	memset ( m_bActionType, false, sizeof( bool ) * SKILL::EMACTION_TYPE_NSIZE );

	m_dwActionLimit = EMACTION_LIMIT_NONE;
	
	m_wACCEPTP_SKILL = 0;
	m_wATTRANGE_SKILL = 0;

    m_fMoveVeloLimit_Min = -1.0f;
    m_fMoveVeloLimit_Max = 1000.0f;

    m_fVariationRateHP = GLCONST_CHAR::fHP_INC_PER + m_sSUMITEM.fIncR_HP + m_sSUM_PASSIVE.m_fINCR_HP;
    m_fVariationRateMP = GLCONST_CHAR::fMP_INC_PER + m_sSUMITEM.fIncR_MP + m_sSUM_PASSIVE.m_fINCR_MP;
    m_fVariationRateSP = GLCONST_CHAR::fSP_INC_PER + m_sSUMITEM.fIncR_SP + m_sSUM_PASSIVE.m_fINCR_SP;

	m_SkillCAIndex.ResetData();

	m_fBasicRateHP = m_fVariationRateHP;

	RF_SCREEN_THROW( m_EffSkillVarSet ).Reset();
	RF_ENTRANCE_EXIT( m_EffSkillVarSet ).ResetValue();
	RF_FEIGN_DEATH( m_EffSkillVarSet ).ResetValue();
	RF_DISGUISE( m_EffSkillVarSet ).ResetValue();
	RF_PROVOKE( m_EffSkillVarSet ).ResetValue();
	RF_IMMUNE( m_EffSkillVarSet ).ResetValue();
	RF_ACTIVE( m_EffSkillVarSet ).ResetValue();
} //GLCHARLOGIC::PrevUPDATE_DATA

void GLCHARLOGIC::UPDATE_DATA_SkillAddon( const SKILL::SIMPACT_ADDON_LEVEL&		addon,
										  int&									nSUM_PA,
										  int&									nSUM_SA, 
										  int&									nSUM_MA )
{
    switch( addon.emADDON )
    {
    case EMIMPACTA_HITRATE:
		m_nSUM_HIT += int(addon.fADDON_VAR);
		break;
    case EMIMPACTA_AVOIDRATE:
		m_nSUM_AVOID += int(addon.fADDON_VAR);
		break;
    case EMIMPACTA_DAMAGE:
		m_gdDAMAGE_SKILL.VAR_PARAM ( int(addon.fADDON_VAR) );
		break;
    case EMIMPACTA_DEFENSE:
		m_nDEFENSE_SKILL += int(addon.fADDON_VAR);
		break;
    case EMIMPACTA_VARHP:
		m_fBasicRateHP += addon.fADDON_VAR;
		m_fVariationRateHP += addon.fADDON_VAR;
		break;
    case EMIMPACTA_VARMP:
		m_fVariationRateMP += addon.fADDON_VAR;
		break;
    case EMIMPACTA_VARSP:
		m_fVariationRateSP += addon.fADDON_VAR;
		break;
    case EMIMPACTA_VARAP:
		m_fBasicRateHP += addon.fADDON_VAR;
	    m_fVariationRateHP += addon.fADDON_VAR;
	    m_fVariationRateMP += addon.fADDON_VAR;
	    m_fVariationRateSP += addon.fADDON_VAR;
	    break;

    case EMIMPACTA_DAMAGE_RATE:
		m_fDamageRate += addon.fADDON_VAR;
		break;
    case EMIMPACTA_DEFENSE_RATE:
		m_fDefenseRate += addon.fADDON_VAR;
		break;		

    case EMIMPACTA_PA:
		nSUM_PA += int(addon.fADDON_VAR);
		break;
    case EMIMPACTA_SA:
		nSUM_SA += int(addon.fADDON_VAR);
		break;
    case EMIMPACTA_MA:
		nSUM_MA += int(addon.fADDON_VAR);
		break;
    case EMIMPACTA_HP_RATE:
		m_fHP_RATE += addon.fADDON_VAR;
		break;
    case EMIMPACTA_MP_RATE:
		m_fMP_RATE += addon.fADDON_VAR;
		break;
    case EMIMPACTA_SP_RATE:
		m_fSP_RATE += addon.fADDON_VAR;
		break;
    case EMIMPACTA_RESIST:
		m_sSUMRESIST_SKILL += int(addon.fADDON_VAR);
		break;
    case EMIMPACTA_STATS:
	    {
		    GLOGICEX::VARIATION ( m_sSUMSTATS_SKILL.wPow, USHRT_MAX, int(addon.fADDON_VAR) );
		    GLOGICEX::VARIATION ( m_sSUMSTATS_SKILL.wDex, USHRT_MAX, int(addon.fADDON_VAR) );
		    GLOGICEX::VARIATION ( m_sSUMSTATS_SKILL.wSpi, USHRT_MAX, int(addon.fADDON_VAR) );
		    break;
	    }
    case EMIMPACTA_HP_ADD:
        m_nHP_ADD += static_cast<int>(addon.fADDON_VAR);
        break;
    case EMIMPACTA_MP_ADD:
        m_nMP_ADD += static_cast<int>(addon.fADDON_VAR);
        break;
    case EMIMPACTA_SP_ADD:
        m_nSP_ADD += static_cast<int>(addon.fADDON_VAR);
        break;
    case EMIMPACTA_CP_ADD:
        m_nCP_ADD += static_cast<int>(addon.fADDON_VAR);
        break;
    case EMIMPACTA_CP_DUR:
        m_fTempVarCP += addon.fADDON_VAR;
        break;
    };
} //GLCHARLOGIC::UPDATE_DATA_SkillAddon

void GLCHARLOGIC::UPDATE_DATA_SkillSpecial( const SSKILLFACT& sSKEFF, const SKILL::SSPEC_ADDON_LEVEL& spec, SDURATION_DAMAGE* sDurDamage, const DWORD dwSkillFactSlot, BOOL bClient )
{
	m_bSkillFactsSpecial[spec.emSPEC] = true;
    switch ( spec.emSPEC )
    {
	case EMSPECA_ACTIVATE_DAMAGE :
		{
			if( bClient ) {
				RF_ACTIVE( m_EffSkillVarSet ).TurnOn();
			}
			else {
				RF_ACTIVE( m_EffSkillVarSet ).TurnOn( sSKEFF.sNATIVEID.dwID, sSKEFF.wLEVEL, spec.sSPEC.nVAR1, spec.sSPEC.nVAR2 );
			}
		}
		break;
	case EMSPECA_SPECIAL_IMMUNE :
		RF_IMMUNE( m_EffSkillVarSet ).TurnOn( !bClient ? (BYTE)spec.sSPEC.nVAR1 : 0 );
		break;
	case EMSPECA_PROVOKE    :
		{
			WORD wValue = (WORD)spec.sSPEC.fVAR1;

			if( wValue == NULL_DISTANCE ) wValue = 0;

			RF_PROVOKE( m_EffSkillVarSet ).TurnOn( sSKEFF.sID, wValue );

			m_bSTATE_CONTROL = false;
		}		
		break;
	case EMSPECA_DISGUISE   :
		RF_DISGUISE( m_EffSkillVarSet ).TurnOn( (int)spec.sSPEC.fVAR2, (int)spec.sSPEC.fVAR1 );
		break;
	case EMSPECA_CAMERA_HIT :
		RF_CAMERA_HIT( m_EffSkillVarSet ).TurnOn( (WORD)spec.sSPEC.fVAR1 );
		m_dwActionLimit |= EMACTION_LIMIT_SKILL;
		break;
	case EMSPECA_FEIGN_DEATH : 
		RF_FEIGN_DEATH( m_EffSkillVarSet ).TurnOn(spec.sSPEC.dwFLAG&FEIGN_DEATH_TARGETING_DISABLE, spec.sSPEC.dwFLAG&FEIGN_DEATH_BUF_DEL, dwSkillFactSlot );
		break;
	case EMSPECA_ENTRANCE_EXIT :
		RF_ENTRANCE_EXIT( m_EffSkillVarSet ).TurnOn();
		break;
	case EMSPECA_SCREEN_THROW :
		RF_SCREEN_THROW( m_EffSkillVarSet ).TurnOn( sSKEFF.sID, spec.sSPEC.fVAR1 );
		m_dwActionLimit |= EMACTION_LIMIT_SKILL;
		break;
	case EMSPECA_COUNTATTACK :
		m_SkillCAIndex.SetData( SNATIVEID(spec.sSPEC.fVAR1, spec.sSPEC.fVAR2 ),  &sSKEFF.pcCAData, dwSkillFactSlot, sSKEFF.sNATIVEID, (int)sSKEFF.wLEVEL );
		break;
    case EMSPECA_NONBLOW:
		m_dwHOLDBLOW = spec.sSPEC.dwFLAG;
		break;
    case EMSPECA_PIERCE:
		m_nSUM_PIERCE += int ( spec.sSPEC.fVAR1 );
		break;
    case EMSPECA_RANGE_PSY_LONG:
		m_fSUM_RANGE_PSY_LONG += spec.sSPEC.fVAR1;
		break;
    case EMSPECA_MOVEVELO:
		m_fSKILL_MOVE += spec.sSPEC.fVAR1;
		break;
    case EMSPECA_INVISIBLE:
		m_bSkillFactsSpecial[EMSPECA_INVISIBLE] = m_sINVISIBLE.SET(spec.sSPEC);
		break;

    case EMSPECA_RECVISIBLE:		
		m_sRECVISIBLE.SET(spec.sSPEC);
		break;

    //	���� �ӵ� ����̹Ƿ� ( ���� �ð��� -0.1 (-10%) ���� ��Ű�� ���ؼ��� ��ȣ ������ �Ͽ�����. )
    case EMSPECA_ATTACKVELO:	
		m_fATTVELO -= spec.sSPEC.fVAR1;
		break;

    case EMSPECA_SKILLDELAY:
        if (CHECK_FLAG_SKILL(spec.sSPEC.dwFLAG, EMSPEC_SKILL_ASING_AFTER) )
        {
            if ( sc::Random::RANDOM_POS()*100.0f <  spec.sSPEC.fVAR2 )
                m_fSKILLDELAY += spec.sSPEC.fVAR1;
        }
		break;

    case EMSPECA_PSY_DAMAGE_REDUCE: // ��ų ȿ�� ���� ����. by luxes.
	    if ( m_sDamageSpec.m_fPsyDamageReduce < spec.sSPEC.fVAR1 )
	    {
		    m_sDamageSpec.m_fPsyDamageReduce = spec.sSPEC.fVAR1;
		    m_sDamageSpec.m_fPsyDamageReduceEFFRate = spec.sSPEC.fRate;
	    }
	    break;

    case EMSPECA_MAGIC_DAMAGE_REDUCE: // ��ų ȿ�� ���� ����. by luxes.
	    if ( m_sDamageSpec.m_fMagicDamageReduce < spec.sSPEC.fVAR1 )
	    {
		    m_sDamageSpec.m_fMagicDamageReduce = spec.sSPEC.fVAR1;
		    m_sDamageSpec.m_fMagicDamageReduceEffRate = spec.sSPEC.fRate;
	    }
	    break;

    case EMSPECA_PSY_DAMAGE_REFLECTION: // ��ų ȿ�� ���� ����. by luxes.
	    if ( m_sDamageSpec.m_fPsyDamageReflection < spec.sSPEC.fVAR1 )
	    {
		    m_sDamageSpec.m_fPsyDamageReflection = spec.sSPEC.fVAR1;		
		    m_sDamageSpec.m_fPsyDamageReflectionRate = spec.sSPEC.fVAR2;
		    m_sDamageSpec.m_fPsyDamageReflectionEffRate = spec.sSPEC.fRate;		
		    m_sDamageSpec.m_fPsyDamageReflectionRateEffRate = spec.sSPEC.fRate2;
	    }
	    break;

    case EMSPECA_MAGIC_DAMAGE_REFLECTION: // ��ų ȿ�� ���� ����. by luxes.
	    if ( m_sDamageSpec.m_fMagicDamageReflection < spec.sSPEC.fVAR1 )
	    {
		    m_sDamageSpec.m_fMagicDamageReflection = spec.sSPEC.fVAR1;	
		    m_sDamageSpec.m_fMagicDamageReflectionRate = spec.sSPEC.fVAR2;
		    m_sDamageSpec.m_fMagicDamageReflectionEffRate = spec.sSPEC.fRate;	
		    m_sDamageSpec.m_fMagicDamageReflectionRateEffRate = spec.sSPEC.fRate2;
	    }
	    break;

    case EMSPECA_DEFENSE_SKILL_ACTIVE: // ��ų ȿ�� ���� ����. by luxes.
	    {
		    m_sDefenseSkill.m_dwSkillID = spec.sSPEC.dwNativeID;
		    m_sDefenseSkill.m_wLevel = static_cast<WORD>(spec.sSPEC.dwFLAG);
		    m_sDefenseSkill.m_fRate = spec.sSPEC.fVAR1;
		    m_sDefenseSkill.m_fRateEffRate = spec.sSPEC.fRate;
	    }
	    break;

	case EMSPECA_INCREASE_EFF:
		m_sIncreaseEff.SET(spec.sSPEC);
		break;

	case EMSPECA_ILLUSION:
		m_sDamageSpec.m_dwFlags |= DAMAGE_SPEC::FLAG_ILLUSION;
		break;

	case EMSPECA_TAUNT:		
		break;

	case EMSPECA_TURN_STATE_BATTLE:			
		m_bSkillFactsSpecial[EMSPECA_TURN_STATE_BATTLE] = m_bSkillFactsSpecial[EMSPECA_TURN_STATE_PEACE] ? false : true;
		m_dwActionLimit |= EMACTION_LIMIT_CARD;
		
		break;

	case EMSPECA_TURN_STATE_PEACE:
		m_bSkillFactsSpecial[EMSPECA_TURN_STATE_BATTLE] = false;
		break;

    case EMSPECA_DURATION_DAMAGE:
	    {
		    int nType = (int)spec.sSPEC.dwFLAG;					
		    if ( nType < 0 || nType >= EMELEMENT_MAXNUM )
				break;

		    // fVAR1 : TickTime, fVAR2 : Damage
		    sDurDamage[nType].m_fTickTime = spec.sSPEC.fVAR1;
		    sDurDamage[nType].m_fDamage += spec.sSPEC.fVAR2;
		    sDurDamage[nType].m_sID = sSKEFF.sID;
			sDurDamage[nType].m_sSkillID = sSKEFF.sNATIVEID;
	    }
	    break;

    case EMSPECA_CURSE: // ��ų ȿ�� ���� ����. by luxes.
	    {
		    if ( m_sDamageSpec.m_fDamageCurse < spec.sSPEC.fVAR1 )
		    {
			    m_sDamageSpec.m_fDamageCurse = spec.sSPEC.fVAR1;
			    m_sDamageSpec.m_fDamageCurseEffRate = spec.sSPEC.fRate;
			    m_sDamageSpec.m_sCurseTargetID = sSKEFF.sID;
		    }
	    }
	    break;

    case EMSPECA_TARRANGE_ALL:
		m_fSUM_TARRANGE_ALL += spec.sSPEC.fVAR1;		
	    break;

    case EMSPECA_APPLYRANGE_ALL:
		m_fSUM_APPLYRANGE_ALL += spec.sSPEC.fVAR1;		
	    break;
	
    case EMSPECA_STUN:
	    {
		    m_dwActionLimit |= EMACTION_LIMIT_ITEM;
		    //	�ٸ� �׸��� �����̻� ���� ȿ���� ��ü
		    m_fSTATE_MOVE_RATE = 0.0f;								//	����.
		    m_bSTATE_STUN = true;
	    }
	    break;
    case EMSPECA_NON_DRUG:
		m_dwActionLimit |= EMACTION_LIMIT_DRUG;
	    break;
    case EMSPECA_NON_SKILL:
		m_dwActionLimit |= EMACTION_LIMIT_SKILL;
	    break;
    case EMSPECA_IGNORED_DAMAGE:
		{
		}
	    break;
    case EMSPECA_ITEMDROP_RATE:
		m_fGENITEM_RATE += spec.sSPEC.fVAR1;
        break;

    case EMSPECA_MONEYDROP_RATE:
		m_fGENMONEY_RATE += spec.sSPEC.fVAR1;
        break;

    case EMSPECA_EXP_RATE:
		m_fEXP_RATE += spec.sSPEC.fVAR1;
		m_fSkill_EXP_RATE += spec.sSPEC.fVAR1;
        break;

    case EMSPECA_NPC_TALK:
        break;

    case EMSPECA_ITEMDROP_SPECIFIC:
        break;

    case EMSPECA_VEHICLE_GETOFF: // ����  
		m_bSkillFactsSpecial[EMSPECA_VEHICLE_GETOFF] = DWORD(spec.sSPEC.fVAR2) ? true : false;
        break;
    case EMSPECA_LINK_HP:
        if ( static_cast<int>(spec.sSPEC.fVAR2) == TRUE )
        {
            if ( spec.sSPEC.fVAR1 < m_sLINKHP.fTake)
                m_sLINKHP.fTake = spec.sSPEC.fVAR1;
        }
        break;

	case EMSPECA_RELEASE_ENDURE:		
		m_sRELEASE_ENDURE.SET(spec.sSPEC, dwSkillFactSlot);
		break;
	case EMSPECA_RELEASE_PET:
		m_bSkillFactsSpecial[EMSPECA_RELEASE_PET] = DWORD(spec.sSPEC.fVAR1) ? true : false;		
		break;
	case EMSPECA_IMMUNE:
		m_sDamageSpec.m_emImmuneApplyType |= DWORD(spec.sSPEC.fVAR1);
		m_sDamageSpec.m_emImmuneActionType |= DWORD(spec.sSPEC.fVAR2);
		break;
	case EMSPECA_STIGMA:
		{
			STARGETID sSelfID(CROW_PC, m_dwUserID);
			m_sSTIGMA_TAKER.PushStigmaPack(sSKEFF.sID, sSelfID, sSKEFF.sNATIVEID, spec.sSPEC);
		}		
		break;
	case EMSPECA_HALLUCINATE:
		if ( m_sHP.nNow )
			m_sHALLUCINATE.SET(spec.sSPEC);
		break;
	case EMSPECA_DOMINATION:	
		if ( m_sHP.nNow )
		{
			m_sDOMINATE.SET(spec.sSPEC);
			m_bSTATE_CONTROL = false;
		}
		break;
	case EMSPECA_RANDOM_EXP_RATE:		
		m_fEXP_RATE += m_ExperienceRate.getTotalRate();
		m_fRANDOM_BOX_EXP_RATE += m_ExperienceRate.getTotalRate();
		break;
	
	//
	case EMSPECA_DAMAGE_VALUE: 
		m_gdDAMAGE_SKILL.VAR_PARAM ( static_cast<int>(spec.sSPEC.fVAR1) );
		break;
	case EMSPECA_DAMAGE_RATE: 
		m_fDamageRate			+= spec.sSPEC.fVAR1;
		break;

	case EMSPECA_DEFENSE_VALUE:
		m_nDEFENSE_SKILL		+= static_cast<int>( spec.sSPEC.fVAR1 );
		break;
	case EMSPECA_DEFENSE_RATE: 
		m_fDefenseRate			+= spec.sSPEC.fVAR1;
		break;
	
	case EMSPECA_HIT_VALUE: 
		m_nSUM_HIT				+= static_cast<int>( spec.sSPEC.fVAR1 );
		break;
	case EMSPECA_HIT_RATE: 
		m_fHitRate				+= spec.sSPEC.fVAR1;
		break;
	
	case EMSPECA_AVOID_VALUE: 
		m_nSUM_AVOID			+= static_cast<int>( spec.sSPEC.fVAR1 );
		break;
	case EMSPECA_AVOID_RATE: 
		m_fAvoidRate			+= spec.sSPEC.fVAR1;
		break;
	
	case EMSPECA_HP_VALUE:
		m_nHP_ADD_SKILL			+= static_cast<int>( spec.sSPEC.fVAR1 );
		break;
	case EMSPECA_MP_VALUE:
		m_nMP_ADD_SKILL			+= static_cast<int>( spec.sSPEC.fVAR1 );
		break;
	case EMSPECA_SP_VALUE:
		m_nSP_ADD_SKILL			+= static_cast<int>( spec.sSPEC.fVAR1 );
		break;
	case EMSPECA_CP_VALUE:		//UPDATE_DATA�� ó�� ���� ����
		m_nCP_ADD_SKILL			+= static_cast<int>( spec.sSPEC.fVAR1 );
		break;
	
	case EMSPECA_HP_VARIATION_RATE:
		m_fBasicRateHP += spec.sSPEC.fVAR1;
		m_fVariationRateHP += spec.sSPEC.fVAR1;
		break;
	case EMSPECA_MP_VARIATION_RATE:
		m_fVariationRateMP += spec.sSPEC.fVAR1;
		break;
	case EMSPECA_SP_VARIATION_RATE:
		m_fVariationRateSP += spec.sSPEC.fVAR1;
		break;
	case EMSPECA_HMS_VARIATION_RATE:
		m_fBasicRateHP += spec.sSPEC.fVAR1;
		m_fVariationRateHP += spec.sSPEC.fVAR1;
		m_fVariationRateMP += spec.sSPEC.fVAR1;
		m_fVariationRateSP += spec.sSPEC.fVAR1;
		break;

	case EMSPECA_ATTACK_RANGE_VALUE:
		m_wATTRANGE_SKILL		+= static_cast<int>( spec.sSPEC.fVAR1 );
		break;
	case EMSPECA_DIS_SP_VALUE:
		m_wACCEPTP_SKILL				+= static_cast<int>( spec.sSPEC.fVAR1 );
		break;
	case EMSPECA_RESIST_VALUE:
		m_sSUMRESIST_SKILL		+= static_cast<int>( spec.sSPEC.fVAR1 );
		break;
	
	case EMSPECA_STATS_POW_VALUE:
		GLOGICEX::VARIATION ( m_sSUMSTATS_SKILL.wPow, USHRT_MAX, static_cast<int>(spec.sSPEC.fVAR1) );
		break;
	case EMSPECA_STATS_STR_VALUE:
		GLOGICEX::VARIATION ( m_sSUMSTATS_SKILL.wStr, USHRT_MAX, static_cast<int>(spec.sSPEC.fVAR1) );
		break;
	case EMSPECA_STATS_SPI_VALUE:
		GLOGICEX::VARIATION ( m_sSUMSTATS_SKILL.wSpi, USHRT_MAX, static_cast<int>(spec.sSPEC.fVAR1) );
		break;
	case EMSPECA_STATS_DEX_VALUE:
		GLOGICEX::VARIATION ( m_sSUMSTATS_SKILL.wDex, USHRT_MAX, static_cast<int>(spec.sSPEC.fVAR1) );
		break;
	case EMSPECA_STATS_STA_VALUE:
		GLOGICEX::VARIATION ( m_sSUMSTATS_SKILL.wSta, USHRT_MAX, static_cast<int>(spec.sSPEC.fVAR1) );
		break;
	
	case EMSPECA_PA_VALUE:
		m_nPA_ADD				+= static_cast<int>( spec.sSPEC.fVAR1 );
		break;
	case EMSPECA_SA_VALUE:
		m_nSA_ADD				+= static_cast<int>( spec.sSPEC.fVAR1 );
		break;
	case EMSPECA_MA_VALUE:
		m_nMA_ADD 				+= static_cast<int>( spec.sSPEC.fVAR1 );
		break;

	case EMSPECA_HP_POTION_INC_VALUE:
		m_nPotion_HP_Inc_SKILL				+= static_cast<int>( spec.sSPEC.fVAR1 );
		break;
	case EMSPECA_MP_POTION_INC_VALUE:
		m_nPotion_MP_Inc_SKILL				+= static_cast<int>( spec.sSPEC.fVAR1 );
		break;
	case EMSPECA_SP_POTION_INC_VALUE:
		m_nPotion_SP_Inc_SKILL				+= static_cast<int>( spec.sSPEC.fVAR1 );
		break;
	
	case EMSPECA_CP_INC_VALUE:
		m_nGET_CP_VALUE						+= static_cast<int>( spec.sSPEC.fVAR1 );
		break;
	case EMSPECA_MOVEVELO_VALUE:
		m_fSTATE_MOVE_VALUE					+= static_cast<int>( spec.sSPEC.fVAR1 );
		break;
	case EMSPECA_DAMAGE_RATE_ALL:
		m_fDamageRate					+= spec.sSPEC.fVAR1;
		break;
	case EMSPECA_VIEW_RANGE_RATE:
		if ( spec.sSPEC.fVAR1 < m_sRateViewRange.fRate )
			m_sRateViewRange = spec.sSPEC.fVAR1;
		break;
	case EMSPECA_DELAY_ACTION_SKILL:
		break;
	case EMSPECA_CLONE:
		m_fDamageRate			+= spec.sSPEC.fVAR2 * spec.sSPEC.fVAR1;
		m_sClone.SetClonNum(spec.sSPEC.fVAR1);
		break;

    case EMSPECA_MOVEVELO_LIMIT:
        {
            if ( spec.sSPEC.fVAR1 >= 0.0f )
                m_fMoveVeloLimit_Min = max(m_fMoveVeloLimit_Min, spec.sSPEC.fVAR1); // fVar1 : �ּ� �̼����� (1.0 == 100%) �� ū ������ ���ŵȴ�.;
            if ( spec.sSPEC.fVAR2 >= 0.0f )
                m_fMoveVeloLimit_Max = min(m_fMoveVeloLimit_Max, spec.sSPEC.fVAR2); // fVar2 : �ִ� �̼����� (1.0 == 100%) �� ���� ������ ���ŵȴ�.;
        }
        break;
    } //switch
} //GLCHARLOGIC::UPDATE_DATA_SkillSpecial

void GLCHARLOGIC::UPDATE_DATA_SetItemBonus()
{
	if ( !GLUseFeatures::GetInstance().IsUsingSetItemUse() ) return;

	//��Ʈ ȿ���� ���� �ǰ� ���� �ʴ� ����Ÿ�Ե� ���� �ɼ� ó���� �ȴٸ� ���Ǵ� ������ ��ȯ ����� �Ѵ�
	SSKILLFACT			sNonuseSkilFact;
	SDURATION_DAMAGE	sNonuseDamage;

	//��Ʈȿ���� ���� ���� Ÿ��SSPEC_ADDON_LEVEL
	for( UINT i = 0; i < m_vInfoPutOnSetItem.size(); ++i )
	{
		const SSETITEM *sSetItem = GLItemSet::GetInstance().GetInfoSetItem(m_vInfoPutOnSetItem[i].dwSetID);
		if(NULL == sSetItem) continue;

		for( UINT j = 0; j < sSetItem->vecBonus.size(); ++j )
		{
			// ��Ʈ�����ۿ� ��� ������ �ɼ� Ȯ�� ó���� ���� �ʴ´� !!Ȯ�θ�!!
			if( !GLItemSet::GetInstance().IsUsableOpt(sSetItem->vecBonus[j].sSpec.emSPEC) ) continue;

			if( m_vInfoPutOnSetItem[i].vItemID.size() >= sSetItem->vecBonus[j].byTerms )
				UPDATE_DATA_SkillSpecial( sNonuseSkilFact, sSetItem->vecBonus[j].sSpec, &sNonuseDamage, static_cast<DWORD> (i) );
		}
	}
}

void GLCHARLOGIC::UPDATE_DATA_SkillFacts(
	float		fElapsedTime,
	BOOL		bClient, 
	int&		nSUM_PA,
	int&		nSUM_SA, 
	int&		nSUM_MA,
	SDURATION_DAMAGE*	sDurDamage )
{
	for (size_t i=0; i<SKILLFACT_SIZE; ++i)
	{
		SSKILLFACT& sSKEFF = GetSkillFact( i );
		if (sSKEFF.sNATIVEID == NATIVEID_NULL())
            continue;
	
		if (!sSKEFF.bEnable)
            continue;

        if ( !sSKEFF.IsAgeInfinite() )
            sSKEFF.fAGE -= fElapsedTime;

		//	Ŭ���̾�Ʈ�� �ƴ� ��� ���⼭ ��ų ����Ʈ�� ��Ȱ��ȭ, Ŭ���̾�Ʈ�� UpdateSkillEffect()���� ��.
        if ( !bClient )
        {
			// �ݰ��� ���������� �̷������ ��� �ݰ��� ���� �ʴ´�;
			if ( false == m_SkillCAIndex.IsAlive( i ) )
			{
				UPDATE_DATA_END( i );
			}
			else if ( false == sSKEFF.IsAlive() )
			{
				UPDATE_DATA_END( i, TRUE );
			}
        } //if

		if ( bClient ) 
		{
			PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( sSKEFF.sNATIVEID );
			if ( pSkill ) 
				m_bActionType[pSkill->m_sBASIC.emACTION_TYPE] = true;
		} //if

        // Note : Ż �� Ÿ�� ȿ�� ����.
        bool bKeepForType = true;

        if ( m_emVehicle != EMVEHICLE_OFF )
        {
            for ( int emType = 0; emType < GLCONST_CHAR::wNON_APPLY_SKILL_TYPE_ON_VEHICLE_NUM; emType++ )
            {
                if ( sSKEFF.emTYPE == GLCONST_CHAR::wNON_APPLY_SKILL_TYPE_ON_VEHICLE[ emType ] )
                {
                    bKeepForType = false;
                    break;
                }
            }
        }

        if ( bKeepForType )
        {
		    switch ( sSKEFF.emTYPE )
		    {
		    case SKILL::EMFOR_VARHP:
				m_fBasicRateHP += sSKEFF.fMVAR;
				m_fVariationRateHP += sSKEFF.fMVAR;
				break;
		    case SKILL::EMFOR_VARMP:
				m_fVariationRateMP += sSKEFF.fMVAR;
				break;
		    case SKILL::EMFOR_VARSP:
				m_fVariationRateSP += sSKEFF.fMVAR;
				break;
		    case SKILL::EMFOR_VARAP:
				m_fBasicRateHP += sSKEFF.fMVAR;
			    m_fVariationRateHP += sSKEFF.fMVAR;
			    m_fVariationRateMP += sSKEFF.fMVAR;
			    m_fVariationRateSP += sSKEFF.fMVAR;
			    break;

		    case SKILL::EMFOR_DEFENSE:
				m_nDEFENSE_SKILL += (int) sSKEFF.fMVAR;
				break;
		    case SKILL::EMFOR_HITRATE:
				m_nSUM_HIT += int(sSKEFF.fMVAR);
				break;
		    case SKILL::EMFOR_AVOIDRATE:
				m_nSUM_AVOID += int(sSKEFF.fMVAR);
				break;
		    case SKILL::EMFOR_VARDAMAGE:
				m_gdDAMAGE_SKILL.VAR_PARAM ( int(sSKEFF.fMVAR) );
				break;
		    case SKILL::EMFOR_VARDEFENSE:
				m_nDEFENSE_SKILL += int(sSKEFF.fMVAR);
				break;
		    case SKILL::EMFOR_PA:
				nSUM_PA += int(sSKEFF.fMVAR);
				break;
		    case SKILL::EMFOR_SA:
				nSUM_SA += int(sSKEFF.fMVAR);
				break;
		    case SKILL::EMFOR_MA:
				nSUM_MA += int(sSKEFF.fMVAR);
				break;
		    case SKILL::EMFOR_HP_RATE:
				m_fHP_RATE += sSKEFF.fMVAR;
				break;
		    case SKILL::EMFOR_MP_RATE:
				m_fMP_RATE += sSKEFF.fMVAR;
				break;
		    case SKILL::EMFOR_SP_RATE:
				m_fSP_RATE += sSKEFF.fMVAR;
				break;
		    case SKILL::EMFOR_RESIST:
				m_sSUMRESIST_SKILL += int(sSKEFF.fMVAR);
				break;
		    case SKILL::EMFOR_SUMMON_ACTIVETIME:
				m_fSUMMONACTIVETIME += sSKEFF.fMVAR;
				break;
		    };
        }

		for ( DWORD j = 0; j < sSKEFF.vecADDON.size(); ++j )
		{
            // Note : Ż �� Ÿ�� ȿ�� ����.
            bool bKeepImpactAddonSkill = true;
            
            if ( m_emVehicle != EMVEHICLE_OFF )
            {
                for ( int emAddon = 0; emAddon < GLCONST_CHAR::wNON_APPLY_SKILL_IMPACT_ON_VEHICLE_NUM; emAddon++ )
                {
                    if ( sSKEFF.vecADDON[j].emADDON == GLCONST_CHAR::wNON_APPLY_SKILL_IMPACT_ON_VEHICLE[ emAddon ] )
                    {
                        bKeepImpactAddonSkill = false;
                        break;
                    }
                }
            }

            if ( bKeepImpactAddonSkill )
				UPDATE_DATA_SkillAddon( sSKEFF.vecADDON[j], nSUM_PA, nSUM_SA, nSUM_MA );
		}

		for ( DWORD j = 0; j < sSKEFF.vecSPEC.size(); ++j )
		{
            // Note : Ż �� Ÿ�� ȿ�� ����.
            bool bKeepSpecAddonSkill = true;

            if ( m_emVehicle != EMVEHICLE_OFF )
            {
                for ( int emSpec = 0; emSpec < GLCONST_CHAR::wNON_APPLY_SKILL_SPEC_ON_VEHICLE_NUM; emSpec++ )
                {
                    if ( sSKEFF.vecSPEC[j].emSPEC == GLCONST_CHAR::wNON_APPLY_SKILL_SPEC_ON_VEHICLE[ emSpec ] )
                    {
                        bKeepSpecAddonSkill = false;
                        break;
                    }
                }
            }
            
			if ( bKeepSpecAddonSkill ) {
				UPDATE_DATA_SkillSpecial( sSKEFF, sSKEFF.vecSPEC[j], sDurDamage, static_cast<DWORD> (i) );

				
			}
		} //for
	} //for

} //GLCHARLOGIC::UPDATE_DATA_SkillFacts

void GLCHARLOGIC::UPDATE_DATA_BlowMulti( float fElapsedTime, BOOL bClient )
{
	for ( int i=0; i < EMBLOW_MULTI; ++i )
	{
		SSTATEBLOW &sSTATEBLOW = m_sSTATEBLOWS[i];
		if ( sSTATEBLOW.emBLOW == EMBLOW_NONE )		continue;

		m_bSTATEBLOW = TRUE;
		sSTATEBLOW.fAGE -= fElapsedTime;
		if ( !bClient && sSTATEBLOW.fAGE <= 0.0f )	DISABLEBLOW(i);

		switch ( sSTATEBLOW.emBLOW )
		{
		case EMBLOW_NUMB:
			m_fSTATE_MOVE_RATE += sSTATEBLOW.fSTATE_VAR1;			//	�̵��ӵ� ����.
			m_fSTATE_DELAY += sSTATEBLOW.fSTATE_VAR2;			//	������ ����.
			break;

		case EMBLOW_STUN:
			m_fSTATE_MOVE_RATE = 0.0f;								//	����.
			m_bSTATE_PANT = true;
			m_bSTATE_STUN = true;
			break;

		case EMBLOW_STONE:
			m_fSTATE_MOVE_RATE += sSTATEBLOW.fSTATE_VAR1;			//	�̵��ӵ� ����.
			m_fTempVarHP += sSTATEBLOW.fSTATE_VAR2*fElapsedTime;
			break;

		case EMBLOW_BURN:
			m_fTempVarHP += sSTATEBLOW.fSTATE_VAR2*fElapsedTime;
			break;

		case EMBLOW_FROZEN:
			m_fSTATE_MOVE_RATE += sSTATEBLOW.fSTATE_VAR1;			//	�̵��ӵ� ����.
			m_fSTATE_DAMAGE += sSTATEBLOW.fSTATE_VAR2;			//	����� ����.
			break;

		case EMBLOW_MAD:
			m_nSUM_HIT = 30;
			m_nSUM_AVOID = 30;
			m_bSTATE_PANT = true;
			break;

		case EMBLOW_POISON:
			m_fTempVarHP += sSTATEBLOW.fSTATE_VAR2*fElapsedTime;
			break;

		case EMBLOW_CURSE:
			m_fTempVarHP += sSTATEBLOW.fSTATE_VAR2*fElapsedTime;
			m_fVariationApplyRateMP = 0.0f;
			break;
		};
	} //for
} //GLCHARLOGIC::UPDATE_DATA_BlowMulti

void GLCHARLOGIC::UPDATE_DATA_QItemFact()
{
	if ( m_sQITEMFACT.IsACTIVE() )
	{
        // Note : Ż �� Ÿ�� ȿ�� ����.
        bool	bKeepQBox = true;
        if ( m_emVehicle != EMVEHICLE_OFF )
        {
            for ( int i = 0; i < GLCONST_CHAR::wNON_APPLY_QBOX_ON_VEHICLE_NUM; i++ )
            {
                // Note : Ż �� Ÿ�� ���� �� ������� QBOX ȿ����.
                if ( m_sQITEMFACT.emType == GLCONST_CHAR::wNON_APPLY_QBOX_ON_VEHICLE[ i ] )
                {
                    bKeepQBox = false;
                    break;
                }
            }
        }

        if ( bKeepQBox )
        {
 		    switch ( m_sQITEMFACT.emType )
		    {
		    case QUESTION_SPEED_UP:		
				m_fSKILL_MOVE += (m_sQITEMFACT.wParam1/100.0f);		
				break;

		    case QUESTION_CRAZY:
			    m_fSKILL_MOVE += (m_sQITEMFACT.wParam1/100.0f);
			    m_fATTVELO += (m_sQITEMFACT.wParam2/100.0f);
			    break;

		    case QUESTION_ATTACK_UP:	
				m_fDamageRate += (m_sQITEMFACT.wParam1/100.0f);		
				break;

		    case QUESTION_EXP_UP:				
				m_fEXP_RATE += (m_sQITEMFACT.wParam1/100.0f);
				m_fQItem_EXP_RATE += (m_sQITEMFACT.wParam1/100.0f);
				break;

            case QUESTION_LUCKY:		
                m_fGENITEM_RATE  += (m_sQITEMFACT.wParam1/100.0f);	
                m_fGENMONEY_RATE += (m_sQITEMFACT.wParam1/100.0f);		
                break;

		    case QUESTION_SPEED_UP_M:	
				m_fSKILL_MOVE += (m_sQITEMFACT.wParam1/100.0f);		
				break;

		    case QUESTION_MADNESS:
			    m_fSKILL_MOVE += (m_sQITEMFACT.wParam1/100.0f);
			    m_fATTVELO += (m_sQITEMFACT.wParam2/100.0f);
			    break;

		    case QUESTION_ATTACK_UP_M:	
				m_fDamageRate += (m_sQITEMFACT.wParam1/100.0f);		
				break;
		    }; //switch
        } //if
	} //if
} //GLCHARLOGIC::UPDATE_DATA_QItemFact

void GLCHARLOGIC::UPDATE_DATA_LandEffects()
{
	for ( int i = 0; i < static_cast<int>(m_vecLandEffect.size()); ++i ) 
	{
		SLANDEFFECT_VALUE landEffect = m_vecLandEffect[i];
		if( !landEffect.IsUse() )
			continue;

		CDebugSet::ToView( 3, i, "%d %2.2f", landEffect.emLandEffectType, landEffect.fValue );

		switch( landEffect.emLandEffectType ) 
		{
		case EMLANDEFFECT_ATK_SPEED: // ���ݼӵ�
			m_fATTVELO	  += landEffect.fValue;
			break;

		case EMLANDEFFECT_MOVE_SPEED: // �̵��ӵ�
			m_fSKILL_MOVE += landEffect.fValue;
			break;

		case EMLANDEFFECT_HP_RATE:	  // HP ������
			m_fHP_RATE    += landEffect.fValue;
			break;

		case EMLANDEFFECT_MP_RATE:    // MP ������
			m_fMP_RATE    += landEffect.fValue;
			break;

		case EMLANDEFFECT_RECOVER_RATE: // ȸ����
			m_fBasicRateHP += landEffect.fValue;
			m_fVariationRateHP += landEffect.fValue;
			m_fVariationRateMP += landEffect.fValue;
			m_fVariationRateSP += landEffect.fValue;
			break;

		case EMLANDEFFECT_DAMAGE_RATE:	// ����ġ ��ȭ��
			m_fDamageRate += landEffect.fValue;
			break;

		case EMLANDEFFECT_DEFENSE_RATE:	// ���ġ ��ȭ��
			m_fDefenseRate += landEffect.fValue;
			break;

		case EMLANDEFFECT_RESIST_RATE:	// ���� ��ġ			
			m_sSUMRESIST_SKILL += (int)landEffect.fValue;
			break;

		case EMLANDEFFECT_SAFE_ZONE:
			m_bSafeZone = true;
			break;
		} //switch
	} //for
} //GLCHARLOGIC::UPDATE_DATA_LandEffects

void GLCHARLOGIC::UPDATE_DATA_MapEffects()
{
	for ( int i = 0; i < static_cast<int>(m_vecMapEffect.size()); ++i ) 
	{
		SMAP_EFFECT mapEffect = m_vecMapEffect[i];

		if ( mapEffect.emType == EMMAP_EFFECT_NONE ) continue;

		CDebugSet::ToView( 5, i, "%d %2.2f", mapEffect.emType, mapEffect.fRate );

		switch( mapEffect.emType ) 
		{
		case EMMAP_EFFECT_ATTACK_SPEED: // ���ݼӵ�
			m_fATTVELO	  += mapEffect.fRate;
			break;

		case EMMAP_EFFECT_MOVE_SPEED: // �̵��ӵ�
			m_fSKILL_MOVE += mapEffect.fRate;
			break;

		case EMMAP_EFFECT_HP_RATE:	  // HP ������
			m_fHP_RATE    += mapEffect.fRate;
			break;

		case EMMAP_EFFECT_MP_RATE:    // MP ������
			m_fMP_RATE    += mapEffect.fRate;
			break;

		case EMMAP_EFFECT_ALL_RATE: // ȸ����
			m_fBasicRateHP += mapEffect.fRate;
			m_fVariationRateHP += mapEffect.fRate;
			m_fVariationRateMP += mapEffect.fRate;
			m_fVariationRateSP += mapEffect.fRate;
			break;

		case EMMAP_EFFECT_DAMAGE_RATE:	// ����ġ ��ȭ��
			m_fDamageRate += mapEffect.fRate;
			break;

		case EMMAP_EFFECT_DEFENSE_RATE:	// ���ġ ��ȭ��
			m_fDefenseRate += mapEffect.fRate;
			break;
		} //switch
	} //for
} //GLCHARLOGIC::UPDATE_DATA_MapEffects

void GLCHARLOGIC::UPDATE_DATA_PetSkill( float fElapsedTime )
{
	//		  ��ġ���� �ִ�ġ�� ����ȴ� (�ߺ� ��ų)
	FLOAT fPETSKILL_INCR_HP		= 0.0f;
	FLOAT fPETSKILL_INCR_MP		= 0.0f;
	FLOAT fPETSKILL_INCR_SP		= 0.0f;
	FLOAT fPETSKILL_DAMAGERATE	= 0.0f;
	FLOAT fPETSKILL_DEFENSERATE	= 0.0f;

	for ( int i=0; i<MAX_PETSKILLSLOT; ++i )
	{
		if ( m_sPETSKILLFACT[i].sNATIVEID !=NATIVEID_NULL() )
		{
			switch ( m_sPETSKILLFACT[i].emTYPE )
			{
			case SKILL::EMFOR_PET_HEAL:				//	������ HP ȸ�� �ӵ� ���
                fPETSKILL_INCR_HP = __max(fPETSKILL_INCR_HP, m_sPETSKILLFACT[i].fMVAR);
				break;
			case SKILL::EMFOR_PET_SUPPROT:			//	������ HP, MP, SP ȸ���� ���
                fPETSKILL_INCR_HP = __max(fPETSKILL_INCR_HP, m_sPETSKILLFACT[i].fMVAR);
                fPETSKILL_INCR_MP = __max(fPETSKILL_INCR_MP, m_sPETSKILLFACT[i].fMVAR);
                fPETSKILL_INCR_SP = __max(fPETSKILL_INCR_SP, m_sPETSKILLFACT[i].fMVAR);
				break;
			case SKILL::EMFOR_PET_BACKUP_ATK:		//	������ ���ݷ�(��) ���
                fPETSKILL_DAMAGERATE = __max(fPETSKILL_DAMAGERATE, m_sPETSKILLFACT[i].fMVAR/100.0f);
				break;
			case SKILL::EMFOR_PET_BACKUP_DEF:		//	������ ����(��) ���
                fPETSKILL_DEFENSERATE = __max(fPETSKILL_DEFENSERATE, m_sPETSKILLFACT[i].fMVAR/100.0f);
				break;
			};

            // ��ų ������Ÿ�� ����
            if ( !m_sPETSKILLFACT[i].IsAgeInfinite() )
                m_sPETSKILLFACT[i].fAGE -= fElapsedTime;    
		}
	}

	m_fVariationRateHP  += fPETSKILL_INCR_HP;
	m_fVariationRateMP += fPETSKILL_INCR_MP;
	m_fVariationRateSP += fPETSKILL_INCR_SP;
	m_fDamageRate  += fPETSKILL_DAMAGERATE;
	m_fDefenseRate += fPETSKILL_DEFENSERATE;

} //GLCHARLOGIC::UPDATE_DATA_PetSkill

void GLCHARLOGIC::UPDATE_DATA_Stats( int& nSUM_PA, int& nSUM_SA, int& nSUM_MA )
{
	if ( m_sSUMSTATS_SKILL != m_sSUMSTATS )
	{	
		EMCHARINDEX emCI = GETCHARINDEX ();										//	�ɸ����� ���� �ε���.
		const GLCONST_CHARCLASS &cCHARCONST = GLCONST_CHAR::cCONSTCLASS[emCI];	//	�ɸ����� �����.

		nSUM_PA += static_cast<int>(( ( m_sSUMSTATS_SKILL.wPow - m_sSUMSTATS.wPow ) * cCHARCONST.fPA_POW ) + 
						   ( ( m_sSUMSTATS_SKILL.wDex - m_sSUMSTATS.wDex ) * cCHARCONST.fPA_DEX ));		

		nSUM_SA += static_cast<int>(( ( m_sSUMSTATS_SKILL.wPow - m_sSUMSTATS.wPow ) * cCHARCONST.fSA_POW ) + 
						   ( ( m_sSUMSTATS_SKILL.wDex - m_sSUMSTATS.wDex ) * cCHARCONST.fSA_DEX ));		
		
		nSUM_MA += static_cast<int>(( ( m_sSUMSTATS_SKILL.wDex - m_sSUMSTATS.wDex ) * cCHARCONST.fMA_DEX ) + 
						   ( ( m_sSUMSTATS_SKILL.wSpi - m_sSUMSTATS.wSpi ) * cCHARCONST.fMA_SPI ) +		
						   ( ( m_sSUMSTATS_SKILL.wInt - m_sSUMSTATS.wInt ) * cCHARCONST.fMA_INT ));

		//	Note : ������, ȸ���� ���.
		m_nSUM_HIT += int ( ( m_sSUMSTATS_SKILL.wDex - m_sSUMSTATS.wDex ) * cCHARCONST.fHIT_DEX );
		m_nSUM_AVOID += int ( ( m_sSUMSTATS_SKILL.wDex - m_sSUMSTATS.wDex ) * cCHARCONST.fAVOID_DEX );
		m_nDEFENSE_SKILL += int ( ( m_sSUMSTATS_SKILL.wDex - m_sSUMSTATS.wDex ) * cCHARCONST.fDEFENSE_DEX );
	}	
} //GLCHARLOGIC::UPDATE_DATA_Stats


void GLCHARLOGIC::UPDATE_DATA(float fTime, float fElapsedTime, BOOL bClient, float fCONFT_POINT_RATE)
{
	//	���� ��ų ������ ����.
	UPDATESKILLDELAY ( fElapsedTime );

	PrevUPDATE_DATA(fElapsedTime);

	int nSUM_PA(0), nSUM_SA(0), nSUM_MA(0);
	SDURATION_DAMAGE sDurDamage[EMELEMENT_MAXNUM];

	UPDATE_DATA_SkillFacts( fElapsedTime, bClient, nSUM_PA, nSUM_SA, nSUM_MA, sDurDamage );

	UPDATE_DATA_BlowMulti( fElapsedTime, bClient );

	UPDATE_DATA_QItemFact();
	
	UPDATE_DATA_SetItemBonus();

	//	Memo :	? ������ �̺�Ʈ ����
	if( m_sEVENTFACT.IsACTIVE( EMGM_EVENT_SPEED ) )
		m_fSKILL_MOVE += (m_sEVENTFACT.wSpeed/100.0f);

	if( m_sEVENTFACT.IsACTIVE( EMGM_EVENT_ASPEED ) )
		m_fATTVELO += (m_sEVENTFACT.wASpeed/100.0f);

	if( m_sEVENTFACT.IsACTIVE( EMGM_EVENT_ATTACK ) )
		m_fDamageRate += (m_sEVENTFACT.wAttack/100.0f);

    // IP Event ����.
    if ( m_sEVENTFACT.IsIPEvent() )
    {
        DWORD AttackValue_IPEvent = ran_event::GLIPEvent::Instance()->GetAttackValue();
        DWORD AttackSpeed_IPEvent = ran_event::GLIPEvent::Instance()->GetAttackSpeed();
        DWORD MoveSpeed_IPEvent = ran_event::GLIPEvent::Instance()->GetMoveSpeed();

        DWORD HPRecovery_IPEvent = ran_event::GLIPEvent::Instance()->GetHPRecovery();
        DWORD MPRecovery_IPEvent = ran_event::GLIPEvent::Instance()->GetMPRecovery();
        DWORD SPRecovery_IPEvent = ran_event::GLIPEvent::Instance()->GetSPRecovery();
        DWORD AllRecovery_IPEvent = ran_event::GLIPEvent::Instance()->GetAllRecovery();

        DWORD ItemDrop_IPEvent = ran_event::GLIPEvent::Instance()->GetItempDrop();
        DWORD MoneyDrop_IPEvent = ran_event::GLIPEvent::Instance()->GetMoneyDrop();
        DWORD EXP_IPEvent = ran_event::GLIPEvent::Instance()->GetExp();

        if ( AttackValue_IPEvent != 0 )
            m_fDamageRate += (AttackValue_IPEvent*0.01f);

        if ( AttackSpeed_IPEvent != 0 )
            m_fATTVELO += (AttackSpeed_IPEvent*0.01f);

        if ( MoveSpeed_IPEvent != 0 )
            m_fSKILL_MOVE += (MoveSpeed_IPEvent*0.01f);

        if ( HPRecovery_IPEvent != 0 )
		{
			m_fBasicRateHP += (HPRecovery_IPEvent*0.01f);
            m_fVariationRateHP += (HPRecovery_IPEvent*0.01f);
		}

        if ( MPRecovery_IPEvent != 0 )
            m_fVariationRateMP += (MPRecovery_IPEvent*0.01f);

        if ( SPRecovery_IPEvent != 0 )
            m_fVariationRateSP += (SPRecovery_IPEvent*0.01f);

        if ( AllRecovery_IPEvent != 0 )
        {
			m_fBasicRateHP += (HPRecovery_IPEvent*0.01f);
            m_fVariationRateHP += (AllRecovery_IPEvent*0.01f);
            m_fVariationRateMP += (AllRecovery_IPEvent*0.01f);
            m_fVariationRateSP += (AllRecovery_IPEvent*0.01f);
        }
    }
    //

	// Note : ���� ������ų ó��
	UPDATE_DATA_PetSkill( fElapsedTime );

	// ���� ȿ�� ó��
	UPDATE_DATA_LandEffects();

	//	�� ȿ����
	UPDATE_DATA_MapEffects();

	//	���� ������ ����
	for ( int i = 0; i < EMELEMENT_MAXNUM; ++i ) 
	{
		if ( !m_sDurDamage[i].IsEqual( sDurDamage[i] ) )
		{
			m_sDurDamage[i] = sDurDamage[i];
			m_sDurDamage[i].m_fTime = m_sDurDamage[i].m_fTickTime;
		}
	}

	//	��ųȿ���� ������ Ʋ������ ����� �ٽ� �Ѵ�.
	UPDATE_DATA_Stats( nSUM_PA, nSUM_SA, nSUM_MA );

	//	Note : HP, MP, SP �ѷ� Ȯ��.
	//
	UPDATE_MAX_POINT ( fCONFT_POINT_RATE );

	//	����. ��ȭ�� �ݿ�.
	m_nDEFENSE_SKILL = int ( m_nDEFENSE_SKILL * m_fDefenseRate );
	if ( m_nDEFENSE_SKILL < 0 )
		m_nDEFENSE_SKILL = 1;
	
	//���߷�, ��ȭ�� �ݿ�.
	m_nSUM_HIT = int ( m_nSUM_HIT * m_fHitRate );	

	//ȸ���� ��ȭ�� �ݿ�
	m_nSUM_AVOID = int ( m_nSUM_AVOID * m_fAvoidRate );	

	m_sSUMRESIST_SKILL.LIMIT();

	//	���� �ӵ�. ( ���� skill �Ӽ� ����� �κп� state, passive skill �Ӽ� ����. )
	m_fATTVELO += m_fSTATE_MOVE_RATE + m_sSUM_PASSIVE.m_fATTVELO;

	m_fSTATE_DELAY += m_fSKILLDELAY + m_sSUM_PASSIVE.m_fSKILLDELAY;

	//	���� ���ɰŸ�.
	m_wSUM_ATTRANGE = m_wATTRANGE + m_wATTRANGE_SKILL;

	//	��ų ȿ���� ����� ������ ���� �� ���ݷ�. ��ȭ�� �ݿ�;
	m_gdDAMAGE_BASIC = m_gdDAMAGE_SKILL;

	//	����ġ ���ġ ����ġ ����.
	const float fAllHitRatioToMelee = GetIncR_AllHitRatioToMelee();
	const float fAllHitRatioToRange = GetIncR_AllHitRatioToRange();
	const float fAllHitRatioToMagic = GetIncR_AllHitRatioToMagic();
	const int nAllHitRatioToMelee = static_cast< const int >( m_nSUM_HIT * fAllHitRatioToMelee );
	const int nAllHitRatioToRange = static_cast< const int >( m_nSUM_HIT * fAllHitRatioToRange );
	const int nAllHitRatioToMagic = static_cast< const int >( m_nSUM_HIT * fAllHitRatioToMagic );

	const int nTotalMelee(m_powerAttribute[SKILL::EMAPPLY_MELEE] + nSUM_PA + m_nPA_ADD + nAllHitRatioToMelee);
	m_sumPowerAttribute[SKILL::EMAPPLY_MELEE] = nTotalMelee < 0 ? 0 : nTotalMelee;

	const int nTotalRange(m_powerAttribute[SKILL::EMAPPLY_RANGE] + nSUM_SA + m_nSA_ADD + nAllHitRatioToRange);
	m_sumPowerAttribute[SKILL::EMAPPLY_RANGE] = nTotalRange < 0 ? 0 : nTotalRange;

	const int nTotalMagic(m_powerAttribute[SKILL::EMAPPLY_MAGIC] + nSUM_MA + m_nMA_ADD + nAllHitRatioToMagic);
	m_sumPowerAttribute[SKILL::EMAPPLY_MAGIC] = nTotalMagic < 0 ? 0 : nTotalMagic;

	// ���ݷ�;
	const float fAllMPRatioToAttack = GetIncR_AllMPRatioToAttack();
	const int nAllMPRatioToAttack = static_cast< const int >( m_sMP.nMax * fAllMPRatioToAttack );

	const int nDamageLow = m_gdDAMAGE_BASIC.nNow + m_sSUMITEM.gdDamage.nNow + nAllMPRatioToAttack;
	const int nDamageHigh = m_gdDAMAGE_BASIC.nMax + m_sSUMITEM.gdDamage.nMax + nAllMPRatioToAttack;

	m_gdDAMAGE_BASIC.nNow = nDamageLow < 0 ? 0 : nDamageLow;
	m_gdDAMAGE_BASIC.nMax = nDamageHigh < 0 ? 0 : nDamageHigh;	
	
	if ( ISLONGRANGE_ARMS() )
		m_gdDAMAGE_BASIC.VAR_PARAM (m_sumPowerAttribute[SKILL::EMAPPLY_RANGE]);	//	��Ÿ� ����.
	else
		m_gdDAMAGE_BASIC.VAR_PARAM (m_sumPowerAttribute[SKILL::EMAPPLY_MELEE]);	//	���� ����.	
	
	// ��ųȿ�� : ��ü ������ ���� ��ȭ��
	m_gdDAMAGE_BASIC.nNow *= m_fDamageRate;
	m_gdDAMAGE_BASIC.nMax *= m_fDamageRate;

	m_gdDAMAGE_BASIC.nNow = m_gdDAMAGE_BASIC.nNow < 0 ? 0 : m_gdDAMAGE_BASIC.nNow;
	m_gdDAMAGE_BASIC.nMax = m_gdDAMAGE_BASIC.nMax < 0 ? 0 : m_gdDAMAGE_BASIC.nMax;	

	const EMSLOT emRHand(GetCurRHand());
	const EMSLOT emLHand(GetCurLHand());

	if ( m_pITEMS[emRHand] )
	{
		m_emITEM_ATT = CONVERT_ITEMATT( m_pITEMS[emRHand]->sSuitOp.emAttack );
		m_wATTRANGE = GET_ATTACKDISTANCE_USING_ITEMATT( m_emITEM_ATT );

		//m_emITEM_ATT = m_pITEMS[emRHand]->sSuitOp.emAttack;
		//m_wATTRANGE = m_pITEMS[emRHand]->sSuitOp.wAttRange;
	}
	else
	{
		m_emITEM_ATT = ITEMATT_NOTHING;
		m_wATTRANGE = GLCONST_CHAR::wMAXATRANGE_SHORT;
	}

	//	Note : ���ڶ�� ��ġ�� �ջ� ( ��ų�� ����� ���� ��ġ�� ���� ) 
	//
	m_wACCEPTP = 0;
	m_wACCEPTP += CALC_ACCEPTP_EX ( GET_SLOT_ITEM(emLHand) );
	m_wACCEPTP += CALC_ACCEPTP_EX ( GET_SLOT_ITEM(emRHand) );

	//	Note : "SP�Һ�� ���Ǵ� �䱸��ġ" + "������ ���� �Һ� SP �ջ�" + ��Ʈ ������ȿ�� SP �Һ� �ɼǰ�
	//
	m_wSUM_DisSP = m_wACCEPTP + m_wACCEPTP_SKILL;

	SITEM* pRHAND = GET_SLOT_ITEMDATA ( emRHand );
	SITEM* pLHAND = GET_SLOT_ITEMDATA ( emLHand );

	if ( pRHAND )
		m_wSUM_DisSP += pRHAND->sSuitOp.wReqSP;

	if ( pLHAND )
		m_wSUM_DisSP += pLHAND->sSuitOp.wReqSP;

    // ��ȭ ���� ���� �ð�;
	float fElap = (fElapsedTime/GLCONST_CHAR::fUNIT_TIME);

    m_fVariationApplyRateHP = max(0, m_fVariationApplyRateHP);  // ��ȭ ��������� -���� �� �� ����;
    m_fVariationApplyRateMP = max(0, m_fVariationApplyRateMP);
    m_fVariationApplyRateSP = max(0, m_fVariationApplyRateSP);

	float fRValue = RF_FEIGN_DEATH( m_EffSkillVarSet).fRateValue;

	m_fVariationApplyRateHP *= fRValue;
	m_fVariationApplyRateMP *= fRValue;
	m_fVariationApplyRateSP *= fRValue;

	float fTempVarHP = m_fTempVarHP * fRValue;
	float fTempVarMP = m_fTempVarMP * fRValue;
	float fTempVarSP = m_fTempVarSP * fRValue;

    // HP��ȭ ������ = (( �ִ�HP * HP��ȭ�� ) + �⺻HP��ȭ�� + ������ HP��ȭ��) * HP��ȭ �������;
	float fVariationHP = fElap * (((float(m_sHP.nMax) * m_fVariationRateHP) + GLCONST_CHAR::fHP_INC + m_sSUMITEM.fInc_HP) * m_fVariationApplyRateHP);
	float fVariationMP = fElap * (((float(m_sMP.nMax) * m_fVariationRateMP) + GLCONST_CHAR::fMP_INC + m_sSUMITEM.fInc_MP) * m_fVariationApplyRateMP);
	float fVariationSP = fElap * (((float(m_sSP.nMax) * m_fVariationRateSP) + GLCONST_CHAR::fSP_INC + m_sSUMITEM.fInc_SP) * m_fVariationApplyRateSP);
    float fVariationCP = 0.0f;

	GLOGICEX::UPDATE_POINT ( m_sHP, m_fTempVarHP, fVariationHP, 1 );  // HP��ȭ�����δ� ������ �ʴ´�;
	GLOGICEX::UPDATE_POINT ( m_sMP, m_fTempVarMP, fVariationMP, 0 );
	GLOGICEX::UPDATE_POINT ( m_sSP, m_fTempVarSP, fVariationSP, 0 );

    m_fCP_Timer += fElapsedTime;
    SET_CP_DUR ( m_sCP, m_fTempVarCP, m_fCP_Timer );	

	UPDATE_DATA_Additional(fElapsedTime);
}
float GLCHARLOGIC::GETCRITICALVELO()
{
	return m_sSUMITEM.fIncR_Critical < 0.0f ? 0.0f : m_sSUMITEM.fIncR_Critical;
}
float GLCHARLOGIC::GETATTVELO()
{
	float fAttVeloRes = m_fATTVELO;
    fAttVeloRes = GLOGICEX::GetAttackVelocity(fAttVeloRes, m_sSUMITEM.fIncR_AtkSpeed);
	if ( m_bParty == FALSE && m_sSUMITEM.fIncR_AtkSpeed_SoloPlay )
		fAttVeloRes = GLOGICEX::GetAttackVelocity(fAttVeloRes, m_sSUMITEM.fIncR_AtkSpeed_SoloPlay);
	if ( m_bParty == TRUE && m_sSUMITEM.fIncR_AtkSpeed_PartyPlay )
		fAttVeloRes = GLOGICEX::GetAttackVelocity(fAttVeloRes, m_sSUMITEM.fIncR_AtkSpeed_PartyPlay);

	return fAttVeloRes;
	//float fATTVELO = m_fATTVELO + m_sSUMITEM.fIncR_AtkSpeed;
	//return fATTVELO<0.0f?0.0f:fATTVELO;
}

float GLCHARLOGIC::GETMOVEVELO()
{
    return GLOGICEX::GetMoveBaseVelocity(m_fSTATE_MOVE_RATE, m_fSKILL_MOVE, m_sSUMITEM.fIncR_MoveSpeed, m_fOPTION_MOVE);
	//float fMOVE = m_fSTATE_MOVE_RATE + m_fSKILL_MOVE + m_fOPTION_MOVE + m_sSUMITEM.fIncR_MoveSpeed;
	//return fMOVE<0.0f?0.0f:fMOVE;
}

float GLCHARLOGIC::GETEXPVELO()
{
	return ( (m_fQItem_EXP_RATE + m_fSkill_EXP_RATE + m_fRANDOM_BOX_EXP_RATE + 1) * m_sSUMITEM.fExpMultiple );
}
float GLCHARLOGIC::GETCRITICAL_ITEM()
{
	return m_sSUMITEM.fInc_Critical * 0.01f;
}
float GLCHARLOGIC::GETATT_ITEM()
{
	float fATTVELO = m_sSUMITEM.fInc_AtkSpeed / 100;
	return fATTVELO;
}

float GLCHARLOGIC::GETMOVE_ITEM()
{
	float fMOVE = ((m_sSUMITEM.fInc_MoveSpeed + m_fSTATE_MOVE_VALUE)/ GLCONST_CHAR::cCONSTCLASS[m_CHARINDEX].fRUNVELO);
	return fMOVE<0.0f?0.0f:fMOVE;
}
float GLCHARLOGIC::GETCRITICAL_TOTAL()
{
	return GETCRITICALVELO() + GETCRITICAL_ITEM();
}
float GLCHARLOGIC::GETATT_TOTAL()
{
	return GETATTVELO() + GETATT_ITEM();
}

float GLCHARLOGIC::GETMOVE_TOTAL()
{
    float _fMoveVelo = GETMOVEVELO() + GETMOVE_ITEM();
    if (m_fMoveVeloLimit_Min > 0.0f)
        _fMoveVelo = max(_fMoveVelo, m_fMoveVeloLimit_Min);

    _fMoveVelo = min(_fMoveVelo, m_fMoveVeloLimit_Max);

    return _fMoveVelo;
}

// EMSLOTCHECK_RESULT	GLCHARLOGIC::CHECKSLOT_ITEM ( const SITEMCUSTOM& sItemCustom, EMSLOT emSlot )
// {
// 	const SITEM* pItem = GLogicData::GetInstance().GetItem ( sItemCustom.GetNativeID() );
// 	if ( pItem==NULL )	return EMSLOTCHECK_FAILED;
// 
// 	// ī�� �������� ��� ��ó���µ� �������ִ� ���¿����� ������ �Ұ��� �ϵ��� ����;
// 	if ( pItem->sSuitOp.emSuit == SUIT_ACCESSORIES && (emSlot == SLOT_LACCESSORIES || emSlot == SLOT_RACCESSORIES) )
// 	{
// 		if ( sItemCustom.wTurnNum > 1 )
// 		{
// 			return EMSLOTCHECK_NUMBEROF_ACCESSORIES;
// 		}
// 	}
// 
// 	return CHECKSLOT_ITEM(sItemCustom.GetNativeID(), emSlot);
// }

//	�ܼ��� ������ġ ������ �˻�.
EMSLOTCHECK_RESULT GLCHARLOGIC::CHECKSLOT_ITEM ( SNATIVEID sNativeID, EMSLOT emSlot )
{
	if ( sNativeID==NATIVEID_NULL() )		return EMSLOTCHECK_FAILED;

	const SITEM* pItem = GLogicData::GetInstance().GetItem ( sNativeID );
	if ( pItem==NULL )	return EMSLOTCHECK_FAILED;

	const SITEM &sItem = *pItem;

	if ( false ==sItem.IsSuitable() )
		return EMSLOTCHECK_FAILED;

	if ( sItem.sSuitOp.IsSTATE(EMITEM_SUIT_UNIQUE) == true )
	{ // ���� �������̰�, ������ �Ѱ� �̻��� ��� ��ȿ�� �˻縦 ���־�� ��;		
		switch ( emSlot )
		{ // �ݴ��� �ڸ��� ���� �������� ������ ���Ƿ� ����Ÿ�� ���� �� ��(��ŷ);
		case SLOT_LFINGER:
			if ( GET_SLOT_ITEM(SLOT_RFINGER).GetNativeID() == sNativeID )
				return EMSLOTCHECK_FAILED_UNIQUE; 
			break;
		case SLOT_RFINGER:
			if ( GET_SLOT_ITEM(SLOT_LFINGER).GetNativeID() == sNativeID )
				return EMSLOTCHECK_FAILED_UNIQUE;
			break;
		case SLOT_LACCESSORIES:
			if ( GET_SLOT_ITEM(SLOT_RACCESSORIES).GetNativeID() == sNativeID )
				return EMSLOTCHECK_FAILED_UNIQUE;
			break;
		case SLOT_RACCESSORIES:
			if ( GET_SLOT_ITEM(SLOT_LACCESSORIES).GetNativeID() == sNativeID )
				return EMSLOTCHECK_FAILED_UNIQUE;
			break;
		}
	}

	//	�ش� ���Կ� ���������� SUIT����.
	EMSUIT emSuit= SLOT_2_SUIT ( emSlot );
	if ( sItem.sSuitOp.emSuit != emSuit )	return EMSLOTCHECK_FAILED;

	/*	��ȹ ��û�� ���� �ּ�ó��
	if ( sItem.sBasicOp.emItemType  == ITEM_REVIVE )
	{
		if ( emSlot != SLOT_NECK )
			return EMSLOTCHECK_FAILED;
	}

	if ( sItem.sBasicOp.emItemType  == ITEM_ANTI_DISAPPEAR   ||
		 sItem.sBasicOp.emItemType  == ITEM_INC_GRINDINGRATE ||
		 sItem.sBasicOp.emItemType  == ITEM_ANTI_RESET )
	{
		// ��� ����� ����
		if ( emSlot != SLOT_NECK && emSlot != SLOT_WRIST )
			return EMSLOTCHECK_FAILED;
	}
	*/

	if ( sItem.sBasicOp.emItemType == ITEM_VEHICLE )
	{
		if ( emSlot != SLOT_VEHICLE ) return EMSLOTCHECK_FAILED;
	}

	// SLOT_RHAND, SLOT_RHAND_S, SLOT_LHAND, SLOT_LHAND_S �� �ϳ��̴�;
	if ( SUIT_HANDHELD == emSuit )
	{
		// ��չ��� �������̶�� ó���Ѵ�;
		if ( sItem.sSuitOp.IsBOTHHAND() )
		{
			if ( SLOT_RHAND != emSlot && SLOT_RHAND_S != emSlot )
				return EMSLOTCHECK_FAILED;
		}
		// �Ѽչ��� �������̶�� ó���Ѵ�;
		else
		{
			// ��� ��� ��밡���� �������� �ƴ϶�� ó���Ѵ�;
			if ( HAND_BOTH != sItem.sSuitOp.emHand )
			{
				// ������ �������ε� ������ ������ �ƴ϶�� �����Ѵ�;
				if ( HAND_RIGHT == sItem.sSuitOp.emHand && SLOT_RHAND != emSlot && SLOT_RHAND_S != emSlot )
					return EMSLOTCHECK_FAILED;

				// �޼� �������ε� ���� ������ �ƴ϶�� �����Ѵ�;
				if ( HAND_LEFT == sItem.sSuitOp.emHand && SLOT_LHAND != emSlot && SLOT_LHAND_S != emSlot )
					return EMSLOTCHECK_FAILED;
			}

			switch ( emSlot )
			{
			case SLOT_RHAND:
			case SLOT_RHAND_S:
				{
					switch ( sItem.sBasicOp.emItemType )
					{
					case ITEM_ARROW:
					case ITEM_CHARM:
					case ITEM_BULLET:
					case ITEM_CHANGE_ACTORS_DISGUISE_INFO:
						return EMSLOTCHECK_FAILED;
					}
				}
				break;

			case SLOT_LHAND:
			case SLOT_LHAND_S:
				{
					EMSLOT emRHand;
					switch ( emSlot )
					{
					case SLOT_LHAND:
						emRHand = SLOT_RHAND;
						break;

					case SLOT_LHAND_S:
						emRHand = SLOT_RHAND_S;
						break;
					}

					// �޼� �������� ��� ������ �������� ������ ������ �Ұ����ϴ�;
					if ( NULL == m_pITEMS[ emRHand ] )
						return EMSLOTCHECK_FAILED;

					// �Ѽ� ������ �����Ѵ�;
					const EMCHARINDEX emClass = GETCHARINDEX();
					switch ( sItem.sBasicOp.emItemType )
					{
					case ITEM_ARROW:
						{
							// ȭ�� ����ÿ��� �����տ� Ȱ�� ������ ���� �ʴٸ� ���� �Ұ����ϴ�;
							if ( GLCI_ARCHER_M == emClass || GLCI_ARCHER_W == emClass )
							{
								if ( ITEMATT_BOW != m_pITEMS[ emRHand ]->sSuitOp.emAttack &&
									ITEMATT_WEAPON_STR != m_pITEMS[ emRHand ]->sSuitOp.emAttack &&
									ITEMATT_WEAPON_DEX != m_pITEMS[ emRHand ]->sSuitOp.emAttack &&
									ITEMATT_WEAPON_SPI != m_pITEMS[ emRHand ]->sSuitOp.emAttack )
								{
									return EMSLOTCHECK_FAILED;
								}
							}
						}
						break;

					case ITEM_CHARM:
						{
							// ���� ����ÿ��� �����տ� â�� ������ ���� �ʴٸ� ���� �Ұ����ϴ�;
							if ( GLCI_SPIRIT_M == emClass || GLCI_SPIRIT_W == emClass )
							{
								if ( ITEMATT_SPEAR != m_pITEMS[ emRHand ]->sSuitOp.emAttack &&
									ITEMATT_WEAPON_STR != m_pITEMS[ emRHand ]->sSuitOp.emAttack &&
									ITEMATT_WEAPON_DEX != m_pITEMS[ emRHand ]->sSuitOp.emAttack &&
									ITEMATT_WEAPON_SPI != m_pITEMS[ emRHand ]->sSuitOp.emAttack )
								{
									return EMSLOTCHECK_FAILED;
								}
							}
						}
						break;

					case ITEM_BULLET:
						{
							// �Ѿ� ����ÿ��� �����տ� ���� ������ ���� �ʴٸ� ���� �Ұ����ϴ�;
							if ( GLCI_SCIENTIST_M == emClass || GLCI_SCIENTIST_W == emClass )
							{
								if ( ITEMATT_PISTOL != m_pITEMS[emRHand]->sSuitOp.emAttack && 
									ITEMATT_RAILGUN != m_pITEMS[emRHand]->sSuitOp.emAttack && 
									ITEMATT_PORTALGUN != m_pITEMS[emRHand]->sSuitOp.emAttack &&
									ITEMATT_WEAPON_STR != m_pITEMS[ emRHand ]->sSuitOp.emAttack &&
									ITEMATT_WEAPON_DEX != m_pITEMS[ emRHand ]->sSuitOp.emAttack &&
									ITEMATT_WEAPON_SPI != m_pITEMS[ emRHand ]->sSuitOp.emAttack )
								{
									return EMSLOTCHECK_FAILED;
								}
							}
						}
						break;

					case ITEM_CHANGE_ACTORS_DISGUISE_INFO:
						{
							// ������ ����������� ����ÿ��� �����տ� ������ ���� �������� ������ ���� �ʴٸ� ���� �Ұ����ϴ�;
							if ( GLCI_ACTOR_M == emClass || GLCI_ACTOR_W == emClass )
							{
								if ( ITEMATT_ACTOR_HAMMER != m_pITEMS[emRHand]->sSuitOp.emAttack && 
									ITEMATT_ACTOR_SHIELD != m_pITEMS[emRHand]->sSuitOp.emAttack && 
									ITEMATT_ACTOR_UMBRELLA != m_pITEMS[emRHand]->sSuitOp.emAttack &&
									ITEMATT_WEAPON_STR != m_pITEMS[ emRHand ]->sSuitOp.emAttack &&
									ITEMATT_WEAPON_DEX != m_pITEMS[ emRHand ]->sSuitOp.emAttack &&
									ITEMATT_WEAPON_SPI != m_pITEMS[ emRHand ]->sSuitOp.emAttack )
								{
									return EMSLOTCHECK_FAILED;
								}
							}
						}
						break;
					}
				}
				break;
			}
		}
	}

	/*EMSLOT emRHand = GetCurRHand();
	EMSLOT emLHand = GetCurLHand();

	//	�տ� ��� ������ �Ѽտ��϶�.
	if ( emSuit==SUIT_HANDHELD )
	{
		//	��� �����.
		//
		if ( sItem.sSuitOp.IsBOTHHAND() )
		{
			if ( emSlot!= SLOT_RHAND && emSlot!= SLOT_RHAND_S  )											
				return EMSLOTCHECK_FAILED;
		}
		//	�Ѽ� �����.
		//
		else
		{
			if ( sItem.sSuitOp.emHand!=HAND_BOTH )	//	��� ��� �����ϸ� ���� ����.
			{
				if ( sItem.sSuitOp.emHand==HAND_RIGHT && emSlot!= SLOT_RHAND && emSlot!= SLOT_RHAND_S )
					return EMSLOTCHECK_FAILED;

				if ( sItem.sSuitOp.emHand==HAND_LEFT && emSlot!= SLOT_LHAND && emSlot!= SLOT_LHAND_S )
					return EMSLOTCHECK_FAILED;
			}

			if ( sItem.sBasicOp.emItemType==ITEM_ARROW )
			{
				//	ȭ�� ����ÿ��� �����տ� Ȱ�� ������ ���� �ʴٸ� ���� �Ұ���.
				if ( !m_pITEMS[emRHand] || m_pITEMS[emRHand]->sSuitOp.emAttack!=ITEMATT_BOW )	
					return EMSLOTCHECK_FAILED;
			}
			else if ( sItem.sBasicOp.emItemType==ITEM_CHARM )
			{
				//	���� ����ÿ��� �����տ� â�� ������ ���� �ʴٸ� ���� �Ұ���.
				if ( !m_pITEMS[emRHand] || m_pITEMS[emRHand]->sSuitOp.emAttack!=ITEMATT_SPEAR )	
					return EMSLOTCHECK_FAILED;
			}
			else if ( sItem.sBasicOp.emItemType==ITEM_BULLET )
			{
				//	���� ����ÿ��� �����տ� â�� ������ ���� �ʴٸ� ���� �Ұ���.
				if ( !m_pITEMS[emRHand] )
					return EMSLOTCHECK_FAILED;

				if ( m_pITEMS[emRHand]->sSuitOp.emAttack!=ITEMATT_PISTOL && 
					 m_pITEMS[emRHand]->sSuitOp.emAttack!=ITEMATT_RAILGUN && 
					 m_pITEMS[emRHand]->sSuitOp.emAttack!=ITEMATT_PORTALGUN )	
					 return EMSLOTCHECK_FAILED;
			}
			else if ( sItem.sBasicOp.emItemType==ITEM_CHANGE_ACTORS_DISGUISE_INFO )
			{
				// ������ ���� ������ ����ÿ��� �����տ� ���������� ������ �־�� ���� ������;
				if ( NULL == m_pITEMS[emRHand] )
					return EMSLOTCHECK_FAILED;

				if ( m_pITEMS[emRHand]->sSuitOp.emAttack != ITEMATT_ACTOR_HAMMER && 
					m_pITEMS[emRHand]->sSuitOp.emAttack != ITEMATT_ACTOR_SHIELD && 
					m_pITEMS[emRHand]->sSuitOp.emAttack != ITEMATT_ACTOR_UMBRELLA )	
					return EMSLOTCHECK_FAILED;
			}
		}
	}*/

	return EMSLOTCHECK_OK;
}

//	�������� ������ �񿴴��� �˻��Ѵ�.
BOOL GLCHARLOGIC::ISEMPTY_SLOT ( SNATIVEID sNativeID, EMSLOT emSlot )
{
	const SITEM* pItem = GLogicData::GetInstance().GetItem ( sNativeID );
	if ( pItem==NULL )	return FALSE;

	EMSUIT emSuit= SLOT_2_SUIT ( emSlot );

	BOOL bEmptySlot = TRUE;
	if ( pItem->sSuitOp.emSuit==SUIT_HANDHELD )
	{
		EMSLOT emRHand = GetCurRHand();
		EMSLOT emLHand = GetCurLHand();

		const EMCHARINDEX emClass = GETCHARINDEX();

		//	�������� �ϴ� �������� ��յ����� ��.
		if ( pItem->sSuitOp.IsBOTHHAND() )
		{
			if ( m_pITEMS[emRHand] || m_pITEMS[emLHand] )
			{
				bEmptySlot = FALSE;
			}

			//	���� �������°� Ȱ�̶��. �޼տ� ȭ���� �־ ��. �� �������� ��� �־��.
			if ( GLCI_ARCHER_M == emClass || GLCI_ARCHER_W == emClass )
			{
				if ( pItem->sSuitOp.emAttack==ITEMATT_BOW ||
					pItem->sSuitOp.emAttack==ITEMATT_WEAPON_STR ||
					pItem->sSuitOp.emAttack==ITEMATT_WEAPON_DEX ||
					pItem->sSuitOp.emAttack==ITEMATT_WEAPON_SPI )
				{
					if ( !m_pITEMS[emRHand] && m_pITEMS[emLHand] )
					{
						if ( m_pITEMS[emLHand]->sBasicOp.emItemType==ITEM_ARROW )
						{
							bEmptySlot = TRUE;
						}
					}
				}
			}

			//	�������� ���� â �����϶� �޼տ� ������ �־ ��.
			if ( GLCI_SPIRIT_M == emClass || GLCI_SPIRIT_W == emClass )
			{
				if ( pItem->sSuitOp.emAttack==ITEMATT_SPEAR ||
					pItem->sSuitOp.emAttack==ITEMATT_WEAPON_STR ||
					pItem->sSuitOp.emAttack==ITEMATT_WEAPON_DEX ||
					pItem->sSuitOp.emAttack==ITEMATT_WEAPON_SPI)
				{
					if ( !m_pITEMS[emRHand] && m_pITEMS[emLHand] )
					{
						if ( m_pITEMS[emLHand]->sBasicOp.emItemType==ITEM_CHARM )
						{
							bEmptySlot = TRUE;
						}
					}
				}
			}
			
			//	�������� ���� �� �����϶� �޼տ� �Ѿ��� �־ ��.
			if ( GLCI_SCIENTIST_M == emClass || GLCI_SCIENTIST_W == emClass )
			{
				if ( pItem->sSuitOp.emAttack==ITEMATT_PISTOL || 
					 pItem->sSuitOp.emAttack==ITEMATT_RAILGUN || 
					 pItem->sSuitOp.emAttack==ITEMATT_PORTALGUN ||
					 pItem->sSuitOp.emAttack==ITEMATT_WEAPON_STR ||
					 pItem->sSuitOp.emAttack==ITEMATT_WEAPON_DEX ||
					 pItem->sSuitOp.emAttack==ITEMATT_WEAPON_SPI)
				{
					if ( !m_pITEMS[emRHand] && m_pITEMS[emLHand] )
					{
						if ( m_pITEMS[emLHand]->sBasicOp.emItemType==ITEM_BULLET )
						{
							bEmptySlot = TRUE;
						}
					}
				}
			}

			//	�������� ���� ������ �������� �� �޼տ� ������ ���� �������� �־ ��;
			if ( GLCI_ACTOR_M == emClass || GLCI_ACTOR_W == emClass )
			{
				if ( pItem->sSuitOp.emAttack == ITEMATT_ACTOR_HAMMER || 
					pItem->sSuitOp.emAttack == ITEMATT_ACTOR_SHIELD || 
					pItem->sSuitOp.emAttack == ITEMATT_ACTOR_UMBRELLA ||
					pItem->sSuitOp.emAttack==ITEMATT_WEAPON_STR ||
					pItem->sSuitOp.emAttack==ITEMATT_WEAPON_DEX ||
					pItem->sSuitOp.emAttack==ITEMATT_WEAPON_SPI )
				{
					if ( ( NULL == m_pITEMS[emRHand] ) && m_pITEMS[emLHand] )
					{
						if ( m_pITEMS[emLHand]->sBasicOp.emItemType == ITEM_CHANGE_ACTORS_DISGUISE_INFO )
						{
							bEmptySlot = TRUE;
						}
					}
				}
			}
		}
		//	�������� �������� �Ѽյ����� ��.
		else
		{
			//	���� ���Ⱑ ����϶�.
			if ( m_pITEMS[emRHand] && m_pITEMS[emRHand]->sSuitOp.IsBOTHHAND() )
			{
				bEmptySlot = FALSE;
			}
			//	���� ���Ⱑ �Ѽ��϶�.
			else
			{
				if ( m_pITEMS[emSlot] )		bEmptySlot = FALSE;
			}

			//	�������� �������� ȭ���϶�.
			if ( GLCI_ARCHER_M == emClass || GLCI_ARCHER_W == emClass )
			{
				if ( pItem->sBasicOp.emItemType==ITEM_ARROW )
				{
					bEmptySlot = FALSE;
					if ( m_pITEMS[emRHand] && !m_pITEMS[emLHand] )
					{
						if ( m_pITEMS[emRHand]->sSuitOp.emAttack==ITEMATT_BOW ||
							m_pITEMS[emRHand]->sSuitOp.emAttack==ITEMATT_WEAPON_STR ||
							m_pITEMS[emRHand]->sSuitOp.emAttack==ITEMATT_WEAPON_DEX ||
							m_pITEMS[emRHand]->sSuitOp.emAttack==ITEMATT_WEAPON_SPI )
						{
							bEmptySlot = TRUE;
						}
					}
				}
			}

			//	�������� �������� �����϶�.
			if ( GLCI_SPIRIT_M == emClass || GLCI_SPIRIT_W == emClass )
			{
				if ( pItem->sBasicOp.emItemType==ITEM_CHARM )
				{
					bEmptySlot = FALSE;
					if ( m_pITEMS[emRHand] && !m_pITEMS[emLHand] )
					{
						if ( m_pITEMS[emRHand]->sSuitOp.emAttack==ITEMATT_SPEAR ||
							m_pITEMS[emRHand]->sSuitOp.emAttack==ITEMATT_WEAPON_STR ||
							m_pITEMS[emRHand]->sSuitOp.emAttack==ITEMATT_WEAPON_DEX ||
							m_pITEMS[emRHand]->sSuitOp.emAttack==ITEMATT_WEAPON_SPI )
						{
							bEmptySlot = TRUE;
						}
					}
				}
			}

			//	�������� �������� �Ѿ��϶�.
			if ( GLCI_SCIENTIST_M == emClass || GLCI_SCIENTIST_W == emClass )
			{
				if ( pItem->sBasicOp.emItemType==ITEM_BULLET )
				{
					bEmptySlot = FALSE;
					if ( m_pITEMS[emRHand] && !m_pITEMS[emLHand] )
					{
						if ( m_pITEMS[emRHand]->sSuitOp.emAttack==ITEMATT_PISTOL || 
							 m_pITEMS[emRHand]->sSuitOp.emAttack==ITEMATT_RAILGUN || 
							 m_pITEMS[emRHand]->sSuitOp.emAttack==ITEMATT_PORTALGUN ||
							 m_pITEMS[emRHand]->sSuitOp.emAttack==ITEMATT_WEAPON_STR ||
							 m_pITEMS[emRHand]->sSuitOp.emAttack==ITEMATT_WEAPON_DEX ||
							 m_pITEMS[emRHand]->sSuitOp.emAttack==ITEMATT_WEAPON_SPI )		
						{
							bEmptySlot = TRUE;
						}
					}
				}
			}

			//	�������� �������� ������ ���� �������� ��;
			if ( GLCI_ACTOR_M == emClass || GLCI_ACTOR_W == emClass )
			{
				if ( pItem->sBasicOp.emItemType == ITEM_CHANGE_ACTORS_DISGUISE_INFO )
				{
					bEmptySlot = FALSE;
					if ( m_pITEMS[emRHand] && ( NULL == m_pITEMS[emLHand] ) )
					{
						if ( m_pITEMS[emRHand]->sSuitOp.emAttack == ITEMATT_ACTOR_HAMMER || 
							m_pITEMS[emRHand]->sSuitOp.emAttack == ITEMATT_ACTOR_SHIELD || 
							m_pITEMS[emRHand]->sSuitOp.emAttack == ITEMATT_ACTOR_UMBRELLA ||
							m_pITEMS[emRHand]->sSuitOp.emAttack==ITEMATT_WEAPON_STR ||
							m_pITEMS[emRHand]->sSuitOp.emAttack==ITEMATT_WEAPON_DEX ||
							m_pITEMS[emRHand]->sSuitOp.emAttack==ITEMATT_WEAPON_SPI )		
						{
							bEmptySlot = TRUE;
						}
					}
				}
			}

		}
	}
	else if ( m_pITEMS[emSlot] )
		bEmptySlot = FALSE;

	return bEmptySlot;
}

BOOL GLCHARLOGIC::ACCEPT_ITEM( const SITEMCUSTOM& sItemCustom )
{
	const SNATIVEID sNativeID(sItemCustom.GetNativeID())
		, sDisguiseID(sItemCustom.nidDISGUISE);

	if ( sNativeID==NATIVEID_NULL() )
		return FALSE;

	const SITEM* pItem = GLogicData::GetInstance().GetItem ( sNativeID );
	if ( pItem==NULL )
		return FALSE;

	const SITEM &sItem = *pItem;
	BOOL bArmor = (( sItem.sSuitOp.emSuit==SUIT_HANDHELD )&&sItem.sBasicOp.emItemType==ITEM_SUIT);
	int nEditedWearableLevel = static_cast< int >(
		sItemCustom.GETOptVALUE( RandomOption::RANDOM_OPTION_VALUE_WEARABLELEVEL_ADD ) );
	int nAPT = ( bArmor ? GLCONST_CHAR::dwACCEPT_LOWERSTATS : 0 );

	if ( !(sItem.sBasicOp.dwReqCharClass&m_emClass) )
		return FALSE;
	if ( !(sItem.sBasicOp.dwReqSchool&index2school(m_wSchool)) )
		return FALSE;

	if ( sItem.sBasicOp.emReqBright!=BRIGHT_BOTH )
	{
		if ( GETBRIGHT()!=sItem.sBasicOp.emReqBright )
			return FALSE;
	}

	if ( (m_powerAttribute[SKILL::EMAPPLY_MELEE] < int(sItem.sBasicOp.wReqPA)) || 
		 (m_powerAttribute[SKILL::EMAPPLY_RANGE] < int(sItem.sBasicOp.wReqSA)))
		return FALSE;

	if (m_wLevel < sItem.sBasicOp.wReqLevelDW + nEditedWearableLevel )
		return FALSE;
	if (sItem.sBasicOp.wReqLevelUP && m_wLevel > sItem.sBasicOp.wReqLevelUP)
		return FALSE;

	if ( sDisguiseID != NATIVEID_NULL() )
	{
		const SITEM* pItemDisguise = GLogicData::GetInstance().GetItem ( sDisguiseID );
		if ( pItemDisguise==NULL )		
			return FALSE;

		const SITEM &sItemDisguise = *pItemDisguise;
		if ( !(sItemDisguise.sBasicOp.dwReqCharClass&m_emClass) )	
			return FALSE;
		if ( m_wLevel < sItemDisguise.sBasicOp.wReqLevelDW + nEditedWearableLevel )
			return FALSE;
		if ( sItemDisguise.sBasicOp.wReqLevelUP && m_wLevel > sItemDisguise.sBasicOp.wReqLevelUP )
			return FALSE;
	}

	//2014.9.18 ��ųȿ���� ��������ȿ���� ������ ������ġ ��꿡 ���Խ�Ű�� ���� ����(CRM#3014);
	if (!m_sSUMSTATS_SKILL.CHECK_REQ ( sItem.sBasicOp.sReqStats, nAPT ))
		return FALSE;

	//  Ȱ�� �䱸 ����Ʈ üũ
	if ( !CHECK_ACTIVITY_POINT(sItem) )
		return FALSE;

	//  �⿩�� üũ
	if ( !CHECK_CONTRIBUTION_POINT(sItem) )
		return FALSE;

	if ( pItem->sSkillBookOp.sSkill_ID!=NATIVEID_NULL() )
	{
		switch ( pItem->sBasicOp.emItemType )
		{
		case ITEM_SKILL:
			if ( CHECKLEARNABLE_SKILL ( pItem->sSkillBookOp.sSkill_ID )!=EMSKILL_LEARN_OK )	return FALSE;
			break;
		}
	}
	return TRUE;
}

BOOL GLCHARLOGIC::SIMPLE_CHECK_ITEM(const SITEMCUSTOM& sItemCustom)
{
	if (sItemCustom.GetRealNativeID() == NATIVEID_NULL())
        return FALSE;

	const SITEM* pItem = GLogicData::GetInstance().GetItem(sItemCustom.GetRealNativeID());
	if (!pItem)
        return FALSE;

	const SITEM &sItem = *pItem;
	BOOL bArmor = ((sItem.sSuitOp.emSuit==SUIT_HANDHELD) && sItem.sBasicOp.emItemType==ITEM_SUIT);
	int nEditedWearableLevel = static_cast< int >(
		sItemCustom.GETOptVALUE( RandomOption::RANDOM_OPTION_VALUE_WEARABLELEVEL_ADD ) );   

	if (!(sItem.sBasicOp.dwReqCharClass&m_emClass))
        return FALSE;
	if (!(sItem.sBasicOp.dwReqSchool&index2school(m_wSchool)))
        return FALSE;

	if (m_wLevel < sItem.sBasicOp.wReqLevelDW + nEditedWearableLevel)
        return FALSE;
	if (sItem.sBasicOp.wReqLevelUP && m_wLevel > sItem.sBasicOp.wReqLevelUP)
        return FALSE;	

	return TRUE;
}

BOOL GLCHARLOGIC::SIMPLE_CHECK_ITEM( const SITEMCUSTOM& sItemCustom, EMINVEN_USE_ITEM& emMsg )
{
	emMsg = EMINVEN_USE_ITEM(EMINVEN_USE_ITEM_OK);
	if (sItemCustom.GetRealNativeID() == NATIVEID_NULL())
	{
		emMsg = EMINVEN_USE_ITEM(EMINVEN_USE_ITEM_FAIL);
		return FALSE;
	}

	const SITEM* pItem = GLogicData::GetInstance().GetItem(sItemCustom.GetRealNativeID());
	if (!pItem)
	{
		emMsg = EMINVEN_USE_ITEM(EMINVEN_USE_ITEM_NOITEM);
		return FALSE;
	}

	const SITEM &sItem = *pItem;
	BOOL bArmor = ((sItem.sSuitOp.emSuit==SUIT_HANDHELD) && sItem.sBasicOp.emItemType==ITEM_SUIT);
	int nEditedWearableLevel = static_cast< int >(
		sItemCustom.GETOptVALUE( RandomOption::RANDOM_OPTION_VALUE_WEARABLELEVEL_ADD ) );

	if (!(sItem.sBasicOp.dwReqCharClass&m_emClass))
	{
		emMsg = EMINVEN_USE_ITEM(EMINVEN_USE_ITEM_BADCLASS);
		return FALSE;
	}
	if (!(sItem.sBasicOp.dwReqSchool&index2school(m_wSchool)))
		return FALSE;

	if (m_wLevel < sItem.sBasicOp.wReqLevelDW + nEditedWearableLevel)
	{
		emMsg = EMINVEN_USE_ITEM(EMINVEN_USE_ITEM_BADLEVEL);
		return FALSE;
	}
	if (sItem.sBasicOp.wReqLevelUP && m_wLevel > sItem.sBasicOp.wReqLevelUP)
	{
		emMsg = EMINVEN_USE_ITEM(EMINVEN_USE_ITEM_BADLEVEL);
		return FALSE;
	}

	return TRUE;
}

BOOL GLCHARLOGIC::SIMPLE_CHECK_ITEM ( const SITEMCUSTOM& sItemCustom, SNATIVEID sDisguiseID, EMINVEN_USE_ITEM& emMsg )
{
	if ( sItemCustom.GetRealNativeID()==NATIVEID_NULL() || sDisguiseID==NATIVEID_NULL())
	{
		emMsg = EMINVEN_USE_ITEM(EMINVEN_USE_ITEM_FAIL);
		return FALSE;
	}

	const SITEM* pItem = GLogicData::GetInstance().GetItem ( sItemCustom.GetRealNativeID() );
	const SITEM* pItemDisguise = GLogicData::GetInstance().GetItem ( sDisguiseID );
	if ( pItem == NULL || pItemDisguise == NULL )
	{
		emMsg = EMINVEN_USE_ITEM(EMINVEN_USE_ITEM_NOITEM);
		return FALSE;
	}

	const SITEM &sItem = *pItem;
	const SITEM &sItemDisguise = *pItemDisguise;
	BOOL bArmor = (( sItem.sSuitOp.emSuit==SUIT_HANDHELD )&&sItem.sBasicOp.emItemType==ITEM_SUIT);
	int nEditedWearableLevel = static_cast< int >(
		sItemCustom.GETOptVALUE( RandomOption::RANDOM_OPTION_VALUE_WEARABLELEVEL_ADD ) );

	if( !(sItem.sBasicOp.dwReqCharClass&m_emClass) ||
		!(sItemDisguise.sBasicOp.dwReqCharClass&m_emClass) )	
	{
		emMsg = EMINVEN_USE_ITEM(EMINVEN_USE_ITEM_BADCLASS);
		return FALSE;
	}
	if ( !(sItem.sBasicOp.dwReqSchool&index2school(m_wSchool)) ) return FALSE;

	if ( (m_wLevel < sItem.sBasicOp.wReqLevelDW + nEditedWearableLevel) ||
		 (sItem.sBasicOp.wReqLevelUP && m_wLevel > sItem.sBasicOp.wReqLevelUP) )
	{
		emMsg = EMINVEN_USE_ITEM(EMINVEN_USE_ITEM_BADLEVEL);
		return FALSE;
	}
	if ( (m_wLevel < sItemDisguise.sBasicOp.wReqLevelDW + nEditedWearableLevel) ||
		(sItemDisguise.sBasicOp.wReqLevelUP && m_wLevel > sItemDisguise.sBasicOp.wReqLevelUP) )
	{
		emMsg = EMINVEN_USE_ITEM(EMINVEN_USE_ITEM_COSTUMEBADLEVEL);
		return FALSE;
	}
	return true;
}

BOOL GLCHARLOGIC::SIMPLE_CHECK_ITEM ( const SITEMCUSTOM& sItemCustom, SNATIVEID sDisguiseID )
{
	if ( sItemCustom.GetRealNativeID()==NATIVEID_NULL() )							return FALSE;
	if ( sDisguiseID==NATIVEID_NULL() )							return FALSE;

	const SITEM* pItem = GLogicData::GetInstance().GetItem ( sItemCustom.GetRealNativeID() );
	if ( pItem==NULL )											return FALSE;

	const SITEM* pItemDisguise = GLogicData::GetInstance().GetItem ( sDisguiseID );
	if ( pItemDisguise==NULL )											return FALSE;

	const SITEM &sItem = *pItem;
	const SITEM &sItemDisguise = *pItemDisguise;
	BOOL bArmor = (( sItem.sSuitOp.emSuit==SUIT_HANDHELD )&&sItem.sBasicOp.emItemType==ITEM_SUIT);
	int nEditedWearableLevel = static_cast< int >(
		sItemCustom.GETOptVALUE( RandomOption::RANDOM_OPTION_VALUE_WEARABLELEVEL_ADD ) );

	//	Memo : �ڽ�Ƭ�� ���� �ڰݿ���� üũ�Ѵ�.
	//		�� ���� �������� ���밡�� �����̴�.
	if( !(sItem.sBasicOp.dwReqCharClass&m_emClass) ||
		!(sItemDisguise.sBasicOp.dwReqCharClass&m_emClass) )	return FALSE;
	if ( !(sItem.sBasicOp.dwReqSchool&index2school(m_wSchool)) ) return FALSE;

	if (m_wLevel < sItem.sBasicOp.wReqLevelDW + nEditedWearableLevel)
        return FALSE;
	if ( sItem.sBasicOp.wReqLevelUP && m_wLevel > sItem.sBasicOp.wReqLevelUP )
        return FALSE;

	return TRUE;
}


/*-----------------------------------------------------------------------------------------------------------------------*/
// ��ų ��ȿ�� üũ ����;

void GLCHARLOGIC::SET_VALIDATION_EXPSKILL_MAP ()
{
	m_mapValidSkill.clear();

	SKILL_MAP_CITER iterExpSkill = m_ExpSkills.begin ();
	SKILL_MAP_CITER eIterExpSkill = m_ExpSkills.end ();
	for ( ; iterExpSkill != eIterExpSkill; ++iterExpSkill )
	{
		const SCHARSKILL& sSkill = (*iterExpSkill).second;
		m_mapValidSkill.insert( std::make_pair( sSkill.sNativeID.dwID, _ValidationSkill( sSkill.sNativeID ) ) );
	}
}

const EMSKILL_VALIDATION GLCHARLOGIC::VALIDATION_EXPSKILL ( const DWORD _dwSkillID )
{
	// ����� �������� ��� ������ ������Ų��;
	if ( false == GLUseFeatures::GetInstance().IsUsingValidationSkill() )
	{
		return EMSKILL_VALIDATION_OK;
	}

	// GM �� ��� ������ ������Ų��;
	if ( UserLevel() >= USER_GM3 )
	{
		return EMSKILL_VALIDATION_OK;
	}

	// �������� ��� �����ϱ� ���� Real Skill ID �� ���Ѵ�;
	SNATIVEID sTempSkillID( _dwSkillID );
	SNATIVEID sTempRealSkillID( false );
	ChangeToOrgSkill( sTempSkillID, sTempRealSkillID );

	// ��ų ��ȿ�� �ʿ��� ã�´�;
	SKILL_VALIDATION_MAP_ITER iterValidSkill = m_mapValidSkill.find( sTempRealSkillID.dwID );
	if ( iterValidSkill != m_mapValidSkill.end() )
	{
		const EMSKILL_VALIDATION emSkillValidation = (*iterValidSkill).second;
		return emSkillValidation;
	}

	// �ʿ� ���� ���� ����� ���� ����̴�;
	return EMSKILL_VALIDATION_NONE_LEARN;
}

const EMSKILL_VALIDATION GLCHARLOGIC::_ValidationSkill ( const SNATIVEID& _sSkillID )
{
	/// ��ų ������ �����´�;
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( _sSkillID.wMainID, _sSkillID.wSubID );
	if ( NULL == pSkill )
	{
		// �������� �ʴ� ��ų�� ��ȿ���� ���� ��ų�̴�;
		return EMSKILL_VALIDATION_NONE_SKILL;
	}

	/// ���� ��� ��ų�� ��ų������ ���Ѵ�;
	WORD wSKILL_LEVEL = 0;
	SKILL_MAP_ITER learnIter = m_ExpSkills.find( _sSkillID.dwID );
	if ( learnIter != m_ExpSkills.end() )
	{
		SCHARSKILL& sSkill = (*learnIter).second;
		wSKILL_LEVEL = sSkill.wLevel;

		// ������ �� ���� ������ ������ ��� ��ȿ���� ���� ��ų�̴�;
		if ( SKILL::MAX_LEVEL <= wSKILL_LEVEL )
			return EMSKILL_VALIDATION_INVALID_SKILLLEVEL;
		if ( pSkill->m_sBASIC.dwMAXLEVEL <= wSKILL_LEVEL )
			return EMSKILL_VALIDATION_INVALID_SKILLLEVEL;
	}
	else
	{
		// ����� �ʾҴٸ� ��ȿ���� ���� ��ų�̴�;
		return EMSKILL_VALIDATION_NONE_LEARN;
	}

	/// ��ų�� ���������� �˻��Ѵ�;
	SKILL::SLEARN& sSKILL_LEARN = pSkill->m_sLEARN;
	SKILL::SLEARN_LVL& sSKILL_LEARN_LVL = pSkill->m_sLEARN.sLVL_STEP[ wSKILL_LEVEL ];
	if ( !( m_emClass&sSKILL_LEARN.dwCLASS ) && !( _sSkillID.wMainID == EMSKILL_BIKE ) )
		return EMSKILL_VALIDATION_INVALID_CLASS;

	// �Ӽ��� �˻��Ѵ�;
	if ( sSKILL_LEARN.emBRIGHT != BRIGHT_BOTH )
	{
		if ( GETBRIGHT() != sSKILL_LEARN.emBRIGHT )
			return EMSKILL_VALIDATION_INVALID_BRIGHT;
	}

	// ������ �˻��Ѵ�;
	if ( static_cast< WORD >( GETLEVEL() ) < sSKILL_LEARN_LVL.dwLEVEL )
		return EMSKILL_VALIDATION_INVALID_LEVEL;

	// �䱸��ų�� �˻��Ѵ�;
	if ( NATIVEID_NULL() != sSKILL_LEARN.sSKILL )
	{
		if ( FALSE == ISLEARNED_SKILL( sSKILL_LEARN.sSKILL, static_cast< WORD >( sSKILL_LEARN_LVL.dwSKILL_LVL ) ) )
			return EMSKILL_VALIDATION_INVALID_NEEDSKILL;
	}

	// ���� �˻�� ���� �ʴ´� ( �ʿ佺�� ��ġ�� ������ ������ ���ؼ� ������ �� �ִ� );


	return EMSKILL_VALIDATION_OK;
}

/*-----------------------------------------------------------------------------------------------------------------------*/

WORD GLCHARLOGIC::CALC_ACCEPTP ( const SITEMCUSTOM& sItemCustom )
{
	int nATP = 0;
	if ( sItemCustom.GetNativeID() == NATIVEID_NULL() )
		return nATP;

	const SITEM* pITEM = GLogicData::GetInstance().GetItem ( sItemCustom.GetNativeID() );
	if ( pITEM==NULL )
		return nATP;
	
	if ( m_sSUMSTATS.wPow < pITEM->sBasicOp.sReqStats.wPow )
		nATP += pITEM->sBasicOp.sReqStats.wPow - m_sSUMSTATS.wPow;

	if ( m_sSUMSTATS.wStr < pITEM->sBasicOp.sReqStats.wStr )
		nATP += pITEM->sBasicOp.sReqStats.wStr - m_sSUMSTATS.wStr;

	if ( m_sSUMSTATS.wSpi < pITEM->sBasicOp.sReqStats.wSpi )
		nATP += pITEM->sBasicOp.sReqStats.wSpi - m_sSUMSTATS.wSpi;

	if ( m_sSUMSTATS.wDex < pITEM->sBasicOp.sReqStats.wDex )
		nATP += pITEM->sBasicOp.sReqStats.wDex - m_sSUMSTATS.wDex;

	if ( m_sSUMSTATS.wInt < pITEM->sBasicOp.sReqStats.wInt )
		nATP += pITEM->sBasicOp.sReqStats.wInt - m_sSUMSTATS.wInt;

	if ( m_sSUMSTATS.wSta < pITEM->sBasicOp.sReqStats.wSta )
		nATP += pITEM->sBasicOp.sReqStats.wSta - m_sSUMSTATS.wSta;

	if ( nATP < 0 )
		nATP = 0;

	if ( nATP > MAXWORD )
		nATP = MAXWORD;

	return static_cast< WORD >( nATP );
}

//	��ų�� ����� �����͸� ������ ����Ѵ�.
WORD GLCHARLOGIC::CALC_ACCEPTP_EX ( const SITEMCUSTOM& sItemCustom )
{
	int nATP = 0;
	if ( sItemCustom.GetNativeID() == NATIVEID_NULL() )
		return nATP;

	const SITEM* pITEM = GLogicData::GetInstance().GetItem ( sItemCustom.GetNativeID() );
	if ( pITEM==NULL )
		return nATP;

	if ( m_sSUMSTATS_SKILL.wPow < pITEM->sBasicOp.sReqStats.wPow )
		nATP += pITEM->sBasicOp.sReqStats.wPow - m_sSUMSTATS_SKILL.wPow;

	if ( m_sSUMSTATS_SKILL.wStr < pITEM->sBasicOp.sReqStats.wStr )
		nATP += pITEM->sBasicOp.sReqStats.wStr - m_sSUMSTATS_SKILL.wStr;

	if ( m_sSUMSTATS_SKILL.wSpi < pITEM->sBasicOp.sReqStats.wSpi )
		nATP += pITEM->sBasicOp.sReqStats.wSpi - m_sSUMSTATS_SKILL.wSpi;

	if ( m_sSUMSTATS_SKILL.wDex < pITEM->sBasicOp.sReqStats.wDex )
		nATP += pITEM->sBasicOp.sReqStats.wDex - m_sSUMSTATS_SKILL.wDex;

	if ( m_sSUMSTATS_SKILL.wInt < pITEM->sBasicOp.sReqStats.wInt )
		nATP += pITEM->sBasicOp.sReqStats.wInt - m_sSUMSTATS_SKILL.wInt;

	if ( m_sSUMSTATS_SKILL.wSta < pITEM->sBasicOp.sReqStats.wSta )
		nATP += pITEM->sBasicOp.sReqStats.wSta - m_sSUMSTATS_SKILL.wSta;

	if ( nATP < 0 )
		nATP = 0;

	if ( nATP > MAXWORD )
		nATP = MAXWORD;

	return static_cast< WORD >( nATP );
}

EMBEGINATTACK_FB GLCHARLOGIC::BEGIN_ATTACK ( WORD wStrikeNum )
{
	EMSLOT emRHand = GetCurRHand();
	EMSLOT emLHand = GetCurLHand();

	SITEM* pRHAND = GET_SLOT_ITEMDATA ( emRHand );
	SITEM* pLHAND = GET_SLOT_ITEMDATA ( emLHand );	

	/*if ( pRHAND )
	{
		const GLITEM_ATT typeArm(pRHAND->sSuitOp.emAttack);
		switch ( typeArm )
		{
		case ITEMATT_PISTOL:
		case ITEMATT_RAILGUN:
		case ITEMATT_PORTALGUN:
			{ // ��밡�ɼ� ���� ��Ŵ;
				if ( !pLHAND || (pLHAND->sBasicOp.emItemType != ITEM_BULLET) )
					return EMBEGINA_BULLET;

				WORD& wTurnNum = m_PutOnItems[emLHand].wTurnNum;
				if ( wTurnNum >= wStrikeNum )
					wTurnNum -= wStrikeNum;
				else
					wTurnNum = 0;
			}
			break;
		case ITEMATT_BOW:
			{
				if ( !pLHAND || (pLHAND->sBasicOp.emItemType != ITEM_ARROW) )
					return EMBEGINA_ARROW;

				//	��밡�ɼ� ���� ��Ŵ.
				WORD& wTurnNum = m_PutOnItems[emLHand].wTurnNum;
				if ( wTurnNum >= wStrikeNum )
					wTurnNum -= wStrikeNum;
				else
					wTurnNum = 0;
			}
			break;
		}
	}*/

	WORD wDisSP = GLCONST_CHAR::wBASIC_DIS_SP;
	if ( pRHAND )
		wDisSP += pRHAND->sSuitOp.wReqSP;
	if ( pLHAND )
		wDisSP += pLHAND->sSuitOp.wReqSP;

	//	SP ���� äũ.
	if ( m_sSP.nNow < int(wDisSP*wStrikeNum) )
		return EMBEGINA_SP;

	return EMBEGINA_OK;
}

BOOL GLCHARLOGIC::VALID_LEVELUP () const
{
	// �ذ��δ� �ִ� ���� üũ�� �ٸ��� �Ѵ�.
	if( m_CHARINDEX == GLCI_EXTREME_M || m_CHARINDEX == GLCI_EXTREME_W )
	{
		if (m_wLevel >= GLCONST_CHAR::wMAX_EXTREME_LEVEL)
            return FALSE;
	}
    else
    {
		if (m_wLevel >= GLCONST_CHAR::wMAX_LEVEL)
            return FALSE;
	}
	return m_sExperience.lnNow >= GET_LEVELUP_EXP();
}

LONGLONG GLCHARLOGIC::GET_LEVELUP_EXP () const
{
	if( m_CHARINDEX == GLCI_EXTREME_M || m_CHARINDEX == GLCI_EXTREME_W )
	{
		return GLOGICEX::GLNEEDEXP2(GETLEVEL());
	}
	else
	{
		return GLOGICEX::GLNEEDEXP(GETLEVEL());
	}
}

void GLCHARLOGIC::LEVLEUP(bool bInitNowExp, bool bJumping, bool bCalExp )
{
	bool bMaxLevel = FALSE;
	if (m_CHARINDEX == GLCI_EXTREME_M || m_CHARINDEX == GLCI_EXTREME_W)
	{
		if (m_wLevel < GLCONST_CHAR::wMAX_EXTREME_LEVEL)
            bMaxLevel = TRUE;
	}
    else
    {
		if (m_wLevel < GLCONST_CHAR::wMAX_LEVEL)
            bMaxLevel = TRUE;
	}


	if ( /*m_wLevel < GLCONST_CHAR::wMAX_LEVEL*/bMaxLevel )
	{
		//	���� ����ġ ����.
		//GASSERT(m_sExperience.lnNow>=GET_LEVELUP_EXP());
		if( bCalExp )
			m_sExperience.lnNow -= GET_LEVELUP_EXP();

		//	������.
		++m_wLevel;

		//	������ ���ʽ� ����Ʈ
		if( m_CHARINDEX == GLCI_EXTREME_M || m_CHARINDEX == GLCI_EXTREME_W )
		{
			m_wStatsPoint += GLCONST_CHAR::wLVL_2ndSTATS_P;
			m_dwSkillPoint += GLCONST_CHAR::wLVL_2ndSKILL_P;

			GLCharSlotMan::Instance()->StatPointUp( CharDbNum(), GLCONST_CHAR::wLVL_2ndSTATS_P );
			GLCharSlotMan::Instance()->SkillPointUp( CharDbNum(), GLCONST_CHAR::wLVL_2ndSKILL_P );
		}
		else
		{
			m_wStatsPoint += GLCONST_CHAR::wLVL_STATS_P;
			m_dwSkillPoint += GLCONST_CHAR::wLVL_SKILL_P;

			GLCharSlotMan::Instance()->StatPointUp( CharDbNum(), GLCONST_CHAR::wLVL_STATS_P );
			GLCharSlotMan::Instance()->SkillPointUp( CharDbNum(), GLCONST_CHAR::wLVL_SKILL_P );
		}

		//	��� ��ġ�� �ٽ� �ʱ�ȭ �Ѵ�.
		INIT_DATA(FALSE,FALSE,1.0f,bInitNowExp, bJumping);

		//	ä���� ��� ȸ�� �����ش�.
		m_sHP.TO_FULL ();
		m_sMP.TO_FULL ();
		m_sSP.TO_FULL ();	
	}

	// ��ȿ�� ��ų �˻�;
	SET_VALIDATION_EXPSKILL_MAP();
}

void GLCHARLOGIC::STATSUP ( EMSTATS emStats )
{
	switch ( emStats )
	{
	case EMPOW:	++m_sStats.wPow; break;
	case EMSTR: ++m_sStats.wStr; break;

	case EMSPI: ++m_sStats.wSpi; break;
	case EMDEX: ++m_sStats.wDex; break;

	case EMINT: ++m_sStats.wInt; break;
	case EMSTA: ++m_sStats.wSta; break;
	};

	//	�׿� STATE�� ����.
	--m_wStatsPoint;

	//	��� ��ġ�� �ٽ� �ʱ�ȭ �Ѵ�.
//	INIT_DATA(FALSE,FALSE);

}

// *****************************************************
// Desc: ��ų���� ����
// *****************************************************
bool GLCHARLOGIC::RESET_SKILL ()
{
	EMCHARINDEX emINDEX = CharClassToIndex(m_emClass);
	const SCHARDATA2& sCHARDATA = GLCONST_CHAR::GET_CHAR_DATA2(m_wSchool,emINDEX);

	//	Note : ����Ʈ���� ȹ���� ��ų ����Ʈ.
	//
	DWORD dwGiftSkill(0);

	GLQuestPlay::MAPQUEST& sQuestEnd = m_cQuestPlay.GetQuestEnd();
	GLQuestPlay::MAPQUEST_ITER pos = sQuestEnd.begin();
	GLQuestPlay::MAPQUEST_ITER end = sQuestEnd.end();

	for ( ; pos!=end; ++pos )
	{
		GLQUESTPROG *pPROG = (*pos).second;
		GLQUEST *pQUEST = GLQuestMan::GetInstance().Find ( pPROG->m_sNID.dwID );
		if ( !pQUEST )
            continue;

		dwGiftSkill += pQUEST->m_dwGiftSKILLPOINT;
	}

	//	Note : ��ų ����Ʈ �ʱ�ȭ.
	//
	if( m_CHARINDEX == GLCI_EXTREME_M || m_CHARINDEX == GLCI_EXTREME_W )
	{
		m_dwSkillPoint = sCHARDATA.m_dwSkillPoint + (m_wLevel-sCHARDATA.m_wLevel)*GLCONST_CHAR::wLVL_2ndSKILL_P + dwGiftSkill;
	}
	else
	{
		m_dwSkillPoint = sCHARDATA.m_dwSkillPoint + (m_wLevel-sCHARDATA.m_wLevel)*GLCONST_CHAR::wLVL_SKILL_P + dwGiftSkill;
	}

	//	Note : ��ų�� ����� ���� ���� ����.
	//
	m_wSKILLQUICK_ACT = 0;
	for ( int i=0; i<EMSKILLQUICK_SIZE; ++i )
	{
		m_sSKILLQUICK[i] = SNATIVEID(false);
	}


	//	Note : ������ ��ų �ʱ�ȭ.
	//

	m_ExpSkills.clear();
	m_ExpChangeSkills.clear();
	//	m_ExpSkills = sCHARDATA.m_ExpSkills;

	//	Note : ��� ��ġ�� �ٽ� �ʱ�ȭ �Ѵ�.
	//
	INIT_DATA(FALSE,FALSE);

	// ��ȿ�� ��ų �˻�;
	SET_VALIDATION_EXPSKILL_MAP();

	return true;
}

// *****************************************************
// Desc: �������� ����
// *****************************************************
bool GLCHARLOGIC::RESET_STATS ( const WORD* pResetStats )
{
	// Note : ��ü ���� ����Ʈ �ʱ�ȭ
	if ( pResetStats==NULL )
	{
		EMCHARINDEX emINDEX = CharClassToIndex(m_emClass);
		const SCHARDATA2& sCHARDATA = GLCONST_CHAR::GET_CHAR_DATA2(m_wSchool,emINDEX);

		//	Note : ����Ʈ���� ȹ���� ���� ����Ʈ.
		//
		WORD wGiftStats(0);

		GLQuestPlay::MAPQUEST& sQuestEnd = m_cQuestPlay.GetQuestEnd();
		GLQuestPlay::MAPQUEST_ITER pos = sQuestEnd.begin();
		GLQuestPlay::MAPQUEST_ITER end = sQuestEnd.end();
		for ( ; pos!=end; ++pos )
		{
			GLQUESTPROG *pPROG = (*pos).second;
			GLQUEST *pQUEST = GLQuestMan::GetInstance().Find ( pPROG->m_sNID.dwID );
			if ( !pQUEST )
                continue;

			wGiftStats += (WORD) pQUEST->m_dwGiftSTATSPOINT;
		}

		if( m_CHARINDEX == GLCI_EXTREME_M || m_CHARINDEX == GLCI_EXTREME_W )
		{
			m_wStatsPoint = sCHARDATA.m_wStatsPoint + (m_wLevel-sCHARDATA.m_wLevel)*GLCONST_CHAR::wLVL_2ndSTATS_P + wGiftStats;
		}
		else
		{
			m_wStatsPoint = sCHARDATA.m_wStatsPoint + (m_wLevel-sCHARDATA.m_wLevel)*GLCONST_CHAR::wLVL_STATS_P + wGiftStats;
		}

		m_sStats.RESET();

		return true;
	}

	SCHARSTATS sStats(m_sStats);
	WORD	   wPoint = 0;

	// Note : �� ���ݺ��� ����
	for ( int i=0; i<EMSIZE; ++i )
	{
		WORD &wSTAT = sStats.GET((EMSTATS)i);
		if ( wSTAT == 0 ) continue;

		// Note : ���� ��ġ�� ���� ��ġ���� �� ũ�� �ʾƾ� �Ѵ�.
		if ( wSTAT < pResetStats[i])
		{
			wPoint += wSTAT;
			wSTAT   = 0;
			continue;
		}

		wSTAT  -= pResetStats[i];
		wPoint += pResetStats[i];
	}

	// Note : ���������� �������� �ٽ� ����.
	m_sStats = sStats;

	//	Note : ���� �ܿ� ���� ����Ʈ�� ����.
	m_wStatsPoint += wPoint;

	//	Note : ��� ��ġ�� �ٽ� �ʱ�ȭ �Ѵ�.
	INIT_DATA(FALSE,FALSE);

	return true;
}

DWORD GLCHARLOGIC::SkillEffectType( DWORD iReleaseType )
{
	EMITEM_DRUG_ER eType = (EMITEM_DRUG_ER)(iReleaseType >> 1);
	
	DWORD dwSpec = (DWORD)EMSPECA_NULL;

	switch( eType)
	{
	case ITEM_DRUG_ER_STUN        : dwSpec = (DWORD)EMSPECA_STUN;        break;
	case ITEM_DRUG_ER_HALLUCINATE : dwSpec = (DWORD)EMSPECA_HALLUCINATE; break;
	case ITEM_DRUG_ER_NON_DRUG    : dwSpec = (DWORD)EMSPECA_NON_DRUG;	 break;
	}

	return dwSpec;
}

bool GLCHARLOGIC::ReleaseCureEffect( DWORD dwCheckList )
{	
	if( dwCheckList == 0 ) return false;

	bool bErase = false;

	std::vector<SKILL::SSPEC_ADDON_LEVEL>::iterator _iter;

	for ( int i=0; i<SKILLFACT_SIZE; ++i)
	{
		SSKILLFACT& sSKEFF = GetSkillFact( i );
		if (sSKEFF.sNATIVEID == NATIVEID_NULL())
			continue;

		if (!sSKEFF.bEnable)
			continue;		

		PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sSKEFF.sNATIVEID );
		
		//if( pSkill->m_sBASIC.emIMPACT_TAR == TAR_SELF ) continue;

		switch( pSkill->m_sBASIC.emIMPACT_SIDE )
		{
		case EMIMPACT_SIDE_PARTY   : continue;
		case EMIMPACT_SIDE_ANYBODY :
			{
				if( !sSKEFF.bSkillUserEnermy ) continue;
			}
			break;
		}
	
		for( int j = 0; j < ITEM_DRUG_ER_SIZE; ++j )
		{
			DWORD iTemp = 1 << j;

			EMSPEC_ADDON eType = (EMSPEC_ADDON)SkillEffectType( dwCheckList & iTemp );

			if( eType == EMSPECA_NULL ) continue;

			_iter = std::find_if( sSKEFF.vecSPEC.begin(), sSKEFF.vecSPEC.end(), SKILL::SSPEC_ADDON_LEVEL(eType) );

			if( _iter != sSKEFF.vecSPEC.end() )
			{
				sSKEFF.vecSPEC.erase(_iter);
				
				bErase = true;
			}
		}		
	}

	return bErase;
}

void GLCHARLOGIC::GetEffList( DWORD dwSlot, BYTE* pSSPECA, BYTE* pSIMA )
{
	if( pSSPECA == NULL || pSIMA == NULL ) return;

	std::vector<SKILL::SIMPACT_ADDON_LEVEL>::iterator _iSater = m_sSKILLFACT[dwSlot].vecADDON.begin();

	int iCnt = 0;

	while( _iSater != m_sSKILLFACT[dwSlot].vecADDON.end())
	{
		pSIMA[iCnt++] = static_cast<BYTE>(_iSater->emADDON);

		++_iSater;
	}

	iCnt = 0;

	BOOL bInsert = TRUE;
	BOOL bCheck  = FALSE;

	BYTE iValue  = 0;

	PGLSKILL pSkill = NULL;

	if( RF_IMMUNE( m_EffSkillVarSet ).IsOn() ) 
	{
		pSkill = GLSkillMan::GetInstance().GetData ( m_sSKILLFACT[dwSlot].sNATIVEID );		

		if( pSkill && pSkill->m_sAPPLY.sDATA_LVL[m_sSKILLFACT[dwSlot].wLEVEL].fBASIC_VAR < 0 )
		{
			bCheck = TRUE;
		}
	}
		
	std::vector<SKILL::SSPEC_ADDON_LEVEL>::iterator _iSSater = m_sSKILLFACT[dwSlot].vecSPEC.begin();
	std::vector<SKILL::SSPEC_ADDON_LEVEL>::iterator _iTempIter;
		
	while( _iSSater != m_sSKILLFACT[dwSlot].vecSPEC.end())
	{
		bInsert = TRUE;

		iValue = static_cast<BYTE>(_iSSater->emSPEC);

		if ( RF_IMMUNE( m_EffSkillVarSet ).IsImmune( iValue ) )
			bInsert = FALSE;

		/*if( bInsert && _iSSater->emSPEC == EMSPECA_ENTRANCE_EXIT ) {

			if( RF_ENTRANCE_EXIT( m_EffSkillVarSet ).IsOn() )
			{
				bInsert = FALSE;
			}			
		}*/
				
		if( bInsert ) {
			pSSPECA[iCnt++] = iValue;

			++_iSSater;
		}
		else {
			_iSSater = m_sSKILLFACT[dwSlot].vecSPEC.erase( _iSSater );
		}
	}	
}
// Note : ���������� ������ �� ���� �ʰ�ȭ
bool GLCHARLOGIC::RESET_STATS_OLD ( const WORD wDIS )
{
	EMCHARINDEX emINDEX = CharClassToIndex(m_emClass);
	const SCHARDATA2& sCHARDATA = GLCONST_CHAR::GET_CHAR_DATA2(m_wSchool,emINDEX);

	// ��ü ���� ����Ʈ �ʱ�ȭ
	if ( wDIS==USHRT_MAX )
	{
		//	Note : ����Ʈ���� ȹ���� ���� ����Ʈ.
		//
		WORD wGiftStats(0);

		GLQuestPlay::MAPQUEST& sQuestEnd = m_cQuestPlay.GetQuestEnd();
		GLQuestPlay::MAPQUEST_ITER pos = sQuestEnd.begin();
		GLQuestPlay::MAPQUEST_ITER end = sQuestEnd.end();
		for ( ; pos!=end; ++pos )
		{
			GLQUESTPROG *pPROG = (*pos).second;
			GLQUEST *pQUEST = GLQuestMan::GetInstance().Find ( pPROG->m_sNID.dwID );
			if ( !pQUEST )
                continue;

			wGiftStats += (WORD) pQUEST->m_dwGiftSTATSPOINT;
		}

		if( m_CHARINDEX == GLCI_EXTREME_M || m_CHARINDEX == GLCI_EXTREME_W )
		{
			m_wStatsPoint = sCHARDATA.m_wStatsPoint + (m_wLevel-sCHARDATA.m_wLevel)*GLCONST_CHAR::wLVL_2ndSTATS_P + wGiftStats;
		}
		else
		{
			m_wStatsPoint = sCHARDATA.m_wStatsPoint + (m_wLevel-sCHARDATA.m_wLevel)*GLCONST_CHAR::wLVL_STATS_P + wGiftStats;
		}

		m_sStats.RESET();

		return true;
	}

	// ������ ��޿� ���� �ʱ�ȭ
	WORD wNOW_DIS(0);
	while(1)
	{
		if ( wNOW_DIS>=wDIS )		break;
		if ( m_sStats.IsZERO() )	break;

		// �� ���ݺ��� �ϳ��� ����
		for ( int i=0; i<EMSIZE; ++i )
		{
			if ( wNOW_DIS>=wDIS )		break;
			if ( m_sStats.IsZERO() )	break;

			WORD &wSTAT = m_sStats.GET((EMSTATS)i);
			if ( wSTAT == 0 )			continue;

			--wSTAT;
			++wNOW_DIS;
		}
	}

	//	Note : ���� �ܿ� ���� ����Ʈ�� ����.
	m_wStatsPoint += wNOW_DIS;

	//	Note : ��� ��ġ�� �ٽ� �ʱ�ȭ �Ѵ�.
	//
	INIT_DATA(FALSE,FALSE);

	return true;
}

bool GLCHARLOGIC::RESET_STATS_SKILL ( const WORD* pResetStats )
{
	//	Note : ���� ȣ�� ������ stats, skill���̿��� �Ѵ�.
	RESET_STATS ( pResetStats );
	RESET_SKILL ();

	return true;
}

void GLCHARLOGIC::GET_EVENLY_DECREASE_STATS ( WORD* pOutResetStats, WORD wAmount )
{
	if ( !pOutResetStats )
	{
		return;
	}

	memset(pOutResetStats, 0, sizeof(WORD)*EMSIZE);

	EMCHARINDEX emINDEX = CharClassToIndex(m_emClass);
	const SCHARDATA2& sCHARDATA = GLCONST_CHAR::GET_CHAR_DATA2(m_wSchool,emINDEX);

	// ��ü ���� ����Ʈ
	if ( wAmount==USHRT_MAX )
	{
		for ( int i=0; i<EMSIZE; ++i )
		{
			pOutResetStats[i] = m_sStats.GET((EMSTATS)i);
		}

		return;
	}

	WORD	   wNowAmount(0);
	SCHARSTATS sStats(m_sStats);

	while(1)
	{
		if ( wNowAmount>=wAmount )	break;
		if ( sStats.IsZERO()     )	break;

		// �� ���ݺ��� �ϳ��� ����
		for ( int i=0; i<EMSIZE; ++i )
		{
			if ( wNowAmount>=wAmount )	break;
			if ( sStats.IsZERO() )		break;

			WORD &wSTAT = sStats.GET((EMSTATS)i);
			if ( wSTAT == 0 ) continue;

			--wSTAT;
			++wNowAmount;

			++pOutResetStats[i];
		}
	}
}

void GLCHARLOGIC::UPDATESKILLDELAY ( float fElapsedTime )
{
	SKILL::DELAY_MAP_ITER iter     = m_SKILLDELAY.begin ();
	SKILL::DELAY_MAP_ITER iter_end = m_SKILLDELAY.end ();
	while( iter != iter_end )
	{
		SKILL::SKILLDELAY &fDelay = (*iter).second;

		fDelay.fDelayTime -= fElapsedTime;
		if ( fDelay.fDelayTime <= 0.0f )
			iter = m_SKILLDELAY.erase ( iter );
		else
			iter++;
	}
}

bool GLCHARLOGIC::SET_SKILLDELAY_INC ( float fINCR, float fChance, DWORD dwFLAG, const SNATIVEID& skill_id )
{
	bool bSuccess = false;
    if ( !CHECK_FLAG_SKILL(dwFLAG, EMSPEC_SKILL_ASING_BEFORE) )
        return bSuccess;

	bSuccess = sc::Random::RANDOM_POS()*100.0f < fChance;

	SKILL::DELAY_MAP_ITER iter_del;

	SKILL::DELAY_MAP_ITER iter     = m_SKILLDELAY.begin ();
	SKILL::DELAY_MAP_ITER iter_end = m_SKILLDELAY.end ();

	while( iter != iter_end )
	{
		const DWORD &dwSkillId = (*iter).first;
		SKILL::SKILLDELAY &fDelay = (*iter).second;

        if ( bSuccess && dwSkillId != skill_id.dwID  )
        {

			float fOldMaxDelay = fDelay.fMAXDelayTime;

			fDelay.fDelayTime *= ( 1 + fINCR );
			//fDelay.fMAXDelayTime *= ( m_fSTATE_DELAY + fINCR );

			float fMAXDelay = fDelay.fDefultMaxDelayTime;
			fMAXDelay *= ( 1 + fINCR );

			if ( fMAXDelay > fOldMaxDelay )
			{
				fDelay.fMAXDelayTime = fMAXDelay;
			}
			else
			{
				fDelay.fMAXDelayTime = fOldMaxDelay;
			}

			if (fDelay.fDelayTime > fDelay.fMAXDelayTime )
			{
				fDelay.fDelayTime = fDelay.fMAXDelayTime;
			}
        }

		if ( fDelay.fDelayTime <= 0.0f )
			iter = m_SKILLDELAY.erase ( iter );
		else
			iter++;
	}
	dwFLAG  &= ~EMSPEC_SKILL_ASING_BEFORE;


	return bSuccess;
}

void GLCHARLOGIC::SET_IGNORED_DAMAGE(const int nIgnoreCount, const int nIgnoreDamage)
{
	m_sIgnoredDamage.nIgnoreCount = nIgnoreCount;
	m_sIgnoredDamage.nIgnoreDamage = nIgnoreDamage;
}

void GLCHARLOGIC::SET_MAXHP_RATE ( const EMSPEC_ADDON& emAddon
                                 , const SKILL::SSPEC& sSPEC
                                 , SSKILLFACT& sSKILLEF
                                 , float fMaxHpRate )
{
    SKILL::SSPEC_ADDON_LEVEL	sAddon;
    sAddon.emSPEC			= emAddon;
    sAddon.sSPEC.fVAR1		= fMaxHpRate;
    sAddon.sSPEC.fVAR2		= sSPEC.fVAR2;
    sAddon.sSPEC.fRate		= sSPEC.fRate;	
    sAddon.sSPEC.fRate2 	= sSPEC.fRate2;	
    sAddon.sSPEC.dwFLAG 	= sSPEC.dwFLAG;
    sAddon.sSPEC.dwNativeID = sSPEC.dwNativeID;

    sSKILLEF.vecSPEC.push_back( sAddon );
}

void GLCHARLOGIC::SET_CP_DUR ( GLPADATA& sCP, float fValue, float& fTime )
{
    float nNow = static_cast<float>(sCP.wNow);

    if( fTime > 2.0f )
    {
        fTime = 0.0f;

        nNow += fValue;

        if ( nNow < 0 )
            nNow = 0.0f;

        sCP.wNow = static_cast<WORD>(nNow); 
        sCP.LIMIT();
    }
}

void GLCHARLOGIC::LEARN_CHANGE_SKILL ( const GLSKILL* pOrgSkill, WORD wLv )
{
	if( !pOrgSkill )
		return;

	SKILL_MAP_ITER _iter = m_ExpChangeSkills.find( pOrgSkill->m_sEXT_DATA.idDisguiseSkill.dwID );

	if( _iter != m_ExpChangeSkills.end() )
	{
		_iter->second.sNativeID = pOrgSkill->m_sBASIC.sNATIVEID;
		_iter->second.wLevel    = wLv;
	}
	else
	{
		m_ExpChangeSkills.insert ( std::make_pair( pOrgSkill->m_sEXT_DATA.idDisguiseSkill.dwID, SCHARSKILL( pOrgSkill->m_sBASIC.sNATIVEID, wLv)) );
	}	
}

void GLCHARLOGIC::LEARN_SKILL ( SNATIVEID skill_id )
{
	//	Note : ��ų ���� ������.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( skill_id.wMainID, skill_id.wSubID );
	if( !pSkill )
		return;

	//	Note : ����ġ ���ҽ�Ŵ.
	//
	SKILL::SLEARN_LVL &sSKILL_LEARN_LVL = pSkill->m_sLEARN.sLVL_STEP[0];
	if( m_dwSkillPoint < sSKILL_LEARN_LVL.dwSKP )
		return;

	m_dwSkillPoint -= sSKILL_LEARN_LVL.dwSKP;

	//	Note : ��� ��ų�� �����.
	//
	m_ExpSkills.insert ( std::make_pair(skill_id.dwID,SCHARSKILL(skill_id,0)) );

	BOOL  bChange   = RF_DISGUISE( m_EffSkillVarSet).IsOn();
	int   dwTableID = RF_DISGUISE( m_EffSkillVarSet ).GetTableID() - 1;

	bChange = (dwTableID < 0) ? FALSE : bChange;

	if( bChange ) {
		bChange = !GSCL_INST.IsChangeTable((UINT)dwTableID) ? FALSE : bChange;
	}

	if( bChange ) {
		SCHARSKILL pTempCharSkill;

		PGLSKILL pChangeSkill = GSCL_INST.GetChangeSkill( (UINT)dwTableID, pSkill, pTempCharSkill );

		if( pChangeSkill != pSkill && pTempCharSkill.sNativeID != NATIVEID_NULL() )	{
			LEARN_CHANGE_SKILL( pSkill, 0 );
		}
	}

	//	Note : ��ú� ��ų�� ��ȭ �Ҷ� �ʱ� ��ġ���� ��� �� ����Ѵ�.
	//
	if( pSkill->HavePassiveFunc() )
		INIT_DATA ( FALSE, FALSE );

	// ��ȿ�� ��ų �˻�;
	SET_VALIDATION_EXPSKILL_MAP();
}

void GLCHARLOGIC::LEARN_SKILL_SIMPLE ( SNATIVEID skill_id )
{
	//	Note : ��ų ���� ������.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( skill_id.wMainID, skill_id.wSubID );
	if( !pSkill )
		return;

	//	Note : ��� ��ų�� �����.
	//
	m_ExpSkills.insert ( std::make_pair(skill_id.dwID,SCHARSKILL(skill_id,0)) );

	BOOL  bChange   = RF_DISGUISE( m_EffSkillVarSet).IsOn();
	int   dwTableID = RF_DISGUISE( m_EffSkillVarSet ).GetTableID() - 1;

	bChange = (dwTableID < 0) ? FALSE : bChange;

	if( bChange ) {
		bChange = !GSCL_INST.IsChangeTable((UINT)dwTableID) ? FALSE : bChange;
	}

	if( bChange ) {
		SCHARSKILL pTempCharSkill;

		PGLSKILL pChangeSkill = GSCL_INST.GetChangeSkill( (UINT)dwTableID, pSkill, pTempCharSkill );

		if( pChangeSkill != pSkill && pTempCharSkill.sNativeID != NATIVEID_NULL() )	{
			LEARN_CHANGE_SKILL( pSkill, 0 );
		}
	}

	//	Note : ��ú� ��ų�� ��ȭ �Ҷ� �ʱ� ��ġ���� ��� �� ����Ѵ�.
	//
	if( pSkill->HavePassiveFunc() )
		INIT_DATA ( FALSE, FALSE );

	// ��ȿ�� ��ų �˻�;
	SET_VALIDATION_EXPSKILL_MAP();
}

bool GLCHARLOGIC::LEARN_SKILL_QUEST ( SNATIVEID skill_id )
{
	//	Note : ��ų ���� ������.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( skill_id.wMainID, skill_id.wSubID );
	if ( !pSkill )
		return false;

	//	Note : ��� ��ų�� �����.
	//

	m_ExpSkills.insert ( std::make_pair(skill_id.dwID,SCHARSKILL(skill_id,0)) );

	BOOL  bChange   = RF_DISGUISE( m_EffSkillVarSet).IsOn();
	int   dwTableID = RF_DISGUISE( m_EffSkillVarSet ).GetTableID() - 1;

	bChange = (dwTableID < 0) ? FALSE : bChange;

	if( bChange ) {
		bChange = !GSCL_INST.IsChangeTable((UINT)dwTableID) ? FALSE : bChange;
	}

	if( bChange ) {
		SCHARSKILL pTempCharSkill;

		PGLSKILL pChangeSkill = GSCL_INST.GetChangeSkill( (UINT)dwTableID, pSkill, pTempCharSkill );

		if( pChangeSkill != pSkill && pTempCharSkill.sNativeID != NATIVEID_NULL() )	{
			LEARN_CHANGE_SKILL( pSkill, 0 );
		}
	}

	//	Note : ��ú� ��ų�� ��ȭ �Ҷ� �ʱ� ��ġ���� ��� �� ����Ѵ�.
	//
	if ( pSkill->m_sBASIC.emROLE == SKILL::EMROLE_PASSIVE )
	{
		INIT_DATA ( FALSE, FALSE );
	}

	// ��ȿ�� ��ų �˻�;
	SET_VALIDATION_EXPSKILL_MAP();

	return true;
}

VOID GLCHARLOGIC::LVLUP_SKILL ( SNATIVEID skill_id, WORD wToLevel )
{
	//	Note : ��ų ���� ������.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( skill_id.wMainID, skill_id.wSubID );
	if ( !pSkill )
		return;

	SKILL_MAP_ITER learniter = m_ExpSkills.find ( skill_id.dwID );
	if ( learniter==m_ExpSkills.end() )
		return;

	SCHARSKILL &sSkill = (*learniter).second;

	//	Note : ����ġ ���ҽ�Ŵ.
	//
	SKILL::SLEARN_LVL &sSKILL_LEARN_LVL = pSkill->m_sLEARN.sLVL_STEP[wToLevel];
	if ( m_dwSkillPoint < sSKILL_LEARN_LVL.dwSKP )
		return;

	m_dwSkillPoint -= sSKILL_LEARN_LVL.dwSKP;

	//	Note : ��ų ���� ��.
	//
	++sSkill.wLevel;

	UPDATE_CHANGE_CHARSKILL(  sSkill );

	//	Note : ��ú� ��ų�� ��ȭ �Ҷ� �ʱ� ��ġ���� ��� �� ����Ѵ�.
	//
	if ( pSkill->HavePassiveFunc() )
		INIT_DATA ( FALSE, FALSE );

	// ��ȿ�� ��ų �˻�;
	SET_VALIDATION_EXPSKILL_MAP();
}

void GLCHARLOGIC::UPDATE_CHANGE_CHARSKILL( const SCHARSKILL& OrgValue )
{
	if( RF_DISGUISE( m_EffSkillVarSet ).IsOn() )
	{
		SKILL_MAP_ITER _iter = m_ExpChangeSkills.begin();

		while( _iter != m_ExpChangeSkills.end() )
		{
			if( _iter->second.sNativeID == OrgValue.sNativeID )
			{
				_iter->second.wLevel = OrgValue.wLevel;
				return;
			}

			++_iter;
		}
	}
}

void GLCHARLOGIC::SAVE_CURRENT_SLOT_INFO ( void )
{
	se::ByteStream sByteStream;

	// ĳ���� ������ �����Ѵ�;
	SCHAR_SLOT_DATA sCharSlotData;

	sCharSlotData.m_sStats = m_sStats;
	sCharSlotData.m_wStatsPoint = m_wStatsPoint;

	GETEXPSKILLS_BYBUF( sByteStream );
	sCharSlotData.SETEXPSKILLS_BYBUF( sByteStream );
	sCharSlotData.m_dwSkillPoint = m_dwSkillPoint;

	GETSKILL_QUICKSLOT( sByteStream );	
	sCharSlotData.SETSKILL_QUICKSLOT( sByteStream );

	//sCharSlotData.m_bLockSlot = false;
	m_arrLockSlotState[ m_wSlotIndex ] = false;

	GLCharSlotMan::Instance()->SaveCharSlot( CharDbNum(), m_wSlotIndex, sCharSlotData );
}

const bool GLCHARLOGIC::CHANGE_CHARACTER_SLOT ( const WORD _wSlotIndex, const bool _bForce )
{
	// ���ο� ������ ������ �����´�;
	const SCHAR_SLOT_DATA& sChangeCharSlotData =
		GLCharSlotMan::Instance()->GetSlotData( CharDbNum(), _wSlotIndex );

	// �⺻ ���� ������ �ƴҰ�� üũ�Ѵ�;
	if ( ( GLCharSlotMan::Instance()->GetDefaultActiveSlotCount() <= _wSlotIndex ) && ( false == _bForce ) )
	{
		// ������ ��������� ����Ѵ�;
		if ( true == m_arrLockSlotState[ _wSlotIndex ] )
			return false;
	}

	se::ByteStream sByteStream;

	// ���� ������ ������ �����Ѵ�;
	SAVE_CURRENT_SLOT_INFO();

	// ��ų �� ������ �ʱ�ȭ�Ѵ�;
	RESET_STATS_SKILL( NULL );

	// �ٲ� ���������� �����Ѵ� ( ������ Ȱ��ȭ ������ ��쿡�� �����Ѵ� );
	if ( sChangeCharSlotData.IsActiveSlot() )
	{
		m_sStats = sChangeCharSlotData.m_sStats;
		m_wStatsPoint = sChangeCharSlotData.m_wStatsPoint;

		sChangeCharSlotData.GETEXPSKILLS_BYBUF( sByteStream );
		SETEXPSKILLS_BYBUF( sByteStream );
		m_dwSkillPoint = sChangeCharSlotData.m_dwSkillPoint;

		sChangeCharSlotData.GETSKILL_QUICKSLOT( sByteStream );
		SETSKILL_QUICKSLOT( sByteStream );
	}

	// ���� ���� ������ �����Ѵ�;
	m_wSlotIndex = _wSlotIndex;

	// ��� ��ġ�� �ٽ� �ʱ�ȭ �Ѵ�;
	INIT_DATA( FALSE, FALSE );

	// ��ȿ�� ��ų�� �˻��Ѵ�;
	SET_VALIDATION_EXPSKILL_MAP();

	return true;
}

const bool GLCHARLOGIC::GET_CHARACTER_SLOT_LOCKSTATE ( const WORD _wSlotIndex ) const
{
	if ( _wSlotIndex < 0 || EMCHAR_SLOT_DATA_SIZE <= _wSlotIndex )
		return false;

	return m_arrLockSlotState[ _wSlotIndex ];
}

const WORD GLCHARLOGIC::GET_CURRENT_CHARACTER_SLOT ( void ) const
{
	return m_wSlotIndex;
}

const bool GLCHARLOGIC::IS_USEABLE_SKILL ( const SNATIVEID& sSkillID, const WORD wSkillLevel ) const
{
	// �����ϴ� ��ų���� Ȯ���Ѵ�;
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( sSkillID.Mid(), sSkillID.Sid() );
	if ( NULL == pSkill )
		return false;

	// ��밡���� ��ų���� Ȯ���Ѵ�;
	if ( RF_DISGUISE( m_EffSkillVarSet ).IsOn() )
	{
		SKILL_MAP_CITER mapIterSkill = m_ExpChangeSkills.find( sSkillID.Id() );
		if ( mapIterSkill != m_ExpChangeSkills.end() )
		{
			SCHARSKILL sTempSkill = (*mapIterSkill).second;
			if ( sTempSkill.wLevel < wSkillLevel )
				return false;

			return true;
		}
	}
	else
	{
		SKILL_MAP_CITER mapIterSkill = m_ExpSkills.find ( sSkillID.Id() );
		if ( mapIterSkill != m_ExpSkills.end() )
		{
			SCHARSKILL sTempSkill = (*mapIterSkill).second;
			if ( sTempSkill.wLevel < wSkillLevel )
				return false;

			return true;
		}
	}
	
	return false;
}

void GLCHARLOGIC::CHANGE_CHARSKILL( const SCHARSKILL& OrgValue, SCHARSKILL& ChangeValue )
{
	if( RF_DISGUISE( m_EffSkillVarSet ).IsOn() )
	{
		SKILL_MAP_ITER _iter = m_ExpChangeSkills.begin();

		while( _iter != m_ExpChangeSkills.end() )
		{
			if( _iter->second.sNativeID == OrgValue.sNativeID )
			{
				ChangeValue = SCHARSKILL( SNATIVEID( _iter->first ), OrgValue.wLevel );
				return;
			}

			++_iter;
		}
	}

	ChangeValue = OrgValue;
}

void GLCHARLOGIC::ChangeToOrgSkill(const SNATIVEID& rID, SNATIVEID& sOrgSkill )
{
	SKILL_MAP_ITER learniter = m_ExpSkills.find ( rID.dwID );

	if ( learniter == m_ExpSkills.end() )
	{
		if( RF_DISGUISE( m_EffSkillVarSet ).IsOn() )
		{
			SKILL_MAP_ITER _iter = m_ExpChangeSkills.find( rID.dwID );

			if( _iter != m_ExpChangeSkills.end() )
			{
				sOrgSkill = _iter->second.sNativeID;
				return;
			}
		}
	}

	sOrgSkill = rID;
}

BOOL GLCHARLOGIC::ISLEARNED_SKILLEX ( SNATIVEID skill_id, SCHARSKILL& charSkill )
{
	//	Note : ���� ��ų���� �˻�.
	SKILL_MAP_ITER learniter = m_ExpSkills.find ( skill_id.dwID );
	if ( learniter == m_ExpSkills.end() )
	{
		if( RF_DISGUISE( m_EffSkillVarSet ).IsOn() )
		{
			SKILL_MAP_ITER _iter = m_ExpChangeSkills.find( skill_id.dwID );

			if( _iter != m_ExpChangeSkills.end() )
			{
				charSkill.sNativeID.dwID = (*_iter).first;
				charSkill.wLevel         = (*_iter).second.wLevel;
				
				return TRUE;
			}
		}

		return FALSE;
	}

	charSkill = (*learniter).second;

	return TRUE;
}

BOOL GLCHARLOGIC::ISLEARNED_SKILL ( SNATIVEID skill_id, WORD wLEVEL )
{
	//	Note : ��ų ���� ������.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( skill_id.wMainID, skill_id.wSubID );
	if ( !pSkill )
		return FALSE;

	//	Note : ���� ��ų���� �˻�.
	SKILL_MAP_ITER learniter = m_ExpSkills.find ( skill_id.dwID );
	if ( learniter == m_ExpSkills.end() )
	{
		if( RF_DISGUISE( m_EffSkillVarSet ).IsOn() )
		{
			SKILL_MAP_ITER _iter = m_ExpChangeSkills.find( skill_id.dwID );

			if( _iter != m_ExpChangeSkills.end() )
			{
				SCHARSKILL &sSkill = (*_iter).second;
				if ( sSkill.wLevel <  wLEVEL )
					return FALSE;

				return TRUE;
			}
		}
		
		return FALSE;
	}

	SCHARSKILL &sSkill = (*learniter).second;
	if ( sSkill.wLevel <  wLEVEL )
		return FALSE;
	
	return TRUE;
}

BOOL GLCHARLOGIC::ISMASTER_SKILL ( SNATIVEID skill_id )
{
	//	Note : ��ų ���� ������.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( skill_id.wMainID, skill_id.wSubID );
	if ( !pSkill )
		return FALSE;

	//	Note : ���� ��ų���� �˻�.
	//
	SKILL_MAP_ITER learniter = m_ExpSkills.find ( skill_id.dwID );
	if ( learniter==m_ExpSkills.end() )
		return FALSE;

	SCHARSKILL &sSkill = (*learniter).second;

	if ( DWORD(sSkill.wLevel+1)>=SKILL::MAX_LEVEL )
		return TRUE;
	if ( DWORD(sSkill.wLevel+1)>=pSkill->m_sBASIC.dwMAXLEVEL )
		return TRUE;

	return FALSE;
}

SCHARSKILL* GLCHARLOGIC::GETLEARNED_SKILL ( SNATIVEID skill_id )
{
	//	Note : ���� ��ų���� �˻�.
	//
	SKILL_MAP_ITER learniter = m_ExpSkills.find ( skill_id.dwID );

	if ( learniter == m_ExpSkills.end() )
	{
		if( RF_DISGUISE( m_EffSkillVarSet ).IsOn() )
		{
			SKILL_MAP_ITER _iter = m_ExpChangeSkills.find( skill_id.dwID );

			if( _iter != m_ExpChangeSkills.end() )
			{				
				return &((*_iter).second);
			}
		}

		return NULL;
	}

	return &((*learniter).second);
}

EMSKILL_LEARNCHECK GLCHARLOGIC::CHECKLEARNABLE_SKILL ( SNATIVEID skill_id )
{
	WORD wSKILL_LEVEL = 0;

	//	Note : ��ų ���� ������.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( skill_id.wMainID, skill_id.wSubID );
	if ( !pSkill )
		return EMSKILL_LEARN_UNKNOWN;

	//	Note : ���� ��ų���� �˻�.
	//
	SKILL_MAP_ITER learniter = m_ExpSkills.find ( skill_id.dwID );
	if ( learniter!=m_ExpSkills.end() )
	{
		SCHARSKILL &sSkill = (*learniter).second;
		wSKILL_LEVEL = sSkill.wLevel + 1;			//	���� ��ų LEVEL

		if ( wSKILL_LEVEL == SKILL::MAX_LEVEL )
			return EMSKILL_LEARN_MAX;
		if ( wSKILL_LEVEL == pSkill->m_sBASIC.dwMAXLEVEL )
			return EMSKILL_LEARN_MAX;
	}

	SKILL::SLEARN &sSKILL_LEARN = pSkill->m_sLEARN;
	SKILL::SLEARN_LVL &sSKILL_LEARN_LVL = pSkill->m_sLEARN.sLVL_STEP[wSKILL_LEVEL];

	//	Note : ��ų �������� �˻�.
	//
	if ( !(m_emClass&sSKILL_LEARN.dwCLASS) && !(skill_id.wMainID == EMSKILL_BIKE ) )
		return EMSKILL_LEARN_NOTCLASS;
	
	if ( sSKILL_LEARN.emBRIGHT!=BRIGHT_BOTH )
	{
		if ( GETBRIGHT()!=sSKILL_LEARN.emBRIGHT )
			return EMSKILL_LEARN_NOTBRIGHT;
	}

	if ( m_dwSkillPoint < sSKILL_LEARN_LVL.dwSKP )
		return EMSKILL_LEARN_NOTTERM;

	if ( WORD(GETLEVEL()) < sSKILL_LEARN_LVL.dwLEVEL )
		return EMSKILL_LEARN_NOTTERM;

	if ( sSKILL_LEARN.bOnlyOneStats )
	{
		//2014.9.18 ��ųȿ���� ��������ȿ���� ������ ������ġ ��꿡 ���Խ�Ű�� ���� ����(CRM#3014)
		if ( !m_sSUMSTATS_SKILL.CHECK_REQ_ONLY_ONE ( sSKILL_LEARN_LVL.sSTATS ) )
			return EMSKILL_LEARN_NOTTERM;
	}
	else
	{
		//2014.9.18 ��ųȿ���� ��������ȿ���� ������ ������ġ ��꿡 ���Խ�Ű�� ���� ����(CRM#3014)
		if ( !m_sSUMSTATS_SKILL.CHECK_REQ ( sSKILL_LEARN_LVL.sSTATS ) )
			return EMSKILL_LEARN_NOTTERM;
	}

	//	�䱸��ų ���� �˻�.
	if ( NATIVEID_NULL()!=sSKILL_LEARN.sSKILL )
	{
		if ( !ISLEARNED_SKILL(sSKILL_LEARN.sSKILL,WORD(sSKILL_LEARN_LVL.dwSKILL_LVL)) )
			return EMSKILL_LEARN_NOTTERM;
	}

	return EMSKILL_LEARN_OK;
}

EMSKILL_LEARNCHECK GLCHARLOGIC::CHECKLEARNABLE_SKILL ( SNATIVEID skill_id, WORD wLevel )
{
	WORD wSKILL_LEVEL = wLevel;

	//	Note : ��ų ���� ������.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( skill_id.wMainID, skill_id.wSubID );
	if ( !pSkill )
		return EMSKILL_LEARN_UNKNOWN;

	//	Note : ���� ��ų���� �˻�.
	//
	//SKILL_MAP_ITER learniter = m_ExpSkills.find ( skill_id.dwID );
	//if ( learniter!=m_ExpSkills.end() )
	//{
	//	return EMSKILL_LEARN_ALREADY;
	//}

	SKILL::SLEARN &sSKILL_LEARN = pSkill->m_sLEARN;
	SKILL::SLEARN_LVL &sSKILL_LEARN_LVL = pSkill->m_sLEARN.sLVL_STEP[wSKILL_LEVEL];

	//	Note : ��ų �������� �˻�.
	//
	if ( !(m_emClass&sSKILL_LEARN.dwCLASS) && !(skill_id.wMainID == EMSKILL_BIKE ) )
		return EMSKILL_LEARN_NOTCLASS;

	if ( sSKILL_LEARN.emBRIGHT!=BRIGHT_BOTH )
	{
		if ( GETBRIGHT()!=sSKILL_LEARN.emBRIGHT )
			return EMSKILL_LEARN_NOTBRIGHT;
	}

	if ( m_dwSkillPoint < sSKILL_LEARN_LVL.dwSKP )
		return EMSKILL_LEARN_NOTTERM;

	if ( WORD(GETLEVEL()) < sSKILL_LEARN_LVL.dwLEVEL )
		return EMSKILL_LEARN_NOTTERM;

	if ( sSKILL_LEARN.bOnlyOneStats )
	{
		//2014.9.18 ��ųȿ���� ��������ȿ���� ������ ������ġ ��꿡 ���Խ�Ű�� ���� ����(CRM#3014)
		if ( !m_sSUMSTATS_SKILL.CHECK_REQ_ONLY_ONE ( sSKILL_LEARN_LVL.sSTATS ) )
			return EMSKILL_LEARN_NOTTERM;
	}
	else
	{
		//2014.9.18 ��ųȿ���� ��������ȿ���� ������ ������ġ ��꿡 ���Խ�Ű�� ���� ����(CRM#3014)
		if ( !m_sSUMSTATS_SKILL.CHECK_REQ ( sSKILL_LEARN_LVL.sSTATS ) )
			return EMSKILL_LEARN_NOTTERM;
	}

	//	�䱸��ų ���� �˻�.
	if ( NATIVEID_NULL()!=sSKILL_LEARN.sSKILL )
	{
		if ( !ISLEARNED_SKILL(sSKILL_LEARN.sSKILL,WORD(sSKILL_LEARN_LVL.dwSKILL_LVL)) )
			return EMSKILL_LEARN_NOTTERM;
	}

	return EMSKILL_LEARN_OK;
}

void GLCHARLOGIC::SETACTIVESKILL ( SNATIVEID skill_id )
{
	m_idACTIVESKILL = skill_id;
}

EMSKILLCHECK GLCHARLOGIC::CHECHSKILL ( SNATIVEID skill_id, WORD wStrikeNum, EMVEHICLE_WHETHER emVehicle, DWORD dwCheckOption ) 
{
	// ��ų ��ȿ�� �˻�;
	const EMSKILL_VALIDATION emSkillValidation = VALIDATION_EXPSKILL( skill_id.dwID );
	if ( EMSKILL_VALIDATION_OK != emSkillValidation )
	{
		sc::writeLogError( sc::string::format(
			"[ Skill ] [ Failed Reaction Skill... %1% ( %2% ), Skill : %3%/%4%, Fail Flag : %5% ]",
			GetName(),
			CharDbNum(),
			skill_id.Mid(),
			skill_id.Sid(),
			emSkillValidation ) );

		return EMSKILL_FAILED;
	}

	//	Note : ���� ��ų���� �˻�.
	//
	SCHARSKILL sSkill;
	if ( dwCheckOption&EMSKILLCHECKOPTION_EXCEPT_NOTLEARN )
	{
		sSkill.sNativeID = skill_id;

		if ( dwCheckOption&EMSKILLCHECKOPTION_DEFENSESKILL )
		{
			if ( skill_id != m_sDefenseSkill.m_dwSkillID ) 
				return EMSKILL_NOTLEARN;

			sSkill.wLevel = m_sDefenseSkill.m_wLevel;
		}
		else
		{
			sSkill.wLevel = 0;
		}
	}
	else
	{		
		//	Note : ��ų ������ Ÿ���� �������� ����.
		//
		SKILL::DELAY_MAP_ITER delayiter = m_SKILLDELAY.find ( skill_id.dwID );
		if ( delayiter!=m_SKILLDELAY.end() )									
			return EMSKILL_DELAYTIME; // ��ų ������ �ð��� ������ ����.

		ISLEARNED_SKILLEX(skill_id, sSkill);
	}

	//	Note : ��ų ���� ������.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( skill_id );
	if ( !pSkill )
		return EMSKILL_FAILED;
	SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[sSkill.wLevel];


	//	Note : ����� �� �ִ� ��ų�������� �˻�.
	if ( pSkill->m_sBASIC.emROLE!=SKILL::EMROLE_NORMAL )
		return EMSKILL_FAILED;

	//	Note : ��ų ���� �����ؾ� �ϴ� ������ ���� �˻�.
	EMSKILLCHECK emRetArms = EMSKILL_OK;
	EMSLOT emLHand, emLHand_S;
	EMSLOT emRHand, emRHand_S;
	EMSLOT emCheckLHand, emCheckRHand;

	if ( IsUseArmSub() )
	{
		emLHand	  = SLOT_LHAND_S;
		emLHand_S = SLOT_LHAND;
		emRHand	  = SLOT_RHAND_S;
		emRHand_S = SLOT_RHAND;
	}
	else
	{
		emLHand	  = SLOT_LHAND;
		emLHand_S = SLOT_LHAND_S;
		emRHand   = SLOT_RHAND;
		emRHand_S = SLOT_RHAND_S;
	}

	emCheckLHand = emLHand;
	emCheckRHand = emRHand;

	const bool bExtremeWeapon	= pSkill->m_sLEARN.bHiddenWeapon;
	SKILL::GLSKILL_ATT emSKILL_LITEM = pSkill->m_sBASIC.emUSE_LITEM;
	SKILL::GLSKILL_ATT emSKILL_RITEM = pSkill->m_sBASIC.emUSE_RITEM;

	UINT nMaxCheckArms = 1;

	//! ����/���� �ι��˻�
	if ( dwCheckOption&EMSKILLCHECKOPTION_SUBARMS )
	{
		//! �ذ��� �϶���
		if( m_emClass&GLCC_EXTREME_M || m_emClass&GLCC_EXTREME_W )
		{
			nMaxCheckArms = 2;
		}
	}

	for ( UINT i=0; i<nMaxCheckArms; ++i )
	{
		emRetArms = EMSKILL_OK;

		if ( emSKILL_LITEM!=SKILL::SKILLATT_NOCARE )
		{
			SITEM* pItem = GET_SLOT_ITEMDATA(emCheckLHand);
			if ( !pItem )
			{
				emRetArms = EMSKILL_NOTITEM;
				goto _FAILED_CHECK_ARMS;
			}

			if ( !CHECHSKILL_ITEM(emSKILL_LITEM,CONVERT_ITEMATT( pItem->sSuitOp.emAttack ),bExtremeWeapon) )
			{
				emRetArms = EMSKILL_NOTITEM;
				goto _FAILED_CHECK_ARMS;
			}
		}

		if ( emSKILL_RITEM!=SKILL::SKILLATT_NOCARE )
		{
			SITEM* pItem = GET_SLOT_ITEMDATA(emCheckRHand);
			if ( !pItem )		
			{
				emRetArms = EMSKILL_NOTITEM;
				goto _FAILED_CHECK_ARMS;
			}

			if ( !CHECHSKILL_ITEM(emSKILL_RITEM,CONVERT_ITEMATT( pItem->sSuitOp.emAttack ),bExtremeWeapon) )		
			{
				emRetArms = EMSKILL_NOTITEM;
				goto _FAILED_CHECK_ARMS;
			}
		}

		//	Note : ȭ�� �Ҹ�� �Ҹ� ���� üũ.
		//		--> �Ҹ��� 2 ���δ� �Ѱ��� �����ִ� ��Ȳ? ���� ���� ���ڸ� �����ϰ� ��ų�� �ߵ��ǰ� ����.
		//
		if ( sSKILL_DATA.wUSE_ARROWNUM!=0 )
		{
			SITEM* pItem = GET_SLOT_ITEMDATA(emCheckLHand);
			if ( !pItem )
			{
				emRetArms = EMSKILL_NOTARROW;
				goto _FAILED_CHECK_ARMS;
			}

			if ( pItem->sBasicOp.emItemType != ITEM_ARROW )					
			{
				emRetArms = EMSKILL_NOTARROW;
				goto _FAILED_CHECK_ARMS;
			}

			//	������ �������� �ʴ´�. ���ڶ� ��� �ִ°� �ѵ������� �Ҹ��Ŵ.
		}

		//	Note : ���� �Ҹ�� �Ҹ� ���� üũ.
		//		--> �Ҹ��� 2 ���δ� �Ѱ��� �����ִ� ��Ȳ? ���� ���� ���ڸ� �����ϰ� ��ų�� �ߵ��ǰ� ����.
		//
		if ( sSKILL_DATA.wUSE_CHARMNUM!=0 )
		{
			SITEM* pItem = GET_SLOT_ITEMDATA(emCheckLHand);
			if ( !pItem )			
			{
				emRetArms = EMSKILL_NOTCHARM;
				goto _FAILED_CHECK_ARMS;
			}

			if ( pItem->sBasicOp.emItemType != ITEM_CHARM )						
			{
				emRetArms = EMSKILL_NOTCHARM;
				goto _FAILED_CHECK_ARMS;
			}

			//	������ �������� �ʴ´�. ���ڶ� ��� �ִ°� �ѵ������� �Ҹ��Ŵ.
		}

		//	Note : �Ѿ� �Ҹ�� �Ҹ� ���� üũ.
		//		--> �Ҹ��� 2 ���δ� �Ѱ��� �����ִ� ��Ȳ? ���� ���� ���ڸ� �����ϰ� ��ų�� �ߵ��ǰ� ����.
		//
		if ( sSKILL_DATA.wUSE_BULLETNUM!=0 )
		{
			SITEM* pItem = GET_SLOT_ITEMDATA(emCheckLHand);
			if ( !pItem )
			{
				emRetArms = EMSKILL_NOTBULLET;
				goto _FAILED_CHECK_ARMS;
			}

			if ( pItem->sBasicOp.emItemType != ITEM_BULLET )
			{
				emRetArms = EMSKILL_NOTBULLET;
				goto _FAILED_CHECK_ARMS;
			}

			//	������ �������� �ʴ´�. ���ڶ� ��� �ִ°� �ѵ������� �Ҹ��Ŵ.
		}

		//	Note : ���͸� �Ҹ�� �Ҹ� ���� üũ.
		//		--> �Ҹ��� 2 ���δ� �Ѱ��� �����ִ� ��Ȳ? ���� ���� ���ڸ� �����ϰ� ��ų�� �ߵ��ǰ� ����.
		//
		if ( sSKILL_DATA.nUSE_BATTERY != 0 )
		{
			SITEM* pItem = GET_SLOT_ITEMDATA( SLOT_VEHICLE );
			if ( !pItem )	
			{
				emRetArms = EMSKILL_NOTBATTERY;
				goto _FAILED_CHECK_ARMS;
			}

			if ( pItem->sBasicOp.emItemType != ITEM_VEHICLE )
			{
				emRetArms = EMSKILL_NOTBATTERY;
				goto _FAILED_CHECK_ARMS;
			}

			if( pItem->sVehicle.emVehicleType != VEHICLE_TYPE_BIKE )				
			{
				emRetArms = EMSKILL_NOTDRIVER;
				goto _FAILED_CHECK_ARMS;
			}

			if( emVehicle != EMSKILL_NOTDRIVER ) // ��� ���ľ� �Ǵ��� �˼� ����.
			{
				emRetArms = EMSKILL_NOTDRIVER;
				goto _FAILED_CHECK_ARMS;
			}

			//	������ �������� �ʴ´�. ���ڶ� ��� �ִ°� �ѵ������� �Ҹ��Ŵ.
		}

		// ��밡���� ��ų�� �ƴҰ�� ó���Ѵ�;
		if ( false == IS_USEABLE_SKILL( skill_id, sSkill.wLevel ) )
			return EMSKILL_NOTLEARN;

_FAILED_CHECK_ARMS:
		if ( emRetArms == EMSKILL_OK )
		{
			break;
		}

		emCheckLHand = emLHand_S;
		emCheckRHand = emRHand_S;
	}

	if ( emRetArms != EMSKILL_OK )
	{
		return emRetArms;
	}

	const int sizeStrike(wStrikeNum);
	//	Note : ���� ü�� �Ҹ� ����.
	if ( m_sHP.nNow <= int(sSKILL_DATA.wUSE_HP * sizeStrike) )
        return EMSKILL_NOTHP;

	if ( m_sMP.nNow < int(sSKILL_DATA.wUSE_MP * sizeStrike) )
        return EMSKILL_NOTMP;

	if ( m_sSP.nNow < int(sSKILL_DATA.wUSE_SP * sizeStrike) )
		return EMSKILL_NOTSP;	


	SITEM* pRHAND = GET_SLOT_ITEMDATA ( emRHand );
	SITEM* pLHAND = GET_SLOT_ITEMDATA ( emLHand );

	int nDisSP(sSKILL_DATA.wUSE_SP);
	if ( pRHAND )
		nDisSP += int(pRHAND->sSuitOp.wReqSP);

	if ( pLHAND )
		nDisSP += int(pLHAND->sSuitOp.wReqSP);

	if ( m_sSP.nNow < int(nDisSP * sizeStrike) )
		return EMSKILL_NOTSP;

	//  Note : ��ų ���� �ʿ� cp üũ
	if ( m_sCP.wNow < (sSKILL_DATA.wUSE_CP * wStrikeNum) )
		return EMSKILL_NOTCP;

	//	NEED : ���� ��Ƽ��ų�� �����Ǿ�����.
	//
	//sSKILL_DATA.wUSE_SP_PTY;

	return EMSKILL_OK;
}

BOOL GLCHARLOGIC::GetSkillLevel( const SNATIVEID& idSkill, SCHARSKILL& sSkill )
{
	sSkill.sNativeID = idSkill;
	if( IsDefenseSkill() )
	{
		if( idSkill != m_sDefenseSkill.m_dwSkillID )
			return FALSE;

		sSkill.wLevel = m_sDefenseSkill.m_wLevel;
	}
	else
	{
		/*
		SKILL_MAP_ITER learniter = m_ExpSkills.find ( idSkill.dwID );
		if( learniter == m_ExpSkills.end() )	
			return FALSE;

		sSkill = (*learniter).second;
		*/
		if(!ISLEARNED_SKILLEX( idSkill, sSkill ) ) return FALSE;
	} //if..else

	return TRUE;
} //GLCHARLOGIC::GetSkillLevel

void GLCHARLOGIC::ACCOUNTSKILL ( const SCHARSKILL& sSkill, WORD wStrikeNum, bool bServer )
{
	PGLSKILL	pSkill = GLSkillMan::GetInstance().GetData( sSkill.sNativeID.wMainID, sSkill.sNativeID.wSubID );
	if( !pSkill )
		return;

	SKILL::CDATA_LVL&		sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[sSkill.wLevel];

	//	��ų ������ ���.
	const float fDecreaseT(sSKILL_DATA.fDELAYTIME*GetDecR_SkillDelayT(pSkill->m_sBASIC.dwGRADE-1))
		, fDelayTime(sSKILL_DATA.fDELAYTIME-fDecreaseT);
    SKILL::SKILLDELAY sSkilldelay;
    sSkilldelay.fDelayTime = GLOGICEX::SKILLDELAY( pSkill->m_sBASIC.dwGRADE,sSkill.wLevel,GETLEVEL(), fDelayTime );

	sSkilldelay.fDefultMaxDelayTime = sSkilldelay.fDelayTime;

	//	Note : �����̻��� ������ ����.
	sSkilldelay.fDelayTime = sSkilldelay.fDelayTime * m_fSTATE_DELAY;
    sSkilldelay.fMAXDelayTime = sSkilldelay.fDelayTime;

	//	Note : ���������� �޽��� ������ ��ŭ ���ҽ��� �ش�.
	if( bServer )
		APPLY_MSGDELAY( sSkilldelay.fDelayTime );

	m_SKILLDELAY.insert( std::make_pair(sSkill.sNativeID.dwID,sSkilldelay) );

	EMSLOT emLHand = GetCurLHand();

	//	ȭ�� �Ҹ�.
	if( sSKILL_DATA.wUSE_ARROWNUM!= 0 )
	{
		SITEM*		pItem = GET_SLOT_ITEMDATA(emLHand);
		if( pItem )
		{
			//	ȭ�� �Ҹ��ϴ� ��ų������ ȭ������ �ݵ�� �־�� ������ ������ �������� ���� ������ ����.
			//	ȭ�� ���� ������ �������� �ʴ´�.
			WORD &wTurnNum = m_PutOnItems[emLHand].wTurnNum;
			if( wTurnNum >= sSKILL_DATA.wUSE_ARROWNUM )
				wTurnNum -= sSKILL_DATA.wUSE_ARROWNUM;
			else
				wTurnNum = 0;
		} //if
	} //if

	//	���� �Ҹ�.
	if( sSKILL_DATA.wUSE_CHARMNUM!= 0 )
	{
		SITEM* pItem = GET_SLOT_ITEMDATA(emLHand);
		if ( pItem )
		{
			//	���� �Ҹ��ϴ� ��ų������ ȭ������ �ݵ�� �־�� ������ ������ �������� ���� ������ ����.
			//	���� ���� ������ �������� �ʴ´�.
			WORD &wTurnNum = m_PutOnItems[emLHand].wTurnNum;
			if( wTurnNum >= sSKILL_DATA.wUSE_CHARMNUM )
				wTurnNum -= sSKILL_DATA.wUSE_CHARMNUM;
			else
				wTurnNum = 0;
		} //if
	} //if

	//	�Ѿ� �Ҹ�.
	if( sSKILL_DATA.wUSE_BULLETNUM!= 0 )
	{
		SITEM* pItem = GET_SLOT_ITEMDATA(emLHand);
		if( pItem )
		{
			//	�Ѿ� ���� ������ �������� �ʴ´�.
			WORD &wTurnNum = m_PutOnItems[emLHand].wTurnNum;
			if( wTurnNum >= sSKILL_DATA.wUSE_BULLETNUM )
				wTurnNum -= sSKILL_DATA.wUSE_BULLETNUM;
			else
				wTurnNum = 0;
		} //if
	} //if

	//	���� ü�� �Ҹ�.
	m_sHP.DECREASE(int(sSKILL_DATA.wUSE_HP * wStrikeNum));
	m_sMP.DECREASE(int(sSKILL_DATA.wUSE_MP * wStrikeNum));
	m_sCP.DECREASE(int(sSKILL_DATA.wUSE_CP * wStrikeNum));
	//m_sExperience.DECREASE ( sSKILL_DATA.wUSE_EXP*wStrikeNum );
}

bool GLCHARLOGIC::CHECKPROBABILITYSKILL(const PGLSKILL pSkill, const WORD wLevel)
{
	// Ȯ��/������ ��� �Ǿ��� ���� �ۿ��ϴ� ��ų�� ��� �̰����� Ȯ�� ��� �� �����ϸ� false ����;
	for ( int i=0; i<SKILLFACT_SIZE; ++i )
	{
// 		const size_t SizeADDON = pSkill->m_sAPPLY.vecADDON.size();
// 		for ( DWORD _i = 0; _i < SizeADDON; ++_i )
// 		{
// 		}
		const size_t SizeSPEC = pSkill->m_sAPPLY.vecSPEC.size();
		for ( DWORD _i = 0; _i < SizeSPEC; ++_i )
		{
			const SKILL::SSPEC& sSPEC = pSkill->m_sAPPLY.vecSPEC[_i].sSPEC[wLevel];
			const EMSPEC_ADDON emSPEC = pSkill->m_sAPPLY.vecSPEC[_i].emSPEC;
			switch ( emSPEC )
			{
			case EMSPECA_DOMINATION:
				if ( sSPEC.fVAR1 < sc::Random::getInstance().RANDOM_POS() )
					return false;
				break;
			case EMSPECA_INVISIBLE:
				
				if ( m_bSkillFactsSpecial[EMSPECA_TURN_STATE_BATTLE] && sSPEC.dwFLAG )
					return false;
				break;
			}
		}
	}
	return true;
}

EMGRINDINGCONDITION GLCHARLOGIC::GRINDING_TL_CONDITION_CHECK( const SITEMCUSTOM &sCusItem, const SITEM* pHold, const SITEMCUSTOM& sHoldItemCustom, CTime time )
{
	// ������ ��� ����� �������� ������ �ʴ´�;
	// �������� ���󰣴�;
	const SITEM* pItem = GLogicData::GetInstance().GetItem ( sCusItem.GetRealNativeID() );

	if ( !pItem ) return EMGRINDINGCONDITION_FAILED;
	if ( !pHold ) return EMGRINDINGCONDITION_FAILED;

	BYTE			 cGRADE			   = sCusItem.GETGRADE ( EMGRINDING_NUNE );
	WORD			 wENCHANT_MIN	   = 0;
	WORD		     wENCHANT_MAX	   = GRADE_LIMIT_MAX;

	// dwCureVolume ���� ��ȭ������ ����Ѵ�. �̹� ��ȭ�� �������� ��޺��� ���ų� �����ÿ��� ����Ҽ� ����.
	if( !sCusItem.IsTempEnchant() && cGRADE >= pHold->sDrugOp.dwCureVolume ) return EMGRINDINGCONDITION_MIN;

	if ( pHold->sBasicOp.emItemType != ITEM_TEMP_ENCHANT) return EMGRINDINGCONDITION_DEFCLASS;

	// ��ȭ�Ұ� ���� üũ
	BOOL bGrinding = (pItem->sBasicOp.emItemType == ITEM_SUIT) &&
		( EMREMODEL_TYPE_DAMAGE == pItem->sSuitOp.wReModelNum || EMREMODEL_TYPE_DEFENCEANDRESIST == pItem->sSuitOp.wReModelNum );
	if ( !bGrinding )
	{
		return EMGRINDINGCONDITION_NOITEM;
	}
	//! �ʼ�üũ 2
	if ( cGRADE >= GRADE_LIMIT_MAX )
	{
		return EMGRINDINGCONDITION_MAX;
	}

	// �Ⱓ���̴��� �ð�üũ�ؼ� ��밡������ �Ǵ��Ѵ�;
	if ( sCusItem.IsTimeLimit( SITEMCUSTOM::CHECKFLAGOP_NORMAL ) )
	{
		// Ÿ�پ������� ����ð��� ���Ѵ�;
		CTime sTargetItemTime( sCusItem.tBORNTIME );
		CTimeSpan sTargetItemSPAN( pItem->sDrugOp.tTIME_LMT );
		sTargetItemTime += sTargetItemSPAN + sCusItem.GetPeriodExBasic();

		// Ȧ������� ( �Ͻð�ȭ������ ) �� ���� ����ð��� ���Ѵ�;
		CTime sHoldItemTime = CTime::GetCurrentTime();
		CTimeSpan sHoldItemSPAN( pHold->sDrugOp.tDuration );
		sHoldItemTime += sHoldItemSPAN;

		if ( sTargetItemTime < sHoldItemTime )
			return EMGRINDINGCONDITION_TIME_LIMIT;

		// �Ⱓ���� ��� ���Ұ�;
		//return EMGRINDINGCONDITION_TIME_LIMIT;
	}

	const CTime cTIME_CUR = time;

	CTime cTimeLimit( sCusItem.sTLGrind.tFireTime );

	if( sCusItem.sTLGrind.cGradeValue > 0 || cTIME_CUR < cTimeLimit )
	{
		return EMGRINDINGCONDITION_RE_ENCHANT;
	}
			
	return EMGRINDINGCONDITION_SUCCESS;
}

EMGRINDINGCONDITION GLCHARLOGIC::GRINDING_CONDITION_CHECK( const SITEMCUSTOM &sCusItem, const SITEM* pHold, const SITEMCUSTOM& sHoldItemCustom )
{
	// ������ ��� ����� �������� ������ �ʴ´�;
	// �������� ���󰣴�;
	const SITEM* pItem = GLogicData::GetInstance().GetItem ( sCusItem.GetRealNativeID() );
	if ( !pItem )
	{
		return EMGRINDINGCONDITION_NOITEM;
	}

	const SINFO_BYITEMLEVEL& sInfoByItemLevel =
		GLItemLevelMan::Instance()->GetInfoByItemLevel( pItem->sBasicOp.emLevel );

	if ( !pHold )
	{
		return EMGRINDINGCONDITION_FAILED;
	}

	EMGRINDING_TYPE  emGRINDING		   = pHold->sGrindingOp.emTYPE;
	EMGRINDER_TYPE	 emGRINDERLEVEL	   = pHold->sGrindingOp.emGRINDER_TYPE;
	EMGRINDING_CLASS emGRINDERPURPOSE  = pHold->sGrindingOp.emCLASS;
	BYTE			 cGRADE			   = sCusItem.GETGRADE ( emGRINDING );
	WORD			 wENCHANT_MIN	   = 0;
	WORD		     wENCHANT_MAX	   = GRADE_LIMIT_MAX;
	bool			 bRANDOM		   = false;
	bool			 bADDED			   = false;
	bool			 bIGNOREDEFAULTREQ = false;


	if( sCusItem.IsTempEnchant() > 0 ) 
	{
		return EMGRINDINGCONDITION_TLGRIND_ENCHANT;
	}
	//! Grinding opt ������ �ִٸ� ���� ������ ���󰣴�.
	SGRINDING_OPT* pGrindingOpt = NULL;

	if ( !pHold->sGrindingOp.strGRINDER_OPT_FILE.empty() )
	{
		pGrindingOpt = GLItemMan::GetInstance().GetItemGrindingOpt( pHold->sBasicOp.sNativeID );

		if ( !pGrindingOpt )
		{
			return EMGRINDINGCONDITION_FAILED;
		}

		if ( !pGrindingOpt->ISTYPENULL() )
		{
			emGRINDING = pGrindingOpt->emTYPE;
			cGRADE	   = sCusItem.GETGRADE ( emGRINDING );
		}

		if ( !pGrindingOpt->ISLEVELNULL() )
		{
			emGRINDERLEVEL = pGrindingOpt->emLEVEL;
		}

		if ( !pGrindingOpt->ISPURPOSENULL() )
		{
			emGRINDERPURPOSE = pGrindingOpt->emPURPOSE;
		}

		if ( !pGrindingOpt->ISENCHANTMINNULL() )
		{
			wENCHANT_MIN = max(wENCHANT_MIN, pGrindingOpt->wENCHANT_MIN);
		}

		if ( !pGrindingOpt->ISENCHANTMAXNULL() )
		{
			wENCHANT_MAX = min(wENCHANT_MAX, pGrindingOpt->wENCHANT_MAX);
		}

		bRANDOM			  = pGrindingOpt->dwATTRIBUTE&EMGRINDING_ATTRIBUTE_RANDOM ? true : false;
		bADDED			  = pGrindingOpt->dwATTRIBUTE&EMGRINDING_ATTRIBUTE_ADDED ? true : false;
		bIGNOREDEFAULTREQ = pGrindingOpt->dwATTRIBUTE&EMGRINDING_ATTRIBUTE_IGNOREDEFAULTREQ ? true : false;
	}

	//! �ʼ�üũ 1
	BOOL bGrinding = (pItem->sBasicOp.emItemType == ITEM_SUIT) &&
		( EMREMODEL_TYPE_DAMAGE == pItem->sSuitOp.wReModelNum || EMREMODEL_TYPE_DEFENCEANDRESIST == pItem->sSuitOp.wReModelNum );
	if ( !bGrinding )
	{
		return EMGRINDINGCONDITION_NOITEM;
	}

	//! �ʼ�üũ 2
	if ( cGRADE >= GRADE_LIMIT_MAX )
	{
		return EMGRINDINGCONDITION_MAX;
	}

	//! ��ũ��Ʈ�� Grinding Logic �� �ִٸ� ������ ���󰣴�.
	if ( pGrindingOpt && pGrindingOpt->ISENABLELOGIC_CONDITION() )
	{
		CGrindingScript* pScript = pGrindingOpt->GETSCRIPT();

		if ( pScript )
		{
			EMGRINDINGCONDITION emRet = EMGRINDINGCONDITION(pScript->DoCondition( sHoldItemCustom, sCusItem ));
			return emRet;
		}
	}

	//! �⺻üũ 1 Ÿ�� üũ
	if ( EMREMODEL_TYPE_DAMAGE == pItem->sSuitOp.wReModelNum )
	{
		if ( EMGRINDING_DAMAGE != emGRINDING ||
			EMGRINDING_CLASS_ARM != emGRINDERPURPOSE )
		{
			return EMGRINDINGCONDITION_DEFCLASS;
		}
	}
	else if ( EMREMODEL_TYPE_DEFENCEANDRESIST == pItem->sSuitOp.wReModelNum )
	{
		if ( ( EMGRINDING_DEFENSE != emGRINDING &&
			EMGRINDING_RESIST_FIRE != emGRINDING &&
			EMGRINDING_RESIST_ICE != emGRINDING &&
			EMGRINDING_RESIST_ELEC != emGRINDING &&
			EMGRINDING_RESIST_POISON != emGRINDING &&
			EMGRINDING_RESIST_SPIRIT != emGRINDING ) ||
			EMGRINDING_CLASS_CLOTH != emGRINDERPURPOSE )
		{
			return EMGRINDINGCONDITION_DEFCLASS;
		}
	}
	else
	{
		return EMGRINDINGCONDITION_DEFCLASS;
	}

	/*if ( pItem->GetAttackDamage() == GLPADATA(0,0) )
	{
		if ( emGRINDERPURPOSE != EMGRINDING_CLASS_CLOTH )
		{
			return EMGRINDINGCONDITION_DEFCLASS;
		}
	}
	else
	{
		if ( emGRINDERPURPOSE != EMGRINDING_CLASS_ARM )
		{
			return EMGRINDINGCONDITION_DEFCLASS;
		}
	}*/

	//! �⺻üũ 2 ���� ������ �ּ� ��ġ üũ
	if ( cGRADE < wENCHANT_MIN )
	{
		return EMGRINDINGCONDITION_MIN;
	}

	//! �⺻üũ 3 ���� ������ �ִ� ��ġ üũ
	if ( !pGrindingOpt || pGrindingOpt->ISENCHANTMAXNULL() )
	{
		if ( emGRINDING == EMGRINDING_DAMAGE || emGRINDING == EMGRINDING_DEFENSE )
		{
			if ( cGRADE>=/*GLCONST_CHAR::wGRADE_MAX*/sInfoByItemLevel.wMaxGrade )
			{
				return EMGRINDINGCONDITION_MAX;
			}
		}
		else
		{
			if ( cGRADE>=/*GLCONST_CHAR::wGRADE_MAX_REGI*/sInfoByItemLevel.wMaxResist )
			{
				return EMGRINDINGCONDITION_MAX;
			}
		}
	}
	
	if ( wENCHANT_MAX <= static_cast<WORD>(cGRADE) )
	{
		if ( bRANDOM )
		{
			return EMGRINDINGCONDITION_RANDOM_MAX;
		}
		else
		{
			return EMGRINDINGCONDITION_DEFCLASS;
		}
	}
	
	if ( pGrindingOpt && bRANDOM && !bADDED && !pGrindingOpt->ISRANGEMAXNULL() )
	{
		if ( pGrindingOpt->wRANGE_MAX <= static_cast<WORD>(cGRADE) ) 
		{
			return EMGRINDINGCONDITION_RANDOM_MAX;
		}
	}

	//! �⺻ �䱸���� ���ð� �ƴ϶��
	if ( !bIGNOREDEFAULTREQ )
	{
		if ( cGRADE >= GRADE_HIGH && pItem->sGrindingOp.emGRINDER_TYPE != EMGRINDER_TOP )
		{
			return EMGRINDINGCONDITION_NOTBESTITEM;
		}

		//! ������ ���, ���� �������� ���� ���� �Ұ���
		if ( cGRADE >= GRADE_HIGH && emGRINDERLEVEL != EMGRINDER_TOP )
		{
			return EMGRINDINGCONDITION_NOTBEST;
		}
		else if ( cGRADE >=GRADE_NORMAL && emGRINDERLEVEL < EMGRINDER_HIGH )
		{
			return EMGRINDINGCONDITION_NOTHIGH;
		}

		//! ������ ���� Ȯ��
		if ( cGRADE >= GRADE_HIGH )
		{
			WORD wNUM = GRINDING_CONSUME_NUM(sCusItem, pHold, sHoldItemCustom);

			if ( wNUM == 0 )
			{
				return EMGRINDINGCONDITION_FAILED;
			}

			if ( wNUM > sHoldItemCustom.wTurnNum )
			{			
				return EMGRINDINGCONDITION_NOTNUM;
			}
		}

		//Note : ���� �������� ���� GRADE_NORMAL �� �̸� ���� �Ұ���.
		//
		//if ( pInvenItem->sItemCustom.GETGRADE(pHold->sGrindingOp.emTYPE)<GRADE_NORMAL && pHold->sGrindingOp.bHIGH )
		//{
		//	m_pInterface->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("GRINDING_NOT_LOW") );
		//	return S_FALSE;
		//}

	}
	else
	{
		//! ��ũ��Ʈ�� �Ҹ𰳼��� �����ϸ� ���� Ȯ���� �Ѵ�
		if ( pGrindingOpt && !pGrindingOpt->ISCONSUMENULL() )
		{
			WORD wNUM = GRINDING_CONSUME_NUM(sCusItem, pHold, sHoldItemCustom);

			if ( wNUM == 0 )
			{
				return EMGRINDINGCONDITION_FAILED;
			}

			if ( wNUM > sHoldItemCustom.wTurnNum )
			{			
				return EMGRINDINGCONDITION_NOTNUM;
			}
		}
	}

	return EMGRINDINGCONDITION_SUCCESS;
}


WORD GLCHARLOGIC::GRINDING_CONSUME_NUM( const SITEMCUSTOM &sCusItem, const SITEM* pHold, const SITEMCUSTOM& sHoldItemCustom )
{
	const SITEM* pItem = GLogicData::GetInstance().GetItem ( sCusItem.GetNativeID() );

	if ( !pItem )
	{
		return NULL;
	}

	const SINFO_BYITEMLEVEL& sInfoByItemLevel =
		GLItemLevelMan::Instance()->GetInfoByItemLevel( pItem->sBasicOp.emLevel );

	EMGRINDING_TYPE  emGRINDING	 = pHold->sGrindingOp.emTYPE;
	BYTE			 cGRADE		 = sCusItem.GETGRADE( emGRINDING );

	//! Grinding opt ������ �ִٸ� ���� ������ ���󰣴�.
	if ( !pHold->sGrindingOp.strGRINDER_OPT_FILE.empty() )
	{
		SGRINDING_OPT* pGrindingOpt = GLItemMan::GetInstance().GetItemGrindingOpt( pHold->sBasicOp.sNativeID );

		if ( !pGrindingOpt )
		{
			return NULL;
		}

		//! �׸��� ��ũ��Ʈ�� Grinding Logic �� �ִٸ� ������ ���󰣴�.
		if ( pGrindingOpt->ISENABLELOGIC_CONSUME() )
		{
			CGrindingScript* pScript = pGrindingOpt->GETSCRIPT();

			if ( pScript )
			{
				WORD wRet = WORD(pScript->DoConsume( sHoldItemCustom, sCusItem ));
				return wRet;
			}
		}

		//! �⺻ �䱸���� üũ ���ð� �ִٸ�
		if ( pGrindingOpt->dwATTRIBUTE&EMGRINDING_ATTRIBUTE_IGNOREDEFAULTREQ )
		{
			return 1;
		}

		if ( !pGrindingOpt->ISCONSUMENULL() )
		{
			return pGrindingOpt->wCONSUME;
		}

		if ( !pGrindingOpt->ISTYPENULL() )
		{
			emGRINDING = pGrindingOpt->emTYPE;
			cGRADE	   = sCusItem.GETGRADE( emGRINDING );
		}
	}

	//! ������ ���� Ȯ��
	if ( cGRADE >= GRADE_HIGH )
	{			
		//return GLCONST_CHAR::wUSE_GRADE_NUM[cGRADE-GRADE_HIGH];
		return sInfoByItemLevel.wArrUseItemCountByHighGrade[cGRADE-GRADE_HIGH];
	}

	return 1;
}

void GLCHARLOGIC::DOTLGRINDING( SITEMCUSTOM &sCusItem, const SITEM *pHold, BYTE& cRESULTGRADE, bool bReEnchant )
{
	const SITEM* pItem = GLogicData::GetInstance().GetItem( sCusItem.GetNativeID() );
	if ( !pItem )
	{
		return;
	}

	EMGRINDING_TYPE emGRINDING = EMGRINDING_NUNE;

	if ( EMREMODEL_TYPE_DAMAGE == pItem->sSuitOp.wReModelNum )
	{
		emGRINDING = EMGRINDING_DAMAGE;
	}
	else if ( EMREMODEL_TYPE_DEFENCEANDRESIST == pItem->sSuitOp.wReModelNum )
	{
		emGRINDING = EMGRINDING_DEFENSE;
	}
	else
	{
		// ���� �� ����;
		sc::writeLogError( "[ Item Log ] [ Failed DOTLGRINDING. ]" );

		return;
	}
	
	/*if ( pItem->GetAttackDamage() == GLPADATA(0,0) )
	{
		emGRINDING = EMGRINDING_DEFENSE;
	}
	else
	{
		emGRINDING = EMGRINDING_DAMAGE;
	}*/

	cRESULTGRADE = pHold->sDrugOp.dwCureVolume;
	
	if( cRESULTGRADE > 0 )
	{
		switch ( emGRINDING )
		{
		case EMGRINDING_DAMAGE:
			{
				if( !bReEnchant ) sCusItem.sTLGrind.cGradeValue = sCusItem.cDAMAGE;

				sCusItem.cDAMAGE		      = cRESULTGRADE;
			}			
			break;
		case EMGRINDING_DEFENSE:		
			{
				if( !bReEnchant ) sCusItem.sTLGrind.cGradeValue = sCusItem.cDEFENSE;

				sCusItem.cDEFENSE		      = cRESULTGRADE; 
			}			
			break;	
		};

		CTime cTIME = CTime::GetCurrentTime();

		CTimeSpan cSPAN(pHold->sDrugOp.tDuration);
		cTIME += cSPAN;

		sCusItem.sTLGrind.tFireTime = cTIME.GetTime();
	}
}

DWORD GLCHARLOGIC::DOGRINDING(
    SITEMCUSTOM& sCusItem, 
    const SITEM* pHold,
    SITEMCUSTOM& sHoldItemCustom,
    DWORD dwGrindingOption,
    BYTE& cRESULTGRADE)
{

	DWORD dwGrindingResult(EMGRINDINGRESULT_FAILED);

	if ( !pHold )
	{
		return dwGrindingResult;
	}

	const SITEM* pItem = GLogicData::GetInstance().GetItem( sCusItem.GetNativeID() );
	if ( !pItem )
	{
		return dwGrindingResult;
	}

	// ������ ��� �� ������ ������ �´�;
	EMITEMLEVEL emItemLevel = pItem->sBasicOp.emLevel;
	const SINFO_BYITEMLEVEL& sInfoByItemLevel = GLItemLevelMan::Instance()->GetInfoByItemLevel( emItemLevel );

	// ���� ������ ��� �ε����� �ϳ� �� ����Ѵ�.
	const UINT nGRADE_LIMIT_MAX = GRADE_LIMIT_MAX+1;

	// �⺻ Ȯ�� ����
	float fGRADERATE[nGRADE_LIMIT_MAX];
	//memcpy(fGRADERATE, GLCONST_CHAR::fGRADE_RATE, sizeof(GLCONST_CHAR::fGRADE_RATE));
	memcpy(fGRADERATE, sInfoByItemLevel.fArrSuccessRateByGrade, sizeof(sInfoByItemLevel.fArrSuccessRateByGrade));

	EMGRINDING_TYPE emGRINDING			= pHold->sGrindingOp.emTYPE;
	EMGRINDER_TYPE	emGRINDERLEVEL		= pHold->sGrindingOp.emGRINDER_TYPE;
	WORD			wENCHANT_MIN		= 0;
	WORD			wENCHANT_MAX		= GRADE_LIMIT_MAX;
	BYTE			cGRADE				= sCusItem.GETGRADE( emGRINDING );
	BYTE			cADDGRADE			= 0; // �����ɼ����� ���� �߰��� ������ġ
	//BYTE			cRESULTGRADE		= 0;
	bool			bRANDOM				= false;
	bool			bNODEGRATION		= false;
	bool			bIGNOREPROTECTED	= false;
	bool			bIGNORERATEINC		= false;
	bool			bIGNOREDEFAULTREQ	= false;
	bool			bADDED				= false;

	//! Grinding opt ������ �ִٸ� ���� ������ ���󰣴�.
	SGRINDING_OPT* pGrindingOpt = NULL;

	if ( !pHold->sGrindingOp.strGRINDER_OPT_FILE.empty() )
	{
		pGrindingOpt = GLItemMan::GetInstance().GetItemGrindingOpt( pHold->sBasicOp.sNativeID );

		if ( !pGrindingOpt )
		{
			sc::writeLogError( std::string("GLCHARLOGIC::DOGRINDING() - pGrindingOpt is null") );
			return dwGrindingResult;
		}

		//! �׸��� ��ũ��Ʈ�� Grinding Logic �� �ִٸ� ������ ���󰣴�.
		if ( pGrindingOpt->ISENABLELOGIC() )
		{
			CGrindingScript* pScript = pGrindingOpt->GETSCRIPT();

			if ( pScript )
			{
				dwGrindingResult = static_cast<DWORD>(pScript->DoEntry( dwGrindingOption, sHoldItemCustom, sCusItem ));
				return dwGrindingResult;
			}
		}

		if ( !pGrindingOpt->ISTYPENULL() )
		{
			emGRINDING = pGrindingOpt->emTYPE;
			cGRADE	   = sCusItem.GETGRADE ( emGRINDING );
		}

		if ( !pGrindingOpt->ISLEVELNULL() )
		{
			emGRINDERLEVEL = pGrindingOpt->emLEVEL;
		}

		if ( !pGrindingOpt->ISENCHANTMINNULL() )
		{
			wENCHANT_MIN = max(wENCHANT_MIN, pGrindingOpt->wENCHANT_MIN);
		}

		if ( !pGrindingOpt->ISENCHANTMAXNULL() )
		{
			wENCHANT_MAX = min(wENCHANT_MAX, pGrindingOpt->wENCHANT_MAX);
		}

		if ( !pGrindingOpt->ISCHANCELISTNULL() )
		{
			SGRINDING_OPT::MAP_CHANCE_ITER iter = pGrindingOpt->mapCHANCE.begin();
			for ( ; iter!=pGrindingOpt->mapCHANCE.end(); ++iter )
			{
				WORD wLevel = iter->first;

				if ( nGRADE_LIMIT_MAX < wLevel )
				{
					continue;
				}

				fGRADERATE[wLevel] = iter->second;
			}
		}

		bRANDOM				= pGrindingOpt->dwATTRIBUTE&EMGRINDING_ATTRIBUTE_RANDOM ? true : false;
		bNODEGRATION		= pGrindingOpt->dwATTRIBUTE&EMGRINDING_ATTRIBUTE_NODEGRATION ? true : false;
		bIGNOREPROTECTED	= pGrindingOpt->dwATTRIBUTE&EMGRINDING_ATTRIBUTE_IGNOREPROTECTED ? true : false;
		bIGNORERATEINC		= pGrindingOpt->dwATTRIBUTE&EMGRINDING_ATTRIBUTE_IGNORERATEINC ? true : false;
		bIGNOREDEFAULTREQ	= pGrindingOpt->dwATTRIBUTE&EMGRINDING_ATTRIBUTE_IGNOREDEFAULTREQ ? true : false;
		bADDED				= pGrindingOpt->dwATTRIBUTE&EMGRINDING_ATTRIBUTE_ADDED ? true : false;
	}

	// �����ɼ����� ���� �߰��� ������ġ ���
	switch( emGRINDING )
	{
	case EMGRINDING_DAMAGE:
		{
			cADDGRADE = static_cast<BYTE>(sCusItem.GETOptVALUE( RandomOption::RANDOM_OPTION_GRIND_DAMAGE ));
		}
		break;

	case EMGRINDING_DEFENSE:
		{
			cADDGRADE = static_cast<BYTE>(sCusItem.GETOptVALUE( RandomOption::RANDOM_OPTION_GRIND_DEFENCE ));
		}
		break;
	};

	if ( !pGrindingOpt || pGrindingOpt->ISENCHANTMAXNULL() )
	{
		if ( emGRINDING == EMGRINDING_DAMAGE || emGRINDING == EMGRINDING_DEFENSE )
		{
			wENCHANT_MAX = min(wENCHANT_MAX, /*GLCONST_CHAR::wGRADE_MAX*/sInfoByItemLevel.wMaxGrade);
		}
		else
		{
		
			wENCHANT_MAX = min(wENCHANT_MAX, /*GLCONST_CHAR::wGRADE_MAX_REGI*/sInfoByItemLevel.wMaxResist);
		}
	}

	if ( GRADE_LIMIT_MAX <= wENCHANT_MAX )
	{
		sc::writeLogError("GLCHARLOGIC::DOGRINDING() - GRADE_LIMIT_MAX <= wENCHANT_MAX");
		return dwGrindingResult;
	}

	if ( (WORD)cGRADE >= wENCHANT_MAX )
	{
		return dwGrindingResult;
	}
	else if ( (WORD)cGRADE < wENCHANT_MIN )
	{
		return dwGrindingResult;
	}

	// ��ȣ ���ð� �ɷ��ִٸ� ��ȭ��,�ҹ��� �� ����
	if ( bIGNOREPROTECTED )
	{
		dwGrindingOption &= ~EMGRINDINGOPTION_ANTIRESET;
		dwGrindingOption &= ~EMGRINDINGOPTION_ANTIDISAPPEAR;
	}

	// ��������, Ȥ�� Ȯ�� ���� ���ð� �ɷ��ִٸ� ��ȭ��,�̺�Ʈ�� ����
	if ( bIGNORERATEINC || bRANDOM )
	{
		dwGrindingOption &= ~EMGRINDINGOPTION_INC_GRINDINGRATE;
	}
	else
	{  
		using namespace boost::lambda;
		std::for_each(fGRADERATE, fGRADERATE+nGRADE_LIMIT_MAX, boost::lambda::_1 += GLCONST_CHAR::fGRADE_EVENT_RATE);

		UINT iTempGrade = cGRADE + 1;

		if( iTempGrade < nGRADE_LIMIT_MAX && iTempGrade >= GRADE_NORMAL && fGRADERATE[iTempGrade-1] >= 100 )
		{
			dwGrindingOption &= ~EMGRINDINGOPTION_INC_GRINDINGRATE;
		}

		// Note : ��ȭ�ָ� ������ �ִٸ� ����
		if ( dwGrindingOption&EMGRINDINGOPTION_INC_GRINDINGRATE )
		{
			// Note : ���ܻ������� ��ȭ�ְ� �ִٸ� ó������ �ʴ´�
			if ( !(dwGrindingOption&EMGRINDINGOPTION_ANTIRESET) )
			{
				/*std::transform(
					fGRADERATE,
					//fGRADERATE+nGRADE_LIMIT_MAX, // ���� �ʰ��� ������ �߻�, �ӽ÷� ����
					fGRADERATE+GRADE_LIMIT_MAX,
					GLCONST_CHAR::fGRADE_ITEMOPTION_INC_GRINDINGRATE,
					fGRADERATE,
					boost::lambda::_1 += boost::lambda::_2);*/
				std::transform(
					fGRADERATE,
					//fGRADERATE+nGRADE_LIMIT_MAX, // ���� �ʰ��� ������ �߻�, �ӽ÷� ����
					fGRADERATE+GRADE_LIMIT_MAX,
					sInfoByItemLevel.fArrItemOptionIncreaseRateByGrade,
					fGRADERATE,
					boost::lambda::_1 += boost::lambda::_2);
				fGRADERATE[nGRADE_LIMIT_MAX] = fGRADERATE[nGRADE_LIMIT_MAX-1];

				dwGrindingResult |= EMGRINDINGRESULT_INC_GRINDINGRATE;
			}
		}
	}

	// ���� ���� ������ ���
	if ( bRANDOM )
	{
		WORD  wMIN = 0;
		WORD  wMAX = wENCHANT_MAX;
		
		if ( pGrindingOpt )
		{
			if ( !pGrindingOpt->ISRANGEMINNULL() )
			{
				wMIN = pGrindingOpt->wRANGE_MIN;
			}

			if ( !pGrindingOpt->ISRANGEMAXNULL() )
			{
				wMAX = pGrindingOpt->wRANGE_MAX;
			}
		}

		// �⺻ �䱸���� ���ð� �ƴ϶�� �� ��޺��� �ּ� �ִ�ġ�� �����
		// ������ bADDED �ɼ��� �ִٸ� ����
		if ( !bADDED && !bIGNOREDEFAULTREQ )
		{
			switch( emGRINDERLEVEL )
			{
			case EMGRINDER_NORMAL:
				{
					wMIN = max(wMIN, 0);
					wMAX = min(wMAX, GRADE_NORMAL);
				}
				break;

			case EMGRINDER_HIGH:
				{
					wMIN = max(wMIN, GRADE_NORMAL);
					wMAX = min(wMAX, GRADE_HIGH);
				}
				break;

			case EMGRINDER_TOP:
				{
					wMIN = max(wMIN, GRADE_HIGH);
					wMAX = min(wMAX, wENCHANT_MAX);
				}
				break;
			};
		}

		// ��� �������� �ʴ� �ɼ��� �ִٸ� ����
		if ( bNODEGRATION )
		{
			wMIN = max(wMIN, (WORD)cGRADE);
		}

		WORD wValue = 0;

		if ( wMIN >= wMAX )
		{
			wValue = wMAX;
		}
		else
		{
			//! ��� ���� Ȯ���� 100%�� �����Ѵ�
			{
				float fRATIO = 0.0f;
				for ( WORD i=wMIN; i<=wMAX; ++i )
				{
					fRATIO += fGRADERATE[i];
				}

				if ( fRATIO <= 0.0f )
				{
					fRATIO = 100.0f;
				}

				fRATIO /= 100.0f;

				using namespace boost::lambda;
				std::for_each(fGRADERATE+wMIN, fGRADERATE+wMAX+1, boost::lambda::_1 /= fRATIO);
			}

			float fRATE  = sc::Random::getInstance().GetPercent();
			float fSUM	 = 0.0f;

			for ( WORD i=wMIN; i<=wMAX; ++i )
			{
				wValue = i;

				fSUM += fGRADERATE[i];

				if ( fRATE < fSUM )
				{
					break;
				}
			}
		}

		// ���ϴ� �ɼ����ִٸ� ���Ѵ�
		if ( bADDED )
		{
			cRESULTGRADE = cGRADE + BYTE(wValue);

			// �⺻ �䱸���� ���ÿɼ��� ���ٸ� ������ ����� �ִ�ġ�� ������ ����
			if ( !bIGNOREDEFAULTREQ )
			{
				switch( pItem->sGrindingOp.emGRINDER_TYPE )
				{
				case EMGRINDER_NORMAL:
				case EMGRINDER_HIGH:
					{
						cRESULTGRADE = min(cRESULTGRADE, GRADE_HIGH);
					}
					break;

				case EMGRINDER_TOP:
					{
						cRESULTGRADE = min(cRESULTGRADE, (BYTE)wENCHANT_MAX);
					}
					break;
				};
			}
		}
		else
		{
			cRESULTGRADE = BYTE(wValue);
		}

		cRESULTGRADE = min( cRESULTGRADE, (BYTE)wENCHANT_MAX );

		// ��ȣ���ð� �ɷ����� ���� ��� ��ȭ�ְ� �������� ������ �������� ���� �����ش�
		if ( !bIGNOREPROTECTED )
		{
			if ( cRESULTGRADE < cGRADE && (dwGrindingOption&EMGRINDINGOPTION_ANTIRESET) )
			{
				// Note : ��ȭ�ָ� ��� ����.
				dwGrindingResult |= EMGRINDINGRESULT_ANTIRESET;
				cRESULTGRADE = cGRADE;
			}
		}

	}
	// �Ϲ� ������ ���
	else
	{
		cRESULTGRADE = cGRADE+1;

		//	4�ܰ� �̻���� ���� Ȯ�� ����.
		if ( cGRADE >= GRADE_NORMAL )
		{
			//	���� Ȯ��.
			float fRATE = fGRADERATE[cGRADE];

			if ( fRATE > 100.0f ) fRATE = 100.0f;

			BOOL bOK = sc::Random::getInstance().IsSucceed(fRATE);
			if ( !bOK )
			{
				// ��ȭ ���н� ��� ���Ҹ� ���� �ʴ´ٸ� ����
				if ( bNODEGRATION )
				{
					return dwGrindingResult;
				}

				//	���� Ȯ��.
				//float fRESET = GLCONST_CHAR::fGRADE_RESET_RATE[cGRADE];
				float fRESET = sInfoByItemLevel.fArrResetRateByGrade[cGRADE];
				bOK = sc::Random::getInstance().IsSucceed(fRESET);
				if ( bOK )
				{
					dwGrindingResult |= EMGRINDINGRESULT_RESET;
					
					// Note : ��ȭ�ָ� ������ �ִٸ� ����
					if ( dwGrindingOption&EMGRINDINGOPTION_ANTIRESET )
					{
						//sCusItem.DecreaseGrind( emGRINDING, GLCONST_CHAR::nGRADE_ITEMOPTION_ANTIRESET[cGRADE] );
						sCusItem.DecreaseGrind( emGRINDING, sInfoByItemLevel.fArrItemOptionAntiResetRateByGrade[cGRADE] );

						// Note : ��ȭ�ָ� ��� ����.
						dwGrindingResult |= EMGRINDINGRESULT_ANTIRESET;
						dwGrindingResult &= ~EMGRINDINGRESULT_RESET;
					}
					else
					{
						sCusItem.ResetGrind(emGRINDING);
					}
				
					//	�ļ� Ȯ��.
					bool bTER = sInfoByItemLevel.bTerminateItem /*CONST_CHAR::bENCHANT_TERMINATE_ITEM*/ && ( emGRINDING==EMGRINDING_DAMAGE || emGRINDING==EMGRINDING_DEFENSE );
					if ( bTER )
					{
						//float fTERMINATE = GLCONST_CHAR::fGRADE_TERMINATE_RATE[cGRADE];
						float fTERMINATE = sInfoByItemLevel.fArrTerminateRateByGrade[cGRADE];
						bOK = sc::Random::getInstance().IsSucceed(fTERMINATE);
						if ( bOK )
						{
							//Note : �ҹ��ָ� ������ �ִٸ�
							if( dwGrindingOption&EMGRINDINGOPTION_ANTIDISAPPEAR )
							{
								// Note : �ҹ��ָ� ����ߴ�.
								dwGrindingResult |= EMGRINDINGRESULT_ANTIDISAPPEAR; 
								dwGrindingResult &= ~EMGRINDINGRESULT_TERMINATE;

								// Note : ��ȭ�ָ� ������ ���� �ʴٸ� 0���� ����
								if ( !(dwGrindingOption & EMGRINDINGOPTION_ANTIRESET) )
								{
									sCusItem.ResetGrind(emGRINDING);
								}
							}
							else
							{
								// Note : �ı��� ��ȭ�� ��� ����
								dwGrindingResult &= ~EMGRINDINGRESULT_ANTIRESET;

								dwGrindingResult |= EMGRINDINGRESULT_TERMINATE;
							}
						}
					}
				}

				return dwGrindingResult;
			}
		} // if ( cGRADE >= GRADE_NORMAL ) end
		else
		{
			// 4�ܰ� �����ϰ�� ��ȭ�� ����� ���� �ʴ´�.
			dwGrindingResult &= ~EMGRINDINGRESULT_ANTIRESET;
		}

	} // if ( bRANDOM ) else end


	//! ���������� �߰� ������ġ��ŭ ���ҽ��Ѽ� ����
	if ( cADDGRADE <= cRESULTGRADE )
	{
		cRESULTGRADE -= cADDGRADE;
	}
	else
	{
		cRESULTGRADE = 0;
	}

	//	Note : ������ ����.
	//
	switch ( emGRINDING )
	{
	case EMGRINDING_DAMAGE:			sCusItem.cDAMAGE		= cRESULTGRADE; break;
	case EMGRINDING_DEFENSE:		sCusItem.cDEFENSE		= cRESULTGRADE; break;
	case EMGRINDING_RESIST_FIRE:	sCusItem.cRESIST_FIRE	= cRESULTGRADE;	break;
	case EMGRINDING_RESIST_ICE:		sCusItem.cRESIST_ICE	= cRESULTGRADE;	break;
	case EMGRINDING_RESIST_ELEC:	sCusItem.cRESIST_ELEC	= cRESULTGRADE;	break;
	case EMGRINDING_RESIST_POISON:	sCusItem.cRESIST_POISON	= cRESULTGRADE;	break;
	case EMGRINDING_RESIST_SPIRIT:	sCusItem.cRESIST_SPIRIT	= cRESULTGRADE;	break;
	};

	dwGrindingResult |= EMGRINDINGRESULT_SUCCESS;

	return dwGrindingResult;
}

//	Note : �ɸ��Ͱ� �������� �����ϰ� �ִ��� �˻�.
//		���뺹��, �κ��丮���� �˻�.
//		��ü �������� ������ �������°ɷ� ����( Full�����۸� üũ�ϴµ��� ���� ) 
//
BOOL GLCHARLOGIC::ISHAVEITEM ( SNATIVEID sNID, DWORD dwNum )
{

	DWORD nCount = GETHAVEITEM( sNID );
	return ( nCount >= dwNum  );
}

DWORD GLCHARLOGIC::GETHAVEITEM( SNATIVEID sNID )
{
	DWORD nCount = 0;
	for ( int i=0; i<SLOT_TSIZE; ++i )
	{
		const SITEM* pITEM = GET_SLOT_ITEMDATA ( EMSLOT(i) );
		if ( !pITEM )								continue;

		const SITEMCUSTOM &sCUSTOM = GET_SLOT_ITEM ( EMSLOT(i) );
		if ( sCUSTOM.GetNativeID()==SNATIVEID(false) )	continue;

		if ( pITEM->sBasicOp.sNativeID==sNID )
		{
			nCount += sCUSTOM.wTurnNum;
		}
	}

	nCount += m_cInventory.CountTurnItem ( sNID );

	return nCount;
}

SITEM* GLCHARLOGIC::FindRevive( EMSLOT* pemSlot )
{
	for( DWORD i = 0; i < SLOT_NSIZE_S_2; ++i )
	{
		if( NULL == m_pITEMS[ i ] )
			continue;

		if( ITEM_DRUG_CALL_REVIVE == m_pITEMS[ i ]->sDrugOp.emDrug )
		{
			if( NULL != pemSlot )
				*pemSlot = EMSLOT( i );

			return m_pITEMS[ i ];
		}
	}

	return NULL;
}

WORD GLCHARLOGIC::GETSKILLRANGE_TAR ( const GLSKILL &sSKILL ) const
{
	int nRANGE = sSKILL.m_sBASIC.wTARRANGE;

	//	Ÿ���� ���϶��� ����
	if ( sSKILL.m_sBASIC.emIMPACT_SIDE==EMIMPACT_SIDE_ENEMY )
		nRANGE += (int) GETSUM_TARRANGE_ALL();

	if ( nRANGE < 0 )
		nRANGE = 0;

	return (WORD)nRANGE;
}

WORD GLCHARLOGIC::GETSKILLRANGE_APPLY ( const GLSKILL &sSKILL, const WORD dwLEVEL ) const
{
	const SKILL::CDATA_LVL &sDATA_LVL = sSKILL.m_sAPPLY.sDATA_LVL[dwLEVEL];

	int nRANGE = sDATA_LVL.wAPPLYRANGE;
	//	Ÿ���� ���϶��� ����
	if ( sSKILL.m_sBASIC.emIMPACT_SIDE==EMIMPACT_SIDE_ENEMY )
		nRANGE += (int) GETSUM_APPLYRANGE_ALL();

	if ( nRANGE < 0 ) 
		nRANGE = 0;

	return (WORD) nRANGE;
}

//	Note : ���� pk ������ �˾ƺ���. ( UINT_MAX �ϰ�� pk �ش���� ����. )
DWORD GLCHARLOGIC::GET_PK_LEVEL ()
{
	if ( m_nBright >= 0 )	return UINT_MAX;

	DWORD dwLEVEL = 0;
	for ( dwLEVEL=0; dwLEVEL<GLCONST_CHAR::EMPK_STATE_LEVEL; ++dwLEVEL )
	{
		if ( GLCONST_CHAR::sPK_STATE[dwLEVEL].nPKPOINT <= m_nBright )		break;
	}

	if ( dwLEVEL>=GLCONST_CHAR::EMPK_STATE_LEVEL )	dwLEVEL = GLCONST_CHAR::EMPK_STATE_LEVEL-1;

	return dwLEVEL;
}


float GLCHARLOGIC::GET_PK_DECEXP_RATE ()
{
	DWORD dwLEVEL = GET_PK_LEVEL();
	if ( dwLEVEL==UINT_MAX )	return 100.0f;

	return GLCONST_CHAR::sPK_STATE[dwLEVEL].fPK_EXP_RATE;
}

std::string GLCHARLOGIC::GET_PK_NAME ()
{
	DWORD dwLEVEL = GET_PK_LEVEL();
	if ( dwLEVEL==UINT_MAX )	return GLCONST_CHAR::sPK_STATE[0].strNAME;

	return GLCONST_CHAR::sPK_STATE[dwLEVEL].strNAME;
}

DWORD GLCHARLOGIC::GET_PK_COLOR ()
{
	DWORD dwLEVEL = GET_PK_LEVEL();
	if ( dwLEVEL==UINT_MAX )	return GLCONST_CHAR::dwPK_NORMAL_NAME_COLOR;

	return GLCONST_CHAR::sPK_STATE[dwLEVEL].dwNAME_COLOR;
}

float GLCHARLOGIC::GET_PK_SHOP2BUY ()
{
	DWORD dwLEVEL = GET_PK_LEVEL();
	if ( dwLEVEL==UINT_MAX )	return 100.0f;

	return GLCONST_CHAR::sPK_STATE[dwLEVEL].fSHOP_2BUY_RATE;
}

float GLCHARLOGIC::GET_PK_SHOP2SALE ()
{
	DWORD dwLEVEL = GET_PK_LEVEL();
	if ( dwLEVEL==UINT_MAX )	return 100.0f;

	return GLCONST_CHAR::sPK_STATE[dwLEVEL].fSHOP_2SALE_RATE;
}

DWORD GLCHARLOGIC::GET_PK_ITEMDROP_NUM ()
{
	DWORD dwLEVEL = GET_PK_LEVEL();
	if ( dwLEVEL==UINT_MAX )
	{
		if ( GLCONST_CHAR::fPK_ITEM_DROP==0 )	return 0;
		return 1;
	}

	return GLCONST_CHAR::sPK_STATE[dwLEVEL].dwITEM_DROP_NUM;
}

bool GLCHARLOGIC::ISPLAYKILLING ()
{
	return !m_mapPlayHostile.empty();
}

//	Note : �ڽ��� �������ΰ�?
bool GLCHARLOGIC::ISOFFENDER ()
{
	return ( m_nBright < GLCONST_CHAR::sPK_STATE[0].nPKPOINT );
}

bool GLCHARLOGIC::IS_HOSTILE_ACTOR ( DWORD dwCHARID )
{
	MAPPLAYHOSTILE_ITER pos = m_mapPlayHostile.find ( dwCHARID );
	if ( pos!=m_mapPlayHostile.end() )
	{
		if( pos->second )
			return ( pos->second->bBAD == TRUE );
		else
            sc::writeLogError(std::string("GLCHARLOGIC::IS_HOSTILE_ACTOR, pos->second = NULL"));
	}

	return false;
}

//	Note : ������ ���. ( pk ���� ),	true : �ű� ��Ͻ�, false : ���� ��� ���Ž�.
bool GLCHARLOGIC::ADD_PLAYHOSTILE( const DWORD dwCHARID, const BOOL bBAD, const float fTime )
{
	if ( m_CharDbNum == dwCHARID )
	{
		//sc::writeLogError(std::string("GLCHARLOGIC::ADD_PLAYHOSTILE, m_dwCharID == dwCHARID"));
		return false;
	}

	bool					bResult = true;
	MAPPLAYHOSTILE_ITER		pos		= m_mapPlayHostile.find( dwCHARID );
	// �̹� ��ϵǾ� ������...
	if( pos != m_mapPlayHostile.end() )
	{
		bResult = false;
		if( pos->second )		//	�ð��� ����. ���� bBAD�� �������� ����. ( ó�� �������� �� ����. )
		{
			pos->second->fTIME = fTime;
			return false;
		}
		else
		{
			// �̹� ��ϵǾ� �־����� ���� ��ȿ���� �ʾƼ� �����Ѵ�.
			sc::writeLogError(std::string("GLCHARLOGIC::ADD_PLAYHOSTILE, pos->second = NULL"));
			m_mapPlayHostile.erase( pos );
		} //fi..else
	} //if

	// ��ϵǾ� ���� �ʰų� ��ȿ���� ���� ���̾��� ������
	// �űԷ� ����Ѵ�.
	SPLAYHOSTILE*	pHOSTILE = new SPLAYHOSTILE;
	pHOSTILE->bBAD		= bBAD;
	pHOSTILE->fTIME		= fTime;

	m_mapPlayHostile.insert( std::make_pair( dwCHARID, pHOSTILE ) );

	return bResult;
}

bool GLCHARLOGIC::DEL_PLAYHOSTILE ( const DWORD dwCHARID )
{
	MAPPLAYHOSTILE_ITER pos = m_mapPlayHostile.find ( dwCHARID );
	if ( pos!=m_mapPlayHostile.end() )
	{
		SAFE_DELETE( pos->second );
		m_mapPlayHostile.erase ( pos );
		return true;
	}
	else
	{	
		return false;
	}
}

void GLCHARLOGIC::DEL_PLAYHOSTILE_ALL ()
{
	for( MAPPLAYHOSTILE_ITER ci = m_mapPlayHostile.begin(); ci != m_mapPlayHostile.end(); ++ci )
		SAFE_DELETE( ci->second );
	m_mapPlayHostile.clear();
}

bool GLCHARLOGIC::IS_PLAYHOSTILE ( DWORD dwCHARID )
{
	MAPPLAYHOSTILE_ITER pos = m_mapPlayHostile.find ( dwCHARID );
	return ( pos != m_mapPlayHostile.end() );
}

// Note : �����ִ� SKILLFACT ������ �ִ°�
bool GLCHARLOGIC::IS_REMAIND_SKILLFACTARRAY ( EMSKILLFACT_TYPE emType )
{
	switch ( emType )
	{
	case EMSKILLFACT_TYPE_NORMAL:
		for ( int i=SKILLFACT_INDEX_NORMAL_START; i<SKILLFACT_INDEX_NORMAL_END; ++i )
		{
			if (GetSkillFactId(i) == NATIVEID_NULL())
				return true;
		}
		return false;

	case EMSKILLFACT_TYPE_ADDITIONAL:
		for( int i = SKILLFACT_INDEX_ADDITIONAL_START; i < SKILLFACT_INDEX_ADDITIONAL_END; ++i )
		{
			if( GetSkillFactId(i) == NATIVEID_NULL() )
				return true;
		}
		return false;

	case EMSKILLFACT_TYPE_LUNCHBOX:
		for (size_t i=SKILLFACT_INDEX_LUNCHBOX_START; i<SKILLFACT_INDEX_LUNCHBOX_END; ++i)
		{
			if (GetSkillFactId(i) == NATIVEID_NULL())
				return true;
		}
		return false;

	case EMSKILLFACT_TYPE_EXP_LUNCHBOX:
		for (size_t i=SKILLFACT_INDEX_EXP_LUNCHBOX_START; i<SKILLFACT_INDEX_EXP_LUNCHBOX_END; ++i)
		{
			if (GetSkillFactId(i) == NATIVEID_NULL())
				return true;
		}
		return false;

    case EMSKILLFACT_TYPE_SYSTEM:
        for (size_t i=SKILLFACT_INDEX_SYSTEM_START; i<SKILLFACT_INDEX_SYSTEM_END; ++i)
        {
            if ( GetSkillFactId(i) == NATIVEID_NULL())
                return true;
        }
        return false;
	} //switch

	return false;
}

// Note : �ش� ��ų�� �ߺ��Ǵ°�
bool GLCHARLOGIC::IS_DUPLICATE_SKILLFACT (const GLSKILL* pSkill, EMSKILLFACT_TYPE emType, WORD &wOutSlot )
{
	if ( !pSkill )
		return false;

	switch ( emType )
	{
	case EMSKILLFACT_TYPE_NORMAL:
		for (size_t i=SKILLFACT_INDEX_NORMAL_START; i<SKILLFACT_INDEX_NORMAL_END; ++i)
		{
			if (GetSkillFactId(i) == pSkill->m_sBASIC.sNATIVEID)
			{
				wOutSlot = (WORD)i;
				return true;
			}
		}
		return false;

	case EMSKILLFACT_TYPE_ADDITIONAL:
		for (size_t i = SKILLFACT_INDEX_ADDITIONAL_START; i < SKILLFACT_INDEX_ADDITIONAL_END; ++i )
		{
			if( GetSkillFactId(i) == pSkill->m_sBASIC.sNATIVEID )
			{
				wOutSlot = (WORD)i;
				return true;
			}
		}
		return false;

	case EMSKILLFACT_TYPE_LUNCHBOX:
        if ( !IS_REMAIND_SKILLFACTARRAY(emType) )
        {
            wOutSlot = (WORD)SKILLFACT_INDEX_LUNCHBOX_START;
            return true;
        }

        for (size_t i=SKILLFACT_INDEX_LUNCHBOX_START; i<SKILLFACT_INDEX_LUNCHBOX_END; ++i)
        {
            if (GetSkillFactId(i) == pSkill->m_sBASIC.sNATIVEID)
            {
				sc::writeLogInfo( sc::string::format(
					"[ Skill ] [ SkillFact(%1%) - Age : %2%, Life : %3%, bIsExpiredTime : %4%, nEXPIREDTIME : %5% ]",
					i,
					m_sSKILLFACT[i].fAGE,
					m_sSKILLFACT[i].fLIFE,
					m_sSKILLFACT[i].bIsExpiredTime ? "TRUE" : "FALSE",
					m_sSKILLFACT[i].nEXPIREDTIME ) );

                wOutSlot = (WORD)i;
                return true;
            }
		}
		return false;
	case EMSKILLFACT_TYPE_EXP_LUNCHBOX:		
		if ( IS_REMAIND_SKILLFACTARRAY(emType) == true )
		{
			wOutSlot = (WORD)SKILLFACT_INDEX_EXP_LUNCHBOX_START;
			return false;
		}

		for (size_t i=SKILLFACT_INDEX_EXP_LUNCHBOX_START; i<SKILLFACT_INDEX_EXP_LUNCHBOX_END; ++i)
		{
			if (GetSkillFactId(i) == pSkill->m_sBASIC.sNATIVEID)
			{				
				wOutSlot = (WORD)i;
				const DWORD _sizeMaxItem(m_ExperienceRate.getMaxSize());
				if ( _sizeMaxItem == 1 )
					return true;
				// ��ø Ƚ���� 1�� ��� �ߺ����� ó���ϰ� ���ο� ȿ���� ���� �����;

				sc::writeLogInfo( sc::string::format(
					"[ Skill ] [ SkillFact(%1%) - Age : %2%, Life : %3%, bIsExpiredTime : %4%, nEXPIREDTIME : %5% ]",
					i,
					m_sSKILLFACT[i].fAGE,
					m_sSKILLFACT[i].fLIFE,
					m_sSKILLFACT[i].bIsExpiredTime ? "TRUE" : "FALSE",
					m_sSKILLFACT[i].nEXPIREDTIME ) );

				return false;
			}
		}

		wOutSlot = SKILLFACT_INDEX_EXP_LUNCHBOX_START;
		return true;


    case EMSKILLFACT_TYPE_SYSTEM:
        for (size_t i=SKILLFACT_INDEX_SYSTEM_START; i<SKILLFACT_INDEX_SYSTEM_END; ++i)
        {
            if (GetSkillFactId(i) == pSkill->m_sBASIC.sNATIVEID)
            {
                wOutSlot = (WORD)i;
                return true;
            }
        }
        return false;
	}

	return false;
}

// Note : �ش� ������ �������ִ°�
bool GLCHARLOGIC::IS_HAVE_SKILLFACT( const SNATIVEID skill_id, const WORD wLevel, OUT WORD &wOutSlot, const bool bOnlyEnable )
{
    for (size_t i=0; i<SKILLFACT_SIZE; ++i)
    {
        const SSKILLFACT& sSkillFact = GetSkillFact(i);

        if ( sSkillFact.sNATIVEID == NATIVEID_NULL() )
        {
            continue;
        }

        if ( bOnlyEnable && !sSkillFact.bEnable )
        {
            continue;
        }

        if (sSkillFact.sNATIVEID == skill_id  && wLevel <= sSkillFact.wLEVEL+1 )
        {
            wOutSlot = (WORD)i;
            return true;
        }
    }

    return false;
}

bool GLCHARLOGIC::IS_HAVE_SKILLFACT ( const SNATIVEID skill_id, OUT WORD &wOutSlot )
{
    if ( skill_id == SNATIVEID(false) )
    {
        return false;
    }

	for (size_t i=0; i<SKILLFACT_SIZE; ++i)
	{
		if (GetSkillFactId(i) == skill_id)
		{
			wOutSlot = (WORD)i;
			return true;
		}
	} 
	return false;
}

const bool GLCHARLOGIC::IS_HAVE_TYPES_IN_SKILLFACT(const EMIMPACT_ADDON emIMPACT_ADDON){
	for (size_t i = SKILLFACT_SIZE; i; --i)
	{
		SSKILLFACT& sSKEFF = m_sSKILLFACT[SKILLFACT_SIZE - i];
		if (sSKEFF.sNATIVEID == NATIVEID_NULL())
			continue;
		if (!sSKEFF.bEnable)
			continue;		

		const size_t ADDONSize = sSKEFF.vecADDON.size();
		for (size_t j = ADDONSize; j ; --j)
		{			
			if (sSKEFF.vecADDON[ADDONSize - j].emADDON == emIMPACT_ADDON )
				return true;
		}
	}
	return false;
}


const bool GLCHARLOGIC::IS_HAVE_SPEC_IN_SKILLFACT(const EMSPEC_ADDON emSPEC_ADDON){	
	for (size_t i = SKILLFACT_SIZE; i; --i)
	{
		SSKILLFACT& sSKEFF = m_sSKILLFACT[SKILLFACT_SIZE - i];
		if (sSKEFF.sNATIVEID == NATIVEID_NULL())
			continue;
		if (!sSKEFF.bEnable)
			continue;		

		const size_t SpecSize = sSKEFF.vecSPEC.size();
		for (size_t j = SpecSize; j ; --j)
		{
			if (sSKEFF.vecSPEC[SpecSize - j].emSPEC == emSPEC_ADDON)
				return true;
		}
	}
	return false;
}

const bool GLCHARLOGIC::IS_HAVE_STYPES_IN_SKILLFACT(const SKILL::EMSPECIALSKILLTYPE emSSTYPE_ADDON){
	for (size_t i = SKILLFACT_SIZE; i; --i)
	{
		SSKILLFACT& sSKEFF = m_sSKILLFACT[SKILLFACT_SIZE - i];
		if (sSKEFF.sNATIVEID == NATIVEID_NULL())
			continue;
		if (!sSKEFF.bEnable)
			continue;

		if ( sSKEFF.dwSpecialSkill == emSSTYPE_ADDON)
			return true;
	}
	return false;
}

const bool GLCHARLOGIC::IS_HAVE_SKILL_IN_SKILLFACT( const SNATIVEID& sNativeID )
{
	for (size_t i = SKILLFACT_SIZE; i; --i)
	{
		SSKILLFACT& sSKEFF = m_sSKILLFACT[SKILLFACT_SIZE - i];
		if (sSKEFF.sNATIVEID == NATIVEID_NULL())
			continue;
		if (!sSKEFF.bEnable)
			continue;		

		if( sSKEFF.sNATIVEID == sNativeID )
			return true;

	}
	return false;
}

const EMSLOT GLCHARLOGIC::GetCurRHand(void) const
{
	if ( IsUseArmSub() )
		return SLOT_RHAND_S;
	else
		return SLOT_RHAND;
}

const EMSLOT GLCHARLOGIC::GetCurLHand(void) const
{
	if ( IsUseArmSub() )
		return SLOT_LHAND_S;
	else
		return SLOT_LHAND;
}

BOOL GLCHARLOGIC::VALID_SLOT_ITEM ( EMSLOT _slot )							
{ 
    if( _slot < 0 || SLOT_TSIZE <= _slot )
        return FALSE;

	if( m_PutOnItems[_slot].GetNativeID()==NATIVEID_NULL() ) 
        return FALSE;

	if( IsUseArmSub() )
	{
		if( _slot == SLOT_RHAND || _slot == SLOT_LHAND ) 
            return FALSE;
	}
	else
	{
		if( _slot == SLOT_RHAND_S || _slot == SLOT_LHAND_S )
            return FALSE;
	}

	return TRUE;
}

BOOL GLCHARLOGIC::VALID_SLOT_ITEM_ALLWEAPON ( EMSLOT _emSlot )
{
	if ( _emSlot < 0 || SLOT_TSIZE <= _emSlot )
		return FALSE;

	if( m_PutOnItems[ _emSlot ].GetNativeID() == NATIVEID_NULL() ) 
		return FALSE;

	return TRUE;
}

const GLITEM_ATT GLCHARLOGIC::CONVERT_ITEMATT ( GLITEM_ATT emAttack )
{
	const EMCHARINDEX emClass = GETCHARINDEX();
	switch ( emAttack )
	{
	case ITEMATT_WEAPON_STR:
		{
			switch ( emClass )
			{
			case GLCI_FIGHTER_M:
			case GLCI_FIGHTER_W:
				{
					emAttack = ITEMATT_GWON;
				}
				break;

			case GLCI_ARMS_M:
			case GLCI_ARMS_W:
				{
					emAttack = ITEMATT_SABER;
				}
				break;

			case GLCI_ARCHER_W:
			case GLCI_ARCHER_M:
				{
					emAttack = ITEMATT_BOW;
				}
				break;

			case GLCI_SPIRIT_W:
			case GLCI_SPIRIT_M:
				{
					emAttack = ITEMATT_SPEAR;
				}
				break;

			case GLCI_EXTREME_M:
			case GLCI_EXTREME_W:
				{
					emAttack = ITEMATT_EXTREME_GLOVE;
				}
				break;

			case GLCI_SCIENTIST_M:
			case GLCI_SCIENTIST_W:
				{
					emAttack = ITEMATT_RAILGUN;
				}
				break;

			case GLCI_ASSASSIN_M:
			case GLCI_ASSASSIN_W:
				{
					emAttack = ITEMATT_SIDE;
				}
				break;

			case GLCI_TRICKER_M:
			case GLCI_TRICKER_W:
				{
					emAttack = ITEMATT_TRICK_WHIP;
				}
				break;

			case GLCI_ETC_M:
			case GLCI_ETC_W:
				{
					emAttack = ITEMATT_NOTHING;
				}
				break;

			case GLCI_ACTOR_M:
			case GLCI_ACTOR_W:
				{
					emAttack = ITEMATT_ACTOR_HAMMER;
				}
				break;
			}
		}
		break;

	case ITEMATT_WEAPON_DEX:
		{
			switch ( emClass )
			{
			case GLCI_FIGHTER_M:
			case GLCI_FIGHTER_W:
				{
					emAttack = ITEMATT_GWON;
				}
				break;

			case GLCI_ARMS_M:
			case GLCI_ARMS_W:
				{
					emAttack = ITEMATT_SWORD;
				}
				break;

			case GLCI_ARCHER_W:
			case GLCI_ARCHER_M:
				{
					emAttack = ITEMATT_BOW;
				}
				break;

			case GLCI_SPIRIT_W:
			case GLCI_SPIRIT_M:
				{
					emAttack = ITEMATT_SPEAR;
				}
				break;

			case GLCI_EXTREME_M:
			case GLCI_EXTREME_W:
				{
					emAttack = ITEMATT_EXTREME_GLOVE;
				}
				break;

			case GLCI_SCIENTIST_M:
			case GLCI_SCIENTIST_W:
				{
					emAttack = ITEMATT_PISTOL;
				}
				break;

			case GLCI_ASSASSIN_M:
			case GLCI_ASSASSIN_W:
				{
					emAttack = ITEMATT_DUALSPEAR;
				}
				break;

			case GLCI_TRICKER_M:
			case GLCI_TRICKER_W:
				{
					emAttack = ITEMATT_TRICK_BOX;
				}
				break;

			case GLCI_ETC_M:
			case GLCI_ETC_W:
				{
					emAttack = ITEMATT_NOTHING;
				}
				break;

			case GLCI_ACTOR_M:
			case GLCI_ACTOR_W:
				{
					emAttack = ITEMATT_ACTOR_SHIELD;
				}
				break;
			}
		}
		break;

	case ITEMATT_WEAPON_SPI:
		{
			switch ( emClass )
			{
			case GLCI_FIGHTER_M:
			case GLCI_FIGHTER_W:
				{
					emAttack = ITEMATT_GWON;
				}
				break;

			case GLCI_ARMS_M:
			case GLCI_ARMS_W:
				{
					emAttack = ITEMATT_SWORD;
				}
				break;

			case GLCI_ARCHER_W:
			case GLCI_ARCHER_M:
				{
					emAttack = ITEMATT_BOW;
				}
				break;

			case GLCI_SPIRIT_W:
			case GLCI_SPIRIT_M:
				{
					emAttack = ITEMATT_SPEAR;
				}
				break;

			case GLCI_EXTREME_M:
			case GLCI_EXTREME_W:
				{
					emAttack = ITEMATT_EXTREME_GLOVE;
				}
				break;

			case GLCI_SCIENTIST_M:
			case GLCI_SCIENTIST_W:
				{
					emAttack = ITEMATT_PORTALGUN;
				}
				break;

			case GLCI_ASSASSIN_M:
			case GLCI_ASSASSIN_W:
				{
					emAttack = ITEMATT_THROWING_KNIFE;
				}
				break;

			case GLCI_TRICKER_M:
			case GLCI_TRICKER_W:
				{
					emAttack = ITEMATT_TRICK_STICK;
				}
				break;

			case GLCI_ETC_M:
			case GLCI_ETC_W:
				{
					emAttack = ITEMATT_NOTHING;
				}
				break;

			case GLCI_ACTOR_M:
			case GLCI_ACTOR_W:
				{
					emAttack = ITEMATT_ACTOR_UMBRELLA;
				}
				break;
			}
		}
		break;
	}

	return emAttack;
}

BOOL GLCHARLOGIC::ISLONGRANGE_ARMS ()							
{
	EMSLOT emRHand = GetCurRHand();

	if ( m_pITEMS[emRHand] )
	{
		return ISLONGRANGE_ARMS ( m_pITEMS[emRHand]->sSuitOp.emAttack );
	}
	else
	{
		return FALSE;
	}
}

BOOL GLCHARLOGIC::ISLONGRANGE_ARMS ( GLITEM_ATT emAttack )
{
	emAttack = CONVERT_ITEMATT( emAttack );

	switch( emAttack )
	{
	case ITEMATT_NOTHING:
	case ITEMATT_SWORD:
	case ITEMATT_SABER:
	case ITEMATT_DAGGER:
	case ITEMATT_SPEAR:
	case ITEMATT_STICK:
	case ITEMATT_GWON:
	case ITEMATT_SIDE:
	case ITEMATT_DUALSPEAR:
	case ITEMATT_THROWING_KNIFE:
	case ITEMATT_EXTREME_GLOVE:
	case ITEMATT_TRICK_STICK:
	case ITEMATT_TRICK_BOX:
	case ITEMATT_TRICK_WHIP:
	case ITEMATT_ACTOR_SHIELD:
	case ITEMATT_ACTOR_HAMMER:
	case ITEMATT_ACTOR_UMBRELLA:
		return FALSE;

	case ITEMATT_BOW:
	case ITEMATT_THROW:
	case ITEMATT_PISTOL:
	case ITEMATT_RAILGUN:
	case ITEMATT_PORTALGUN:
		return TRUE;
		break;

	default:
		return FALSE;
	}
}

SKILL::EMAPPLY GLCHARLOGIC::GetStatType()
{
	SKILL::EMAPPLY emApply = SKILL::EMAPPLY_MELEE;

	WORD wPower = GetPow();
	WORD wDex = GetDex();
	WORD wSpirit = GetSpi();

	emApply = ( wPower >= wDex ) ? SKILL::EMAPPLY_MELEE : SKILL::EMAPPLY_RANGE;
	if ( emApply == SKILL::EMAPPLY_MELEE )
		emApply = ( wPower >= wSpirit ) ? SKILL::EMAPPLY_MELEE : SKILL::EMAPPLY_MAGIC;
	else
		emApply = ( wDex >= wSpirit ) ? SKILL::EMAPPLY_RANGE : SKILL::EMAPPLY_MAGIC;

	return emApply;
}

BOOL GLCHARLOGIC::IsCurUseArm( EMSLOT emSlot )
{
	if( IsUseArmSub() )
	{
		if( emSlot == SLOT_RHAND || emSlot == SLOT_LHAND ) return FALSE;
	}
	else
	{
		if( emSlot == SLOT_RHAND_S || emSlot == SLOT_LHAND_S ) return FALSE;
	}

	return TRUE;
}


void GLCHARLOGIC::DISABLEALLLANDEFF()
{
	if (!m_vecLandEffect.empty())
        m_vecLandEffect.clear();
}

void GLCHARLOGIC::SETENABLEALLLUNCHBOX(const BOOL bEnable)
{
	m_ExperienceRate.setLock(bEnable == TRUE ? false : true);

	for (size_t i=SKILLFACT_INDEX_LUNCHBOX_START; i<SKILLFACT_INDEX_EXP_LUNCHBOX_END; ++i)
	{
		SetSkillFact(i, bEnable);
	}
}

void GLCHARLOGIC::ADDLANDEFF( SLANDEFFECT_VALUE& landEffect )
{
	if ( EMLANDEFFECT_MULTI > m_vecLandEffect.size() ) 
		m_vecLandEffect.push_back( landEffect );
}

void GLCHARLOGIC::DELLANDEFF( SLANDEFFECT_VALUE& landEffect )
{
	VEC_LANDEFF_VALUE_ITER pos = std::find( m_vecLandEffect.begin(), m_vecLandEffect.end(), landEffect );
	if ( pos != m_vecLandEffect.end() )
		m_vecLandEffect.erase( pos );
}

void GLCHARLOGIC::DISABLEALLMAPEFF()
{
	if ( !m_vecMapEffect.empty() ) m_vecMapEffect.clear();
}

void GLCHARLOGIC::ADDMAPEFF( SMAP_EFFECT& mapEffect )
{
	if ( EMLANDEFFECT_MULTI > m_vecMapEffect.size() ) 
		m_vecMapEffect.push_back( mapEffect );
}

void GLCHARLOGIC::DELMAPEFF( SMAP_EFFECT& mapEffect )
{
	VEC_MAP_EFFECT_ITER pos = std::find( m_vecMapEffect.begin(), m_vecMapEffect.end(), mapEffect );
	if ( pos != m_vecMapEffect.end() )
		m_vecMapEffect.erase( pos );
}

bool GLCHARLOGIC::IsValidGarbageUse( const SNATIVEID& sDeleteItem, BYTE& nType, LONGLONG& nCost )
{
	const SITEM* pDeleteItem = GLogicData::GetInstance().GetItem( sDeleteItem );
	if( !pDeleteItem )
		return false;

	SINVENITEM* pInvenItem = m_cInventory.GetItem( m_ItemGarbagePosX, m_ItemGarbagePosY );
	if( pInvenItem )
	{
		const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->Id() );
		if( pItem )
		{
			if( pItem->BasicType() == ITEM_GARBAGE_CARD )
			{
				nType = EGarbageCard;
				return true;
			}
		}
	}

	float fCostPer = GLCONST_CHAR::fTrash_SellCostPer;
	if( fCostPer > 0.0f )
	{
		nType = EGarbageMoney;

		//fCostPer *= float( pDeleteItem->sBasicOp.dwBuyPrice );
		nCost = LONGLONG( fCostPer );

		return ( GetInvenMoney() >= nCost ) ? true : false;
	}

	nCost = LONGLONG( GLCONST_CHAR::wTrash_Cost );
	if( nCost > 0 )
	{
		nType = EGarbageMoney;

		return ( GetInvenMoney() >= nCost ) ? true : false;
	}

	nCost = LONGLONG( GLCONST_CHAR::wTrash_Point );
	if( nCost > 0 )
	{
		nType = EGarbagePoint;

		return ( PointShopPoint() >= nCost ) ? true : false;
	}

	return false;
}

void GLCHARLOGIC::DurabilityDropHit( unsigned int nDamage, BOOL bPvp = FALSE )
{
    if ( GLUseFeatures::GetInstance().IsUsingItemDurability() == false )
        return;

    for( int i = SLOT_HEADGEAR; i < SLOT_TSIZE; i++ )
    {
        SITEMCUSTOM& sItemCustom = m_PutOnItems[i];
        
        if( ItemDurability::GetInstance().DurabilityDropHit( sItemCustom, nDamage, bPvp ) )
            Durabilitiy_INIT_DATA( (EMSLOT)i );
    }
}

void GLCHARLOGIC::DurabilityDropUseSkill()
{
    if ( GLUseFeatures::GetInstance().IsUsingItemDurability() == false )
        return;

	EMSLOT eRHand = SLOT_RHAND;
	EMSLOT eLHand = SLOT_LHAND;

	if( IsUseArmSub() )
	{
		eRHand = SLOT_RHAND_S;
		eLHand = SLOT_LHAND_S;
	}

    {
        SITEMCUSTOM& sItemCustom = m_PutOnItems[(DWORD)eRHand];

        if( ItemDurability::GetInstance().DurabilityDropUseSkill( sItemCustom ) )
            Durabilitiy_INIT_DATA( eRHand );
    }

    {
        SITEMCUSTOM& sItemCustom = m_PutOnItems[(DWORD)eLHand];

        if( ItemDurability::GetInstance().DurabilityDropUseSkill( sItemCustom ) )
            Durabilitiy_INIT_DATA( eLHand );
    }

}

void GLCHARLOGIC::DurabilityDropDeath( BOOL bPvp/* = FALSE */)
{
    if ( GLUseFeatures::GetInstance().IsUsingItemDurability() == false )
        return;

    for( int i = SLOT_HEADGEAR; i < SLOT_TSIZE; i++ )
    {
        SITEMCUSTOM& sItemCustom = m_PutOnItems[i];

        if( ItemDurability::GetInstance().DurabilityDropDeath( sItemCustom, bPvp ) )
            Durabilitiy_INIT_DATA( (EMSLOT)i );
    }

}

void GLCHARLOGIC::ItemRepairing( EMSLOT _slot )
{
    if ( GLUseFeatures::GetInstance().IsUsingItemDurability() == false )
        return;

    SITEMCUSTOM& sItemCustom = m_PutOnItems[_slot];

    if( ItemDurability::GetInstance().ItemRepairing( sItemCustom ) )
        INIT_DATA( FALSE, FALSE );
}

void GLCHARLOGIC::Durabilitiy_INIT_DATA( EMSLOT slot )
{
    SITEMCUSTOM& sItemCustom = m_PutOnItems[slot];
    if( sItemCustom.GetNativeID()!=NATIVEID_NULL() ) 
    {
        float fPerformance = ItemDurability::GetInstance().GetPerformance( sItemCustom );
        float& fSlotPerformence = m_DurabilitySlotItemPerformance[slot];

        if( fPerformance < fSlotPerformence )
        {
            INIT_DATA( FALSE, FALSE );
        }

        fSlotPerformence = fPerformance;
    }
}

void GLCHARLOGIC::Update_AccumulateHit( DWORD dwDamage, const EMCROW eCrow, const DWORD dwID )
{
	RF_ACTIVE( m_EffSkillVarSet ).Accumulate( dwDamage, eCrow, dwID );	

}