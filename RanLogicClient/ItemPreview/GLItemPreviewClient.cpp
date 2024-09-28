#include "pch.h"

#include "../GLGaeaClient.h"
#include "../RanLogic/GLogicDataMan.h"
#include "../RanLogicClient/Char/GLCharacter.h"

#include "GLItemPreviewClient.h"


namespace ItemPreview
{
	GLItemPreviewClient::GLItemPreviewClient(GLGaeaClient* const pGaeaClient)
		: m_pGaeaClient(pGaeaClient)
		, m_bTurnLeft(false)
		, m_bTurnRight(false)
		, m_fMinViewPosZ( -35.0f )
		, m_fMaxViewPosZ( 0.0f )
	{
		m_sPreItemCustom = SITEMCUSTOM( NATIVEID_NULL() );
		m_fTurnSpeed = 1.4f;
	}

	GLItemPreviewClient::~GLItemPreviewClient(void)
	{
	}

	void GLItemPreviewClient::OpenItemPreview( const SITEMCUSTOM& sItemCustom )
	{
		GLWidgetTextureItemPreview* const pTexture = 
			(GLWidgetTextureItemPreview*) GLWidgetTextureMan::GetInstance().FindTexture( EMWIDGET_TEXTURE_ITEM_PREVIEW );
		if( pTexture )
		{
			// UI 닫혀있으면 미리보기 아이템 초기화하고 열어주자.
			if( false == GLWidgetScript::GetInstance().LuaCallFunc< bool >( 
				NSWIDGET_SCRIPT::g_strFunc_ItemPreview_IsOpen ) )
				pTexture->ResetPreviewItem();
		}
		else
		{
			m_sPreItemCustom = sItemCustom;
		}

		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_ItemPreview_Open );
	}

	void GLItemPreviewClient::SetPreItem()
	{
		if( m_sPreItemCustom == SITEMCUSTOM( NATIVEID_NULL() ) )
			return;

		SetItem( m_sPreItemCustom );
		m_sPreItemCustom = SITEMCUSTOM( NATIVEID_NULL() );
	}

	void GLItemPreviewClient::MoveViewPosZ( float fValue )
	{
		GLWidgetTextureItemPreview* const pTexture = 
			(GLWidgetTextureItemPreview*) GLWidgetTextureMan::GetInstance().FindTexture( EMWIDGET_TEXTURE_ITEM_PREVIEW );
		if ( pTexture == NULL )
			return;

		if( 0.0f > fValue && m_fMinViewPosZ >= pTexture->GetViewPosition().z )
			return;

		if( 0.0f < fValue && m_fMaxViewPosZ <= pTexture->GetViewPosition().z )
			return;

		pTexture->MoveViewPosZ( fValue );
	}

	void GLItemPreviewClient::SetItem( const SITEMCUSTOM& sItemCustom )
	{
		const SITEM* pItem = GLogicData::GetInstance().GetItem ( sItemCustom.GetNativeID() );
		if ( !pItem )
			return;

		GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
		if( !pCharacter )
			return;

		GLWidgetTextureItemPreview* const pTexture = 
			(GLWidgetTextureItemPreview*) GLWidgetTextureMan::GetInstance().FindTexture( EMWIDGET_TEXTURE_ITEM_PREVIEW );
		if ( pTexture == NULL )
			return;

		pTexture->SetPreviewItem( sItemCustom );

		GLWidgetScript::GetInstance().UpdateItemPreviewRT();
	}

	bool GLItemPreviewClient::IsEnableItemPreview( const SITEM* pItem )
	{
		if( NULL == pItem )
			return false;

		EMCHARINDEX emCharIndex = CharClassToIndex( m_pGaeaClient->GetCharacter()->m_emClass );
		if( !pItem->GetWearingFile( emCharIndex ) || !pItem->GetWearingFile( emCharIndex )[ 0 ] )
		{
			if( !pItem->GetWearingFileEx( emCharIndex ) || !pItem->GetWearingFileEx( emCharIndex )[ 0 ] )
				return false;
		}

		return true;
	}
}