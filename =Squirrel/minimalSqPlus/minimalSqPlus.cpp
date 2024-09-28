// minimalSqPlus.cpp
// A minimal example and an example with a simple class.
// Example for use with the remote debugger is also shown.
// Created 10/08/05, John Schultz
// Free for any use.

#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <string>

#define USE_REMOTE_DEBUGGER

#define SQRAT   1
#define SQPLUS  2

#define SQ_BINDER SQRAT
//#define SQ_BINDER SQPLUS

#if SQ_BINDER == SQRAT

bool myTestError(bool b, std::string msg);

	#include "sqrat.h"

#elif SQ_BINDER == SQPLUS
	#include "sqplus.h"
#endif

#include "sqstdio.h"
#include "sqstdblob.h"
#include "sqstdmath.h"
#include "sqstdstring.h"

#include "squirrelex.h"

#include <windows.h>
#include <tchar.h>

// Define USE_REMOTE_DEBUGGER to use the remote debugger. See the example and docs in the sqdbg directory for
// more information.
//#define USE_REMOTE_DEBUGGER // Remote debugger must be started after the program is launched.
#ifdef USE_REMOTE_DEBUGGER
#include "sqrdbg.h"
#endif

// The following pragmas can be used to link the correct library for Debug/Release builds
// without requiring the application to include the associated Squirrel projects.
// (The libraries must be created before compiling this example).
// Set SQ_REL_PATH as needed for your application.

#define SQ_REL_PATH "../"

#ifdef UNICODE
#define LIB  "U.lib"
#else
#define LIB  ".lib"
#endif

#ifdef _MSC_VER
#ifdef _DEBUG
#pragma comment(lib,SQ_REL_PATH "lib/squirrelD" LIB)
#pragma comment(lib,SQ_REL_PATH "lib/sqstdlibD" LIB)
#ifdef USE_REMOTE_DEBUGGER
#pragma comment(lib,SQ_REL_PATH "lib/sqdbglibD" LIB)
#endif
//#pragma comment(lib,SQ_REL_PATH "lib/sqplusD" LIB)
#else // Release
#pragma comment(lib,SQ_REL_PATH "lib/squirrel" LIB)
#pragma comment(lib,SQ_REL_PATH "lib/sqstdlib" LIB)
#ifdef USE_REMOTE_DEBUGGER
#pragma comment(lib,SQ_REL_PATH "lib/sqdbglib" LIB)
#endif
//#pragma comment(lib,SQ_REL_PATH "lib/sqplus" LIB)
#endif
#endif


#undef GetObject

#ifdef USE_REMOTE_DEBUGGER
void printSQDBGError(HSQUIRRELVM v) {
  const SQChar *err;
  sq_getlasterror(v);
  if(SQ_SUCCEEDED(sq_getstring(v,-1,&err))) {
    scprintf(_T("SQDBG error : %s"),err);
  }else {
    scprintf(_T("SQDBG error"),err);
  } // if
  sq_poptop(v);
} // printSQDBGError
#endif

bool myTestError(bool b,std::string msg)
{
	if(b)
		return b;

	sqstd_printcallstack(Sqrat::DefaultVM::Get());
	printf("msg : %s \n",msg.c_str());
	return b;
}

static SQInteger PrintError(HSQUIRRELVM v)
{
	SQPRINTFUNCTION pf = sq_geterrorfunc(v);
	if(pf) {
		const SQChar *sErr = 0;
		if(sq_gettop(v)>=1) {
			if(SQ_SUCCEEDED(sq_getstring(v,2,&sErr)))	{
				pf(v,_SC("\nAN ERROR HAS OCCURED [%s]\n"),sErr);
			}
			else{
				pf(v,_SC("\nAN ERROR HAS OCCURED [unknown]\n"));
			}
		}
	}

	sqstd_printcallstack(v);

	throw Sqrat::Exception(_SC(""));
	return 0;
}

static void printFunc(HSQUIRRELVM v,const SQChar * s,...) {
  static SQChar temp[2048];
  va_list vl;
  va_start(vl,s);
  scvsprintf( temp,s,vl);
//  SCPUTS(temp);
  _putts(temp);
  va_end(vl);
} // printFunc

struct TestForEach : public Sqrat::ForEachFunction<int,int>
{
	virtual void operator()(const int& key,const int& value )
	{
		int myKey = key;
		int myValue = value;
	}
};

struct TestVal : public Sqrat::SqObjectType
{
	virtual int GetID() { return 1; }
	
	Sqrat::Object test_get(Sqrat::string index) 
	{
		HSQUIRRELVM vm = Sqrat::DefaultVM::Get();
		sq_pushinteger(vm , x);
		HSQOBJECT xIntObj;
		sq_getstackobj(vm, -1, &xIntObj);
		Sqrat::Object obj(xIntObj);
		sq_pop(vm,1);
		return  obj;
	}

