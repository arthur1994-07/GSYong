#ifndef GLMATERIALCLIENT_H_
#define GLMATERIALCLIENT_H_

#pragma once

#include "../../EngineLib/NaviMesh/Actor.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinChar.h"

#include "../../RanLogic/Crow/GLCrowData.h"

#include "../ClientActor.h"

#ifndef GLLandManClient
	class GLLandManClient;
	class GLClientNode;
	typedef SQUADNODE<GLClientNode> CLIENTQUADNODE;
#endif //GLLandMan

struct SDROP_MATERIAL;

class GLMaterialClient : public ClientActor
{
public:
	PCROWDATA			m_pCrowData;					//	Crow �⺻ ����.
	SNATIVEID			m_sNativeID;					//	Crow NativeID.

protected:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	DxSkinChar*			m_pSkinChar;

	//	Note : AABB
protected:
	D3DXVECTOR3		m_vMaxOrg;
	D3DXVECTOR3		m_vMinOrg;

public:
	D3DXVECTOR3		m_vMax;
	D3DXVECTOR3		m_vMin;
	float			m_fHeight;

	//	Note : ���� �ڵ�.
	//
public:
	DWORD				m_dwGlobalID;				//	���� �޸� �ε�����( �ش� GLLandMan������ ����ũ�� )
	DWORD				m_dwCeID;				//	�� ID.

	GLLandManClient*			m_pLandManClient;	//	�� �ν��Ͻ� ������.
	//sc::SGLNODE<GLMaterialClient*>*	m_pGlobNode;		//	�� ����Ʈ ���.
	//CLIENTQUADNODE*				m_pQuadNode;		//	���� Ʈ�� ���.
	//sc::SGLNODE<GLMaterialClient*>*	m_pCellNode;		//	�� ����Ʈ ���.

	//	Note : �ൿ.
protected:
	float				m_fAge;					//	��� �ð�.
//	EMACTIONTYPE		m_Action;				//	�ൿ.	

protected:
	float				m_fIdleTime;			//	�޽� �ð�.

private:
	std::tr1::shared_ptr<ActorWorldLight>		m_spWorldLight;
	std::tr1::shared_ptr<ActorNameCollision>	m_spNameCollision;

public:
//	BOOL IsValidBody();
//	BOOL IsDie();
//	virtual BOOL isAction( EMACTIONTYPE emCur ) const override { return m_Action == emCur; }
	BOOL IsHaveVisibleBody();

	BOOL	IsVisibleDisplayName() const;

protected:
// 	BOOL IsActState( DWORD dwState ) const { return m_dwActState&dwState; }
// 	void SetActState( DWORD dwState )					{ m_dwActState |= dwState; }
// 	void ReSetActState( DWORD dwState )				{ m_dwActState &= ~dwState; }

public:
	HRESULT CreateMaterial( GLLandManClient *pLandManClient, SDROP_MATERIAL *pCrowDrop, LPDIRECT3DDEVICEQ pd3dDevice );

public:
//	void SetPosition( D3DXVECTOR3 &vPos );
//	virtual const D3DXVECTOR3 &GetPosition()		{ return m_vPos; }
	float GetDirection();
	DxSkinChar* GetCharSkin()						{ return m_pSkinChar; }

public:
	void TurnAction( EMACTIONTYPE toAction );

public:	
    virtual const DWORD GetCharID() const	override { return m_dwGaeaID; }
	virtual const char* GetName() const		{ return m_pCrowData->GetName(); }
	virtual GLDWDATA GetHp() const			{ return GLDWDATA(); }

	virtual WORD GetBodyRadius();
	virtual float GetBodyHeight()			{ return m_fHeight; }
	virtual D3DXVECTOR3 GetPosBodyHeight()	{ return GetPosition() + D3DXVECTOR3(0.0f, m_fHeight, 0.0f); }

	virtual const D3DXVECTOR3 &GetMaxOrg() { return m_vMaxOrg; }
	virtual const D3DXVECTOR3 &GetMinOrg() { return m_vMinOrg; }

public:
	virtual void MsgProcess(NET_MSG_GENERIC* nmg);
	virtual void ReceiveDamage(const int nDamage, DWORD dwDamageFlag, WORD wSchool, const STARGETID& sACTOR) override {}
	virtual void ReceiveHeal(const int nHeal, DWORD dwHealFlag, WORD wSchool, const STARGETID& sACTOR) override {}
	virtual void ReceiveBlocking(const STARGETID& sACTOR) override {}
	virtual void ReceiveAVoid( const STARGETID& sACTOR ) override {}
	virtual void ReceiveSwing() override {}

	const BOOL IsNonTargetable(void) const override { return FALSE; }

public:
	virtual void UPDATE_DATA_FINISH(const DWORD dwSkillFactIndex) override {;}	
protected:
	HRESULT UpdateAnimation( float fTime, float fElapsedTime );	

public:
	BOOL IsCollisionVolume();
	HRESULT FrameMove( const boost::shared_ptr<DxLandMan>& spLandMan, float fTime, float fElapsedTime );

	HRESULT RenderShadow( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv, BOOL bWorldSystemOLD );
	HRESULT RenderShadow_SpotLight( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &cv, BOOL bUsedMaterialSystem, DWORD dwSlot );
	HRESULT Render( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv, BOOL bRendAABB );
	HRESULT RenderSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv );
	HRESULT RenderMaterialSkinEffect( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv );

public:
	HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT InvalidateDeviceObjects();

public:
    virtual void FactEffectProcessing ( const EMFACTEFFPROCESSING emFactEffactCase ) {}

public:
	GLMaterialClient(GLGaeaClient* const pGaeaClient);
	~GLMaterialClient(void);
};

typedef GLMaterialClient* PGLMATERIALCLIENT;

typedef sc::CGLLIST<PGLMATERIALCLIENT>	GLMATERIALCLIENTLIST;
typedef sc::SGLNODE<PGLMATERIALCLIENT>	GLMATERIALCLIENTNODE;

#endif // GLMATERIALCLIENT_H_
