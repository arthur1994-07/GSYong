#include "pch.h"

#include "../../DxTools/editmeshs.h"

#include "../../DxEffect/Frame/DxEffectMan.h"

#include "../../DxEffect/Single/DxEffSingle.h"
#include "../Single/DxEffSinglePropGMan.h"
#include "../../../SigmaCore/File/SerialFile.h"

#include "../../DxMeshs/SkinMesh/DxSkinChar.h"
#include "../../DxMeshs/SkinMesh/DxAttBone.h"
#include "../../DxMeshs/SkinMesh/DxVehicle.h"

#include "../../DxTools/DxWeatherMan.h"
#include "../../DxTools/RENDERPARAM.h"

#include "./DxEffAniGhosting.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

DWORD		DxEffAniData_Ghosting::TYPEID			= EMEFFANI_GHOSTING;
DWORD		DxEffAniData_Ghosting::VERSION			= 0x0100;
char		DxEffAniData_Ghosting::NAME[64]	= "Ghosting EFFECT";

DxEffAni* DxEffAniData_Ghosting::NEWOBJ( DxSkinAnimation* pAnimContainer )
{
	DxEffAniGhosting*	pEff = new DxEffAniGhosting;

	pEff->m_dwFlags			= m_dwFlags;
	pEff->m_dwFrame_1		= m_dwFrame_1;
	pEff->m_dwFrame_2		= m_dwFrame_2;
	pEff->m_dwGhostingNum	= m_dwGhostingNum;
	pEff->m_dwAlphaMax		= m_dwAlphaMax;
	pEff->m_dwUnit			= m_dwUnit;
	pEff->m_dwColor			= m_dwColor;

	pEff->Create ( m_pd3dDevice, pAnimContainer );

	return	pEff;
}

HRESULT	DxEffAniData_Ghosting::LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	DWORD dwVer, dwSize, dwTypeID;

	SFile >> dwTypeID;
	SFile >> dwVer;
	SFile >> dwSize;

	//	Note : 버전이 일치할 경우. 
	//
	if ( dwVer == VERSION )
	{
		SFile.ReadBuffer ( &m_Property, sizeof(m_Property) );
	}
	else
	{
		SFile.SetOffSet ( SFile.GetfTell()+dwSize );
		
		return E_FAIL;
	}

	if ( pd3dDevice )
	{
		hr = Create ( pd3dDevice );
		if ( FAILED(hr) )	return hr;
	}

	return S_OK;
}

HRESULT	DxEffAniData_Ghosting::SaveFile ( sc::SerialFile &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	SFile << (DWORD) ( sizeof(m_Property) );

	//	Note : 이팩트의 Property 를 저장.
	//
	SFile.WriteBuffer ( &m_Property, sizeof(m_Property) );

	return S_OK;
}

BOOL DxEffAniData_Ghosting::CheckEff ( DWORD dwCurKeyTime, DWORD dwPrevFrame )
{
	if ( !m_bCreated )			// 다음에는 현재 효과를 또 생성하지 않는다.
	{
		m_bCreated = TRUE;

		return TRUE;
	}

	return FALSE;		
}

//	-----------------------------------------------	--------------------------------------------------

DxEffAniGhosting::DxEffAniGhosting(void) :
	DxEffAni(),
	m_pd3dDevice(NULL),
	m_pSavedBaseSB(NULL),
	m_pEffectBaseSB(NULL),
	m_nPrevKeyTime(-1)
{
}

DxEffAniGhosting::~DxEffAniGhosting(void)
{
	m_pd3dDevice = NULL;

	m_listGhosting.clear();

	CleanUp ();
}

//DxEffAni* DxEffAniGhosting::CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice )
//{
//	HRESULT hr;
//	DxEffAniGhosting *pEffAni = new DxEffAniGhosting;
//	pEffAni->SetProperty ( &m_Property );
//
//	hr = pEffAni->Create ( pd3dDevice );
//	if ( FAILED(hr) )
//	{
//		SAFE_DELETE(pEffAni);
//		return NULL;
//	}
//
//	return pEffAni;
//}

HRESULT DxEffAniGhosting::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT DxEffAniGhosting::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT DxEffAniGhosting::OneTimeSceneInit ()
{
	return S_OK;
}