	Sqrat::Object test_next() 
	{
//		sq_pushnull(Sqrat::DefaultVM::Get());		
//		return _T("0"); 
		
		if( index == 0 )
		{
			++index;

			HSQUIRRELVM vm = Sqrat::DefaultVM::Get();
			sq_pushstring(vm ,_T("x"), (SQInteger)_tcslen(_T("x")) );
			HSQOBJECT xStringObj;
			sq_getstackobj(vm, -1, &xStringObj);
			
			Sqrat::Object obj(xStringObj);
			sq_pop(vm,1);
			return  obj;
		}

		index = 0;
		return Sqrat::Object();
	}
//	TestVal() {x=y=0;}
	TestVal(int x1 = -1 ,int y1 = -1) { x = x1; y = y1; index = 0;} 
	int x,y;
	int index;
};

class MyClass : public Sqrat::SqObjectType
{
public:
	virtual int GetID() { return 2; }

  int classVal;
  Sqrat::Function func;
  // See examples in testSqPlus2.cpp for passing arguments to the constructor (including variable arguments).
  MyClass() : classVal(123) {}
  bool process(int iVal,const SQChar * sVal) {
    scprintf(_T("classVal: %d, iVal: %d, sVal %s\n"),classVal,iVal,sVal);
    classVal += iVal;
    return iVal > 1;
  } // process

  void getVal(Sqrat::Object& myObject ) 
    {
		SQObjectType type =  myObject.GetType();

		const SQChar* myName = sq_objtostring(&myObject.GetObject());
//		SQArray* pArray = _array((HSQOBJECT)myObject);
//		int test =	pArray->Size();
		
		HSQUIRRELVM vm = myObject.GetVM();

		sq_pushobject(vm, myObject.GetObject() );
		SQInteger test = sq_getsize(vm,-1);

		HSQOBJECT obj;
		
		//sq_pushstring(vm, "my1", -1);
		sq_pushinteger(vm,0);
		if(SQ_FAILED(sq_get(vm, -2))) {
			sq_pushnull(vm);
		}
		sq_getstackobj(vm, -1, &obj);
		sq_pop(vm, 2);
	
		Sqrat::Object testObj(obj);
		testObj.GetType();
		Sqrat::string aaa = testObj.Cast<Sqrat::string>();

		
		
		SQInteger curTop = sq_gettop(vm);
		sq_pushobject(vm, myObject.GetObject() );
//		sq_arrayresize(vm,-1,0);
//		SQInteger ss = sq_getsize(vm,-1);
//		sq_arrayresize(vm,-1,15);

		sq_pushinteger(vm,10);
		sq_pushinteger(vm,21);

		if(SQ_FAILED(sq_set(vm, -3))) {
			sq_pushnull(vm);
		}
		
		SQInteger endTop = sq_gettop(vm);
		sq_pop(vm, endTop - curTop );


// 		Sqrat::ContainerArray containerArray(myObject);
// 		int testSize =	containerArray.GetSize();
// 
// 		ForEach(myObject , TestForEach());

		SqStdVector<Sqrat::Object> testVec;
 		GetVectorFromArray(myObject,&testVec);
//		SqStdMap<Sqrat::string,int> intMap;
//		GetMapFromTable(myObject,&intMap);
		TestVal* pTestVal =	testVec[0].Cast<TestVal*>();

//		testVec[0]->x = 5;
//		testVec.clear();
		GetVectorFromArray(myObject,&testVec);
		

		int aa = 0;
		//SQTable* pTable = _table((HSQOBJECT)myObject);	
		//		SQObjectPtr key = SQObjectPtr(SQString::Create(_ss(myObject.GetVM()),"my1", -1));
//		SQObjectPtr value;
//		if( pTable->Get(key,value) )
//		{
//			int aa = 0;
//			value.Null()
//			Sqrat::Object obj(value);
//			Sqrat::Object(value._unVal)
//		}

//		sq_getsize(myObject.GetVM(),2);
//		Sqrat::string ss = myObject.Cast<Sqrat::string>();
//		const SQChar* ss1 = myObject.Cast<const SQChar*>();
	  // 포인터를 직접 사용하면 뻑나므로
	 //  Sqrat::Object 로 받아서 처리 한다
	 //  대시 좀 자동화 및 직관적으로 이해 할수 있게
     //  Sqrat::Object 를 템플릿 개념으로 상속받아 처리 하자
     //  인자로 Sqrat::Object<TestVal> 처럼  밑에 주석을 참고
     //  나머진 정확한 타입을 분석해 처리
  }


//   void getVal(Sqrat::Object& Value ) 
//   {
// 	  SQObjectType type =  Value.GetType();
// 
//  	  TypeID* ss =	Value.Cast<TypeID*>();
// 	  int id = ss->GetID();
// 	  switch(id)
// 	  {
// 	  case 0:
// 		  {
// 			  int a= 0;
// 			  break;
// 		  }
// 	  case 1:
// 		  {
// 			  int a= 0;
// 			  break;
// 		  }
// 	  case 2:
// 		  {
// 			  int a= 0;
// 			  break;
// 		  }
// 	  }
// // 	  ss->x = 2;  ss->y = 2; 
// //	  OT_INTEGER;
// 
// //	  int pInt = Value.Cast<TestVal>();
// //	  pInt = 7;
//   }


