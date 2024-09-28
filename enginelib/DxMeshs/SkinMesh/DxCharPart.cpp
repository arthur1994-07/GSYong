#include "pch.h"
#include <algorithm>

#include "../../../SigmaCore/String/StringUtils.h"

#include "../../DxTools/IrradianceVolume/NSLightMapAmbientColor.h"
#include "../../DxTools/MaterialSkin/DxMultiMaterialSkinMesh.h"
#include "../../DxTools/MaterialSkin/MaterialSkinMesh.h"
#include "../../DxTools/MaterialSkin/MaterialSkinDef.h"
#include "../../DxTools/Texture/NSTexCombine.h"
#include "../../DxTools/RENDERPARAM.h"
#include "../../DxEffect/char/DxEffChar.h"
#include "../../DxEffect/Char/DxEffCharData.h"
#include "../../DxEffect/Char/DxEffCharAmbient.h"
#include "../../DxEffect/Char/DxEffCharOverlay.h"
#include "../../DxEffect/Char/DxEffCharUserColor.h"
#include "../../DxEffect/Char/DxEffCharVisualMaterial.h"
#include "../../G-Logic/NSGameLogicInfo.h"
#include "../../PhysX/NSPhysX.h"

#include "./_new/DxSkinMesh9_Material.h"
#include "./DxSkinAniControlThreadNeedData.h"
#include "./NSSkinAniThread.h"
#include "./DxSkeletonMaintain.h"
#include "./DxSkinAniControl.h"

#include "DxCharPart.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


extern BOOL g_bCHAR_EDIT_PIECE_RENDER_FIXED;


//////////////////////////////////////////////////////////////////////////
DxPhysXCloth::DxPhysXCloth()
{

}

DxPhysXCloth::~DxPhysXCloth()
{

}

void DxPhysXCloth::Cleanup()
{
	m_vecSkinMeshForCloth.clear();
}

void DxPhysXCloth::Import( const DxSkinPiece* pSkinPiece )
{
	Cleanup();

	if ( !pSkinPiece )
		return;

	if ( !pSkinPiece->m_pmcMesh )
		return;

	if ( !pSkinPiece->m_pmcMesh->m_spMultiMaterialSkinMesh.get() )
		return;

	
	pSkinPiece->m_pmcMesh->m_spMultiMaterialSkinMesh->Export( m_vecSkinMeshForCloth );
}

MaterialSkinMeshForCloth* DxPhysXCloth::GetMaterialSkinMeshForCloth( DWORD dwIndex ) const
{
	// ASSERT 가 생기면 구조적 문제이다.
	// if 로 대처하지 말고 원인파악하여 수정하자.
	GASSERT( dwIndex < m_vecSkinMeshForCloth.size() );

	// 이 함수에 들어 왔다면 m_vecSkinMeshForCloth 값은 확실히 있어야 한다.
	// 아에 empty 라면 이 함수가 호출 자체가 되면 안된다. 그래야 한다.
	return m_vecSkinMeshForCloth[dwIndex].get();
}

BOOL DxPhysXCloth::IsValid_PxCloth( DWORD dwIndex ) const
{
	return GetMaterialSkinMeshForCloth( dwIndex )->IsValid();
}



//////////////////////////////////////////////////////////////////////////
DxCharPart::DxCharPart ()
    : m_rSkinMesh(NULL)
	, m_rSkinCharEffAniBase(NULL)
	, m_rSkeleton(NULL)
	, m_emWeaponWhereBack(EMPEACE_WEAPON_RSLOT)
    , m_cEffAmbientType(USETEXCOLOR)
    , m_cEffAmbientColor(1.f,1.f,1.f,1.f)

	, m_bMyChar(FALSE)
	, m_dwFlag(NULL)
	, m_dwFlags(NULL)
	, m_rMeshContainer(NULL)
	, m_bCompletedLINK(TRUE)

	, m_rSkinPiece(NULL)

	, m_dwMaterialNum(0)
	//, m_rSkinPiece->m_arrayMaterialPiece(NULL)

	, m_bRender(TRUE)
	, m_bAttackMode(TRUE)
	, m_pHandHeld(NULL)
	, m_pSlotHeld(NULL)

	, m_emHairHairOption(HAT_HAIR_BASE)		// 헤어 Part 에 영향을 준다. 내 자신의 렌더와는 상관없다.
	, m_strAddHairCps(_T(""))

	, m_rTraceBone(NULL)	

	, m_nGrindLevel(0)

	, m_vFlagConvert(0.f,0.f,1.f,1.f)

	, m_strUserBackSlot(_T(""))
	, m_bKeepUserBackSlot(FALSE)

	, m_dwHairColor(0L)
	, m_dwSubColor(0L) 
	, m_apOverlayTex(NULL)

	, m_scpEffectMaterialData(new EffectMaterialData)
	, m_pBackUpData(NULL)
{
	m_strNameCPS = _T("");
	m_stTextureName = _T("");
	//D3DXMatrixIdentity( &m_matCombine );

	//m_vecEFFECT.reserve(20);

	D3DXMatrixIdentity( &m_matStaticLinkBone );

	m_wwMoveParentBoneIDForSafeModeWeaponSlot.dwData = -1;
}

DxCharPart::~DxCharPart ()
{
	ReSetPart ();
}

void DxCharPart::ClearEffList ( )
{
    //// VisualMaterial 이 삭제된거라면..
    //if ( m_dwFlags & EMECF_VISUAL_MATERIAL )
    //{
    //    if ( m_pmcMesh )
    //        m_pmcMesh->GenerateSkinnedMeshNON_public( m_pd3dDevice );
    //}

	m_dwFlags = NULL;

	std::for_each ( m_vecEFFECT.begin(), m_vecEFFECT.end(), std_afunc::DeleteObject() );
	m_vecEFFECT.clear();
}

void DxCharPart::SumEffFlag ()
{
	m_dwFlags = NULL;

	for ( size_t n=0; n<m_vecEFFECT.size(); ++n )
	{
		m_dwFlags |= m_vecEFFECT[n]->GetFlag();
	}
}

//	Note : 신규 이펙트 생성시.
//
void DxCharPart::AddEffList ( DxEffChar* pEffChar )
{
	GASSERT(pEffChar);
    if (pEffChar)
    {
	    m_vecEFFECT.push_back( pEffChar );
	    std::sort ( m_vecEFFECT.begin(), m_vecEFFECT.end(), DXEFFCHAR_OPER() );
	    //	flag가 지정된 효과의 경우 flag를 합산.
	    m_dwFlags |= pEffChar->GetFlag();
    }
}

//	Note : 효과 생명이 끝났을때
//
void DxCharPart::DelEffList( LPDIRECT3DDEVICEQ pd3dDevice, DxEffChar* pEffChar )
{
	if ( !pEffChar )	return;

	DWORD dwFlag = pEffChar->GetFlag();
	DWORD dwOrder = pEffChar->GetStateOrder();

	GLEFFCHAR_VEC_ITER pos = std::lower_bound ( m_vecEFFECT.begin(), m_vecEFFECT.end(), dwOrder, DXEFFCHAR_OPER() );
	for ( ; pos!=m_vecEFFECT.end(); ++pos )
	{
		DxEffChar* pEFFECT = (*pos);
		if ( pEFFECT->GetStateOrder() > dwOrder )	break;	//	존제하지 않는다.
		if ( pEFFECT!=pEffChar )					continue;

		//	삭제.
		SAFE_DELETE(pEFFECT);
		m_vecEFFECT.erase(pos);
		break;	// 반복자로 할 경우 삭제시 유효하지 않을 수 있으므로 나가야한다.
	}

    //BOOL bUsedVisualMaterial = dwFlag & EMECF_VISUAL_MATERIAL;

	//	flag가 있었던 효과가 삭제될 경우 flag를 다시 합산.
	if ( dwFlag )		SumEffFlag ();

    //if ( bUsedVisualMaterial )
    //{
    //    // VisualMaterial 이 삭제된거라면..
    //    if ( !(m_dwFlags & EMECF_VISUAL_MATERIAL) )
    //    {
    //        if ( m_pmcMesh )
    //            m_pmcMesh->GenerateSkinnedMeshNON_public( pd3dDevice );
    //    }
    //}
}

//	Note : 강제로 삭제시, 지속효과 제거시 호출.
//
void DxCharPart::DelEffList ( const char* szEffFile )
{
	if ( szEffFile[0] == NULL )		return;
	
	DWORD dwFlag=NULL;

	for ( size_t n=0; n<m_vecEFFECT.size(); )
	{
		DxEffChar* pEFFECT = m_vecEFFECT[n];
		if ( pEFFECT->GetFileName() == szEffFile )
		{
			dwFlag |= pEFFECT->GetFlag();

			SAFE_DELETE(pEFFECT);
			m_vecEFFECT.erase(m_vecEFFECT.begin()+n);
		}
		else
		{
			++n;
		}
	}

    // 삭제된 Ambient가 있다면 다시 재정립.
    if ( dwFlag & EMECF_AMBIENT )
    {
        m_cEffAmbientType = USETEXCOLOR;
        m_cEffAmbientColor.r = 1.f;
        m_cEffAmbientColor.g = 1.f;
        m_cEffAmbientColor.b = 1.f;
        for ( size_t n=0; n<m_vecEFFECT.size(); ++n )
	    {
		    DxEffChar* pEFFECT = m_vecEFFECT[n];

            // Note : Ambient Color를 얻어서 셋팅한다.
            //          Overlay 와 VisualMaterial 등. 형상이 다른 것에서 같은 효과를 내기위해 셋팅한다.
            if ( pEFFECT->GetTypeID() == EMEFFCHAR_AMBIENT )
            {
                // [2011.03.31] 속도를 생각해서 dynamic_cast -> static_cast로 변경.
                //DxEffCharAmbient* pEffCharAmbient = dynamic_cast<DxEffCharAmbient*>( pEFFECT );
                DxEffCharAmbient* pEffCharAmbient = MIN_BOOST::polymorphic_downcast<DxEffCharAmbient*>( pEFFECT );
                m_cEffAmbientType = pEffCharAmbient->GetEffAmbientType();
                m_cEffAmbientColor = pEffCharAmbient->GetEffAmbientColor();
            }
	    }
    }

	//	flag가 있었던 효과가 삭제될 경우 flag를 다시 합산.
	if ( dwFlag )	SumEffFlag ();
}

//	Note : 지속효과 관리시 존제하는지 검사후 넣어줄때 사용.
//
BOOL DxCharPart::FindEffList ( const char* szEffFile )
{
	if ( szEffFile[0] == NULL )		return FALSE;

	for ( size_t n=0; n<m_vecEFFECT.size(); ++n )
	{
		DxEffChar* pEFFECT = m_vecEFFECT[n];
		if ( pEFFECT->GetFileName() == szEffFile )
		{
			return TRUE;
		}
	}

	return FALSE;
}

void DxCharPart::GetDefaultOverlayColor( DWORD& dwMainColor, DWORD& dwSubColor )
{
	for ( size_t n=0; n<m_vecEFFECT.size(); ++n )
	{
		DxEffChar* pEFFECT = m_vecEFFECT[n];
		if ( pEFFECT->GetTypeID() == EMEFFCHAR_OVERLAY )
		{
			EFFCHAR_PROPERTY_OVERLAY sProp = *(EFFCHAR_PROPERTY_OVERLAY*)((DxEffCharOverlay*)pEFFECT)->GetProperty();
			sProp.Assign( ((DxEffCharOverlay*)pEFFECT)->GetPropertyOverlay( sProp.m_nSelectOverlay ), sProp.m_dwSelectMaterial );

			dwMainColor = sProp.m_cColor1;
			dwSubColor = sProp.m_cColor2;
			return;
		}
        else if ( pEFFECT->GetTypeID() == EMEFFCHAR_VISUAL_MATERIAL )
		{
            // [2011.03.31] 속도를 생각해서 dynamic_cast -> static_cast로 변경.
            //DxEffCharVisualMaterial* pVisualMaterial = dynamic_cast<DxEffCharVisualMaterial*>(pEFFECT);
			DxEffCharVisualMaterial* pVisualMaterial = MIN_BOOST::polymorphic_downcast<DxEffCharVisualMaterial*>(pEFFECT);
            const D3DXVECTOR3& vColor1 = pVisualMaterial->GetUserColor1();
            const D3DXVECTOR3& vColor2 = pVisualMaterial->GetUserColor2();
			dwMainColor = D3DXCOLOR(vColor1.x,vColor1.y,vColor1.z,1.f);
			dwSubColor = D3DXCOLOR(vColor2.x,vColor2.y,vColor2.z,1.f);
			return;
		}
	}
	dwMainColor = D3DCOLOR_RGBA ( 0, 0, 0, 0 );
	//dwMainColor = D3DCOLOR_RGBA ( 0, 0, 0, 0 );
}

