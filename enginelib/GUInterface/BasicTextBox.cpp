#include "pch.h"
#include "BasicTextBox.h"
#include "UIRenderQueue.h"
#include "../DxTools/DxFontMan.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringFormat.h"

#include "../DxTools/DebugSet.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const	float	CBasicTextBox::fDEFAULT_LINE_INTERVAL = 2.0f;
const	float	CBasicTextBox::fDEFAULT_PART_INTERVAL = 2.0f;
const	int		CBasicTextBox::nDEFAULT_STARTLINE = 0;
const	int		CBasicTextBox::nDEFAULT_VISIBLE_LINE = 1;
const	int		CBasicTextBox::nDEFAULT_LINE_LIMIT = 100;

CBasicTextBox::CBasicTextBox (EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_nRenderdLine( 0 )
    , m_nRenderPartIndex( 0 )
    , m_nRenderLineIndex( 0 )
    , m_bRenderTextColor( FALSE )
    , m_dwRenderTextColor( 0xffffffff )

    , m_fLineInterval( fDEFAULT_LINE_INTERVAL )
    , m_fPartInterval( fDEFAULT_PART_INTERVAL )

    , m_nAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP )

    , m_nVisibleLine( nDEFAULT_VISIBLE_LINE )

    , m_nCurLine( nDEFAULT_STARTLINE )
    , m_nPartIndex( 0 )
    , m_nLineIndex( 0 )

    , m_nLimitLine( nDEFAULT_LINE_LIMIT )
    , m_nTotalLine( 0 )

//    , m_nImageGenID( BASE_IMAGE_CONTROLID )

    , m_pd3dDevice( NULL )

    , m_dwOverColor( 0xffffffff )
    , m_nOverIndex( 0 )
    , m_bUseOverColor( FALSE )
    , m_bAutoChangeFontSize(false)
    , m_pFontOriginTemp(NULL)
{
}

CBasicTextBox::~CBasicTextBox ()
{
}

HRESULT CBasicTextBox::RenderWord ( STEXTWORD& sTextWord )
{
	if (!m_pFont)
	{
		GASSERT ( 0 && "��Ʈ�� ���Դϴ�. SetFont()�� �̿��Ͽ� ��Ʈ���� ���� �����ʽÿ�." );
		return E_FAIL;
	}

	const float fPosX = sTextWord.x;
	const float fPosY = sTextWord.y;
	CString& strWord = sTextWord.strWord;
	D3DCOLOR dwColor = sTextWord.dwColor;

	if ( m_bRenderTextColor )
		dwColor = m_dwRenderTextColor;

	bool bOverIndex(m_nRenderPartIndex==m_nOverIndex);	

	if( bOverIndex && m_bUseOverColor )
		dwColor = m_dwOverColor;	

	//	���̵� ��/�ƿ�
	D3DXCOLOR TempColor( dwColor );
	TempColor.a *= GetVisibleRate ();

	// �ؽ��Ŀ� ���ڸ� �������ϴ� ������� ����
	return m_pFont->DrawText( fPosX, fPosY, TempColor, (TCHAR*)strWord.GetString(), 0L, FALSE );
}

HRESULT CBasicTextBox::RenderLine ( VECTORTEXTWORD& vectorTextWord )
{
	HRESULT hr;

	const int nTextWord = (int)vectorTextWord.size ();

	for ( int i = 0; i < nTextWord; ++i )
	{
		hr = RenderWord ( vectorTextWord[i] );
		if ( FAILED ( hr ) ) return hr;
	}

	return S_OK;
}

HRESULT CBasicTextBox::RenderPart ( DEQUETEXTLINE& dequeTextLine )
{
	HRESULT hr = S_OK;

	const int nTextLine = (int)dequeTextLine.size ();

	for ( ; m_nRenderLineIndex < nTextLine; ++m_nRenderLineIndex )
	{
		hr = RenderLine ( dequeTextLine[m_nRenderLineIndex].vectorWord );
		if ( FAILED ( hr ) ) return hr;

		++m_nRenderdLine;

		if ( m_nVisibleLine == m_nRenderdLine ) break;
	}

	return S_OK;
}

HRESULT CBasicTextBox::RenderText()
{
	if (!m_pFont)
	{
		GASSERT ( 0 && "��Ʈ�� ���Դϴ�. SetFont()�� �̿��Ͽ� ��Ʈ���� ���� �����ʽÿ�." );
		return E_FAIL;
	}

	HRESULT hr;

	const int nTextSize = (int) m_TextBox.size();
	if (nTextSize < 1)
        return S_FALSE;

    m_nRenderdLine = 0;
	m_nRenderPartIndex = m_nPartIndex;
	m_nRenderLineIndex = m_nLineIndex;

	for ( ; m_nRenderPartIndex < nTextSize; ++m_nRenderPartIndex)
	{
		m_bRenderTextColor = m_TextBox[m_nRenderPartIndex].m_bUseTextColor;
		m_dwRenderTextColor = m_TextBox[m_nRenderPartIndex].m_dwColor;

		hr = RenderPart(m_TextBox[m_nRenderPartIndex].dequeLine);
		if (FAILED(hr))
            return hr;

		m_nRenderLineIndex = 0;
        
		if (m_nVisibleLine <= m_nRenderdLine)
            break;
	}

	return S_OK;
}

HRESULT CBasicTextBox::Render ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;

	if ( !IsVisible() ) return S_OK;

	hr = CUIGroup::Render( pd3dDevice );
	if ( FAILED ( hr ) ) return hr;

	CUIRenderQueue::Get()->RenderImmediately();
	


	hr = RenderText();

	if ( FAILED ( hr ) ) return hr;

	return S_OK;
}

