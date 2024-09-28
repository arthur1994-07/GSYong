#include "pch.h"

#include "../../../SigmaCore/File/SerialFile.h"
#include "../../DxTools/EDITMESHS.h"

#include "../../DxMeshs/SkinMesh/DxSkinChar.h"
#include "../../DxMeshs/SkinMesh/DxAttBone.h"
#include "../../DxMeshs/SkinMesh/DxVehicle.h"
#include "../../DxMeshs/SkinMesh/DxSkeletonManager.h"

#include "./DxEffAniTrace.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

DWORD		DxEffAniData_Trace::TYPEID		= EMEFFANI_TRACE;
DWORD		DxEffAniData_Trace::VERSION		= 0x0101;
char		DxEffAniData_Trace::NAME[64]	= "Trace EFFECT";

DxEffAni* DxEffAniData_Trace::NEWOBJ ( DxSkinAnimation* pAnimContainer )
{
	DxEffAniTrace*	pEff = new DxEffAniTrace;

	pEff->m_fScale	= m_fScale;
	pEff->Create( m_pd3dDevice, pAnimContainer );

	return	pEff;
}

HRESULT	DxEffAniData_Trace::LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	DWORD dwVer, dwSize, dwTypeID;

	SFile >> dwTypeID;
	SFile >> dwVer;
	SFile >> dwSize;

	//	Note : 버전이 일치할 경우. 
	if ( dwVer == VERSION )
	{
		//	Note : 이팩트의 Property 를 저장.
		SFile.ReadBuffer ( &m_Property, sizeof(m_Property) );
	}
    else if ( dwVer == 0x0100 )
	{
		//	Note : 이팩트의 Property 를 저장.
        EFFANI_PROPERTY_TRACE_100 sProp;
		SFile.ReadBuffer ( &sProp, sizeof(sProp) );
        m_Property.m_fScale = sProp.m_fScale;
	}
	else
	{
		//	Note : 버전이 틀릴 경우에는 파일에 쓰여진 DATA영역을 건더 띄는 작업을 진행.
		SFile.SetOffSet ( SFile.GetfTell()+dwSize );
		
		return E_FAIL;
	}

	//	Note : Device 자원을 생성한다.
	if ( pd3dDevice )
	{
		hr = Create ( pd3dDevice );
		if ( FAILED(hr) )	return hr;
	}

	return S_OK;
}

HRESULT	DxEffAniData_Trace::SaveFile ( sc::SerialFile &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	SFile << (DWORD) ( sizeof(m_Property) );

	//	Note : 이팩트의 Property 를 저장.
	SFile.WriteBuffer ( &m_Property, sizeof(m_Property) );

	return S_OK;
}

void DxEffAniData_Trace::SetEffAniData ( DxEffAniData*	pData )
{
	DxEffAniData::SetEffAniData ( pData );
}

HRESULT DxEffAniData_Trace::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	DxEffAniData::InitDeviceObjects ( pd3dDevice );
	return S_OK;
}

HRESULT DxEffAniData_Trace::DeleteDeviceObjects ()
{
	DxEffAniData::DeleteDeviceObjects ();
	return S_OK;
}

//	-----------------------------------------------	--------------------------------------------------

DxEffAniTrace::DxEffAniTrace(void) :
	DxEffAni()
{
}

DxEffAniTrace::~DxEffAniTrace(void)
{
	CleanUp ();
}

HRESULT DxEffAniTrace::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT DxEffAniTrace::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT DxEffAniTrace::OneTimeSceneInit ()
{
	return S_OK;
}

HRESULT DxEffAniTrace::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT DxEffAniTrace::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT DxEffAniTrace::InvalidateDeviceObjects ()
{
	return S_OK;
}

HRESULT DxEffAniTrace::DeleteDeviceObjects ()
{
	return S_OK;
}

HRESULT DxEffAniTrace::FinalCleanup ()
{
	return S_OK;
}

HRESULT DxEffAniTrace::FrameMove ( DxSkinCharEffAniBase* pSkinCharBase, float fTime, float fElapsedTime )
{
	return S_OK;
}

HRESULT	DxEffAniTrace::Render ( const LPDIRECT3DDEVICEQ pd3dDevice, DxSkinCharEffAniBase* pSkinChar, const D3DXMATRIX& matCurPos, float fAlpha )
{
	D3DXVECTOR3 vLeft( -m_fScale, 0.f, 0.f );
	D3DXVECTOR3 vRight( m_fScale, 0.f, 0.f );
	
	D3DXVec3TransformCoord( &vLeft, &vLeft, &matCurPos );
	D3DXVec3TransformCoord( &vRight, &vRight, &matCurPos );

	EDITMESHS::RENDERSPHERE( pd3dDevice, vLeft, 1.f, NULL );
	EDITMESHS::RENDERSPHERE( pd3dDevice, vRight, 1.f, NULL );

	return S_OK;
}

//HRESULT	DxEffAniTrace::Render ( const LPDIRECT3DDEVICEQ pd3dDevice, DxAttBone* pAttBone, const D3DXMATRIX& matCurPos, float fAlpha )
//{
//	D3DXVECTOR3 vLeft( -m_fScale, 0.f, 0.f );
//	D3DXVECTOR3 vRight( m_fScale, 0.f, 0.f );
//
//	D3DXVec3TransformCoord( &vLeft, &vLeft, &matCurPos );
//	D3DXVec3TransformCoord( &vRight, &vRight, &matCurPos );
//
//	EDITMESHS::RENDERSPHERE( pd3dDevice, vLeft, 1.f, NULL );
//	EDITMESHS::RENDERSPHERE( pd3dDevice, vRight, 1.f, NULL );
//
//	return S_OK;
//}
//
//HRESULT	DxEffAniTrace::Render ( const LPDIRECT3DDEVICEQ pd3dDevice, DxVehicle* pVehicle, const D3DXMATRIX& matCurPos, float fAlpha )
//{
//	D3DXVECTOR3 vLeft( -m_fScale, 0.f, 0.f );
//	D3DXVECTOR3 vRight( m_fScale, 0.f, 0.f );
//
//	D3DXVec3TransformCoord( &vLeft, &vLeft, &matCurPos );
//	D3DXVec3TransformCoord( &vRight, &vRight, &matCurPos );
//
//	EDITMESHS::RENDERSPHERE( pd3dDevice, vLeft, 1.f, NULL );
//	EDITMESHS::RENDERSPHERE( pd3dDevice, vRight, 1.f, NULL );
//
//	return S_OK;
//}
//
