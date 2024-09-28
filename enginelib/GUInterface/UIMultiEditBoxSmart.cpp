#include "pch.h"

#include "../EngineLib/Common/DXInputString.h"
#include "../EngineLib/DxTools/d3dfont.h"
#include "../EngineLib/DxTools/RENDERPARAM.h"


#include "../DxResponseMan.h"
#include "./UIMultiEditBoxSmart.h"
#include "./UIEditBox.h"
#include "./UIKeyCheck.h"
#include "./UITextUtil.h"
#include "./BasicCarrat.h"
#include "./UIRenderQueue.h"
#include "./BasicScrollBarEx.h"
#include "./BasicScrollThumbFrame.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

#define WTEMP_STRING 2048

// Note : bjju.MultiLineEditBox

// Note : ���ٷ� ��� 1024byte �� �Ѿ�� ������ �߻��Ѵ�. 
//		  ���߿� ���ľ��� wTemp[1024] �� �κ��̹���

// Note : ������
//		  1. �ڵ������� Ǯ�� ����. �� �κ��� �ʿ�� ����
//		  2. ���� ��ũ�ѹٴ� ������ �ʾ���.

CUIMultiEditBoxSmart::CUIMultiEditBoxSmart(EngineDeviceMan* pEngineDevice)
	: CUIEditBox(pEngineDevice)
    , m_nMousePosX		( 0 )
	, m_nMousePosY		( 0 )
	, m_nVScrollBorder	( 0 )
	, m_nHScrollBorder	( 0 )
	, m_nVertical		( 0 )
	, m_fCarratPosY		( 0.0f )	
	, m_nViewPosX		( 0 )
	, m_nViewPosY		( 0 )
	, m_nUpdateKeyDown	( 0	)
	, m_bUpdateChar		( false )
	, m_bVScroll		( false )
	, m_bHScroll		( false )
	, m_bAutoTurn		( false )
	, m_pVScrollBar		( NULL  )
{
	m_strCarrotPos  = "";
	m_strCarrotPosR = "";
}

CUIMultiEditBoxSmart::~CUIMultiEditBoxSmart()
{
	// Blank
}

void CUIMultiEditBoxSmart::CreateScrollVertical ( const UINT32 nBorder )
{
	m_nVScrollBorder = nBorder;

	// Note : ���� ��ũ�ѹ�
	if ( !m_pVScrollBar )
	{
		m_pVScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
		m_pVScrollBar->CreateSub	( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, MULTIEDITBOX_SCROLLBAR );
		m_pVScrollBar->CreateThumb  ( "BASIC_SCROLLBAR_THUMB_FRAME", "BASIC_SCROLLBAR_THUMB_MAIN", "BASIC_SCROLLBAR_THUMB_BODY", "BASIC_SCROLLBAR_THUMB_UP", "BASIC_SCROLLBAR_THUMB_DN", TRUE );
		m_pVScrollBar->CreateButton ( "BASIC_SCROLLBAR_BUTTONUP",	 "BASIC_SCROLLBAR_BUTTONUP_F", "BASIC_SCROLLBAR_BUTTONDN",	 "BASIC_SCROLLBAR_BUTTONDN_F", TRUE );
		m_pVScrollBar->GetThumbFrame()->SetState ( 1, 0 );

		CBasicScrollThumbFrame* const pThumbFrame = m_pVScrollBar->GetThumbFrame ();
		pThumbFrame->SetState	( 0,0  );
		pThumbFrame->SetPercent ( 0.0f );
	}

	const UIRECT& rcParentPos	  = GetLocalPos ();
	const UIRECT& rcControlOldPos = m_pVScrollBar->GetLocalPos ();

	UIRECT rcControlNewPos;
	rcControlNewPos.left   = rcParentPos.sizeX - (float)nBorder;
	rcControlNewPos.right  = rcParentPos.sizeX;
	rcControlNewPos.top	   = 0.0f;
	rcControlNewPos.bottom = rcParentPos.sizeY;
	rcControlNewPos.sizeX  = (float)nBorder;
	rcControlNewPos.sizeY  = rcParentPos.sizeY;

	m_pVScrollBar->AlignSubControl ( rcControlOldPos, rcControlNewPos );
	m_pVScrollBar->SetLocalPos	   ( D3DXVECTOR2 ( rcControlNewPos.left, rcControlNewPos.top ) );

	RegisterControl( m_pVScrollBar );

	SetScrollVertical ( true );
}

void CUIMultiEditBoxSmart::SetScrollVertical ( const bool bEnable )
{
	if ( bEnable )
	{
		if ( !m_pVScrollBar )
		{
			CreateScrollVertical ();
		}
	}

	m_bVScroll = bEnable;
}

int CUIMultiEditBoxSmart::GetEditWidth  ()
{
	int nSize = (int)( GetGlobalPos().sizeX - fLINE_END_GAP );

	if ( m_bVScroll )
	{
		nSize -= (int)m_nVScrollBorder;
	}

	if ( nSize < 0 )
	{
		nSize = 0;
	}

	return nSize;
}

int CUIMultiEditBoxSmart::GetEditHeight ()
{
	int nSize = (int)( GetGlobalPos().sizeY - fLINE_END_GAP );

	if ( m_bHScroll )
	{
		nSize -= (int)m_nHScrollBorder;
	}

	if ( nSize < 0 )
	{
		nSize = 0;
	}

	return nSize;
}

void CUIMultiEditBoxSmart::SetInsertPos()
{
	CUIEditBox::SetInsertPos();
}

void CUIMultiEditBoxSmart::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case MULTIEDITBOX_SCROLLBAR :
		break;

	default:
		if ( CHECK_MOUSE_IN ( dwMsg ) && (dwMsg&UIMSG_LB_DOWN) )
		{
			MouseDownLButtonToEdit ( m_nMousePosX, m_nMousePosY );
		}
		break;
	}
}

