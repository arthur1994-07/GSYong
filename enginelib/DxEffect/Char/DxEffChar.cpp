#include "pch.h"

#include "../../DxTools/DxShadowMap.h"

#include "./DxEffCharSingle.h"
#include "./DxEffCharBlur.h"
#include "./DxEffCharCloneBlur.h"
#include "./DxEffCharDust.h"
#include "./DxEffCharShock.h"
#include "./DxEffCharAttribute.h"
#include "./DxEffCharArrow.h"
#include "./DxEffCharSpecular.h"
#include "./DxEffCharSpecular2.h"
#include "./DxEffCharEmit.h"
#include "./DxEffCharAlpha.h"
#include "./DxEffCharNeon.h"
#include "./DxEffCharMark.h"
#include "./DxEffCharNoAlpha.h"
#include "./DxEffCharReflection2.h"
#include "./DxEffCharAmbient.h"
#include "./DxEffCharDot3.h"
#include "./DxEffCharMultiTex.h"
#include "./DxEffCharGhosting.h"
#include "./DxEffCharLevel.h"
#include "./DxEffCharToon.h"
#include "./DxEffCharTexDiff.h"
#include "./DxEffCharParticle.h"
#include "./DxEffCharBonePosEff.h"
#include "./DxEffCharBoneListEff.h"
#include "./DxEffCharUserColor.h"
#include "./DxEffCharNormal.h"
#include "./DxEffCharLine2BoneEff.h"
#include "./DxEffCharHLSL.h"
#include "./DxCharAroundEff.h"
#include "./DxEffCharOverlay.h"
#include "./DxEffCharTotalShader.h"
#include "./DxEffCharVisualMaterial.h"
#include "./DxEffCharSplineSingleEff.h"
#include "./DxEffCharWorldBattleFlag.h"
#include "./DxEffCharNone.h"

#include "./DxEffChar.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

int		g_nITEMLEVEL	= 0;
int		g_nITEMLEVEL_MIN = 0;
int		g_nITEMLEVEL_MAX = 11;

extern BOOL		g_bCHAR_EDIT_PIECE_RENDER_FIXED;

//----------------------------------------------------------------------------------------------------------------------------
//                                  D   x       E   f   f       C   h   a   r
DxEffChar::DxEffChar(void)
	: m_dwRunFlag(NULL)
	, m_dwFlag(NULL)
	, m_fAge(0.0f)
	, m_pCharPart(NULL)
	, m_pSkinPiece(NULL)
	, m_bMyChar(FALSE)
	, m_dwKeyTime(0)
	, m_CurAniMType(AN_GUARD_N)
	, m_CurAniSType(AN_SUB_NONE)
	, m_rSkinAniControl(NULL)
	, m_rSkeleton(NULL)
{
	D3DXMatrixIdentity( &m_matWorld );
}

DxEffChar::~DxEffChar(void)
{
	CleanUp ();
}

void DxEffChar::Render( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* arrayBoneMatrices, const SKINEFFDATA& sSKINEFFDATA )
{
    Render( pd3dDevice, arrayBoneMatrices, g_bCHAR_EDIT_PIECE_RENDER_FIXED );
}

void DxEffChar::RenderGlow( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* arrayBoneMatrices, BOOL bPieceRender, BOOL bActiveVisualMaterial )
{
	// bPieceRender 나중에 지우기 바람.
    Render( pd3dDevice, arrayBoneMatrices, g_bCHAR_EDIT_PIECE_RENDER_FIXED );
}

//----------------------------------------------------------------------------------------------------------------------------
//                                  D   x       E   f   f       C   h   a   r       M   a   n
DxEffCharMan& DxEffCharMan::GetInstance()
{
	static DxEffCharMan Instance;
	return Instance;
}

