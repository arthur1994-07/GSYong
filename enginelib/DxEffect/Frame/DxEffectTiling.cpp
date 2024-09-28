// Terrain0.cpp: implementation of the DxEffectTiling class.
//
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "../../../VisualMaterialLib/Define/DxVMDefine.h"

#include "../../../SigmaCore/File/SerialFile.h"

#include "../../DxEffect/TexEff/DxTexEffVisualMaterial.h"
#include "../../DxEffect/TexEff/DxTexEff.h"
#include "../../DxEffect/Frame/DxEffectDefine.h"
#include "../../DxEffect/Frame/DxEffectMan.h"
#include "../../DxEffect/Mesh/DxStaticMesh.h"
#include "../../DxMeshs/FrameMesh/NSVERTEXNORTEX_FX.h"
#include "../../DxMeshs/FrameMesh/DxFrameMesh.h"
#include "../../DxMeshs/SkinMesh/DxMaterial.h"
#include "../../DxTools/TextureManager.h"
#include "../../DxTools/Material/NSMaterialManager.h"
#include "../../DxTools/RENDERPARAM.h"
#include "../../DxTools/DxViewPort.h"
#include "../../DxTools/EditMeshs.h"
#include "../../DxTools/Collision.h"
#include "../../Common/StlFunctions.h"
#include "../../DxLand/LightMap/LightMapObject.h"
#include "../../DxLand/LightMap/NSLightMap.h"
#include "../../DxLand/DxPiece/DxSingleTexMesh.h"
#include "../../DxLand/DxLandMan.h"
#include "../TexEff/DxTextureEffMan.h"

#include "DxEffectTiling.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

extern BOOL g_bWIREFRAME_VIEW;

const DWORD DxEffectTiling::TYPEID = DEF_EFFECT_TILING;
const DWORD	DxEffectTiling::VERSION = 0x00000106;
const char DxEffectTiling::NAME[] = "[ Ÿ�� �ý��� : ���� ]";
const DWORD DxEffectTiling::FLAG = _EFF_SINGLE;

LPDIRECT3DSTATEBLOCK9		DxEffectTiling::m_pSavedSB			= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffectTiling::m_pDrawSB			= NULL;

LPDIRECT3DSTATEBLOCK9		DxEffectTiling::m_pSavedSB_Select	= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffectTiling::m_pDrawSB_Select	= NULL;

const char* DxEffectTiling::GetName ()
{
	m_strPriorityName.Format( "%s - %d", NAME, m_dwRenderPriority );
	return m_strPriorityName.GetString();
}

void DxEffectTiling::GetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD &dwVer )
{
	pProp = (PBYTE) &m_Property;
	dwSize = sizeof(TILING_PROPERTY);
	dwVer = VERSION;
}

void DxEffectTiling::SetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD dwVer )
{

	if ( dwVer==VERSION && dwSize==sizeof(TILING_PROPERTY) )
	{
		memcpy ( &m_Property, pProp, dwSize );
	}
	if ( dwVer==0x00000102 && dwSize==sizeof(TILING_PROPERTY_OLD) )
	{
		TILING_PROPERTY_OLD oldProperty;
		memcpy ( &oldProperty, pProp, dwSize );
		m_Property.Assign( oldProperty );
	}
	if ( dwVer==0x00000101 && dwSize==sizeof(TILING_PROPERTY_OLD) )
	{
		TILING_PROPERTY_OLD oldProperty;
		memcpy ( &oldProperty, pProp, dwSize );
		m_Property.Assign( oldProperty );
	}
	else if ( dwVer==0x00000100 && dwSize==sizeof(TILING_PROPERTY_OLD) )
	{
		TILING_PROPERTY_OLD oldProperty;
		memcpy ( &oldProperty, pProp, dwSize );
		m_Property.Assign( oldProperty );
	}
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
DxEffectTiling::DxEffectTiling () :
	m_pPoint(NULL),
	m_pPointEX(NULL),
	m_pPointEX_Load(NULL),
	m_pTriangle(NULL),
	m_pMaterials(NULL),
	m_dwPointEX(0),
	m_dwPointEX_Load(0),
	m_dwMaterials(0),
	m_dwCurTILE(0),
	m_dwCurPOINT(0),
	m_dwCurPOINT_PREV(0),
	m_bSelectDRAW(FALSE),
	m_vMax(0.f,0.f,0.f),
	m_vMin(0.f,0.f,0.f),
	m_dwVertices(0),
	m_dwFaces(0),
	m_dwMaterials_B(0),
	m_dwUNIQUE_NUMBER(1),
	m_dwRenderPriority(0)
{

	m_dwFlag = USE_TEX_DRAW;


	UseAffineMatrix ();
	D3DXMatrixIdentity ( &m_matFrameComb );

	m_cCurCOLOR.a = 0.f;
	m_cCurCOLOR.r = 0.6f;
	m_cCurCOLOR.g = 0.6f;
	m_cCurCOLOR.b = 0.6f;

	m_cCurRANDOM.a = 0.7f;
	m_cCurRANDOM.r = 0.4f;
	m_cCurRANDOM.g = 0.4f;
	m_cCurRANDOM.b = 0.4f;

    m_pLightMapData = new LightMapObject_Data;
}

DxEffectTiling::~DxEffectTiling ()
{
	//SAFE_DELETE_ARRAY(m_szAdaptFrame);

	SAFE_DELETE_ARRAY ( m_pPoint );
	SAFE_DELETE_ARRAY ( m_pPointEX );
	SAFE_DELETE_ARRAY ( m_pPointEX_Load );
	SAFE_DELETE_ARRAY ( m_pTriangle );
	SAFE_DELETE_ARRAY ( m_pMaterials );

	//std::for_each ( m_mapMaterials.begin(), m_mapMaterials.end(), std_afunc::DeleteMapObject() );
	m_mapMaterials.clear();

	m_listSelectTriangle.clear();

	m_mapFindEX.clear();

    SAFE_DELETE( m_pLightMapData );
	//CleanUp ();
}

HRESULT DxEffectTiling::AdaptToDxFrameChild ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	DxMeshes *pmsMeshs;

	if ( pframeCur->pmsMeshs != NULL )
	{
		pmsMeshs = pframeCur->pmsMeshs;

		while ( pmsMeshs != NULL )
		{
			if ( pmsMeshs->m_pLocalMesh )
			{
				SetBaseQuad( pd3dDevice, pmsMeshs );
			}

			pmsMeshs = pmsMeshs->m_pMeshNext;
		}
	}

	return hr;
}

