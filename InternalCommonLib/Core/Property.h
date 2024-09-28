/**
 * \date	2011/01/27
 * \author	Jun-Hyuk Choi
 */
#pragma once

#include <boost/function.hpp>
#include <map>
#include <string>
#include <vector>

#include "./CommonUtil.h"

namespace ic
{
    class CBaseObject;

    enum EPropertyType
    {
        ENone,

        Ebool,
        EBool,
        EByte,
        EColor,
        EDouble,
        EDWord,
        EFlags,
        EFloat,
        EInt,
        ELongLong,
        EObject,
        ESelect,
        EString,
        EVector2,
        EVector3,
        EVector4,
        EWord,

        EFunctionArgs,
        EFunctionVoid,
    };

    class CProperty
    {
    public:
        virtual ~CProperty() {}

    public:
        static BYTE Type() { return ENone; }
        virtual BYTE VType() { return Type(); }
    };

    typedef std::vector< CProperty* > PropertyVec;
    typedef PropertyVec::iterator PropertyVecIter;

    class IPropertyValue : public CProperty
    {
    public:
        virtual VOID operator = ( bool tValue ) {}
        virtual VOID operator = ( BYTE tValue ) {}
        virtual VOID operator = ( DOUBLE tValue ) {}
        virtual VOID operator = ( DWORD tValue ) {}
        virtual VOID operator = ( FLOAT tValue ) {}
        virtual VOID operator = ( INT tValue ) {}
        virtual VOID operator = ( LONGLONG tValue ) {}
        virtual VOID operator = ( CBaseObject* tValue ) {}
        virtual VOID operator = ( TSTRING tValue ) {}
        virtual VOID operator = ( D3DXVECTOR2 tValue ) {}
        virtual VOID operator = ( D3DXVECTOR3 tValue ) {}
        virtual VOID operator = ( D3DXVECTOR4 tValue ) {}
        virtual VOID operator = ( WORD tValue ) {}

    public:
        virtual operator bool () { return false; }
        virtual operator BYTE () { return 0; }
        virtual operator DOUBLE () { return 0.0f; }
        virtual operator DWORD () { return 0; }
        virtual operator FLOAT () { return 0.0f; }
        virtual operator INT () { return 0; }
        virtual operator LONGLONG () { return 0; }
        virtual operator CBaseObject* () { return (CBaseObject*)NULL; }
        virtual operator TSTRING () { return _T( "" ); }
        virtual operator D3DXVECTOR2 () { return D3DXVECTOR2( 0.0f, 0.0f ); }
        virtual operator D3DXVECTOR3 () { return D3DXVECTOR3( 0.0f, 0.0f, 0.0f ); }
        virtual operator D3DXVECTOR4 () { return D3DXVECTOR4( 0.0f, 0.0f, 0.0f, 0.0f ); }
        virtual operator WORD () { return 0; }
    };

    typedef std::vector< IPropertyValue* > IPropertyValueVec;
    typedef IPropertyValueVec::iterator IPropertyValueVecIter;

    typedef boost::function< VOID( IPropertyValueVec& ) > FunctionArgs;
    typedef boost::function< VOID() > FunctionVoid;

    template< class TType >
    class CPropertyValue : public IPropertyValue
    {
    private:
        TType* m_pValue;
        FunctionVoid m_pOnChanged;

    public:
        CPropertyValue()
            : m_pValue( NULL )
            , m_pOnChanged( NULL )
        {
        }

    public:
        VOID SetPtr( TType* pValue ) { m_pValue = pValue; }
        VOID SetCall( FunctionVoid pOnChanged ) { m_pOnChanged = pOnChanged; }
        VOID SetValue( TType tValue ) { *m_pValue = tValue; OnChanged(); }

    public:
        TType* GetPtr() { return m_pValue; }
        FunctionVoid GetCall() { return m_pOnChanged; }
        TType& GetValue() { return *m_pValue; }

    protected:
        VOID OnChanged() { if( m_pOnChanged ) m_pOnChanged(); }

        template< class TCast >
        VOID SetValueCast( TCast tValue ) { SetValue( static_cast< TType >( tValue ) ); }
    };

