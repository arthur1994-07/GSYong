#include "pch.h"

#include "../../../DxTools/TextureManager.h"
#include "../../../DxTools/DxDynamicVB.h"

#include "../../../DxEffect/Frame/DxEffectMan.h"
#include "../../../DxEffect/Single/DxEffSingleMan.h"
#include "../../../../SigmaCore/File/SerialFile.h"

#include "./DxEffCircle.h"

// ----------------------------------------------------------------------------
#include "../../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//---------------------------------------------------------------------[CAMERA_PROPERTY]
const DWORD	CIRCLE_PROPERTY::VERSION = 0x0100;
const char	CIRCLE_PROPERTY::NAME[]	 = "��";

DxEffSingle* CIRCLE_PROPERTY::NEWOBJ ( boost::shared_ptr<DxEffSingleGroup>& spThisGroup )
{
	DxEffCircle *pEffect = new DxEffCircle;

	//	Note : TransPorm Set.		[���뼳��]
	pEffect->m_matLocal = m_matLocal;

	// Note : �ð� ����.			[���뼳��]
	pEffect->m_fGBeginTime = m_fGBeginTime;
	pEffect->m_fGLifeTime  = m_fGLifeTime;

	// Note : ���ư��� ������Ʈ	[���뼳��]
	pEffect->m_bMoveObj	= m_bMoveObj;

	// Note : ���� ����
	pEffect->UpdateProperty( (*this) );

	return pEffect;
}

HRESULT CIRCLE_PROPERTY::SaveFile ( sc::SerialFile &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	//	Note : DATA�� ����� ����Ѵ�. Load �ÿ� ������ Ʋ�� ��� ����.
	//
	SFile << (DWORD) ( GetSizeBase() + sizeof(m_Property) );

	//	Note : �θ� Ŭ������ ����.
	SFile.WriteBuffer( m_matLocal, sizeof(D3DXMATRIX) );

	SFile << m_bMoveObj;
	SFile << m_fGBeginTime;
	SFile << m_fGLifeTime;

	//	Note : ����Ʈ�� Property �� ����.
	//
	SFile.WriteBuffer ( &m_Property, sizeof(PROPERTY) );

	//	Note : ����, �ڽ� ����.
	//
	EFF_PROPERTY::SaveFile ( SFile );

	return S_OK;
}

HRESULT CIRCLE_PROPERTY::LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache, BOOL bOldDecalUSE )
{
	HRESULT hr;
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	//	Note : ������ ��ġ�� ���. 
	//
	if( dwVer == VERSION )
	{
		//	Note : �θ� Ŭ������ ����.
		SFile.ReadBuffer ( m_matLocal, sizeof(D3DXMATRIX) );

		SFile >> m_bMoveObj;
		SFile >> m_fGBeginTime;
		SFile >> m_fGLifeTime;

		//	Note : ����Ʈ�� Property �� ����.
		//
		SFile.ReadBuffer ( &m_Property, sizeof(PROPERTY) );

		m_textureRes.Clear();
	}
	else
	{
		//	Note : ������ Ʋ�� ��쿡�� ���Ͽ� ������ DATA������ �Ǵ� ��� �۾��� ����.
		//
		DWORD dwCur = SFile.GetfTell ();
		SFile.SetOffSet ( dwCur+dwSize );
	}

	//	Note : Device �ڿ��� �����Ѵ�.
	//
	hr = Create ( pd3dDevice, bBackUpCache );
	if ( FAILED(hr) )	return hr;

	//	Note : ����, �ڽ� �б�.
	//
	EFF_PROPERTY::LoadFile ( SFile, pd3dDevice, bBackUpCache, bOldDecalUSE );

	return S_OK;
}

HRESULT CIRCLE_PROPERTY::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache )
{
	m_textureRes = TextureManager::GetInstance().LoadTexture(
		m_szTexture,
		false,
		TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
		0,
		D3DFMT_UNKNOWN,
		TEXTURE_RESIZE_NONE,
		TEXTURE_EFFECT,
		true,
		bBackUpCache );

	//	Note : ����, �ڽ�.
	//
	EFF_PROPERTY::InitDeviceObjects ( pd3dDevice, bBackUpCache );

	return S_OK;
}

