#include "pch.h"

#include "../../../SigmaCore/File/SerialFile.h"
#include "../../../SigmaCore/File/FileUtil.h"
#include "../../../SigmaCore/File/FileFind.h"

#include "DxSkinMeshManager.h"
#include "DxSkinDefine.h"

#include "../../G-Logic/GLogic.h"

#include "../../DxTools/DxInputDevice.h"
#include "../../G-Logic/GLPeriod.h"
#include "../../DxLand/DxLandMan.h"
#include "../../DxTools/Light/DxLightMan.h"
#include "../../DxEffect/Frame/DxEffectMan.h"
#include "../../DxTools/RENDERPARAM.h"
#include "../../DxTools/DxDynamicVB.h"
#include "../../Archive/Archive.h"

#include "../../DxTools/EDITMESHS.h"
#include "../../DxTools/COLLISION.h"

#include "./DxAttBoneLinkThreadNeedData.h"
#include "./DxSkeletonMaintain.h"

#include "dxattbonelink.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------




char DxAttBoneLink::m_szPath[MAX_PATH] = "";

void DxAttBoneLink::SetPath ( char* szPath )
{
	StringCchCopy( m_szPath, MAX_PATH, szPath );
}


DxAttBoneLink::DxAttBoneLink(void) 
		: m_dwRef(0)
		, m_bUseCPS(FALSE)
		, m_strAttBoneName("")
		, m_strPieceName("")
		, m_strBaseBoneName("")
		, m_emParentObjType(EMATTBONEPARRENTOBJTYPE_CHAR)
		, m_emPieceCharType(PIECE_HEAD)

		, m_bRender(TRUE)

		, m_nOldAttSTEP(-1)

{
	D3DXMatrixIdentity( &m_matWorld );

	m_spThreadNeedData = std::tr1::shared_ptr<DxAttBoneLinkThreadNeedData>(new DxAttBoneLinkThreadNeedData);
}

DxAttBoneLink::~DxAttBoneLink(void)
{
	ClearAll();
}


void DxAttBoneLink::ClearAll ()
{
	m_AttBone.ResetAttBone();

	m_spThreadNeedData->ClearAll ();

	m_nOldAttSTEP = -1;

	m_bUseCPS = FALSE;
	m_strAttBoneName.clear();
	m_strPieceName.clear();
	m_strBaseBoneName.clear();
}


void DxAttBoneLink::SetLink ( DxAttBoneLink* pAttBoneLink, LPDIRECT3DDEVICEQ pd3dDevice, bool bAmbientUP, int nLevel, const char* pSelfBodyEffect )
{
	StringCchCopy ( m_szFileName, FILE_LENGTH, pAttBoneLink->m_szFileName );

	m_strAttBoneName	= pAttBoneLink->m_strAttBoneName;
	m_strPieceName		= pAttBoneLink->m_strPieceName;
	m_strBaseBoneName	= pAttBoneLink->m_strBaseBoneName;

	m_bUseCPS			= pAttBoneLink->m_bUseCPS;

	m_emParentObjType   = pAttBoneLink->m_emParentObjType;
	m_emPieceCharType	= pAttBoneLink->m_emPieceCharType;

	m_nOldAttSTEP		= -1;

	m_spThreadNeedData->Import ( pAttBoneLink->m_spThreadNeedData.get() );

	SetABFFile( m_strAttBoneName.c_str(), pd3dDevice, bAmbientUP, nLevel, EMSCD_ALL_THREAD, pSelfBodyEffect );	
}