    class CPropertybool : public CPropertyValue< bool >
    {
    public:
        static BYTE Type() { return Ebool; }
        virtual BYTE VType() { return Type(); }

    public:
        virtual VOID operator = ( bool tValue ) { SetValue( tValue ); }
        virtual VOID operator = ( INT tValue ) { SetValue( BOOLTobool( tValue ) ); }

    public:
        virtual operator bool () { return GetValue(); }
        virtual operator INT () { return boolToBOOL( GetValue() ); }
    };

    class CPropertyBool : public CPropertyValue< BOOL >
    {
    public:
        static BYTE Type() { return EBool; }
        virtual BYTE VType() { return Type(); }

    public:
        virtual VOID operator = ( bool tValue ) { SetValue( boolToBOOL( tValue ) ); }
        virtual VOID operator = ( INT tValue ) { SetValue( tValue ); }

    public:
        virtual operator bool () { return BOOLTobool( GetValue() ); }
        virtual operator INT () { return GetValue(); }
    };

    class CPropertyByte : public CPropertyValue< BYTE >
    {
    public:
        static BYTE Type() { return EByte; }
        virtual BYTE VType() { return Type(); }

    public:
        virtual VOID operator = ( BYTE tValue ) { SetValue( tValue ); }
        virtual VOID operator = ( DOUBLE tValue ) { SetValueCast( tValue ); }
        virtual VOID operator = ( DWORD tValue ) { SetValueCast( tValue ); }
        virtual VOID operator = ( FLOAT tValue ) { SetValueCast( tValue ); }
        virtual VOID operator = ( INT tValue ) { SetValueCast( tValue ); }
        virtual VOID operator = ( LONGLONG tValue ) { SetValueCast( tValue ); }
        virtual VOID operator = ( WORD tValue ) { SetValueCast( tValue ); }

    public:
        virtual operator BYTE () { return GetValue(); }
        virtual operator DOUBLE () { return static_cast< DOUBLE >( GetValue() ); }
        virtual operator DWORD () { return static_cast< DWORD >( GetValue() ); }
        virtual operator FLOAT () { return static_cast< FLOAT >( GetValue() ); }
        virtual operator INT () { return static_cast< INT >( GetValue() ); }
        virtual operator LONGLONG () { return static_cast< LONGLONG >( GetValue() ); }
        virtual operator WORD () { return static_cast< WORD >( GetValue() ); }
    };

    class CPropertyColor : public CPropertyValue< DWORD >
    {
    public:
        static BYTE Type() { return EColor; }
        virtual BYTE VType() { return Type(); }

    public:
        virtual VOID operator = ( DWORD tValue ) { SetValue( tValue ); }

    public:
        virtual operator DWORD () { return GetValue(); }
    };

    class CPropertyDouble : public CPropertyValue< DOUBLE >
    {
    public:
        static BYTE Type() { return EDouble; }
        virtual BYTE VType() { return Type(); }

    public:
        virtual VOID operator = ( BYTE tValue ) { SetValueCast( tValue ); }
        virtual VOID operator = ( DOUBLE tValue ) { SetValue( tValue ); }
        virtual VOID operator = ( DWORD tValue ) { SetValueCast( tValue ); }
        virtual VOID operator = ( FLOAT tValue ) { SetValueCast( tValue ); }
        virtual VOID operator = ( INT tValue ) { SetValueCast( tValue ); }
        virtual VOID operator = ( LONGLONG tValue ) { SetValueCast( tValue ); }
        virtual VOID operator = ( WORD tValue ) { SetValueCast( tValue ); }

    public:
        virtual operator BYTE () { return static_cast< BYTE >( GetValue() ); }
        virtual operator DOUBLE () { return GetValue(); }
        virtual operator DWORD () { return static_cast< DWORD >( GetValue() ); }
        virtual operator FLOAT () { return static_cast< FLOAT >( GetValue() ); }
        virtual operator INT () { return static_cast< INT >( GetValue() ); }
        virtual operator LONGLONG () { return static_cast< LONGLONG >( GetValue() ); }
        virtual operator WORD () { return static_cast< WORD >( GetValue() ); }
    };

