#include "pch.h"
#include "BasicTextBox.h"
#include "UIRenderQueue.h"
#include "../DxTools/DxFontMan.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

long CBasicTextBox::GetLongestLine ()
{
	long lLongestLine = 0l; // MEMO

	int nTextBox = (int)m_TextBox.size ();

	for ( int x = 0; x < nTextBox; x++ )
	{
		STEXTPART& sTEXTPART = m_TextBox[x];
		DEQUETEXTLINE& dequeLine = sTEXTPART.dequeLine;

		for ( int i = 0; i < (int) dequeLine.size (); ++i )
			lLongestLine = new_max ( lLongestLine, dequeLine[i].strSize.cx );
	}

	return lLongestLine;
}


long CBasicTextBox::GetLongestPart ()
{
    long lLongestPart = 0l; // MEMO

    int nTextBox = (int)m_TextBox.size ();

    for ( int x = 0; x < nTextBox; x++ )
    {
        STEXTPART& sTEXTPART = m_TextBox[x];
        DEQUETEXTLINE& dequeLine = sTEXTPART.dequeLine;
        long lPart = 0; // MEMO

        for ( int i = 0; i < (int) dequeLine.size (); ++i )
            lPart += dequeLine[i].strSize.cx;

        lLongestPart = new_max( lPart, lLongestPart );
    }

    return lLongestPart;
}

void	CBasicTextBox::SetTextDataEx ( int nIndex, DWORD dwData )
{
	if ( nIndex < 0 || (int) m_TextBox.size () <= nIndex )
	{
		GASSERT ( 0 && "범위를 넘어섭니다. 확인하세요!!!" );
		return ;
	}
	
	m_TextBox[nIndex].m_dwDataEx = dwData;
}

DWORD	CBasicTextBox::GetTextDataEx ( int nIndex )
{
	if ( nIndex < 0 || (int) m_TextBox.size () <= nIndex )
	{
		GASSERT ( 0 && "범위를 넘어섭니다. 확인하세요!!!" );
		return UINT_MAX;
	}
	
	return (DWORD)m_TextBox[nIndex].m_dwDataEx;
}

void CBasicTextBox::SetTextData ( int nIndex, DWORD_PTR dwData )
{
	if ( nIndex < 0 || (int) m_TextBox.size () <= nIndex )
	{
		GASSERT ( 0 && "범위를 넘어섭니다. 확인하세요!!!" );
		return ;
	}
	
	m_TextBox[nIndex].m_dwData = dwData;	
}

DWORD CBasicTextBox::GetTextData ( int nIndex )
{
	if ( nIndex < 0 || (int) m_TextBox.size () <= nIndex )
	{
		return UINT_MAX;
	}
	
	return static_cast<DWORD>(m_TextBox[nIndex].m_dwData);
}

void CBasicTextBox::SetOverColor( int nIndex, D3DCOLOR TextColor )
{
	m_nOverIndex = nIndex;
	m_dwOverColor = TextColor;
}

void	CBasicTextBox::SetTextColor ( int nIndex, D3DCOLOR dwTextColor )
{
	if ( nIndex < 0 || (int) m_TextBox.size () <= nIndex )
	{
		GASSERT ( 0 && "범위를 넘어섭니다. 확인하세요!!!" );
		return;
	}

	STEXTPART& sTEXTPART = m_TextBox[nIndex];
	sTEXTPART.m_dwColor = dwTextColor;
}

D3DCOLOR	CBasicTextBox::GetTextColor ( int nIndex )
{
	if ( nIndex < 0 || (int) m_TextBox.size () <= nIndex )
	{
		GASSERT ( 0 && "범위를 넘어섭니다. 확인하세요!!!" );
		return UINT_MAX;
	}

	const STEXTPART& sTEXTPART = m_TextBox[nIndex];
	return sTEXTPART.m_dwColor;
}

void	CBasicTextBox::SetUseTextColor ( int nIndex, BOOL bUseTextColor )
{
	if ( nIndex < 0 || (int) m_TextBox.size () <= nIndex )
	{
		GASSERT ( 0 && "범위를 넘어섭니다. 확인하세요!!!" );
		return ;
	}

	STEXTPART& sTEXTPART = m_TextBox[nIndex];
	sTEXTPART.m_bUseTextColor = bUseTextColor;
}