//void DxCharPart::SetPart ( DxSkinPiece* pSkinPiece, LPDIRECT3DDEVICEQ pd3dDevice, bool bAmbientUP )
BOOL DxCharPart::SetPart ( const DxSkinCharEffAniBase* pSkinCharEffAniBase, DxSkinPiece* pSkinPiece, LPDIRECT3DDEVICEQ pd3dDevice, bool bAmbientUP, const char* pSelfBodyEffect )
{
	m_pd3dDevice			= pd3dDevice;
	m_rSkinCharEffAniBase	= pSkinCharEffAniBase;

	// 같은 정보를 바꾸려하면 동작하지 않도록 한다.
	if ( m_sUsedLoadingChangeData.IsSame( pSkinPiece, bAmbientUP, pSelfBodyEffect ) )
	{
		return FALSE;
	}

	m_bCompletedLINK	= FALSE;		// 새로운것이 들어왔으니 바꿔야 한다.

	// 일단 BackUp 해 놓는다.
	m_sBackUpLoadingChangeData.Import( pSkinPiece, bAmbientUP, pSelfBodyEffect );

	//// 바로 필요해서 셋팅해놓음.
	//if( pSkinPiece )
	//{
	//	m_bTwoHandsWeapon = pSkinPiece->m_bTwoHandsWeapon;
	//}

	SetPart_Detail();


	// [ Thread Work shhan ]

 //   if ( !pSkinPiece ) return;

	//StringCchCopy ( m_szFileName, FILE_LENGTH, pSkinPiece->m_szFileName );

 //   m_pd3dDevice = pd3dDevice;
 //   m_pSkinMesh = pSkinPiece->m_skinMeshRes.get();
	//m_rSkeleton = pSkinPiece->m_skeletonRes.get();
	//m_emType = pSkinPiece->m_emType;
	//m_emWeaponWhereBack = pSkinPiece->m_emWeaponWhereBack;

	//m_pmcMesh = pSkinPiece->m_pmcMesh;
	//m_dwMaterialNum = pSkinPiece->m_dwMaterialNum;

	//if ( pSkinPiece->m_pMaterialPiece )
	//{
	//	SAFE_DELETE_ARRAY(m_pMaterialPiece);
	//	m_pMaterialPiece = new SMATERIAL_PIECE[m_dwMaterialNum];
	//	for ( DWORD i=0; i<m_dwMaterialNum; ++i )
	//	{
	//		m_pMaterialPiece[i] = pSkinPiece->m_pMaterialPiece[i];
	//	}
	//}

	//m_vecTrace = pSkinPiece->m_vecTrace;

	//ClearEffList ( );

	////	효과 복제.
	//size_t nSIZE = pSkinPiece->m_vecEFFECT.size();
	//for ( size_t n=0; n<nSIZE; ++n )
	//{
	//	DxEffChar* pEFFECT = pSkinPiece->m_vecEFFECT[n];
	//	
	//	DxEffChar* pNEW_EFFECT = pEFFECT->CloneInstance ( pd3dDevice, this, NULL );
 //       if (pNEW_EFFECT)
	//	    m_vecEFFECT.push_back ( pNEW_EFFECT );
	//}

	//m_dwFlag = pSkinPiece->m_dwFlag;

	//if( bAmbientUP )
	//{
	//	UINT i;
	//	for( i = 0; i < m_vecEFFECT.size(); i++ )
	//	{
 //           if ( !m_vecEFFECT[i] )
 //           {
 //               CDebugSet::ToLogFile ( _T("%s : m_vecEFECT is null."), m_szFileName );
 //               continue;
 //           }

	//		if( m_vecEFFECT[i]->GetFlag() == EMECF_AMBIENT )
	//		{			
	//			return;
	//		}
	//	}
	//	
	//	DxEffChar* pEffChar;
	//	pEffChar = DxEffCharMan::GetInstance().AddDefaultAmbient( this, NULL );
	//	AddEffList( pEffChar );
	//}

	return TRUE;
}

void DxCharPart::SetHairColor( DWORD dwColor, BOOL bMakeOverlayTex )	
{
	m_dwHairColor = dwColor; 

	if ( bMakeOverlayTex ) 
	{
		MakeOverlayTex();
	}
}

void DxCharPart::SetSubColor( DWORD dwColor, BOOL bMakeOverlayTex )	
{ 
	m_dwSubColor = dwColor;

	if ( bMakeOverlayTex ) 
	{
		MakeOverlayTex();
	}
}

void DxCharPart::MakeOverlayTex()
{
	// 1.1 도 안된다면 작업할 필요가 없다.
	if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
		return;

	// Texture 에 변화를 준다.
	for ( DWORD i=0; i<m_dwMaterialNum; ++i )
	{
		size_t nSIZE = m_vecEFFECT.size();
		for ( size_t n=0; n<nSIZE; ++n )
		{
			DxEffChar* pEFFECT = m_vecEFFECT[n];

			switch ( pEFFECT->GetTypeID() )
			{
			case EMEFFCHAR_USERCOLOR:
				{
					DxEffCharUserColor* pUserColor = MIN_BOOST::polymorphic_downcast<DxEffCharUserColor*>( pEFFECT );
					NSTexCombine::CreateUserColor
					( 
						m_pd3dDevice,
						i,
						&m_rSkinPiece->m_arrayMaterialPiece[i].m_textureRes,
						pUserColor,
						m_dwHairColor, 
						m_apOverlayTex[i]
					);
				}
				break;

			case EMEFFCHAR_OVERLAY:
				{
					DxEffCharOverlay* pOverlay = MIN_BOOST::polymorphic_downcast<DxEffCharOverlay*>( pEFFECT );
					NSTexCombine::CreateOverlay
					( 
						m_pd3dDevice,
						i,
						&m_rSkinPiece->m_arrayMaterialPiece[i].m_textureRes,
						pOverlay, 
						m_dwHairColor, 
						m_dwSubColor, 
						m_apOverlayTex[i]
					);
				}
				break;
			};
		}
	}
}

void DxCharPart::SetWorldBattleFlag( const TCHAR* pWorldBattleFlag )
{
	if ( !m_apOverlayTex )
		return;

	// Texture 에 변화를 준다.
	size_t nSIZE = m_vecEFFECT.size();
	for ( size_t n=0; n<nSIZE; ++n )
	{
		m_vecEFFECT[n]->SetWorldBattleFlag( m_apOverlayTex, pWorldBattleFlag );
	}
}

