#include "StdAfx.h"

#include "../../../EngineLib/Common/DXInputString.h"
#include "../../../EngineLib/GUInterface/BasicCarrat.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/GUInterface/UITextUtil.h"
#include "../../../EngineLib/GUInterface/UIRenderQueue.h"
#include "../../../EngineLib/DxResponseMan.h"

#include "../../../RanLogicClient/RanParamClient.h"
#include "../../../RanLogicClient/Input/GLInputManager.h"

#include "ShotCutBox.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const int	CShotCutBox::nDEFAULT_LIMIT_INPUT = 10;
const float	CShotCutBox::fLINE_END_GAP = 2.0f;

UIGUID		CShotCutBox::DEBUG_BEGIN_ID = NO_ID;
UIGUID		CShotCutBox::DEBUG_END_ID = NO_ID;

CShotCutBox::CShotCutBox(EngineDeviceMan* pEngineDevice)
	: CUIGroup(pEngineDevice)
    , m_bModeNative ( false )
	, m_bBeginEdit ( false )
	, m_bLimited( false )
	, m_nLimitInput ( nDEFAULT_LIMIT_INPUT )
	, m_pCarrat ( NULL )
	, m_dwTextColor ( NS_UITEXTUTIL::ENABLE )
	, m_bKeyInput( TRUE )
	, m_xViewPos( 0 )
	, m_xInsertPos( 0 )
	, m_xInsertPosR( 0 )
	, m_fCarratPos( 0.0f )
{
//	DXInputString::GetInstance().m_bCaratMove = true;
}

CShotCutBox::~CShotCutBox ()
{
}

void CShotCutBox::CreateCarrat ( char* szCarrat, BOOL bUseRender, DWORD dwDiffuse )
{
	m_pCarrat = new CBasicCarrat(m_pEngineDevice);
	m_pCarrat->CreateSub ( this, szCarrat );
	m_pCarrat->SetUseRender ( bUseRender );
	m_pCarrat->SetVisibleSingle( FALSE );
	RegisterControl ( m_pCarrat );

	m_fCaretSizeX = m_pCarrat->GetLocalPos().sizeX;
	m_dwCarratDiffuse = dwDiffuse;
}

void CShotCutBox::SetFont ( CD3DFontPar* pFont )
{
	if ( !pFont )
	{
		GASSERT ( 0 && "폰트가 널입니다." );
		return;
	}
	m_pFont = pFont;
	m_fDefaultFontSize = static_cast<float>(m_pFont->GetFontHeight());
}

void CShotCutBox::SetGlobalPos ( const UIRECT& rcPos )
{
	CUIGroup::SetGlobalPos ( rcPos );

	UpdateRenderText();
	UpdatePosition ();
}

void CShotCutBox::SetGlobalPos ( const D3DXVECTOR2& vPos )
{
	CUIGroup::SetGlobalPos ( vPos );

	UpdateRenderText();
	UpdatePosition ();
}

void CShotCutBox::SetImpossableKey()
{
	for( int i =0; i< 12; ++i){
		m_NotSetKey[i] = DIK_F1 + i;
	}

	m_NotSetKey[12] = DIK_LCONTROL;
	m_NotSetKey[13] = DIK_LMENU;
	m_NotSetKey[14] = DIK_RCONTROL;
	m_NotSetKey[15] = DIK_RMENU;
	m_NotSetKey[16] = DIK_RSHIFT;
	m_NotSetKey[17] = DIK_LSHIFT;
	m_NotSetKey[18] = DIK_TAB;
	m_NotSetKey[19] = DIK_ESCAPE;
	m_NotSetKey[20] = DIK_RETURN;
	m_NotSetKey[21] = DIK_GRAVE;	
	m_NotSetKey[22] = DIK_Y;

    {
        m_NumPadKey[0] = DIK_NUMPAD0;
        m_NumPadKey[1] = DIK_NUMPAD1;
        m_NumPadKey[2] = DIK_NUMPAD2;
        m_NumPadKey[3] = DIK_NUMPAD3;
        m_NumPadKey[4] = DIK_NUMPAD4;
        m_NumPadKey[5] = DIK_NUMPAD5;
        m_NumPadKey[6] = DIK_NUMPAD6;
        m_NumPadKey[7] = DIK_NUMPAD7;
        m_NumPadKey[8] = DIK_NUMPAD8;
        m_NumPadKey[9] = DIK_NUMPAD9;
    }
}

