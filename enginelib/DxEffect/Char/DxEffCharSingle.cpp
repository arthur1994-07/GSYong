#include "pch.h"

#include "../../DxEffect/Frame/DxEffectMan.h"

#include "../../DxEffect/Single/DxEffSingle.h"
#include "../Single/DxEffSinglePropGMan.h"
#include "../../../SigmaCore/File/SerialFile.h"

#include "../../DxTools/DxWeatherMan.h"
#include "../../DxTools/Light/DxLightMan.h"
#include "../../G-Logic/GLPeriod.h"
#include "../../DxMeshs/SkinMesh/DxCharPart.h"
#include "../../DxMeshs/SkinMesh/DxSkinPieceContainer.h"

#include "../../DxTools/EDITMESHS.h"

#include "./DxEffCharSingle.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

extern BOOL g_bCHAR_EDIT_RUN;
extern BOOL g_bEFFECT_SPHERE_VISIBLE;

DWORD		DxEffCharSingle::TYPEID				= EMEFFCHAR_SINGLE;
const DWORD	DxEffCharSingle::VERSION			= 0x0108;
char		DxEffCharSingle::NAME[MAX_PATH]		= "4.�ܵ� ����Ʈ";

DxEffCharSingle::DxEffCharSingle(void)
	: DxEffChar()
	, m_pd3dDevice(NULL)

	, m_sPropGroupRes(NULL)
	, m_vTransUp(0.f,0.f,0.f)
	, m_vPrevUp(0.f,0.f,0.f)
	, m_vTransDown(0.f,0.f,0.f)
	, m_vBackUp(0.f,0.f,0.f)
	, m_fTarDirection(0.f)
	, m_vDirection(0,0,0)
	, m_fCoolTime(10.f)
	, m_fCoolTimeADD(0.f)
	, m_vCurPos(0.f,0.f,0.f)
	, m_pSkinMesh(NULL)
	, m_fPlayTime(0.f)
	, m_nCalculationMatrixMode(0)
{
	memset( m_szFileName, 0, sizeof(char)*MAX_PATH );
	memset( m_szTrace, 0, sizeof(char)*STRACE_NSIZE );
	memset( m_szTraceBack, 0, sizeof(char)*STRACE_NSIZE );

    D3DXMatrixIdentity( &m_matBase );
    //D3DXMatrixIdentity( &m_matWorld );
}

DxEffCharSingle::~DxEffCharSingle(void)
{
	CleanUp ();
}

DxEffChar* DxEffCharSingle::CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DWORD dwMaterialNum )
{
	HRESULT hr;
	DxEffCharSingle *pEffCharSingle = new DxEffCharSingle;
	pEffCharSingle->SetFileName ( GetFileName() );
	pEffCharSingle->SetLinkObj ( pCharPart, pSkinPiece );
	pEffCharSingle->SetProperty ( &m_Property );

	// �̰� ������ �־��̴�. �Ф�. �ڵ带 �������� �ִ�.
	pEffCharSingle->m_nCalculationMatrixMode = m_nCalculationMatrixMode;

	hr = pEffCharSingle->Create ( pd3dDevice );
	if ( FAILED(hr) )
	{
		SAFE_DELETE(pEffCharSingle);
		return NULL;
	}

	return pEffCharSingle;
}

// DxEffSinglePropGMan�� ThreadLoading �ϵ��� �ٲپ��� ������ DxSingleGroup�� �ε��ȵǾ����� �ε��� �õ��Ѵ�.
void DxEffCharSingle::CreateDxEffSingleGroup()
{
	if ( m_spSingleGroup.get() )
		return;

	if ( m_sPropGroupRes.Update() )	
	{	
		m_spSingleGroup.reset();						// ���� �����ʹ� �����ش�.
		m_spSingleGroup = m_sPropGroupRes.get()->NEWEFFGROUP ();
		if ( m_spSingleGroup.get() )
		{
			//	Note : Eff Group Create Dev.
			//
			m_spSingleGroup->Create ( m_pd3dDevice );
			m_spSingleGroup->m_matWorld = m_matTrans;
		}
	}
}

HRESULT DxEffCharSingle::OneTimeSceneInit ()
{
	if ( m_spSingleGroup.get() )
        m_spSingleGroup->OneTimeSceneInit ();

	return S_OK;
}

