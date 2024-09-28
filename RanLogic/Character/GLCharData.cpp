#include "../pch.h"

#include "../../SigmaCore/String/StringUtils.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/GUInterface/UITextControl.h"

#include "../Item/GLItemMan.h"
#include "../Inventory/GLInventoryStorage.h"
#include "../GLogicDataMan.h"
#include "../GLogicData.h"
#include "GLCharData.h"


// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const DWORD SCHARSKILL::VERSION	= 0x0100;
const DWORD SCHARSKILL::SIZE	= sizeof(SCHARSKILL);

const DWORD SCHARSAVESKILLFACT::VERSION = 0x0104;
const DWORD SCHARSAVESKILLFACT::SIZE    = sizeof(SCHARSAVESKILLFACT);

namespace COMMENT
{
	std::string szEMREACTION[REACT_SIZE+1] =
	{
		"REACT_MOB",
		"REACT_NPC",
		"REACT_P2P",
		"REACT_PVP",
		"REACT_ITEM",
		"REACT_SKILL",
		"REACT_NULL",
	};
};

const DWORD SSTIGMA::PushStigmaPack(const STARGETID& sUserID, const STARGETID& sTakerID, const SNATIVEID sSkillID, const SKILL::SSPEC& sSPEC)
{	
	DWORD dwIndex = IsExistStigmaPack(sUserID, sTakerID, sSkillID);
	if ( dwIndex == SKILLFACT_SIZE )
	{
		for ( DWORD _i = SKILLFACT_SIZE; _i; --_i )
		{
			if ( !sSTIGMA_PACK[SKILLFACT_SIZE - _i].bUse )
			{
				dwIndex = SKILLFACT_SIZE - _i;
				break;
			}
		}
		if ( dwIndex == SKILLFACT_SIZE )
			return SKILLFACT_SIZE;
	}

	sSTIGMA_PACK[dwIndex].bUse = true;
	sSTIGMA_PACK[dwIndex].sUserID = sUserID;
	sSTIGMA_PACK[dwIndex].sTakerID = sTakerID;
	sSTIGMA_PACK[dwIndex].sSkillID = sSkillID;
	sSTIGMA_PACK[dwIndex].dwFLAG = sSPEC.dwFLAG;
	sSTIGMA_PACK[dwIndex].dwAsignDistance = DWORD(sSPEC.fVAR1);
	sSTIGMA_PACK[dwIndex].fRate = sSPEC.fVAR2;

	return dwIndex;
}

const DWORD SSTIGMA::IsExistStigmaPack(const STARGETID& sUserID, const STARGETID& sTakerID, const SNATIVEID sSkillID){
	for ( DWORD _i = SKILLFACT_SIZE; _i; --_i )
	{
		if ( !sSTIGMA_PACK[_i - 1].bUse )
			continue;
		
		if ( sSTIGMA_PACK[_i - 1].sUserID == sUserID &&
			sSTIGMA_PACK[_i - 1].sTakerID == sTakerID &&	
			sSTIGMA_PACK[_i - 1].sSkillID == sSkillID )
			return _i - 1;		
	}
	return SKILLFACT_SIZE;
}
const DWORD SSTIGMA::IsExistStigmaPack(const STARGETID& sUserID, const SNATIVEID sSkillID){
	for ( DWORD _i = SKILLFACT_SIZE; _i; --_i )
	{
		if ( !sSTIGMA_PACK[_i - 1].bUse )
			continue;
		
		if ( sSTIGMA_PACK[_i - 1].sUserID == sUserID &&
			sSTIGMA_PACK[_i - 1].sSkillID == sSkillID )
			return _i - 1;		
	}
	return SKILLFACT_SIZE;
}
const SSTIGMA::SSTIGMA_PACK* SSTIGMA::GetStigmaPack(const DWORD dwIndex){
	if ( dwIndex < SKILLFACT_SIZE )
		return &sSTIGMA_PACK[dwIndex];
	else
		return NULL;
}
const SSTIGMA::SSTIGMA_PACK* SSTIGMA::GetStigmaPack(void){	
	if ( this->dwStigmaIndex < SKILLFACT_SIZE )
		return &sSTIGMA_PACK[this->dwStigmaIndex++];
	else
		return NULL;
}

void SSTIGMA::DeleteStigmaPack(const DWORD dwIndex){
	if ( sSTIGMA_PACK[dwIndex].bUse )	
		sSTIGMA_PACK[dwIndex].bUse = false;
}




SEXPERIENCERATE::ExperienceElement::ExperienceElement(const float _fLifeTime, const float _fIncreaseRate)
: fLifeTime(_fLifeTime)
, fIncreaseRate(_fIncreaseRate)
{
}

SEXPERIENCERATE::ExperienceElement::~ExperienceElement(void)
{
	fLifeTime = 0.0f;
	fIncreaseRate = 0.0f;
}

SEXPERIENCERATE::SEXPERIENCERATE(void)
: sizeMax(0)
, fTotalIncreaseRate(0.0f)
, bLock(false)
, skillID(SNATIVEID(false))
{
}

const float SEXPERIENCERATE::getRateLatest(void) const
{
	const unsigned int _sizeElement(lifeTimeVector.size());
	if ( _sizeElement == 0 )
		return 0.0f;
	return lifeTimeVector[_sizeElement - 1].fIncreaseRate;
}
const float SEXPERIENCERATE::getRate(const unsigned int _index) const
{
	if ( _index >= lifeTimeVector.size() )
		return 0.0f;

	return lifeTimeVector[_index].fIncreaseRate;
}
const float SEXPERIENCERATE::getTime(const unsigned int _index) const
{
	if ( _index >= lifeTimeVector.size() )
		return 0.0f;

	return lifeTimeVector[_index].fLifeTime;
}


void SEXPERIENCERATE::_refresh(void)
{
	float _fIncreaseRate(0.0f);
	for ( LifeTimeVectorIter _iter(lifeTimeVector.begin()); _iter != lifeTimeVector.end(); ++_iter )
	{
		ExperienceElement& experienceItem = *_iter;
		_fIncreaseRate += experienceItem.fIncreaseRate;
	}
	this->fTotalIncreaseRate = _fIncreaseRate;
}

const float SEXPERIENCERATE::updateTime(const float fElapsedTime)
{
	if ( this->bLock == true )
		return -1.0f;

	float fRate(-1.0f);
	for ( LifeTimeVectorIter _iteratorElement(lifeTimeVector.begin()); _iteratorElement != lifeTimeVector.end(); )
	{
		float& fLifeTime = _iteratorElement->fLifeTime;
		fLifeTime -= fElapsedTime;
		if ( fLifeTime < 0.0f )
		{
			fRate = _iteratorElement->fIncreaseRate;
			_iteratorElement = lifeTimeVector.erase(_iteratorElement);
			break ;
		}
		else
			++_iteratorElement;
	}

	if ( fRate > 0.0f )
	{
		SEXPERIENCERATE::_refresh();
		return fRate;
	}

	return -1.0f;
}
const bool SEXPERIENCERATE::push_back(const SNATIVEID& _skillID, const DWORD _sizeMax, const float _fLifeTime, const float _fIncreaseRate)
{
	if ( _sizeMax > 10 )
		return false;

	if ( _fLifeTime == 0.0f || _fIncreaseRate == 0.0f )
		return false;

	if ( skillID != _skillID )
		SEXPERIENCERATE::clear();	

	skillID = _skillID;
	sizeMax = _sizeMax;

	if ( lifeTimeVector.size() >= _sizeMax )
		lifeTimeVector.erase(lifeTimeVector.begin());

	lifeTimeVector.push_back(ExperienceElement(_fLifeTime, _fIncreaseRate));

	SEXPERIENCERATE::_refresh();

	return true;
}

void SEXPERIENCERATE::clear(void)
{
	sizeMax = 0;
	fTotalIncreaseRate = 0.0f;
	bLock = false;
	skillID = SNATIVEID(false);
	lifeTimeVector.clear();
}


SAIRBORNE::Element::Element(const float _fTimeLife, const float _fTimeLevitate, const float _fHeightMax)
: fTimeElapsed(0.0f)
, fUTimeLife(1.0f / _fTimeLife)
, fLevitateT(1.0f - (_fTimeLevitate / _fTimeLife))
, fSlopeRiseFall(0.15f / (1.0f - _fTimeLevitate / _fTimeLife))
, fSlopeLevitate(0.85f / (_fTimeLevitate / _fTimeLife))
, fInterceptY(1.0f - (0.85f / (_fTimeLevitate / _fTimeLife)))
, fHeightMax(_fHeightMax)
{
}

SAIRBORNE::Element::Element(const SAIRBORNE::Element& _rhs)
: fTimeElapsed(_rhs.fTimeElapsed)
, fUTimeLife(_rhs.fUTimeLife)
, fLevitateT(_rhs.fLevitateT)
, fSlopeRiseFall(_rhs.fSlopeRiseFall)
, fSlopeLevitate(_rhs.fSlopeLevitate)
, fInterceptY(_rhs.fInterceptY)
, fHeightMax(_rhs.fHeightMax)
{
}

const float SAIRBORNE::Element::update(const float _fTimeElapsed)
{ // ret : height, time over : height < 0.0f;
	this->fTimeElapsed += _fTimeElapsed;

	// t : 1.0f -> 0.0f, 0.0f -> 1.0f;
	const float _fT(::abs(this->fTimeElapsed * this->fUTimeLife - 0.5f) * 2.0f);

	const float _fNewT(_fT < this->fLevitateT ? (_fT * this->fSlopeRiseFall) : (_fT * this->fSlopeLevitate + this->fInterceptY));

	return ::cos((3.14159265359f * 0.5f) * _fNewT) * this->fHeightMax;
}


SAIRBORNE::SAIRBORNE(void)
: fHeightCurrent(0.0f)
{
}


void SAIRBORNE::update(const float _fTimeElapsed)
{
	float _fHeightCurrent(0.0f);

	for ( SAIRBORNE::ElementListIterator _iteratorElement(this->listElement.begin()); _iteratorElement != this->listElement.end(); )
	{
		SAIRBORNE::Element& _element(*_iteratorElement);
		const float _fHeight(_element.update(_fTimeElapsed));
		if ( _fHeight < 0.0f )
		{
			_iteratorElement = this->listElement.erase(_iteratorElement);
			continue;
		}

		if ( _fHeight > _fHeightCurrent )
			_fHeightCurrent = _fHeight;

		++_iteratorElement;
	}

	this->fHeightCurrent = _fHeightCurrent;
}



