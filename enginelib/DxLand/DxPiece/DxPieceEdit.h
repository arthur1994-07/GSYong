#pragma once

#include <boost/function.hpp>

#include "./DxPieceDef.h"

class SUBPATH;
struct DxFrame;
struct CLIPVOLUME;
class DxFrameMesh;
class DxPieceEff;
class DxStaticAniFrame;
class DxPieceTexEff;
class CCollisionMap;
class DxTextureEffMan;

namespace sc
{
	class SerialFile;
};

//----------------------------------------------------------------------------------------------------------------------
//										D	x		P	i	e	c	e		E	d	i	t
//	Note : 
//----------------------------------------------------------------------------------------------------------------------
class DxPieceEdit
{
private:
    enum
    {
        VERSION_PEF = 0x0110,
		VERSION_PIE = 0x0109,
        VERSION_LM = 0x100,
    };

protected:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	float m_fTime;
	float m_fAniTime;

protected:
	std::string	m_strFileName;		// �������� �̸�
	std::string	m_strFrameName;		// FrameMesh �̸�
    D3DXVECTOR3 m_vMin;				// ��ü�� Min	< ��ü �G, ȿ���� �������� >
	D3DXVECTOR3 m_vMax;				// ��ü�� Max	< ��ü �G, ȿ���� �������� >
	D3DXVECTOR3	m_vStaticMin;		// StaticMesh ���� ����. < ĳ���͸� ������츦 üũ�ϱ� ���� >
	D3DXVECTOR3	m_vStaticMax;		// StaticMesh ���� ����. < ĳ���͸� ������츦 üũ�ϱ� ���� >
	D3DXVECTOR3 m_vCenter;			// ��ü�� ����. 						< 1�� üũ�� ���� >		��ȯ�� ��
	float		m_fLength;			// �������� ���� �׷��� ����� ������.	< 1�� üũ�� ���� >
    //D3DXMATRIX	m_matLocal;			// ������ Local Matrix
    DWORD		m_dwFlags;			// PIECE_OPTION
	float       m_fVisibleRadius;   // Piece�� alpha�� 1�� �Ǵ� �ݰ�.
	float       m_fInvisibleRadius; // Piece�� alpha�� 0�� �Ǵ� �ݰ�.


protected:
	DxFrameMesh*	m_pFrameMesh;		// FrameMesh EDIT ��.
	DxPieceEff*		m_pPieceEffEDIT;	// ���� EDIT �ϱ� ���� �������.
	CCollisionMap*	m_pCollisionMap;	// ���� EDIT �ϱ� ���� �������.

public:
	DxStaticAniFrame*	m_pStaticAniFrame;
	DxPieceEff*			m_pPieceEff;
	DxTextureEffMan*	m_pTextureEff;

public:
	int		m_nRefCount;

public:
    friend class CStaticMeshCreate;

public:
	const std::string&	GetFileName()			{ return m_strFileName; }
	const char*			GetFrameName()			{ return m_strFrameName.c_str(); }
	D3DXVECTOR3			GetAABBMax()			{ return m_vMax; }
	D3DXVECTOR3			GetAABBMin()			{ return m_vMin; }
	D3DXVECTOR3			GetStaticMax()			{ return m_vStaticMax; }
	D3DXVECTOR3			GetStaticMin()			{ return m_vStaticMin; }
	D3DXVECTOR3			GetCenter()				{ return m_vCenter; }
	float				GetLength()				{ return m_fLength; }
	//D3DXMATRIX*			GetMatrixLocal()    	{ return &m_matLocal; }
    DWORD				GetFlags() const		{ return m_dwFlags; }
	float               GetVisibleRadius()  { return m_fVisibleRadius; }
	float               GetInvisibleRadius(){ return m_fInvisibleRadius; }