HRESULT DxEffCharSingle::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;

	m_pd3dDevice = pd3dDevice;

	if ( m_pCharPart )
	{
		m_pSkinMesh = m_pCharPart->m_rSkinMesh;
	}
	else if ( m_pSkinPiece )
	{
		m_pSkinMesh = m_pSkinPiece->m_skinMeshRes.get();
	}

	// �̸��� ������ �۾���.
	if ( strlen(m_szFileName) )
	{
		m_sPropGroupRes = DxEffSinglePropGMan::GetInstance().LoadEffectGProp ( m_szFileName, TRUE, FALSE );

		// DxEffSinglePropGMan�� ThreadLoading �ϵ��� �ٲپ��� ������ DxSingleGroup�� �ε��ȵǾ����� �ε��� �õ��Ѵ�.
		CreateDxEffSingleGroup();
	}

	//	Note : Eff Group�� ��Ʈ���� Ʈ������ ����.
	//
	if ( m_szTrace[0]==NULL )	
		return E_FAIL;

	D3DXVECTOR3	vNormal;
	if ( m_pCharPart )
	{
		hr = m_pCharPart->CalculateVertexInflu ( m_szTrace, m_vTransUp, vNormal );
		if ( FAILED(hr) )	return E_FAIL;
		D3DXMatrixTranslation ( &m_matTrans, m_vTransUp.x, m_vTransUp.y, m_vTransUp.z );
	}
	else if ( m_pSkinPiece )
	{
		hr = m_pSkinPiece->CalculateVertexInflu ( m_szTrace, m_vTransUp, vNormal );
		if ( FAILED(hr) )	return E_FAIL;
		D3DXMatrixTranslation ( &m_matTrans, m_vTransUp.x, m_vTransUp.y, m_vTransUp.z );
	}

	m_fPlayTime = 0.f;


	//m_sPropGroupRes = DxEffSinglePropGMan::GetInstance().LoadEffectGProp( m_szFileName, TRUE );
	//if ( !m_sPropGroupRes )	return E_FAIL;

	//if ( m_pPropGroup )
	//{
	//	D3DXVECTOR3	vNormal;

	//	m_spSingleGroup.reset();						// ���� �����ʹ� �����ش�.
	//	m_spSingleGroup = m_pPropGroup->NEWEFFGROUP ();
	//	if ( m_spSingleGroup.get() == NULL )
 //           return E_FAIL;

	//	//	Note : Eff Group Create Dev.
	//	//
	//	m_spSingleGroup->Create ( pd3dDevice );

	//	//	Note : Eff Group�� ��Ʈ���� Ʈ������ ����.
	//	//
	//	if ( m_szTrace[0]==NULL )	return E_FAIL;

	//	if ( m_pCharPart )
	//	{
	//		hr = m_pCharPart->CalculateVertexInflu ( m_szTrace, m_vTransUp, vNormal );
	//		if ( FAILED(hr) )	return E_FAIL;
	//		D3DXMatrixTranslation ( &m_matTrans, m_vTransUp.x, m_vTransUp.y, m_vTransUp.z );
	//	}
	//	else if ( m_pSkinPiece )
	//	{
	//		hr = m_pSkinPiece->CalculateVertexInflu ( m_szTrace, m_vTransUp, vNormal );
	//		if ( FAILED(hr) )	return E_FAIL;
	//		D3DXMatrixTranslation ( &m_matTrans, m_vTransUp.x, m_vTransUp.y, m_vTransUp.z );
	//	}

	//	m_fPlayTime = 0.f;
	//	m_spSingleGroup->m_matWorld = m_matTrans;
	//}

	//if ( m_spSingleGroup.get() )
 //       m_spSingleGroup->InitDeviceObjects ( pd3dDevice );

	return S_OK;
}

HRESULT DxEffCharSingle::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_spSingleGroup.get() )
        m_spSingleGroup->RestoreDeviceObjects ( pd3dDevice );

	return S_OK;
}

HRESULT DxEffCharSingle::InvalidateDeviceObjects ()
{
	if ( m_spSingleGroup.get() )
        m_spSingleGroup->InvalidateDeviceObjects ();

	return S_OK;
}

HRESULT DxEffCharSingle::DeleteDeviceObjects ()
{
	if ( m_spSingleGroup.get() )
        m_spSingleGroup->DeleteDeviceObjects ();

	return S_OK;
}

HRESULT DxEffCharSingle::FinalCleanup ()
{
	if ( m_spSingleGroup.get() )
        m_spSingleGroup->FinalCleanup();
	m_spSingleGroup.reset();

	return S_OK;
}

HRESULT DxEffCharSingle::FrameMove ( float fTime, float fElapsedTime )
{
	HRESULT hr = S_OK;

	// �����Ϳ����� ��� �������Ѵ�.
	if( !g_bCHAR_EDIT_RUN )
	{
		if ( !SetTime() )			return S_OK;
		if ( !SetWeather() )		return S_OK;
	}

	m_fTime = fTime;
	m_fPlayTime += fElapsedTime;

	// DxEffSinglePropGMan�� ThreadLoading �ϵ��� �ٲپ��� ������ DxSingleGroup�� �ε��ȵǾ����� �ε��� �õ��Ѵ�.
	CreateDxEffSingleGroup();

	if ( m_spSingleGroup.get() )
	{
		m_spSingleGroup->FrameMove ( fTime, fElapsedTime );

		if ( m_dwFlag&USE_REPEAT )
		{
			m_fCoolTimeADD += fElapsedTime;
			if ( m_fCoolTimeADD > m_fCoolTime )
			{
				m_fCoolTimeADD = 0.f;
				m_spSingleGroup->ReStartEff();
			}
		}
		else
		{
			if ( m_spSingleGroup->m_dwAliveCount == 0 )
			{
				SetEnd();
			}
		}
	}

	return S_OK;
}