SCHARDATA::SCHARDATA()
	: m_dwUserID(0)
	, m_dwUserLvl(0)
	, m_tPREMIUM(0)
	, m_bPREMIUM(false)
	, m_tCHATBLOCK(0)
	, m_wINVENLINE(0)
	, m_dwServerID(0)
	, m_CharDbNum(0)
	, m_emTribe(TRIBE_HUMAN)
	, m_emClass(GLCC_FIGHTER_M)
	, m_wSchool(0)
	, m_wSex(0)
	, m_wHair(0)
	, m_wHairColor(0)
	, m_wFace(0)
	, m_nBright(1)
	, m_nLiving(0)
	, m_wLevel(0)
	, m_lnMoney(0)
	, m_lVNGainSysMoney(0)
	, m_bMoneyUpdate(FALSE)
	, m_bStorageMoneyUpdate(FALSE)
	, m_wTempLevel(0)
	, m_lnTempMoney(0)
	, m_lnTempStorageMoney(0)
	, m_ClubDbNum(CLUB_NULL)
	, m_tSECEDE(0)
	, m_wStatsPoint(0)
	, m_powerAttack(0)
	, m_powerDefence(0)
	, m_dwSkillPoint(0)
	, m_wPK(0)	
	, m_EventStartTime(0)
	, m_EventBusterTime(0)
	, m_EventBusterExpGain(0.0f)
	, m_EventBusterItemGain(0.0f)
	, m_bEventBuster(FALSE)
	, m_bEventStart(FALSE)
	, m_tLoginTime(0)
	, m_EventStartLv(0)
	, m_EventEndLv(0)
	, m_bEventApply(FALSE)
	, m_RemainEventTime(0)
	, m_RemainBusterTime(0)
	, m_VietnamGameTime(0)
	, m_dwVietnamGainType(0)
	, m_lVNGainSysExp(0)
	, m_dwVietnamInvenCount(0)
	, m_lnRestorableExp ( 0 )
	, m_fCDMSafeTime( 0.0f )
	, m_fRemainMacroTime ( 0.0f )
{
	memset(m_szName, 0, sizeof(char) * CHAR_SZNAME);
	memset(m_szNick, 0, sizeof(char) * CHAR_SZNAME);
	memset(m_szPhoneNumber, 0, sizeof(TCHAR) * SMS_RECEIVER);
	memset(m_powerAttribute, 0x00L, sizeof(int) * SKILL::EMAPPLY_NSIZE);

	for ( int i=0; i<EMSTORAGE_CHANNEL_SPAN_SIZE; ++i )
		m_tSTORAGE[i] = 0;
	for ( int i=0; i<EMSTORAGE_CHANNEL_SPAN_SIZE; ++i )
		m_bSTORAGE[i] = false;
}

VOID SCHARDATA::RegistProperty()
{
    //Super::RegistProperty();

    //{
    //    ic::CPropertyWord* pProperty = ic::CPropertyFactory::New< ic::CPropertyWord >();
    //    pProperty->SetPtr( &m_wSchool );
    //    InsertProperty( _T( "School" ), pProperty );
    //}
    //{
    //    ic::CPropertyWord* pProperty = ic::CPropertyFactory::New< ic::CPropertyWord >();
    //    pProperty->SetPtr( &m_wSex );
    //    InsertProperty( _T( "Sex" ), pProperty );
    //}
    //{
    //    ic::CPropertyWord* pProperty = ic::CPropertyFactory::New< ic::CPropertyWord >();
    //    pProperty->SetPtr( &m_wLevel );
    //    InsertProperty( _T( "Level" ), pProperty );
    //}
}

bool SCHARDATA::IsKEEP_STORAGE ( DWORD dwCHANNEL )
{
	//	기본 창고일 경우 참.
	if ( EMSTORAGE_CHANNEL_DEF <= dwCHANNEL && dwCHANNEL < EMSTORAGE_CHANNEL_DEF+EMSTORAGE_CHANNEL_DEF_SIZE )
        return true;

	//	추가 창고일 경우.
	if ( EMSTORAGE_CHANNEL_SPAN <= dwCHANNEL && dwCHANNEL < EMSTORAGE_CHANNEL_SPAN+EMSTORAGE_CHANNEL_SPAN_SIZE )
	{
		return m_bSTORAGE[dwCHANNEL-EMSTORAGE_CHANNEL_SPAN];
	}

	//	추가 창고일 경우.
	if ( EMSTORAGE_CHANNEL_PREMIUM <= dwCHANNEL && dwCHANNEL < EMSTORAGE_CHANNEL_PREMIUM+EMSTORAGE_CHANNEL_PREMIUM_SIZE )
	{
		return m_bPREMIUM;
	}

	return false;
}

CTime SCHARDATA::GetStorageTime (  DWORD dwCHANNEL )
{
	if ( EMSTORAGE_CHANNEL_DEF <= dwCHANNEL && dwCHANNEL < EMSTORAGE_CHANNEL_DEF+EMSTORAGE_CHANNEL_DEF_SIZE )
        return CTime(0);

	if ( EMSTORAGE_CHANNEL_SPAN <= dwCHANNEL && dwCHANNEL < EMSTORAGE_CHANNEL_SPAN+EMSTORAGE_CHANNEL_SPAN_SIZE )
	{
		return CTime(m_tSTORAGE[dwCHANNEL-EMSTORAGE_CHANNEL_SPAN]);
	}

	if ( EMSTORAGE_CHANNEL_PREMIUM <= dwCHANNEL && dwCHANNEL < EMSTORAGE_CHANNEL_PREMIUM+EMSTORAGE_CHANNEL_PREMIUM_SIZE )
	{
		return CTime(0);
	}

	return CTime(0);
}

WORD SCHARDATA::GetOnINVENLINE ()
{
	// EInvenPremiumLine
	if( m_bPREMIUM )
		return min( m_wINVENLINE + EInvenPremiumLine, EInvenTotalLine - GLCONST_CHAR::wInvenDefaultLine );
	
	return min( m_wINVENLINE, EInvenTotalLine - GLCONST_CHAR::wInvenDefaultLine - EInvenPremiumLine );
}

void SCHARDATA::CalcPREMIUM ()
{
	CTime tPREMIUM( m_tPREMIUM );
	const CTime cTIME_CUR = CTime::GetCurrentTime();

	bool bPREMIUM( true );
	if( m_tPREMIUM == 0 || tPREMIUM < cTIME_CUR )
		bPREMIUM = false;

	m_bPREMIUM = bPREMIUM;
}

void SCHARDATA::SetInvenMoney( LONGLONG llMoney )
{
	m_bMoneyUpdate = TRUE;

	if ( llMoney >= 0 )
	{
		m_lnMoney = llMoney;
	}

	if ( m_lnMoney < 0 )
	{
		m_lnMoney = 0;
	}
}

LONGLONG SCHARDATA::AddInvenMoney( LONGLONG llMoney )
{
	m_bMoneyUpdate = TRUE;

	if ( llMoney >= 0 )
	{
		m_lnMoney = m_lnMoney + llMoney;
	}

	if ( m_lnMoney < 0 )
	{
		m_lnMoney = 0;
	}

	return m_lnMoney;
}

LONGLONG SCHARDATA::SubtractInvenMoney( LONGLONG llMoney )
{
	m_bMoneyUpdate = TRUE;

	if ( llMoney >= 0 )
	{
		m_lnMoney = m_lnMoney - llMoney;
	}

	if ( m_lnMoney < 0 )
	{
		m_lnMoney = 0;
	}

	return m_lnMoney;
}

void SCHARDATA::SetTempInvenMoney( LONGLONG llMoney )
{
	if ( llMoney >= 0 )
	{
		m_lnTempMoney = llMoney;
	}

	if ( m_lnTempMoney < 0 )
	{
		m_lnTempMoney = 0;
	}
}

SCHARDATA2::SCHARDATA2() :
	m_lnStorageMoney(0),
	m_bServerStorage(FALSE),

	m_wSKILLQUICK_ACT(0),

	m_dwStartGate(0),
	m_vStartPos(0,0,0),

	m_sSaveMapID(false),
	m_vSavePos(0,0,0),

	m_sLastCallMapID(false),
	m_vLastCallPos(0,0,0),

	m_emIPBonus(EMIP_BONUS_NONE),
	m_bVietnamLevelUp(false),

	m_bTracingUser(FALSE),
    m_nContributionPoint(0),
	m_nRewardContribution(0),
	m_TexasHoldemChip(0),
    m_ConfignmentSaleSlotExpansionDate(0),
	m_sBasicSummonID( SNATIVEID( false )  ),
	m_wSlotIndex(0)
{
	for ( int i=0; i<EMSKILLQUICK_SIZE; ++i )
		m_sSKILLQUICK[i] = NATIVEID_NULL();

	for ( int i=0; i<EMSTORAGE_CHANNEL; ++i )
		m_bStorage[i] = FALSE;

	for ( int i=0; i<SKILLFACT_SIZE; ++i )
		m_sSAVESKILLFACT[i] = SCHARSAVESKILLFACT();

	for ( int i=0; i<EMCHAR_SLOT_DATA_SIZE; ++i )
		m_arrLockSlotState[i] = true;

	InitRebuildData();	// ITEMREBUILD_MARK
	CloseRebuild();
}