void CUIMultiEditBoxSmart::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	m_nMousePosX = x;
	m_nMousePosY = y;

	// Note : ���� ��ũ�� �� Visible
	if ( m_pVScrollBar )
	{
		if ( m_bVScroll )
		{
			m_pVScrollBar->SetVisibleSingle( TRUE );
		}
		else
		{
			m_pVScrollBar->SetVisibleSingle( FALSE );
		}
	}

	if ( !IsVisible () ) return;
	if ( !m_pFont )		 return;
	if ( !m_pCarrat )	 return;

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	const int lEditWidth  = GetEditWidth  ();
	const int lEditHeight = GetEditHeight ();

	if ( IsBegin () )
	{
		m_bUpdateChar	 = false;
		m_nUpdateKeyDown = 0;

		//	GetInputInstance().Move( CRect(100, 100, 500, 500) );
		//CUIControl* pParent = GetTopParent ();
		//BOOL bFocus = pParent->IsFocusControl();
		//if ( !bFocus )
		//{
		//	EndEdit ();
		//	return;
		//}

        //DXInputString* pInputString = DxResponseMan::GetInstance().GetInputString();
        DXInputString* pInputString = m_pEngineDevice->GetInputString();
		if (!pInputString->IsOn())
		{
			EndEdit ();
			return;
		}

		SetMODE_NATIVE(pInputString->IsNativeMode() ? true : false);

		switch( RENDERPARAM::emSERVICE_TYPE )
		{
		case RENDERPARAM::EMSERVICE_INDONESIA:
		case RENDERPARAM::EMSERVICE_MALAYSIA_EN:
		case RENDERPARAM::EMSERVICE_PHILIPPINES:
			//		case EMSERVICE_VIETNAM:
			if (IsMODE_NATIVE())
			{
				pInputString->ConverseNativeToEng();
			}
			break;
		};

		if (!m_bKeyInput)
			pInputString->ConverseFullToHalf();

		CString strInput = pInputString->GetString();
		int xInsertPos   = pInputString->GetInsertPos();

		//CDebugSet::ToView ( 1, 16, "[Origin]%s", strInput );

		// Note : ESC Ű�� ����������
		if( DxInputDevice::GetInstance().GetKeyState ( DIK_ESCAPE ) & DXKEY_DOWNED )
		{
			EndEdit();
			return;
		}

		// 	�����Է� �̿ܿ� ������� Ű�� ������ ���, ��� ����
		m_bVALID_INPUT = true;
		if( !m_bMinusSign && ( strInput[0] == '-' ) ) m_bVALID_INPUT = false;
		if( DxInputDevice::GetInstance().GetKeyState ( DIK_LCONTROL ) & DXKEY_DOWNED )	m_bVALID_INPUT = false;
		if( DxInputDevice::GetInstance().GetKeyState ( DIK_LMENU ) & DXKEY_DOWNED )		m_bVALID_INPUT = false;
		if( DxInputDevice::GetInstance().GetKeyState ( DIK_RCONTROL ) & DXKEY_DOWNED )	m_bVALID_INPUT = false;
		if( DxInputDevice::GetInstance().GetKeyState ( DIK_RMENU ) & DXKEY_DOWNED )		m_bVALID_INPUT = false;

		if( !m_bVALID_INPUT )
		{
			SET_STRING_BUFFER ( m_strText );
			SetInsertPos();
			return;
		}

		// Note : Ű �Է��� �Ǿ��� ����
		m_bUpdateChar	 = pInputString->IsOnChar();
		m_nUpdateKeyDown = pInputString->IsOnKeyDown ();

		//	�Է� ����â ��ġ ����
		const UIRECT& rcGlobalPos = GetGlobalPos ();
		CRect rtEditBox ( (int)rcGlobalPos.left, (int)rcGlobalPos.top, (int)rcGlobalPos.right, (int)rcGlobalPos.bottom ); 
		pInputString->Move(rtEditBox);

		//	�Է��Ѱ踦 ���� ���
		if ( m_nLimitInput < strInput.GetLength () )
		{
			m_bLimited = true;

			SET_STRING_BUFFER ( m_strText );
			SetInsertPos();
			return;	
		}
		else
		{
			m_bLimited = false;
		}

		// Note : ���� ��ũ���� ���ٸ� ���� ������ �Ѿ�� ���ϰ� �Ѵ�.
		if ( m_bUpdateChar )
		{
			static MULTISTRING sMulti;
			UpdateMultiString( strInput, sMulti );

			int nVertical = (int)sMulti.size();

			SIZE sizeTemp;
			m_pFont->GetTextExtent( " ", sizeTemp ); // ��Ʈ�� ����� �˾ƿ��� ����.

			// Note : ���� ��ũ�ѹ��� ���� �缳��
			if ( m_pVScrollBar && (0 < sizeTemp.cy) )
			{
				int nEditVertical = (int)lEditHeight/sizeTemp.cy;

				CBasicScrollThumbFrame* const pThumbFrame = m_pVScrollBar->GetThumbFrame ();
				pThumbFrame->SetState ( nVertical, nEditVertical );
			}

			// Note : ���� ���� ������ �Ѵ´ٸ�
			if ( sizeTemp.cy*nVertical > lEditHeight )
			{
				if ( !m_bVScroll )
				{
					m_bLimited = true;

					SET_STRING_BUFFER ( m_strText );
					SetInsertPos();
					return;
				}
				// Note : ���� ��ũ�� �� ����
				else if ( m_pVScrollBar && (0 < sizeTemp.cy) )
				{
					int nEditVertical = (int)lEditHeight/sizeTemp.cy;
					int	nCurGap		  = (nVertical   - nEditVertical);
					int	nPreGap		  = (m_nVertical - nEditVertical);

					if ( nCurGap && nPreGap )
					{
						float fCurRatio = 1.0f/(float)nCurGap;
						float fPreRatio = 1.0f/(float)nPreGap;

						CBasicScrollThumbFrame* const pThumbFrame = m_pVScrollBar->GetThumbFrame ();
						pThumbFrame->SetPercent ( pThumbFrame->GetPercent() * ( fCurRatio/fPreRatio ) );
					}
				}
			}

			m_nVertical = nVertical;
		}

		// �Է� ���� ���ڰ� ��ȿ�ϹǷ� m_xInsertPos �����Ѵ�
		m_xInsertPos  = xInsertPos;
		m_xInsertPosR = m_xInsertPos;

		//	���ڸ� �Է� ���
		if( IsUSE_NUMBER() && !strInput.IsEmpty() )
		{
			// ���� ���ڿ� 0�� �����Ѵ�.
			while( TRUE )
			{
				if( strInput.IsEmpty() || (_istdigit( strInput[0] ) ) ) //&& strInput[0] != '0') )
					break;

				strInput.Delete( 0, 1 );
			}

			// ���� ���ڵ��� �����Ѵ�.
			int strLength = strInput.GetLength();
			for ( int i = 0; i < strLength; ++i )
			{
				if( !_istdigit( strInput[i] ) )
				{
					strInput.Delete( i, strLength - i );
					break;
				}
			}

			SET_STRING_BUFFER( strInput );
			SetInsertPos();

			//CDebugSet::ToView ( 1, 17, "[bNumber]%s", strInput );
		}

		m_strText = strInput;
		SetRenderText ( m_strText );

		//CDebugSet::ToView ( 1, 18, "[m_strText]%s", m_strText );
		//CDebugSet::ToView ( 1, 19, "[m_strRenderText]%s", m_strRenderText );
	}

	//	�Է°� ������
	if ( IsHide () )
	{
		const char cAsterisk = '*';
		std::string strAsterisk ( m_strRenderText.GetLength (), cAsterisk );
		SetRenderText ( strAsterisk.c_str() );
	}

	//	�� ���� ���
	if ( m_nMoneyUNIT && !m_strText.IsEmpty() )
	{
		LONGLONG lnMoney = (LONGLONG) _ttoi64( m_strText.GetString() );
		if( lnMoney > 0)
		{
			SetRenderText ( NS_UITEXTUTIL::MAKE_MONEY_FORMAT ( lnMoney, m_nMoneyUNIT, m_strUNIT_SYMBOL ) );

			if( DXInputString::m_bCaratMove )
			{
				// ���ԵǴ� �ɹ��� ���缭 m_xInsertPosR ����
				int iLen = m_strText.GetLength();

				for( int i=iLen-m_nMoneyUNIT; i>0; i-=m_nMoneyUNIT )
				{
					if( i < m_xInsertPos )
						m_xInsertPosR += m_strUNIT_SYMBOL.GetLength();
				}
			}
		}
	}

	UpdateRenderText();
	UpdatePosition();

	// �ؽ��Ŀ� ���ڸ� �������ϴ� ������� ����
	//m_pFont->PushText( (TCHAR*)m_strRenderTextR.GetString() );
}