int CBasicTextBox::AddOneLineText( const CString& strText, bool bUseEllipsis /* = FALSE */, const D3DCOLOR& TextColor /* = NS_UITEXTUTIL::ENABLE */ )
{
	if ( !m_pFont )
	{
		GASSERT ( 0 && "��Ʈ�� ���Դϴ�. SetFont()�� �̿��Ͽ� ��Ʈ���� ���� �����ʽÿ�." );
		return -1;
	}

	if ( strText.IsEmpty() )
	{
		GASSERT ( 0 && "CBasicTextBox::AddOneLineText �� ���ڰ� strText �� ���� �����ϴ�." );
		return -1;
	}
	
    float fWidth = GetLocalPos().sizeX;

	if( bUseEllipsis ) 
		fWidth -= 20.0f;

	//	���ڿ� �ڸ���
	NS_UITEXTUTIL::ClearCRLFText();
	NS_UITEXTUTIL::MULTILINETEXT& MLText = NS_UITEXTUTIL::GetMLTextWithoutCRLF( strText, fWidth, m_pFont );
	if ( MLText.empty() )
	{
		GASSERT ( 0 && "NS_UITEXTUTIL::GetMLTextWithoutCRLF �� Zero ���� �Ѿ�Խ��ϴ�." );
		return -1;
	}

	int nSplitLine = (int)MLText.size();

    if ( m_bAutoChangeFontSize )
        AutoChangeFontSize( strText, MLText, nSplitLine, false );

	STEXTPART sTextPart;

	//	����Ÿ�� ����ֱ�
	STEXTLINE sTextLine;

	SIZE strSize;

	if( nSplitLine >= 2 && bUseEllipsis ) 
		MLText[0].strLine += "...";

	m_pFont->GetTextExtent ( MLText[0].strLine.GetString(), strSize );

	sTextLine.strSize = strSize;
	sTextLine.bLASTLINE = MLText[0].bLASTLINE;
	if ( m_nAlign == TEXT_ALIGN_BOTH_X )
	{				
		NS_UITEXTUTIL::ClearWordText ();
		NS_UITEXTUTIL::STRINGVECTOR& WordText = NS_UITEXTUTIL::GetWordText ( MLText[0].strLine );

		for ( int k = 0; k < (int)WordText.size(); ++k )
		{
			STEXTWORD sTextWord;

			sTextWord.strWord = WordText[k];
			sTextWord.dwColor = TextColor;

			if ( WordText[k].GetLength () )
			{
				m_pFont->GetTextExtent ( WordText[k].GetString(), strSize );
				m_pFont->PushText( WordText[k].GetString() );
			}
			else
			{
				strSize.cx = 0;
				strSize.cy = BLANKSIZE.cy;
			}

			sTextWord.strSize = strSize;

			sTextLine.vectorWord.push_back ( sTextWord );
		}

		m_nTotalLine++;
		sTextPart.dequeLine.push_back ( sTextLine );
	}
	else
	{
		STEXTWORD sTextWord;
		sTextWord.strWord = MLText[0].strLine;
		sTextWord.dwColor = TextColor;

		sTextWord.strSize = strSize;

		if( MLText[0].strLine.GetLength() )
			m_pFont->PushText( MLText[0].strLine.GetString() );

		sTextLine.vectorWord.push_back ( sTextWord );
		sTextPart.dequeLine.push_back ( sTextLine );

		m_nTotalLine++;
	}	

	m_TextBox.push_back ( sTextPart );	

	ReCalcRenderPos ();

	return (int)m_TextBox.size () - 1;
}

int	CBasicTextBox::AddText( const CString& strText, const D3DCOLOR& TextColor, bool bCutWord )
{
	if ( !m_pFont )
	{
		GASSERT( 0 && "��Ʈ�� ���Դϴ�. SetFont()�� �̿��Ͽ� ��Ʈ���� ���� �����ʽÿ�." );
		return -1;
	}
    if ( !m_pFontOriginTemp )
    {
        GASSERT( 0 && "m_pFontOriginTemp ��Ʈ�� ���Դϴ�. SetFont()�� �̿��Ͽ� ��Ʈ���� ���� �����ʽÿ�." );
        return -1;
    }


	//	���ڿ� �ڸ���
	NS_UITEXTUTIL::ClearCRLFText();

	if( bCutWord )
	{
		NS_UITEXTUTIL::MULTILINETEXT& MLText =
			NS_UITEXTUTIL::GetMLTextWithoutCRLF(
			strText, GetLocalPos().sizeX, m_pFont );
		int nSplitLine = static_cast<int>(MLText.size());

		if ( m_bAutoChangeFontSize )
			AutoChangeFontSize( strText, MLText, nSplitLine, false );
	}
	else
	{
		NS_UITEXTUTIL::MULTILINETEXT& MLText =
			NS_UITEXTUTIL::GetMLTextWithoutCRLFAndCutWord(
			strText, GetLocalPos().sizeX, m_pFont );
		int nSplitLine = static_cast<int>(MLText.size());

		if ( m_bAutoChangeFontSize )
			AutoChangeFontSize( strText, MLText, nSplitLine, false );
	}

    PushText( strText, TextColor, bCutWord );

	return (int)m_TextBox.size () - 1;
}

int CBasicTextBox::AddTextWithCRLF( const CString& strText, const D3DCOLOR& TextColor )
{
	if ( !m_pFont )
	{
		GASSERT ( 0 && "��Ʈ�� ���Դϴ�. SetFont()�� �̿��Ͽ� ��Ʈ���� ���� �����ʽÿ�." );
		return -1;
	}

	//	���ڿ� �ڸ���
	NS_UITEXTUTIL::ClearCRLFText();
	NS_UITEXTUTIL::MULTILINETEXT& MLText = NS_UITEXTUTIL::GetMLTextWithCRLF( strText, GetLocalPos().sizeX, m_pFont );

	int nSplitLine = (int)MLText.size();

    if ( m_bAutoChangeFontSize )
        AutoChangeFontSize( strText, MLText, nSplitLine, true );

	STEXTPART sTextPart;

	//	����Ÿ�� ����ֱ�
	for ( int i = 0; i < nSplitLine; ++i )
	{		
		STEXTLINE sTextLine;

		if ( (i - 1 == nSplitLine) && !MLText[i].strLine.GetLength () )
		{
			//	������������ �� ���.
		}
		else
		{
			SIZE strSize;
			m_pFont->GetTextExtent ( MLText[i].strLine.GetString(), strSize );

			sTextLine.strSize = strSize;
			sTextLine.bLASTLINE = MLText[i].bLASTLINE;
			if ( m_nAlign == TEXT_ALIGN_BOTH_X )
			{				
				NS_UITEXTUTIL::ClearWordText ();
				NS_UITEXTUTIL::STRINGVECTOR& WordText = NS_UITEXTUTIL::GetWordText ( MLText[i].strLine );

				for ( int k = 0; k < (int)WordText.size(); ++k )
				{
					STEXTWORD sTextWord;

					sTextWord.strWord = WordText[k];
					sTextWord.dwColor = TextColor;

					if ( WordText[k].GetLength () )
					{
						m_pFont->GetTextExtent ( WordText[k].GetString(), strSize );
						m_pFont->PushText( WordText[k].GetString() );
					}
					else
					{
						strSize.cx = 0;
						strSize.cy = BLANKSIZE.cy;
					}

					sTextWord.strSize = strSize;

					sTextLine.vectorWord.push_back ( sTextWord );
				}

				m_nTotalLine++;
				sTextPart.dequeLine.push_back ( sTextLine );
			}
			else
			{
				STEXTWORD sTextWord;
				sTextWord.strWord = MLText[i].strLine;
				sTextWord.dwColor = TextColor;

				sTextWord.strSize = strSize;

				if( MLText[i].strLine.GetLength() )
					m_pFont->PushText( MLText[i].strLine.GetString() );

				sTextLine.vectorWord.push_back ( sTextWord );
				sTextPart.dequeLine.push_back ( sTextLine );

				m_nTotalLine++;
			}
		}
	}	

	m_TextBox.push_back ( sTextPart );

	//	���� ���� ������ ���� ���, �߶�
	ReCalcEndLind();

	ReCalcRenderPos ();

	return (int)m_TextBox.size () - 1;
}