BOOL DxAttBoneLink::SetABFFile ( const char* szFile, LPDIRECT3DDEVICEQ pd3dDevice, bool bAmbientUP /*= FALSE*/, int nLevel /*=0*/, EMSCD_FLAGS emFlags, const char* pSelfBodyEffect )
{
	m_strAttBoneName = szFile;

	DxAttBoneData *pAttBoneData = DxAttBoneDataContainer::GetInstance().LoadAttBoneData(m_strAttBoneName, emFlags );
	if( !pAttBoneData ) return FALSE;

	if( m_AttBone.GetFileName() )
	{
		m_AttBone.ResetAttBone();
	}


	m_AttBone.SetAttBoneData( pAttBoneData, pd3dDevice, FALSE, nLevel, emFlags, pSelfBodyEffect );

	return TRUE;
}

BOOL DxAttBoneLink::LOAD_Ver100( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	SFile >> m_dwRef;
	SFile >> m_strAttBoneName;
	SFile >> m_strPieceName;
	SFile >> m_strBaseBoneName;
	SFile >> m_spThreadNeedData->m_strLinkBoneName;
	SFile >> m_bUseCPS;
	DWORD dwSize;
	SFile >> dwSize;
	m_emPieceCharType = dwSize;
	SFile >> dwSize;
	m_spThreadNeedData->m_emWeaponWhereBack = (EMPEACEZONEWEAPON) dwSize;
	if ( m_spThreadNeedData->m_emWeaponWhereBack == EMPEACE_WEAPON_NONE_MINUS )	// 다음버젼이 생기면 이 작업은 필요 없다.
	{
		m_spThreadNeedData->m_emWeaponWhereBack = EMPEACE_WEAPON_NONE;
	}

	SFile.ReadBuffer ( m_spThreadNeedData->m_vPos, sizeof(D3DXVECTOR3) );
	SFile.ReadBuffer ( m_spThreadNeedData->m_vRotate, sizeof(D3DXVECTOR3) );
	SFile.ReadBuffer ( m_spThreadNeedData->m_vScale, sizeof(D3DXVECTOR3) );

	if ( m_spThreadNeedData->m_vScale.x < 0.f )
		m_spThreadNeedData->m_vScale.x = -m_spThreadNeedData->m_vScale.x;
	if ( m_spThreadNeedData->m_vScale.y < 0.f )
		m_spThreadNeedData->m_vScale.y = -m_spThreadNeedData->m_vScale.y;
	if ( m_spThreadNeedData->m_vScale.z < 0.f )
		m_spThreadNeedData->m_vScale.z = -m_spThreadNeedData->m_vScale.z;

	SFile.ReadBuffer ( m_spThreadNeedData->m_vNonAtkPos, sizeof(D3DXVECTOR3) );
	SFile.ReadBuffer ( m_spThreadNeedData->m_vNonAtkRotate, sizeof(D3DXVECTOR3) );
	SFile.ReadBuffer ( m_spThreadNeedData->m_vNonAtkScale, sizeof(D3DXVECTOR3) );

	if ( m_spThreadNeedData->m_vNonAtkScale.x < 0.f )
		m_spThreadNeedData->m_vNonAtkScale.x = -m_spThreadNeedData->m_vNonAtkScale.x;
	if ( m_spThreadNeedData->m_vNonAtkScale.y < 0.f )
		m_spThreadNeedData->m_vNonAtkScale.y = -m_spThreadNeedData->m_vNonAtkScale.y;
	if ( m_spThreadNeedData->m_vNonAtkScale.z < 0.f )
		m_spThreadNeedData->m_vNonAtkScale.z = -m_spThreadNeedData->m_vNonAtkScale.z;

	m_spThreadNeedData->m_bInverseMatrix = FALSE;

	return TRUE;
}

