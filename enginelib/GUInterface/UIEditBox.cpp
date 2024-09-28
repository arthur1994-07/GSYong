#include "pch.h"

#include "../Common/DXInputString.h"
#include "../DxTools/d3dfont.h"
#include "../DxTools/RENDERPARAM.h"

#include "../DxResponseMan.h"
#include "BasicCarrat.h"
#include "UITextUtil.h"
#include "UIRenderQueue.h"
#include "UIEditBox.h"
#include "../SigmaCore/Log/LogMan.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const int	CUIEditBox::nDEFAULT_LIMIT_INPUT = 256;
const float	CUIEditBox::fLINE_END_GAP = 2.0f;
const int	CUIEditBox::nNOT_USE_MONEY_FORMAT = 0;

UIGUID		CUIEditBox::DEBUG_BEGIN_ID = NO_ID;
UIGUID		CUIEditBox::DEBUG_END_ID = NO_ID;

CUIEditBox::CUIEditBox (EngineDeviceMan* pEngineDevice)
	: CUIGroup(pEngineDevice)
    , m_bModeNative ( true )
	, m_bBeginEdit ( false )
	, m_bLimited( false )
	, m_nLimitInput ( nDEFAULT_LIMIT_INPUT )

	, m_pCarrat ( NULL )

	, m_nTabIndex ( 0 )
	, m_bHide ( FALSE )
	, m_bUseNumber ( false )
	, m_bUseDecimalNumber ( false )
	, m_nMoneyUNIT ( nNOT_USE_MONEY_FORMAT )
	, m_nAlign ( EDIT_LEFT_ALIGN )
	, m_dwTextColor ( NS_UITEXTUTIL::ENABLE )
	, m_bVALID_INPUT ( true )
	, m_bWideCarret ( false ) 

	, m_bMinusSign( TRUE )
	, m_bKeyInput( TRUE )

	, m_xViewPos( 0 )
	, m_xInsertPos( 0 )
	, m_xInsertPosR( 0 )
	, m_fCarratPos( 0.0f )
{
	m_strText = _T("");
	m_strRenderText = _T("");
	m_strRenderTextR = _T("");
}

CUIEditBox::~CUIEditBox ()
{
}

void CUIEditBox::CreateCarrat (const char* szCarrat, BOOL bUseRender, DWORD dwDiffuse )
{
	m_pCarrat = new CBasicCarrat(m_pEngineDevice);
	m_pCarrat->CreateSub ( this, szCarrat );
	m_pCarrat->SetUseRender ( bUseRender );
	m_pCarrat->SetVisibleSingle( FALSE );
	RegisterControl ( m_pCarrat );

	m_fCaretSizeX = m_pCarrat->GetLocalPos().sizeX;
	m_dwCarratDiffuse = dwDiffuse;
}

void CUIEditBox::SetFont ( CD3DFontPar* pFont )
{
	if ( !pFont )
	{
		GASSERT ( 0 && "��Ʈ�� ���Դϴ�." );
		return;
	}
	m_pFont = pFont;
	m_fDefaultFontSize = static_cast<FLOAT>(m_pFont->GetFontHeight());
}

void CUIEditBox::SetGlobalPos ( const UIRECT& rcPos )
{
	CUIGroup::SetGlobalPos ( rcPos );

	UpdateRenderText();
	UpdatePosition ();
}

void CUIEditBox::SetGlobalPos ( const D3DXVECTOR2& vPos )
{
	CUIGroup::SetGlobalPos ( vPos );

	UpdateRenderText();
	UpdatePosition ();
}

