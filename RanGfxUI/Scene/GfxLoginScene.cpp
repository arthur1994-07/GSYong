#include "StdAfx.h"
#include "GfxLoginScene.h"

GfxLoginScene::GfxLoginScene()
{
}

GfxLoginScene::~GfxLoginScene()
{
}

HRESULT GfxLoginScene::OnInit( LPDIRECT3DDEVICE9 pDevice )
{
	m_strFilePath = NSGFXFILE::g_strGfxPath;
	m_strFilePath.append( NSGFXFILE::g_strLoginSceneName.c_str() );
	m_strFilePath = sc::string::ansi_to_utf8( m_strFilePath );

	return LoadMovie( pDevice );
}