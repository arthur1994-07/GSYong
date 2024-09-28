#include "stdafx.h"
#include "../ViewPort/DxViewPortVM.h"

#include "DxEditVM.h"

// ----------------------------------------------------------------------------
#include "../../InternalCommonLib/debuginclude.h"
// ----------------------------------------------------------------------------



namespace
{
    // Channel 글씨관련 
    float CHANNEL_EMPTY_RIGHT   = 0.1f; // 오른쪽을 살짝 떨어뜨린다.
    float CHANNEL_EMPTY_HEIGHT  = 1.f;  // 상하단 떨어뜨리는 범위

    // InPut Box Size
    float INPUT_BOX_SIZE        = 0.6f;                 // 박스의 사이즈
    float INPUT_BOX_SIZE_HALF   = INPUT_BOX_SIZE*0.5f;  // 박스의 사이즈 절반
    float INPUT_EMPTY_HEIGHT    = 0.3f;                 // 사이를 띄어놓는다.

    // Image Size
    float IMAGE_SIZE            = 5.f;  // Image

    // Color 설정
    DWORD COLOR_BACKGROUND_TITLE_   = 0xff555555; // BackGround Color // Title
    DWORD COLOR_BACKGROUND_CHANNEL  = 0xff888888; // BackGround Color // CHANNEL
    DWORD COLOR_TEXT_TITLE          = 0xffffff00; // Text Color // Title
    DWORD COLOR_TEXT_CHANNEL        = 0xffffffff; // Text Color // CHANNEL
};


//-----------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------- DxEditVMText
//-----------------------------------------------------------------------------------------------------------
DxEditVMText::DxEditVMText()
    : m_strText(_T(""))
    , m_pMesh(NULL)
    , m_sMeshRect(0.f,0.f,0.f,0.f)
{
}

DxEditVMText::~DxEditVMText()
{
    SAFE_RELEASE( m_pMesh );
}

float DxEditVMText::GetWidth()
{
    return m_sMeshRect.w - m_sMeshRect.z;
}

float DxEditVMText::GetHeight()
{
    return m_sMeshRect.x - m_sMeshRect.y;
}

void DxEditVMText::Render( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR2& vPos, float fWidth, float fHeight, DWORD dwFlags )
{
    if( !m_pMesh )
        return;

    D3DXMATRIX matWorld;
    D3DXMatrixIdentity( &matWorld );
    matWorld._41 = vPos.x - m_sMeshRect.z;
    matWorld._42 = vPos.y - m_sMeshRect.x;

    // 가로 센터정렬
    if ( dwFlags & DEVM_CENTER_V )
    {
        matWorld._41 += fWidth * 0.5f;
        matWorld._41 -= GetWidth() * 0.5f;
    }
    // 우측으로 붙임.
    else if ( dwFlags & DEVM_RIGHT )
    {
        matWorld._41 += fWidth;
        matWorld._41 -= GetWidth();
    }

    // 세로 센터정렬
    if ( dwFlags & DEVM_CENTER_H )
    {
        matWorld._42 -= fHeight * 0.5f;
        matWorld._42 += GetHeight() * 0.5f;
    }

    pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

    m_pMesh->DrawSubset( 0 );
}


//-----------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------ DxEditVM
//-----------------------------------------------------------------------------------------------------------
LPDIRECT3DSTATEBLOCK9	DxEditVM::m_pSavedBaseSB(NULL);
LPDIRECT3DSTATEBLOCK9	DxEditVM::m_pDrawBaseSB(NULL);
LPDIRECT3DSTATEBLOCK9	DxEditVM::m_pSavedImageSB(NULL);
LPDIRECT3DSTATEBLOCK9	DxEditVM::m_pDrawImageSB(NULL);

DxEditVM::DxEditVM(void)
    : m_pd3dDevice(NULL)
    , m_bImage(FALSE)
    , m_bTitle(FALSE)
    , m_bSelectBaseBox(FALSE)
    , m_bOutputColor(FALSE)
    , m_dwCommandID(0)
    , m_dwColorBG(0L)
    , m_cColorText(0xffffffff)
    , m_vMin(0.f,0.f)
    , m_vMax(0.f,0.f)
{
}