//	Note : DxFrame �� ȿ���� ���� ��쿡 ���ȴ�.
//
HRESULT DxEffectTiling::AdaptToDxFrame ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !pFrame )		return S_OK;

	m_pAdaptFrame = pFrame;
	m_matFrameComb = pFrame->matCombined;

	this->pLocalFrameEffNext = pFrame->pEffectNext;
	pFrame->pEffectNext = this;

	GASSERT(pFrame->szName);
	if ( pFrame->szName )
	{
		SAFE_DELETE_ARRAY(m_szAdaptFrame);
		size_t nStrLen = strlen(pFrame->szName)+1;
		m_szAdaptFrame = new char[nStrLen];

		StringCchCopy( m_szAdaptFrame, nStrLen, pFrame->szName );
	}

	return AdaptToDxFrameChild ( pFrame, pd3dDevice );
}

HRESULT DxEffectTiling::AdaptToEffList ( const DxFrame *const pFrame, LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT ( pFrame );

	m_pAdaptFrame = NULL;
	m_matFrameComb = pFrame->matCombined;

	if ( pFrame->szName )
	{
		SAFE_DELETE_ARRAY(m_szAdaptFrame);
		size_t nStrLen = strlen(pFrame->szName)+1;
		m_szAdaptFrame = new char[nStrLen];

		StringCchCopy( m_szAdaptFrame, nStrLen, pFrame->szName );
	}

	return S_OK;
}

HRESULT DxEffectTiling::OneTimeSceneInit ()
{
	HRESULT hr = S_OK;

	return hr;
}

HRESULT DxEffectTiling::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	float fBias = -0.00001f;
	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState( D3DRS_LIGHTING,			FALSE );

		pd3dDevice->SetRenderState( D3DRS_DEPTHBIAS,		*((DWORD*)&fBias) );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_INVSRCALPHA );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG2 );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedSB );
		else			pd3dDevice->EndStateBlock( &m_pDrawSB );
	}

	fBias = -0.00002f;
	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,			*((DWORD*)&fBias) );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR,		0x44000000 );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1 );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG1 );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedSB_Select );
		else			pd3dDevice->EndStateBlock( &m_pDrawSB_Select );
	}

	return S_OK;
}

HRESULT DxEffectTiling::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSavedSB );
	SAFE_RELEASE( m_pDrawSB );
	SAFE_RELEASE( m_pSavedSB_Select );
	SAFE_RELEASE( m_pDrawSB_Select );

	return S_OK;
}

HRESULT DxEffectTiling::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT DxEffectTiling::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT DxEffectTiling::InvalidateDeviceObjects ()
{
	return S_OK;
}

HRESULT DxEffectTiling::DeleteDeviceObjects ()
{
	//DxStaticMeshColor_THREAD::GetInstance().DeleteList();	//	���� List�� �����.

	return S_OK;
}

HRESULT DxEffectTiling::FinalCleanup ()
{
	return S_OK;
}