BOOL DxAttBoneLink::LOAD_Ver101( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	SFile >> m_dwRef;
	SFile >> m_strAttBoneName;
	SFile >> m_strPieceName;
	SFile >> m_strBaseBoneName;
	SFile >> m_spThreadNeedData->m_strLinkBoneName;
	SFile >> m_bUseCPS;
	DWORD dwSize;
	SFile >> dwSize;
	m_emParentObjType = (EMATTBONEPARRENTOBJTYPE) dwSize;
	SFile >> dwSize;
	m_emPieceCharType = dwSize;
	SFile >> dwSize;
	m_spThreadNeedData->m_emWeaponWhereBack = (EMPEACEZONEWEAPON) dwSize;
	if ( m_spThreadNeedData->m_emWeaponWhereBack == EMPEACE_WEAPON_NONE_MINUS )	// 다음버젼이 생기면 이 작업은 필요 없다.
	{
		m_spThreadNeedData->m_emWeaponWhereBack = EMPEACE_WEAPON_NONE;
	}

	SFile.ReadBuffer ( m_spThreadNeedData->m_vPos, sizeof(D3DXVECTOR3) );
	SFile.ReadBuffer ( m_spThreadNeedData->m_vRotate, sizeof(D3DXVECTOR3) );
	SFile.ReadBuffer ( m_spThreadNeedData->m_vScale, sizeof(D3DXVECTOR3) );

	if ( m_spThreadNeedData->m_vScale.x < 0.f )
		m_spThreadNeedData->m_vScale.x = -m_spThreadNeedData->m_vScale.x;
	if ( m_spThreadNeedData->m_vScale.y < 0.f )
		m_spThreadNeedData->m_vScale.y = -m_spThreadNeedData->m_vScale.y;
	if ( m_spThreadNeedData->m_vScale.z < 0.f )
		m_spThreadNeedData->m_vScale.z = -m_spThreadNeedData->m_vScale.z;

	SFile.ReadBuffer ( m_spThreadNeedData->m_vNonAtkPos, sizeof(D3DXVECTOR3) );
	SFile.ReadBuffer ( m_spThreadNeedData->m_vNonAtkRotate, sizeof(D3DXVECTOR3) );
	SFile.ReadBuffer ( m_spThreadNeedData->m_vNonAtkScale, sizeof(D3DXVECTOR3) );

	if ( m_spThreadNeedData->m_vNonAtkScale.x < 0.f )
		m_spThreadNeedData->m_vNonAtkScale.x = -m_spThreadNeedData->m_vNonAtkScale.x;
	if ( m_spThreadNeedData->m_vNonAtkScale.y < 0.f )
		m_spThreadNeedData->m_vNonAtkScale.y = -m_spThreadNeedData->m_vNonAtkScale.y;
	if ( m_spThreadNeedData->m_vNonAtkScale.z < 0.f )
		m_spThreadNeedData->m_vNonAtkScale.z = -m_spThreadNeedData->m_vNonAtkScale.z;

	m_spThreadNeedData->m_bInverseMatrix = FALSE;

	return TRUE;
}

BOOL DxAttBoneLink::LOAD_Ver102( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	SFile >> m_dwRef;
	SFile >> m_strAttBoneName;
	SFile >> m_strPieceName;
	SFile >> m_strBaseBoneName;
	SFile >> m_bUseCPS;

	DWORD dwSize;
	SFile >> dwSize;
	m_emParentObjType = (EMATTBONEPARRENTOBJTYPE) dwSize;
	SFile >> dwSize;
	m_emPieceCharType = dwSize;

	m_spThreadNeedData->Load( SFile );

	return TRUE;
}

BOOL DxAttBoneLink::LoadABLFile(const std::string& FileName, LPDIRECT3DDEVICEQ pd3dDevice, EMSCD_FLAGS emFlags )
{
    std::string PathName(GetPath());
	PathName.append(FileName);

	std::auto_ptr<sc::BaseStream> pBStream(
        GLOGIC::openfile_basestream(
            GLOGIC::UseEngineZipFile(), 
		    GLOGIC::strSKINOBJ_ZIPFILE, 
		    PathName.c_str(), 
		    FileName));

	if (!pBStream.get())
		return FALSE;

	sc::BaseStream& SFile = *pBStream;

	DWORD dwVer;
	SFile >> dwVer;

	switch (dwVer)
	{
	case 0x0100:
		LOAD_Ver100(SFile, pd3dDevice);
		break;
	case 0x0101:
		LOAD_Ver101(SFile, pd3dDevice);
		break;
	case 0x0102:
		LOAD_Ver102(SFile, pd3dDevice);
		break;
	default:
		return FALSE;
	};

	SetABFFile(m_strAttBoneName.c_str(), pd3dDevice, FALSE, 0, emFlags );	

	// 스킨 케릭터 이를 설정.
	StringCchCopy(m_szFileName, FILE_LENGTH, FileName.c_str());
	return TRUE;
}