	void                SetVisibleRadius( float _VisibleRadius ) { m_fVisibleRadius = _VisibleRadius; }
	void                SetInvisibleRadius( float _InvisibleRadius ) { m_fInvisibleRadius = _InvisibleRadius; }

public:
	DxFrameMesh*		    GetFrameMesh()		        { return m_pFrameMesh; }
	DxPieceEff*			    GetPieceEffEDIT()	        { return m_pPieceEffEDIT; }
    const DxFrameMesh*      GetFrameMesh_const() const  { return m_pFrameMesh; }
    const DxTextureEffMan*  GetTextureEffMan() const    { return m_pTextureEff; }

public:
	void	NewFrameMesh(LPDIRECT3DDEVICEQ pd3dDevice, const char* szName, const TSTRING& strPieceEditPath );
	//void	NewLightMapUV( LPDIRECT3DDEVICEQ pd3dDevice, const char* szName, const SUBPATH* pPath );
	void	Export( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bRan_1_0 );

public:
	DxFrame*	GetFrameRoot();

public:
	BOOL IsCollision_NoneConst ( const D3DXVECTOR3 &vP1, D3DXVECTOR3 &vP2, BOOL bEditor, const DxFrame* pFrame, char* pName );
	DxFrame* const GetCollisionDetectedFrame ();
	const D3DXVECTOR3 GetCollisionPos ();

	BOOL SetEff( LPDIRECT3DDEVICEQ pd3dDevice, const char* szFrameName, const D3DXVECTOR3& vPos, const char* szEffName, std::string& strEditName );
	void DelEff( const char* szEffName );
	void ModifyEff( LPDIRECT3DDEVICEQ pd3dDevice, const char* szEditName, const char* szEffName );
	LPD3DXMATRIX GetEffMatrix( const char* szEditName );
    void ReloadTexEffVMFX( LPDIRECT3DDEVICEQ pd3dDevice );

	void SetCheckCollPoint( const char* szEffName, BOOL bUse );
	BOOL GetCheckCollPoint( const char* szEffName );
	const char* GetEffName( const char* pEditName );

public:
	void OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice();
	void OnDestroyDevice();

public:
	void FrameMoveFrame( const float fTime, const float fElapsedTime );
	void RenderFrame( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, const D3DXMATRIX& matWorld, BOOL bEditMode );
	void RenderFrameAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matWorld, float fAlpha );

	void FrameMove( const float fTime, const float fElapsedTime );
	void Render( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DCOLOR& colorClear, const D3DXMATRIX& matWorld );
    //void RenderAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matWorld, float fAlpha );

	// 
	void RenderDeffered( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matWorld );
	void RenderDefferedDecal( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matWorld );
	void RenderDefferedSoftAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matWorld );
	void RenderDecalPass2( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matWorld );

public:
	void CleanUp();

public:
	void SavePEF( const char* szName, const TSTRING& strXFileName );
	HRESULT LoadPEF( LPDIRECT3DDEVICEQ pd3dDevice, const char* szName, const TSTRING& strPieceEditPath );
	void LoadFrameEff( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, DxFrameMesh* pFrameMesh );
	
	void SavePIE( const char* szName );
	HRESULT LoadPIE( LPDIRECT3DDEVICEQ pd3dDevice, const char* szFullPath, const TSTRING& strPieceEditPath, const TSTRING& strFileName, boost::function<void (const TCHAR*)> pArchive = 0 );

	HRESULT LoadPIE_0100( sc::SerialFile& SFile );
	HRESULT LoadPIE_0101( sc::SerialFile& SFile );
	HRESULT LoadPIE_0102( sc::SerialFile& SFile );
	HRESULT LoadPIE_0103( sc::SerialFile& SFile );																	//	������ ���� ���� ����
    HRESULT LoadPIE_0104_0106( sc::SerialFile& SFile );																//	m_dwFlags �߰�
	HRESULT LoadPIE_0107( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, const TSTRING& strPieceEditPath );	//	FrameMeshName �� ������ ����ǵ��� ������.
	HRESULT LoadPIE_0108( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, const TSTRING& strPieceEditPath );	//	LOD(AlphaBlend) ��� �߰�
	HRESULT LoadPIE_0109( sc::SerialFile& SFile );																	//	FrameMeshName �� ���� ���� �ʿ����.

    BOOL IsLightMapPiece();
    void SaveLightMapPiece( sc::SerialFile& SFile );
    void LoadLightMapPiece( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile );

	BOOL ChangeDxStaticMaterialMesh( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	DxPieceEdit();
	~DxPieceEdit();
};