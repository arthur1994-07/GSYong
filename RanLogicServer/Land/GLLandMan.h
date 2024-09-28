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

	SNATIVEID	idSKILL;		//	스킬 지속 영향.
	WORD		wSKILL_LVL;		//
    WORD        wSELSLOT;

	float		fPUSH_PULL;		//	스킬 움직이는 거리.
	float		fPUSH_SPEED;	//	스킬 움직이는 속도

	DWORD		dwCUREFLAG;		//	상태 이상 치료.
	SSTATEBLOW	sSTATEBLOW;		//	상태 이상 발생.

    bool		dwRemoveFlag[SKILL::EMACTION_TYPE_NSIZE];	//	버프/디버프 제거
	DWORD		dwAniType;		//	Push_pull 관련 애니메이션 번호

	bool		bOnward;		//	진격 여부


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
	ITEMDROPLIST	m_ItemList;			//	SUIT 아이템 리스트.
	MONEYDROPLIST	m_MoneyList;		//	몹 리스트.

	
	GLCHARLIST		m_PCList;			//	CHAR 리스트

	GLCROWLIST		m_CROWList;			//	CROW 리스트
	GLMATERIALLIST	m_MaterialList;		//	Materail 리스트
	
	// PET
	GLPETLIST		m_PETList;			//  PET 리스트

	// SUMMON
	GLSUMMONLIST    m_SummonList;		//	Summon 리스트

	// LANDEVENT NODE
	LANDEVENTNODESERVERLIST m_LandEventNodeList; // LandEventServerNode 리스트

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
	GLMobSchedule* m_pMobSchedule; // 몬스터 스케쥴러;
	SNATIVEID m_sMobLinkID; // 연결된 몬스터의 ID;
	float m_fPositionXZ[2]; // 몬스터 좌표;
	float m_fMobLinkScale; // 몬스터 크기 비율;
	float m_fMobLinkDelay; // 몬스터 등장 딜레이;
	float m_fIdleTime; // 누적 시간;
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
	enum { INSTANTMAP_DELETE_TIME = 5 }; //5초

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
	CCollisionMap			m_CollisionMap;			//	충돌 맵.
	WEATHER_PROPERTY		m_sWeatherProp;			//	날씨 속성.
	LANDQUADTREE			m_LandTree;				//	Cell Tree.

	// [shhan][2016.04.20] 
	// 3번 연속 server crash 가 발생하여서, Actor::m_Parent 가 단순 참조에서, boost::shared_ptr 로 변경한다.
	//Actor::GotoLocation->NavigationMesh::IsCollision->DxAABBNode::IsCollision
	// 깊이 복사임.
	boost::shared_ptr<NavigationMesh>			m_spNaviMesh;			//	길에 대한 정보를 가진 메쉬	// 깊이복사 됨.



	//////////////////////////////////////////////////////////////////////////
	// shared_ptr 이기 때문에 tbb 에서 사용되면 문제가 될 수 있어서 사용하지는 않는다. ( 정상적으로 read 만 하면 문제 될건 없다. )
	//															{ 사용하려면 write 를 하면 안되는데 .. 코드 확인을 안해봐서 일단 사용하지 말자. )
	// 실제 Navi 를 돌리는 건 m_spNaviMesh 에서 한다.
	// 이건 m_spNaviMesh 값을 만들기 위한 Src 로 보면 됨.
	//
private:
	// wld 에 포함된 Navi
	// 이걸 사용하지는 않음.
	// 실제 길찾기에 쓰이는 건 m_spNaviMesh 를 사용함.
	boost::shared_ptr<NavigationMesh>			m_spNaviMeshInWld;

	// 네비변경 기능이 들어가면서 추가되었음.
	// 이걸 사용하지는 않음.
	// 실제 길찾기에 쓰이는 건 m_spNaviMesh 를 사용함.
	boost::shared_ptr<NavigationMesh>			m_spNaviMeshBypass;
	//
	// shared_ptr 이기 때문에 tbb 에서 사용되면 문제가 될 수 있어서 사용하지는 않는다.
	//////////////////////////////////////////////////////////////////////////


	
