#include "pch.h"

#include "../../DxTools/DxDynamicVB.h"
#include "../../DxTools/DxSurfaceTex.h"
#include "../../DxTools/DxEnvironment.h"
#include "../../DxEffect/Frame/DxEffectMan.h"

#include "../../DxEffect/Single/DxEffSingle.h"
#include "../Single/DxEffSinglePropGMan.h"

#include "../../../SigmaCore/File/SerialFile.h"
#include "../../../SigmaCore/Log/LogMan.h"

#include "../../DxMeshs/SkinMesh/DxCharPart.h"
#include "../../DxMeshs/SkinMesh/DxSkinPieceContainer.h"

#include "./DxEffCharBlur.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

DWORD		DxEffCharBlur::TYPEID			= EMEFFCHAR_BLUR;
const DWORD	DxEffCharBlur::VERSION			= 0x0106;
char		DxEffCharBlur::NAME[MAX_PATH]	= "4.ĳ���� �ܻ�";

LPDIRECT3DSTATEBLOCK9		DxEffCharBlur::m_pSavedStateBlock	= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffCharBlur::m_pEffectStateBlock	= NULL;

DxEffCharBlur::DxEffCharBlur(void) :
	DxEffChar(),
	m_dwPrevKeyFrame(0),
	m_dwKeyPointCOUNT(0),
	m_bBaseBlurRender(FALSE),

	m_fTime(0.f),
	m_fElapsedTime(0.0f)
{
	m_fFullLife = 0.f;

	m_vColorUP = D3DCOLOR_ARGB(255,255,255,255);
	m_vColorDOWN = D3DCOLOR_ARGB(0,255,255,255);

	m_fAlpha[0] = 1.f;
	m_fAlpha[1] = 0.f;

	m_dwColor[0] = 0xffffff;
	m_dwColor[1] = 0xffffff;

	D3DXMatrixIdentity( &m_matIdentity );

	memset( m_szTraceUP, 0, sizeof(char)*STRACE_NSIZE );
	memset( m_szTraceDOWN, 0, sizeof(char)*STRACE_NSIZE );
}

DxEffCharBlur::~DxEffCharBlur(void)
{
	// Note : Key Data�� clear �Ѵ�.
	ClearKeyFrame();

	// Note : Blur Data�� clear �Ѵ�.
	ClearBlurDara();

	CleanUp ();
}

DxEffChar* DxEffCharBlur::CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DWORD dwMaterialNum )
{
	HRESULT hr;
	DxEffCharBlur *pEffChar = new DxEffCharBlur;
	pEffChar->SetFileName ( GetFileName() );
	pEffChar->SetLinkObj ( pCharPart, pSkinPiece );
	pEffChar->SetProperty ( &m_Property );

	hr = pEffChar->Create ( pd3dDevice );
	if ( FAILED(hr) )
	{
		SAFE_DELETE(pEffChar);
		return NULL;
	}

	return pEffChar;
}

HRESULT DxEffCharBlur::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{	
	D3DCAPSQ	d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		// Note : Base ��
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0x40 );

		//	Note : SetRenderState() ����
		pd3dDevice->SetRenderState ( D3DRS_CULLMODE, D3DCULL_NONE );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE, FALSE );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING, FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE, TRUE );

		//	Note : SetTextureStageState() ����
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2,	D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedStateBlock );
		else			pd3dDevice->EndStateBlock( &m_pEffectStateBlock );
	}

	return S_OK;
}

HRESULT DxEffCharBlur::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSavedStateBlock );
	SAFE_RELEASE( m_pEffectStateBlock );
	

	return S_OK;
}

void DxEffCharBlur::OnCreateDevice_STATIC()
{
}

void DxEffCharBlur::OnDestroyDevice_STATIC()
{
}

HRESULT DxEffCharBlur::OneTimeSceneInit ()
{

	return S_OK;
}

HRESULT DxEffCharBlur::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	D3DXVECTOR3	vNormal;

	m_fFullLife = m_fFullLifeTemp;
	m_vColorUP = m_vColorUPTemp;
	m_vColorDOWN = m_vColorDOWNTemp;

	m_fOneDiveFullLife = 1.f / m_fFullLifeTemp;

	m_fAlpha[0] = ((DWORD)(m_vColorUP & 0xff000000) >> 24) / 255.f;;
	m_fAlpha[1] = ((DWORD)(m_vColorDOWN & 0xff000000) >> 24) / 255.f;;
	m_dwColor[0] = m_vColorUP & 0xffffff;
	m_dwColor[1] = m_vColorDOWN & 0xffffff;

	//	Note : Eff Group�� ��Ʈ���� Ʈ������ ����.
	if ( m_szTraceUP[0] == NULL )	return E_FAIL;
	if ( m_szTraceDOWN[0] == NULL )	return E_FAIL;

    m_textureRes = TextureManager::GetInstance().LoadTexture(
        m_szTexture,
        false,
        TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
        0,
        D3DFMT_UNKNOWN,
        TEXTURE_RESIZE_NONE,
        TEXTURE_CHAR,
		true,
		FALSE );

	return S_OK;
}

HRESULT DxEffCharBlur::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	return S_OK;
}

HRESULT DxEffCharBlur::InvalidateDeviceObjects ()
{
	m_pd3dDevice = NULL;

	// Note : Key Data�� clear �Ѵ�.
	ClearKeyFrame();

	// Note : Blur Data�� clear �Ѵ�.
	ClearBlurDara();

	return S_OK;
}

HRESULT DxEffCharBlur::DeleteDeviceObjects ()
{
	m_textureRes.Clear();

	return S_OK;
}

HRESULT DxEffCharBlur::FinalCleanup ()
{

	return S_OK;
}

