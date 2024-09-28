#include "pch.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "DxSoundLib.h"
#include "DxSoundMan.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

DxSoundLib::DxSoundLib ()
{
}

DxSoundLib::~DxSoundLib ()
{
	ReleaseAllSound ();
}

bool DxSoundLib::CreateSound(const std::string& strKeyword, const std::string& strFileName, int nVolType, bool bLoop)
{
	SOUNDLIB_MAP_CITER iterFind = m_mapSoundLib.find(strKeyword);
	if (iterFind != m_mapSoundLib.end())
        return true;

	// 사운드 생성
	SSoundEx* pNewSound = new SSoundEx;
	pNewSound->m_emVolType = (ET_SOUNDTYPE) nVolType; // 볼륨 타입
	if (!pNewSound->Load(strFileName, bLoop, true))
	{
		SAFE_DELETE(pNewSound);
        std::string ErrorMsg(
            sc::string::format(
                "[DxSoundLib::CreateSound] KeyWord:%1%, FileName:%2% Load Fail.", 
                strKeyword, 
                strFileName));
		CDebugSet::ToLogFile(ErrorMsg);
		return false;
	}
	m_mapSoundLib.insert(SOUNDLIB_MAP_VALUE(strKeyword, pNewSound));
	return true;
}

bool DxSoundLib::ReleaseSound(const std::string& strKeyword)
{
	SOUNDLIB_MAP_CITER iterFind = m_mapSoundLib.find(strKeyword);	
	if (iterFind == m_mapSoundLib.end())
        return false;

	//	사운드 해제	
	SSoundEx* pReleaseSound = iterFind->second;	
	SAFE_DELETE(pReleaseSound);
	m_mapSoundLib.erase(iterFind);
	return true;
}

bool DxSoundLib::PlaySound(const std::string& strKeyword)
{
	SOUNDLIB_MAP_CITER iterFind = m_mapSoundLib.find(strKeyword);	
	if (iterFind == m_mapSoundLib.end())
        return false;

	const long lMapVolume = DxSoundMan::GetInstance().GetMapVolume();
	const long lSfxVolume = DxSoundMan::GetInstance().GetSfxVolume();
	const BOOL bMapMute = DxSoundMan::GetInstance().IsMapMute();
	const BOOL bSfxMute = DxSoundMan::GetInstance().IsSfxMute();

	SSoundEx* pPlaySound = iterFind->second;

	//	MUTE 처리
	switch (pPlaySound->m_emVolType)
	{
	case MAP_SOUND:	
		{
			if (bMapMute)
                return true;
		}
		break;
	case SFX_SOUND:
		{
			if (bSfxMute)
                return true;
		}
		break;
	}

	if (!pPlaySound->Play())
        return false;

	//	볼륨 조절
	switch (pPlaySound->m_emVolType)
	{
	case MAP_SOUND:
		{
			if (!pPlaySound->SetVolume(lMapVolume))
                return false;
		}
		break;
	case SFX_SOUND:
		{
			if (!pPlaySound->SetVolume(lSfxVolume))
                return false;
		}
		break;
	}

	return true;
}

bool DxSoundLib::StopSound(const std::string& strKeyword)
{
	if (m_mapSoundLib.empty())
        return false;

	SOUNDLIB_MAP_CITER iterFind = m_mapSoundLib.find(strKeyword);	
	if (iterFind == m_mapSoundLib.end())
    {
        return false;
    }
    else
    {
	    SSoundEx* pStopSound = iterFind->second;
	    return pStopSound->Stop();
    }
}

bool DxSoundLib::IsPlaying(const std::string& strKeyword)
{
    SOUNDLIB_MAP_CITER iterFind = m_mapSoundLib.find(strKeyword);
	if (iterFind == m_mapSoundLib.end())
    {
        return false;
    }
    else
    {
	    SSoundEx* pPlayingSound = iterFind->second;
	    return pPlayingSound->IsPlaying();
    }
}

bool DxSoundLib::ReleaseAllSound()
{
	SOUNDLIB_MAP_CITER citer = m_mapSoundLib.begin ();
	SOUNDLIB_MAP_CITER citer_end = m_mapSoundLib.end ();

	for (; citer!=citer_end; ++citer)
	{
		SSoundEx* pReleaseSound = citer->second;
		SAFE_DELETE(pReleaseSound);
	}
	m_mapSoundLib.clear();
	return true;
}

DxSoundLib*	DxSoundLib::GetInstance ()
{
	static	DxSoundLib sInstance;
	return &sInstance;
}

void DxSoundLib::ReleaseInstance()
{
}

bool DxSoundLib::Update()
{
	const long lMapVolume = DxSoundMan::GetInstance().GetMapVolume ();
	const long lSfxVolume = DxSoundMan::GetInstance().GetSfxVolume ();
	const BOOL bMapMute = DxSoundMan::GetInstance().IsMapMute ();
	const BOOL bSfxMute = DxSoundMan::GetInstance().IsSfxMute ();

	//	기타 여러가지 상황을 업데이트 함
	SOUNDLIB_MAP_CITER citer = m_mapSoundLib.begin ();
	SOUNDLIB_MAP_CITER citer_end = m_mapSoundLib.end ();
	for ( ; citer != citer_end; ++citer )
	{
		SSoundEx* pUpdateSound = citer->second;
		if (!pUpdateSound->IsPlaying())
            continue;

		switch (pUpdateSound->m_emVolType)
		{
		case MAP_SOUND:
			{
				if (bMapMute && !pUpdateSound->Stop())
                    return false;
				if (!pUpdateSound->SetVolume(lMapVolume))
                    return false;				
			}
			break;
		case SFX_SOUND:
			{
				if (bSfxMute && !pUpdateSound->Stop())
                    return false;
				if (!pUpdateSound->SetVolume(lSfxVolume))
                    return false;
			}
			break;
        default:
            break;
		}
	}
	return true;
}