DxEditVM::~DxEditVM(void)
{
    //std::for_each( m_vecChannelText.begin(), m_vecChannelText.end(), std_afunc::DeleteObject() );
    m_vecChannelText.clear();
}

void DxEditVM::OnResetDeviceSTATIC( LPDIRECT3DDEVICEQ pd3dDevice )
{
    for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		// Note : SetRenderState()
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,		    FALSE );
        pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	    FALSE );
        pd3dDevice->SetRenderState( D3DRS_ZENABLE,		    FALSE );
        pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xffffffff );

        // Note : SetTextureStageState()
        pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_TFACTOR );
        pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );
        pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );
        pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_DISABLE );

		if( which==0 )	pd3dDevice->EndStateBlock ( &m_pSavedBaseSB );
		else			pd3dDevice->EndStateBlock ( &m_pDrawBaseSB );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		// Note : SetRenderState()
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,		    FALSE );
        pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	    FALSE );
        pd3dDevice->SetRenderState( D3DRS_ZENABLE,		    FALSE );
        pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xffffffff );

        // Note : SetTextureStageState()
        pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
        pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );
        pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );
        pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_DISABLE );

		if( which==0 )	pd3dDevice->EndStateBlock ( &m_pSavedImageSB );
		else			pd3dDevice->EndStateBlock ( &m_pDrawImageSB );
	}
}

void DxEditVM::OnLostDeviceSTATIC()
{
    SAFE_RELEASE( m_pSavedBaseSB );
    SAFE_RELEASE( m_pDrawBaseSB );
    SAFE_RELEASE( m_pSavedImageSB );
    SAFE_RELEASE( m_pDrawImageSB );
}

void DxEditVM::Create( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bTitle )
{
    // 단순 수치셋팅
    m_pd3dDevice = pd3dDevice;
    m_bTitle = bTitle;

    if ( m_bTitle )
    {
        m_dwColorBG = COLOR_BACKGROUND_TITLE_;
        m_cColorText = COLOR_TEXT_TITLE;
    }
    else
    {
        m_dwColorBG = COLOR_BACKGROUND_CHANNEL;
        m_cColorText = COLOR_TEXT_CHANNEL;
    }
}

void DxEditVM::AddChannel( const TSTRING& strTitleName )
{    
    LPD3DXMESH pMesh(NULL);
    D3DXVECTOR4 sMeshRect(0.f,0.f,0.f,0.f);   // 상,하,좌,우

    HRESULT hr(S_OK);
    hr = CreateD3DXTextMesh( m_pd3dDevice, 
                            &pMesh, 
                            strTitleName.c_str(),
                            12,
                            FALSE, 
                            FALSE );

    if( hr == S_OK )
    {
        // CalMeshRect
        CalMeshRect( pMesh, sMeshRect );

        // m_vecChannelText에 정보 삽입.
        {
            std::tr1::shared_ptr<DxEditVMText> spNew(new DxEditVMText);
            spNew.get()->m_strText = strTitleName;
            spNew.get()->m_sMeshRect = sMeshRect;
            spNew.get()->m_pMesh = pMesh;
            m_vecChannelText.push_back( spNew );
        }
    }
}

void DxEditVM::ChangeChannel( const TSTRING& strTitleName )
{
    if ( m_vecChannelText.empty() )
        return;

    LPD3DXMESH pMesh(NULL);
    D3DXVECTOR4 sMeshRect(0.f,0.f,0.f,0.f);   // 상,하,좌,우

    HRESULT hr(S_OK);
    hr = CreateD3DXTextMesh( m_pd3dDevice, 
                            &pMesh, 
                            strTitleName.c_str(),
                            12,
                            FALSE, 
                            FALSE );

    if( hr == S_OK )
    {
        // CalMeshRect
        CalMeshRect( pMesh, sMeshRect );

        // 이전 정보 삭제
        {
            SAFE_RELEASE( m_vecChannelText[0]->m_pMesh );
        }

        // m_vecChannelText에 정보 삽입.
        {
            m_vecChannelText[0]->m_strText = strTitleName;
            m_vecChannelText[0]->m_sMeshRect = sMeshRect;
            m_vecChannelText[0]->m_pMesh = pMesh;
        }
    }
}