void SCHARDATA2::Assign ( SCHARDATA2 &sDATA )
{
	GETCHARDATA() = sDATA.GETCHARDATA();

	StringCchCopy( m_szUID, USR_ID_LENGTH+1, sDATA.m_szUID );

	m_ExpSkills = sDATA.m_ExpSkills;
	m_ExpChangeSkills = sDATA.m_ExpChangeSkills;

	memcpy ( m_PutOnItems, sDATA.m_PutOnItems, sizeof(SITEMCUSTOM)*SLOT_TSIZE );

	m_wSKILLQUICK_ACT = sDATA.m_wSKILLQUICK_ACT;
	memcpy ( m_sSKILLQUICK, sDATA.m_sSKILLQUICK, sizeof(SNATIVEID)*EMSKILLQUICK_SIZE );
	memcpy ( m_sACTIONQUICK, sDATA.m_sACTIONQUICK, sizeof(SACTION_SLOT)*EMACTIONQUICK_SIZE );

	m_cInventory.Assign ( sDATA.m_cInventory );

    EMSERVICE_PROVIDER sp = GLogicData::GetInstance().GetServiceProvider();

//#if defined(VN_PARAM) //vietnamtest%%%
	if (sp == SP_VIETNAM)
	{
		m_cVietnamInventory.Assign( sDATA.m_cVietnamInventory );
	}
//#endif
	
	m_bServerStorage = sDATA.m_bServerStorage;
	m_lnStorageMoney = sDATA.m_lnStorageMoney;

	for ( int i=0; i<EMSTORAGE_CHANNEL; ++i )
	{
		m_bStorage[i] = sDATA.m_bStorage[i];
		m_cStorage[i].Assign ( sDATA.m_cStorage[i] );
	}

	m_cQuestPlay = sDATA.m_cQuestPlay;

	m_mapCharged = sDATA.m_mapCharged;
	m_cInvenCharged.Assign ( sDATA.m_cInvenCharged );
	m_mapChargedKey = sDATA.m_mapChargedKey;

	m_sStartMapID = sDATA.m_sStartMapID;
	m_dwStartGate = sDATA.m_dwStartGate;
	m_vStartPos = sDATA.m_vStartPos;

	m_sSaveMapID = sDATA.m_sSaveMapID;
	m_vSavePos = sDATA.m_vSavePos;

	m_sLastCallMapID = sDATA.m_sLastCallMapID;
	m_vLastCallPos = sDATA.m_vLastCallPos;

	m_mapCoolTimeType = sDATA.m_mapCoolTimeType;
	m_mapCoolTimeID = sDATA.m_mapCoolTimeID;

	m_nContributionPoint = sDATA.m_nContributionPoint;

//#if defined(_RELEASED) || defined(TW_PARAM) || defined(TH_PARAM) || defined(HK_PARAM) // ***Tracing Log print	
	//if (sp == SP_OFFICE_TEST || sp == SP_TAIWAN || sp == SP_HONGKONG || sp == SP_THAILAND)
	//{
		//m_bTracingUser = sDATA.m_bTracingUser;
	//}
//#endif

	m_emIPBonus	= sDATA.m_emIPBonus;

	m_TexasHoldemChip = sDATA.m_TexasHoldemChip;

    m_ConfignmentSaleSlotExpansionDate = sDATA.m_ConfignmentSaleSlotExpansionDate;

	m_sBasicSummonID = sDATA.m_sBasicSummonID;
	m_sSummonable = sDATA.m_sSummonable;

	m_wSlotIndex = sDATA.m_wSlotIndex;
	memcpy( m_arrLockSlotState, sDATA.m_arrLockSlotState, sizeof(bool)*EMCHAR_SLOT_DATA_SIZE );

	memcpy(m_sSAVESKILLFACT, sDATA.m_sSAVESKILLFACT, sizeof(SCHARSAVESKILLFACT)*SKILLFACT_SIZE);

	m_sBackUpRebuildInvenItem = sDATA.m_sBackUpRebuildInvenItem;
}

BOOL SCHARDATA2::SETEXPSKILLS_BYBUF ( se::ByteStream &ByteStream )
{
	if ( ByteStream.IsEmpty() )
	{
		//CONSOLEMSG_WRITE ( "SETITEM_BYBUFFER() se::ByteStream::IsEmpty()" );
		return TRUE;
	}

	m_ExpSkills.clear();

	//	DWORD[VERSION] + DWORD[SIZE] + DWORD[NUMBER] + EXPSKILLS_ARRAY[SIZE]
	//
	DWORD dwVersion, dwSize, dwNum;

	ByteStream >> dwVersion;
	ByteStream >> dwSize;
	ByteStream >> dwNum;

	GASSERT ( (dwSize==sizeof(SCHARSKILL)) && (SCHARSKILL::VERSION==dwVersion) );

	SCHARSKILL sCharSkill;
	for ( DWORD i=0; i<dwNum; i++ )
	{
		ByteStream.ReadBuffer ( (LPBYTE)&sCharSkill, sizeof(SCHARSKILL) );
		m_ExpSkills.insert ( std::make_pair(sCharSkill.sNativeID.dwID,sCharSkill) );
	}

	return TRUE;
}

BOOL SCHARDATA2::ValidationCharData()
{
	BOOL bFindError = FALSE;
	SCHARSTATS sOriStats;
	DWORD dwOriAttack = 0;
	DWORD dwOriDefence = 0;

	sOriStats = m_sStats;
	dwOriAttack = m_powerAttack;
	dwOriDefence = m_powerDefence;

	if( ValidationCharStateMin(&(m_sStats.wDex)) == FALSE || ValidationCharStateMax(&(m_sStats.wDex)) == FALSE ) bFindError = TRUE;

	if( ValidationCharStateMin(&(m_sStats.wInt)) == FALSE || ValidationCharStateMax(&(m_sStats.wInt)) == FALSE ) bFindError = TRUE;

	if( ValidationCharStateMin(&(m_sStats.wStr)) == FALSE || ValidationCharStateMax(&(m_sStats.wStr)) == FALSE) bFindError = TRUE;

	if( ValidationCharStateMin(&(m_sStats.wPow)) == FALSE || ValidationCharStateMax(&(m_sStats.wPow)) == FALSE) bFindError = TRUE;

	if( ValidationCharStateMin(&(m_sStats.wSpi)) == FALSE || ValidationCharStateMax(&(m_sStats.wSpi)) == FALSE) bFindError = TRUE;

	if( ValidationCharStateMin(&(m_sStats.wSta)) == FALSE || ValidationCharStateMax(&(m_sStats.wSta)) == FALSE) bFindError = TRUE;

	if( ValidationCharValueMin(&m_powerAttack) == FALSE || ValidationCharValueMax(&m_powerAttack) == FALSE) bFindError = TRUE;

	if( ValidationCharValueMin(&m_powerDefence) == FALSE || ValidationCharValueMax(&m_powerDefence) == FALSE) bFindError = TRUE;

	if( bFindError == TRUE )
	{
		sc::writeLogError(sc::string::format("[CRITICAL HACKING] UserID %1%, ChaDBNum %2%, ChaName %3% - Ori wDex %4%, wInt %5%, wStr %6%, wPow %7%, wSpi %8%, wSta %9%, m_powerAttack %10%, m_powerDefence %11% ", m_dwUserID, m_CharDbNum, m_szName, sOriStats.wDex, sOriStats.wInt, sOriStats.wStr, sOriStats.wPow, sOriStats.wSpi, sOriStats.wSta, dwOriAttack, dwOriDefence ));
		sc::writeLogError(sc::string::format("[CRITICAL HACKING] UserID %1%, ChaDBNum %2%, ChaName %3% - Validated wDex %4%, wInt %5%, wStr %6%, wPow %7%, wSpi %8%, wSta %9%, m_powerAttack %10%, m_powerDefence %11% ", m_dwUserID, m_CharDbNum, m_szName, m_sStats.wDex, m_sStats.wInt, m_sStats.wStr, m_sStats.wPow, m_sStats.wSpi, m_sStats.wSta, m_powerAttack, m_powerDefence ));
	}

	return bFindError;
}

BOOL SCHARDATA2::ValidationCharStateMin(WORD *value)
{
	if( *value < 0 )
	{
		*value = 0;
		return FALSE;
	}

	return TRUE;
}

BOOL SCHARDATA2::ValidationCharValueMin(int *value)
{
	if( *value < 0 )
	{
		*value = 0;
		return FALSE;
	}

	return TRUE;
}

BOOL SCHARDATA2::ValidationCharStateMax(WORD *value)
{
	if( *value > 5000 )
	{
		*value = 0;
		return FALSE;
	}

	return TRUE;
}

BOOL SCHARDATA2::ValidationCharValueMax(int *value)
{
	if( *value > 30000 )
	{
		*value = 0;
		return FALSE;
	}

	return TRUE;
}

BOOL SCHARDATA2::ValidatePutOnTurnNum()
{
	BOOL Result = TRUE;
	for ( DWORD loop = 0 ; loop < SLOT_TSIZE ; loop++ )
	{
		if ( m_PutOnItems[loop].GetNativeID() == NATIVEID_NULL() )
			continue;

		const SITEM* pItem = GLogicData::GetInstance().GetItem( m_PutOnItems[loop].GetNativeID() );
		if ( !pItem )
		{
			continue;
		}

		switch ( pItem->sBasicOp.emItemType )
		{
		case ITEM_BOX:
		case ITEM_PREMIUMSET:
		case ITEM_RANDOMITEM:
		case ITEM_LOCK_BOX:
		case ITEM_SELECTIVEFORM_BOX:
			{
				if ( 1 == pItem->sDrugOp.wPileNum && m_PutOnItems[loop].wTurnNum > 1 )
				{
					sc::writeLogError( sc::string::format( 
						"ValidatePutOnTurnNum - pItem->sDrugOp.wPileNum is 1, but m_PutOnItems[loop].wTurnNum is %1% guid %2% ", 
						m_PutOnItems[loop].wTurnNum, 
						sc::string::uuidToString( m_PutOnItems[loop].guid ).c_str() ) );

					m_PutOnItems[loop].wTurnNum = 1;
					Result = FALSE;
				}

				/*
				if ( m_PutOnItems[loop].wTurnNum > pItem->sDrugOp.wPileNum )
				{
					sc::writeLogError( sc::string::format( 
						"ValidatePutOnTurnNum - pInvenItem->sItemCustom.wTurnNum > pItem->sDrugOp.wPileNum wTurnNum %1% wPileNum %2% guid %3% ", 
						m_PutOnItems[loop].wTurnNum, 
						pItem->sDrugOp.wPileNum, 
						sc::string::uuidToString( m_PutOnItems[loop].guid ).c_str() ) );

					m_PutOnItems[loop].wTurnNum = pItem->sDrugOp.wPileNum;
					Result = FALSE;
				}*/
			}
			break;
		default:
			break;
		}
	}
	return Result;
}

BOOL SCHARDATA2::ValidateVehicleRandomOption()
{
	BOOL Result = TRUE;
	for ( DWORD loop = 0 ; loop < SLOT_TSIZE ; loop++ )
	{
		if ( m_PutOnItems[loop].GetNativeID() == NATIVEID_NULL() )
			continue;

		const SITEM* pItem = GLogicData::GetInstance().GetItem( m_PutOnItems[loop].GetNativeID() );
		if ( !pItem )
		{
			continue;
		}

		if ( pItem->sBasicOp.emItemType != ITEM_VEHICLE )
		{
			continue;
		}

		m_PutOnItems[loop].cDAMAGE = 0;
		m_PutOnItems[loop].cDEFENSE = 0;

		if ( !m_PutOnItems[loop].IsOpItem() )
		{
			continue;
		}

		RandomOpt option;
		m_PutOnItems[loop].randomOption = option;
		Result = FALSE;

		sc::writeLogError( sc::string::format( 
			"ValidateVehicleRandomOption mid %1% sid %2%", 
			m_PutOnItems[loop].GetNativeID().wMainID, 
			m_PutOnItems[loop].GetNativeID().wSubID ) );
	}
	return Result;
}