void	DxEffCharSingle::PrecalculateBeforeRenderEff()
{
	HRESULT hr = S_OK;

	if( m_spSingleGroup.get() == NULL )
		return;

	// �����Ϳ����� ��� �������Ѵ�.
	if( !g_bCHAR_EDIT_RUN )
	{
		if ( !SetTime() )			return;
		if ( !SetWeather() )		return;
	}

	if ( m_dwFlag&(USE_1POINT | USE_MATRIX) )
	{
		if ( m_pCharPart )
		{
			hr = m_pCharPart->CalculateVertexInflu( m_szTrace, m_vTransUp, m_vNormalUp, &m_matVertex );
			if ( FAILED(hr) )	return;
		}
		else if ( m_pSkinPiece )
		{
			hr = m_pSkinPiece->CalculateVertexInflu( m_szTrace, m_vTransUp, m_vNormalUp, &m_matVertex );
			if ( FAILED(hr) )	return;
		}
	}
	else if ( m_dwFlag&USE_2POINT )
	{
		if ( m_pCharPart )
		{
			hr = m_pCharPart->CalculateVertexInflu( m_szTrace, m_vTransUp, m_vNormalUp, &m_matVertex );
			if ( FAILED(hr) )	return;

			hr = m_pCharPart->CalculateVertexInflu( m_szTraceBack, m_vTransDown, m_vNormalDown, &m_matVertex );
			if ( FAILED(hr) )	return;
		}
		else if ( m_pSkinPiece )
		{
			hr = m_pSkinPiece->CalculateVertexInflu( m_szTrace, m_vTransUp, m_vNormalUp, &m_matVertex );
			if ( FAILED(hr) )	return;

			hr = m_pSkinPiece->CalculateVertexInflu( m_szTraceBack, m_vTransDown, m_vNormalDown, &m_matVertex );
			if ( FAILED(hr) )	return;
		}
	}
}