BOOL DxAttBoneLink::SaveABLFile ( const char* szFile, LPDIRECT3DDEVICEQ pd3dDevice, boost::function<void (const TCHAR*)> pArchive )
{
	char szPathName[MAX_PATH] = "";
	StringCchCopy( szPathName, MAX_PATH, DxAttBoneDataContainer::GetInstance().GetPath() );
	StringCchCat( szPathName, MAX_PATH, szFile );

    SPSerialFile spFile;

    if (Archive::CreateSerialFile(pArchive, spFile) == false)
    {
        return FALSE;
    }

    sc::SerialFile& SFile = *spFile;

	if ( !SFile.OpenFile ( FOT_WRITE, szPathName ) )		return FALSE;
	SFile << VERSION;
	SFile << m_dwRef;
	SFile << m_strAttBoneName;
	SFile << m_strPieceName;
	SFile << m_strBaseBoneName;
	SFile << m_bUseCPS;

	DWORD dwType;
	dwType = (DWORD)m_emParentObjType;
	SFile << dwType;
	dwType = (DWORD)m_emPieceCharType;
	SFile << dwType;

	m_spThreadNeedData->Save( SFile );

	return TRUE;
}

HRESULT DxAttBoneLink::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_AttBone.InitDeviceObjects( pd3dDevice );
	return S_OK;
}

HRESULT DxAttBoneLink::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_AttBone.RestoreDeviceObjects( pd3dDevice );
	return S_OK;
}

HRESULT DxAttBoneLink::InvalidateDeviceObjects ()
{
	m_AttBone.InvalidateDeviceObjects();
	return S_OK;
}

HRESULT DxAttBoneLink::DeleteDeviceObjects()
{
	m_AttBone.DeleteDeviceObjects();
	return S_OK;
}

HRESULT DxAttBoneLink::FrameMove ( const float fTime, 
								  const float fElapsedTime, 
								  BOOL bNoneOverFrame,
								  std::tr1::shared_ptr<DxSkinAniControl>& spLinkSkinAniControl,	//const DxSkeletonMaintain *pLinkSkeleton,
								  float fKeyJiggleWeight,
								  const BOOL bContinue/* =TRUE */, 
								  BOOL const bFreeze/* =FALSE */ )
{
	//m_spThreadNeedData->ComputeMatrix( m_matWorld, pLinkSkeleton );
	//D3DXMATRIX matBase, matScale, matRotate, matTrans;
	//D3DXMatrixIdentity( &matBase );

	//if( pLinkSkeleton )
	//{
	//	if( m_bAttackMode )
	//	{
	//		const DxBoneTransMaintain* pBoneTran = pLinkSkeleton->FindBone( m_strLinkBoneName.c_str() );
	//		if( pBoneTran )
	//			matBase = pBoneTran->m_matCombined;

	//		D3DXMatrixScaling( &matScale, m_vScale.x, m_vScale.y, m_vScale.z );
	//		D3DXMatrixRotationYawPitchRoll( &matRotate, D3DXToRadian(m_vRotate.y), D3DXToRadian(m_vRotate.x), D3DXToRadian(m_vRotate.z) );
	//		D3DXMatrixTranslation( &matTrans, m_vPos.x, m_vPos.y, m_vPos.z );

	//		m_matWorld = matScale * matRotate * matTrans * matBase;
	//	}
	//	else
	//	{
	//		const DxBoneTransMaintain* pBoneTran = pLinkSkeleton->FindBone( SKINTYPE_STING::m_szWHEREBACKTYPE[m_emWeaponWhereBack] );
	//		if( pBoneTran )
	//			matBase = pBoneTran->m_matCombined;

	//		D3DXMatrixScaling( &matScale, m_vNonAtkScale.x, m_vNonAtkScale.y, m_vNonAtkScale.z );
	//		D3DXMatrixRotationYawPitchRoll( &matRotate, D3DXToRadian(m_vNonAtkRotate.y), D3DXToRadian(m_vNonAtkRotate.x), D3DXToRadian(m_vNonAtkRotate.z) );
	//		D3DXMatrixTranslation( &matTrans, m_vNonAtkPos.x, m_vNonAtkPos.y, m_vNonAtkPos.z );

	//		m_matWorld = matScale * matRotate * matTrans * matBase;
	//	}
	//}

	m_AttBone.FrameMove( fTime, fElapsedTime, spLinkSkinAniControl, m_spThreadNeedData, bNoneOverFrame, fKeyJiggleWeight, bContinue, bFreeze );

	return S_OK;
}

