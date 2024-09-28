#include "pch.h"
#include "./DxDynamicVB.h"

#include "DebugSet.h"

#include "../GlobalParam.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const bool g_bUsageSystemMemory = true;
const DWORD g_nDynamicMaxFace = 512;

DxDynamicVB::BIGVB DxDynamicVB::g_sVB_PDT;
DxDynamicVB::BIGVB DxDynamicVB::g_sVB_PDT_HARD_ALPHA;
DxDynamicVB::BIGVB DxDynamicVB::g_sVB_PNT;
DxDynamicVB::BIGVB DxDynamicVB::g_sVB_PNTI;
DxDynamicVB::BIGVB DxDynamicVB::g_sVB_I_24BYTE;
DxDynamicVB::BIGVB DxDynamicVB::g_sVB_PT2I;

LPDIRECT3DDEVICE9 DxDynamicVB::g_pd3dDevice = NULL;
LPDIRECT3DINDEXBUFFER9 DxDynamicVB::g_pIBuffer = NULL;

DxDynamicVB::BIGVB::BIGVB()
	: pVB( NULL )

	, dwFVF( 0 )
	, nVertexSize( 0 )
	, nFullByte( 0 )

	, iStartVertex( 0 )
	, nStartToLock( 0 )
	, nOffsetToLock( 0 )

	, pSystemMemory( NULL )
{
}

DxDynamicVB::BIGVB::~BIGVB()
{
	SAFE_DELETE_ARRAY( pSystemMemory );
}

VOID DxDynamicVB::BIGVB::Clear()
{
	iStartVertex = 0;
	nStartToLock = 0;
	nOffsetToLock = 0;
}

VOID DxDynamicVB::BIGVB::CreateVB( LPDIRECT3DDEVICE9 pd3dDevice, BOOL bCreateSysMem )
{
	pd3dDevice->CreateVertexBuffer(
		nFullByte,
		D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
		dwFVF,
		D3DPOOL_DEFAULT,
		&pVB,
		NULL );

    if( !g_bUsageSystemMemory )
        return;

	if( bCreateSysMem && !pSystemMemory )
		pSystemMemory = new BYTE[ nFullByte ];
}

VOID DxDynamicVB::BIGVB::ReleaseVB()
{
	SAFE_RELEASE( pVB );
}

BOOL DxDynamicVB::BIGVB::IsValidSize( UINT uiSize )
{
    if( uiSize <= 0 || uiSize > nFullByte )
        return FALSE;

    return TRUE;
}

BOOL DxDynamicVB::BIGVB::IsValidCopy( UINT uiSize )
{
	if( nOffsetToLock + uiSize > nFullByte )
		return FALSE;

	return TRUE;
}

VOID* DxDynamicVB::BIGVB::LockVB( UINT uiSize )
{
    if( !IsValidSize( uiSize ) )
        return NULL;

	DWORD dwFlag = D3DLOCK_NOOVERWRITE;
	if( !IsValidCopy( uiSize ) )
	{
		dwFlag = D3DLOCK_DISCARD;
		Clear();
	}

	VOID* pLocked = NULL;
	if( pSystemMemory )
		pLocked = &pSystemMemory[ nOffsetToLock ];
	else
   		pVB->Lock( nOffsetToLock, uiSize, &pLocked, dwFlag );

	return pLocked;
}

UINT DxDynamicVB::BIGVB::UnlockVB( UINT uiSize, UINT uiVertice )
{
	if( pSystemMemory )
	{
	}
	else
	{
        pVB->Unlock();
	}

	UINT uiStartVetex = iStartVertex;

	nOffsetToLock += uiSize;
	iStartVertex += uiVertice;

	return uiStartVetex;
}

UINT DxDynamicVB::BIGVB::CopyToVB( const VOID* buffer, UINT uiSize, UINT uiVertice )
{
	VOID* pLocked = LockVB( uiSize );
    if( !pLocked )
        return 0;

	memcpy( pLocked, buffer, uiSize );
	UINT uiStartVertex = UnlockVB( uiSize, uiVertice );

	return uiStartVertex;
}

VOID DxDynamicVB::BIGVB::UpdateVB()
{
	if( !pSystemMemory )
		return;

	if( nStartToLock >= nOffsetToLock )
		return;

	UINT uiSize = nOffsetToLock - nStartToLock;

	DWORD dwFlag = D3DLOCK_NOOVERWRITE;
	if( nStartToLock == 0 )
		dwFlag = D3DLOCK_DISCARD;

	VOID* pLocked = NULL;
    pVB->Lock( nStartToLock, uiSize, &pLocked, dwFlag );
    if( !pLocked )
        return;

	memcpy( pLocked, &pSystemMemory[ nStartToLock ], uiSize );

	pVB->Unlock();

	nStartToLock = nOffsetToLock;
}

VOID DxDynamicVB::BIGVB::DiscardVB()
{
    VOID* pLocked = NULL;
    if (pVB)
    {
        pVB->Lock( 0, 0, &pLocked, D3DLOCK_DISCARD );
        pVB->Unlock();
    }
}

