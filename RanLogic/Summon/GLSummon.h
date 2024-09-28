#pragma once

#include "../Item/GLItem.h"
#include "../Character/GLCharData.h"
#include "../Crow/GLCrowData.h"
//#include "../GLogicData.h"
//#include "./GLContrlMsg.h"

#include "../Actor/GLActorDefine.h"

#include "../../EngineLib/G-Logic/GLDefine.h"
#include "../Skill/GLCountAttackSkill.h"
#include "../Skill/GLSkillEffVarSet.h"

using namespace NSSKILL_EFFSET;

enum EMSUMMONGESTURESTATE
{
	EM_SUMMONGESTURE_NONE   = -1,
	EM_SUMMONGESTURE_MAX    = 10,
};

struct GLSUMMON
{
	// Note : 외부 정보;
	SUMMON_TYPE		 m_emTYPE;					// 코드 AI Type ( Script 용 AI Type은 이 타입을 일반타입으로 두면 된다;
	SNATIVEID		 m_sSummonID;				// 소환할 Crow ID;
	
	DWORD			 m_dwOwnerGaeaID;			// 주인 GaeaID ( GaeaID는 따로 취급함 );
	Faction::GLActorID	m_sOwnerID;				// 주인의 ActorID;

	PCROWDATA		 m_pSummonCrowData;			// Crow 기본 정보;
	FLOAT            m_fRemainedTime;           // 소환 유지시간;

	float		     m_fOwnerIncHP;		        // 주인의 HP 회복율;
	WORD		     m_wOwnerAttack;            // 주인의 공격력;
	WORD		     m_wOwnerDefense;           // 주인의 방어력;
	WORD		     m_wOwnerSkillLevel;        // 주인의 스킬 레벨;
	BYTE             m_cOwnerWeaponsGrade;      // 주인의 무기 강화;
    ITEM::SSATE_BLOW m_sOwnerBlow;              // 주인의 무기 속성;
	GLITEM_ATT		 m_emOwnerItemAtt;		// 주인의 무기;
    WORD             m_wOwnerSchool;            // 주인의 학원;

	SKILL::GLSKILL_ATT m_emNeedItemAtt;			// 소환에 필요한 무기종류

	float			 m_fRunArea;                 // 달려야만 하는 거리;
	float			 m_fWalkArea;                // 걸어야만 하는 거리;
	float			 m_fOwnerDistance;           // 주인과의 유지 거리;
	float			 m_fOwnerDegree;             // 주인과의 유지 각도;
	float			 m_fPartyDistance;           // 파티 인식 거리;
	float			 m_fMoveRefreshDistance;     // 이동 갱신 거리;
	float			 m_fOwnerDamagePer;			 // 주인의 데미지 계승률;
	float			m_fOwnerMovePer;			// 주인의 이동속도 계승률;
    bool			 m_bOwnerSpeed;				 // 주인의 속도를 따라 가는지 여부;



	// Note : 소환수 개인 정보;
	DWORD				m_dwGlobalID;					// GLCrow 구조 따라감;
	float				m_fACTIONDELAY[SCROWDATA::EMMAXATTACK];	//	동작 cool time;

	SSKILLFACT			m_sSKILLFACT[CROW_SKILLFACT_SIZE];	//	스킬효과들.
	VEC_LANDEFF_VALUE	m_vecLandEffect;				//  지형 효과들
	VEC_MAP_EFFECT		m_vecMapEffect;					//	맵에 적용되는 효과들

	DWORD				m_dwHOLDBLOW;					//	상태이상이 방지됨.

	SSTATEBLOW			m_sSTATEBLOWS[EMBLOW_MULTI];	//	상태이상.
	float				m_fSTATE_MOVE_RATE;					//	상태 이상 이동속도. ( 1.0f == 100% )
	float				m_fSTATE_DELAY;					//	상태 이상 딜래이. ( 1.0f == 100% ) (+)
	float				m_fSTATE_DAMAGE;	 			//	상태 이상 대미지 증가.

	bool				m_bSkillFactsSpecial[EMSPECA_NSIZE];
	bool				m_bForcedSkillFactsSpecial[EMSPECA_NSIZE];
	bool				m_bSTATE_PANT;					//	상태이상 헐떡임.
	bool				m_bSTATE_STUN;					//	상태이상 기절.
	SINVISIBLE			m_sINVISIBLE;					// 은신;
	SRECVISIBLE			m_sRECVISIBLE;					// 은신감지;
	//bool				m_bINVISIBLE;					// 투명;
	//bool				m_bRECVISIBLE;					// 투명 확인;

