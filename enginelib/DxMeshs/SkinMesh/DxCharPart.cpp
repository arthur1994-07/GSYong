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
	// ASSERT �� ����� ������ �����̴�.
	// if �� ��ó���� ���� �����ľ��Ͽ� ��������.
	GASSERT( dwIndex < m_vecSkinMeshForCloth.size() );

	// �� �Լ��� ��� �Դٸ� m_vecSkinMeshForCloth ���� Ȯ���� �־�� �Ѵ�.
	// �ƿ� empty ��� �� �Լ��� ȣ�� ��ü�� �Ǹ� �ȵȴ�. �׷��� �Ѵ�.
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

	, m_emHairHairOption(HAT_HAIR_BASE)		// ��� Part �� ������ �ش�. �� �ڽ��� �����ʹ� �������.
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
    //// VisualMaterial �� �����ȰŶ��..
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

//	Note : �ű� ����Ʈ ������.
//
void DxCharPart::AddEffList ( DxEffChar* pEffChar )
{
	GASSERT(pEffChar);
    if (pEffChar)
    {
	    m_vecEFFECT.push_back( pEffChar );
	    std::sort ( m_vecEFFECT.begin(), m_vecEFFECT.end(), DXEFFCHAR_OPER() );
	    //	flag�� ������ ȿ���� ��� flag�� �ջ�.
	    m_dwFlags |= pEffChar->GetFlag();
    }
}

//	Note : ȿ�� ������ ��������
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
		if ( pEFFECT->GetStateOrder() > dwOrder )	break;	//	�������� �ʴ´�.
		if ( pEFFECT!=pEffChar )					continue;

		//	����.
		SAFE_DELETE(pEFFECT);
		m_vecEFFECT.erase(pos);
		break;	// �ݺ��ڷ� �� ��� ������ ��ȿ���� ���� �� �����Ƿ� �������Ѵ�.
	}

    //BOOL bUsedVisualMaterial = dwFlag & EMECF_VISUAL_MATERIAL;

	//	flag�� �־��� ȿ���� ������ ��� flag�� �ٽ� �ջ�.
	if ( dwFlag )		SumEffFlag ();

    //if ( bUsedVisualMaterial )
    //{
    //    // VisualMaterial �� �����ȰŶ��..
    //    if ( !(m_dwFlags & EMECF_VISUAL_MATERIAL) )
    //    {
    //        if ( m_pmcMesh )
    //            m_pmcMesh->GenerateSkinnedMeshNON_public( pd3dDevice );
    //    }
    //}
}

//	Note : ������ ������, ����ȿ�� ���Ž� ȣ��.
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

    // ������ Ambient�� �ִٸ� �ٽ� ������.
    if ( dwFlag & EMECF_AMBIENT )
    {
        m_cEffAmbientType = USETEXCOLOR;
        m_cEffAmbientColor.r = 1.f;
        m_cEffAmbientColor.g = 1.f;
        m_cEffAmbientColor.b = 1.f;
        for ( size_t n=0; n<m_vecEFFECT.size(); ++n )
	    {
		    DxEffChar* pEFFECT = m_vecEFFECT[n];

            // Note : Ambient Color�� �� �����Ѵ�.
            //          Overlay �� VisualMaterial ��. ������ �ٸ� �Ϳ��� ���� ȿ���� �������� �����Ѵ�.
            if ( pEFFECT->GetTypeID() == EMEFFCHAR_AMBIENT )
            {
                // [2011.03.31] �ӵ��� �����ؼ� dynamic_cast -> static_cast�� ����.
                //DxEffCharAmbient* pEffCharAmbient = dynamic_cast<DxEffCharAmbient*>( pEFFECT );
                DxEffCharAmbient* pEffCharAmbient = MIN_BOOST::polymorphic_downcast<DxEffCharAmbient*>( pEFFECT );
                m_cEffAmbientType = pEffCharAmbient->GetEffAmbientType();
                m_cEffAmbientColor = pEffCharAmbient->GetEffAmbientColor();
            }
	    }
    }

	//	flag�� �־��� ȿ���� ������ ��� flag�� �ٽ� �ջ�.
	if ( dwFlag )	SumEffFlag ();
}