HRESULT DxDynamicVB::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
    g_pd3dDevice = pd3dDevice;

	DWORD dwDynamicMaxFace(0);

	if (dwDynamicMaxFace < PNT_NUM )
		dwDynamicMaxFace = PNT_NUM;
	if (dwDynamicMaxFace < PDT_HARD_ALPHA_NUM )
		dwDynamicMaxFace = PDT_HARD_ALPHA_NUM;
	if (dwDynamicMaxFace < PDT_NUM )
		dwDynamicMaxFace = PDT_NUM;
	if (dwDynamicMaxFace < P_NUM )
		dwDynamicMaxFace = P_NUM;
	if (dwDynamicMaxFace < PNTI_NUM )
		dwDynamicMaxFace = PNTI_NUM;
	if (dwDynamicMaxFace < PWDTI_NUM )
		dwDynamicMaxFace = PWDTI_NUM;
	if (dwDynamicMaxFace < PDTI_NUM )
		dwDynamicMaxFace = PDTI_NUM;
    if (dwDynamicMaxFace < PT2I_NUM )
		dwDynamicMaxFace = PT2I_NUM;

	// Note : 기본적인 것
	g_sVB_PNT.dwFVF			= D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1;		// 원래는 0 였는데 dwFVF 를 사용하는 부분이 있어서 넣어줌.
	g_sVB_PNT.nVertexSize	= D3DXGetFVFVertexSize( g_sVB_PNT.dwFVF );
	g_sVB_PNT.nFullByte		= g_sVB_PNT.nVertexSize * 4 * PNT_NUM;										// 32 Byte * 4 * 300 = 37.5 KByte

	// Note : 파티클, 등.. 점을 한번에 많이 사용
	g_sVB_PDT.dwFVF			= D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1;
	g_sVB_PDT.nVertexSize	= D3DXGetFVFVertexSize( g_sVB_PDT.dwFVF );
	g_sVB_PDT.nFullByte		= g_sVB_PDT.nVertexSize * 4 * PDT_NUM;										// 24 Byte * 4 * 6000 = 562.5 KByte

	// Note : MobCharacter
	g_sVB_PDT_HARD_ALPHA.dwFVF			= D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1;
	g_sVB_PDT_HARD_ALPHA.nVertexSize	= D3DXGetFVFVertexSize( g_sVB_PDT_HARD_ALPHA.dwFVF );
	g_sVB_PDT_HARD_ALPHA.nFullByte		= g_sVB_PDT_HARD_ALPHA.nVertexSize * 4 * PDT_HARD_ALPHA_NUM;	// 20 Byte * 4 * 400 = 32 KByte

	// Note : 지면, 천효과 등... 천효과에서 많이 사용.
	g_sVB_PNTI.dwFVF		= D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1;
	g_sVB_PNTI.nVertexSize	= D3DXGetFVFVertexSize( g_sVB_PNTI.dwFVF );
	g_sVB_PNTI.nFullByte	= g_sVB_PNTI.nVertexSize * 4 * PNTI_NUM;									// 32 Byte * 4 * 3000 = 375 KByte

	// Note : 파티클, 등.. 점을 한번에 많이 사용 & DxEffProj(D3DFVF_XYZRHW|D3DFVF_TEX1)
	g_sVB_I_24BYTE.dwFVF		= 0L;	// (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1) (D3DFVF_XYZRHW|D3DFVF_TEX1)	// 사용시 직접 FVF 를 써줘야함.
	g_sVB_I_24BYTE.nVertexSize	= 24;
	g_sVB_I_24BYTE.nFullByte	= g_sVB_I_24BYTE.nVertexSize * 4 * PDTI_NUM;							// 24 Byte * 4 * 2000 = 187.5 KByte

    // Note : 지형 이펙트 FlowUV 등 쓰임
	g_sVB_PT2I.dwFVF		= D3DFVF_XYZ|D3DFVF_TEX2;
	g_sVB_PT2I.nVertexSize	= D3DXGetFVFVertexSize( g_sVB_PT2I.dwFVF );
	g_sVB_PT2I.nFullByte	= g_sVB_PT2I.nVertexSize * 4 * PT2I_NUM;									// 28 Byte * 4 * 1000 = 112 KByte

	// Note : CreateVertexBuffer
	g_sVB_PNT.CreateVB( g_pd3dDevice, TRUE );
	g_sVB_PDT.CreateVB( g_pd3dDevice, TRUE );
	g_sVB_PDT_HARD_ALPHA.CreateVB( g_pd3dDevice, TRUE );
	g_sVB_PNTI.CreateVB( g_pd3dDevice, FALSE );
	g_sVB_I_24BYTE.CreateVB( g_pd3dDevice, FALSE );
    g_sVB_PT2I.CreateVB( g_pd3dDevice, FALSE );

	// 인덱스 버퍼 생성
	g_pd3dDevice->CreateIndexBuffer(
		sizeof( WORD ) * 6 * dwDynamicMaxFace,
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&g_pIBuffer,
		NULL );

	// 인덱스 버퍼 초기화
	WORD* pIndies;
	g_pIBuffer->Lock( 0, 0, (VOID**)&pIndies, 0 );
	for( DWORD i=0; i<dwDynamicMaxFace; ++i )
	{
		*pIndies++ = (WORD)( i * 4 + 0 );
		*pIndies++ = (WORD)( i * 4 + 1 );
		*pIndies++ = (WORD)( i * 4 + 2 );
		*pIndies++ = (WORD)( i * 4 + 1 );
		*pIndies++ = (WORD)( i * 4 + 3 );
		*pIndies++ = (WORD)( i * 4 + 2 );
	}
	g_pIBuffer->Unlock();

	return S_OK;
}

HRESULT DxDynamicVB::InvalidateDeviceObjects()
{
	g_sVB_PNT.ReleaseVB();
	g_sVB_PDT.ReleaseVB();
	g_sVB_PDT_HARD_ALPHA.ReleaseVB();
	g_sVB_PNTI.ReleaseVB();
    g_sVB_I_24BYTE.ReleaseVB();
    g_sVB_PT2I.ReleaseVB();

	SAFE_RELEASE( g_pIBuffer );

	return S_OK;
}

void DxDynamicVB::FrameMove()
{
    g_sVB_PNT.DiscardVB();
	g_sVB_PDT.DiscardVB();
	g_sVB_PDT_HARD_ALPHA.DiscardVB();
    g_sVB_PNTI.DiscardVB();
    g_sVB_I_24BYTE.DiscardVB();
    g_sVB_PT2I.DiscardVB();

	g_sVB_PNT.Clear();
	g_sVB_PDT.Clear();
	g_sVB_PDT_HARD_ALPHA.Clear();
    g_sVB_PNTI.Clear();
    g_sVB_I_24BYTE.Clear();
    g_sVB_PT2I.Clear();
}

//-----------------------------------------------------------------------------------------------------------------------------------
//											O	P	T	M		B	a	s	e
//-----------------------------------------------------------------------------------------------------------------------------------
LPDIRECT3DDEVICEQ		OPTMBase::g_pd3dDevice = NULL;

