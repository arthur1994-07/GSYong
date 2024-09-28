#include "pch.h"
//#include <algorithm>

//#include "../../CoreCommon/String/SerialFile.h"

//#include "../../LogicCommonLib/RANPARAM.h"

//#include "../../EngineBaseLib/Common/StlFunctions.h"
//#include "../../EngineBaseLib/Define/DxVertexFVF.h"

#include "../PostProcessing/NSLightShafts.h"
#include "../../DxMeshs/DxSimpleMeshMan.h"
#include "../DxVertexFVF.h"
#include "../DxViewPort.h"
//#include "../GlobalParam.h"

#include "DxSkyBoxMan.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


//----------------------------------------------------------------------------------------
//					A	G		S	k	y		B	o	x		M	a	n
//----------------------------------------------------------------------------------------
//	Note : 싱글톤 클래스 제어.
DxSkyBoxMan& DxSkyBoxMan::GetInstance()
{
	static DxSkyBoxMan Instance;
	return Instance;
}

DWORD DxSkyBoxMan::VERSION = 0x0107;

DxSkyBoxMan::DxSkyBoxMan()
	: m_fAlpha(1.f)
	, m_fLightShaftsWeight(0.5f)
	, m_fLightShaftsDecay(0.5f)
	, m_fLightShaftsLast(1.f)
	, m_vLightShaftsDir(-1.0f, -2.0f, 2.0f)
{
	m_pvecSKYBOX = new VEC_SKYBOX;
}

DxSkyBoxMan::~DxSkyBoxMan()
{
	CleanUp();

	SAFE_DELETE( m_pvecSKYBOX );
}

void DxSkyBoxMan::CleanUp()
{
	m_fLightShaftsLast = 1.f;

	std::for_each( m_pvecSKYBOX->begin(), m_pvecSKYBOX->end(), std_afunc::DeleteObject() );
	m_pvecSKYBOX->clear();
}

void DxSkyBoxMan::FrameMove( float fElapsedTime )
{
	if( m_pvecSKYBOX->empty() )
		return;

	//if( !GLOBAL_PARAM::IsValue( GP_SKYBOX ) )
	//	return;

	VEC_SKYBOX_ITER iter = m_pvecSKYBOX->begin();
	for( ; iter!=m_pvecSKYBOX->end(); ++iter )
	{
		(*iter)->FrameMove( fElapsedTime );
	}
}

void DxSkyBoxMan::Render_Local( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX* pmatReflect )
{
	pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,	D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,	D3DBLEND_INVSRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );

	Render( pd3dDevice, pmatReflect );
}

void DxSkyBoxMan::Render( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX* pmatReflect )
{
	if( m_pvecSKYBOX->empty() )
		return;

	//if( !GLOBAL_PARAM::IsValue( GP_SKYBOX ) )
	//	return;

	if( m_fAlpha <= 0.f )
		return;

	DWORD	dwFogEnable;
	pd3dDevice->GetRenderState( D3DRS_FOGENABLE, &dwFogEnable );

	//	Note : 뷰포트
	DxViewPort::GetInstance().BackUpFarPlane();
	DxViewPort::GetInstance().SetProjection_FarPlane( 25000.f );	// 최대 시아를 제한한다.

	DWORD dwAlphaOP(0);
	DWORD dwColorARG2(0);
	DWORD dwAlphaBlendEnable(0);
	pd3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAOP, &dwAlphaOP );
	pd3dDevice->GetTextureStageState( 0, D3DTSS_COLORARG2, &dwColorARG2 );
	pd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE, &dwAlphaBlendEnable );

	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );

	//DWORD dwAlpha = (DWORD) (m_fAlpha*255.f);
	//DWORD dwColor = (dwAlpha<<24) + 0xffffff;
	//pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, dwColor );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );

	pd3dDevice->SetRenderState( D3DRS_ZENABLE ,		TRUE );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	FALSE );

	const D3DXVECTOR3& vPos = DxViewPort::GetInstance().GetLookatPt();
	VEC_SKYBOX_ITER iter = m_pvecSKYBOX->begin();
	for( ; iter!=m_pvecSKYBOX->end(); ++iter )
	{
		(*iter)->Render( pd3dDevice, vPos, pmatReflect, m_fAlpha );
	}

	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, dwAlphaOP );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, dwColorARG2 );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, dwAlphaBlendEnable );

	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, dwFogEnable );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE , TRUE );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	TRUE );

	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );

	DxViewPort::GetInstance().RollBackFarPlane( pd3dDevice );
}

