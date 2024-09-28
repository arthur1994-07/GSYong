#pragma once

#include "./UITextureListXml.h"
#include "./UIControlListXml.h"
#include "./UIControl.h"

class CUIConfigMgr
{
public:
	static CUIConfigMgr& GetInstance();

private:
    TCHAR m_szPath[MAX_PATH];

	UITextureList::ControlMap m_mapTextureList;
	UIControlList::ControlMap m_mapControlList;

public:
	void ClearTexture();
	void ClearControl();

    BOOL LoadTextureList( const TCHAR * filename);
    BOOL LoadControlList( const TCHAR * filename, const CString& country );

	UITextureList::SControl* FindTexture( const std::string& strID );
	UIControlList::SControl* FindControl( const std::string& strID );

	bool FindConfig( const std::string& strID, INTERFACE_CFG& sCfg );


    TCHAR* GetPath ()				{ return m_szPath; }
    void SetPath( TCHAR* szPath )	{ StringCchCopy( m_szPath, MAX_PATH, szPath ); }
};