OPTMBase::OPTMBase(void) :
	m_pSavedSB(NULL),
	m_pDrawSB(NULL)
{
}

OPTMBase::~OPTMBase(void)
{
}

inline void OPTMBase::UserMatrix( D3DXMATRIX* pIdentity, D3DXMATRIX* pWorld, BOOL& bChange )
{
	if( pWorld )
	{
		bChange = TRUE;
		g_pd3dDevice->SetTransform( D3DTS_WORLD, pWorld );
	}
	else if( bChange )
	{
		bChange = FALSE;
		g_pd3dDevice->SetTransform( D3DTS_WORLD, pIdentity );
	}
}

inline void OPTMBase::UserRenderState( D3DRENDERSTATETYPE dwState, RenderState& sBase, const RenderState& sRS )
{
	if( sRS.m_bChange )
	{
		sBase.m_bChange = TRUE;
		g_pd3dDevice->SetRenderState( dwState,	sRS.m_dwValue );
	}
	else if( sBase.m_bChange )
	{
		sBase.m_bChange = FALSE;
		g_pd3dDevice->SetRenderState( dwState,	sBase.m_dwValue );
	}
}

inline void OPTMBase::UserTextureStageState( D3DTEXTURESTAGESTATETYPE dwType, TextureStageState& sBase, const TextureStageState& sTSS )
{
	if( sTSS.m_bChange )
	{
		sBase.m_bChange = TRUE;
		g_pd3dDevice->SetTextureStageState( 0, dwType,	sTSS.m_dwValue );
	}
	else if( sBase.m_bChange )
	{
		sBase.m_bChange = FALSE;
		g_pd3dDevice->SetTextureStageState( 0, dwType,	sBase.m_dwValue );
	}
}

//-----------------------------------------------------------------------------------------------------------------------------------
//							O	P	T	M		S	i	n	g	l	e		B	i	l	l	b	o	a	r	d
//-----------------------------------------------------------------------------------------------------------------------------------
OPTMSingleBillboard::OPTMSingleBillboard(void)
	: m_pDataHead(NULL)
	, m_pDataPool(NULL)
	, m_pBaseTexture(NULL)
{
}

OPTMSingleBillboard::~OPTMSingleBillboard(void)
{
}

void OPTMSingleBillboard::OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Note : 디바이스를 얻는다.
	g_pd3dDevice = pd3dDevice;

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		// 변하지 않음 ( 효과 Default 값. )
		pd3dDevice->SetRenderState( D3DRS_COLORVERTEX,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,			0x00000000 );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_ALPHAREF,			0x80 );

		// 계속 변하는 값 ( Default랑 틀림. 효과 Default 값. )
		pd3dDevice->SetRenderState( D3DRS_CULLMODE,			D3DCULL_CCW );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,		D3DTOP_SELECTARG1 );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,		D3DTOP_SELECTARG1 );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedSB );
		else			pd3dDevice->EndStateBlock( &m_pDrawSB );
	}

	m_pDataPool = new sc::ObjectPool<DATA>;
}

void OPTMSingleBillboard::OnLostDevice()
{
	SAFE_RELEASE( m_pSavedSB );
	SAFE_RELEASE( m_pDrawSB );

	// Note : memoryPOOL로 반환한다.
	DATA* pDel(NULL);
	DATA* pCur = m_pDataHead;
	while( pCur )
	{
		pDel = pCur;
		pCur = pCur->pNext;
		m_pDataPool->releaseMem( pDel );
	}

	// Note : 모두 반환. Head도 마찬가지.
	m_pDataHead = NULL;

	SAFE_DELETE( m_pDataPool );
}

void OPTMSingleBillboard::FrameMove()
{
	// Note : memoryPOOL로 반환한다.
	DATA* pDel(NULL);
	DATA* pCur = m_pDataHead;
	while( pCur )
	{
		pDel = pCur;
		pCur = pCur->pNext;
		m_pDataPool->releaseMem( pDel );
	}

	// Note : 모두 반환. Head도 마찬가지.
	m_pDataHead = NULL;
}

void OPTMSingleBillboard::Render( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Note : 디바이스를 얻는다.
	g_pd3dDevice = pd3dDevice;

	DxDynamicVB::g_sVB_PDT_HARD_ALPHA.UpdateVB();

	pd3dDevice->SetFVF( DxDynamicVB::g_sVB_PDT_HARD_ALPHA.dwFVF );
	pd3dDevice->SetStreamSource ( 0, DxDynamicVB::g_sVB_PDT_HARD_ALPHA.pVB, 0, DxDynamicVB::g_sVB_PDT_HARD_ALPHA.GetFVFSize() );

	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );

	m_pSavedSB->Capture();
	m_pDrawSB->Apply();

	m_pBaseTexture = NULL;
	pd3dDevice->SetTexture( 0, NULL );

	int nCount(0);
	DATA* pCur = m_pDataHead;
	while( pCur )
	{
		// Note : 텍스쳐가 같지 않을 경우만 바꾼다.
		if( m_pBaseTexture != pCur->pTex )
		{
			m_pBaseTexture = pCur->pTex;
			pd3dDevice->SetTexture( 0, pCur->pTex );
		}

		pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, pCur->nVertexCount, pCur->nFaceCount );

		pCur = pCur->pNext;

		++nCount;
	}

	CDebugSet::ToView( 5, 10, "%d", nCount );

	m_pSavedSB->Apply();

	// Note : memoryPOOL로 반환한다.
	DATA* pDel(NULL);
	pCur = m_pDataHead;
	while( pCur )
	{
		pDel = pCur;
		pCur = pCur->pNext;
		m_pDataPool->releaseMem( pDel );
	}

	// Note : 모두 반환. Head도 마찬가지.
	m_pDataHead = NULL;
}

void OPTMSingleBillboard::InsertData( DATA* pData )
{
	pData->pNext = m_pDataHead;
	m_pDataHead = pData;
}