void CBasicTextBox::ReCalcEndLind()
{
	for ( ; m_nLimitLine < m_nTotalLine; --m_nTotalLine )
	{
		// �̰����� ���ø��� ������ üũ�� �ص���.
		// ���� ���������̴�.
		if ( m_TextBox.empty() )
		{
			CDebugSet::ToLogFile( std::string( _T("m_TextBox.empty(), m_nTotalLine") ) );
			GASSERT( 0 && _T("m_TextBox.empty(), m_nTotalLine") );
			m_nTotalLine = 0;
			return;
		}

		STEXTPART& sDelTextPart = m_TextBox.front ();
		DEQUETEXTLINE& dequeLine = sDelTextPart.dequeLine;

		dequeLine.pop_front ();

		//	����, ���ܿ� ���� ���� ���� ���,
		//	������ ������.
		if ( dequeLine.size () < 1 )
		{
			if ( m_ImageList.size () )
			{
				DeleteControl ( m_ImageList[0], 0 );
				m_ImageList.pop_front ();
			}
			m_TextBox.pop_front ();
		}
	}
}

void CBasicTextBox::EraseTextBoxIndex( int nIndex )
{
	int nCount(0);
	STEXTBOX_ITER iter = m_TextBox.begin();
	for ( ; iter!=m_TextBox.end(); ++iter, ++nCount )
	{
		if ( nCount == nIndex )
		{
			m_TextBox.erase( iter );
			return;
		}
	}
}

int	CBasicTextBox::AddString( int nIndex, const CString& strText, const D3DCOLOR& TextColor )
{
	if ( !m_pFont )
	{
		GASSERT( 0 && "��Ʈ�� ���Դϴ�. SetFont()�� �̿��Ͽ� ��Ʈ���� ���� �����ʽÿ�." );
		return -1;
	}

	if ( nIndex < 0 || (int) m_TextBox.size() <= nIndex )
	{
		GASSERT( 0 && "�ε����� ������ �Ѿ�ϴ�." );
		return -1;
	}

	if ( !strText.GetLength() )
		return -1;

    if ( m_bAutoChangeFontSize )
    {
        CString strTotal = "";
        CString strOrigin = "";
        for ( size_t i = 0; i < m_TextBox.size(); i++ )
        {
            strTotal.Append( GetText(i) );
            strOrigin.Append( GetText(i) );
        }
        strTotal.Append( strText );

        NS_UITEXTUTIL::MULTILINETEXT MLText;
        int nSplitLine;

        AutoChangeFontSize( strTotal, MLText, nSplitLine, false );
        ClearText();
        PushText( strOrigin, TextColor );
    }

	CString strTextCopy = strText;

	STEXTPART& sTextPart = m_TextBox[nIndex];	

	{
		if ( sTextPart.dequeLine.empty() )
		{
			//if ( m_ImageList.size() < m_TextBox.size() )
			//{
			//	m_TextBox.pop_back();
			//}

			CUIControl* pControl = GetParent();
			if ( pControl )
			{
				std::string strParent = pControl->GetControlNameEx();
				std::string ErrMsg = sc::string::format("Parent Control Name = '%s'�� Index - %d, Tex BoxSize - %d, '%s'�� 'AddString'�� �� �����ϴ�.", strParent.c_str(), nIndex, m_TextBox.size(), strText.GetString());

				CDebugSet::ToLogFile( ErrMsg );
				GASSERT ( 0 && _T("AddString'�� �� �����ϴ�. �α������� Ȯ���� �ּ���.") );

				EraseTextBoxIndex( nIndex );

				return -1;
			}
			else
			{
				std::string ErrMsg = sc::string::format("Index - %d, Tex BoxSize - %d, '%s'�� 'AddString'�� �� �����ϴ�.", nIndex, m_TextBox.size(), strText.GetString());

				CDebugSet::ToLogFile( ErrMsg );
				GASSERT ( 0 && _T("AddString'�� �� �����ϴ�. �α������� Ȯ���� �ּ���.") );

				EraseTextBoxIndex( nIndex );

				return -1;
			}
		}

		STEXTLINE& sTextLine = sTextPart.dequeLine.back ();

		//	���� ���ڿ� ������ ���ο� �� ���̱�		
		float fOldSize = (float) sTextLine.strSize.cx;

		//	������ ���ڿ��� ���� ��ŭ�� �ڸ���
		NS_UITEXTUTIL::ClearCRLFText ();
		NS_UITEXTUTIL::MULTILINETEXT& MLText =
			NS_UITEXTUTIL::GetMLTextWithoutCRLF( strTextCopy, GetLocalPos().sizeX - fOldSize, m_pFont );

		if ( MLText.size () )
		{
			STEXTWORD sTextWord;
			sTextWord.strWord = MLText[0].strLine;
			sTextWord.dwColor = TextColor;
			m_pFont->GetTextExtent ( sTextWord.strWord.GetString(), sTextWord.strSize );

			if( MLText[0].strLine.GetLength() )
				m_pFont->PushText( MLText[0].strLine.GetString() );

			sTextLine.vectorWord.push_back ( sTextWord );
			sTextLine.strSize.cx += sTextWord.strSize.cx;
			sTextLine.strSize.cy = sTextWord.strSize.cy;

			int nCutPos = strTextCopy.GetLength () - sTextWord.strWord.GetLength ();
			strTextCopy = strTextCopy.Right ( nCutPos );
		}		
	}

	if ( strTextCopy.GetLength () )
	{
		//	������ ���ڿ��� ���� ��ŭ�� �ڸ���
		NS_UITEXTUTIL::ClearCRLFText ();
		NS_UITEXTUTIL::MULTILINETEXT& MLText =
			NS_UITEXTUTIL::GetMLTextWithoutCRLF ( strTextCopy, GetLocalPos().sizeX, m_pFont );

		int nSplitLine = (int)MLText.size ();
		
		//	����Ÿ�� ����ֱ�
		for ( int i = 0; i < nSplitLine; ++i )
		{		
			STEXTLINE sTextLine;
			STEXTWORD sTextWord;

			sTextWord.strWord = MLText[i].strLine;
			sTextWord.dwColor = TextColor;
			
			if ( (i - 1 == nSplitLine) && !sTextWord.strWord.GetLength () )
			{
				//	������������ �� ���.
			}
			else
			{
				m_pFont->GetTextExtent ( sTextWord.strWord.GetString(), sTextWord.strSize );

				if( MLText[i].strLine.GetLength() )
					m_pFont->PushText( MLText[i].strLine.GetString() );

				//	������ word�� �� �ǹ̸� ������ ���Ѵ�.
				//	���� �׻� 1���� �����Ѵٰ� �����ϰ�, 0�� �ε����� ����Ѵٰ�
				//	����Ѵ�.
				sTextLine.vectorWord.push_back ( sTextWord );

				sTextLine.strSize.cx = sTextWord.strSize.cx;
				sTextLine.strSize.cy = sTextWord.strSize.cy;
				sTextPart.dequeLine.push_back ( sTextLine );

				m_nTotalLine++;

				//	���� ������ ���ǰ�, �ɸ��� ���
	//			if ( m_nLimitLine && m_nLimitLine == i )	break;
			}
		}	
	}

//	m_TextBox.push_back ( sTextString );

	//	���� ���� ������ ���� ���, �߶�
	ReCalcEndLind();

	ReCalcRenderPos ();

	return (int) m_TextBox.size () - 1;
}