void	DxEffCharSingle::OnlyRenderEff( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( m_spSingleGroup.get() == NULL )
		return;

	// �����Ϳ����� ��� �������Ѵ�.
	if( !g_bCHAR_EDIT_RUN )
	{
		if ( !SetTime() )			return;
		if ( !SetWeather() )		return;
	}

	BOOL bUse = TRUE;
	if ( (m_nAniMType==1) && (m_CurAniMType!=AN_ATTACK) )		bUse = FALSE;		// ���ݽ� ����
	else if ( (m_nAniMType==2) && (m_CurAniMType==AN_ATTACK) )	bUse = FALSE;	// ���� �ܿ� ����

	if( m_dwFlag&USE_1POINT )
	{
		m_spSingleGroup->m_bOut_Particle = bUse;

		if ( m_vBackUp != m_vTransUp )
		{
			m_vPrevUp = m_vBackUp;
			m_vBackUp = m_vTransUp;
		}


		D3DXMATRIX	matRotate;	

		D3DXMatrixRotationY( &matRotate, m_fTarDirection+(D3DX_PI*0.5f) );

		m_vTransUp = m_vTransUp - (m_vNormalUp*m_fDepthUp);
		D3DXMatrixTranslation( &m_matTrans, m_vTransUp.x, m_vTransUp.y, m_vTransUp.z );

		D3DXMatrixMultiply( &m_spSingleGroup->m_matWorld, &matRotate, &m_matTrans );

		m_spSingleGroup->m_fOut_Scale = m_fScale;				// ������

		m_spSingleGroup->Render( pd3dDevice, D3DXVECTOR3(1.f,1.f,1.f), 1.f );
	}
	else if( m_dwFlag&USE_2POINT )
	{
		m_spSingleGroup->m_bOut_Particle = bUse;

		if ( m_vBackUp != m_vTransUp )
		{
			m_vPrevUp = m_vBackUp;
			m_vBackUp = m_vTransUp;
		}

		m_spSingleGroup->m_vTargetPos	= m_vTransDown - (m_vNormalDown*m_fDepthDown);					// ��ǥ ��ġ
		m_spSingleGroup->m_vStartPos		= m_vTransUp - (m_vNormalUp*m_fDepthUp);							// ���� ��ġ
		m_spSingleGroup->m_vDirect		= m_spSingleGroup->m_vTargetPos - m_spSingleGroup->m_vStartPos;	// ����

		if( m_dwFlag&USE_REPEATMOVE )
		{
			m_vCurPos = m_spSingleGroup->m_vStartPos + ( m_spSingleGroup->m_vDirect * (sinf(m_fTime*m_fRepeatSpeed)+1.f)*0.5f);
		}
		else
		{
			if ( m_dwFlag&USE_AUTOMOVE )	m_vCurPos = m_spSingleGroup->m_vStartPos + ( m_spSingleGroup->m_vDirect * (m_fPlayTime*m_fRepeatSpeed));
			else							m_vCurPos = m_spSingleGroup->m_vStartPos;
		}

		m_spSingleGroup->m_matWorld._41 = m_vCurPos.x;
		m_spSingleGroup->m_matWorld._42 = m_vCurPos.y;
		m_spSingleGroup->m_matWorld._43 = m_vCurPos.z;

		if ( m_dwFlag&USE_AUTOMOVE )	m_spSingleGroup->m_bAutoMove = TRUE;
		else							m_spSingleGroup->m_bAutoMove = FALSE;

		m_spSingleGroup->m_bOut_Direct	= TRUE;			// ������� �־� �ش�.

		m_spSingleGroup->m_fOut_Scale	= m_fScale;		// ������

		DxBillboardLookAtDir( m_spSingleGroup->m_matOutRotate, &m_spSingleGroup->m_vDirect );			// �ܺ��� ��Ʈ����

		m_spSingleGroup->Render( pd3dDevice, D3DXVECTOR3(1.f,1.f,1.f), 1.f );
	}
	else if( m_dwFlag&USE_MATRIX )
	{
		//////////////////////////////////////////////////////////////////////////
		// ���� �ڵ� - ���߿� ���� �ø��� �� ���� ����.
		//				Matrix ������� ����� �ǵ��� ����.
		//////////////////////////////////////////////////////////////////////////
		//	��ũ�� �ٷ� ���� ������ ����.
		// ��ũ ������ �־ �����߾�����. ( 2014.4.23 )
		//////////////////////////////////////////////////////////////////////////
		// ��ũ ������ ��ġ�鼭 ��Ų���� Matrix�� ��Ʋ�ȴ�.
		// �������� �ǵ���. ( 2015.1.20 )
		//////////////////////////////////////////////////////////////////////////
		if ( m_nCalculationMatrixMode == 1 )
		{
			// �ܵ� ����Ʈ ���⼺�� ������ �ʿ��Ҷ� ���
			m_spSingleGroup->m_bOut_Particle = bUse;

			if ( m_vBackUp != m_vTransUp )
			{
				m_vPrevUp = m_vBackUp;
				m_vBackUp = m_vTransUp;
			}


			D3DXMATRIX matRotateLocal;
			D3DXMATRIX	matRotate, matRotateCombine ;
			D3DXMatrixIdentity( &matRotate );

			if ( m_pCharPart )
			{
				//if( !m_pCharPart->m_rMeshContainer->pSkinInfo )
				//{
				//	matRotateCombine = m_pCharPart->GetStaticLinkBone();

				//	D3DXMatrixRotationY( &matRotateLocal, m_fTarDirection+(D3DX_PI*0.5f) );
				//}
				//else
				//{
				//	matRotateCombine = m_matVertex;
				//	D3DXMatrixRotationY( &matRotate, m_fTarDirection+(D3DX_PI*0.5f) );
				//	D3DXMatrixMultiply( &matRotateCombine, &matRotate,  &matRotateCombine );

				//	// ��Ų�� ��ũ�� �ٸ��� ���۵Ǵٴ�...
				//	// ����.
				//	D3DXMatrixIdentity( &matRotateLocal );
				//}

				if( !m_pCharPart->m_rMeshContainer->pSkinInfo )
				{
					matRotateCombine = m_pCharPart->GetStaticLinkBone();
				}
				else
				{
					matRotateCombine = m_matVertex;
				}

				D3DXMatrixRotationY( &matRotate, m_fTarDirection+(D3DX_PI*0.5f) );
				D3DXMatrixMultiply( &matRotateCombine, &matRotate,  &matRotateCombine );

				D3DXMatrixIdentity( &matRotateLocal );

				matRotateCombine._41 = 0.0f;
				matRotateCombine._42 = 0.0f;
				matRotateCombine._43 = 0.0f;
			}
			else if ( m_pSkinPiece )
			{
				//if( m_pSkinPiece->m_pmcMesh && !m_pSkinPiece->m_pmcMesh->pSkinInfo )
				//{
				//	D3DXMatrixIdentity( &matRotateCombine );

				//	D3DXMatrixRotationY( &matRotateLocal, m_fTarDirection+(D3DX_PI*0.5f) );
				//}
				//else
				//{
				//	matRotateCombine = m_matVertex;
				//	D3DXMatrixRotationY( &matRotate, m_fTarDirection+(D3DX_PI*0.5f) );
				//	D3DXMatrixMultiply( &matRotateCombine, &matRotate,  &matRotateCombine );

				//	// ��Ų�� ��ũ�� �ٸ��� ���۵Ǵٴ�...
				//	// ����.
				//	D3DXMatrixIdentity( &matRotateLocal );
				//}

				if( m_pSkinPiece->m_pmcMesh && !m_pSkinPiece->m_pmcMesh->pSkinInfo )
				{
					D3DXMatrixIdentity( &matRotateCombine );
				}
				else
				{
					matRotateCombine = m_matVertex;
				}

				D3DXMatrixRotationY( &matRotate, m_fTarDirection+(D3DX_PI*0.5f) );
				D3DXMatrixMultiply( &matRotateCombine, &matRotate,  &matRotateCombine );

				D3DXMatrixIdentity( &matRotateLocal );

				matRotateCombine._41 = 0.0f;
				matRotateCombine._42 = 0.0f;
				matRotateCombine._43 = 0.0f;
			}

			m_vTransUp = m_vTransUp - (m_vNormalUp*m_fDepthUp);
			D3DXMatrixTranslation( &m_matTrans, m_vTransUp.x, m_vTransUp.y, m_vTransUp.z );

			D3DXMATRIX matWorld;
			D3DXMatrixMultiply( &matWorld, &matRotateCombine, &m_matTrans );
			D3DXMatrixMultiply( &matWorld, &m_matBase, &matWorld );
			D3DXMatrixMultiply( &m_spSingleGroup->m_matWorld, &matRotateLocal, &matWorld );

			m_spSingleGroup->m_fOut_Scale = m_fScale;				// ������

			m_spSingleGroup->Render( pd3dDevice, D3DXVECTOR3(1.f,1.f,1.f), 1.f );
		}
		else
		{
			// �ܵ� ����Ʈ ���⼺�� ������ �ʿ��Ҷ� ���
			m_spSingleGroup->m_bOut_Particle = bUse;

			if ( m_vBackUp != m_vTransUp )
			{
				m_vPrevUp = m_vBackUp;
				m_vBackUp = m_vTransUp;
			}


			D3DXMATRIX	matRotate, matRotateCombine ;
			D3DXMatrixIdentity( &matRotate );

			if ( m_pCharPart )
			{
				if( !m_pCharPart->m_rMeshContainer->pSkinInfo )
					matRotateCombine = m_pCharPart->GetStaticLinkBone();
				else
					matRotateCombine = m_matVertex;

				matRotateCombine._41 = 0.0f;
				matRotateCombine._42 = 0.0f;
				matRotateCombine._43 = 0.0f;
			}
			else if ( m_pSkinPiece )
			{
				if( m_pSkinPiece->m_pmcMesh && !m_pSkinPiece->m_pmcMesh->pSkinInfo )
				{
					D3DXMatrixIdentity( &matRotateCombine );
				}
				else
				{
					matRotateCombine = m_matVertex;
				}

				matRotateCombine._41 = 0.0f;
				matRotateCombine._42 = 0.0f;
				matRotateCombine._43 = 0.0f;
			}

			D3DXMATRIX matRotateLocal;
			D3DXMatrixRotationY( &matRotateLocal, m_fTarDirection+(D3DX_PI*0.5f) );

			m_vTransUp = m_vTransUp - (m_vNormalUp*m_fDepthUp);
			D3DXMatrixTranslation( &m_matTrans, m_vTransUp.x, m_vTransUp.y, m_vTransUp.z );

			D3DXMATRIX matWorld;
			D3DXMatrixMultiply( &matWorld, &matRotateCombine, &m_matTrans );
			D3DXMatrixMultiply( &matWorld, &m_matBase, &matWorld );
			D3DXMatrixMultiply( &m_spSingleGroup->m_matWorld, &matRotateLocal, &matWorld );

			m_spSingleGroup->m_fOut_Scale = m_fScale;				// ������

			m_spSingleGroup->Render( pd3dDevice, D3DXVECTOR3(1.f,1.f,1.f), 1.f );
		}

		

		//
		//////////////////////////////////////////////////////////////////////////
		// ���� �ڵ� - ���߿� ���� �ø��� �� ���� ����.
		//				Matrix ������� ����� �ǵ��� ����.
		//////////////////////////////////////////////////////////////////////////
	}

	m_vBackUp = m_vTransUp;
}