DxEffCharMan::DxEffCharMan ()
{
	// 미리 Texture Bake
	RegistType( DxEffCharOverlay::TYPEID,	        DxEffCharOverlay::NAME );
	RegistType( DxEffCharWorldBattleFlag::TYPEID,	DxEffCharWorldBattleFlag::NAME );

	// 이게 사용되면 형상과 같이 렌더링이 제외된다.
	RegistType( DxEffCharNone::TYPEID,				DxEffCharNone::NAME );
	RegistType( DxEffCharVisualMaterial::TYPEID,	DxEffCharVisualMaterial::NAME );

	// 형상과 같이 렌더링.
	RegistType( DxEffCharTexDiff::TYPEID,		    DxEffCharTexDiff::NAME );
	RegistType( DxEffCharMultiTex::TYPEID,		    DxEffCharMultiTex::NAME );
	RegistType( DxEffCharLevel::TYPEID,			    DxEffCharLevel::NAME );

	// 형상을 추가 렌더링
	RegistType( DxEffCharNeon::TYPEID,			    DxEffCharNeon::NAME );			// ColorOP 조정해야 함

	// 형상과는 연관없이 렌더되는 것들. 파티클, 잔상
	RegistType( DxEffCharSingle::TYPEID,		    DxEffCharSingle::NAME );
	RegistType( DxEffCharBlur::TYPEID,			    DxEffCharBlur::NAME );
	RegistType( DxEffCharCloneBlur::TYPEID,		    DxEffCharCloneBlur::NAME );
	RegistType( DxEffCharArrow::TYPEID,			    DxEffCharArrow::NAME );
	RegistType( DxEffCharParticle::TYPEID,		    DxEffCharParticle::NAME );
	RegistType( DxEffCharBonePosEff::TYPEID,	    DxEffCharBonePosEff::NAME );
	RegistType( DxEffCharBoneListEff::TYPEID,	    DxEffCharBoneListEff::NAME );
	RegistType( DxEffCharLine2BoneEff::TYPEID,  	DxEffCharLine2BoneEff::NAME );	
	RegistType( DxCharAroundEff::TYPEID,	        DxCharAroundEff::NAME );
	RegistType( DxEffCharSplineSingleEff::TYPEID,	DxEffCharSplineSingleEff::NAME );

	//RegistType( DxEffCharShock::TYPEID,			    DxEffCharShock::NAME );			// MaterialSystem 으로 가면서 사용안함.
	//RegistType( DxEffCharSpecular2::TYPEID,		    DxEffCharSpecular2::NAME );		// MaterialSystem 으로 가면서 사용안함.
	//RegistType( DxEffCharAlpha::TYPEID,			    DxEffCharAlpha::NAME );			// MaterialSystem 으로 가면서 사용안함.
	//RegistType( DxEffCharNoAlpha::TYPEID,		    DxEffCharNoAlpha::NAME );			// MaterialSystem 으로 가면서 사용안함.
	//RegistType( DxEffCharReflection2::TYPEID,	    DxEffCharReflection2::NAME );		// MaterialSystem 으로 가면서 사용안함.
	//RegistType( DxEffCharAmbient::TYPEID,		    DxEffCharAmbient::NAME );			// MaterialSystem 으로 가면서 사용안함.
	//RegistType( DxEffCharMark::TYPEID,			    DxEffCharMark::NAME );			// MaterialSystem 으로 가면서 사용안함.
	//RegistType( DxEffCharGhosting::TYPEID,		    DxEffCharGhosting::NAME );		// MaterialSystem 으로 가면서 사용안함.
	//RegistType( DxEffCharUserColor::TYPEID,		    DxEffCharUserColor::NAME );		// MaterialSystem 으로 가면서 사용안함.
	//RegistType( DxEffCharNormal::TYPEID,		    DxEffCharNormal::NAME );			// MaterialSystem 으로 가면서 사용안함.
	//RegistType( DxEffCharTotalShader::TYPEID,   	DxEffCharTotalShader::NAME );		// MaterialSystem 으로 가면서 사용안함.
	//RegistType( DxEffCharToon::TYPEID,			    DxEffCharToon::NAME );			// 원래부터 사용안함.
	//RegistType ( DxEffCharEmit::TYPEID,			DxEffCharEmit::NAME );				// ColorOP 조정해야 함
	//RegistType ( DxEffCharDot3::TYPEID,		DxEffCharDot3::NAME );					// ColorOP 조정해야 함
	//RegistType ( DxEffCharAttribute::TYPEID,	DxEffCharAttribute::NAME );				// 현재 안 쓰인다.
	//RegistType ( DxEffCharSpecular::TYPEID,		DxEffCharSpecular::NAME );			// 현재 안 쓰인다.
	//RegistType( DxEffCharDust::TYPEID,			DxEffCharDust::NAME );				// 현재 안 쓰인다.
}

