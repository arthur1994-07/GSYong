#include "pch.h"
#include "./TextGroup.h"
#include "./TextNode.h"
#include "./TextObject.h"
#include "./TextUtil.h"

#include "../GUInterface/UIDataType.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CTextGroup::CTextGroup()
	: m_pFont( NULL )
{
	m_listObject.clear();
}

CTextGroup::~CTextGroup()
{
	for( OBJECT_LIST::iterator oi = m_listObject.begin(); oi != m_listObject.end(); ++oi )
		SAFE_DELETE( *oi );
	m_listObject.clear();
}

VOID CTextGroup::FrameMovePrev()
{
	for( OBJECT_LIST::iterator oi = m_listObject.begin(); oi != m_listObject.end(); ++oi )
		( *oi )->FrameMove();
}

VOID CTextGroup::FrameMoveNext()
{
	OBJECT_LIST::iterator oi = m_listObject.begin();
	while( oi != m_listObject.end() )
	{
		if( ( *oi )->GetDelete() )
		{
			SAFE_DELETE( *oi );
			m_listObject.erase( oi++ );
		}
		else
		{
			++oi;
		}
	}
}

BOOL CTextGroup::DrawText( const TCHAR* strText, D3DCOLOR dwColor, FLOAT fX, FLOAT fY )
{
	CTextObject* pObject = GetTextObject( strText );
	if( !pObject )
        return FALSE;

	LPDIRECT3DDEVICEQ pd3dDevice = CTextUtil::Get()->GetDevice();
	if( !pd3dDevice )
        return FALSE;

    if( CTextUtil::m_bUsageRenderQueue == FALSE )
		pd3dDevice->SetFVF( UIVERTEX::D3DFVF_UIVERTEX );
	pObject->DrawText( dwColor, fX, fY );

    return TRUE;
}

BOOL CTextGroup::DrawText( const TCHAR* strText, D3DXCOLOR dwColor, FLOAT fX, FLOAT fY, FLOAT fZoom )
{
	CTextObject* pObject = GetTextObject( strText );
	if( !pObject )
		return FALSE;

	LPDIRECT3DDEVICEQ pd3dDevice = CTextUtil::Get()->GetDevice();
	if( !pd3dDevice )
		return FALSE;

    pd3dDevice->SetFVF( UIVERTEX::D3DFVF_UIVERTEX );
	pObject->DrawText( dwColor, fX, fY, fZoom );

	return TRUE;
}

CTextObject* CTextGroup::CreateTextObject( const TCHAR* strText )
{
	if( !strText )
		return NULL;

	INT iLength = (INT)strlen( strText );
	if( !iLength )
		return NULL;

	CTextObject* pObject = new CTextObject;
	pObject->SetLength( iLength );
	pObject->CreateTextPart( strText, m_pFont );
	m_listObject.push_back( pObject );

	return pObject;
}

CTextObject* CTextGroup::GetTextObject( const TCHAR* strText )
{
	if( !strText )
		return NULL;

	INT iLength = (INT)strlen( strText );
	if( !iLength )
		return NULL;

	for( OBJECT_LIST::iterator oi = m_listObject.begin(); oi != m_listObject.end(); ++oi )
	{
		if( ( *oi )->GetLength() != iLength )
			continue;
		if( strcmp( ( *oi )->GetText(), strText ) )
			continue;
		return *oi;
	}

	return NULL;
}

VOID CTextGroup::DeleteTextObject( CTextObject* pObject )
{
	for( OBJECT_LIST::iterator oi = m_listObject.begin(); oi != m_listObject.end(); ++oi )
	{
		if( *oi == pObject )
		{
			SAFE_DELETE( *oi );
			m_listObject.erase( oi );
			return;
		}
	}
}