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

	// 사운드 카드가 없다면 m_bSoundEnable 가 FALSE 이다.
	// BGM 도 동작 안되도록 하자.
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

	// 바로 적용한다.
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
    // DxSound 의설정 가능한 값의 범위는, 
    // DSBVOLUME_MAX (감쇠 없음)로부터 DSBVOLUME_MIN (무음)까지이다. 
    // 이러한 값은, Dsound.h 내에서는 각각 0 및 10, 000 으로 정의되고 있다. 
    // 값 DSBVOLUME_MAX 는, 스트림 본래의 미조정의 볼륨을 나타낸다. 
    // 값 DSBVOLUME_MIN 는, 오디오의 볼륨이 100 dB 감쇠하고 있는 것을 나타내, 이것은 실제적으로는 무음을 의미한다.
    // DirectSound 는 증폭을 지원 하고 있지 않다. 
    if (m_pPlayer)
    {
        unsigned int LeftVolume = 0;
        unsigned int RightVolume = 0;
        m_pPlayer->GetPlayerVolume(&LeftVolume, &RightVolume);
        // ZPlay 는 0 ~ 100 까지 이기 때문에
        // DxSound -10000 ~ 0 으로 맞춘다.
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
    // DxSound 의설정 가능한 값의 범위는, 
    // DSBVOLUME_MAX (감쇠 없음)로부터 DSBVOLUME_MIN (무음)까지이다. 
    // 이러한 값은, Dsound.h 내에서는 각각 0 및 10, 000 으로 정의되고 있다. 
    // 값 DSBVOLUME_MAX 는, 스트림 본래의 미조정의 볼륨을 나타낸다. 
    // 값 DSBVOLUME_MIN 는, 오디오의 볼륨이 100 dB 감쇠하고 있는 것을 나타내, 이것은 실제적으로는 무음을 의미한다.
    // DirectSound 는 증폭을 지원 하고 있지 않다. 
    if (m_pPlayer)
    {
		// DxSound -10000 ~ 0 이기 때문에
		// ZPlay 는 0 ~ 100 로 맞춘다.  
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