DxEffCharMan::~DxEffCharMan ()
{
	SAFE_DELETE(m_pEffectTypeList);
}

void DxEffCharMan::RegistType ( const DWORD TypeID, const char *Name )
{
	GASSERT(Name);

	DxEffCharType *pCurEff = m_pEffectTypeList;
	while(pCurEff)
	{
		GASSERT ( pCurEff->TYPEID != TypeID && "TypeID 중복 등록될수 없습니다." );
		GASSERT ( strcmp(pCurEff->NAME,Name) && "NAME가 중복 등록될수 없습니다." );

		pCurEff = pCurEff->pNext;
	}

	//	Note : 새 노드 생성후 대입.
	//
	DxEffCharType *pNewType = new DxEffCharType;
	pNewType->TYPEID = TypeID;
	StringCchCopy( pNewType->NAME, MAX_PATH, Name );

	//	Note : 리스트에 등록.
	//
	pNewType->pNext = m_pEffectTypeList;
	m_pEffectTypeList = pNewType;
}

DxEffChar* DxEffCharMan::CreateEffInstance ( DWORD TypeID )
{
	switch ( TypeID )
	{
	case EMEFFCHAR_SINGLE:
		return new DxEffCharSingle;
	case EMEFFCHAR_BLUR:
		return new DxEffCharBlur;
	case EMEFFCHAR_CLONEBLUR:
		return new DxEffCharCloneBlur;
	case EMEFFCHAR_DUST:
		return new DxEffCharDust;
	case EMEFFCHAR_SHOCK:
		return new DxEffCharShock;
	case EMEFFCHAR_ATTRIBUTE:
		return new DxEffCharAttribute;
	case EMEFFCHAR_ARROW:
		return new DxEffCharArrow;
	case EMEFFCHAR_SPECULAR:
		return new DxEffCharSpecular;
	case EMEFFCHAR_SPECULAR2:
		return new DxEffCharSpecular2;
	case EMEFFCHAR_EMIT:
		return new DxEffCharEmit;
	case EMEFFCHAR_ALPHA:
		return new DxEffCharAlpha;
	case EMEFFCHAR_NEON:
		return new DxEffCharNeon;
	case EMEFFCHAR_MARK:
		return new DxEffCharMark;
	case EMEFFCHAR_NOALPHA:
		return new DxEffCharNoAlpha;
	case EMEFFCHAR_REFLECTION2:
		return new DxEffCharReflection2;
	case EMEFFCHAR_AMBIENT:
		return new DxEffCharAmbient;
	case EMEFFCHAR_DOT3:
		return new DxEffCharDot3;
	case EMEFFCHAR_MULTITEX:
		return new DxEffCharMultiTex;
	case EMEFFCHAR_GHOSTING:
		return new DxEffCharGhosting;
	case EMEFFCHAR_LEVEL:
		return new DxEffCharLevel;
	case EMEFFCHAR_TOON:
		return new DxEffCharToon;
	case EMEFFCHAR_TEXDIFF:
		return new DxEffCharTexDiff;
	case EMEFFCHAR_PARTICLE:
		return new DxEffCharParticle;
	case EMEFFCHAR_BONEPOSEFF:
		return new DxEffCharBonePosEff;
	case EMEFFCHAR_BONELISTEFF:
		return new DxEffCharBoneListEff;
	case EMEFFCHAR_USERCOLOR:
		return new DxEffCharUserColor;
	case EMEFFCHAR_NORMALMAP:
		return new DxEffCharNormal;
	case EMEFFCHAR_LINE2BONE:
		return new DxEffCharLine2BoneEff;
	case EMEFFCHAR_AROUNDEFFECT:
		return new DxCharAroundEff;
	case EMEFFCHAR_OVERLAY:
		return new DxEffCharOverlay;
	case EMEFFCHAR_TOTALSHADER:
		return new DxEffCharTotalShader;
    case EMEFFCHAR_VISUAL_MATERIAL:
		return new DxEffCharVisualMaterial;
    case EMEFFCHAR_SPLINE_SINGLE_EFF:
		return new DxEffCharSplineSingleEff;
	case EMEFFCHAR_WORLD_BATTLE_FLAG:
		return new DxEffCharWorldBattleFlag;
	case EMEFFCHAR_NULL:
		return new DxEffCharNone;
	};

	

	return NULL;
}

