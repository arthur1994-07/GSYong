#ifndef GLANYPET_H_
#define GLANYPET_H_

#pragma once

#include "../../SigmaCore/Container/GLList.h"
#include "../../EngineLib/NaviMesh/Actor.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinChar.h"
#include "../../EngineLib/G-Logic/GLQuadTree.h"

#include "../../RanLogic/Pet/GLPet.h"
#include "../../RanLogic/Skill/GLSkill.h"

#ifndef GLLandManClient
	class GLLandManClient;
	class GLClientNode;
	typedef SQUADNODE<GLClientNode> CLIENTQUADNODE;
#endif //GLLandMan

struct ActorWorldLight;
class GLGaeaClient;

class GLAnyPet : public GLPET
{
public:
    GLAnyPet(GLGaeaClient* pGaeaClient);
    virtual ~GLAnyPet(void);

protected:
    GLGaeaClient* m_pGaeaClient;
	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	Actor				m_actorMove;
	DxSkinChar*			m_pSkinChar;

	D3DXVECTOR3			m_vDir;
	D3DXVECTOR3			m_vDirOrig;
	D3DXVECTOR3			m_vPos;
	D3DXMATRIX			m_matTrans;

	D3DXVECTOR3			m_vTarPos;

public:
	//sc::SGLNODE<GLAnyPet*>*	m_pGlobNode;		//	맵 리스트 노드.
	//CLIENTQUADNODE*		m_pQuadNode;		//	쿼드 트리 노드.
	//sc::SGLNODE<GLAnyPet*>*	m_pCellNode;		//	셀 리스트 노드.

	DWORD				m_dwActionFlag;
	WORD				m_wAniSub;

	float				m_fAttackTimer;
	float				m_fFunyTimer;

public:
	D3DXVECTOR3			m_vMax;
	D3DXVECTOR3			m_vMin;
	float				m_fHeight;

	D3DXVECTOR3			m_vMaxOrg;
	D3DXVECTOR3			m_vMinOrg;

private:
	std::tr1::shared_ptr<ActorWorldLight>		m_spWorldLight;
	std::tr1::shared_ptr<ActorNameCollision>	m_spNameCollision;

public:
	void ASSIGN ( PSDROPPET pDropPet );

	DxSkinChar* GetSkinChar ()							{ return m_pSkinChar; }

	BOOL	IsVisibleDisplayName() const;

	HRESULT Create ( PSDROPPET pDropPet, boost::shared_ptr<NavigationMesh> spNavi, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT SkinLoad ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT UpdateAnimation ( float fTime, float fElapsedTime );
	HRESULT	UpdatePetState ( float fTime, float fElapsedTime );
	HRESULT UpdateSuit ();

	D3DXVECTOR3 GetPosition ()							{ return m_vPos; }
	D3DXVECTOR3 GetDirect()								{ return m_vDir; }
	D3DXMATRIX& GetTransMatrix()						{ return m_matTrans; }

	BOOL IsSTATE ( DWORD dwStateFlag )		 			{ return m_dwActionFlag&dwStateFlag; }
	void SetSTATE ( DWORD dwStateFlag )					{ m_dwActionFlag |= dwStateFlag; }
	void ReSetSTATE ( DWORD dwStateFlag )				{ m_dwActionFlag &= ~dwStateFlag; }
	void ReSetAllSTATE ()								{ m_dwActionFlag = 0; }

	void SetAttackState( const D3DXVECTOR3& vTargetPos );

	bool IsUsePetSkinPack() { return m_sPetSkinPackState.bUsePetSkinPack; }
	const bool IsVisibleDetect(void) const;

	void MsgGoto(NET_MSG_GENERIC* nmg);
	void MsgStop(NET_MSG_GENERIC* nmg);
	void MsgSkillChange(NET_MSG_GENERIC* nmg);
	void MsgUpdateMoveState(NET_MSG_GENERIC* nmg);
	void MsgRename(NET_MSG_GENERIC* nmg);
	void MsgChangeColor(NET_MSG_GENERIC* nmg);
	void MsgChangeStyle(NET_MSG_GENERIC* nmg);
	void MsgChangeAccessory(NET_MSG_GENERIC* nmg);
	void MsgLearnSkill(NET_MSG_GENERIC* nmg);
	void MsgRemoveSlotItem(NET_MSG_GENERIC* nmg);
	void MsgAttack(NET_MSG_GENERIC* nmg);
	void MsgSad(NET_MSG_GENERIC* nmg);
	void MsgFunny(NET_MSG_GENERIC* nmg);
	void MsgAccessoryDelete(NET_MSG_GENERIC* nmg);
	void MsgPetSkinPack(NET_MSG_GENERIC* nmg);
	void MsgSkillSlotExpansion(NET_MSG_GENERIC* nmg);
	void MsgJumpPos(NET_MSG_GENERIC* nmg);

	// Default FrameWork
	HRESULT FrameMove( const boost::shared_ptr<DxLandMan>& spLandMan, float fTime, float fElapsedTime );
	HRESULT Render( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RenderSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RenderMaterialSkinEffect( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RenderShadow( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv, BOOL bWorldSystemOLD );
	HRESULT RenderShadow_SpotLight( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &cv, BOOL bUsedMaterialSystem, DWORD dwSlot );
	HRESULT RenderReflect( LPDIRECT3DDEVICEQ pd3dDevice );

private:
	void RenderMyChar( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matWorld, BOOL bOpaqueState, BOOL bHALF_VISIBLE );

public:
	HRESULT InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT InvalidateDeviceObjects ();
	HRESULT DeleteDeviceObjects ();

	void SK_EFF_SELFZONE ( const SANIMSTRIKE &sStrikeEff, const SKILL::EMEFFECTPOS emPOS, const char* const szZONE_EFF, STARGETID *pTarget );

	void MsgProcess(NET_MSG_GENERIC* nmg);
};

typedef GLAnyPet* PGLANYPET;
typedef sc::CGLLIST<PGLANYPET> GLANYPETLIST;
typedef sc::SGLNODE<PGLANYPET> GLANYPETNODE;

#endif // GLANYPET_H_