void CShotCutBox::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if (!IsVisible() || !m_pFont || !m_pCarrat)
        return;
	
	BOOL bInput = false;

	CUIGroup::Update(x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl);
	SetImpossableKey();

	if (IsBegin())
	{
        DXInputString* pInputString = m_pEngineDevice->GetInputString();
		if (!pInputString->IsOn())
		{
			EndEdit();
			return;
		}

		SetMODE_NATIVE(false);

		pInputString->ConverseNativeToEng();
		
		if (!m_bKeyInput)
			pInputString->ConverseFullToHalf();

		CString strInput = pInputString->GetString();
		int xInsertPos = pInputString->GetInsertPos();

		strInput.MakeUpper();

		// 	문자입력 이외에 관계없는 키가 눌러진 경우, 모두 무시
		m_bVALID_INPUT = true;

        BYTE keyState[256];
        bool bNumLock = false;
        if ( GetKeyboardState( keyState ) )
        {
            if ( keyState[ VK_NUMLOCK ] & 1 )
                bNumLock = true;
        }

        if ( GetAsyncKeyState( VK_NUMLOCK ) & 0x8000 )
        {
            const std::string& strShorcutString =
                GLInputManager::GetInstance().GetDxKeyboardValueToString( static_cast<DWORD>( DIK_NUMLOCK ) );
            strInput = strShorcutString.c_str();
        }

        if ( !bNumLock )
        {
            for ( int i = 0; i< nNUM_NUMPAD; ++i)
            {
                if( DxInputDevice::GetInstance().GetKeyState( m_NumPadKey[i] ) & DXKEY_DOWNED )
                {
                    const std::string& strShorcutString =
                        GLInputManager::GetInstance().GetDxKeyboardValueToString( static_cast<DWORD>( m_NumPadKey[i] ) );
                    strInput = strShorcutString.c_str();
                }
            }
        }

		for ( int i = 0; i< 21; ++i){
			if( DxInputDevice::GetInstance().GetKeyState( m_NotSetKey[i] ) & DXKEY_DOWNED ){
				m_bVALID_INPUT = false;
			}
		}

		if( !m_bVALID_INPUT )
		{
			SET_STRING_BUFFER( m_strText );
			SetInsertPos();
			return;
		}

		if (strInput != "")
        {
            bInput = true;

            std::string strKeyString = stringformat( "%1%", strInput );
            const SSHORTCUTS::KEY& sShortCutKey = GLInputManager::GetInstance().GetStringToKey( strKeyString );

            const std::string& strShorcutString =
                GLInputManager::GetInstance().GetDxKeyboardValueToString( static_cast<DWORD>( sShortCutKey.nVALUE ) );
            strInput = strShorcutString.c_str();
		}		

		//! 현재 눌러진 키를 검사
		/*{
			const DxInputDevice::PUTSET& sKeyPutset = DxInputDevice::GetInstance().GetKeyDownSet();

			DxInputDevice::PUTSET_CITER IterKey = sKeyPutset.begin();
			for ( ; IterKey!=sKeyPutset.end(); ++IterKey )
			{
				std::string strKey = GLInputManager::GetInstance().GetDxKeyboardValueToString( static_cast<DWORD>((*IterKey)) );

				if ( strKey == GLInputManager::GetInstance().GetNullKeyString() )
				{
					SET_STRING_BUFFER ( m_strText );
					SetInsertPos();
					return;
				}

				strInput = strKey.c_str();
			}
		}*/

		//	입력 조합창 위치 갱신
		const UIRECT& rcGlobalPos = GetGlobalPos();
		CRect rtEditBox ( (int)rcGlobalPos.left, (int)rcGlobalPos.top, (int)rcGlobalPos.right, (int)rcGlobalPos.bottom ); 
		pInputString->Move(rtEditBox);

		//	입력한계를 넘은 경우
		if ( m_nLimitInput < strInput.GetLength() )
		{
			m_bLimited = true;

			SET_STRING_BUFFER( m_strText );
			SetInsertPos();
			return;	
		}
		else
		{
			m_bLimited = false;
		}

		// 입력 받은 문자가 유효하므로 m_xInsertPos 갱신한다
		m_xInsertPos = xInsertPos;
		m_xInsertPosR = m_xInsertPos;

		m_strRenderText = m_strText = strInput;

	}

	UpdateRenderText();
	UpdatePosition();

	// 텍스쳐에 글자를 렌더링하는 방식으로 변경
	m_pFont->PushText( (TCHAR*)m_strRenderTextR.GetString() );

	if(bInput)	
		EndEdit();
}