	SHALLUCINATE		m_sHALLUCINATE;					// 환독 효과;
	SRELEASE_ENDURE		m_sRELEASE_ENDURE;				// 피격시 효과 해제;
	SDOMINATE			m_sDOMINATION;					// 지배 효과;
	SSTIGMA				m_sSTIGMA_USER;					// 낙인 효과 - 사용 ( 현재 캐릭터만이 사용 가능 );
	SSTIGMA				m_sSTIGMA_TAKER;				// 낙인 효과 - 적용;
	SINCREASEEFF		m_sIncreaseEff;					// 강화 효과;

    SLINKHP				m_sLINKHP;						// 고리효과;
	SIGNORED_DAMAGE		m_sIgnoredDamage;				// 데미지 흡수;
	SVIEWRANGERATE		m_sRateViewRange;				// 시야제한;
	SAIRBORNE			m_sAirborne;					// 에어본;
	SCLONE				m_sClone;						// 복제효과

	float				m_fATTVELO;						//	공격속도.
	float				m_fSKILL_MOVE;					//	스킬 이동속도.

	int					m_nSUM_HIT;						//	명중율 + 스킬효과.
	int					m_nSUM_AVOID;					//	회피율 + 스킬효과.

	int					m_nSUM_DEFENSE;					//	방어력 ( DP + POW*계수 + ITEM_DEF ) * 공간적응 보정 + 스킬.
	int					m_nDX_DAMAGE;					//	기본 공격력. + 스킬효과.

	int					m_nSUM_PIERCE;					//	추가 관통 정도.
	
	float				m_fDamageRate;					//	공격력 변화율.
	float				m_fDefenseRate;					//	방어력 변화율.

	SRESIST				m_sSUMRESIST;					//	저항값.

	DAMAGE_SPEC			m_sDamageSpec;					//	데미지 흡수 및 반사
	DEFENSE_SKILL		m_sDefenseSkill;				//  발동형 스킬
	SDURATION_DAMAGE	m_sDurDamage[EMELEMENT_MAXNUM];	//	지속 데미지

	DWORD				m_dwActionLimit;				//	행동제약 

	DWORD				m_dwAType;						//	공격 속성 순번.
	bool				m_bUsingEventSkill;				//	이벤트 스킬 사용.

	DWORD				m_dwNowHP;						//	생명량.
	WORD				m_wNowMP;						//	정신량.
	WORD				m_wNowSP;						//	정신량

	SNATIVEID			m_idACTIVESKILL;				//	활성 스킬.
	WORD				m_wACTIVESKILL_LVL;				//	활성 스킬  래벨.

	bool				m_bSafeZone;                    //  세이프 존 여부

	D3DXVECTOR3			m_RandPos[8];

    float               m_fVariationRateHP;             //  HP 회복률
    float               m_fVariationRateMP;             //  MP 회복률
    float               m_fVariationRateSP;             //  SP 회복률

    float               m_fVariationApplyRateHP;        // HP 변화 적용률;
    float               m_fVariationApplyRateMP;        // MP 변화 적용률;
    float               m_fVariationApplyRateSP;        // SP 변화 적용률;

	//	Note : 회복되는 수치 산출용 임시 변수.
	float				m_fTempVarHP;
	float				m_fTempVarMP;
	float				m_fTempVarSP;

    float               m_fMoveVeloLimit_Min;   // 최저 이동속도 제한;
    float               m_fMoveVeloLimit_Max;   // 최고 이동속도 제한;

	// Note : 소환수 제스쳐.
	EMSUMMONGESTURESTATE m_emGestureState;
	EMSUMMONGESTURESTATE m_emGestureableArray[EM_SUMMONGESTURE_MAX]; // 제스쳐 사용 가능한 배열  
	DWORD                m_dwGestureNum;                             // 제스쳐 갯수

	// Note : 현재 스피드
	float				m_fCurSpeed;

	// 이벤트 스킬 슬롯;
	DWORD				m_dwCurSkillPage;

	CSkillCAIdx			m_SkillCAIndex;			// 반격리스트

	CSkillEffSummonSet	m_EffSkillVarSet;

	// constructor & operator
	GLSUMMON () :
		m_emTYPE(SUMMON_TYPE_NONE),

		m_pSummonCrowData(NULL),

		m_sSummonID(NATIVEID_NULL()),

		m_dwOwnerGaeaID(0),

		m_fRemainedTime(0.0f),