void DxCharPart::SetPart_Detail()
{
	if( !m_bCompletedLINK && m_sBackUpLoadingChangeData.m_rSkinPiece && m_sBackUpLoadingChangeData.m_rSkinPiece->m_bCompletedLOAD )
	{
		// BackUp 했던 정보를 다 넘겨준다.
		m_sUsedLoadingChangeData = m_sBackUpLoadingChangeData;

		ClearEffList();

		for ( DWORD i=0; i<m_dwMaterialNum; ++i )
		{
			SAFE_RELEASE ( m_apOverlayTex[i] );
		}

		m_rSkinPiece = m_sBackUpLoadingChangeData.m_rSkinPiece;
		m_strNameCPS = m_sUsedLoadingChangeData.m_rSkinPiece->m_szFileName;

		m_rSkinMesh = m_sUsedLoadingChangeData.m_rSkinPiece->m_skinMeshRes.get();
		m_rSkeleton =m_sUsedLoadingChangeData. m_rSkinPiece->m_skeletonRes.get();
		//m_rTraceBone = m_rSkinPiece->m_pTraceBone;
		//m_emType = m_rSkinPiece->m_emType;						// SetPart에서 해준다. RHAND->LHAND 때문에 이것을 쓰면 안된다.
		m_emWeaponWhereBack = m_sUsedLoadingChangeData.m_rSkinPiece->m_emWeaponWhereBack;
		//m_bTwoHandsWeapon = m_rSkinPiece->m_bTwoHandsWeapon;
		m_strUserBackSlot = m_sUsedLoadingChangeData.m_rSkinPiece->m_strUserBackSlot;
		m_matUserBackSlot = m_sUsedLoadingChangeData.m_rSkinPiece->m_matUserBackSlot;
		m_bKeepUserBackSlot = m_sUsedLoadingChangeData.m_rSkinPiece->m_bKeepUserBackSlot;

		m_emHairHairOption = m_sUsedLoadingChangeData.m_rSkinPiece->GetHatHairOption();
		m_strAddHairCps = m_sUsedLoadingChangeData.m_rSkinPiece->GetAddHairCps();

		m_rMeshContainer = m_sUsedLoadingChangeData.m_rSkinPiece->m_pmcMesh;


		m_dwMaterialNum = m_sUsedLoadingChangeData.m_rSkinPiece->m_dwMaterialNum;

		//m_rSkinPiece->m_arrayMaterialPiece = m_sUsedLoadingChangeData.m_rSkinPiece->m_arrayMaterialPiece;

		SAFE_DELETE_ARRAY( m_apOverlayTex );
		if ( m_dwMaterialNum > 0 )
		{
			m_apOverlayTex = new LPDIRECT3DTEXTUREQ[m_dwMaterialNum];
			for ( DWORD i=0; i<m_dwMaterialNum; ++i )
			{
				m_apOverlayTex[i] = NULL;
			}
		}

		m_vecTrace = m_sUsedLoadingChangeData.m_rSkinPiece->m_vecTrace;


		//////////////////////////////////////////////////////////////////////////
		// 어떤 이유였던지 이곳으로는 늦게 들어올 수 있다.
		// 하지만 게임중 m_vecEFFECT 효과가 붙을 수 있기 때문에 유지시켜주도록 하자.
		// 다른 이유일 수도 있다. 현재는 그렇게 보임.
		//////////////////////////////////////////////////////////////////////////
		// 개인적인 효과 백업
		{
			GLEFFCHAR_VEC vecEFFECT;
			{
				size_t nSIZE = m_vecEFFECT.size();
				for ( size_t n=0; n<nSIZE; ++n )
				{
					DxEffChar* pEFFECT = m_vecEFFECT[n];
					DxEffChar* pNEW_EFFECT = pEFFECT->CloneInstance ( m_pd3dDevice, this, NULL, m_dwMaterialNum );

					if( pNEW_EFFECT )
					{
						vecEFFECT.push_back ( pNEW_EFFECT );

						if( m_bMyChar )
						{
							pNEW_EFFECT->SetMyChar();
						}
					}
				}
			}

			// 개인적인 효과 삭제
			{
				ClearEffList ();
			}

			// 효과 복원
			m_vecEFFECT = vecEFFECT;
			vecEFFECT.clear();
		}


		//////////////////////////////////////////////////////////////////////////
		//	효과 복제.
		size_t nSIZE = m_sUsedLoadingChangeData.m_rSkinPiece->m_vecEFFECT.size();
		for ( size_t n=0; n<nSIZE; ++n )
		{
			DxEffChar* pEFFECT = m_sUsedLoadingChangeData.m_rSkinPiece->m_vecEFFECT[n];

			// 새맵이면 작동되지 않도록 한다.
			if ( pEFFECT->GetTypeID() == EMEFFCHAR_AMBIENT )
			{
				// Material System 을 쓸 경우 EMEFFCHAR_AMBIENT 는 동작되지 않도록 한다.
				// LightMapAmbientColor 가 활성화되어 있으면(새로운맵), EMEFFCHAR_AMBIENT 는 DxCharPart 에서 사용되지 않도록 한다.
				if ( NSLightMapAmbientColor::g_bActiveLightMapAmbientColor || RENDERPARAM::g_emCharacterQulity >= TnL_CHAR_PS_1_1 )
				{
					// 조건을 좀 더 체크하겠다.!!!!

					// [shhan][2014.08.04] 속도를 생각해서 dynamic_cast -> static_cast로 변경.
					//DxEffCharAmbient* pEffCharAmbient = dynamic_cast<DxEffCharAmbient*>( pEFFECT );
					DxEffCharAmbient* pEffCharAmbient = MIN_BOOST::polymorphic_downcast<DxEffCharAmbient*>( pEFFECT );
					if ( pEffCharAmbient )
					{
						// USETEXCOLOR 만 효과 복제를 하고 아닐경우는 continue 한다.
						if ( pEffCharAmbient->GetEffAmbientType() == 0 )
						{
							continue;
						}
					}
					else
					{
						continue;
					}
				}
			}

			DxEffChar* pNEW_EFFECT = pEFFECT->CloneInstance ( m_pd3dDevice, this, NULL, m_dwMaterialNum );

			if( pNEW_EFFECT )
			{
				m_vecEFFECT.push_back ( pNEW_EFFECT );

				if( m_bMyChar )
				{
					pNEW_EFFECT->SetMyChar();
				}
			}
		}
		std::sort( m_vecEFFECT.begin(), m_vecEFFECT.end(), DXEFFCHAR_OPER() );

		m_dwFlag = m_sUsedLoadingChangeData.m_rSkinPiece->m_dwFlag;

		SumEffFlag();
		AddDefaultAmbient();

		// 코스튬 기본 색 입히기.
		{
			DWORD dwMainColor = 0, dwSubColor = 0;
			dwMainColor = GetHairColor();
			dwSubColor = GetSubColor();
			if ( dwMainColor == D3DCOLOR_RGBA ( 0, 0, 0, 0 ) && dwSubColor == D3DCOLOR_RGBA ( 0, 0, 0, 0 ) )
			{
				GetDefaultOverlayColor( dwMainColor, dwSubColor );

				if( dwMainColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 ) && dwSubColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 ) )
				{
					SetHairColor( dwMainColor, FALSE );
					SetSubColor( dwSubColor, FALSE );
				}
			}
		}

		// Texture 에 변화를 준다.
		MakeOverlayTex();
		SetWorldBattleFlag( NSGameLogicInfo::g_strWinnerCountryWB.c_str() );
		//CreateEffRenderTarget();

		if ( m_sUsedLoadingChangeData.m_rSelfBodyEffect )
		{
			D3DXVECTOR3 vDirect(0.f,0.f,1.f);
			DxEffcharDataMan::GetInstance().PutEffect ( this, m_sUsedLoadingChangeData.m_rSelfBodyEffect, &vDirect );
		}

		// m_pBackUpDataArray는 참조만한다;
		if ( m_pBackUpData )
		{
			// rEffCharDataArrayBackup->SetPart ( pCharPart, pd3dDevice, pDir, fTarDir );
			D3DXVECTOR3 vDirect(0.f,0.f,1.f);
			m_pBackUpData->SetPart ( this, m_pd3dDevice, &vDirect );
			m_pBackUpData = NULL;
		}
		
		// SafeMode 시 무기가 EMPEACE_WEAPON_WHIP_SLOT, EMPEACE_WEAPON_BOX_SLOT, EMPEACE_WEAPON_UMBRELLA_SLOT 경우 미리 본의 위치를 가지고 있도록 한다.
		switch( m_emWeaponWhereBack )
		{
		case EMPEACE_WEAPON_WHIP_SLOT:
		case EMPEACE_WEAPON_BOX_SLOT:
		case EMPEACE_WEAPON_UMBRELLA_SLOT:
			{
				int nSlotIndexA = m_rSkeleton->GetReferenceIndex_t( SKINTYPE_STING::m_szSTART_SKIN_BONE[m_emWeaponWhereBack] );
				int nSlotIndexB = m_rSkeleton->GetReferenceIndex_t( SKINTYPE_STING::m_szWHEREBACKTYPE[m_emWeaponWhereBack] );
				if ( nSlotIndexA != -1 && nSlotIndexB != -1 )
				{
					m_wwMoveParentBoneIDForSafeModeWeaponSlot.wA = nSlotIndexA;
					m_wwMoveParentBoneIDForSafeModeWeaponSlot.wB = nSlotIndexB;
				}
			}
			break;
		default:
			m_wwMoveParentBoneIDForSafeModeWeaponSlot.dwData = -1;
			break;
		};

		//// 속성공격 몹 셋팅.
		//if( m_bAttackBlow_Load )
		//{
		//	m_bAttackBlow_Load = FALSE;
		//	GLEFFCHAR_VEC_ITER pos = m_vecEFFECT.begin();
		//	for ( ; pos!=m_vecEFFECT.end(); ++pos )
		//	{
		//		DxEffChar* pEFFECT = (*pos);
		//		pEFFECT->SetColor_Line2BoneForBlow( m_dwAttackBlow_Color1, m_dwAttackBlow_Color2 );
		//		pEFFECT->SetChange_EffSkin_b( m_strAttackBlow_EffSkin_b );
		//	}
		//}

		//// CharPart의 Matrix와 DxBoneTransOptimize의 Matrix를 연결시킨다.
		//if( m_prBoneMatrixPtrs )
		//{
		//	SAFE_DELETE_ARRAY( m_prBoneMatrixPtrs );
		//}
		//m_pSkinMesh->SetupBoneMatrixPointers( m_rBoneTransOptimize, &m_prBoneMatrixPtrs, m_rSkinPiece->m_szMeshName );        

		// 형상이 변경되면 지워줘야 하는 Data
		m_spCharPartBoneMatrices.reset();
		m_spPhysXCloth.reset();

		m_bCompletedLINK = TRUE;

		// [shhan][2015.06.02]
		// chf 상 셋팅시 맞았지만. cps 그 후에 변경해버리면, 알맞지 않은 bone 이 셋팅과 연결이 되어 문제가 된다.
		// 이 때는 그냥 셋팅을 안하는게 맞다.
		//
		// [shhan][2015.06.08]
		// Link가 아닌 Skin 만 작업해야 한다.
		// Link가 다 문제 있다고 나와버려서 문제가 되어버린다.
		//
		if ( m_rMeshContainer && m_rMeshContainer->pSkinInfo && 
			m_rSkinPiece && m_rSkinPiece->GetBoneName() && m_rSkinPiece->GetFileName() &&
			m_rSkinCharEffAniBase && m_rSkinCharEffAniBase->GetBoneName() && m_rSkinCharEffAniBase->GetFileName() )
		{
			CString strFile1( m_rSkinPiece->GetBoneName() );
			strFile1.MakeLower();

			CString strFile2( m_rSkinCharEffAniBase->GetBoneName() );
			strFile2.MakeLower();

			if( _tcscmp(strFile1.GetString(),strFile2.GetString()) != 0 )
			{
				TSTRING strName(_T("chf : "));
				strName += m_rSkinCharEffAniBase->GetFileName();
				strName += _T(" - bone name : ");
				strName += m_rSkinCharEffAniBase->GetBoneName();
				strName += _T(" , cps : ");
				strName += m_rSkinPiece->GetFileName() ? m_rSkinPiece->GetFileName() : _T("");
				strName += _T(" - bone name : ");
				strName += m_rSkinPiece->GetBoneName() ? m_rSkinPiece->GetBoneName() : _T("");

				CDebugSet::ToLogFile( strName );

				GASSERT( 0 && _T("DxSkinChar::SetCharData - chf 의 bone 과 cps 의 bone 이 다릅니다.") );

				// 문제가 있으므로 빼 버린다.
				ReSetPart();
			}
		}
	}
}

void DxCharPart::SetLODPart_Detail()
{
 //   if ( m_sUsedLoadingChangeData.m_rSkinPiece->m_skinMeshRes.IsValid() )
 //   {
 //       m_rSkinMesh = m_sUsedLoadingChangeData.m_rSkinPiece->m_skinMeshRes.get();
 //   }
 //   else
 //   {
 //       m_rSkinMesh = 0;
 //   }

 //   if ( m_sUsedLoadingChangeData.m_rSkinPiece->m_skeletonRes.IsValid() )
 //   {
 //       m_rSkeleton = m_sUsedLoadingChangeData.m_rSkinPiece->m_skeletonRes.get();
 //   }
 //   else
 //   {
 //       m_rSkeleton = 0;
 //   }    

 //   m_rMeshContainer = m_sUsedLoadingChangeData.m_rSkinPiece->m_pmcMesh;

	//for ( DWORD i=0; i<m_dwMaterialNum; ++i )
	//{
	//	SAFE_RELEASE ( m_apOverlayTex[i] );
	//}

 //   //SAFE_DELETE_ARRAY(m_rSkinPiece->m_arrayMaterialPiece);

 //   m_dwMaterialNum = m_sUsedLoadingChangeData.m_rSkinPiece->m_dwMaterialNum;

 //   //if( m_sUsedLoadingChangeData.m_rSkinPiece->m_rSkinPiece->m_arrayMaterialPiece )
 //   //{
 //   //    m_rSkinPiece->m_arrayMaterialPiece = new SMATERIAL_PIECE[m_dwMaterialNum];
 //   //    for ( DWORD i=0; i<m_dwMaterialNum; ++i )
 //   //    {
 //   //        m_rSkinPiece->m_arrayMaterialPiece[i] = m_sUsedLoadingChangeData.m_rSkinPiece->m_rSkinPiece->m_arrayMaterialPiece[i];
 //   //    }
 //   //}

	//SAFE_DELETE_ARRAY( m_apOverlayTex );
	//m_apOverlayTex = new LPDIRECT3DTEXTUREQ[m_dwMaterialNum];
	//for ( DWORD i=0; i<m_dwMaterialNum; ++i )
	//{
	//	m_apOverlayTex[i] = NULL;
	//}

 //   GLEFFCHAR_VEC_ITER it = m_vecEFFECT.begin();

 //   for ( ; it != m_vecEFFECT.end(); ++it)
 //   {
 //       DxEffChar* pEFFECT = *it;

 //       if (pEFFECT)
 //       {
 //           DxEffChar* pNEW_EFFECT = pEFFECT->CloneInstance(m_pd3dDevice, this, NULL);

 //           if (pNEW_EFFECT)
 //           {
 //               *it = pNEW_EFFECT;

 //               if( m_bMyChar )
 //               {
 //                   pNEW_EFFECT->SetMyChar();
 //               }
 //           }

 //           SAFE_DELETE(pEFFECT);
 //       }
 //   }               

	//// Texture 에 변화를 준다.
	//MakeOverlayTex();

 //   if ( m_sUsedLoadingChangeData.m_rSelfBodyEffect )
 //   {
 //       D3DXVECTOR3 vDirect(0.f,0.f,1.f);
 //       DxEffcharDataMan::GetInstance().PutEffect ( this, m_sUsedLoadingChangeData.m_rSelfBodyEffect, &vDirect );
 //   }

	//// 형상이 변경되면 지워줘야 하는 Data
	//m_spCharPartBoneMatrices.reset();
	//m_spPhysXCloth.reset();
}

