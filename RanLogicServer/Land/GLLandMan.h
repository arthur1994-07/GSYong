#ifndef GLLANDMAN_H_
#define GLLANDMAN_H_

#pragma once

#include <string>
#include <stack>
#include <boost/circular_buffer.hpp>
#include "../../SigmaCore/Container/MList.h"
//#include "../../SigmaCore/Memory/CMemPool.h"
#include "../../SigmaCore/Memory/ObjectPool.h"
#include "../../SigmaCore/File/SerialFile.h"
#include "../../SigmaCore/Util/GLSingletone.h"

#include "../../EngineLib/NaviMesh/NavigationMesh.h"
#include "../../EngineLib/DxTools/DxWeatherMan.h"
#include "../../EngineLib/DxLand/CollisionMap.h"
#include "../../EngineLib/DxTools/DxFontMan.h"
#include "../../EngineLib/DxLand/DxLandDef.h"
#include "../../EngineLib/DxLand/DxLandMan.h"
#include "../../EngineLib/DxLand/DxLandGateMan.h"
#include "../../EngineLib/DxTools/DxViewPort.h"
#include "../../EngineLib/DxTools/EditMeshs.h"
#include "../../EngineLib/G-Logic/GLOGIC.h"
#include "../../EngineLib/G-Logic/GLPeriod.h"
#include "../../EngineLib/G-Logic/GLQuadTree.h"
#include "../../EngineLib/DxMeshs/DxSimpleMeshMan.h"
#include "../../EngineLib/NaviMesh/NavigationMesh.h"

#include "../../RanLogic/Skill/GLSkill.h"
#include "../../RanLogic/Item/GLItem.h"
#include "../../RanLogic/RANPARAM.h"
#include "../../RanLogic/TriggerSystem/TriggerSystemType.h"
#include "../../RanLogic/PVE/PVEInDunID.h"

#include "../Level/GLLevelFileServer.h"
#include "../LandEvent/GLLandEventNodeServer.h"
#include "../Material/GLMaterial.h"
#include "../Pet/GLPetField.h"
#include "../Summon/GLSummonField.h"
#include "../Crow/GLCrow.h"

#include "./Faction/FactionManagerField.h"
#include "../SkillTargetSystem/SkillTargetSystem.h"
#include "../CountRecorder/CountRecorder.h"
//
//mjeon.tbb
//
#ifdef _USE_TBB

#include "../../=TBB/include/tbb/concurrent_vector.h"

#endif


#ifndef GLChar
	class GLChar;
	typedef GLChar* PGLCHAR;

	typedef sc::CGLLIST<PGLCHAR>			GLCHARLIST;
	typedef sc::SGLNODE<PGLCHAR>			GLCHARNODE;

	//
	//mjeon.tbb
	//
#ifdef _USE_TBB
	typedef tbb::concurrent_vector<PGLCHAR>	CVECGLCHAR;
	typedef CVECGLCHAR::iterator			ITER_CVECGLCHAR;
#endif

	typedef std::map<std::string,PGLCHAR>	GLCHAR_MAP;
	typedef std::pair<std::string,PGLCHAR>	GLCHAR_MAP_PAIR;
	typedef GLCHAR_MAP::iterator			GLCHAR_MAP_ITER;
#endif //GLChar

#ifndef GLPetField
	class GLPetField;
	typedef GLPetField* PGLPETFIELD;

	typedef sc::CGLLIST<PGLPETFIELD>		GLPETLIST;
	typedef sc::SGLNODE<PGLPETFIELD>		GLPETNODE;
#endif

#ifndef std_afunc
	namespace std_afunc
	{
		struct CHARDIST;
		struct CROWDIST;
		struct SUMMONDIST;
	};
#endif

typedef std::list<std_afunc::CHARDIST>		LIST_CHAR;
typedef std::vector<std_afunc::CHARDIST>	ARRAY_CHAR;
typedef ARRAY_CHAR::iterator				ARRAY_CHAR_ITER;

typedef std::list<std_afunc::CROWDIST>		LIST_CROW;
typedef std::vector<std_afunc::CROWDIST>	ARRAY_CROW;
typedef ARRAY_CROW::iterator				ARRAY_CROW_ITER;

typedef std::list<std_afunc::SUMMONDIST>	LIST_SUMMON;
typedef std::vector<std_afunc::SUMMONDIST>	ARRAY_SUMMON;
typedef ARRAY_SUMMON::iterator				ARRAY_SUMMON_ITER;

typedef std::set<DWORD>	                    CHARDBNUM_LIST;
typedef CHARDBNUM_LIST::iterator			CHARDBNUM_LIST_ITER;
typedef CHARDBNUM_LIST::const_iterator	    CHARDBNUM_LIST_CITER;

typedef std::vector<STARDIST> TARDISTMAP;
typedef TARDISTMAP::iterator  TARDISTMAP_ITER;

typedef std::vector<STARGETID>						VEC_SK_TAR;
typedef VEC_SK_TAR::iterator							VEC_SK_ITER;
typedef std::pair<VEC_SK_ITER,VEC_SK_ITER>	VEC_SK_ITER_RANGE;

struct NET_MSG_GENERIC;
class NavigationMesh;
class GLLandEventServer;
namespace InstanceSystem
{
	class InstanceField;
}

struct SSKILLACT
{
	STARGETID		sID;
	STARGETID		sID_TAR;
	SNATIVEID		sSkillID;
	float				fDELAY;
	SKILL::EMAPPLY	emAPPLY;

	DWORD		dwDamageFlag;
	int				nVAR_HP, nVAR_MP, nVAR_SP;
	int				nGATHER_HP, nGATHER_MP, nGATHER_SP;
	WORD		wSkillLevel;

	SSKILLACT () 
		: sSkillID(SNATIVEID(false))
		, fDELAY(0.0f)
		, emAPPLY(SKILL::EMAPPLY_MELEE)
		, dwDamageFlag( DAMAGE_TYPE_NONE )
		
		, nVAR_HP(0)
		, nVAR_MP(0)
		, nVAR_SP(0)
		
		, nGATHER_HP(0)
		, nGATHER_MP(0)
		, nGATHER_SP(0)
	{
	}

	bool IsDELAY () const
	{
		return fDELAY!=0.0f;
	}

	bool IsATTACK () const
	{
		return nVAR_HP<0 || nVAR_MP<0 || nVAR_SP<0;
	}

	bool VALID_VAR () const
	{
		return (nVAR_HP|nVAR_MP|nVAR_SP|dwDamageFlag)!=NULL;
	}

	bool VALID_GATHER () const
	{
		return (nGATHER_HP|nGATHER_MP|nGATHER_SP)!=NULL;
	}

	bool VALID () const
	{
		return VALID_VAR() | VALID_GATHER();
	}
};

struct SSKILLACTEX
{
	STARGETID	sID;
	STARGETID	sID_TAR;
	float		fDELAY;

	SNATIVEID	idSKILL;		//	��ų ���� ����.
	WORD		wSKILL_LVL;		//
    WORD        wSELSLOT;

	float		fPUSH_PULL;		//	��ų �����̴� �Ÿ�.
	float		fPUSH_SPEED;	//	��ų �����̴� �ӵ�

	DWORD		dwCUREFLAG;		//	���� �̻� ġ��.
	SSTATEBLOW	sSTATEBLOW;		//	���� �̻� �߻�.

    bool		dwRemoveFlag[SKILL::EMACTION_TYPE_NSIZE];	//	����/����� ����
	DWORD		dwAniType;		//	Push_pull ���� �ִϸ��̼� ��ȣ

	bool		bOnward;		//	���� ����


	SSKILLACTEX () :
		fDELAY(0.0f),

		idSKILL(false),
		wSKILL_LVL(0),
        wSELSLOT(0),

		fPUSH_PULL(0.0f),
		fPUSH_SPEED(0.0f),
		dwCUREFLAG(NULL),
		dwAniType(AN_SUB_PULLIN),
		bOnward(false)
	{
        memset( dwRemoveFlag, 0, sizeof(dwRemoveFlag) );
	}

	bool IsDELAY () const
	{
		return fDELAY!=0.0f;
	}

	bool VALID() const
	{
        if( ( idSKILL != SNATIVEID(false) )				||
			( fPUSH_PULL != 0.0f )						||
			( dwCUREFLAG != NULL )						||
			( sSTATEBLOW.emBLOW != EMBLOW_NONE ) )
		{
			return true;
		} //if

        for ( int i = 0; i < SKILL::EMACTION_TYPE_NSIZE; ++i )
        {
            if( dwRemoveFlag[i] == true )
                return true;
        }

		return false;
	}
};

typedef std::list<SSKILLACT>		LISTSACT;
typedef LISTSACT::iterator			LISTSACT_ITER;

typedef std::list<SSKILLACTEX>		LISTSACTEX;
typedef LISTSACTEX::iterator		LISTSACTEX_ITER;

typedef std::list<DWORD>			LISTDELCROWID;
typedef LISTDELCROWID::iterator		LISTDELCROWID_ITER;

typedef std::list<DWORD>			LISTDELMATERIALID;
typedef LISTDELMATERIALID::iterator	LISTDELMATERIALID_ITER;

struct SGROUPINFO
{
	DWORD	  dwLeaderID;
	STARGETID sTargetID;

	SGROUPINFO()
		: dwLeaderID(0)
	{
		sTargetID.RESET();
	}
};
typedef std::set<DWORD>				SETGROUPMEM;
typedef SETGROUPMEM::iterator		SETGROUPMEM_ITER;
typedef SETGROUPMEM::const_iterator	SETGROUPMEM_CITER;

typedef std::map< std::string,SETGROUPMEM>	MAPMOBGROUP;
typedef MAPMOBGROUP::iterator				MAPMOBGROUP_ITER;
typedef MAPMOBGROUP::const_iterator			MAPMOBGROUP_CITER;

typedef std::map<std::string,SGROUPINFO> MAPGROUPINFO;
typedef MAPMOBGROUP::iterator			 MAPGROUPINFO_ITER;

typedef std::vector<GLLandMan*> LandManVector;
typedef LandManVector::iterator LandManVectorIter;

class GLLandNode
{
public:
	ITEMDROPLIST	m_ItemList;			//	SUIT ������ ����Ʈ.
	MONEYDROPLIST	m_MoneyList;		//	�� ����Ʈ.

	
	GLCHARLIST		m_PCList;			//	CHAR ����Ʈ

	GLCROWLIST		m_CROWList;			//	CROW ����Ʈ
	GLMATERIALLIST	m_MaterialList;		//	Materail ����Ʈ
	
	// PET
	GLPETLIST		m_PETList;			//  PET ����Ʈ

	// SUMMON
	GLSUMMONLIST    m_SummonList;		//	Summon ����Ʈ

	// LANDEVENT NODE
	LANDEVENTNODESERVERLIST m_LandEventNodeList; // LandEventServerNode ����Ʈ

public:
	GLLandNode ()
	{
	}
	~GLLandNode ()
	{
	}
};

typedef SQUADNODE<GLLandNode>	LANDQUADNODE;
typedef GLQuadTree<GLLandNode>	LANDQUADTREE;

struct SDropCrow
{
	GLMobSchedule* m_pMobSchedule; // ���� �����췯;
	SNATIVEID m_sMobLinkID; // ����� ������ ID;
	float m_fPositionXZ[2]; // ���� ��ǥ;
	float m_fMobLinkScale; // ���� ũ�� ����;
	float m_fMobLinkDelay; // ���� ���� ������;
	float m_fIdleTime; // ���� �ð�;
	SDropCrow(GLMobSchedule* pMobSchedule, const SNATIVEID sMobLinkID, 
		const float fPositionX, const float fPositionZ, const float fMobLinkScale, const float fMobLinkDelay) 
		: m_pMobSchedule(pMobSchedule)
		, m_sMobLinkID(sMobLinkID)
		, m_fMobLinkScale(fMobLinkScale)
		, m_fMobLinkDelay(fMobLinkDelay)
		, m_fIdleTime(0.0f)
	{
		this->m_fPositionXZ[0] = fPositionX;
		this->m_fPositionXZ[1] = fPositionZ;
	}
};



//
//mjeon.tbb
//
struct FRAMEMOVE_TIMEVARS
{
	float fTime;
	float fElapsedTime;

	FRAMEMOVE_TIMEVARS(float fTime_, float fElapsedTime_)
	:fTime(fTime_)
	,fElapsedTime(fElapsedTime_)
	{
	}
};


class GLGuidance;
class GLLandManager;

typedef boost::unordered_map<DWORD, GLCrow*> CROW_DROP_MAP;
typedef CROW_DROP_MAP::iterator              CROW_DROP_MAP_ITER;
typedef CROW_DROP_MAP::const_iterator        CROW_DROP_MAP_CITER;
typedef CROW_DROP_MAP::value_type            CROW_DROP_MAP_VALUE;

typedef boost::unordered_map<DWORD, CItemDrop*> ITEM_DROP_MAP;
typedef ITEM_DROP_MAP::iterator                 ITEM_DROP_MAP_ITER;
typedef ITEM_DROP_MAP::const_iterator           ITEM_DROP_MAP_CITER;
typedef ITEM_DROP_MAP::value_type               ITEM_DROP_MAP_VALUE;

typedef boost::unordered_map<DWORD, CMoneyDrop*> MONEY_DROP_MAP;
typedef MONEY_DROP_MAP::iterator                 MONEY_DROP_MAP_ITER;
typedef MONEY_DROP_MAP::const_iterator           MONEY_DROP_MAP_CITER;
typedef MONEY_DROP_MAP::value_type               MONEY_DROP_MAP_VALUE;

typedef boost::unordered_map<DWORD, GLMaterial*> MATERIAL_DROP_MAP;
typedef MATERIAL_DROP_MAP::iterator              MATERIAL_DROP_MAP_ITER;
typedef MATERIAL_DROP_MAP::const_iterator        MATERIAL_DROP_MAP_CITER;
typedef MATERIAL_DROP_MAP::value_type            MATERIAL_DROP_MAP_VALUE;

typedef boost::unordered_map<DWORD, GLSummonField*> SUMMON_DROP_MAP;
typedef SUMMON_DROP_MAP::iterator                   SUMMON_DROP_MAP_ITER;
typedef SUMMON_DROP_MAP::const_iterator             SUMMON_DROP_MAP_CITER;
typedef SUMMON_DROP_MAP::value_type                 SUMMON_DROP_MAP_VALUE;