HRESULT DxAttBoneLink::FrameMoveForTool ( const float fTime, 
									  const float fElapsedTime, 
									  const DxSkeletonMaintain *pLinkSkeleton )
{
	//D3DXMATRIX matBase, matScale, matRotate, matTrans;
	//D3DXMatrixIdentity( &matBase );

	//D3DXMATRIX matWorld;
	//D3DXMatrixIdentity( &matWorld );
	//if( pLinkSkeleton )
	//{
	//	if( m_bAttackMode )
	//	{
	//		const DxBoneTransMaintain* pBoneTran = pLinkSkeleton->FindBone( m_strLinkBoneName.c_str() );
	//		if( pBoneTran )
	//			matBase = pBoneTran->m_matCombined;

	//		D3DXMatrixScaling( &matScale, m_vScale.x, m_vScale.y, m_vScale.z );
	//		D3DXMatrixRotationYawPitchRoll( &matRotate, D3DXToRadian(m_vRotate.y), D3DXToRadian(m_vRotate.x), D3DXToRadian(m_vRotate.z) );
	//		D3DXMatrixTranslation( &matTrans, m_vPos.x, m_vPos.y, m_vPos.z );

	//		matWorld = matScale * matRotate * matTrans * matBase;
	//	}
	//	else
	//	{
	//		const DxBoneTransMaintain* pBoneTran = pLinkSkeleton->FindBone( SKINTYPE_STING::m_szWHEREBACKTYPE[m_emWeaponWhereBack] );
	//		if( pBoneTran )
	//			matBase = pBoneTran->m_matCombined;

	//		D3DXMatrixScaling( &matScale, m_vNonAtkScale.x, m_vNonAtkScale.y, m_vNonAtkScale.z );
	//		D3DXMatrixRotationYawPitchRoll( &matRotate, D3DXToRadian(m_vNonAtkRotate.y), D3DXToRadian(m_vNonAtkRotate.x), D3DXToRadian(m_vNonAtkRotate.z) );
	//		D3DXMatrixTranslation( &matTrans, m_vNonAtkPos.x, m_vNonAtkPos.y, m_vNonAtkPos.z );

	//		matWorld = matScale * matRotate * matTrans * matBase;
	//	}
	//}

	D3DXMATRIX matWorld;
	m_spThreadNeedData->ComputeMatrix( matWorld, pLinkSkeleton );

	m_AttBone.FrameMoveForTool( fTime, fElapsedTime, matWorld );

	return S_OK;
}



void DxAttBoneLink::SELECTANI ( EMANI_MAINTYPE MType )
{
	m_AttBone.SELECTANI( MType, AN_SUB_NONE );
}

