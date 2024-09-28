#pragma once

#include "../RanLogic/Character/GLCharDefine.h"
#include "../EngineLib/NaviMesh/actor.h"

#include "../ComponentSystem/IComponentBase.h"

#include "./GLActorDefine.h"

#include <list>

/*
 * dwDbNum : 케릭터 DB 인덱스 4byte;
 * sNativeID : 몹 인덱스 4byte;
*/

class NavigationMesh;
class NavigationPath;
class ActorState;
class GLLevelFile;
class LayerCollision;

class GLActor
{
public:
	/************************************************************************/
	/* Component System Map Container                                       */
	/************************************************************************/
	typedef std::map< ComponentSystem::EMFAMILY_ID, ComponentSystem::IComponentBase* >	COMP_MAP;
	typedef COMP_MAP::iterator															COMP_MAP_ITER;
	typedef COMP_MAP::const_iterator													COMP_MAP_CITER;


	/************************************************************************/
	/* Child Crow List Container                                            */
	/************************************************************************/
	typedef std::list< Faction::GLActorID >		CROWIDLIST;
	typedef CROWIDLIST::iterator				CROWIDLIST_ITER;
	typedef CROWIDLIST::const_iterator			CROWIDLIST_CITER;

public:
	virtual const Faction::GLActorID GetActorID() const;

public:
    virtual const DWORD GetCharID() const	{ return 0; }
	virtual const EMCROW GetCrow(void) const;
	virtual const DWORD GetGaeaID(void) const;	
	virtual void SetGaeaID(const DWORD dwGaeaID);

public: // ActorState/Action;
	virtual void SetActState(const DWORD dwState);
	virtual void ReSetActState(const DWORD dwState);
	virtual const bool IsActState(const DWORD dwState) const;
	virtual const DWORD GetActState(void) const;

	virtual void SetAction(const EMACTIONTYPE emAction);
	virtual const EMACTIONTYPE& GetAction(void) const;	
	virtual const bool isAction(const EMACTIONTYPE emAction) const;
	virtual const bool isAction(const EMACTIONTYPE emAction0, const EMACTIONTYPE emAction1) const;
	virtual const bool isAction(const EMACTIONTYPE emAction0, const EMACTIONTYPE emAction1, const EMACTIONTYPE emAction2) const;
	virtual const bool IsValidBody(void) const;
	virtual const bool IsDie(void) const;
	virtual const bool IsIdle(void) const;

public: // ActorMove;
	virtual void InitializeActorMove(const D3DXVECTOR3& vPosition, boost::shared_ptr<NavigationMesh> spNavi, const D3DXVECTOR3& vDirect);
	virtual void InitializeActorMove(const D3DXVECTOR3& vPosition, boost::shared_ptr<NavigationMesh> spNavi);
	virtual void InitializeActorMove(const D3DXVECTOR3& vPosition);
	virtual void RenderActorMove(LPDIRECT3DDEVICEQ pd3dDevice);
	virtual void CreateActorMove(boost::shared_ptr<NavigationMesh> spNavi, const D3DXVECTOR3& vPosition, const bool bInstant = false, const DWORD CurrentCellID = -1);

	HRESULT Update( const std::vector<boost::shared_ptr<LayerCollision>>& vecLayerCollision, float fElapsedTime, BOOL bAccuracyUp = FALSE );
	virtual void StopActorMove(void);
	virtual const bool IsCollision(const D3DXVECTOR3& vPoint1, const D3DXVECTOR3& vPoint2, D3DXVECTOR3& vCollision, DWORD& CollisionID, BOOL& bCollision) const;
	virtual const bool GotoLocation(const D3DXVECTOR3& vPoint, const float fDeviation = 10.0f);
	virtual const bool LineOfSightTest( const D3DXVECTOR3& vPoint, const float fDeviation = 10.0f );
	virtual const bool PathIsActive(void) const;
	virtual const bool GetAABB(D3DXVECTOR3& vMax, D3DXVECTOR3& vMin);	

	virtual const D3DXVECTOR3& RefreshPosition(const float fPrecisionY); // 현재 위치 = 실제 위치 + fPrecisionY값 보정;

	virtual const D3DXVECTOR3& GetDirectOrig(void) const; // <- 기준 벡터인듯, 디파인값으로 대체 해도 될듯;
	virtual const D3DXVECTOR3& GetDirect(void) const; // Actor 방향 벡터;
	virtual const D3DXVECTOR3&	GetPosition(void) const; // 참조 위치;		// 되도록 사용하지 않도록 하자.
	virtual const D3DXVECTOR3& GetNextPosition(void) const; // 다음 위치;
	virtual const D3DXVECTOR3& GetPositionActor(void) const; // 실제 위치;		
	virtual const D3DXMATRIX& GetTransMatrix(void) const; // 월드 행렬;
	virtual const DWORD GetCurrentCellID(void) const;	
	virtual const NavigationMesh* const GetParentMesh(void) const;
	