void CUIMultiEditBoxSmart::MouseDownLButtonToEdit ( const int nMousePosX, const int nMousePosY )
{
	if ( !m_pFont )
	{
		return;
	}

	SIZE sizeTemp;
	m_pFont->GetTextExtent( " ", sizeTemp ); // ��Ʈ�� ����� �˾ƿ��� ����.

	const LONG lEditWidth  = GetEditWidth  ();
	const LONG lEditHeight = GetEditHeight ();
	const LONG lVertical   = (LONG)m_MultiRenderString.size();
	const LONG lFontHeight = (LONG)sizeTemp.cy;
	
	const UIRECT& rcParentPos = GetGlobalPos ();

	if ( lFontHeight <= 0 || lVertical <= 0 )
	{
		return;
	}

	if ( ((float)nMousePosX < rcParentPos.left) || ((rcParentPos.left + (float)lEditWidth) < (float)nMousePosX) )
	{
		return;
	}

	if ( ((float)nMousePosY < rcParentPos.top) || ((rcParentPos.top + (float)lEditHeight) < (float)nMousePosY) )
	{
		return;
	}

	const int  nLocalPosX    = static_cast<const int>(nMousePosX - rcParentPos.left);
	const int  nLocalPosY    = static_cast<const int>(nMousePosY - rcParentPos.top);
	const LONG lEditVertical = (LONG)lEditHeight/lFontHeight;

	if ( nLocalPosX < 0.0f || nLocalPosY < 0.0f )
	{
		return;
	}

	LONG lSelectIndexY = ((LONG)nLocalPosY/lFontHeight);

	if ( lVertical <= lSelectIndexY )
	{
		lSelectIndexY = lVertical-1;
	}

	MULTISTRINGITER	  iter	  = m_MultiRenderString.begin() + lSelectIndexY;
	SMULTISTRINGINFO& sInfo   = (*iter);
	CString&		  strLine = sInfo.Data;

	int nIndexPosX = m_pFont->GetTextCountByWidth( strLine, static_cast<const float>(nLocalPosX), 0.5f );
	int nIndexPosY = lSelectIndexY;

	// Note : ViewPos�� ���Ѵ�.
	nIndexPosX += m_nViewPosX;
	nIndexPosY += m_nViewPosY;

	SetCarratPosition ( m_MultiString, nIndexPosX, nIndexPosY );
}

INT CUIMultiEditBoxSmart::WideToMultiPos ( CString& strSource, INT iWidePos )
{
	WCHAR wTemp[ WTEMP_STRING ];
	m_pFont->MultiToWide( strSource.GetString(), strSource.GetLength(), wTemp, 1022 );
	CStringW strTempW = wTemp;

	return CUIEditBox::WideToMultiPos( &strTempW, iWidePos );
}

bool CUIMultiEditBoxSmart::BeginEdit ()
{
	if (m_bBeginEdit)
        return true;

    //DXInputString* pInputString = DxResponseMan::GetInstance().GetInputString();
    DXInputString* pInputString = m_pEngineDevice->GetInputString();
	if (pInputString->IsOn())
        return false;

	//	����, Native ����� ��� 'Native'�� ��ȯ
	if ( IsMODE_NATIVE() )
	{
		pInputString->ConverseEngToNative();
	}

	m_bBeginEdit = true;

	pInputString->OnInput(EMIMETYPE_MULTILINE);
	SET_STRING_BUFFER ( m_strText );
	SetInsertPos();

	if ( m_pCarrat->IsUseRender () )
	{
		m_pCarrat->SetDiffuse ( m_dwCarratDiffuse );
	}

	// Note : ĳ�� ��ġ�� �°� ��ũ�� �ٸ� �����Ѵ�.
	{
		if ( m_bVScroll && m_pVScrollBar )
		{
			int nPosX	   = 0;
			int nPosY	   = 0;
			int MInsertPos = WideToMultiPos( m_strText, m_xInsertPos );

			GetIndexPosition( m_MultiString, MInsertPos, nPosX, nPosY );
			UpdateScrollByCarrat ( m_MultiString, nPosX, nPosY );
		}
	}

	return true;
}

