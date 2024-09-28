#ifndef S_CCLIENTLOGIN_H_
#define S_CCLIENTLOGIN_H_

#pragma once

#include "./NetClientManager.h"

//! * History
//! 2002.05.30 jgkim Create
//!
//! Copyright (c) MIN COMMUNICATIONS, INC. All rights reserved.                 
//! 
//! * Note
//!
class NetLoginClientMan : public NetClientManager
{
public :
	NetLoginClientMan(int nMaxClient, HANDLE hIOServer, int nReferenceCountMax);
	virtual ~NetLoginClientMan();

public:
	/*
	mjeon.NetClientMan(parent)의 것을 그대로 써도 무방할듯

	int	GetCurrentClientNumber(void); // 현재 접속중인 클라이언트 숫자
	*/
};

#endif // S_CCLIENTLOGIN_H_