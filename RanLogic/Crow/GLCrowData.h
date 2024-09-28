#pragma once

#include <string>
#include <boost/tr1/memory.hpp>
#include "../../EngineLib/G-Logic/GLDefine.h"
#include "../../EngineLib/DxMeshs/SkinMesh/SAnimation.h"
#include "../../EngineLib/NaviMesh/NavigationPath.h"

#include "../Character/GLCharData.h"
#include "../Character/GLCharDefine.h"
#include "../Inventory/GLInventorySale.h"
#include "../NpcTalk/NpcDialogueSet.h"
#include "./CrowAction.h"
#include "./CrowGen.h"
#include "./ItemGen.h"
#include "./CrowAttack.h"
#include "./CrowBasic.h"

typedef std::map<DWORD,LONGLONG>	NPCSELL_PRICE_MAP;
typedef NPCSELL_PRICE_MAP::iterator NPCSELL_PRICE_MAP_ITER;

typedef std::vector<SCROWATTACK>	CROWATTACK_VEC;
typedef CROWATTACK_VEC::iterator	CROWATTACK_VEC_ITER;

class gltexfile;

struct SCROWDATA
{
	enum
	{
		VERSION = 0x0101,
		EMMAXATTACK = 10,
		EMMAXPAGE = 5,			// File이 저장되거나 로딩될때 영향을 준다. ( 함부로 바꾸지 말자 );

		FILE_SBASIC			= 1,
		FILE_SACTION		= 2,
		FILE_SGEN			= 3,
		FILE_SATTACK		= 4,
		FILE_SATTACK_EVENT	= 5,
		FILE_END_DATA		= 0xEDEDEDED,
	};

	enum
	{
		CROW_LOAD_ERROR1	= 0x0001,
		CROW_LOAD_ERROR2	= 0x0002,
		CROW_LOAD_ERROR3	= 0x0004,
		CROW_LOAD_ERROR4	= 0x0008,
		CROW_LOAD_ERROR5	= 0x0010,
		CROW_LOAD_ETC		= 0x0012,
		CROW_LOAD_EXCHANGE_SUCCESS 	= 0x1000,
		CROW_LOAD_SALE_SUCCESS      = 0x1002,
		CROW_LOAD_SUCCESS	= 0x1004,
	};

	union
	{
		struct
		{
		    SCROWBASIC	m_sBasic;
		};
		struct
		{
		SNATIVEID			sNativeID;					//	고유 ID.
		
		EMCROW				m_emCrow;					//	Npc, Mob.
		EMTRIBE				m_emTribe;					//	종족.
		EMBRIGHT			m_emBright;					//	속성 (광/암)
		
		char				m_szName[CHAR_SZNAME];		//	이름.		

		WORD				m_wLevel;					//	레벨.

		DWORD				m_dwHP;						//	HP
		WORD				m_wMP;						//	MP
		WORD				m_wSP;						//	SP

		//	회복율.
		float				m_fRegenHP;					//	HP 회복율.
		float				m_fRegenMP;					//	MP 회복율.
		float				m_fRegenSP;					//	SP 회복율.

		WORD				m_wDefense;					//	방어력.

		WORD				m_wViewRange;				//	시야 거리.
		WORD				m_wAvoidRate;				//	회피율.
		WORD				m_wHitRate;					//	명중율.

		DWORD				m_wBonusExp;				//	추가경험치.
		WORD				m_wBonusCP;					//	추가 CP.

		SRESIST				m_sResist;					//	저항값.

		bool				m_bOverlapAttack;			//  겹쳤을때 공격 가능 여부

		bool                m_bNonDamaged;              //  공격 받지 않음 여부

		DWORD				m_dwGenTime;

		WORD				m_wGatherTimeLow;			// 채집시간(Low)
		WORD				m_wGatherTimeHigh;			// 채집시간(High)
		WORD				m_fGatherRate;				// 채집 확률
		WORD				m_wGatherAnimation;			// 애니메이션

		bool				m_bNoAllPenalty;			// 불리한 효과 방지
		DWORD				m_dwNoPenaltyFlag;			// 타입별 불리한 효과 방지

		// PET
		int				    m_emPetType;

