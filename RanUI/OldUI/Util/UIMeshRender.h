#pragma once

#include "../../../EngineLib/GUInterface/UIControl.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/DxTools/DxBackUpRendTarget.h"

#include "./UIMeshData.h"

#include <map>
#include <algorithm>

//-----------------------------------------------------------------------------------------------//

class CUIMeshData_SkinChar;
class CUIMeshData_SkinVehicle;


class CUIMeshRender : public CUIControl
{

public:
	enum MESH_ID
	{
		MESH_ID_CHARACTER,
		MESH_ID_VEHICLE,
	};

    typedef std::map<MESH_ID, IUIMeshData*>     IUIMESHDATAMAP;
    typedef IUIMESHDATAMAP::iterator            IUIMESHDATAMAPITER;
    typedef std::pair<MESH_ID, IUIMeshData*>    IUIMESHDATAPAIR;

public:
    CUIMeshRender(EngineDeviceMan* pEngineDevice);
    virtual ~CUIMeshRender();

public:
    CUIMeshData_SkinChar*    LoadSkinChar( const MESH_ID nID, const std::string& szFile );
	CUIMeshData_SkinVehicle* LoadSkinVehicle( const MESH_ID nID, const std::string& szFile );

    IUIMeshData*            FindMeshData  ( const MESH_ID nID );

	void DeleteMeshData(const MESH_ID nID );
public:
    const BOOL              IsUseAble       () { return !(!m_pd3dDevice); }

public:
    virtual HRESULT         InitDeviceObjects      ( LPDIRECT3DDEVICEQ pd3dDevice );
    virtual HRESULT         RestoreDeviceObjects   ( LPDIRECT3DDEVICEQ pd3dDevice );
    virtual HRESULT         InvalidateDeviceObjects();
    virtual HRESULT         DeleteDeviceObjects();

public:
    virtual void            Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void            TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

public:
    LPDIRECT3DDEVICEQ       GetDevice() { return m_pd3dDevice; }

private:
    HRESULT                 CreateTargetTexture ( LPDIRECT3DDEVICEQ pd3dDevice );
	void				    LoadHelper(IUIMeshData* pData,const MESH_ID nID, const std::string& szFile);
private:
    HRESULT                 Render      ( LPDIRECT3DDEVICEQ pd3dDevice );
    HRESULT                 RenderBegin ( LPDIRECT3DDEVICEQ pd3dDevice );
    HRESULT                 RenderEnd   ( LPDIRECT3DDEVICEQ pd3dDevice );

protected:
    virtual HRESULT         RenderMesh  ( LPDIRECT3DDEVICEQ pd3dDevice );

	//////////////////////////////////////////////////////////////////////////

	LPDIRECT3DDEVICEQ       m_pd3dDevice;

protected:
    //LPDIRECT3DSURFACEQ		m_pTargetSurface;
    //LPDIRECT3DSURFACEQ	    m_pTargetZBuf;
    //LPDIRECT3DTEXTUREQ      m_pTargetTexture;
    LPDIRECT3DSTATEBLOCK9	m_pSavedSB;
    LPDIRECT3DSTATEBLOCK9	m_pDrawSB;
    D3DSURFACE_DESC			m_descBackBuffer;
    DxBackupTarget          m_sBackupTarget;
    UIVERTEX	            m_TargetUIVertex[VERTEX_SIZE];

private:
    IUIMESHDATAMAP          m_MeshMap;

};
