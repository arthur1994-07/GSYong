#pragma once

#define RF_SCREEN_THROW( var )  var.m_sScreenThrow
#define RF_ENTRANCE_EXIT( var ) var.m_sEntranceExit
#define RF_FEIGN_DEATH( var )   var.m_sFeignDeath
#define RF_CAMERA_HIT( var )	var.m_sCameraHit
#define RF_DISGUISE( var )		var.m_sDisguise
#define RF_PROVOKE( var )		var.m_sProvoke
#define RF_IMMUNE( var )		var.m_sSpImmune
#define RF_ACTIVE( var )		var.m_sActive

struct GLCHARLOGIC;

namespace NSSKILL_EFFSET
{
	enum
	{
		// 일단 요기에  GLCONST_CHAR의 값을 사용할수 없음. GLSummon.h 가 GLogicData.h에 포함되어 있다. GLSummon.h에는 이 파일을 포함시켜 놨다. GLogicData.h를 이 파일은 인식할수 없다..빌드시....
		// 추후 수정
		SCREEN_EFFECT_NUM = 3,
	};

/*== EMSPECA_SCREEN_THROW ==============================*/
typedef struct _sScreenThrow
{
	struct sInfo
	{
		BOOL		bEffID;
		STARGETID	sTagetID;

		sInfo() : bEffID( FALSE ) {}
		void Reset() { bEffID = FALSE; sTagetID.RESET(); }
	};

	sInfo		sEffInfo[SCREEN_EFFECT_NUM];

	_sScreenThrow();

public:
	void ResetValue(WORD wEffID);
	void Reset();

public :
	void		TurnOn( const STARGETID& sID, WORD wEffID );
	BOOL		IsOn(WORD wEffID);
	STARGETID&	GetTargetID( WORD wEffID );

}SSCREEN_THROW;

/*== EMSPECA_ENTRANCE_EXIT ==============================*/
typedef struct _sEntranceExit
{
	BOOL	bOn;

	_sEntranceExit() : bOn( FALSE ) {}
	_sEntranceExit( BOOL bSwitch ) : bOn( bSwitch ) {}
	_sEntranceExit( const _sEntranceExit& sData ) { bOn = sData.bOn; }

public :
	void ResetValue() { bOn = FALSE; }
	void TurnOn() { bOn = TRUE; }
	BOOL IsOn() { return bOn; }

}ENTRANCE_EXIT;

/*== EMSPECA_FEIGN_DEATH ==============================*/
typedef struct _sFeignDeath
{
	BOOL	bOn;
	float   fRateValue;
	BOOL	bSelectTargetDisable;
	BOOL    bDelBufDisable;
	int		iSlotIndex;

	_sFeignDeath() : bOn( FALSE ), fRateValue( 1.f ), bSelectTargetDisable( FALSE ), bDelBufDisable( FALSE ), iSlotIndex( -1 ) {}
	_sFeignDeath( BOOL bSwitch, BOOL bTarget, BOOL bDelBuf, float fValue, int iSlotNum ) : bOn( bSwitch ), bSelectTargetDisable( FALSE ), bDelBufDisable( FALSE ), fRateValue( fValue ), iSlotIndex( iSlotNum ) {}
	_sFeignDeath( const _sFeignDeath& sData ) { bOn = sData.bOn; bSelectTargetDisable = sData.bSelectTargetDisable; bDelBufDisable = sData.bDelBufDisable; fRateValue = sData.fRateValue; iSlotIndex = sData.iSlotIndex; }

public :
	void ResetValue() { bOn = FALSE; bSelectTargetDisable = FALSE; bDelBufDisable = FALSE; fRateValue = 1.f; iSlotIndex = -1; }
	void TurnOn( BOOL bTarEnable, BOOL bDBuf, int iSlotNum ) { bOn = TRUE; bSelectTargetDisable = bTarEnable; bDelBufDisable = bDBuf; fRateValue = 0.f; iSlotIndex = iSlotNum; }
	void End( GLCHARLOGIC* const pCharLogic, BOOL bForceEnd );
	BOOL  IsOn() { return bOn; }
	const BOOL IsSelectDisable() const { return bSelectTargetDisable; }
	const BOOL IsBufDelDisalbe() const { return bDelBufDisable;       }
	int	  GetSlotIdx() { return iSlotIndex; }

}FEIGN_DEATH;

/*== EMSPECA_CAMERA_HIT ==============================*/
typedef struct _sCameraHit
{
	BOOL  bOn;
	WORD  wEffNum;

	_sCameraHit() : bOn( FALSE ), wEffNum( 0xffff ){}
	_sCameraHit( BOOL bSwitch, WORD wNum ) : bOn( bSwitch ), wEffNum( wNum ) {}
	_sCameraHit( const _sCameraHit& sData ) { bOn = sData.bOn; wEffNum = sData.wEffNum; }
	
public :
	void  TurnOn( WORD wNum ) { bOn = TRUE; wEffNum = wNum; }
	BOOL  IsOn() { return bOn; }
	WORD  GetEffNum() { return wEffNum; }

	void ResetValue() { bOn = FALSE; wEffNum = 0xffff; }

}CAMERA_HIT;

/*== EMSPECA_DISGUISE ==============================*/
typedef struct _sDisguise
{
	BOOL	bOn;
	UINT	iIndex;
	DWORD	dwTableID;

	_sDisguise() : bOn( FALSE ), iIndex( 0 ), dwTableID( 0 ) {}
	_sDisguise( BOOL bSwitch, UINT iIdx, DWORD dwID ) : bOn( bSwitch ), iIndex( iIdx ), dwTableID( dwID ) {}
	_sDisguise( const _sDisguise& sData ) { bOn = sData.bOn; iIndex = sData.iIndex; dwTableID = sData.dwTableID;  }

public :
	void ResetValue() { bOn = FALSE; iIndex = 0; dwTableID = 0; }
	void TurnOn( int iIdx, DWORD dwID ) { bOn = TRUE; iIndex = iIdx; dwTableID = dwID; }
	BOOL IsOn() const { return bOn; }
	UINT GetIndex() { return iIndex; }
	DWORD GetTableID() { return dwTableID; }

}DISGUISE_EX;
/*======================================================**/

/*== EMSPECA_PROVOKE ====================================*/
#define NULL_DISTANCE 0xFFFF

typedef struct _sProvoke
{	
	BOOL			bOn;
	WORD			wEffDistance;	// 유효거리
	STARGETID		sID;			// 스킬을 건 주체자
	STARGETID		sPreTargetID;

	_sProvoke() : bOn( FALSE ), wEffDistance( NULL_DISTANCE ){}
	_sProvoke( BOOL bSwitch, STARGETID sOrID, STARGETID sPreID, WORD wDist ) : bOn( bSwitch ), sID( sOrID ), sPreTargetID( sPreID), wEffDistance( wDist ) {}
	_sProvoke( const _sProvoke& sData ) { bOn = sData.bOn; sID = sData.sID; wEffDistance = sData.wEffDistance; sPreTargetID = sData.sPreTargetID; }

public :
	void ResetValue() { bOn = FALSE; sID.RESET(); wEffDistance = NULL_DISTANCE; }
	void TurnOn( STARGETID sOrID, WORD wDist ) { bOn = TRUE; sID = sOrID; wEffDistance = wDist; }

	void SetPreTargetID( const STARGETID& sID ) { sPreTargetID = sID; }
	void ResetPreTarget() { sPreTargetID.RESET(); }

	BOOL IsOn() const { return bOn; }

	const STARGETID& GetHostID() { return sID; }
	const STARGETID& GetPreTargetID() { return sPreTargetID; }
	const WORD& GetEffDistance() const { return wEffDistance*10; }
	const WORD& GetEffDistOrig() const { return wEffDistance; }

}PROVOKE;
/*===============================================================*/

/*== EMSPECA_SPECIAL_IMMUINE ====================================*/
typedef struct _sSpecialImmune
{
	enum {
		IMMUNE_LIST_MAX = 5,
	};

	BOOL    bOn;
	
	typedef std::vector<BYTE>			vList;
	typedef vList::iterator				vList_iter;
	typedef vList::const_iterator		vList_const_iter;

	vList	vImmuneList;

	_sSpecialImmune() : bOn( FALSE ) { vImmuneList.reserve(IMMUNE_LIST_MAX); }
	_sSpecialImmune( const _sSpecialImmune& sData )  { bOn = sData.bOn; vImmuneList = sData.vImmuneList; }

public :
	void ResetValue() { bOn = FALSE; vImmuneList.clear(); }
	void TurnOn( BYTE iIndex );
	BOOL IsOn() const { return bOn; }
	BOOL IsImmune( BYTE iIndex ) const;

}SIMMUNE;
/*===============================================================*/

/*== EMSPECA_ACTIVATE_DAMAGE ====================================*/
typedef struct _sActiveDamage
{
	enum { 	ACTIVE_LIST_MAX = 5, };
	enum {
		VALUE_NONE  = 0x00,
		VALUE_OR	= 0x01,
		VALUE_BOTH	= 0x02,
	};

	/****************************************************************************************************************************************/
	struct sAccuInfo 
	{		
		int		iAccuCnt;
		int     iAccuDmg;

		WORD	wSkillLv;
		DWORD   dwSkillId;

		BYTE	iCheck;

		STARGETID	sID;
		
		sAccuInfo() : wSkillLv( 0 ), iAccuCnt( 0 ), iAccuDmg( 0 ), dwSkillId( 0 ), iCheck( VALUE_NONE ) {}
		sAccuInfo( DWORD id, WORD wLv, int cnt, int damage, BYTE iValue ) { dwSkillId = id; wSkillLv = wLv; iAccuCnt = cnt; iAccuDmg = damage; iCheck = iValue; sID.RESET(); }
		sAccuInfo( const sAccuInfo& sData ) { dwSkillId = sData.dwSkillId; wSkillLv = sData.wSkillLv; iAccuCnt = sData.iAccuCnt; iAccuDmg = sData.iAccuDmg; iCheck = sData.iCheck; sID = sData.sID; }
	};
	/****************************************************************************************************************************************/

	typedef std::vector< sAccuInfo > vList;
	typedef vList::iterator			 vList_iter;

	vList	vAccuList;

	BOOL    bOn;
	
	_sActiveDamage() : bOn( FALSE )  { vAccuList.reserve(ACTIVE_LIST_MAX); }
	_sActiveDamage( const _sActiveDamage& sData )  { bOn = sData.bOn; vAccuList = sData.vAccuList; }

public :
	void ResetValue( BOOL bAll = FALSE ) { bOn = FALSE; if( bAll ) vAccuList.clear(); }
	void TurnOn() { bOn = TRUE; }
	void TurnOn( const DWORD& idSkill, const WORD& wLv, const int& iCnt, const int& iDamage );

	BOOL IsOn() const { return bOn; }
	BOOL IsSkill( const DWORD& dwID );
	BOOL UpdateSkill( const DWORD& idSkill, const WORD& wLv, const int& iCnt, const int& iDamage );
	BOOL Invoke( WORD& wLv, DWORD& dwSkill, STARGETID& sAID );

	void Accumulate( DWORD dwDamage, const EMCROW eCrow, const DWORD dwID );
	void DeleteSkill( const DWORD& dwID );

}ACTIVEDAMAGE;
/*===============================================================*/

class CSkillEffSet
{
public :
	CSkillEffSet(){};
	~CSkillEffSet(){};

public :
	SSCREEN_THROW	m_sScreenThrow;
	ENTRANCE_EXIT	m_sEntranceExit;
	FEIGN_DEATH		m_sFeignDeath;
	CAMERA_HIT		m_sCameraHit;
	DISGUISE_EX		m_sDisguise;
	PROVOKE			m_sProvoke;
	SIMMUNE			m_sSpImmune;
	ACTIVEDAMAGE	m_sActive;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////
class CSkillEffCrowSet
{
public :
	CSkillEffCrowSet(){};
	~CSkillEffCrowSet(){};

public :
	ENTRANCE_EXIT	m_sEntranceExit;
	FEIGN_DEATH		m_sFeignDeath;
	PROVOKE			m_sProvoke;
	SIMMUNE			m_sSpImmune;
	ACTIVEDAMAGE	m_sActive;
};
//////////////////////////////////////////////////////////////////////////////////////////////////////
class CSkillEffSummonSet
{
public :
	CSkillEffSummonSet(){};
	~CSkillEffSummonSet(){};

public :
	PROVOKE			m_sProvoke;
	SIMMUNE			m_sSpImmune;
	ACTIVEDAMAGE	m_sActive;
};

};