void DxSkyBoxMan::Render_DOF( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( m_pvecSKYBOX->empty() )
		return;

	//if( !GLOBAL_PARAM::IsValue( GP_SKYBOX ) )
	//	return;

	if( m_fAlpha <= 0.f )
		return;

	//	Note : 뷰포트
	DxViewPort::GetInstance().BackUpFarPlane();
	DxViewPort::GetInstance().SetProjection_FarPlane( 25000.f );	// 최대 시아를 제한한다.

	DWORD	dwFogEnable;
	pd3dDevice->GetRenderState( D3DRS_FOGENABLE, &dwFogEnable );

	DWORD	dwZWriteEnable;
	pd3dDevice->GetRenderState( D3DRS_ZWRITEENABLE,	&dwZWriteEnable );

	DWORD dwAlphaOP(0);
	DWORD dwColorARG1, dwColorOP;
	pd3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAOP,	&dwAlphaOP );
	pd3dDevice->GetTextureStageState( 0, D3DTSS_COLORARG1,	&dwColorARG1 );
	pd3dDevice->GetTextureStageState( 0, D3DTSS_COLOROP,	&dwColorOP );

	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );

	DWORD dwColor = 0xffffffff;
	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, dwColor );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	FALSE );

	const D3DXVECTOR3& vPos = DxViewPort::GetInstance().GetLookatPt();
	VEC_SKYBOX_ITER iter = m_pvecSKYBOX->begin();
	for( ; iter!=m_pvecSKYBOX->end(); ++iter )
	{
		(*iter)->Render_DOF( pd3dDevice, vPos );
	}

	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	dwAlphaOP );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,	dwColorARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	dwColorOP );

	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, dwFogEnable );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	dwZWriteEnable );

	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );

	DxViewPort::GetInstance().RollBackFarPlane( pd3dDevice );
}

BOOL DxSkyBoxMan::Render_LightShafts( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( m_pvecSKYBOX->empty() )
		return FALSE;

	//if( !GLOBAL_PARAM::IsValue( GP_SKYBOX ) )
	//	return FALSE;

	if( m_fAlpha <= 0.f )
		return FALSE;

	// Shafts 수치 셋팅
    BOOL bActiveLightShafts(FALSE);
	VEC_SKYBOX_ITER iter = m_pvecSKYBOX->begin();
	for( ; iter!=m_pvecSKYBOX->end(); ++iter )
	{
		if( (*iter)->IsHaveLightShafts() )
		{
            bActiveLightShafts = TRUE;
			NSLightShafts::OnSetting( pd3dDevice, m_fAlpha, m_fLightShaftsWeight, m_fLightShaftsDecay, m_fLightShaftsLast, m_vLightShaftsDir );
			break;
		}
	}

    // LightShafts가 없다면 FALSE를 리턴
    if( !bActiveLightShafts )
        return FALSE;

	//	Note : 뷰포트
	DxViewPort::GetInstance().BackUpFarPlane();
	DxViewPort::GetInstance().SetProjection_FarPlane( 25000.f );	// 최대 시아를 제한한다.

	const D3DXVECTOR3& vPos = DxViewPort::GetInstance().GetLookatPt();
	iter = m_pvecSKYBOX->begin();
	for( ; iter!=m_pvecSKYBOX->end(); ++iter )
	{
		(*iter)->Render_LightShafts( pd3dDevice, vPos );
	}

	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );

	DxViewPort::GetInstance().RollBackFarPlane( pd3dDevice );

    return TRUE;
}

void DxSkyBoxMan::Render_LightShafts_Local( LPDIRECT3DDEVICEQ pd3dDevice )
{
    // Light Shafts가 안 될 경우는, 강도를 조금 줄여준다.
    if( !Render_LightShafts( pd3dDevice ) )
    {
        NSLightShafts::SetLowLevel( m_fAlpha );
    }
}

void DxSkyBoxMan::InsertSkyBox()
{
	DxSkyBox* pNew = new DxSkyBox;

	m_pvecSKYBOX->push_back( pNew );
}