typedef std::list<SDropCrow> DROP_CROW_LIST;
typedef DROP_CROW_LIST::iterator DROP_CROW_LIST_ITER;

typedef std::vector< UINT >						VEC_REMOVED_MOB_LAYER;
typedef VEC_REMOVED_MOB_LAYER::iterator			VEC_REMOVED_MOB_LAYER_ITER;

class GLLandMan : public GLLevelFileServer
{
	enum { INSTANTMAP_DELETE_TIME = 5 }; //5��

public:
    GLLandMan(GLGaeaServer* pServer, GLLandManager* pLandManager, const SNATIVEID& baseMapID);
    GLLandMan(const GLLandMan& _rhs, GLLandManager* pLandManager);

    virtual ~GLLandMan(void);
    //void ResetLandMan();

protected:
	LPDIRECT3DDEVICEQ m_pd3dDevice;
    GLGaeaServer* m_pGLGaeaServer;
    GLLandManager* m_pLandManager;

public:
	void SetD3dDevice ( LPDIRECT3DDEVICEQ pd3dDevice )	{ m_pd3dDevice = pd3dDevice; }

protected:
	BOOL					m_bInit;
	bool					m_bEmulator;
	DWORD					m_nUsedMemory;

protected:
	D3DXVECTOR3				m_vMax;
	D3DXVECTOR3				m_vMin;
	CCollisionMap			m_CollisionMap;			//	�浹 ��.
	WEATHER_PROPERTY		m_sWeatherProp;			//	���� �Ӽ�.
	LANDQUADTREE			m_LandTree;				//	Cell Tree.

	// [shhan][2016.04.20] 
	// 3�� ���� server crash �� �߻��Ͽ���, Actor::m_Parent �� �ܼ� ��������, boost::shared_ptr �� �����Ѵ�.
	//Actor::GotoLocation->NavigationMesh::IsCollision->DxAABBNode::IsCollision
	// ���� ������.
	boost::shared_ptr<NavigationMesh>			m_spNaviMesh;			//	�濡 ���� ������ ���� �޽�	// ���̺��� ��.



	//////////////////////////////////////////////////////////////////////////
	// shared_ptr �̱� ������ tbb ���� ���Ǹ� ������ �� �� �־ ��������� �ʴ´�. ( ���������� read �� �ϸ� ���� �ɰ� ����. )
	//															{ ����Ϸ��� write �� �ϸ� �ȵǴµ� .. �ڵ� Ȯ���� ���غ��� �ϴ� ������� ����. )
	// ���� Navi �� ������ �� m_spNaviMesh ���� �Ѵ�.
	// �̰� m_spNaviMesh ���� ����� ���� Src �� ���� ��.
	//
private:
	// wld �� ���Ե� Navi
	// �̰� ��������� ����.
	// ���� ��ã�⿡ ���̴� �� m_spNaviMesh �� �����.
	boost::shared_ptr<NavigationMesh>			m_spNaviMeshInWld;

	// �׺񺯰� ����� ���鼭 �߰��Ǿ���.
	// �̰� ��������� ����.
	// ���� ��ã�⿡ ���̴� �� m_spNaviMesh �� �����.
	boost::shared_ptr<NavigationMesh>			m_spNaviMeshBypass;
	//
	// shared_ptr �̱� ������ tbb ���� ���Ǹ� ������ �� �� �־ ��������� �ʴ´�.
	//////////////////////////////////////////////////////////////////////////


	
public:
	DWORD					m_dwClubMapID;			//	���� �� CDM ID
	GLGuidance*			m_pGuidance;			//	���� ����.
	bool					m_bGuidBattleMap;		//	���� Ŭ�� ���� ��.
	bool					m_bGuidBattleMapHall;	//	���� Ŭ�� ���� ��.

	bool					m_bClubDeathMatchMap;			//	Ŭ��������ġ ��
	bool					m_bClubDeathMatchMapHall;		//	Ŭ��������ġ ��

	bool					m_bNonConfrontMode;				// ��� �Ұ��ɸ��

	DWORD					m_dwGuidClubID;			//	���� Ŭ�� ID.

	float					m_fElapsedLimitTime;	//  ���ѽð� ������Ʈ.
	int						m_nLastSendRemainTime;	//  ���������� ���� �����ð�.
	bool					m_bExcessiveLimitTime;  //  �δ� �ʰ��ð� �������� üũ

protected :
	float					m_fCommissionRate;		//	��� Ȱ�� ��������.
    LONGLONG                m_llBuyCommissionMoney;	 //	��� ���� Ȱ�� ������ ������.;
    LONGLONG                m_llSellCommissionMoney; //	��� �Ǹ� Ȱ�� ������ ������.;
    float					m_fCommissionLogTimer;	//	��� Ȱ�� ������ �α� Ÿ�̸�;

public:
	//
	//mjeon.CaptureTheField
	//
	BOOL					m_bCaptureTheField;		// Map for CaptureTheField ?

	BOOL					m_bCopied;				//mjeon.InstantMap�� ��� SrcLandMan���κ��� ���簡 ���������� �Ǿ��°�?
	void					CopyDone()	{m_bCopied = TRUE;}
	BOOL					HasCopied()	{return m_bCopied;}

protected:
	DWORD						m_dwOldWeather;
	std::vector<SONEMAPWEATHER> m_vecMapOldWeather;
	bool					    m_bServerStop;			// ������ �����ϴ���

	float					m_fInstantMapDeleteTime;	
    SPVEInDunID             m_sPVEIndunID;

public:
	LISTDELCROWID			m_listDelGlobIDEx;		// ���� �ֱٿ� mob_gen_ex ���� ������ ���� ID
	LISTDELCROWID			m_listDelGlobID;		// ���� �ֱٿ� mob_gen���� ������ ���� ID, ������ �ʿ�
	LISTDELMATERIALID		m_listDelGlobIDExMat;	// ���� �ֱٿ� mob_gen_ex ���� ������ Meterial�� ID
	LISTDELMATERIALID		m_listDelGlobIDMat;		// ���� �ֱٿ� mob_gen���� ������ Material�� ID, ������ �ʿ�

public:
	boost::shared_ptr<NavigationMesh>	GetNavi ()			{ return m_spNaviMesh; }
	LANDQUADTREE*						GetLandTree ()		{ return &m_LandTree; }
	CCollisionMap&						GetCollisionMap ()	{ return m_CollisionMap; }

	GLLandManager* const GetParentLandManager(void) { return m_pLandManager; }
	
	// ���� �ð� �ʰ� ���� �Ǵ�
	BOOL IsExcessiveLimitTime ()					{ return m_bExcessiveLimitTime; }

	BOOL IsWeatherActive () const					{ return m_sWeatherProp.m_dwFlag&ISACTIVE; }

	const bool isRawDataMap(void) const { return m_mapID.isValid() == true ? false : true; }

	// ������ ����( �ʸ���Ʈ�� ������ ������ off �Ǿ� ������ ������ 0 )
	void SetCommissionRate( float fCommissionRate );
	float GetCommissionRate() { return m_fCommissionRate; }