HRESULT CIRCLE_PROPERTY::DeleteDeviceObjects ()
{
	m_textureRes.Clear();

	//	Note : ����, �ڽ�.
	//
	EFF_PROPERTY::DeleteDeviceObjects ();

	return S_OK;
}


//-------------------------------------------- [���� �ɹ� ���� �� �ʱ�ȭ] ----------------------------------------------
//
//
const DWORD	DxEffCircle::TYPEID	= EFFSINGLE_CIRCLE;
const DWORD	DxEffCircle::FLAG	= NULL;
const char	DxEffCircle::NAME[]	= "��";

LPDIRECT3DSTATEBLOCK9 DxEffCircle::m_pSavedRenderStats = NULL;
LPDIRECT3DSTATEBLOCK9 DxEffCircle::m_pDrawRenderStats  = NULL;

//	Note : 
//
BOOL DxEffCircle:: VAILEDDEVICE = FALSE;

HRESULT DxEffCircle::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;	

	VAILEDDEVICE = TRUE;

	D3DCAPSQ d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState( D3DRS_COLORVERTEX,		TRUE );
		pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,			0x00000000 );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_CULLMODE,			D3DCULL_NONE );

		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_ALPHAREF,			0x01 );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2,	D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );

		pd3dDevice->SetSamplerState ( 0, D3DSAMP_BORDERCOLOR, 0x00000000 );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSU,	D3DTADDRESS_BORDER );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSV,	D3DTADDRESS_BORDER );

		if( which==0 )	pd3dDevice->EndStateBlock ( &m_pSavedRenderStats );
		else			pd3dDevice->EndStateBlock ( &m_pDrawRenderStats );
	}

	return S_OK;
}

HRESULT DxEffCircle::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	VAILEDDEVICE = FALSE;

	SAFE_RELEASE( m_pSavedRenderStats );
	SAFE_RELEASE( m_pDrawRenderStats );

	return S_OK;
}

void DxEffCircle::OnInitDevice_STATIC()
{
}

void DxEffCircle::OnDeleteDevice_STATIC()
{
}


HRESULT DxEffCircle::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	//	Note : ����, �ڽ� ȣ��
	//
	if ( m_rThisGroup )
	{
		DxEffSingle::RestoreDeviceObjects ( pd3dDevice );
	}
	else
	{
		if ( m_pChild )		m_pChild->RestoreDeviceObjects ( pd3dDevice );
		if ( m_pSibling )	m_pSibling->RestoreDeviceObjects ( pd3dDevice );
	}

	return S_OK;
}

HRESULT DxEffCircle::InvalidateDeviceObjects ()
{
	//	Note : ����, �ڽ� ȣ��
	//
	DxEffSingle::InvalidateDeviceObjects ();

	return S_OK;
}

//-------------------------------------------- [Ŭ���� ��ü] ------------------------------------------------------------
//
//
DxEffCircle::DxEffCircle () 
: m_pd3dDevice(NULL)
, m_pVB(NULL)
, m_dwVerSize(50)
{
}

DxEffCircle::~DxEffCircle ()
{
	SAFE_RELEASE(m_pVB);
	CleanUp ();
}

HRESULT DxEffCircle::FinalCleanup()
{
	//	Note : ���� �ڽ� ���.
	//
	DxEffSingle::FinalCleanup ();

	return S_OK;
}