		m_fOwnerIncHP(0.0f),
		m_wOwnerAttack(0),
		m_wOwnerDefense(0),
		m_wOwnerSkillLevel(0),
		m_cOwnerWeaponsGrade(0),
		m_dwGlobalID(GAEAID_NULL),
		m_sOwnerBlow(),
		m_emOwnerItemAtt(ITEMATT_NOTHING),

		m_dwHOLDBLOW(NULL),
		m_fSTATE_MOVE_RATE(1.0f),
		m_fSTATE_DELAY(1.0f),
		m_fSTATE_DAMAGE(1.0f),
        m_sLINKHP(),

		m_bSTATE_PANT(false),
		m_bSTATE_STUN(false),
		//m_bINVISIBLE(false),
		//m_bRECVISIBLE(false),

		m_fATTVELO(0.0f),
		m_fSKILL_MOVE(0.0f),

		m_nSUM_HIT(0),
		m_nSUM_AVOID(0),

		m_fVariationRateHP(0.0f),
        m_fVariationRateMP(0.0f),
        m_fVariationRateSP(0.0f),
        m_fVariationApplyRateHP(1.0f),
        m_fVariationApplyRateMP(1.0f),
        m_fVariationApplyRateSP(1.0f),
		m_nSUM_DEFENSE(0),
		m_nDX_DAMAGE(0),

		m_nSUM_PIERCE(0),

		m_fDamageRate(1.0f),
		m_fDefenseRate(1.0f),

		m_dwAType(0),
		m_bUsingEventSkill(false),

		m_dwNowHP(0),
		m_wNowMP(0),
		m_wNowSP(0),

		m_idACTIVESKILL(false),
		m_wACTIVESKILL_LVL(0),

		m_bSafeZone(false),

		m_fTempVarHP(0.0f),
		m_fTempVarMP(0.0f),
		m_fTempVarSP(0.0f),

		m_fRunArea(60.0f),
		m_fWalkArea(40.0f),
		m_fOwnerDistance(20.0f),
		m_fOwnerDegree(0.0f),
		m_fPartyDistance(100.0f),
		m_fMoveRefreshDistance(200.0f),
		m_fOwnerDamagePer(100.0f),
		m_fOwnerMovePer(1.f),
		m_bOwnerSpeed(false),
        m_wOwnerSchool(0),

		m_emGestureState(EM_SUMMONGESTURE_NONE),
		m_dwGestureNum(0),

		m_fCurSpeed (0.0f),
		m_dwCurSkillPage(0)

        , m_fMoveVeloLimit_Min(-1.0f)
        , m_fMoveVeloLimit_Max(1000.0f)
		, m_emNeedItemAtt( SKILL::SKILLATT_NOTHING )
	{
		for ( int i=0; i< SCROWDATA::EMMAXATTACK; ++i )
			m_fACTIONDELAY[i] = 0.0f;

		for ( int i=0; i<EM_SUMMONGESTURE_MAX; ++i )
			m_emGestureableArray[i] = EM_SUMMONGESTURE_NONE;

		m_RandPos[0] = D3DXVECTOR3(0,0,m_fOwnerDistance);			
		m_RandPos[1] = D3DXVECTOR3(0,0,-m_fOwnerDistance);
		m_RandPos[2] = D3DXVECTOR3(m_fOwnerDistance,0,0);
		m_RandPos[3] = D3DXVECTOR3(-m_fOwnerDistance,0,0);
		m_RandPos[4] = D3DXVECTOR3(cos(45.0f)*m_fOwnerDistance,0,cos(45.0f)*m_fOwnerDistance);
		m_RandPos[5] = D3DXVECTOR3(-cos(45.0f)*m_fOwnerDistance,0,cos(45.0f)*m_fOwnerDistance);
		m_RandPos[6] = D3DXVECTOR3(cos(45.0f)*m_fOwnerDistance,0,-cos(45.0f)*m_fOwnerDistance);
		m_RandPos[7] = D3DXVECTOR3(-cos(45.0f)*m_fOwnerDistance,0,-cos(45.0f)*m_fOwnerDistance);

		for (int i = 0; i < CROW_SKILLFACT_SIZE; i++ )
			RESETSKEFF( i );

		ZeroMemory(m_bSkillFactsSpecial, sizeof(m_bSkillFactsSpecial));
		ZeroMemory(m_bForcedSkillFactsSpecial, sizeof(m_bForcedSkillFactsSpecial));
		
	}
	GLSUMMON ( const GLSUMMON& value ) { operator=( value ); }
	GLSUMMON& operator= ( const GLSUMMON& rvalue );
    virtual ~GLSUMMON() {}

