#include "pch.h"
#include <process.h>

#include "./NSSoundDef.h"

#include "BgmSound.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

#define LIB_ZPLAY_STATIC
#include "../../ThirdParty/LibZplay/include/libzplay.h"

// use libZPlay namespace
using namespace libZPlay;

int __stdcall BgmCallbackFunc(void* instance, void *user_data, TCallbackMessage message, unsigned int param1, unsigned int param2)
{
    DxBgmSound* pBgmSound = (DxBgmSound*) user_data;
    if (DxBgmSound::BGM_STOP == pBgmSound->GetStatus())
        return 0;

    libZPlay::ZPlay* PlayerInstance = (libZPlay::ZPlay*) instance;

    switch (message)
    {
    case MsgStop:
        PlayerInstance->Play();
        break;

    case MsgStopAsync: 
        PlayerInstance->Play();
        break;

    default:
        break;
    }
    return 0;
}

DxBgmSound& DxBgmSound::GetInstance()
{
	static DxBgmSound Instance;
	return Instance;
}

DxBgmSound::DxBgmSound()
	: m_bSoundEnable(FALSE)
{
    m_Volume = 0;
    m_SoundLength = 0;

    m_pPlayer = CreateZPlay();
    if (m_pPlayer)
    {
        int Version = m_pPlayer->GetVersion();
        unsigned int LeftVolume = 0;
        unsigned int RightVolume = 0;
        m_pPlayer->GetPlayerVolume(&LeftVolume, &RightVolume);

        m_Volume = (LeftVolume+RightVolume)/2;        

        m_pPlayer->SetCallbackFunc(
            BgmCallbackFunc,
            (TCallbackMessage) (MsgStop|MsgStopAsync),
            this);
    }
}

DxBgmSound::~DxBgmSound()
{
    if (m_pPlayer)
    {
        Stop();
        m_pPlayer->Release();
    }
}

void DxBgmSound::SetMute(BOOL bMute)
{   
    if (bMute)
    {
        Stop();
        m_Status = BGM_MUTE;
    }
    else
    {
        m_Status = BGM_STOP;
        Play();
    }
}

BOOL DxBgmSound::IsMute()
{
    if (m_Status == BGM_PLAY)
        return FALSE;
    else
        return TRUE;
}

BOOL DxBgmSound::Run( HWND hWnd, BOOL bSoundEnable )
{	
	m_bSoundEnable = bSoundEnable;

	// ���� ī�尡 ���ٸ� m_bSoundEnable �� FALSE �̴�.
	// BGM �� ���� �ȵǵ��� ����.
	if ( !m_bSoundEnable )
	{
		if (m_pPlayer)
		{
			m_pPlayer->Release();
			m_pPlayer = NULL;
		}
	}
	return TRUE;
}

// BOOL DxBgmSound::SetHWND(HWND hWnd)
// {
//     return TRUE;
// }

BOOL DxBgmSound::SetPath(const CString& strPath)
{
	m_Path = strPath.GetString();
	return TRUE;
}

const char* DxBgmSound::GetPath() const
{
	return m_Path.c_str();
}

BOOL DxBgmSound::SetFile(const CString& strFileName)
{
	if( strFileName.IsEmpty() )
		return FALSE;

	if( m_BgmFileName == strFileName.GetString() )
		return TRUE;

	m_BgmFileName.clear();
    m_BgmFileName = strFileName.GetString();

    return OpenFile();
}

// BOOL DxBgmSound::SetFile(const TCHAR* pszFileName)
// {
//     if (!pszFileName)
//         return FALSE;
//     m_BgmFileName.clear();
// 	m_BgmFileName = pszFileName;
// 
//     return OpenFile();
// }

BOOL DxBgmSound::OpenFile()
{
    if (!m_pPlayer)
        return false;

    if (m_BgmFileName.empty())
        return false;

    Stop();

    std::string FilePath = m_Path + m_BgmFileName;

    if (m_pPlayer->OpenFile(FilePath.c_str(), libZPlay::sfAutodetect) == 0)
    {
        // OpenFile error
        return FALSE;
    }    

    libZPlay::TStreamInfo pInfo;
    m_pPlayer->GetStreamInfo(&pInfo);
    const libZPlay::TStreamTime& Length = pInfo.Length;
    m_SoundLength = Length.ms;
    return TRUE;
}

void DxBgmSound::ClearFile()
{
	m_BgmFileName.clear();
}

