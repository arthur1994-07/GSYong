#ifndef _ENGINE_DX_SOUND_LIB_H_
#define _ENGINE_DX_SOUND_LIB_H_

#pragma	once

#include <boost/tr1/unordered_map.hpp>
#include <string>
#include <boost/unordered_map.hpp>
#include "../../InternalCommonLib/Interface/DxSoundInterface.h"
#include "./SuperSound.h"

enum ET_SOUNDTYPE
{
	SFX_SOUND,
	MAP_SOUND
};

struct SSoundEx : public SSound
{
	ET_SOUNDTYPE	m_emVolType;
};

class DxSoundLib : public ic::IDxSound
{
private:
	DxSoundLib();
	virtual	~DxSoundLib();

public:
    static DxSoundLib* GetInstance();
    static void ReleaseInstance();	

public:
    typedef	std::tr1::unordered_map<std::string,SSoundEx*> SOUNDLIB_MAP;
	typedef	SOUNDLIB_MAP::const_iterator                SOUNDLIB_MAP_CITER;
	typedef	SOUNDLIB_MAP::iterator                      SOUNDLIB_MAP_ITER;
	typedef	SOUNDLIB_MAP::size_type                     SOUNDLIB_MAP_SIZE;
    typedef SOUNDLIB_MAP::value_type                    SOUNDLIB_MAP_VALUE;

private:
    SOUNDLIB_MAP m_mapSoundLib;

public:
	bool CreateSound(const std::string& strKeyword, const std::string& strFileName, int nVolType, bool bLoop=false);
	bool ReleaseSound(const std::string& strKeyword);
	bool PlaySound(const std::string& strKeyword);
	bool StopSound(const std::string& strKeyword);
	bool IsPlaying(const std::string& strKeyword);
	bool Update();
	bool ReleaseAllSound();
};

#endif // _ENGINE_DX_SOUND_LIB_H_