HRESULT DxAttBoneLink::RenderFixed( const LPDIRECT3DDEVICEQ pd3dDevice, 
								   float fAlpha,
								   const BOOL bShadow, 
								   const BOOL bEff, 
								   const BOOL bReverse, 
								   float fKeyJiggleWeight )
{
	if( !m_bRender ) return S_OK;

	m_AttBone.RenderFixed( pd3dDevice, fAlpha, m_matWorld, bShadow, bEff, bReverse );

	return S_OK;
}

HRESULT DxAttBoneLink::RenderShadow( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX &matRot, const D3DXMATRIX& matViewProj, const BOOL bDay )
{
	m_AttBone.RenderShadow( pd3dDevice, matRot, matViewProj, bDay );
	return S_OK;
}

HRESULT DxAttBoneLink::RenderShadowForMaterialSystem( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matViewProj )
{
	m_AttBone.RenderShadowForMaterialSystem( pd3dDevice, matViewProj );
	return S_OK;
}

void DxAttBoneLink::SetPosOffset( const D3DXVECTOR3& vPosOffset )
{
	if( !m_bRender ) 
		return;

	m_AttBone.SetPosOffset( vPosOffset );
}

void DxAttBoneLink::RenderMaterialOpaque( LPDIRECT3DDEVICEQ pd3dDevice, 
										 float fAlpha, 
										 const BOOL bShadow, 
										 const BOOL bEff,
										 const DxClothColl* pClothColl,
										 const std::vector<physx::PxClothCollisionSphere>* pvecClothCollSpheres,
										 float fAttParentBoneScale )
{
	if( !m_bRender ) 
		return;

	m_AttBone.RenderMaterialOpaque( pd3dDevice, fAlpha, bShadow, bEff, pClothColl, pvecClothCollSpheres, fAttParentBoneScale );

	return;
}

void DxAttBoneLink::RenderMaterialHardAlpha( LPDIRECT3DDEVICEQ pd3dDevice, float fAlpha )
{
	if( !m_bRender ) 
		return;

	m_AttBone.RenderMaterialHardAlpha( pd3dDevice, fAlpha );
}

void DxAttBoneLink::RenderMaterialSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, float fAlpha )
{
	if( !m_bRender ) 
		return;

	m_AttBone.RenderMaterialSoftAlpha( pd3dDevice, fAlpha );
}

void DxAttBoneLink::RenderEff( LPDIRECT3DDEVICEQ pd3dDevice, float fAlpha )
{
	if( !m_bRender ) 
		return;

	m_AttBone.RenderEff( pd3dDevice, fAlpha );
}

void DxAttBoneLink::RenderMaterialAdd( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( !m_bRender ) 
		return;

	m_AttBone.RenderMaterialAdd( pd3dDevice );
}

void DxAttBoneLink::RenderMaterialGlow( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( !m_bRender ) 
		return;

	m_AttBone.RenderMaterialGlow( pd3dDevice );
}

BOOL DxAttBoneLink::RenderMaterialGhosting( LPDIRECT3DDEVICEQ pd3dDevice, float fAlpha, size_t unGhostingIndex )
{
	if( !m_bRender ) 
		return FALSE;

	return m_AttBone.RenderMaterialGhosting( pd3dDevice, fAlpha, unGhostingIndex );
}

