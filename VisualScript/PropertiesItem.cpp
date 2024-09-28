#include "stdafx.h"

#include "./PropertiesItem.h"

#include "../SigmaCore/DebugInclude.h"

CPropertyGridItembool::CPropertyGridItembool( const TSTRING& strCaption, BOOL bDefault )
    : CXTPPropertyGridItemBool( strCaption.c_str(), bDefault, &m_tValue )
{
    m_pItem = dynamic_cast< CXTPPropertyGridItem* >( this );
    m_tValue = bDefault;
}

VOID CPropertyGridItembool::BindPtr( bool* pValuePtr, ic::FunctionVoid pOnChanged )
{
    if( m_vecData.empty() )
    {
        SetBool( ic::boolToBOOL( *pValuePtr ) );
    }
    else
    {
        if( *m_vecData[ 0 ].first != *pValuePtr )
            CXTPPropertyGridItem::SetValue( _T( "" ) );
    }

    CPropertyGridItemBase::BindPtr( pValuePtr, pOnChanged );
}

VOID CPropertyGridItembool::OnValueChanged( CString strValue )
{
    CXTPPropertyGridItemBool::OnValueChanged( strValue );
    CPropertyGridItemBase::OnValueChanged();
}

VOID CPropertyGridItembool::OnInplaceButtonDown( CXTPPropertyGridInplaceButton* pButton )
{
    CPropertyGridItemBase::OnButton();
}

CPropertyGridItemBool::CPropertyGridItemBool( const TSTRING& strCaption, BOOL bDefault )
    : CXTPPropertyGridItemBool( strCaption.c_str(), bDefault, &m_tValue )
{
    m_pItem = dynamic_cast< CXTPPropertyGridItem* >( this );
    m_tValue = bDefault;
}

VOID CPropertyGridItemBool::BindPtr( BOOL* pValuePtr, ic::FunctionVoid pOnChanged )
{
    if( m_vecData.empty() )
    {
        SetBool( *pValuePtr );
    }
    else
    {
        if( *m_vecData[ 0 ].first != *pValuePtr )
            CXTPPropertyGridItem::SetValue( _T( "" ) );
    }

    CPropertyGridItemBase::BindPtr( pValuePtr, pOnChanged );
}

VOID CPropertyGridItemBool::OnValueChanged( CString strValue )
{
    CXTPPropertyGridItemBool::OnValueChanged( strValue );
    CPropertyGridItemBase::OnValueChanged();
}

VOID CPropertyGridItemBool::OnInplaceButtonDown( CXTPPropertyGridInplaceButton* pButton )
{
    CPropertyGridItemBase::OnButton();
}

CPropertyGridItemColor::CPropertyGridItemColor( const TSTRING& strCaption, DWORD nDefault )
    : CXTPPropertyGridItemColor( strCaption.c_str(), nDefault, &m_tValue )
{
    m_pItem = dynamic_cast< CXTPPropertyGridItem* >( this );
    m_tValue = nDefault;
}

VOID CPropertyGridItemColor::BindPtr( DWORD* pValuePtr, ic::FunctionVoid pOnChanged )
{
    if( m_vecData.empty() )
    {
        D3DXCOLOR sColor( *pValuePtr );
        DWORD nColor = RGB( BYTE( sColor.r * 255.0f ), BYTE( sColor.g * 255.0f ), BYTE( sColor.b * 255.0f ) );

        SetColor( nColor );
    }
    else
    {
        if( *m_vecData[ 0 ].first != *pValuePtr )
            CXTPPropertyGridItem::SetValue( _T( "" ) );
    }

    CPropertyGridItemBase::BindPtr( pValuePtr, pOnChanged );
}

VOID CPropertyGridItemColor::OnValueChanged( CString strValue )
{
	CXTPPropertyGridItemColor::OnValueChanged( strValue );

    D3DXCOLOR sValue;
    sValue.r = GetRValue( m_tValue ) / 255.0f;
    sValue.g = GetGValue( m_tValue ) / 255.0f;
    sValue.b = GetBValue( m_tValue ) / 255.0f;

    size_t nSize = m_vecData.size();
    for( size_t i=0; i<nSize; ++i )
    {
        D3DXCOLOR sColor( *( m_vecData[ i ].first ) );
        sColor.r = sValue.r;
        sColor.g = sValue.g;
        sColor.b = sValue.b;
        *( m_vecData[ i ].first ) = sColor;

        OnChangedData( i );
    }

    CPropertyGridItemBase::OnCallBack();
}