BOOL	CBasicTextBox::IsUseTextColor ( int nIndex )
{
	if ( nIndex < 0 || (int) m_TextBox.size () <= nIndex )
	{
		GASSERT ( 0 && "범위를 넘어섭니다. 확인하세요!!!" );
		return FALSE;
	}

	const STEXTPART& sTEXTPART = m_TextBox[nIndex];
	return sTEXTPART.m_bUseTextColor;
}

int		CBasicTextBox::GetLineCount ( int nIndex )
{
	if ( nIndex < 0 || (int) m_TextBox.size () <= nIndex )
	{
		GASSERT ( 0 && "범위를 넘어섭니다. 확인하세요!!!" );
		return 0;
	}

	return (int)m_TextBox[nIndex].dequeLine.size ();
}

//void	CBasicTextBox::PopFrontTextLine ( int nIndex )
//{
//	if ( nIndex < 0 || (int) m_TextBox.size () <= nIndex )
//	{
//		GASSERT ( 0 && "범위를 넘어섭니다. 확인하세요!!!" );
//		return ;
//	}
//
//	m_TextBox[nIndex].dequeLine.pop_front ();
//}

void CBasicTextBox::SetCurLine ( int nCurLine )
{
	if ( nCurLine < nDEFAULT_STARTLINE )	nCurLine = nDEFAULT_STARTLINE;
	
	if ( m_nVisibleLine < m_nTotalLine )
	{
		const int nEndStartIndex = m_nTotalLine - m_nVisibleLine;
		if ( nEndStartIndex < nCurLine )	nCurLine = nEndStartIndex;
	}

	if ( nCurLine == m_nCurLine ) return ;

	m_nCurLine = nCurLine;
	
	ReCalcRenderPos ();
}

CString CBasicTextBox::GetText ( int nIndex )
{
	const int nSize = (int)m_TextBox.size ();

	CString strCombine;

	if ( nSize < 1 )
	{
		GASSERT ( 0 && "텍스트박스가 비어 있습니다." );
		return strCombine;
	}

	if ( nIndex < 0 || nSize <= nIndex )
	{
		GASSERT ( 0 && "잘못된 인덱스입니다." );
		return strCombine;
	}	

	STEXTPART& sTEXTPART = m_TextBox[nIndex];
	DEQUETEXTLINE& dequeTextLine = sTEXTPART.dequeLine;
	const int nLineSize = (int)dequeTextLine.size();
	for ( int i = 0; i < nLineSize; i++ )
	{
		STEXTLINE& sTextLine = dequeTextLine[i];

		VECTORTEXTWORD& vectorWord = sTextLine.vectorWord;
		const int nWordSize = (int)vectorWord.size ();
		for ( int j = 0; j < nWordSize; j++ )
		{
			STEXTWORD& sTextWord = vectorWord[j];
			strCombine += sTextWord.strWord;
		}
	}

	return strCombine;
}


void CBasicTextBox::AlignSubControl ( const UIRECT& rcParentOldPos, const UIRECT& rcParentNewPos )
{
	CUIGroup::AlignSubControl ( rcParentOldPos, rcParentNewPos );

	ReCalcRenderPos ();
}

void  CBasicTextBox::SetGlobalPos ( const D3DXVECTOR2& vPos )
{
	CUIGroup::SetGlobalPos ( vPos );

	ReCalcRenderPos ();
}

void CBasicTextBox::SetGlobalPos ( const UIRECT& rcPos )
{
	CUIGroup::SetGlobalPos ( rcPos );
	
	ReCalcRenderPos ();	
}

void CBasicTextBox::SetFont ( CD3DFontPar* pFont )
{
	if ( !pFont )
	{
		GASSERT ( 0 && "폰트가 널입니다." );
		return ;
	}

	if ( m_pFont )
	{
		GASSERT ( pFont && "폰트를 교체할 경우, 텍스트박스를 벗어나 출력하는 문제가 발생할 수 있습니다." );
		return ;
	}

	if ( pFont == NULL )
	{
		std::string ErrorMsg(
			"[CBasicTextBox::SetFont] : pFont is NULL");
		GASSERT( 0 && ErrorMsg.c_str() );
		sc::writeLogError(ErrorMsg);
		return;
	}

	else if ( pFont->GetEmLang() < language::DEFAULT || pFont->GetEmLang() > language::VIETNAM)
	{
		std::string ErrorMsg(
			"pFont->GetEmLang() < language::DEFAULT || pFont->GetEmLang() > language::VIETNAM");
		GASSERT( 0 && ErrorMsg.c_str() );
		sc::writeLogError(ErrorMsg);
		return;
	}


	m_pFont = pFont;
	m_pFont->GetTextExtent ( " ", BLANKSIZE );
	m_fDefaultFontSize = static_cast<FLOAT>(m_pFont->GetFontHeight());

    m_pFontOriginTemp = m_pEngineDevice->LoadFont( m_pFont->GetFontName(),
        m_pFont->GetFontHeight(),
        m_pFont->GetFontFlags() );
}