void CUIEditBox::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return;
	if ( !m_pFont )		 return;
	if ( !m_pCarrat )	 return;

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( IsBegin () )
	{
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
			EndEdit();
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
		int xInsertPos = pInputString->GetInsertPos();

		//CDebugSet::ToView ( 1, 16, "[Origin]%s", strInput );


		// 	�����Է� �̿ܿ� ������� Ű�� ������ ���, ��� ����
		m_bVALID_INPUT = true;
		if( !m_bMinusSign && ( strInput[0] == '-' ) ) m_bVALID_INPUT = false;
		if( DxInputDevice::GetInstance().GetKeyState ( DIK_LCONTROL ) & DXKEY_DOWNED ) m_bVALID_INPUT = false;
		if( DxInputDevice::GetInstance().GetKeyState ( DIK_LMENU ) & DXKEY_DOWNED ) m_bVALID_INPUT = false;
		if( DxInputDevice::GetInstance().GetKeyState ( DIK_RCONTROL ) & DXKEY_DOWNED ) m_bVALID_INPUT = false;
		if( DxInputDevice::GetInstance().GetKeyState ( DIK_RMENU ) & DXKEY_DOWNED ) m_bVALID_INPUT = false;

		if( !m_bVALID_INPUT )
		{
			SET_STRING_BUFFER ( m_strText );
			SetInsertPos();
			return;
		}

		//	�Է� ����â ��ġ ����
		const UIRECT& rcGlobalPos = GetGlobalPos ();
		CRect rtEditBox((int) rcGlobalPos.left, (int)rcGlobalPos.top, (int)rcGlobalPos.right, (int)rcGlobalPos.bottom); 
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

		// �Է� ���� ���ڰ� ��ȿ�ϹǷ� m_xInsertPos �����Ѵ�
		m_xInsertPos = xInsertPos;
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

		//	�Ҽ��� �Է� ���
		if( IsUSE_DECIMALNUMBER() && !strInput.IsEmpty() )
		{
			// ���� ���ڿ� 0�� �����Ѵ�.
			while( TRUE )
			{
				if( strInput.IsEmpty() || (_istdigit( strInput[0] ) ) || strInput[0] == '.' || (m_bMinusSign && strInput[0] == '-')) //&& strInput[0] != '0') )
				{
					if(strInput[0] == '.')
					{
						strInput ="0";
					}					
					break;
				}

				strInput.Delete( 0, 1 );
			}

			// ���� ���ڵ��� �����Ѵ�.
			int strLength = strInput.GetLength();
			int nCountPoint = 0;
			for ( int i = 0; i < strLength; ++i )
			{
				if(strInput[i] == '.')
					nCountPoint++;

				if( ( !_istdigit( strInput[i] ) &&  ( strInput[i] !='.' && strInput[i] !='-') ) || ( nCountPoint > 1  )  )
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
	m_pFont->PushText( (TCHAR*)m_strRenderTextR.GetString() );
}

HRESULT CUIEditBox::Render ( LPDIRECT3DDEVICEQ pd3dDevice )
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
		CString strRenderTextL = m_strRenderTextR.Left( nSel ) ;
		CString strRenderText  = m_strRenderTextR.Mid( nSel, 2 ) ; // ���� ����

		SIZE TextSize;
		m_pFont->GetTextExtent ( strRenderTextL.GetString(), TextSize );
		D3DXVECTOR2 vRenderPos = m_vRenderPos;		
		vRenderPos.x = vRenderPos.x + (float)TextSize.cx;

		hr = m_pFont->DrawText( m_vRenderPos.x, m_vRenderPos.y, m_dwTextColor, m_strRenderTextR.GetString(), 0L, FALSE );	
		m_pFont->DrawText( vRenderPos.x, vRenderPos.y, NS_UITEXTUTIL::BLACK , strRenderText.GetString(), 0L, FALSE );	

		if ( FAILED ( hr ) ) return hr;
		
		return S_OK;
	}

	hr = m_pFont->DrawText( m_vRenderPos.x, m_vRenderPos.y, m_dwTextColor, m_strRenderTextR.GetString(), 0L, FALSE );	

	if ( FAILED ( hr ) ) return hr;

	return S_OK;
}