HRESULT DxEffAniGhosting::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	return S_OK;
}

HRESULT DxEffAniGhosting::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
    InvalidateDeviceObjects ();

	if ( !pd3dDevice )
        return S_OK;

	D3DCAPSQ d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );

	m_pd3dDevice = pd3dDevice;

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		//	Note : SetRenderState() 선언
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,			FALSE );

		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );

		pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR,		0xffffffff );

		//	Note : SetTextureStageState() 선언
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );

		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLORARG1,	D3DTA_CURRENT );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLORARG2,	D3DTA_TFACTOR );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,	D3DTOP_MODULATE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2,	D3DTA_TFACTOR );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedBaseSB );
		else			pd3dDevice->EndStateBlock( &m_pEffectBaseSB );
	}

	return S_OK;
}

HRESULT DxEffAniGhosting::InvalidateDeviceObjects ()
{
	SAFE_RELEASE( m_pSavedBaseSB );
	SAFE_RELEASE( m_pEffectBaseSB );

	return S_OK;
}

HRESULT DxEffAniGhosting::DeleteDeviceObjects ()
{
	return S_OK;
}

HRESULT DxEffAniGhosting::FinalCleanup ()
{
	return S_OK;
}

HRESULT DxEffAniGhosting::FrameMove ( DxSkinCharEffAniBase* pSkinCharBase, float fTime, float fElapsedTime )
{
	DWORD dwKeyTime( pSkinCharBase->GETCURKEYTIME() );

	float fLifeTime = static_cast<float>((m_dwGhostingNum+1)*m_dwUnit*pSkinCharBase->GETUNITTIME())/UNITANIKEY_PERSEC;

	if ( m_bFrameMove && (m_dwFrame_1<=dwKeyTime) && (dwKeyTime<=m_dwFrame_2) )		// 범위 안에 들어오면 생성
	{
		pSkinCharBase->GhostingON( TRUE, fLifeTime );
	}
	else
	{
		pSkinCharBase->GhostingON( FALSE, fLifeTime );
	}

	return S_OK;
}

