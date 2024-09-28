#pragma once

/**
 * \file	DxViewPortVM.h
 * \date	2010/01/05
 * \author	Sung-Hwan Han
 * \brief	VisualMaterial 과 같은 카메라 확대 축소 이동을 위해서 사용됨.
 */
class DxViewPortVM
{
private:
    HWND                m_hWnd;
	LPDIRECT3DDEVICEQ   m_pd3dDevice;
    float               m_fElapsedTime;
    D3DVIEWPORTQ	    m_sViewPort;
    D3DXMATRIX          m_matView;
    D3DXMATRIX          m_matProj;
    D3DXVECTOR3         m_vFromPt;
	D3DXVECTOR3         m_vLookatPt;
	D3DXVECTOR3         m_vUpVec;
    float               m_fFOV;
    float               m_fWidth;
    float               m_fHeight;
    float               m_fNearPlane;
    float               m_fFarPlane;

public:
    HRESULT SetViewPort();
    void SetViewTrans();
    void SetViewTrans( const D3DXVECTOR3 &vFromPt,const  D3DXVECTOR3 &vLookatPt, const D3DXVECTOR3 &vUpVec );
    void SetProjection( float fFOV, float fWidth, float fHeight, float fNearPlane, float fFarPlane );
    void SetTransform();

    const D3DVIEWPORTQ& GetViewPort()	{ return m_sViewPort; }
    const D3DXMATRIX& GetMatView()      { return m_matView; }
    const D3DXMATRIX& GetMatProj()      { return m_matProj; }
    const D3DXVECTOR3& GetFromPt()      { return m_vFromPt; }
    BOOL GetMouseTargetPosWnd( D3DXVECTOR3& vTargetPos );

private:
    void ComputeBoundBox( D3DXVECTOR3 *pt );

public:
    void OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice, HWND hWnd );

    HRESULT FrameMove( float fElapsedTime );

private:
    void CameraMove( const D3DXVECTOR3& vMove );
	void CameraZoom( float fZoom );

public:
	DxViewPortVM();
	~DxViewPortVM();

    // 싱글톤 관련 변수 및 함수
private:
    static DxViewPortVM*			g_pInstance;

public:
	static DxViewPortVM* GetInstance();
	static void ReleaseInstance();
};