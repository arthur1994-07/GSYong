#include "pch.h"
#include "../../../SigmaCore/String/StringUtil.h"
#include "../../../SigmaCore/File/SerialFile.h"
#include "../../../SigmaCore/Log/LogMan.h"

#include "./DxEffectParticleSys.h"
#include "./DxEffectGround.h"
#include "./DxEffectMesh.h"
#include "./DxEffectSequence.h"
#include "./DxEffectDecal.h"
#include "./DxEffectCamera.h"
#include "./DxEffectBlurSys.h"
#include "./DxEffectLighting.h"
#include "./DxEffectMoveRotate.h"
#include "./DxEffectSkinMesh.h"
#include "./DxEffectWave.h"
#include "./DxEffectPointLight.h"
#include "./DxEffectMoveTarget.h"
#include "./DxEffSinglePropGMan.h"
#include "./DxEffectTargetInfluence.h"
#include "./Util/DxEffFanwise.h"
#include "./Util/DxEffPlane.h"
#include "./Util/DxEffCircle.h"

#include "../../DxTools/NsLoadingLagTester.h"
#include "../../DxTools/DxMethods.h"
#include "../../DxSound/DxSoundMan.h"
#include "../../G-Logic/GLogic.h"
#include "../../Common/ByteStream.h"
#include "../../Archive/Archive.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------]

extern BOOL g_bImportMobile;

const DWORD	EFF_PROPGROUP::FILEVERSION = 0x00000002;

//-------------------------------------------------------------------------------------[EFF_PROPERTY]

EFF_PROPERTY* EFF_PROPERTY::NEW_PROP ( DWORD TypeID )
{
	EFF_PROPERTY* pNew = NULL;

	if ( TypeID == PARTICLESYS_PROPERTY::TYPEID		)	pNew = new PARTICLESYS_PROPERTY;
	else if ( TypeID == GROUND_PROPERTY::TYPEID		)	pNew = new GROUND_PROPERTY;
	else if ( TypeID == MESH_PROPERTY::TYPEID		)   pNew = new MESH_PROPERTY;
	else if ( TypeID == SEQUENCE_PROPERTY::TYPEID	)   pNew = new SEQUENCE_PROPERTY;
	else if ( TypeID == DECAL_PROPERTY::TYPEID		)   pNew = new DECAL_PROPERTY;
	else if ( TypeID == CAMERA_PROPERTY::TYPEID		)   pNew = new CAMERA_PROPERTY;
	else if ( TypeID == BLURSYS_PROPERTY::TYPEID	)   pNew = new BLURSYS_PROPERTY;
	else if ( TypeID == LIGHTNING_PROPERTY::TYPEID	)   pNew = new LIGHTNING_PROPERTY;
	else if ( TypeID == MOVEROTATE_PROPERTY::TYPEID	)   pNew = new MOVEROTATE_PROPERTY;
	else if ( TypeID == SKINMESH_PROPERTY::TYPEID	)   pNew = new SKINMESH_PROPERTY;
	else if ( TypeID == WAVE_PROPERTY::TYPEID		)	pNew = new WAVE_PROPERTY;
	else if ( TypeID == POINTLIGHT_PROPERTY::TYPEID	)	pNew = new POINTLIGHT_PROPERTY;
	else if ( TypeID == MOVETARGET_PROPERTY::TYPEID	)	pNew = new MOVETARGET_PROPERTY;
	else if ( TypeID == SCREEN_PROPERTY::TYPEID		)	pNew = new SCREEN_PROPERTY;
	else if ( TypeID == FANWISE_PROPERTY::TYPEID	)	pNew = new FANWISE_PROPERTY;
	else if ( TypeID == PLANE_PROPERTY::TYPEID		)	pNew = new PLANE_PROPERTY;
	else if ( TypeID == CIRCLE_PROPERTY::TYPEID		)	pNew = new CIRCLE_PROPERTY;
	else if ( TypeID == TARGETINFLUENCE_PROPERTY::TYPEID)	pNew = new TARGETINFLUENCE_PROPERTY;

	return pNew;
}

void	EFF_PROPERTY::CloneEffProp ( LPDIRECT3DDEVICEQ pd3dDevice, EFF_PROPERTY* pSrcProp, BOOL bBackUpCache )
{
	this->m_fGBeginTime		= pSrcProp->m_fGBeginTime;
	this->m_fGLifeTime		= pSrcProp->m_fGLifeTime;
	this->m_MovSound		= pSrcProp->m_MovSound;
	this->m_matLocal		= pSrcProp->m_matLocal;

	DWORD dwType = pSrcProp->GetTypeID ();
	switch ( dwType )
	{
	case PARTICLESYS_PROPERTY::TYPEID:
		((PARTICLESYS_PROPERTY*)this)->m_Property = ((PARTICLESYS_PROPERTY*) pSrcProp)->m_Property;
		break;
	case GROUND_PROPERTY::TYPEID:
		((GROUND_PROPERTY*) this)->m_Property = ((GROUND_PROPERTY*) pSrcProp)->m_Property;
		break;
	case MESH_PROPERTY::TYPEID:
		((MESH_PROPERTY*) this)->m_Property = ((MESH_PROPERTY*) pSrcProp)->m_Property;
		break;
	case SEQUENCE_PROPERTY::TYPEID:
		((SEQUENCE_PROPERTY*) this)->m_Property = ((SEQUENCE_PROPERTY*) pSrcProp)->m_Property;
		break;
	case DECAL_PROPERTY::TYPEID:
		((DECAL_PROPERTY*) this)->m_Property = ((DECAL_PROPERTY*) pSrcProp)->m_Property;
		break;
	case CAMERA_PROPERTY::TYPEID:
		((CAMERA_PROPERTY*) this)->m_Property = ((CAMERA_PROPERTY*) pSrcProp)->m_Property;
		break;
	case BLURSYS_PROPERTY::TYPEID:
		((BLURSYS_PROPERTY*) this)->m_Property = ((BLURSYS_PROPERTY*) pSrcProp)->m_Property;
		break;
	case LIGHTNING_PROPERTY::TYPEID:
		((LIGHTNING_PROPERTY*) this)->m_Property = ((LIGHTNING_PROPERTY*) pSrcProp)->m_Property;
		break;
	case MOVEROTATE_PROPERTY::TYPEID:
		((MOVEROTATE_PROPERTY*) this)->m_Property = ((MOVEROTATE_PROPERTY*) pSrcProp)->m_Property;
		break;
	case SKINMESH_PROPERTY::TYPEID:
		((SKINMESH_PROPERTY*) this)->m_Property = ((SKINMESH_PROPERTY*) pSrcProp)->m_Property;
		break;
	case WAVE_PROPERTY::TYPEID:
		((WAVE_PROPERTY*) this)->m_Property = ((WAVE_PROPERTY*) pSrcProp)->m_Property;
		break;
	case POINTLIGHT_PROPERTY::TYPEID:
		((POINTLIGHT_PROPERTY*) this)->m_Property = ((POINTLIGHT_PROPERTY*) pSrcProp)->m_Property;
		break;
	case SCREEN_PROPERTY::TYPEID:
		((SCREEN_PROPERTY*) this)->m_Property = ((SCREEN_PROPERTY*) pSrcProp)->m_Property;
		break;
	case MOVETARGET_PROPERTY::TYPEID:
		((MOVETARGET_PROPERTY*) this)->m_Property = ((MOVETARGET_PROPERTY*) pSrcProp)->m_Property;
		break;
	case FANWISE_PROPERTY::TYPEID:
		((FANWISE_PROPERTY*) this)->m_Property = ((FANWISE_PROPERTY*) pSrcProp)->m_Property;
		break;
	case PLANE_PROPERTY::TYPEID:
		((PLANE_PROPERTY*) this)->m_Property = ((PLANE_PROPERTY*) pSrcProp)->m_Property;
		break;
	case CIRCLE_PROPERTY::TYPEID:
		((CIRCLE_PROPERTY*) this)->m_Property = ((CIRCLE_PROPERTY*) pSrcProp)->m_Property;
		break;
	case TARGETINFLUENCE_PROPERTY::TYPEID:
		((TARGETINFLUENCE_PROPERTY*) this)->m_Property = ((TARGETINFLUENCE_PROPERTY*) pSrcProp)->m_Property;
		break;
	};

	this->DeleteDeviceObjects ();
	this->InitDeviceObjects ( pd3dDevice, bBackUpCache );
}

