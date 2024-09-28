#include "stdafx.h"
#include "UIEdit.h"
#include "ViewDialog.h"

#include "../EngineLib/GUInterface/UIDataType.h"
#include "../EngineLib/TextTexture/TextUtil.h"

#include "UIEditDoc.h"
#include "UIEditView.h"
#include "MainFrm.h"

// CViewDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(CViewDialog, CDialog)

CViewDialog::CViewDialog(CWnd* pParent /*=NULL*/)
: CDialog(CViewDialog::IDD, pParent)
, m_nSTART_POS_X(0)
, m_nSTART_POS_Y(0)
, m_nTEXTURE_START_POS_X(0)
, m_nTEXTURE_START_POS_Y(0)
, m_nTEXTURE_SIZE_WIDTH(1024)
, m_nTEXTURE_SIZE_HEIGHT(1024)
, m_fMultply(1.0f)
, m_cRect( 0, 0, 0, 0 )
, m_cLimitRect( 0, 0, 0, 0)
, m_BgColor( D3DCOLOR_XRGB( 0,255,255 ) )
{
}

CViewDialog::~CViewDialog()
{
}

VOID CViewDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CViewDialog, CDialog)
END_MESSAGE_MAP()

// CViewDialog 메시지 처리기입니다.
VOID CViewDialog::Render()
{
	CMainFrame* pFrame = ( CMainFrame* )AfxGetApp()->m_pMainWnd;
	if( !pFrame )
		return;

	CUIEditView* pView = ( CUIEditView* )pFrame->GetActiveView();
	if( !pView )
		return;

	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3dDevice();
	if( !pd3dDevice )
		return;

	LPDIRECT3DSWAPCHAIN9 pSwapChain;
	LPDIRECT3DSWAPCHAIN9 tempSwapChain;

	//LPDIRECT3DSWAPCHAIN9 pSwapChain = pView->m_pSwapChainTextureView;
	pd3dDevice->GetSwapChain( 0, &pSwapChain );
	pSwapChain = pView->m_pSwapChainTextureView;
	tempSwapChain = pView->m_pSwapChainTextureView;

	D3DPRESENT_PARAMETERS tempPram;
	pSwapChain->GetPresentParameters( &tempPram );

	D3DPRESENT_PARAMETERS presentParams;
	ZeroMemory( &presentParams, sizeof( presentParams ) );

	presentParams.Windowed = TRUE;
	presentParams.SwapEffect = D3DSWAPEFFECT_COPY;
	presentParams.BackBufferWidth = 1024;
	presentParams.BackBufferHeight = 800;
	presentParams.BackBufferFormat = D3DFORMAT( D3DADAPTER_DEFAULT );

	SAFE_RELEASE( pSwapChain );
	pd3dDevice->CreateAdditionalSwapChain( &presentParams, &pSwapChain );

	if( !pSwapChain )
		return;

	LPDIRECT3DSURFACE9 pBackBuffer = NULL;
	LPDIRECT3DSURFACE9 pBackBuffer2 = NULL;
	pSwapChain->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );

	pd3dDevice->SetRenderTarget( 0, pBackBuffer );
	pd3dDevice->Clear( 0, 0, D3DCLEAR_TARGET, m_BgColor, 1.0f, 0 );

	if( SUCCEEDED( pd3dDevice->BeginScene() ) )
	{
		//CTextUtil::Get()->SetStateBlock( TRUE );

		DrawSprite( pd3dDevice );
		DrawLimitRect( pd3dDevice );
		DrawRect( pd3dDevice );

		//CTextUtil::Get()->SetStateBlock( FALSE );

		pd3dDevice->EndScene();

		pSwapChain->Present( NULL, NULL, m_hWnd, NULL, 0 );
		//pd3dDevice->Present( NULL, NULL, this->m_hWnd, NULL );
	}

	pBackBuffer->Release();
}

VOID CViewDialog::DrawSprite( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( !m_textureRes.GetTexture() )
		return;

	UIVERTEX vVertexArray[ 4 ]; 
	SetVertexPos( vVertexArray,	m_nSTART_POS_X + m_cLimitRect.left, m_nSTART_POS_Y + m_cLimitRect.top,m_cLimitRect.right, m_cLimitRect.bottom );
	SetTexturePos( vVertexArray, 0.0f, 0.0f, 1.0f, 1.0f );

	pd3dDevice->SetTexture( 0, m_textureRes.GetTexture() );
	pd3dDevice->SetFVF( UIVERTEX::D3DFVF_UIVERTEX );
	pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, vVertexArray, sizeof( UIVERTEX ) );
	pd3dDevice->SetTexture( 0, NULL );
}

