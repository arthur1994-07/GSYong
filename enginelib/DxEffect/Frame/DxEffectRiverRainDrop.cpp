// Terrain0.cpp: implementation of the DxEffectRiver class.
//
//	UPDATE [03.02.20] : ���Ӱ� �� ȿ���� �ٲ�
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "../../DxTools/TextureManager.h"
#include "../../DxMeshs/FrameMesh/DxFrameMesh.h"
#include "../../DxEffect/Frame/DxEffectDefine.h"

#include "../../DxEffect/Frame/DxEffectMan.h"
#include "../../DxTools/DxShadowMap.h"
#include "../../DxTools/DxSurfaceTex.h"
#include "../../DxTools/DxEnvironment.h"
#include "../../DxTools/DxCubeMap.h"

#include "../../DxTools/DxViewPort.h"

#include "../../../SigmaCore/File/SerialFile.h"
#include "../../DxTools/Collision.h"

#include "./DxEffectRiver.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const DWORD DXRAINDROPMAN::VERTEXCOLOR::FVF = D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1;

BOOL RAINDROP::FrameMove ( float fETime )
{
	float fRate = fTime/0.5f;			// 0~1 �� ���� ���´�.

	fSize = 1.f - fRate;				// ũ�Ⱑ  1~0 ����.

	fTime += fETime;
	if ( fTime > 0.5f )	return FALSE;	// Ÿ�� ��

	return TRUE;
}

DXRAINDROPMAN::DXRAINDROPMAN () :
	m_fWidth(10.f)
{
	StringCchCopy( m_szRainDropTEX, MAX_PATH, "_Wa_water1.bmp" );
}

DXRAINDROPMAN::~DXRAINDROPMAN ()
{
}

HRESULT DXRAINDROPMAN::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
    m_textureRes = TextureManager::GetInstance().LoadTexture(
        m_szRainDropTEX,
        false,
        TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
        0,
        D3DFMT_UNKNOWN,
        TEXTURE_RESIZE_NONE,
        TEXTURE_EFFECT,
		false,
		FALSE );

	return S_OK;
}

HRESULT DXRAINDROPMAN::DeleteDeviceObjects ()
{
	m_textureRes.Clear();

	return S_OK;
}

void	DXRAINDROPMAN::SetMaxMin ( D3DXVECTOR3 vMax, D3DXVECTOR3 vMin )
{
	float fRangeX = vMax.x - vMin.x;
	float fRangeZ = vMax.z - vMin.z;

	m_fAREA = fRangeX*fRangeZ;		//����
}

HRESULT DXRAINDROPMAN::FrameMove ( float fTime, float fElapsedTime )
{
	return S_OK;
}

HRESULT DXRAINDROPMAN::Render ( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DVERTEXBUFFERQ pVB, LPDIRECT3DINDEXBUFFERQ pIB )
{
	return S_OK;
}