HRESULT EFF_PROPERTY::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache )
{
	if ( m_pChild )		m_pChild->InitDeviceObjects ( pd3dDevice, bBackUpCache );
	if ( m_pSibling )	m_pSibling->InitDeviceObjects ( pd3dDevice, bBackUpCache );

	return S_OK;
}

//HRESULT EFF_PROPERTY::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
//{
//	if ( m_pChild )		m_pChild->RestoreDeviceObjects ( pd3dDevice );
//	if ( m_pSibling )	m_pSibling->RestoreDeviceObjects ( pd3dDevice );
//
//	return S_OK;
//}
//
//HRESULT EFF_PROPERTY::InvalidateDeviceObjects ()
//{
//	if ( m_pChild )		m_pChild->InvalidateDeviceObjects ();
//	if ( m_pSibling )	m_pSibling->InvalidateDeviceObjects ();
//
//	return S_OK;
//}

HRESULT EFF_PROPERTY::DeleteDeviceObjects ()
{
	if ( m_pChild )		m_pChild->DeleteDeviceObjects ();
	if ( m_pSibling )	m_pSibling->DeleteDeviceObjects ();

	return S_OK;
}
	
DWORD EFF_PROPERTY::GetSizeBase ()
{
	return ( sizeof(float)+sizeof(float)+sizeof(BOOL)+sizeof(D3DXMATRIX) );
}

void EFF_PROPERTY::importMobileObjectFromXFile()
{
	if ( m_pSibling )	m_pSibling->importMobileObjectFromXFile();
	if ( m_pChild )		m_pChild->importMobileObjectFromXFile();
}

HRESULT EFF_PROPERTY::LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache, BOOL bOldDecalUSE )
{
	HRESULT hr(S_OK);
	BOOL bValid = FALSE;

	WORD MovSoundVER;
	WORD SoundVER;

	SFile >> MovSoundVER;
	SFile >> SoundVER;

    m_MovSound.LoadSet ( SFile, MovSoundVER, SoundVER );

	//	Note : 형제 읽기.
	SFile >> bValid;
	if ( bValid )
	{
		DWORD dwTypeID;
		SFile >> dwTypeID;

		EFF_PROPERTY* pNewProp(NULL);

		// 문제되는 Decal 일 경우 Ground 로 변경해준다.
		if ( DECAL_PROPERTY::TYPEID == dwTypeID && !bOldDecalUSE )
		{
			pNewProp = EFF_PROPERTY::NEW_PROP ( GROUND_PROPERTY::TYPEID );
			GROUND_PROPERTY* pGROUND_PROPERTY = MIN_BOOST::polymorphic_downcast<GROUND_PROPERTY*>( pNewProp );
			pGROUND_PROPERTY->LoadFile_DecalProp ( SFile, pd3dDevice, bBackUpCache );
		}
		else
		{
			pNewProp = EFF_PROPERTY::NEW_PROP ( dwTypeID );
			GASSERT(pNewProp);

			pNewProp->LoadFile ( SFile, pd3dDevice, bBackUpCache, bOldDecalUSE );
		}

		m_pSibling = pNewProp;
	}

	//	Note : 자식 읽기.
	SFile >> bValid;
	if ( bValid )
	{
		DWORD dwTypeID;
		SFile >> dwTypeID;

		EFF_PROPERTY* pNewProp(NULL);

		// 문제되는 Decal 일 경우 Ground 로 변경해준다.
		if ( DECAL_PROPERTY::TYPEID == dwTypeID && !bOldDecalUSE )
		{
			pNewProp = EFF_PROPERTY::NEW_PROP ( GROUND_PROPERTY::TYPEID );
			GROUND_PROPERTY* pGROUND_PROPERTY = MIN_BOOST::polymorphic_downcast<GROUND_PROPERTY*>( pNewProp );
			pGROUND_PROPERTY->LoadFile_DecalProp ( SFile, pd3dDevice, bBackUpCache );
		}
		else
		{
			pNewProp = EFF_PROPERTY::NEW_PROP ( dwTypeID );
			GASSERT(pNewProp);

			pNewProp->LoadFile ( SFile, pd3dDevice, bBackUpCache, bOldDecalUSE );
		}

		m_pChild = pNewProp;
	}

	return S_OK;
}

HRESULT EFF_PROPERTY::SaveFile ( sc::SerialFile &SFile )
{
	SFile << m_MovSound.m_MovSoundVER;
	SFile << m_MovSound.m_SoundVER;

    m_MovSound.SaveSet ( SFile );

	//	Note : 형제 저장.
	if ( m_pSibling )	SFile << TRUE;
	else			SFile << FALSE;
	
	if ( m_pSibling )	m_pSibling->SaveFile ( SFile );

	//	Note : 자식 저장.
	if ( m_pChild )	SFile << TRUE;
	else			SFile << FALSE;
	
	if ( m_pChild )		m_pChild->SaveFile ( SFile );

	return S_OK;
}

//-----------------------------------------------------------------------------------------[EFF_PROPGROUP]
const DWORD	EFF_PROPGROUP::VERSION = 0x0106;

BOOL EFF_PROPGROUP::RESET_MOVEOBJ_PROP ( PEFF_PROPERTY &pPropHead, EFF_PROPERTY &pCurEffProp )
{
	PEFF_PROPERTY pPropCur = pPropHead;
	while ( pPropCur )
	{
		pPropCur->m_bMoveObj = FALSE;

		//	Note : 하위 자식 노드 탐색.
		//
		if ( pPropCur->m_pChild )
		{
			RESET_MOVEOBJ_PROP ( pPropCur->m_pChild, pCurEffProp );
		}

		pPropCur = pPropCur->m_pSibling;
	}

	pCurEffProp.m_bMoveObj = TRUE;

	return TRUE;
}