void	DxEffCharSingle::RenderEff( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* pMatrix, const float fScale )
{
	PrecalculateBeforeRenderEff();
	OnlyRenderEff( pd3dDevice );
}

BOOL	DxEffCharSingle::SetWeather ()
{
	if ( m_nWeather == 1 )		// ������
	{
		if ( DxWeatherMan::GetInstance()->GetRain()->UseRain() ) return FALSE;
	}
	else if ( m_nWeather == 2 )		// ����³�
	{
		if ( !DxWeatherMan::GetInstance()->GetRain()->UseRain() ) return FALSE;
	}

	return TRUE;
}

BOOL	DxEffCharSingle::SetTime ()
{
	if ( GLPeriod::GetInstance().IsOffLight() || DxLightMan::GetInstance()->IsNightAndDay() == FALSE)	// ��
	{
		if ( m_dwFlag & USE_NOT_TIMEDAY )	return FALSE;
		else							    return TRUE;
	}
	else									// ��
	{
		if ( m_dwFlag & USE_NOT_TIMENIGHT )	return FALSE;
		else							    return TRUE;
	}

	return TRUE;
}

void DxEffCharSingle::ConvertTracePoint()
{
	if( strcmp( m_szTrace, _T("ENERGY02") ) == 0 )
	{
		StringCchCopy( m_szTrace, STRACE_NSIZE, _T("ENERGY01") );
	}
	if( strcmp( m_szTraceBack, _T("ENERGY02") ) == 0 )
	{
		StringCchCopy( m_szTraceBack, STRACE_NSIZE, _T("ENERGY01") );
	}

	if( strcmp( m_szTrace, _T("IMAGE02") ) == 0 )
	{
		StringCchCopy( m_szTrace, STRACE_NSIZE, _T("IMAGE01") );
	}
	if( strcmp( m_szTraceBack, _T("IMAGE02") ) == 0 )
	{
		StringCchCopy( m_szTraceBack, STRACE_NSIZE, _T("IMAGE01") );
	}
}

