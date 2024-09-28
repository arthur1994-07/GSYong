// NSCharAniSequence.h: interface for the CCollision class.
//////////////////////////////////////////////////////////////////////

#pragma once

class DxSkinChar;
class DxSkinCharData;
class EngineDeviceMan;
class DxLandMan;
class GLFlyCameraKeyFrameControl;

#include <boost/smart_ptr/shared_ptr.hpp>
#include <vector>

namespace NSCharAniSequence
{
	enum 
	{
		EMANISEQ_ONLYCURRENTFRAME	= 0x0001,
		EMANISEQ_MULTIANIM			= 0x0002,
	};
	extern TSTRING	g_strPath;

	void Create( LPDIRECT3DDEVICEQ pd3dDevice, 
				EngineDeviceMan* pEngineDevice,
				DxSkinCharData* pSkinCharDataSRC,
				const TCHAR* pCharName, 
				//const TCHAR* pAniName, 
				std::vector<std::string>& vecAnimName,
				//const TCHAR* pLandName,
				boost::shared_ptr<DxLandMan> spLandMan,
				DWORD dwScreenSizeType,
				DWORD dwAA,
				DWORD dwFPS,
				float fAddTime,
				float fColorPower, 
				const D3DXMATRIX& matWorld,
				DWORD dwAnimSeqFlag,
				DWORD dwCurrentFrame,
				GLFlyCameraKeyFrameControl* pFlyCam = NULL);

	BOOL IsComplete();
	void SetCustomScnWidth(INT nWidth);
	void SetCustomScnHeight(INT nHeight);
	INT GetCustomScnWidth(void);
	INT GetCustomScnHeight(void);
	INT GetScnWidth(INT idx);
	INT GetScnHeight(INT idx);

}