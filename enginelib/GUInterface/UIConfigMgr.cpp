#include "pch.h"

#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringFormat.h"

#include "./UIConfigMgr.h"

#include "../../SigmaCore/DebugInclude.h"

CUIConfigMgr& CUIConfigMgr::GetInstance()
{
	static CUIConfigMgr s_cInstance;
	return s_cInstance;
}

void CUIConfigMgr::ClearTexture()
{
	m_mapTextureList.clear();
}

void CUIConfigMgr::ClearControl()
{
	m_mapControlList.clear(); 
}

BOOL CUIConfigMgr::LoadTextureList( const TCHAR * filename )
{
    CString strFileName;
    strFileName = GetPath();
    strFileName += filename;
    return UITextureList::CXmlLoader().Load( strFileName.GetString() ,m_mapTextureList );
}

BOOL CUIConfigMgr::LoadControlList( const TCHAR * filename , const CString& country )
{
    CString strFilePath;
    strFilePath = GetPath();

    return UIControlList::CXmlLoader().Load( strFilePath.GetString(), filename, m_mapControlList );
}

UITextureList::SControl* CUIConfigMgr::FindTexture( const std::string& strID )
{
	UITextureList::ControlMapIter it = m_mapTextureList.find( strID );
	if( it != m_mapTextureList.end() )
		return &( it->second );

	return NULL;
}

UIControlList::SControl* CUIConfigMgr::FindControl( const std::string& strID )
{
	UIControlList::ControlMapIter it = m_mapControlList.find( strID );
	if( it != m_mapControlList.end() )
		return &( it->second );

	return NULL;
}

bool CUIConfigMgr::FindConfig( const std::string& strID, INTERFACE_CFG& sCfg )
{
	UIControlList::SControl* pControl = FindControl( strID );
	if( !pControl )
		return false;

	sCfg.rcControlPos = UIRECT(
		pControl->m_vPos.x,
		pControl->m_vPos.y,
		pControl->m_vPos.z,
		pControl->m_vPos.w );

	if( pControl->m_strTextureID.empty() )
		return true;

	UITextureList::SControl* pTexture = FindTexture( pControl->m_strTextureID );
	if( !pTexture )
		return false;

	sCfg.vBorder = pTexture->m_vBorder;

	sCfg.X_Size = pTexture->m_vSize.x;
	sCfg.Y_Size = pTexture->m_vSize.y;
	sCfg.strTextureName = pTexture->m_strFileName.c_str();

	if( sCfg.X_Size <= 0.0f || sCfg.Y_Size <= 0.0f )
		return false;

	sCfg.rcTexturePos = UIRECT(
		( pTexture->m_vPos.x + 0.25f ) / sCfg.X_Size,
		( pTexture->m_vPos.y + 0.25f ) / sCfg.Y_Size,
		pTexture->m_vPos.z / sCfg.X_Size,
		pTexture->m_vPos.w / sCfg.Y_Size );

	return true;
}