WORD ConvertRGBToWORD( int wR, int wG, int wB )
{
	// �����
	int wSum;
	wR = wR >> 3;
	wG = wG >> 3;
	wB = wB >> 3;
	wSum = (wR<<10) + (wG<<5) + wB;

	//CDebugSet::ToLogFile( "%d", wSum );
	//CDebugSet::ToLogFile( " %d, %d, %d", wR, wG, wB );

	// Ǯ���
	wR = wSum >> 10;
	wG = wSum >> 5;
	wG = wG&0x1f;
	wB = wSum&0x1f;

	//CDebugSet::ToLogFile( " %d, %d, %d", wR, wG, wB );
	
	wR = (WORD)( wR * 8.225806f );	//<< 3;
	wG = (WORD)( wG * 8.225806f );	//<< 3;
	wB = (WORD)( wB * 8.225806f );	//<< 3;

	//CDebugSet::ToLogFile( " %d, %d, %d \n", wR, wG, wB );

	return 1;
}

HRESULT DxEffCharBlur::FrameMove ( float fTime, float fElapsedTime )
{
	HRESULT hr = S_OK;

	m_fTime += fElapsedTime;
	m_fElapsedTime += fElapsedTime;

	// Note ; ��ų�� �پ� ���� ��� ��ġ ������ �ȴ�.
	if( m_dwFlag&USEALLACTION )
	{
		if ( m_fTime > 4.5f )	SetEnd();
	}

	// Note : ���� ������ �Ǿ� ���� ��� ���� �����Ѵ�.
	//			�ʿ��ϴٸ� ���ϰ� �� �� ������.... ��.
	if( (m_dwFlag & USEREFRACT) && DxSurfaceTex::GetInstance().IsEnable() )
	{
		m_fFullLife = 0.6f;
		m_vColorUP = 0xffffffff;
		m_vColorDOWN = 0xffffffff;

		m_fOneDiveFullLife = 1.f / 0.6f;

		m_fAlpha[0] = ((DWORD)(0xff000000) >> 24) / 255.f;;
		m_fAlpha[1] = ((DWORD)(0xff000000) >> 24) / 255.f;;
		m_dwColor[0] = 0xffffff;
		m_dwColor[1] = 0xffffff;
	}
	else
	{
		m_fFullLife = m_fFullLifeTemp;
		m_vColorUP = m_vColorUPTemp;
		m_vColorDOWN = m_vColorDOWNTemp;

		m_fOneDiveFullLife = 1.f / m_fFullLifeTemp;

		m_fAlpha[0] = ((DWORD)(m_vColorUP & 0xff000000) >> 24) / 255.f;;
		m_fAlpha[1] = ((DWORD)(m_vColorDOWN & 0xff000000) >> 24) / 255.f;;
		m_dwColor[0] = m_vColorUP & 0xffffff;
		m_dwColor[1] = m_vColorDOWN & 0xffffff;
	}

	//static BOOL bUse (TRUE);
	//if( bUse )
	//{
	//	ConvertRGBToWORD( 238, 250, 255 );	// 0
	//	ConvertRGBToWORD( 176, 102,  92 );	// 1
	//	ConvertRGBToWORD( 255, 190,  51 );	// 2
	//	ConvertRGBToWORD( 189, 216, 255 );	// 3
	//	ConvertRGBToWORD( 255, 134,  21 );	// 4
	//	ConvertRGBToWORD( 255, 208, 148 );	// 5
	//	ConvertRGBToWORD( 243, 250, 255 );	// 6
	//	ConvertRGBToWORD( 255, 255, 255 );	// 7
	//	ConvertRGBToWORD( 138, 151, 192 );	// 0
	//	ConvertRGBToWORD( 238, 148,  53 );	// 1
	//	ConvertRGBToWORD( 241, 196, 123 );	// 2
	//	ConvertRGBToWORD( 189, 112,  36 );	// 3
	//	ConvertRGBToWORD( 255, 191,  64 );	// 4
	//	ConvertRGBToWORD( 211, 152, 110 );	// 5
	//	ConvertRGBToWORD( 219, 182, 103 );	// 6

	//	bUse = FALSE;
	//}
	
	
	
	/*
	m1_hair_00 	238 250 255
	m1_hair_01	176 102  92
	m1_hair_02	255 190  51
	m1_hair_03	189 216 255
	m1_hair_04	255 134  21
	m1_hair_05	255 208 148

	SHW0001_M	243 250 255
	SHW0002_M	255 255 255


	W1_hair_00	138 151 192
	W1_hair_01	238 148  53
	W1_hair_02	241 196 123
	W1_hair_03	189 112  36
	W1_hair_04	255 191  64

	SHW0001_W	211 152 110
	SHW0002_W	219 182 103
	*/

	
	


	return S_OK;
}

void DxEffCharBlur::PrecalculateBeforeRenderEff()
{
	// Note : Key�� �ٲ�� �⺻������ ����� Ű�� �����ش�.
	if( m_dwPrevKeyFrame!=m_dwKeyTime )
	{
		D3DXVECTOR3 vUp(0.f,0.f,0.f);
		D3DXVECTOR3 vDown(0.f,0.f,0.f);

		// Note : ���� ��ġ�� ��´�. ���� ���ٸ� �����Ŷ� �������� �ʴ´�.
		if( !IsUnionKey( vUp, vDown ) )	
			return;

		// Note : �ʱ�ȭ.
		m_bBaseBlurRender = FALSE;

		// Note : ó���� ����
		FrameMoveKeyFrame();

		// ���� ��쿡�� �ܻ��� ���� �����Ѵ�. || ��� �׼��� ��쿡 �����Ѵ�.
		if( (m_CurAniMType==m_SetAniMType) || (m_dwFlag&USEALLACTION) )
		{
			CreateNewKeyFrame( vUp, vDown );	// m_deqKeyPoint �� �ִ´�.
			CreateBlur();						// ����.
		}
		else
		{
			ClearKeyFrame();	// �ִϸ��̼��� �ٲ� ���� ����� Key �����͸� �����Ѵ�.
		}

		FrameMoveBlur();		// Ȱ��.

		// ��� �� �ʱ�ȭ.
		m_dwPrevKeyFrame = m_dwKeyTime;
		m_fElapsedTime = 0.f;
	}
}

