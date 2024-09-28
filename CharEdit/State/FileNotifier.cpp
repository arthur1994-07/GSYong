#include "stdafx.h"
#include "FileNotifier.h"
#include "CharStateMachine.h"
#include <sys/stat.h>

#include "../EngineLib/DxMeshs/SkinMesh/DxCharPart.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinAnimation.h"

CFileNotifier::CFileNotifier() :
	m_time(0)
{
	
}

bool CFileNotifier::Init()
{
	return false;
}

bool CFileNotifier::Classification(CParam& kParam)
{
	TCHAR* pFile = 0;
	kParam >> pFile >> Reset();
	return IsSameFile(pFile, GetFileName());
}

int CFileNotifier::Notify()
{
	struct _stat buf;

	if (_stat(m_strFile.c_str(), &buf) == 0 && difftime(buf.st_mtime, m_time) != 0)
	{
		if (IsFileOpen() == false)
		{
			return RETURNCODE_FAIL;
		}

		m_time = buf.st_mtime;
		return OnNotify();
	}

	return RETURNCODE_FAIL;
}

bool CFileNotifier::IsFileOpen()
{
	HANDLE hMapping = CreateFile(m_strFile.c_str(), GENERIC_READ, FILE_SHARE_READ, 
		0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (hMapping == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	CloseHandle(hMapping);
	return true;
}

bool CFileNotifier::IsSameFile(const TCHAR* pFile1, const TCHAR* pFile2)
{
	if (pFile1 != 0 && pFile2 != 0)
	{
		if (_tcsicmp(pFile1, pFile2) == 0)
		{
			return true;
		}
	}

	return false;
}

bool CFileNotifier::SetFile(TSTRING& strFile)
{
	struct _stat buf;

	if (_stat(strFile.c_str(), &buf) == 0)
	{
		m_strFile = strFile;
		m_time = buf.st_mtime;
		return true;
	}

	return false;
}

const TCHAR* CFileNotifier::GetFileName()
{
    const TCHAR* p = _tcsrchr(m_strFile.c_str(), _T('\\'));

    if (p != 0)
    {
        return p + 1;
    }

    return m_strFile.c_str();
}