DxEffChar* DxEffCharMan::CreateEffect ( DWORD TypeID, EFFCHAR_PROPERTY* pProp, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece )
{
	GASSERT(m_pd3dDevice);

	HRESULT hr = S_OK;

	DxEffChar* pEffChar = CreateEffInstance ( TypeID );
	if ( !pEffChar )	return NULL;

	pEffChar->SetLinkObj ( pCharPart, pSkinPiece );
	pEffChar->SetProperty ( pProp );
	hr = pEffChar->Create ( m_pd3dDevice );
	if ( FAILED(hr) )
	{
		SAFE_DELETE(pEffChar);
		return NULL;
	}

	return pEffChar;
}

DxEffChar* DxEffCharMan::CopyEffect( DxEffChar* pEff, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece )
{
	GASSERT(m_pd3dDevice);

	HRESULT hr = S_OK;

	DxEffChar* pEffChar = CreateEffInstance ( pEff->GetTypeID() );
	if ( !pEffChar )	return NULL;

	pEffChar->SetLinkObj ( pCharPart, pSkinPiece );
	pEffChar->SetProperty ( pEff->GetProperty() );
	hr = pEffChar->Create ( m_pd3dDevice );
	if ( FAILED(hr) )
	{
		SAFE_DELETE(pEffChar);
		return NULL;
	}

	return pEffChar;
}


DxEffChar* DxEffCharMan::AddDefaultAmbient ( DxCharPart* pCharPart, DxSkinPiece* pSkinPiece )
{
	GASSERT(m_pd3dDevice);

	HRESULT hr = S_OK;

	DxEffChar* pEffChar = CreateEffInstance ( DxEffCharAmbient::TYPEID );
	if ( !pEffChar )	return NULL;

	EFFCHAR_PROPERTY_AMBIENT	Property;
    Property.m_dwFlage |= USE_EFFCHAR_AMBIENT_BASECOLOR;
	Property.m_cColor = D3DXCOLOR( 180.0f/255.0f, 180.0f/255.0f, 180.0f/255.0f, 255.0f/255.0f );

	pEffChar->SetLinkObj ( pCharPart, pSkinPiece );
	pEffChar->SetProperty ( &Property );
	hr = pEffChar->Create ( m_pd3dDevice );
	if ( FAILED(hr) )
	{
		SAFE_DELETE(pEffChar);
		return NULL;
	}

	return pEffChar;
}

void	DxEffCharMan::DeleteEffect ( DxEffChar* pEffChar )
{
	SAFE_DELETE ( pEffChar );
}