void DxEffCharBlur::OnlyRenderEff( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// �����϶�� �ߴµ� �ɼ��� �������� �ʰų� ���Ұ����̸� ���������� �ʴ´�.
	if( (m_dwFlag & USEREFRACT) && !DxSurfaceTex::GetInstance().IsEnable() )
		return;

	m_pSavedStateBlock->Capture();
	m_pEffectStateBlock->Apply();

	D3DCAPSQ d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );

	//	Note : ���� ��� ����
	//
	switch ( m_nBlend )
	{
	case 1 :
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );

		break;
	case 2:
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE2X );

		break;
	case 3:
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE4X );

		break;			
	case 4:
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );

		break;
	case 5:
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		TRUE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	FALSE );

		if( d3dCaps.AlphaCmpCaps & D3DPCMPCAPS_GREATEREQUAL )
		{
			pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
			pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0x80 );
		}
		break;
	}

	pd3dDevice->SetTransform( D3DTS_WORLD, &m_matIdentity );

	if( m_dwFlag & USEREFRACT )
	{
		DxEnvironment::GetInstance().SetRefractPlayON();	// Ȱ��ȭ
		pd3dDevice->SetTexture( 0, DxSurfaceTex::GetInstance().m_pWaveTex );

		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,				D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );
	}
	else
	{
		pd3dDevice->SetTexture( 0, m_textureRes.GetTexture() );
	}

	RenderBlur( pd3dDevice );

	m_pSavedStateBlock->Apply();
}