HRESULT DxEffCharSingle::LoadFile_0100 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{

	EFFCHAR_PROPERTY_SINGLE_100		Property;

	//	Note : ����Ʈ�� Property �� ����.
	//
	SFile.ReadBuffer ( &Property, sizeof(EFFCHAR_PROPERTY_SINGLE_100) );

	m_nWeather	= Property.m_nWeather;						//	����..
	m_nAniMType = 0;					//	���� Ÿ��
	m_fScale = 1.f;

	//	Ver.103 ���� �߰�
	m_fDepthUp		= 0.5f;
	m_fDepthDown	= 0.5f;

	//	Ver.104 ���� �߰�
	m_dwFlag		= 0L;
	m_fCoolTime		= 10.f;

	//	Ver.105 ���� �߰�
	m_fRepeatSpeed	= 1.f;
	if ( Property.m_nType )	
	{
		m_dwFlag |= USE_2POINT;
		m_dwFlag &= ~USE_1POINT;

		m_dwFlag |= USE_AUTOMOVE;
	}
	else
	{
		m_dwFlag |= USE_1POINT;
		m_dwFlag &= ~USE_2POINT;
	}

	StringCchCopy(m_szFileName,		MAX_PATH, Property.m_szFileName);	
	StringCchCopy(m_szTrace,		STRACE_NSIZE, Property.m_szTrace);	
	StringCchCopy(m_szTraceBack,	STRACE_NSIZE, Property.m_szTraceBack);

	return S_OK;
}

HRESULT DxEffCharSingle::LoadFile_0101 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	EFFCHAR_PROPERTY_SINGLE_101		Property;

	//	Note : ����Ʈ�� Property �� ����.
	//
	SFile.ReadBuffer ( &Property, sizeof(EFFCHAR_PROPERTY_SINGLE_101) );

	m_nWeather	= Property.m_nWeather;						//	����..
	m_nAniMType = 0;					//	���� Ÿ��
	m_fScale = 1.f;

	//	Ver.103 ���� �߰�
	m_fDepthUp		= 0.5f;
	m_fDepthDown	= 0.5f;

	//	Ver.104 ���� �߰�
	m_dwFlag		= 0L;
	m_fCoolTime		= 10.f;

	//	Ver.105 ���� �߰�
	m_fRepeatSpeed	= 1.f;
	if ( Property.m_nType )	
	{
		m_dwFlag |= USE_2POINT;
		m_dwFlag &= ~USE_1POINT;

		m_dwFlag |= USE_AUTOMOVE;
	}
	else
	{
		m_dwFlag |= USE_1POINT;
		m_dwFlag &= ~USE_2POINT;
	}

	StringCchCopy(m_szFileName,		MAX_PATH, Property.m_szFileName);	
	StringCchCopy(m_szTrace,		STRACE_NSIZE, Property.m_szTrace);	
	StringCchCopy(m_szTraceBack,	STRACE_NSIZE, Property.m_szTraceBack);

	return S_OK;
}