//	Note : ������ ���� ������ �ʿ� ���� �κ��� �����ϰ�
//		������ ���� �������� ���� �氨�� ���� ���� �ܼ��ϰ� �����Ѵ�.
//
HRESULT DxEffectTiling::FrameMove ( float fTime, float fElapsedTime )
{
	m_listSelectTriangle.clear();

	m_dwCurPOINT_PREV = m_dwCurPOINT;

	// ������� ���� Tiling �� �Ⱥ��̵��� ����.
	if ( RENDERPARAM::g_emGroundQulity <= TnL_FIXED )
	{
		return S_OK;
	}

	//// Note : ��Ʈ���� ��츸 �۵�
	//if( !DxStaticMeshColor_THREAD::GetInstance().m_bOctreeMode )	return S_OK;

 //   MATERIALMAP_ITER iter = m_mapMaterials.begin();
	//for ( ; iter!=m_mapMaterials.end(); ++iter )
	//{
	//	if( (*iter).second->m_pMeshList )
	//	{
 //           (*iter).second->m_pMeshList->FrameMove( fElapsedTime );
	//	}
	//}

	//// Note : �÷� ����� ���Ͽ��� ����Ʈ�� ����ִٸ� �ٽ� �����Ѵ�.
	//if( DxStaticMeshColor_THREAD::GetInstance().IsInsertListData() )
	//{
	//	DxStaticMeshColor_THREAD::GetInstance().EnterCS();
	//	{
	//		LOADINGDATALIST& sListColorData = DxStaticMeshColor_THREAD::GetInstance().m_listColorData;

	//		MATERIALMAP_ITER iter = m_mapMaterials.begin();
	//		for ( ; iter!=m_mapMaterials.end(); ++iter )
	//		{
	//			if( !(*iter).second->m_pMeshList )
	//			{
	//				DxStaticMeshColor_THREAD::GetInstance().LeaveCS();
	//				return S_OK;
	//			}

	//			(*iter).second->m_pMeshList->InsertColorList( sListColorData );
	//		}
	//	}
	//	DxStaticMeshColor_THREAD::GetInstance().LeaveCS();
	//}

	return S_OK;
}

HRESULT DxEffectTiling::Render( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan )
{
    return Render( pFrame, pd3dDevice, pFrameMesh, pLandMan, NULL );
}

HRESULT DxEffectTiling::Render ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan, DxTextureEffMan* pTextureEff )
{
	PROFILE_BEGIN("DxEffectTiling");

	HRESULT hr = S_OK;

	// ������� ���� Tiling �� �Ⱥ��̵��� ����.
	if ( RENDERPARAM::g_emGroundQulity == TnL_FIXED )
	{
		return S_OK;
	}

	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity ( &matIdentity );
	matIdentity._42 += 0.05f;
	pd3dDevice->SetTransform ( D3DTS_WORLD, &matIdentity );

	DWORD dwFogColor;
	if ( g_bWIREFRAME_VIEW )
	{
		pd3dDevice->GetRenderState ( D3DRS_FOGCOLOR,	&dwFogColor );
		pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR,	0xff808080 );

        NSMaterialManager::SetFogColorForEditor ( 0xff808080 );
	}

	if ( pFrameMesh )
	{
		PROFILE_BEGIN("DxEffectTiling_Create");
		CreateTiles ( pd3dDevice, NULL );					// �ﰢ�� ����		// �ﰢ�� ���ý� ������ ���̵��� �ؾ� �Ѵ�.
		PROFILE_END("DxEffectTiling_Create");

		PROFILE_BEGIN("DxEffectTiling_Render");
		RenderTiles ( pd3dDevice, m_mapMaterials, pTextureEff );	// �ﰢ�� �Ѹ���
		PROFILE_END("DxEffectTiling_Render");

		if ( m_bSelectDRAW )
		{
			m_listSelectTriangle.clear();
			MATERIALMAP_ITER iter = m_mapMaterials.find(m_dwCurTILE);
			if ( iter != m_mapMaterials.end() )
			{
				TRIANGLELIST_ITER iter2 = (*iter).second->listTriangle.begin();
				for ( ; iter2!=(*iter).second->listTriangle.end(); ++iter2 )
				{
					m_listSelectTriangle.push_back ( (*iter2) );
				}
			}
			Render_Select ( pd3dDevice, FALSE );
		}
	}

	// ��Ʈ�� �϶�
	if ( pLandMan )
	{
		m_pSavedSB->Capture();
		m_pDrawSB->Apply();

		CLIPVOLUME	cv = DxViewPort::GetInstance().GetClipVolume ();

        D3DXMATRIX matView, matProj;
        matView = DxViewPort::GetInstance().GetMatView();
        matProj = DxViewPort::GetInstance().GetMatProj();

		MATERIALMAP_ITER iter = m_mapMaterials.begin();
		for ( ; iter!=m_mapMaterials.end(); ++iter )
		{
            if( (*iter).second->m_bAlpha )
            {
                pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );  // Default ���ø� ����.
                //pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG1,  D3DTA_TEXTURE );    // LightMap�� ����
                pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG2,  D3DTA_CURRENT );    // LightMap�� ����
                pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,    D3DTOP_SELECTARG2 );// LightMap�� ����
            }
	        else
            {
                pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG2 );// Default ���ø� ����.
                pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,    D3DTOP_DISABLE );   // LightMap�� ����
            }

			if ( (*iter).second->m_pMeshList )
			{
				sc::SerialFile& SFile = pLandMan->GetSerialFile();
				(*iter).second->m_pMeshList->RenderDYNAMIC( pd3dDevice, NSLandThread::TYPE_BASEMESH_CULL, cv, &SFile );
			}
		}

		m_pSavedSB->Apply();
	}

	if ( g_bWIREFRAME_VIEW )
	{
		pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR,	dwFogColor );

        NSMaterialManager::SetFogColorForEditor( dwFogColor );
	}

	PROFILE_END("DxEffectTiling");

	return S_OK;
}