// CalMeshRect
void DxEditVM::CalMeshRect( LPD3DXMESH pMesh, D3DXVECTOR4& sMeshRect )
{
    float fUp, fDown, fLeft, fRight;
    vm::VERTEXNOR* pVertices(NULL);
    DWORD dwSize = pMesh->GetNumVertices();
    pMesh->LockVertexBuffer( D3DLOCK_READONLY, (VOID**)&pVertices );
    for( DWORD i=0; i<dwSize; ++i )
    {
        if( i==0 )
        {
            fLeft = fRight = pVertices[i].vPos.x;
            fUp = fDown = pVertices[i].vPos.y;
        }
        else
        {
            if( fLeft > pVertices[i].vPos.x )
                fLeft = pVertices[i].vPos.x;

            if( fRight < pVertices[i].vPos.x )
                fRight = pVertices[i].vPos.x;

            if( fUp < pVertices[i].vPos.y )
                fUp = pVertices[i].vPos.y;

            if( fDown > pVertices[i].vPos.y )
                fDown = pVertices[i].vPos.y;
        }
    }
    pMesh->UnlockVertexBuffer();

    sMeshRect.x = fUp;
    sMeshRect.y = fDown;
    sMeshRect.z = fLeft;
    sMeshRect.w = fRight;
}

void DxEditVM::AddImage()
{
    m_bImage = TRUE;
}

void DxEditVM::AddOutputColor()
{
    m_bOutputColor = TRUE;
}

void DxEditVM::SetYellowBox( VM_CAN_SELECT_INDEX& sCanSelectIndex )
{
    if ( sCanSelectIndex.m_emMode != CAN_SELECT_ZERO &&
        sCanSelectIndex.m_dwID == m_dwCommandID )
    {
        m_sYellowBox.m_emMode = sCanSelectIndex.m_emMode;
        m_sYellowBox.m_nIndexBox = sCanSelectIndex.m_nIndexBox;
    }
    else
    {
        m_sYellowBox.m_emMode = CAN_SELECT_ZERO;
    }
}

void DxEditVM::SetSelectBaseBox( BOOL bSelect )
{
    m_bSelectBaseBox = bSelect;
}

//-------------------------------------------------------------------------------------------------- Mouse Action
BOOL DxEditVM::IsMouseCanSelect( const D3DXVECTOR3& vTargetPt, const D3DXVECTOR3& vFromPt )
{
    IsMouseCanSelect_InputBox( vTargetPt, vFromPt );
    IsMouseCanSelect_OutputBox( vTargetPt, vFromPt );

    return TRUE;
}

BOOL DxEditVM::IsMouseCanSelect_InputBox( const D3DXVECTOR3& vTargetPt, const D3DXVECTOR3& vFromPt )
{
    return TRUE;
}

BOOL DxEditVM::IsMouseCanSelect_OutputBox( const D3DXVECTOR3& vTargetPt, const D3DXVECTOR3& vFromPt )
{
    return TRUE;
}

