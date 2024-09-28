#pragma once

// bjju 11.03.08

#include <string>

class IUIMeshData
{
public:
    IUIMeshData  ();
    virtual ~IUIMeshData () = 0;

public:
    virtual HRESULT InitDeviceObjects       ( LPDIRECT3DDEVICEQ pd3dDevice ) = 0;
    virtual HRESULT RestoreDeviceObjects    ( LPDIRECT3DDEVICEQ pd3dDevice ) = 0;
    virtual HRESULT InvalidateDeviceObjects ()                               = 0;
    virtual HRESULT DeleteDeviceObjects     ()                               = 0;

public:
    virtual const BOOL Load      ( LPDIRECT3DDEVICEQ pd3dDevice, const std::string& szFile )   = 0;
    virtual void       FrameMove ( LPDIRECT3DDEVICEQ pd3dDevice, float fElapsedTime )          = 0;
    virtual void       Render    ( LPDIRECT3DDEVICEQ pd3dDevice )                              = 0;

public:
    void               SetWorldMatrix ( const D3DXMATRIX& cMat ) { m_matWorld = cMat; }
    void               SetViewMatrix  ( const D3DXMATRIX& cMat, const D3DXVECTOR3& vCameraFromPt ) { m_matView = cMat; m_vCameraFromPt = vCameraFromPt; }
    void               SetProjMatrix  ( const D3DXMATRIX& cMat ) { m_matProj  = cMat; }

    void               SetPosition    ( const D3DXVECTOR3& vPos );

protected:
    LPDIRECT3DDEVICEQ   m_pd3dDevice;
    D3DXMATRIX          m_matWorld;
    D3DXMATRIX          m_matView;
    D3DXMATRIX          m_matProj;
	D3DXVECTOR3			m_vCameraFromPt;

};