HRESULT DxEffCharSingle::LoadFile_0102 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	EFFCHAR_PROPERTY_SINGLE_102		Property;

	//	Note : ����Ʈ�� Property �� ����.
	//
	SFile.ReadBuffer ( &Property, sizeof(EFFCHAR_PROPERTY_SINGLE_102) );

	m_dwPlaySlngle	= Property.m_dwPlaySlngle;
	m_nAniMType		= Property.m_nAniMType;
	m_nWeather		= Property.m_nWeather;
	m_fScale		= Property.m_fScale;

	//	Ver.103 ���� �߰�
	m_fDepthUp		= 0.5f;
	m_fDepthDown	= 0.5f;

	//	Ver.104 ���� �߰�
	m_dwFlag		= 0L;
	m_fCoolTime		= 10.f;

	//	Ver.105 ���� �߰�
	m_fRepeatSpeed	= 1.f;
	if ( Property.m_nType )	
	{
		m_dwFlag |= USE_2POINT;
		m_dwFlag &= ~USE_1POINT;

		m_dwFlag |= USE_AUTOMOVE;
	}
	else
	{
		m_dwFlag |= USE_1POINT;
		m_dwFlag &= ~USE_2POINT;
	}

	StringCchCopy(m_szFileName,		MAX_PATH, Property.m_szFileName);	
	StringCchCopy(m_szTrace,		STRACE_NSIZE, Property.m_szTrace);	
	StringCchCopy(m_szTraceBack,	STRACE_NSIZE, Property.m_szTraceBack);

	return S_OK;
}

HRESULT DxEffCharSingle::LoadFile_0103 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	EFFCHAR_PROPERTY_SINGLE_103 	Property;

	//	Note : ����Ʈ�� Property �� ����.
	//
	SFile.ReadBuffer ( &Property, sizeof(EFFCHAR_PROPERTY_SINGLE_103) );

	m_dwPlaySlngle	= Property.m_dwPlaySlngle;
	m_nAniMType		= Property.m_nAniMType;
	m_nWeather		= Property.m_nWeather;
	m_fScale		= Property.m_fScale;

	//	Ver.103 ���� �߰�
	m_fDepthUp		= Property.m_fDepthUp;
	m_fDepthDown	= Property.m_fDepthDown;

	//	Ver.104 ���� �߰�
	m_dwFlag		= 0L;
	m_fCoolTime		= 10.f;

	//	Ver.105 ���� �߰�
	m_fRepeatSpeed	= 1.f;
	if ( Property.m_nType )	
	{
		m_dwFlag |= USE_2POINT;
		m_dwFlag &= ~USE_1POINT;

		m_dwFlag |= USE_AUTOMOVE;
	}
	else
	{
		m_dwFlag |= USE_1POINT;
		m_dwFlag &= ~USE_2POINT;
	}

	StringCchCopy(m_szFileName,		MAX_PATH, Property.m_szFileName);	
	StringCchCopy(m_szTrace,		STRACE_NSIZE, Property.m_szTrace);	
	StringCchCopy(m_szTraceBack,	STRACE_NSIZE, Property.m_szTraceBack);

	return S_OK;
}

HRESULT DxEffCharSingle::LoadFile_0104 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	EFFCHAR_PROPERTY_SINGLE_104 	Property;

	//	Note : ����Ʈ�� Property �� ����.
	//
	SFile.ReadBuffer ( &Property, sizeof(EFFCHAR_PROPERTY_SINGLE_104) );

	m_dwPlaySlngle	= Property.m_dwPlaySlngle;
	m_nAniMType		= Property.m_nAniMType;
	m_nWeather		= Property.m_nWeather;
	m_fScale		= Property.m_fScale;

	//	Ver.103 ���� �߰�
	m_fDepthUp		= Property.m_fDepthUp;
	m_fDepthDown	= Property.m_fDepthDown;

	//	Ver.104 ���� �߰�
	m_dwFlag		= Property.m_dwFlag;
	m_fCoolTime		= Property.m_fCoolTime;

	//	Ver.105 ���� �߰�
	m_fRepeatSpeed	= 1.f;
	if ( Property.m_nType )	
	{
		m_dwFlag |= USE_2POINT;
		m_dwFlag &= ~USE_1POINT;

		m_dwFlag |= USE_AUTOMOVE;
	}
	else
	{
		m_dwFlag |= USE_1POINT;
		m_dwFlag &= ~USE_2POINT;
	}

	StringCchCopy(m_szFileName,		MAX_PATH, Property.m_szFileName);	
	StringCchCopy(m_szTrace,		STRACE_NSIZE, Property.m_szTrace);	
	StringCchCopy(m_szTraceBack,	STRACE_NSIZE, Property.m_szTraceBack);

	return S_OK;
}

