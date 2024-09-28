#pragma once

#include "../EngineLib/DxBase/D3DViewport.h"
#include "../EngineLib/G-Logic/TypeDefine.h"


class SkillIconImage : public CD3DViewport
{
public:
	enum { EIconSize = 35 };
	
private:
	HWND m_hWndTarget;

	std::string m_strFileName;
	SNATIVEID m_sIconID;

public:
	SkillIconImage();
	virtual ~SkillIconImage();

public:
	void AttachWnd( HWND hWndTarget );
	void DetachWnd();

	void SetFileName( const std::string& strFileName )
	{
		m_strFileName = strFileName;
	}
	void SetIconID( const SNATIVEID& sIconID )
	{
		m_sIconID = sIconID;
	}

public:
	virtual VOID OnFrameRender(
		IDirect3DDevice9* pd3dDevice
		, DOUBLE fTime
		, FLOAT fElapsedTime );

	virtual HWND GetTargetWnd()
	{
		return m_hWndTarget;
	}

//SaveIcon 
	void SaveIconToFile( const CString& strSavePath, const D3DXIMAGE_FILEFORMAT SaveType = D3DXIFF_JPG);
	void SaveIconToFile( const CString& strSavePath, const WORD wMID, const WORD wSID, const D3DXIMAGE_FILEFORMAT SaveType = D3DXIFF_JPG);
	
	bool ExtractIcon( LPDIRECT3DTEXTURE9 &ResultTexture, const LPDIRECT3DTEXTURE9 &TargetTexture, const D3DXVECTOR2 &vIconPos, const D3DXVECTOR2 &vIconSize );
	bool CreateDirectoryFunc( LPCTSTR path );
	bool CreateCustomFileName( CString OUT &strResult, const char *szSkillID, const char *szTextureName, const SNATIVEID& sIconIndex, D3DXIMAGE_FILEFORMAT SaveType = D3DXIFF_JPG);

	void SaveIconErrorListToFile( const CString& strSavePath, const CString& strFileName, const CString& strComment, bool bSaveCSV = false);
	void SaveIconErrorListToCSV( const CString& strSavePath, const CString& strFileName, const CString& strComment );
	void SaveIconErrorListToTXT( const CString& strSavePath, const CString& strFileName, const CString& strComment );

private:
	std::vector<TCHAR *> m_vecError;
//end SaveIcon
};