HRESULT	DxEffAniGhosting::Render ( const LPDIRECT3DDEVICEQ pd3dDevice, DxSkinCharEffAniBase* pSkinChar, const D3DXMATRIX& matCurPos, float fAlphaParent )
{
	// 최소 PixelShader 는 되야한다.
	if ( RENDERPARAM::g_emCharacterQulity < TnL_CHAR_PS_1_1 )
		return S_OK;
	
	//	Render 시작
	//m_pSavedBaseSB->Capture();
	//m_pEffectBaseSB->Apply();	// State Block 셋팅.

	DWORD dwAlphaBlendEnable(0L);
	pd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE, &dwAlphaBlendEnable );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	size_t unGhostingIndex(1);
	if ( m_dwUnit > 0 )
		unGhostingIndex = m_dwUnit-1;

	for ( DWORD i=0; i<m_dwGhostingNum; ++i, unGhostingIndex+=m_dwUnit )
	{
		float fAlpha = static_cast<float>(m_dwGhostingNum-i) / (m_dwGhostingNum+1);
		if ( !pSkinChar->RenderMaterialGhosting( pd3dDevice, fAlpha*fAlphaParent, unGhostingIndex ) )
			break;
	}

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, dwAlphaBlendEnable );


	//m_pSavedBaseSB->Apply();


	//HRESULT hr = S_OK;
	//D3DXVECTOR3 vNormalUp;
	//D3DXVECTOR3 vNormalDown;
	//D3DXMATRIX	matWorld;
	//D3DXMatrixIdentity ( &matWorld );

	//DWORD	dwUnit;
	//float	fAlpha;
	//DWORD	GHOSTMAX	= m_dwGhostingNum*m_dwUnit;
	//DWORD	dwColor		= 0xffffffff;
	//DWORD	dwUnitTime	= pSkinChar->GETUNITTIME();
	//float	fOldCurTime	= pSkinChar->GETCURTIME ();
	//DWORD	dwKeyTime	= pSkinChar->GETCURKEYTIME();

	//// Note : 렌더할 고스팅이 없다면...
	//if( !m_listGhosting.empty() )
	//{
	//	//	Render 시작
	//	m_pSavedBaseSB->Capture();
	//	{
	//		if ( m_dwColor == 0x00ffffff )		// 흰색일 경우 색의 변화는 주지 않는다.
	//		{
	//			pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,	D3DTOP_DISABLE );
	//		}

	//		GHOSTINGLIST_ITER iter = m_listGhosting.begin();
	//		for ( ; iter!=m_listGhosting.end(); ++iter )
	//		{
	//			if ( !(iter->m_Use) )		continue;

	//			m_pEffectBaseSB->Apply();	// State Block 셋팅.

	//			//	Note : Skin Animation.
	//			pSkinChar->SETCURTIME ( (float)(iter->m_dwKeyTime*dwUnitTime) );
	//			pSkinChar->CalculateAniBone( iter->m_matCurAni );

	//			fAlpha = (float)(GHOSTMAX-iter->m_UseNum);

	//			dwColor		=	m_dwColor;
	//			dwColor		+= (DWORD)((fAlpha*fAlphaParent/GHOSTMAX)*m_dwAlphaMax)<<24;
	//			pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR, dwColor );



	//			CHARSETTING sCharSetting;
	//			sCharSetting.pMeshContainerBase = pSkinChar->m_PartArray[PIECE_HEAD].m_rMeshContainer;
	//			sCharSetting.pmtrlPiece			= pSkinChar->m_PartArray[PIECE_HEAD].m_arrayMaterialPiece;
	//			sCharSetting.pmtrlSpecular		= NULL;
	//			sCharSetting.bWorldIdentity		= FALSE;
	//			sCharSetting.emRDOP				= CTOP_BASE;

	//			switch( pSkinChar->m_sSkinEffData.m_bNewHeadDRAW )
	//			{
	//			case TRUE:
	//				if( !pSkinChar->m_PartArray[PIECE_HEAD].m_rSkinMesh )	break;
	//				if( !pSkinChar->m_PartArray[PIECE_HEAD].m_rMeshContainer )		break;

	//				pd3dDevice->SetTexture( 0, pSkinChar->m_sSkinEffData.m_rFaceTex );
	//				sCharSetting.emRDOP		= CTOP_NEWTEX;
	//				pSkinChar->m_PartArray[PIECE_HEAD].m_rSkinMesh->DrawMeshContainer( pd3dDevice, sCharSetting, pSkinChar->GetSkeletonMaintain() );
	//				break;

	//			case FALSE:
	//				if( !pSkinChar->m_PartArray[PIECE_HEAD].m_rSkinMesh )	break;
	//				if( !pSkinChar->m_PartArray[PIECE_HEAD].m_rMeshContainer )		break;

	//				sCharSetting.emRDOP		= CTOP_BASE;
	//				pSkinChar->m_PartArray[PIECE_HEAD].m_rSkinMesh->SetDrawState( TRUE, FALSE, FALSE, FALSE );
	//				pSkinChar->m_PartArray[PIECE_HEAD].m_rSkinMesh->DrawMeshContainer( pd3dDevice, sCharSetting, pSkinChar->GetSkeletonMaintain() );
	//				break;
	//			}

	//			for ( int i=1; i<PIECE_SAVE_SIZE; i++ )
	//			{
	//				if ( !pSkinChar->m_PartArray[i].m_rSkinMesh )	continue;
	//				if ( !pSkinChar->m_PartArray[i].m_rMeshContainer )		continue;

	//				CHARSETTING sCharSetting;
	//				sCharSetting.pMeshContainerBase = pSkinChar->m_PartArray[i].m_rMeshContainer;
	//				sCharSetting.pmtrlPiece			= pSkinChar->m_PartArray[i].m_arrayMaterialPiece;
	//				sCharSetting.pmtrlSpecular		= NULL;
	//				sCharSetting.bWorldIdentity		= FALSE;
	//				sCharSetting.emRDOP				= CTOP_BASE;

	//				pSkinChar->m_PartArray[i].m_rSkinMesh->SetDrawState( TRUE, FALSE, FALSE, FALSE );
	//				pSkinChar->m_PartArray[i].m_rSkinMesh->DrawMeshContainer( pd3dDevice, sCharSetting, pSkinChar->GetSkeletonMaintain() );
	//			}
	//		}
	//	}
	//	m_pSavedBaseSB->Apply();

	//	pSkinChar->SETCURTIME ( fOldCurTime );
	//}

	//
	//if ( m_nPrevKeyTime != dwKeyTime )
	//{
	//	// Note : Count 를 올려서 한계에 차면 삭제시킴.
	//	GHOSTINGLIST_ITER	iter = m_listGhosting.begin();
	//	GHOSTINGLIST_ITER	iter_del;
	//	for ( ; iter!=m_listGhosting.end(); )
	//	{
	//		iter->m_UseNum += 1;

	//		iter_del = iter++;

	//		if ( iter_del->m_UseNum >= GHOSTMAX )
	//		{
	//			m_listGhosting.erase ( iter_del );
	//		}
	//	}

	//	m_nPrevKeyTime = dwKeyTime;

	//	// 새로운 고스팅을 생성해도 될 경우.
	//	if( m_bFrameMove )
	//	{
	//		if ( (m_dwFrame_1<=dwKeyTime) && (dwKeyTime<=m_dwFrame_2) )		// 범위 안에 들어오면 생성
	//		{
	//			// 새로운 것을 위한 작업 시작
	//			GHOSTING_INFORMATION	sGhost_Information;

	//			sGhost_Information.m_Use		= FALSE;
	//			sGhost_Information.m_UseNum		= 0;
	//			sGhost_Information.m_dwKeyTime	= dwKeyTime;
	//			sGhost_Information.m_matCurAni	= matCurPos;

	//			m_listGhosting.push_back ( sGhost_Information );
	//		}
	//	}
	//	else
	//	{
	//		// 임시 수정판... 이상하게.. 위쪽의 erase가 제대로 안되는 것 같다.
	//		m_listGhosting.clear();
	//	}

	//	// Note : 뿌려야 하는 때를 셋팅한다.
	//	iter = m_listGhosting.begin();
	//	for ( ; iter!=m_listGhosting.end(); ++iter )
	//	{
	//		dwUnit = iter->m_UseNum + 1;
	//		dwUnit = dwUnit%m_dwUnit;
	//		if ( dwUnit )	iter->m_Use = FALSE;
	//		else			iter->m_Use = TRUE;
	//	}
	//}

	return S_OK;
}
//
//HRESULT	DxEffAniGhosting::Render ( const LPDIRECT3DDEVICEQ pd3dDevice, DxAttBone* pAttBone, const D3DXMATRIX& matCurPos, float fAlphaParent )
//{
//	// 최소 PixelShader 는 되야한다.
//	if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
//		return S_OK;
//
//	//	Render 시작
//	m_pSavedBaseSB->Capture();
//	m_pEffectBaseSB->Apply();	// State Block 셋팅.
//
//	int nGhostingIndex(m_dwUnit);
//	for ( DWORD i=0; i<m_dwGhostingNum; ++i, nGhostingIndex+=m_dwUnit )
//	{
//		const D3DXMATRIXA16* parrayBoneMatrices = pSkinChar->GetBoneMatricesForGhosting( nGhostingIndex );
//		if ( !parrayBoneMatrices )
//			break;
//
//		float fAlpha = static_cast<float>(m_dwGhostingNum-i) / (m_dwGhostingNum+1);
//		//pAttBone->RenderMaterialOpaque( pd3dDevice, fAlpha, FALSE, parrayBoneMatrices, FALSE, FALSE, FALSE, FALSE );
//		//pAttBone->RenderMaterialHardAlpha( pd3dDevice, fAlpha );
//		//pAttBone->RenderMaterialSoftAlpha( pd3dDevice, fAlpha );
//	}
//
//	m_pSavedBaseSB->Apply();
//
//
//
//	//HRESULT hr = S_OK;
//	//D3DXVECTOR3 vNormalUp;
//	//D3DXVECTOR3 vNormalDown;
//	//D3DXMATRIX	matWorld;
//	//D3DXMatrixIdentity ( &matWorld );
//
//	//DWORD	dwUnit;
//	//float	fAlpha;
//	//DWORD	GHOSTMAX	= m_dwGhostingNum*m_dwUnit;
//	//DWORD	dwColor		= 0xffffffff;
//	//DWORD	dwUnitTime	= pAttBone->GETUNITTIME();
//	//float	fOldCurTime	= pAttBone->GETCURTIME ();
//	//DWORD	dwKeyTime	= pAttBone->GETCURKEYTIME();
//
//	//// Note : 렌더할 고스팅이 없다면...
//	//if( !m_listGhosting.empty() )
//	//{
//	//	//	Render 시작
//	//	m_pSavedBaseSB->Capture();
//	//	{
//	//		if ( m_dwColor == 0x00ffffff )		// 흰색일 경우 색의 변화는 주지 않는다.
//	//		{
//	//			pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,	D3DTOP_DISABLE );
//	//		}
//
//	//		GHOSTINGLIST_ITER iter = m_listGhosting.begin();
//	//		for ( ; iter!=m_listGhosting.end(); ++iter )
//	//		{
//	//			if ( !(iter->m_Use) )		continue;
//
//	//			m_pEffectBaseSB->Apply();	// State Block 셋팅.
//
//	//			//	Note : Skin Animation.
//	//			pAttBone->SETCURTIME ( (float)(iter->m_dwKeyTime*dwUnitTime) );
//	//			pAttBone->CalculateAniBone( iter->m_matCurAni );
//
//	//			fAlpha = (float)(GHOSTMAX-iter->m_UseNum);
//
//	//			dwColor		=	m_dwColor;
//	//			dwColor		+= (DWORD)((fAlpha*fAlphaParent/GHOSTMAX)*m_dwAlphaMax)<<24;
//	//			pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR, dwColor );
//
//
//
//	//			CHARSETTING sCharSetting;
//	//			sCharSetting.pMeshContainerBase = pAttBone->m_AttBoneArray[ATTBONE_BODY].m_rMeshContainer;
//	//			sCharSetting.pmtrlPiece			= pAttBone->m_AttBoneArray[ATTBONE_BODY].m_arrayMaterialPiece;
//	//			sCharSetting.pmtrlSpecular		= NULL;
//	//			sCharSetting.bWorldIdentity		= FALSE;
//	//			sCharSetting.emRDOP				= CTOP_BASE;
//
//	//			switch( pAttBone->m_sSkinEffData.m_bNewHeadDRAW )
//	//			{
//	//			case TRUE:
//	//				if( !pAttBone->m_AttBoneArray[ATTBONE_BODY].m_rSkinMesh )	break;
//	//				if( !pAttBone->m_AttBoneArray[ATTBONE_BODY].m_rMeshContainer )		break;
//
//	//				pd3dDevice->SetTexture( 0, pAttBone->m_sSkinEffData.m_rFaceTex );
//	//				sCharSetting.emRDOP		= CTOP_NEWTEX;
//	//				pAttBone->m_AttBoneArray[ATTBONE_BODY].m_rSkinMesh->DrawMeshContainer( pd3dDevice, sCharSetting, pAttBone->GetSkeletonMaintain() );
//	//				break;
//
//	//			case FALSE:
//	//				if( !pAttBone->m_AttBoneArray[ATTBONE_BODY].m_rSkinMesh )	break;
//	//				if( !pAttBone->m_AttBoneArray[ATTBONE_BODY].m_rMeshContainer )		break;
//
//	//				sCharSetting.emRDOP		= CTOP_BASE;
//	//				pAttBone->m_AttBoneArray[ATTBONE_BODY].m_rSkinMesh->SetDrawState( TRUE, FALSE, FALSE, FALSE );
//	//				pAttBone->m_AttBoneArray[ATTBONE_BODY].m_rSkinMesh->DrawMeshContainer( pd3dDevice, sCharSetting, pAttBone->GetSkeletonMaintain() );
//	//				break;
//	//			}
//
//	//			for ( int i=1; i<ATTBONE_SIZE; i++ )
//	//			{
//	//				if ( !pAttBone->m_AttBoneArray[i].m_rSkinMesh )	continue;
//	//				if ( !pAttBone->m_AttBoneArray[i].m_rMeshContainer )		continue;
//
//	//				CHARSETTING sCharSetting;
//	//				sCharSetting.pMeshContainerBase = pAttBone->m_AttBoneArray[i].m_rMeshContainer;
//	//				sCharSetting.pmtrlPiece			= pAttBone->m_AttBoneArray[i].m_arrayMaterialPiece;
//	//				sCharSetting.pmtrlSpecular		= NULL;
//	//				sCharSetting.bWorldIdentity		= FALSE;
//	//				sCharSetting.emRDOP				= CTOP_BASE;
//
//	//				pAttBone->m_AttBoneArray[i].m_rSkinMesh->SetDrawState( TRUE, FALSE, FALSE, FALSE );
//	//				pAttBone->m_AttBoneArray[i].m_rSkinMesh->DrawMeshContainer( pd3dDevice, sCharSetting, pAttBone->GetSkeletonMaintain() );
//	//			}
//	//		}
//	//	}
//	//	m_pSavedBaseSB->Apply();
//
//	//	pAttBone->SETCURTIME ( fOldCurTime );
//	//}
//
//
//	//if ( m_nPrevKeyTime != dwKeyTime )
//	//{
//	//	// Note : Count 를 올려서 한계에 차면 삭제시킴.
//	//	GHOSTINGLIST_ITER	iter = m_listGhosting.begin();
//	//	GHOSTINGLIST_ITER	iter_del;
//	//	for ( ; iter!=m_listGhosting.end(); )
//	//	{
//	//		iter->m_UseNum += 1;
//
//	//		iter_del = iter++;
//
//	//		if ( iter->m_UseNum >= GHOSTMAX )
//	//		{
//	//			m_listGhosting.erase ( iter_del );
//	//		}
//	//	}
//
//	//	m_nPrevKeyTime = dwKeyTime;
//
//	//	// 새로운 고스팅을 생성해도 될 경우.
//	//	if( m_bFrameMove )
//	//	{
//	//		if ( (m_dwFrame_1<=dwKeyTime) && (dwKeyTime<=m_dwFrame_2) )		// 범위 안에 들어오면 생성
//	//		{
//	//			// 새로운 것을 위한 작업 시작
//	//			GHOSTING_INFORMATION	sGhost_Information;
//
//	//			sGhost_Information.m_Use		= FALSE;
//	//			sGhost_Information.m_UseNum		= 0;
//	//			sGhost_Information.m_dwKeyTime	= dwKeyTime;
//	//			sGhost_Information.m_matCurAni	= matCurPos;
//
//	//			m_listGhosting.push_back ( sGhost_Information );
//	//		}
//	//	}
//	//	else
//	//	{
//	//		// 임시 수정판... 이상하게.. 위쪽의 erase가 제대로 안되는 것 같다.
//	//		m_listGhosting.clear();
//	//	}
//
//	//	// Note : 뿌려야 하는 때를 셋팅한다.
//	//	iter = m_listGhosting.begin();
//	//	for ( ; iter!=m_listGhosting.end(); ++iter )
//	//	{
//	//		dwUnit = iter->m_UseNum + 1;
//	//		dwUnit = dwUnit%m_dwUnit;
//	//		if ( dwUnit )	iter->m_Use = FALSE;
//	//		else			iter->m_Use = TRUE;
//	//	}
//	//}
//
//	return S_OK;
//}
//
//HRESULT	DxEffAniGhosting::Render ( const LPDIRECT3DDEVICEQ pd3dDevice, DxVehicle* pVehicle, const D3DXMATRIX& matCurPos, float fAlphaParent )
//{
//	// 최소 PixelShader 는 되야한다.
//	if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
//		return S_OK;
//
//	//	Render 시작
//	m_pSavedBaseSB->Capture();
//	m_pEffectBaseSB->Apply();	// State Block 셋팅.
//
//	int nGhostingIndex(m_dwUnit);
//	for ( DWORD i=0; i<m_dwGhostingNum; ++i, nGhostingIndex+=m_dwUnit )
//	{
//		const D3DXMATRIXA16* parrayBoneMatrices = pSkinChar->GetBoneMatricesForGhosting( nGhostingIndex );
//		if ( !parrayBoneMatrices )
//			break;
//
//		float fAlpha = static_cast<float>(m_dwGhostingNum-i) / (m_dwGhostingNum+1);
//		//pVehicle->RenderMaterialOpaque( pd3dDevice, fAlpha, parrayBoneMatrices, FALSE, FALSE, FALSE );
//		//pVehicle->RenderMaterialHardAlpha( pd3dDevice, fAlpha );
//		//pVehicle->RenderMaterialSoftAlpha( pd3dDevice, fAlpha );
//	}
//
//	m_pSavedBaseSB->Apply();
//
//
//
//	//HRESULT hr = S_OK;
//	//D3DXVECTOR3 vNormalUp;
//	//D3DXVECTOR3 vNormalDown;
//	//D3DXMATRIX	matWorld;
//	//D3DXMatrixIdentity ( &matWorld );
//
//	//DWORD	dwUnit;
//	//float	fAlpha;
//	//DWORD	GHOSTMAX	= m_dwGhostingNum*m_dwUnit;
//	//DWORD	dwColor		= 0xffffffff;
//	//DWORD	dwUnitTime	= pVehicle->GETUNITTIME();
//	//float	fOldCurTime	= pVehicle->GETCURTIME ();
//	//DWORD	dwKeyTime	= pVehicle->GETCURKEYTIME();
//
//	//// Note : 렌더할 고스팅이 없다면...
//	//if( !m_listGhosting.empty() )
//	//{
//	//	//	Render 시작
//	//	m_pSavedBaseSB->Capture();
//	//	{
//	//		if ( m_dwColor == 0x00ffffff )		// 흰색일 경우 색의 변화는 주지 않는다.
//	//		{
//	//			pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,	D3DTOP_DISABLE );
//	//		}
//
//	//		GHOSTINGLIST_ITER iter = m_listGhosting.begin();
//	//		for ( ; iter!=m_listGhosting.end(); ++iter )
//	//		{
//	//			if ( !(iter->m_Use) )		continue;
//
//	//			m_pEffectBaseSB->Apply();	// State Block 셋팅.
//
//	//			//	Note : Skin Animation.
//	//			pVehicle->SETCURTIME( (float)(iter->m_dwKeyTime*dwUnitTime) );
//	//			pVehicle->CalculateAniBone( iter->m_matCurAni );
//
//	//			fAlpha = (float)(GHOSTMAX-iter->m_UseNum);
//
//	//			dwColor		=	m_dwColor;
//	//			dwColor		+= (DWORD)((fAlpha*fAlphaParent/GHOSTMAX)*m_dwAlphaMax)<<24;
//	//			pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR, dwColor );
//
//
//
//	//			CHARSETTING sCharSetting;
//	//			sCharSetting.pMeshContainerBase = pVehicle->m_V_PartsArray[PART_V_BODY].m_rMeshContainer;
//	//			sCharSetting.pmtrlPiece			= pVehicle->m_V_PartsArray[PART_V_BODY].m_arrayMaterialPiece;
//	//			sCharSetting.pmtrlSpecular		= NULL;
//	//			sCharSetting.bWorldIdentity		= FALSE;
//	//			sCharSetting.emRDOP				= CTOP_BASE;
//
//	//			switch( pVehicle->m_sSkinEffData.m_bNewHeadDRAW )
//	//			{
//	//			case TRUE:
//	//				if( !pVehicle->m_V_PartsArray[PART_V_BODY].m_rSkinMesh )	break;
//	//				if( !pVehicle->m_V_PartsArray[PART_V_BODY].m_rMeshContainer )		break;
//
//	//				pd3dDevice->SetTexture( 0, pVehicle->m_sSkinEffData.m_rFaceTex );
//	//				sCharSetting.emRDOP		= CTOP_NEWTEX;
//	//				pVehicle->m_V_PartsArray[PART_V_BODY].m_rSkinMesh->DrawMeshContainer( pd3dDevice, sCharSetting, pVehicle->GetSkeletonMaintain() );
//	//				break;
//
//	//			case FALSE:
//	//				if( !pVehicle->m_V_PartsArray[PART_V_BODY].m_rSkinMesh )	break;
//	//				if( !pVehicle->m_V_PartsArray[PART_V_BODY].m_rMeshContainer )		break;
//
//	//				sCharSetting.emRDOP		= CTOP_BASE;
//	//				pVehicle->m_V_PartsArray[PART_V_BODY].m_rSkinMesh->SetDrawState( TRUE, FALSE, FALSE, FALSE );
//	//				pVehicle->m_V_PartsArray[PART_V_BODY].m_rSkinMesh->DrawMeshContainer( pd3dDevice, sCharSetting, pVehicle->GetSkeletonMaintain() );
//	//				break;
//	//			}
//
//	//			for ( int i=1; i<PART_V_SIZE; i++ )
//	//			{
//	//				if ( !pVehicle->m_V_PartsArray[i].m_rSkinMesh )	continue;
//	//				if ( !pVehicle->m_V_PartsArray[i].m_rMeshContainer )		continue;
//
//	//				CHARSETTING sCharSetting;
//	//				sCharSetting.pMeshContainerBase = pVehicle->m_V_PartsArray[i].m_rMeshContainer;
//	//				sCharSetting.pmtrlPiece			= pVehicle->m_V_PartsArray[i].m_arrayMaterialPiece;
//	//				sCharSetting.pmtrlSpecular		= NULL;
//	//				sCharSetting.bWorldIdentity		= FALSE;
//	//				sCharSetting.emRDOP				= CTOP_BASE;
//
//	//				pVehicle->m_V_PartsArray[i].m_rSkinMesh->SetDrawState( TRUE, FALSE, FALSE, FALSE );
//	//				pVehicle->m_V_PartsArray[i].m_rSkinMesh->DrawMeshContainer( pd3dDevice, sCharSetting, pVehicle->GetSkeletonMaintain() );
//	//			}
//	//		}
//	//	}
//	//	m_pSavedBaseSB->Apply();
//
//	//	pVehicle->SETCURTIME ( fOldCurTime );
//	//}
//
//
//	//if ( m_nPrevKeyTime != dwKeyTime )
//	//{
//	//	// Note : Count 를 올려서 한계에 차면 삭제시킴.
//	//	GHOSTINGLIST_ITER	iter = m_listGhosting.begin();
//	//	GHOSTINGLIST_ITER	iter_del;
//	//	for ( ; iter!=m_listGhosting.end(); )
//	//	{
//	//		iter->m_UseNum += 1;
//
//	//		iter_del = iter++;
//
//	//		if ( iter->m_UseNum >= GHOSTMAX )
//	//		{
//	//			m_listGhosting.erase ( iter_del );
//	//		}
//	//	}
//
//	//	m_nPrevKeyTime = dwKeyTime;
//
//	//	// 새로운 고스팅을 생성해도 될 경우.
//	//	if( m_bFrameMove )
//	//	{
//	//		if ( (m_dwFrame_1<=dwKeyTime) && (dwKeyTime<=m_dwFrame_2) )		// 범위 안에 들어오면 생성
//	//		{
//	//			// 새로운 것을 위한 작업 시작
//	//			GHOSTING_INFORMATION	sGhost_Information;
//
//	//			sGhost_Information.m_Use		= FALSE;
//	//			sGhost_Information.m_UseNum		= 0;
//	//			sGhost_Information.m_dwKeyTime	= dwKeyTime;
//	//			sGhost_Information.m_matCurAni	= matCurPos;
//
//	//			m_listGhosting.push_back ( sGhost_Information );
//	//		}
//	//	}
//	//	else
//	//	{
//	//		// 임시 수정판... 이상하게.. 위쪽의 erase가 제대로 안되는 것 같다.
//	//		m_listGhosting.clear();
//	//	}
//
//	//	// Note : 뿌려야 하는 때를 셋팅한다.
//	//	iter = m_listGhosting.begin();
//	//	for ( ; iter!=m_listGhosting.end(); ++iter )
//	//	{
//	//		dwUnit = iter->m_UseNum + 1;
//	//		dwUnit = dwUnit%m_dwUnit;
//	//		if ( dwUnit )	iter->m_Use = FALSE;
//	//		else			iter->m_Use = TRUE;
//	//	}
//	//}
//
//	return S_OK;
//}