    void AddBuyCommissionMoney(LONGLONG _llCommissionMoney) { m_llBuyCommissionMoney += _llCommissionMoney; }
    void AddSellCommissionMoney(LONGLONG _llCommissionMoney) { m_llSellCommissionMoney += _llCommissionMoney; }

public:
	const BOOL IsInit () const						{ return m_bInit; }
	const DWORD GetUsedMemory(void) const { return m_nUsedMemory; }
	void SetEmulator ( bool bEMUL )				{ m_bEmulator = bEMUL; }
	void SetUsedMemory(const DWORD nUsedMemory)	{m_nUsedMemory = nUsedMemory; }

	void SetServerStop ()							{ m_bServerStop = true; }

public:
public:
	friend class CLandMarkPage;

public:
	BOOL LoadWldFile ( const char* szWldFile, bool bLandGateLoad );
	BOOL LoadNaviBypassFile( const char* szNaviFile );
	void ClearNaviBypassFile();

	HRESULT CreateLandTree ();

	//void CleanUp (); �ܼ��� �ϴ� ���� ResetMap()�� ȣ���ϴ� �� ���̹Ƿ�, ResetMap() �� ��ü�� ��ü�� �ȴ�.

protected:
    //! Item ------------------------------------------------------------------
    ITEM_DROP_MAP           m_ItemDropData; //! �ٴڿ� �����ִ� �����۵�

    //! Money -----------------------------------------------------------------
    MONEY_DROP_MAP          m_MoneyDropData; //! �ٴڿ� �����ִ� ���ӸӴ�

    //! Crow ------------------------------------------------------------------
    CROW_DROP_MAP           m_CrowDrop;

    //! Material --------------------------------------------------------------
    MATERIAL_DROP_MAP       m_MaterialDrop;

    //! Summon ----------------------------------------------------------------
    SUMMON_DROP_MAP         m_SummonDrop;	

    //! Event -----------------------------------------------------------------
	GLLandEventServer*      m_pLandEvent;				//  LandEvent

	MAPMOBGROUP				m_mapMobGroup;				//  ���� �׷� ����
	MAPGROUPINFO			m_mapGroupInfo;				//  ���� �׷� ���� ����

	GLCountRecorder*				m_pCountRecorder;
public:
	GLCountRecorder* const GetCountRecorder(void) const { return m_pCountRecorder; }
	void AddDropOutCrowCount(const SNATIVEID& sNID);
	void AddDropItemCount(const SNATIVEID& sNID);
	void AddDropMoneyCount(const LONGLONG& lMoney);
	void AddEnterPlayerCount(void);

	// ���� ���� ī��Ʈ ������ ��Ʈ�� �Ѵ�;
	const bool AddCurPlayerCount ( void );
	const bool AddCurPartyPlayerCount ( const DWORD dwID );
	const bool AddCurClubPlayerCount ( const DWORD dwID );
	const bool AddCurCountryPlayerCount ( const DWORD dwID );
	const bool AddCurSchoolPlayerCount ( const DWORD dwID );

	const bool DelCurPlayerCount ( void );
	const bool DelCurPartyPlayerCount ( const DWORD dwID );
	const bool DelCurClubPlayerCount ( const DWORD dwID );
	const bool DelCurCountryPlayerCount ( const DWORD dwID );
	const bool DelCurSchoolPlayerCount ( const DWORD dwID );

	const bool AddCurFactionPlayerCount ( const Faction::FactionID& sFactionID );
	const bool DelCurFactionPlayerCount ( const Faction::FactionID& sFactionID );

	// ���� ���� ī��Ʈ ������ ��������;
	const DWORD GetCurPlayerCount ( void ) const;
	const bool GetCurPartyPlayerCount ( GLCountRecorder::MAP_COUNTER& _refPlayerCounter ) const;
	const bool GetCurClubPlayerCount ( GLCountRecorder::MAP_COUNTER& _refPlayerCounter ) const;
	const bool GetCurCountryPlayerCount ( GLCountRecorder::MAP_COUNTER& _refPlayerCounter ) const;
	const bool GetCurSchoolPlayerCount ( GLCountRecorder::MAP_COUNTER& _refPlayerCounter ) const;
	const bool GetCurFactionPlayerCount ( GLCountRecorder::MAP_FACTION_COUNTER& _refPlayerCounter ) const;

protected:
    CRITICAL_SECTION  m_GlobPCListLock; //! m_GlobPCList ������ ������ �־� �����Ҷ� lock �� �Ǵ�.
    std::set<sc::SGLNODE<GLChar*>* > m_GlobPCListDelList;
    
    void FrameMoveClearDelList();

public:
	GLCHARLIST m_GlobPCList; //! main list of characters in this LandMan.
    const bool AddGlobPcDelList(sc::SGLNODE<GLChar*>* pNode);

public:
	//
	//mjeon
	//
	void					EnterLand(GLChar *pChar);
	
	GLPETLIST				m_GlobPETList;
	DROP_CROW_LIST	m_DropCrowList;

	void UpdateWeather();

protected:	
	ARRAY_CHAR				m_vTempPC;
	ARRAY_CHAR				m_vDetectPC;

	ARRAY_CROW				m_vTempCROW;
	ARRAY_CROW				m_vDetectCROW;

	VEC_SK_TAR				m_vecDetectID;

protected:
	LISTSACT				m_listSkillAct;
	LISTSACTEX				m_listSkillActEx;
	
	TriggerSystem::SpTriggerFinger m_spTriggerFinger; // Ʈ���� �۵���

protected:
	void UpdateSkillAct ( float fElapsedTime );

public:
	HRESULT SetMap ( BOOL bLevelEdit = FALSE );
	HRESULT ReSetMap ();

public:
    Faction::ManagerField* const GetFactionManager(void) const;
    InstanceSystem::InstanceField* const GetScriptInstance(void) const;

	BOOL ImportLayer(const char* szFile, MapAxisInfoMan* pAxisMan, BOOL bExceptEffect);
	BOOL LoadFile(const char* szFile, MapAxisInfoMan* pAxisMan);
	BOOL CopyLandManData( GLLandMan *pBaseData );
	void CleanUpMobSchMan(GLMobScheduleManServer* pMobSchMan);

	void TriggerStop();

//     //! ������ ���, Ex���������� �� �����.
//     void ClearCrowAndSchedule();
//     void AddMobSchMan(UINT index);

public:
	/// Object Layer ����;
    typedef std::map< UINT, SpMobScheduleManServer >	MOB_SCH_MAN_MAP;
    typedef MOB_SCH_MAN_MAP::iterator					MOB_SCH_MAN_MAP_ITER;
    typedef MOB_SCH_MAN_MAP::const_iterator				MOB_SCH_MAN_MAP_CITER;
    typedef MOB_SCH_MAN_MAP::value_type					MOB_SCH_MAN_MAP_VALUE;

    typedef std::list< SpMobScheduleManServer >			MOB_SCH_MAN_LIST;
    typedef MOB_SCH_MAN_LIST::iterator					MOB_SCH_MAN_LIST_ITER;
    typedef MOB_SCH_MAN_LIST::const_iterator			MOB_SCH_MAN_LIST_CITER;

	/// �̹� �߰��� ���̾��� ��쿡�� ���� ����ġ �ʰ� TRUE �� ��ȯ�Ѵ�;
	// Layer �߰�;
    BOOL AddMobSchManLayer ( UINT nLayerIndex, BOOL bOnce = false );