void OPTMSingleBillboard::ReleaseData( DATA* pData )
{
	m_pDataPool->releaseMem( pData );
}

//-----------------------------------------------------------------------------------------------------------------------------------
//							O	P	T	M		S	i	n	g	l	e		S	e	q	u	e	n	c	e
//-----------------------------------------------------------------------------------------------------------------------------------
OPTMSingleSequence::OPTMSingleSequence(void) :
	m_pDataHead(NULL),
	m_pDataPool(NULL),
	m_bBaseMatrix(TRUE),
	m_pBaseTexture(NULL)
{
}

OPTMSingleSequence::~OPTMSingleSequence(void)
{
}

void OPTMSingleSequence::OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Note : 디바이스를 얻는다.
	g_pd3dDevice = pd3dDevice;

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		// 변하지 않음 ( 효과 Default 값. )
		pd3dDevice->SetRenderState( D3DRS_COLORVERTEX,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,			0x00000000 );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_ALPHAREF,			0x01 );

		// 계속 변하는 값 ( Default랑 틀림. 효과 Default 값. )
		pd3dDevice->SetRenderState( D3DRS_CULLMODE,			D3DCULL_CCW );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,		D3DTOP_MODULATE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,		D3DTOP_MODULATE );

		// 계속 변하는 값 ( Default로 설정해야 함. )
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_INVSRCALPHA );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedSB );
		else			pd3dDevice->EndStateBlock( &m_pDrawSB );
	}

	m_sBaseCullMode.m_dwValue			= D3DCULL_CCW;
	m_sBaseDestBlend.m_dwValue			= D3DBLEND_INVSRCALPHA;
	m_sBaseZWriteEnable.m_dwValue		= FALSE;
	m_sBaseAlphaBlendEnable.m_dwValue	= TRUE;
	m_sBaseColorOP.m_dwValue			= D3DTOP_MODULATE;

    m_pDataPool = new sc::ObjectPool<DATA>;
}

void OPTMSingleSequence::OnLostDevice()
{
	SAFE_RELEASE( m_pSavedSB );
	SAFE_RELEASE( m_pDrawSB );

	// Note : memoryPOOL로 반환한다.
	DATA* pDel(NULL);
	DATA* pCur = m_pDataHead;
	while( pCur )
	{
		pDel = pCur;
		pCur = pCur->pNext;
		m_pDataPool->releaseMem( pDel );
	}

	// Note : 모두 반환. Head도 마찬가지.
	m_pDataHead = NULL;

	SAFE_DELETE( m_pDataPool );
}

void OPTMSingleSequence::FrameMove()
{
	// Note : memoryPOOL로 반환한다.
	DATA* pDel(NULL);
	DATA* pCur = m_pDataHead;
	while( pCur )
	{
		pDel = pCur;
		pCur = pCur->pNext;
		m_pDataPool->releaseMem( pDel );
	}

	// Note : 모두 반환. Head도 마찬가지.
	m_pDataHead = NULL;
}

void OPTMSingleSequence::Render( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Note : 디바이스를 얻는다.
	g_pd3dDevice = pd3dDevice;

    DxDynamicVB::g_sVB_PDT.UpdateVB();

	pd3dDevice->SetFVF( DxDynamicVB::g_sVB_PDT.dwFVF );
	pd3dDevice->SetStreamSource ( 0, DxDynamicVB::g_sVB_PDT.pVB, 0, DxDynamicVB::g_sVB_PDT.GetFVFSize() );

	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );

	m_pSavedSB->Capture();
	m_pDrawSB->Apply();

	m_pBaseTexture = NULL;
	pd3dDevice->SetTexture( 0, NULL );

	int nCount(0);
	DATA* pCur = m_pDataHead;
	while( pCur )
	{
		if( pCur->bChangeMatrix )
		{
			m_bBaseMatrix = FALSE;
			pd3dDevice->SetTransform( D3DTS_WORLD, &pCur->matWorld );
		}
		else if( !m_bBaseMatrix )
		{
			m_bBaseMatrix = TRUE;
			pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );
		};

		UserRenderState( D3DRS_CULLMODE,			m_sBaseCullMode,			pCur->sCULLMODE );
		UserRenderState( D3DRS_DESTBLEND,			m_sBaseDestBlend,			pCur->sDESTBLEND );
		UserRenderState( D3DRS_ZWRITEENABLE,		m_sBaseZWriteEnable,		pCur->sZWRITEENABLE );
		UserRenderState( D3DRS_ALPHABLENDENABLE,	m_sBaseAlphaBlendEnable,	pCur->sALPHABLENDENABLE );

		UserTextureStageState( D3DTSS_COLOROP, m_sBaseColorOP, pCur->sCOLOROP );

		// Note : 텍스쳐가 같지 않을 경우만 바꾼다.
		if( m_pBaseTexture != pCur->pTex )
		{
			m_pBaseTexture = pCur->pTex;
			pd3dDevice->SetTexture( 0, pCur->pTex );
		}

		pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, pCur->nVertexCount, pCur->nFaceCount );

		pCur = pCur->pNext;

		++nCount;
	}

	CDebugSet::ToView( 5, 10, "%d", nCount );

	m_pSavedSB->Apply();

	// Note : memoryPOOL로 반환한다.
	DATA* pDel(NULL);
	pCur = m_pDataHead;
	while( pCur )
	{
		pDel = pCur;
		pCur = pCur->pNext;
		m_pDataPool->releaseMem( pDel );
	}

	// Note : 모두 반환. Head도 마찬가지.
	m_pDataHead = NULL;
}

void OPTMSingleSequence::InsertData( DATA* pData )
{
	pData->pNext = m_pDataHead;
	m_pDataHead = pData;
}

void OPTMSingleSequence::ReleaseData( DATA* pData )
{
    m_pDataPool->releaseMem( pData );
}

//-----------------------------------------------------------------------------------------------------------------------------------
//							O	P	T	M		S	i	n	g	l	e		G	r	o	u	n	d
//-----------------------------------------------------------------------------------------------------------------------------------
OPTMSingleGround::OPTMSingleGround(void) :
	m_pDataHead(NULL),
	m_pDataPool(NULL),
	m_pBaseTexture(NULL)
{
}