const char*	DxBgmSound::GetFile()
{
	return m_BgmFileName.c_str();
}


BOOL DxBgmSound::Play()
{
    if ( m_Status == BGM_MUTE || m_BgmFileName.empty() )
        return FALSE;

    if (m_pPlayer && m_Status != BGM_PLAY)
    {
        m_pPlayer->Play();
        m_Status = BGM_PLAY;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL DxBgmSound::Stop()
{
    if ( m_Status == BGM_MUTE )
        return FALSE;

    if (m_pPlayer)
    {
        m_Status = BGM_STOP;
        m_pPlayer->Stop();
    }

	// �ٷ� �����Ѵ�.
	CtrlVolume();
	return TRUE;
}

// BOOL DxBgmSound::Playing()
// {    
// }

// BOOL DxBgmSound::IsPlaying()
// {
//     if (m_Status == BGM_PLAY)
//         return TRUE;
//     else
//         return FALSE;
// }

BOOL DxBgmSound::ForceStop()
{	
    Stop();
    return TRUE;
}

void DxBgmSound::SetActivate(BOOL bActivity)
{
    if (!m_pPlayer)
        return;

    if (bActivity && m_Status == BGM_STOP)
    {
        Play();
    }
    else if ( m_Status == BGM_PLAY )
    {
        Stop();
    }
}

LONG DxBgmSound::GetVolume()
{
    // DxSound �Ǽ��� ������ ���� ������, 
    // DSBVOLUME_MAX (���� ����)�κ��� DSBVOLUME_MIN (����)�����̴�. 
    // �̷��� ����, Dsound.h �������� ���� 0 �� 10, 000 ���� ���ǵǰ� �ִ�. 
    // �� DSBVOLUME_MAX ��, ��Ʈ�� ������ �������� ������ ��Ÿ����. 
    // �� DSBVOLUME_MIN ��, ������� ������ 100 dB �����ϰ� �ִ� ���� ��Ÿ��, �̰��� ���������δ� ������ �ǹ��Ѵ�.
    // DirectSound �� ������ ���� �ϰ� ���� �ʴ�. 
    if (m_pPlayer)
    {
        unsigned int LeftVolume = 0;
        unsigned int RightVolume = 0;
        m_pPlayer->GetPlayerVolume(&LeftVolume, &RightVolume);
        // ZPlay �� 0 ~ 100 ���� �̱� ������
        // DxSound -10000 ~ 0 ���� �����.
        unsigned int Volume = (LeftVolume+RightVolume)/2;

		LONG lVolume = LONG( NSSoundDef::CONTROL_DECIBEL * Volume );
		if( lVolume < 1 )
			lVolume = DSBVOLUME_MIN;
		else
			lVolume = lVolume - NSSoundDef::CONTROL_DECIBEL;

		return lVolume;
    }
    else
    {
        return 0;
    }
}

void DxBgmSound::SetVolume(LONG Volume)
{
    // DxSound �Ǽ��� ������ ���� ������, 
    // DSBVOLUME_MAX (���� ����)�κ��� DSBVOLUME_MIN (����)�����̴�. 
    // �̷��� ����, Dsound.h �������� ���� 0 �� 10, 000 ���� ���ǵǰ� �ִ�. 
    // �� DSBVOLUME_MAX ��, ��Ʈ�� ������ �������� ������ ��Ÿ����. 
    // �� DSBVOLUME_MIN ��, ������� ������ 100 dB �����ϰ� �ִ� ���� ��Ÿ��, �̰��� ���������δ� ������ �ǹ��Ѵ�.
    // DirectSound �� ������ ���� �ϰ� ���� �ʴ�. 
    if (m_pPlayer)
    {
		// DxSound -10000 ~ 0 �̱� ������
		// ZPlay �� 0 ~ 100 �� �����.  
		float fVolume = 0.0f;
		if( DSBVOLUME_MIN != Volume )
			fVolume = float( Volume + NSSoundDef::CONTROL_DECIBEL ) / NSSoundDef::CONTROL_DECIBEL;

		m_Volume = static_cast<LONG>( fVolume * 100 );

        m_pPlayer->SetPlayerVolume(m_Volume, m_Volume);
    }
}

HRESULT DxBgmSound::CtrlVolume()
{
    if (m_pPlayer)
    {
        m_pPlayer->SetPlayerVolume(m_Volume, m_Volume);
        return S_OK;
    }
    else
    {
        return S_FALSE;
    }
}