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
		EMMAXPAGE = 5,			// File�� ����ǰų� �ε��ɶ� ������ �ش�. ( �Ժη� �ٲ��� ���� );

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
		SNATIVEID			sNativeID;					//	���� ID.
		
		EMCROW				m_emCrow;					//	Npc, Mob.
		EMTRIBE				m_emTribe;					//	����.
		EMBRIGHT			m_emBright;					//	�Ӽ� (��/��)
		
		char				m_szName[CHAR_SZNAME];		//	�̸�.		

		WORD				m_wLevel;					//	����.

		DWORD				m_dwHP;						//	HP
		WORD				m_wMP;						//	MP
		WORD				m_wSP;						//	SP

		//	ȸ����.
		float				m_fRegenHP;					//	HP ȸ����.
		float				m_fRegenMP;					//	MP ȸ����.
		float				m_fRegenSP;					//	SP ȸ����.

		WORD				m_wDefense;					//	����.

		WORD				m_wViewRange;				//	�þ� �Ÿ�.
		WORD				m_wAvoidRate;				//	ȸ����.
		WORD				m_wHitRate;					//	������.

		DWORD				m_wBonusExp;				//	�߰�����ġ.
		WORD				m_wBonusCP;					//	�߰� CP.

		SRESIST				m_sResist;					//	���װ�.

		bool				m_bOverlapAttack;			//  �������� ���� ���� ����

		bool                m_bNonDamaged;              //  ���� ���� ���� ����

		DWORD				m_dwGenTime;

		WORD				m_wGatherTimeLow;			// ä���ð�(Low)
		WORD				m_wGatherTimeHigh;			// ä���ð�(High)
		WORD				m_fGatherRate;				// ä�� Ȯ��
		WORD				m_wGatherAnimation;			// �ִϸ��̼�

		bool				m_bNoAllPenalty;			// �Ҹ��� ȿ�� ����
		DWORD				m_dwNoPenaltyFlag;			// Ÿ�Ժ� �Ҹ��� ȿ�� ����

		// PET
		int				    m_emPetType;

		// SUMMON
		SUMMON_TYPE			m_emSummonType;

		// SUMMON
		NPC_TYPE			m_emNPCType;

        bool                m_bHideNaming;              //  �̸�/HP ǥ������ ����
        bool                m_bNonTargeting;            //  Ÿ���� ���� ���ϰ� ��

        float               m_fScale;                   // �����ϰ�

		bool				m_bHideMinimapInfo;			// �̴ϸ� �������� �Ⱥ��̵��� ����;
		WORD				m_wResist_Probability;		// ���� Ȯ��;

		float				m_fBarrierRadius;			// ��ֹ� �ݰ�
		float				m_fBarrierWidth;			// ��ֹ� �β�

        float               m_fRefenceActionableRange;  // ��ȣ�ۿ� �ݰ�;
		};
	};

public:
    SCROWDATA();
    ~SCROWDATA();

public:
	SCROWACTION				m_sAction;
	SCROWGEN				m_sGenerate; //GenItem�� ���� ����

	//	npc talk ����.
    std::tr1::shared_ptr<CNpcDialogueSet> m_spNpcTalkDlg;

	//	����, Skill ����.
	CROWATTACK_VEC			m_vecCrowAttack;
	CROWATTACK_VEC			m_vecCrowAttackEvent;

	std::vector< CROWATTACK_VEC > m_vecCrowAttackPage;
	std::vector< CROWATTACK_VEC > m_vecCrowAttackEventPage;

    // ���� �����۱� ���� [server ������ �ʿ��� ����]
    // 11-07-22 Item ������ ��� GenItem�� ó���ϵ��� ���� (bjju)
#if defined( _ITEM_GEN_060417_H_ ) || defined( _QUESTION_ITEM_GEN_060417_H_ )
        SGENITEM_060417		m_sGenItem;
        SQTGENITEM_060417   m_sQtGenItem; // Question Item ����
#else
        CGENITEM		    m_cGenItem;
#endif

	// �Ǹ� ��� ������ ����; CommentStrTalbe.xml CROWSALE_SALETYPE
	// ���ڷ� ����������� �ش� ������ ������ ����Ǿ�������� ���ڸ� ���ڿ��� ��´�;
	std::string				m_strSaleType[SCROWACTION::SALENUM];
	GLInventorySale			m_sSaleItems[SCROWACTION::SALENUM];
	//  �Ǹ� NPC���� �Էµ� ����
	NPCSELL_PRICE_MAP		m_mapNpcSellPrice;

	//	���ϸ��̼� ����. ( �ð�/���� ) [server ������ �ʿ��� ����]
	//	
	VECANIATTACK* m_pANIMATION;

	// item_exchange
	typedef boost::function<int ( gltexfile* const txtparsing, GLInventorySale &_sINVENTORY, std::string &_strSaleType, NPCSELL_PRICE_MAP &_mapNpcSellPrice)> fun_saleLoad;

	fun_saleLoad m_fun_ptr_crow[CROW_SALE_V_INDEX(GLInventorySale::CROW_SALE_VERSION_MAX)];

public:
    //!	���� ID
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

    //! ����
    WORD GetLevel() const { return m_wLevel; }

    //! HP
    DWORD GetHp() const { return m_dwHP; }

    //! �߰�����ġ
    DWORD GetBonusExp() const { return m_wBonusExp; }
    
    //! �߰� CP
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