OPTMSingleGround::~OPTMSingleGround(void)
{
}

void OPTMSingleGround::OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Note : 디바이스를 얻는다.
	g_pd3dDevice = pd3dDevice;

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState( D3DRS_COLORVERTEX,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,			0x00000000 );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_ALPHAREF,			0x01 );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2,	D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );

		// 계속 변하는 값 ( Default로 설정해야 함. )
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_INVSRCALPHA );

		////if( d3dCaps.AlphaCmpCaps & D3DPCMPCAPS_GREATEREQUAL )
		//{
		//	pd3dDevice->SetRenderState( D3DRS_ALPHAREF,	0x80 );
		//	pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
		//}

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedSB );
		else			pd3dDevice->EndStateBlock( &m_pDrawSB );
	}

	m_sBaseALPHAREF.m_dwValue			= 0x01;
	m_sBaseLighting.m_dwValue			= FALSE;
	m_sBaseDestBlend.m_dwValue			= D3DBLEND_INVSRCALPHA;
	m_sBaseZWriteEnable.m_dwValue		= FALSE;
	m_sBaseAlphaBlendEnable.m_dwValue	= TRUE;
	m_sBaseColorOP.m_dwValue			= D3DTOP_MODULATE;

    m_pDataPool = new sc::ObjectPool<DATA>;
}

void OPTMSingleGround::OnLostDevice()
{
	SAFE_RELEASE( m_pSavedSB );
	SAFE_RELEASE( m_pDrawSB );

	// Note : memoryPOOL로 반환한다.
	DATA* pDel(NULL);
	DATA* pCur = m_pDataHead;
	while( pCur )
	{
		pDel = pCur;
		pCur = pCur->pNext;
		m_pDataPool->releaseMem( pDel );
	}

	// Note : 모두 반환. Head도 마찬가지.
	m_pDataHead = NULL;

	SAFE_DELETE( m_pDataPool );
}

void OPTMSingleGround::FrameMove()
{
	// Note : memoryPOOL로 반환한다.
	DATA* pDel(NULL);
	DATA* pCur = m_pDataHead;
	while( pCur )
	{
		pDel = pCur;
		pCur = pCur->pNext;
		m_pDataPool->releaseMem( pDel );
	}

	// Note : 모두 반환. Head도 마찬가지.
	m_pDataHead = NULL;
}

void OPTMSingleGround::Render( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Note : 디바이스를 얻는다.
	g_pd3dDevice = pd3dDevice;

	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );

	m_pSavedSB->Capture();
	m_pDrawSB->Apply();

	m_pBaseTexture = NULL;
	pd3dDevice->SetTexture( 0, NULL );

	DATA* pCur = m_pDataHead;
	while( pCur )
	{
        DxDynamicVB::BIGVB* pBigVB = NULL;
        if( pCur->dwFVF == DxDynamicVB::g_sVB_PDT.dwFVF )
        {
            pBigVB = &DxDynamicVB::g_sVB_PDT;
        }
        else if( pCur->dwFVF == DxDynamicVB::g_sVB_PNT.dwFVF )
        {
            pBigVB = &DxDynamicVB::g_sVB_PNT;
        }
        else
        {
            MIN_ASSERT( 0 );
        }

        pBigVB->UpdateVB();

		pd3dDevice->SetFVF( pCur->dwFVF );
		pd3dDevice->SetStreamSource( 0, pBigVB->pVB, 0, pBigVB->nVertexSize );

        pd3dDevice->SetTransform( D3DTS_WORLD, &pCur->matWorld );

		UserRenderState( D3DRS_ALPHAREF,			m_sBaseLighting,			pCur->sALPHAREF );
		UserRenderState( D3DRS_LIGHTING,			m_sBaseLighting,			pCur->sLIGHTING );
		UserRenderState( D3DRS_DESTBLEND,			m_sBaseDestBlend,			pCur->sDESTBLEND );
		UserRenderState( D3DRS_ZWRITEENABLE,		m_sBaseZWriteEnable,		pCur->sZWRITEENABLE );
		UserRenderState( D3DRS_ALPHABLENDENABLE,	m_sBaseAlphaBlendEnable,	pCur->sALPHABLENDENABLE );

		UserTextureStageState( D3DTSS_COLOROP, m_sBaseColorOP, pCur->sCOLOROP );

		// Note : 텍스쳐가 같지 않을 경우만 바꾼다.
		if( m_pBaseTexture != pCur->pTex )
		{
			m_pBaseTexture = pCur->pTex;
			pd3dDevice->SetTexture( 0, pCur->pTex );
		}

		// Note : Material Setting
		if( pCur->pMaterial )	pd3dDevice->SetMaterial( pCur->pMaterial );

		pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, pCur->nVertexCount, pCur->nFaceCount );

		pCur = pCur->pNext;
	}

	m_pSavedSB->Apply();

	// Note : memoryPOOL로 반환한다.
	DATA* pDel(NULL);
	pCur = m_pDataHead;
	while( pCur )
	{
		pDel = pCur;
		pCur = pCur->pNext;
		m_pDataPool->releaseMem( pDel );
	}

	// Note : 모두 반환. Head도 마찬가지.
	m_pDataHead = NULL;
}

void OPTMSingleGround::InsertData( DATA* pData )
{
	pData->pNext = m_pDataHead;
	m_pDataHead = pData;
}

void OPTMSingleGround::ReleaseData( DATA* pData )
{
    m_pDataPool->releaseMem( pData );
}

//-----------------------------------------------------------------------------------------------------------------------------------
//							O	P	T	M		S	i	n	g	l	e		B	l	u	r	S	y	s
//-----------------------------------------------------------------------------------------------------------------------------------
OPTMSingleBlurSys::OPTMSingleBlurSys(void) :
	m_pDataHead(NULL),
	m_pDataPool(NULL),
	m_pBaseTexture(NULL),
	m_bMatrixChange(FALSE)
{
}

