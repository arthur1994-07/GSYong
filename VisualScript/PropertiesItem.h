/**
 * \date	2011/01/25
 * \author	Jun-Hyuk Choi
 */
#pragma once

#include "../InternalCommonLib/Core/Property.h"

typedef VOID( CALLBACK *ONVALUECHANGED )( CXTPPropertyGridItem* pItem );

template< class TType, class TBind >
class CPropertyGridItemBase
{
public:
    typedef std::pair< TType*, ic::FunctionVoid > DataPair;

protected:
    CXTPPropertyGridItem* m_pItem;
    TBind m_tValue;
    std::vector< DataPair > m_vecData;
    ONVALUECHANGED m_pCallBack;
    ONVALUECHANGED m_pOnButton;

public:
    CPropertyGridItemBase();
    virtual ~CPropertyGridItemBase() {}

public:
    virtual VOID BindPtr( TType* pValuePtr, ic::FunctionVoid pOnChanged );

protected:
    virtual VOID OnValueChanged();
    virtual VOID OnChangedData( size_t nIndex );
    virtual VOID OnCallBack() { if( m_pCallBack ) m_pCallBack( m_pItem ); }
    virtual VOID OnButton() { if( m_pOnButton ) m_pOnButton( m_pItem ); }

public:
    virtual VOID SetCallBack( ONVALUECHANGED pCallBack ) { m_pCallBack = pCallBack; }
    virtual VOID SetOnButton( ONVALUECHANGED pOnButton ) { m_pOnButton = pOnButton; }
};

template< class TType, class TBind >
CPropertyGridItemBase< TType, TBind >::CPropertyGridItemBase()
    : m_pItem( NULL )
    , m_pCallBack( NULL )
    , m_pOnButton( NULL )
{
}

template< class TType, class TBind >
inline VOID CPropertyGridItemBase< TType, TBind >::BindPtr( TType* pValuePtr, ic::FunctionVoid pOnChanged )
{
    m_vecData.push_back( DataPair( pValuePtr, pOnChanged ) );
}

template< class TType, class TBind >
inline VOID CPropertyGridItemBase< TType, TBind >::OnValueChanged()
{
    size_t nSize = m_vecData.size();
    for( size_t i=0; i<nSize; ++i )
    {
        *( m_vecData[ i ].first ) = static_cast< TType >( m_tValue );
        OnChangedData( i );
    }
    OnCallBack();
}

template<>
inline VOID CPropertyGridItemBase< bool, BOOL >::OnValueChanged()
{
    size_t nSize = m_vecData.size();
    for( size_t i=0; i<nSize; ++i )
    {
        *( m_vecData[ i ].first ) = ic::BOOLTobool( m_tValue );
        OnChangedData( i );
    }
    OnCallBack();
}

template<>
inline VOID CPropertyGridItemBase< TSTRING, CString >::OnValueChanged()
{
    size_t nSize = m_vecData.size();
    for( size_t i=0; i<nSize; ++i )
    {
        *( m_vecData[ i ].first ) = m_tValue.GetString();
        OnChangedData( i );
    }
    OnCallBack();
}

template< class TType, class TBind >
inline VOID CPropertyGridItemBase< TType, TBind >::OnChangedData( size_t nIndex )
{
    if( m_vecData[ nIndex ].second )
        m_vecData[ nIndex  ].second();
}

class CPropertyGridItembool : public CPropertyGridItemBase< bool, BOOL >, public CXTPPropertyGridItemBool
{
public:
    CPropertyGridItembool( const TSTRING& strCaption, BOOL bDefault = FALSE );

public:
    virtual VOID BindPtr( bool* pValuePtr, ic::FunctionVoid pOnChanged );

protected:
    virtual VOID OnValueChanged( CString strValue );
    virtual VOID OnInplaceButtonDown( CXTPPropertyGridInplaceButton* pButton );
};

class CPropertyGridItemBool : public CPropertyGridItemBase< BOOL, BOOL >, public CXTPPropertyGridItemBool
{
public:
    CPropertyGridItemBool( const TSTRING& strCaption, BOOL bDefault = FALSE );

public:
    virtual VOID BindPtr( BOOL* pValuePtr, ic::FunctionVoid pOnChanged );

protected:
    virtual VOID OnValueChanged( CString strValue );
    virtual VOID OnInplaceButtonDown( CXTPPropertyGridInplaceButton* pButton );
};

class CPropertyGridItemColor : public CPropertyGridItemBase< DWORD, DWORD >, public CXTPPropertyGridItemColor
{
public:
    CPropertyGridItemColor( const TSTRING& strCaption, DWORD nDefault = 0 );

public:
    virtual VOID BindPtr( DWORD* pValuePtr, ic::FunctionVoid pOnChanged );

protected:
    virtual VOID OnValueChanged( CString strValue );
    virtual VOID OnInplaceButtonDown( CXTPPropertyGridInplaceButton* pButton );
};

class CPropertyGridItemDouble : public CPropertyGridItemBase< DOUBLE, DOUBLE >, public CXTPPropertyGridItemDouble
{
public:
    CPropertyGridItemDouble( const TSTRING& strCaption, const TSTRING& strFormat, DOUBLE fDefault = 0 );

public:
    virtual VOID BindPtr( DOUBLE* pValuePtr, ic::FunctionVoid pOnChanged );

protected:
    virtual VOID OnValueChanged( CString strValue );
    virtual VOID OnInplaceButtonDown( CXTPPropertyGridInplaceButton* pButton );
};