void DxEffCharBlur::Render ( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* arrayBoneMatrices, BOOL bPieceRender )
{
	PrecalculateBeforeRenderEff();
	OnlyRenderEff( pd3dDevice );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//																K	e	y
//------------------------------------------------------------------------------------------------------------------------------------------------
BOOL DxEffCharBlur::IsUnionKey( D3DXVECTOR3& vUp, D3DXVECTOR3& vDown )
{
	// Note : ������ ��ġ�� ��´�.
	HRESULT hr(S_OK);
	D3DXVECTOR3 vNormal(0.f,0.f,0.f);
	if ( m_pCharPart )
	{
		hr = m_pCharPart->CalculateVertexInflu ( m_szTraceUP, vUp, vNormal );
		if ( FAILED(hr) )	return FALSE;

		hr = m_pCharPart->CalculateVertexInflu ( m_szTraceDOWN, vDown, vNormal );
		if ( FAILED(hr) )	return FALSE;
	}
	else if ( m_pSkinPiece )
	{
		hr = m_pSkinPiece->CalculateVertexInflu ( m_szTraceUP, vUp, vNormal );
		if ( FAILED(hr) )	return FALSE;

		hr = m_pSkinPiece->CalculateVertexInflu ( m_szTraceDOWN, vDown, vNormal );
		if ( FAILED(hr) )	return FALSE;
	}

	if( !m_deqKeyPoint.empty() )
	{
		// ���� ��ġ�� ��� ���� ���� �ʴ´�.
		if( m_deqKeyPoint[0]->sPos[0].x == vUp.x &&
			m_deqKeyPoint[0]->sPos[0].y == vUp.y &&
			m_deqKeyPoint[0]->sPos[0].z == vUp.z )
		{
			return FALSE;
		}
	}

	return TRUE;
}

void DxEffCharBlur::CreateNewKeyFrame( D3DXVECTOR3& vUp, D3DXVECTOR3& vDown )
{
	// Note : ���� �ִ´�.
	VERTEX_UPDOWN* pNew = new VERTEX_UPDOWN;
	pNew->sPos[0] = vUp;
	pNew->sPos[1] = vDown;

	m_deqKeyPoint.push_front( pNew );
	++m_dwKeyPointCOUNT;
}

void DxEffCharBlur::FrameMoveKeyFrame()
{
	VERTEX_UPDOWN* pVector(NULL);
	while( m_deqKeyPoint.size() >= 4 )
	{
		pVector = m_deqKeyPoint.back();

		SAFE_DELETE( pVector );

		m_deqKeyPoint.pop_back();
		--m_dwKeyPointCOUNT;
	}
}

void DxEffCharBlur::ClearKeyFrame()
{
	for( DWORD i=0; i<m_deqKeyPoint.size(); ++i )
	{
		SAFE_DELETE( m_deqKeyPoint[i] );
	}
	m_deqKeyPoint.clear();

	m_dwKeyPointCOUNT = 0;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//															B	l	u	r
//------------------------------------------------------------------------------------------------------------------------------------------------
void DxEffCharBlur::CreateBlur()
{
	float fAlpha(1.f);
	int nNUM(0);
	BLUR_UPDOWN* pNew(NULL);
	BLUR_UPDOWN* pNew1(NULL);

	// Key�� ������ ���� ������ Ʋ���� ��.
	switch( m_dwKeyPointCOUNT ) 
	{
	case 2:
		m_bBaseBlurRender = TRUE;
		m_sBaseBlur[0].vPos = m_deqKeyPoint[0]->sPos[0];
		m_sBaseBlur[1].vPos = m_deqKeyPoint[0]->sPos[1];
		m_sBaseBlur[2].vPos = m_deqKeyPoint[1]->sPos[0];
		m_sBaseBlur[3].vPos = m_deqKeyPoint[1]->sPos[1];
		break;


	case 3:
		m_bBaseBlurRender = TRUE;
		m_sBaseBlur[0].vPos = m_deqKeyPoint[0]->sPos[0];
		m_sBaseBlur[1].vPos = m_deqKeyPoint[0]->sPos[1];
		m_sBaseBlur[2].vPos = m_deqKeyPoint[1]->sPos[0];
		m_sBaseBlur[3].vPos = m_deqKeyPoint[1]->sPos[1];

		// Note : ������ ����.
		pNew = new BLUR_UPDOWN;
		pNew->fLife = m_fFullLife;
		pNew->sVertex[0].vPos = m_deqKeyPoint[2]->sPos[0];
		pNew->sVertex[1].vPos = m_deqKeyPoint[2]->sPos[1];
		m_deqBlurPoint.push_back( pNew );

		pNew1 = new BLUR_UPDOWN;
		pNew1->fLife = m_fFullLife;
		pNew1->sVertex[0].vPos = m_deqKeyPoint[1]->sPos[0];
		pNew1->sVertex[1].vPos = m_deqKeyPoint[1]->sPos[1];
		m_deqBlurPoint.push_back( pNew1 );

		break;


	case 4:
		m_bBaseBlurRender = TRUE;
		m_sBaseBlur[0].vPos = m_deqKeyPoint[0]->sPos[0];
		m_sBaseBlur[1].vPos = m_deqKeyPoint[0]->sPos[1];
		m_sBaseBlur[2].vPos = m_deqKeyPoint[1]->sPos[0];
		m_sBaseBlur[3].vPos = m_deqKeyPoint[1]->sPos[1];

		// Note : �߰� �ܰ� ����.
		nNUM = SeparateNumOfLine();
		switch( nNUM )
		{
		case 0:
			break;

		case 1:
			DxInsertBlurPoint( 0.5f );
			break;

		case 2:
			DxInsertBlurPoint( 0.66f );
			DxInsertBlurPoint( 0.33f );
			break;

		case 3:
			DxInsertBlurPoint( 0.75f );
			DxInsertBlurPoint( 0.5f );
			DxInsertBlurPoint( 0.25f );
			break;

		case 4:
			DxInsertBlurPoint( 0.8f );
			DxInsertBlurPoint( 0.6f );
			DxInsertBlurPoint( 0.4f );
			DxInsertBlurPoint( 0.2f );
			break;

		case 5:
			DxInsertBlurPoint( 0.83f );
			DxInsertBlurPoint( 0.66f );
			DxInsertBlurPoint( 0.5f );
			DxInsertBlurPoint( 0.33f );
			DxInsertBlurPoint( 0.17f );
			break;

		case 6:
			DxInsertBlurPoint( 0.858f );
			DxInsertBlurPoint( 0.715f );
			DxInsertBlurPoint( 0.572f );
			DxInsertBlurPoint( 0.429f );
			DxInsertBlurPoint( 0.286f );
			DxInsertBlurPoint( 0.143f );
			break;

		case 7:
			fAlpha = 1.f;
			DxInsertBlurPoint( 0.875f );
			DxInsertBlurPoint( 0.75f );
			DxInsertBlurPoint( 0.625f );
			DxInsertBlurPoint( 0.5f );
			DxInsertBlurPoint( 0.375f );
			DxInsertBlurPoint( 0.25f );
			DxInsertBlurPoint( 0.125f );
			break;

		case 15:
			fAlpha = 1.f;					fAlpha -= 0.0625f;
			DxInsertBlurPoint( fAlpha );	fAlpha -= 0.0625f;
			DxInsertBlurPoint( fAlpha );	fAlpha -= 0.0625f;
			DxInsertBlurPoint( fAlpha );	fAlpha -= 0.0625f;
			DxInsertBlurPoint( fAlpha );	fAlpha -= 0.0625f;
			DxInsertBlurPoint( fAlpha );	fAlpha -= 0.0625f;
			DxInsertBlurPoint( fAlpha );	fAlpha -= 0.0625f;
			DxInsertBlurPoint( fAlpha );	fAlpha -= 0.0625f;
			DxInsertBlurPoint( fAlpha );	fAlpha -= 0.0625f;
			DxInsertBlurPoint( fAlpha );	fAlpha -= 0.0625f;
			DxInsertBlurPoint( fAlpha );	fAlpha -= 0.0625f;
			DxInsertBlurPoint( fAlpha );	fAlpha -= 0.0625f;
			DxInsertBlurPoint( fAlpha );	fAlpha -= 0.0625f;
			DxInsertBlurPoint( fAlpha );	fAlpha -= 0.0625f;
			DxInsertBlurPoint( fAlpha );	fAlpha -= 0.0625f;
			DxInsertBlurPoint( fAlpha );
			break;

		default:
            {
                std::string ErrorMsg(
                    sc::string::format(
                        "DxEffCharBlur::CreateBlur %1% SeparateNumOfLine %2%",
                        DxEffCharBlur::NAME,
                        nNUM));
			    CDebugSet::ToLogFile(ErrorMsg);
            }
		};

		// Note : ���� �� ����.
		pNew = new BLUR_UPDOWN;
		pNew->fLife = m_fFullLife;
		pNew->sVertex[0].vPos = m_deqKeyPoint[1]->sPos[0];
		pNew->sVertex[1].vPos = m_deqKeyPoint[1]->sPos[1];
		m_deqBlurPoint.push_back( pNew );		

		break;

	default:
		m_bBaseBlurRender = FALSE;
		break;
	};
}

int DxEffCharBlur::SeparateNumOfLine()
{
	D3DXVECTOR3 vLine[2];
	vLine[0] = m_deqKeyPoint[0]->sPos[0] - m_deqKeyPoint[2]->sPos[0];
	vLine[1] = m_deqKeyPoint[1]->sPos[0] - m_deqKeyPoint[3]->sPos[0];

	D3DXVec3Normalize( &vLine[0], &vLine[0] );
	D3DXVec3Normalize( &vLine[1], &vLine[1] );

	float fDot = D3DXVec3Dot( &vLine[0], &vLine[1] );

	int nData(0);
	if( fDot > 0.97f )			nData = 0;
	else if( fDot > 0.93f )		nData = 1;
	else if( fDot > 0.89f )		nData = 2;
	else if( fDot > 0.83f )		nData = 3;
	else if( fDot > 0.75f )		nData = 4;
	else if( fDot > 0.55f )		nData = 5;
	else if( fDot > 0.3f )		nData = 6;
	else if( fDot > 0.0f )		nData = 7;
	else						nData = 15;

	// ĳ���� �������� ���ٸ� ������ ���� ���� �ʴ´�.
	if( DxEffectMan::GetInstance().GetSkinDetail() == SKD_SPEC )
	{
		if( nData==0 )			nData = 0;
		else if( nData==1 )		nData = 0;
		else if( nData==2 )		nData = 1;
		else if( nData==3 )		nData = 2;
		else if( nData==4 )		nData = 3;
		else if( nData==5 )		nData = 4;
		else if( nData==6 )		nData = 5;
		else if( nData==7 )		nData = 6;
		else if( nData==15 )	nData = 7;
	}
	else if( DxEffectMan::GetInstance().GetSkinDetail() == SKD_NONE )
	{
		if( nData==0 )			nData = 0;
		else if( nData==1 )		nData = 0;
		else if( nData==2 )		nData = 1;
		else if( nData==3 )		nData = 1;
		else if( nData==4 )		nData = 2;
		else if( nData==5 )		nData = 2;
		else if( nData==6 )		nData = 3;
		else if( nData==7 )		nData = 3;
		else if( nData==15 )	nData = 4;
	}

	return nData;
}

void DxEffCharBlur::DxInsertBlurPoint( float fRate )
{
	D3DXVECTOR3 vPos1(0.f,0.f,0.f);
	D3DXVec3CatmullRom( &vPos1, &m_deqKeyPoint[0]->sPos[0], &m_deqKeyPoint[1]->sPos[0], 
								&m_deqKeyPoint[2]->sPos[0], &m_deqKeyPoint[3]->sPos[0], fRate );

	D3DXVECTOR3 vPos2(0.f,0.f,0.f);
	D3DXVec3CatmullRom( &vPos2, &m_deqKeyPoint[0]->sPos[1], &m_deqKeyPoint[1]->sPos[1], 
								&m_deqKeyPoint[2]->sPos[1], &m_deqKeyPoint[3]->sPos[1], fRate );

	BLUR_UPDOWN* pNew = new BLUR_UPDOWN;
	pNew->fLife = m_fFullLife;
	pNew->sVertex[0].vPos = vPos1;
	pNew->sVertex[1].vPos = vPos2;
	m_deqBlurPoint.push_back( pNew );
}

void DxEffCharBlur::FrameMoveBlur()
{
	// �ð��� �긮��,,
	DWORD dwSize = (DWORD)m_deqBlurPoint.size();
	for( DWORD i=0; i<dwSize; ++i )
	{
		m_deqBlurPoint[i]->fLife -= m_fElapsedTime;
	}

	// �ð� �� �� ���� �����ش�.
	// �� �ð��� �����ϰ� ���� ������ ť�������� �����.
	BLUR_UPDOWN* pCur(NULL);
	while( !m_deqBlurPoint.empty() )
	{
		pCur = m_deqBlurPoint.front();

		if( pCur->fLife > 0.f )	goto _RETURN;

		SAFE_DELETE( pCur );
		m_deqBlurPoint.pop_front();
	}

_RETURN:

	D3DVIEWPORT9* pViewPort(NULL);
	D3DXMATRIX* pmatView(NULL);
	D3DXMATRIX* pmatProj(NULL);
	float fOneDivWidth(0.f);
	float fOneDivHeight(0.f);
	float fAddWidth(0.f);
	float fAddHeight(0.f);

	if( (m_dwFlag & USEREFRACT) && DxSurfaceTex::GetInstance().IsEnable() )
	{
		pViewPort = &DxViewPort::GetInstance().GetViewPort();
		pmatView = &DxViewPort::GetInstance().GetMatView();
		pmatProj = &DxViewPort::GetInstance().GetMatProj();

		if( pViewPort->Width < 1 )	return;
		if( pViewPort->Height < 1 )	return;

		fOneDivWidth = 1.f / pViewPort->Width;
		fOneDivHeight = 1.f / pViewPort->Height;

		// ȸ��
		float fSin = sinf(m_fTime)*0.01f;
		float fCos = cosf(m_fTime)*0.01f;

		fAddWidth = fCos + fSin;
		fAddHeight = fCos - fSin;
	}

	// Note : 
	dwSize = (DWORD)m_deqBlurPoint.size();
	float fTemp(0.f);
	float fOneDivSize(0.f);

	if( m_bBaseBlurRender )
	{
		fOneDivSize = 1.f / (dwSize+1);	// Base �ܻ� ������. 1�� �����־���.
	}
	else if( dwSize )
	{
		fOneDivSize = 1.f / dwSize;
	}

	// Note : �⺻ �ܻ�.
	int nCountUV(0);
	if( m_bBaseBlurRender )
	{
		m_sBaseBlur[0].dwColor = m_vColorUP;
		m_sBaseBlur[1].dwColor = m_vColorDOWN;

		if( m_deqBlurPoint.empty() )
		{
			m_sBaseBlur[2].dwColor = m_dwColor[0];
			m_sBaseBlur[3].dwColor = m_dwColor[1];
		}
		else
		{
			fTemp = m_deqBlurPoint.back()->fLife * m_fOneDiveFullLife;
			fTemp = fTemp*255.f;

			m_sBaseBlur[2].dwColor = (DWORD)(fTemp*m_fAlpha[0]) << 24;
			m_sBaseBlur[3].dwColor = (DWORD)(fTemp*m_fAlpha[1]) << 24;
			m_sBaseBlur[2].dwColor += m_dwColor[0];
			m_sBaseBlur[3].dwColor += m_dwColor[1];
		}

		
		if( (m_dwFlag & USEREFRACT) && DxSurfaceTex::GetInstance().IsEnable() )
		{
			D3DXVECTOR3 vPos(0.f,0.f,0.f);
			D3DXVec3Project( &vPos, &m_sBaseBlur[0].vPos, pViewPort, pmatProj, pmatView, &m_matIdentity );
			vPos.x = vPos.x*fOneDivWidth + fAddWidth;
			vPos.y = vPos.y*fOneDivHeight + fAddHeight;
			m_sBaseBlur[0].vTex = D3DXVECTOR2( vPos.x, vPos.y );

			D3DXVec3Project( &vPos, &m_sBaseBlur[1].vPos, pViewPort, pmatProj, pmatView, &m_matIdentity );
			vPos.x = vPos.x*fOneDivWidth + fAddWidth;
			vPos.y = vPos.y*fOneDivHeight + fAddHeight;
			m_sBaseBlur[1].vTex = D3DXVECTOR2( vPos.x, vPos.y );

			D3DXVec3Project( &vPos, &m_sBaseBlur[2].vPos, pViewPort, pmatProj, pmatView, &m_matIdentity );
			vPos.x = vPos.x*fOneDivWidth + fAddWidth;
			vPos.y = vPos.y*fOneDivHeight + fAddHeight;
			m_sBaseBlur[2].vTex = D3DXVECTOR2( vPos.x, vPos.y );

			D3DXVec3Project( &vPos, &m_sBaseBlur[3].vPos, pViewPort, pmatProj, pmatView, &m_matIdentity );
			vPos.x = vPos.x*fOneDivWidth + fAddWidth;
			vPos.y = vPos.y*fOneDivHeight + fAddHeight;
			m_sBaseBlur[3].vTex = D3DXVECTOR2( vPos.x, vPos.y );
		}
		else
		{
			m_sBaseBlur[0].vTex = D3DXVECTOR2( 0.f, 0.f );
			m_sBaseBlur[1].vTex = D3DXVECTOR2( 0.f, 1.f );
			m_sBaseBlur[2].vTex = D3DXVECTOR2( fOneDivSize, 0.f );
			m_sBaseBlur[3].vTex = D3DXVECTOR2( fOneDivSize, 1.f );
		}

		++nCountUV;	// ���� ���� ���ؼ� �־�д�.
	}

	// Note : ��ǻ�� ���� ��ȭ ��Ų��.	
	for( DWORD i=0; i<dwSize; ++i, ++nCountUV )
	{
		fTemp = m_deqBlurPoint[i]->fLife * m_fOneDiveFullLife *255.f;

		m_deqBlurPoint[i]->sVertex[0].dwColor = (DWORD)(fTemp*m_fAlpha[0]) << 24;
		m_deqBlurPoint[i]->sVertex[1].dwColor = (DWORD)(fTemp*m_fAlpha[1]) << 24;
		m_deqBlurPoint[i]->sVertex[0].dwColor += m_dwColor[0];
		m_deqBlurPoint[i]->sVertex[1].dwColor += m_dwColor[1];

		if( (m_dwFlag & USEREFRACT) && DxSurfaceTex::GetInstance().IsEnable() )
		{
			D3DXVECTOR3 vPos(0.f,0.f,0.f);
			D3DXVec3Project( &vPos, &m_deqBlurPoint[i]->sVertex[0].vPos, pViewPort, pmatProj, pmatView, &m_matIdentity );
			vPos.x = vPos.x*fOneDivWidth + fAddWidth;
			vPos.y = vPos.y*fOneDivHeight + fAddHeight;
			m_deqBlurPoint[i]->sVertex[0].vTex = D3DXVECTOR2( vPos.x, vPos.y );

			D3DXVec3Project( &vPos, &m_deqBlurPoint[i]->sVertex[1].vPos, pViewPort, pmatProj, pmatView, &m_matIdentity );
			vPos.x = vPos.x*fOneDivWidth + fAddWidth;
			vPos.y = vPos.y*fOneDivHeight + fAddHeight;
			m_deqBlurPoint[i]->sVertex[1].vTex = D3DXVECTOR2( vPos.x, vPos.y );
		}
		else
		{
			fTemp = nCountUV*fOneDivSize;
			m_deqBlurPoint[i]->sVertex[0].vTex = D3DXVECTOR2( fTemp, 0.f );
			m_deqBlurPoint[i]->sVertex[1].vTex = D3DXVECTOR2( fTemp, 1.f );
		}
	}
}

void DxEffCharBlur::ClearBlurDara()
{
	for( DWORD i=0; i<m_deqBlurPoint.size(); ++i )
	{
		SAFE_DELETE( m_deqBlurPoint[i] );
	}
	m_deqBlurPoint.clear();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//															R	E	N	D	E	R
//------------------------------------------------------------------------------------------------------------------------------------------------
void DxEffCharBlur::RenderBlur( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr(S_OK);

	pd3dDevice->SetIndices ( NULL );

	// Note : �⺻ �ܻ�.
	if( m_bBaseBlurRender )
	{
        DWORD dwVertexSizeFULL = DxDynamicVB::g_sVB_I_24BYTE.GetFVFSize()*4;
        if( !DxDynamicVB::g_sVB_I_24BYTE.IsValidSize( dwVertexSizeFULL ) )
            return;

        UINT uiStartVertex = DxDynamicVB::g_sVB_I_24BYTE.CopyToVB(
	        m_sBaseBlur,
	        dwVertexSizeFULL,
	        4 );

		pd3dDevice->SetFVF ( D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1 );
		pd3dDevice->SetStreamSource ( 0, DxDynamicVB::g_sVB_I_24BYTE.pVB, 0, DxDynamicVB::g_sVB_I_24BYTE.GetFVFSize() );

		pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, uiStartVertex, 2 );
	}
	
	// Note : ���� �ܻ�.
	if( !m_deqBlurPoint.empty() )
	{
		DWORD dwBlurPointSize = (DWORD)m_deqBlurPoint.size();
		if( dwBlurPointSize <= 1 )
            return;

        DWORD dwVerticesNUM = dwBlurPointSize * 2;
		DWORD dwVertexSizeFULL = DxDynamicVB::g_sVB_I_24BYTE.GetFVFSize()*dwVerticesNUM;
        if( !DxDynamicVB::g_sVB_I_24BYTE.IsValidSize( dwVertexSizeFULL ) )
            return;

        VOID* pLocked = DxDynamicVB::g_sVB_I_24BYTE.LockVB( dwVertexSizeFULL );
        if( !pLocked )
            return;

		VERTEXCOLORTEX *pVertices = (VERTEXCOLORTEX*)pLocked;
		for( DWORD i=0; i<dwBlurPointSize; ++i )
		{
			pVertices[i*2+0] = m_deqBlurPoint[i]->sVertex[0];
			pVertices[i*2+1] = m_deqBlurPoint[i]->sVertex[1];
		}

        UINT iStartVertex = DxDynamicVB::g_sVB_I_24BYTE.UnlockVB( dwVertexSizeFULL, dwVerticesNUM );

		pd3dDevice->SetFVF ( D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1 );
		pd3dDevice->SetStreamSource ( 0, DxDynamicVB::g_sVB_I_24BYTE.pVB, 0, DxDynamicVB::g_sVB_I_24BYTE.GetFVFSize() );

		pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, iStartVertex, dwVerticesNUM-2 );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//												L	O	A	D		&&		 S	A	V	E
//------------------------------------------------------------------------------------------------------------------------------------------------
HRESULT DxEffCharBlur::LoadFile_0100 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	char szIMAGE[4][STRACE_NSIZE] =
	{
		STRACE_IMAGE01,
		STRACE_IMAGE02,
		STRACE_IMAGE11,
		STRACE_IMAGE12,
	};

	//	Note : ����Ʈ�� Property �� ����.
	//
	EFFCHAR_PROPERTY_BLUR_100	m_Property100;
	SFile.ReadBuffer ( &m_Property100, sizeof(EFFCHAR_PROPERTY_BLUR_100) );

	m_Property.m_nBlend				= m_Property100.m_nBlend;
	StringCchCopy(m_Property.m_szTraceUP,	STRACE_NSIZE, szIMAGE[m_Property100.m_dwTraceIndexUP]);
	StringCchCopy(m_Property.m_szTraceDOWN,	STRACE_NSIZE, szIMAGE[m_Property100.m_dwTraceIndexDOWN]);
	m_Property.m_vColorUPTemp			= m_Property100.m_vColorUPTemp;
	m_Property.m_vColorDOWNTemp			= m_Property100.m_vColorDOWNTemp;
	StringCchCopy( m_Property.m_szTexture, TEXTURE_LENGTH, m_Property100.m_szTexture ); 

	//	Ver.102 �� �߰� �� �͵�
	m_Property.m_SetAniMType		= AN_ATTACK;
	m_Property.m_SetAniSType		= AN_SUB_NONE;

	//	Blur ���� ���׷� ���� A/S
	std::string strSrcName = ((DxEffChar*)this)->GetFileName(); // "effskin"
	std::string::size_type idx = strSrcName.find("effskin");
	if ( idx != std::string::npos )		m_dwFlag |= USEALLACTION;	// ��ġ�ϴ� ���� �ִ�.

	return S_OK;
}

HRESULT DxEffCharBlur::LoadFile_0101 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	char szIMAGE[4][STRACE_NSIZE] =
	{
		STRACE_IMAGE01,
		STRACE_IMAGE02,
		STRACE_IMAGE11,
		STRACE_IMAGE12,
	};

	//	Note : ����Ʈ�� Property �� ����.
	//
	EFFCHAR_PROPERTY_BLUR_101	m_Property101;
	SFile.ReadBuffer ( &m_Property101, sizeof(EFFCHAR_PROPERTY_BLUR_101) );

	m_Property.m_nBlend				= m_Property101.m_nBlend;
	
	StringCchCopy(m_Property.m_szTraceUP,	STRACE_NSIZE, szIMAGE[m_Property101.m_dwTraceIndexUP]);
	StringCchCopy(m_Property.m_szTraceDOWN,	STRACE_NSIZE, szIMAGE[m_Property101.m_dwTraceIndexDOWN]);
	m_Property.m_vColorUPTemp			= m_Property101.m_vColorUPTemp;
	m_Property.m_vColorDOWNTemp			= m_Property101.m_vColorDOWNTemp;
	StringCchCopy( m_Property.m_szTexture, TEXTURE_LENGTH, m_Property101.m_szTexture ); 

	//	Ver.102 �� �߰� �� �͵�
	m_Property.m_SetAniMType		= AN_ATTACK;
	m_Property.m_SetAniSType		= AN_SUB_NONE;

	//	Blur ���� ���׷� ���� A/S
	std::string strSrcName = ((DxEffChar*)this)->GetFileName(); // "effskin"
	std::string::size_type idx = strSrcName.find("effskin");
	if ( idx != std::string::npos )		m_dwFlag |= USEALLACTION;	// ��ġ�ϴ� ���� �ִ�.

	return S_OK;
}

HRESULT DxEffCharBlur::LoadFile_0102 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	char szIMAGE[4][STRACE_NSIZE] =
	{
		STRACE_IMAGE01,
		STRACE_IMAGE02,
		STRACE_IMAGE11,
		STRACE_IMAGE12,
	};

	//	Note : ����Ʈ�� Property �� ����.
	//
	EFFCHAR_PROPERTY_BLUR_102	pProperty;
	SFile.ReadBuffer ( &pProperty, sizeof(EFFCHAR_PROPERTY_BLUR_102) );

	m_Property.m_SetAniMType		= pProperty.m_SetAniMType;
	m_Property.m_SetAniSType		= pProperty.m_SetAniSType;

	m_Property.m_nBlend				= pProperty.m_nBlend;
	
	StringCchCopy(m_Property.m_szTraceUP,	STRACE_NSIZE, szIMAGE[pProperty.m_dwTraceIndexUP]);
	StringCchCopy(m_Property.m_szTraceDOWN,	STRACE_NSIZE, szIMAGE[pProperty.m_dwTraceIndexDOWN]);
	m_Property.m_vColorUPTemp			= pProperty.m_vColorUPTemp;
	m_Property.m_vColorDOWNTemp			= pProperty.m_vColorDOWNTemp;
	StringCchCopy( m_Property.m_szTexture, TEXTURE_LENGTH, pProperty.m_szTexture ); 

	//	Blur ���� ���׷� ���� A/S
	std::string strSrcName = ((DxEffChar*)this)->GetFileName(); // "effskin"
	std::string::size_type idx = strSrcName.find("effskin");
	if ( idx != std::string::npos )		m_dwFlag |= USEALLACTION;	// ��ġ�ϴ� ���� �ִ�.

	return S_OK;
}