void DxSkyBoxMan::DeleteSkyBox( int nIndex )
{
	VEC_SKYBOX_ITER iter = m_pvecSKYBOX->begin();
	for( ; iter!=m_pvecSKYBOX->end(); ++iter )
	{
		// 여긴 왜 delete 안하지? 081016
		//SAFE_DELETE( *iter );

		m_pvecSKYBOX->erase( iter );
		return;
	}
}

DxSkyBox* DxSkyBoxMan::GetSkyBox( int nIndex )
{
	if( nIndex >= (int)m_pvecSKYBOX->size() )	return NULL;

	return (*m_pvecSKYBOX)[nIndex];
}

void DxSkyBoxMan::Reload( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for( DWORD i=0; i<m_pvecSKYBOX->size(); ++i )
	{
		Reload( pd3dDevice, i );
	}
}

void DxSkyBoxMan::Reload( LPDIRECT3DDEVICEQ pd3dDevice, int nIndex )
{
	if( nIndex >= (int)m_pvecSKYBOX->size() )	return;

	(*m_pvecSKYBOX)[nIndex]->OnCreateDevice( pd3dDevice, TRUE );
}

//BOOL DxSkyBoxMan::IsCollision( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd ) const
//{
//	VEC_SKYBOX_ITER iter = m_pvecSKYBOX->begin();
//	for( ; iter!=m_pvecSKYBOX->end(); ++iter )
//	{
//		(*iter)->IsCollision( vStart, vEnd );
//		return TRUE;
//	}
//	return FALSE;
//}

void DxSkyBoxMan::SetAlpha( float fAlpha )
{
	m_fAlpha = fAlpha;
}

VOID DxSkyBoxMan::CreateClone( LPDIRECT3DDEVICEQ pd3dDevice, VEC_SKYBOX& refSkyBox )
{
	std::for_each( refSkyBox.begin(), refSkyBox.end(), std_afunc::DeleteObject() );
	refSkyBox.clear();

	VEC_SKYBOX_ITER iter = m_pvecSKYBOX->begin();
	for( ; iter != m_pvecSKYBOX->end(); ++iter )
	{
		DxSkyBox* pClone = ( *iter )->CreateClone( pd3dDevice );
		if( pClone )
			refSkyBox.push_back( pClone );
	}
}

VOID DxSkyBoxMan::RenderClone( LPDIRECT3DDEVICEQ pd3dDevice, VEC_SKYBOX& refSkyBox, float fElapsedTime )
{
	if( refSkyBox.empty() )
		return;

	//if( !GLOBAL_PARAM::IsValue( GP_SKYBOX ) )
	//	return;

	DWORD	dwForEnable;
	pd3dDevice->GetRenderState( D3DRS_FOGENABLE, &dwForEnable );

	//	Note : 뷰포트
	DxViewPort::GetInstance().BackUpFarPlane();
	DxViewPort::GetInstance().SetProjection_FarPlane( 25000.f );	// 최대 시아를 제한한다.

	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );

	DWORD dwColor = 0xffffffff;
	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, dwColor );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );

	pd3dDevice->SetRenderState( D3DRS_ZENABLE ,		FALSE );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	FALSE );

	const D3DXVECTOR3& vPos = DxViewPort::GetInstance().GetLookatPt();
	VEC_SKYBOX_ITER iter = refSkyBox.begin();
	for( ; iter!=refSkyBox.end(); ++iter )
	{
		(*iter)->FrameMove( fElapsedTime );
		(*iter)->Render( pd3dDevice, vPos, NULL, 1.f );
	}

	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, dwForEnable );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE , TRUE );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	TRUE );

	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );

	DxViewPort::GetInstance().RollBackFarPlane( pd3dDevice );
}

VOID DxSkyBoxMan::DeleteClone( VEC_SKYBOX& refSkyBox )
{
	std::for_each( refSkyBox.begin(), refSkyBox.end(), std_afunc::DeleteObject() );
	refSkyBox.clear();
}