//-------------------------------------------------------------------------------------------------- Render
void DxEditVM::Render( DWORD dwID, 
                      VM_CAN_SELECT_INDEX& sCanSelectIndex, 
                      const D3DXVECTOR2& vPos, 
                      float fWidth, 
                      float fHeight, 
                      DWORD dwFlags, 
                      const D3DXVECTOR2& sMousePt,
                      ID3DXEffect* pFX )
{
    m_dwCommandID = dwID;

    // 디바이스 관련 초기작업
    {
        D3DXMATRIX matWorld;
        D3DXMatrixIdentity( &matWorld );
        m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
    }

    // Width와 Height를 구한다.
    fWidth = GetCalculateWidth( fWidth );
    fHeight = GetCalculateHeight( fHeight );

    // InputBox를 그린다.
    ActionInputBox( sCanSelectIndex, dwID, vPos, fWidth, sMousePt );

    // Output을 출력한다.
    // Output Color를 출력한다.
    if ( m_bOutputColor )
    {
        ActionOutputColor( sCanSelectIndex, dwID, vPos, sMousePt );
    }
    else
    {
        ActionOutputOne( sCanSelectIndex, dwID, vPos, sMousePt );
    }

    // Box를 그린다.
    RenderBaseBox( vPos, fWidth, fHeight );

    // Image를 그린다.
    RenderImage( vPos, pFX );

    // Line을 그린다.
    RenderLine( vPos, fWidth, fHeight );

    // 문자 렌더링
    {
        m_pSavedBaseSB->Capture();
		m_pDrawBaseSB->Apply();

        m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, m_cColorText );

        // 타이틀일 경우
        if ( m_bTitle && !m_vecChannelText.empty() )
        {
            m_vecChannelText[0]->Render( m_pd3dDevice, vPos, fWidth, fHeight, dwFlags );
        }
        else
        {
            D3DXVECTOR2 vPosNEW = vPos;
            vPosNEW.x -= CHANNEL_EMPTY_RIGHT;
            vPosNEW.y -= CHANNEL_EMPTY_HEIGHT;
            for ( DWORD i=0; i<m_vecChannelText.size(); ++i )
            {
                m_vecChannelText[i]->Render( m_pd3dDevice, vPosNEW, fWidth, fHeight, dwFlags );
                vPosNEW.y -= m_vecChannelText[i]->GetHeight();
                vPosNEW.y -= CHANNEL_EMPTY_HEIGHT;
            }
        }

        m_pSavedBaseSB->Apply();
    }
}

float DxEditVM::GetCalculateWidth( float fWidth )
{
    // 글씨 길이
    float fWidthTemp(1.f);
    for ( DWORD i=0; i<m_vecChannelText.size(); ++i )
    {
        if( fWidthTemp < m_vecChannelText[i]->GetWidth() )
            fWidthTemp = m_vecChannelText[i]->GetWidth();
    }

    // 텍스쳐가 있으면 그만큼 늘려준다.
    if ( m_bImage )
    {
        float fWidthAddImage = fWidthTemp + IMAGE_SIZE;
        if ( fWidth < fWidthAddImage )
            fWidth = fWidthAddImage;
    }
    else
    {
        // 글씨가 셋팅된 것보다 크면 그쪽으로 변경
        if ( fWidth < fWidthTemp )
            fWidth = fWidthTemp;
    }

    return fWidth;
}

float DxEditVM::GetCalculateHeight( float fHeight )
{
    // 이미지가 있다.
    if ( m_bImage )
    {
        if ( fHeight < IMAGE_SIZE )
            fHeight = IMAGE_SIZE;
    }

    if ( m_vecChannelText.empty() )
        return fHeight;

    if ( m_bTitle )
    {
        return fHeight;
    }
    else
    {
        float fHeightNEW(0.f);
        fHeightNEW += CHANNEL_EMPTY_HEIGHT;
        for ( DWORD i=0; i<m_vecChannelText.size(); ++i )
        {
            fHeightNEW += m_vecChannelText[i]->GetHeight();
            fHeightNEW += CHANNEL_EMPTY_HEIGHT;
        }

        if( fHeight < fHeightNEW )
            fHeight = fHeightNEW;

        return fHeight;
    }
}

void DxEditVM::GetPointInput( D3DXVECTOR3& vPoint, int nIndexBox )
{
    if ( nIndexBox >= static_cast<int>( m_vecInputPoint.size() ) )
        return;

    vPoint = m_vecInputPoint[nIndexBox];
}

void DxEditVM::GetPointOutput( D3DXVECTOR3& vPoint, int nIndexBox )
{
    if ( nIndexBox >= static_cast<int>( m_vecOutputPoint.size() ) )
        return;

    vPoint = m_vecOutputPoint[nIndexBox];
}

BOOL DxEditVM::IsSelectAtBox( const D3DXVECTOR2& vMin, const D3DXVECTOR2& vMax )
{
    if( m_vMin.x > vMin.x && m_vMin.x > vMax.x )    return FALSE;
    if( m_vMin.y > vMin.y && m_vMin.y > vMax.y )    return FALSE;
    if( m_vMax.x < vMin.x && m_vMax.x < vMax.x )    return FALSE;
    if( m_vMax.y < vMin.y && m_vMax.y < vMax.y )    return FALSE;

    return TRUE;
}