    class CPropertyDWord : public CPropertyValue< DWORD >
    {
    public:
        static BYTE Type() { return EDWord; }
        virtual BYTE VType() { return Type(); }

    public:
        virtual VOID operator = ( BYTE tValue ) { SetValueCast( tValue ); }
        virtual VOID operator = ( DOUBLE tValue ) { SetValueCast( tValue ); }
        virtual VOID operator = ( DWORD tValue ) { SetValue( tValue ); }
        virtual VOID operator = ( FLOAT tValue ) { SetValueCast( tValue ); }
        virtual VOID operator = ( INT tValue ) { SetValueCast( tValue ); }
        virtual VOID operator = ( LONGLONG tValue ) { SetValueCast( tValue ); }
        virtual VOID operator = ( WORD tValue ) { SetValueCast( tValue ); }

    public:
        virtual operator BYTE () { return static_cast< BYTE >( GetValue() ); }
        virtual operator DOUBLE () { return static_cast< DOUBLE >( GetValue() ); }
        virtual operator DWORD () { return GetValue(); }
        virtual operator FLOAT () { return static_cast< FLOAT >( GetValue() ); }
        virtual operator INT () { return static_cast< INT >( GetValue() ); }
        virtual operator LONGLONG () { return static_cast< LONGLONG >( GetValue() ); }
        virtual operator WORD () { return static_cast< WORD >( GetValue() ); }
    };

    class CPropertyFlags : public CPropertyValue< DWORD >
    {
    public:
        typedef std::pair< DWORD_PTR, TSTRING > DataPair;
        typedef std::vector< DataPair > DataPairVec;

    private:
        DataPairVec m_vecData;

    public:
        static BYTE Type() { return EFlags; }
        virtual BYTE VType() { return Type(); }

    public:
        VOID PushData( DataPair& pairData ) { m_vecData.push_back( pairData ); }
        DataPairVec& GetData() { return m_vecData; }

    public:
        virtual VOID operator = ( DWORD tValue ) { SetValue( tValue ); }

    public:
        virtual operator DWORD () { return GetValue(); }
    };

    class CPropertyFloat : public CPropertyValue< FLOAT >
    {
    public:
        static BYTE Type() { return EFloat; }
        virtual BYTE VType() { return Type(); }

    public:
        virtual VOID operator = ( BYTE tValue ) { SetValueCast( tValue ); }
        virtual VOID operator = ( DOUBLE tValue ) { SetValueCast( tValue ); }
        virtual VOID operator = ( DWORD tValue ) { SetValueCast( tValue ); }
        virtual VOID operator = ( FLOAT tValue ) { SetValue( tValue ); }
        virtual VOID operator = ( INT tValue ) { SetValueCast( tValue ); }
        virtual VOID operator = ( LONGLONG tValue ) { SetValueCast( tValue ); }
        virtual VOID operator = ( WORD tValue ) { SetValueCast( tValue ); }

    public:
        virtual operator BYTE () { return static_cast< BYTE >( GetValue() ); }
        virtual operator DOUBLE () { return static_cast< DOUBLE >( GetValue() ); }
        virtual operator DWORD () { return static_cast< DWORD >( GetValue() ); }
        virtual operator FLOAT () { return GetValue(); }
        virtual operator INT () { return static_cast< INT >( GetValue() ); }
        virtual operator LONGLONG () { return static_cast< LONGLONG >( GetValue() ); }
        virtual operator WORD () { return static_cast< WORD >( GetValue() ); }
    };

    class CPropertyInt : public CPropertyValue< INT >
    {
    public:
        static BYTE Type() { return EInt; }
        virtual BYTE VType() { return Type(); }

    public:
        virtual VOID operator = ( BYTE tValue ) { SetValueCast( tValue ); }
        virtual VOID operator = ( DOUBLE tValue ) { SetValueCast( tValue ); }
        virtual VOID operator = ( DWORD tValue ) { SetValueCast( tValue ); }
        virtual VOID operator = ( FLOAT tValue ) { SetValueCast( tValue ); }
        virtual VOID operator = ( INT tValue ) { SetValue( tValue ); }
        virtual VOID operator = ( LONGLONG tValue ) { SetValueCast( tValue ); }
        virtual VOID operator = ( WORD tValue ) { SetValueCast( tValue ); }