HRESULT DxEffCharBlur::LoadFile_0103 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : ����Ʈ�� Property �� ����.
	//
	EFFCHAR_PROPERTY_BLUR_103	pProperty;
	SFile.ReadBuffer ( &pProperty, sizeof(EFFCHAR_PROPERTY_BLUR_103) );

	m_Property.m_SetAniMType		= pProperty.m_SetAniMType;
	m_Property.m_SetAniSType		= pProperty.m_SetAniSType;

	m_Property.m_nBlend				= pProperty.m_nBlend;

	m_Property.m_vColorUPTemp			= pProperty.m_vColorUPTemp;
	m_Property.m_vColorDOWNTemp			= pProperty.m_vColorDOWNTemp;

	StringCchCopy( m_Property.m_szTexture,		TEXTURE_LENGTH,		pProperty.m_szTexture ); 
	StringCchCopy( m_Property.m_szTraceUP,		STRACE_NSIZE,		pProperty.m_szTraceUP);
	StringCchCopy( m_Property.m_szTraceDOWN,	STRACE_NSIZE,		pProperty.m_szTraceDOWN);

	//	Blur ���� ���׷� ���� A/S
	std::string strSrcName = ((DxEffChar*)this)->GetFileName(); // "effskin"
	std::string::size_type idx = strSrcName.find("effskin");
	if ( idx != std::string::npos )		m_dwFlag |= USEALLACTION;	// ��ġ�ϴ� ���� �ִ�.

	return S_OK;
}