void DxCharPart::ReSetPart_Detail()
{
	m_bCompletedLINK = FALSE;

	m_rMeshContainer = NULL;

	// rm #906, rm #1213
	// 이걸 지워주지 않으면 CalculateVertexInflu 에서 m_vecTrace 값은 있고,
	// m_rMeshContainer 값은 없어서 vVert 값을 셋팅 하지 않는다.
	// m_vecTrace 가 있다면 m_rMeshContainer 가 무조건 있다고 생각하도록 한다.
	// GetTracor 체크가 성공한다는 건 CalculateVertexInflu 도 무조건 문제가 없이 vVert 값을 넘겨줄 수 있다는 것이다.
	m_vecTrace.clear();

	ClearEffList();

	for ( DWORD i=0; i<m_dwMaterialNum; ++i )
	{
		SAFE_RELEASE ( m_apOverlayTex[i] );
	}
	SAFE_DELETE_ARRAY( m_apOverlayTex );
	//SAFE_DELETE_ARRAY(m_rSkinPiece->m_arrayMaterialPiece);

	m_dwMaterialNum = 0;

	m_emHairHairOption = HAT_HAIR_BASE;
	m_strAddHairCps = _T("");

	m_spCharPartBoneMatrices.reset();
	m_spPhysXCloth.reset();

	SetPart_Detail();
}

void DxCharPart::AddDefaultAmbient()
{
	// Fixed 에서만 밝게 해주도록 한다.
	if ( RENDERPARAM::g_emCharacterQulity >= TnL_CHAR_PS_1_1 )
		return;

	if ( m_sUsedLoadingChangeData.m_bAmbientON )
	{
		UINT i;
		for( i = 0; i < m_vecEFFECT.size(); i++ )
		{
			if ( !m_vecEFFECT[i] )
				continue;

			if( m_vecEFFECT[i]->GetFlag() == EMECF_AMBIENT )
			{			
				return;
			}
		}

		DxEffChar* pEffChar;
		pEffChar = DxEffCharMan::GetInstance().AddDefaultAmbient( this, NULL );
		AddEffList( pEffChar );
	}
}

void DxCharPart::ReSetPart ()
{
	m_sUsedLoadingChangeData.CleanUp();
	m_sBackUpLoadingChangeData.CleanUp();

	ClearEffList ();

	if( !m_strNameCPS.empty() )
	{
		//DxSkinPieceContainer::GetInstance( pd3dDevice )->ReleasePiece ( m_strNameCPS.c_str() );	// 굳이 이 작업을 할 필요가 없다.
		//																							// 한다면 동기화 시켜줘야 한다.
		m_strNameCPS = _T("");
	}

	m_rSkinCharEffAniBase = NULL;
	m_rSkinPiece = NULL;
	m_rSkinMesh = NULL;
	m_rSkeleton = NULL;
	m_rMeshContainer = NULL;
	m_bCompletedLINK = TRUE;		// 작업할것이 없으므로 완료로 셋팅한다.

	// rm #906, rm #1213
	// 이걸 지워주지 않으면 CalculateVertexInflu 에서 m_vecTrace 값은 있고,
	// m_rMeshContainer 값은 없어서 vVert 값을 셋팅 하지 않는다.
	// m_vecTrace 가 있다면 m_rMeshContainer 가 무조건 있다고 생각하도록 한다.
	// GetTracor 체크가 성공한다는 건 CalculateVertexInflu 도 무조건 문제가 없이 vVert 값을 넘겨줄 수 있다는 것이다.
	m_vecTrace.clear();

	//SAFE_DELETE_ARRAY(m_rSkinPiece->m_arrayMaterialPiece);

	for ( DWORD i=0; i<m_dwMaterialNum; ++i )
	{
		SAFE_RELEASE ( m_apOverlayTex[i] );
	}
	SAFE_DELETE_ARRAY( m_apOverlayTex );

	m_dwMaterialNum = 0;

	m_emHairHairOption = HAT_HAIR_BASE;
	m_strAddHairCps = _T("");

	m_spCharPartBoneMatrices.reset();
	m_spPhysXCloth.reset();
}

BOOL DxCharPart::IsMeshContainer()
{
	if( m_rMeshContainer )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

HRESULT DxCharPart::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
    m_pd3dDevice = pd3dDevice;

	size_t nSIZE = m_vecEFFECT.size();
	for ( size_t n=0; n<nSIZE; ++n )
	{
		DxEffChar* pEFFECT = m_vecEFFECT[n];
		pEFFECT->InitDeviceObjects ( pd3dDevice );
	}

	return S_OK;
}

HRESULT DxCharPart::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	size_t nSIZE = m_vecEFFECT.size();
	for ( size_t n=0; n<nSIZE; ++n )
	{
		DxEffChar* pEFFECT = m_vecEFFECT[n];
		pEFFECT->RestoreDeviceObjects ( pd3dDevice );
	}

	// Release 를 했기 때문에 재생성이 필요하다.
	MakeOverlayTex();

	return S_OK;
}

HRESULT DxCharPart::InvalidateDeviceObjects ()
{
	size_t nSIZE = m_vecEFFECT.size();
	for ( size_t n=0; n<nSIZE; ++n )
	{
		DxEffChar* pEFFECT = m_vecEFFECT[n];
		pEFFECT->InvalidateDeviceObjects ();
	}

	// D3DPOOL_DEFAULT 로 만들어졌을 수도 있다.
	for ( DWORD i=0; i<m_dwMaterialNum; ++i )
	{
		SAFE_RELEASE ( m_apOverlayTex[i] );
	}

	return S_OK;
}

HRESULT DxCharPart::DeleteDeviceObjects()
{
	size_t nSIZE = m_vecEFFECT.size();
	for ( size_t n=0; n<nSIZE; ++n )
	{
		DxEffChar* pEFFECT = m_vecEFFECT[n];
		pEFFECT->DeleteDeviceObjects ();
	}

	ReSetPart();

	return S_OK;
}

HRESULT DxCharPart::FrameMove ( float fTime, float fETime, std::tr1::shared_ptr<DxSkinAniControl>& spSkinAniControlThread, BOOL bNoneOverFrame )
{
	SetPart_Detail();

	EMANI_MAINTYPE MType = spSkinAniControlThread->GETCURMTYPE();
	EMANI_SUBTYPE SType = spSkinAniControlThread->GETCURSTYPE();
	DWORD dwKeyTime = spSkinAniControlThread->GETCURKEYTIME();

	//	Note : 이팩트 업데이트.
	//
	for ( size_t n=0; n<m_vecEFFECT.size(); )
	{
		DxEffChar* pEFFECT = m_vecEFFECT[n];

		pEFFECT->SetCurrentAni ( MType, SType, dwKeyTime );
		pEFFECT->FrameMove ( fTime, fETime );

		//	Note : 종료시점 도달시에 Effect 제거.
		if ( pEFFECT->IsEnd() )
		{
 			DWORD dwFlag = pEFFECT->GetFlag();
			SAFE_DELETE(pEFFECT);
			m_vecEFFECT.erase ( m_vecEFFECT.begin()+n );

			if ( dwFlag )
				SumEffFlag();
		}
		else
		{
			++n;
		}
	}

	SumEffFlag();

	if ( !m_rMeshContainer )
		return S_OK;

	if ( !m_spCharPartBoneMatrices.get() )
	{
		m_spCharPartBoneMatrices = std::tr1::shared_ptr<CharPartBoneMatrices>(new CharPartBoneMatrices(*m_rMeshContainer, spSkinAniControlThread));
	}

	// Fps가 느릴 경우, 결국 매번 Prev 과 현재 Key 가 같기 때문에 쓸모가 없다. 
	// 캐릭터 애니메이션은 보통 30fps.
	//if ( spSkinAniControlThread->GetPrevKeyFrame() != dwKeyTime )
	{
		NSSkinAniThread::AddCharPartBoneMatrices( m_spCharPartBoneMatrices, bNoneOverFrame );
	}

	// [shhan][2015.04.29] m_Tracor 값은 순서 문제도 발생한다.
	//						일단 중복계산을 해야겠다. 좋은 방법을 생각해보자.
	//
	{
		// FALSE 라면 무언가 이상한 문제가 생긴거다.
		// 혹은 처음 생성되었고 Thread 작업중이라 값이 없을 수 있다.
		const DxSkeletonMaintain* pSkeleton = spSkinAniControlThread->GetSkeletonMaintain();
		if ( pSkeleton->IsActiveBone() )
		{
			size_t nSize = m_vecTrace.size();
			for ( size_t n=0; n<nSize; ++n )
			{
				STRACOR &sTracor = m_vecTrace[n];

				m_rMeshContainer->CalculateVertexInfluences
				( 
					sTracor.m_sVertexInflu, 
					sTracor.m_vPos, 
					sTracor.m_vNormal, 
					&sTracor.m_BoneMatrice, 
					FALSE, 
					*pSkeleton, 
					m_matStaticLinkBone 
				);
			}
		}
	}

	//
	//// [shhan][2015.04.22] rm #906, rm #1213
	////						FrameMove 의 경우는 매프레임 계산 되지만, Render 의 경우는 안 보이면 계산이 되지 않는다.
	////						그런데 m_vecTrace 경우 m_vPos 초기값은 FLT_MAX 이고, Render 가 안된 상태에서, 그 유저가 어떠한 Effect 를 맞았을 경우,
	////						위치값을 m_vecTrace 으로 찾는데 FLT_MAX 값이 들어가 있어서 문제가 되었다.
	////						이 문제되는 값으로 DxEffectMesh::Render 에서 pLandMan->IsCollisionEX 를 하고, 결국 잘못된 입력값으로 fps 가 느려지는 현상이 발생하였다.
	////
	////						m_vecTrace 경우 m_vPos 초기값을 Zero 로 일단 변경.
	////						
	////						초기값을 Zero 로 셋팅하면 Effect 가 원점에서 생겨서 문제가 될 것이므로, Bone 의 Root 값으로 셋팅하여 렌더링 오류도 없도록 한다.
	////						( 유저가 들어오자마자 FrameMove 도 하기전에 Effect 를 맞는다면, 원점에서 나올 수는 있다. (극히 낮은확률) )
	////
	////						결론적으로 Render 에서 sTracor.m_vPos 작업을 하지만 Render가 Culling 으로 실행안 될 수 있어서 
	////						FrameMove 상에서 초기값을 셋팅한다.
	////
	//size_t nSize = m_vecTrace.size();
	//for ( size_t n=0; n<nSize; ++n )
	//{
	//	STRACOR &sTracor = m_vecTrace[n];
	//	sTracor.m_vPos.x = spSkinAniControlThread->GetWorld()._41;
	//	sTracor.m_vPos.y = spSkinAniControlThread->GetWorld()._42;
	//	sTracor.m_vPos.z = spSkinAniControlThread->GetWorld()._43;
	//}
	

	return S_OK;
}

HRESULT DxCharPart::DrawMeshContainer ( LPDIRECT3DDEVICEQ pd3dDevice, 
									   BOOL bOriginDraw, 
									   BOOL bAlpha, 
									   BOOL bShadow, 
									   BOOL bAlphaTex, 
									   float fHeight, 
									   const DxSkeletonMaintain* pSkeleton )
{
	if ( !IsEnableRender() )
		return S_OK;		

	HRESULT hr = S_OK;

	if ( m_rSkinMesh )
	{
		m_rSkinMesh->SetDrawState( bOriginDraw, bAlpha, bShadow, bAlphaTex, FALSE, fHeight );

		if( m_rMeshContainer->pSkinInfo )	
		{
			CHARSETTING sCharSetting;
			sCharSetting.pMeshContainerBase = m_rMeshContainer;
			sCharSetting.pmtrlPiece			= m_rSkinPiece->m_arrayMaterialPiece;
			sCharSetting.pmtrlSpecular		= NULL;
			sCharSetting.bWorldIdentity		= FALSE;
			sCharSetting.emRDOP				= CTOP_BASE;
			sCharSetting.m_r_apOverlayTex	= m_apOverlayTex;

			m_rSkinMesh->DrawMeshContainer( pd3dDevice, sCharSetting, pSkeleton, m_matStaticLinkBone, m_spCharPartBoneMatrices->GetBoneMatrices() );
		}
		else		
		{
			m_rSkinMesh->DrawFrameMesh( pd3dDevice, m_rMeshContainer, m_rSkinPiece->m_arrayMaterialPiece, &m_matStaticLinkBone, m_apOverlayTex );
		}

		return S_OK;
	}

	return E_FAIL;
}