void CBasicTextBox::ReplaceWord(int nIndex, const CString& strTypeField, const CString& strWord, const D3DCOLOR& TextColor)
{
	if ( nIndex < 0 )
	{
		return;
	}

	if (m_TextBox.size() <= static_cast<size_t>(nIndex))
	{
		return;
	}

	int nTypeField = strTypeField.GetLength();

	for (DEQUETEXTLINE_ITER it = m_TextBox[nIndex].dequeLine.begin(); it != m_TextBox[nIndex].dequeLine.end(); ++it)
	{
		for (VECTORTEXTWORD::iterator wit = it->vectorWord.begin(); wit != it->vectorWord.end(); ++wit)
		{
			// Ÿ�� �ʵ带 ã�´�.
			int n = wit->strWord.Find(strTypeField);

			if (n != -1)
			{
				CString cTemp = wit->strWord;
				DWORD dwColor = wit->dwColor;
				int nString = wit->strWord.GetLength();

				// ���� ���ڿ� ����
				wit = it->vectorWord.erase(wit);

				// ���� ���ڿ� �߰�
				if (n + nTypeField < nString)
				{
					wit = it->vectorWord.insert(wit, MakeTextWord(cTemp.Mid(n + nTypeField, nString), dwColor));
				}

				// �� ���ڿ� �߰�
				wit = it->vectorWord.insert(wit, MakeTextWord(strWord, TextColor));

				// ���� ���ڿ� �߰�
				if (n > 0)
				{
					wit = it->vectorWord.insert(wit, MakeTextWord(cTemp.Mid(0, n), dwColor));
				}
			}
		}
	}
}

void CBasicTextBox::ReplaceText(int nIndex, const CString& strTypeField, const CString& strText, const D3DCOLOR& TextColor)
{
	if( !m_pFont )
		return;

	if ( nIndex < 0 )
	{
		return;
	}

	if (m_TextBox.size() <= static_cast<size_t>(nIndex))
	{
		return;
	}

	for (DEQUETEXTLINE_ITER it = m_TextBox[nIndex].dequeLine.begin(); it != m_TextBox[nIndex].dequeLine.end(); ++it)
	{
		for (VECTORTEXTWORD::iterator wit = it->vectorWord.begin(); wit != it->vectorWord.end(); ++wit)
		{
			// Ÿ�� �ʵ带 ã�´�.
			int n = wit->strWord.Find(strTypeField);

			if (n != -1)
			{
				it = m_TextBox[nIndex].dequeLine.erase(it);
				--m_nTotalLine;

				NS_UITEXTUTIL::ClearCRLFText();
				NS_UITEXTUTIL::MULTILINETEXT& MLText = NS_UITEXTUTIL::GetMLTextWithoutCRLF( strText, GetLocalPos().sizeX, m_pFont );

				for (NS_UITEXTUTIL::MULTILINETEXT::reverse_iterator mit = MLText.rbegin(); mit != MLText.rend(); ++mit)
				{
					it = m_TextBox[nIndex].dequeLine.insert(it, MakeTextLine(mit->strLine, TextColor, mit->bLASTLINE));
					++m_nTotalLine;
				}
				
				break;
			}
		}
	}

	// m_TextBox[nIndex].dequeLine.empty ��� m_TextBox[nIndex] ������ ��������Ѵ�.
	if ( m_TextBox[nIndex].dequeLine.empty() )
	{
		EraseTextBoxIndex( nIndex );
	}

	ReCalcRenderPos();
}

void CBasicTextBox::SetTextToLimit( const CString& strText, bool bUseEllipsis, const D3DCOLOR& TextColor )
{
	if( !m_pFont )
		return;

	CString strLimitText = strText;

	SIZE strSize;
	m_pFont->GetTextExtent( strText, strSize );

	float fTotalSize = ( float )strSize.cx + m_nTextureSizeWidth;

	if( m_rcLocalPos.sizeX < fTotalSize )
	{
		if( bUseEllipsis )
		{
			m_pFont->GetTextExtent( "...", strSize );
			int nLenth = m_pFont->GetTextCountByWidth( strText, m_rcLocalPos.sizeX - strSize.cx );

			strLimitText = strText.Left( nLenth );
			strLimitText += "...";
		}
		else
		{
			int nLenth = m_pFont->GetTextCountByWidth( strText, m_rcLocalPos.sizeX );

			strLimitText = strText.Left( nLenth );
		}
	}

	SetTextNoSplit( strLimitText, TextColor );
}

int	CBasicTextBox::AddStringToLimit( int nIndex, const CString& strText, const D3DCOLOR& TextColor )
{
	if ( !m_pFont )
		return -1;

	if ( nIndex < 0 )
	{
		return -1;
	}

	if ( (int) m_TextBox.size() <= nIndex )
		return -1;

	CString strLimitText = strText;

	SIZE strSize;
	m_pFont->GetTextExtent( strText, strSize );

	float fOldSize = ( float )m_TextBox[nIndex].dequeLine.back().strSize.cx;
	float fTotalSize = ( float )strSize.cx + m_nTextureSizeWidth + fOldSize;

	if( m_rcLocalPos.sizeX < fTotalSize )
	{
		float fLimitSize = strSize.cx - ( fTotalSize - m_rcLocalPos.sizeX );
		int nLenth = m_pFont->GetTextCountByWidth( strText, fLimitSize );

		strLimitText = strText.Left( nLenth );
	}

	return AddString( nIndex, strLimitText, TextColor );
}

CBasicTextBox::STEXTWORD CBasicTextBox::MakeTextWord(const CString& strWord, const D3DCOLOR TextColor)
{
	STEXTWORD sTextWord;
	sTextWord.strWord = strWord;
	sTextWord.dwColor = TextColor;

	if (m_pFont)
	{
		m_pFont->GetTextExtent(strWord.GetString(), sTextWord.strSize);
		m_pFont->PushText(strWord.GetString());
	}

	return sTextWord;
}