VOID CPropertyGridItemColor::OnInplaceButtonDown( CXTPPropertyGridInplaceButton* pButton )
{
    CPropertyGridItemBase::OnButton();
}

CPropertyGridItemDouble::CPropertyGridItemDouble( const TSTRING& strCaption, const TSTRING& strFormat, DOUBLE fDefault )
    : CXTPPropertyGridItemDouble( strCaption.c_str(), fDefault, strFormat.c_str(), &m_tValue )
{
    m_pItem = dynamic_cast< CXTPPropertyGridItem* >( this );
    m_tValue = fDefault;
}

VOID CPropertyGridItemDouble::BindPtr( DOUBLE* pValuePtr, ic::FunctionVoid pOnChanged )
{
    if( m_vecData.empty() )
    {
        SetDouble( *pValuePtr );
    }
    else
    {
        if( *m_vecData[ 0 ].first != *pValuePtr )
            CXTPPropertyGridItem::SetValue( _T( "" ) );
    }

    CPropertyGridItemBase::BindPtr( pValuePtr, pOnChanged );
}

VOID CPropertyGridItemDouble::OnValueChanged( CString strValue )
{
    CXTPPropertyGridItemDouble::OnValueChanged( strValue );
	CPropertyGridItemBase::OnValueChanged();
}

VOID CPropertyGridItemDouble::OnInplaceButtonDown( CXTPPropertyGridInplaceButton* pButton )
{
    CPropertyGridItemBase::OnButton();
}

CPropertyGridItemFlags::CPropertyGridItemFlags( const TSTRING& strCaption, INT nDefault )
    : CXTPPropertyGridItemOption( strCaption.c_str(), nDefault, &m_tValue )
{
    m_pItem = dynamic_cast< CXTPPropertyGridItem* >( this );
    m_tValue = nDefault;
}

VOID CPropertyGridItemFlags::BindPtr( DWORD* pValuePtr, ic::FunctionVoid pOnChanged )
{
    if( m_vecData.empty() )
    {
        SetOption( static_cast< INT >( *pValuePtr ) );
    }
    else
    {
        if( *m_vecData[ 0 ].first != *pValuePtr )
            SetOption( 0 );
    }

    CPropertyGridItemBase::BindPtr( pValuePtr, pOnChanged );
}

VOID CPropertyGridItemFlags::OnValueChanged( CString strValue )
{
	CXTPPropertyGridItemOption::OnValueChanged( strValue );
    CPropertyGridItemBase::OnValueChanged();
}

VOID CPropertyGridItemFlags::OnInplaceButtonDown( CXTPPropertyGridInplaceButton* pButton )
{
    CPropertyGridItemBase::OnButton();
}

CPropertyGridItemFloat::CPropertyGridItemFloat( const TSTRING& strCaption, const TSTRING& strFormat, DOUBLE fDefault )
    : CXTPPropertyGridItemDouble( strCaption.c_str(), fDefault, strFormat.c_str(), &m_tValue )
{
    m_pItem = dynamic_cast< CXTPPropertyGridItem* >( this );
    m_tValue = fDefault;
}

VOID CPropertyGridItemFloat::BindPtr( FLOAT* pValuePtr, ic::FunctionVoid pOnChanged )
{
    if( m_vecData.empty() )
    {
        SetDouble( static_cast< DOUBLE >( *pValuePtr ) );
    }
    else
    {
        if( *m_vecData[ 0 ].first != static_cast< DOUBLE >( *pValuePtr ) )
            CXTPPropertyGridItem::SetValue( _T( "" ) );
    }

    CPropertyGridItemBase::BindPtr( pValuePtr, pOnChanged );
}

VOID CPropertyGridItemFloat::OnValueChanged( CString strValue )
{
    CXTPPropertyGridItemDouble::OnValueChanged( strValue );
	CPropertyGridItemBase::OnValueChanged();
}

VOID CPropertyGridItemFloat::OnInplaceButtonDown( CXTPPropertyGridInplaceButton* pButton )
{
    CPropertyGridItemBase::OnButton();
}

CPropertyGridItemInt::CPropertyGridItemInt( const TSTRING& strCaption, LONG nDefault )
    : CXTPPropertyGridItemNumber( strCaption.c_str(), nDefault, &m_tValue )
{
    m_pItem = dynamic_cast< CXTPPropertyGridItem* >( this );
    m_nValue = nDefault;
}

