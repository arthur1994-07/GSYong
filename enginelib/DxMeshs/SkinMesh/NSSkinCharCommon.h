#pragma once

#include "../../DxTools\DxVertexFVF.h"

class DxSkinAniControlData;

namespace NSSkinCharCommon
{
	BOOL LoadAnimationFromTool( const TCHAR* szNameCHF, 
								const TCHAR* szNameCFG, 
								const TCHAR* szNameXFile, 
								LPDIRECT3DDEVICEQ pd3dDevice, 
								bool bREPRESH, 
								DxSkinAniControlData& sSkinAniControlData,
								VEC_TSTRING& vecAniFile );

	void ReleaseAnimationFromTool( LPDIRECT3DDEVICEQ pd3dDevice, 
									const TCHAR* szAnim,
									DxSkinAniControlData& sSkinAniControlData,
									VEC_TSTRING& vecAniFile );
};