    public:
        virtual operator BYTE () { return static_cast< BYTE >( GetValue() ); }
        virtual operator DOUBLE () { return static_cast< DOUBLE >( GetValue() ); }
        virtual operator DWORD () { return static_cast< DWORD >( GetValue() ); }
        virtual operator FLOAT () { return static_cast< FLOAT >( GetValue() ); }
        virtual operator INT () { return GetValue(); }
        virtual operator LONGLONG () { return static_cast< LONGLONG >( GetValue() ); }
        virtual operator WORD () { return static_cast< WORD >( GetValue() ); }
    };

    class CPropertyLongLong : public CPropertyValue< LONGLONG >
    {
    public:
        static BYTE Type() { return ELongLong; }
        virtual BYTE VType() { return Type(); }

    public:
        virtual VOID operator = ( BYTE tValue ) { SetValueCast( tValue ); }
        virtual VOID operator = ( DOUBLE tValue ) { SetValueCast( tValue ); }
        virtual VOID operator = ( DWORD tValue ) { SetValueCast( tValue ); }
        virtual VOID operator = ( FLOAT tValue ) { SetValueCast( tValue ); }
        virtual VOID operator = ( INT tValue ) { SetValueCast( tValue ); }
        virtual VOID operator = ( LONGLONG tValue ) { SetValue( tValue ); }
        virtual VOID operator = ( WORD tValue ) { SetValueCast( tValue ); }

    public:
        virtual operator BYTE () { return static_cast< BYTE >( GetValue() ); }
        virtual operator DOUBLE () { return static_cast< DOUBLE >( GetValue() ); }
        virtual operator DWORD () { return static_cast< DWORD >( GetValue() ); }
        virtual operator FLOAT () { return static_cast< FLOAT >( GetValue() ); }
        virtual operator INT () { return static_cast< INT >( GetValue() ); }
        virtual operator LONGLONG () { return GetValue(); }
        virtual operator WORD () { return static_cast< WORD >( GetValue() ); }
    };

    class CPropertyObject : public CPropertyValue< CBaseObject* >
    {
    public:
        static BYTE Type() { return EObject; }
        virtual BYTE VType() { return Type(); }

    public:
        virtual VOID operator = ( CBaseObject* tValue ) { SetValue( tValue ); }

    public:
        virtual operator CBaseObject* () { return GetValue(); }
    };

    class CPropertySelect : public CPropertyValue< DWORD >
    {
    public:
        typedef std::pair< DWORD_PTR, TSTRING > DataPair;
        typedef std::vector< DataPair > DataPairVec;

    private:
        DataPairVec m_vecData;

    public:
        static BYTE Type() { return ESelect; }
        virtual BYTE VType() { return Type(); }

    public:
        VOID PushData( DataPair& pairData ) { m_vecData.push_back( pairData ); }
        DataPairVec& GetData() { return m_vecData; }

    public:
        virtual VOID operator = ( DWORD tValue ) { SetValue( tValue ); }

    public:
        virtual operator DWORD () { return GetValue(); }
    };

    class CPropertyString : public CPropertyValue< TSTRING >
    {
    public:
        static BYTE Type() { return EString; }
        virtual BYTE VType() { return Type(); }

    public:
        virtual VOID operator = ( TSTRING tValue ) { SetValue( tValue ); }

    public:
        virtual operator TSTRING () { return GetValue(); }
    };

    class CPropertyVector2 : public CPropertyValue< D3DXVECTOR2 >
    {
    public:
        static BYTE Type() { return EVector2; }
        virtual BYTE VType() { return Type(); }

    public:
        virtual VOID operator = ( D3DXVECTOR2 tValue ) { SetValue( tValue ); }

    public:
        virtual operator D3DXVECTOR2 () { return GetValue(); }
    };