BOOL EFF_PROPGROUP::NEWEFFSINGLE ( PEFF_PROPERTY &pCurEffProp, PDXEFFSINGLE &pCurEffSingle, SPDXEFFSINGLEGROUP spThisGroup )
{
	BOOL bValid;
	GASSERT(pCurEffProp);

	//	Note : 현제 노드 생성.
	//
	pCurEffSingle = pCurEffProp->NEWOBJ( spThisGroup );
	if ( !pCurEffSingle )	return FALSE;

	//	Note : 생성될 그룹의 포인터 지정.
	//
	pCurEffSingle->m_rThisGroup = spThisGroup.get();

	//	Note : 형제, 자식 노드 순회.
	//
	if ( pCurEffProp->m_pSibling )
	{
		bValid = NEWEFFSINGLE ( pCurEffProp->m_pSibling, pCurEffSingle->m_pSibling, spThisGroup );
		if ( !bValid )	return bValid;
	}

	if ( pCurEffProp->m_pChild )
	{
		bValid = NEWEFFSINGLE ( pCurEffProp->m_pChild, pCurEffSingle->m_pChild, spThisGroup );
		if ( !bValid )	return bValid;
	}

	return TRUE;
}

BOOL EFF_PROPGROUP::NEWEFFSINGLE ( PEFF_PROPERTY &pCurEffProp, PDXEFFSINGLE &pCurEffSingle, SPDXEFFSINGLEGROUP spThisGroup, EFF_PROPERTY* pTargetProp )
{
	BOOL bValid;
	GASSERT(pCurEffProp);

	//	Note : 현제 노드 생성.
	//
	pCurEffSingle = pCurEffProp->NEWOBJ( spThisGroup );
	if ( !pCurEffSingle )	return FALSE;

	//	Note : 그려지지 않는 이펙트 표시.
	//
	if ( pTargetProp != pCurEffProp )
		pCurEffSingle->m_dwRunFlag |= EFF_PLY_RENDPASS;

	//	Note : 생성될 그룹의 포인터 지정.
	//
	pCurEffSingle->m_rThisGroup = spThisGroup.get();

	//	Note : 형제, 자식 노드 순회.
	//
	if ( pCurEffProp->m_pSibling )
	{
		bValid = NEWEFFSINGLE ( pCurEffProp->m_pSibling, pCurEffSingle->m_pSibling, spThisGroup, pTargetProp );
		if ( !bValid )	return bValid;
	}

	if ( pCurEffProp->m_pChild )
	{
		bValid = NEWEFFSINGLE ( pCurEffProp->m_pChild, pCurEffSingle->m_pChild, spThisGroup, pTargetProp );
		if ( !bValid )	return bValid;
	}

	return TRUE;
}

SPDXEFFSINGLEGROUP EFF_PROPGROUP::NEWEFFGROUP ()
{
	if ( !m_pPropRoot )
        return SPDXEFFSINGLEGROUP();

	SPDXEFFSINGLEGROUP spNewEffGroup = SPDXEFFSINGLEGROUP( new DxEffSingleGroup );
	spNewEffGroup->m_strFileName = m_strFileName;

	//	Note : Group 설정.
	//
	spNewEffGroup->m_dwFlag = m_dwFlag;
	spNewEffGroup->m_pMax = &m_vMax;
	spNewEffGroup->m_pMin = &m_vMin;
	spNewEffGroup->m_vLocal = m_vLocal;

	//	Note : Single 개체 생성.
	//
	BOOL bValid = NEWEFFSINGLE ( m_pPropRoot, spNewEffGroup->m_pEffSingleRoot, spNewEffGroup );

	if ( !bValid )
        return SPDXEFFSINGLEGROUP();

	return spNewEffGroup;
}

SPDXEFFSINGLEGROUP EFF_PROPGROUP::NEWEFFGROUP ( EFF_PROPERTY* pTargetProp )
{
	if ( !m_pPropRoot )
        return SPDXEFFSINGLEGROUP();

	SPDXEFFSINGLEGROUP spNewEffGroup = SPDXEFFSINGLEGROUP( new DxEffSingleGroup );
	spNewEffGroup->m_strFileName = m_strFileName;

	//	Note : 설정.
	spNewEffGroup->m_pMax = &m_vMax;
	spNewEffGroup->m_pMin = &m_vMin;

	BOOL bValid = NEWEFFSINGLE ( m_pPropRoot, spNewEffGroup->m_pEffSingleRoot, spNewEffGroup, pTargetProp );

	if ( !bValid )
        return SPDXEFFSINGLEGROUP();

	return spNewEffGroup;
}

SPDXEFFSINGLEGROUP EFF_PROPGROUP::NEWEFFSINGLE ( EFF_PROPERTY* pTargetProp )
{
	if ( !pTargetProp )
        return SPDXEFFSINGLEGROUP();

	SPDXEFFSINGLEGROUP spNewEffGroup = SPDXEFFSINGLEGROUP( new DxEffSingleGroup );
	spNewEffGroup->m_strFileName = "_singletest_";

	//	Note : 설정.
	spNewEffGroup->m_pMax = &m_vMax;
	spNewEffGroup->m_pMin = &m_vMin;

	//	Note : 현제 노드 생성.
	spNewEffGroup->m_pEffSingleRoot = pTargetProp->NEWOBJ( spNewEffGroup );
	if ( !spNewEffGroup->m_pEffSingleRoot )
        return SPDXEFFSINGLEGROUP();

	//	Note : 생성될 그룹의 포인터 지정.
	//
	spNewEffGroup->m_pEffSingleRoot->m_rThisGroup = spNewEffGroup.get();

	return spNewEffGroup;
}

HRESULT EFF_PROPGROUP::DeleteProp ( PEFF_PROPERTY &pPropHead, PEFF_PROPERTY pProp )
{
	HRESULT hr = S_OK;

	PEFF_PROPERTY pPropDel = NULL;
	if ( pPropHead == pProp )
	{
		pPropDel = pPropHead;
		pPropHead = pPropHead->m_pSibling;

		pPropDel->m_pSibling = NULL;
		SAFE_DELETE(pPropDel);

		return S_OK;
	}

	PEFF_PROPERTY pPropCur = pPropHead, pPropPrev = pPropHead;
	while ( pPropCur )
	{
		if ( pPropCur == pProp )
		{
			pPropPrev->m_pSibling = pPropCur->m_pSibling;

			pPropCur->m_pSibling = NULL;
			SAFE_DELETE(pPropCur);
			return S_OK;
		}

		//	Note : 하위 자식 노드 탐색.
		//
		if ( pPropCur->m_pChild )
		{
			hr = DeleteProp ( pPropCur->m_pChild, pProp );
			if ( hr == S_OK )	return hr;
		}

		pPropPrev = pPropCur;
		pPropCur = pPropCur->m_pSibling;
	}

	return S_FALSE;
}

