#ifndef _BGM_SOUND_H_
#define _BGM_SOUND_H_

#pragma once

#include <string>

namespace libZPlay
{
    class ZPlay;
}

class BgmSound
{
public:
    enum STATUS
    {
        BGM_STOP,
        BGM_PLAY,
    };

public:
    BgmSound();
    ~BgmSound();

private:
    libZPlay::ZPlay* m_pPlayer;
    std::string m_SoundFileName;
    unsigned int m_SoundLength;
    STATUS m_Status;

public:
    bool OpenFile(const std::string& FileName);    
    void AddPlayList(const std::string& FileName);
    void Play();
    void Stop();
    void Pause();
    void Resume();
    void SetVolume(int Volume);
    __forceinline STATUS GetStatus() const { return m_Status; }
};

#endif