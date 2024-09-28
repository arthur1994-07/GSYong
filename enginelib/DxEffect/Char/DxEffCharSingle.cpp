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
char		DxEffCharSingle::NAME[MAX_PATH]		= "4.단독 이펙트";

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

	// 이것 때문에 최악이다. ㅠㅠ. 코드를 더럽히고 있다.
	pEffCharSingle->m_nCalculationMatrixMode = m_nCalculationMatrixMode;

	hr = pEffCharSingle->Create ( pd3dDevice );
	if ( FAILED(hr) )
	{
		SAFE_DELETE(pEffCharSingle);
		return NULL;
	}

	return pEffCharSingle;
}

// DxEffSinglePropGMan를 ThreadLoading 하도록 바꾸었기 때문에 DxSingleGroup가 로딩안되었으면 로딩을 시도한다.
void DxEffCharSingle::CreateDxEffSingleGroup()
{
	if ( m_spSingleGroup.get() )
		return;

	if ( m_sPropGroupRes.Update() )	
	{	
		m_spSingleGroup.reset();						// 이전 데이터는 지워준다.
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

	// 이름이 있으면 작업함.
	if ( strlen(m_szFileName) )
	{
		m_sPropGroupRes = DxEffSinglePropGMan::GetInstance().LoadEffectGProp ( m_szFileName, TRUE, FALSE );

		// DxEffSinglePropGMan를 ThreadLoading 하도록 바꾸었기 때문에 DxSingleGroup가 로딩안되었으면 로딩을 시도한다.
		CreateDxEffSingleGroup();
	}

	//	Note : Eff Group의 매트릭스 트랜스폼 지정.
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

	//	m_spSingleGroup.reset();						// 이전 데이터는 지워준다.
	//	m_spSingleGroup = m_pPropGroup->NEWEFFGROUP ();
	//	if ( m_spSingleGroup.get() == NULL )
 //           return E_FAIL;

	//	//	Note : Eff Group Create Dev.
	//	//
	//	m_spSingleGroup->Create ( pd3dDevice );

	//	//	Note : Eff Group의 매트릭스 트랜스폼 지정.
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

	// 에디터에서는 계속 나오게한다.
	if( !g_bCHAR_EDIT_RUN )
	{
		if ( !SetTime() )			return S_OK;
		if ( !SetWeather() )		return S_OK;
	}

	m_fTime = fTime;
	m_fPlayTime += fElapsedTime;

	// DxEffSinglePropGMan를 ThreadLoading 하도록 바꾸었기 때문에 DxSingleGroup가 로딩안되었으면 로딩을 시도한다.
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

	// 에디터에서는 계속 나오게한다.
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

	// 에디터에서는 계속 나오게한다.
	if( !g_bCHAR_EDIT_RUN )
	{
		if ( !SetTime() )			return;
		if ( !SetWeather() )		return;
	}

	BOOL bUse = TRUE;
	if ( (m_nAniMType==1) && (m_CurAniMType!=AN_ATTACK) )		bUse = FALSE;		// 공격시 쓰임
	else if ( (m_nAniMType==2) && (m_CurAniMType==AN_ATTACK) )	bUse = FALSE;	// 공격 외에 쓰임

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

		m_spSingleGroup->m_fOut_Scale = m_fScale;				// 스케일

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

		m_spSingleGroup->m_vTargetPos	= m_vTransDown - (m_vNormalDown*m_fDepthDown);					// 목표 위치
		m_spSingleGroup->m_vStartPos		= m_vTransUp - (m_vNormalUp*m_fDepthUp);							// 시작 위치
		m_spSingleGroup->m_vDirect		= m_spSingleGroup->m_vTargetPos - m_spSingleGroup->m_vStartPos;	// 방향

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

		m_spSingleGroup->m_bOut_Direct	= TRUE;			// 방향까지 넣어 준다.

		m_spSingleGroup->m_fOut_Scale	= m_fScale;		// 스케일

		DxBillboardLookAtDir( m_spSingleGroup->m_matOutRotate, &m_spSingleGroup->m_vDirect );			// 외부의 매트릭스

		m_spSingleGroup->Render( pd3dDevice, D3DXVECTOR3(1.f,1.f,1.f), 1.f );
	}
	else if( m_dwFlag&USE_MATRIX )
	{
		//////////////////////////////////////////////////////////////////////////
		// 개판 코드 - 나중에 버젼 올리고 재 정리 하자.
		//				Matrix 연산부터 제대로 되도록 하자.
		//////////////////////////////////////////////////////////////////////////
		//	링크는 바로 이전 버젼과 동일.
		// 링크 문제가 있어서 수정했었었다. ( 2014.4.23 )
		//////////////////////////////////////////////////////////////////////////
		// 링크 문제를 고치면서 스킨쪽의 Matrix가 뒤틀렸다.
		// 이전으로 되돌림. ( 2015.1.20 )
		//////////////////////////////////////////////////////////////////////////
		if ( m_nCalculationMatrixMode == 1 )
		{
			// 단독 이펙트 방향성을 지정이 필요할때 사용
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

				//	// 스킨과 링크가 다르게 동작되다니...
				//	// 망함.
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

				//	// 스킨과 링크가 다르게 동작되다니...
				//	// 망함.
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

			m_spSingleGroup->m_fOut_Scale = m_fScale;				// 스케일

			m_spSingleGroup->Render( pd3dDevice, D3DXVECTOR3(1.f,1.f,1.f), 1.f );
		}
		else
		{
			// 단독 이펙트 방향성을 지정이 필요할때 사용
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

			m_spSingleGroup->m_fOut_Scale = m_fScale;				// 스케일

			m_spSingleGroup->Render( pd3dDevice, D3DXVECTOR3(1.f,1.f,1.f), 1.f );
		}

		

		//
		//////////////////////////////////////////////////////////////////////////
		// 개판 코드 - 나중에 버젼 올리고 재 정리 하자.
		//				Matrix 연산부터 제대로 되도록 하자.
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
	if ( m_nWeather == 1 )		// 맑은날
	{
		if ( DxWeatherMan::GetInstance()->GetRain()->UseRain() ) return FALSE;
	}
	else if ( m_nWeather == 2 )		// 비오는날
	{
		if ( !DxWeatherMan::GetInstance()->GetRain()->UseRain() ) return FALSE;
	}

	return TRUE;
}

BOOL	DxEffCharSingle::SetTime ()
{
	if ( GLPeriod::GetInstance().IsOffLight() || DxLightMan::GetInstance()->IsNightAndDay() == FALSE)	// 낮
	{
		if ( m_dwFlag & USE_NOT_TIMEDAY )	return FALSE;
		else							    return TRUE;
	}
	else									// 밤
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

	//	Note : 이팩트의 Property 를 저장.
	//
	SFile.ReadBuffer ( &Property, sizeof(EFFCHAR_PROPERTY_SINGLE_100) );

	m_nWeather	= Property.m_nWeather;						//	날씨..
	m_nAniMType = 0;					//	공격 타임
	m_fScale = 1.f;

	//	Ver.103 에서 추가
	m_fDepthUp		= 0.5f;
	m_fDepthDown	= 0.5f;

	//	Ver.104 에서 추가
	m_dwFlag		= 0L;
	m_fCoolTime		= 10.f;

	//	Ver.105 에서 추가
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

	//	Note : 이팩트의 Property 를 저장.
	//
	SFile.ReadBuffer ( &Property, sizeof(EFFCHAR_PROPERTY_SINGLE_101) );

	m_nWeather	= Property.m_nWeather;						//	날씨..
	m_nAniMType = 0;					//	공격 타임
	m_fScale = 1.f;

	//	Ver.103 에서 추가
	m_fDepthUp		= 0.5f;
	m_fDepthDown	= 0.5f;

	//	Ver.104 에서 추가
	m_dwFlag		= 0L;
	m_fCoolTime		= 10.f;

	//	Ver.105 에서 추가
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

	//	Note : 이팩트의 Property 를 저장.
	//
	SFile.ReadBuffer ( &Property, sizeof(EFFCHAR_PROPERTY_SINGLE_102) );

	m_dwPlaySlngle	= Property.m_dwPlaySlngle;
	m_nAniMType		= Property.m_nAniMType;
	m_nWeather		= Property.m_nWeather;
	m_fScale		= Property.m_fScale;

	//	Ver.103 에서 추가
	m_fDepthUp		= 0.5f;
	m_fDepthDown	= 0.5f;

	//	Ver.104 에서 추가
	m_dwFlag		= 0L;
	m_fCoolTime		= 10.f;

	//	Ver.105 에서 추가
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

	//	Note : 이팩트의 Property 를 저장.
	//
	SFile.ReadBuffer ( &Property, sizeof(EFFCHAR_PROPERTY_SINGLE_103) );

	m_dwPlaySlngle	= Property.m_dwPlaySlngle;
	m_nAniMType		= Property.m_nAniMType;
	m_nWeather		= Property.m_nWeather;
	m_fScale		= Property.m_fScale;

	//	Ver.103 에서 추가
	m_fDepthUp		= Property.m_fDepthUp;
	m_fDepthDown	= Property.m_fDepthDown;

	//	Ver.104 에서 추가
	m_dwFlag		= 0L;
	m_fCoolTime		= 10.f;

	//	Ver.105 에서 추가
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

	//	Note : 이팩트의 Property 를 저장.
	//
	SFile.ReadBuffer ( &Property, sizeof(EFFCHAR_PROPERTY_SINGLE_104) );

	m_dwPlaySlngle	= Property.m_dwPlaySlngle;
	m_nAniMType		= Property.m_nAniMType;
	m_nWeather		= Property.m_nWeather;
	m_fScale		= Property.m_fScale;

	//	Ver.103 에서 추가
	m_fDepthUp		= Property.m_fDepthUp;
	m_fDepthDown	= Property.m_fDepthDown;

	//	Ver.104 에서 추가
	m_dwFlag		= Property.m_dwFlag;
	m_fCoolTime		= Property.m_fCoolTime;

	//	Ver.105 에서 추가
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
	//	Note : 이팩트의 Property 를 저장.
	//
	EFFCHAR_PROPERTY_SINGLE_105 Property;

	SFile.ReadBuffer ( &Property, sizeof(EFFCHAR_PROPERTY_SINGLE_105) );

	m_Property.Assign( Property );

	return S_OK;
}

HRESULT DxEffCharSingle::LoadFile_0106 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
    //	Note : 이팩트의 Property 를 저장.
    //
    EFFCHAR_PROPERTY_SINGLE_106 Property;

    SFile.ReadBuffer( &Property, sizeof(EFFCHAR_PROPERTY_SINGLE_106) );

    m_Property.Assign( Property );

    return S_OK;
}