HRESULT EFF_PROPGROUP::GetMatCombParent ( PEFF_PROPERTY &pPropHead, PEFF_PROPERTY pProp, D3DXMATRIX &matParent, D3DXMATRIX &matComb )
{
	HRESULT hr = S_OK;

	PEFF_PROPERTY pPropCur = pPropHead;
	while ( pPropCur )
	{
		if ( pPropCur == pProp )
		{
			matComb = matParent;
			return S_OK;
		}

		//	Note : 하위 자식 노드 탐색.
		//
		if ( pPropCur->m_pChild )
		{
			//	Note : 메트릭스를 계산. matCombined = matRot * matParent
			D3DXMATRIX matThisComb;
			D3DXMatrixMultiply ( &matThisComb, &pPropCur->m_matLocal, &matParent );
			hr = GetMatCombParent ( pPropCur->m_pChild, pProp, matThisComb, matComb );
			if ( hr == S_OK )	return hr;
		}

		pPropCur = pPropCur->m_pSibling;
	}

	return S_FALSE;
}

BOOL EFF_PROPGROUP::IsFlags( EFFECTFLAG emFlag )
{
	return m_dwFlag & emFlag;
}

void EFF_PROPGROUP::importMobileObjectFromXFile()
{
	if ( m_pPropRoot )
		m_pPropRoot->importMobileObjectFromXFile();
}

HRESULT EFF_PROPGROUP::LoadFile_0100 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache, BOOL bOldDecalUSE )
{
	HRESULT hr = S_OK;

	SFile >> m_dwFlag;
	SFile >> m_vMax;
	SFile >> m_vMin;

	m_vLocal = D3DXVECTOR3 ( 0.f, 0.f, 0.f );

	BOOL bNeverDie	= FALSE;
	BOOL bTargetPOS	= FALSE;
	BOOL bTargetID	= FALSE;
	BOOL bNoneMove	= FALSE;
	BOOL bNone		= FALSE;
	BOOL bEvent		= FALSE;

	if ( m_dwFlag & 0x10000 )		bEvent = TRUE;
	if ( m_dwFlag & 0x01000 )		bNeverDie = TRUE;
	else if ( m_dwFlag & 0x02000 )	bTargetPOS = TRUE;
	else if ( m_dwFlag & 0x04000 )	bTargetID = TRUE;
	else if ( m_dwFlag & 0x08000 )	bNoneMove = TRUE;
	else							bNone = TRUE;

	m_dwFlag = 0;

	if ( bNeverDie )	m_dwFlag |= (EFF_CFG_NEVERDIE | EFF_CFG_NONE);
	if ( bTargetPOS )	m_dwFlag |= EFF_CFG_TARGETPOS;
	if ( bTargetID )	m_dwFlag |= EFF_CFG_TARGETID;
	if ( bNoneMove )	m_dwFlag |= EFF_CFG_MOVE;
	if ( bNone )		m_dwFlag |= EFF_CFG_NONE;
	if ( bEvent )		m_dwFlag |= EFF_CFG_EVENT;

	m_dwFlag |= EFF_CFG_AVERAGE;	// 데이터 추가
	m_dwFlag |= EFF_CFG_AUTOAABBBOX;	// 데이터 추가

	//	Note : EFF_PROP
	//
	BOOL bValid;
	SFile >> bValid;
	if ( bValid )
	{
		DWORD dwTypeID;
		SFile >> dwTypeID;

		//EFF_PROPERTY* pNewProp = EFF_PROPERTY::NEW_PROP ( dwTypeID );
		//GASSERT(pNewProp);

		//hr = pNewProp->LoadFile ( SFile, pd3dDevice, bBackUpCache );
		//if ( FAILED(hr) )
		//{
		//	SAFE_DELETE(pNewProp);
		//	return hr;
		//}

		EFF_PROPERTY* pNewProp(NULL);

		// 문제되는 Decal 일 경우 Ground 로 변경해준다.
		if ( DECAL_PROPERTY::TYPEID == dwTypeID && !bOldDecalUSE )
		{
			pNewProp = EFF_PROPERTY::NEW_PROP ( GROUND_PROPERTY::TYPEID );
			GROUND_PROPERTY* pGROUND_PROPERTY = MIN_BOOST::polymorphic_downcast<GROUND_PROPERTY*>( pNewProp );
			hr = pGROUND_PROPERTY->LoadFile_DecalProp ( SFile, pd3dDevice, bBackUpCache );
			if ( FAILED(hr) )
			{
				SAFE_DELETE(pNewProp);
				return hr;
			}
		}
		else
		{
			pNewProp = EFF_PROPERTY::NEW_PROP ( dwTypeID );
			GASSERT(pNewProp);

			hr = pNewProp->LoadFile ( SFile, pd3dDevice, bBackUpCache, bOldDecalUSE );
			if ( FAILED(hr) )
			{
				SAFE_DELETE(pNewProp);
				return hr;
			}
		}

		m_pPropRoot = pNewProp;
	}

	return S_OK;
}

HRESULT EFF_PROPGROUP::LoadFile_0101 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache, BOOL bOldDecalUSE )
{
	HRESULT hr = S_OK;

	SFile >> m_dwFlag;
	SFile >> m_vMax;
	SFile >> m_vMin;

	m_vLocal = D3DXVECTOR3 ( 0.f, 0.f, 0.f );

	BOOL bNoneMove	= FALSE;

	if ( m_dwFlag & 0x08000 )
	{
		m_dwFlag |= EFF_CFG_TARGETID;
		m_dwFlag &= ~EFF_CFG_MOVE;
	}

	m_dwFlag |= EFF_CFG_AVERAGE;	// 데이터 추가
	m_dwFlag |= EFF_CFG_AUTOAABBBOX;	// 데이터 추가

	//	Note : EFF_PROP
	//
	BOOL bValid;
	SFile >> bValid;
	if ( bValid )
	{
		DWORD dwTypeID;
		SFile >> dwTypeID;

		//EFF_PROPERTY* pNewProp = EFF_PROPERTY::NEW_PROP ( dwTypeID );
		//GASSERT(pNewProp);

		//hr = pNewProp->LoadFile ( SFile, pd3dDevice, bBackUpCache );
		//if ( FAILED(hr) )
		//{
		//	SAFE_DELETE(pNewProp);
		//	return hr;
		//}

		EFF_PROPERTY* pNewProp(NULL);

		// 문제되는 Decal 일 경우 Ground 로 변경해준다.
		if ( DECAL_PROPERTY::TYPEID == dwTypeID && !bOldDecalUSE )
		{
			pNewProp = EFF_PROPERTY::NEW_PROP ( GROUND_PROPERTY::TYPEID );
			GROUND_PROPERTY* pGROUND_PROPERTY = MIN_BOOST::polymorphic_downcast<GROUND_PROPERTY*>( pNewProp );
			hr = pGROUND_PROPERTY->LoadFile_DecalProp ( SFile, pd3dDevice, bBackUpCache );
			if ( FAILED(hr) )
			{
				SAFE_DELETE(pNewProp);
				return hr;
			}
		}
		else
		{
			pNewProp = EFF_PROPERTY::NEW_PROP ( dwTypeID );
			GASSERT(pNewProp);

			hr = pNewProp->LoadFile ( SFile, pd3dDevice, bBackUpCache, bOldDecalUSE );
			if ( FAILED(hr) )
			{
				SAFE_DELETE(pNewProp);
				return hr;
			}
		}

		m_pPropRoot = pNewProp;
	}

	return S_OK;
}

