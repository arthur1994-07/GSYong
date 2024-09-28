#include "stdafx.h"
#include "./BgmSound.h"

#define LIB_ZPLAY_STATIC
#include "../../../LibZplay/include/libzplay.h"

// use libZPlay namespace
using namespace libZPlay;

int __stdcall BgmCallbackFunc(void* instance, void *user_data, TCallbackMessage message, unsigned int param1, unsigned int param2)
{
    BgmSound* pBgmSound = (BgmSound*) user_data;
    if (BgmSound::BGM_STOP == pBgmSound->GetStatus())
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

BgmSound::BgmSound()
{
    m_pPlayer = CreateZPlay();
    if (!m_pPlayer)
    {
        // create error
    }
    int Version = m_pPlayer->GetVersion();
    unsigned int LeftVolume = 0;
    unsigned int RightVolume = 0;
    m_pPlayer->GetMasterVolume(&LeftVolume, &RightVolume);
    m_SoundLength = 0;

     m_pPlayer->SetCallbackFunc(
         BgmCallbackFunc,
         (TCallbackMessage) (MsgStop|MsgStopAsync),
         this);
}

BgmSound::~BgmSound()
{
    if (m_pPlayer)
    {
        Stop();
        m_pPlayer->Release();
    }
}

void BgmSound::AddPlayList(const std::string& FileName)
{
    if (m_pPlayer)
        m_pPlayer->AddFile(FileName.c_str(), libZPlay::sfAutodetect);
}

bool BgmSound::OpenFile(const std::string& FileName)
{
    if (!m_pPlayer)
        return false;

    Stop();

    if (m_pPlayer->OpenFile(FileName.c_str(), libZPlay::sfAutodetect) == 0)
    {
        // OpenFile error
        return false;
    }
    
    m_SoundFileName = FileName;

    libZPlay::TStreamInfo pInfo;
    m_pPlayer->GetStreamInfo(&pInfo);
    const libZPlay::TStreamTime& Length = pInfo.Length;
    m_SoundLength = Length.ms;

    return true;
}

void BgmSound::Pause()
{
    if (m_pPlayer)
        m_pPlayer->Pause();
}

void BgmSound::Play()
{
    if (m_pPlayer)
    {
        //TStreamTime CurrentPos;
        //memset(&CurrentPos, 0, sizeof(TStreamTime));
        //m_pPlayer->GetPosition(&CurrentPos);        
        //TStreamTime EndPosition;
        //EndPosition.ms = CurrentPos.ms + m_SoundLength;
        m_pPlayer->Play();
        //m_pPlayer->PlayLoop(tfMillisecond, &CurrentPos, tfMillisecond, &EndPosition, 3, 1);

        m_Status = BGM_PLAY;

//         TStreamStatus status;
//         m_pPlayer->GetStatus(&status);
//         if (status.fPlay == 0)
//             break; // exit checking loop
// 
//         // get current status
//         m_pPlayer->GetStatus(&status); 
// 
//         char *st = 0;
//         if(status.fPlay)
//             st = "Play ";
//         else if(status.fPause)
//             st = "Pause";
//         else
//             st = "Stop ";		


    }
}

void BgmSound::Stop()
{
    if (m_pPlayer)
    {
        m_Status = BGM_STOP;
        m_pPlayer->Stop();
    }
}

void BgmSound::Resume()
{
    if (m_pPlayer)
        m_pPlayer->Resume();
}

void BgmSound::SetVolume(int Volume)
{
    if (m_pPlayer)
        m_pPlayer->SetMasterVolume(Volume, Volume);
}