HRESULT DxEffCharMan::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	NSCHARHLSL::CreateDevice( pd3dDevice );
	DxEffCharToon::StaticCreateDevice( pd3dDevice );
	DxEffCharSpecular2::StaticCreateDevice( pd3dDevice );
	DxEffCharBlur::OnCreateDevice_STATIC();
    DxEffCharOverlay::StaticInitDeviceObjects( pd3dDevice );
    DxEffCharLine2BoneEff::OnCreateDevice_STATIC ( m_pd3dDevice );
    DxEffCharBonePosEff::OnCreateDevice_STATIC ( m_pd3dDevice );
    DxEffCharBoneListEff::OnCreateDevice_STATIC ( m_pd3dDevice );
    DxEffCharParticle::OnCreateDevice_STATIC ( m_pd3dDevice );

	return S_OK;
}

HRESULT DxEffCharMan::RestoreDeviceObjects ()
{
	SetViewBoard ();	// 최대 최소 점 구하기

	//DxEffCharSingle::CreateDevice ( m_pd3dDevice );
	DxEffCharBlur::CreateDevice ( m_pd3dDevice );
	DxEffCharCloneBlur::CreateDevice ( m_pd3dDevice );
	DxEffCharDust::CreateDevice ( m_pd3dDevice );
	DxEffCharShock::CreateDevice ( m_pd3dDevice );
	DxEffCharAttribute::CreateDevice ( m_pd3dDevice );
	DxEffCharArrow::CreateDevice ( m_pd3dDevice );
	DxEffCharSpecular::CreateDevice ( m_pd3dDevice );
	DxEffCharEmit::CreateDevice ( m_pd3dDevice );
	DxEffCharNeon::CreateDevice ( m_pd3dDevice );
	DxEffCharMark::CreateDevice ( m_pd3dDevice );
	DxEffCharNoAlpha::CreateDevice ( m_pd3dDevice );
	DxEffCharReflection2::CreateDevice ( m_pd3dDevice );
	DxEffCharAmbient::CreateDevice ( m_pd3dDevice );
	DxEffCharDot3::CreateDevice ( m_pd3dDevice );
	DxEffCharMultiTex::CreateDevice ( m_pd3dDevice );
	DxEffCharGhosting::CreateDevice ( m_pd3dDevice );
	DxEffCharLevel::CreateDevice ( m_pd3dDevice );
	DxEffCharTexDiff::CreateDevice ( m_pd3dDevice );
	DxCharAroundEff::CreateDevice ( m_pd3dDevice );
	DxEffCharUserColor::CreateDevice ( m_pd3dDevice );

	NSCHARHLSL::ResetDevice( m_pd3dDevice );
	DxEffCharToon::StaticResetDevice( m_pd3dDevice );
	DxEffCharSpecular2::StaticResetDevice( m_pd3dDevice );
    DxEffCharOverlay::StaticRestoreDeviceObjects();
	DxEffCharNone::StaticResetDevice ( m_pd3dDevice );

	return S_OK;
}