void DxEffectTiling::RenderTiles ( LPDIRECT3DDEVICEQ pd3dDevice, MATERIALMAP& mapMaterials, DxTextureEffMan* pTextureEff )
{
	m_pSavedSB->Capture();
	m_pDrawSB->Apply();

	pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );	// FrameMesh�� Normal�� ����Ѵ�.

	CLIPVOLUME	cv = DxViewPort::GetInstance().GetClipVolume ();

	MATERIALMAP_ITER iter = m_mapMaterials.begin();
	for ( ; iter!=m_mapMaterials.end(); ++iter )
	{
		if( (*iter).second->m_bAlpha )	pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );
		else							pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG2 );

		RenderTiles ( pd3dDevice, (*iter).second.get(), pTextureEff );
		//if ( (*iter).first == m_dwCurTILE )
		//{
		//	RenderTiles ( pd3dDevice, (*iter).second );
		//}
		//else if ( (*iter).second->m_pMeshList )
		//{
		//	(*iter).second->m_pMeshList->Render ( pd3dDevice, &cv );
		//}
	}

	m_pSavedSB->Apply();
}

void DxEffectTiling::RenderTiles ( LPDIRECT3DDEVICEQ pd3dDevice, MATERIALEX* pMaterialEX, DxTextureEffMan* pTextureEff )
{
    D3DXMATRIX matIdentify;
    D3DXMatrixIdentity( &matIdentify );

	pd3dDevice->SetFVF ( VERTEXNORCOLORTEX::FVF );

	pd3dDevice->SetTexture ( 0, pMaterialEX->m_textureRes.GetTexture() );

    DxTexEffVisualMaterial* pTexEffVisualMaterial = NULL;
    if ( DxEffectMan::GetInstance().IsEnableCharPixelShader() )
    {
        if ( pTextureEff )
        {
            DxTexEffBase* pTexEff = pTextureEff->GetEffRoot( pMaterialEX->strTexture.c_str() );
            while ( pTexEff )
            {
                if ( pTexEff->GetTypeID() == DEF_TEXEFF_VISUAL_MATERIAL )
                {
                    // [2011.03.31] �ӵ��� �����ؼ� dynamic_cast -> static_cast�� ����.
                    //pTexEffVisualMaterial = dynamic_cast<DxTexEffVisualMaterial*>( pTexEff );
                    pTexEffVisualMaterial = MIN_BOOST::polymorphic_downcast<DxTexEffVisualMaterial*>( pTexEff );
                    pTexEffVisualMaterial->PreSettingFX( pTexEffVisualMaterial->GetVMFX_COLOR(), matIdentify, NULL, NULL, NULL, NULL, D3DXVECTOR2(0.f,0.f), 1.f );
                }
                pTexEff = pTexEff->m_pNext;
            }
        }
    }

    D3DXVECTOR3 vMax(-FLT_MAX,-FLT_MAX,-FLT_MAX);
    D3DXVECTOR3 vMin(-FLT_MAX,-FLT_MAX,-FLT_MAX);
    CLIPVOLUME sCV = DxViewPort::GetInstance().GetClipVolume();
	FACELIST_ITER iter = pMaterialEX->listFace.begin();
	for ( ; iter!=pMaterialEX->listFace.end(); ++iter )
	{
        if ( pTexEffVisualMaterial )
        {
            D3DXVECTOR3 vVector(0.f,0.f,0.f);
            vm::VERTEXNORCOLORTEXTANGENT_COMPRESS vVerticesArray[3];
            for ( DWORD i=0; i<3; ++i )
            {
                vVerticesArray[i].vPos = (*iter).m_sVertex[i].vPos;
                vVerticesArray[i].cColor = (*iter).m_sVertex[i].dwColor;
                vVerticesArray[i].vTex = (*iter).m_sVertex[i].vTex;

                D3DXVec3Normalize( &vVector, &(*iter).m_sVertex[i].vNor );	// ��ֶ���� ���ش�.
                vVerticesArray[i].vNor[0] = static_cast<short>( vVector.x * 32767.f );
                vVerticesArray[i].vNor[1] = static_cast<short>( vVector.y * 32767.f );
                vVerticesArray[i].vNor[2] = static_cast<short>( vVector.z * 32767.f );

                D3DXVec3Normalize( &vVector, &(*iter).m_vTangetArray[i] );	// ��ֶ���� ���ش�.
                vVerticesArray[i].vTan[0] = static_cast<short>( vVector.x * 32767.f );
                vVerticesArray[i].vTan[1] = static_cast<short>( vVector.y * 32767.f );
                vVerticesArray[i].vTan[2] = static_cast<short>( vVector.z * 32767.f );
            }

            pTexEffVisualMaterial->InsertColorTriangle( pd3dDevice, reinterpret_cast<BYTE*>( &vVerticesArray[0] ) );
        }
        else
        {
			NSVERTEXNORTEX_FX::SetWorld( matIdentify, 1.f );
            NSVERTEXNORTEX_FX::SetBaseTexture( pMaterialEX->m_textureRes.GetTexture() );
            NSVERTEXNORTEX_FX::Render( pd3dDevice, VERTEXNORCOLORTEX::FVF, 1, (*iter).m_sVertex, sizeof(VERTEXNORCOLORTEX) );
		    //pd3dDevice->DrawPrimitiveUP ( D3DPT_TRIANGLELIST, 1, (*iter).m_sVertex, sizeof(VERTEXNORCOLORTEX) );
        }
	}

    // VisualMaterial�� ���� �ѹ��� ������ �ϵ��� �Ѵ�.
    if ( pTexEffVisualMaterial )
        pTexEffVisualMaterial->RenderColorTriangle( pd3dDevice );
}