HRESULT DxCharPart::Render( LPDIRECT3DDEVICEQ pd3dDevice, 
						   const DxSkeletonMaintain* pSkeleton,
						   float* parrayfWeaponScale,
						   SKINEFFDATA& sSKINEFFDATA, 
						   const D3DXMATRIX& matWorld, 
						   const SKIN_SPEC_SSS_INFO& sSkinInfo, 
						   BOOL bShadow/* =FALSE */, 
						   BOOL bEff/* =TRUE */ )
{
	if ( !IsEnableRender() )
		return S_OK;

	if ( RENDERPARAM::g_emCharacterQulity > TnL_CHAR_FIXED_HIGH )
		return S_OK;

	RenderFixed( pd3dDevice, pSkeleton, parrayfWeaponScale, sSKINEFFDATA, matWorld, bShadow, bEff );

	return S_OK;
}

void DxCharPart::RenderFixed( LPDIRECT3DDEVICEQ pd3dDevice, 
							 const DxSkeletonMaintain* pSkeleton, 
							 float* parrayfWeaponScale,
							 SKINEFFDATA& sSKINEFFDATA, 
							 const D3DXMATRIX& matWorld, 
							 BOOL bShadow, 
							 BOOL bEff )
{
	BOOL bOriginDraw(TRUE);	
	BOOL bCrashRender(FALSE);
	BOOL bAlphaTex(TRUE);
	BOOL bAlpha(FALSE);

	if( (m_dwFlags&EMECF_NORMALMAP) && (m_dwFlags&EMECF_NONHLSL) )	
		bCrashRender = TRUE;	// 렌더시 충돌이 일어난다.

	switch( RENDERPARAM::emCharRenderTYPE )
	{
	case EMCRT_SOFTWARE:
	case EMCRT_NORMAL:
	case EMCRT_VERTEX:
		if( m_dwFlags & EMECF_ORGINNON )	bOriginDraw = FALSE;
		if( m_dwFlags & EMECF_NORMALMAP )	bOriginDraw = FALSE;
		break;
	case EMCRT_PIXEL:
		if( m_dwFlags & EMECF_NORMALMAP )	bOriginDraw = FALSE;
		break;
	}

	if ( m_dwFlags&(EMECF_NOALPHA|EMECF_ALPHA) )
    {
        bAlphaTex = FALSE;
        bAlpha = TRUE;
    }	

	//if ( m_dwFlags&(EMECF_NOALPHA|EMECF_ALPHA) )
    //    bAlpha = TRUE;

	// Note : Bone 을 셋팅해 놓는다.
	//		 좋지 않은 코드.
	for ( DWORD i=0; i<m_vecEFFECT.size(); ++i )
	{
		m_vecEFFECT[i]->SetSkeleton( pSkeleton );
	}

	//	Note : 일반 메시일 경우 Bone에 링크되어 있는걸 가정.
	//		연결 설정된 본과 다른 곳에 사용된다는걸 가정하여 본을 다시 연결	시켜줌.
	//
	if ( !m_rMeshContainer->pSkinInfo )
	{
		//	Note : 영향 받는 본의 matrix 포인터를 다시 찾는다.
		//			m_pmcMesh->m_szBoneNames[0];	//	[LOCAL]
		//			m_pmcMesh->m_szBoneNames[1];	//	[LOCAL]
		//			m_pmcMesh->m_szBoneNames[2];	//	[COMBINED] <-- 이것만 실제로 BONE에서 영향을 받는것.
		m_pHandHeld = pSkeleton->FindBone ( m_rMeshContainer->m_strBoneName );

		switch( m_emWeaponWhereBack )
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
			m_pSlotHeld = pSkeleton->FindBone ( SKINTYPE_STING::m_szWHEREBACKTYPE[m_emWeaponWhereBack] );
			break;
		case EMPEACE_WEAPON_USER_SLOT:
			m_pSlotHeld = pSkeleton->FindBone ( m_strUserBackSlot );
			break;
		};

		// Note : 활이 등에 붙는 위치의 이름이 바뀌었는데 .. 예전 이름도 아직 사용중이라서 이렇게 해야한다. < 극강부 들어가면 삭제해야함 >
		if( !m_pSlotHeld && (m_emWeaponWhereBack==EMPEACE_WEAPON_WHAL_SLOT) )
		{
			m_pSlotHeld = pSkeleton->FindBone ( _T("slot_lhand") );
		}

		// m_bUserBackSlot 가 TRUE 이고, Weapon Use Slot 으로 셋팅 되어 있으면, UserBackSlot 으로만 동작되도록 한다.
		if( m_bKeepUserBackSlot && m_emWeaponWhereBack == EMPEACE_WEAPON_USER_SLOT && m_pSlotHeld )
		{
			m_matStaticLinkBone = m_pSlotHeld->m_matCombined;
			D3DXMatrixMultiply( &m_matStaticLinkBone, &m_matUserBackSlot, &m_matStaticLinkBone );
		}
		else
		{
			if ( m_bAttackMode && m_pHandHeld )	
			{
				m_matStaticLinkBone = m_pHandHeld->m_matCombined;
				//m_pmcMesh->m_ppBoneMatrixPtrs[2] = &const_cast<DxBoneTransMaintain*>(m_pHandHeld)->m_matCombined;
				//m_pmcMesh->ppBoneMatrixPtrs[2] = &m_pHandHeld->matCombined;
			}
			else if ( !m_bAttackMode && m_pSlotHeld )	
			{
				m_matStaticLinkBone = m_pSlotHeld->m_matCombined;
				//m_pmcMesh->m_ppBoneMatrixPtrs[2] = &const_cast<DxBoneTransMaintain*>(m_pSlotHeld)->m_matCombined;

				if ( m_emWeaponWhereBack == EMPEACE_WEAPON_USER_SLOT )
				{
					D3DXMatrixMultiply( &m_matStaticLinkBone, &m_matUserBackSlot, &m_matStaticLinkBone );
				}

				//m_pmcMesh->ppBoneMatrixPtrs[2] = &m_pSlotHeld->matCombined;
			}
			else
			{
				m_matStaticLinkBone = pSkeleton->FindBone_Index( m_rMeshContainer->m_arrayBoneIndex[0] )->m_matCombined;
			}
		}

		// Note : 일반 메쉬인 경우 매트릭스를 미리 계산해 놓는다.
		D3DXMatrixMultiply( &m_matStaticLinkBone, m_rMeshContainer->m_ppStaticMatrixPtrs[1], &m_matStaticLinkBone );
		D3DXMatrixMultiply( &m_matStaticLinkBone, m_rMeshContainer->m_ppStaticMatrixPtrs[0], &m_matStaticLinkBone );

		if ( parrayfWeaponScale )
		{
			D3DXMATRIX matScale;
			D3DXMatrixScaling( &matScale, parrayfWeaponScale[m_emWeaponWhereBack],parrayfWeaponScale[m_emWeaponWhereBack], parrayfWeaponScale[m_emWeaponWhereBack] );
			D3DXMatrixMultiply( &m_matStaticLinkBone, &matScale, &m_matStaticLinkBone );
		}
	}

	//	Note : Setting State.
	//
	if ( !bShadow )
	{
		size_t nSIZE = m_vecEFFECT.size();
		for ( size_t n=0; n<nSIZE; ++n )
		{
			DxEffChar* pEFFECT = m_vecEFFECT[n];
			if ( pEFFECT->GetStateOrder() > EMEFFSO_RENDERSTATE )	break;

			pEFFECT->SettingState ( pd3dDevice, sSKINEFFDATA );
		}
	}

	//	Note : 캐릭터 텍스쳐 파일에 스펙큘러 맵이 들어 있을 경우 1번만 뿌리기 위해서 기본 조각파일을 뿌리지 않는다.
	//
	if ( bOriginDraw )
	{
		DrawMeshContainer( pd3dDevice, TRUE, bAlpha, bShadow, bAlphaTex, 0.f, pSkeleton );
	}
	else
	{
		if ( m_rSkinMesh )
		{
			m_rSkinMesh->SetDrawState( FALSE, bAlpha, bShadow, bAlphaTex, FALSE, 0.f );
		}
	}

	//	Note : 기본 캐릭터 출력시 trace point calc, render effect skin 를 행한다.
	//
	if ( !bShadow && bEff )
	{
		size_t nSize = m_vecTrace.size();
		for ( size_t n=0; n<nSize; ++n )
		{
			STRACOR &sTracor = m_vecTrace[n];

			m_rMeshContainer->CalculateVertexInfluences ( sTracor.m_sVertexInflu, sTracor.m_vPos, sTracor.m_vNormal, &sTracor.m_BoneMatrice, FALSE, *pSkeleton, m_matStaticLinkBone );
		}

        // 연마의 Ambient Material 을 가져온다.
        {
            SKINEFF_LEVEL_DATA* pLevelData(NULL);
		    GLEFFCHAR_VEC_ITER pos = m_vecEFFECT.begin();
		    for ( ; pos!=m_vecEFFECT.end(); ++pos )
		    {
			    (*pos)->GetLevelData( pd3dDevice, &pLevelData );
		    }
		    sSKINEFFDATA.m_rLevelData = pLevelData;

            // 렌더링
            {
		        //	Note : render effect skin
		        //
		        GLEFFCHAR_VEC_ITER pos = std::lower_bound ( m_vecEFFECT.begin(), m_vecEFFECT.end(), EMEFFSO_NORMAL, DXEFFCHAR_OPER() );
		        for ( ; pos!=m_vecEFFECT.end(); ++pos )
		        {
			        DxEffChar* pEFFECT = (*pos);
			        if ( pEFFECT->GetStateOrder() > EMEFFSO_NORMAL )	break;	//	EMEFFSO_NORMAL이 아니면 중단.

			        if( bCrashRender )
			        {
				        switch( RENDERPARAM::emCharRenderTYPE )
				        {
				        case EMCRT_SOFTWARE:
				        case EMCRT_NORMAL:
				        case EMCRT_VERTEX:
					        if( pEFFECT->GetFlag() & EMECF_NORMALMAP )	continue;	// Pixel Shader 안 쓴다고 셋팅해 놓으면 작동 안한다.
					        break;
				        case EMCRT_PIXEL:
					        if( pEFFECT->GetFlag() & EMECF_NONHLSL )	continue;	// 충돌이 일어나는 것을 뿌리지 않는다.
					        break;
				        }
			        }

					g_bCHAR_EDIT_PIECE_RENDER_FIXED = FALSE;
			        pEFFECT->SetWorldMatrix( matWorld );
			        pEFFECT->Render ( pd3dDevice, m_spCharPartBoneMatrices->GetBoneMatrices(), sSKINEFFDATA );
		        }

				//	Note : render effect skin
				//
				pos = std::lower_bound ( m_vecEFFECT.begin(), m_vecEFFECT.end(), EMEFFSO_ADD, DXEFFCHAR_OPER() );
				for ( ; pos!=m_vecEFFECT.end(); ++pos )
				{
					DxEffChar* pEFFECT = (*pos);
					if ( pEFFECT->GetStateOrder() > EMEFFSO_ADD )	break;	//	EMEFFSO_ADD이 아니면 중단.

					g_bCHAR_EDIT_PIECE_RENDER_FIXED = FALSE;
					pEFFECT->SetWorldMatrix( matWorld );
					pEFFECT->Render ( pd3dDevice, m_spCharPartBoneMatrices->GetBoneMatrices(), sSKINEFFDATA );
				}
            }

            // 초기화
            sSKINEFFDATA.m_rLevelData = NULL;
        }
	}

	//	Note : Setting State.
	//
	if ( !bShadow )
	{
		GLEFFCHAR_VEC_RITER pos = std::lower_bound ( m_vecEFFECT.rbegin(), m_vecEFFECT.rend(), EMEFFSO_RENDERSTATE, DXEFFCHAR_DISOPER() );
		for ( ; pos!=m_vecEFFECT.rend(); ++pos )
		{
			DxEffChar* pEFFECT = (*pos);

			pEFFECT->RestoreState ( pd3dDevice );
		}
	}

	return;
}

void DxCharPart::SetPosOffset( const D3DXVECTOR3& vPosOffset )
{
	if ( !m_spCharPartBoneMatrices.get() )
		return;

	m_spCharPartBoneMatrices->SetPosOffset( vPosOffset );
}