	// Ư�� Base Layer �� Mob �� �����Ͽ� Once Layer �� �ٿ��ִ´�;
	// ���� �ʿ��� ��찡 �ƴ϶�� ������� ���� ( �޸� ���� );
	//void CopyBaseLayer ( UINT nOnceLayerIndex, UINT nBaseLayerIndex );

	// Once Layer ����;
	void DelMobSchManOnceLayer ( UINT nLayerIndex, const bool bDelete = false );					// Ư�� Once Layer �� �����Ѵ�;
	void DelAllMobSchManOnceLayer ( const bool bDelete = false );									// ��� Once Layer �� �����Ѵ�;

	// Base Layer ����;
    void SetMobSchManRegenLayer ( UINT nLayerIndex );												// ��� Base Layer �� �����ϰ� ������ Base Layer �ϳ��� �����Ѵ�;
    void DelMobSchManRegenLayer ( UINT nLayerIndex, const bool bDelete = false );					// Ư�� Base Layer �� �����Ѵ�;
    void DelAllMobSchManRegenLayer ( const bool bDelete = false );									// ��� Base Layer �� �����Ѵ�;
    
	// ��� Layer ����;
    void DelAllMobSchLayer ( const bool bDelete = false );

	// ���� �����̳ʿ� ��ϵ� �����췯�� ��� �����Ѵ�;
	HRESULT UpdateRemoveMobSchManLayer ( void );

private:
    /** ���� Object �����췯 �����̳�;
        �ش� �����̳ʴ� Data �� ���õǾ� �ִ� Base Layer �� �״�� �ҷ��� ����ϴ� �����̳��̴�;
		������ �����췯�̱� ������ ��Ʈ���� �ʿ����� ���� Object ���� �����Ͽ� ����ϴ� ���� ����;
		�����췯 �����͸� �״�� �ҷ��� ����ϱ� ������ ��뿡 ���ǰ� �ʿ��ϴ�;
		Map �� �����Ҷ� �⺻������ 0�� Layer �� �����Ͽ� ����Ѵ�;
    */
    MOB_SCH_MAN_MAP m_mapMobSchManRegen;

    /** ���� Object �����췯 �����̳�;
        ���״�� �������� ���� ���� Once Layer �� �����ϴ� �����̳��̴�;
        ��� ���Ͱ� �Ҹ�Ǹ� �����췯�� �ڵ����� ���Ǿ� �����ȴ�;
    */
    MOB_SCH_MAN_LIST m_mapMobSchManOnce;

	/**	�����췯�� ������ ��� �����ϴ� �����̳�;
		�����췯�� ������ ��� �����Ͽ� ������� �����ǵ��� �ؾ��Ѵ�;
	*/
	VEC_REMOVED_MOB_LAYER m_vecRemovedMobSchManRegen;
	VEC_REMOVED_MOB_LAYER m_vecRemovedMobSchManOnce;
    

public:
	void ADD_GROUPMEMBER ( const std::string& strGroupName, const DWORD dwGlobID );
	void ADD_GROUPLEADER ( const std::string& strGroupName, const DWORD dwGlobID );
	void DEL_GROUPMEMBER ( const std::string& strGroupName, const DWORD dwGlobID );
	void DEL_GROUPLEADER ( const std::string& strGroupName );
	const SETGROUPMEM&	GET_GROUPMEMBER ( const std::string& strGroupName ) const;
	PGLCROW				GET_GROUPLEADER  ( const std::string& strGroupName );
	BOOL				IS_GROUPMEMBER ( const std::string& strGroupName, const DWORD dwGlobID );
	STARGETID			GET_GROUPTARGET ( const std::string& strGroupName );
	void				SET_GROUPTARGET ( const std::string& strGroupName, const STARGETID& sTargetID );
	void				RESET_GROUPTARGET ( const std::string& strGroupName );

public:	
	void ReserveDropCrow(
		GLMobSchedule* pMobSchedule, const SNATIVEID sMobLinkID, 
		const float fPositionX, const float fPositionZ, const float fMobLinkScale, const float fMobLinkDelay = 0.0f);

	CItemDrop* DropGenItem ( D3DXVECTOR3 &vPos, const SNATIVEID &sNID, EMITEMGEN emGENTYPE, EMGROUP emGroup=EMGROUP_ONE, DWORD dwHoldGID=GAEAID_NULL );
	CItemDrop* DropItem(const D3DXVECTOR3& vPos, const SITEMCUSTOM* pItemCustom, EMGROUP emGroup=EMGROUP_ONE, DWORD dwHoldGID=GAEAID_NULL);
	BOOL DropMoney ( D3DXVECTOR3 &vPos, LONGLONG lnAmount, bool bDropMonster = FALSE, EMGROUP emGroup=EMGROUP_ONE, DWORD dwHoldGID=GAEAID_NULL );

	DWORD DropCrow ( GLMobSchedule *pMobSch, bool bFirstCreateForInstMap );
	DWORD DropCrow ( GLMobSchedule *pMobSch, SNATIVEID sNID, FLOAT fPosX, FLOAT fPosY, FLOAT fScale );
	DWORD DropCrow ( SNATIVEID sNID, WORD wPosX, WORD wPosY, float fRotate = 0.0f, DWORD dwLayerIndex = 0 );
	DWORD DropCrow ( SNATIVEID sNID, float fPosX, float fPosY, float fRotate = 0.0f, DWORD dwLayerIndex = 0 );
    void  DropCrowPreCreate (PGLCROW pNewCrow );

	DWORD DropMaterial( GLMobSchedule *pMobSch, bool bFirstCreateForInstMap );
	DWORD DropMaterial( SNATIVEID sNID, WORD wPosX, WORD wPosY );
	DWORD DropMaterial( SNATIVEID sNID, float fPosX, float fPosY );
	
	DWORD DropSummon ( PGLSUMMON pInSummonInfo, const D3DXVECTOR3& vPos, const STARGETID& sTarget, DWORD dwMaxSummon = UINT_MAX, DWORD dwAIType=0, bool bEventSkill=false );

	BOOL DropOutItem( DWORD dwGlobID, bool DbUpdate = false );
	BOOL DropOutMoney ( DWORD dwGlobID );
	BOOL DropOutCrow ( DWORD dwGlobID = MAXCROW );
	BOOL DropOutMaterial ( DWORD dwGlobID = MAXCROW );
	BOOL DropOutSummon ( DWORD dwGlobID, const BOOL bErrlog=TRUE );
	BOOL DropSaveCommission();

	// Char
	BOOL DropChar ( PGLCHAR pChar );
	BOOL RemoveChar ( PGLCHAR pChar );

	// PET
	BOOL RegistPet ( PGLPETFIELD pPet );
	BOOL RemovePet ( PGLPETFIELD pPet );
	BOOL MovePet ( DWORD dwGUID, const D3DXVECTOR3 &vPos );

	BOOL MoveCrow ( DWORD dwGlobID,   const D3DXVECTOR3 &vPos );
	BOOL MoveChar ( DWORD dwGaeaID,   const D3DXVECTOR3 &vPos );
	BOOL MoveSummon ( DWORD dwGlobID, const D3DXVECTOR3 &vPos );

