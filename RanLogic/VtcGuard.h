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

    //! VTC Guard DLL 핸들
    HINSTANCE m_hGuardLibrary;

    //! VTC Guard 초기화
    func1 fnVTCDLLInit;

    //! VTC Guard 상태
    func2 fnVTCDLLGetStatus;

    //! Log In 했을때 호출
    func3 fnVTCDLLSetUser;

    //! Log Out 했을때 호출
    func4 fnVTCDLLExit;

public:
    bool Init();
    void HackDetect();
    void GameLogIn(char* UserName);
    void GameLogOut();
};

#endif // _VTC_GUARD_H_