		// SUMMON
		SUMMON_TYPE			m_emSummonType;

		// SUMMON
		NPC_TYPE			m_emNPCType;

        bool                m_bHideNaming;              //  이름/HP 표시하지 않음
        bool                m_bNonTargeting;            //  타켓팅 하지 못하게 함

        float               m_fScale;                   // 스케일값

		bool				m_bHideMinimapInfo;			// 미니맵 정보에서 안보이도록 숨김;
		WORD				m_wResist_Probability;		// 저항 확률;

		float				m_fBarrierRadius;			// 장애물 반경
		float				m_fBarrierWidth;			// 장애물 두께

        float               m_fRefenceActionableRange;  // 상호작용 반경;
		};
	};

public:
    SCROWDATA();
    ~SCROWDATA();

public:
	SCROWACTION				m_sAction;
	SCROWGEN				m_sGenerate; //GenItem에 대한 정보

	//	npc talk 정보.
    std::tr1::shared_ptr<CNpcDialogueSet> m_spNpcTalkDlg;

	//	공격, Skill 설정.
	CROWATTACK_VEC			m_vecCrowAttack;
	CROWATTACK_VEC			m_vecCrowAttackEvent;

	std::vector< CROWATTACK_VEC > m_vecCrowAttackPage;
	std::vector< CROWATTACK_VEC > m_vecCrowAttackEventPage;

    // 생성 아이템군 설정 [server 에서만 필요한 정보]
    // 11-07-22 Item 관련은 모두 GenItem이 처리하도록 변경 (bjju)
#if defined( _ITEM_GEN_060417_H_ ) || defined( _QUESTION_ITEM_GEN_060417_H_ )
        SGENITEM_060417		m_sGenItem;
        SQTGENITEM_060417   m_sQtGenItem; // Question Item 생성
#else
        CGENITEM		    m_cGenItem;
#endif

	// 판매 목록 데이터 통합; CommentStrTalbe.xml CROWSALE_SALETYPE
	// 숫자로 받으면되지만 해당 데이터 버전이 변경되어야함으로 숫자를 문자열로 담는다;
	std::string				m_strSaleType[SCROWACTION::SALENUM];
	GLInventorySale			m_sSaleItems[SCROWACTION::SALENUM];
	//  판매 NPC에게 입력된 가격
	NPCSELL_PRICE_MAP		m_mapNpcSellPrice;

	//	에니메이션 정보. ( 시간/스탭 ) [server 에서만 필요한 정보]
	//	
	VECANIATTACK* m_pANIMATION;

	// item_exchange
	typedef boost::function<int ( gltexfile* const txtparsing, GLInventorySale &_sINVENTORY, std::string &_strSaleType, NPCSELL_PRICE_MAP &_mapNpcSellPrice)> fun_saleLoad;

	fun_saleLoad m_fun_ptr_crow[CROW_SALE_V_INDEX(GLInventorySale::CROW_SALE_VERSION_MAX)];

public:
    //!	고유 ID
    SNATIVEID GetId() const { return sNativeID; }

    bool IsBasicTalk(int nAction) const;
    std::tr1::shared_ptr<SNpcTalk> GetTalk(DWORD dwTalkID) const;

	VECANIATTACK&			GetAnimation ( EMANI_MAINTYPE emMType, EMANI_SUBTYPE emSType );

	inline BOOL IsCheckedActFlags( DWORD dwFlags ) 	{ return m_sAction.m_dwActFlag & dwFlags; }
	inline BOOL IsCdCertify()	const   			{ return m_sAction.m_dwActFlag & EMCROWACT_CDCERTIFY; }
	inline BOOL IsBusUnit()		const     			{ return m_sAction.m_dwActFlag & EMCROWACT_BUSUNIT; }
	inline BOOL IsTarShort()	const    			{ return m_sAction.m_dwActFlag & EMCROWACT_TARSHORT; }
	inline BOOL IsIgnoreShock()	const 				{ return m_sAction.m_dwActFlag & EMCROWACT_IGNORE_SHOCK; }

	inline BOOL IsInVisible()	const  				{ return m_sAction.m_dwActFlag & EMCROWACT_INVISIBLE; }
	inline BOOL IsRecVisible()	const 				{ return m_sAction.m_dwActFlag & EMCROWACT_RECVISIBLE; }