HRESULT DxEffCharMan::InvalidateDeviceObjects ()
{
	//DxEffCharSingle::ReleaseDevice ( m_pd3dDevice );
	DxEffCharBlur::ReleaseDevice ( m_pd3dDevice );
	DxEffCharCloneBlur::ReleaseDevice ( m_pd3dDevice );
	DxEffCharDust::ReleaseDevice ( m_pd3dDevice );
	DxEffCharShock::ReleaseDevice ( m_pd3dDevice );
	DxEffCharAttribute::ReleaseDevice ( m_pd3dDevice );
	DxEffCharArrow::ReleaseDevice ( m_pd3dDevice );
	DxEffCharSpecular::ReleaseDevice ( m_pd3dDevice );
	DxEffCharEmit::ReleaseDevice ( m_pd3dDevice );
	DxEffCharNeon::ReleaseDevice ( m_pd3dDevice );
	DxEffCharMark::ReleaseDevice ( m_pd3dDevice );
	DxEffCharNoAlpha::ReleaseDevice ( m_pd3dDevice );
	DxEffCharReflection2::ReleaseDevice ( m_pd3dDevice );
	DxEffCharAmbient::ReleaseDevice ( m_pd3dDevice );
	DxEffCharDot3::ReleaseDevice ( m_pd3dDevice );
	DxEffCharMultiTex::ReleaseDevice ( m_pd3dDevice );
	DxEffCharGhosting::ReleaseDevice ( m_pd3dDevice );
	DxEffCharLevel::ReleaseDevice ( m_pd3dDevice );
	DxEffCharTexDiff::ReleaseDevice( m_pd3dDevice );
	DxCharAroundEff::ReleaseDevice ( m_pd3dDevice );
	DxEffCharUserColor::ReleaseDevice ( m_pd3dDevice );

	NSCHARHLSL::LostDevice();
	DxEffCharToon::StaticLostDevice();
	DxEffCharSpecular2::StaticLostDevice();
    DxEffCharOverlay::StaticInvalidateDeviceObjects();
	DxEffCharNone::StaticLostDevice();

	return S_OK;
}

HRESULT DxEffCharMan::DeleteDeviceObjects()
{
	m_pd3dDevice = NULL;

	NSCHARHLSL::DestroyDevice();
	DxEffCharToon::StaticDestroyDevice();
	DxEffCharSpecular2::StaticDestroyDevice();
	DxEffCharBlur::OnDestroyDevice_STATIC();
    DxEffCharOverlay::StaticDeleteDeviceObjects();
    DxEffCharLine2BoneEff::OnDestroyDevice_STATIC ( m_pd3dDevice );
    DxEffCharBonePosEff::OnDestroyDevice_STATIC ( m_pd3dDevice );
    DxEffCharBoneListEff::OnDestroyDevice_STATIC ( m_pd3dDevice );
    DxEffCharParticle::OnDestroyDevice_STATIC( m_pd3dDevice );

	return S_OK;
}

