#ifndef	__DX_BGM_SOUND__
#define	__DX_BGM_SOUND__

#pragma once

#include <string>
#include "../DxCommon/dsutil.h"
#include "../DxCommon/dxutil.h"

namespace libZPlay
{
    class ZPlay;
}

class DxBgmSound
{
public:
    enum STATUS
    {
        BGM_STOP,
        BGM_PLAY,
        BGM_MUTE,
    };

private:
	DxBgmSound();
	~DxBgmSound();
    
public:
    static DxBgmSound& GetInstance();

private:
	BOOL m_bSoundEnable;
    std::string m_Path;
    std::string m_BgmFileName;
    libZPlay::ZPlay* m_pPlayer;    
    unsigned int m_SoundLength;
    STATUS m_Status;
    unsigned int m_Volume;

private:
    BOOL OpenFile();
    HRESULT	CtrlVolume();
    BOOL IsMute();

public:
    //BOOL    SetHWND(HWND hWnd);
	BOOL    Run( HWND hWnd, BOOL bSoundEnable );

	BOOL	SetPath(const CString& strPath);
	const char* GetPath() const;

	BOOL	SetFile(const CString& strFileName);
//	BOOL	SetFile(const TCHAR* pszFileName);
	const char* GetFile();
	void	ClearFile();

	BOOL	Play();
	BOOL	Stop();
	BOOL	ForceStop();
	//BOOL	IsPlaying();
    //BOOL    Playing();

	LONG	GetVolume();
	void	SetVolume(LONG Volume);
	
	void	SetMute(BOOL bMute);
	
    void SetActivate(BOOL bActivity);
	//BOOL IsActivated() const { return  m_bActivated; }

    __forceinline STATUS GetStatus() const { return m_Status; }
};

#endif	//	__DX_BGM_SOUND__