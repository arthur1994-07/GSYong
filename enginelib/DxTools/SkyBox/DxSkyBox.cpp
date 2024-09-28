#include "pch.h"

#include "../../DxMeshs/DxSimpleMeshMan.h"
#include "../../DxTools/DxVertexFVF.h"
#include "../../DxTools/DxViewPort.h"
//#include "../GlobalParam.h"

#include "./DxSkyBox.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


//----------------------------------------------------------------------------------------
//							A	G		S	k	y		B	o	x
//----------------------------------------------------------------------------------------
DxSkyBox::DxSkyBox()
	: m_fRotationY(0.f)
	, m_fRotationBaseY(0.f)
	, m_fRotationSPEED(0.f)
	, m_fScale(1.f)		// 1.5f - 1스테이지 이다.
	, m_fHeight(0.f)
	, m_emBlend(BLEND_NONE)
	, m_fPower(1.0f)
{
}

DxSkyBox::~DxSkyBox()
{
}

void DxSkyBox::OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bTexThread )
{
	m_fRotationY = m_fRotationBaseY;	// 초기화
	m_SimMeshRes = DxSimpleMeshMan::GetInstance().Load(
		m_strName.c_str(),
		TRUE,
        bTexThread,
        TEXTURE_RESIZE_512,
        TEXTURE_SKYBOX );
}

void DxSkyBox::OnDestroyDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
}

void DxSkyBox::FrameMove( float fElapsedTime )
{
	m_fRotationY += m_fRotationSPEED*fElapsedTime;
}

void DxSkyBox::Render( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vPos, D3DXMATRIX* pmatReflect, float fAlpha )
{
	if( !m_SimMeshRes.Update() )
		return;

	if( m_emBlend == BLEND_LIGHT_SHAFTS )
		return;

	D3DXMATRIX matScale, matRotationY;
	D3DXMatrixScaling( &matScale, m_fScale, m_fScale, m_fScale );
	D3DXMatrixRotationY( &matRotationY, m_fRotationY*D3DX_PI/180.f );
	D3DXMatrixMultiply( &matScale, &matScale, &matRotationY );
	matScale._41 = vPos.x;
	matScale._42 = vPos.y + m_fHeight;
	matScale._43 = vPos.z;

	// Note : 반사맵을 뿌려야 할 경우.
	if( pmatReflect )
	{
		D3DXMatrixMultiply( &matScale, &matScale, pmatReflect );
	}

	pd3dDevice->SetTransform( D3DTS_WORLD, &matScale );

	DWORD dwColor = ((static_cast<DWORD>(m_fPower*fAlpha*255.f))<<24) + 0xffffff;
	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	dwColor );

	if( m_emBlend & BLEND_ALPHA )
	{
		DWORD dwAlphaBlendEnable(FALSE);
		DWORD dwAlphaARG1(0);
		DWORD dwAlphaARG2(0);
		pd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE,		&dwAlphaBlendEnable );
		pd3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAARG1,	&dwAlphaARG1 );
		pd3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAARG2,	&dwAlphaARG2 );

		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_INVSRCALPHA );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );

		m_SimMeshRes.get()->Render( pd3dDevice );

		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,		dwAlphaBlendEnable );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1,	dwAlphaARG1 );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2,	dwAlphaARG2 );
	}
	else if( m_emBlend & BLEND_ADD )
	{
		DWORD dwAlphaBlendEnable(FALSE);
		DWORD dwAlphaARG1(0);
		DWORD dwAlphaARG2(0);
		pd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE,		&dwAlphaBlendEnable );
		pd3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAARG1, &dwAlphaARG1 );
		pd3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAARG2, &dwAlphaARG2 );

		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_ONE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );

		m_SimMeshRes.get()->Render( pd3dDevice );

		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,		dwAlphaBlendEnable );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, dwAlphaARG1 );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, dwAlphaARG2 );
	}
	else
	{
		m_SimMeshRes.get()->Render( pd3dDevice );
	}
}

void DxSkyBox::Render_DOF( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vPos )
{
	if( !m_SimMeshRes.Update() )
		return;

	if( m_emBlend == BLEND_LIGHT_SHAFTS )
		return;

	D3DXMATRIX matScale, matRotationY;
	D3DXMatrixScaling( &matScale, m_fScale, m_fScale, m_fScale );
	D3DXMatrixRotationY( &matRotationY, m_fRotationY*D3DX_PI/180.f );
	D3DXMatrixMultiply( &matScale, &matScale, &matRotationY );
	matScale._41 = vPos.x;
	matScale._42 = vPos.y + m_fHeight;
	matScale._43 = vPos.z;

	pd3dDevice->SetTransform( D3DTS_WORLD, &matScale );
	m_SimMeshRes.get()->Render( pd3dDevice );
}

void DxSkyBox::Render_LightShafts( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vPos )
{
	if( !m_SimMeshRes.Update() )
		return;

	if( m_emBlend != BLEND_LIGHT_SHAFTS )
		return;

	D3DXMATRIX matScale, matRotationY;
	D3DXMatrixScaling( &matScale, m_fScale, m_fScale, m_fScale );
	D3DXMatrixRotationY( &matRotationY, m_fRotationY*D3DX_PI/180.f );
	D3DXMatrixMultiply( &matScale, &matScale, &matRotationY );
	matScale._41 = vPos.x;
	matScale._42 = vPos.y + m_fHeight;
	matScale._43 = vPos.z;

	pd3dDevice->SetTransform( D3DTS_WORLD, &matScale );
	m_SimMeshRes.get()->Render( pd3dDevice );
}