void DxSkyBoxMan::GetOnlyData( VEC_SKYBOX& refSkyBox )
{
	std::for_each( refSkyBox.begin(), refSkyBox.end(), std_afunc::DeleteObject() );
	refSkyBox.clear();

	VEC_SKYBOX_ITER iter = m_pvecSKYBOX->begin();
	for( ; iter != m_pvecSKYBOX->end(); ++iter )
	{
		DxSkyBox* pNew = new DxSkyBox;
		pNew->GetOnlyData( (*iter) );

		refSkyBox.push_back( pNew );
	}
}

void DxSkyBoxMan::Save( sc::SerialFile &SFile )
{
	SFile << (DWORD)VERSION;

	SFile.BeginBlock();
	{
		SFile << m_fLightShaftsWeight;
		SFile << m_fLightShaftsDecay;
		SFile << m_fLightShaftsLast;
		SFile << m_vLightShaftsDir;

		SFile << (DWORD)m_pvecSKYBOX->size();

		VEC_SKYBOX_ITER iter = m_pvecSKYBOX->begin();
		for( ; iter!=m_pvecSKYBOX->end(); ++iter )
		{
			(*iter)->Save( SFile );
		}
	}
	SFile.EndBlock();
}

void DxSkyBoxMan::Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile &SFile, BOOL bTexThread )
{
	CleanUp();

	DWORD dwVer, dwBufferSize;

	SFile >> dwVer;
	SFile >> dwBufferSize;

	if( dwVer == VERSION )
	{
		SFile >> m_fLightShaftsWeight;
		SFile >> m_fLightShaftsDecay;
		SFile >> m_fLightShaftsLast;
		SFile >> m_vLightShaftsDir;

		DWORD dwSize(0);
		SFile >> dwSize;
		for( DWORD i=0; i<dwSize; ++i )
		{
			DxSkyBox* pNew = new DxSkyBox;
			pNew->Load( pd3dDevice, SFile, bTexThread );

			m_pvecSKYBOX->push_back( pNew );
		}
	}
	else if( dwVer == 0x0106 )
	{
		SFile >> m_fLightShaftsWeight;
		SFile >> m_fLightShaftsDecay;
		SFile >> m_fLightShaftsLast;

		DWORD dwSize(0);
		SFile >> dwSize;
		for( DWORD i=0; i<dwSize; ++i )
		{
			DxSkyBox* pNew = new DxSkyBox;
			pNew->Load( pd3dDevice, SFile, bTexThread );

			m_pvecSKYBOX->push_back( pNew );
		}
	}
	else if( dwVer == 0x0105 )
	{
		SFile >> m_fLightShaftsWeight;
		SFile >> m_fLightShaftsDecay;

		DWORD dwSize(0);
		SFile >> dwSize;
		for( DWORD i=0; i<dwSize; ++i )
		{
			DxSkyBox* pNew = new DxSkyBox;
			pNew->Load( pd3dDevice, SFile, bTexThread );

			m_pvecSKYBOX->push_back( pNew );
		}
	}
	else if( dwVer == 0x0104 )
	{
		DWORD dwSize(0);
		SFile >> dwSize;
		for( DWORD i=0; i<dwSize; ++i )
		{
			DxSkyBox* pNew = new DxSkyBox;
			pNew->Load( pd3dDevice, SFile, bTexThread );

			m_pvecSKYBOX->push_back( pNew );
		}
	}
	else if( dwVer == 0x0103 )
	{
		DWORD dwSize(0);
		SFile >> dwSize;
		for( DWORD i=0; i<dwSize; ++i )
		{
			DxSkyBox* pNew = new DxSkyBox;
			pNew->Load_Ver103( pd3dDevice, SFile, bTexThread );

			m_pvecSKYBOX->push_back( pNew );
		}
	}
	else if( dwVer == 0x0102 )
	{
		DWORD dwSize(0);
		SFile >> dwSize;
		for( DWORD i=0; i<dwSize; ++i )
		{
			DxSkyBox* pNew = new DxSkyBox;
			pNew->Load_Ver102( pd3dDevice, SFile, bTexThread );

			m_pvecSKYBOX->push_back( pNew );
		}
	}
	else if( dwVer == 0x0101 )
	{
		DWORD dwSize(0);
		SFile >> dwSize;
		for( DWORD i=0; i<dwSize; ++i )
		{
			DxSkyBox* pNew = new DxSkyBox;
			pNew->Load_Ver101( pd3dDevice, SFile, bTexThread );

			m_pvecSKYBOX->push_back( pNew );
		}
	}
	else if( dwVer == 0x0100 )
	{
		DWORD dwSize(0);
		SFile >> dwSize;
		for( DWORD i=0; i<dwSize; ++i )
		{
			DxSkyBox* pNew = new DxSkyBox;
			pNew->Load_Ver100( pd3dDevice, SFile, bTexThread );

			m_pvecSKYBOX->push_back( pNew );
		}
	}
	else
	{
		SFile.SetOffSet( SFile.GetfTell() + dwBufferSize );
	}
}

