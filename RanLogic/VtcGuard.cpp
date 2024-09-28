#include "pch.h"
#include "../SigmaCore/Util/SystemInfo.h"
#include "./VtcGuard.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

VtcGuard::VtcGuard(EMSERVICE_PROVIDER sp)
    : m_sp(sp)
{    
    m_hGuardLibrary = NULL;
    fnVTCDLLInit = NULL;
    fnVTCDLLGetStatus = NULL;
    fnVTCDLLSetUser = NULL;
    fnVTCDLLExit = NULL;
    m_vtcGuardCheckTime = GetTickCount();
}

VtcGuard::~VtcGuard()
{
    GameLogOut();
}

bool VtcGuard::Init()
{
    if (m_sp != SP_INDONESIA)
        return true;

    std::string LibPath(sc::getAppPath());
    LibPath.append("\\vtguard.pl");
    m_hGuardLibrary = LoadLibrary(LibPath.c_str());
    if (!m_hGuardLibrary)
    {
        AfxMessageBox("VTC Guard Init Failed", MB_OK|MB_ICONEXCLAMATION);
        return false;
    }

    fnVTCDLLInit      = (func1) GetProcAddress(m_hGuardLibrary, "func1");
    fnVTCDLLGetStatus = (func2) GetProcAddress(m_hGuardLibrary, "func2");
    fnVTCDLLSetUser   = (func3) GetProcAddress(m_hGuardLibrary, "func3");
    fnVTCDLLExit      = (func4) GetProcAddress(m_hGuardLibrary, "func4");
    if (!fnVTCDLLInit || !fnVTCDLLGetStatus || !fnVTCDLLSetUser || !fnVTCDLLExit)
    {
        fnVTCDLLInit = NULL;
        fnVTCDLLGetStatus = NULL;
        fnVTCDLLSetUser = NULL;
        fnVTCDLLExit = NULL;
        FreeLibrary(m_hGuardLibrary);
        m_hGuardLibrary = NULL;
        AfxMessageBox("VTC Guard Function Init Failed", MB_OK|MB_ICONEXCLAMATION);
        return false;
    }

    fnVTCDLLInit();
    m_CheckThread = boost::thread(boost::bind(&VtcGuard::HackDetect, this));
    return true;
}

void VtcGuard::GameLogIn(char* UserName)
{
    if (m_sp == SP_INDONESIA && m_hGuardLibrary && UserName)
        fnVTCDLLSetUser((DWORD) UserName);
}

void VtcGuard::GameLogOut()
{
    if (m_sp == SP_INDONESIA && m_hGuardLibrary)
    {
        fnVTCDLLExit();
        fnVTCDLLInit = NULL;
        fnVTCDLLGetStatus = NULL;
        fnVTCDLLSetUser = NULL;
        fnVTCDLLExit = NULL;            
        FreeLibrary(m_hGuardLibrary);
        m_hGuardLibrary = NULL;
    }
}

void VtcGuard::HackDetect()
{    
    while (TRUE) 
    {        
        if (!fnVTCDLLGetStatus())
        {
            //MessageBox(NULL, "vtcGuardThread fnVTCDLLGetStatus fnVTCDLLExit", "OK", MB_OK);
            fnVTCDLLExit();
            // exit game 
            exit(1);
        }
        Sleep(5000);
    }
}