	// LandEvent
	template <typename LANDEVENTNODE>
	LANDEVENTNODE*  	DropLandEvent( DWORD& dwOutGUID, const Faction::GLActorID& sOwnerID, const D3DXVECTOR3& vInPos, const FLOAT fEndTime, const FLOAT fEventTime );
    const BOOL      	DropOutLandEvent ( DWORD dwGUID );

	CItemDrop* GetDropItem(DWORD dwGlobID) const;
    void       SetDropItem(DWORD dwGlobID, CItemDrop* pDropItem);
    void       ResetDropItem(DWORD dwGlobID);

	CMoneyDrop* GetDropMoney(DWORD dwGlobID) const;
    void        SetDropMoney(DWORD dwGlobID, CMoneyDrop* pDropMoney);
    void        ResetDropMoney(DWORD dwGlobID);

    unsigned int GetCrowListCount() const;
	GLCrow* GetCrow(DWORD dwID) const;
	GLCrow* GetCrowByID(const SNATIVEID& sID) const;
	GLCrow* GetCrowByID(const SNATIVEID& sID, WORD wPosX, WORD wPosY) const;
    void    SetCrow(DWORD dwID, GLCrow* pCrow);
    void    ResetCrow(DWORD dwID);

	GLMaterial* GetMaterial(DWORD dwID) const;
    void        SetMaterial(DWORD dwID, GLMaterial* pMaterial);
    void        ResetMaterial(DWORD dwID);

	GLSummonField* GetSummon(DWORD dwID) const;
    void           SetSummon(DWORD dwID, GLSummonField* pSummon);
    void           ResetSummon(DWORD dwID);    

	GLChar* GetChar(DWORD dwID);
    GLChar* GetCharByDbNum(DWORD dwCharDbNum);
    ServerActor* const GetActor(const Faction::GLActorID& _actorID);
	ServerActor* const GetActor(const STARGETID &sTargetID);

    CEVENT_NODE_SERVER* GetLandEvent ( DWORD dwGlobID ) const;

	HRESULT				ClearDropObj ();
	// �ɸ��͸� ������ ������Ʈ ����
	HRESULT				ClearExptChaObj ();

public:
	BOOL 				RequestGenItemFieldFB  ( GLMSG::SNET_REQ_GENITEM_FLD_FB  * pNetMsg );
	BOOL 				RequestNonConfrontMode ( GLMSG::SNET_GM_NONCONFRONT_MODE * pNetMsg );

public:
	void				SendSkillAct( const SSKILLACT& sACT, ServerActor* pACTOR, ServerActor* pREACTOR, int nVAR_HP, bool damageswap = false );
	void 				DoSkillAct ( const SSKILLACT &sACT );
	void 				DoSkillActEx ( const SSKILLACTEX &sACTEX );
	void 				DoSkillActExII ( const SSKILLACTEX &sACTEX, const SSKILLACTEX &sCAACTEX );

public:
	void 				RegSkillAct ( const SSKILLACT &sACT );
	void 				RegSkillActEx ( const SSKILLACTEX &sACTEX );
	void 				RegSkillActExII ( const SSKILLACTEX &sACTEX, const SSKILLACTEX &sCAACTEX );

public:

	bool				GetPushPullPos( D3DXVECTOR3& vDestPos, ServerActor* pACTOR, ServerActor* pREACTOR, const float fPUSH_PULL );
	void				SwapPosAct( const STARGETID& dwActorID, const STARGETID& dwReActorID, const float fPUSH_SPEED, DWORD dwSubAni, DWORD dwSwapType );
	void				PushPullAct( const STARGETID& dwActorID, const STARGETID& dwReActorID, const float fPUSH_PULL, float fPUSH_SPEED, DWORD dw );

public:
	PGLCHAR				FindEnemyChar ( const PGLCROW pFinder );

	//ARRAY_CHAR* 		DetectPC ( const D3DXVECTOR3 &vDetectPosA, const D3DXVECTOR3 &vDetectPosB, const DWORD dwMaxFind );
	//ARRAY_CHAR* 		DetectPC ( const D3DXVECTOR3 &vDetectPos, const int nRange, const DWORD dwMaxFind );
	ARRAY_CROW* 		DetectCROW ( const D3DXVECTOR3 &vDetectPos, const int nRange, const DWORD dwMaxFind );

	VEC_SK_TAR*			Detect_CROW_PC ( const D3DXVECTOR3 &vDetectPosA, const D3DXVECTOR3 &vDetectPosB );
	VEC_SK_TAR*			Detect_CROW_PC ( const D3DXVECTOR3 &vDetectPos, const int nRange );
	VEC_SK_TAR*			Detect_CROW_MOB ( const D3DXVECTOR3 &vDetectPos, const int nRange );
	VEC_SK_TAR*			Detect_CROW_SUMMON ( const D3DXVECTOR3 &vDetectPos, const int nRange );
	VEC_SK_TAR*			Detect_CROW ( DWORD emCrow, const D3DXVECTOR3 &vDetectPosA, const int nRange );

//	const bool SelectSkillTarget(const STARGETID& _agentID, const STARGETID& _targetID, const SNATIVEID& _skillID, STARID _targetIDs[EMTARGET_NET]);

	LANDQUADNODE*		FindCellNode ( int nx, int nz );
	//HRESULT 			SendMsgPCViewAround ( int nX, int nZ, GLChar* const pReqChar, const LPNET_MSG_GENERIC &nmg ); // ����� �ȵǰ� ����.
	HRESULT 			SendMsgPC ( const LPNET_MSG_GENERIC &nmg );
    HRESULT 			SendMsgPC ( DWORD dwCharDbNum, const LPNET_MSG_GENERIC &nmg );
    HRESULT 			SendMsgPC ( EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc=false );    
    HRESULT 			SendMsgPC ( DWORD dwCharDbNum, EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc=false );
    HRESULT             SendMsgMySelf ( DWORD dwCharDbNum, EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc=false );
    HRESULT 			SendMsgAgent ( EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc=false );

	BOOL 				IsCollisionNavi ( D3DXVECTOR3 &vPoint1, D3DXVECTOR3 &vPoint2, D3DXVECTOR3 &vCollision );
	BOOL 				IsCollision ( const D3DXVECTOR3 &vP1, /*const*/ D3DXVECTOR3 &vP2, EMCOLLMODE emCullMode );

	BOOL 				DoGateOutPCAll ( DWORD dwExceptClubID, DWORD dwGateID );
	BOOL 				DoGateOutPCAll ( DWORD dwGateID );

	BOOL 				DoMapEffectPC ( DWORD dwClubID, SMAP_EFFECT& sMapEffect );

public:
	void 				GetNaviMeshAABB ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin ) { vMax = m_vMax; vMin = m_vMin; }

    void                StartTriggerFinger(int nStage, int nLevel, const std::string& strGuid);
	void				RemoveTriggerFinger();
	void				SendTriggerMessage(TriggerSystem::SMessage* msg);
	void				PostTriggerMessage(TriggerSystem::SpMessage msg);

protected:
	HRESULT UpdateItem(float fTime, float fElapsedTime);
	HRESULT UpdateCrow(float fTime, float fElapsedTime);
    void UpdateMaterial(float fTime, float fElapsedTime);
    void UpdateCrowNormal(float fTime, float fElapsedTime);
	HRESULT UpdateReserveDropCrow(float fTime, float fElapsedTime);
    HRESULT	UpdateSummon(float fTime, float fElapsedTime);

