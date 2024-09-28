#include "pch.h"
#include "./DxEffSingleMan.h"
#include "./DxEffectParticleSys.h"
#include "./DxEffectGround.h"
#include "./DxEffectMesh.h"
#include "./DxEffectSequence.h"
#include "./DxEffectDecal.h"
#include "./DxEffectCamera.h"
#include "./DxEffectBlurSys.h"
#include "./DxEffectLighting.h"
#include "./DxEffectMoveRotate.h"
#include "./DxEffectTargetInfluence.h"
#include "./DxEffectSkinMesh.h"
#include "./DxEffectWave.h"
#include "./DxEffectPointLight.h"
#include "./DxEffectMoveTarget.h"
#include "./DxEffectScreen.h"
#include "./Util/DxEffFanwise.h"
#include "./Util/DxEffPlane.h"
#include "./Util/DxEffCircle.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

DxEffSingleMan& DxEffSingleMan::GetInstance()
{
	static DxEffSingleMan Instance;
	return Instance;
}

DxEffSingleMan::DxEffSingleMan(void) :
	pEffectTypeList(NULL)
{
	RegistType ( DxEffectParticleSys::TYPEID, DxEffectParticleSys::NAME );
	RegistType ( DxEffectGround::TYPEID, DxEffectGround::NAME );
	RegistType ( DxEffectMesh::TYPEID, DxEffectMesh::NAME );
	RegistType ( DxEffectSequence::TYPEID, DxEffectSequence::NAME );
	//RegistType ( DxEffectDecal::TYPEID, DxEffectDecal::NAME );
	RegistType ( DxEffectCamera::TYPEID, DxEffectCamera::NAME );
	RegistType ( DxEffectBlurSys::TYPEID, DxEffectBlurSys::NAME );
	RegistType ( DxEffectLightning::TYPEID, DxEffectLightning::NAME );
	RegistType ( DxEffectMoveRotate::TYPEID, DxEffectMoveRotate::NAME );
	RegistType ( DxEffectTargetInfluence::TYPEID, DxEffectTargetInfluence::NAME );
	RegistType ( DxEffectSkinMesh::TYPEID, DxEffectSkinMesh::NAME );
	RegistType ( DxEffectWave::TYPEID, DxEffectWave::NAME );
	RegistType ( DxEffectPointLight::TYPEID, DxEffectPointLight::NAME );
	RegistType ( DxEffectMoveTarget::TYPEID, DxEffectMoveTarget::NAME );
	RegistType ( DxEffectScreen::TYPEID, DxEffectScreen::NAME );
	RegistType ( DxEffFanwise::TYPEID, DxEffFanwise::NAME );
	RegistType ( DxEffPlane::TYPEID, DxEffPlane::NAME );
	RegistType ( DxEffCircle::TYPEID, DxEffCircle::NAME );
}

DxEffSingleMan::~DxEffSingleMan(void)
{
	SAFE_DELETE(pEffectTypeList);
}

void DxEffSingleMan::RegistType ( const DWORD TypeID, const char *Name )
{
	GASSERT(Name);

	DxEffSingleType *pCurEff = pEffectTypeList;
	while(pCurEff)
	{
		GASSERT ( pCurEff->TYPEID != TypeID && "TypeID �ߺ� ��ϵɼ� �����ϴ�." );
		GASSERT ( strcmp(pCurEff->NAME,Name) && "NAME�� �ߺ� ��ϵɼ� �����ϴ�." );

		pCurEff = pCurEff->pNext;
	}

	//	Note : �� ��� ������ ����.
	//
	DxEffSingleType *pNewType = new DxEffSingleType;
	pNewType->TYPEID = TypeID;
	StringCchCopy( pNewType->NAME, MAX_PATH, Name );

	//	Note : ����Ʈ�� ���.
	//
	pNewType->pNext = pEffectTypeList;
	pEffectTypeList = pNewType;
}

