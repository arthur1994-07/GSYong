#pragma once

#include "../Util/UIGroupHelper.h"

class CInnerInterface;
class EngineDeviceMan;

class DxFrameMesh;

class CViewFrameMeshComponent : public CUIGroupHelper
{
private:
	typedef boost::function< void () > FunctionVoid;

public:
	struct RenderState
	{	
		DWORD dwLight;
		DWORD dwNormalize;
		DWORD dwSpecular;
		DWORD dwBiasPrev;
		DWORD dwMagFilter, dwMinFilter;
		DWORD dwZFunc;
		DWORD dwZEnable;
		DWORD dwZWriteEnable;
		D3DLIGHT9 Light[3];
		D3DMATERIAL9 Material;
		D3DXMATRIXA16 mViewPREV;
		D3DXMATRIXA16 mProjPREV;

		RenderState()
			: dwLight( true )
			, dwNormalize( true )
			, dwSpecular( true )
			, dwBiasPrev( 0 )
			, dwMagFilter( D3DTEXF_LINEAR )
			, dwMinFilter( D3DTEXF_LINEAR )
			, dwZFunc( D3DCMP_LESSEQUAL )
			, dwZEnable( true )
			, dwZWriteEnable( true )
		{

		}
	};

	struct FRAME_MESH
	{
		std::string filePath;
		std::string fileName;
		DxFrameMesh* pFrameMesh;
		bool bAnimation;
		float fAnimationTime;

		//D3DMATERIAL9 material;
		LPDIRECT3DTEXTUREQ pTexture;

		FRAME_MESH()
			: pFrameMesh( NULL )
			, bAnimation( false )
			, fAnimationTime( 0.f )
			, pTexture( NULL )
		{
			//ZeroMemory( &material, sizeof( material ) );
		}

		~FRAME_MESH();
	};

	struct STOP_ANIMATION
	{
		float fTime;
		float fDelayTime;
		float fTimeDelta;
		int idx;
		bool bDelay;
		bool bCurFrameDelay;

		STOP_ANIMATION()
			: fTime( 0.f )
			, fDelayTime( 0.f )
			, fTimeDelta( 0.f )
			, idx( 0 )
			, bDelay( false )
			, bCurFrameDelay( false )
		{

		}
	};

public:
	CViewFrameMeshComponent( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~CViewFrameMeshComponent();

public:
	virtual void CreateSub( CUIControl* pParent, const char* szControlKeyword, WORD wAlignFlag = UI_FLAG_DEFAULT, UIGUID WndID = NO_ID );
	virtual void CreateSubControl( const char* szControlKeyword, const char* szTextureKeyword );
	virtual void SetVisibleSingle( BOOL bVisible );

	// View Matrix 생성;
	void CreateViewMatrix( const D3DXVECTOR3& vEye, const D3DXVECTOR3& vAt );

public:
	virtual HRESULT InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();

	virtual HRESULT Render( LPDIRECT3DDEVICEQ pd3dDevice );

protected:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

private:
	void SetRenderState( LPDIRECT3DDEVICEQ pd3dDevice, RenderState& renderState );
	void RenderItem( LPDIRECT3DDEVICEQ pd3dDevice);
	void RecoverRenderState( LPDIRECT3DDEVICEQ pd3dDevice, const RenderState& renderState );
	const HRESULT RenderViewer( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	// Update Animation;
	void UpdateAnimation( float fElapsedTime );

	// Frame Mesh 컨트롤;
	void AddFrameMesh( const std::string& fileName );
	void ClearFrameMesh();
	void SetFrameMeshTexture( LPDIRECT3DTEXTUREQ pTexture, int idx = -1 );

	// Frame Mesh Animation 컨트롤;
	void ResetAnimation( int idx = -1 );
	void StartAnimation( int idx = -1 );
	void SetAnimationTime( float fTime, int idx = -1 );
	void AddStopAnimationTime( float fTime, float fDelayTime, int idx );

	// Light ( 현재 Light는 Property에 맞춰서 3개로 고정한다 );
	// idx에는 1~3까지의 값만 들어올 수 있다;
	void SetLight( const D3DLIGHT9& light, int idx );
	void SetMaterial( const D3DMATERIAL9& material );

public:
	inline void SetMaterialState( bool bUseMaterial ) { m_bUseMaterial = bUseMaterial; }

	inline void SetFOV( float fov ) { m_fov = fov; }

	// Callback;
	inline void AddCallbackFunc( FunctionVoid fnConfirm ) { m_callbackFuncs.push_back( fnConfirm ); }

private:
	CInnerInterface* m_pInterface;

	CUIControl* m_pViewer;

private:
	LPDIRECT3DDEVICEQ m_pd3dDevice;

	LPDIRECT3DTEXTUREQ m_pTargetTexture;
	LPDIRECT3DSURFACEQ m_pTargetSurface;
	LPDIRECT3DSURFACEQ m_pTargetZBuffer;

	D3DSURFACE_DESC	m_d3dBackBufferDesc;

	D3DXMATRIXA16 m_mView;
	D3DXMATRIXA16 m_mProjection;

	D3DLIGHT9 m_Light[3];
	D3DMATERIAL9 m_Material;

	D3DXVECTOR3 m_vAt;
	D3DXVECTOR3 m_vEye;

	UIVERTEX m_targetUIVertex[ VERTEX_SIZE ];

	std::vector<FRAME_MESH> m_pMeshList;
	std::vector<STOP_ANIMATION> m_pStopAnimationList;

	bool m_bUseMaterial;

	float m_fov;

	std::vector< FunctionVoid > m_callbackFuncs;
};