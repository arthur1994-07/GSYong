/**
 * \date	2011/01/25
 * \author	Jun-Hyuk Choi
 * \ref     Unreal3, GPG5 1.4 Reflection
 */
#pragma once

#include <boost/foreach.hpp>
#include <string>

#include "./Property.h"
#include "./Serialize.h"

namespace ic
{
    enum
    {
        EInternal       = 0x00000001,   // 비공개 클래스
    };

    class CRtti;
    class CBaseObject;

    // CRttiObject 클래스를 상속받은 클래스 중에서
    // 실제로 쓰는 곳이 있으면 여기에 자동으로 등록된다
    class CRttiMgr
    {
    public:
        static VOID Destroy();
        static VOID Regist( CRtti* pRtti );

        static size_t Count();

        static CRtti* FindRtti( const TSTRING& strName );
        static CRtti* FindRtti( size_t nIndex );

        static CBaseObject* FindStatic( const TSTRING& strName );
        static CBaseObject* FindStatic( size_t nIndex );

        static CBaseObject* GetStatic( const CRtti* pRtti );
    };

    class CRtti
    {
    public:
        typedef VOID( *Factory )( CBaseObject*& pNew );

    private:
        friend class CRttiMgr;
        TSTRING m_strName;
        size_t m_nIndex;
        const CRtti* m_pBase;
        CBaseObject* m_pStatic;
        Factory m_pFactory;

    public:
        explicit CRtti( const TSTRING& strName, const CRtti* pBase, Factory pFactory );
        ~CRtti();

    public:
        const TSTRING& Name() const { return m_strName; }
        size_t Index() const { return m_nIndex; }
        const CRtti* Base() const { return m_pBase; }
        CBaseObject* Static() const;
    };

    // CRttiObject 에서 Rtti 메타템플릿의 끝을 만들기 위한 클래스다
    // 실제로 선언해서 사용될 수 없다
    class CRootObject
    {
    protected:
        explicit CRootObject() {}
        virtual ~CRootObject() {}

    public:
        static const CRtti* Rtti() { return NULL; }

        virtual PropertyMap* VPropertyMap( size_t nIndex ) { return NULL; }
        virtual CProperty* VProperty( const TSTRING& strKey ) { return NULL; }
        virtual BOOL VHasProperty() { return FALSE; }
    };

    // Rtti 정보와 멤버 변수/함수를 조회하고 사용할 수 있게 해주는 클래스
    // static, virtual 함수 각각 의미를 잘 파악해서 써야함
    template< class TClass, class TBase, DWORD TVer, DWORD TFlags >
    class CRttiObject : public TBase
    {
    public:
        enum { EVersion = TVer, EFlags = TFlags };
        typedef TBase Super;

    private:
        static CRtti s_cRtti;

    private:
        PropertyMap m_mapProperty;

    public:
        virtual ~CRttiObject();

    public:
        static VOID New( CBaseObject*& pNew );
        static BOOL Flags( DWORD nFlags );
        static const CRtti* Rtti() { return &s_cRtti; }
        static const TSTRING& Name() { return s_cRtti.Name(); }
        static size_t Index() { return s_cRtti.Index(); }
        static CBaseObject* Static() { return s_cRtti.Static(); }

        virtual VOID VNew( CBaseObject*& pNew ) const { New( pNew ); }
        virtual BOOL VFlags( DWORD nFlags ) const { return Flags( nFlags ); }
        virtual const CRtti* VRtti() const { return Rtti(); }
        virtual const TSTRING& VName() const { return Name(); }
        virtual size_t VIndex() const { return Index(); }

        virtual VOID VRegistProperty();
        virtual PropertyMap* VPropertyMap( size_t nIndex );
        virtual CProperty* VProperty( const TSTRING& strKey );
        virtual BOOL VHasProperty();

    protected:
        VOID InsertProperty( const TSTRING& strKey, CProperty* pProperty );
    };

    template< class TClass, class TBase, DWORD TVer, DWORD TFlags >
    CRtti CRttiObject< TClass, TBase, TVer, TFlags >::s_cRtti(
        typeid( TClass ).name(), TBase::Rtti(), (CRtti::Factory)TClass::New );

    template< class TClass, class TBase, DWORD TVer, DWORD TFlags >
    CRttiObject< TClass, TBase, TVer, TFlags >::~CRttiObject()
    {
        BOOST_FOREACH( PropertyMapValue& it, m_mapProperty )
            SAFE_DELETE( it.second );
        m_mapProperty.clear();
    }

    template< class TClass, class TBase, DWORD TVer, DWORD TFlags >
    VOID CRttiObject< TClass, TBase, TVer, TFlags >::New( CBaseObject*& pNew )
    {
        TClass* pClass = new TClass;
        pNew = pClass;
        pClass->PostNew();
    }