HRESULT EFF_PROPGROUP::LoadFile_0102 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache, BOOL bOldDecalUSE )
{
	HRESULT hr = S_OK;

	SFile >> m_dwFlag;
	SFile >> m_vMax;
	SFile >> m_vMin;

	SFile >> m_vLocal;

	if ( m_dwFlag == 0L )	m_dwFlag |= EFF_CFG_NONE;

	m_dwFlag |= EFF_CFG_AVERAGE;	// 데이터 추가
	m_dwFlag |= EFF_CFG_AUTOAABBBOX;	// 데이터 추가	

	//	Note : EFF_PROP
	//
	BOOL bValid;
	SFile >> bValid;
	if ( bValid )
	{
		DWORD dwTypeID;
		SFile >> dwTypeID;

		//EFF_PROPERTY* pNewProp = EFF_PROPERTY::NEW_PROP ( dwTypeID );
		//GASSERT(pNewProp);

		//hr = pNewProp->LoadFile ( SFile, pd3dDevice, bBackUpCache );
		//if ( FAILED(hr) )
		//{
		//	SAFE_DELETE(pNewProp);
		//	return hr;
		//}

		EFF_PROPERTY* pNewProp(NULL);

		// 문제되는 Decal 일 경우 Ground 로 변경해준다.
		if ( DECAL_PROPERTY::TYPEID == dwTypeID && !bOldDecalUSE )
		{
			pNewProp = EFF_PROPERTY::NEW_PROP ( GROUND_PROPERTY::TYPEID );
			GROUND_PROPERTY* pGROUND_PROPERTY = MIN_BOOST::polymorphic_downcast<GROUND_PROPERTY*>( pNewProp );
			hr = pGROUND_PROPERTY->LoadFile_DecalProp ( SFile, pd3dDevice, bBackUpCache );
			if ( FAILED(hr) )
			{
				SAFE_DELETE(pNewProp);
				return hr;
			}
		}
		else
		{
			pNewProp = EFF_PROPERTY::NEW_PROP ( dwTypeID );
			GASSERT(pNewProp);

			hr = pNewProp->LoadFile ( SFile, pd3dDevice, bBackUpCache, bOldDecalUSE );
			if ( FAILED(hr) )
			{
				SAFE_DELETE(pNewProp);
				return hr;
			}
		}

		m_pPropRoot = pNewProp;
	}

	return S_OK;
}

HRESULT EFF_PROPGROUP::LoadFile_0103 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache, BOOL bOldDecalUSE )
{
	HRESULT hr = S_OK;

	SFile >> m_dwFlag;
	SFile >> m_vMax;
	SFile >> m_vMin;

	SFile >> m_vLocal;

	m_dwFlag |= EFF_CFG_AUTOAABBBOX;	// 데이터 추가

	//	Note : EFF_PROP
	//
	BOOL bValid;
	SFile >> bValid;
	if ( bValid )
	{
		DWORD dwTypeID;
		SFile >> dwTypeID;

		//EFF_PROPERTY* pNewProp = EFF_PROPERTY::NEW_PROP ( dwTypeID );
		//GASSERT(pNewProp);

		//hr = pNewProp->LoadFile ( SFile, pd3dDevice, bBackUpCache );
		//if ( FAILED(hr) )
		//{
		//	SAFE_DELETE(pNewProp);
		//	return hr;
		//}

		EFF_PROPERTY* pNewProp(NULL);

		// 문제되는 Decal 일 경우 Ground 로 변경해준다.
		if ( DECAL_PROPERTY::TYPEID == dwTypeID && !bOldDecalUSE )
		{
			pNewProp = EFF_PROPERTY::NEW_PROP ( GROUND_PROPERTY::TYPEID );
			GROUND_PROPERTY* pGROUND_PROPERTY = MIN_BOOST::polymorphic_downcast<GROUND_PROPERTY*>( pNewProp );
			hr = pGROUND_PROPERTY->LoadFile_DecalProp ( SFile, pd3dDevice, bBackUpCache );
			if ( FAILED(hr) )
			{
				SAFE_DELETE(pNewProp);
				return hr;
			}
		}
		else
		{
			pNewProp = EFF_PROPERTY::NEW_PROP ( dwTypeID );
			GASSERT(pNewProp);

			hr = pNewProp->LoadFile ( SFile, pd3dDevice, bBackUpCache, bOldDecalUSE );
			if ( FAILED(hr) )
			{
				SAFE_DELETE(pNewProp);
				return hr;
			}
		}

		m_pPropRoot = pNewProp;
	}

	return S_OK;
}

HRESULT EFF_PROPGROUP::LoadFile_0104 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache, BOOL bOldDecalUSE )
{
	HRESULT hr = S_OK;

	SFile >> m_dwFlag;
	SFile >> m_vMax;
	SFile >> m_vMin;

	SFile >> m_vLocal;

	m_dwFlag |= EFF_CFG_AUTOAABBBOX;	// 데이터 추가

	//	Note : EFF_PROP
	//
	BOOL bValid;
	SFile >> bValid;
	if ( bValid )
	{
		DWORD dwTypeID;
		SFile >> dwTypeID;

		//EFF_PROPERTY* pNewProp = EFF_PROPERTY::NEW_PROP ( dwTypeID );
		//GASSERT(pNewProp);

		//hr = pNewProp->LoadFile ( SFile, pd3dDevice, bBackUpCache );
		//if ( FAILED(hr) )
		//{
		//	SAFE_DELETE(pNewProp);
		//	return hr;
		//}

		EFF_PROPERTY* pNewProp(NULL);

		// 문제되는 Decal 일 경우 Ground 로 변경해준다.
		if ( DECAL_PROPERTY::TYPEID == dwTypeID && !bOldDecalUSE )
		{
			pNewProp = EFF_PROPERTY::NEW_PROP ( GROUND_PROPERTY::TYPEID );
			GROUND_PROPERTY* pGROUND_PROPERTY = MIN_BOOST::polymorphic_downcast<GROUND_PROPERTY*>( pNewProp );
			hr = pGROUND_PROPERTY->LoadFile_DecalProp ( SFile, pd3dDevice, bBackUpCache );
			if ( FAILED(hr) )
			{
				SAFE_DELETE(pNewProp);
				return hr;
			}
		}
		else
		{
			pNewProp = EFF_PROPERTY::NEW_PROP ( dwTypeID );
			GASSERT(pNewProp);

			hr = pNewProp->LoadFile ( SFile, pd3dDevice, bBackUpCache, bOldDecalUSE );
			if ( FAILED(hr) )
			{
				SAFE_DELETE(pNewProp);
				return hr;
			}
		}

		m_pPropRoot = pNewProp;
	}

	return S_OK;
}