BOOL SCHARDATA2::ValidateRandomOption( std::vector< SITEMCUSTOM >& vecDeleteItems )
{
	BOOL Result = TRUE;
	for ( DWORD loop = 0 ; loop < SLOT_TSIZE ; loop++ )
	{
		if ( m_PutOnItems[loop].GetNativeID() == NATIVEID_NULL() )
			continue;

		const SITEM* pItem = GLogicData::GetInstance().GetItem( m_PutOnItems[loop].GetNativeID() );
		if ( !pItem )
		{
			continue;
		}

		if ( !m_PutOnItems[loop].IsOpItem() )
		{
			continue;
		}

		Result = FALSE;
		vecDeleteItems.push_back( m_PutOnItems[loop] );

		sc::writeLogInfo( sc::string::format( 
			"ValidateRandomOption puton deleted mid %1% sid %2%", 
			m_PutOnItems[loop].GetNativeID().wMainID, 
			m_PutOnItems[loop].GetNativeID().wSubID ) );

		m_PutOnItems[loop] = SITEMCUSTOM( SNATIVEID( false ) );
	}
	return Result;
}

DWORD SCHARDATA2::GetExpSkillsDataSize()
{
	DWORD		dwDataSize = 0;

	dwDataSize += sizeof( SCHARSKILL::VERSION );
	dwDataSize += sizeof( SCHARSKILL::SIZE );

	dwDataSize += sizeof( DWORD );		// m_ExpSkills.size();
	dwDataSize += sizeof( SCHARSKILL ) * m_ExpSkills.size();

	return dwDataSize;
}

BOOL SCHARDATA2::GetExpSkillsData( BYTE* pDest, const DWORD dwDestSize )
{
	se::SerializeWritableData	data( pDest, dwDestSize );
	
	data << SCHARSKILL::VERSION;
	data << SCHARSKILL::SIZE;

	DWORD dwSize = (DWORD) m_ExpSkills.size();
	data << dwSize;

	SKILL_MAP_CITER iter = m_ExpSkills.begin ();
	SKILL_MAP_CITER iter_end = m_ExpSkills.end ();
	for( ;iter != iter_end; ++iter )
	{
		data.Write( &(*iter).second, sizeof( SCHARSKILL ) );
	}

	return TRUE;
}

BOOL SCHARDATA2::GETEXPSKILLS_BYBUF ( se::ByteStream &ByteStream ) const
{
	//	DWORD[VERSION] + DWORD[SIZE] + DWORD[NUMBER] + EXPSKILLS_ARRAY[SIZE]
	//
	ByteStream.ClearBuffer ();

	ByteStream << SCHARSKILL::VERSION;
	ByteStream << SCHARSKILL::SIZE;
	
	DWORD dwSize = (DWORD) m_ExpSkills.size();
	ByteStream << dwSize;

	SKILL_MAP_CITER iter = m_ExpSkills.begin ();
	SKILL_MAP_CITER iter_end = m_ExpSkills.end ();

	for ( ; iter!=iter_end; ++iter )
	{
		const SCHARSKILL &sSkill = (*iter).second;
		ByteStream.WriteBuffer ( &sSkill, (DWORD)sizeof(SCHARSKILL) );
	}

	return TRUE;
}

DWORD SCHARDATA2::GetPutOnItemsDataSize()
{
	DWORD		dwDataSize = 0;

	dwDataSize += sizeof( SITEMCUSTOM::VERSION );
	dwDataSize += sizeof( DWORD );	//sizeof(SITEMCUSTOM);
	dwDataSize += sizeof( DWORD );	//SLOT_TSIZE;

	dwDataSize += sizeof( SITEMCUSTOM ) * SLOT_TSIZE;

	return dwDataSize;
}

BOOL SCHARDATA2::GetPutOnItemsData( BYTE* pDest, const DWORD dwDestSize )
{
	se::SerializeWritableData	data( pDest, dwDestSize );

	data << SITEMCUSTOM::VERSION
		 << (DWORD)sizeof(SITEMCUSTOM)
		 << (DWORD)SLOT_TSIZE;

	data.Write( m_PutOnItems, sizeof( SITEMCUSTOM ) * SLOT_TSIZE );

	return TRUE;
}

BOOL SCHARDATA2::GETPUTONITEMS_BYBUF ( se::ByteStream &ByteStream ) const
{
	ByteStream.ClearBuffer ();

	ByteStream << SITEMCUSTOM::VERSION;
	ByteStream << (DWORD)sizeof(SITEMCUSTOM);
	
	DWORD dwSize = (DWORD) SLOT_TSIZE;
	ByteStream << dwSize;

	ByteStream.WriteBuffer ( (LPBYTE)&m_PutOnItems[0], (DWORD)sizeof(SITEMCUSTOM)*dwSize );

	return TRUE;
}

BOOL SCHARDATA2::SETSKILL_QUICKSLOT ( se::ByteStream &ByteStream )
{
	if ( ByteStream.IsEmpty() )
	{
		//CONSOLEMSG_WRITE ( "SETSKILL_QUICKSLOT() se::ByteStream::IsEmpty()" );
		return TRUE;
	}

	for ( int i = 0; i<EMSKILLQUICK_SIZE; ++i )
		m_sSKILLQUICK[i] = NATIVEID_NULL();

	//	DWORD[VERSION] + DWORD[SIZE] + DWORD[NUMBER] + m_sSKILLQUICK[SIZE]
	//
	DWORD dwVersion, dwSize, dwNum;

	ByteStream >> dwVersion;

	switch ( dwVersion )
	{
	case 0x0100:
		{
			ByteStream >> dwSize;
			ByteStream >> dwNum;

			SNATIVEID sSKILL_ID;
			for ( DWORD i=0; i<dwNum; i++ )
			{
				if ( i>=EMSKILLQUICK_SIZE_OLD )		break;

				GASSERT ( dwSize==sizeof(SNATIVEID) );
				ByteStream.ReadBuffer ( (LPBYTE)&sSKILL_ID, sizeof(SNATIVEID) );

				m_sSKILLQUICK[i] = sSKILL_ID;
			}
		}
		break;

	case 0x0101:
		{
			ByteStream >> dwSize;
			ByteStream >> dwNum;

			SNATIVEID sSKILL_ID;
			for ( DWORD i=0; i<dwNum; i++ )
			{
				if ( i>=EMSKILLQUICK_SIZE_OLD )		break;

				GASSERT ( dwSize==sizeof(SNATIVEID) );
				ByteStream.ReadBuffer ( (LPBYTE)&sSKILL_ID, sizeof(SNATIVEID) );

				m_sSKILLQUICK[i] = sSKILL_ID;
			}

			ByteStream >> m_wSKILLQUICK_ACT;
		}
		break;
	case EMSKILLQUICK_VERSION:
		{
			ByteStream >> dwSize;
			ByteStream >> dwNum;

			SNATIVEID sSKILL_ID;
			for ( DWORD i=0; i<dwNum; i++ )
			{
				if ( i>=EMSKILLQUICK_SIZE )		break;

				GASSERT ( dwSize==sizeof(SNATIVEID) );
				ByteStream.ReadBuffer ( (LPBYTE)&sSKILL_ID, sizeof(SNATIVEID) );

				m_sSKILLQUICK[i] = sSKILL_ID;
			}

			ByteStream >> m_wSKILLQUICK_ACT;
		}
		break;

	};

	return TRUE;
}

DWORD SCHARDATA2::GetSkillQuickSlotDataSize()
{
	DWORD		dwDataSize = 0;

	dwDataSize += sizeof( EMSKILLQUICK_VERSION );
	dwDataSize += sizeof( DWORD );		//sizeof(SNATIVEID);

	dwDataSize += sizeof( DWORD );		//EMSKILLQUICK_SIZE;
	dwDataSize += sizeof( SNATIVEID ) * EMSKILLQUICK_SIZE;

	dwDataSize += sizeof( m_wSKILLQUICK_ACT );

	return dwDataSize;
}

BOOL SCHARDATA2::GetSkillQuickSlotData( BYTE* pDest, const DWORD dwDestSize )
{
	se::SerializeWritableData	data( pDest, dwDestSize );

	data << EMSKILLQUICK_VERSION
		 << (DWORD)sizeof(SNATIVEID)
		 << (DWORD)EMSKILLQUICK_SIZE;

	data.Write( (LPBYTE)m_sSKILLQUICK, sizeof( SNATIVEID ) * EMSKILLQUICK_SIZE );

	data << m_wSKILLQUICK_ACT;

	return TRUE;
}

BOOL SCHARDATA2::GETSKILL_QUICKSLOT ( se::ByteStream &ByteStream ) const
{
	ByteStream.ClearBuffer ();

	ByteStream << EMSKILLQUICK_VERSION;
	ByteStream << (DWORD)sizeof(SNATIVEID);
	
	DWORD dwSize = (DWORD) EMSKILLQUICK_SIZE;
	ByteStream << dwSize;

	ByteStream.WriteBuffer ( (LPBYTE)m_sSKILLQUICK, (DWORD)sizeof(SNATIVEID)*dwSize );

	ByteStream << m_wSKILLQUICK_ACT;

	return TRUE;
}

BOOL SCHARDATA2::SETACTION_QUICKSLOT ( se::ByteStream &ByteStream )
{
	if ( ByteStream.IsEmpty() )
	{
		//CONSOLEMSG_WRITE ( "SETACTION_QUICKSLOT() se::ByteStream::IsEmpty()" );
		return TRUE;
	}

	//	DWORD[VERSION] + DWORD[SIZE] + DWORD[NUMBER] + m_sActionSLOT[SIZE]
	//
	DWORD dwVersion, dwSize, dwNum;

	ByteStream >> dwVersion;
	ByteStream >> dwSize;
	ByteStream >> dwNum;

	GASSERT ( (dwSize==sizeof(SACTION_SLOT)) && (SACTION_SLOT::VERSION==dwVersion) );

	SACTION_SLOT sACTION_SLOT;
	for ( DWORD i=0; i<dwNum; i++ )
	{
		if ( i>=EMACTIONQUICK_SIZE )		break;

		ByteStream.ReadBuffer ( (LPBYTE)&sACTION_SLOT, sizeof(SACTION_SLOT) );

		m_sACTIONQUICK[i] = sACTION_SLOT;
	}

	return TRUE;
}

DWORD SCHARDATA2::GetActionQuickSlotDataSize()
{
	DWORD		dwDataSize = 0;

	dwDataSize += sizeof( SACTION_SLOT::VERSION );
	dwDataSize += sizeof( DWORD );		//sizeof(SACTION_SLOT);

	dwDataSize += sizeof( DWORD );		//EMACTIONQUICK_SIZE;
	dwDataSize += sizeof( SACTION_SLOT ) * EMACTIONQUICK_SIZE;

	return dwDataSize;
}