	inline BOOL IsBarrier() 	const 				{ return m_sAction.m_dwActFlag & EMCROWACT_BARRIER; }
	inline BOOL IsPosHold() 	const 				{ return m_sAction.m_dwActFlag & EMCROWACT_POSHOLD; }
	inline BOOL IsDirHold() 	const 				{ return m_sAction.m_dwActFlag & EMCROWACT_DIRHOLD; }
	inline BOOL IsBoss()		const   			{ return m_sAction.m_dwActFlag & EMCROWACT_BOSS; } 
    inline BOOL IsKnock()		const   			{ return m_sAction.m_dwActFlag & EMCROWACT_KNOCK; }


	DWORD GetSaleNum() const { return SCROWACTION::SALENUM; }
	GLInventory* GetSaleInven(DWORD dwIndex)
	{
		if (GetSaleNum() <= dwIndex)
            return NULL;
        else
		    return &m_sSaleItems[dwIndex];
	}
	
	std::string GetSaleType(DWORD dwIndex)
	{
		if (GetSaleNum() <= dwIndex)
            return "";
        else
		    return m_strSaleType[dwIndex];
	}
	LONGLONG GetNpcSellPrice ( DWORD dwID )
	{
		NPCSELL_PRICE_MAP_ITER it = m_mapNpcSellPrice.find(dwID);
		if (it == m_mapNpcSellPrice.end())
			return 0;
        else
		    return it->second;
	}

	const char* GetName();
    const std::string GetNameStr() const;

	const char*        GetTalkFile() const { return m_sAction.m_strTalkFile.c_str(); }
    const std::string& GetTalkFileStr() const { return m_sAction.m_strTalkFile; }

	const char* GetSkinObjFile() const { return m_sAction.m_strSkinObj.c_str(); }
    const std::string& GetSkinObjFileStr() const { return m_sAction.m_strSkinObj; }

	void LoadAniSet ( const char* szSkinObj );
	SCROWDATA& Assign ( SCROWDATA &Data );

	HRESULT LoadFile ( sc::BaseStream &SFile, BOOL bServer, bool bPastLoad );
	HRESULT SaveFile ( sc::SerialFile &SFile );

	static VOID SaveCsvHead ( std::fstream &SFile );
	VOID SaveCsv ( std::fstream &SFile );
	VOID LoadCsv ( CStringArray &StrArray );    

    DWORD GenTime() const { return m_dwGenTime; }

    //! 레벨
    WORD GetLevel() const { return m_wLevel; }

    //! HP
    DWORD GetHp() const { return m_dwHP; }

    //! 추가경험치
    DWORD GetBonusExp() const { return m_wBonusExp; }
    
    //! 추가 CP
    WORD GetBonusCp() const { return m_wBonusCP; }

	int SaleInvenLoadFileVersion( const char* szFileName, int iArrayIdx );
};
typedef SCROWDATA* PCROWDATA;

BOOL SaleInvenLoadFile ( const char* szFileName, GLInventory &_sINVENTORY, std::string &_strSaleType, NPCSELL_PRICE_MAP &_mapNpcSellPrice );

int SaleInvenLoadVer1( gltexfile* const txtparsing, GLInventorySale &_sINVENTORY, std::string &_strSaleType, NPCSELL_PRICE_MAP &_mapNpcSellPrice );
int SaleInvenLoadVer2( gltexfile* const txtparsing, GLInventorySale &_sINVENTORY, std::string &_strSaleType, NPCSELL_PRICE_MAP &_mapNpcSellPrice );



namespace COMMENT
{
	extern std::string szCROWACT_UP[EMCROWACT_UP_NSIZE];
	extern std::string szCROWACT_DN[EMCROWACT_DN_NSIZE];
	extern std::string NPC_TYPE[NPC_TYPE_SIZE];
};

class CROW_PATTERN_CMP
{
public:
	bool operator () ( const SCROWPATTERN &lvalue, const SCROWPATTERN &rvalue )
	{
		if( lvalue.m_fPatternDNRate > rvalue.m_fPatternDNRate ) return true;
		return false;
	}
};