HRESULT CShotCutBox::Render ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;

	hr = CUIGroup::Render ( pd3dDevice );
	if ( FAILED ( hr ) ) return hr;

	CUIRenderQueue::Get()->RenderImmediately();

	// 텍스쳐에 글자를 렌더링하는 방식으로 변경
	hr = m_pFont->DrawText( m_vRenderPos.x, m_vRenderPos.y, m_dwTextColor, m_strRenderTextR.GetString(), 0L, FALSE );
	if ( FAILED ( hr ) ) return hr;

	return S_OK;
}

void CShotCutBox::UpdateRenderText()
{
	m_strRenderTextR = m_strRenderText;

	if( DXInputString::m_bCaratMove == FALSE )
		return;

	const int lEditWidth = (int)( GetGlobalPos().sizeX - fLINE_END_GAP );
	int iLen = m_strRenderTextR.GetLength();

	CString strTemp;
	SIZE sizeTemp;

	// m_strRenserText ( 멀티바이트 ) <= 동기화 => m_xViewPos, m_xInsertPosR ( 유니코드 )
	int iViewPosM = m_xViewPos;
	int iInsertPosRM = m_xInsertPosR;
	if( m_pFont )
	{
		WCHAR wTemp[ 1024 ];
		m_pFont->MultiToWide( m_strRenderTextR.GetString(), iLen, wTemp, 1022 );
		CStringW strTempW = wTemp;

		iViewPosM = WideToMultiPos( &strTempW, m_xViewPos );
		iInsertPosRM = WideToMultiPos( &strTempW, m_xInsertPosR );
	}

	// m_xInsertPosR 가 m_xViewPos 보다 앞에 있다면 보정
	if( iInsertPosRM < iViewPosM )
	{
		iViewPosM = iInsertPosRM;

		// 페이징
		for( int i=0; i<7; ++i )
		{
			if( iViewPosM <= 0 )
				break;
			if( NS_UITEXTUTIL::IsDBCS( m_strRenderTextR[ iViewPosM - 1 ] ) )
				iViewPosM -= 2;
			else
				iViewPosM -= 1;
		}

		iViewPosM = max( iViewPosM, 0 );
	}

	// 출력범위 ( m_xViewPos 부터 lEditWidth 만큼 ) 안에 m_xInsertPosR 가 없다면 보정
	if( iInsertPosRM - iViewPosM > 0 )
	{
		strTemp = m_strRenderTextR.Mid( iViewPosM, iInsertPosRM - iViewPosM );
		m_pFont->GetTextExtent( strTemp.GetString(), sizeTemp );

		if( sizeTemp.cx > lEditWidth )
		{
			while( sizeTemp.cx > lEditWidth )
			{
				if( NS_UITEXTUTIL::IsDBCS( m_strRenderTextR[ iViewPosM ] ) )
					iViewPosM += 2;
				else
					iViewPosM += 1;
				strTemp = m_strRenderTextR.Mid( iViewPosM, iInsertPosRM - iViewPosM );
				m_pFont->GetTextExtent( strTemp.GetString(), sizeTemp );
			}

			// 페이징
			int iTemp = iInsertPosRM;
			for( int i=0; i<7; ++i )
			{
				if( iTemp >= iLen )
					break;
				if( NS_UITEXTUTIL::IsDBCS( m_strRenderTextR[ iViewPosM ] ) )
				{
					iViewPosM += 2;
					iTemp += 2;
				}
				else
				{
					iViewPosM += 1;
					iTemp += 1;
				}
			}
		}
	}

	// 보정된 위치 iViewPosM ( 멀티 바이트 ) <= 동기화 => m_xViewPos ( 유니코드 )
	m_xViewPos = MultiToWidePos( &m_strRenderTextR, iViewPosM );

	// 캐럿 위치 계산
	{
		strTemp = m_strRenderTextR.Mid( iViewPosM, iInsertPosRM - iViewPosM );
		m_pFont->GetTextExtent ( strTemp.GetString(), sizeTemp );
		m_fCarratPos = (float)sizeTemp.cx;
	}

	// m_xViewPos 부터 시작해서 끝까지 길이 계산 후 lEditWidth 안에 들어올때까지 자른다
	if( iLen > 0 )
	{
		m_strRenderTextR = m_strRenderTextR.Right( iLen - iViewPosM );
		m_pFont->GetTextExtent( m_strRenderTextR.GetString(), sizeTemp );
		iLen = m_strRenderTextR.GetLength();

		while( sizeTemp.cx > lEditWidth )
		{
			if( NS_UITEXTUTIL::IsDBCS( m_strRenderTextR[ iLen-1 ] ) )
				m_strRenderTextR = m_strRenderTextR.Left( iLen-2 );
			else
				m_strRenderTextR = m_strRenderTextR.Left( iLen-1 );
			m_pFont->GetTextExtent( m_strRenderTextR.GetString(), sizeTemp );
			iLen = m_strRenderTextR.GetLength();
		}
	}
}