BOOL SCHARDATA2::GetActionQuickSlotData( BYTE* pDest, const DWORD dwDestSize )
{
	se::SerializeWritableData	data( pDest, dwDestSize );

	data << SACTION_SLOT::VERSION
		 << (DWORD)sizeof(SACTION_SLOT)
		 << (DWORD)EMACTIONQUICK_SIZE;

	data.Write( m_sACTIONQUICK, sizeof( SACTION_SLOT) * EMACTIONQUICK_SIZE );

	return TRUE;
}

BOOL SCHARDATA2::GETACTION_QUICKSLOT ( se::ByteStream &ByteStream ) const
{
	ByteStream.ClearBuffer ();

	ByteStream << SACTION_SLOT::VERSION;
	ByteStream << (DWORD)sizeof(SACTION_SLOT);
	
	DWORD dwSize = (DWORD) EMACTIONQUICK_SIZE;
	ByteStream << dwSize;

	ByteStream.WriteBuffer ( (LPBYTE)m_sACTIONQUICK, (DWORD)sizeof(SACTION_SLOT)*dwSize );
	
	return TRUE;
}

BOOL SCHARDATA2::SETINVENTORY_BYBUF ( se::ByteStream &ByteStream )
{
	if ( ByteStream.IsEmpty() )
		return TRUE;

	GLInventory cInven;
	BOOL bOk = cInven.SETITEM_BYBUFFER ( ByteStream );
	if ( !bOk )
		return TRUE;

	CalcPREMIUM();
	m_cInventory.SetAddLine( GetOnINVENLINE(), true );

	WORD wSIZEX = cInven.GetSizeX();
	WORD wSIZEY = cInven.GetSizeY();
	if ( wSIZEX == EInvenX && wSIZEY == EInvenTotalLine )
	{
		m_cInventory.Assign( cInven );
	}
	else
	{
		m_cInventory.SetState( EInvenX, EInvenTotalLine );

		const GLInventory::CELL_MAP* pmapITEM = cInven.GetItemList();
		GLInventory::CELL_MAP_CITER pos = pmapITEM->begin();
		GLInventory::CELL_MAP_CITER end = pmapITEM->end();
		for ( ; pos!=end; ++pos )
		{
			m_cInventory.InsertItem ( (*pos).second->sItemCustom );
		}
	}

	return TRUE;
}

BOOL SCHARDATA2::SETINVENTORY_BYBUF_FOR_MIGRATION( se::ByteStream &ByteStream )
{
	if ( ByteStream.IsEmpty() )
		return TRUE;

	GLInventory cInven;
	BOOL bOk = cInven.SETITEM_BYBUFFER_FOR_MIGRATION( ByteStream );
	if ( !bOk )
		return TRUE;

	CalcPREMIUM();
	m_cInventory.SetAddLine( GetOnINVENLINE(), true );

	WORD wSIZEX = cInven.GetSizeX();
	WORD wSIZEY = cInven.GetSizeY();
	if ( wSIZEX == EInvenX && wSIZEY == EInvenTotalLine )
	{
		m_cInventory.Assign( cInven );
	}
	else
	{
		m_cInventory.SetState( EInvenX, EInvenTotalLine );

		const GLInventory::CELL_MAP* pmapITEM = cInven.GetItemList();
		GLInventory::CELL_MAP_CITER pos = pmapITEM->begin();
		GLInventory::CELL_MAP_CITER end = pmapITEM->end();
		for ( ; pos!=end; ++pos )
		{
			m_cInventory.InsertItem( ( *pos ).second->sItemCustom );
		}
	}

	return TRUE;
}

DWORD SCHARDATA2::GetInventoryDataSize()
{
	return m_cInventory.GetItemDataSize();
}

BOOL SCHARDATA2::GetInventoryData( BYTE* pDest, const DWORD dwDestSize )
{
	se::SerializeWritableData	data( pDest, dwDestSize );
	return m_cInventory.GetItemData( data );
}

BOOL SCHARDATA2::GETINVENTORYE_BYBUF ( se::ByteStream &ByteStream ) const
{
	ByteStream.ClearBuffer ();
	m_cInventory.GETITEM_BYBUFFER ( ByteStream );

	return TRUE;
}

BOOL SCHARDATA2::SETSTORAGE_BYBUF ( se::ByteStream &ByteStream )
{
	m_bServerStorage = TRUE;

	if ( ByteStream.IsEmpty() )		return TRUE;

	DWORD dwChannel = 0;
	ByteStream >> dwChannel;
	if ( dwChannel > EMSTORAGE_CHANNEL )	dwChannel = EMSTORAGE_CHANNEL;
	
	for ( DWORD i=0; i<dwChannel; ++i )
	{
		BOOL bOk = m_cStorage[i].SETITEM_BYBUFFER ( ByteStream );
		if ( !bOk )		return TRUE;	//ERROR
	}

	return TRUE;
}

DWORD SCHARDATA2::GetStorageDataSize()
{
	DWORD		dwDataSize = 0;

	dwDataSize += sizeof( DWORD );		//(EMSTORAGE_CHANNEL);
	for( DWORD i = 0; i < EMSTORAGE_CHANNEL; ++i )
		dwDataSize += m_cStorage[i].GetItemStorageDataSize();

	return dwDataSize;
}

BOOL SCHARDATA2::GetStorageData( BYTE* pDest, const DWORD dwDestSize )
{
	se::SerializeWritableData	data( pDest, dwDestSize );

	data << (DWORD)EMSTORAGE_CHANNEL;
	for (DWORD i=0; i<EMSTORAGE_CHANNEL; ++i)
		m_cStorage[i].GetItemStorageData( data );

	return TRUE;
}

BOOL SCHARDATA2::GETSTORAGE_BYBUF(se::ByteStream& ByteStream) const
{
	ByteStream.ClearBuffer();
	ByteStream << DWORD(EMSTORAGE_CHANNEL);
	for (DWORD i=0; i<EMSTORAGE_CHANNEL; ++i)
        m_cStorage[i].GETITEM_BYBUFFER_FORSTORAGE(ByteStream);
	return TRUE;
}

BOOL SCHARDATA2::SETVTADDINVENTORY_BYBUF ( se::ByteStream &ByteStream )
{
	if ( ByteStream.IsEmpty() )
		return TRUE;

	GLInventory cInven;
	BOOL bOk = cInven.SETITEM_BYBUFFER ( ByteStream );
	if ( !bOk )
		return TRUE;

	m_cVietnamInventory.SetAddLine ( 6, true );

	const GLInventory::CELL_MAP* pmapITEM = cInven.GetItemList();
	GLInventory::CELL_MAP_CITER pos = pmapITEM->begin();
	GLInventory::CELL_MAP_CITER end = pmapITEM->end();
	for ( ; pos!=end; ++pos )
	{
		(*pos).second->sItemCustom.bVietnamGainItem = true;		
	}

	WORD wSIZEX = cInven.GetSizeX();
	WORD wSIZEY = cInven.GetSizeY();
	if ( wSIZEX == EInvenX && wSIZEY == EInvenTotalLine )
	{
		m_cVietnamInventory.Assign ( cInven );
	}
	else
	{
		m_cVietnamInventory.SetState ( EInvenX, EInvenTotalLine );

		const GLInventory::CELL_MAP* pmapITEM = cInven.GetItemList();
		GLInventory::CELL_MAP_CITER pos = pmapITEM->begin();
		GLInventory::CELL_MAP_CITER end = pmapITEM->end();
		for ( ; pos!=end; ++pos )
		{
			m_cVietnamInventory.InsertItem ( (*pos).second->sItemCustom );
		}
	}

	return TRUE;
}

DWORD SCHARDATA2::GetItemCoolTimeDataSize()
{
	DWORD		dwDataSize = 0;
	dwDataSize += sizeof( ITEM_COOLTIME::VERSION );
	dwDataSize += sizeof( DWORD );		//sizeof(ITEM_COOLTIME);

	dwDataSize += sizeof( DWORD );		//m_mapCoolTimeID.size();
	dwDataSize += sizeof( ITEM_COOLTIME ) * m_mapCoolTimeID.size();

	dwDataSize += sizeof( DWORD );		// m_mapCoolTimeType.size();
	dwDataSize += sizeof( ITEM_COOLTIME ) * m_mapCoolTimeType.size();

	return dwDataSize;
}

BOOL SCHARDATA2::GetItemCoolTimeData( BYTE* pDest, const DWORD dwDestSize )
{
	se::SerializeWritableData	data( pDest, dwDestSize );
	
	data << ITEM_COOLTIME::VERSION
		 << (DWORD)sizeof(ITEM_COOLTIME)
		 << (DWORD)m_mapCoolTimeID.size();

	for( COOLTIME_MAP_CITER	pos = m_mapCoolTimeID.begin(); 
		 pos != m_mapCoolTimeID.end(); 
		 ++pos )
	{
		data.Write( &pos->second, sizeof( ITEM_COOLTIME ) );
	}

	data << (DWORD)m_mapCoolTimeType.size();
	for( COOLTIME_MAP_CITER	pos = m_mapCoolTimeType.begin();
		 pos != m_mapCoolTimeType.end();
		 ++pos )	
	{
		data.Write( &pos->second, sizeof( ITEM_COOLTIME ) );
	}

	return TRUE;
}

BOOL SCHARDATA2::GETITEMCOOLTIME_BYBUF( se::ByteStream &ByteStream ) const
{
	ByteStream.ClearBuffer ();

	ByteStream << ITEM_COOLTIME::VERSION;
	ByteStream << (DWORD)sizeof(ITEM_COOLTIME);
	
	DWORD dwSize = (DWORD) m_mapCoolTimeID.size();
	ByteStream << dwSize;

	for (COOLTIME_MAP_CITER	pos=m_mapCoolTimeID.begin(); pos!=m_mapCoolTimeID.end(); ++pos)
	{
		const ITEM_COOLTIME& pCoolTime = pos->second;
		ByteStream.WriteBuffer((LPBYTE) &pCoolTime, sizeof(ITEM_COOLTIME));
	}

	dwSize = (DWORD) m_mapCoolTimeType.size();
	ByteStream << dwSize;
	
	for (COOLTIME_MAP_CITER	pos=m_mapCoolTimeType.begin(); pos!=m_mapCoolTimeType.end(); ++pos)	
	{
		const ITEM_COOLTIME& pCoolTime = pos->second;
		ByteStream.WriteBuffer((LPBYTE) &pCoolTime, sizeof(ITEM_COOLTIME));
	}
	return TRUE;
}