OPTMSingleBlurSys::~OPTMSingleBlurSys(void)
{
}

void OPTMSingleBlurSys::OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Note : 디바이스를 얻는다.
	g_pd3dDevice = pd3dDevice;

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,			0x00000000 );
		pd3dDevice->SetRenderState( D3DRS_CULLMODE,			D3DCULL_NONE );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_ALPHAREF,			0x01 );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );

		// 계속 변하는 값 ( Default로 설정해야 함. )
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_INVSRCALPHA );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedSB );
		else			pd3dDevice->EndStateBlock( &m_pDrawSB );
	}

	m_sBaseDestBlend.m_dwValue			= D3DBLEND_INVSRCALPHA;
	m_sBaseZWriteEnable.m_dwValue		= FALSE;
	m_sBaseAlphaBlendEnable.m_dwValue	= TRUE;
	m_sBaseColorOP.m_dwValue			= D3DTOP_MODULATE;

    m_pDataPool = new sc::ObjectPool<DATA>;
}

void OPTMSingleBlurSys::OnLostDevice()
{
	SAFE_RELEASE( m_pSavedSB );
	SAFE_RELEASE( m_pDrawSB );

	// Note : memoryPOOL로 반환한다.
	DATA* pDel(NULL);
	DATA* pCur = m_pDataHead;
	while( pCur )
	{
		pDel = pCur;
		pCur = pCur->pNext;
		m_pDataPool->releaseMem( pDel );
	}

	// Note : 모두 반환. Head도 마찬가지.
	m_pDataHead = NULL;

	SAFE_DELETE( m_pDataPool );
}

void OPTMSingleBlurSys::FrameMove()
{
	// Note : memoryPOOL로 반환한다.
	DATA* pDel(NULL);
	DATA* pCur = m_pDataHead;
	while( pCur )
	{
		pDel = pCur;
		pCur = pCur->pNext;
		m_pDataPool->releaseMem( pDel );
	}

	// Note : 모두 반환. Head도 마찬가지.
	m_pDataHead = NULL;
}

void OPTMSingleBlurSys::Render( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Note : 디바이스를 얻는다.
	g_pd3dDevice = pd3dDevice;

	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );

	m_pSavedSB->Capture();
	m_pDrawSB->Apply();

	m_pBaseTexture = NULL;
	pd3dDevice->SetTexture( 0, NULL );

    DxDynamicVB::g_sVB_PDT.UpdateVB();

	pd3dDevice->SetFVF( DxDynamicVB::g_sVB_PDT.dwFVF );
	pd3dDevice->SetStreamSource ( 0, DxDynamicVB::g_sVB_PDT.pVB, 0, DxDynamicVB::g_sVB_PDT.GetFVFSize() );

	DATA* pCur = m_pDataHead;
	while( pCur )
	{
		UserMatrix( &matIdentity, pCur->pmatWorld, m_bMatrixChange );

		UserRenderState( D3DRS_DESTBLEND,			m_sBaseDestBlend,			pCur->sDESTBLEND );
		UserRenderState( D3DRS_ZWRITEENABLE,		m_sBaseZWriteEnable,		pCur->sZWRITEENABLE );
		UserRenderState( D3DRS_ALPHABLENDENABLE,	m_sBaseAlphaBlendEnable,	pCur->sALPHABLENDENABLE );

		UserTextureStageState( D3DTSS_COLOROP, m_sBaseColorOP, pCur->sCOLOROP );

		// Note : 텍스쳐가 같지 않을 경우만 바꾼다.
		if( m_pBaseTexture != pCur->pTex )
		{
			m_pBaseTexture = pCur->pTex;
			pd3dDevice->SetTexture( 0, pCur->pTex );
		}

		pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, pCur->nVertexCount, pCur->nFaceCount );

		pCur = pCur->pNext;
	}

	m_pSavedSB->Apply();

	// Note : memoryPOOL로 반환한다.
	DATA* pDel(NULL);
	pCur = m_pDataHead;
	while( pCur )
	{
		pDel = pCur;
		pCur = pCur->pNext;
		m_pDataPool->releaseMem( pDel );
	}

	// Note : 모두 반환. Head도 마찬가지.
	m_pDataHead = NULL;
}

void OPTMSingleBlurSys::InsertData( DATA* pData )
{
	pData->pNext = m_pDataHead;
	m_pDataHead = pData;
}

void OPTMSingleBlurSys::ReleaseData( DATA* pData )
{
    m_pDataPool->releaseMem( pData );
}

//-----------------------------------------------------------------------------------------------------------------------------------
//							O	P	T	M		S	i	n	g	l	e		P	a	r	t	i	c	l	e
//-----------------------------------------------------------------------------------------------------------------------------------
OPTMSingleParticle::OPTMSingleParticle(void) :
	m_pDataHead(NULL),
	m_pDataPool(NULL),
	m_bBaseMatrix(TRUE),
	m_pBaseTexture(NULL)
{
}

OPTMSingleParticle::~OPTMSingleParticle(void)
{
}

void OPTMSingleParticle::OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Note : 디바이스를 얻는다.
	g_pd3dDevice = pd3dDevice;

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		// 변하지 않음 ( 효과 Default 값. )
		pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,			0x00000000 );
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_ALPHAREF,			0x01 );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );

		// 계속 변하는 값 ( Default랑 틀림. 효과 Default 값. )
		pd3dDevice->SetRenderState( D3DRS_CULLMODE,			D3DCULL_CCW );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_BLENDOP,			D3DBLENDOP_ADD );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,			FALSE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2,	D3DTA_DIFFUSE );

		// 계속 변하는 값 ( Default로 설정해야 함. )
		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xffffffff );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_INVSRCALPHA );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedSB );
		else			pd3dDevice->EndStateBlock( &m_pDrawSB );
	}

	m_sBaseCullMode.m_dwValue			= D3DCULL_CCW;
	m_sBaseDestBlend.m_dwValue			= D3DBLEND_INVSRCALPHA;
	m_sBaseZWriteEnable.m_dwValue		= FALSE;
	m_sBaseAlphaBlendEnable.m_dwValue	= TRUE;
	m_sBaseColorOP.m_dwValue			= D3DTOP_MODULATE;

    m_pDataPool = new sc::ObjectPool<DATA>;
}