public:
	void				CHECKVALID();

public:
	HRESULT 			InitDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice);
	HRESULT 			RestoreDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice);

	HRESULT 			InvalidateDeviceObjects();
	HRESULT 			DeleteDeviceObjects();

public:

	HRESULT 			FrameMove(float fTime, float fElapsedTime, __time64_t &CurrentTime); //FRAMEMOVE_TIMEVARS &fmtv
	HRESULT				RendQuad ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT 			Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv );
	void				ExtraFrameMoveInstantMap ( float fElapsedTime );	//mjeon.tbb.renamed
	

	//
	//mjeon
	// InstantMap�� TutorialMap�� ó���� ��ġ���� �Ѵ�. (���ʿ��� �ڵ��ߺ�, ���� ��)
	//
	BOOL				CanBeDeleted(float fElapsedTime);	// �� Instant LandMan�� �����ص� �Ǵ°�?
	BOOL				Empty();							// �� Instant LandMan�� ������ �������� �ʴ°�?
	BOOL				MoveOutInstantMap( PGLCHAR pChar );


	//
	//mjeon.CaptureTheField
	//
	//  ��������� HP�� �����п� ������ RESET�Ѵ�.
	//
	void	ResetCrow4CTF();

    
    /**
		�ʿ��� �����Ǵ� ���͵��� ���� ������ ����� ���� �Ѵ�.
        HP, ���ݷ�, ����, ����ġ, ������ ����� ����� ���� �� �� �ִ�.
        �δ��� ���̵��� ������ ���� ����� ����.

     */
    void                SetCrowMultipleHP(float fMultiple);
    const float         GetCrowMultipleHP() const;
    void                SetCrowMultipleAttack(float fMultiple);
    const float         GetCrowMultipleAttack() const;
    void                SetCrowMultipleDefense(float fMultiple);
    const float         GetCrowMultipleDefense() const;
    void                SetCrowMultipleExp(float fMultiple);
    const float         GetCrowMultipleExp() const;
	void                SetCrowMultipleDrop(float fMultiple);
	const float         GetCrowMultipleDrop() const;

    /**
        ���� ���� ��� ������(CharDbNum)�� ��´�.

     */
    void                CollectPC(CHARDBNUM_LIST& list);

    /**
        PVE �������� �δ��� ���� ID �� ���� �Ѵ�.
        ������ ���� �Ǿ� �δ� ����� ��� �Ѵ�.        

     */
    void                SetPVEIndunID(const SPVEInDunID id) { m_sPVEIndunID = id; }
    const SPVEInDunID   GetPVEIndunID() const { return m_sPVEIndunID; }

    /**
        ������ ����� ��ġ�� �̵� ��Ų��.

     */
    void                Teleport(const DWORD dwCharDbNum);
	BOOL				IsObstacle(  D3DXVECTOR3 vThisPos, D3DXVECTOR3 vPreviousPos );
	BOOL				IsInObstacle( float fRadius, float fWidth, float fLength );
	//BOOL				ISCollisionSphereToLine( D3DXVECTOR3 vSphereCenter,
	//											 float fRadius, 
	//											 D3DXVECTOR3 vLineBegin,
	//											 D3DXVECTOR3 vLineDirecton);
	BOOL				IsIn( float fRadius, float fLength );

protected:
    float m_fCrowMultipleHP;	    //@ HP ���
    float m_fCrowMultipleAttack;	//@ ���ݷ� ���
    float m_fCrowMultipleDefense;   //@ ���� ���
    float m_fCrowMultipleExp;       //@ ����ġ ���
	float m_fCrowMultipleDrop;      //@ ������ ����� ���

public:
	SkillTargetSystem m_SkillTargetSystem;

public:
	// Layer �� Ȱ��ȭ �Ǿ� �ִٸ� Layer Effect Rendering �� �϶�� Client ���� �����ش�.
	void SendLayerActiveInfo();
};

typedef GLLandMan*				PGLLANDMAN;

typedef sc::CGLLIST<PGLLANDMAN>		GLLANDMANLIST;
typedef sc::SGLNODE<PGLLANDMAN>		GLLANDMANNODE;

//--------------------------------------------------------------------------------------------
template <typename LANDEVENTNODE>
LANDEVENTNODE* GLLandMan::DropLandEvent(
    DWORD& dwOutGUID,
    const Faction::GLActorID& sOwnerID,
    const D3DXVECTOR3& vInPos,
    const FLOAT fEndTime,
    const FLOAT fEventTime)
{
    return m_pLandEvent->NewEvent<LANDEVENTNODE>(dwOutGUID, sOwnerID, this, vInPos, fEndTime, fEventTime);
}

class GLLandManager
{
    friend class GLLandMan;
public:
    void frameMove(const float _fTime, const float _fElapsedTime, __time64_t& _currentTime);
    void sendMsgPC(NET_MSG_GENERIC* _pMessage);
    void sendMsgPC(EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc = false);
    void sendMsgFaction(const Faction::FactionID _factionID, NET_MSG_GENERIC* _pMessage);
    void sendMsgFaction(const Faction::FactionID _factionID, EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc = false);
    const bool loadLevelFile(const DWORD _nSubMapID, const char* szFile, MapAxisInfoMan* pAxisMan);


    GLLandMan* const getLand(const DWORD _nSubMapID = 0) const;
    GLLandMan* const getLand(const SNATIVEID& _mapID) const;
    const unsigned int getInstanceMapMainID(void) const;
    const unsigned int getBaseMapMainID(void) const;    
	const unsigned int getNStage(void) const;
	const int getUsedMemory(void) const;

    const bool addLand(GLLandMan* const _pBaseLand = 0);
    const bool setPositionInstance(const Faction::GLActorID& _actorID, const Faction::GLActorID& _targetActorID, const SNATIVEID& _targetGaeaMapID, const D3DXVECTOR3& _targetPosition, const bool bResurrect = false);
    const bool dropCrow(const DWORD _nSubMapID, const SNATIVEID& _nativeCrowID, const WORD _nPositionX, const WORD _nPositionZ);
    const bool dropCrow(const DWORD _nSubMapID, const SNATIVEID& _nativeCrowID, const float _fPositionX, const float _fPositionZ);	

    GLChar* const				getCharacter(const DWORD _dwGaeaID) const;
    GLCrow* const			getCrow(const DWORD _dwGaeaID) const;
    CItemDrop* const		getItem(const DWORD _dwGaeaID) const;
    CMoneyDrop* const		getMoney(const DWORD _dwGaeaID) const;
    GLSummonField* const	getSummon(const DWORD _dwGaeaID) const;
    GLMaterial* const			getMaterial(const DWORD _dwGaeaID) const;
    ServerActor* const		getActor(const Faction::GLActorID& _actorID) const;