void DxCharPart::RenderMaterialOpaque( LPDIRECT3DDEVICEQ pd3dDevice, 
									  const DxSkeletonMaintain& sSkeleton,
									  float* parrayfWeaponScale,
									  SKINEFFDATA& sSKINEFFDATA, 
									  float fAlpha, 
									  const SKIN_SPEC_SSS_INFO& sSkinInfo, 
									  const D3DXMATRIX* pmatAttParentBone,	// AttBone 의 부모일 수 있다.
									  const D3DXMATRIX& matWorld,
									  const float fScale,
									  const DxClothColl* pClothColl,
									  const std::vector<physx::PxClothCollisionSphere>* pvecClothCollSpheres,
									  float fAttParentBoneScale,
									  BOOL bEffect )
{
	// 무언가 이상한 문제가 생긴거다.
	// 혹은 처음 생성되었고 Thread 작업중이라 값이 없을 수 있다.
	if ( !sSkeleton.IsActiveBone() )
		return;

	if ( !IsEnableRender() )
		return;

	if ( RENDERPARAM::g_emCharacterQulity < TnL_CHAR_PS_1_1 )
		return;

	//	Note : 일반 메시일 경우 Bone에 링크되어 있는걸 가정.
	//		연결 설정된 본과 다른 곳에 사용된다는걸 가정하여 본을 다시 연결	시켜줌.
	//
	if ( !m_rMeshContainer->pSkinInfo )
	{
		//	Note : 영향 받는 본의 matrix 포인터를 다시 찾는다.
		//			m_pmcMesh->m_szBoneNames[0];	//	[LOCAL]
		//			m_pmcMesh->m_szBoneNames[1];	//	[LOCAL]
		//			m_pmcMesh->m_szBoneNames[2];	//	[COMBINED] <-- 이것만 실제로 BONE에서 영향을 받는것.
		m_pHandHeld = sSkeleton.FindBone ( m_rMeshContainer->m_strBoneName );

		switch( m_emWeaponWhereBack )
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
			m_pSlotHeld = sSkeleton.FindBone ( SKINTYPE_STING::m_szWHEREBACKTYPE[m_emWeaponWhereBack] );
			break;
		case EMPEACE_WEAPON_USER_SLOT:
			m_pSlotHeld = sSkeleton.FindBone ( m_strUserBackSlot );
			break;
		};

		// Note : 활이 등에 붙는 위치의 이름이 바뀌었는데 .. 예전 이름도 아직 사용중이라서 이렇게 해야한다. < 극강부 들어가면 삭제해야함 >
		if( !m_pSlotHeld && (m_emWeaponWhereBack==EMPEACE_WEAPON_WHAL_SLOT) )
		{
			m_pSlotHeld = sSkeleton.FindBone ( _T("slot_lhand") );
		}

		// m_bUserBackSlot 가 TRUE 이고, Weapon Use Slot 으로 셋팅 되어 있으면, UserBackSlot 으로만 동작되도록 한다.
		if( m_bKeepUserBackSlot && m_emWeaponWhereBack == EMPEACE_WEAPON_USER_SLOT && m_pSlotHeld )
		{
			m_matStaticLinkBone = m_pSlotHeld->m_matCombined;
			D3DXMatrixMultiply( &m_matStaticLinkBone, &m_matUserBackSlot, &m_matStaticLinkBone );
		}
		else
		{
			if ( m_bAttackMode && m_pHandHeld )	
			{
				m_matStaticLinkBone = m_pHandHeld->m_matCombined;
			}
			else if ( !m_bAttackMode && m_pSlotHeld )	
			{
				m_matStaticLinkBone = m_pSlotHeld->m_matCombined;

				if ( m_emWeaponWhereBack == EMPEACE_WEAPON_USER_SLOT )
				{
					D3DXMatrixMultiply( &m_matStaticLinkBone, &m_matUserBackSlot, &m_matStaticLinkBone );
				}
			}
			else
			{
				D3DXMatrixIdentity( &m_matStaticLinkBone );
				//m_matStaticLinkBone = sSkeleton.FindBone_Index( m_rMeshContainer->m_arrayBoneIndex[0] )->m_matCombined;
			}
		}

		// Note : 일반 메쉬인 경우 매트릭스를 미리 계산해 놓는다.
		D3DXMatrixMultiply( &m_matStaticLinkBone, m_rMeshContainer->m_ppStaticMatrixPtrs[1], &m_matStaticLinkBone );
		D3DXMatrixMultiply( &m_matStaticLinkBone, m_rMeshContainer->m_ppStaticMatrixPtrs[0], &m_matStaticLinkBone );

		if ( parrayfWeaponScale )
		{
			D3DXMATRIX matScale;
			D3DXMatrixScaling( &matScale, parrayfWeaponScale[m_emWeaponWhereBack],parrayfWeaponScale[m_emWeaponWhereBack], parrayfWeaponScale[m_emWeaponWhereBack] );
			D3DXMatrixMultiply( &m_matStaticLinkBone, &matScale, &m_matStaticLinkBone );
		}
	}



	//////////////////////////////////////////////////////////////////////////
	// m_matStaticLinkBone 셋팅 후에 이 부분이 작업되야 한다.
	// 계산을 해 놓아야 한다.
	size_t nSize = m_vecTrace.size();
	for ( size_t n=0; n<nSize; ++n )
	{
		STRACOR &sTracor = m_vecTrace[n];

		m_rMeshContainer->CalculateVertexInfluences ( sTracor.m_sVertexInflu, sTracor.m_vPos, sTracor.m_vNormal, &sTracor.m_BoneMatrice, FALSE, sSkeleton, m_matStaticLinkBone );
	}


	//////////////////////////////////////////////////////////////////////////
	// Note : m_vecTrace 가 계산 된 후에 PrecalculateForMaterialSystem 가 불려져야 한다.
	m_scpEffectMaterialData->InitForPrecalculateForMaterialSystem();
	for ( DWORD i=0; i<m_vecEFFECT.size(); ++i )
	{
		m_vecEFFECT[i]->SetSkeleton( &sSkeleton );
		m_vecEFFECT[i]->SetWorldMatrix( matWorld );
		m_vecEFFECT[i]->PrecalculateForMaterialSystem( pd3dDevice, m_scpEffectMaterialData.get(), pmatAttParentBone ? pmatAttParentBone : &matWorld , fScale );

		// 효과를 뿌릴 때만 미리계산을 하도록 한다.
		if ( bEffect )
		{
			m_vecEFFECT[i]->PrecalculateBeforeRenderEff( pd3dDevice, pmatAttParentBone ? pmatAttParentBone : &matWorld );
		}
	}


	for ( DWORD i=0; i<m_dwMaterialNum; ++i )
	{
		m_rSkinPiece->m_arrayMaterialPiece[i].PrecalculateForMaterialSystem( m_scpEffectMaterialData.get(), i );
	}


	// VisualMaterial 이라면 VM 으로 렌더링하고 Material System 으로 렌더하지는 않도록 한다.
	if ( m_dwFlags & EMECF_VISUAL_MATERIAL )
	{
		// 연마의 Ambient Material 을 가져온다.
		{
			SKINEFF_LEVEL_DATA* pLevelData(NULL);
			GLEFFCHAR_VEC_ITER pos = m_vecEFFECT.begin();
			for ( ; pos!=m_vecEFFECT.end(); ++pos )
			{
				(*pos)->GetLevelData( pd3dDevice, &pLevelData );
			}
			sSKINEFFDATA.m_rLevelData = pLevelData;
		}

		for ( DWORD i=0; i<m_vecEFFECT.size(); ++i )
		{
			if ( m_vecEFFECT[i]->GetTypeID() == EMEFFCHAR_VISUAL_MATERIAL )
			{
				m_vecEFFECT[i]->RenderVM( pd3dDevice, &sSKINEFFDATA, EMTTF_OPAQUE );
			}
		}

		return;
	}
	else if ( m_dwFlags & EMECF_TOTAL_SHADER )
	{
		for ( DWORD i=0; i<m_vecEFFECT.size(); ++i )
		{
			if ( m_vecEFFECT[i]->GetTypeID() == EMEFFCHAR_TOTALSHADER )
			{
				m_vecEFFECT[i]->Render( pd3dDevice, m_spCharPartBoneMatrices->GetBoneMatrices(), FALSE );
			}
		}

		return;
	}
	else if ( m_dwFlags & EMECF_NORMALMAP )
	{
		for ( DWORD i=0; i<m_vecEFFECT.size(); ++i )
		{
			if ( m_vecEFFECT[i]->GetTypeID() == EMEFFCHAR_NORMALMAP )
			{
				m_vecEFFECT[i]->Render( pd3dDevice, m_spCharPartBoneMatrices->GetBoneMatrices(), FALSE );
			}
		}

		return;
	}

	//	Note : 캐릭터 텍스쳐 파일에 스펙큘러 맵이 들어 있을 경우 1번만 뿌리기 위해서 기본 조각파일을 뿌리지 않는다.
	{
		DrawMeshContainerMaterialOpaque
		( 
			pd3dDevice, 
			sSkinInfo, 
			pmatAttParentBone, 
			pClothColl,
			sSkeleton,
			parrayfWeaponScale,
			pvecClothCollSpheres, 
			fAttParentBoneScale,
			fAlpha 
		);
	}

	return;
}

BOOL DxCharPart::DrawMeshContainerMaterialOpaque( LPDIRECT3DDEVICEQ pd3dDevice, 
												 const SKIN_SPEC_SSS_INFO& sSkinInfo, 
												 const D3DXMATRIX* pmatAttParentBone,
												 const DxClothColl* pClothColl,
												 const DxSkeletonMaintain& sSkeleton,
												 float* parrayfWeaponScale,
												 const std::vector<physx::PxClothCollisionSphere>* pvecClothCollSpheres,
												 float fAttParentBoneScale,
												 float fAlpha )
{
	DxSkinMesh9_Material* pSkinMesh = dynamic_cast<DxSkinMesh9_Material*>( m_rSkinMesh );
	if ( !pSkinMesh )
		return FALSE;

	// Opaque 렌더링.
	if ( m_sUsedLoadingChangeData.m_rSkinPiece->m_emType == PIECE_HEAD )
	{
		SKIN_SPEC_SSS_INFO_REFERENCE sSkinInfoRefernce( &sSkinInfo.m_vFaceSpecColor_Power, &sSkinInfo.m_vFaceSpec2Color_Power, &sSkinInfo.m_vSSS_Color );

		// Opaque
		pSkinMesh->RenderOpaque
		( 
			pd3dDevice, 
			m_rMeshContainer, 
			m_spCharPartBoneMatrices->GetBoneMatrices(), 
			m_rSkinPiece->m_arrayMaterialPiece, 
			sSkinInfoRefernce, 
			m_scpEffectMaterialData.get(), 
			m_apOverlayTex, 
			pmatAttParentBone,
			pClothColl,
			pvecClothCollSpheres,
			fAlpha,
			m_matStaticLinkBone,
			m_spPhysXCloth.get(),
			fAttParentBoneScale
		);
	}
	else
	{
		SKIN_SPEC_SSS_INFO_REFERENCE sSkinInfoRefernce( &sSkinInfo.m_vBodySpecColor_Power, &sSkinInfo.m_vBodySpec2Color_Power, &sSkinInfo.m_vSSS_Color );

		// Opaque
		pSkinMesh->RenderOpaque
		( 
			pd3dDevice, 
			m_rMeshContainer, 
			m_spCharPartBoneMatrices->GetBoneMatrices(), 
			m_rSkinPiece->m_arrayMaterialPiece, 
			sSkinInfoRefernce, 
			m_scpEffectMaterialData.get(), 
			m_apOverlayTex, 
			pmatAttParentBone,
			pClothColl,
			pvecClothCollSpheres,
			fAlpha,
			m_matStaticLinkBone,
			m_spPhysXCloth.get(),
			fAttParentBoneScale
		);
	}

	return TRUE;
}