void OPTMSingleParticle::OnLostDevice()
{
	SAFE_RELEASE( m_pSavedSB );
	SAFE_RELEASE( m_pDrawSB );

	// Note : memoryPOOL로 반환한다.
	DATA* pDel(NULL);
	DATA* pCur = m_pDataHead;
	while( pCur )
	{
		pDel = pCur;
		pCur = pCur->pNext;
		m_pDataPool->releaseMem( pDel );
	}

	// Note : 모두 반환. Head도 마찬가지.
	m_pDataHead = NULL;

	SAFE_DELETE( m_pDataPool );
}

void OPTMSingleParticle::FrameMove()
{
	// Note : memoryPOOL로 반환한다.
	DATA* pDel(NULL);
	DATA* pCur = m_pDataHead;
	while( pCur )
	{
		pDel = pCur;
		pCur = pCur->pNext;
		m_pDataPool->releaseMem( pDel );
	}

	// Note : 모두 반환. Head도 마찬가지.
	m_pDataHead = NULL;
}

void OPTMSingleParticle::Render( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Note : 디바이스를 얻는다.
	g_pd3dDevice = pd3dDevice;

	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );

	m_pSavedSB->Capture();
	m_pDrawSB->Apply();

	m_pBaseTexture = NULL;
	pd3dDevice->SetTexture( 0, NULL );

    DxDynamicVB::g_sVB_PDT.UpdateVB();

	pd3dDevice->SetFVF( DxDynamicVB::g_sVB_PDT.dwFVF );
	pd3dDevice->SetStreamSource ( 0, DxDynamicVB::g_sVB_PDT.pVB, 0, DxDynamicVB::g_sVB_PDT.GetFVFSize() );

	DATA* pCur = m_pDataHead;
	while( pCur )
	{
		if( pCur->bChangeMatrix )
		{
			m_bBaseMatrix = FALSE;
			pd3dDevice->SetTransform( D3DTS_WORLD, &pCur->matWorld );
		}
		else if( !m_bBaseMatrix )
		{
			m_bBaseMatrix = TRUE;
			pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );
		};

		UserRenderState( D3DRS_CULLMODE,			m_sBaseCullMode,			pCur->sCULLMODE );
		UserRenderState( D3DRS_DESTBLEND,			m_sBaseDestBlend,			pCur->sDESTBLEND );
		UserRenderState( D3DRS_ZWRITEENABLE,		m_sBaseZWriteEnable,		pCur->sZWRITEENABLE );
		UserRenderState( D3DRS_ALPHABLENDENABLE,	m_sBaseAlphaBlendEnable,	pCur->sALPHABLENDENABLE );

		UserTextureStageState( D3DTSS_COLOROP, m_sBaseColorOP, pCur->sCOLOROP );

		// Note : 텍스쳐가 같지 않을 경우만 바꾼다.
		if( m_pBaseTexture != pCur->pTex )
		{
			m_pBaseTexture = pCur->pTex;
			pd3dDevice->SetTexture( 0, pCur->pTex );
		}

		pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, pCur->nVertexCount, pCur->nFaceCount );

		pCur = pCur->pNext;
	}

	m_pSavedSB->Apply();

	// Note : memoryPOOL로 반환한다.
	DATA* pDel(NULL);
	pCur = m_pDataHead;
	while( pCur )
	{
		pDel = pCur;
		pCur = pCur->pNext;
		m_pDataPool->releaseMem( pDel );
	}

	// Note : 모두 반환. Head도 마찬가지.
	m_pDataHead = NULL;
}

void OPTMSingleParticle::InsertData( DATA* pData )
{
	pData->pNext = m_pDataHead;
	m_pDataHead = pData;
}

void OPTMSingleParticle::ReleaseData( DATA* pData )
{
    m_pDataPool->releaseMem( pData );
}

//-----------------------------------------------------------------------------------------------------------------------------------
//							O	P	T	M		S	i	n	g	l	e		P	a	r	t	i	c	l	e
//-----------------------------------------------------------------------------------------------------------------------------------
OPTMCharParticle::OPTMCharParticle(void) :
	m_pDataHead(NULL),
	m_pDataPool(NULL),
	m_pBaseTexture(NULL)
{
}

OPTMCharParticle::~OPTMCharParticle(void)
{
}

void OPTMCharParticle::OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Note : 디바이스를 얻는다.
	g_pd3dDevice = pd3dDevice;

	float fBias(-0.00002f);
	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState( D3DRS_DEPTHBIAS,		*((DWORD*)&fBias) );
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_ONE );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_ONE );

		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,			FALSE );
		
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		D3DTOP_MODULATE );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_ALPHAOP,		D3DTOP_DISABLE );

		//pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1,		D3DTA_TEXTURE );
		//pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2,		D3DTA_DIFFUSE );

		//pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,		D3DTOP_DISABLE );
		//pd3dDevice->SetTextureStageState ( 1, D3DTSS_ALPHAOP,		D3DTOP_DISABLE );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedSB );
		else			pd3dDevice->EndStateBlock( &m_pDrawSB );
	}

    m_pDataPool = new sc::ObjectPool<DATA>;
}

void OPTMCharParticle::OnLostDevice()
{
	SAFE_RELEASE( m_pSavedSB );
	SAFE_RELEASE( m_pDrawSB );

	// Note : memoryPOOL로 반환한다.
	DATA* pDel(NULL);
	DATA* pCur = m_pDataHead;
	while( pCur )
	{
		pDel = pCur;
		pCur = pCur->pNext;
		m_pDataPool->releaseMem( pDel );
	}

	// Note : 모두 반환. Head도 마찬가지.
	m_pDataHead = NULL;

	SAFE_DELETE( m_pDataPool );
}

