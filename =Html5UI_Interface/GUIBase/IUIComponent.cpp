#include "StdAfx.h"
#include "IUIComponent.h"
#include "UIEventManager.h"
#include "UIRenderManager.h"
#include "IUIFactory.h"

namespace GUIBase
{

IUIComponent::IUIComponent(  UIID uiID
						   , const std::string& uiStrID
						   , UIRECT rcParentGlobalPos
						   , UIRECT rcLocalPos
						   , IUIComponent* pParent )
: m_uiID( uiID )
, m_uiStrID( uiStrID )
, m_bShow( false )
, m_parentGlobalPos( rcParentGlobalPos )
, m_autoMakeID( 1 )
, m_pUIFactory( NULL )
, m_pMyParent( pParent )
{
	// 위치 세팅;
	m_position.rcLocalPos = rcLocalPos;
	m_position.rcGlobalPos = rcLocalPos;
	m_position.rcGlobalPos.left += rcParentGlobalPos.left;
	m_position.rcGlobalPos.right += rcParentGlobalPos.left;
	m_position.rcGlobalPos.top += rcParentGlobalPos.top;
	m_position.rcGlobalPos.bottom += rcParentGlobalPos.top;	
}

IUIComponent::~IUIComponent() { UIDestroy(); }

// Component Render
bool IUIComponent::Render( float timeDelta )
{
	bool bReturn = true;
	if( !m_textureID.empty() )
	{
		// Rendering Texture;
		bReturn = CUIRenderManager::GetInstance().RenderTextureUsingFile(
			m_uiID, m_textureID, m_position );
	}

	return bReturn;
}

/// 기본적으로 이루어져야 하는 부분;
void IUIComponent::UIInitialize()
{
	// Initialize
	Initialize();

	// Child Component
	LIST_ITER iter = m_list.begin();
	LIST_ITER iter_end = m_list.end();
	for( ; iter!=iter_end; ++iter )
	{
		IUIComponent* tempComponent = *iter;
		if( tempComponent )
			tempComponent->UIInitialize();

	}
}

void IUIComponent::UIDestroy()
{
	// Destroy
	Destroy();

	// Child Component
	RemoveAll();
}

bool IUIComponent::UIFrameMove( float timeDelta )
{
	if( !m_bShow )
		return true;

	// FrameMove
	if( !FrameMove( timeDelta ) )
		return false;

	// Child Component
	LIST_ITER iter = m_list.begin();
	LIST_ITER iter_end = m_list.end();
	for( ; iter!=iter_end; ++iter )
	{
		IUIComponent* tempComponent = *iter;
		if( tempComponent )
		{
			if( !tempComponent->UIFrameMove( timeDelta ) )
				return false;
		}
	}

	return true;
}

bool IUIComponent::UIRender( float timeDelta )
{
	if( !m_bShow )
		return true;

	// Render
	if( !Render( timeDelta ) )
		return false;

	// Child Component
	LIST_ITER iter = m_list.begin();
	LIST_ITER iter_end = m_list.end();
	for( ; iter!=iter_end; ++iter )
	{
		IUIComponent* tempComponent = *iter;
		if( tempComponent )
		{
			if( !tempComponent->UIRender( timeDelta ) )
				return false;
		}
	}

	return true;
}

/// Event 처리;
bool IUIComponent::OnKeyEvent( std::tr1::shared_ptr<CUIKeyEvent> keyEvent )
{
	if( !keyEvent.get() )
		return false;

	// Child Component
	LIST_ITER iter = m_list.begin();
	LIST_ITER iter_end = m_list.end();
	for( ; iter!=iter_end; ++iter )
	{
		IUIComponent* tempComponent = *iter;
		if( tempComponent )
			tempComponent->OnKeyEvent( keyEvent );
	}

	return true;
}

bool IUIComponent::OnMouseEvent( std::tr1::shared_ptr<CUIMouseEvent> mouseEvent )
{
	if( !mouseEvent.get() )
		return false;

	// 마우스가 들어왔는지 확인;
	if( !CEventUtil::MouseIn(
		mouseEvent->GetGlobalPosition(),
		m_position.rcGlobalPos ) )
		return false;

	// 마우스의 LocalPos 갱신;
	UIPOINT<int> localPoint = mouseEvent->GetGlobalPosition();
	localPoint.x -= static_cast<int>( m_position.rcGlobalPos.left );
	localPoint.y -= static_cast<int>( m_position.rcGlobalPos.top );
	mouseEvent->SetPosition( localPoint );
	mouseEvent->SetUIID( m_uiID );

	// Child Component
	LIST_ITER iter = m_list.begin();
	LIST_ITER iter_end = m_list.end();
	for( ; iter!=iter_end; ++iter )
	{
		IUIComponent* tempComponent = *iter;
		if( tempComponent )
		{
			// 컴포넌트에 걸리면 return;
			if( tempComponent->OnMouseEvent( mouseEvent ) )
				return true;
		}
	}

	return true;
}

bool IUIComponent::OnWheelEvent( std::tr1::shared_ptr<CUIWheelEvent> wheelEvent )
{
	if( !wheelEvent.get() )
		return false;

	// 마우스가 들어왔는지 확인;
	if( !CEventUtil::MouseIn(
		wheelEvent->GetGlobalPosition(),
		m_position.rcGlobalPos ) )
		return false;

	// 마우스의 LocalPos 갱신;
	UIPOINT<int> localPoint = wheelEvent->GetGlobalPosition();
	localPoint.x -= static_cast<int>( m_position.rcGlobalPos.left );
	localPoint.y -= static_cast<int>( m_position.rcGlobalPos.top );
	wheelEvent->SetPosition( localPoint );
	wheelEvent->SetUIID( m_uiID );

	// Child Component
	LIST_ITER iter = m_list.begin();
	LIST_ITER iter_end = m_list.end();
	for( ; iter!=iter_end; ++iter )
	{
		IUIComponent* tempComponent = *iter;
		if( tempComponent )
		{
			// 컴포넌트에 걸리면 return;
			if( tempComponent->OnWheelEvent( wheelEvent ) )
				return true;
		}
	}

	return true;
}

bool IUIComponent::OnInvokeEvent( std::tr1::shared_ptr<CUIInvokeEvent> invokeEvent )
{
	if( !invokeEvent.get() )
		return false;

	// Child Component.
	LIST_ITER iter = m_list.begin();
	LIST_ITER iter_end = m_list.end();
	for( ; iter!=iter_end; ++iter )
	{
		IUIComponent* tempComponent = *iter;
		if( tempComponent )
			tempComponent->OnInvokeEvent( invokeEvent );
	}

	return true;
}

/// Show
void IUIComponent::ShowComponent()
{
	m_bShow = true;
}

void IUIComponent::HideComponent()
{
	m_bShow = false;
}

//! 위치 이동;
void IUIComponent::MoveComponentPoint( UIPOINT<int> point )
{
	const UISIZE<int>& resolution =
		CUIRenderManager::GetInstance().GetResolution();
	UISIZE<int> uiSize(
		static_cast<int>( m_position.rcLocalPos.sizeX ),
		static_cast<int>( m_position.rcLocalPos.sizeY ) );
	if( point.x + uiSize.sizeX > resolution.sizeX )
		point.x = resolution.sizeX-uiSize.sizeX;
	else if( point.x < 0 )
		point.x = 0;

	if( point.y + uiSize.sizeY > resolution.sizeY )
		point.y = resolution.sizeY-uiSize.sizeY;
	else if( point.y < 0 )
		point.y = 0;

	UIPOINT<int> delta;
	delta.x = static_cast<int>( point.x - m_position.rcLocalPos.left );
	delta.y = static_cast<int>( point.y - m_position.rcLocalPos.top );
	m_position.rcLocalPos.left = static_cast<float>( point.x );
	m_position.rcLocalPos.top = static_cast<float>( point.y );
	m_position.rcGlobalPos.left += delta.x;
	m_position.rcGlobalPos.right += delta.x;
	m_position.rcGlobalPos.top += delta.y;
	m_position.rcGlobalPos.bottom += delta.y;

}

// Component 추가;
void IUIComponent::RemoveAll ( bool bDELETE )
{
	if( bDELETE )
	{
		LIST_ITER iter = m_list.begin();
		LIST_ITER iter_end = m_list.end();
		for( ; iter != iter_end; ++iter )
			SAFE_DELETE ( *iter );
	}

	m_list.clear();
	m_map.clear();
}

bool IUIComponent::IsEmpty()
{	
	return m_list.empty();
}

unsigned int IUIComponent::Size()
{
	return m_list.size();
}

IUIComponent* IUIComponent::At( UIID uiID )
{
	MAP_ITER found = m_map.find( uiID );
	if( found != m_map.end() )
		return *(found->second);
	else
		return NULL;
}

UIID IUIComponent::LoadUIIDUsingStrID( const std::string& strID )
{
	LIST_ITER iter = m_list.begin();
	LIST_ITER iter_end = m_list.end();
	for( ; iter != iter_end; ++iter )
	{
		IUIComponent* pComponent = *iter;
		if( pComponent->GetStrID() == strID )
			return pComponent->GetUIID();
	}

	return NO_ID;
}

bool IUIComponent::Insert( const std::string& strID
							 , WINDOW_CFG& windowCFG
							 , UIRECT rcGlobalPos )
{
	IUIComponent* pComponent = NULL;
	if( m_pUIFactory )
	{
		pComponent = m_pUIFactory->CreateWindowForm(
			strID, windowCFG, rcGlobalPos );
	}

	if( !pComponent )
		return false;

	UIID uiID = pComponent->GetUIID();	
	bool bNO_ID = ( uiID == NO_ID );

	// 아이디가 없다면;
	if( bNO_ID )
	{
		uiID = ( m_autoMakeID++ ) + FORCE_ID;
		if( INT_MAX <= uiID )
			return false;

		pComponent->SetUIID( uiID );		
		bNO_ID = false;
	}

	// ID 중복검사;
	if( !bNO_ID && At( uiID ) )
		return false;

	m_list.push_back( pComponent );

	// 맵에 등록;
	if( !bNO_ID )
		m_map.insert( std::make_pair( uiID, --m_list.end() ) );

	return true;
}

bool IUIComponent::Erase( UIID uiID, bool bDelete )
{
	MAP_ITER found = m_map.find( uiID );
	if( found != m_map.end() )
	{
		if( bDelete )
			SAFE_DELETE( *(found->second) );

		m_list.erase( found->second );
		m_map.erase( found );
		return true;
	}

	return false;
}

void IUIComponent::ChangeUIFactory( IUIFactory* pUIFactory )
{
	SAFE_DELETE( m_pUIFactory );
	m_pUIFactory = pUIFactory;
}

}