HRESULT EFF_PROPGROUP::LoadFile_0105 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache, BOOL bOldDecalUSE )
{
	HRESULT hr = S_OK;

	SFile >> m_dwFlag;
	SFile >> m_vMax;
	SFile >> m_vMin;

	SFile >> m_vLocal;

	//	Note : EFF_PROP
	//
	BOOL bValid;
	SFile >> bValid;
	if ( bValid )
	{
		DWORD dwTypeID;
		SFile >> dwTypeID;

		//EFF_PROPERTY* pNewProp = EFF_PROPERTY::NEW_PROP ( dwTypeID );
		//GASSERT(pNewProp);

		//hr = pNewProp->LoadFile ( SFile, pd3dDevice, bBackUpCache );
		//if ( FAILED(hr) )
		//{
		//	SAFE_DELETE(pNewProp);
		//	return hr;
		//}

		EFF_PROPERTY* pNewProp(NULL);

		// 문제되는 Decal 일 경우 Ground 로 변경해준다.
		if ( DECAL_PROPERTY::TYPEID == dwTypeID && !bOldDecalUSE )
		{
			pNewProp = EFF_PROPERTY::NEW_PROP ( GROUND_PROPERTY::TYPEID );
			GROUND_PROPERTY* pGROUND_PROPERTY = MIN_BOOST::polymorphic_downcast<GROUND_PROPERTY*>( pNewProp );
			hr = pGROUND_PROPERTY->LoadFile_DecalProp ( SFile, pd3dDevice, bBackUpCache );
			if ( FAILED(hr) )
			{
				SAFE_DELETE(pNewProp);
				return hr;
			}
		}
		else
		{
			pNewProp = EFF_PROPERTY::NEW_PROP ( dwTypeID );
			GASSERT(pNewProp);

			hr = pNewProp->LoadFile ( SFile, pd3dDevice, bBackUpCache, bOldDecalUSE );
			if ( FAILED(hr) )
			{
				SAFE_DELETE(pNewProp);
				return hr;
			}
		}

		m_pPropRoot = pNewProp;
	}

	return S_OK;
}

HRESULT EFF_PROPGROUP::LoadFile_0106 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache, BOOL bOldDecalUSE )
{
	HRESULT hr = S_OK;

	SFile >> m_dwFlag;
	SFile >> m_vLocal;
	
	SFile >> m_vMin;
	SFile >> m_vMax;

	//	Note : EFF_PROP
	//
	BOOL bValid;
	SFile >> bValid;
	if ( bValid )
	{
		DWORD dwTypeID;
		SFile >> dwTypeID;

		//EFF_PROPERTY* pNewProp = EFF_PROPERTY::NEW_PROP ( dwTypeID );
		//GASSERT(pNewProp);

		//hr = pNewProp->LoadFile ( SFile, pd3dDevice, bBackUpCache );
		//if ( FAILED(hr) )
		//{
		//	SAFE_DELETE(pNewProp);
		//	return hr;
		//}

		EFF_PROPERTY* pNewProp(NULL);

		// 문제되는 Decal 일 경우 Ground 로 변경해준다.
		if ( DECAL_PROPERTY::TYPEID == dwTypeID && !bOldDecalUSE )
		{
			pNewProp = EFF_PROPERTY::NEW_PROP ( GROUND_PROPERTY::TYPEID );
			GROUND_PROPERTY* pGROUND_PROPERTY = MIN_BOOST::polymorphic_downcast<GROUND_PROPERTY*>( pNewProp );
			hr = pGROUND_PROPERTY->LoadFile_DecalProp ( SFile, pd3dDevice, bBackUpCache );
			if ( FAILED(hr) )
			{
				SAFE_DELETE(pNewProp);
				return hr;
			}
		}
		else
		{
			pNewProp = EFF_PROPERTY::NEW_PROP ( dwTypeID );
			GASSERT(pNewProp);

			hr = pNewProp->LoadFile ( SFile, pd3dDevice, bBackUpCache, bOldDecalUSE );
			if ( FAILED(hr) )
			{
				SAFE_DELETE(pNewProp);
				return hr;
			}
		}

		m_pPropRoot = pNewProp;
	}

	return S_OK;
}

BOOL EFF_PROPGROUP::LoadFile( const std::string& FileName, void* pBuf,UINT bufSize, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache )
{
	se::ByteStream stream( (LPBYTE)pBuf,bufSize);
	stream.ReadFileType();

	m_strFileName = FileName;
	sc::string::lowerString(m_strFileName);	

	m_bBackUpCache = bBackUpCache;

	BOOL bOldDecalUSE(FALSE);
	std::string::size_type idx = m_strFileName.find("head_ligh");
	if ( idx != std::string::npos )
		bOldDecalUSE = TRUE;

	if ( S_OK == LoadFile(stream,pd3dDevice,bOldDecalUSE) )
		return TRUE;
	else
		return FALSE;
}

HRESULT EFF_PROPGROUP::LoadFile(const std::string& FileName, LPDIRECT3DDEVICEQ pd3dDevice, boost::function<void (const TCHAR*)> pArchive)
{	
	if (FileName.empty())
        return S_FALSE;
	
	m_strFileName = FileName;
    sc::string::lowerString(m_strFileName);	

	HRESULT hr = S_OK;

    std::string FullPath(DxEffSinglePropGMan::GetInstance().GetPath().c_str());
	FullPath.append(m_strFileName);

    /* 아카이브 기능 추가로 변경 된 코드, 향후 안정화되면 삭제 예정
	std::auto_ptr<sc::BaseStream> pBStream(
        GLOGIC::openfile_basestream(
            GLOGIC::UseEngineZipFile(), 
            GLOGIC::strEFFECT_ZIPFILE,
            FullPath.c_str(), 
            FileName));

	if (!pBStream.get())
	{
		if (!FileName.empty() && std::string("null") != FileName)
        {
            std::string ErrorMsg(
                sc::string::format(
                    "Effect File Load Fail : %1%", FileName));
			CDebugSet::ToLogFile(ErrorMsg);
        }
		return E_FAIL;
	}

	LoadFile(*pBStream,pd3dDevice);
    */

    SPBaseStream spFile;

    if (Archive::CreateFileBaseStream(FullPath.c_str(), FileName.c_str(), GLOGIC::strEFFECT_ZIPFILE.c_str(), pArchive, spFile) == false)
    {
        if (!FileName.empty() && std::string("null") != FileName)
        {
            std::string ErrorMsg(
                sc::string::format(
                "Effect File Load Fail : %1%", FileName));
            CDebugSet::ToLogFile(ErrorMsg);
        }

        return E_FAIL;
    }

    sc::BaseStream& SFile = *spFile;

	BOOL bOldDecalUSE(FALSE);
	std::string::size_type idx = m_strFileName.find("head_ligh");
	if ( idx != std::string::npos )
		bOldDecalUSE = TRUE;

    LoadFile(SFile, pd3dDevice, bOldDecalUSE);

	if ( g_bImportMobile )
	{
		importMobileObjectFromXFile();
	}

	return hr;
}