void DxSkyBoxMan::Clone( LPDIRECT3DDEVICEQ pd3dDevice, const DxSkyBoxMan* pSrc )
{
	CleanUp();
		
	DWORD dwSize = (DWORD) pSrc->m_pvecSKYBOX->size();
	for( DWORD i=0; i<dwSize; ++i )
	{
		DxSkyBox* pNew = new DxSkyBox;
		pNew->Clone( pd3dDevice, (*pSrc->m_pvecSKYBOX)[i], TRUE );

		m_pvecSKYBOX->push_back( pNew );
	}
}

void DxSkyBoxMan::SetSkyBoxRotateY( const VEC_SKYBOX* pSrc )
{
	if( m_pvecSKYBOX->size() == pSrc->size() )
	{
		VEC_SKYBOX_ITER iter = m_pvecSKYBOX->begin();
		VEC_SKYBOX_CITER iter_SRC = pSrc->begin();
		for( ; iter!=m_pvecSKYBOX->end(); ++iter, ++iter_SRC )
		{
			(*iter)->SetSkyBoxRotateY( (*iter_SRC ) );
		}
	}
}

void DxSkyBoxMan::ItemUp( DxSkyBoxMan* pSkyBoxMan, int nIndex )
{
	if( nIndex == 0 )
		return;

	if( nIndex > (int)pSkyBoxMan->m_pvecSKYBOX->size() )
		return;

	DxSkyBox* pSkyBoxA(NULL);
	DxSkyBox* pSkyBoxB(NULL);

	pSkyBoxA = (*pSkyBoxMan->m_pvecSKYBOX)[nIndex-1];
	pSkyBoxB = (*pSkyBoxMan->m_pvecSKYBOX)[nIndex];
	(*pSkyBoxMan->m_pvecSKYBOX)[nIndex-1] = pSkyBoxB;
	(*pSkyBoxMan->m_pvecSKYBOX)[nIndex] = pSkyBoxA;
}

void DxSkyBoxMan::ItemDown( DxSkyBoxMan* pSkyBoxMan, int nIndex )
{
	if( nIndex > (int)pSkyBoxMan->m_pvecSKYBOX->size()-1 )
		return;

	DxSkyBox* pSkyBoxA(NULL);
	DxSkyBox* pSkyBoxB(NULL);

	pSkyBoxA = (*pSkyBoxMan->m_pvecSKYBOX)[nIndex];
	pSkyBoxB = (*pSkyBoxMan->m_pvecSKYBOX)[nIndex+1];
	(*pSkyBoxMan->m_pvecSKYBOX)[nIndex] = pSkyBoxB;
	(*pSkyBoxMan->m_pvecSKYBOX)[nIndex+1] = pSkyBoxA;
}

void DxSkyBoxMan::SetObjRotate90( float fRotate )
{
    DWORD dwSize = (DWORD) m_pvecSKYBOX->size();
	for( DWORD i=0; i<dwSize; ++i )
	{
		(*m_pvecSKYBOX)[i]->SetObjRotate90( fRotate );
	}
}

// SceneManager에서만 쓰임.
void DxSkyBoxMan::GetSkyBoxMan_SCENE( VEC_SKYBOX** ppvecSkyBox )
{
	(*ppvecSkyBox) = m_pvecSKYBOX;
	m_pvecSKYBOX = NULL;
	m_pvecSKYBOX = new VEC_SKYBOX;
}

// SceneManager에서만 쓰임.
void DxSkyBoxMan::SetSkyBoxMan_SCENE( VEC_SKYBOX* pvecSkyBox )
{
	CleanUp();
	SAFE_DELETE( m_pvecSKYBOX );
	
	// RollBack
	m_pvecSKYBOX = pvecSkyBox;
}