    template< class TClass, class TBase, DWORD TVer, DWORD TFlags >
    BOOL CRttiObject< TClass, TBase, TVer, TFlags >::Flags( DWORD nFlags )
    {
        return ( ( EFlags & nFlags ) == nFlags ) ? TRUE : FALSE;
    }

    template< class TClass, class TBase, DWORD TVer, DWORD TFlags >
    VOID CRttiObject< TClass, TBase, TVer, TFlags >::VRegistProperty()
    {
        if( VHasProperty() )
            return;
        TClass* pClass = dynamic_cast< TClass* >( this );
        pClass->RegistProperty();
    }

    template< class TClass, class TBase, DWORD TVer, DWORD TFlags >
    PropertyMap* CRttiObject< TClass, TBase, TVer, TFlags >::VPropertyMap( size_t nIndex )
    {
        if( s_cRtti.Index() == nIndex )
            return &m_mapProperty;
        return TBase::VPropertyMap( nIndex );
    }

    template< class TClass, class TBase, DWORD TVer, DWORD TFlags >
    CProperty* CRttiObject< TClass, TBase, TVer, TFlags >::VProperty( const TSTRING& strKey )
    {
        PropertyMapIter fi = m_mapProperty.find( strKey );
        if( fi != m_mapProperty.end() )
            return fi->second;
        return TBase::VProperty( strKey );
    }

    template< class TClass, class TBase, DWORD TVer, DWORD TFlags >
    BOOL CRttiObject< TClass, TBase, TVer, TFlags >::VHasProperty()
    {
        if( !m_mapProperty.empty() )
            return TRUE;
        return TBase::VHasProperty();
    }

    template< class TClass, class TBase, DWORD TVer, DWORD TFlags >
    VOID CRttiObject< TClass, TBase, TVer, TFlags >::InsertProperty( const TSTRING& strKey, CProperty* pProperty )
    {
        CProperty* pFind = VProperty( strKey );
        __gassert_( !pFind );
        m_mapProperty.insert( PropertyMapValue( strKey, pProperty ) );
    }

    // Rtti 정보를 사용하기 위해서 상속받게 될 최상위 클래스
    // 언리얼 엔진의 BaseObject 클래스와 GPG Reflection 클래스를 적절히 짬뽕!!
    class CBaseObject : public CRttiObject< CBaseObject, CRootObject, 1, EInternal >
    {
    private:
        DWORD_PTR m_nIdentity;
        CBaseObject* m_pParent;

    public:
        CBaseObject();

        VOID PostNew() {}
        VOID RegistProperty() {}

    public:
        BOOL IsA( const CRtti* pTarget );
        BOOL IsA( const CBaseObject* pTarget );
        BOOL IsChildOf( const CRtti* pTarget );
        BOOL IsChildOf( const CBaseObject* pTarget );

    public:
        VOID SetIdentity() { m_nIdentity = reinterpret_cast< DWORD_PTR >( this ); }
        VOID SetIdentityForIO( DWORD nIdentity ) { m_nIdentity = static_cast< DWORD_PTR >( nIdentity ); }
        VOID SetParent( CBaseObject* pParent ) { m_pParent = pParent; }

    public:
        virtual const TSTRING& GetClassName() const { return Name(); }

        const DWORD_PTR& GetIdentity() const { return m_nIdentity; }
        const DWORD& GetIdentityForIO() const { return static_cast< DWORD >( m_nIdentity ); }
        CBaseObject* GetParent() const { return m_pParent; }
    };

    struct CompBaseObject
    {
        bool operator () ( const CBaseObject* pLeft, const CBaseObject* pRight )
        {
            return pLeft->GetClassName().compare( pRight->GetClassName() ) < 0;
        }
    };

    // typeid( TClass ).name() 함수에서 이름 앞에 class/struct 붙어서 나온다
    // 이름만으로 충분히 구분이 가능하므로 앞에꺼는 제거해버림
    struct TrimClassName
    {
        static VOID Trim( const TSTRING& strSrc, TSTRING& strDest )
        {
            size_t nSpace = strSrc.find_last_of( _T( " " ) );
            if( nSpace == TSTRING::npos )
                strDest = strSrc;
            else
                strDest = strSrc.substr( nSpace + 1, strSrc.size() - nSpace - 1 );
        }
    };
}

#define IMPLEMENT_HELP_VERSION( TClass, TArg ) \
    SERIALIZE_VERSION( TClass )

#define IMPLEMENT_HELP_REGIST( TClass, TArg ) \
    SERIALIZE_REGIST( TArg, TClass )

#define IMPLEMENT_HELP( TClass, TMethod, TArg ) \
    IMPLEMENT_HELP_##TMethod( TClass, TArg )

#define IMPLEMENT_HELP_BASEOBJECT( TMethod, TArg ) \
    IMPLEMENT_HELP( ic::CBaseObject, TMethod, TArg )