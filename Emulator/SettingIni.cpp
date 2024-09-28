#include "stdafx.h"
#include "SettingIni.h"
#include "../SigmaCore/Util/vstscanf.h"

CSettingIni::CSettingIni(const TCHAR* pFile, const TCHAR* pKey)
{
    SetFile(pFile);
    SetKey(pKey);
}

CSettingIni::~CSettingIni()
{

}

void _tPathSetFileName(const TCHAR* pSrc, const TCHAR* pFileName, TCHAR* pDest, int nDestSize)
{
    TCHAR strDir[128];
    _tsplitpath_s(pSrc, pDest, nDestSize, strDir, 128, NULL, 0, NULL, 0);
    _tcscat_s(pDest, nDestSize, strDir);
    _tcscat_s(pDest, nDestSize, pFileName);
}

void _tExcutePathSetFileName(const TCHAR* pFileName, TCHAR* pDest, int nDestSize)
{
    TCHAR strExePath[256];
    GetModuleFileName(NULL, strExePath, 256);
    _tPathSetFileName(strExePath, pFileName, pDest, nDestSize);
}

bool CSettingIni::Save(const TCHAR* pCatagory, const TCHAR* pFormat, ...)
{
    if (m_strKey.empty())
    {
        return false;
    }

    TCHAR temp[256];
    va_list valist;
    va_start(valist, pFormat);
    _vstprintf_s(temp, 256, pFormat, valist);
    va_end(valist);

    TCHAR strPath[_MAX_PATH];
    PathGetIni(strPath);

    WritePrivateProfileString(m_strKey.c_str(), pCatagory, temp, strPath);

    return true;
}

int CSettingIni::Load(const TCHAR* pCatagory, const TCHAR* pFormat, ...)
{
    int nCount = 0;

    if (m_strKey.empty())
    {
        return nCount;
    }

    TCHAR strPath[_MAX_PATH];
    PathGetIni(strPath);

    static TCHAR temp[256] = {0};

    if (GetPrivateProfileString(m_strKey.c_str(), pCatagory, _T(""), temp, 256, strPath) <= 0)
    {	
        return nCount;
    }

    va_list valist;
    va_start(valist, pFormat);
    nCount = _vstscanf_s(temp, pFormat, valist);
    va_end(valist);

    return nCount;
}

void CSettingIni::PathGetIni(TCHAR* pPath)
{
    _tExcutePathSetFileName(m_strFile.c_str(), pPath, _MAX_PATH);
}

void CSettingIni::SavePosition(CWnd* pWnd)
{
    WINDOWPLACEMENT sWndPmt;
    memset(&sWndPmt, 0, sizeof(sWndPmt));
    sWndPmt.length = sizeof(WINDOWPLACEMENT);

    if (pWnd && pWnd->GetWindowPlacement(&sWndPmt) == TRUE)
    {
        Save(_T("Screen"), _T("%u %u %u %u %u %u %u %u %u %u"), 
            sWndPmt.flags, sWndPmt.showCmd,
            sWndPmt.rcNormalPosition.top, sWndPmt.rcNormalPosition.left,
            sWndPmt.rcNormalPosition.bottom, sWndPmt.rcNormalPosition.right,
            sWndPmt.ptMinPosition.x, sWndPmt.ptMinPosition.y,
            sWndPmt.ptMaxPosition.x, sWndPmt.ptMaxPosition.y);
    }
}

void CSettingIni::LoadPosition(CWnd* pWnd)
{
    WINDOWPLACEMENT sWndPmt;
    memset(&sWndPmt, 0, sizeof(sWndPmt));
    sWndPmt.length = sizeof(WINDOWPLACEMENT);

    if (Load(_T("Screen"), _T("%u %u %u %u %u %u %u %u %u %u"), 
        &sWndPmt.flags, &sWndPmt.showCmd,
        &sWndPmt.rcNormalPosition.top, &sWndPmt.rcNormalPosition.left,
        &sWndPmt.rcNormalPosition.bottom, &sWndPmt.rcNormalPosition.right,
        &sWndPmt.ptMinPosition.x, &sWndPmt.ptMinPosition.y,
        &sWndPmt.ptMaxPosition.x, &sWndPmt.ptMaxPosition.y) == 10)
    {
        if (sWndPmt.showCmd != SW_SHOWMAXIMIZED)
        {
            sWndPmt.showCmd |= SWP_FRAMECHANGED;
        }

        pWnd->SetWindowPlacement(&sWndPmt);
    }
}