    class CPropertyVector3 : public CPropertyValue< D3DXVECTOR3 >
    {
    public:
        static BYTE Type() { return EVector3; }
        virtual BYTE VType() { return Type(); }

    public:
        virtual VOID operator = ( D3DXVECTOR3 tValue ) { SetValue( tValue ); }

    public:
        virtual operator D3DXVECTOR3 () { return GetValue(); }
    };

    class CPropertyVector4 : public CPropertyValue< D3DXVECTOR4 >
    {
    public:
        static BYTE Type() { return EVector4; }
        virtual BYTE VType() { return Type(); }

    public:
        virtual VOID operator = ( D3DXVECTOR4 tValue ) { SetValue( tValue ); }

    public:
        virtual operator D3DXVECTOR4 () { return GetValue(); }
    };

    class CPropertyWord : public CPropertyValue< WORD >
    {
    public:
        static BYTE Type() { return EWord; }
        virtual BYTE VType() { return Type(); }

    public:
        virtual VOID operator = ( BYTE tValue ) { SetValueCast( tValue ); }
        virtual VOID operator = ( DOUBLE tValue ) { SetValueCast( tValue ); }
        virtual VOID operator = ( DWORD tValue ) { SetValueCast( tValue ); }
        virtual VOID operator = ( FLOAT tValue ) { SetValueCast( tValue ); }
        virtual VOID operator = ( INT tValue ) { SetValueCast( tValue ); }
        virtual VOID operator = ( LONGLONG tValue ) { SetValueCast( tValue ); }
        virtual VOID operator = ( WORD tValue ) { SetValue( tValue ); }

    public:
        virtual operator BYTE () { return static_cast< BYTE >( GetValue() ); }
        virtual operator DOUBLE () { return static_cast< DOUBLE >( GetValue() ); }
        virtual operator DWORD () { return static_cast< DWORD >( GetValue() ); }
        virtual operator FLOAT () { return static_cast< FLOAT >( GetValue() ); }
        virtual operator INT () { return static_cast< INT >( GetValue() ); }
        virtual operator LONGLONG () { return static_cast< LONGLONG >( GetValue() ); }
        virtual operator WORD () { return GetValue(); }
    };

    class IPropertyFn : public CProperty
    {
    };

    class CPropertyFnArgs : public IPropertyFn
    {
    public:
        typedef std::pair< BYTE, TSTRING > ArgType;
        typedef std::vector< ArgType > ArgTypeVec;

    private:
        FunctionArgs m_pFunction;
        ArgTypeVec m_vecArgType;

    public:
        CPropertyFnArgs()
            : m_pFunction( NULL )
        {
        }

    public:
        static BYTE Type() { return EFunctionArgs; }
        virtual BYTE VType() { return Type(); }

    public:
        VOID SetFunction( FunctionArgs pFunction ) { m_pFunction = pFunction; }
        VOID PushArgType( ArgType& pairArg ) { m_vecArgType.push_back( pairArg ); }
        ArgTypeVec& GetArgTypes() { return m_vecArgType; }

    public:
        VOID Call( IPropertyValueVec& vecArgs ) { if( m_pFunction ) m_pFunction( vecArgs ); }
    };

    class CPropertyFnVoid : public IPropertyFn
    {
    private:
        FunctionVoid m_pFunction;

    public:
        CPropertyFnVoid()
            : m_pFunction( NULL )
        {
        }

    public:
        static BYTE Type() { return EFunctionVoid; }
        virtual BYTE VType() { return Type(); }

    public:
        VOID SetFunction( FunctionVoid pFunction ) { m_pFunction = pFunction; }

    public:
        VOID Call() { if( m_pFunction ) m_pFunction(); }
    };

    class CPropertyFactory
    {
    public:
        template< class T >
        static T* New()
        {
            T* pProperty = new T;
            __gassert_( dynamic_cast< CProperty* >( pProperty ) );
            return pProperty;
        }
    };

    typedef std::map< TSTRING, CProperty* > PropertyMap;
    typedef PropertyMap::value_type PropertyMapValue;
    typedef PropertyMap::iterator PropertyMapIter;
    typedef PropertyMap::const_iterator PropertyMapCIter;
}