BOOL SCHARDATA2::SETITEMCOOLTIME_BYBUF( se::ByteStream &ByteStream )
{
	if ( ByteStream.IsEmpty() )
	{
		return TRUE;
	}

	//	DWORD[VERSION] + DWORD[SIZE] + DWORD[NUMBER] + ITEM_COOLTIME[SIZE]
	//
	DWORD dwVersion, dwSize, dwNum;

	ByteStream >> dwVersion;
	ByteStream >> dwSize;
	ByteStream >> dwNum;

	GASSERT ( (dwSize==sizeof(ITEM_COOLTIME)) && (ITEM_COOLTIME::VERSION==dwVersion) );

	ITEM_COOLTIME sCoolTime;
	
	for ( DWORD i=0; i<dwNum; i++ )
	{
		ByteStream.ReadBuffer ( (LPBYTE)&sCoolTime, sizeof(ITEM_COOLTIME) );
		m_mapCoolTimeID.insert(
            SCHARDATA2::COOLTIME_MAP_VALUE(sCoolTime.dwCoolID, sCoolTime));
	}

	ByteStream >> dwNum;

	for ( DWORD i=0; i<dwNum; i++ )
	{
		ByteStream.ReadBuffer ( (LPBYTE)&sCoolTime, sizeof(ITEM_COOLTIME) );
        m_mapCoolTimeType.insert(
            SCHARDATA2::COOLTIME_MAP_VALUE(sCoolTime.dwCoolID ,sCoolTime));;
	}

	return TRUE;
}

DWORD SCHARDATA2::GetVTAddInventoryDataSize()
{
	return m_cVietnamInventory.GetItemDataSize();
}

BOOL SCHARDATA2::GetVTAddInventoryData( BYTE* pDest, const DWORD dwDestSize )
{
	se::SerializeWritableData	data( pDest, dwDestSize );
	return m_cVietnamInventory.GetItemData( data );
}

BOOL SCHARDATA2::GETVTADDINVENTORYE_BYBUF ( se::ByteStream &ByteStream ) const
{
	ByteStream.ClearBuffer ();
	m_cVietnamInventory.GETITEM_BYBUFFER ( ByteStream );

	return TRUE;
}


BOOL SCHARDATA2::SETQUESTPLAY(se::ByteStream& ByteStream, bool bCheck)
{
	m_cQuestPlay.GET_BYBUFFER(ByteStream, bCheck);
	return TRUE;
}

BOOL SCHARDATA2::SETQUESTPLAY_FOR_MIGRATION( se::ByteStream& ByteStream, bool bCheck )
{
	m_cQuestPlay.GET_BYBUFFER_FOR_MIGRATION( ByteStream, bCheck );
	return TRUE;
}

BOOL SCHARDATA2::SETQUESTPLAY_NEW( 
								  se::ByteStream& ByteStream, 
								  bool bCheck, 
								  std::multimap< DWORD, SINVENITEM_SAVE >& mapProcItems, 
								  std::multimap< DWORD, SINVENITEM_SAVE >& mapEndItems, 
								  std::vector< sc::MGUID >& vecDeleteItems )
{
	m_cQuestPlay.GET_BYBUFFER_NEW( ByteStream, bCheck, mapProcItems, mapEndItems, vecDeleteItems );
	return TRUE;
}

DWORD SCHARDATA2::GetQuestPlayDataSize()
{
	return m_cQuestPlay.GetDataSize();
}

BOOL SCHARDATA2::GetQuestPlayData( BYTE* pDest, const DWORD dwDestSize )
{
	se::SerializeWritableData	data( pDest, dwDestSize );
	return m_cQuestPlay.GetData( data );
}

BOOL SCHARDATA2::GETQUESTPLAY( se::ByteStream& ByteStream ) const
{
	ByteStream.ClearBuffer();
	m_cQuestPlay.SET_BYBUFFER( ByteStream );

	return TRUE;
}

BOOL SCHARDATA2::SETSKILLFACT_BYBUF( se::ByteStream &ByteStream )
{
	if ( ByteStream.IsEmpty() )
	{
		//CONSOLEMSG_WRITE ( "SETSKILL_QUICKSLOT() se::ByteStream::IsEmpty()" );
		return TRUE;
	}

	DWORD dwVersion, dwSize, dwNum;

	ByteStream >> dwVersion;

	switch ( dwVersion )
	{
    case 0x0100:
        {
            ByteStream >> dwSize;
            ByteStream >> dwNum;

            GASSERT ( dwSize==sizeof(SCHARSAVESKILLFACT_100) );

            SCHARSAVESKILLFACT_100 sSkillFact;

            for ( DWORD i=0; i<dwNum; ++i )
            {
                ByteStream.ReadBuffer ( (LPBYTE)&sSkillFact, (DWORD)sizeof(SCHARSAVESKILLFACT_100) );	
                m_sSAVESKILLFACT[i].Assign( sSkillFact );
            }
        }
        break;
	case 0x0101:
		{
			ByteStream >> dwSize;
			ByteStream >> dwNum;

			GASSERT ( dwSize==sizeof(SCHARSAVESKILLFACT_101) );

			SCHARSAVESKILLFACT_101 sSkillFact;

			for ( DWORD i=0; i<dwNum; ++i )
			{
				ByteStream.ReadBuffer ( (LPBYTE)&sSkillFact, (DWORD)sizeof(SCHARSAVESKILLFACT_101) );	
				m_sSAVESKILLFACT[i].Assign( sSkillFact );
			}
		}
		break;
	case 0x0102:
		{
			ByteStream >> dwSize;
			ByteStream >> dwNum;

			GASSERT ( dwSize==sizeof(SCHARSAVESKILLFACT_102) );

			SCHARSAVESKILLFACT_102 sSkillFact;

			for ( DWORD i=0; i<dwNum; ++i )
			{
				ByteStream.ReadBuffer ( (LPBYTE)&sSkillFact, (DWORD)sizeof(SCHARSAVESKILLFACT_102) );	
				m_sSAVESKILLFACT[i].Assign( sSkillFact );
			}
		}
		break;
	case 0x0103:
		{
			ByteStream >> dwSize;
			ByteStream >> dwNum;

			GASSERT ( dwSize==sizeof(SCHARSAVESKILLFACT_103) );

			SCHARSAVESKILLFACT_103 sSkillFact;

			for ( DWORD i=0; i<dwNum; ++i )
			{
				ByteStream.ReadBuffer ( (LPBYTE)&sSkillFact, (DWORD)sizeof(SCHARSAVESKILLFACT_103) );	
				m_sSAVESKILLFACT[i].Assign( sSkillFact );
			}
		}
		break;

	case SCHARSAVESKILLFACT::VERSION:
		{
			ByteStream >> dwSize;
			ByteStream >> dwNum;

			GASSERT ( dwSize==sizeof(SCHARSAVESKILLFACT) );

			ByteStream.ReadBuffer ( (LPBYTE)m_sSAVESKILLFACT, (DWORD)sizeof(SCHARSAVESKILLFACT)*dwNum );	
		}
		break;
	};

	return TRUE;
}

DWORD SCHARDATA2::GetSkillFactNum()
{
	DWORD		dwNum = 0;
	for( DWORD i = 0; i < SKILLFACT_SIZE; ++i )
	{
		if( m_sSAVESKILLFACT[i].sNATIVEID == NATIVEID_NULL() )
			continue;
		++dwNum;
	}

	return dwNum;
}

DWORD SCHARDATA2::GetSkillFactDataSize()
{
	DWORD		dwDataSize = 0;

	dwDataSize += sizeof( SCHARSAVESKILLFACT::VERSION );
	dwDataSize += sizeof( DWORD );			//sizeof(SCHARSAVESKILLFACT);
	dwDataSize += sizeof( DWORD );			//GetSkillFactNum();
	dwDataSize += sizeof( SCHARSAVESKILLFACT ) * GetSkillFactNum();

	return dwDataSize;
}

BOOL SCHARDATA2::GetSkillFactData( BYTE* pDest, const DWORD dwDestSize )
{
	se::SerializeWritableData	data( pDest, dwDestSize );

	data << SCHARSAVESKILLFACT::VERSION
		 << (DWORD)sizeof(SCHARSAVESKILLFACT)
		 << GetSkillFactNum();

	for( int i = 0; i < SKILLFACT_SIZE; ++i )
	{
		if( m_sSAVESKILLFACT[i].sNATIVEID == NATIVEID_NULL() )
			continue;

		data.Write( &m_sSAVESKILLFACT[i], sizeof( SCHARSAVESKILLFACT ) );
	}

	return TRUE;
}

BOOL SCHARDATA2::GETSKILLFACT_BYBUF( se::ByteStream &ByteStream ) const
{
	ByteStream.ClearBuffer ();

	ByteStream << SCHARSAVESKILLFACT::VERSION;
	ByteStream << (DWORD)sizeof(SCHARSAVESKILLFACT);

	DWORD dwSize = 0;

	for ( WORD i=0; i<SKILLFACT_SIZE; ++i )
	{
		if ( m_sSAVESKILLFACT[i].sNATIVEID == NATIVEID_NULL() )
		{
			continue;
		}

		++dwSize;
	}

	ByteStream << dwSize;

	DWORD nIndex = 0;

	for ( WORD i=0; i<SKILLFACT_SIZE; ++i )
	{
		if ( m_sSAVESKILLFACT[i].sNATIVEID == NATIVEID_NULL() )
		{
			continue;
		}

		ByteStream.WriteBuffer ( (LPBYTE)&m_sSAVESKILLFACT[nIndex++], (DWORD)sizeof(SCHARSAVESKILLFACT) );

		if ( nIndex >= dwSize )
		{
			break;
		}
	}

	return TRUE;
}

void SCHARDATA2::SetStorageMoney( LONGLONG llMoney )
{
	m_bStorageMoneyUpdate = TRUE;

	if ( llMoney >= 0 )
	{
		m_lnStorageMoney = llMoney;
	}

	if ( m_lnStorageMoney < 0 )
	{
		m_lnStorageMoney = 0;
	}
}

void SCHARDATA2::AddStorageMoney( LONGLONG llMoney )
{
	m_bStorageMoneyUpdate = TRUE;

	if ( llMoney >= 0 )
	{
		m_lnStorageMoney = m_lnStorageMoney + llMoney;
	}

	if ( m_lnStorageMoney < 0 )
	{
		m_lnStorageMoney = 0;
	}
}