bool CUIMultiEditBoxSmart::EndEdit ()
{	
	if ( !m_bBeginEdit ) return false;

	m_pCarrat->SetVisibleSingle ( FALSE );

    //DXInputString* pInputString = DxResponseMan::GetInstance().GetInputString();
    DXInputString* pInputString = m_pEngineDevice->GetInputString();
	//	����, Native ����ϰ�� �������� '������' ��ȯ
	if (IsMODE_NATIVE())
		pInputString->ConverseNativeToEng();

	m_bBeginEdit = false;
	pInputString->OffInput();
	FLUSH_STRING_BUFFER();

	return true;
}

void CUIMultiEditBoxSmart::ClearEdit ()
{
	CUIEditBox::ClearEdit();

	m_MultiString.clear();
	m_MultiRenderString.clear();

	m_nVertical = 0;

	m_strCarrotPos  = "";
	m_strCarrotPosR = "";

	m_fCarratPosY = 0.0f;

	m_nViewPosX = 0;
	m_nViewPosY = 0;

	m_bUpdateChar	 = false;
	m_nUpdateKeyDown = false;

	// Note : ��ũ�� �� �ʱ�ȭ
	if ( m_bVScroll && m_pVScrollBar )
	{
		CBasicScrollThumbFrame* const pThumbFrame = m_pVScrollBar->GetThumbFrame ();
		pThumbFrame->SetState   ( 0,0 );
		pThumbFrame->SetPercent ( 0.0f );
	}
}

HRESULT CUIMultiEditBoxSmart::Render ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;

	hr = CUIGroup::Render ( pd3dDevice );
	if ( FAILED ( hr ) ) return hr;

	CUIRenderQueue::Get()->RenderImmediately();

	// �ؽ��Ŀ� ���ڸ� �������ϴ� ������� ����
	// Ȯ��ĳ���� ��� ���� ���� ó�� 
	if( IsBegin() && m_bWideCarret &&m_pCarrat->IsRender() )
	{
		int nSel = (int)(( m_pCarrat->GetGlobalPos().left - m_vRenderPos.x) / 6);// ĳ���� ������ ���� ��ġ(���ڸ�)
		CString strRenderTextL = m_strCarrotPosR.Left( nSel ) ;
		CString strRenderText  = m_strCarrotPosR.Mid( nSel, 2 ) ; // ���� ����

		SIZE TextSize;
		m_pFont->GetTextExtent ( strRenderTextL.GetString(), TextSize );
		D3DXVECTOR2 vRenderPos = m_vRenderPos;		
		vRenderPos.x = vRenderPos.x + (float)TextSize.cx;
		vRenderPos.y = vRenderPos.y + m_fCarratPosY;

		// Note : ��Ƽ ���� ��Ʈ�� ����Ʈ�� �׸���.
		MULTISTRINGITER iter = m_MultiRenderString.begin();

		for ( ; iter!=m_MultiRenderString.end(); ++iter )
		{
			SMULTISTRINGINFO& sInfo   = (*iter);
			CString&		  strLine = sInfo.Data;

			if ( strLine.IsEmpty() )
			{
				continue;
			}

			int nLength = strLine.GetLength();

			if ( nLength >= 2 )
			{
				if ( strLine [ nLength - 2 ] == L'\r' )
				{
					if ( nLength == 2 )
					{
						continue;
					}

					nLength -= 2;
				}
			}

			hr = m_pFont->DrawText( sInfo.Pos.x, sInfo.Pos.y, m_dwTextColor, strLine.Left( nLength ).GetString(), 0L, FALSE );
			if ( FAILED ( hr ) ) return hr;
		}	

		m_pFont->DrawText( vRenderPos.x, vRenderPos.y, NS_UITEXTUTIL::BLACK, strRenderText.GetString(), 0L, FALSE );	

		return S_OK;
	}

	// Note : ��Ƽ ���� ��Ʈ�� ����Ʈ�� �׸���.
	MULTISTRINGITER iter = m_MultiRenderString.begin();

	for ( ; iter!=m_MultiRenderString.end(); ++iter )
	{
		SMULTISTRINGINFO& sInfo   = (*iter);
		CString&		  strLine = sInfo.Data;

		if ( strLine.IsEmpty() )
		{
			continue;
		}

		int nLength = strLine.GetLength();

		if ( nLength >= 2 )
		{
			if ( strLine [ nLength - 2 ] == L'\r' )
			{
				if ( nLength == 2 )
				{
					continue;
				}

				nLength -= 2;
			}
		}

		hr = m_pFont->DrawText( sInfo.Pos.x, sInfo.Pos.y, m_dwTextColor, strLine.Left( nLength ).GetString(), 0L, FALSE );
		if ( FAILED ( hr ) ) return hr;
	}	

	return S_OK;
}