VOID CViewDialog::DrawRect( LPDIRECT3DDEVICEQ pd3dDevice )
{
	UIVERTEX stLine[ 5 ];
	for( INT i=0; i<5; ++i )
		stLine[ i ].SetDiffuse( D3DCOLOR_ARGB( 255,255,0,0 ) );

	stLine[ 0 ].SetPos( D3DXVECTOR2( m_cRect.left + m_nSTART_POS_X, m_cRect.top  + m_nSTART_POS_Y) );
	stLine[ 1 ].SetPos( D3DXVECTOR2( m_cRect.right + m_nSTART_POS_X, m_cRect.top + m_nSTART_POS_Y ) );
	stLine[ 2 ].SetPos( D3DXVECTOR2( m_cRect.right + m_nSTART_POS_X, m_cRect.bottom + m_nSTART_POS_Y ) );
	stLine[ 3 ].SetPos( D3DXVECTOR2( m_cRect.left + m_nSTART_POS_X, m_cRect.bottom + m_nSTART_POS_Y) );
	stLine[ 4 ].SetPos( D3DXVECTOR2( m_cRect.left + m_nSTART_POS_X, m_cRect.top + m_nSTART_POS_Y ) );

	pd3dDevice->SetTexture( 0, NULL );
	pd3dDevice->SetFVF( UIVERTEX::D3DFVF_UIVERTEX );
	pd3dDevice->DrawPrimitiveUP( D3DPT_LINESTRIP, 4, stLine, sizeof( UIVERTEX ) );
}

VOID CViewDialog::DrawLimitRect( LPDIRECT3DDEVICEQ pd3dDevice )
{
	UIVERTEX stLine[ 5 ];
	for( INT i=0; i<5; ++i )
		stLine[ i ].SetDiffuse( D3DCOLOR_ARGB( 255,0,0,0 ) );

	stLine[ 0 ].SetPos( D3DXVECTOR2( m_cLimitRect.left + m_nSTART_POS_X, m_cLimitRect.top  + m_nSTART_POS_Y) );
	stLine[ 1 ].SetPos( D3DXVECTOR2( m_cLimitRect.right + m_nSTART_POS_X, m_cLimitRect.top + m_nSTART_POS_Y ) );
	stLine[ 2 ].SetPos( D3DXVECTOR2( m_cLimitRect.right + m_nSTART_POS_X, m_cLimitRect.bottom + m_nSTART_POS_Y ) );
	stLine[ 3 ].SetPos( D3DXVECTOR2( m_cLimitRect.left + m_nSTART_POS_X, m_cLimitRect.bottom + m_nSTART_POS_Y) );
	stLine[ 4 ].SetPos( D3DXVECTOR2( m_cLimitRect.left + m_nSTART_POS_X, m_cLimitRect.top + m_nSTART_POS_Y ) );

	pd3dDevice->SetTexture( 0, NULL );
	pd3dDevice->SetFVF( UIVERTEX::D3DFVF_UIVERTEX );
	pd3dDevice->DrawPrimitiveUP( D3DPT_LINESTRIP, 4, stLine, sizeof( UIVERTEX ) );
}

VOID CViewDialog::SetStartPos(int nX, int nY)
{
	m_nSTART_POS_X = nX;
	m_nSTART_POS_Y = nY;
}

VOID CViewDialog::SetTextureStartPos(int nX, int nY)
{
	m_nTEXTURE_START_POS_X = nX;
	m_nTEXTURE_START_POS_Y = nY;
}
VOID CViewDialog::SetTextureSize(int nWidth, int nHeight)
{
	m_nTEXTURE_SIZE_WIDTH = nWidth;
	m_nTEXTURE_SIZE_HEIGHT = nHeight;
}

VOID CViewDialog::SetMultiply(float fMultply)
{
	m_fMultply = fMultply;
}

VOID CViewDialog::CloseWindow()
{
	//m_textureRes.Clear();
}

VOID CViewDialog::SetBgColor(UINT r, UINT g, UINT b)
{
	m_BgColor = D3DCOLOR_XRGB(r, g, b);
}

D3DCOLOR CViewDialog::GetBgColor()
{
	return m_BgColor;
}

VOID CViewDialog::SetVertexPos( 
							   UIVERTEX* pVertexArray, 
							   FLOAT LEFT, 
							   FLOAT TOP, 
							   FLOAT SIZEX, 
							   FLOAT SIZEY )
{
	FLOAT RIGHT = LEFT + SIZEX;
	FLOAT BOTTOM = TOP + SIZEY;

	pVertexArray[ 0 ] = D3DXVECTOR2( LEFT - 0.5f, TOP - 0.5f );
	pVertexArray[ 1 ] = D3DXVECTOR2( RIGHT - 0.5f, TOP - 0.5f );
	pVertexArray[ 2 ] = D3DXVECTOR2( RIGHT - 0.5f, BOTTOM - 0.5f );
	pVertexArray[ 3 ] = D3DXVECTOR2( LEFT - 0.5f, BOTTOM - 0.5f );
}

VOID CViewDialog::SetTexturePos( 
								UIVERTEX* pVertexArray, 
								FLOAT LEFT, 
								FLOAT TOP, 
								FLOAT RIGHT, 
								FLOAT BOTTOM )
{
	pVertexArray[ 0 ].SetTexturePos( LEFT , TOP  );
	pVertexArray[ 1 ].SetTexturePos( RIGHT , TOP  );
	pVertexArray[ 2 ].SetTexturePos( RIGHT , BOTTOM  );
	pVertexArray[ 3 ].SetTexturePos( LEFT , BOTTOM  );
}