HRESULT DxEffectTiling::Render_Select ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bCenter )
{
	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity ( &matIdentity );
	pd3dDevice->SetTransform ( D3DTS_WORLD, &matIdentity );

	m_pSavedSB_Select->Capture();
	m_pDrawSB_Select->Apply();

	pd3dDevice->SetTexture ( 0, NULL );

	D3DMATERIALQ sMaterial;
	sMaterial.Diffuse.r = 1.f;
	sMaterial.Diffuse.g = 1.f;
	sMaterial.Diffuse.b = 1.f;
	sMaterial.Diffuse.a = 1.f;
	sMaterial.Ambient.r = 1.f;
	sMaterial.Ambient.g = 1.f;
	sMaterial.Ambient.b = 1.f;
	sMaterial.Ambient.a = 1.f;
	sMaterial.Emissive.r = 0.f;
	sMaterial.Emissive.g = 0.f;
	sMaterial.Emissive.b = 0.f;
	sMaterial.Emissive.a = 0.f;
	sMaterial.Specular.r = 0.f;
	sMaterial.Specular.g = 0.f;
	sMaterial.Specular.b = 0.f;
	sMaterial.Specular.a = 0.f;
	pd3dDevice->SetMaterial ( &sMaterial );

	D3DCOLOR	cColor_Src = 0x44000000;
	D3DCOLOR	cColor_Dest = 0x00000000;

	if ( bCenter )
	{
		cColor_Src = 0x4400ff00;
		cColor_Dest = 0x0000ff00;
	}
	else
	{
		cColor_Src = 0x44ff0000;
		cColor_Dest = 0x44ff0000;
	}

	int nCount = 0;
	DWORD dwVERTEX_NUM;
	DWORD dwFaces_NUM;

	dwFaces_NUM = (DWORD)m_listSelectTriangle.size();
	VERTEXNORCOLORTEX* pVertex = new VERTEXNORCOLORTEX[dwFaces_NUM*3];

	TRIANGLELIST_ITER iter = m_listSelectTriangle.begin();
	for ( ; iter != m_listSelectTriangle.end(); ++iter )
	{
		dwFaces_NUM = (*iter);

		dwVERTEX_NUM = dwFaces_NUM*3+0;
		pVertex[nCount].vPos = m_pPoint[dwVERTEX_NUM].vPos;
		pVertex[nCount].vNor = m_pPoint[dwVERTEX_NUM].vNor;
		pVertex[nCount].dwColor = cColor_Dest;
		pVertex[nCount].vTex = m_pPoint[dwVERTEX_NUM].vTex;
		if ( m_dwCurPOINT == m_pPoint[dwVERTEX_NUM].dwPointEx  )
		{
			pVertex[nCount].dwColor = cColor_Src;
		}
		++nCount;

		dwVERTEX_NUM = dwFaces_NUM*3+1;
		pVertex[nCount].vPos = m_pPoint[dwVERTEX_NUM].vPos;
		pVertex[nCount].vNor = m_pPoint[dwVERTEX_NUM].vNor;
		pVertex[nCount].dwColor = cColor_Dest;
		pVertex[nCount].vTex = m_pPoint[dwVERTEX_NUM].vTex;
		if ( m_dwCurPOINT == m_pPoint[dwVERTEX_NUM].dwPointEx  )
		{
			pVertex[nCount].dwColor = cColor_Src;
		}
		++nCount;

		dwVERTEX_NUM = dwFaces_NUM*3+2;
		pVertex[nCount].vPos = m_pPoint[dwVERTEX_NUM].vPos;
		pVertex[nCount].vNor = m_pPoint[dwVERTEX_NUM].vNor;
		pVertex[nCount].dwColor = cColor_Dest;
		pVertex[nCount].vTex = m_pPoint[dwVERTEX_NUM].vTex;
		if ( m_dwCurPOINT == m_pPoint[dwVERTEX_NUM].dwPointEx  )
		{
			pVertex[nCount].dwColor = cColor_Src;
		}
		++nCount;
	}

	dwFaces_NUM = (DWORD)m_listSelectTriangle.size();

	pd3dDevice->SetFVF ( VERTEXNORCOLORTEX::FVF );
	pd3dDevice->DrawPrimitiveUP ( D3DPT_TRIANGLELIST, dwFaces_NUM, pVertex, sizeof(VERTEXNORCOLORTEX) );

	m_pSavedSB_Select->Apply();

	SAFE_DELETE ( pVertex );

	return S_OK;
}

