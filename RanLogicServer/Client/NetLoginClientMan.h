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
	mjeon.NetClientMan(parent)�� ���� �״�� �ᵵ �����ҵ�

	int	GetCurrentClientNumber(void); // ���� �������� Ŭ���̾�Ʈ ����
	*/
};

#endif // S_CCLIENTLOGIN_H_