void DxAttBoneLink::RenderCpsForEdit( LPDIRECT3DDEVICEQ pd3dDevice, DxSkeleton* pAttBoneLinkSkeleton )
{
	if( !m_bRender ) 
		return;

	//	Note : Piece를 읽어온다.
	DxSkinPiece* pSkinPiece = DxSkinPieceContainer::GetInstance().LoadPiece( m_strPieceName, 0L );
	if ( !pSkinPiece )
		return;

	TSTRING strSkeleton( pSkinPiece->m_szSkeleton );
	std::transform ( strSkeleton.begin(), strSkeleton.end(), strSkeleton.begin(), tolower );
	std::transform ( m_strBaseBoneName.begin(), m_strBaseBoneName.end(), m_strBaseBoneName.begin(), tolower );

	if ( m_strBaseBoneName != strSkeleton )	
		return;

	D3DXMATRIXA16 matWorld;
	D3DXMatrixIdentity( &matWorld );

	if ( pSkinPiece->GetLinkBoneName() )
	{
		DxSkeletonMaintain sSkeletonMaintain;
		sSkeletonMaintain.Import_PureThread( pAttBoneLinkSkeleton );

		const DxBoneTransMaintain* pSlotHeld(NULL);
		const DxBoneTransMaintain* pHandHeld = sSkeletonMaintain.FindBone( pSkinPiece->GetLinkBoneName() );
		if ( pHandHeld )
		{
			switch( pSkinPiece->m_emWeaponWhereBack )
			{
			case EMPEACE_WEAPON_RSLOT:
			case EMPEACE_WEAPON_WHAL_SLOT:
			case EMPEACE_WEAPON_STICK_RSLOT:
			case EMPEACE_WEAPON_R_GUN_SLOT:
			case EMPEACE_WEAPON_L_GUN_SLOT:
			case EMPEACE_WEAPON_GUN_RSLOT:
			case EMPEACE_WEAPON_L_SPEAR_SLOT:
			case EMPEACE_WEAPON_R_SPEAR_SLOT:
			case EMPEACE_WEAPON_SIDE_SLOT:
			case EMPEACE_WEAPON_KUNAI_SLOT:
			case EMPEACE_WEAPON_HAMMER_SLOT:
			case EMPEACE_WEAPON_SHIELD_SLOT:
			case EMPEACE_WEAPON_MAGIC_STICK_SLOT:
				pSlotHeld = sSkeletonMaintain.FindBone ( SKINTYPE_STING::m_szWHEREBACKTYPE[pSkinPiece->m_emWeaponWhereBack] );
				break;
			case EMPEACE_WEAPON_USER_SLOT:
				pSlotHeld = sSkeletonMaintain.FindBone ( pSkinPiece->m_strUserBackSlot );
				break;
			};	

			// m_bUserBackSlot 가 TRUE 이고, Weapon Use Slot 으로 셋팅 되어 있으면, UserBackSlot 으로만 동작되도록 한다.
			if( pSkinPiece->m_bKeepUserBackSlot && pSkinPiece->m_emWeaponWhereBack == EMPEACE_WEAPON_USER_SLOT && pSlotHeld )
			{
				matWorld = pSlotHeld->m_matCombined;
				D3DXMatrixMultiply( &matWorld, &pSkinPiece->m_matUserBackSlot, &matWorld );
			}
			else
			{
				if ( m_spThreadNeedData->m_bAttackMode && pHandHeld )	
				{
					matWorld = pHandHeld->m_matCombined;
				}
				else if ( !m_spThreadNeedData->m_bAttackMode && pSlotHeld )	
				{
					matWorld = pSlotHeld->m_matCombined;

					if ( pSkinPiece->m_emWeaponWhereBack == EMPEACE_WEAPON_USER_SLOT )
					{
						D3DXMatrixMultiply( &matWorld, &pSkinPiece->m_matUserBackSlot, &matWorld );
					}
				}
			}

			// Note : 일반 메쉬인 경우 매트릭스를 미리 계산해 놓는다.
			D3DXMatrixMultiply( &matWorld, pSkinPiece->m_pmcMesh->m_ppStaticMatrixPtrs[1], &matWorld );
			D3DXMatrixMultiply( &matWorld, pSkinPiece->m_pmcMesh->m_ppStaticMatrixPtrs[0], &matWorld );
		}
	}

	pSkinPiece->Render( pd3dDevice, matWorld, FALSE );
}

void DxAttBoneLink::GhostingON( BOOL bGhosting, float fLifeTime )
{
	m_AttBone.GhostingON( bGhosting, fLifeTime );
}