void SCHARDATA2::SubtractStorageMoney( LONGLONG llMoney )
{
	m_bStorageMoneyUpdate = TRUE;

	if ( llMoney >= 0 )
	{
		m_lnStorageMoney = m_lnStorageMoney - llMoney;
	}

	if ( m_lnStorageMoney < 0 )
	{
		m_lnStorageMoney = 0;
	}
}

void SCHARDATA2::SetTempStorageMoney( LONGLONG llMoney )
{
	if ( llMoney >= 0 )
	{
		m_lnTempStorageMoney = llMoney;
	}

	if ( m_lnTempStorageMoney < 0 )
	{
		m_lnTempStorageMoney = 0;
	}
}

void SCHARDATA2::ClearSwapInventory()
{
	for( int i = 0; i < EInvenTotalSlot; i++ )
	{
		m_sSwapInventory[ i ].nOrigin = -1;;
		m_sSwapInventory[ i ].nTarget = -1;;
	}
}

BOOL SCHARDATA2::SETSHOPPURCHASE(VECSHOP& vecSHOP)
{
	m_mapCharged.clear();
	size_t dwSIZE = vecSHOP.size();
	for (size_t i=0; i<dwSIZE; ++i)
	{
		const SHOPPURCHASE& sPURCHASE = vecSHOP[i];
		m_mapCharged.insert(MAPSHOP_VALUE(sPURCHASE.strPurKey, sPURCHASE));
	}

	return TRUE;
}

BOOL SCHARDATA2::ADDSHOPPURCHASE ( const char* szPurKey, SNATIVEID nidITEM )
{
	SITEMCUSTOM sCUSTOM(nidITEM);

	const SITEM* pITEM_DATA = GLogicData::GetInstance().GetItem ( nidITEM );
	if (!pITEM_DATA)
        return FALSE;

	sCUSTOM.wTurnNum = pITEM_DATA->GETAPPLYNUM();

	sCUSTOM.cDAMAGE = (BYTE)pITEM_DATA->sBasicOp.wGradeAttack;
	sCUSTOM.cDEFENSE = (BYTE)pITEM_DATA->sBasicOp.wGradeDefense;

	WORD wPosX(0), wPosY(0);
	m_cInvenCharged.FindInsertable ( wPosX, wPosY );
	BOOL bOK = m_cInvenCharged.InsertItem ( sCUSTOM, wPosX, wPosY );
	if ( !bOK )
	{
		return FALSE;
	}

	SNATIVEID nid(wPosX,wPosY);
	m_mapChargedKey.insert ( std::make_pair(nid.dwID,szPurKey) );

	return TRUE;
}

BOOL SCHARDATA2::DELSHOPPURCHASE ( const DWORD dwID )
{
	SNATIVEID nidPOS(dwID);
	BOOL bOK = m_cInvenCharged.DeleteItem ( nidPOS.wMainID, nidPOS.wSubID );
	if ( !bOK )		return FALSE;

	MAPSHOP_KEY_ITER iter = m_mapChargedKey.find ( dwID );
	if( iter==m_mapChargedKey.end() )	return FALSE;
	m_mapChargedKey.erase ( iter );

	return TRUE;
}

//! ITEMREBUILD_MARK
SITEMCUSTOM SCHARDATA2::GET_REBUILD_ITEM() const
{
	SITEMCUSTOM sItemCustom(NATIVEID_NULL());

	if (!m_sRebuildItem.VALID())
		return sItemCustom;

	const SINVENITEM* pResistItem = m_cInventory.GetItem(m_sRebuildItem.wPosX, m_sRebuildItem.wPosY);
	if (!pResistItem)
		return sItemCustom;

	sItemCustom = pResistItem->sItemCustom;
	return sItemCustom;
}

SITEMCUSTOM SCHARDATA2::GET_REBUILD_ITEM_BACKUP() const
{
	return m_sBackUpRebuildInvenItem.sItemCustom;
}

SITEMCUSTOM SCHARDATA2::GET_STAMP_ITEM() const
{
	SITEMCUSTOM sItemCustom(NATIVEID_NULL());

	if (!m_sStampItem.VALID())
		return sItemCustom;

	const SINVENITEM* pResistItem = m_cInventory.GetItem(m_sStampItem.wPosX, m_sStampItem.wPosY);
	if (!pResistItem)
		return sItemCustom;

	sItemCustom = pResistItem->sItemCustom;
	return sItemCustom;
}

SITEMCUSTOM SCHARDATA2::GET_PREHOLD_ITEM() const
{
    SITEMCUSTOM sItemCustom(NATIVEID_NULL());

	if (!m_sPreInventoryItem.VALID())
		return sItemCustom;

	SINVENITEM* pResistItem = m_cInventory.GetItem(m_sPreInventoryItem.wPosX, m_sPreInventoryItem.wPosY);
	if (!pResistItem)
		return sItemCustom;

	sItemCustom = pResistItem->sItemCustom;
	return sItemCustom;
}

void SCHARDATA2::RESET_FIX_OPTION()
{
	m_fixOption.clear();
}

void SCHARDATA2::PUSH_FIX_OPTION(const BYTE cOptType, const DWORD nFixMaximum)
{
	m_fixOption.push(cOptType, nFixMaximum);
}

const BYTE SCHARDATA2::GET_FIX_OPTION_TYPE ( const UINT _nIndex ) const
{
	return m_fixOption.GetOptType( _nIndex );
}

const DWORD SCHARDATA2::GetNFixOption(void) const
{
	return m_fixOption.getNSize();
}
VOID SCHARDATA2::InitRebuildData()
{
	m_wRebuildType = EMREBUILDTYPE::EMREBUILDTYPE_NONE;
	m_sRebuildItem.RESET();
	m_sStampItem.RESET();
	m_sPreInventoryItem.RESET();
	m_fixOption.clear();
	m_i64RebuildCost = 0;
	m_i64RebuildInput = 0;

	m_sBackUpRebuildInvenItem = SINVENITEM();
}

void SCHARDATA2::InitCostumStatData()
{
	m_sPreInventoryItem.RESET();
}

void SCHARDATA2::SetTexasHoldemChip( LONGLONG chip )
{
	if ( chip >= 0 )
	{
		m_TexasHoldemChip = chip;
	}

	if ( m_TexasHoldemChip < 0 )
	{
		m_TexasHoldemChip = 0;
	}
}

void SCHARDATA2::AddTexasHoldemChip( LONGLONG chip )
{
	if ( chip >= 0 )
	{
		m_TexasHoldemChip = m_TexasHoldemChip + chip;
	}

	if ( m_TexasHoldemChip < 0 )
	{
		m_TexasHoldemChip = 0;
	}
}

void SCHARDATA2::SubtractTexasHoldemChip( LONGLONG chip )
{
	if ( chip >= 0 )
	{
		m_TexasHoldemChip = m_TexasHoldemChip - chip;
	}

	if ( m_TexasHoldemChip < 0 )
	{
		m_TexasHoldemChip = 0;
	}
}

void SCHARDATA2::SetConsignmentSaleSlotExpansionDate( __time64_t tTime )
{
    m_ConfignmentSaleSlotExpansionDate = tTime;
}

void SCHARDATA2::SETPUTONITEM( EMSLOT emSlot, const SITEMCUSTOM& sItem )
{
	if ( emSlot >= 0 && emSlot < SLOT_TSIZE )
	{
		m_PutOnItems[emSlot] = sItem;
	}
}

BOOL SCHARDATA2::SETINVENTORY_BYVECTOR( std::vector< SINVENITEM_SAVE >& vecItems )
{
	GLInventory cInven;
	BOOL bOk = cInven.SET_ITEM_BY_VECTOR( vecItems );
	if ( !bOk )
		return TRUE;

	CalcPREMIUM();
	m_cInventory.SetAddLine( GetOnINVENLINE(), true );

	WORD wSIZEX = cInven.GetSizeX();
	WORD wSIZEY = cInven.GetSizeY();
	if ( wSIZEX == EInvenX && wSIZEY == EInvenTotalLine )
	{
		m_cInventory.Assign( cInven );
	}
	else
	{
		m_cInventory.SetState( EInvenX, EInvenTotalLine );

		const GLInventory::CELL_MAP* pmapITEM = cInven.GetItemList();
		GLInventory::CELL_MAP_CITER pos = pmapITEM->begin();
		GLInventory::CELL_MAP_CITER end = pmapITEM->end();
		for ( ; pos!=end; ++pos )
		{
			m_cInventory.InsertItem ( (*pos).second->sItemCustom );
		}
	}

	return TRUE;
}

BOOL SCHARDATA2::SETSTORAGE_BYVECTOR( std::vector< SINVENITEM_SAVE >& vecItems )
{
	m_bServerStorage = TRUE;

	for ( size_t loop = 0; loop < EMSTORAGE_CHANNEL; loop++ )
	{
		m_cStorage[loop].DeleteItemAll();
	}

	GASSERT( EMSTORAGE_CHANNEL == 5 );

	size_t ItemSize = vecItems.size();
	for ( size_t loop = 0; loop < ItemSize; loop++ )
	{
		SINVENITEM_SAVE& sItem = vecItems[loop];
		if ( sItem.wPosX >= 0 && sItem.wPosX < EStorageX )
		{
			m_cStorage[0].InsertItem( sItem.sItemCustom, sItem.wPosX, sItem.wPosY, true );
		}
		else if ( sItem.wPosX >= EStorageX && sItem.wPosX < EStorageX*2 )
		{
			sItem.wPosX = sItem.wPosX - EStorageX;
			m_cStorage[1].InsertItem( sItem.sItemCustom, sItem.wPosX, sItem.wPosY, true );
		}
		else if ( sItem.wPosX >= EStorageX*2 && sItem.wPosX < EStorageX*3 )
		{
			sItem.wPosX = sItem.wPosX - ( EStorageX*2 );
			m_cStorage[2].InsertItem( sItem.sItemCustom, sItem.wPosX, sItem.wPosY, true );
		}
		else if ( sItem.wPosX >= EStorageX*3 && sItem.wPosX < EStorageX*4 )
		{
			sItem.wPosX = sItem.wPosX - ( EStorageX*3 );
			m_cStorage[3].InsertItem( sItem.sItemCustom, sItem.wPosX, sItem.wPosY, true );
		}
		else if ( sItem.wPosX >= EStorageX*4 && sItem.wPosX < EStorageX*5 )
		{
			sItem.wPosX = sItem.wPosX - ( EStorageX*4 );
			m_cStorage[4].InsertItem( sItem.sItemCustom, sItem.wPosX, sItem.wPosY, true );
		}
	}

	return TRUE;
}