void DxEffectTiling::CloneDataLightMap ( DxEffectBase* pSrcEffect, 
                                        LPDIRECT3DDEVICEQ pd3dDevice, 
                                        DxTextureEffMan* pTexEff, 
                                        const TSTRING& strLightMapName, 
										VEC_LIGHTMAPUV_MAIN& vecLightMapUV_MAIN )
{
	DWORD dwTextureID(0);
	LIGHTMAPUV* pLightMapUV(NULL);
	for( DWORD i=0; i<vecLightMapUV_MAIN.size(); ++i )
	{
		SP_LIGHTMAPUV_MAIN spLightMapUV_MAIN = vecLightMapUV_MAIN[i];
		MAP_LIGHTMAPUV_CITER citer = spLightMapUV_MAIN->m_mapLightMapUV.find( pSrcEffect->GetAdaptFrameName() );
		if ( citer != spLightMapUV_MAIN->m_mapLightMapUV.end() )
		{
			// ���� �ִٸ�...
			pLightMapUV = (*citer).second.get();
			dwTextureID = i;
		}
	}

	if ( !pLightMapUV )
		return;

    // �����۾�
    float fLightMapDetail(1.f);
    {
        DxEffectTiling *pEffSrc = (DxEffectTiling*)pSrcEffect;

        DxFrame* pFrame = pEffSrc->GetAdaptFrame();
        if ( pFrame )
        {
            *m_pLightMapData = pFrame->GetLightMapData();
            fLightMapDetail = pFrame->GetLightMapDetail();
        }

        m_matFrameComb	= pEffSrc->m_matFrameComb;

        //std::for_each ( m_mapMaterials.begin(), m_mapMaterials.end(), std_afunc::DeleteMapObject() );
        m_mapMaterials.clear();

        MATERIALMAP_ITER iter		= pEffSrc->m_mapMaterials.begin();
        MATERIALMAP_ITER iter_end	= pEffSrc->m_mapMaterials.end();

        for ( ; iter!=pEffSrc->m_mapMaterials.end(); ++iter )
        {
	        std::tr1::shared_ptr<MATERIALEX> spMaterials( new MATERIALEX(pEffSrc->m_dwPointEX) );
	        spMaterials->strTexture = ((*iter).second)->strTexture;

            spMaterials->m_textureRes = TextureManager::GetInstance().LoadTexture(
                spMaterials->strTexture,
                false,
                TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
                0,
                D3DFMT_UNKNOWN,
                TEXTURE_RESIZE_NONE,
                TEXTURE_EFFECT,
				false,
				FALSE );

            if( spMaterials->m_textureRes.GetTexture() )
                spMaterials->m_bAlpha = (BOOL)spMaterials->m_textureRes.GetAlphaType();	// ���� ������ �Ⱦ����� üũ

			// // Ÿ���� �� �����Ѵ�.
			{
				pEffSrc->CreateTiles( pd3dDevice, pLightMapUV );
			}

			// 
			{
				FACELIST_ITER _iter = (*iter).second->listFace.begin();
				for ( ; _iter!=(*iter).second->listFace.end(); ++_iter )
				{
					if ( ((*_iter).m_sVertex[0].dwColor>>24) || ((*_iter).m_sVertex[1].dwColor>>24) || ((*_iter).m_sVertex[2].dwColor>>24) )
					{
						spMaterials->listFace.push_back ( (*_iter) );
					}
				}
			}

	        m_mapMaterials.insert ( std::make_pair((*iter).first,spMaterials) );		// ����
        }
    }

    // LightMap �� ���� �ƴ� ��� ...
    if ( !strLightMapName.empty() )
    {
        // ���� ���
        {
            ExportProgress::SetStateString( "LightMap ������ - DxEffectTiling" );
        }

        MATERIALMAP_ITER iter = m_mapMaterials.begin();
        for ( ; iter!=m_mapMaterials.end(); ++iter )
        {
	        CreateAABBOctreeLM( pd3dDevice, (*iter).second.get(), pTexEff, strLightMapName, pLightMapUV, dwTextureID );
        }
    }
    else
    {
	    MATERIALMAP_ITER iter = m_mapMaterials.begin();
	    for ( ; iter!=m_mapMaterials.end(); ++iter )
	    {
		    CreateAABBOctree ( pd3dDevice, (*iter).second.get(), pTexEff );
	    }
    }
}

void DxEffectTiling::CreateAABBOctree( LPDIRECT3DDEVICEQ pd3dDevice, MATERIALEX* pMaterialEX, DxTextureEffMan* pTexEff )
{
	//DWORD dwSize = (DWORD)pMaterialEX->listFace.size();
	//if ( dwSize )
	//{
	//	VERTEXNORCOLORTEX* pVert = new VERTEXNORCOLORTEX[dwSize*3];

	//	int nCount = 0;
	//	FACELIST_ITER iter = pMaterialEX->listFace.begin();
	//	for ( ; iter!=pMaterialEX->listFace.end(); ++iter )
	//	{
	//		pVert[nCount*3+0] = (*iter).m_sVertex[0];
	//		pVert[nCount*3+1] = (*iter).m_sVertex[1];
	//		pVert[nCount*3+2] = (*iter).m_sVertex[2];

 //           ++nCount;
	//	}

	//	SAFE_DELETE ( pMaterialEX->m_pMeshList );
	//	pMaterialEX->m_pMeshList = new DxSingleTexMesh;
	//	pMaterialEX->m_pMeshList->Create( pd3dDevice, pMaterialEX->strTexture.c_str(), pMaterialEX->strTexture.c_str(), dwSize, (BYTE*)pVert, VERTEXNORCOLORTEX::FVF, FALSE, 0L, 0.f, FALSE );
 //       pMaterialEX->m_pMeshList->ConvertTexEffMesh( pd3dDevice, pTexEff );

	//	SAFE_DELETE_ARRAY( pVert );
	//}
}