HRESULT DxEffCharBlur::LoadFile_0104 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : ����Ʈ�� Property �� ����.
	//
	EFFCHAR_PROPERTY_BLUR_104	pProperty;
	SFile.ReadBuffer ( &pProperty, sizeof(EFFCHAR_PROPERTY_BLUR_104) );

	m_Property.m_SetAniMType		= pProperty.m_SetAniMType;
	m_Property.m_SetAniSType		= pProperty.m_SetAniSType;

	m_Property.m_nBlend				= pProperty.m_nBlend;
	m_Property.m_dwFlag				= pProperty.m_dwFlag;

	m_Property.m_vColorUPTemp			= pProperty.m_vColorUPTemp;
	m_Property.m_vColorDOWNTemp			= pProperty.m_vColorDOWNTemp;

	StringCchCopy( m_Property.m_szTexture,		TEXTURE_LENGTH,		pProperty.m_szTexture ); 
	StringCchCopy( m_Property.m_szTraceUP,		STRACE_NSIZE,		pProperty.m_szTraceUP);
	StringCchCopy( m_Property.m_szTraceDOWN,	STRACE_NSIZE,		pProperty.m_szTraceDOWN);

	//	Blur ���� ���׷� ���� A/S
	std::string strSrcName = ((DxEffChar*)this)->GetFileName(); // "effskin"
	std::string::size_type idx = strSrcName.find("effskin");
	if ( idx != std::string::npos )		m_dwFlag |= USEALLACTION;	// ��ġ�ϴ� ���� �ִ�.

	return S_OK;
}

