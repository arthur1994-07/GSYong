#ifndef _VTC_GUARD_H_
#define _VTC_GUARD_H_

#pragma once

#include <boost/thread.hpp>
#include "../EngineLib/ServiceProviderDefine.h"

// define for VTCGuard
typedef bool (_stdcall *func1) ();
typedef bool (_stdcall *func2) ();
typedef bool (_stdcall *func3) (DWORD);
//typedef bool (_stdcall *func3) (char*);
typedef bool (_stdcall *func4) ();

class VtcGuard
{
public:
    VtcGuard(EMSERVICE_PROVIDER sp);
    ~VtcGuard();

protected:
    EMSERVICE_PROVIDER m_sp;
    DWORD m_vtcGuardCheckTime;
    boost::thread m_CheckThread;

    //! VTC Guard DLL �ڵ�
    HINSTANCE m_hGuardLibrary;

    //! VTC Guard �ʱ�ȭ
    func1 fnVTCDLLInit;

    //! VTC Guard ����
    func2 fnVTCDLLGetStatus;

    //! Log In ������ ȣ��
    func3 fnVTCDLLSetUser;

    //! Log Out ������ ȣ��
    func4 fnVTCDLLExit;

public:
    bool Init();
    void HackDetect();
    void GameLogIn(char* UserName);
    void GameLogOut();
};

#endif // _VTC_GUARD_H_