VOID CPropertyGridItemInt::BindPtr( INT* pValuePtr, ic::FunctionVoid pOnChanged )
{
    if( m_vecData.empty() )
    {
        SetNumber( static_cast< LONG >( *pValuePtr ) );
    }
    else
    {
        if( *m_vecData[ 0 ].first != static_cast< LONG >( *pValuePtr ) )
            CXTPPropertyGridItem::SetValue( _T( "" ) );
    }

    CPropertyGridItemBase::BindPtr( pValuePtr, pOnChanged );
}

VOID CPropertyGridItemInt::OnValueChanged( CString strValue )
{
    CXTPPropertyGridItemNumber::OnValueChanged( strValue );
	CPropertyGridItemBase::OnValueChanged();
}

VOID CPropertyGridItemInt::OnInplaceButtonDown( CXTPPropertyGridInplaceButton* pButton )
{
    CPropertyGridItemBase::OnButton();
}

CPropertyGridItemSelect::CPropertyGridItemSelect( const TSTRING& strCaption, INT nDefault )
    : CXTPPropertyGridItemEnum( strCaption.c_str(), nDefault, &m_tValue )
{
    m_pItem = dynamic_cast< CXTPPropertyGridItem* >( this );
    m_tValue = nDefault;
}

VOID CPropertyGridItemSelect::BindPtr( DWORD* pValuePtr, ic::FunctionVoid pOnChanged )
{
    if( m_vecData.empty() )
    {
        SetEnum( static_cast< INT >( *pValuePtr ) );
    }
    else
    {
        if( *m_vecData[ 0 ].first != *pValuePtr )
            SetEnum( 0 );
    }

    CPropertyGridItemBase::BindPtr( pValuePtr, pOnChanged );
}

VOID CPropertyGridItemSelect::OnValueChanged( CString strValue )
{
	CXTPPropertyGridItemEnum::OnValueChanged( strValue );
    CPropertyGridItemBase::OnValueChanged();
}

VOID CPropertyGridItemSelect::OnInplaceButtonDown( CXTPPropertyGridInplaceButton* pButton )
{
    CPropertyGridItemBase::OnButton();
}

CPropertyGridItemString::CPropertyGridItemString( const TSTRING& strCaption, CString strDefault )
    : CXTPPropertyGridItem( strCaption.c_str(), strDefault, &m_tValue )
{
    m_pItem = dynamic_cast< CXTPPropertyGridItem* >( this );
    m_tValue = strDefault;
}

VOID CPropertyGridItemString::BindPtr( TSTRING* pValuePtr, ic::FunctionVoid pOnChanged )
{
    if( m_vecData.empty() )
    {
        SetValue( pValuePtr->c_str() );
    }
    else
    {
        if( m_vecData[ 0 ].first->compare( pValuePtr->c_str() ) != 0 )
            CXTPPropertyGridItem::SetValue( _T( "" ) );
    }

    CPropertyGridItemBase::BindPtr( pValuePtr, pOnChanged );
}

VOID CPropertyGridItemString::OnValueChanged( CString strValue )
{
	CXTPPropertyGridItem::OnValueChanged( strValue );
    CPropertyGridItemBase::OnValueChanged();
}

VOID CPropertyGridItemString::OnInplaceButtonDown( CXTPPropertyGridInplaceButton* pButton )
{
    CPropertyGridItemBase::OnButton();
}

CPropertyGridItemVector2::CPropertyGridItemVector2( const TSTRING& strCaption, const TSTRING& strFormat, D3DXVECTOR2 vDefault )
    : CXTPPropertyGridItem( strCaption.c_str(), _T( "" ), NULL )
    , m_pX( new CPropertyGridItemFloat( _T( "X" ), strFormat, vDefault.x ) )
    , m_pY( new CPropertyGridItemFloat( _T( "Y" ), strFormat, vDefault.y ) )
{
    m_pItem = dynamic_cast< CXTPPropertyGridItem* >( this );
    m_tValue = vDefault;

    SetFlags( 0 );
}

VOID CPropertyGridItemVector2::BindPtr( D3DXVECTOR2* pValuePtr, ic::FunctionVoid pOnChanged )
{
    m_pX->BindPtr( &pValuePtr->x, pOnChanged );
    m_pY->BindPtr( &pValuePtr->y, pOnChanged );

    CPropertyGridItemBase::BindPtr( pValuePtr, pOnChanged );
}

VOID CPropertyGridItemVector2::SetCallBack( ONVALUECHANGED pCallBack )
{
    m_pX->SetCallBack( pCallBack );
    m_pY->SetCallBack( pCallBack );
}