	void RESET_DATA();
	bool VALIDACTION();
	bool VALIDSKILL();
	bool VALIDATTACK();

	void ASSIGN(const GLSUMMON& sSummonData);
	void RESET();

	const SUMMON_TYPE GETSUMMONTYPE() const;
	WORD GETSUMMON_BODYRADIUS() const;
	WORD GETSUMMON_ATTACKRANGE() const;

	EMCROW GETCROW() const;
	WORD GETLEVEL() const;
	const char* GETNAME();

	DWORD GETHP() const;
	DWORD GETMAXHP() const;
    float GETMAXHPRATE() const;
    void  SETMAXHPRATE( float fRate );

	int GETMP() const;
	int GETMAXMP() const;

	int GETSP() const;
	int GETMAXSP() const;

	int GETHIT() const;
	int GETAVOID() const;

	EMBRIGHT GETBRIGHT() const;

	int GETDEFENSE() const;

	int GETSUM_PIERCE() const;

	WORD GETBODYRADIUS() const;
	WORD GETATTACKRANGE() const;

	WORD GETSKILLRANGE_TAR ( const GLSKILL &sSKILL ) const;
	WORD GETSKILLRANGE_APPLY ( const GLSKILL &sSKILL, const WORD dwLEVEL ) const;

	const int GETMAXVIEWRANGE(void) const;
	const float GETMAXVIEWRANGE_RATE(void) const { return m_sRateViewRange.fRate; }

	CSkillCAIdx* GetSkillCA() { return &m_SkillCAIndex; }

	void		 GetEffList( DWORD dwSlot, BYTE* pSSPECA, BYTE* pSIMA );

	void Update_AccumulateHit( DWORD dwDamage, const EMCROW eCrow, const DWORD dwID );

	BOOL INIT_DATA ();
	
	/*bool CALCDAMAGE ( int& rResultDAMAGE, BOOL &bShock, const STARGETID &cTargetID, const GLLandMan* pLandMan,
	const GLSKILL* pSkill=NULL, DWORD dwskill_lev=0, DWORD dwWeatherFlag=NULL, DWORD dwDivCount=0 );*/

	DWORD RECEIVE_DAMAGE(const WORD wDamage);
	void RELEASE_ENDURE_PROC(void);
	DWORD RECEIVE_HEAL(const int nHeal);

	void RESETSKEFF(size_t i)
    { 
        if (i < CROW_SKILLFACT_SIZE)
            m_sSKILLFACT[i].sNATIVEID = NATIVEID_NULL();
    }
	void DISABLEBLOW ( int i ) { m_sSTATEBLOWS[i].emBLOW = EMBLOW_NONE; }

	void PrevUPDATE_DATA ( const float fElapsedTime );
	void UPDATE_DATA ( float fTime, float fElapsedTime, BOOL bClient=FALSE );

	virtual void UPDATE_DATA_END ( const DWORD dwSkillfactIndex, BOOL bTimeDel = FALSE ) { }
	virtual void UPDATE_DATA_Additional ( const float fElapsedTime );

	void SET_IGNORED_DAMAGE(const int nIgnoreCount, const int nIgnoreDamage);

	void         SETACTIVESKILL ( SNATIVEID skill_id, WORD wLevel );
	SNATIVEID    GETACTIVESKILL() { return m_idACTIVESKILL; }
	EMSKILLCHECK CHECHSKILL ( DWORD dwAType, bool bNotLearn = false, bool bEventSkill = false );
	void         ACCOUNTSKILL ( WORD wStrikeNum );

	virtual float GETATTVELO();
	virtual float GETMOVEVELO();

	inline bool IsCheckedSkillFlagSpecial(const DWORD dwFlag) const { return m_bSkillFactsSpecial[dwFlag]; }

	void RemoveSkillFactBySkillID(const SNATIVEID& skill_id);
	void RemoveSkillFactBySpecial(const EMSPEC_ADDON emSPEC_ADDON);
	void RemoveSkillFactofSpecial(const SNATIVEID& skill_id, const EMSPEC_ADDON* emSPEC_ADDON);

	// Note : 지형효과
	void DISABLEALLLANDEFF();
	void ADDLANDEFF( SLANDEFFECT_VALUE& landEffect );
	void DELLANDEFF( SLANDEFFECT_VALUE& landEffect );

	void DISABLEALLMAPEFF();
	void ADDMAPEFF( SMAP_EFFECT& mapEffect );
	void DELMAPEFF( SMAP_EFFECT& mapEffect );

