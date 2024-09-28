#include "stdafx.h"
#include "UIXMLManager.h"

#include "../SigmaCore/Log/LogMan.h"
#include "../SigmaCore/String/StringFormat.h"

namespace HTML5UI
{

CUIXMLManager& CUIXMLManager::GetInstance()
{
	static CUIXMLManager instance;
	return instance;
}

std::string CUIXMLManager::GetText( const TCHAR* fileName )
{
	CString strFileName;
	strFileName = GetPath();
	strFileName += fileName;

	return strFileName.GetString();
}

void CUIXMLManager::ClearTexture()
{
	m_vectorTextureList.clear();
}

static bool SetTextureInfo( CUIXMLManager::TEXTURE_INFO& sTextureInfo, UICompTexList::SCOMPONENT* pTexture )
{
	if( !pTexture )
		return false;

	sTextureInfo.textureID = pTexture->m_textureID;
	sTextureInfo.border = GUIBase::UIRECT(
		pTexture->m_vBorder.x,
		pTexture->m_vBorder.y,
		pTexture->m_vBorder.z,
		pTexture->m_vBorder.w );
	sTextureInfo.X_Size = pTexture->m_vSize.x;
	sTextureInfo.Y_Size = pTexture->m_vSize.y;
	sTextureInfo.textureName = pTexture->m_strFileName;

	if( sTextureInfo.X_Size <= 0.0f || sTextureInfo.Y_Size <= 0.0f )
		return false;

	sTextureInfo.rcTexturePos = GUIBase::UIRECT(
		( pTexture->m_vPos.x + 0.25f ) / sTextureInfo.X_Size,
		( pTexture->m_vPos.y + 0.25f ) / sTextureInfo.Y_Size,
		pTexture->m_vPos.z / sTextureInfo.X_Size,
		pTexture->m_vPos.w / sTextureInfo.Y_Size );

	return true;
}

BOOL CUIXMLManager::LoadTextureList( const TCHAR * fileName )
{

	bool bReturn = UICompTexList::CXmlLoader().Load(
		GetText( fileName ), m_vectorTextureList );

	for( unsigned i=0; i<m_vectorTextureList.size(); ++i )
	{
		TEXTURE_INFO textureInfo;
		if( !SetTextureInfo( textureInfo, &m_vectorTextureList.at( i ) ) )
			return false;
		m_componentTexVector.push_back( textureInfo );
	}

	return bReturn;
}

}