void DxAttBoneLink::SetAttackMode ( BOOL bMode )
{ 
	m_spThreadNeedData->m_bAttackMode = bMode; 
}

void DxAttBoneLink::SetWorldBattleFlag( const TCHAR* pWorldBattleFlag )
{
	m_AttBone.SetWorldBattleFlag( pWorldBattleFlag );
}

//////////////////////////////////////////////////////////////////////////
// 외부 옵션에 따른 ON/OFF 로 인한 내부 Data 변화시 호출 함수 및 변수
BOOL DxAttBoneLink::SetPhysX_Cloth( BOOL bOnOff )
{
	return m_AttBone.SetPhysX_Cloth( bOnOff );
}




//--------------------------------------------------------------------------[DxAttBoneLinkContainer]
//

DxAttBoneLinkContainer& DxAttBoneLinkContainer::GetInstance()
{
	static DxAttBoneLinkContainer Instance;
	return Instance;
}


DxAttBoneLinkContainer::DxAttBoneLinkContainer ()
	: m_pd3dDevice(NULL)
{
	memset( m_szPath, 0, sizeof(char)*MAX_PATH );
}

DxAttBoneLinkContainer::~DxAttBoneLinkContainer ()
{
}

HRESULT DxAttBoneLinkContainer::CleanUp ()
{
	SKINATTBONELINKMAP_ITER iter = m_mapAttBoneLink.begin ();
	SKINATTBONELINKMAP_ITER iterEnd = m_mapAttBoneLink.end ();

	for ( ; iter!=iterEnd; iter++ )
	{
		delete (*iter).second;
	}

	m_mapAttBoneLink.clear ();

	return S_OK;
}

void DxAttBoneLinkContainer::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;
}

DxAttBoneLink* DxAttBoneLinkContainer::FindLink(const std::string& FileName)
{
	SKINATTBONELINKMAP_ITER iter = m_mapAttBoneLink.find(FileName);
	if (iter != m_mapAttBoneLink.end())
        return iter->second;
    else
	    return NULL;
}

BOOL DxAttBoneLinkContainer::DeleteLink ( const char* szFile )
{
	SKINATTBONELINKMAP_ITER iter = m_mapAttBoneLink.find ( std::string(szFile) );
	if ( iter == m_mapAttBoneLink.end() )	return FALSE;

	delete (*iter).second;
	m_mapAttBoneLink.erase ( iter );

	return TRUE;
}

void DxAttBoneLinkContainer::ReleaseLink ( const char* szFile )
{
	DxAttBoneLink* pRes = FindLink ( szFile );
	if ( !pRes )	return;

	if ( pRes->m_dwRef == 0 )
	{
		DeleteLink ( szFile );
		return;
	}

	pRes->m_dwRef--;
	return;
}

DxAttBoneLink* DxAttBoneLinkContainer::LoadLink(const std::string& FileName, EMSCD_FLAGS emFlags )
{
	DxAttBoneLink* pAttBonLink = FindLink(FileName);
	if (pAttBonLink)
	{
		pAttBonLink->m_dwRef++;
		return pAttBonLink;
	}

	pAttBonLink = new DxAttBoneLink;
	BOOL bOk = pAttBonLink->LoadABLFile(FileName, m_pd3dDevice, emFlags );
	if (!bOk)
	{
		SAFE_DELETE(pAttBonLink);
		return NULL;
	}

	pAttBonLink->m_emSkinPieceType = DxSkinPieceBase::SKIN_ATT_BONE_LINK;
	pAttBonLink->m_dwRef++;

	CDebugSet::ToListView("DxAttBoneLink::LoadFile, %s", FileName.c_str());
	
    // 트리에 달기.
	m_mapAttBoneLink[std::string(pAttBonLink->GetFileName())] = pAttBonLink;
	return pAttBonLink;
}