HRESULT DxEffCircle::FrameMove ( float fTime, float fElapsedTime )
{
	D3DXMATRIX	matLocal;

	if ( m_dwRunFlag & EFF_PLY_END )
	{
		goto _RETURN;
	}

	//	Note : Eff�� ���̸� ���.
	//
	DxEffSingle::m_fGAge += fElapsedTime;


	//	Note : �ݺ��� �� �ð� �� ���� ���� �ʱ�ȭ �Ѵ�.
	//
	if ( m_rThisGroup && m_rThisGroup->m_dwFlag&EFF_CFG_NEVERDIE )
	{
		if ( m_fGAge>=(m_rThisGroup->m_fGBeginTime+m_rThisGroup->m_fGLifeTime) )
		{
			m_fGAge		= 0.f;
			m_dwRunFlag &= ~EFF_PLY_PLAY;
			m_dwRunFlag &= ~EFF_PLY_DONE;
			m_dwRunFlag &= ~EFF_PLY_END;
		}
	}

	//	Note : ���۵��� �ʾҴ��� �˻�, ���� �ð��̸� Ȱ��ȭ ��Ŵ.
	//
	if ( !( DxEffSingle::m_dwRunFlag & EFF_PLY_PLAY ) )
	{
		if ( m_fGAge >= m_fGBeginTime )		m_dwRunFlag |= EFF_PLY_PLAY;
		else								goto _RETURN;
	}

	//	Note : ���� ������ �Ǿ����� �˻�.
	//
	if ( m_rThisGroup )
	{
		if ( IsReadyToDie() )					
			DxEffSingle::m_dwRunFlag |= EFF_PLY_END;
	}

	if ( DxEffSingle::m_dwRunFlag&EFF_PLY_RENDPASS )		
		goto _RETURN;	// ��鸮�� �ʵ���

	UpdateProperty();

_RETURN:
	//	Note : ����, �ڽ� ���.
	//

	if ( m_rThisGroup )
	{
		DxEffSingle::FrameMove( fTime, fElapsedTime );
		DxEffSingle::FrameMove( fTime, fElapsedTime );
	}
	else
	{
		if ( m_pChild )		m_pChild->FrameMove( fTime, fElapsedTime );
		if ( m_pSibling )	m_pSibling->FrameMove ( fTime, fElapsedTime );
	}

	return S_OK;
}

HRESULT DxEffCircle::Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matComb, float fAlpha )
{
	HRESULT hr = S_OK;

	if ( !std::string(m_Property.m_szTexture).empty() && !m_Property.m_textureRes.GetTexture() )
	{
		return S_FALSE;
	}

	D3DXMATRIX matWorld = m_matLocal;
	D3DXMatrixMultiply ( &matWorld, &m_matLocal, &matComb );

	if ( !(DxEffSingle::m_dwRunFlag&EFF_PLY_PLAY) )		goto _RETURN;
	if ( DxEffSingle::m_dwRunFlag&EFF_PLY_END )			goto _RETURN;
	if ( DxEffSingle::m_dwRunFlag&EFF_PLY_RENDPASS )	goto _RETURN;

	if ( !m_pVB )
	{
		goto _RETURN;
	}

	pd3dDevice->SetTransform ( D3DTS_WORLD, &matWorld );

	m_pSavedRenderStats->Capture();
	m_pDrawRenderStats->Apply();

	{
		DWORD dwVerCount = 0;

		pd3dDevice->SetIndices( NULL );

		if ( CalculateInnerVerticesNum() )
		{
			if( m_Property.m_textureRes.Update() )
			{
				pd3dDevice->SetTexture(0, m_Property.m_textureRes.GetTexture());
			}
			else
			{
				pd3dDevice->SetTexture(0, NULL);
			}

			pd3dDevice->SetStreamSource ( 0, m_pVB, 0, sizeof(VERTEXCOLORTEX) );
			pd3dDevice->SetFVF ( VERTEXCOLORTEX::FVF );
			pd3dDevice->DrawPrimitive ( D3DPT_TRIANGLELIST, dwVerCount, CalculateInnerFaceNum() );

			dwVerCount += CalculateInnerVerticesNum();
		}

		if ( CalculateOuterVerticesNum() )
		{
			pd3dDevice->SetTexture(0, NULL);
			pd3dDevice->SetStreamSource ( 0, m_pVB, 0, sizeof(VERTEXCOLORTEX) );
			pd3dDevice->SetFVF ( VERTEXCOLORTEX::FVF );
			pd3dDevice->DrawPrimitive ( D3DPT_TRIANGLELIST, dwVerCount, CalculateOuterFaceNum() );

			dwVerCount += CalculateOuterVerticesNum();
		}
	}

	m_pSavedRenderStats->Apply();

_RETURN:
	//	Note : ����, �ڽ� ���.
	//
	if ( m_pChild )		m_pChild->Render ( pd3dDevice, matWorld, fAlpha );
	if ( m_pSibling )	m_pSibling->Render ( pd3dDevice, matWorld, fAlpha );

	return hr;
}