VOID	DxEffCharMan::SetViewBoard ()
{
	//	Note : 평면과 카메라 CV의 만나는 점 구하기.
	//
	D3DXVECTOR3	vViewMax, vViewMin;
	D3DXVECTOR3	vTemp;
	D3DXVECTOR3	vCenter = D3DXVECTOR3 ( 0.f, 0.f, 0.f );

	D3DXVECTOR3		vUpVec ( 0.f, 1.f, 0.f );
	D3DXVECTOR3		vTempLookatPt	= vCenter + vUpVec*100.f;
	vUpVec = D3DXVECTOR3 ( 0.f, 0.f, -1.f );
	D3DXVECTOR3		vLookatPt	= vCenter;
	D3DXVECTOR3		vFromPt		= vTempLookatPt;
	D3DXVECTOR3		vPosition[4];

	CLIPVOLUME pCV = DxViewPort::GetInstance().ComputeClipVolume ( vCenter, vTempLookatPt, vUpVec, 0.5f, 10000.f, 512, 512, D3DX_PI/4 );	// CLIPVOLUME

	float	fNearPlane	= 0.5f;
	float	fFarPlane	= 10000.f;
	float	fWidth		= 128.f;
	float	fHeight		= 128.f;
	float	fFOV		= D3DX_PI/4;

	FLOAT dist, t;
	D3DXVECTOR3 pt[8];
	D3DXVECTOR3 v1, v2, n;
	D3DXVECTOR3 vDir, vUp, vPos;	
	D3DXVECTOR3 vCross;

	vDir = vLookatPt - vFromPt;
	vUp = vUpVec;
	vPos = vFromPt;

	D3DXVec3Normalize ( &vUp, &vUp );
	D3DXVec3Normalize ( &vDir, &vDir );

	D3DXVec3Cross ( &vCross, &vUp, &vDir );
	D3DXVec3Normalize ( &vCross, &vCross );

	D3DXVec3Cross ( &vUp, &vDir, &vCross );
	D3DXVec3Normalize ( &vUp, &vUp );

	for(INT i = 0; i < 8; i++)
	{
		//	Note	:	i의 값이 4이상일때
		//				먼곳에 대한 좌표이다.

		//	(1)
		dist = (i & 0x4) ? fFarPlane : fNearPlane;
		pt[i] = dist * vDir;

		//	(2)
		t = dist * tanf(fFOV/2);
		t = (i & 0x2) ? t : -t;
		pt[i] += vUp * t;
		
		t = dist * tanf(fFOV/2) * fWidth/fHeight; // take into account screen proportions
		t = (i & 0x1) ? -t : t;
		pt[i] += vCross *t;
		pt[i] = vPos + pt[i];
	}

	//	Note : 면 만들기
	D3DXPLANE	pPlane;
	vPosition[0] = D3DXVECTOR3 ( 1.f, 0.f, 0.f );
	vPosition[1] = D3DXVECTOR3 ( 0.f, 0.f, 1.f );
	vPosition[2] = D3DXVECTOR3 ( -1.f, 0.f, 0.f );
	D3DXPlaneFromPoints ( &pPlane, &vPosition[0], &vPosition[1], &vPosition[2] );

	//	Note : 점 만들기
	vPosition[0] = DxSplit ( &pPlane, &pt[4], &pt[0] );
	vPosition[1] = DxSplit ( &pPlane, &pt[5], &pt[1] );
	vPosition[2] = DxSplit ( &pPlane, &pt[6], &pt[2] );
	vPosition[3] = DxSplit ( &pPlane, &pt[7], &pt[3] );

	m_vViewMax = m_vViewMin = vPosition[0];

	m_vViewMax.x = ( m_vViewMax.x > vPosition[0].x ) ? m_vViewMax.x : vPosition[0].x;
	m_vViewMax.x = ( m_vViewMax.x > vPosition[1].x ) ? m_vViewMax.x : vPosition[1].x;
	m_vViewMax.x = ( m_vViewMax.x > vPosition[2].x ) ? m_vViewMax.x : vPosition[2].x;
	m_vViewMax.x = ( m_vViewMax.x > vPosition[3].x ) ? m_vViewMax.x : vPosition[3].x;

	m_vViewMax.z = ( m_vViewMax.z > vPosition[0].z ) ? m_vViewMax.z : vPosition[0].z;
	m_vViewMax.z = ( m_vViewMax.z > vPosition[1].z ) ? m_vViewMax.z : vPosition[1].z;
	m_vViewMax.z = ( m_vViewMax.z > vPosition[2].z ) ? m_vViewMax.z : vPosition[2].z;
	m_vViewMax.z = ( m_vViewMax.z > vPosition[3].z ) ? m_vViewMax.z : vPosition[3].z;

	m_vViewMin.x = ( m_vViewMin.x < vPosition[0].x ) ? m_vViewMin.x : vPosition[0].x;
	m_vViewMin.x = ( m_vViewMin.x < vPosition[1].x ) ? m_vViewMin.x : vPosition[1].x;
	m_vViewMin.x = ( m_vViewMin.x < vPosition[2].x ) ? m_vViewMin.x : vPosition[2].x;
	m_vViewMin.x = ( m_vViewMin.x < vPosition[3].x ) ? m_vViewMin.x : vPosition[3].x;

	m_vViewMin.z = ( m_vViewMin.z < vPosition[0].z ) ? m_vViewMin.z : vPosition[0].z;
	m_vViewMin.z = ( m_vViewMin.z < vPosition[1].z ) ? m_vViewMin.z : vPosition[1].z;
	m_vViewMin.z = ( m_vViewMin.z < vPosition[2].z ) ? m_vViewMin.z : vPosition[2].z;
	m_vViewMin.z = ( m_vViewMin.z < vPosition[3].z ) ? m_vViewMin.z : vPosition[3].z;
}