void DxCharPart::RenderMaterialHardAlpha( LPDIRECT3DDEVICEQ pd3dDevice, 
										 float fAlpha, 
										 const SKIN_SPEC_SSS_INFO& sSkinInfo )
{
	if ( !IsEnableRender() )
		return;

	if ( m_dwFlags & EMECF_VISUAL_MATERIAL )
	{
		for ( DWORD i=0; i<m_vecEFFECT.size(); ++i )
		{
			if ( m_vecEFFECT[i]->GetTypeID() == EMEFFCHAR_VISUAL_MATERIAL )
			{
				m_vecEFFECT[i]->RenderVM( pd3dDevice, NULL, EMTTF_ALPHA_HARD );
			}
		}

		return;
	}
	else if ( m_dwFlags & EMECF_TOTAL_SHADER )
	{
		return;
	}
	else if ( m_dwFlags & EMECF_NORMALMAP )
	{
		return;
	}

	DxSkinMesh9_Material* pSkinMesh = dynamic_cast<DxSkinMesh9_Material*>( m_rSkinMesh );
	if ( !pSkinMesh )
		return;

	// DxEffCharMultiTex 의 pEffMaterialData->m_rMultiTex 값이 변동이 될수 있기 때문에, 렌더전에 그려지도록 한다.
	for ( DWORD i=0; i<m_vecEFFECT.size(); ++i )
	{
		m_vecEFFECT[i]->PrecalculateForMaterialSystemHardAlpha( pd3dDevice, m_scpEffectMaterialData.get() );
	}

	if ( m_sUsedLoadingChangeData.m_rSkinPiece->m_emType == PIECE_HEAD )
	{
		SKIN_SPEC_SSS_INFO_REFERENCE sSkinInfoRefernce( &sSkinInfo.m_vFaceSpecColor_Power, &sSkinInfo.m_vFaceSpec2Color_Power, &sSkinInfo.m_vSSS_Color );

		pSkinMesh->RenderHardAlpha
		( 
			pd3dDevice, 
			m_rMeshContainer, 
			m_spCharPartBoneMatrices->GetBoneMatrices(), 
			m_rSkinPiece->m_arrayMaterialPiece, 
			sSkinInfoRefernce, 
			m_scpEffectMaterialData.get(), 
			m_apOverlayTex,
			fAlpha,
			m_matStaticLinkBone,
			m_spPhysXCloth.get()
		);
	}
	else
	{
		SKIN_SPEC_SSS_INFO_REFERENCE sSkinInfoRefernce( &sSkinInfo.m_vBodySpecColor_Power, &sSkinInfo.m_vBodySpec2Color_Power, &sSkinInfo.m_vSSS_Color );

		pSkinMesh->RenderHardAlpha
		( 
			pd3dDevice, 
			m_rMeshContainer, 
			m_spCharPartBoneMatrices->GetBoneMatrices(), 
			m_rSkinPiece->m_arrayMaterialPiece, 
			sSkinInfoRefernce, 
			m_scpEffectMaterialData.get(), 
			m_apOverlayTex,
			fAlpha,
			m_matStaticLinkBone,
			m_spPhysXCloth.get()
		);
	}
}

void DxCharPart::RenderMaterialSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, 
										 float fAlpha, 
										 const SKIN_SPEC_SSS_INFO& sSkinInfo )
{
	if ( !IsEnableRender() )
		return;

	if ( m_dwFlags & EMECF_VISUAL_MATERIAL )
	{
		for ( DWORD i=0; i<m_vecEFFECT.size(); ++i )
		{
			if ( m_vecEFFECT[i]->GetTypeID() == EMEFFCHAR_VISUAL_MATERIAL )
			{
				m_vecEFFECT[i]->RenderVM( pd3dDevice, NULL, EMTTF_ALPHA_SOFT );
			}
		}

		return;
	}
	else if ( m_dwFlags & EMECF_TOTAL_SHADER )
	{
		return;
	}
	else if ( m_dwFlags & EMECF_NORMALMAP )
	{
		return;
	}

	DxSkinMesh9_Material* pSkinMesh = dynamic_cast<DxSkinMesh9_Material*>( m_rSkinMesh );
	if ( !pSkinMesh )
		return;

	// DxEffCharMultiTex 의 pEffMaterialData->m_rMultiTex 값이 변동이 될수 있기 때문에, 렌더전에 그려지도록 한다.
	for ( DWORD i=0; i<m_vecEFFECT.size(); ++i )
	{
		m_vecEFFECT[i]->PrecalculateForMaterialSystemSoftAlpha( pd3dDevice, m_scpEffectMaterialData.get() );
	}

	if ( m_sUsedLoadingChangeData.m_rSkinPiece->m_emType == PIECE_HEAD )
	{
		SKIN_SPEC_SSS_INFO_REFERENCE sSkinInfoRefernce( &sSkinInfo.m_vFaceSpecColor_Power, &sSkinInfo.m_vFaceSpec2Color_Power, &sSkinInfo.m_vSSS_Color );

		pSkinMesh->RenderSoftAlpha
		( 
			pd3dDevice, 
			m_rMeshContainer, 
			m_spCharPartBoneMatrices->GetBoneMatrices(), 
			m_rSkinPiece->m_arrayMaterialPiece, 
			sSkinInfoRefernce, 
			m_scpEffectMaterialData.get(), 
			m_apOverlayTex,
			fAlpha,
			m_matStaticLinkBone,
			m_spPhysXCloth.get()
		);
	}
	else
	{
		SKIN_SPEC_SSS_INFO_REFERENCE sSkinInfoRefernce( &sSkinInfo.m_vBodySpecColor_Power, &sSkinInfo.m_vBodySpec2Color_Power, &sSkinInfo.m_vSSS_Color );

		pSkinMesh->RenderSoftAlpha
		( 
			pd3dDevice, 
			m_rMeshContainer, 
			m_spCharPartBoneMatrices->GetBoneMatrices(), 
			m_rSkinPiece->m_arrayMaterialPiece, 
			sSkinInfoRefernce, 
			m_scpEffectMaterialData.get(), 
			m_apOverlayTex,
			fAlpha,
			m_matStaticLinkBone,
			m_spPhysXCloth.get()
		);
	}
}

void DxCharPart::RenderMaterialAdd ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !IsEnableRender() )
		return;

	if ( m_dwFlags & EMECF_VISUAL_MATERIAL )
		return;

	DxSkinMesh9_Material* pSkinMesh = dynamic_cast<DxSkinMesh9_Material*>( m_rSkinMesh );
	if ( !pSkinMesh )
		return;

	// DxEffCharMultiTex 의 pEffMaterialData->m_rMultiTex 값이 변동이 될수 있기 때문에, 렌더전에 그려지도록 한다.
	for ( DWORD i=0; i<m_vecEFFECT.size(); ++i )
	{
		m_vecEFFECT[i]->PrecalculateForMaterialSystemAdd( pd3dDevice, m_scpEffectMaterialData.get() );
	}

	GLEFFCHAR_VEC_ITER pos = std::lower_bound ( m_vecEFFECT.begin(), m_vecEFFECT.end(), EMEFFSO_ADD, DXEFFCHAR_OPER() );
	for ( ; pos!=m_vecEFFECT.end(); ++pos )
	{
		DxEffChar* pEFFECT = (*pos);
		if ( pEFFECT->GetStateOrder() > EMEFFSO_ADD )	break;	//	EMEFFSO_ADD이 아니면 중단.

		pEFFECT->RenderMaterialAdd( pd3dDevice, m_rSkinPiece->m_arrayMaterialPiece, m_matStaticLinkBone, *pSkinMesh, m_spPhysXCloth.get() );
	}
}

void DxCharPart::RenderMaterialGlow ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !IsEnableRender() )
		return;

	//if ( m_dwFlags & EMECF_VISUAL_MATERIAL )
	//	return;

	DxSkinMesh9_Material* pSkinMesh = dynamic_cast<DxSkinMesh9_Material*>( m_rSkinMesh );
	if ( !pSkinMesh )
		return;

	GLEFFCHAR_VEC_ITER pos = std::lower_bound ( m_vecEFFECT.begin(), m_vecEFFECT.end(), EMEFFSO_GLOW, DXEFFCHAR_OPER() );
	for ( ; pos!=m_vecEFFECT.end(); ++pos )
	{
		DxEffChar* pEFFECT = (*pos);
		if ( pEFFECT->GetStateOrder() > EMEFFSO_LEVEL )	break;	//	EMEFFSO_GLOW, EMEFFSO_LEVEL이 아니면 중단.

		pEFFECT->RenderMaterialGlow( pd3dDevice, m_rSkinPiece->m_arrayMaterialPiece, m_matStaticLinkBone, *pSkinMesh, m_spPhysXCloth.get() );
	}
}

BOOL DxCharPart::RenderMaterialGhosting( LPDIRECT3DDEVICEQ pd3dDevice, SKINEFFDATA& sSKINEFFDATA, const SKIN_SPEC_SSS_INFO& sSkinInfo, float fAlpha, size_t unGhostingIndex )
{
	// m_rMeshContainer 가 NULL 일 수 있다. 하지만 그게 m_spCharPartBoneMatrices->GetBoneMatricesForGhosting( unGhostingIndex ); NULL 은 아니다.
	if ( !IsEnableRender() )
		return TRUE;

	DxSkinMesh9_Material* pSkinMesh = dynamic_cast<DxSkinMesh9_Material*>( m_rSkinMesh );
	if ( !pSkinMesh )
		return FALSE;


	////////////////////////////////////////////////////////////////////////////
	//// VisualMaterial 이라면 VM 으로 렌더링하고 Material System 으로 렌더하지는 않도록 한다.
	//if ( m_dwFlags & EMECF_VISUAL_MATERIAL )
	//{
	//	for ( DWORD i=0; i<m_vecEFFECT.size(); ++i )
	//	{
	//		if ( m_vecEFFECT[i]->GetTypeID() == EMEFFCHAR_VISUAL_MATERIAL )
	//		{
	//			m_vecEFFECT[i]->Render( pd3dDevice, sSKINEFFDATA );
	//		}
	//	}

	//	return TRUE;
	//}
	//else if ( m_dwFlags & EMECF_TOTAL_SHADER )
	//{
	//	for ( DWORD i=0; i<m_vecEFFECT.size(); ++i )
	//	{
	//		if ( m_vecEFFECT[i]->GetTypeID() == EMEFFCHAR_TOTALSHADER )
	//		{
	//			m_vecEFFECT[i]->Render( pd3dDevice, FALSE );
	//		}
	//	}

	//	return TRUE;
	//}
	//else if ( m_dwFlags & EMECF_NORMALMAP )
	//{
	//	for ( DWORD i=0; i<m_vecEFFECT.size(); ++i )
	//	{
	//		if ( m_vecEFFECT[i]->GetTypeID() == EMEFFCHAR_NORMALMAP )
	//		{
	//			m_vecEFFECT[i]->Render( pd3dDevice, FALSE );
	//		}
	//	}

	//	return TRUE;
	//}


	//////////////////////////////////////////////////////////////////////////
	const D3DXMATRIX* rarrayBoneMatrices(NULL);
	if ( unGhostingIndex != -1 )
	{
		rarrayBoneMatrices = m_spCharPartBoneMatrices->GetBoneMatricesForGhosting( unGhostingIndex );
		if ( !rarrayBoneMatrices )
			return FALSE;
	}
	else
	{
		return FALSE;
	}

	if ( m_sUsedLoadingChangeData.m_rSkinPiece->m_emType == PIECE_HEAD )
	{
		SKIN_SPEC_SSS_INFO_REFERENCE sSkinInfoRefernce( &sSkinInfo.m_vFaceSpecColor_Power, &sSkinInfo.m_vFaceSpec2Color_Power, &sSkinInfo.m_vSSS_Color );

		return pSkinMesh->RenderGhosting
		( 
			pd3dDevice, 
			m_rMeshContainer, 
			rarrayBoneMatrices, 
			m_rSkinPiece->m_arrayMaterialPiece, 
			sSkinInfoRefernce, 
			m_scpEffectMaterialData.get(), 
			m_apOverlayTex,
			fAlpha,
			m_matStaticLinkBone,
			m_spPhysXCloth.get()
		);
	}
	else
	{
		SKIN_SPEC_SSS_INFO_REFERENCE sSkinInfoRefernce( &sSkinInfo.m_vBodySpecColor_Power, &sSkinInfo.m_vBodySpec2Color_Power, &sSkinInfo.m_vSSS_Color );

		return pSkinMesh->RenderGhosting
		( 
			pd3dDevice, 
			m_rMeshContainer, 
			rarrayBoneMatrices, 
			m_rSkinPiece->m_arrayMaterialPiece, 
			sSkinInfoRefernce, 
			m_scpEffectMaterialData.get(), 
			m_apOverlayTex,
			fAlpha,
			m_matStaticLinkBone,
			m_spPhysXCloth.get()
		);
	}
}