//-------------------------------------------------------------------------------------------------- RenderBox & Line
void DxEditVM::RenderBaseBox( const D3DXVECTOR2& vPos, float fWidth, float fHeight )
{
    vm::VERTEXCOLOR vBoxPos[4];
    for( DWORD i=0; i<4; ++i )
    {
        vBoxPos[i].vPos = D3DXVECTOR3( vPos.x, vPos.y, 0.f );
        vBoxPos[i].dwColor = m_dwColorBG;
    }

    // 박스 초기값 계산
    {
        vBoxPos[0].vPos += D3DXVECTOR3( 0.f,    0.f,        0.f );
        vBoxPos[1].vPos += D3DXVECTOR3( fWidth, 0.f,        0.f );
        vBoxPos[2].vPos += D3DXVECTOR3( 0.f,    -fHeight,   0.f );
        vBoxPos[3].vPos += D3DXVECTOR3( fWidth, -fHeight,   0.f );

        // 사이즈 저장시켜놓음.
        m_vMin.x = vBoxPos[2].vPos.x;
        m_vMin.y = vBoxPos[2].vPos.y;
        m_vMax.x = vBoxPos[1].vPos.x;
        m_vMax.y = vBoxPos[1].vPos.y;
    }

    // 상자를 그린다.
    {
        m_pSavedBaseSB->Capture();
		m_pDrawBaseSB->Apply();

        m_pd3dDevice->SetFVF( vm::VERTEXCOLOR::FVF );

        // 내부채움.
        m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, vBoxPos, sizeof(vm::VERTEXCOLOR) );

        m_pSavedBaseSB->Apply();
    }
}

void DxEditVM::RenderLine( const D3DXVECTOR2& vPos, float fWidth, float fHeight )
{
    vm::VERTEXCOLOR vLine[2];
    if ( m_bSelectBaseBox )
    {
        vLine[0].dwColor = 0xffffff00;
        vLine[1].dwColor = 0xffffff00;
    }
    else
    {
        vLine[0].dwColor = 0L;
        vLine[1].dwColor = 0L;
    }

    // 상자를 그린다.
    {
        m_pSavedBaseSB->Capture();
		m_pDrawBaseSB->Apply();

        m_pd3dDevice->SetFVF( vm::VERTEXCOLOR::FVF );

        // 상
        vLine[0].vPos = D3DXVECTOR3( m_vMin.x, m_vMax.y, 0.f );
        vLine[1].vPos = D3DXVECTOR3( m_vMax.x, m_vMax.y, 0.f );
        m_pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, vLine, sizeof(vm::VERTEXCOLOR) );

        // 좌
        vLine[0].vPos = D3DXVECTOR3( m_vMin.x, m_vMin.y, 0.f );
        vLine[1].vPos = D3DXVECTOR3( m_vMin.x, m_vMax.y, 0.f );
        m_pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, vLine, sizeof(vm::VERTEXCOLOR) );

        // 우
        vLine[0].vPos = D3DXVECTOR3( m_vMax.x, m_vMin.y, 0.f );
        vLine[1].vPos = D3DXVECTOR3( m_vMax.x, m_vMax.y, 0.f );
        m_pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, vLine, sizeof(vm::VERTEXCOLOR) );

        // 하
        vLine[0].vPos = D3DXVECTOR3( m_vMin.x, m_vMin.y, 0.f );
        vLine[1].vPos = D3DXVECTOR3( m_vMax.x, m_vMin.y, 0.f );
        m_pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, vLine, sizeof(vm::VERTEXCOLOR) );

        m_pSavedBaseSB->Apply();
    }
}