	void addUsedMemory(const int _nSize);
    void setScriptInstance(InstanceSystem::InstanceField* _pScriptInstance);
    void createFaction(const Faction::FactionID& _factionID);
    void setFactionID(const Faction::GLActorID& _actorID, const Faction::FactionID& _factionID);
    void setRelation(const Faction::FactionID& _factionID, const Faction::FactionID& _targetFactionID, const Faction::EMRELATION_TYPE _emRelation);
    void setRelationToAll(const Faction::FactionID& _factionID, const Faction::EMRELATION_TYPE _emRelation);
    void setRelationFromAll(const Faction::FactionID& _factionID, const Faction::EMRELATION_TYPE _emRelation);
    void setHpVisible(const Faction::EMRELATION_FLAG_TYPE _emHpVisible);
	void setRawDataMap(void);
    Faction::ManagerField* GetFactionManager(void) { return this->pFactionManager; }
    InstanceSystem::InstanceField* GetScriptInstance(void);

    void removeLand(const unsigned int _nSubMapID);

    const sc::MGUID& getGUID() const {return landGUID;};

    // ----------- old version -----------//;
    const bool addLand(const unsigned int _nSubMapID, const bool _bPeaceZone, const bool _bPKZone, GLLandMan* const _pBaseLand = 0);	
    const bool isCopied(void) const;
    const bool isOldInstantMap(void) const;
    const bool isNewInstantMap(void) const;
    const bool isEnableDelete(const float _fElapsedTime);
    void setInstantMapData(const unsigned int _nGaeaID, const GLPartyID& _partyID);
    void setInformation(LPDIRECT3DDEVICEQ pd3dDevice, const bool _bEmulator);	
    void setMapState(void);
    void extraFrameMoveInstantMap(const float _fElapsedTime);
    void StartTriggerFinger(int nStage, int nLevel, const std::string& strGuid);
    void RemoveTriggerFinger();
    void SendTriggerMessage(TriggerSystem::SMessage* msg);
    void PostTriggerMessage(TriggerSystem::SpMessage msg);	
    void clearObjectExceptCharacter(void);
    void clearDropObject(void);
    void render(LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv);
    // ----------- old version -----------//;	

public:
	// ���� ���� ī��Ʈ ������ ��������;
	const DWORD GetCurPlayerCount ( void ) const;
	const bool GetCurPartyPlayerCount ( GLCountRecorder::MAP_COUNTER& _refPlayerCounter ) const;
	const bool GetCurClubPlayerCount ( GLCountRecorder::MAP_COUNTER& _refPlayerCounter ) const;
	const bool GetCurCountryPlayerCount ( GLCountRecorder::MAP_COUNTER& _refPlayerCounter ) const;
	const bool GetCurSchoolPlayerCount ( GLCountRecorder::MAP_COUNTER& _refPlayerCounter ) const;
	const bool GetCurFactionPlayerCount ( GLCountRecorder::MAP_FACTION_COUNTER& _refPlayerCounter ) const;

public:
	inline const bool IsActiveFactionChatting () const { return m_bActiveFactionChatting; }
	inline void ActiveFactionChatting () { m_bActiveFactionChatting = true; }
	inline void InactiveFactionChatting () { m_bActiveFactionChatting = false; }

public:
    GLLandManager(GLGaeaServer* const _pGaeaServer, const unsigned int nInstanceMapID, const unsigned int _nBaseMapID);
    GLLandManager(const GLLandManager& _rhs, const unsigned int nInstanceMapID);
    ~GLLandManager(void);

private:
    GLGaeaServer* const pGaeaServer;
    const unsigned int nInstanceMapMainID; // instance�� base map�� ��� NULL �̴�;
    const unsigned int nBaseMapID;
	bool bRawDataMap;

    boost::circular_buffer<unsigned int> freeEventID;
    boost::circular_buffer<unsigned int> freeSummonID;
    boost::circular_buffer<unsigned int> freeMaterialID;
    boost::circular_buffer<unsigned int> freeCrowID;	
    boost::circular_buffer<unsigned int> freeItemID;	
    boost::circular_buffer<unsigned int> freeMoneyID;

    sc::BOOST_OBJECT_POOL_LOCK_CHECKER<CItemDrop>	itemMemoryPool;//sc::ObjectPool<CItemDrop> itemMemoryPool;//sc::CMemPool<CItemDrop> itemMemoryPool;
    sc::BOOST_OBJECT_POOL_LOCK_CHECKER<CMoneyDrop>	moneyMemoryPool;//sc::ObjectPool<CMoneyDrop> moneyMemoryPool;//sc::CMemPool<CMoneyDrop> moneyMemoryPool;


    CROW_DROP_MAP dropCrowReferenceMap; //CROW_DROP_MAP_ITER; CROW_DROP_MAP_CITER;
    ITEM_DROP_MAP dropItemReferenceMap; //ITEM_DROP_MAP_ITER; ITEM_DROP_MAP_CITER;
    MONEY_DROP_MAP dropMoneyReferenceMap; //MONEY_DROP_MAP_ITER; MONEY_DROP_MAP_CITER;
    MATERIAL_DROP_MAP dropMaterialReferenceMap; //MATERIAL_DROP_MAP_ITER; MATERIAL_DROP_MAP_CITER;
    SUMMON_DROP_MAP dropSummonReferenceMap; //SUMMON_DROP_MAP_ITER; SUMMON_DROP_MAP_CITER;

    InstanceSystem::InstanceField*	pScriptInstance;		// �ű� �δ� ��ũ��Ʈ;
    Faction::ManagerField* pFactionManager;		//  ���� ���� �Ŵ���;
    GLLandMan* pLand[MAX_LAND_SID];
    unsigned int nStage;
	int nUsedMemory; // �뷫���� �޸� �����̴�, ���� �ŷ��ؼ��� �ȵȴ�(MB);

    sc::MGUID landGUID;

	bool m_bActiveFactionChatting;
};




class LandPoolField : public CSingletone<LandPoolField>
{
protected:
	friend CSingletone;

public:
    GLLandMan* const newLand(GLGaeaServer* const _pGaeaServer, GLLandManager* const _pLandManager, const SNATIVEID& _baseMapID, const SNATIVEID& _instanceMapID);
    GLLandMan* const newLand(const GLLandMan* const _pLand, GLLandManager* const _pLandManager, const SNATIVEID& _instanceMapID);
    GLLandManager* const newLandManager(GLGaeaServer* const _pGaeaServer, const unsigned int nInstanceMapID, const unsigned int _nBaseMapID);

    void release(GLLandMan* const _pLand);
    void release(GLLandManager* const _pLandManager);	

private:
    __forceinline void _lockLand(void)
    {
        ::EnterCriticalSection(&this->csObjectPoolLand);
    }
    __forceinline void _unlockLand(void)
    {
        ::LeaveCriticalSection(&this->csObjectPoolLand);
    }
	__forceinline void _lockLandManager(void)
	{
		::EnterCriticalSection(&this->csObjectPoolLandManager);
	}
	__forceinline void _unlockLandManager(void)
	{
		::LeaveCriticalSection(&this->csObjectPoolLandManager);
	}

public:
    //static LandPoolField* const getInstance(void);
    ~LandPoolField(void);

private:
    LandPoolField(void);

private:
    CRITICAL_SECTION csObjectPoolLand;
	CRITICAL_SECTION csObjectPoolLandManager;
    sc::BOOST_OBJECT_POOL_CHECKER<GLLandMan> landPool;
    sc::BOOST_OBJECT_POOL_CHECKER<GLLandManager> landManagerPool;
};

#include "GLLandMan.hpp"


#endif // GLLANDMAN_H_