HRESULT DxCharPart::RenderShadow ( LPDIRECT3DDEVICEQ pd3dDevice, const DxSkeletonMaintain* pSkeleton, float fHeight )
{
	if ( !IsEnableRender() )
		return S_OK;

	if( !(m_dwFlag&EMCF_SHADOW) )	
		return S_OK;	// 그림자 옵션이 없으면 끈다.

	DrawMeshContainer ( pd3dDevice, TRUE, FALSE, TRUE, TRUE, fHeight, pSkeleton );

	return S_OK;
}

void DxCharPart::RenderGlow ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !IsEnableRender() )
		return;

	GLEFFCHAR_VEC_ITER pos = std::lower_bound ( m_vecEFFECT.begin(), m_vecEFFECT.end(), EMEFFSO_GLOW, DXEFFCHAR_OPER() );
	for ( ; pos!=m_vecEFFECT.end(); ++pos )
	{
		DxEffChar* pEFFECT = (*pos);
		if ( pEFFECT->GetStateOrder() > EMEFFSO_LEVEL )	break;	//	EMEFFSO_GLOW, EMEFFSO_LEVEL이 아니면 중단.

		pEFFECT->RenderGlow( pd3dDevice, m_spCharPartBoneMatrices->GetBoneMatrices(), FALSE, (m_dwFlags & EMECF_VISUAL_MATERIAL) );
	}
}

void DxCharPart::RenderEff( LPDIRECT3DDEVICEQ pd3dDevice, const float fScale, const D3DXMATRIX* pAttBone )
{
	if ( !IsEnableRender() )
		return;

	const D3DXMATRIX* pMatrix(NULL);
	if ( !m_rMeshContainer->pSkinInfo )    // 일반 메쉬이면 계산 한 것을 가지고 뿌려준다.
    {
        pMatrix = &m_matStaticLinkBone;
    }
    else if ( pAttBone )
    {
        pMatrix = pAttBone;
    }

	if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
	{
		// Note : Render
		GLEFFCHAR_VEC_ITER pos = std::lower_bound ( m_vecEFFECT.begin(), m_vecEFFECT.end(), EMEFFSO_SINGLE_EFF, DXEFFCHAR_OPER() );
		for ( ; pos!=m_vecEFFECT.end(); ++pos )
		{
			DxEffChar* pEFFECT = (*pos);
			if ( pEFFECT->GetStateOrder() > EMEFFSO_SINGLE_EFF )	break;	//	EMEFFSO_SINGLE_EFF이 아니면 중단.

			// 위쪽에서 해주어서 또 않해도 된다.
			//pEFFECT->SetSkeleton( pSkeleton );
			//pEFFECT->SetWorldMatrix( );
			pEFFECT->RenderEff( pd3dDevice, pMatrix, fScale );
		}
	}
	else
	{
		// Note : 
		for ( DWORD i=0; i<m_vecEFFECT.size(); ++i )
		{
			m_vecEFFECT[i]->RenderEffForMaterialSystem( pd3dDevice, pMatrix, fScale );
		}
	}
}

HRESULT DxCharPart::RenderShadowForMaterial( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matViewProj, BOOL bMaterialSystemWLD )
{
	if ( !IsEnableRender() )
		return S_OK;

	if( !(m_dwFlag&EMCF_SHADOW) )	
		return S_OK;	// 그림자 옵션이 없으면 끈다.

	// 사양이 너무 낮다면 렌더링 하지 않는다.
	if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_PS_1_1 )
		return S_OK;

	DxSkinMesh9_Material* pSkinMesh = dynamic_cast<DxSkinMesh9_Material*>( m_rSkinMesh );
	if ( !pSkinMesh )
		return S_OK;

	pSkinMesh->RenderMaterialShadow
	( 
		pd3dDevice,
		m_rMeshContainer, 
		m_spCharPartBoneMatrices->GetBoneMatrices(), 
		m_rSkinPiece->m_arrayMaterialPiece, 
		matViewProj, 
		bMaterialSystemWLD,
		m_matStaticLinkBone,
		m_spPhysXCloth.get()
	);

	return S_OK;
}

SVERTEXINFLU* DxCharPart::GetTracePos(const std::string& strTrace)
{
	VECTRACE_ITER found = std::lower_bound ( m_vecTrace.begin(), m_vecTrace.end(), strTrace.c_str(),STRACOR_OPER() );
	if (found == m_vecTrace.end())
        return NULL;

	if (!strcmp((*found).m_szName, strTrace.c_str()))
        return &(*found).m_sVertexInflu;
	return NULL;
}

STRACOR* DxCharPart::GetTracor(const std::string& strTrace)
{
	VECTRACE_ITER found = std::lower_bound ( m_vecTrace.begin(), m_vecTrace.end(), strTrace.c_str(),STRACOR_OPER() );
	if (found == m_vecTrace.end())
        return NULL;

	if (!strcmp((*found).m_szName,strTrace.c_str()))
        return &(*found);

	return NULL;
}

// rm #906, rm #1213
// m_vecTrace 가 있다면 m_rMeshContainer 가 무조건 있다고 생각하도록 한다.
// GetTracor 체크가 성공한다는 건 CalculateVertexInflu 도 무조건 문제가 없이 vVert 값을 넘겨줄 수 있다는 것이다.
//
// m_vecTrace 과 m_rMeshContainer 이 매칭이 안되면서 vVert 에 값을 못넣어줬고, 사용시에도 vVert 값 초기화를 해주지 않아서 문제가 발생하였다.
//
//	pBoneMatrice -> (주의) 영향을 받는 본의 갯수가 여러개일 경우에 문제가 생김.
//	랜더시 미리 계산되어 있으므로 값을 꺼내서 반환.
HRESULT DxCharPart::CalculateVertexInflu ( std::string strTrace, D3DXVECTOR3 &vVert, D3DXVECTOR3 &vNormal, LPD3DXMATRIX pBoneMatrice )
{
	if ( strTrace.empty() ) return E_FAIL;

	STRACOR* pTracor = GetTracor(strTrace);
	if ( !pTracor )		
		return E_FAIL;

	if ( !m_rMeshContainer )
		return E_FAIL;

	if ( pTracor->m_vPos.x > 100000.f || pTracor->m_vPos.y > 100000.f || pTracor->m_vPos.z > 100000.f ||
		pTracor->m_vPos.x < -100000.f || pTracor->m_vPos.y < -100000.f || pTracor->m_vPos.z < -100000.f)
	{
		return E_FAIL;
	}

	vVert = pTracor->m_vPos;
	vNormal = pTracor->m_vNormal;
	if ( pBoneMatrice )	*pBoneMatrice = pTracor->m_BoneMatrice;

	return S_OK;
}

void DxCharPart::ReloadVMFX( LPDIRECT3DDEVICEQ pd3dDevice )
{
    for ( DWORD i=0; i<m_vecEFFECT.size(); ++i )
	{
        m_vecEFFECT[i]->ReloadVMFX( pd3dDevice );
	}
}

void DxCharPart::GetStaticVertexData( std::vector<VERTEXNORCOLORTEX4>& vecVertices, const DxSkeleton* pSkeleton )
{
	if ( !m_sUsedLoadingChangeData.m_rSkinPiece )
		return;
	if ( !m_rMeshContainer )
		return;

	m_sUsedLoadingChangeData.m_rSkinPiece->m_pmcMesh->GetStaticVertexData( vecVertices, pSkeleton, m_matStaticLinkBone );
}

//void DxCharPart::SetFlagWinnerClubCountry( const TSTRING& stTextureName, const D3DXVECTOR4& vFlagConvert )
//{
//	m_stTextureName = stTextureName;
//	m_vFlagConvert = vFlagConvert;
//}


bool DxCharPart::UpdateLOD(const D3DXVECTOR3& vPos, LPDIRECT3DDEVICEQ pd3dDevice)
{
	// [shhan][2015.01.12] ran 에는 캐릭터 LOD 기능이 필요가 없다. 캐릭터 렌더링 되는 시야가 멀지 않다.
	return true;

    //if ( m_sUsedLoadingChangeData.m_rSkinPiece && m_sUsedLoadingChangeData.m_rSkinPiece->UpdateLOD(vPos, pd3dDevice) == S_OK)
    //{
    //    SetLODPart_Detail();
    //    return true;
    //}

    //return false;
}

BOOL DxCharPart::IsEnableRender()
{
	if ( !m_rMeshContainer )
		return FALSE;

	if ( !m_bRender )
		return FALSE;

	if ( !GetBoneMatrices() )
		return FALSE;

	return TRUE;
}

const D3DXMATRIXA16* DxCharPart::GetBoneMatrices() const
{
	if ( !m_spCharPartBoneMatrices.get() )
		return NULL;

	return m_spCharPartBoneMatrices->GetBoneMatrices();
}

void DxCharPart::GhostingON( BOOL bGhosting, float fLifeTime )
{
	if ( !m_spCharPartBoneMatrices.get() )
		return;

	m_spCharPartBoneMatrices->GhostingON( bGhosting, fLifeTime );
}

// Material 이전 셋팅 DxEffCharNone 을 썼을 경우, 새로운 Material Rendering 쪽에서 기본렌더를 안해야 한다고 알아차려야 한다.
void DxCharPart::SetBaseRenderOff()
{
	if ( !m_rSkinPiece->m_arrayMaterialPiece )
		return;

	for ( DWORD i=0; i<m_dwMaterialNum; ++i )
	{
		m_rSkinPiece->m_arrayMaterialPiece[i].m_bBaseRenderOff = TRUE;
	}
}

//////////////////////////////////////////////////////////////////////////
// 외부 옵션에 따른 ON/OFF 로 인한 내부 Data 변화시 호출 함수 및 변수
BOOL DxCharPart::SetPhysX_Cloth( BOOL bOnOff )
{
	if ( !m_bCompletedLINK )
		return FALSE;

	// 만들어준다.
	if ( bOnOff && !m_spPhysXCloth.get() && NSPhysX::IsPhysX() )
	{
		m_spPhysXCloth = boost::shared_ptr<DxPhysXCloth>(new DxPhysXCloth);
		m_spPhysXCloth->Import( m_sUsedLoadingChangeData.m_rSkinPiece );
	}
	else if ( !bOnOff && m_spPhysXCloth.get() )
	{
		// 지워준다.
		m_spPhysXCloth.reset();
	}

	return TRUE;
}

// SafeMode 에서 WeaponSlot 위치이동. Link 가 아닌 Skin 만을 위함
void DxCharPart::GetMoveParentBoneIDForSafeModeWeaponSlot( WORDWORD& wwOut )
{
	wwOut.dwData = -1;

	if ( !m_rMeshContainer )
		return;

	if ( m_rMeshContainer->pSkinInfo && !m_bAttackMode )
	{
		switch( m_emWeaponWhereBack )
		{
		case EMPEACE_WEAPON_WHIP_SLOT:
		case EMPEACE_WEAPON_BOX_SLOT:
		case EMPEACE_WEAPON_UMBRELLA_SLOT:
			wwOut = m_wwMoveParentBoneIDForSafeModeWeaponSlot;
			break;

		default:
			break;
		};
	}
}