//-------------------------------------------------------------------------------------------------- InputBox Action
void DxEditVM::ActionInputBox( VM_CAN_SELECT_INDEX& sCanSelectIndex, DWORD dwID, const D3DXVECTOR2& vPos, float fWidth, const D3DXVECTOR2& sMousePt )
{
    m_vecInputPoint.clear();

    if ( m_bTitle )
        return;

    if ( m_vecChannelText.empty() )
        return;

    // 내부 채움
    D3DXVECTOR2 vPosNEW = vPos;
    vPosNEW.y -= CHANNEL_EMPTY_HEIGHT;
    for ( DWORD i=0; i<m_vecChannelText.size(); ++i )
    {
        vm::VERTEXCOLOR vBoxPos[4];
        CreateInputBox( vBoxPos, vPosNEW, fWidth, i );
        RenderInputBox( vBoxPos );
        SetCanSelectBox( sCanSelectIndex, dwID, vBoxPos, sMousePt, i, CAN_SELECT_INPUT );

        vPosNEW.y -= m_vecChannelText[i]->GetHeight();
        vPosNEW.y -= CHANNEL_EMPTY_HEIGHT;
    }    
}

void DxEditVM::CreateInputBox( vm::VERTEXCOLOR vBoxPos[], const D3DXVECTOR2& vPos, float fWidth, DWORD dwIndex )
{
    for( DWORD i=0; i<4; ++i )
    {
        vBoxPos[i].vPos = D3DXVECTOR3( vPos.x+fWidth, vPos.y, 0.f );

        // 선택할 수 있는 Box
        if ( m_sYellowBox.m_emMode == CAN_SELECT_INPUT &&
            m_sYellowBox.m_nIndexBox == dwIndex )
        {
            vBoxPos[i].dwColor = 0xffffff00;
        }
        // 기본 Box
        else
        {
            vBoxPos[i].dwColor = 0L;
        }
    }

    // 박스 초기값 계산
    {
        vBoxPos[0].vPos += D3DXVECTOR3( 0.f,            0.f,                0.f );
        vBoxPos[1].vPos += D3DXVECTOR3( INPUT_BOX_SIZE, 0.f,                0.f );
        vBoxPos[2].vPos += D3DXVECTOR3( 0.f,            -INPUT_BOX_SIZE,    0.f );
        vBoxPos[3].vPos += D3DXVECTOR3( INPUT_BOX_SIZE, -INPUT_BOX_SIZE,    0.f );
    }

    m_vecInputPoint.push_back( vBoxPos[0].vPos + D3DXVECTOR3( INPUT_BOX_SIZE, -INPUT_BOX_SIZE_HALF, 0.f ) );
}

void DxEditVM::RenderInputBox( vm::VERTEXCOLOR vBoxPos[] )
{
    m_pSavedBaseSB->Capture();
    m_pDrawBaseSB->Apply();

    m_pd3dDevice->SetFVF( vm::VERTEXCOLOR::FVF );

    m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, vBoxPos, sizeof(vm::VERTEXCOLOR) );

    m_pSavedBaseSB->Apply();
}


//-------------------------------------------------------------------------------------------------- Action OutputBox
void DxEditVM::ActionOutputColor( VM_CAN_SELECT_INDEX& sCanSelectIndex, 
                                DWORD dwID,  
                                const D3DXVECTOR2& vPos, 
                                const D3DXVECTOR2& sMousePt )
{
    m_vecOutputPoint.clear();

    if ( m_bTitle )
        return;

    // 내부 채움
    vm::VERTEXCOLOR vBoxPos[4];
    D3DXVECTOR2 vPosNEW = vPos;

    vPosNEW.y -= INPUT_BOX_SIZE;
    RenderOutputBox( vBoxPos, vPosNEW, 0L, 0 );
    SetCanSelectBox( sCanSelectIndex, dwID, vBoxPos, sMousePt, 0, CAN_SELECT_OUTPUT );

    vPosNEW.y -= INPUT_BOX_SIZE;
    vPosNEW.y -= INPUT_EMPTY_HEIGHT;
    RenderOutputBox( vBoxPos, vPosNEW, 0xffff0000, 1 );
    SetCanSelectBox( sCanSelectIndex, dwID, vBoxPos, sMousePt, 1, CAN_SELECT_OUTPUT );

    vPosNEW.y -= INPUT_BOX_SIZE;
    vPosNEW.y -= INPUT_EMPTY_HEIGHT;
    RenderOutputBox( vBoxPos, vPosNEW, 0xff00ff00, 2 );
    SetCanSelectBox( sCanSelectIndex, dwID, vBoxPos, sMousePt, 2, CAN_SELECT_OUTPUT );

    vPosNEW.y -= INPUT_BOX_SIZE;
    vPosNEW.y -= INPUT_EMPTY_HEIGHT;
    RenderOutputBox( vBoxPos, vPosNEW, 0xff0000ff, 3 );
    SetCanSelectBox( sCanSelectIndex, dwID, vBoxPos, sMousePt, 3, CAN_SELECT_OUTPUT );

    vPosNEW.y -= INPUT_BOX_SIZE;
    vPosNEW.y -= INPUT_EMPTY_HEIGHT;
    RenderOutputBox( vBoxPos, vPosNEW, 0xffffffff, 4 );
    SetCanSelectBox( sCanSelectIndex, dwID, vBoxPos, sMousePt, 4, CAN_SELECT_OUTPUT );
}

