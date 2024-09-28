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
	// Note : �ܺ� ����;
	SUMMON_TYPE		 m_emTYPE;					// �ڵ� AI Type ( Script �� AI Type�� �� Ÿ���� �Ϲ�Ÿ������ �θ� �ȴ�;
	SNATIVEID		 m_sSummonID;				// ��ȯ�� Crow ID;
	
	DWORD			 m_dwOwnerGaeaID;			// ���� GaeaID ( GaeaID�� ���� ����� );
	Faction::GLActorID	m_sOwnerID;				// ������ ActorID;

	PCROWDATA		 m_pSummonCrowData;			// Crow �⺻ ����;
	FLOAT            m_fRemainedTime;           // ��ȯ �����ð�;

	float		     m_fOwnerIncHP;		        // ������ HP ȸ����;
	WORD		     m_wOwnerAttack;            // ������ ���ݷ�;
	WORD		     m_wOwnerDefense;           // ������ ����;
	WORD		     m_wOwnerSkillLevel;        // ������ ��ų ����;
	BYTE             m_cOwnerWeaponsGrade;      // ������ ���� ��ȭ;
    ITEM::SSATE_BLOW m_sOwnerBlow;              // ������ ���� �Ӽ�;
	GLITEM_ATT		 m_emOwnerItemAtt;		// ������ ����;
    WORD             m_wOwnerSchool;            // ������ �п�;

	SKILL::GLSKILL_ATT m_emNeedItemAtt;			// ��ȯ�� �ʿ��� ��������

	float			 m_fRunArea;                 // �޷��߸� �ϴ� �Ÿ�;
	float			 m_fWalkArea;                // �ɾ�߸� �ϴ� �Ÿ�;
	float			 m_fOwnerDistance;           // ���ΰ��� ���� �Ÿ�;
	float			 m_fOwnerDegree;             // ���ΰ��� ���� ����;
	float			 m_fPartyDistance;           // ��Ƽ �ν� �Ÿ�;
	float			 m_fMoveRefreshDistance;     // �̵� ���� �Ÿ�;
	float			 m_fOwnerDamagePer;			 // ������ ������ ��·�;
	float			m_fOwnerMovePer;			// ������ �̵��ӵ� ��·�;
    bool			 m_bOwnerSpeed;				 // ������ �ӵ��� ���� ������ ����;



	// Note : ��ȯ�� ���� ����;
	DWORD				m_dwGlobalID;					// GLCrow ���� ����;
	float				m_fACTIONDELAY[SCROWDATA::EMMAXATTACK];	//	���� cool time;

	SSKILLFACT			m_sSKILLFACT[CROW_SKILLFACT_SIZE];	//	��ųȿ����.
	VEC_LANDEFF_VALUE	m_vecLandEffect;				//  ���� ȿ����
	VEC_MAP_EFFECT		m_vecMapEffect;					//	�ʿ� ����Ǵ� ȿ����

	DWORD				m_dwHOLDBLOW;					//	�����̻��� ������.

	SSTATEBLOW			m_sSTATEBLOWS[EMBLOW_MULTI];	//	�����̻�.
	float				m_fSTATE_MOVE_RATE;					//	���� �̻� �̵��ӵ�. ( 1.0f == 100% )
	float				m_fSTATE_DELAY;					//	���� �̻� ������. ( 1.0f == 100% ) (+)
	float				m_fSTATE_DAMAGE;	 			//	���� �̻� ����� ����.

	bool				m_bSkillFactsSpecial[EMSPECA_NSIZE];
	bool				m_bForcedSkillFactsSpecial[EMSPECA_NSIZE];
	bool				m_bSTATE_PANT;					//	�����̻� �涱��.
	bool				m_bSTATE_STUN;					//	�����̻� ����.
	SINVISIBLE			m_sINVISIBLE;					// ����;
	SRECVISIBLE			m_sRECVISIBLE;					// ���Ű���;
	//bool				m_bINVISIBLE;					// ����;
	//bool				m_bRECVISIBLE;					// ���� Ȯ��;

	SHALLUCINATE		m_sHALLUCINATE;					// ȯ�� ȿ��;
	SRELEASE_ENDURE		m_sRELEASE_ENDURE;				// �ǰݽ� ȿ�� ����;
	SDOMINATE			m_sDOMINATION;					// ���� ȿ��;
	SSTIGMA				m_sSTIGMA_USER;					// ���� ȿ�� - ��� ( ���� ĳ���͸��� ��� ���� );
	SSTIGMA				m_sSTIGMA_TAKER;				// ���� ȿ�� - ����;
	SINCREASEEFF		m_sIncreaseEff;					// ��ȭ ȿ��;

    SLINKHP				m_sLINKHP;						// ��ȿ��;
	SIGNORED_DAMAGE		m_sIgnoredDamage;				// ������ ���;
	SVIEWRANGERATE		m_sRateViewRange;				// �þ�����;
	SAIRBORNE			m_sAirborne;					// ���;
	SCLONE				m_sClone;						// ����ȿ��

	float				m_fATTVELO;						//	���ݼӵ�.
	float				m_fSKILL_MOVE;					//	��ų �̵��ӵ�.

	int					m_nSUM_HIT;						//	������ + ��ųȿ��.
	int					m_nSUM_AVOID;					//	ȸ���� + ��ųȿ��.

	int					m_nSUM_DEFENSE;					//	���� ( DP + POW*��� + ITEM_DEF ) * �������� ���� + ��ų.
	int					m_nDX_DAMAGE;					//	�⺻ ���ݷ�. + ��ųȿ��.

	int					m_nSUM_PIERCE;					//	�߰� ���� ����.
	
	float				m_fDamageRate;					//	���ݷ� ��ȭ��.
	float				m_fDefenseRate;					//	���� ��ȭ��.

	SRESIST				m_sSUMRESIST;					//	���װ�.

	DAMAGE_SPEC			m_sDamageSpec;					//	������ ��� �� �ݻ�
	DEFENSE_SKILL		m_sDefenseSkill;				//  �ߵ��� ��ų
	SDURATION_DAMAGE	m_sDurDamage[EMELEMENT_MAXNUM];	//	���� ������

	DWORD				m_dwActionLimit;				//	�ൿ���� 

	DWORD				m_dwAType;						//	���� �Ӽ� ����.
	bool				m_bUsingEventSkill;				//	�̺�Ʈ ��ų ���.

	DWORD				m_dwNowHP;						//	����.
	WORD				m_wNowMP;						//	���ŷ�.
	WORD				m_wNowSP;						//	���ŷ�

	SNATIVEID			m_idACTIVESKILL;				//	Ȱ�� ��ų.
	WORD				m_wACTIVESKILL_LVL;				//	Ȱ�� ��ų  ����.

	bool				m_bSafeZone;                    //  ������ �� ����

	D3DXVECTOR3			m_RandPos[8];

    float               m_fVariationRateHP;             //  HP ȸ����
    float               m_fVariationRateMP;             //  MP ȸ����
    float               m_fVariationRateSP;             //  SP ȸ����

    float               m_fVariationApplyRateHP;        // HP ��ȭ �����;
    float               m_fVariationApplyRateMP;        // MP ��ȭ �����;
    float               m_fVariationApplyRateSP;        // SP ��ȭ �����;

	//	Note : ȸ���Ǵ� ��ġ ����� �ӽ� ����.
	float				m_fTempVarHP;
	float				m_fTempVarMP;
	float				m_fTempVarSP;

    float               m_fMoveVeloLimit_Min;   // ���� �̵��ӵ� ����;
    float               m_fMoveVeloLimit_Max;   // �ְ� �̵��ӵ� ����;

	// Note : ��ȯ�� ������.
	EMSUMMONGESTURESTATE m_emGestureState;
	EMSUMMONGESTURESTATE m_emGestureableArray[EM_SUMMONGESTURE_MAX]; // ������ ��� ������ �迭  
	DWORD                m_dwGestureNum;                             // ������ ����

	// Note : ���� ���ǵ�
	float				m_fCurSpeed;

	// �̺�Ʈ ��ų ����;
	DWORD				m_dwCurSkillPage;

	CSkillCAIdx			m_SkillCAIndex;			// �ݰݸ���Ʈ

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

	// Note : ����ȿ��
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

	Faction::GLActorID	m_sOwnerID;				// ���� ActorID;
	DWORD			 m_dwOwner;					// ���� GaeaID
	SNATIVEID		 m_sMapID;					// �� ID
	DWORD			 m_dwCellID;

	FLOAT            m_fRemainedTime;           // ���� �ð�

	float		     m_fOwnerIncHP;		        // ������  HP ȸ����
	WORD		     m_wOwnerAttack;            // ������  ���ݷ�
	WORD		     m_wOwnerDefense;           // ������  ����
	WORD             m_wOwnerSkillLevel;        // ������  ��ų ����
	BYTE             m_cOwnerWeaponsGrade;      // ������  ���� ��ȭ
	ITEM::SSATE_BLOW m_sOwnerBlow;              // ������  ���� �Ӽ�
    WORD             m_wOwnerSchool;            // ������  �п�

	D3DXVECTOR3		 m_vPos;
	D3DXVECTOR3		 m_vDir;
	float			 m_fSpeed;

	DWORD			 m_dwActionFlag;
	EMACTIONTYPE     m_emAction;

	WORD			 m_wAniSub;					// �ִϸ��̼� ����

	DWORD			 m_dwNowHP;					// ����.
	WORD			 m_wNowMP;					// ���ŷ�.

	STARGETID        m_sTarget;                 // Ÿ������

	// ���� Ŭ���̾�Ʈ�� ��ȯ���� ��ġ ����ȭ�� ����
	D3DXVECTOR3			     vStartPos;          // �̵��� ������ġ.
	NavigationPath::WAYPOINT sNextWaypoint;      // ���� ��������Ʈ.

	// �ش� ��ȯ����Ʈ������ ����
	DWORD                    m_EventNodeSummonGateID;

	// ��ȯ�� ������
	EMSUMMONGESTURESTATE     m_emGestureState;
	FLOAT                    m_fIdleTime;

	// ��ų FACT
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
