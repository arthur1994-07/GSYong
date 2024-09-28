#ifndef CORE_COMMON_H_
#define CORE_COMMON_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//
//#include "./lock.h"
//#include "gassert.h"
//// 실수로 인한 클래스이름을 틀리게 적을 경우  실제로 존재하는지까지 컴파일 타임 체크까지
//#define CoreCommonGetClassName( INPUT_STRING,CLASS_NAME ) INPUT_STRING = #CLASS_NAME; BOOST_STATIC_ASSERT(sizeof(CLASS_NAME));
//#define CoreCommonGetClassNameEx( CLASS_NAME ) CLASS_NAME::CLASS_TYPE_STRING = #CLASS_NAME; BOOST_STATIC_ASSERT(sizeof(CLASS_NAME));
//
//// enum 크기는 DWORD와 같아야한다 (현재 DWORD로 쓰고 있음)
//BOOST_STATIC_ASSERT(sizeof(enum _TEST_ENUM) == sizeof(DWORD));
//BOOST_STATIC_ASSERT(sizeof(int) == sizeof(DWORD));
//

#ifdef _MSC_VER
	#define OVERRIDE override
#else
	#define OVERRIDE 
#endif


#include <boost/foreach.hpp>
#include <boost/scope_exit.hpp>

#endif // CORE_COMMON_H_