void CShotCutBox::UpdatePosition ()
{
	if( DXInputString::m_bCaratMove )
	{
		SIZE TextSize;
		m_pFont->GetTextExtent ( m_strRenderTextR.GetString(), TextSize );

		//	글자 뿌릴 위치
		UIRECT rcEditBoxPos = GetGlobalPos ();
		{
			const UIRECT rcCarratPos = m_pCarrat->GetLocalPos ();
			m_vRenderPos.x = rcEditBoxPos.left + ((rcEditBoxPos.sizeX - (float)TextSize.cx) / 2.0f);
			m_vRenderPos.y = rcEditBoxPos.top + rcCarratPos.top;
		}

		//	캐럿 뿌릴 위치
		UIRECT rcCarratPos = m_pCarrat->GetGlobalPos ();
		{
			rcCarratPos.left = m_vRenderPos.x + m_fCarratPos;
			m_pCarrat->SetGlobalPos ( rcCarratPos );
		}
	}
	else
	{
		const long lEditWidth = (long)(GetGlobalPos().sizeX - fLINE_END_GAP);

		//	문자열이 에디트박스를 넘어설 경우 앞부분 잘라내기
		SIZE TextSize;
		m_pFont->GetTextExtent ( m_strRenderTextR.GetString(), TextSize );

		if ( lEditWidth < TextSize.cx )
		{
			int nStrLength = m_strRenderTextR.GetLength ();
			while ( lEditWidth < TextSize.cx && nStrLength )
			{
				int nCut = nStrLength - 1;		
				if ( NS_UITEXTUTIL::IsDBCS ( m_strRenderTextR[0] ) )	nCut--;

				m_strRenderTextR = m_strRenderTextR.Right ( nCut );

				nStrLength = m_strRenderTextR.GetLength ();
				m_pFont->GetTextExtent ( m_strRenderTextR.GetString (), TextSize );
			}
		}

		//	글자 뿌릴 위치
		UIRECT rcEditBoxPos = GetGlobalPos ();
		{
			const UIRECT rcCarratPos = m_pCarrat->GetLocalPos ();
			m_vRenderPos.x = rcEditBoxPos.left + ((rcEditBoxPos.sizeX - (float)TextSize.cx) / 2.0f);		
			m_vRenderPos.y = rcEditBoxPos.top + rcCarratPos.top;
		}

		//	캐럿 뿌릴 위치
		UIRECT rcCarratPos = m_pCarrat->GetGlobalPos ();
		{
			rcCarratPos.left = m_vRenderPos.x + (float)TextSize.cx;
			m_pCarrat->SetGlobalPos ( rcCarratPos );
		}
	}
}