void CUIEditBox::UpdateRenderText()
{
	m_strRenderTextR = m_strRenderText;

	if( DXInputString::m_bCaratMove == FALSE )
		return;

	const int lEditWidth = (int)( GetGlobalPos().sizeX - fLINE_END_GAP );
	int iLen = m_strRenderTextR.GetLength();

	CString strTemp;
	SIZE sizeTemp;

	// m_strRenserText ( ��Ƽ����Ʈ ) <= ����ȭ => m_xViewPos, m_xInsertPosR ( �����ڵ� )
	int iViewPosM = m_xViewPos;
	int iInsertPosRM = m_xInsertPosR;
	if( m_pFont )
	{
		WCHAR wTemp[4096];
		m_pFont->MultiToWide( m_strRenderTextR.GetString(), iLen, wTemp, 4094 );
		CStringW strTempW = wTemp;

		iViewPosM = WideToMultiPos( &strTempW, m_xViewPos );
		iInsertPosRM = WideToMultiPos( &strTempW, m_xInsertPosR );
	}

	// m_xInsertPosR �� m_xViewPos ���� �տ� �ִٸ� ����
	if( iInsertPosRM < iViewPosM )
	{
		iViewPosM = iInsertPosRM;

		// ����¡
		for( int i=0; i<7; ++i )
		{
			if( iViewPosM <= 0 )
				break;
			if( NS_UITEXTUTIL::IsDBCS( m_strRenderTextR[ iViewPosM - 1 ] ) )
				iViewPosM -= 2;
			else
				iViewPosM -= 1;
		}

		iViewPosM = new_max( iViewPosM, 0 );
	}

	// ��¹��� ( m_xViewPos ���� lEditWidth ��ŭ ) �ȿ� m_xInsertPosR �� ���ٸ� ����
	if( iInsertPosRM - iViewPosM > 0 )
	{
		strTemp = m_strRenderTextR.Mid( iViewPosM, iInsertPosRM - iViewPosM );
		m_pFont->GetTextExtent( strTemp.GetString(), sizeTemp );

		if( sizeTemp.cx > lEditWidth )
		{
			while( sizeTemp.cx > lEditWidth )
			{
				if( iLen < iViewPosM )
				{
					sc::writeLogError(std::string(m_strRenderTextR.GetString()));
					return;
				}

                if( NS_UITEXTUTIL::IsDBCS( m_strRenderTextR[ iViewPosM ] ) )
					iViewPosM += 2;
				else
					iViewPosM += 1;
				strTemp = m_strRenderTextR.Mid( iViewPosM, iInsertPosRM - iViewPosM );
				m_pFont->GetTextExtent( strTemp.GetString(), sizeTemp );
			}

			// ����¡
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

	// ������ ��ġ iViewPosM ( ��Ƽ ����Ʈ ) <= ����ȭ => m_xViewPos ( �����ڵ� )
	m_xViewPos = MultiToWidePos( &m_strRenderTextR, iViewPosM );

	// ĳ�� ��ġ ���
	{
		strTemp = m_strRenderTextR.Mid( iViewPosM, iInsertPosRM - iViewPosM );
		m_pFont->GetTextExtent ( strTemp.GetString(), sizeTemp );
		m_fCarratPos = (float)sizeTemp.cx;
		m_fCarratPosY = (float)sizeTemp.cy;
	}

	// m_xViewPos ���� �����ؼ� ������ ���� ��� �� lEditWidth �ȿ� ���ö����� �ڸ���
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

void CUIEditBox::UpdatePosition ()
{
	if( DXInputString::m_bCaratMove )
	{
		SIZE TextSize;
		std::string strRenderTextR = m_strRenderTextR.GetString();
		if( strRenderTextR == "" || strRenderTextR.empty() )
		{
			m_pFont->GetTextExtent ( "0", TextSize );
			TextSize.cx = 0;
		}
		else
			m_pFont->GetTextExtent ( m_strRenderTextR.GetString(), TextSize );

		//	���� �Ѹ� ��ġ
		UIRECT rcEditBoxPos = GetGlobalPos ();
		{
			const UIRECT rcCarratPos = m_pCarrat->GetLocalPos ();
			m_vRenderPos.x = rcEditBoxPos.left;
			m_vRenderPos.y = rcEditBoxPos.top + rcCarratPos.top;

			switch ( m_nAlign )
			{
			case EDIT_CENTER_ALIGN:
				m_vRenderPos.x = rcEditBoxPos.left + ((rcEditBoxPos.sizeX - (float)TextSize.cx) / 2.0f);
				break;

			case EDIT_RIGHT_ALIGN:
				m_vRenderPos.x = rcEditBoxPos.right - (float)TextSize.cx;
				break;

			case EDIT_CENTER_XY_ALIGN:
				m_vRenderPos.x = rcEditBoxPos.left + ((rcEditBoxPos.sizeX - (float)TextSize.cx) / 2.0f);
				m_vRenderPos.y = rcEditBoxPos.top + ((rcEditBoxPos.sizeY - (float)TextSize.cy) / 2.0f);
				break;
			}
		}

		//	ĳ�� �Ѹ� ��ġ
		UIRECT rcCarratPos = m_pCarrat->GetGlobalPos ();
		{
			rcCarratPos.left = m_vRenderPos.x + m_fCarratPos;
			rcCarratPos.top = m_vRenderPos.y;
			m_pCarrat->SetGlobalPos ( rcCarratPos );
		}
	}
	else
	{
		const long lEditWidth = (long)(GetGlobalPos().sizeX - fLINE_END_GAP);

		//	���ڿ��� ����Ʈ�ڽ��� �Ѿ ��� �պκ� �߶󳻱�
		SIZE TextSize;
		std::string strRenderTextR = m_strRenderTextR.GetString();
		if( strRenderTextR == "" || strRenderTextR.empty() )
		{
			m_pFont->GetTextExtent ( "0", TextSize );
			TextSize.cx = 0;
		}
		else
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

		//	���� �Ѹ� ��ġ
		UIRECT rcEditBoxPos = GetGlobalPos ();
		{
			const UIRECT rcCarratPos = m_pCarrat->GetLocalPos ();
			m_vRenderPos.x = rcEditBoxPos.left;
			m_vRenderPos.y = rcEditBoxPos.top + rcCarratPos.top;

			switch ( m_nAlign )
			{
			case EDIT_CENTER_ALIGN:
				m_vRenderPos.x = rcEditBoxPos.left + ((rcEditBoxPos.sizeX - (float)TextSize.cx) / 2.0f);
				break;

			case EDIT_RIGHT_ALIGN:
				m_vRenderPos.x = rcEditBoxPos.right - (float)TextSize.cx;
				break;

			case EDIT_CENTER_XY_ALIGN:
				m_vRenderPos.x = rcEditBoxPos.left + ((rcEditBoxPos.sizeX - (float)TextSize.cx) / 2.0f);
				m_vRenderPos.y = rcEditBoxPos.top + ((rcEditBoxPos.sizeY - (float)TextSize.cy) / 2.0f);
				break;
			}
		}

		//	ĳ�� �Ѹ� ��ġ
		UIRECT rcCarratPos = m_pCarrat->GetGlobalPos ();
		{
			rcCarratPos.left = m_vRenderPos.x + (float)TextSize.cx;
			rcCarratPos.top = m_vRenderPos.y;
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
			if( !DXInputString::m_bCaratMove ) rcCaretNewPos.left -= 12;
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

void CUIEditBox::GetInsertPos()
{
    //DXInputString* pInputString = DxResponseMan::GetInstance().GetInputString();
    DXInputString* pInputString = m_pEngineDevice->GetInputString();
	m_xInsertPos = pInputString->GetInsertPos();		
	m_xInsertPosR = m_xInsertPos;
}

void CUIEditBox::SetInsertPos()
{
    //DXInputString* pInputString = DxResponseMan::GetInstance().GetInputString();
    DXInputString* pInputString = m_pEngineDevice->GetInputString();
	m_xInsertPos = pInputString->SetInsertPos(m_xInsertPos);
	m_xInsertPosR = m_xInsertPos;
}

int CUIEditBox::MultiToWidePos( CString* strMulti, int iMultiPos )
{
	if( m_pFont )
	{
		CString strTemp = strMulti->Left( iMultiPos );
		WCHAR wTemp[ 512 ]={0};
		return m_pFont->MultiToWide( strTemp.GetString(), strTemp.GetLength(), wTemp, 510 );
	}
	return 0;
}

int CUIEditBox::WideToMultiPos( CStringW* strWide, int iWidePos )
{
	if( m_pFont )
	{
		CStringW strTemp = strWide->Left( iWidePos );
		TCHAR cTemp[ 4096 ]={0};
		return m_pFont->WideToMulti( strTemp.GetString(), strTemp.GetLength(), cTemp, 4094 );
	}
	return 0;
}

void CUIEditBox::FLUSH_STRING_BUFFER ()
{
    //DXInputString* pInputString = DxResponseMan::GetInstance().GetInputString();
    DXInputString* pInputString = m_pEngineDevice->GetInputString();
	pInputString->SetString();
}

void CUIEditBox::SET_STRING_BUFFER ( const char* pStr )
{	
    FLUSH_STRING_BUFFER();	
    //DXInputString* pInputString = DxResponseMan::GetInstance().GetInputString();
    DXInputString* pInputString = m_pEngineDevice->GetInputString();
	pInputString->SetString(pStr);
}

void CUIEditBox::INSERT_STRING_BUFFER ( const char* pStr )
{
    //DXInputString* pInputString = DxResponseMan::GetInstance().GetInputString();
    DXInputString* pInputString = m_pEngineDevice->GetInputString();
	pInputString->InsertString(pStr);
}

bool CUIEditBox::BeginEdit()
{
	if (m_bBeginEdit)
        return true;

    //DXInputString* pInputString = DxResponseMan::GetInstance().GetInputString();
    DXInputString* pInputString = m_pEngineDevice->GetInputString();
	if (pInputString->IsOn())
        return false;

	//	����, Native ����� ��� 'Native'�� ��ȯ
	if (IsMODE_NATIVE())
	{
		pInputString->ConverseEngToNative();
	}

	m_bBeginEdit = true;

	pInputString->OnInput();
	SET_STRING_BUFFER(m_strText);
	SetInsertPos();

	m_pCarrat->SetVisibleSingle(TRUE);
	if (m_pCarrat->IsUseRender())
	{
		m_pCarrat->SetDiffuse(m_dwCarratDiffuse);
	}

	return true;
}

bool CUIEditBox::EndEdit ()
{	
	if (!m_bBeginEdit)
        return false;

	m_pCarrat->SetVisibleSingle(FALSE);

    //DXInputString* pInputString = DxResponseMan::GetInstance().GetInputString();
    DXInputString* pInputString = m_pEngineDevice->GetInputString();
	// ����, Native ����ϰ�� �������� '������' ��ȯ
	if (IsMODE_NATIVE())
	{ 
		pInputString->ConverseNativeToEng ();
	}

	m_bBeginEdit = false;
	pInputString->OffInput();
	FLUSH_STRING_BUFFER();

	return true;
}

void CUIEditBox::ClearEdit ()
{
	FLUSH_STRING_BUFFER ();
	m_strText.Empty ();
	m_strRenderText.Empty ();

	m_xViewPos = 0;
	m_xInsertPos = 0;
	m_xInsertPosR = 0;
}

void CUIEditBox::SetEditString ( const CString& strInput )
{
	if( m_strText == strInput.Left( m_nLimitInput ) ) return;
	m_strText = strInput.Left( m_nLimitInput );
	SetRenderText ( m_strText );

	m_xViewPos = 0;

	// ���� ����Ʈ ���� ��Ʈ�Ѹ� ��� ������ ġȯ�Ѵ�.
	// �׷��� ���� ���� ���, ���� ��ȯ�� �Ͼ�� �� �ϴ� ���� ����.
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

void CUIEditBox::InsertString(const CString& strInput)
{
	if (m_strText.GetLength() + strInput.GetLength() > m_nLimitInput)
        return;
	if (!IsBegin())
        return;
	
	INSERT_STRING_BUFFER( strInput );
	GetInsertPos();

    //DXInputString* pInputString = DxResponseMan::GetInstance().GetInputString();
    DXInputString* pInputString = m_pEngineDevice->GetInputString();
	m_strText = pInputString->GetString();
	SetRenderText ( m_strText );

	UpdateRenderText();
	UpdatePosition();

	// �ؽ��Ŀ� ���ڸ� �������ϴ� ������� ����
	m_pFont->PushText( (TCHAR*)m_strRenderTextR.GetString() );
}

bool CUIEditBox::SET_MONEY_FORMAT ( int nMoneyUNIT, const CString& strUNIT_SYMBOL )
{	
	if ( nMoneyUNIT < 0 ) return false;
	if ( 0 < nMoneyUNIT && strUNIT_SYMBOL.IsEmpty() ) return false;

	m_nMoneyUNIT = nMoneyUNIT;
	m_strUNIT_SYMBOL = strUNIT_SYMBOL;

	//	�� ����� ������, ������ ���� �Է����� �ٲ۴�.
	DoUSE_NUMBER( nMoneyUNIT ? true : false );

	return true;
}

void CUIEditBox::DoMODE_TOGGLE()
{
	SET_STRING_BUFFER(m_strText);
	SetInsertPos();

    //DXInputString* pInputString = DxResponseMan::GetInstance().GetInputString();
    DXInputString* pInputString = m_pEngineDevice->GetInputString();
	if (IsMODE_NATIVE())
	{
		pInputString->ConverseNativeToEng();
		SetMODE_NATIVE(false);
	}
	else
	{
		pInputString->ConverseEngToNative();
		SetMODE_NATIVE(true);
	}
}

const UIRECT& CUIEditBox::GetCarratGlobalPos()
{
	return m_pCarrat->GetGlobalPos();
}

const UIRECT& CUIEditBox::GetCarratLocalPos()
{
	return m_pCarrat->GetLocalPos();
}

void CUIEditBox::SetDiffuseAlpha ( WORD wAlpha )
{
    CUIGroup::SetDiffuseAlpha( wAlpha );

    DWORD dwColor = GetTextColor();
    SetTextColor ( D3DCOLOR_ARGB ( wAlpha, (dwColor>>16)&0xFF, (dwColor>>8)&0xFF, (dwColor)&0xFF ) );
}