CBasicTextBox::STEXTLINE CBasicTextBox::MakeTextLine(const CString& strText, const D3DCOLOR TextColor, bool bLastLine)
{
	STEXTLINE sTextLine;
	sTextLine.vectorWord.push_back(MakeTextWord(strText, TextColor));
	sTextLine.bLASTLINE = bLastLine;

	if (m_pFont)
	{
		m_pFont->GetTextExtent(strText.GetString(), sTextLine.strSize);
	}

	return sTextLine;
}

void CBasicTextBox::AutoChangeFontSize( const CString& strText, NS_UITEXTUTIL::MULTILINETEXT& MLText, int& nSplitLine, bool bCRLF )
{
    m_pFont = m_pFontOriginTemp;

	if (!m_pFont )
	{
		GASSERT ( 0 && "��Ʈ�� ���Դϴ�. SetFont()�� �̿��Ͽ� ��Ʈ���� ���� �����ʽÿ�." );
		return;
	}

    const UIRECT& rcGlobalPos = GetGlobalPos();
    CD3DFontPar* pAutoFont = m_pFont;
    DWORD dwCurFontHeight = pAutoFont->GetFontHeight();
    float fTotalSize = 0.0f;

    do 
    {
        //	���ڿ� �ڸ���
        if ( bCRLF )
            MLText = NS_UITEXTUTIL::GetMLTextWithCRLF( strText, GetLocalPos().sizeX, pAutoFont );
        else
            MLText = NS_UITEXTUTIL::GetMLTextWithoutCRLF( strText, GetLocalPos().sizeX, pAutoFont );
        nSplitLine = static_cast<int>(MLText.size());

        fTotalSize = 0.0f;
        for ( int i = 0; i < nSplitLine; ++i )
        {
            SIZE strSize;
            pAutoFont->GetTextExtent( MLText[i].strLine.GetString(), strSize );
            fTotalSize += strSize.cy;
            if ( i != nSplitLine-1 )
                fTotalSize += GetLineInterval();
        }

        if ( rcGlobalPos.sizeY < fTotalSize )
        {
            dwCurFontHeight = pAutoFont->GetFontHeight();
            dwCurFontHeight -= 1;
            if ( dwCurFontHeight < 1 )
                dwCurFontHeight = 1;
            pAutoFont = m_pEngineDevice->LoadFont( m_pFont->GetFontName(),
                dwCurFontHeight,
                m_pFont->GetFontFlags() );
        }
    } while ( dwCurFontHeight > 1 && rcGlobalPos.sizeY < fTotalSize );

    m_pFont = pAutoFont;
}

void CBasicTextBox::PushText( const CString& strText, D3DCOLOR TextColor, bool bCutWord )
{
	if (!m_pFont )
	{
		GASSERT ( 0 && "��Ʈ�� ���Դϴ�. SetFont()�� �̿��Ͽ� ��Ʈ���� ���� �����ʽÿ�." );
		return;
	}

	NS_UITEXTUTIL::MULTILINETEXT& MLText = 
		( bCutWord ) ?
		NS_UITEXTUTIL::GetMLTextWithoutCRLF( strText, GetLocalPos().sizeX, m_pFont )
		: NS_UITEXTUTIL::GetMLTextWithoutCRLFAndCutWord( strText, GetLocalPos().sizeX, m_pFont );
	int nSplitLine = static_cast<int>(MLText.size());	

    STEXTPART sTextPart;

    //	����Ÿ�� ����ֱ�
    for ( int i = 0; i < nSplitLine; ++i )
    {		
        STEXTLINE sTextLine;

        if ( (i - 1 == nSplitLine) && !MLText[i].strLine.GetLength() )
        {
            //	������������ �� ���.
        }
        else
        {
            SIZE strSize;
            m_pFont->GetTextExtent( MLText[i].strLine.GetString(), strSize );

            sTextLine.strSize = strSize;
            sTextLine.bLASTLINE = MLText[i].bLASTLINE;
            if ( m_nAlign == TEXT_ALIGN_BOTH_X )
            {				
                NS_UITEXTUTIL::ClearWordText();
                NS_UITEXTUTIL::STRINGVECTOR& WordText = NS_UITEXTUTIL::GetWordText( MLText[i].strLine );

                for ( int k = 0; k < (int)WordText.size(); ++k )
                {
                    STEXTWORD sTextWord;

                    sTextWord.strWord = WordText[k];
                    sTextWord.dwColor = TextColor;

                    if ( WordText[k].GetLength() )
                    {
                        m_pFont->GetTextExtent( WordText[k].GetString(), strSize );
                        m_pFont->PushText( WordText[k].GetString() );
                    }
                    else
                    {
                        strSize.cx = 0;
                        strSize.cy = BLANKSIZE.cy;
                    }

                    sTextWord.strSize = strSize;
					
                    sTextLine.vectorWord.push_back ( sTextWord );
                }

                m_nTotalLine++;
                sTextPart.dequeLine.push_back ( sTextLine );
            }
            else
            {
                STEXTWORD sTextWord;
                sTextWord.strWord = MLText[i].strLine;
                sTextWord.dwColor = TextColor;

                sTextWord.strSize = strSize;

                if( MLText[i].strLine.GetLength() )
                    m_pFont->PushText( MLText[i].strLine.GetString() );

                sTextLine.vectorWord.push_back ( sTextWord );
                sTextPart.dequeLine.push_back ( sTextLine );

                m_nTotalLine++;
            }
        }
    }	

    m_TextBox.push_back( sTextPart );	

	//	���� ���� ������ ���� ���, �߶�
	ReCalcEndLind();

    ReCalcRenderPos();
}

void	CBasicTextBox::SetText( const CString& strText, const D3DCOLOR& TextColor, bool bCutWord )
{
	if(  1 == GetCount () )
	{
		if( strText == GetText( 0 ) )
		{
			if ( TextColor != UINT_MAX )
			{
				SetUseTextColor ( 0, TextColor );
				SetTextColor	( 0, TextColor );
			}

			return;
		}
	}

	ClearText ();
	AddText ( strText, TextColor, bCutWord );
}

void CBasicTextBox::SetOneLineText( const CString& strText, const D3DCOLOR& TextColor )
{
	if ( 1 == GetCount () )
	{
		if ( strText == GetText( 0 ) )
		{
			if ( TextColor != NS_UITEXTUTIL::ENABLE )
			{
				SetUseTextColor ( 0, TextColor );
				SetTextColor	( 0, TextColor );
			}

			return ;
		}
	}

	ClearText ();
	AddText ( strText, TextColor );
}

int CBasicTextBox::SetTextNoSplit ( const CString& strText, const D3DCOLOR& TextColor )
{
	if(  1 == GetCount () )
	{
		if( strText == GetText( 0 ) )
		{
			//  [2/27/2015 gbgim] ;
			// �ؽ�Ʈ �⺻ ���� ���� UINT_MAX��; �⺻ ���ڸ� �������� ������ �Ʒ� ����ó���� �ȵ�;
			//if ( TextColor != UINT_MAX )
			//{
				SetUseTextColor ( 0, TextColor );
				SetTextColor	( 0, TextColor );
			//}

			return -1;
		}
	}

	ClearText ();	
	return AddTextNoSplit( strText, TextColor );
}

