//#pragma once
//
//#include "../../=Squirrel/include/sqrat.h"
//
//#include "../../SigmaCore/Log/LogMan.h"
//#include "../../SigmaCore/String/StringFormat.h"
//
//#include "../../RanLogic/TriggerManager/GLTriggerManager.h"
//#include "../../EngineLib/G-Logic/TypeDefine.h"
//
//#include "../FieldServer/GLGaeaServer.h"
//
//#include "./GLTriggerManagerFD.h"
//
//#include <boost/function.hpp>
//#include <boost/bind.hpp>
//#include <boost/lexical_cast.hpp>
//
//// --------------------------------------------------------------------------------------------------------------------------//
////! Squirrel 스크립트 함수 인자에 사용자 객체(ex. 클래스)를 넣기 위해서는 Sqrat::SqObjectType를 상속 받아야하는데
////  아무것도 안하므로 임시로 캐스팅을 제거 (어디까지나 임시임 나중에 꼭 프록시로 바꾸자)
////
//
//#define IMPLEMENT_SQUIRREL_USERTYPE_VAR( type )  \
//namespace Sqrat \
//{ \
//    template<> \
//    void detail::CheckVar(type& value,HSQUIRRELVM vm, SQInteger idx)  \
//    { \
//        HSQOBJECT obj; \
//        sq_getstackobj(vm, idx, &obj); \
// \
//        SQRAT_ASSERT_ARG_TYPE( !sq_isnull(obj) ,vm,idx ,"object is null"); \
//        SQRAT_ASSERT_ARG_TYPE( sq_isinstance(obj) ,vm, idx , "object is not instance"); \
// \
//        const type* pValue = ClassType<type>::GetInstance(vm, idx); \
//        SQRAT_ASSERT_ARG_TYPE( pValue ? true : false ,vm, idx , (_SC("arg must be ") + SQRAT_GET_CLASS_NAME(T)).c_str() ); \
//        if( !pValue ) \
//            return; \
// \
//        value = *pValue; \
//    } \
// \
//    template<> \
//    void detail::CheckVar(type*& value,HSQUIRRELVM vm, SQInteger idx) \
//    { \
//        HSQOBJECT obj; \
//        sq_getstackobj(vm, idx, &obj); \
// \
//        SQRAT_ASSERT_ARG_TYPE( !sq_isnull(obj) ,vm, idx , "object is null"); \
//        SQRAT_ASSERT_ARG_TYPE( sq_isinstance(obj) ,vm, idx , "object is not instance"); \
// \
//        value = ClassType<type>::GetInstance(vm, idx); \
// \
//        type* pValue = value; \
//        SQRAT_ASSERT_ARG_TYPE( pValue ? true : false ,vm, idx , (_SC("arg must be ") + SQRAT_GET_CLASS_NAME(T)).c_str() ); \
//        if( !pValue ) \
//            return; \
// \
//        value = pValue; \
//    } \
// \
//    template<> \
//    void Var<type*>::push(HSQUIRRELVM vm, type* value) { \
//        ClassType<type>::PushInstance(vm, value); \
//    } \
// \
//    template<> \
//    void Var<type>::push(HSQUIRRELVM vm, type value) { \
//        ClassType<type>::PushInstanceCopy(vm, value); \
//    } \
// \
//    template<> \
//    void Var<type&>::push(HSQUIRRELVM vm, type value) { \
//        ClassType<type>::PushInstanceCopy(vm, value); \
//    } \
// \
//    template<> \
//    void Var<const type*>::push(HSQUIRRELVM vm, type* value) { \
//        ClassType<type>::PushInstance(vm, value); \
//    } \
// \
//    template<> \
//    void Var<const type&>::push(HSQUIRRELVM vm, type value) { \
//        ClassType<type>::PushInstanceCopy(vm, value); \
//    } \
//};
//
//
//#define IMPLEMENT_SQUIRREL_USERTYPE_POINTER( type )  \
//    namespace Sqrat \
//{ \
//    template<> \
//    void detail::CheckVar(type*& value,HSQUIRRELVM vm, SQInteger idx) \
//    { \
//        HSQOBJECT obj; \
//        sq_getstackobj(vm, idx, &obj); \
// \
//        SQRAT_ASSERT_ARG_TYPE( !sq_isnull(obj) ,vm, idx , "object is null"); \
//        SQRAT_ASSERT_ARG_TYPE( sq_isinstance(obj) ,vm, idx , "object is not instance"); \
// \
//        value = ClassType<type>::GetInstance(vm, idx); \
// \
//        type* pValue = value; \
//        SQRAT_ASSERT_ARG_TYPE( pValue ? true : false ,vm, idx , (_SC("arg must be ") + SQRAT_GET_CLASS_NAME(T)).c_str() ); \
//        if( !pValue ) \
//        return; \
// \
//        value = pValue; \
//    } \
// \
//    template<> \
//    void Var<type*>::push(HSQUIRRELVM vm, type* value) { \
//        ClassType<type>::PushInstance(vm, value); \
//    } \
// \
//    template<> \
//    void Var<type>::push(HSQUIRRELVM vm, type value) { \
//        ClassType<type>::PushInstanceCopy(vm, value); \
//    } \
// \
//    template<> \
//    void Var<type&>::push(HSQUIRRELVM vm, type value) { \
//        ClassType<type>::PushInstanceCopy(vm, value); \
//    } \
// \
//    template<> \
//    void Var<const type*>::push(HSQUIRRELVM vm, type* value) { \
//        ClassType<type>::PushInstance(vm, value); \
//    } \
// \
//    template<> \
//    void Var<const type&>::push(HSQUIRRELVM vm, type value) { \
//        ClassType<type>::PushInstanceCopy(vm, value); \
//    } \
//};
//
//// --------------------------------------------------------------------------------------------------------------------------//
//IMPLEMENT_SQUIRREL_USERTYPE_POINTER( GLChar );
//IMPLEMENT_SQUIRREL_USERTYPE_VAR( ENUM_SCHOOL );
//IMPLEMENT_SQUIRREL_USERTYPE_VAR( EMSKILLFACT_TYPE );
//IMPLEMENT_SQUIRREL_USERTYPE_VAR( SNATIVEID );
//
//// --------------------------------------------------------------------------------------------------------------------------//
//namespace triggermanager
//{
//    class GLCharContainer : public Sqrat::SqObjectType
//    {
//    public:
//        GLCharContainer( GLGaeaServer* pGaeaServer )
//            : m_pCurCharNode(NULL)
//        {
//            if ( !pGaeaServer )
//            {
//                GASSERT( !"GLCharContainer::GLCharContainer() - pGaeaServer is Null" );
//            }
//
//            m_pGaeaServer = pGaeaServer;
//            m_pCharList   = &pGaeaServer->GetCharList();
//        }
//
//    public:
//        Sqrat::Object _nexti(Sqrat::string strIterator)
//        {
//            if ( !m_pCurCharNode )
//            {
//                m_pCurCharNode = m_pCharList->m_pHead;
//                return Sqrat::Object( m_pCurCharNode->Data );
//            }
//
//            m_pCurCharNode = m_pCurCharNode->pNext;
//
//            if ( !m_pCurCharNode )
//            {
//                return Sqrat::Object();
//            }
//
//            return Sqrat::Object( m_pCurCharNode->Data );
//        }
//
//        Sqrat::Object _get(Sqrat::string strIterator)
//        {
//            if ( m_pCurCharNode )
//            {
//                return Sqrat::Object( m_pCurCharNode->Data );
//            }
//
//            return Sqrat::Object();
//        }
//
//    private:
//        GLGaeaServer* m_pGaeaServer;
//
//    private:
//        GLCHARLIST* m_pCharList;
//        GLCHARNODE* m_pCurCharNode;
//
//    };
//
//    class Global : public Sqrat::SqObjectType
//    {
//    public:
//        Global()
//            : m_pGaeaServer(NULL)
//        {}
//
//    public:
//        static GLCharContainer GetCharList( Global* pGlobal )
//        {
//            return GLCharContainer( pGlobal->m_pGaeaServer );
//        }
//
//    public:
//        void SetGaeaServer( GLGaeaServer* pGaeaServer )
//        {
//            m_pGaeaServer = pGaeaServer;
//        }
//
//    private:
//        GLGaeaServer* m_pGaeaServer;
//    };
//
//
//    //! 루트 테이블
//    // --------------------------------------------------------------------------------------------------------------------------//
//    template<>
//    const bool GLTriggerManager::Bind_Environment( Sqrat::RootTable& Object )
//    {
//        using namespace Sqrat;
//        GLTriggerManagerFD* pManager = dynamic_cast<GLTriggerManagerFD*>(this);
//
//        if ( !pManager )
//        {
//            return false;
//        }
//
//        HSQUIRRELVM vm = Object.GetVM();
//
//        Sqrat::RootTable(vm).Bind( "GlobalClass", Sqrat::Class<Global>(vm)
//            .GlobalFunc( "GetCharList", &Global::GetCharList )
//            );
//
//        Global cGlobal;
//        cGlobal.SetGaeaServer( pManager->m_pGaeaServer );
//        Sqrat::RootTable(vm).SetValue( "Global", cGlobal );
//
//        Sqrat::RootTable(vm).Bind( "id", Sqrat::ImprovedClass<SNATIVEID>(vm)
//            .Ctor<WORD,WORD>()
//            .Func("GetMid", &SNATIVEID::Mid )
//            .Func("GetSid", &SNATIVEID::Sid )
//            .Func("SetMid", &SNATIVEID::SetMid )
//            .Func("SetSid", &SNATIVEID::SetSid )
//            );
//
//        Sqrat::RootTable(vm).Bind( "GLChar", Sqrat::Class<GLChar,NoConstructor>(vm)
//            .Func( "GetGaeaID", &GLChar::GaeaId )
//            .Func( "GetSchool", &GLChar::GetSchool )
//            .Overload<bool (GLChar::*)(const SNATIVEID&,const WORD)>( "BuffInsert", &GLChar::Receive_Buff_SendClient )
//            .Overload<bool (GLChar::*)(const SNATIVEID&,const WORD,float)>( "BuffInsert", &GLChar::Receive_Buff_SendClient )
//            .Overload<bool (GLChar::*)(const SNATIVEID&,const WORD,float,EMSKILLFACT_TYPE)>( "BuffInsert", &GLChar::Receive_Buff_SendClient )
//            .Overload<bool (GLChar::*)(const SNATIVEID&,const WORD,float,EMSKILLFACT_TYPE,const DWORD)>( "BuffInsert", &GLChar::Receive_Buff_SendClient )
//            .Overload<bool (GLChar::*)(const SNATIVEID&,const WORD)>( "BuffInsertBrd", &GLChar::Receive_Buff_SendMassiveClient )
//            .Overload<bool (GLChar::*)(const SNATIVEID&,const WORD,float)>( "BuffInsertBrd", &GLChar::Receive_Buff_SendMassiveClient )
//            .Overload<bool (GLChar::*)(const SNATIVEID&,const WORD,float,EMSKILLFACT_TYPE)>( "BuffInsertBrd", &GLChar::Receive_Buff_SendMassiveClient )
//            .Overload<bool (GLChar::*)(const SNATIVEID&,const WORD,float,EMSKILLFACT_TYPE,const DWORD)>( "BuffInsertBrd", &GLChar::Receive_Buff_SendMassiveClient )
//            .Overload<void (GLChar::*)(const SNATIVEID&)>( "BuffDelete", &GLChar::Delete_Buff_SendClient )
//            .Overload<void (GLChar::*)(const SNATIVEID&)>( "BuffDeleteBrd", &GLChar::Delete_Buff_SendMassiveClient )
//            );
//
//        Sqrat::RootTable(vm).Bind( "GLCharContainer", Sqrat::Class<GLCharContainer,CopyOnly<GLCharContainer> >(vm)
//            .Func( "_nexti", &GLCharContainer::_nexti )
//            .Func( "_get",   &GLCharContainer::_get )
//            );
//
//        Sqrat::ConstTable(vm).Enum("ENUM_SCHOOL", Enumeration(vm)
//            .Const( "SCHOOL_SM", SCHOOL_SM )
//            .Const( "SCHOOL_BH", SCHOOL_BH )
//            .Const( "SCHOOL_HA", SCHOOL_HA )
//            );
//
//        Sqrat::ConstTable(vm).Enum("BUFF_TYPE", Enumeration(vm)
//            .Const( "BUFF_TYPE_NORMAL",   EMSKILLFACT_TYPE_NORMAL )
//            .Const( "BUFF_TYPE_LUNCHBOX", EMSKILLFACT_TYPE_LUNCHBOX )
//            .Const( "BUFF_TYPE_SYSTEM",   2 )
//            );
//
//        return true;
//    }
//
//    //! 테이블
//    // --------------------------------------------------------------------------------------------------------------------------//
//    class SQTableBindTrigger : private Sqrat::ForEachFunction<std::string, Sqrat::Object>
//    {
//    public:
//        typedef std::map<std::string, Sqrat::Function> MFUNCTION;
//        typedef MFUNCTION::iterator                    MFUNCTION_ITER;
//
//        virtual void operator()(const std::string& key, const Sqrat::Object& value )
//        {
//            using namespace Sqrat;
//
//            switch ( value.GetType() )
//            {
//            case OT_CLOSURE:
//                {
//                    Sqrat::Function sqFunc = value.Cast<Sqrat::Function>();
//                    m_mFunction.insert( std::make_pair(key, sqFunc) );
//                }
//                break;
//
//            default:
//                return;
//            }
//        }
//
//    public:
//        SQTableBindTrigger( HSQUIRRELVM hvm ) 
//            : m_hvm(hvm)
//        {}
//
//    public:
//        const bool Parse( Sqrat::Table& Object )
//        {
//            Sqrat::ForEach( Object, (*this) );
//            return true;
//        }
//
//    public:
//        template<typename TRIGGERLIST>
//        void Bind( const std::string& string, TRIGGERLIST& TriggerList )
//        {
//            MFUNCTION_ITER finditer = m_mFunction.find( string );
//
//            if ( finditer == m_mFunction.end() )
//            {
//                return;
//            }
//
//            TriggerList.Register( typename TRIGGERLIST::FNCALLBACK( finditer->second ) );
//        }
//
//    private:
//        HSQUIRRELVM m_hvm;
//        MFUNCTION   m_mFunction;
//    };
//
//    template<>
//    const bool GLTriggerManager::Bind_Environment( Sqrat::Table& Object )
//    {
//        GLTriggerManagerFD* pManager = dynamic_cast<GLTriggerManagerFD*>(this);
//
//        if ( !pManager )
//        {
//            return false;
//        }
//
//        SQTableBindTrigger cTriggerBinder(Object.GetVM());
//
//        if ( !cTriggerBinder.Parse(Object) )
//        {
//            return false;
//        }
//
//        cTriggerBinder.Bind( "Trigger_Connect_To_Field", pManager->Get_Trigger_Connect_To_Field() );
//        cTriggerBinder.Bind( "Trigger_CTF_Begin",        pManager->Get_Trigger_CTF_Begin() );
//        cTriggerBinder.Bind( "Trigger_CTF_End",          pManager->Get_Trigger_CTF_End() );
//
//        return true;
//    }
//};
//