void CUIMultiEditBoxSmart::UpdateRenderText()
{
	m_strRenderTextR = m_strRenderText;

	// Note : ������Ʈ�� ���� �ÿ��� ����
	if ( m_bUpdateChar )
	{
		UpdateMultiString ( m_strRenderTextR, m_MultiString );
	}

	int nVertical = (int)m_MultiString.size();

	if( DXInputString::m_bCaratMove == FALSE )
		return;

	const int lEditWidth  = GetEditWidth  ();
	const int lEditHeight = GetEditHeight ();

	int iLen = m_strRenderTextR.GetLength();

	// Note : ��Ʈ�� ���̸� �˾ƿ´�.
	CString strTemp;
	SIZE    sizeTemp;
	m_pFont->GetTextExtent( " ", sizeTemp );

	LONG lFontHeight = sizeTemp.cy;

	// Note : Insert ���� �����´�.
	int iInsertPosRM = m_xInsertPosR;

	if( m_pFont )
	{
		iInsertPosRM = WideToMultiPos( m_strRenderTextR, m_xInsertPos );
	}

	// Note : ĳ���� ��ġ���� ������
	int nCarrotPosX = 0;
	int nCarrotPosY = 0;

	GetIndexPosition ( m_MultiString, iInsertPosRM, nCarrotPosX, nCarrotPosY );

	// Note : �Է� Ű�� ���� ó���� �Ѵ�.
	if ( m_nUpdateKeyDown )
	{
		switch ( m_nUpdateKeyDown )
		{
		// Note : �� ó���� IME���� �Ѵ�.
		case VK_HOME   :
		case VK_END    :
		case VK_DELETE :
		case VK_LEFT   :
		case VK_RIGHT  :
			break;

		case VK_UP:
			{
				if ( 1 <= nCarrotPosY )
				{
					SetCarratPosition ( m_MultiString, nCarrotPosX, --nCarrotPosY );
					iInsertPosRM = WideToMultiPos( m_strRenderTextR, m_xInsertPos );
				}
			}
			break;

		case VK_DOWN:
			{
				if ( nCarrotPosY+1 < (int)m_MultiString.size() )
				{
					SetCarratPosition ( m_MultiString, nCarrotPosX, ++nCarrotPosY );
					iInsertPosRM = WideToMultiPos( m_strRenderTextR, m_xInsertPos );
				}
			}
			break;
		};
	}

	// Note : Ŀ���� ���� ��ũ�ѹ��� ���� ����
	if ( m_nUpdateKeyDown )
	{
		UpdateScrollByCarrat ( m_MultiString, nCarrotPosX, nCarrotPosY );
	}

	// Note : �ش� �ε����� ��ġ�� ������ ��Ʈ���� �����´�.
	CString strLine = "";

	if ( nCarrotPosY < nVertical )
	{
		strLine = m_MultiString[nCarrotPosY].Data;
	}

	m_strCarrotPos = strLine;

	// Note : VIEW ���� �����´�.
	int iViewPosM = m_xViewPos;

	if( m_pFont )
	{
		iViewPosM = WideToMultiPos( strLine, m_xViewPos );
	}

	// Note : ��ũ�� X ��ǥ VIEW ( X ��ǥ�� ��ũ�ѹٰ� ��� �̾����� �Ѵ� )
	{
		// nPosX �� m_xViewPos ���� �տ� �ִٸ� ����
		if( nCarrotPosX < iViewPosM )
		{
			iViewPosM = nCarrotPosX;

			// ����¡
			for( int i=0; i<7; ++i )
			{
				if( iViewPosM <= 0 )
					break;
				if( NS_UITEXTUTIL::IsDBCS( strLine[ iViewPosM - 1 ] ) )
					iViewPosM -= 2;
				else
					iViewPosM -= 1;
			}

			iViewPosM = new_max( iViewPosM, 0 );
		}
		
		// ��¹��� ( iViewPosM ���� lEditWidth ��ŭ ) �ȿ� nCarrotPosX �� ���ٸ� ����
		if( nCarrotPosX - iViewPosM > 0 )
		{
			strTemp = strLine.Mid( iViewPosM, nCarrotPosX - iViewPosM );
			m_pFont->GetTextExtent( strTemp.GetString(), sizeTemp );

			if( sizeTemp.cx > lEditWidth )
			{
				while( sizeTemp.cx > lEditWidth )
				{
					if( NS_UITEXTUTIL::IsDBCS( strLine[ iViewPosM ] ) )
						iViewPosM += 2;
					else
						iViewPosM += 1;

					strTemp = strLine.Mid( iViewPosM, nCarrotPosX - iViewPosM );
					m_pFont->GetTextExtent( strTemp.GetString(), sizeTemp );
				}

				// ����¡
				int iTemp = nCarrotPosX;
				for( int i=0; i<7; ++i )
				{
					if( iTemp >= iLen )
						break;
					if( NS_UITEXTUTIL::IsDBCS( strLine[ iViewPosM ] ) )
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
	} // ��ũ�� X ��ǥ VIEW END

	// Note : ��ġ ���
	{
		CString strView		 = "";
		CString strViewBegin = "";

		if ( nCarrotPosY < (int)m_MultiString.size() )
		{
			strView		 = strLine.Mid( iViewPosM, (nCarrotPosX - iViewPosM) );
			strViewBegin = strLine.Mid( 0, iViewPosM );
		}

		// Note : �� ��ġ
		m_pFont->GetTextExtent ( strViewBegin.GetString(), sizeTemp );
		m_nViewPosX  = sizeTemp.cx;

		// Note : Y ��ũ�ѹٸ� ���󰣴�
		if ( m_bVScroll && lFontHeight )
		{
			CBasicScrollThumbFrame* const pThumbFrame = m_pVScrollBar->GetThumbFrame ();
			float fPercent	    = pThumbFrame->GetPercent();
			int	  nEditVertical = (int)lEditHeight/lFontHeight;

			if ( nEditVertical < nVertical )
			{
				float fRatio = 1.0f/(float)(nVertical - nEditVertical);
				m_nViewPosY = (int)(fPercent/fRatio);
			}
			else
			{
				m_nViewPosY = 0;
			}
		}
		else
		{
			m_nViewPosY = 0;
		}

		// Note : ĳ�� ��ġ
		m_pFont->GetTextExtent ( strView.GetString(), sizeTemp );
		m_fCarratPos  = (float)sizeTemp.cx;
		m_fCarratPosY = (float)((nCarrotPosY-m_nViewPosY) * lFontHeight);

		// Note : ���� ������ ������ ��Ʈ��
		m_strCarrotPosR = "";

		if ( nCarrotPosY < (int)m_MultiString.size() )
		{
			int nLen = (int)strLine.GetLength();

			if ( iViewPosM < nLen )
			{
				m_strCarrotPosR	= strLine.Mid( iViewPosM, (nLen - iViewPosM) );
			}
		}
	}

	// Note : ������ ��ġ iViewPosM ( ��Ƽ ����Ʈ ) <= ����ȭ => m_xViewPos ( �����ڵ� )
	m_xViewPos = MultiToWidePos( &strLine, iViewPosM );

	// Note : ���������� �������� ��Ƽ��Ʈ���� ������Ʈ
	UpdateMultiRenderString( m_MultiString );
}

void CUIMultiEditBoxSmart::UpdatePosition ()
{
	if( DXInputString::m_bCaratMove )
	{
		MULTISTRINGITER iter  = m_MultiRenderString.begin();
		float			fPosY = 0.0f;

		for ( ; iter!=m_MultiRenderString.end(); ++iter )
		{
			SMULTISTRINGINFO& sInfo   = (*iter);
			CString&		  strLine = sInfo.Data;

			SIZE TextSize;
			m_pFont->GetTextExtent ( strLine.GetString(), TextSize );

			float fHeight = (float)TextSize.cy;

			//	���� �Ѹ� ��ġ
			UIRECT rcEditBoxPos = GetGlobalPos ();
			{
				const UIRECT rcCarratPos = m_pCarrat->GetLocalPos ();
				sInfo.Pos.x = rcEditBoxPos.left;
				switch ( m_nAlign )
				{
				case EDIT_CENTER_ALIGN:
					sInfo.Pos.x = rcEditBoxPos.left + ((rcEditBoxPos.sizeX - (float)TextSize.cx) / 2.0f);
					break;

				case EDIT_RIGHT_ALIGN:
					sInfo.Pos.x = rcEditBoxPos.right - (float)TextSize.cx;
					break;
				}
			}

			sInfo.Pos.y = rcEditBoxPos.top + fPosY;

			fPosY += fHeight;
		}

		// ���� ���� �Ѹ� ��ġ
		SIZE TextSize;
		m_pFont->GetTextExtent ( m_strCarrotPosR.GetString(), TextSize );

		UIRECT rcEditBoxPos = GetGlobalPos ();
		{
			const UIRECT rcCarratPos = m_pCarrat->GetLocalPos ();
			m_vRenderPos.x = rcEditBoxPos.left;
			switch ( m_nAlign )
			{
			case EDIT_CENTER_ALIGN:
				m_vRenderPos.x = rcEditBoxPos.left + ((rcEditBoxPos.sizeX - (float)TextSize.cx) / 2.0f);
				break;

			case EDIT_RIGHT_ALIGN:
				m_vRenderPos.x = rcEditBoxPos.right - (float)TextSize.cx;
				break;
			}

			m_vRenderPos.y = rcEditBoxPos.top + rcCarratPos.top;
		}

		//	ĳ�� �Ѹ� ��ġ
		UIRECT rcCarratPos = m_pCarrat->GetGlobalPos ();
		{
			rcCarratPos.left = m_vRenderPos.x + m_fCarratPos;
			rcCarratPos.top  = m_vRenderPos.y + m_fCarratPosY;
			m_pCarrat->SetGlobalPos ( rcCarratPos );

			// Note : Ŀ���� ��â�� �������� �������� �ʴ´�.
			const int nEditWidth  = GetEditWidth  ();
			const int nEditHeight = GetEditHeight ();

			// Y��
			{
				if ( m_bBeginEdit )
				{
					m_pCarrat->SetVisibleSingle ( TRUE );

					if ( rcCarratPos.top < rcEditBoxPos.top )
					{
						m_pCarrat->SetVisibleSingle ( FALSE );
					}

					if ( rcEditBoxPos.bottom < rcCarratPos.top + (float)TextSize.cy )
					{
						m_pCarrat->SetVisibleSingle ( FALSE );
					}
				}
				else
				{
					m_pCarrat->SetVisibleSingle ( FALSE );
				}
			}
		}
	}
	else
	{
		const long lEditWidth = (long)(GetGlobalPos().sizeX - fLINE_END_GAP);

		//	���ڿ��� ����Ʈ�ڽ��� �Ѿ ��� �պκ� �߶󳻱�
		MULTISTRINGITER iter  = m_MultiRenderString.begin();
		float			fPosY = 0.0f;

		for ( ; iter!=m_MultiRenderString.end(); ++iter )
		{
			SMULTISTRINGINFO& sInfo   = (*iter);
			CString&		  strLine = sInfo.Data;

			SIZE TextSize;
			m_pFont->GetTextExtent ( strLine.GetString(), TextSize );

			float fHeight = (float)TextSize.cy;

			if ( lEditWidth < TextSize.cx )
			{
				int nStrLength = strLine.GetLength ();
				while ( lEditWidth < TextSize.cx && nStrLength )
				{
					int nCut = nStrLength - 1;		
					if ( NS_UITEXTUTIL::IsDBCS ( strLine[0] ) )	nCut--;

					strLine = strLine.Right ( nCut );

					nStrLength = strLine.GetLength ();
					m_pFont->GetTextExtent ( strLine.GetString (), TextSize );
				}
			}

			UIRECT rcEditBoxPos = GetGlobalPos ();
			{
				const UIRECT rcCarratPos = m_pCarrat->GetLocalPos ();
				sInfo.Pos.x = rcEditBoxPos.left;
				switch ( m_nAlign )
				{
				case EDIT_CENTER_ALIGN:
					sInfo.Pos.x = rcEditBoxPos.left + ((rcEditBoxPos.sizeX - (float)TextSize.cx) / 2.0f);
					break;

				case EDIT_RIGHT_ALIGN:
					sInfo.Pos.x = rcEditBoxPos.right - (float)TextSize.cx;
					break;
				}

				sInfo.Pos.y = rcEditBoxPos.top + fPosY;
			}

			fPosY += fHeight;
		}

		//	���� ���� �Ѹ� ��ġ
		SIZE TextSize;
		m_pFont->GetTextExtent ( m_strCarrotPosR.GetString(), TextSize );

		UIRECT rcEditBoxPos = GetGlobalPos ();
		{
			const UIRECT rcCarratPos = m_pCarrat->GetLocalPos ();
			m_vRenderPos.x = rcEditBoxPos.left;
			switch ( m_nAlign )
			{
			case EDIT_CENTER_ALIGN:
				m_vRenderPos.x = rcEditBoxPos.left + ((rcEditBoxPos.sizeX - (float)TextSize.cx) / 2.0f);
				break;

			case EDIT_RIGHT_ALIGN:
				m_vRenderPos.x = rcEditBoxPos.right - (float)TextSize.cx;
				break;
			}

			m_vRenderPos.y = rcEditBoxPos.top + rcCarratPos.top;
		}

		//	ĳ�� �Ѹ� ��ġ
		UIRECT rcCarratPos = m_pCarrat->GetGlobalPos ();
		{
			rcCarratPos.left = m_vRenderPos.x + (float)TextSize.cx;
			rcCarratPos.top  = m_vRenderPos.y + m_fCarratPosY;
			m_pCarrat->SetGlobalPos ( rcCarratPos );
		}
	}

	{ // �ѱ���� ���� �����϶� ĳ���� Ȯ���Ѵ�.
		UIRECT rcCaretOldPos = m_pCarrat->GetGlobalPos();
		UIRECT rcCaretNewPos(rcCaretOldPos);

        //DXInputString* pInputString = DxResponseMan::GetInstance().GetInputString();
        DXInputString* pInputString = m_pEngineDevice->GetInputString();
		if (pInputString->CheckWideCaret())
		{
			m_bWideCarret = true;
			rcCaretNewPos.sizeX = 12; // ��Ʈ 9, ����ü���� ���� �ϳ��� ������
			if (!DXInputString::m_bCaratMove)
                rcCaretNewPos.left -= 12;
		}
		else
		{
			m_bWideCarret = false;
			rcCaretNewPos.sizeX = m_fCaretSizeX;
		}

		m_pCarrat->SetGlobalPos( rcCaretNewPos );
		m_pCarrat->AlignSubControl( rcCaretOldPos, rcCaretNewPos );
	}
}

void CUIMultiEditBoxSmart::UpdateMultiString ( const CString& strRender, MULTISTRING& strMult )
{
	strMult.clear();

	const int lEditWidth = GetEditWidth ();

	int	nBegin	= 0;
	int	nLength	= strRender.GetLength();

	if ( !nLength )
	{
		return;
	}

	for ( int i=0; i<nLength; ++i )
	{
		if ( strRender[i] == L'\n' )
		{
			SMULTISTRINGINFO sInfo;
			sInfo.Data = strRender.Mid( nBegin, (i-nBegin) + 1 );
			strMult.push_back( sInfo );

			nBegin = i+1;
		}
	}

	SMULTISTRINGINFO sInfo;
	sInfo.Data = strRender.Mid( nBegin, (nLength-nBegin) );
	strMult.push_back( sInfo );

	// Note : �ڵ�����
	if ( m_bAutoTurn )
	{
		MULTISTRINGITER iter = strMult.begin();

		for ( ; iter!=strMult.end(); ++iter )
		{
			SMULTISTRINGINFO& sInfo   = (*iter);
			CString			  strLine = sInfo.Data; 
			
			SIZE sizeTemp;
			m_pFont->GetTextExtent( strLine, sizeTemp );

			while ( sizeTemp.cx > lEditWidth )
			{
				SMULTISTRINGINFO& sTempInfo = (*iter);
				int				  nCount	= 0;

				strLine  = sTempInfo.Data;
				int	nLen = strLine.GetLength();
				
				int nEditWidthCount = m_pFont->GetTextCountByWidth ( sTempInfo.Data.GetString(), (float)lEditWidth );
				if ( !nEditWidthCount )
				{
					break;
				}

				sTempInfo.Data = strLine.Left ( nEditWidthCount );

				SMULTISTRINGINFO sInsertInfo;
				sInsertInfo.Data = strLine.Right( nLen - nEditWidthCount );

				if ( sInsertInfo.Data.IsEmpty() )
				{
					break;
				}

				iter = strMult.insert( ++iter, sInsertInfo );

				if ( iter == strMult.end() )
				{
					break;
				}

				m_pFont->GetTextExtent( sInsertInfo.Data.GetString(), sizeTemp );
			}
		}
	}
}

void CUIMultiEditBoxSmart::UpdateMultiRenderString ( MULTISTRING& strMulti )
{
	const int lEditWidth    = GetEditWidth  ();
	const int lEditHeight   = GetEditHeight ();
	int		  nEditVertical = 0;

	// Note  ���������� �� �� �ִ� �ִ� ���̸� ���Ѵ�.
	SIZE sizeTemp;
	m_pFont->GetTextExtent( " ", sizeTemp );

	if ( sizeTemp.cy )
	{
		nEditVertical = (int)lEditHeight/sizeTemp.cy;
	}

	// Note : m_MultiRenderString �� ä���.
	m_MultiRenderString.clear();

	MULTISTRINGITER iter   = strMulti.begin() + m_nViewPosY;
	int				nCount = 0;

	for ( ; iter!=strMulti.end(); ++iter, ++nCount )
	{
		// Note : ����Ʈ�ڽ��� ǥ���� �� �ִ� �ִ� ������ ���� �����ϸ�
		if ( nEditVertical <= nCount )
		{
			break;
		}

		SMULTISTRINGINFO& sInfo   = (*iter);
		CString&		  strLine = sInfo.Data;
		int				  nLength = strLine.GetLength();

		if( nLength > 0 )
		{
			m_pFont->GetTextExtent( strLine.GetString(), sizeTemp );

			float fHeight = (float)sizeTemp.cy;

			if ( sizeTemp.cx < m_nViewPosX )
			{
				SMULTISTRINGINFO sInfo;
				sInfo.Data = CString(L"\r\n");

				m_MultiRenderString.push_back( sInfo );
				continue;
			}
			
			CString strLineRender;
			
			// X�� ����
			{
				// Note : �պκ� �ڸ���
				{
					if ( m_xViewPos )
					{
						// Note : ��ȯ�� View���� �����´�.
						int iLineViewPosM = WideToMultiPos( strLine, m_xViewPos );
						strLineRender     = strLine.Right( nLength - iLineViewPosM );
					}
					else
					{
						strLineRender = strLine;
					}
				}

				// Note : �޺κ� �ڸ���
				{
					m_pFont->GetTextExtent( strLineRender.GetString(), sizeTemp );
					int nRenderLength = strLineRender.GetLength();

					while( sizeTemp.cx > lEditWidth )
					{
						if( NS_UITEXTUTIL::IsDBCS( strLineRender[ nRenderLength-1 ] ) )
							strLineRender = strLineRender.Left( nRenderLength-2 );
						else
							strLineRender = strLineRender.Left( nRenderLength-1 );
						m_pFont->GetTextExtent( strLineRender.GetString(), sizeTemp );
						nRenderLength = strLineRender.GetLength();
					}
				}
			}

			SMULTISTRINGINFO sInfo;
			sInfo.Data = strLineRender;

			m_MultiRenderString.push_back( sInfo );
		}
		else
		{
			SMULTISTRINGINFO sInfo;
			m_MultiRenderString.push_back( sInfo );
		}
	}
}

void CUIMultiEditBoxSmart::UpdateScrollByCarrat	( MULTISTRING& strMulti, const int nPosX, const int nPosY )
{
	SIZE sizeTemp;
	m_pFont->GetTextExtent( " ", sizeTemp );

	LONG lEditWidth  = GetEditWidth  ();
	LONG lEditHeight = GetEditHeight ();
	LONG lFontHeight = sizeTemp.cy;
	LONG nVertical   = (LONG)strMulti.size();

	// Note : V ��ũ�� ��
	if ( m_bVScroll && m_pVScrollBar && nVertical && lFontHeight )
	{
		// Note : ���� Ŀ���� ��â�� �Ѿ������
		int	nEditVertical = (int)lEditHeight/lFontHeight;

		if ( (nPosY < m_nViewPosY) || (m_nViewPosY+nEditVertical) <= nPosY )
		{
			if ( nEditVertical < nVertical )
			{
				int	  nGap	 = (nVertical - nEditVertical);
				float fRatio = 1.0f/(float)nGap;
				float fPer   = 0.0f;

				// Note : Ŀ���� ��â�� �ö󰬴ٸ�.
				if ( nPosY < m_nViewPosY )
				{
					fPer = (float)(nPosY) * fRatio;
				}
				else
				{
					fPer = (float)((nPosY - nEditVertical)+1) * fRatio;
				}

				if ( 1.0f <= fPer )
				{
					fPer = 1.0f;
				}
				else if ( fPer <= 0.0f )
				{
					fPer = 0.0f;
				}

				CBasicScrollThumbFrame* const pThumbFrame = m_pVScrollBar->GetThumbFrame ();
				pThumbFrame->SetPercent	( fPer );
			}
			else
			{
				CBasicScrollThumbFrame* const pThumbFrame = m_pVScrollBar->GetThumbFrame ();
				pThumbFrame->SetPercent	( 0.0f );
			}
		}
	}
}

void CUIMultiEditBoxSmart::GetLineStringByIndex ( const CString& strIn, CString& strOut, int nIndex )
{
	int nLength = strIn.GetLength();
	int nBegin  = 0;

	if ( nLength < nIndex )
	{
		nIndex = nLength;
	}

	for ( int i=0; i<nIndex; ++i )
	{
		if ( strIn[i] == L'\n' )
		{
			nBegin = i+1;
		}
	}

	if ( nLength - nBegin < 0 )
	{
		strOut = "";
		return;
	}

	strOut = strIn.Mid( nBegin, nLength - nBegin );
}

void CUIMultiEditBoxSmart::GetIndexPosition ( const MULTISTRING& strMulti, const int nIndex, int& nPosX, int& nPosY )
{
	MULTISTRINGCITER iter		= strMulti.begin();
	int				 nCurLength	= 0;
	int				 nCurIndex	= 0;
	int				 nLength	= 0;

	for ( ; iter!=strMulti.end(); ++iter )
	{
		const SMULTISTRINGINFO& sInfo   = (*iter);
		const CString&			strLine = sInfo.Data;

		 nLength = strLine.GetLength();

		if ( nIndex < (nCurLength + nLength) )
		{
			nPosX = (nIndex - nCurLength);
			nPosY = nCurIndex;
			return;
		}

		nCurLength += nLength;		
		++nCurIndex;
	}

	nPosX = nLength;
	nPosY = 0;

	if ( strMulti.size() )
	{
		nPosY = static_cast<int>( strMulti.size() - 1 );
		return;
	}
}

int	CUIMultiEditBoxSmart::SetCarratPosition	( MULTISTRING& strMulti, const int nPosX, const int nPosY )
{
	if ( (int)strMulti.size() <= nPosY )
	{
		m_xInsertPos = 0;

		MULTISTRINGITER iter = strMulti.begin();
		for ( ; iter!=strMulti.end(); ++iter )
		{
			m_xInsertPos += MultiToWidePos( &(*iter).Data, (*iter).Data.GetLength() );
		}

		SetInsertPos ();

		return m_xInsertPos;
	}
	else if ( nPosX < 0 || nPosY < 0 )
	{
		m_xInsertPos = 0;

		SetInsertPos ();

		return m_xInsertPos;
	}

	SMULTISTRINGINFO& sInfo   = strMulti[ nPosY ];
	CString&		  strLine = sInfo.Data;

	int nMByteLength = strLine.GetLength();
	int nWidthIndex  = MultiToWidePos( &strLine, nPosX );
	int nLineIndex   = 0;

	// Note : �������� ���๮���� ��� �� ��������
	if ( nWidthIndex && nMByteLength && nMByteLength <= nPosX )
	{
		if ( strLine[ nMByteLength-1 ] == L'\n' )
		{
			nWidthIndex -= 2; // \r ����

			if ( nWidthIndex < 0 )
			{
				nWidthIndex = 0;
			}
		}
	}

	m_xInsertPos = nWidthIndex;

	MULTISTRINGITER iter = strMulti.begin();
	for ( ; iter!=strMulti.end(); ++iter )
	{
		if ( nPosY <= nLineIndex )
		{
			break;
		}

		m_xInsertPos += MultiToWidePos( &(*iter).Data, (*iter).Data.GetLength() );

		++nLineIndex;
	}

	SetInsertPos ();

	return m_xInsertPos;
}