HRESULT DxEffCharSingle::LoadFile_0105 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : ����Ʈ�� Property �� ����.
	//
	EFFCHAR_PROPERTY_SINGLE_105 Property;

	SFile.ReadBuffer ( &Property, sizeof(EFFCHAR_PROPERTY_SINGLE_105) );

	m_Property.Assign( Property );

	return S_OK;
}

HRESULT DxEffCharSingle::LoadFile_0106 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
    //	Note : ����Ʈ�� Property �� ����.
    //
    EFFCHAR_PROPERTY_SINGLE_106 Property;

    SFile.ReadBuffer( &Property, sizeof(EFFCHAR_PROPERTY_SINGLE_106) );

    m_Property.Assign( Property );

    return S_OK;
}

HRESULT DxEffCharSingle::LoadFile_0107 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : ����Ʈ�� Property �� ����.
	//
	SFile.ReadBuffer( &m_Property, sizeof(m_Property) );

	return S_OK;
}

HRESULT DxEffCharSingle::LoadFile_0108 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : ����Ʈ�� Property �� ����.
	//
	SFile.ReadBuffer( &m_Property, sizeof(m_Property) );

	SFile >> m_nCalculationMatrixMode;

	return S_OK;
}

HRESULT	DxEffCharSingle::LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	switch ( dwVer )
	{
	case 0x0100:	LoadFile_0100( SFile, pd3dDevice );	break;
	case 0x0101:	LoadFile_0101( SFile, pd3dDevice );	break;
	case 0x0102:	LoadFile_0102( SFile, pd3dDevice );	break;
	case 0x0103:	LoadFile_0103( SFile, pd3dDevice );	break;
	case 0x0104:	LoadFile_0104( SFile, pd3dDevice );	break;
	case 0x0105:	LoadFile_0105( SFile, pd3dDevice );	break;
    case 0x0106:	LoadFile_0106( SFile, pd3dDevice );	break;
	case 0x0107:	LoadFile_0107( SFile, pd3dDevice );	break;
	case 0x0108:	LoadFile_0108( SFile, pd3dDevice );	break;
	default:
		{
			//	Note : ������ Ʋ�� ��쿡�� ���Ͽ� ������ DATA������ �Ǵ� ��� �۾��� ����.
			//
			SFile.SetOffSet ( SFile.GetfTell()+dwSize );

			return E_FAIL;
		}
		break;
	};

	//	Note : Device �ڿ��� �����Ѵ�.
	//
	hr = Create ( pd3dDevice );
	if ( FAILED(hr) )	return hr;

	return S_OK;
}

void DxEffCharSingle::SetCharSingleHellMatrixMode( int nMode )
{
	// DxSkinPiece ���� �ε��� ������ �Ǿ�����. �ֽŹ����� ȣ����� �ʴ´�.
	//
	// [2014.4.23] ~ [2014.8.4] ���̿� ���õȰ� ��¿�� ����. �� �������־�� �Ѵ�.
	//
	// m_nCalculationMatrixMode �� ���� ���� �Ǿ��ٴ� �� 0x0108 �������� ����Ǿ��ܰ� �ǹ��ϰ�
	// ��� Ư�� �������� ����ϰڴٴ� �� �ǹ���.
	// [2014.8.4]������ m_nCalculationMatrixMode == 1  ��.
	// [2014.8.4] ~ [2015.1.19]�� m_nCalculationMatrixMode == 2  ��.
	// [2015.1.19] ���Ĵ� 0 ��.
	m_nCalculationMatrixMode = nMode;
}

HRESULT	DxEffCharSingle::SaveFile ( sc::BaseStream &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	SFile.BeginBlock();

	//	Note : ����Ʈ�� Property �� ����.
	//
	SFile.WriteBuffer ( &m_Property, sizeof(m_Property) );

	// �̰� �ϴ� ������ ������ ������ ���� 1, Ȥ�� 2 ���� �۵���.
	// ���� �̰� ��� ������� ���� ���� ������.
	// 1 ���� ������, Save �� Load �ϸ� SetCharSingleHellMatrixMode �� �޾� �� �� 2 �� ���۵Ǳ� ������ ������ �����.
	// �׷��� Matrix ����带 �����ϵ��� �Ѵ�.
	SFile << m_nCalculationMatrixMode;

	SFile.EndBlock();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// Material System �� ���� ȿ������ ���̱� ���� �۾�.
void DxEffCharSingle::PrecalculateBeforeRenderEff( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* pMatrix )
{
	PrecalculateBeforeRenderEff();
}

void DxEffCharSingle::RenderEffForMaterialSystem( LPDIRECT3DDEVICEQ pd3dDevice, 
												 const D3DXMATRIX* pMatrix, 
												 const float fScale )
{
	OnlyRenderEff( pd3dDevice );
}