  // 포인터나 래퍼런스로 받지 말것!!! 뻑나네 , 뭔지 모르지만 아마 내부적으로 참조카운팅어쩔때 뻑나네
  void setFunc(Sqrat::Function functest)
  {
	  SQStackInfos stack;
	  sq_stackinfos(Sqrat::DefaultVM::Get(),1,&stack);
	  printf("%s %d : %s \n",stack.source,stack.line,stack.funcname);

	  assert(!functest.IsNull());
	  HSQOBJECT& hSqObj = functest.GetFunc();
	  printf("obj name :%s \n",sq_objtostring(&hSqObj));
	  assert( sq_isfunction(hSqObj) );

      func = functest;
  }
};


void compilerHook(HSQUIRRELVM vm,const SQChar * desc,const SQChar * source,SQInteger line,SQInteger column)
{
	scprintf(_SC("%s line = (%d) column = (%d) : error %s\n"),source,line,column,desc);
}

// void MyDebugHook(HSQUIRRELVM vm, SQInteger type, const SQChar * sourcename, SQInteger line, const SQChar * funcname)
// {
// 	scprintf("error : %s  %d , %s \n",sourcename,line,funcname);
// }



int main(int argc,char * argv[]) {

#if SQ_BINDER == SQRAT
	HSQUIRRELVM vm = sq_open(1024);


//	sq_setnativedebughook(vm,MyDebugHook);
	sq_setprintfunc(vm, printFunc,printFunc);

 	sq_pushroottable(vm);
// 	sqstd_register_iolib(vm);
// 	sqstd_register_bloblib(vm);
// 	sqstd_register_mathlib(vm);
// 	sqstd_register_stringlib(vm);

	//sqstd_seterrorhandlers(vm);
	sq_setcompilererrorhandler(vm,compilerHook);
	sq_newclosure(vm,PrintError,0);
	sq_seterrorhandler(vm);

	sq_pop(vm,1);
	//	sq_seterrorhandler(vm);
	//	sq_newclosure(vm, PrintError,0);

	using namespace Sqrat;
	DefaultVM::Set(vm);

#elif SQ_BINDER == SQPLUS
	using namespace SqPlus;

	SquirrelVM::Init();
//	This example shows how to redirect print output to your own custom
//	print function (the default handler prints to stdout).
	sq_setprintfunc(SquirrelVM::GetVMPtr(),printFunc);

	HSQUIRRELVM vm = SquirrelVM::GetVMPtr();
#endif

#ifdef USE_REMOTE_DEBUGGER
	HSQREMOTEDBG rdbg = NULL;
	rdbg = sq_rdbg_init(vm,1234,SQTrue);
	if(rdbg) 
	{
		// Enable debug info generation (for the compiler).
		sq_enabledebuginfo(vm,SQTrue);
		scprintf(_T("Waiting for SQDBG connection...\n"));
		// Suspends the app until the debugger client connects.
		if(SQ_SUCCEEDED(sq_rdbg_waitforconnections(rdbg))) {
			printf("SQDBG: connected.\n");
		} // if
	} else {
		printSQDBGError(vm);
		return 0;
	} // if
#endif



  // See examples in testSqPlus2.cpp for script read-only vars, constants,
  // enums, static/global functions, variable arguments, constructor arguments, 
  // passing/returning classes/structs by value or by address, etc.

#if SQ_BINDER == SQRAT
	
		RootTable().Bind(_T("TestVal"), Class<TestVal>()
		.Var(_T("x"), &TestVal::x)
		.Var(_T("y"), &TestVal::y)
		.Func(_T("_nexti"),&TestVal::test_next)
		.Func(_T("_get"),&TestVal::test_get)
		);


	RootTable().Bind(_T("MyClass"), Class<MyClass>()
		.Func(_T("process"), &MyClass::process)
		.Var(_T("classVal"), &MyClass::classVal)
		.Var(_T("func"),&MyClass::func)
		.Func(_T("setFunc"), &MyClass::setFunc)
		.Func(_T("getVal"),&MyClass::getVal)
		);

#elif SQ_BINDER == SQPLUS
	SQClassDef<MyClass>(_T("MyClass")).
		func(&MyClass::process,_T("process")).
		var(&MyClass::classVal,_T("classVal"));
#endif

	//////////////////////////////////////////////////////////////////////////


  //////////////////////////////////////////////////////////////////////////
  
//   SquirrelObject helloSqPlus = SquirrelVM::CompileScript("test.nut");

  //////////////////////////////////////////////////////////////////////////


#if SQ_BINDER == SQRAT
	try {
		Script script1;

		printf("compile start \n");
		script1.CompileFile(_T("test.nut"));
//		script1.CompileFile(_T("main.nut"));

		printf("compile End \n");

		printf("Run Script - start\n");
		script1.Run();
//		script2.Run();
		printf("Run Script - end\n");



 		Function func = RootTable().GetFunction( _T("MyTestFunc") );
 		MyClass testMyClass;
 		func(testMyClass,5);
// 
// 		Function funcMakeGen = RootTable().GetFunction( _T("MakeGen") );010 6219 2515
// 
// 
// 		Sqrat::Object gen =	funcMakeGen.Evaluate<Sqrat::Object>();
// 
// 	
// 		int aa = sq_gettop(vm);
// 
// 		sq_pushobject(vm,gen.GetObject());
// 		sq_resume(vm,true,true);
// 		
// 		SQFloat reInt;
// 		sq_getfloat(vm,-1,&reInt);
// 
// 		sq_pop(vm,2);
// 
//  
// 		sq_pushobject(vm,gen.GetObject());
// 		sq_resume(vm,true,true);
// 		sq_pop(vm,2);
// 
// 		sq_pushobject(vm,gen.GetObject());
// 		sq_resume(vm,true,true);
// 		sq_pop(vm,2);
// 
// 		sq_pushobject(vm,gen.GetObject());
// 		sq_resume(vm,true,true);
// 		sq_pop(vm,2);
// 
// 		sq_pushobject(vm,gen.GetObject());
// 		sq_resume(vm,true,true);
// 		sq_pop(vm,2);
// 
// 		sq_pushobject(vm,gen.GetObject());
// 		sq_resume(vm,true,true);
// 		sq_pop(vm,2);
// 
// 		SQBool genedead = sqex_isgeneratordead(&gen.GetObject());
// 	
// 		if( !genedead )
// 		{
// 			sq_pushobject(vm,gen.GetObject());
// 			sq_resume(vm,true,true);
// 			sq_pop(vm,2);
// 
// 		}


// 		Function funcDoGtor = RootTable().GetFunction( _T("doGtor") );
// 		funcDoGtor(gen);
// 		funcDoGtor(gen);
// 		funcDoGtor(gen);
// 		funcDoGtor(gen);
// 		funcDoGtor(gen);
// 		funcDoGtor(gen);
// 		funcDoGtor(gen);

//		HSQOBJECT s1,s2;
//		sq_getstackobj(vm,-1,&s1);
//		sq_getstackobj(vm,-2,&s2);

// 		sq_pushobject(vm, func.GetFunc());
// 		sq_pushobject(vm, func.GetEnv());
// 
// 		sq_resume(vm,false,true);

//		int aa = func.Evaluate<int>(testMyClass,5);
#ifdef USE_REMOTE_DEBUGGER
		if (rdbg) {
			sq_rdbg_shutdown(rdbg);
		} // if
#endif

	} catch(const Exception& e) {
		// Handle exceptions here
//		PrintError(DefaultVM::Get());
		scprintf(_SC("error : %s \n"),e.Message().c_str());
//		PrintError(vm);

	}
#elif SQ_BINDER == SQPLUS

    SquirrelObject testOb = SquirrelVM::CompileScript("test.nut");
    SquirrelObject mainOb = SquirrelVM::CompileScript("main.nut");

    //////////////////////////////////////////////////////////////////////////

	try {
	  SquirrelVM::RunScript(testOb);	  
	  SquirrelVM::RunScript(mainOb);

	  SqPlus::SquirrelFunction<void>(_T("TestFunc"))(100,100);

	} catch (SquirrelError & e) {
		scprintf(_T("Error: %s, %s\n"),e.desc,_T(" Test "));
	} // catch
	
#endif
    //////////////////////////////////////////////////////////////////////////
  scprintf(_T("Press RETURN to exit."));
  getchar();




#if SQ_BINDER == SQRAT
	sq_close(vm);
#elif SQ_BINDER == SQPLUS
	SquirrelVM::Shutdown();
#endif

	return 0;

} // main



#ifdef SQPLUS_SMARTPOINTER_OPT
unsigned char* getSmartPointerPointee(unsigned char *up) {
	return up;
}
#endif

// minimalSqPlus.cpp