HRESULT DxEffSingleMan::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	DxEffectParticleSys::CreateDevice ( pd3dDevice );
	DxEffectGround::CreateDevice ( pd3dDevice );
	DxEffectMesh::CreateDevice ( pd3dDevice );
	DxEffectSequence::CreateDevice ( pd3dDevice );
	DxEffectDecal::CreateDevice ( pd3dDevice );
	DxEffectCamera::CreateDevice ( pd3dDevice );
	DxEffectBlurSys::CreateDevice ( pd3dDevice );
	DxEffectLightning::CreateDevice ( pd3dDevice );
	DxEffectMoveRotate::CreateDevice ( pd3dDevice );
	DxEffectTargetInfluence::CreateDevice ( pd3dDevice );
	DxEffectSkinMesh::CreateDevice ( pd3dDevice );
	DxEffectWave::CreateDevice ( pd3dDevice );
	DxEffectPointLight::CreateDevice ( pd3dDevice );
	DxEffectMoveTarget::CreateDevice ( pd3dDevice );
	DxEffFanwise::CreateDevice( pd3dDevice );
	DxEffPlane::CreateDevice( pd3dDevice );
	DxEffCircle::CreateDevice( pd3dDevice );

	return S_OK;
}

HRESULT DxEffSingleMan::InvalidateDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	DxEffectParticleSys::ReleaseDevice ( pd3dDevice );
	DxEffectGround::ReleaseDevice ( pd3dDevice );
	DxEffectMesh::ReleaseDevice ( pd3dDevice );
	DxEffectSequence::ReleaseDevice ( pd3dDevice );
	DxEffectDecal::ReleaseDevice ( pd3dDevice );
	DxEffectCamera::ReleaseDevice ( pd3dDevice );
	DxEffectBlurSys::ReleaseDevice ( pd3dDevice );
	DxEffectLightning::ReleaseDevice ( pd3dDevice );
	DxEffectMoveRotate::ReleaseDevice ( pd3dDevice );
	DxEffectTargetInfluence::ReleaseDevice ( pd3dDevice );
	DxEffectSkinMesh::ReleaseDevice ( pd3dDevice );
	DxEffectWave::ReleaseDevice ( pd3dDevice );
	DxEffectPointLight::ReleaseDevice ( pd3dDevice );
	DxEffectMoveTarget::ReleaseDevice ( pd3dDevice );
	DxEffFanwise::ReleaseDevice( pd3dDevice );
	DxEffPlane::ReleaseDevice( pd3dDevice );
	DxEffCircle::ReleaseDevice( pd3dDevice );

	return S_OK;
}

HRESULT DxEffSingleMan::OneTimeSceneInit()
{
	DxEffectGround::OnInitDevice_STATIC();
	DxEffectMesh::OnInitDevice_STATIC();
	DxEffectSequence::OnInitDevice_STATIC();
	DxEffectDecal::OnInitDevice_STATIC();
	DxEffectCamera::OnInitDevice_STATIC();
	DxEffectBlurSys::OnInitDevice_STATIC();
	DxEffectLightning::OnInitDevice_STATIC();
	DxEffectMoveRotate::OnInitDevice_STATIC();
	DxEffectTargetInfluence::OnInitDevice_STATIC();
	DxEffectSkinMesh::OnInitDevice_STATIC();
	DxEffectWave::OnInitDevice_STATIC();
	DxEffectPointLight::OnInitDevice_STATIC();
	DxEffectMoveTarget::OnInitDevice_STATIC();
	DxEffFanwise::OnInitDevice_STATIC();
	DxEffPlane::OnInitDevice_STATIC();
	DxEffCircle::OnInitDevice_STATIC();

	return S_OK;
}

HRESULT DxEffSingleMan::FinalCleanup()
{
	DxEffectGround::OnDeleteDevice_STATIC();
	DxEffectMesh::OnDeleteDevice_STATIC();
	DxEffectSequence::OnDeleteDevice_STATIC();
	DxEffectDecal::OnDeleteDevice_STATIC();
	DxEffectCamera::OnDeleteDevice_STATIC();
	DxEffectBlurSys::OnDeleteDevice_STATIC();
	DxEffectLightning::OnDeleteDevice_STATIC();
	DxEffectMoveRotate::OnDeleteDevice_STATIC();
	DxEffectTargetInfluence::OnDeleteDevice_STATIC();
	DxEffectSkinMesh::OnDeleteDevice_STATIC();
	DxEffectWave::OnDeleteDevice_STATIC();
	DxEffectPointLight::OnDeleteDevice_STATIC();
	DxEffectMoveTarget::OnDeleteDevice_STATIC();
	DxEffFanwise::OnDeleteDevice_STATIC();
	DxEffPlane::OnDeleteDevice_STATIC();
	DxEffCircle::OnDeleteDevice_STATIC();

	return S_OK;
}