VOID CPropertyGridItemVector2::SetOnButton( ONVALUECHANGED pOnButton )
{
    m_pX->SetOnButton( pOnButton );
    m_pY->SetOnButton( pOnButton );
}

VOID CPropertyGridItemVector2::OnAddChildItem()
{
    AddChildItem( m_pX );
    AddChildItem( m_pY );
}

CPropertyGridItemVector3::CPropertyGridItemVector3( const TSTRING& strCaption, const TSTRING& strFormat, D3DXVECTOR3 vDefault )
    : CXTPPropertyGridItem( strCaption.c_str(), _T( "" ), NULL )
    , m_pX( new CPropertyGridItemFloat( _T( "X" ), strFormat, vDefault.x ) )
    , m_pY( new CPropertyGridItemFloat( _T( "Y" ), strFormat, vDefault.y ) )
    , m_pZ( new CPropertyGridItemFloat( _T( "Z" ), strFormat, vDefault.z ) )
{
    m_pItem = dynamic_cast< CXTPPropertyGridItem* >( this );
    m_tValue = vDefault;

    SetFlags( 0 );
}

VOID CPropertyGridItemVector3::BindPtr( D3DXVECTOR3* pValuePtr, ic::FunctionVoid pOnChanged )
{
    m_pX->BindPtr( &pValuePtr->x, pOnChanged );
    m_pY->BindPtr( &pValuePtr->y, pOnChanged );
    m_pZ->BindPtr( &pValuePtr->z, pOnChanged );

    CPropertyGridItemBase::BindPtr( pValuePtr, pOnChanged );
}

VOID CPropertyGridItemVector3::SetCallBack( ONVALUECHANGED pCallBack )
{
    m_pX->SetCallBack( pCallBack );
    m_pY->SetCallBack( pCallBack );
    m_pZ->SetCallBack( pCallBack );
}

VOID CPropertyGridItemVector3::SetOnButton( ONVALUECHANGED pOnButton )
{
    m_pX->SetOnButton( pOnButton );
    m_pY->SetOnButton( pOnButton );
    m_pZ->SetOnButton( pOnButton );
}

VOID CPropertyGridItemVector3::OnAddChildItem()
{
    AddChildItem( m_pX );
    AddChildItem( m_pY );
    AddChildItem( m_pZ );
}

CPropertyGridItemVector4::CPropertyGridItemVector4( const TSTRING& strCaption, const TSTRING& strFormat, D3DXVECTOR4 vDefault )
    : CXTPPropertyGridItem( strCaption.c_str(), _T( "" ), NULL )
    , m_pX( new CPropertyGridItemFloat( _T( "X" ), strFormat, vDefault.x ) )
    , m_pY( new CPropertyGridItemFloat( _T( "Y" ), strFormat, vDefault.y ) )
    , m_pZ( new CPropertyGridItemFloat( _T( "Z" ), strFormat, vDefault.z ) )
    , m_pW( new CPropertyGridItemFloat( _T( "W" ), strFormat, vDefault.w ) )
{
    m_pItem = dynamic_cast< CXTPPropertyGridItem* >( this );
    m_tValue = vDefault;

    SetFlags( 0 );
}

VOID CPropertyGridItemVector4::BindPtr( D3DXVECTOR4* pValuePtr, ic::FunctionVoid pOnChanged )
{
    m_pX->BindPtr( &pValuePtr->x, pOnChanged );
    m_pY->BindPtr( &pValuePtr->y, pOnChanged );
    m_pZ->BindPtr( &pValuePtr->z, pOnChanged );
    m_pW->BindPtr( &pValuePtr->w, pOnChanged );

    CPropertyGridItemBase::BindPtr( pValuePtr, pOnChanged );
}

VOID CPropertyGridItemVector4::SetCallBack( ONVALUECHANGED pCallBack )
{
    m_pX->SetCallBack( pCallBack );
    m_pY->SetCallBack( pCallBack );
    m_pZ->SetCallBack( pCallBack );
    m_pW->SetCallBack( pCallBack );
}

VOID CPropertyGridItemVector4::SetOnButton( ONVALUECHANGED pOnButton )
{
    m_pX->SetOnButton( pOnButton );
    m_pY->SetOnButton( pOnButton );
    m_pZ->SetOnButton( pOnButton );
    m_pW->SetOnButton( pOnButton );
}

VOID CPropertyGridItemVector4::OnAddChildItem()
{
    AddChildItem( m_pX );
    AddChildItem( m_pY );
    AddChildItem( m_pZ );
    AddChildItem( m_pW );
}