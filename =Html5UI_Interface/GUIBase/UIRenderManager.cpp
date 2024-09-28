#include "StdAfx.h"
#include "UIRenderManager.h"

namespace GUIBase
{

CUIRenderManager& CUIRenderManager::GetInstance()
{
	static CUIRenderManager instance;
	return instance;
}

//! ���� Render State;
void CUIRenderManager::SetRenderState()
{
	if( m_renderer )
		m_renderer->SetRenderState();
}

//! ȸ�� Render State;
void CUIRenderManager::RecoverRenderState()
{
	if( m_renderer )
		m_renderer->RecoverRenderState();
}

//! Device
bool CUIRenderManager::InitDevice()
{
	if( m_renderer )
		return m_renderer->InitDevice();

	return true;
}

bool CUIRenderManager::RestoreDevice()
{
	if( m_renderer )
		return m_renderer->RestoreDevice();

	return true;
}

bool CUIRenderManager::InvalidateDevice()
{
	if( m_renderer )
		return m_renderer->InvalidateDevice();

	return true;
}

bool CUIRenderManager::DeleteDevice()
{
	if( m_renderer )
		return m_renderer->DeleteDevice();

	return true;
}

//! ������ �̿��� Texture �׸���;
bool CUIRenderManager::RenderTextureUsingFile(
	UIID uiID,
	const std::string& textureID,
	COMPONENT_POSITION& compPosition )
{
	if( m_renderer )
	{
		return m_renderer->RenderTextureUsingFile(
			uiID, textureID, compPosition );
	}
	return false;
}

//! size ũ���� Texture�� Image Data ���Ž�Ű��;
bool CUIRenderManager::RenderTextureUsingData(
	UIID uiID,
	UI_IMAGE_DATA& imgData,
	COMPONENT_POSITION& compPosition )
{
	if( m_renderer )
	{
		return m_renderer->RenderTextureUsingData(
			uiID, imgData, compPosition );
	}

	return false;
}

//! ������� UI �׸���;
void CUIRenderManager::RenderUI(  UIRECT rcGlobalPos
								, UIID uiID )
{
	if( m_renderer )
		m_renderer->RenderUI( rcGlobalPos, uiID );
}

}