int CBasicTextBox::AddTextNoSplit ( const CString& strText, const D3DCOLOR& TextColor )
{
	if ( !m_pFont )
	{
		GASSERT ( 0 && "��Ʈ�� ���Դϴ�. SetFont()�� �̿��Ͽ� ��Ʈ���� ���� �����ʽÿ�." );
		return -1;
	}

	STEXTPART sTEXTPART;
	STEXTLINE sTEXTLINE;

	{
		STEXTWORD sTEXTWORD;
		sTEXTWORD.strWord = strText;
		sTEXTWORD.dwColor = TextColor;
		if ( m_pFont->GetTextExtent ( sTEXTWORD.strWord.GetString(), sTEXTWORD.strSize ) != S_OK )
		{
			return -1;
		}

		if( strText.GetLength() )
			m_pFont->PushText( strText.GetString() );

		sTEXTLINE.vectorWord.push_back ( sTEXTWORD );

		sTEXTLINE.strSize.cx = sTEXTWORD.strSize.cx;
		sTEXTLINE.strSize.cy = sTEXTWORD.strSize.cy;
	}

	sTEXTPART.dequeLine.push_back ( sTEXTLINE );
	m_TextBox.push_back ( sTEXTPART );

	m_nTotalLine++;

	//	���� ���� ������ ���� ���, �߶�
	ReCalcEndLind();
    
	ReCalcRenderPos ();

	return (int)m_TextBox.size () - 1;
}

void CBasicTextBox::CalcMaxHEIGHT ( float* pHEIGHT, int* pVISIBLELINE )
{
	float& fTextHeight = *pHEIGHT;
	int& nVisibleLine = *pVISIBLELINE;

	const UIRECT& rcGlobalPos = GetGlobalPos ();

	//	�ʱ�ȭ
	fTextHeight = 0.0f;
	nVisibleLine = 0;

	int nPartSize = (int) m_TextBox.size ();
	for ( int i = nPartSize-1; 0 <= i; --i )
	{
		STEXTPART& sTextPart = m_TextBox[i];
		DEQUETEXTLINE& sDequeTextLine = sTextPart.dequeLine;
		int nLineCount = (int) sDequeTextLine.size ();

		//	���ε� �� �ִ밪�� �̾ƿ�
		for ( int j = nLineCount-1; 0 <= j; --j )
		{
			STEXTLINE& sTextLine = sDequeTextLine[j];

			//	�� ���� ����
			float fLINE_HEIGHT = (float)sTextLine.strSize.cy;

			//	�� ���� �Ѹ� �� �ִ°�?
			if ( rcGlobalPos.sizeY <= (fTextHeight+fLINE_HEIGHT) )
			{
				if ( !nVisibleLine )
				{
//					sc::writeLogError ( "[%s] ����� �ʹ� �۽��ϴ�.", GetControlName() );

					//	������ �� ������ �������� ������
					nVisibleLine = 1;
				}

				return ;
			}

			fTextHeight += fLINE_HEIGHT;

			//	�������� �ִ� �� ��
			++nVisibleLine;

			//	�� ����
			if ( j != 0 )
			{
				float fLINE_GAP = GetLineInterval ();
				fTextHeight += fLINE_GAP;
			}
		}			

		//	���� ����
		if ( i != 0 )
		{
			float fPART_GAP = GetPartInterval ();
			fTextHeight += fPART_GAP;
		}
	}
}

void CBasicTextBox::CalcMaxHEIGHT ( float* pHEIGHT, int* pVISIBLELINE, int nPartIndexCOPY, int nLineIndexCOPY )
{
	float& fTextHeight = *pHEIGHT;
	int& nVisibleLine = *pVISIBLELINE;

	const UIRECT& rcGlobalPos = GetGlobalPos ();

	//	�ʱ�ȭ
	fTextHeight = 0.0f;
	nVisibleLine = 0;

	int nPartSize = (int) m_TextBox.size ();
	for ( int i = nPartIndexCOPY; i < nPartSize; ++i )
	{
		STEXTPART& sTextPart = m_TextBox[i];
		DEQUETEXTLINE& sDequeTextLine = sTextPart.dequeLine;
		int nLineCount = (int) sDequeTextLine.size ();

		//	���ε� �� �ִ밪�� �̾ƿ�
		for ( int j = nLineIndexCOPY; j < nLineCount; ++j )
		{
			STEXTLINE& sTextLine = sDequeTextLine[j];

			//	�� ���� ����
			float fLINE_HEIGHT = (float)sTextLine.strSize.cy;

			//	�� ���� �Ѹ� �� �ִ°�?
			if ( rcGlobalPos.sizeY < (fTextHeight+fLINE_HEIGHT) )
			{
				if ( !nVisibleLine )
				{
//					sc::writeLogError ( "[%s] ����� �ʹ� �۽��ϴ�.", GetControlName() );

					//	������ �� ������ �������� ������
					nVisibleLine = 1;
				}

				return ;
			}

			fTextHeight += fLINE_HEIGHT;

			//	�������� �ִ� �� ��
			++nVisibleLine;

			//	�� ����
			if ( (j+1) != nLineCount )
			{
				float fLINE_GAP = GetLineInterval ();
				fTextHeight += fLINE_GAP;
			}
		}			

		//	���� ����
		if ( (i+1) != nPartSize )
		{
			float fPART_GAP = GetPartInterval ();
			fTextHeight += fPART_GAP;
		}

		nLineIndexCOPY = 0;
	}
}

float CBasicTextBox::CalcMaxHEIGHT ( int nBeginLine, int nEndLine )
{
	if ( nBeginLine < 0 ) return 0.0f;	
	if ( nEndLine < nBeginLine ) return 0.0f;

	//	�ʱ�ȭ
	float fTextHeight = 0.0f;
	int nBEGIN_PART = 0;
	int nBEGIN_LINE = 0;
	int nAllLineCountEnd = nEndLine - nBeginLine;
	int nAllLineCount = 0;

	if ( !FindStartIndex ( nBEGIN_PART, nBEGIN_LINE, nBeginLine ) ) return 0.0f;

	int nPartSize = (int) m_TextBox.size ();
	for ( int i = nBEGIN_PART; i < nPartSize; ++i )
	{
		STEXTPART& sTextPart = m_TextBox[i];
		DEQUETEXTLINE& sDequeTextLine = sTextPart.dequeLine;
		int nLineCount = (int) sDequeTextLine.size ();

		//	���ε� �� �ִ밪�� �̾ƿ�
		for ( int j = nBEGIN_LINE; j < nLineCount; ++j )
		{
			STEXTLINE& sTextLine = sDequeTextLine[j];

			//	�� ���� ����			
			fTextHeight += (float)sTextLine.strSize.cy;

			//	���� ���
			++nAllLineCount;
			if ( nAllLineCountEnd < nAllLineCount ) return fTextHeight;

			//	�� ����
			if ( (j+1) != nLineCount ) fTextHeight += m_fLineInterval;
		}	

		//	���� ����
		if ( (i+1) != nPartSize ) fTextHeight += m_fPartInterval;

		nBEGIN_LINE = 0;
	}

	return fTextHeight;
}