void DxEffCircle::UpdateProperty( const CIRCLE_PROPERTY& cProperty )
{
	bool bRebuild   = false;
	bool bTexReload = false;

	if ( m_Property.ISREBUILD(cProperty) )
	{
		bRebuild = true;
	}

	std::string strTextureSrc(cProperty.m_szTexture);
	std::string strTextureDst(m_Property.m_szTexture);

	if ( strTextureDst != strTextureSrc )
	{
		bTexReload = true;
	}

	//! �ؽ��� ������ �������� �ʴ´�.
	TextureResource OrgResource = m_Property.m_textureRes;
	m_Property = cProperty;
	m_Property.m_textureRes = OrgResource;

	UpdateProperty( bTexReload, bRebuild );
}

void DxEffCircle::UpdateProperty( const bool bTexReload, const bool bRebuild )
{
	if ( !m_pd3dDevice )
	{
		return;
	}

	//! �ؽ��� �ε�
	if ( bTexReload || !m_Property.m_textureRes.IsValid() || !m_Property.m_textureRes.GetTexture() )
	{
		m_Property.m_textureRes.Clear();
		std::string strTexture = m_Property.m_szTexture;

		if ( !strTexture.empty() )
		{
			m_Property.m_textureRes = TextureManager::GetInstance().LoadTexture(
				m_Property.m_szTexture,
				false,
				TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
				0,
				D3DFMT_UNKNOWN,
				TEXTURE_RESIZE_NONE,
				TEXTURE_EFFECT,
				true,
				FALSE );
		}
	}

	DWORD dwVerCount = CalculateOuterVerticesNum() + CalculateInnerVerticesNum();

	if ( m_dwVerSize < dwVerCount )
	{
		m_dwVerSize = dwVerCount;
		SAFE_RELEASE(m_pVB);
	}

	if ( !m_pVB )
	{
		HRESULT hr = m_pd3dDevice->CreateVertexBuffer(
			m_dwVerSize*sizeof(VERTEXCOLORTEX),
			D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
			VERTEXCOLORTEX::FVF,
			D3DPOOL_SYSTEMMEM,
			&m_pVB,
			NULL );

		BuildCircle();
	}
	else if ( bRebuild )
	{
		BuildCircle();
	}
}

const DWORD DxEffCircle::CalculateInnerFaceNum( const bool bRenderCheck )
{
	DWORD dwFaceCount = 0;
	
	if ( !bRenderCheck || m_Property.m_dwColor != 0 )
	{
		dwFaceCount += DWORD(m_Property.m_fVertexPerDegree * 360.0f);
	}

	return dwFaceCount;
}

const DWORD DxEffCircle::CalculateInnerVerticesNum()
{
	return CalculateInnerFaceNum()*3;
}

const DWORD DxEffCircle::CalculateOuterFaceNum()
{
	DWORD dwFaceCount = 0;

	if ( 0.0f < m_Property.m_fOutLine && m_Property.m_dwOutLineColor != 0 )
	{
		dwFaceCount += DWORD(m_Property.m_fVertexPerDegree * 360.0f * 2.0f/*���� �Ѹ�� �ΰ���*/);
	}

	return dwFaceCount;
}

const DWORD DxEffCircle::CalculateOuterVerticesNum()
{
	return CalculateOuterFaceNum()*3;
}