HRESULT EFF_PROPGROUP::LoadFile( sc::BaseStream& stream, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bOldDecalUSE )
{
	DWORD dwFileVer;
	char szFileType[FILETYPESIZE] = "";
	stream.GetFileType( szFileType, FILETYPESIZE, dwFileVer );
	if ( dwFileVer > EFF_PROPGROUP::FILEVERSION )
	{
		MessageBox ( NULL, "현제 App의 버전보다 상위에서 편집된 파일입니다.\r\n상위버전을 이용하세요.", "ERROR", MB_OK );
		return E_FAIL;
	}

	//	Note : EFF_PROPGROUP
	//
	DWORD dwVer;
	stream >> dwVer;

	HRESULT hr(S_OK);
	switch ( dwVer )
	{
	case 0x0100:	hr = LoadFile_0100( stream, pd3dDevice, m_bBackUpCache, bOldDecalUSE );	break;
	case 0x0101:	hr = LoadFile_0101( stream, pd3dDevice, m_bBackUpCache, bOldDecalUSE );	break;
	case 0x0102:	hr = LoadFile_0102( stream, pd3dDevice, m_bBackUpCache, bOldDecalUSE );	break;
	case 0x0103:	hr = LoadFile_0103( stream, pd3dDevice, m_bBackUpCache, bOldDecalUSE );	break;
	case 0x0104:	hr = LoadFile_0104( stream, pd3dDevice, m_bBackUpCache, bOldDecalUSE );	break;
	case 0x0105:	hr = LoadFile_0105( stream, pd3dDevice, m_bBackUpCache, bOldDecalUSE );	break;
	case VERSION:	hr = LoadFile_0106( stream, pd3dDevice, m_bBackUpCache, bOldDecalUSE );	break;
	default:
		{
			return E_FAIL;
		}
		break;
	};

	// cache를 할 필요가 없어서 강제로 셋팅 해준다.
	stream.CloseFile();

	return S_OK;
}

HRESULT EFF_PROPGROUP::SaveFile ( const char* szFileName, boost::function<void (const TCHAR*)> pArchive )
{
	SPSerialFile spFile;

	if (Archive::CreateSerialFile(pArchive, spFile) == false)
	{
		return FALSE;
	}

	sc::SerialFile& SFile = *spFile;

	GASSERT(szFileName);

	m_strFileName = szFileName;
	std::transform ( m_strFileName.begin(), m_strFileName.end(), m_strFileName.begin(), tolower );

	//sc::SerialFile SFile;
	SFile.SetFileType ( "EFF_PROPGROUP", EFF_PROPGROUP::FILEVERSION );

	if ( SFile.OpenFile(FOT_WRITE,m_strFileName.c_str()) )
	{
		//	Note : EFF_PROPGROUP
		//
		SFile << EFF_PROPGROUP::VERSION;
		SFile << m_dwFlag;
		SFile << m_vLocal;
		SFile << m_vMin;
		SFile << m_vMax;

		//	Note : EFF_PROP
		//
		if ( m_pPropRoot )	SFile << TRUE;
		else				SFile << FALSE;

		if ( m_pPropRoot )	m_pPropRoot->SaveFile ( SFile );
	}

	return S_OK;
}

HRESULT EFF_PROPGROUP::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_pPropRoot )	m_pPropRoot->InitDeviceObjects ( pd3dDevice, m_bBackUpCache );

	return S_OK;
}

//HRESULT EFF_PROPGROUP::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
//{
//	if ( m_pPropRoot )	m_pPropRoot->RestoreDeviceObjects ( pd3dDevice );
//
//	return S_OK;
//}
//
//HRESULT EFF_PROPGROUP::InvalidateDeviceObjects ()
//{
//	if ( m_pPropRoot )	m_pPropRoot->InvalidateDeviceObjects ();
//
//	return S_OK;
//}

HRESULT EFF_PROPGROUP::DeleteDeviceObjects ()
{
	if ( m_pPropRoot )	m_pPropRoot->DeleteDeviceObjects ();

	SAFE_DELETE(m_pPropRoot);

	return S_OK;
}

//
////-------------------------------------------------------------------------------------[DxEffSinglePropGMan]
//
//DxEffSinglePropGMan& DxEffSinglePropGMan::GetInstance()
//{
//	static DxEffSinglePropGMan Instance;
//	return Instance;
//}
//
//DxEffSinglePropGMan::DxEffSinglePropGMan(void) :
//	m_pd3dDevice(NULL)
//{
//	memset( m_szPath, 0, sizeof(char)*MAX_PATH );
//}
//
//DxEffSinglePropGMan::~DxEffSinglePropGMan(void)
//{
//}
//
//EFF_PROPGROUP* DxEffSinglePropGMan::FindEffGProp ( const char* szFileName )
//{
//	GASSERT(szFileName);
//
//	std::string strFindFile = szFileName;
//	std::transform ( strFindFile.begin(), strFindFile.end(), strFindFile.begin(), tolower );
//
//	MAPPROPGROUP_ITER pos = m_mapPROPGROUP.find ( strFindFile );
//	if ( pos == m_mapPROPGROUP.end() )	return NULL;
//
//	return (*pos).second;
//}
//
//HRESULT DxEffSinglePropGMan::DeleteEffectGProp ( const char* szFileName )
//{
//	GASSERT(szFileName);
//
//	std::string strFindFile = szFileName;
//	std::transform ( strFindFile.begin(), strFindFile.end(), strFindFile.begin(), tolower );
//
//	MAPPROPGROUP_ITER pos = m_mapPROPGROUP.find ( strFindFile );
//	if ( pos == m_mapPROPGROUP.end() )	return NULL;
//
//	SAFE_DELETE((*pos).second);
//	m_mapPROPGROUP.erase ( pos );
//
//	return E_FAIL;
//}
//
//EFF_PROPGROUP* DxEffSinglePropGMan::LoadEffectGProp ( const char* szFileName )
//{
//	HRESULT hr;
//
//	GASSERT(szFileName);
//	if( !m_pd3dDevice )
//	{
//        CDebugSet::ToLogFile(std::string("LoadEffectGProp - m_pd3dDevice == NULL"));
//		return NULL;
//	}
//
//	std::string strFindFile = szFileName;
//	std::transform ( strFindFile.begin(), strFindFile.end(), strFindFile.begin(), tolower );
//
//	EFF_PROPGROUP* pPropGroup;
//
//	pPropGroup = FindEffGProp ( strFindFile.c_str() );
//	if ( pPropGroup )	return pPropGroup;
//
//
//	pPropGroup = new EFF_PROPGROUP;
//	hr = pPropGroup->LoadFile ( strFindFile.c_str(), m_pd3dDevice );
//	if ( FAILED(hr) )
//	{
//		SAFE_DELETE(pPropGroup);
//		return NULL;
//	}
//
//	//	Note : 로드한 이팩트 Prop Group을 등록.
//	//
//	m_mapPROPGROUP.insert ( std::make_pair(strFindFile,pPropGroup) );
//
//	return pPropGroup;
//}
//
//HRESULT DxEffSinglePropGMan::OneTimeSceneInit ( const char *szPath )
//{
//	SetPath ( szPath );
//
//	return S_OK;
//}
//
//HRESULT DxEffSinglePropGMan::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
//{
//	m_pd3dDevice = pd3dDevice;
//
//	MAPPROPGROUP_ITER pos = m_mapPROPGROUP.begin();
//	MAPPROPGROUP_ITER end = m_mapPROPGROUP.end();
//	for ( ; pos!=end; ++pos )
//	{
//		(*pos).second->InitDeviceObjects ( m_pd3dDevice );
//	}
//
//	return S_OK;
//}
//
//HRESULT DxEffSinglePropGMan::RestoreDeviceObjects ()
//{
//	MAPPROPGROUP_ITER pos = m_mapPROPGROUP.begin();
//	MAPPROPGROUP_ITER end = m_mapPROPGROUP.end();
//	for ( ; pos!=end; ++pos )
//	{
//		(*pos).second->RestoreDeviceObjects ( m_pd3dDevice );
//	}
//
//	return S_OK;
//}
//
//HRESULT DxEffSinglePropGMan::InvalidateDeviceObjects ()
//{
//	MAPPROPGROUP_ITER pos = m_mapPROPGROUP.begin();
//	MAPPROPGROUP_ITER end = m_mapPROPGROUP.end();
//	for ( ; pos!=end; ++pos )
//	{
//		(*pos).second->InvalidateDeviceObjects ();
//	}
//
//	return S_OK;
//}
//
//HRESULT DxEffSinglePropGMan::DeleteDeviceObjects ()
//{
//	MAPPROPGROUP_ITER pos = m_mapPROPGROUP.begin();
//	MAPPROPGROUP_ITER end = m_mapPROPGROUP.end();
//	for ( ; pos!=end; ++pos )
//	{
//		(*pos).second->DeleteDeviceObjects ();
//		SAFE_DELETE ( (*pos).second );
//	}
//
//	m_mapPROPGROUP.clear ();
//
//	return S_OK;
//}