void CBasicTextBox::ReCalcRenderPos ()
{	
	const UIRECT& rcGlobalPos = GetGlobalPos ();

	//	�Ѹ� ���ڿ� ��ġ Ȯ��	
	if ( !(m_nAlign & TEXT_ALIGN_BOTTOM) )
	{
		if ( !FindStartIndex ( m_nPartIndex, m_nLineIndex, m_nCurLine ) ) return ;
	}

	//	������ ����
	for ( int nImage = 0; nImage < (int)m_ImageList.size (); ++nImage )
	{
		CUIControl* pImage = FindControl ( m_ImageList[nImage] );
		if ( pImage ) pImage->SetVisibleSingle ( FALSE );
	}

	const int nVisibleLineBack = m_nVisibleLine;

	//	�Ѹ� �ִ� Y ������ ���
	float fTextHeight = 0.0f;
	if ( m_nAlign & TEXT_ALIGN_BOTTOM )
	{
		CalcMaxHEIGHT ( &fTextHeight, &m_nVisibleLine );
		int nEND_LINE = m_nTotalLine - m_nVisibleLine;
		if ( nEND_LINE < m_nCurLine ) m_nCurLine = nEND_LINE;
		if ( !FindStartIndex ( m_nPartIndex, m_nLineIndex, m_nCurLine ) ) return ;
	}
	else
	{
		CalcMaxHEIGHT ( &fTextHeight, &m_nVisibleLine, m_nPartIndex, m_nLineIndex );
	}

	//	���纻
	int nPartIndex = m_nPartIndex;
	int nLineIndex = m_nLineIndex;

	//	X�� �⺻ ������
	float fLEFT = (float)ceil(rcGlobalPos.left);

	//	Y�� �⺻ ������
	float fTOP = (float)ceil(rcGlobalPos.top);			
	if ( m_nAlign & TEXT_ALIGN_TOP )
		;
	else if ( m_nAlign & TEXT_ALIGN_BOTTOM )
		fTOP = (float)ceil(rcGlobalPos.bottom - fTextHeight);
	else if ( m_nAlign & TEXT_ALIGN_CENTER_Y )
		fTOP = (float)ceil(rcGlobalPos.top + (rcGlobalPos.sizeY-fTextHeight) * 0.5f);
    
	bool bBREAK = false;
	int nCurrentLine = 0;
	int nPartSize = (int) m_TextBox.size ();
	float fTEXT_POS_Y = fTOP;
	for ( int i = nPartIndex; i < nPartSize; ++i )
	{
		STEXTPART& sTextPart = m_TextBox[i];
		UIRECT& rcPart = sTextPart.m_rcPart;
		DEQUETEXTLINE& sDequeTextLine = sTextPart.dequeLine;
		int nLineCount = (int)sDequeTextLine.size ();

		D3DXVECTOR2 vSIZE ( 0.0f, 0.0f );
		D3DXVECTOR2 vBEGIN( 0.0f, 0.0f );

		//	NOTE
		//		���ο� �����ΰ�?
		//		������ ���� �Ǵ� ���� ���� ���� �����ΰ�?		
		bool bNEWLINE = true;
		for ( int j = nLineIndex; j < nLineCount; ++j )
		{
			++nCurrentLine;
			if ( m_nVisibleLine < nCurrentLine )
			{
				bBREAK = true;
				break;
			}

			STEXTLINE& sTextLine = sDequeTextLine[j];
			float fSizeX = (float) sTextLine.strSize.cx;
			float fSizeY = (float) sTextLine.strSize.cy;

			//	X ����
			float fPosX = fLEFT;
			if ( m_nAlign & TEXT_ALIGN_LEFT  )
				;
			else if ( m_nAlign & TEXT_ALIGN_RIGHT )
				fPosX = (float)ceil(rcGlobalPos.right - fSizeX);
			else if ( m_nAlign & TEXT_ALIGN_CENTER_X )
				fPosX = (float)ceil(fLEFT + (rcGlobalPos.sizeX - fSizeX) / 2.0f);            
			
			//	Y ����
			float fPosY = fTEXT_POS_Y;

			fTEXT_POS_Y += fSizeY;				//	���� �ִ� ��
			if ( j != (nLineCount-1) ) fTEXT_POS_Y += GetLineInterval();	//	�ٰ�

			//	��Ʈ�� ù��° �ٿ��� ������ ���
			if ( j == 0 )
			{
				if ( i < (int) m_ImageList.size () )
				{
					CUIControl* pImage = FindControl ( m_ImageList[i] );
					if ( pImage )
					{
						const UIRECT& rcImagePos = pImage->GetGlobalPos ();
						const UIRECT& rcImageLocalPos = pImage->GetLocalPos();
						pImage->SetGlobalPos (D3DXVECTOR2(fPosX-rcImagePos.sizeX-5.0f + rcImageLocalPos.left,fPosY));
						pImage->SetVisibleSingle ( TRUE );					
					}
				}
			}


			VECTORTEXTWORD& vectorTextWord = sTextLine.vectorWord;
			if ( m_nAlign & TEXT_ALIGN_BOTH_X && 1 < vectorTextWord.size() )
			{
				long lGAP = 0;
				long OneGap = 0;
				int nBlankCount = (int)vectorTextWord.size() - 1;
				if ( !nBlankCount ) nBlankCount = 1;

				//	NOTE
				//		�ǵ������� ùĭ�� ������ �ƴѵ�,
				//		ùĭ�� ����, ������� �ʴ´�.
				if ( !vectorTextWord[0].strWord.GetLength() && !bNEWLINE )
				{
					lGAP += BLANKSIZE.cx;
					nBlankCount--;
					if ( nBlankCount == 0 ) nBlankCount = 1;
				}

				//	NOTE
				//		������ ĭ�� ��ĭ�� ��� ���� ��� ������� �ʴ´�.				
				if ( !vectorTextWord[vectorTextWord.size()-1].strWord.GetLength() )
				{
					lGAP += BLANKSIZE.cx;
					nBlankCount--;
					if ( nBlankCount == 0 ) nBlankCount = 1;
				}

				//	NOTE
				//		������ ��µǰ� ���� �κ��� ���鿡 ���ؼ�
				//		�󸶸�ŭ���� ���� ������ ���鿡 �����ų�������� ����
				//		����Ѵ�.
				lGAP += (long(ceil(rcGlobalPos.sizeX)) - sTextLine.strSize.cx);				
				OneGap = (long)ceil(float(lGAP) / float(nBlankCount));

				for ( int k = 0; k < (int) vectorTextWord.size (); ++k )
				{
					STEXTWORD& sTextWord = vectorTextWord[k];

					if ( k == 0 && j == nLineIndex )
					{
						vBEGIN.x = fPosX;
						vBEGIN.y = fPosY;
					}

					//	NOTE
					//		�ǵ������� ùĭ�� ������ �ƴѵ�,
					//		ùĭ�� �� ���, �����Ѵ�.
					if ( k == 0 && !bNEWLINE )
					{
						if ( !sTextWord.strWord.GetLength () )
						{
							continue;
						}
					}

					//	NOTE
					//		������ ĭ�� �� ��� ������� �ʴ´�.
					bool bLINE_END = (k == (vectorTextWord.size () - 1));
					if ( bLINE_END )
					{
						if ( !sTextWord.strWord.GetLength () )
						{
							continue;
						}
					}
					
					sTextWord.x = fPosX;
					sTextWord.y = fPosY;

					fPosX += sTextWord.strSize.cx;
					fPosX += BLANKSIZE.cx;

					//	NOTE
					//		���������� CR/LF�� ���� ����Ƿ��� �ϴ°�?
					//		bLASTLINE�� true�� ���, ������ �ڽ��� ������ �ʰ�
					//		����ȴٴ� ���� �ǹ��Ѵ�.
					if ( !sTextLine.bLASTLINE )
					{
						if ( 0 < lGAP )
						{
							//	����, ������ ���̸� '��'�� �� �Һ��Ѵ�.
							if ( bLINE_END )
							{
								fPosX += lGAP;
								lGAP = 0;
							}
							else
							{
								if ( OneGap < lGAP )
								{
									fPosX += OneGap;
									lGAP -= OneGap;
								}
								else
								{
									fPosX += lGAP;
									lGAP = 0;
								}
							}
						}

						bNEWLINE = false;
					}
					else
					{
						bNEWLINE = true;
					}
				}
			}
			else
			{
				for ( int k = 0; k < (int) vectorTextWord.size (); ++k )
				{
					STEXTWORD& sTextWord = vectorTextWord[k];
					sTextWord.x = fPosX;
					sTextWord.y = fPosY;

					if ( k == 0 && j == nLineIndex )
					{
						vBEGIN.x = fPosX;
						vBEGIN.y = fPosY;
					}

					fPosX += sTextWord.strSize.cx;
				}
			}

			vSIZE.x = new_max(sTextLine.strSize.cx, vSIZE.x);
			vSIZE.y += fSizeY;

			if ( j != (nLineCount-1) )
			{
				vSIZE.y += m_fLineInterval;
			}
		}

		if ( bBREAK ) return;

		if ( i != (nPartSize-1) ) fTEXT_POS_Y += GetPartInterval ();	//	��Ʈ ���͹�

		rcPart = UIRECT ( vBEGIN.x, vBEGIN.y, vSIZE.x, vSIZE.y );

		nLineIndex = 0;
	}	
}