void DxEditVM::ActionOutputOne( VM_CAN_SELECT_INDEX& sCanSelectIndex, 
                                DWORD dwID,  
                                const D3DXVECTOR2& vPos, 
                                const D3DXVECTOR2& sMousePt )
{
    m_vecOutputPoint.clear();

    if ( m_bTitle )
        return;

    // Core는 렌더링 및 선택되면 안된다.
    if ( !m_bImage )
        return;

    // 내부 채움
    vm::VERTEXCOLOR vBoxPos[4];
    D3DXVECTOR2 vPosNEW = vPos;

    vPosNEW.y -= INPUT_BOX_SIZE;
    RenderOutputBox( vBoxPos, vPosNEW, 0L, 0 );
    SetCanSelectBox( sCanSelectIndex, dwID, vBoxPos, sMousePt, 0, CAN_SELECT_OUTPUT );
}

void DxEditVM::RenderOutputBox( vm::VERTEXCOLOR vBoxPos[], const D3DXVECTOR2& vPos, DWORD dwColor, DWORD dwIndex )
{
    for( DWORD i=0; i<4; ++i )
    {
        vBoxPos[i].vPos = D3DXVECTOR3( vPos.x, vPos.y, 0.f );

        // 선택할 수 있는 Box
        if ( m_sYellowBox.m_emMode == CAN_SELECT_OUTPUT &&
            m_sYellowBox.m_nIndexBox == dwIndex )
        {
            vBoxPos[i].dwColor = 0xffffff00;
        }
        // 기본 Box
        else
        {
            vBoxPos[i].dwColor = dwColor;
        }
    }

    // 박스 초기값 계산
    {
        vBoxPos[0].vPos += D3DXVECTOR3( -INPUT_BOX_SIZE,    0.f,                0.f );
        vBoxPos[1].vPos += D3DXVECTOR3( 0.f,                0.f,                0.f );
        vBoxPos[2].vPos += D3DXVECTOR3( -INPUT_BOX_SIZE,    -INPUT_BOX_SIZE,    0.f );
        vBoxPos[3].vPos += D3DXVECTOR3( 0.f,                -INPUT_BOX_SIZE,    0.f );
    }

    m_vecOutputPoint.push_back( vBoxPos[0].vPos + D3DXVECTOR3( 0.f, -INPUT_BOX_SIZE_HALF, 0.f ) );

    m_pSavedBaseSB->Capture();
    m_pDrawBaseSB->Apply();

    m_pd3dDevice->SetFVF( vm::VERTEXCOLOR::FVF );

    m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, vBoxPos, sizeof(vm::VERTEXCOLOR) );

    m_pSavedBaseSB->Apply();
}