//void DxEffectTiling::CreateAABBOctreeLM_1( LPDIRECT3DDEVICEQ pd3dDevice, 
//										  MATERIALEX* pMaterialEX,
//										  DxStaticMesh* pStaticMesh, 
//										  DxPieceManager* pPieceManager, 
//										  LIGHTMAPUV* pLightMapUV, 
//										  DWORD dwTextureID )
//{
//	DWORD dwSize = (DWORD)pMaterialEX->listFace.size();
//	if ( dwSize )
//	{
//		VERTEXNORCOLORTEX2* pVert = new VERTEXNORCOLORTEX2[dwSize*3];
//
//		int nCount = 0;
//		FACELIST_ITER iter = pMaterialEX->listFace.begin();
//		for ( ; iter!=pMaterialEX->listFace.end(); ++iter )
//		{
//			pVert[nCount*3+0] = (*iter).m_sVertex[0];
//			pVert[nCount*3+1] = (*iter).m_sVertex[1];
//			pVert[nCount*3+2] = (*iter).m_sVertex[2];
//
//            ++nCount;
//		}
//
//        // LightMap ������ ������ Color �� ����� 
//        {
//            D3DXMATRIX matIdentity;
//            D3DXMatrixIdentity( &matIdentity );
//
//            NSLightMap::CreateLightMap( pd3dDevice,
//                                        pStaticMesh->GetLightMapName(), 
//                                        _T("_T"), 
//                                        TRUE,
//                                        FALSE,
//                                        pMaterialEX->m_vecLightMapObject,
//                                        dwSize, 
//                                        reinterpret_cast<BYTE*>(pVert),
//                                        VERTEXNORCOLORTEX2::FVF,
//                                        pStaticMesh,
//                                        pPieceManager,
//                                        matIdentity,
//                                        dwStartTextureSize,
//                                        pStaticMesh->GetLightMapDetail() * fLightMapDetail,
//                                        m_emLightMapType,
//                                        FALSE );
//        }
//
//		SAFE_DELETE_ARRAY( pVert );
//	}
//}

