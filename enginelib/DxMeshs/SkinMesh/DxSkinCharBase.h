#pragma once

// [shhan][2013.06.03]
//  DxVehicle, DxSkinChar �� ���� Ŭ����
//  DxShadowMap ���� �ߺ��ڵ尡 ���뽺������ �������.
//  MaterialSystem �κи� ���ǰ� ������ ���̴� ���� ������.
//  ���߿� �ð��� ���� ���´ٸ� �۾��ϸ� ������.
//

class DxLandMan;

class DxSkeletonMaintain;

class DxSkinCharBase
{
protected:
	D3DXVECTOR3		m_vDiffuseColor;		// 
	D3DXVECTOR3		m_vPointLightColor;		// 

	//////////////////////////////////////////////////////////////////////////
	// ���⿡�� ������
public:
	void SetWorldLight( const D3DXVECTOR3 &vDiffuseColor, const D3DXVECTOR3 &vPointLight );

	//////////////////////////////////////////////////////////////////////////
	// ���� ���� �Լ�
public:
	virtual HRESULT RenderFixed( const LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPointLightRT, float fAlpha, const BOOL bShadow=FALSE, const BOOL bEff=TRUE, 
								const BOOL bReverse = FALSE, const BOOL bBoardRender = FALSE, BOOL bFogON = FALSE ) = 0;;

	// const BOOL bShadow=FALSE, const BOOL bEff=TRUE, const BOOL bReverse = FALSE, const BOOL bBoardRender = FALSE, BOOL bFogON = FALSE
	virtual void SetPhysX_Cloth( BOOL bOnOff ) = 0;
	virtual void RenderMaterialOpaque( const LPDIRECT3DDEVICEQ pd3dDevice, float fAlpha ) = 0;
	virtual void RenderMaterialHardAlpha( LPDIRECT3DDEVICEQ pd3dDevice, float fAlpha ) = 0;
	virtual void RenderMaterialSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, float fAlpha ) = 0;
	virtual void RenderMaterialEffect( LPDIRECT3DDEVICEQ pd3dDevice, float fAlpha ) = 0;

	virtual HRESULT RenderReflect( const LPDIRECT3DDEVICEQ pd3dDevice, float fAlpha ) = 0;

	virtual HRESULT RenderShadowForMaterialSystem( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matViewProj ) = 0;

	virtual void GetAABBBox( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin ) const = 0;

public:
	DxSkinCharBase(void);
	virtual ~DxSkinCharBase(void);
};


class DxSkinCharEffAniBase
{
public:
	virtual const DxSkeletonMaintain* GetSkeletonMaintain() const = 0;
	virtual void GhostingON( BOOL bGhosting, float fLifeTime ) = 0;
	virtual DWORD GETCURKEYTIME() const = 0;
	virtual DWORD GETUNITTIME () const = 0;
	virtual BOOL RenderMaterialGhosting( LPDIRECT3DDEVICEQ pd3dDevice, float fAlpha, size_t unGhostingIndex ) = 0;
	virtual const TCHAR* GetFileName() const = 0;
	virtual const TCHAR* GetBoneName() const = 0;

public:
	DxSkinCharEffAniBase(void);
	virtual ~DxSkinCharEffAniBase(void);
};




