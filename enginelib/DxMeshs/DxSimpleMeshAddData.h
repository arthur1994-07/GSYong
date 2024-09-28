#pragma once

struct SIMPLEMESH_ADD_DATA
{
	BOOL				m_bTexThread;
	TEXTURE_RESIZE		m_uiResizeLevel;
	TEXTURE_CATEGORY	m_emCategory;

	SIMPLEMESH_ADD_DATA()
		: m_bTexThread(TRUE)
		, m_uiResizeLevel(TEXTURE_RESIZE_NONE)
		, m_emCategory(TEXTURE_ETC)
	{
	}

	SIMPLEMESH_ADD_DATA( BOOL bTexThread, TEXTURE_RESIZE uiResizeLevel, TEXTURE_CATEGORY emCategory )
		: m_bTexThread(bTexThread)			//TRUE)
		, m_uiResizeLevel(uiResizeLevel)	//TEXTURE_RESIZE_NONE)
		, m_emCategory(emCategory)			//TEXTURE_ETC)
	{
	}
};