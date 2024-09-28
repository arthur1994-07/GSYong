#pragma once

#include "../EngineLib/DxBase/D3DViewport.h"
#include "../EngineLib/G-Logic/TypeDefine.h"

class CIconPreview : public CD3DViewport
{
public:
	enum
	{
		EIconSize = 35,
	};

private:
	HWND m_hWndTarget;

	CString m_strFileName;
	SNATIVEID m_sIconID;

public:
	CIconPreview();
	virtual ~CIconPreview();

public:
	void AttachWnd( HWND hWndTarget );
	void DetachWnd();

public:
    virtual VOID OnFrameRender(
        IDirect3DDevice9* pd3dDevice,
        DOUBLE fTime,
        FLOAT fElapsedTime );

public:
	void SetFileName( const CString& strFileName ) { m_strFileName = strFileName; }
	void SetIconID( const SNATIVEID& sIconID ) { m_sIconID = sIconID; }

public:
	virtual HWND GetTargetWnd() { return m_hWndTarget; }
};