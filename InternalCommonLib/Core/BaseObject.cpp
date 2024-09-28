#include "stdafx.h"

#include <boost/functional/hash.hpp>
#include <map>
#include <vector>

#include "./BaseObject.h"

#include "../DebugInclude.h"

namespace ic
{
    typedef std::vector< CRtti* > RttiVec;
    typedef boost::hash< TSTRING > RttiHash;
    typedef std::map< size_t, CRtti* > RttiMap;
    typedef RttiMap::value_type RttiMapValue;
    typedef RttiMap::iterator RttiMapIter;

    static BOOL s_bCreateRtti = FALSE;
    static RttiVec* s_pRttiVec = NULL;
    static RttiHash* s_pRttiHash = NULL;
    static RttiMap* s_pRttiMap = NULL;

    static VOID Create()
    {
        if( s_bCreateRtti )
            return;

        s_bCreateRtti = TRUE;
        s_pRttiVec = new RttiVec;
        s_pRttiHash = new RttiHash;
        s_pRttiMap = new RttiMap;
    };

    VOID CRttiMgr::Destroy()
    {
        SAFE_DELETE( s_pRttiVec );
        SAFE_DELETE( s_pRttiHash );
        SAFE_DELETE( s_pRttiMap );
        s_bCreateRtti = FALSE;
    }

    VOID CRttiMgr::Regist( CRtti* pRtti )
    {
        Create();

        size_t nHash = ( *s_pRttiHash )( pRtti->m_strName );
        RttiMapIter it = s_pRttiMap->find( nHash );
        if( it != s_pRttiMap->end() )
            return;

        pRtti->m_nIndex = s_pRttiVec->size();

        s_pRttiVec->push_back( pRtti );
        s_pRttiMap->insert( RttiMapValue( nHash, pRtti ) );
    }

    size_t CRttiMgr::Count()
    {
        __gassert_( s_pRttiVec );

        return s_pRttiVec->size();
    }

    CRtti* CRttiMgr::FindRtti( const TSTRING& strName )
    {
        __gassert_( s_pRttiHash );

        size_t nHash = ( *s_pRttiHash )( strName );
        RttiMapIter it = s_pRttiMap->find( nHash );
        if( it == s_pRttiMap->end() )
            return NULL;

        return it->second;
    }

    CRtti* CRttiMgr::FindRtti( size_t nIndex )
    {
        __gassert_( s_pRttiVec );

        if( nIndex < 0 || nIndex >= s_pRttiVec->size() )
            return NULL;

        return ( *s_pRttiVec )[ nIndex ];
    }

    CBaseObject* CRttiMgr::FindStatic( const TSTRING& strName )
    {
        return GetStatic( FindRtti( strName ) );
    }

    CBaseObject* CRttiMgr::FindStatic( size_t nIndex )
    {
        return GetStatic( FindRtti( nIndex ) );
    }

    CBaseObject* CRttiMgr::GetStatic( const CRtti* pRtti )
    {
        return pRtti ? pRtti->Static() : NULL;
    }

    CRtti::CRtti( const TSTRING& strName, const CRtti* pBase, Factory pFactory )
        : m_nIndex( 0 )
        , m_pBase( pBase )
        , m_pStatic( NULL )
        , m_pFactory( pFactory )
    {
        TrimClassName::Trim( strName, m_strName );
        CRttiMgr::Regist( this );
    }

    CRtti::~CRtti()
    {
        SAFE_DELETE( m_pStatic );
    }

    CBaseObject* CRtti::Static() const
    {
        if( m_pStatic )
            return m_pStatic;

        if( m_pFactory )
            m_pFactory( const_cast< CBaseObject* >( m_pStatic ) );

        return m_pStatic;
    }

    CBaseObject::CBaseObject()
        : m_pParent( NULL )
    {
        SetIdentity();
    }

    BOOL CBaseObject::IsA( const CRtti* pTarget )
    {
        return VRtti() == pTarget ? TRUE : FALSE;
    }

    BOOL CBaseObject::IsA( const CBaseObject* pTarget )
    {
        return IsA( pTarget->VRtti() );
    }

    BOOL CBaseObject::IsChildOf( const CRtti* pTarget )
    {
        const CRtti* pThis = VRtti();
        while( pThis )
        {
            if( pThis == pTarget )
                return TRUE;
            pThis = pThis->Base();
        }

        return FALSE;
    }

    BOOL CBaseObject::IsChildOf( const CBaseObject* pTarget )
    {
        return IsChildOf( pTarget->VRtti() );
    }
}