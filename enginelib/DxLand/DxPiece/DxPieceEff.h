#pragma once

class DxAnimationMan;
class DxStaticMeshLightMap;
class DxFrameMesh;
struct PieceParameter;

#include "../../DxEffect/Frame/DxEffectFrame.h"
#include "../../DxEffect/Single/DxEffSinglePropGMan.h"
#include "../../DxEffect/Single/DxEffSinglePropGMan.h"
#include "../../DxEffect/Single/DxEffSingle.h"

//----------------------------------------------------------------------------------------------------------------------
//									D	x		L	a	n	d		E	f	f
//----------------------------------------------------------------------------------------------------------------------
class DxLandEff
{
protected:
	LPDIRECT3DDEVICEQ m_pd3dDevice;

	// Note : �ʿ��� File Name ����
	std::string			m_strFrameName;

	// Note : ȿ�� ����
	DxEffSinglePropGroupResource	m_sPropGroupRes;
	SPDXEFFSINGLEGROUP				m_spSingleGroup;

	// Note : ��ġ ����
	DxFrame*	m_pFrame;
	D3DXMATRIX	m_matLocal;	// ȿ�� �ڽ��� Matrix		// Save, Load
	D3DXMATRIX	m_matFrame;	// ���� �����ִ� Matrix		// ����
	D3DXMATRIX	m_matLast;	// ���� Matrix				// Edit �ÿ��� ���

protected:
	// Note : ȿ���� �浹 ��ġ�� ���� ����.
	BOOL		m_bGroudPos;	// ������ġ�� �ʿ��Ѱ�?									// SAVE, LOAD
	BOOL		m_bCheckColl;	// 1�� üũ�ϸ� TRUE�� �ٲ��. ���� Collision�� ����.	// NO
	D3DXVECTOR3	m_vCollPos;																// NO

public:
	std::string			m_strEffName;	// Play ȿ�� �̸�
	std::string			m_strEditName;	// Edit ���� ���̴� ȿ�� �̸�

public:
	BOOL CreateEff( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame* pFrame, const D3DXVECTOR3& vPos, 
					const char* szFrameName, const char* szEffName, const char* szEditName );
	void SetMatrix()	{ if( m_pFrame ) m_matFrame = m_pFrame->matCombined; }
	LPD3DXMATRIX GetEffMatrix();
	void ModifyMatrix();
	BOOL ModifyEff( LPDIRECT3DDEVICEQ pd3dDevice, const char* pEffName );

	void SetCheckCollPoint( BOOL bUse )	{ m_bGroudPos = bUse; }
	BOOL GetCheckCollPoint()			{ return m_bGroudPos; }

public:
	void SetAABBBox( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin );

public:
	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice();
	void CleanUp();

public:
	void FrameMove( const float fTime, const float fElapsedTime );
	void Render( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matWorld, DxStaticMeshLightMap* pStaticMeshLightMap, DxFrameMesh* pFrameMesh, float fAlpha );

private:
	// DxEffSinglePropGMan�� ThreadLoading �ϵ��� �ٲپ��� ������ DxSingleGroup�� �ε��ȵǾ����� �ε��� �õ��Ѵ�.
	BOOL CreateDxEffSingleGroup();

	// DxEffSingleGroup �� �����Ѵ�.
	void DeleteDxEffSingleGroup();

public:
	void Save( sc::SerialFile& SFile );
	void LoadEdit( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, DxFrameMesh* pFrameMesh );
	void Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, DxAnimationMan* pAnimation );

	void LoadEdit100( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, DxFrameMesh* pFrameMesh );
	void Load100( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, DxAnimationMan* pAnimation );

public:
	void Export( LPDIRECT3DDEVICEQ pd3dDevice, DxLandEff* pSrcEff, DxAnimationMan* pAniHead );
	void Clone( LPDIRECT3DDEVICEQ pd3dDevice, DxLandEff* pSrcEff );

public:
	DxLandEff();
	~DxLandEff();
};

//----------------------------------------------------------------------------------------------------------------------
//									D	X		P	I	E	C	E		E	F	F
//----------------------------------------------------------------------------------------------------------------------
class DxPieceEff
{
protected:
	static const DWORD VERSION;

	// Note : Replace Eff
protected:
	DWORD			m_dwEffectCount;
	PDXEFFECTBASE	m_pEffectHead;

	// Note : Single Eff
public:
	typedef	std::vector<DxLandEff*>	VECLANDEFF;
	typedef	VECLANDEFF::iterator	VECLANDEFF_ITER;

	VECLANDEFF	m_vecLandEff;

public:
	BOOL SetEff( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame* pFrame, const D3DXVECTOR3& vPos, 
				const char* szFrameName, const char* szEffName, std::string& strEditName );	// strEditName �� Tool ���� ���� ����.
	void DelEff( const char* szEffName );
	LPD3DXMATRIX GetEffMatrix( const char* szEffName );
	void ModifyMatrix();
	void ModifyEff( LPDIRECT3DDEVICEQ pd3dDevice, const char* szEditName, const char* szEffName );

	void SetCheckCollPoint( const char* szEffName, BOOL bUse );
	BOOL GetCheckCollPoint( const char* szEffName );
	const char* GetEffName( const char* pEditName );

protected:
	void CreateEditEffName( const char* pEffName, std::string& strEditName );

public:
	void SetMatrix();
	void SetAABBBox( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin );
public:
	void OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice();
	void OnDestroyDevice();
	void CleanUp();

public:
	void FrameMove( const float fTime, const float fElapsedTime );
	void RenderSingleEff( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matWorld, DxStaticMeshLightMap* pStaticMeshLightMap, DxFrameMesh* pFrameMesh, float fAlpha );
	void RenderMS( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matWorld, PieceParameter* pParameter, BOOL bMPShading );
	void RenderMS_Deffered( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matWorld, PieceParameter* pParameter, BOOL bMPShading, BOOL bReflect );
	void RenderMSForPSF( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matWorld, PieceParameter* pParameter );
	void RenderCascadeShadow( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	void SaveEdt( sc::SerialFile& SFile, DxFrameMesh* pFrameMesh );
	void LoadEdt( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, DxFrameMesh* pFrameMesh );

	void Save( sc::SerialFile& SFile );
	void Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, DxAnimationMan* pAnimation );

	void Load_0101( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, DxAnimationMan* pAnimation );	// 0x0100 ~ 0x0101
	void Load_0102( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, DxAnimationMan* pAnimation );
	void Load_0103( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, DxAnimationMan* pAnimation );	//	������ ���� ���� ����


public:
	void Export( LPDIRECT3DDEVICEQ pd3dDevice, DxPieceEff* pSrcEff, DxFrameMesh* pFrameMesh, DxAnimationMan* pAniHead );
	BOOL Clone( LPDIRECT3DDEVICEQ pd3dDevice, DxPieceEff* pSrcEff );

protected:
	HRESULT EffectAdaptToList( DxFrame *pFrameRoot, DWORD TypeID, char* szFrame, LPDIRECT3DDEVICEQ pd3dDevice,
								PBYTE pProperty, DWORD dwSize, DWORD dwVer, LPDXAFFINEPARTS pAffineParts, DxEffectBase* pEffectSrc, const CMaterialSetting* pMaterialSetting );

	HRESULT EffectLoadToList( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile &SFile, PDXEFFECTBASE &pEffList );

public:
	DxPieceEff ();
	~DxPieceEff();
};