HRESULT DxEffCharSingle::LoadFile_0107 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 이팩트의 Property 를 저장.
	//
	SFile.ReadBuffer( &m_Property, sizeof(m_Property) );

	return S_OK;
}

HRESULT DxEffCharSingle::LoadFile_0108 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 이팩트의 Property 를 저장.
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
			//	Note : 버전이 틀릴 경우에는 파일에 쓰여진 DATA영역을 건더 띄는 작업을 진행.
			//
			SFile.SetOffSet ( SFile.GetfTell()+dwSize );

			return E_FAIL;
		}
		break;
	};

	//	Note : Device 자원을 생성한다.
	//
	hr = Create ( pd3dDevice );
	if ( FAILED(hr) )	return hr;

	return S_OK;
}

void DxEffCharSingle::SetCharSingleHellMatrixMode( int nMode )
{
	// DxSkinPiece 에서 로딩시 셋팅이 되어진다. 최신버젼은 호출되지 않는다.
	//
	// [2014.4.23] ~ [2014.8.4] 사이에 셋팅된건 어쩔수 없다. 재 수정해주어야 한다.
	//
	// m_nCalculationMatrixMode 에 값이 셋팅 되었다는 건 0x0108 버젼으로 저장되었단걸 의미하고
	// 어떠한 특정 버젼으로 사용하겠다는 걸 의미함.
	// [2014.8.4]이전은 m_nCalculationMatrixMode == 1  임.
	// [2014.8.4] ~ [2015.1.19]는 m_nCalculationMatrixMode == 2  임.
	// [2015.1.19] 이후는 0 임.
	m_nCalculationMatrixMode = nMode;
}

HRESULT	DxEffCharSingle::SaveFile ( sc::BaseStream &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	SFile.BeginBlock();

	//	Note : 이팩트의 Property 를 저장.
	//
	SFile.WriteBuffer ( &m_Property, sizeof(m_Property) );

	// 이걸 하는 이유는 이전에 셋팅한 것은 1, 혹은 2 모드로 작동됨.
	// 툴로 이걸 열어서 재셋팅을 했을 때가 문제임.
	// 1 모드로 되지만, Save 후 Load 하면 SetCharSingleHellMatrixMode 로 받아 올 때 2 로 동작되기 때문에 문제가 생긴다.
	// 그래서 Matrix 계산모드를 저장하도록 한다.
	SFile << m_nCalculationMatrixMode;

	SFile.EndBlock();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// Material System 에 현재 효과들을 붙이기 위한 작업.
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