BOOL CBasicTextBox::FindStartIndex ( int& nPartIndex, int& nLineIndex, const int nCurLine )
{
	if ( m_nTotalLine < 1 )	return FALSE;

	const int nPartSize = (int)m_TextBox.size ();
    
	int nCurLineAmount = 0;
	for ( int nPartIterIndex = 0; nPartIterIndex < nPartSize; ++nPartIterIndex )
	{
		const STEXTPART& sTextPart = m_TextBox[nPartIterIndex];

        const int nLineSize = (int)sTextPart.dequeLine.size ();
		nCurLineAmount += nLineSize;

		if ( nCurLine < nCurLineAmount )
		{
			nPartIndex = nPartIterIndex;			
			nLineIndex = nCurLine - (nCurLineAmount - nLineSize);

			return TRUE;
		}
	}

	return FALSE;
}

void CBasicTextBox::SetDiffuseAlpha ( WORD wAlpha )
{
    const int nTextSize = (int)m_TextBox.size ();
    if ( nTextSize < 1 ) return;

    int nIndex = 0;

    for ( ; nIndex < nTextSize; ++nIndex )
    {
        STEXTPART& sTEXTPART = m_TextBox[nIndex];
        sTEXTPART.m_dwColor  = (sTEXTPART.m_dwColor | 0xFF000000) & ((((0xFF)&wAlpha) << 24) | (0x00FFFFFF));

        DEQUETEXTLINE_ITER iterPart = sTEXTPART.dequeLine.begin ();
        for ( ; iterPart != sTEXTPART.dequeLine.end(); ++iterPart )
        {
            VECTORTEXTWORD&	vectorWord = (*iterPart).vectorWord;	
            VECTORTEXTWORD::iterator iterWord = vectorWord.begin();

            for ( ; iterWord != vectorWord.end(); ++iterWord )
            {
                STEXTWORD& sWord = (*iterWord);	
                sWord.dwColor = (sWord.dwColor | 0xFF000000) & ((((0xFF)&wAlpha) << 24) | (0x00FFFFFF));
            }
        }
    }

    if ( m_bUseOverColor )
    {
        m_dwOverColor = (m_dwOverColor | 0xFF000000) & ((((0xFF)&wAlpha) << 24) | (0x00FFFFFF));
    }

    CUIControl::SetDiffuseAlpha( wAlpha );
}

HRESULT CBasicTextBox::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;
	return CUIGroup::InitDeviceObjects ( pd3dDevice );
}

//void CBasicTextBox::RemoveLine(int nIndex, int nLine)
//{
//	if ( nIndex < 0 )
//	{
//		return;
//	}
//
//	if (m_TextBox.size() <= static_cast<size_t>(nIndex))
//	{
//		return;
//	}
//
//	if (m_TextBox[nIndex].dequeLine.size() <= static_cast<size_t>(nLine))
//	{
//		return;
//	}
//
//	DEQUETEXTLINE_ITER it = m_TextBox[nIndex].dequeLine.begin();
//
//	std::advance(it, nLine);
//	m_TextBox[nIndex].dequeLine.erase(it);
//	--m_nTotalLine;
//
//	ReCalcRenderPos();
//}


CBasicTextBox* CreateBasicTextControl(CUIGroup* pParent, const char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID WndID /*= NO_ID*/)
{
    if ( pParent == NULL )
        return NULL;

    CBasicTextBox* pBasicText = new CBasicTextBox(pParent->GetEngineDevice());
    pBasicText->CreateSub ( pParent, szControlKeyword, UI_FLAG_DEFAULT, WndID );
    pBasicText->SetFont ( pFont );
    pBasicText->SetTextAlign ( nAlign );	
    pParent->RegisterControl ( pBasicText );

    return pBasicText;
}