void OPTMCharParticle::FrameMove()
{
	// Note : memoryPOOL로 반환한다.
	DATA* pDel(NULL);
	DATA* pCur = m_pDataHead;
	while( pCur )
	{
		pDel = pCur;
		pCur = pCur->pNext;
		m_pDataPool->releaseMem( pDel );
	}

	// Note : 모두 반환. Head도 마찬가지.
	m_pDataHead = NULL;
}

void OPTMCharParticle::Render( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Note : 디바이스를 얻는다.
	g_pd3dDevice = pd3dDevice;

	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );

	m_pSavedSB->Capture();
	m_pDrawSB->Apply();

	m_pBaseTexture = NULL;
	pd3dDevice->SetTexture( 0, NULL );

    DxDynamicVB::g_sVB_PDT.UpdateVB();

	pd3dDevice->SetFVF( DxDynamicVB::g_sVB_PDT.dwFVF );
	pd3dDevice->SetStreamSource( 0, DxDynamicVB::g_sVB_PDT.pVB, 0, DxDynamicVB::g_sVB_PDT.GetFVFSize() );
	pd3dDevice->SetIndices( DxDynamicVB::g_pIBuffer );

	DATA* pCur = m_pDataHead;
	while( pCur )
	{
		// Note : 텍스쳐가 같지 않을 경우만 바꾼다.
		if( m_pBaseTexture != pCur->pTex )
		{
			m_pBaseTexture = pCur->pTex;
			pd3dDevice->SetTexture( 0, pCur->pTex );
		}

		DWORD dwState_SrcBlend, dwState_DestBlend, dwState_AlphaOp;
		
		//pd3dDevice->GetRenderState( D3DRS_DEPTHBIAS, &dwState_Depth );
		pd3dDevice->GetRenderState( D3DRS_SRCBLEND, &dwState_SrcBlend );
		pd3dDevice->GetRenderState( D3DRS_DESTBLEND, &dwState_DestBlend );
		pd3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAOP, &dwState_AlphaOp );

		if( pCur->bAlphaTex )
		{
			//float fBias(0.0002f);
			//pd3dDevice->SetRenderState( D3DRS_DEPTHBIAS,		*((DWORD*)&fBias) );

			pd3dDevice->SetRenderState( D3DRS_SRCBLEND,				D3DBLEND_SRCALPHA );
			pd3dDevice->SetRenderState( D3DRS_DESTBLEND,			D3DBLEND_ONE );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );
		}

		pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, pCur->nVertexCount, 0, pCur->nFaceCount*4, 0, pCur->nFaceCount*2 );

		if ( pCur->bAlphaTex ) 
		{
			//pd3dDevice->SetRenderState( D3DRS_DEPTHBIAS,		*((DWORD*)&dwState_Depth) );
			pd3dDevice->SetRenderState( D3DRS_SRCBLEND,				dwState_SrcBlend );
			pd3dDevice->SetRenderState( D3DRS_DESTBLEND,			dwState_DestBlend );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	dwState_AlphaOp );
		}		

		pCur = pCur->pNext;
	}

	m_pSavedSB->Apply();

	// Note : memoryPOOL로 반환한다.
	DATA* pDel(NULL);
	pCur = m_pDataHead;
	while( pCur )
	{
		pDel = pCur;
		pCur = pCur->pNext;
		m_pDataPool->releaseMem( pDel );
	}

	// Note : 모두 반환. Head도 마찬가지.
	m_pDataHead = NULL;
}

void OPTMCharParticle::InsertData( DATA* pData )
{
	pData->pNext = m_pDataHead;
	m_pDataHead = pData;
}

void OPTMCharParticle::ReleaseData( DATA* pData )
{
    m_pDataPool->releaseMem( pData );
}

//-----------------------------------------------------------------------------------------------------------------------------------
//											O	P	T	M		M	a	n	a	g	e	r
//-----------------------------------------------------------------------------------------------------------------------------------
OPTMManager& OPTMManager::GetInstance()
{
	static OPTMManager Instance;
	return Instance;
}

OPTMManager::OPTMManager(void)
    : m_pd3dDevice( NULL )
{
}

OPTMManager::~OPTMManager(void)
{
}

void OPTMManager::OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
    m_pd3dDevice = pd3dDevice;

	m_sSingleParticle.OnResetDevice( pd3dDevice );
	m_sSingleBillboard.OnResetDevice( pd3dDevice );
	m_sSingleSequence.OnResetDevice( pd3dDevice );
	m_sSingleGround.OnResetDevice( pd3dDevice );
	m_sSingleBlurSys.OnResetDevice( pd3dDevice );
	m_sCharParticle.OnResetDevice( pd3dDevice );
}

void OPTMManager::OnLostDevice()
{
	m_sSingleParticle.OnLostDevice();
	m_sSingleBillboard.OnLostDevice();
	m_sSingleSequence.OnLostDevice();
	m_sSingleGround.OnLostDevice();
	m_sSingleBlurSys.OnLostDevice();
	m_sCharParticle.OnLostDevice();
}

void OPTMManager::FrameMove()
{
    DxDynamicVB::FrameMove();

	m_sSingleParticle.FrameMove();
	m_sSingleBillboard.FrameMove();
	m_sSingleSequence.FrameMove();
	m_sSingleGround.FrameMove();
	m_sSingleBlurSys.FrameMove();
	m_sCharParticle.FrameMove();
}

void OPTMManager::Render_HardAlpha()
{
	if( !GLOBAL_PARAM::IsValue( GP_EFFECT ) )
		return;

	m_sSingleBillboard.Render( m_pd3dDevice );
}

void OPTMManager::Render()
{
    if( !GLOBAL_PARAM::IsValue( GP_EFFECT ) )
		return;

	m_sSingleParticle.Render( m_pd3dDevice );
	m_sSingleSequence.Render( m_pd3dDevice );
	m_sSingleGround.Render( m_pd3dDevice );
	m_sSingleBlurSys.Render( m_pd3dDevice );
	m_sCharParticle.Render( m_pd3dDevice );
}