//-------------------------------------------------------------------------------------------------- SetCanSelectBox
void DxEditVM::SetCanSelectBox( VM_CAN_SELECT_INDEX& sCanSelectIndex, 
                                DWORD dwID,
                                vm::VERTEXCOLOR vBoxPos[], 
                                const D3DXVECTOR2& sMousePt, 
                                int nIndex,
                                VM_CAN_SELECT_BOX emMode )
{
    if ( sMousePt.x < vBoxPos[0].vPos.x )    return;
    if ( sMousePt.x > vBoxPos[1].vPos.x )    return;
    if ( sMousePt.y > vBoxPos[0].vPos.y )    return;
    if ( sMousePt.y < vBoxPos[2].vPos.y )    return;

    sCanSelectIndex.m_emMode    = emMode;
    sCanSelectIndex.m_dwID      = dwID;
    sCanSelectIndex.m_nIndexBox = nIndex;
}

//-------------------------------------------------------------------------------------------------- RenderImage
void DxEditVM::RenderImage( const D3DXVECTOR2& vPos, ID3DXEffect* pFX )
{
    if ( !pFX )
        return;

    if ( !m_bImage )
        return;

    vm::POSTEX vBoxPos[4];
    for( DWORD i=0; i<4; ++i )
    {
        vBoxPos[i].vPos = D3DXVECTOR3( vPos.x, vPos.y, 0.f );
    }

    // 박스 초기값 계산
    {
        vBoxPos[0].vPos += D3DXVECTOR3( 0.f,        0.f,            0.f );
        vBoxPos[1].vPos += D3DXVECTOR3( IMAGE_SIZE, 0.f,            0.f );
        vBoxPos[2].vPos += D3DXVECTOR3( 0.f,        -IMAGE_SIZE,    0.f );
        vBoxPos[3].vPos += D3DXVECTOR3( IMAGE_SIZE, -IMAGE_SIZE,    0.f );
        vBoxPos[0].vTex = D3DXVECTOR2( 0.f, 0.f );
        vBoxPos[1].vTex = D3DXVECTOR2( 1.f, 0.f );
        vBoxPos[2].vTex = D3DXVECTOR2( 0.f, 1.f );
        vBoxPos[3].vTex = D3DXVECTOR2( 1.f, 1.f );
    }

    // Draw
    m_pSavedImageSB->Capture();
    m_pDrawImageSB->Apply();

    m_pd3dDevice->SetFVF( vm::POSTEX::FVF );

    HRESULT hr(S_OK);
    UINT numPasses;
	hr = pFX->Begin( &numPasses, 0 ); 
	if( 0 < numPasses )
	{
		hr = pFX->BeginPass( 0 );
        {
            m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, vBoxPos, sizeof(vm::POSTEX) );
        }
		hr = pFX->EndPass();
	}
	hr = pFX->End();

    hr = m_pd3dDevice->SetPixelShader(NULL); 

    m_pSavedImageSB->Apply();
}

//-------------------------------------------------------------------------------------------------- CreateD3DXTextMesh
HRESULT DxEditVM::CreateD3DXTextMesh( IDirect3DDevice9* pd3dDevice, 
                                        LPD3DXMESH* ppMesh, 
                                        const TCHAR* pName,
                                        DWORD dwSize,
                                        BOOL bBold, 
                                        BOOL bItalic )
{
    HRESULT hr;
    LPD3DXMESH pMeshNew = NULL;
    HDC hdc = CreateCompatibleDC( NULL );
    if( hdc == NULL )
        return E_OUTOFMEMORY;
    INT nHeight = -MulDiv( dwSize, GetDeviceCaps(hdc, LOGPIXELSY), 72 );
    HFONT hFont;
    HFONT hFontOld;

    hFont = CreateFont(nHeight, 0, 0, 0, bBold ? FW_BOLD : FW_NORMAL, bItalic, FALSE, FALSE, DEFAULT_CHARSET, 
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, NSVISUAL_MATERIAL::g_strFont.c_str() );
    
    hFontOld = (HFONT)SelectObject(hdc, hFont); 

    hr = D3DXCreateText( pd3dDevice, hdc, pName, 
                         0.1f, 0.001f, &pMeshNew, NULL, NULL);

    SelectObject(hdc, hFontOld);
    DeleteObject( hFont );
    DeleteDC( hdc );

    if( SUCCEEDED( hr ) )
        *ppMesh = pMeshNew;

    return hr;
}