void CShotCutBox::GetInsertPos()
{
    DXInputString* pInputString = m_pEngineDevice->GetInputString();
	m_xInsertPos = pInputString->GetInsertPos();		
	m_xInsertPosR = m_xInsertPos;
}

void CShotCutBox::SetInsertPos()
{
    DXInputString* pInputString = m_pEngineDevice->GetInputString();
	m_xInsertPos = pInputString->SetInsertPos(m_xInsertPos);
	m_xInsertPosR = m_xInsertPos;
}

int CShotCutBox::MultiToWidePos( CString* strMulti, int iMultiPos )
{
	if( m_pFont )
	{
		CString strTemp = strMulti->Left( iMultiPos );
		WCHAR wTemp[ 512 ]={0};
		return m_pFont->MultiToWide( strTemp.GetString(), strTemp.GetLength(), wTemp, 510 );
	}
	return 0;
}

int CShotCutBox::WideToMultiPos( CStringW* strWide, int iWidePos )
{
	if( m_pFont )
	{
		CStringW strTemp = strWide->Left( iWidePos );
		TCHAR cTemp[ 1024 ]={0};
		return m_pFont->WideToMulti( strTemp.GetString(), strTemp.GetLength(), cTemp, 1022 );
	}
	return 0;
}

void CShotCutBox::FLUSH_STRING_BUFFER ()
{
    DXInputString* pInputString = m_pEngineDevice->GetInputString();
    pInputString->SetString();
}

void CShotCutBox::SET_STRING_BUFFER(const char* pStr)
{
	FLUSH_STRING_BUFFER();	
    DXInputString* pInputString = m_pEngineDevice->GetInputString();
	pInputString->SetString(pStr);
}

bool CShotCutBox::BeginEdit()
{
	if (m_bBeginEdit)
        return true;

    DXInputString* pInputString = m_pEngineDevice->GetInputString();
	if (pInputString->IsOn())
        return false;

	//	만약, Native 모드일 경우 'Native'로 전환
	if (IsMODE_NATIVE())
	{
		pInputString->ConverseEngToNative();
	}

	m_bBeginEdit = true;	

	SET_STRING_BUFFER(m_strText);
	pInputString->OnInput();
	SetInsertPos();

	m_pCarrat->SetVisibleSingle ( TRUE );
	if ( m_pCarrat->IsUseRender () )
	{
		//m_pCarrat->SetDiffuse ( m_dwCarratDiffuse );
		// 캐럿 알파값을 고정시켰다.
		m_pCarrat->SetDiffuse ( D3DCOLOR_ARGB(127,255,255,255) );
	}

	return true;
}

bool CShotCutBox::EndEdit ()
{	
	if (!m_bBeginEdit)
        return false;

	m_pCarrat->SetVisibleSingle(FALSE);

    DXInputString* pInputString = m_pEngineDevice->GetInputString();
	//	만약, Native 모드일경우 영문으로 '무조건' 전환
	if (IsMODE_NATIVE())
	{
		pInputString->ConverseNativeToEng();
	}

	m_bBeginEdit = false;

	pInputString->OffInput();
	FLUSH_STRING_BUFFER();

	return true;
}

void CShotCutBox::ClearEdit ()
{
	FLUSH_STRING_BUFFER ();
	m_strText.Empty ();
	m_strRenderText.Empty ();

	m_xViewPos = 0;
	m_xInsertPos = 0;
	m_xInsertPosR = 0;
}

void CShotCutBox::SetEditString ( const CString& strInput )
{
	m_strText = strInput.Left( m_nLimitInput );
	m_strRenderText = m_strText;

	m_xViewPos = 0;

	// 현재 에디트 중인 컨트롤만 즉시 문장을 치환한다.
	// 그렇지 않은 것의 경우, 실제 전환이 일어났을 때 하는 것이 좋다.
	if ( IsBegin () )
	{
		SET_STRING_BUFFER ( m_strText );
		GetInsertPos();
	}
	else
	{
		m_xInsertPos = MultiToWidePos( &m_strText, m_strText.GetLength() );
		m_xInsertPosR = m_xInsertPos;
	}
}