	virtual void SetNextWayPoint(const NavigationPath::WAYPOINT& WayPoint); // 다음 웨이포인트 지정;
	virtual void SetPosition(const D3DXVECTOR3& vPosition); // 참조 위치;
	virtual const bool SetPositionActor(const D3DXVECTOR3& vPosition, const DWORD dwCurrentCellID = -1);
	virtual void SetDirect(const D3DXVECTOR3& vDirect); // Actor 방향 벡터;	
	virtual void SetTransMatrix(const D3DXMATRIX& matTrans); // 월드 행렬;
	virtual void SetMaxSpeed(const float fMaxSpeed);	

	/**
        Actor 의 움직임을 봉쇄 한다.
        움직임이란 스킬을 포함 한다.
     */
	virtual void SetLockMovement(const bool bLock) { }
	__forceinline const bool isLockMovement(void) const { return m_bLockMoveMent; }

public:
	/************************************************************************/
	/* Component System                                                     */
	/************************************************************************/
	void InsertComponent( ComponentSystem::IComponentBase* pComponent );
	ComponentSystem::IComponentBase* GetComponent( const ComponentSystem::EMFAMILY_ID& emFamilyID );
	const ComponentSystem::IComponentBase* GetComponent( const ComponentSystem::EMFAMILY_ID& emFamilyID ) const;
	void RemoveAllComponent();

public:
	/************************************************************************/
	/* Managing Child Crow                                                  */
	/************************************************************************/
	void InsertChildCrow( const Faction::GLActorID& sID );				// ChildCrow 추가;
	void RemoveChildCrow( const Faction::GLActorID& sID );				// 특정 ChildCrow 제거;
	bool IsEmptyChildCrow();											// ChildCrow Empty 여부;
	const DWORD GetChildCrowCount() const;								// ChildCrow 개수;

	virtual void RemoveAllChildCrow() { }								// 모든 ChildCrow의 제거;
	virtual void KillAllChildCrow() { }									// 모든 ChildCrow를 Die상태로 변경;

	inline CROWIDLIST& GetChildCrowList() { return m_listCrowID; }		// ChildCrow List;

private:
	void _ResetActorMove(void);

public:
	virtual void Reset(void);

public:
	GLActor(const EMCROW emCrow, const bool bServer = true);
	virtual ~GLActor(void);

protected:
	// Actor의 속성(CROW, PC, NPC, MATERIAL, SUMMON); PET도 포함 해야 함;
	const EMCROW m_emCrow;

	// 상태 이상 정보 및 현재 상태에 대한 정보를 가짐;
	ActorState* m_pActorState; // 추후 정리를 위해 묶어둠;
		
	DWORD m_dwGaeaID; // #1 참조

protected:	// Actor의 이동 정보를 가짐;	
	const D3DXVECTOR3 m_vDirectOrig; // 기준 방향;
	D3DXVECTOR3 m_vDirect; // 액터가 바라보고 있는 방향;
	D3DXMATRIX m_matTrans; // 액터(플레이어, 몹, NPC)의 월드 행렬;
	Actor m_actorMove; // 이동 클래스;(절대 위치 포함)
	bool m_bLockMoveMent;

	// 이 값 때문에 위치에 대한 혼동이 온다. 너무 묶여있는게 많다. 일단 나두도록 한다.
	// Actor.m_Position 랑 중복값이고 SetPositionActor() 사용하는게 더 좋은 방향인거 같음.
	D3DXVECTOR3 m_vPosition; // 참조 위치(절대 위치 아님);

private:
	/************************************************************************/
	/* Component System Map Container                                       */
	/************************************************************************/
	COMP_MAP m_mapComponet;

public:
	/************************************************************************/
	/* Child Crow List Container                                            */
	/************************************************************************/
	CROWIDLIST m_listCrowID;
};

/* #1
 * Ran Online의 현재(2013.02.15) ID 쓰임;
 * DWORD : GaeaID, GlobalID, DbNum;
 * EMCROW : CROW_PC, CROW_MOB, CROW_NPC, CROW_SUMMON등;
 * SNATIVEID : sNativeID;
 * STARGETID(EMCROW, GaeaID);
 * 
 * GaeaID의 갱신이 필요한 경우 반드시 SetGaeaID() 함수를 통해서 갱신 해야 함;
 *
 * 좀 더 자세한 내용은 MinWiki 참조;
 * http://minwiki.mincoms.co.kr/index.php?title=%EB%9E%80_Actor%EB%B3%84_ID_%EC%93%B0%EC%9E%84&action=edit&redlink=1
*/