void DxEffCircle::BuildCircle()
{
	if ( !m_pVB )
	{
		return;
	}

	VERTEXCOLORTEX* pLocked = NULL;
	HRESULT hResult = m_pVB->Lock( 0, 0, (VOID**)&pLocked, 0 );

	if ( SUCCEEDED(hResult) )
	{
		float fRadius = m_Property.m_fRadius;

		//! ���� �� �׸���
		if ( CalculateInnerFaceNum() )
		{
			DWORD dwVerCount = CalculateInnerVerticesNum();
			DWORD dwFaceNum  = CalculateInnerFaceNum();

			float fCurRadian     = 0.0f;
			float fRadianPerFace = (2.0f*D3DX_PI/static_cast<float>(dwFaceNum));

			float fCurTexRadian		= 0.0f;
			float fTexRadianPerFace = (2.0f*D3DX_PI/static_cast<float>(dwFaceNum));

			for ( DWORD i=0; i<dwFaceNum; ++i )
			{
				if ( m_Property.m_dwColor != 0 )
				{
					pLocked->vPos	 = D3DXVECTOR3(sin(fCurRadian) * fRadius, 0.0f, cos(fCurRadian) * fRadius);
					pLocked->dwColor = m_Property.m_dwColor;
					pLocked->vTex	 = D3DXVECTOR2( 0.5f + sin(fCurTexRadian)*0.5f,  0.5f + cos(fCurTexRadian)*0.5f );
					++pLocked;

					pLocked->vPos	 = D3DXVECTOR3(sin(fCurRadian + fRadianPerFace) * fRadius, 0.0f, cos(fCurRadian + fRadianPerFace) * fRadius);
					pLocked->dwColor = m_Property.m_dwColor;
					pLocked->vTex	 = D3DXVECTOR2( 0.5f + sin(fCurTexRadian + fTexRadianPerFace)*0.5f,  0.5f + cos(fCurTexRadian + fTexRadianPerFace)*0.5f );
					++pLocked;

					pLocked->vPos	 = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
					pLocked->dwColor = m_Property.m_dwColor;
					pLocked->vTex	 = D3DXVECTOR2( 0.5f, 0.5f );
					++pLocked;
				}

				fCurRadian    += fRadianPerFace;
				fCurTexRadian += fTexRadianPerFace;
			}
		}

		//! �ܰ��� �׸���
		if ( CalculateOuterFaceNum() )
		{
			//! �ܰ����� ���� �Ѹ�� ���� �ΰ��� �ʿ��ϴ�
			DWORD dwInnerFaceNum = CalculateInnerFaceNum(false);

			float fCurRadian     = 0.0f;
			float fRadianPerFace = (2.0f*D3DX_PI/static_cast<float>(dwInnerFaceNum));

			for ( DWORD i=0; i<dwInnerFaceNum; ++i )
			{
				if ( m_Property.m_dwOutLineColor != 0 )
				{
					D3DXVECTOR3 vPtLT = D3DXVECTOR3(sin(fCurRadian) * fRadius, 0.0f, cos(fCurRadian) * fRadius);
					D3DXVECTOR3 vPtRT = D3DXVECTOR3(sin(fCurRadian + fRadianPerFace) * fRadius, 0.0f, cos(fCurRadian + fRadianPerFace) * fRadius);
					D3DXVECTOR3 vPtRB = D3DXVECTOR3(sin(fCurRadian + fRadianPerFace) * (fRadius - m_Property.m_fOutLine)
						, 0.0f, cos(fCurRadian + fRadianPerFace) * (fRadius - m_Property.m_fOutLine));
					D3DXVECTOR3 vPtLB = D3DXVECTOR3(sin(fCurRadian) * (fRadius - m_Property.m_fOutLine)
						, 0.0f, cos(fCurRadian) * (fRadius - m_Property.m_fOutLine));

					pLocked->vPos	 = vPtLT;
					pLocked->dwColor = m_Property.m_dwOutLineColor;
					pLocked->vTex	 = D3DXVECTOR2( 0.0f, 0.0f );
					++pLocked;

					pLocked->vPos	 = vPtRT;
					pLocked->dwColor = m_Property.m_dwOutLineColor;
					pLocked->vTex	 = D3DXVECTOR2( 0.0f, 0.0f );
					++pLocked;

					pLocked->vPos	 = vPtRB;
					pLocked->dwColor = m_Property.m_dwOutLineColor;
					pLocked->vTex	 = D3DXVECTOR2( 0.0f, 0.0f );
					++pLocked;

					pLocked->vPos	 = vPtRB;
					pLocked->dwColor = m_Property.m_dwOutLineColor;
					pLocked->vTex	 = D3DXVECTOR2( 0.0f, 0.0f );
					++pLocked;

					pLocked->vPos	 = vPtLB;
					pLocked->dwColor = m_Property.m_dwOutLineColor;
					pLocked->vTex	 = D3DXVECTOR2( 0.0f, 0.0f );
					++pLocked;

					pLocked->vPos	 = vPtLT;
					pLocked->dwColor = m_Property.m_dwOutLineColor;
					pLocked->vTex	 = D3DXVECTOR2( 0.0f, 0.0f );
					++pLocked;
				}

				fCurRadian    += fRadianPerFace;
			}
		}
	}	
	m_pVB->Unlock();
}