BOOL SCHARDATA2::GETSTORAGE_BYVECTOR( std::vector< SINVENITEM_SAVE >& vecItems )
{
	GASSERT( EMSTORAGE_CHANNEL == 5 );

	for ( size_t x = 0; x < EMSTORAGE_CHANNEL; x++ )
	{
		std::vector< SINVENITEM_SAVE > vecItemsTemp;
		GLInventoryStorage& Inven = m_cStorage[x];
		Inven.GET_ITEM_BY_VECTOR( vecItemsTemp );

		size_t ItemSize = vecItemsTemp.size();
		for ( size_t loop = 0; loop < ItemSize; loop++ )
		{
			SINVENITEM_SAVE& sItem = vecItemsTemp[loop];
			if ( 1 == x )
			{
				sItem.wPosX = sItem.wPosX + EStorageX;
			}
			else if ( 2 == x )
			{
				sItem.wPosX = sItem.wPosX + ( EStorageX*2 );
			}
			else if ( 3 == x )
			{
				sItem.wPosX = sItem.wPosX + ( EStorageX*3 );
			}
			else if ( 4 == x )
			{
				sItem.wPosX = sItem.wPosX + ( EStorageX*4 );
			}
			vecItems.push_back( vecItemsTemp[loop] );
		}
	}

	return TRUE;
}

BOOL SCHARDATA2::GETPUTONITEMS_BYVECTOR( std::vector< SINVENITEM_SAVE >& vecItems )
{
	for( int i = 0; i < SLOT_TSIZE; i++ )
	{
		if ( m_PutOnItems[i].GetNativeID() != NATIVEID_NULL() )
		{
			SINVENITEM_SAVE sItem( m_PutOnItems[i], i, 0 );
			vecItems.push_back( sItem );
		}
	}
	return TRUE;
}

BOOL SSKILLFACT::IsHaveSPECADDON(EMSPEC_ADDON emSPECADDON)
{
	const size_t nSPEC = vecSPEC.size();
	for ( DWORD _i = nSPEC; _i; --_i )
	{
		if ( vecSPEC[nSPEC - _i].emSPEC == emSPECADDON )
			return TRUE;
	}
	return FALSE;
}

void SSKILLFACT::RESET ()
{
	sNATIVEID    = NATIVEID_NULL();
	wLEVEL       = 0;
	fAGE         = 0.0f;
    nEXPIREDTIME = 0;
	emTYPE	     = SKILL::EMFOR_VARHP;
	fMVAR	     = 0.0f;	

	sID.RESET();

	vecADDON.clear();
	vecSPEC.clear();	

	dwSpecialSkill = 0;
	bRanderSpecialEffect = FALSE;

	bEnable = FALSE;
	bSkillUserEnermy = FALSE;

	pcCAData.AllReset();
}


SSKILLFACT::STIME::STIME( const SSKILLFACT& sSkillFact )
{
    if ( sSkillFact.nEXPIREDTIME )
    {
        IsEXPIREDTIME = true;
        nEXPIREDTIME  = sSkillFact.nEXPIREDTIME;
    }
    else
    {
        IsEXPIREDTIME = false;
        fAGE          = sSkillFact.fAGE;
    }

	fLIFE = _SKILLFACT_DEFAULTTIME;
}

SSKILLFACT::STIME::STIME( const SDROP_SKILLFACT& sDropSkill )
{
    if ( sDropSkill.IsEXPIREDTIME )
    {
        IsEXPIREDTIME = true;
        nEXPIREDTIME  = sDropSkill.nEXPIREDTIME;
    }
    else
    {
        IsEXPIREDTIME = false;
        fAGE          = sDropSkill.fAGE;
    }

	fLIFE = _SKILLFACT_DEFAULTTIME;
}

SSKILLFACT::STIME::STIME( const SCHARSAVESKILLFACT& sSaveSkill )
{
    if ( sSaveSkill.IsEXPIREDTIME )
    {
        IsEXPIREDTIME = true;
        nEXPIREDTIME  = sSaveSkill.nEXPIREDTIME;
    }
    else
    {
        IsEXPIREDTIME = false;
        fAGE          = sSaveSkill.fAGE;
    }
	
	fLIFE = _SKILLFACT_DEFAULTTIME;
}

BOOL SSKILLFACT::IsAlive()
{
    if ( IsHaveExpiredTime() )
    {
        __time64_t tTime = CTime::GetCurrentTime().GetTime();

        if ( nEXPIREDTIME <= static_cast<UINT>(tTime) )
        {
            return FALSE;
        }
    }
	else if ( fAGE <= 0.0f )
	{
		return FALSE;
	}

    return TRUE;
}

void SSKILLFACT::GetTimeByStringFormat(CString& strDest, __time64_t tCurrentTime, const SSKILLFACT::STIME sTime, const EMTIMESTRINGFORMAT emTimeType)
{
    if ( !sTime.IsHaveExpiredTime() && sTime.IsAgeInfinite() )
    {
        strDest = ID2GAMEINTEXT("SKILLFACT_LIFETIME_DISPLAY", 3);
        return;
    }

    INT  nDay      = 0;
	INT  nHour     = 0;
	INT  nMin      = 0;
	INT  nSec      = 0;
    BOOL bNegative = FALSE;
    BOOL bExpired  = FALSE;

    if ( sTime.IsHaveExpiredTime() )
    {
        __time64_t tExpiredTime = static_cast<__time64_t>(sTime.nEXPIREDTIME);
        
        if ( tExpiredTime < tCurrentTime )
        {
            strDest = ID2GAMEINTEXT("SKILLFACT_LIFETIME_DISPLAY", 5);
            return;
        }

        CTimeSpan tTimeSpan( tExpiredTime - tCurrentTime );

        nDay  = static_cast<INT>(tTimeSpan.GetDays());
        nHour = tTimeSpan.GetHours();
        nMin  = tTimeSpan.GetMinutes();
        nSec  = tTimeSpan.GetSeconds();

        bExpired = TRUE;
    }
    else
    {
        INT nTime = (INT)sTime.fAGE;

	    if ( nTime < 0 )
	    {
		    nTime = -nTime;
		    bNegative = TRUE;
	    }

        if ( nTime >= 86400 )
        {
            nDay = nTime/86400;
            nTime%=86400;
        }

	    if ( nTime >= 3600 )
	    {
		    nHour = nTime/3600;
		    nTime%=3600; 
	    }

	    if ( nTime >= 60 )
	    {
		    nMin = nTime/60;
		    nTime%=60; 
	    }

        nSec = nTime;
    }

	strDest = "";

	switch ( emTimeType )
	{
	case EMTIMESTRINGFORMAT_DETAIL:
        if (nDay)
            strDest.Append(sc::string::format(ID2GAMEINTEXT("SKILLFACT_LIFETIME_DISPLAY", 4), nDay).c_str());
        if (nHour)
            strDest.Append(sc::string::format(ID2GAMEINTEXT("SKILLFACT_LIFETIME_DISPLAY", 0), nHour).c_str());
		if (nMin)
            strDest.Append(sc::string::format(ID2GAMEINTEXT("SKILLFACT_LIFETIME_DISPLAY", 1), nMin).c_str());
		if (nSec || (!nHour && !nMin))
            strDest.Append(sc::string::format(ID2GAMEINTEXT("SKILLFACT_LIFETIME_DISPLAY", 2), nSec).c_str());
		break;

    case EMTIMESTRINGFORMAT_SIMPLE:
        if (nDay)
            strDest.Append(sc::string::format(ID2GAMEINTEXT("SKILLFACT_LIFETIME_DISPLAY", 4), nDay).c_str());
		if (nHour && !nDay)
            strDest.Append(sc::string::format(ID2GAMEINTEXT("SKILLFACT_LIFETIME_DISPLAY", 0), nHour).c_str());
		if (nMin && !nHour && !nDay)
            strDest.Append(sc::string::format(ID2GAMEINTEXT("SKILLFACT_LIFETIME_DISPLAY", 1), nMin).c_str());
		if (!nHour && !nDay && ! nMin)
            strDest.Append(sc::string::format( ID2GAMEINTEXT("SKILLFACT_LIFETIME_DISPLAY", 2), nSec).c_str());
		break;

	case EMTIMESTRINGFORMAT_SMART:
        if (nDay)
            strDest.Append(sc::string::format( ID2GAMEINTEXT("SKILLFACT_LIFETIME_DISPLAY", 4), nDay).c_str());
		if (nHour)
            strDest.Append(sc::string::format( ID2GAMEINTEXT("SKILLFACT_LIFETIME_DISPLAY", 0), nHour).c_str());
		if (nMin && (!nDay))
            strDest.Append(sc::string::format( ID2GAMEINTEXT("SKILLFACT_LIFETIME_DISPLAY", 1), nMin).c_str());
		if ((nSec && nMin && !nDay) || (!nDay && !nHour && !nMin))
            strDest.Append(sc::string::format( ID2GAMEINTEXT("SKILLFACT_LIFETIME_DISPLAY", 2), nSec).c_str());
		break;
	}

	if (bNegative)
	{
		strDest = "-" + strDest;
	}

    if ( bExpired )
    {
        strDest.Append( ID2GAMEINTEXT("SKILLFACT_LIFETIME_DISPLAY", 6) );
    }
}

bool SSKILLFACT::GetTime( int& nDay, int& nHour, int& nMin, int& nSec, __time64_t tCurrentTime, 
						  const SSKILLFACT::STIME sTime, const EMTIMESTRINGFORMAT emTimeType )
{
	if( FALSE == sTime.IsHaveExpiredTime() && TRUE == sTime.IsAgeInfinite() )
		return false;

	if( TRUE == sTime.IsHaveExpiredTime() )
	{
		__time64_t tExpiredTime = static_cast< __time64_t >( sTime.nEXPIREDTIME );

		if( tExpiredTime < tCurrentTime )
			return false;

		CTimeSpan tTimeSpan( tExpiredTime - tCurrentTime );
		nDay  = static_cast< int >( tTimeSpan.GetDays() );
		nHour = tTimeSpan.GetHours();
		nMin  = tTimeSpan.GetMinutes();
		nSec  = tTimeSpan.GetSeconds();
	}
	else
	{
		int nTime = (int) sTime.fAGE;

		if( nTime < 0 )
			nTime = -nTime;

		if( nTime >= 86400 )
		{
			nDay = nTime / 86400;
			nTime %= 86400;
		}

		if( nTime >= 3600 )
		{
			nHour = nTime / 3600;
			nTime %= 3600; 
		}

		if( nTime >= 60 )
		{
			nMin = nTime / 60;
			nTime %= 60; 
		}

		nSec = nTime;
	}

	return true;
}