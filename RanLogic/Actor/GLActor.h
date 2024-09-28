#pragma once

#include "../RanLogic/Character/GLCharDefine.h"
#include "../EngineLib/NaviMesh/actor.h"

#include "../ComponentSystem/IComponentBase.h"

#include "./GLActorDefine.h"

#include <list>

/*
 * dwDbNum : �ɸ��� DB �ε��� 4byte;
 * sNativeID : �� �ε��� 4byte;
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

	virtual const D3DXVECTOR3& RefreshPosition(const float fPrecisionY); // ���� ��ġ = ���� ��ġ + fPrecisionY�� ����;

	virtual const D3DXVECTOR3& GetDirectOrig(void) const; // <- ���� �����ε�, �����ΰ����� ��ü �ص� �ɵ�;
	virtual const D3DXVECTOR3& GetDirect(void) const; // Actor ���� ����;
	virtual const D3DXVECTOR3&	GetPosition(void) const; // ���� ��ġ;		// �ǵ��� ������� �ʵ��� ����.
	virtual const D3DXVECTOR3& GetNextPosition(void) const; // ���� ��ġ;
	virtual const D3DXVECTOR3& GetPositionActor(void) const; // ���� ��ġ;		
	virtual const D3DXMATRIX& GetTransMatrix(void) const; // ���� ���;
	virtual const DWORD GetCurrentCellID(void) const;	
	virtual const NavigationMesh* const GetParentMesh(void) const;
	


	virtual void SetNextWayPoint(const NavigationPath::WAYPOINT& WayPoint); // ���� ��������Ʈ ����;
	virtual void SetPosition(const D3DXVECTOR3& vPosition); // ���� ��ġ;
	virtual const bool SetPositionActor(const D3DXVECTOR3& vPosition, const DWORD dwCurrentCellID = -1);
	virtual void SetDirect(const D3DXVECTOR3& vDirect); // Actor ���� ����;	
	virtual void SetTransMatrix(const D3DXMATRIX& matTrans); // ���� ���;
	virtual void SetMaxSpeed(const float fMaxSpeed);	

	/**
        Actor �� �������� ���� �Ѵ�.
        �������̶� ��ų�� ���� �Ѵ�.
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
	void InsertChildCrow( const Faction::GLActorID& sID );				// ChildCrow �߰�;
	void RemoveChildCrow( const Faction::GLActorID& sID );				// Ư�� ChildCrow ����;
	bool IsEmptyChildCrow();											// ChildCrow Empty ����;
	const DWORD GetChildCrowCount() const;								// ChildCrow ����;

	virtual void RemoveAllChildCrow() { }								// ��� ChildCrow�� ����;
	virtual void KillAllChildCrow() { }									// ��� ChildCrow�� Die���·� ����;

	inline CROWIDLIST& GetChildCrowList() { return m_listCrowID; }		// ChildCrow List;

private:
	void _ResetActorMove(void);

public:
	virtual void Reset(void);

public:
	GLActor(const EMCROW emCrow, const bool bServer = true);
	virtual ~GLActor(void);

protected:
	// Actor�� �Ӽ�(CROW, PC, NPC, MATERIAL, SUMMON); PET�� ���� �ؾ� ��;
	const EMCROW m_emCrow;

	// ���� �̻� ���� �� ���� ���¿� ���� ������ ����;
	ActorState* m_pActorState; // ���� ������ ���� �����;
		
	DWORD m_dwGaeaID; // #1 ����

protected:	// Actor�� �̵� ������ ����;	
	const D3DXVECTOR3 m_vDirectOrig; // ���� ����;
	D3DXVECTOR3 m_vDirect; // ���Ͱ� �ٶ󺸰� �ִ� ����;
	D3DXMATRIX m_matTrans; // ����(�÷��̾�, ��, NPC)�� ���� ���;
	Actor m_actorMove; // �̵� Ŭ����;(���� ��ġ ����)
	bool m_bLockMoveMent;

	// �� �� ������ ��ġ�� ���� ȥ���� �´�. �ʹ� �����ִ°� ����. �ϴ� ���ε��� �Ѵ�.
	// Actor.m_Position �� �ߺ����̰� SetPositionActor() ����ϴ°� �� ���� �����ΰ� ����.
	D3DXVECTOR3 m_vPosition; // ���� ��ġ(���� ��ġ �ƴ�);

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
 * Ran Online�� ����(2013.02.15) ID ����;
 * DWORD : GaeaID, GlobalID, DbNum;
 * EMCROW : CROW_PC, CROW_MOB, CROW_NPC, CROW_SUMMON��;
 * SNATIVEID : sNativeID;
 * STARGETID(EMCROW, GaeaID);
 * 
 * GaeaID�� ������ �ʿ��� ��� �ݵ�� SetGaeaID() �Լ��� ���ؼ� ���� �ؾ� ��;
 *
 * �� �� �ڼ��� ������ MinWiki ����;
 * http://minwiki.mincoms.co.kr/index.php?title=%EB%9E%80_Actor%EB%B3%84_ID_%EC%93%B0%EC%9E%84&action=edit&redlink=1
*/