//-------------------------------------------------------------------------------------[DxEffSinglePropGMan]

DxEffSinglePropGMan& DxEffSinglePropGMan::GetInstance()
{
	static DxEffSinglePropGMan s; return s;
}

DxEffSinglePropGMan::DxEffSinglePropGMan()
{
	m_pd3dDevice = NULL;

	m_deleteCacheTime = 1000 * 60 * 60 * 24;	// 하루 정도 안보면 지워진다. <맵 이동을 하지 않는 이상, 안 지워진다라고 생각하면 된다.>
}

HRESULT DxEffSinglePropGMan::CleanUp ()
{
	InvalidateDeviceObjects();
	DeleteDeviceObjects();

	return S_OK;
}

HRESULT DxEffSinglePropGMan::OneTimeSceneInit ( const char *szPath )
{
	SetPath ( szPath );

	return S_OK;
}

HRESULT DxEffSinglePropGMan::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	COMPLTED_DATA_MAP::iterator iter = m_completedDataMap.begin();
	for ( ; iter!=m_completedDataMap.end(); ++iter )
	{
		(*iter).second.m_spData->InitDeviceObjects ( m_pd3dDevice );
	}
	return S_OK;
}

HRESULT DxEffSinglePropGMan::RestoreDeviceObjects ()
{
	//COMPLTED_DATA_MAP::iterator iter = m_completedDataMap.begin();
	//for ( ; iter!=m_completedDataMap.end(); ++iter )
	//{
	//	(*iter).second.m_spData->RestoreDeviceObjects ( m_pd3dDevice );
	//}
	return S_OK;
}

HRESULT DxEffSinglePropGMan::InvalidateDeviceObjects ()
{
	//COMPLTED_DATA_MAP::iterator iter = m_completedDataMap.begin();
	//for ( ; iter!=m_completedDataMap.end(); ++iter )
	//{
	//	(*iter).second.m_spData->InvalidateDeviceObjects ();
	//}
	return S_OK;
}

HRESULT DxEffSinglePropGMan::DeleteDeviceObjects ()
{
	COMPLTED_DATA_MAP::iterator iter = m_completedDataMap.begin();
	for ( ; iter!=m_completedDataMap.end(); ++iter )
	{
		(*iter).second.m_spData->DeleteDeviceObjects ();
	}

	DeleteCacheDataAll();

	return S_OK;
}

DxEffSinglePropGroupResource DxEffSinglePropGMan::LoadEffectGProp( const TSTRING& strName, BOOL bThread, BOOL bBackUpCache )
{
	if ( strName.size() <= 4 )
		return DxEffSinglePropGroupResource();

	EFF_PROPGROUP_ADD_DATA sAddData;
	sAddData.m_bBackUpCache = bBackUpCache;
	//Resource res = LoadData( GetPath() + strName, false, sAddData, bThread?true:false );
	Resource res = LoadData( strName, false, sAddData, bThread?true:false );

	return res;
}

void DxEffSinglePropGMan::ProcessCompleteData(
	SP_QUEUE_DATA_VECTOR& dataVec,
	NAME_N_SP_COMPLETED_DATA_VECTOR* pCompletedDataVec,
	DWORD dwPrevTick, 
	DWORD nWorkTime )
{
	SP_QUEUE_DATA_VECTOR::iterator it = dataVec.begin();
	while( it != dataVec.end() )
	{
		DWORD dwTick( timeGetTime() );

		const SP_QUEUE_DATA& spResData = *it;
		++it;

		MIN_ASSERT(!spResData->m_fileName.empty());
		MIN_ASSERT(spResData->m_spRawData != NULL);
		MIN_ASSERT(m_pd3dDevice);

		SP_COMPLETED_DATA spPropGroup(new EFF_PROPGROUP);
		spPropGroup->LoadFile( spResData->m_fileName, spResData->m_spRawData.get(), spResData->m_rawDataSize, m_pd3dDevice, spResData->m_addData.m_bBackUpCache );

		pCompletedDataVec->push_back( std::make_pair(spResData->m_fileName,spPropGroup) );

#ifdef _RELEASED
		if ( NSLoadingLagTester::g_DxEffSinglePropGMan_ProcessCompleteData < timeGetTime() - dwTick )
		{
			NSLoadingLagTester::g_DxEffSinglePropGMan_ProcessCompleteData = timeGetTime() - dwTick;
			NSLoadingLagTester::g_strDxEffSinglePropGMan_ProcessCompleteData = spResData->m_fileName;
		}
#endif

		//// Log에 남겨본다.
		//if ( dwTick > 10 )
		//	sc::writeLogInfo( sc::string::format("DxEffSinglePropGMan egp Load %1% - %2%", spResData->m_fileName, dwTick ) );

		if ( timeGetTime() - dwPrevTick > nWorkTime )
		{
			break;
		}
	}

	if( !dataVec.empty() )
		dataVec.erase( dataVec.begin(), it );
}

void DxEffSinglePropGMan::ProcessCompleteDataBackUpCache( NAME_N_SP_COMPLETED_DATA_VECTOR& CompletedDataVec )
{
	NAME_N_SP_COMPLETED_DATA_VECTOR::iterator iter = CompletedDataVec.begin();
	while( iter != CompletedDataVec.end() )
	{
		if ( (*iter).second->m_bBackUpCache )
		{
			BackUpCache( (*iter).first );
		}
		++iter;
	}
}

void DxEffSinglePropGMan::DeleteCacheDataAll()
{
	ResourceManager::DeleteCacheDataAll();
}

void DxEffSinglePropGMan::Archive( boost::function<void (const TCHAR*)> fun )
{
    BOOST_FOREACH(COMPLTED_DATA_MAP::value_type& it, m_completedDataMap)
    {
        fun( (m_path + it.first).c_str() );
    }

    BOOST_FOREACH(COMPLTED_DATA_MAP::value_type& it, m_backupCacheCompletedDataMap)
    {
        fun( (m_path + it.first).c_str() );
    }
}