BOOL DxSkyBox::IsHaveLightShafts()
{
	if( m_emBlend == BLEND_LIGHT_SHAFTS )
		return TRUE;
	return FALSE;
}

//BOOL DxSkyBox::IsCollision( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd )
//{
//	return m_SimMeshRes.get()->IsCollision( vStart, vEnd );
//}

DxSkyBox* DxSkyBox::CreateClone( LPDIRECT3DDEVICEQ pd3dDevice )
{
	DxSkyBox* pClone = new DxSkyBox;
	*pClone = *this;

	pClone->OnCreateDevice( pd3dDevice, TRUE );

	return pClone;
}

void DxSkyBox::GetOnlyData( const DxSkyBox* pSkyBox )
{
	m_fRotationY		= pSkyBox->m_fRotationY;
	m_fRotationBaseY	= pSkyBox->m_fRotationBaseY;
	m_fRotationSPEED	= pSkyBox->m_fRotationSPEED;
	m_fScale			= pSkyBox->m_fScale;
	m_fHeight			= pSkyBox->m_fHeight;
	m_emBlend			= pSkyBox->m_emBlend;
	m_fPower			= pSkyBox->m_fPower;
	m_strName			= pSkyBox->m_strName;
}

void DxSkyBox::SetSkyBoxRotateY( const DxSkyBox* pSrc )
{
	m_fRotationY		= pSrc->m_fRotationY;
}

void DxSkyBox::SetObjRotate90( float fRotate )
{
    m_fRotationY	    += fRotate;
    m_fRotationBaseY    += fRotate;

    if( m_fRotationY > 360.f )
        m_fRotationY -= 360.f;
    if( m_fRotationBaseY > 360.f )
        m_fRotationBaseY -= 360.f;
}

void DxSkyBox::Save( sc::SerialFile& SFile )
{
	SFile << VERSION;

	SFile.BeginBlock();
	{
		SFile << m_fRotationBaseY;
		SFile << m_fRotationSPEED;
		SFile << m_fScale;
		SFile << m_fHeight;
		SFile << m_emBlend;
		SFile << m_fPower;
		SFile << m_strName;
	}
	SFile.EndBlock();
}

void DxSkyBox::Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bTexThread )
{
	DWORD dwVer, dwBufferSize;

	SFile >> dwVer;
	SFile >> dwBufferSize;

	DWORD dwTemp;
	SFile >> m_fRotationBaseY;
	SFile >> m_fRotationSPEED;
	SFile >> m_fScale;
	SFile >> m_fHeight;
	SFile >> dwTemp;			m_emBlend = static_cast<BLEND>(dwTemp);
	SFile >> m_fPower;
	SFile >> m_strName;

	if( dwVer <= 0x104 )
	{
		m_fPower = 1.f;
	}

	OnCreateDevice( pd3dDevice, bTexThread );
}

void DxSkyBox::Load_Ver103( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bTexThread )
{
	BOOL bTemp(FALSE);

	SFile >> m_fRotationBaseY;
	SFile >> m_fRotationSPEED;
	SFile >> m_fScale;
	SFile >> m_fHeight;
	SFile >> bTemp;
	SFile >> m_strName;

	m_emBlend = BLEND_NONE;
	m_fPower = 1.0f;
	if( bTemp )
	{
		m_emBlend = BLEND_ALPHA;
	}

	OnCreateDevice( pd3dDevice, bTexThread );
}

void DxSkyBox::Load_Ver102( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bTexThread )
{
	BOOL bTemp(FALSE);

	SFile >> m_fRotationBaseY;
	SFile >> m_fScale;
	SFile >> m_fHeight;
	SFile >> bTemp;
	SFile >> m_strName;

	m_fRotationSPEED = 0.f;
	m_emBlend = BLEND_NONE;
	m_fPower = 1.0f;
	if( bTemp )
	{
		m_emBlend = BLEND_ALPHA;
	}

	OnCreateDevice( pd3dDevice, bTexThread );
}

void DxSkyBox::Load_Ver101( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bTexThread )
{
	BOOL bTemp(FALSE);

	SFile >> m_fScale;
	SFile >> m_fHeight;
	SFile >> bTemp;
	SFile >> m_strName;

	m_fRotationBaseY = 0.f;
	m_fRotationSPEED = 0.f;
	m_emBlend = BLEND_NONE;
	m_fPower = 1.0f;
	if( bTemp )
	{
		m_emBlend = BLEND_ALPHA;
	}

	OnCreateDevice( pd3dDevice, bTexThread );
}

void DxSkyBox::Load_Ver100( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bTexThread )
{
	BOOL bTemp(FALSE);

	SFile >> m_fScale;
	SFile >> bTemp;
	SFile >> m_strName;

	m_fHeight = 0.f;
	m_fRotationBaseY = 0.f;
	m_fRotationSPEED = 0.f;
	m_emBlend = BLEND_NONE;
	m_fPower = 1.0f;
	if( bTemp )
	{
		m_emBlend = BLEND_ALPHA;
	}

	OnCreateDevice( pd3dDevice, bTexThread );
}

void DxSkyBox::Clone( LPDIRECT3DDEVICEQ pd3dDevice, const DxSkyBox* pSrc, BOOL bTexThread )
{
	m_fRotationBaseY	= pSrc->m_fRotationBaseY;
	m_fRotationSPEED	= pSrc->m_fRotationSPEED;
	m_fScale			= pSrc->m_fScale;
	m_fHeight			= pSrc->m_fHeight;
	m_emBlend			= pSrc->m_emBlend;
	m_fPower			= pSrc->m_fPower;
	m_strName			= pSrc->m_strName;

	OnCreateDevice( pd3dDevice, bTexThread );
}