void CBasicTextBox::SetTextAlign ( int nAlign )
{
	m_nAlign = nAlign;

	ReCalcRenderPos ();
}

void CBasicTextBox::SetLineInterval ( const float& fLineInterval )
{
	m_fLineInterval = fLineInterval;

	ReCalcRenderPos ();
}

void CBasicTextBox::SetPartInterval ( const float& fPartInterval )
{
	m_fPartInterval = fPartInterval;

	ReCalcRenderPos ();
}

void CBasicTextBox::ClearText ()
{
	m_TextBox.clear ();
	m_nTotalLine = 0;
	m_nCurLine = nDEFAULT_STARTLINE;

//	m_nImageGenID = BASE_IMAGE_CONTROLID;
	for ( int i = 0; i < (int)m_ImageList.size(); ++i )
	{
		DeleteControl ( m_ImageList[i], 0 );
	}
	m_ImageList.clear ();
}

void CBasicTextBox::SetTextImage ( int nIndex, const CString& strKeyword, int nXpos )
{
	if ( nIndex < 0 || (int) m_TextBox.size () <= nIndex )
	{
		GASSERT ( 0 && "범위를 넘어섭니다. 확인하세요!!!" );
		return;
	}

//	m_nImageGenID++;
//	if ( BASE_IMAGE_CONTROLID_END <= m_nImageGenID ) m_nImageGenID = BASE_IMAGE_CONTROLID;

	CUIControl* pImage = new CUIControl(m_pEngineDevice);
	pImage->CreateSub ( this, strKeyword.GetString(), UI_FLAG_DEFAULT);
	pImage->InitDeviceObjects ( m_pd3dDevice );	
	pImage->RestoreDeviceObjects ( m_pd3dDevice );
	RegisterControl ( pImage );

	m_ImageList.push_back ( pImage->GetWndID() );

	pImage->SetLocalPos( D3DXVECTOR2( static_cast<FLOAT>( nXpos ), 0 ) );
	m_nTextureSizeWidth = pImage->GetLocalPos().right - pImage->GetLocalPos().left;//pImage->GetTextureSizeWidth();

	ReCalcRenderPos ();
	pImage->SetAlignFlag(UI_FLAG_CENTER_Y);
}

HRESULT CBasicTextBox::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT( pd3dDevice );

	HRESULT hr;
	hr = CUIGroup::RestoreDeviceObjects( pd3dDevice );
	if ( FAILED ( hr ) ) return hr;

	ReCalcRenderPos ();

	return S_OK;
}

bool CBasicTextBox::GetPartInfo ( const int nIndex, UIRECT& rcPart )
{
	if ( nIndex < 0 || GetCount () <= nIndex ) return false;

	const STEXTPART& sPart = m_TextBox[nIndex];
	rcPart = sPart.m_rcPart;

	return true;
}

const CBasicTextBox::STEXTPART& CBasicTextBox::GetTextPart ( const int& nIndex )
{
	if ( nIndex < 0 || GetCount () <= nIndex )
	{
		static STEXTPART sDUMMY;
		
		// 메모리가 지속적으로 증가해서 한개만 넣도록 함.
		if ( sDUMMY.dequeLine.empty() )
		{
			// crash 방지 dequeLine[0] 이 있다고 가정하고 사용하는 부분이 너무 많다. 일일이 크기 체크추가하지 않고, 여기에서 dummy 추가함 
			STEXTLINE sTextLine;
			sDUMMY.dequeLine.push_back( sTextLine );
		}
		
		return sDUMMY;
	}

	return m_TextBox[nIndex];
}