HRESULT DxEffCharBlur::LoadFile_0105 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : ����Ʈ�� Property �� ����.
	//
	EFFCHAR_PROPERTY_BLUR_105	pProperty;
	SFile.ReadBuffer ( &pProperty, sizeof(EFFCHAR_PROPERTY_BLUR_105) );

	m_Property.Assign( pProperty );

	//	Blur ���� ���׷� ���� A/S
	std::string strSrcName = ((DxEffChar*)this)->GetFileName(); // "effskin"
	std::string::size_type idx = strSrcName.find("effskin");
	if ( idx != std::string::npos )		m_dwFlag |= USEALLACTION;	// ��ġ�ϴ� ���� �ִ�.

	return S_OK;
}

HRESULT DxEffCharBlur::LoadFile_0106 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : ����Ʈ�� Property �� ����.
	//
	SFile.ReadBuffer ( &m_Property, sizeof(EFFCHAR_PROPERTY_BLUR) );

	//	Blur ���� ���׷� ���� A/S
	std::string strSrcName = ((DxEffChar*)this)->GetFileName(); // "effskin"
	std::string::size_type idx = strSrcName.find("effskin");
	if ( idx != std::string::npos )		m_dwFlag |= USEALLACTION;	// ��ġ�ϴ� ���� �ִ�.

	return S_OK;
}

