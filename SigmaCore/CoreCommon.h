#ifndef CORE_COMMON_H_
#define CORE_COMMON_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//
//#include "./lock.h"
//#include "gassert.h"
//// �Ǽ��� ���� Ŭ�����̸��� Ʋ���� ���� ���  ������ �����ϴ������� ������ Ÿ�� üũ����
//#define CoreCommonGetClassName( INPUT_STRING,CLASS_NAME ) INPUT_STRING = #CLASS_NAME; BOOST_STATIC_ASSERT(sizeof(CLASS_NAME));
//#define CoreCommonGetClassNameEx( CLASS_NAME ) CLASS_NAME::CLASS_TYPE_STRING = #CLASS_NAME; BOOST_STATIC_ASSERT(sizeof(CLASS_NAME));
//
//// enum ũ��� DWORD�� ���ƾ��Ѵ� (���� DWORD�� ���� ����)
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