//	Note : ����ȿ�� ������ �����ϴ��� �˻��� �־��ٶ� ���.
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
            // [2011.03.31] �ӵ��� �����ؼ� dynamic_cast -> static_cast�� ����.
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

	// ���� ������ �ٲٷ��ϸ� �������� �ʵ��� �Ѵ�.
	if ( m_sUsedLoadingChangeData.IsSame( pSkinPiece, bAmbientUP, pSelfBodyEffect ) )
	{
		return FALSE;
	}

	m_bCompletedLINK	= FALSE;		// ���ο���� �������� �ٲ�� �Ѵ�.

	// �ϴ� BackUp �� ���´�.
	m_sBackUpLoadingChangeData.Import( pSkinPiece, bAmbientUP, pSelfBodyEffect );

	//// �ٷ� �ʿ��ؼ� �����س���.
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

	////	ȿ�� ����.
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
	// 1.1 �� �ȵȴٸ� �۾��� �ʿ䰡 ����.
	if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
		return;

	// Texture �� ��ȭ�� �ش�.
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

	// Texture �� ��ȭ�� �ش�.
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
		// BackUp �ߴ� ������ �� �Ѱ��ش�.
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
		//m_emType = m_rSkinPiece->m_emType;						// SetPart���� ���ش�. RHAND->LHAND ������ �̰��� ���� �ȵȴ�.
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
		// � ���������� �̰����δ� �ʰ� ���� �� �ִ�.
		// ������ ������ m_vecEFFECT ȿ���� ���� �� �ֱ� ������ ���������ֵ��� ����.
		// �ٸ� ������ ���� �ִ�. ����� �׷��� ����.
		//////////////////////////////////////////////////////////////////////////
		// �������� ȿ�� ���
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

			// �������� ȿ�� ����
			{
				ClearEffList ();
			}

			// ȿ�� ����
			m_vecEFFECT = vecEFFECT;
			vecEFFECT.clear();
		}


		//////////////////////////////////////////////////////////////////////////
		//	ȿ�� ����.
		size_t nSIZE = m_sUsedLoadingChangeData.m_rSkinPiece->m_vecEFFECT.size();
		for ( size_t n=0; n<nSIZE; ++n )
		{
			DxEffChar* pEFFECT = m_sUsedLoadingChangeData.m_rSkinPiece->m_vecEFFECT[n];

			// �����̸� �۵����� �ʵ��� �Ѵ�.
			if ( pEFFECT->GetTypeID() == EMEFFCHAR_AMBIENT )
			{
				// Material System �� �� ��� EMEFFCHAR_AMBIENT �� ���۵��� �ʵ��� �Ѵ�.
				// LightMapAmbientColor �� Ȱ��ȭ�Ǿ� ������(���ο��), EMEFFCHAR_AMBIENT �� DxCharPart ���� ������ �ʵ��� �Ѵ�.
				if ( NSLightMapAmbientColor::g_bActiveLightMapAmbientColor || RENDERPARAM::g_emCharacterQulity >= TnL_CHAR_PS_1_1 )
				{
					// ������ �� �� üũ�ϰڴ�.!!!!

					// [shhan][2014.08.04] �ӵ��� �����ؼ� dynamic_cast -> static_cast�� ����.
					//DxEffCharAmbient* pEffCharAmbient = dynamic_cast<DxEffCharAmbient*>( pEFFECT );
					DxEffCharAmbient* pEffCharAmbient = MIN_BOOST::polymorphic_downcast<DxEffCharAmbient*>( pEFFECT );
					if ( pEffCharAmbient )
					{
						// USETEXCOLOR �� ȿ�� ������ �ϰ� �ƴҰ��� continue �Ѵ�.
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

		// �ڽ�Ƭ �⺻ �� ������.
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

		// Texture �� ��ȭ�� �ش�.
		MakeOverlayTex();
		SetWorldBattleFlag( NSGameLogicInfo::g_strWinnerCountryWB.c_str() );
		//CreateEffRenderTarget();

		if ( m_sUsedLoadingChangeData.m_rSelfBodyEffect )
		{
			D3DXVECTOR3 vDirect(0.f,0.f,1.f);
			DxEffcharDataMan::GetInstance().PutEffect ( this, m_sUsedLoadingChangeData.m_rSelfBodyEffect, &vDirect );
		}

		// m_pBackUpDataArray�� �������Ѵ�;
		if ( m_pBackUpData )
		{
			// rEffCharDataArrayBackup->SetPart ( pCharPart, pd3dDevice, pDir, fTarDir );
			D3DXVECTOR3 vDirect(0.f,0.f,1.f);
			m_pBackUpData->SetPart ( this, m_pd3dDevice, &vDirect );
			m_pBackUpData = NULL;
		}
		
		// SafeMode �� ���Ⱑ EMPEACE_WEAPON_WHIP_SLOT, EMPEACE_WEAPON_BOX_SLOT, EMPEACE_WEAPON_UMBRELLA_SLOT ��� �̸� ���� ��ġ�� ������ �ֵ��� �Ѵ�.
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

		//// �Ӽ����� �� ����.
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

		//// CharPart�� Matrix�� DxBoneTransOptimize�� Matrix�� �����Ų��.
		//if( m_prBoneMatrixPtrs )
		//{
		//	SAFE_DELETE_ARRAY( m_prBoneMatrixPtrs );
		//}
		//m_pSkinMesh->SetupBoneMatrixPointers( m_rBoneTransOptimize, &m_prBoneMatrixPtrs, m_rSkinPiece->m_szMeshName );        

		// ������ ����Ǹ� ������� �ϴ� Data
		m_spCharPartBoneMatrices.reset();
		m_spPhysXCloth.reset();

		m_bCompletedLINK = TRUE;

		// [shhan][2015.06.02]
		// chf �� ���ý� �¾�����. cps �� �Ŀ� �����ع�����, �˸��� ���� bone �� ���ð� ������ �Ǿ� ������ �ȴ�.
		// �� ���� �׳� ������ ���ϴ°� �´�.
		//
		// [shhan][2015.06.08]
		// Link�� �ƴ� Skin �� �۾��ؾ� �Ѵ�.
		// Link�� �� ���� �ִٰ� ���͹����� ������ �Ǿ������.
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

				GASSERT( 0 && _T("DxSkinChar::SetCharData - chf �� bone �� cps �� bone �� �ٸ��ϴ�.") );

				// ������ �����Ƿ� �� ������.
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

	//// Texture �� ��ȭ�� �ش�.
	//MakeOverlayTex();

 //   if ( m_sUsedLoadingChangeData.m_rSelfBodyEffect )
 //   {
 //       D3DXVECTOR3 vDirect(0.f,0.f,1.f);
 //       DxEffcharDataMan::GetInstance().PutEffect ( this, m_sUsedLoadingChangeData.m_rSelfBodyEffect, &vDirect );
 //   }

	//// ������ ����Ǹ� ������� �ϴ� Data
	//m_spCharPartBoneMatrices.reset();
	//m_spPhysXCloth.reset();
}

void DxCharPart::ReSetPart_Detail()
{
	m_bCompletedLINK = FALSE;

	m_rMeshContainer = NULL;

	// rm #906, rm #1213
	// �̰� �������� ������ CalculateVertexInflu ���� m_vecTrace ���� �ְ�,
	// m_rMeshContainer ���� ��� vVert ���� ���� ���� �ʴ´�.
	// m_vecTrace �� �ִٸ� m_rMeshContainer �� ������ �ִٰ� �����ϵ��� �Ѵ�.
	// GetTracor üũ�� �����Ѵٴ� �� CalculateVertexInflu �� ������ ������ ���� vVert ���� �Ѱ��� �� �ִٴ� ���̴�.
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
	// Fixed ������ ��� ���ֵ��� �Ѵ�.
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
		//DxSkinPieceContainer::GetInstance( pd3dDevice )->ReleasePiece ( m_strNameCPS.c_str() );	// ���� �� �۾��� �� �ʿ䰡 ����.
		//																							// �Ѵٸ� ����ȭ ������� �Ѵ�.
		m_strNameCPS = _T("");
	}

	m_rSkinCharEffAniBase = NULL;
	m_rSkinPiece = NULL;
	m_rSkinMesh = NULL;
	m_rSkeleton = NULL;
	m_rMeshContainer = NULL;
	m_bCompletedLINK = TRUE;		// �۾��Ұ��� �����Ƿ� �Ϸ�� �����Ѵ�.

	// rm #906, rm #1213
	// �̰� �������� ������ CalculateVertexInflu ���� m_vecTrace ���� �ְ�,
	// m_rMeshContainer ���� ��� vVert ���� ���� ���� �ʴ´�.
	// m_vecTrace �� �ִٸ� m_rMeshContainer �� ������ �ִٰ� �����ϵ��� �Ѵ�.
	// GetTracor üũ�� �����Ѵٴ� �� CalculateVertexInflu �� ������ ������ ���� vVert ���� �Ѱ��� �� �ִٴ� ���̴�.
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

	// Release �� �߱� ������ ������� �ʿ��ϴ�.
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

	// D3DPOOL_DEFAULT �� ��������� ���� �ִ�.
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

	//	Note : ����Ʈ ������Ʈ.
	//
	for ( size_t n=0; n<m_vecEFFECT.size(); )
	{
		DxEffChar* pEFFECT = m_vecEFFECT[n];

		pEFFECT->SetCurrentAni ( MType, SType, dwKeyTime );
		pEFFECT->FrameMove ( fTime, fETime );

		//	Note : ������� ���޽ÿ� Effect ����.
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

	// Fps�� ���� ���, �ᱹ �Ź� Prev �� ���� Key �� ���� ������ ���� ����. 
	// ĳ���� �ִϸ��̼��� ���� 30fps.
	//if ( spSkinAniControlThread->GetPrevKeyFrame() != dwKeyTime )
	{
		NSSkinAniThread::AddCharPartBoneMatrices( m_spCharPartBoneMatrices, bNoneOverFrame );
	}

	// [shhan][2015.04.29] m_Tracor ���� ���� ������ �߻��Ѵ�.
	//						�ϴ� �ߺ������ �ؾ߰ڴ�. ���� ����� �����غ���.
	//
	{
		// FALSE ��� ���� �̻��� ������ ����Ŵ�.
		// Ȥ�� ó�� �����Ǿ��� Thread �۾����̶� ���� ���� �� �ִ�.
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
	////						FrameMove �� ���� �������� ��� ������, Render �� ���� �� ���̸� ����� ���� �ʴ´�.
	////						�׷��� m_vecTrace ��� m_vPos �ʱⰪ�� FLT_MAX �̰�, Render �� �ȵ� ���¿���, �� ������ ��� Effect �� �¾��� ���,
	////						��ġ���� m_vecTrace ���� ã�µ� FLT_MAX ���� �� �־ ������ �Ǿ���.
	////						�� �����Ǵ� ������ DxEffectMesh::Render ���� pLandMan->IsCollisionEX �� �ϰ�, �ᱹ �߸��� �Է°����� fps �� �������� ������ �߻��Ͽ���.
	////
	////						m_vecTrace ��� m_vPos �ʱⰪ�� Zero �� �ϴ� ����.
	////						
	////						�ʱⰪ�� Zero �� �����ϸ� Effect �� �������� ���ܼ� ������ �� ���̹Ƿ�, Bone �� Root ������ �����Ͽ� ������ ������ ������ �Ѵ�.
	////						( ������ �����ڸ��� FrameMove �� �ϱ����� Effect �� �´´ٸ�, �������� ���� ���� �ִ�. (���� ����Ȯ��) )
	////
	////						��������� Render ���� sTracor.m_vPos �۾��� ������ Render�� Culling ���� ����� �� �� �־ 
	////						FrameMove �󿡼� �ʱⰪ�� �����Ѵ�.
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
		bCrashRender = TRUE;	// ������ �浹�� �Ͼ��.

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

	// Note : Bone �� ������ ���´�.
	//		 ���� ���� �ڵ�.
	for ( DWORD i=0; i<m_vecEFFECT.size(); ++i )
	{
		m_vecEFFECT[i]->SetSkeleton( pSkeleton );
	}

	//	Note : �Ϲ� �޽��� ��� Bone�� ��ũ�Ǿ� �ִ°� ����.
	//		���� ������ ���� �ٸ� ���� ���ȴٴ°� �����Ͽ� ���� �ٽ� ����	������.
	//
	if ( !m_rMeshContainer->pSkinInfo )
	{
		//	Note : ���� �޴� ���� matrix �����͸� �ٽ� ã�´�.
		//			m_pmcMesh->m_szBoneNames[0];	//	[LOCAL]
		//			m_pmcMesh->m_szBoneNames[1];	//	[LOCAL]
		//			m_pmcMesh->m_szBoneNames[2];	//	[COMBINED] <-- �̰͸� ������ BONE���� ������ �޴°�.
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

		// Note : Ȱ�� � �ٴ� ��ġ�� �̸��� �ٲ���µ� .. ���� �̸��� ���� ������̶� �̷��� �ؾ��Ѵ�. < �ذ��� ���� �����ؾ��� >
		if( !m_pSlotHeld && (m_emWeaponWhereBack==EMPEACE_WEAPON_WHAL_SLOT) )
		{
			m_pSlotHeld = pSkeleton->FindBone ( _T("slot_lhand") );
		}

		// m_bUserBackSlot �� TRUE �̰�, Weapon Use Slot ���� ���� �Ǿ� ������, UserBackSlot ���θ� ���۵ǵ��� �Ѵ�.
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

		// Note : �Ϲ� �޽��� ��� ��Ʈ������ �̸� ����� ���´�.
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

	//	Note : ĳ���� �ؽ��� ���Ͽ� ����ŧ�� ���� ��� ���� ��� 1���� �Ѹ��� ���ؼ� �⺻ ���������� �Ѹ��� �ʴ´�.
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

	//	Note : �⺻ ĳ���� ��½� trace point calc, render effect skin �� ���Ѵ�.
	//
	if ( !bShadow && bEff )
	{
		size_t nSize = m_vecTrace.size();
		for ( size_t n=0; n<nSize; ++n )
		{
			STRACOR &sTracor = m_vecTrace[n];

			m_rMeshContainer->CalculateVertexInfluences ( sTracor.m_sVertexInflu, sTracor.m_vPos, sTracor.m_vNormal, &sTracor.m_BoneMatrice, FALSE, *pSkeleton, m_matStaticLinkBone );
		}

        // ������ Ambient Material �� �����´�.
        {
            SKINEFF_LEVEL_DATA* pLevelData(NULL);
		    GLEFFCHAR_VEC_ITER pos = m_vecEFFECT.begin();
		    for ( ; pos!=m_vecEFFECT.end(); ++pos )
		    {
			    (*pos)->GetLevelData( pd3dDevice, &pLevelData );
		    }
		    sSKINEFFDATA.m_rLevelData = pLevelData;

            // ������
            {
		        //	Note : render effect skin
		        //
		        GLEFFCHAR_VEC_ITER pos = std::lower_bound ( m_vecEFFECT.begin(), m_vecEFFECT.end(), EMEFFSO_NORMAL, DXEFFCHAR_OPER() );
		        for ( ; pos!=m_vecEFFECT.end(); ++pos )
		        {
			        DxEffChar* pEFFECT = (*pos);
			        if ( pEFFECT->GetStateOrder() > EMEFFSO_NORMAL )	break;	//	EMEFFSO_NORMAL�� �ƴϸ� �ߴ�.

			        if( bCrashRender )
			        {
				        switch( RENDERPARAM::emCharRenderTYPE )
				        {
				        case EMCRT_SOFTWARE:
				        case EMCRT_NORMAL:
				        case EMCRT_VERTEX:
					        if( pEFFECT->GetFlag() & EMECF_NORMALMAP )	continue;	// Pixel Shader �� ���ٰ� ������ ������ �۵� ���Ѵ�.
					        break;
				        case EMCRT_PIXEL:
					        if( pEFFECT->GetFlag() & EMECF_NONHLSL )	continue;	// �浹�� �Ͼ�� ���� �Ѹ��� �ʴ´�.
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
					if ( pEFFECT->GetStateOrder() > EMEFFSO_ADD )	break;	//	EMEFFSO_ADD�� �ƴϸ� �ߴ�.

					g_bCHAR_EDIT_PIECE_RENDER_FIXED = FALSE;
					pEFFECT->SetWorldMatrix( matWorld );
					pEFFECT->Render ( pd3dDevice, m_spCharPartBoneMatrices->GetBoneMatrices(), sSKINEFFDATA );
				}
            }

            // �ʱ�ȭ
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
									  const D3DXMATRIX* pmatAttParentBone,	// AttBone �� �θ��� �� �ִ�.
									  const D3DXMATRIX& matWorld,
									  const float fScale,
									  const DxClothColl* pClothColl,
									  const std::vector<physx::PxClothCollisionSphere>* pvecClothCollSpheres,
									  float fAttParentBoneScale,
									  BOOL bEffect )
{
	// ���� �̻��� ������ ����Ŵ�.
	// Ȥ�� ó�� �����Ǿ��� Thread �۾����̶� ���� ���� �� �ִ�.
	if ( !sSkeleton.IsActiveBone() )
		return;

	if ( !IsEnableRender() )
		return;

	if ( RENDERPARAM::g_emCharacterQulity < TnL_CHAR_PS_1_1 )
		return;

	//	Note : �Ϲ� �޽��� ��� Bone�� ��ũ�Ǿ� �ִ°� ����.
	//		���� ������ ���� �ٸ� ���� ���ȴٴ°� �����Ͽ� ���� �ٽ� ����	������.
	//
	if ( !m_rMeshContainer->pSkinInfo )
	{
		//	Note : ���� �޴� ���� matrix �����͸� �ٽ� ã�´�.
		//			m_pmcMesh->m_szBoneNames[0];	//	[LOCAL]
		//			m_pmcMesh->m_szBoneNames[1];	//	[LOCAL]
		//			m_pmcMesh->m_szBoneNames[2];	//	[COMBINED] <-- �̰͸� ������ BONE���� ������ �޴°�.
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

		// Note : Ȱ�� � �ٴ� ��ġ�� �̸��� �ٲ���µ� .. ���� �̸��� ���� ������̶� �̷��� �ؾ��Ѵ�. < �ذ��� ���� �����ؾ��� >
		if( !m_pSlotHeld && (m_emWeaponWhereBack==EMPEACE_WEAPON_WHAL_SLOT) )
		{
			m_pSlotHeld = sSkeleton.FindBone ( _T("slot_lhand") );
		}

		// m_bUserBackSlot �� TRUE �̰�, Weapon Use Slot ���� ���� �Ǿ� ������, UserBackSlot ���θ� ���۵ǵ��� �Ѵ�.
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

		// Note : �Ϲ� �޽��� ��� ��Ʈ������ �̸� ����� ���´�.
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
	// m_matStaticLinkBone ���� �Ŀ� �� �κ��� �۾��Ǿ� �Ѵ�.
	// ����� �� ���ƾ� �Ѵ�.
	size_t nSize = m_vecTrace.size();
	for ( size_t n=0; n<nSize; ++n )
	{
		STRACOR &sTracor = m_vecTrace[n];

		m_rMeshContainer->CalculateVertexInfluences ( sTracor.m_sVertexInflu, sTracor.m_vPos, sTracor.m_vNormal, &sTracor.m_BoneMatrice, FALSE, sSkeleton, m_matStaticLinkBone );
	}


	//////////////////////////////////////////////////////////////////////////
	// Note : m_vecTrace �� ��� �� �Ŀ� PrecalculateForMaterialSystem �� �ҷ����� �Ѵ�.
	m_scpEffectMaterialData->InitForPrecalculateForMaterialSystem();
	for ( DWORD i=0; i<m_vecEFFECT.size(); ++i )
	{
		m_vecEFFECT[i]->SetSkeleton( &sSkeleton );
		m_vecEFFECT[i]->SetWorldMatrix( matWorld );
		m_vecEFFECT[i]->PrecalculateForMaterialSystem( pd3dDevice, m_scpEffectMaterialData.get(), pmatAttParentBone ? pmatAttParentBone : &matWorld , fScale );

		// ȿ���� �Ѹ� ���� �̸������ �ϵ��� �Ѵ�.
		if ( bEffect )
		{
			m_vecEFFECT[i]->PrecalculateBeforeRenderEff( pd3dDevice, pmatAttParentBone ? pmatAttParentBone : &matWorld );
		}
	}


	for ( DWORD i=0; i<m_dwMaterialNum; ++i )
	{
		m_rSkinPiece->m_arrayMaterialPiece[i].PrecalculateForMaterialSystem( m_scpEffectMaterialData.get(), i );
	}


	// VisualMaterial �̶�� VM ���� �������ϰ� Material System ���� ���������� �ʵ��� �Ѵ�.
	if ( m_dwFlags & EMECF_VISUAL_MATERIAL )
	{
		// ������ Ambient Material �� �����´�.
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

	//	Note : ĳ���� �ؽ��� ���Ͽ� ����ŧ�� ���� ��� ���� ��� 1���� �Ѹ��� ���ؼ� �⺻ ���������� �Ѹ��� �ʴ´�.
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

	// Opaque ������.
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

	// DxEffCharMultiTex �� pEffMaterialData->m_rMultiTex ���� ������ �ɼ� �ֱ� ������, �������� �׷������� �Ѵ�.
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

	// DxEffCharMultiTex �� pEffMaterialData->m_rMultiTex ���� ������ �ɼ� �ֱ� ������, �������� �׷������� �Ѵ�.
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

	// DxEffCharMultiTex �� pEffMaterialData->m_rMultiTex ���� ������ �ɼ� �ֱ� ������, �������� �׷������� �Ѵ�.
	for ( DWORD i=0; i<m_vecEFFECT.size(); ++i )
	{
		m_vecEFFECT[i]->PrecalculateForMaterialSystemAdd( pd3dDevice, m_scpEffectMaterialData.get() );
	}

	GLEFFCHAR_VEC_ITER pos = std::lower_bound ( m_vecEFFECT.begin(), m_vecEFFECT.end(), EMEFFSO_ADD, DXEFFCHAR_OPER() );
	for ( ; pos!=m_vecEFFECT.end(); ++pos )
	{
		DxEffChar* pEFFECT = (*pos);
		if ( pEFFECT->GetStateOrder() > EMEFFSO_ADD )	break;	//	EMEFFSO_ADD�� �ƴϸ� �ߴ�.

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
		if ( pEFFECT->GetStateOrder() > EMEFFSO_LEVEL )	break;	//	EMEFFSO_GLOW, EMEFFSO_LEVEL�� �ƴϸ� �ߴ�.

		pEFFECT->RenderMaterialGlow( pd3dDevice, m_rSkinPiece->m_arrayMaterialPiece, m_matStaticLinkBone, *pSkinMesh, m_spPhysXCloth.get() );
	}
}

BOOL DxCharPart::RenderMaterialGhosting( LPDIRECT3DDEVICEQ pd3dDevice, SKINEFFDATA& sSKINEFFDATA, const SKIN_SPEC_SSS_INFO& sSkinInfo, float fAlpha, size_t unGhostingIndex )
{
	// m_rMeshContainer �� NULL �� �� �ִ�. ������ �װ� m_spCharPartBoneMatrices->GetBoneMatricesForGhosting( unGhostingIndex ); NULL �� �ƴϴ�.
	if ( !IsEnableRender() )
		return TRUE;

	DxSkinMesh9_Material* pSkinMesh = dynamic_cast<DxSkinMesh9_Material*>( m_rSkinMesh );
	if ( !pSkinMesh )
		return FALSE;


	////////////////////////////////////////////////////////////////////////////
	//// VisualMaterial �̶�� VM ���� �������ϰ� Material System ���� ���������� �ʵ��� �Ѵ�.
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
		return S_OK;	// �׸��� �ɼ��� ������ ����.

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
		if ( pEFFECT->GetStateOrder() > EMEFFSO_LEVEL )	break;	//	EMEFFSO_GLOW, EMEFFSO_LEVEL�� �ƴϸ� �ߴ�.

		pEFFECT->RenderGlow( pd3dDevice, m_spCharPartBoneMatrices->GetBoneMatrices(), FALSE, (m_dwFlags & EMECF_VISUAL_MATERIAL) );
	}
}

void DxCharPart::RenderEff( LPDIRECT3DDEVICEQ pd3dDevice, const float fScale, const D3DXMATRIX* pAttBone )
{
	if ( !IsEnableRender() )
		return;

	const D3DXMATRIX* pMatrix(NULL);
	if ( !m_rMeshContainer->pSkinInfo )    // �Ϲ� �޽��̸� ��� �� ���� ������ �ѷ��ش�.
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
			if ( pEFFECT->GetStateOrder() > EMEFFSO_SINGLE_EFF )	break;	//	EMEFFSO_SINGLE_EFF�� �ƴϸ� �ߴ�.

			// ���ʿ��� ���־ �� ���ص� �ȴ�.
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
		return S_OK;	// �׸��� �ɼ��� ������ ����.

	// ����� �ʹ� ���ٸ� ������ ���� �ʴ´�.
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
// m_vecTrace �� �ִٸ� m_rMeshContainer �� ������ �ִٰ� �����ϵ��� �Ѵ�.
// GetTracor üũ�� �����Ѵٴ� �� CalculateVertexInflu �� ������ ������ ���� vVert ���� �Ѱ��� �� �ִٴ� ���̴�.
//
// m_vecTrace �� m_rMeshContainer �� ��Ī�� �ȵǸ鼭 vVert �� ���� ���־����, ���ÿ��� vVert �� �ʱ�ȭ�� ������ �ʾƼ� ������ �߻��Ͽ���.
//
//	pBoneMatrice -> (����) ������ �޴� ���� ������ �������� ��쿡 ������ ����.
//	������ �̸� ���Ǿ� �����Ƿ� ���� ������ ��ȯ.
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
	// [shhan][2015.01.12] ran ���� ĳ���� LOD ����� �ʿ䰡 ����. ĳ���� ������ �Ǵ� �þ߰� ���� �ʴ�.
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

// Material ���� ���� DxEffCharNone �� ���� ���, ���ο� Material Rendering �ʿ��� �⺻������ ���ؾ� �Ѵٰ� �˾������� �Ѵ�.
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
// �ܺ� �ɼǿ� ���� ON/OFF �� ���� ���� Data ��ȭ�� ȣ�� �Լ� �� ����
BOOL DxCharPart::SetPhysX_Cloth( BOOL bOnOff )
{
	if ( !m_bCompletedLINK )
		return FALSE;

	// ������ش�.
	if ( bOnOff && !m_spPhysXCloth.get() && NSPhysX::IsPhysX() )
	{
		m_spPhysXCloth = boost::shared_ptr<DxPhysXCloth>(new DxPhysXCloth);
		m_spPhysXCloth->Import( m_sUsedLoadingChangeData.m_rSkinPiece );
	}
	else if ( !bOnOff && m_spPhysXCloth.get() )
	{
		// �����ش�.
		m_spPhysXCloth.reset();
	}

	return TRUE;
}

// SafeMode ���� WeaponSlot ��ġ�̵�. Link �� �ƴ� Skin ���� ����
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