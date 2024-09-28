#pragma once

#include <string>
#include <boost/tr1/memory.hpp>
#include "../../EngineLib/DxBase/D3DObject.h"
#include "../../EngineLib/GUInterface/UITextureListXml.h"

class EngineDeviceMan;

class CEditor : public CD3DObject
{
private:
    bool m_bisLoad;

protected:
    EngineDeviceMan* m_pEngineDevice;

	std::string m_strSelectedID;
	UITextureList::ControlMap m_mapControl;

public:
    CEditor();
    virtual ~CEditor();

public:
	void Reset();
	bool LoadXml( const std::string& strFilePath );
    bool AddXml( const std::string& strFilePath );
	bool SaveXml( const std::string& strFilePath );
    bool IsLoaded();

public:
    virtual HRESULT OnDeviceCreated(
        IDirect3DDevice9* pd3dDevice,
        const D3DSURFACE_DESC* pBackBufferSurfaceDesc );

    virtual VOID OnDeviceDestroyed();

    virtual HRESULT OnDeviceReset(
        IDirect3DDevice9* pd3dDevice,
        const D3DSURFACE_DESC* pBackBufferSurfaceDesc );

    virtual VOID OnDeviceLost();

    virtual VOID OnFrameMove(
        IDirect3DDevice9* pd3dDevice,
        DOUBLE fTime,
        FLOAT fElapsedTime );

    virtual VOID OnFrameRender(
        IDirect3DDevice9* pd3dDevice,
        DOUBLE fTime,
        FLOAT fElapsedTime );

public:
    void AddNewTemp();  // 새 임시항목을 추가한다.
    bool DelControl();

    bool CEditor::FindControl( D3DXVECTOR2 vPos, std::string& strOutItem );

	void SetSelectedID( std::string strSelectedID ) { m_strSelectedID = strSelectedID; }
	const std::string& GetSelectedID() { return m_strSelectedID; }
	UITextureList::ControlMap& GetControlMap() { return m_mapControl; }
};

extern CEditor* g_pEditor;