class CPropertyGridItemFlags : public CPropertyGridItemBase< DWORD, INT >, public CXTPPropertyGridItemOption
{
public:
    CPropertyGridItemFlags( const TSTRING& strCaption, INT nDefault = 0 );

public:
    virtual VOID BindPtr( DWORD* pValuePtr, ic::FunctionVoid pOnChanged );

protected:
    virtual VOID OnValueChanged( CString strValue );
    virtual VOID OnInplaceButtonDown( CXTPPropertyGridInplaceButton* pButton );
};

class CPropertyGridItemFloat : public CPropertyGridItemBase< FLOAT, DOUBLE >, public CXTPPropertyGridItemDouble
{
public:
    CPropertyGridItemFloat( const TSTRING& strCaption, const TSTRING& strFormat, DOUBLE fDefault = 0 );

public:
    virtual VOID BindPtr( FLOAT* pValuePtr, ic::FunctionVoid pOnChanged );

protected:
    virtual VOID OnValueChanged( CString strValue );
    virtual VOID OnInplaceButtonDown( CXTPPropertyGridInplaceButton* pButton );
};

class CPropertyGridItemInt : public CPropertyGridItemBase< INT, LONG >, public CXTPPropertyGridItemNumber
{
public:
    CPropertyGridItemInt( const TSTRING& strCaption, LONG nDefault = 0 );

public:
    virtual VOID BindPtr( INT* pValuePtr, ic::FunctionVoid pOnChanged );

protected:
    virtual VOID OnValueChanged( CString strValue );
    virtual VOID OnInplaceButtonDown( CXTPPropertyGridInplaceButton* pButton );
};

class CPropertyGridItemSelect : public CPropertyGridItemBase< DWORD, INT >, public CXTPPropertyGridItemEnum
{
public:
    CPropertyGridItemSelect( const TSTRING& strCaption, INT nDefault = 0 );

public:
    virtual VOID BindPtr( DWORD* pValuePtr, ic::FunctionVoid pOnChanged );

protected:
    virtual VOID OnValueChanged( CString strValue );
    virtual VOID OnInplaceButtonDown( CXTPPropertyGridInplaceButton* pButton );
};

class CPropertyGridItemString : public CPropertyGridItemBase< TSTRING, CString >, public CXTPPropertyGridItem
{
public:
    CPropertyGridItemString( const TSTRING& strCaption, CString strDefault = _T( "" ) );

public:
    virtual VOID BindPtr( TSTRING* pValuePtr, ic::FunctionVoid pOnChanged );

protected:
    virtual VOID OnValueChanged( CString strValue );
    virtual VOID OnInplaceButtonDown( CXTPPropertyGridInplaceButton* pButton );
};

class CPropertyGridItemVector2 : public CPropertyGridItemBase< D3DXVECTOR2, D3DXVECTOR2 >, public CXTPPropertyGridItem
{
protected:
    CPropertyGridItemFloat* m_pX;
    CPropertyGridItemFloat* m_pY;

public:
    CPropertyGridItemVector2( const TSTRING& strCaption, const TSTRING& strFormat, D3DXVECTOR2 vDefault = D3DXVECTOR2( 0.0f, 0.0f ) );

public:
    virtual VOID BindPtr( D3DXVECTOR2* pValuePtr, ic::FunctionVoid pOnChanged );
    virtual VOID SetCallBack( ONVALUECHANGED pCallBack );
    virtual VOID SetOnButton( ONVALUECHANGED pOnButton );

protected:
    virtual VOID OnAddChildItem();
};

class CPropertyGridItemVector3 : public CPropertyGridItemBase< D3DXVECTOR3, D3DXVECTOR3 >, public CXTPPropertyGridItem
{
protected:
    CPropertyGridItemFloat* m_pX;
    CPropertyGridItemFloat* m_pY;
    CPropertyGridItemFloat* m_pZ;

public:
    CPropertyGridItemVector3( const TSTRING& strCaption, const TSTRING& strFormat, D3DXVECTOR3 vDefault = D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) );

public:
    virtual VOID BindPtr( D3DXVECTOR3* pValuePtr, ic::FunctionVoid pOnChanged );
    virtual VOID SetCallBack( ONVALUECHANGED pCallBack );
    virtual VOID SetOnButton( ONVALUECHANGED pOnButton );

protected:
    virtual VOID OnAddChildItem();
};

class CPropertyGridItemVector4 : public CPropertyGridItemBase< D3DXVECTOR4, D3DXVECTOR4 >, public CXTPPropertyGridItem
{
protected:
    CPropertyGridItemFloat* m_pX;
    CPropertyGridItemFloat* m_pY;
    CPropertyGridItemFloat* m_pZ;
    CPropertyGridItemFloat* m_pW;

public:
    CPropertyGridItemVector4( const TSTRING& strCaption, const TSTRING& strFormat, D3DXVECTOR4 vDefault = D3DXVECTOR4( 0.0f, 0.0f, 0.0f, 0.0f ) );

public:
    virtual VOID BindPtr( D3DXVECTOR4* pValuePtr, ic::FunctionVoid pOnChanged );
    virtual VOID SetCallBack( ONVALUECHANGED pCallBack );
    virtual VOID SetOnButton( ONVALUECHANGED pOnButton );

protected:
    virtual VOID OnAddChildItem();
};