public:
	DWORD					m_dwClubMapID;			//	선도 및 CDM ID
	GLGuidance*			m_pGuidance;			//	선도 정보.
	bool					m_bGuidBattleMap;		//	선도 클럽 전투 존.
	bool					m_bGuidBattleMapHall;	//	선도 클럽 전투 존.

	bool					m_bClubDeathMatchMap;			//	클럽데스매치 존
	bool					m_bClubDeathMatchMapHall;		//	클럽데스매치 존

	bool					m_bNonConfrontMode;				// 대련 불가능모드

	DWORD					m_dwGuidClubID;			//	선도 클럽 ID.

	float					m_fElapsedLimitTime;	//  제한시간 업데이트.
	int						m_nLastSendRemainTime;	//  마지막으로 보낸 남은시간.
	bool					m_bExcessiveLimitTime;  //  인던 초과시간 지났는지 체크

protected :
	float					m_fCommissionRate;		//	상업 활동 수수료율.
    LONGLONG                m_llBuyCommissionMoney;	 //	상업 구매 활동 수수료 누적금.;
    LONGLONG                m_llSellCommissionMoney; //	상업 판매 활동 수수료 누적금.;
    float					m_fCommissionLogTimer;	//	상업 활동 수수료 로그 타이머;

public:
	//
	//mjeon.CaptureTheField
	//
	BOOL					m_bCaptureTheField;		// Map for CaptureTheField ?

	BOOL					m_bCopied;				//mjeon.InstantMap의 경우 SrcLandMan으로부터 복사가 정상적으로 되었는가?
	void					CopyDone()	{m_bCopied = TRUE;}
	BOOL					HasCopied()	{return m_bCopied;}

protected:
	DWORD						m_dwOldWeather;
	std::vector<SONEMAPWEATHER> m_vecMapOldWeather;
	bool					    m_bServerStop;			// 서버를 중지하는지

	float					m_fInstantMapDeleteTime;	
    SPVEInDunID             m_sPVEIndunID;

public:
	LISTDELCROWID			m_listDelGlobIDEx;		// 가장 최근에 mob_gen_ex 으로 생성된 몹의 ID
	LISTDELCROWID			m_listDelGlobID;		// 가장 최근에 mob_gen으로 생성된 몹의 ID, 삭제시 필요
	LISTDELMATERIALID		m_listDelGlobIDExMat;	// 가장 최근에 mob_gen_ex 으로 생성된 Meterial의 ID
	LISTDELMATERIALID		m_listDelGlobIDMat;		// 가장 최근에 mob_gen으로 생성된 Material의 ID, 삭제시 필요

public:
	boost::shared_ptr<NavigationMesh>	GetNavi ()			{ return m_spNaviMesh; }
	LANDQUADTREE*						GetLandTree ()		{ return &m_LandTree; }
	CCollisionMap&						GetCollisionMap ()	{ return m_CollisionMap; }

	GLLandManager* const GetParentLandManager(void) { return m_pLandManager; }
	
	// 제한 시간 초과 여부 판단
	BOOL IsExcessiveLimitTime ()					{ return m_bExcessiveLimitTime; }

	BOOL IsWeatherActive () const					{ return m_sWeatherProp.m_dwFlag&ISACTIVE; }

	const bool isRawDataMap(void) const { return m_mapID.isValid() == true ? false : true; }

	// 수수료 설정( 맵리스트의 수수료 설정이 off 되어 있으면 무조건 0 )
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

	//void CleanUp (); 단순히 하는 일은 ResetMap()을 호출하는 것 뿐이므로, ResetMap() 그 자체로 대체가 된다.

protected:
    //! Item ------------------------------------------------------------------
    ITEM_DROP_MAP           m_ItemDropData; //! 바닥에 놓여있는 아이템들

    //! Money -----------------------------------------------------------------
    MONEY_DROP_MAP          m_MoneyDropData; //! 바닥에 놓여있는 게임머니

    //! Crow ------------------------------------------------------------------
    CROW_DROP_MAP           m_CrowDrop;

    //! Material --------------------------------------------------------------
    MATERIAL_DROP_MAP       m_MaterialDrop;

    //! Summon ----------------------------------------------------------------
    SUMMON_DROP_MAP         m_SummonDrop;	

    //! Event -----------------------------------------------------------------
	GLLandEventServer*      m_pLandEvent;				//  LandEvent

	MAPMOBGROUP				m_mapMobGroup;				//  몬스터 그룹 관리
	MAPGROUPINFO			m_mapGroupInfo;				//  몬스터 그룹 리더 관리

	GLCountRecorder*				m_pCountRecorder;