	D3DXVECTOR3 GetRandomPosition ();
	const D3DXVECTOR3* GetRandPos();

    BOOL LoadFile(const std::string& FileName, SUMMON_TYPE emType);

	SUMMON_TYPE GetSummonType() { if(m_pSummonCrowData) return m_pSummonCrowData->m_emSummonType;  return SUMMON_TYPE_NONE; }

	const float GET_INTERACTION_RANGE() const; //{ return (m_pSummonCrowData->m_fRefenceActionableRange > 0.0f) ? m_pSummonCrowData->m_fRefenceActionableRange : GLCONST_CHAR::fNPC_INTERACTION_RANGE; }

	bool ISSummonNPC_INTERACTIONABLE( const D3DXVECTOR3& vReqPos, const D3DXVECTOR3& vCurPos, const float fReqRadius, int nNpcAction ) const;

	const SCROWATTACK* GetCrowAttack();
};
typedef GLSUMMON* PGLSUMMON;

struct SDROPSUMMON
{
	DWORD			 m_dwGUID;
	SNATIVEID		 m_sSummonID;

	SUMMON_TYPE		 m_emTYPE;

	Faction::GLActorID	m_sOwnerID;				// 주인 ActorID;
	DWORD			 m_dwOwner;					// 주인 GaeaID
	SNATIVEID		 m_sMapID;					// 맵 ID
	DWORD			 m_dwCellID;

	FLOAT            m_fRemainedTime;           // 남은 시간

	float		     m_fOwnerIncHP;		        // 주인의  HP 회복율
	WORD		     m_wOwnerAttack;            // 주인의  공격력
	WORD		     m_wOwnerDefense;           // 주인의  방어력
	WORD             m_wOwnerSkillLevel;        // 주인의  스킬 레벨
	BYTE             m_cOwnerWeaponsGrade;      // 주인의  무기 강화
	ITEM::SSATE_BLOW m_sOwnerBlow;              // 주인의  무기 속성
    WORD             m_wOwnerSchool;            // 주인의  학원

	D3DXVECTOR3		 m_vPos;
	D3DXVECTOR3		 m_vDir;
	float			 m_fSpeed;

	DWORD			 m_dwActionFlag;
	EMACTIONTYPE     m_emAction;

	WORD			 m_wAniSub;					// 애니메이션 관련

	DWORD			 m_dwNowHP;					// 생명량.
	WORD			 m_wNowMP;					// 정신량.

	STARGETID        m_sTarget;                 // 타겟정보

	// 서버 클라이언트간 소환수의 위치 동기화를 위해
	D3DXVECTOR3			     vStartPos;          // 이동시 시작위치.
	NavigationPath::WAYPOINT sNextWaypoint;      // 다음 웨이포인트.

	// 해당 소환게이트에대한 정보
	DWORD                    m_EventNodeSummonGateID;

	// 소환수 제스쳐
	EMSUMMONGESTURESTATE     m_emGestureState;
	FLOAT                    m_fIdleTime;

	// 스킬 FACT
	SDROP_SKILLFACT	m_sSKILLFACT[CROW_SKILLFACT_SIZE];
	SSTATEBLOW	    m_sSTATEBLOWS[EMBLOW_MULTI];

	SDROPSUMMON () :
	m_dwGUID(UINT_MAX),
		m_sSummonID(NATIVEID_NULL()),

		m_dwOwner(0),

		m_sMapID(NATIVEID_NULL()),
		m_dwCellID(0),

		m_fRemainedTime(0.0f),

		m_fOwnerIncHP(0.0f),
		m_wOwnerAttack(0),
		m_wOwnerDefense(0),
		m_wOwnerSkillLevel(0),
		m_cOwnerWeaponsGrade(0),
		m_sOwnerBlow(),

		m_vPos   (0,0,0),
		m_vDir   (0,0,-1),
		m_fSpeed (0.0f),

		m_dwActionFlag(0),
		m_emAction(GLAT_IDLE),

		m_emTYPE(SUMMON_TYPE_NONE),

		m_wAniSub(1),

		m_dwNowHP(0),
		m_wNowMP(0),

		m_EventNodeSummonGateID(UINT_MAX),

		m_emGestureState(EM_SUMMONGESTURE_NONE),
		m_fIdleTime(0.0f)
	{
	}
};

typedef SDROPSUMMON* PSDROPSUMMON;

namespace COMMENT
{
	extern std::string SUMMON_TYPE[SUMMON_TYPE_SIZE];
};