HRESULT	DxEffCharBlur::LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
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
	case VERSION:	LoadFile_0106( SFile, pd3dDevice );	break;
	default:
		{
			//	Note : ������ Ʋ�� ��쿡�� ���Ͽ� ������ DATA������ �Ǵ� ��� �۾��� ����.
			//
			SFile.SetOffSet ( SFile.GetfTell()+dwSize );
			GASSERT ( 0 && "DxEffCharBlur���� ������ ����� ���� ���Ͽ����ϴ�." );
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

HRESULT	DxEffCharBlur::SaveFile ( sc::BaseStream &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	//	Note : DATA�� ����� ����Ѵ�. Load �ÿ� ������ Ʋ�� ��� ����.
	//
	SFile << (DWORD) ( sizeof(m_Property) );

	//	Note : ����Ʈ�� Property �� ����.
	//
	SFile.WriteBuffer ( &m_Property, sizeof(m_Property) );

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// Material System �� ���� ȿ������ ���̱� ���� �۾�.
void DxEffCharBlur::PrecalculateBeforeRenderEff( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* pMatrix )
{
	PrecalculateBeforeRenderEff();
}

void DxEffCharBlur::RenderEffForMaterialSystem( LPDIRECT3DDEVICEQ pd3dDevice, 
											   const D3DXMATRIX* pMatrix, 
											   const float fScale )
{
	OnlyRenderEff( pd3dDevice );
}