public:
	GLCountRecorder* const GetCountRecorder(void) const { return m_pCountRecorder; }
	void AddDropOutCrowCount(const SNATIVEID& sNID);
	void AddDropItemCount(const SNATIVEID& sNID);
	void AddDropMoneyCount(const LONGLONG& lMoney);
	void AddEnterPlayerCount(void);

	// 현재 유저 카운트 정보를 컨트롤 한다;
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

	// 현재 유저 카운트 정보를 가져간다;
	const DWORD GetCurPlayerCount ( void ) const;
	const bool GetCurPartyPlayerCount ( GLCountRecorder::MAP_COUNTER& _refPlayerCounter ) const;
	const bool GetCurClubPlayerCount ( GLCountRecorder::MAP_COUNTER& _refPlayerCounter ) const;
	const bool GetCurCountryPlayerCount ( GLCountRecorder::MAP_COUNTER& _refPlayerCounter ) const;
	const bool GetCurSchoolPlayerCount ( GLCountRecorder::MAP_COUNTER& _refPlayerCounter ) const;
	const bool GetCurFactionPlayerCount ( GLCountRecorder::MAP_FACTION_COUNTER& _refPlayerCounter ) const;

protected:
    CRITICAL_SECTION  m_GlobPCListLock; //! m_GlobPCList 삭제시 오류가 있어 삭제할때 lock 을 건다.
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
	
	TriggerSystem::SpTriggerFinger m_spTriggerFinger; // 트리거 작동기

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

//     //! 무한의 계단, Ex스테이지용 몹 지우기.
//     void ClearCrowAndSchedule();
//     void AddMobSchMan(UINT index);

public:
	/// Object Layer 관리;
    typedef std::map< UINT, SpMobScheduleManServer >	MOB_SCH_MAN_MAP;
    typedef MOB_SCH_MAN_MAP::iterator					MOB_SCH_MAN_MAP_ITER;
    typedef MOB_SCH_MAN_MAP::const_iterator				MOB_SCH_MAN_MAP_CITER;
    typedef MOB_SCH_MAN_MAP::value_type					MOB_SCH_MAN_MAP_VALUE;

    typedef std::list< SpMobScheduleManServer >			MOB_SCH_MAN_LIST;
    typedef MOB_SCH_MAN_LIST::iterator					MOB_SCH_MAN_LIST_ITER;
    typedef MOB_SCH_MAN_LIST::const_iterator			MOB_SCH_MAN_LIST_CITER;

	/// 이미 추가된 레이어의 경우에는 새로 생성치 않고 TRUE 를 반환한다;
	// Layer 추가;
    BOOL AddMobSchManLayer ( UINT nLayerIndex, BOOL bOnce = false );

	// 특정 Base Layer 의 Mob 을 복사하여 Once Layer 에 붙여넣는다;
	// 정말 필요한 경우가 아니라면 사용하지 말자 ( 메모리 낭비 );
	//void CopyBaseLayer ( UINT nOnceLayerIndex, UINT nBaseLayerIndex );

	// Once Layer 관리;
	void DelMobSchManOnceLayer ( UINT nLayerIndex, const bool bDelete = false );					// 특정 Once Layer 를 제거한다;
	void DelAllMobSchManOnceLayer ( const bool bDelete = false );									// 모든 Once Layer 를 제거한다;

	// Base Layer 관리;
    void SetMobSchManRegenLayer ( UINT nLayerIndex );												// 모든 Base Layer 를 제거하고 설정한 Base Layer 하나만 세팅한다;
    void DelMobSchManRegenLayer ( UINT nLayerIndex, const bool bDelete = false );					// 특정 Base Layer 를 제거한다;
    void DelAllMobSchManRegenLayer ( const bool bDelete = false );									// 모든 Base Layer 를 제거한다;
    
	// 모든 Layer 제거;
    void DelAllMobSchLayer ( const bool bDelete = false );

	// 삭제 컨테이너에 등록된 스케쥴러를 모두 제거한다;
	HRESULT UpdateRemoveMobSchManLayer ( void );