void DxEffectTiling::CreateAABBOctreeLM( LPDIRECT3DDEVICEQ pd3dDevice, 
										MATERIALEX* pMaterialEX,
										DxTextureEffMan* pTexEff, 
										const TSTRING& strLightMapName, 
										LIGHTMAPUV* pLightMapUV, 
										DWORD dwTextureID )
{
	//int nTexCount = dwTextureID;
	//int n100 = nTexCount/100;
	//int n10 = (nTexCount-n100*100) / 10;
	//int n1 = nTexCount%10;
	//TCHAR c100 = (TCHAR)(n100+48);
	//TCHAR c10 = (TCHAR)(n10+48);
	//TCHAR c1 = (TCHAR)(n1+48);
	//TSTRING strTextureDay = _T("");
	//TSTRING strTextureNight = _T("");
	//TSTRING strTextureAvgDay = _T("");
	//TSTRING strTextureAvgNight = _T("");
	//strTextureDay = strTextureDay + strLightMapName + _T("_S") + _T("_D") + c100 + c10 + c1 + _T(".dds");
	//strTextureNight = strTextureNight + strLightMapName + _T("_S") + _T("_N") + c100 + c10 + c1 + _T(".dds");
	//strTextureAvgDay = strTextureAvgDay + strLightMapName + _T("_S") + _T("_AD") + c100 + c10 + c1 + _T(".dds");
	//strTextureAvgNight = strTextureAvgNight + strLightMapName + _T("_S") + _T("_AN") + c100 + c10 + c1 + _T(".dds");

 //   DWORD dwSize = (DWORD)pMaterialEX->listFace.size();
	//if ( dwSize )
	//{
	//	VERTEXNORCOLORTEX2* pVert = new VERTEXNORCOLORTEX2[dwSize*3];

	//	int nCount = 0;
	//	FACELIST_ITER iter = pMaterialEX->listFace.begin();
	//	for ( ; iter!=pMaterialEX->listFace.end(); ++iter, ++nCount )
	//	{
 //           for ( DWORD i=0; i<3; ++i )
 //           {
	//		    pVert[nCount*3+i].vPos = (*iter).m_sVertex[i].vPos;
 //               pVert[nCount*3+i].vNor = (*iter).m_sVertex[i].vNor;
 //               pVert[nCount*3+i].dwColor = (*iter).m_sVertex[i].dwColor;
 //               pVert[nCount*3+i].vTex[0] = (*iter).m_sVertex[i].vTex;
 //               pVert[nCount*3+i].vTex[1] = (*iter).m_avLightMapUV[i];
 //           }
	//	}

 //       // DxSingleTexMesh �� �����.
 //       {
 //           SAFE_DELETE ( pMaterialEX->m_pMeshList );
 //           pMaterialEX->m_pMeshList = new DxSingleTexMesh;

 //           // DxSingleTexMesh - LightMap ������ �۾��ϸ� �� �� ����.
 //           {   
 //               pMaterialEX->m_pMeshList->CreateLightMap
 //               ( 
 //                   pd3dDevice, 
 //                   pMaterialEX->strTexture.c_str(), 
	//				pMaterialEX->strTexture.c_str(), 
 //                   dwSize, 
 //                   reinterpret_cast<BYTE*>( pVert ), // �۾�����.
 //                   FALSE,
	//				0.f,
 //                   VERTEXNORCOLORTEX2::FVF,
 //                   strTextureDay,
 //                   strTextureNight,
 //                   strTextureAvgDay,
 //                   strTextureAvgNight,
	//				FALSE,
	//				TRUE
 //               );
 //               pMaterialEX->m_pMeshList->ConvertTexEffMesh( pd3dDevice, pTexEff );

	//			pMaterialEX->m_pMeshList->LoadLightMap( pd3dDevice );
 //           }
 //       }


	//	SAFE_DELETE_ARRAY( pVert );
	//}
}
//
//void DxEffectTiling::CreateAABBOctreeLM_2( LPDIRECT3DDEVICEQ pd3dDevice, MATERIALEX* pMaterialEX, DxTextureEffMan* pTexEff, const TSTRING& strLightMapName )
//{
//    //TSTRING strTextureDay(_T(""));
//    //TSTRING strTextureNight(_T(""));
//    //TSTRING strTextureAvgDay(_T(""));
//    //TSTRING strTextureAvgNight(_T(""));
//
//    //// ������ �����۾�.
//    //NSLightMap::CreateLightMap2( pMaterialEX->m_vecLightMapObject, 
//    //                            strLightMapName, 
//    //                            _T("_T"), 
//    //                            TRUE,
//    //                            FALSE,
//    //                            strTextureDay, 
//    //                            strTextureNight, 
//    //                            strTextureAvgDay, 
//    //                            strTextureAvgNight,
//    //                            WORDWORD(0,0),
//    //                            TRUE,
//				//				TRUE );
//
//    //DWORD dwSize = (DWORD)pMaterialEX->listFace.size();
//
//    //// Vertex ������
//    //DWORD dwFaceStartNum(0);
//    //VERTEXNORCOLORTEX2* pVertices = new VERTEXNORCOLORTEX2[dwSize*3];
//    //for ( DWORD j=0; j<pMaterialEX->m_vecLightMapObject.size(); ++j )
//    //{
//    //    pMaterialEX->m_vecLightMapObject[j]->GetVerticesData( pVertices, dwFaceStartNum, dwFaceStartNum );
//    //}
//
//    //// DxSingleTexMesh �� �����.
//    //{
//    //    SAFE_DELETE ( pMaterialEX->m_pMeshList );
//    //    pMaterialEX->m_pMeshList = new DxSingleTexMesh;
//
//    //    // DxSingleTexMesh - LightMap ������ �۾��ϸ� �� �� ����.
//    //    {   
//    //        pMaterialEX->m_pMeshList->CreateLightMap
//    //        ( 
//    //            pd3dDevice, 
//    //            pMaterialEX->strTexture.c_str(), 
//    //            dwSize, 
//    //            reinterpret_cast<BYTE*>( pVertices ), // �۾�����.
//    //            FALSE,
//    //            VERTEXNORCOLORTEX2::FVF,
//    //            strTextureDay,
//    //            strTextureNight,
//    //            strTextureAvgDay,
//    //            strTextureAvgNight,
//				//FALSE
//    //        );
//    //        pMaterialEX->m_pMeshList->ConvertTexEffMesh( pd3dDevice, pTexEff );
//    //    }
//    //}
//
//    //SAFE_DELETE_ARRAY( pVertices );
//}

//void DxEffectTiling::CreateLightMap2( LPDIRECT3DDEVICEQ pd3dDevice, DxTextureEffMan* pTexEff, BOOL bCreatLightMap, const TSTRING& strLightMapName )
//{
//    // LightMap �� ��츸 �۾��Ѵ�.
//    if ( bCreatLightMap )
//    {
//        MATERIALMAP_ITER iter = m_mapMaterials.begin();
//        for ( ; iter!=m_mapMaterials.end(); ++iter )
//        {
//            //if ( m_emLightMapType == NSLIGHTMAPDEF::LMT_GROUND )
//            //{
//	            CreateAABBOctreeLM_2_Ground( pd3dDevice, (*iter).second.get(), pTexEff );
//            //}
//            //else
//            //{
//            //    CreateAABBOctreeLM_2( pd3dDevice, (*iter).second.get(), pTexEff, strLightMapName );
//            //}
//        }
//    }
//}
//
//void DxEffectTiling::LoadLightMap( LPDIRECT3DDEVICEQ pd3dDevice )
//{
//    // �ؽ��� �ε�
//    MATERIALMAP_ITER iter = m_mapMaterials.begin();
//    for ( ; iter!=m_mapMaterials.end(); ++iter )
//    {
//        if ( (*iter).second->m_pMeshList )
//        {
//            (*iter).second->m_pMeshList->LoadLightMap( pd3dDevice );
//        }
//    }
//}

void DxEffectTiling::SetNSLandThread()
{
	MATERIALMAP_ITER iter = m_mapMaterials.begin();
	for ( ; iter!=m_mapMaterials.end(); ++iter )
	{
		if ( (*iter).second->m_pMeshList )
		{
			(*iter).second->m_pMeshList->CreateDxCullingMeshSwap();
		}
	}
}