private:
    /** 정적 Object 스케쥴러 컨테이너;
        해당 컨테이너는 Data 에 세팅되어 있는 Base Layer 를 그대로 불러와 사용하는 컨테이너이다;
		정적인 스케쥴러이기 때문에 컨트롤이 필요하지 않은 Object 들을 설정하여 사용하는 것이 좋다;
		스케쥴러 데이터를 그대로 불러와 사용하기 때문에 사용에 주의가 필요하다;
		Map 을 세팅할때 기본적으로 0번 Layer 를 세팅하여 사용한다;
    */
    MOB_SCH_MAN_MAP m_mapMobSchManRegen;

    /** 동적 Object 스케쥴러 컨테이너;
        말그대로 동적으로 쓰기 위한 Once Layer 를 관리하는 컨테이너이다;
        모든 몬스터가 소멸되면 스케쥴러가 자동으로 폐기되어 관리된다;
    */
    MOB_SCH_MAN_LIST m_mapMobSchManOnce;

	/**	스케쥴러를 삭제할 경우 저장하는 컨테이너;
		스케쥴러를 삭제할 경우 저장하여 순서대로 삭제되도록 해야한다;
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
	// 케릭터를 제외한 오브젝트 제거
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
	//HRESULT 			SendMsgPCViewAround ( int nX, int nZ, GLChar* const pReqChar, const LPNET_MSG_GENERIC &nmg ); // 사용이 안되고 있음.
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
	// InstantMap과 TutorialMap의 처리를 합치도록 한다. (불필요한 코드중복, 연산 등)
	//
	BOOL				CanBeDeleted(float fElapsedTime);	// 이 Instant LandMan을 삭제해도 되는가?
	BOOL				Empty();							// 이 Instant LandMan에 유저가 존재하지 않는가?
	BOOL				MoveOutInstantMap( PGLCHAR pChar );


	//
	//mjeon.CaptureTheField
	//
	//  인증기들의 HP와 점령학원 정보를 RESET한다.
	//
	void	ResetCrow4CTF();

    
    /**
		맵에서 생성되는 몬스터들의 세부 사항의 배수를 설정 한다.
        HP, 공격력, 방어력, 경험치, 아이템 드랍율 배수를 설정 할 수 있다.
        인던의 난이도를 설정을 위해 만들어 졌다.

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
        현재 맵의 모든 유저들(CharDbNum)을 얻는다.

     */
    void                CollectPC(CHARDBNUM_LIST& list);

    /**
        PVE 컨텐츠용 인던의 고유 ID 를 설정 한다.
        생성시 설정 되어 인던 진행시 사용 한다.        

     */
    void                SetPVEIndunID(const SPVEInDunID id) { m_sPVEIndunID = id; }
    const SPVEInDunID   GetPVEIndunID() const { return m_sPVEIndunID; }

    /**
        유저를 저장된 위치로 이동 시킨다.

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
    float m_fCrowMultipleHP;	    //@ HP 배수
    float m_fCrowMultipleAttack;	//@ 공격력 배수
    float m_fCrowMultipleDefense;   //@ 방어력 배수
    float m_fCrowMultipleExp;       //@ 경험치 배수
	float m_fCrowMultipleDrop;      //@ 아이템 드랍율 배수

public:
	SkillTargetSystem m_SkillTargetSystem;

public:
	// Layer 가 활성화 되어 있다면 Layer Effect Rendering 을 하라고 Client 에게 보내준다.
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
	// 현재 유저 카운트 정보를 가져간다;
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
    const unsigned int nInstanceMapMainID; // instance의 base map인 경우 NULL 이다;
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

    InstanceSystem::InstanceField*	pScriptInstance;		// 신규 인던 스크립트;
    Faction::ManagerField* pFactionManager;		//  진영 정보 매니저;
    GLLandMan* pLand[MAX_LAND_SID];
    unsigned int nStage;
	int nUsedMemory; // 대략적인 메모리 사용양이다, 절대 신뢰해서는 안된다(MB);

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
