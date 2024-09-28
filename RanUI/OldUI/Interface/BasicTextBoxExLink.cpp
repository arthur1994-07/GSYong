#include "StdAfx.h"
#include "../../../SigmaCore/Log/LogMan.h"
#include "../../../SigmaCore/String/StringFormat.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "BasicTextBoxExLink.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CBasicTextBoxExLink::CBasicTextBoxExLink(EngineDeviceMan* pEngineDevice)
	: CBasicTextBoxEx(pEngineDevice)
    , m_nSelectWordData ( nNOT_VISIBLE )
{
}


CBasicTextBoxExLink::~CBasicTextBoxExLink()
{
}

void CBasicTextBoxExLink::SetTextLink ( const CString& strText, const VEC_LINK_TEXT_DATA& vecLinkData, const D3DCOLOR& TextColor )
{
	ClearText ();
	AddTextLink ( strText, vecLinkData, TextColor );
}

int CBasicTextBoxExLink::IsLinkDataStart( const VEC_LINK_DATA_POS& vecLinkPos, int nStart )
{
	int nFind = -1;

	for ( DWORD i = 0; i < vecLinkPos.size(); ++i )
	{
		if ( vecLinkPos[i].nStart == nStart ) return i;
	}

	return nFind;
}

int CBasicTextBoxExLink::IsLinkData( const VEC_LINK_DATA_POS& vecLinkPos, int nStart, int nEnd )
{
	int nFind = -1;

	for ( DWORD i = 0; i < vecLinkPos.size(); ++i )
	{
		if ( vecLinkPos[i].nStart == nStart && vecLinkPos[i].nEnd == nEnd ) return i;
	}

	return nFind;

}

int	CBasicTextBoxExLink::AddTextLink ( const CString& strText,  const VEC_LINK_TEXT_DATA& vecLinkTextData, const D3DCOLOR& TextColor )
{
//	return	CBasicTextBox::AddText( strText, TextColor );

	if ( !m_pFont )
	{
		GASSERT ( 0 && "폰트가 널입니다. SetFont()를 이용하여 폰트부터 값을 넣으십시오." );
		return 0;
	}

	VEC_LINK_DATA_POS vecLinkPos;

	for ( DWORD i = 0; i < vecLinkTextData.size(); ++i ) 
	{
		vecLinkPos.push_back( vecLinkTextData[i].sLinkPos );
	}

	//	문자열 자르기
	NS_UITEXTUTIL::MULTILINETEXT MLText;
	GetMLTextWithoutCRLF( strText, GetLocalPos().sizeX, m_pFont, MLText, vecLinkPos );

	int nSplitLine = (int)MLText.size ();
	
	STEXTPART sTextPart;

	//	데이타로 집어넣기
	int nStart = 0;
	for ( int i = 0; i < nSplitLine; ++i )
	{		
		STEXTLINE sTextLine;

		if ( (i - 1 == nSplitLine) && !MLText[i].strLine.GetLength () )
		{
			//	마지막라인이 빈 경우.
		}
		else
		{
			nStart = strText.Find( MLText[i].strLine, nStart );

			SIZE strSize;
			m_pFont->GetTextExtent ( MLText[i].strLine.GetString(), strSize );

			sTextLine.strSize = strSize;
			sTextLine.bLASTLINE = MLText[i].bLASTLINE;
			if ( m_nAlign & TEXT_ALIGN_BOTH_X )
			{				
				NS_UITEXTUTIL::ClearWordText ();
				NS_UITEXTUTIL::STRINGVECTOR WordText;

				GetWordText ( MLText[i].strLine, WordText, vecLinkPos, nStart );

				for ( int k = 0; k < (int)WordText.size(); ++k )
				{
					STEXTWORD sTextWord;

					sTextWord.strWord = WordText[k];
					sTextWord.dwColor = TextColor;

					int nIndex = IsLinkData ( vecLinkPos, nStart, nStart+WordText[k].GetLength() );
					if ( nIndex >= 0 )
					{
						sTextWord.dwColor = vecLinkTextData[nIndex].dwTextColor;
						sTextWord.nData = vecLinkTextData[nIndex].nLinkTypeIndex;
					}

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

					nStart += WordText[k].GetLength();
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

	//	제한 라인 범위가 넘을 경우, 잘라냄
	ReCalcEndLind();
	
	ReCalcRenderPos ();

	return (int)m_TextBox.size () - 1;
}

int CBasicTextBoxExLink::AddStringLink( int nIndex, const CString& strText, const VEC_LINK_TEXT_DATA& vecLinkTextData, const D3DCOLOR& TextColor )
{
    if ( !m_pFont )
    {
        GASSERT ( 0 && "폰트가 널입니다. SetFont()를 이용하여 폰트부터 값을 넣으십시오." );
        return -1;
    }

    if ( nIndex < 0 || (int) m_TextBox.size () <= nIndex )
    {
        GASSERT ( 0 && "인덱스가 범위를 넘어섭니다." );
        return -1;
    }

    if ( !strText.GetLength () )
    {
        GASSERT ( 0 && "문자열이 없습니다." );
        return -1;
    }

    CString strTextCopy = strText;

    STEXTPART& sTextPart = m_TextBox[nIndex];

    VEC_LINK_DATA_POS vecLinkPos;
    for ( DWORD i = 0; i < vecLinkTextData.size(); ++i ) 
    {
        vecLinkPos.push_back( vecLinkTextData[i].sLinkPos );
    }
    int nStart = 0;

	int nPrevLineLength = 0;
    {
        if ( sTextPart.dequeLine.empty() )
        {
			//if ( m_ImageList.size() < m_TextBox.size() )
			//{
			//	m_TextBox.pop_back();
			//}

    //        CUIControl* pControl = GetParent();
    //        if ( pControl )
    //        {
    //            std::string strParent = pControl->GetControlNameEx();
    //            std::string ErrMsg = sc::string::format("Parent Control Name = '%s'에 \"%s\"를 'AddString'할 수 없습니다.", strParent.c_str(), strText.GetString());
				//AfxMessageBox(ErrMsg.c_str(), MB_OK);
				//return -1;
    //        }

			CUIControl* pControl = GetParent();
			if ( pControl )
			{
				std::string strParent = pControl->GetControlNameEx();
				std::string ErrMsg = sc::string::format("Parent Control Name = '%s'에 Index - %d, Tex BoxSize - %d, '%s'를 'AddString'할 수 없습니다.", strParent.c_str(), nIndex, m_TextBox.size(), strText.GetString());

				CDebugSet::ToLogFile( ErrMsg );
				GASSERT ( 0 && _T("AddString'할 수 없습니다. 로그파일을 확인해 주세요.") );

				EraseTextBoxIndex( nIndex );

				return -1;
			}
			else
			{
				std::string ErrMsg = sc::string::format("Index - %d, Tex BoxSize - %d, '%s'를 'AddString'할 수 없습니다.", nIndex, m_TextBox.size(), strText.GetString());

				CDebugSet::ToLogFile( ErrMsg );
				GASSERT ( 0 && _T("AddString'할 수 없습니다. 로그파일을 확인해 주세요.") );

				EraseTextBoxIndex( nIndex );

				return -1;
			}
        }

        STEXTLINE& sTextLine = sTextPart.dequeLine.back ();

        //	기존 문자열 마지막 라인에 글 붙이기		
        float fOldSize = (float) sTextLine.strSize.cx;

        //	기존의 문자열에 붙일 만큼만 자르기
        NS_UITEXTUTIL::MULTILINETEXT MLText;
        GetMLTextWithoutCRLF( strTextCopy, GetLocalPos().sizeX - fOldSize, m_pFont, MLText, vecLinkPos );

        if ( MLText.size () )
        {

            nStart = strTextCopy.Find( MLText[0].strLine, nStart );

            NS_UITEXTUTIL::ClearWordText();
            NS_UITEXTUTIL::STRINGVECTOR WordText;

            GetWordText( MLText[0].strLine, WordText, vecLinkPos, nStart );

            for ( int k = 0; k < (int)WordText.size(); ++k )
            {
                STEXTWORD sTextWord;

                sTextWord.strWord = WordText[k];
                sTextWord.dwColor = TextColor;

                int nIndex = IsLinkData( vecLinkPos, nStart, nStart+WordText[k].GetLength() );
                if ( nIndex >= 0 )
                {
                    sTextWord.dwColor = vecLinkTextData[nIndex].dwTextColor;
                    sTextWord.nData = vecLinkTextData[nIndex].nLinkTypeIndex;
                }

                if( WordText[k].GetLength() )
                {
                    m_pFont->GetTextExtent( WordText[k].GetString(), sTextWord.strSize );
                    m_pFont->PushText( WordText[k].GetString() );
                }
                else
                {
                    sTextWord.strSize.cx = 0;
                    sTextWord.strSize.cy = BLANKSIZE.cy;
                }

                sTextLine.vectorWord.push_back( sTextWord );
                sTextLine.strSize.cx += sTextWord.strSize.cx;
                sTextLine.strSize.cy = sTextWord.strSize.cy;

                nStart += WordText[k].GetLength();
            }

            nPrevLineLength = MLText[0].strLine.GetLength();
            int nCutPos = strTextCopy.GetLength() - nPrevLineLength;
            strTextCopy = strTextCopy.Right( nCutPos );
        }
    }

    if ( strTextCopy.GetLength () )
    {
        //	기존의 문자열에 붙일 만큼만 자르기
        NS_UITEXTUTIL::MULTILINETEXT MLText;
        GetMLTextWithoutCRLF( strTextCopy, GetLocalPos().sizeX, m_pFont, MLText, vecLinkPos, true, nPrevLineLength );

        int nSplitLine = (int)MLText.size ();

        //	데이타로 집어넣기
        for ( int i = 0; i < nSplitLine; ++i )
        {
            STEXTLINE sTextLine;

            SIZE strSize;
            m_pFont->GetTextExtent( MLText[i].strLine.GetString(), strSize );

            sTextLine.strSize = strSize;
            sTextLine.bLASTLINE = MLText[i].bLASTLINE;

            if ( (i - 1 == nSplitLine) && !MLText[i].strLine.GetLength () )
            {
                //	마지막라인이 빈 경우.
            }
            else
            {
                NS_UITEXTUTIL::ClearWordText ();
                NS_UITEXTUTIL::STRINGVECTOR WordText;

                GetWordText( MLText[i].strLine, WordText, vecLinkPos, nStart );

                for ( int k = 0; k < (int)WordText.size(); ++k )
                {
                    STEXTWORD sTextWord;

                    sTextWord.strWord = WordText[k];
                    sTextWord.dwColor = TextColor;

                    int nIndex = IsLinkData( vecLinkPos, nStart, nStart+WordText[k].GetLength() );
                    if ( nIndex >= 0 )
                    {
                        sTextWord.dwColor = vecLinkTextData[nIndex].dwTextColor;
                        sTextWord.nData = vecLinkTextData[nIndex].nLinkTypeIndex;
                    }

                    if( WordText[k].GetLength() )
                    {
                        m_pFont->GetTextExtent( WordText[k].GetString(), sTextWord.strSize );
                        m_pFont->PushText( WordText[k].GetString() );
                    }
                    else
                    {
                        sTextWord.strSize.cx = 0;
                        sTextWord.strSize.cy = BLANKSIZE.cy;
                    }

                    //	아직은 word가 별 의미를 가지지 못한다.
                    //	따라서 항상 1개만 존재한다고 가정하고, 0번 인덱스를 사용한다고
                    //	약속한다.
                    sTextLine.vectorWord.push_back( sTextWord );

                    nStart += WordText[k].GetLength();
                }

                sTextPart.dequeLine.push_back( sTextLine );

                m_nTotalLine++;

                //	제한 라인이 사용되고, 걸리는 경우
                //			if ( m_nLimitLine && m_nLimitLine == i )	break;
            }
        }	
    }

    //	m_TextBox.push_back ( sTextString );

	//	제한 라인 범위가 넘을 경우, 잘라냄
	ReCalcEndLind();

    ReCalcRenderPos();

    return (int) m_TextBox.size() - 1;
}

void CBasicTextBoxExLink::ReCalcRenderPos ()
{
	const UIRECT& rcGlobalPos = GetGlobalPos ();

	//	뿌릴 문자열 위치 확인	
	if ( !(m_nAlign & TEXT_ALIGN_BOTTOM) )
	{
		if ( !FindStartIndex ( m_nPartIndex, m_nLineIndex, m_nCurLine ) ) return ;
	}

	//	아이콘 리셋
	for ( int nImage = 0; nImage < (int)m_ImageList.size (); ++nImage )
	{
		CUIControl* pImage = FindControl ( m_ImageList[nImage] );
		if ( pImage ) pImage->SetVisibleSingle ( FALSE );
	}

	const int nVisibleLineBack = m_nVisibleLine;

	//	뿌릴 최대 Y 사이즈 계산
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

	//	복사본
	int nPartIndex = m_nPartIndex;
	int nLineIndex = m_nLineIndex;

	//	X축 기본 포지션
	float fLEFT = (float)ceil(rcGlobalPos.left);

	//	Y축 기본 포지션
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
		//		새로운 라인인가?
		//		문장이 엔터 또는 널을 만나 끝난 문장인가?		
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

			//	X 시작
			float fPosX = fLEFT;
			if ( m_nAlign & TEXT_ALIGN_LEFT  )
				;
			else if ( m_nAlign & TEXT_ALIGN_RIGHT )
				fPosX = (float)ceil(rcGlobalPos.right - fSizeX);
			else if ( m_nAlign & TEXT_ALIGN_CENTER_X )
				fPosX = (float)ceil(fLEFT + (rcGlobalPos.sizeX - fSizeX) / 2.0f);            
			
			//	Y 시작
			float fPosY = fTEXT_POS_Y;

			fTEXT_POS_Y += fSizeY;				//	글이 있는 줄
			if ( j != (nLineCount-1) ) fTEXT_POS_Y += GetLineInterval();	//	줄간

			//	파트의 첫번째 줄에서 아이콘 출력
			if ( j == 0 )
			{
				if ( i < (int) m_ImageList.size () )
				{
					CUIControl* pImage = FindControl ( m_ImageList[i] );
					if ( pImage )
					{
						const UIRECT& rcImagePos = pImage->GetGlobalPos ();
						pImage->SetGlobalPos (D3DXVECTOR2(fPosX-rcImagePos.sizeX-5.0f,fPosY));
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
				//		의도적으로 첫칸을 띄운것이 아닌데,
				//		첫칸이 빈경우, 출력하지 않는다.
				if ( !vectorTextWord[0].strWord.GetLength() && !bNEWLINE )
				{
					lGAP += BLANKSIZE.cx;
					nBlankCount--;
					if ( nBlankCount == 0 ) nBlankCount = 1;
				}

				//	NOTE
				//		마지막 칸에 빈칸이 들어 있을 경우 출력하지 않는다.				
				if ( !vectorTextWord[vectorTextWord.size()-1].strWord.GetLength() )
				{
					lGAP += BLANKSIZE.cx;
					nBlankCount--;
					if ( nBlankCount == 0 ) nBlankCount = 1;
				}

				//	NOTE
				//		문장이 출력되고 남은 부분의 여백에 대해서
				//		얼마만큼으로 나눠 각각의 공백에 적용시킬것인지에 대해
				//		계산한다.
//				lGAP += (long(ceil(rcGlobalPos.sizeX)) - sTextLine.strSize.cx);				
//				OneGap = (long)ceil(float(lGAP) / float(nBlankCount));

				for ( int k = 0; k < (int) vectorTextWord.size (); ++k )
				{
					STEXTWORD& sTextWord = vectorTextWord[k];

					if ( k == 0 && j == nLineIndex )
					{
						vBEGIN.x = fPosX;
						vBEGIN.y = fPosY;
					}

					//	NOTE
					//		의도적으로 첫칸을 띄운것이 아닌데,
					//		첫칸이 빈 경우, 무시한다.
					if ( k == 0 && !bNEWLINE )
					{
						if ( !sTextWord.strWord.GetLength () )
						{
							continue;
						}
					}

					//	NOTE
					//		마지막 칸이 빈 경우 출력하지 않는다.
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
//					fPosX += BLANKSIZE.cx;

					//	NOTE
					//		실질적으로 CR/LF를 만나 개행되려고 하는가?
					//		bLASTLINE이 true인 경우, 문장이 박스에 꽉차지 않고
					//		종료된다는 것을 의미한다.
					if ( !sTextLine.bLASTLINE )
					{
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

			vSIZE.x = max(sTextLine.strSize.cx, vSIZE.x);
			vSIZE.y += fSizeY;

			if ( j != (nLineCount-1) )
			{
				vSIZE.y += m_fLineInterval;
			}
		}

		if ( bBREAK ) return;

		if ( i != (nPartSize-1) ) fTEXT_POS_Y += GetPartInterval ();	//	파트 인터벌

		rcPart = UIRECT ( vBEGIN.x, vBEGIN.y, vSIZE.x, vSIZE.y );

		nLineIndex = 0;
	}	
}

void CBasicTextBoxExLink::GetMLTextWithoutCRLF ( const CString& strOrigin, float fWidth, CD3DFontPar* pFont, 
								NS_UITEXTUTIL::MULTILINETEXT& MLText, const VEC_LINK_DATA_POS& vecLinkPos, bool bSplit, int nPrevLineLength )
{
//	RemoveCRLF ( strOrigin );		
	NS_UITEXTUTIL::STRINGVECTOR sVector;

	CString strCRLF = "\r\n";
		
	CString strLeft;
	CString strRight = strOrigin;
	int nFind = strRight.Find( strCRLF );
	while ( nFind > 0 ) 
	{
		strLeft = strOrigin.Left( nFind );
		sVector.push_back( strLeft );

		strRight = strRight.Right( strRight.GetLength() - nFind -2 );
		nFind = strRight.Find( strCRLF );
	}
		
	sVector.push_back( strRight );

	
	if ( sVector.size() )
	{
		for ( NS_UITEXTUTIL::MULTILINETEXT::size_type i = 0; i < sVector.size(); ++i )
		{				
			SplitMLText ( sVector[i], fWidth, pFont, MLText, vecLinkPos, bSplit, nPrevLineLength );
		}
	}
}

void CBasicTextBoxExLink::GetWordText( const CString &strOrigin, NS_UITEXTUTIL::STRINGVECTOR& WordText, const VEC_LINK_DATA_POS& vecLinkPos, int nStart )
{
	CString strTemp;
	int strLength = strOrigin.GetLength ();

	for ( int i = 0; i < strLength; )
	{   				
		BOOL bDBCS = FALSE;
		if ( (i + 1) < strLength )
		{				
		
			int nIndex = IsLinkDataStart( vecLinkPos, i+nStart );
			if ( 0 <= nIndex )
			{
				if( strTemp.GetLength() ) WordText.push_back ( strTemp );
				
				int nSize = vecLinkPos[nIndex].nEnd - vecLinkPos[nIndex].nStart;
				if ( nSize > 0 )
				{
					strTemp = strOrigin.Mid( i, nSize );
					
					WordText.push_back ( strTemp );
					strTemp.Empty();

					i += nSize;
					continue;
				}
			}


			//	한글인가?
			if ( NS_UITEXTUTIL::IsDBCS ( strOrigin[i] ) )
			{
				bDBCS = TRUE;
			}
		}

		//	영문
		{
			strTemp += strOrigin[i];
			i++;
		}

		//	한글이면 하나 더 처리...
		if ( bDBCS )
		{				
			strTemp += strOrigin[i];
			i++;
		}
	}

	if ( strTemp.GetLength() && strTemp[strTemp.GetLength()-1] == ' ' )
	{
		// 공백 제거하는 부분인데 요것 때문에 아이템 링크들 사이에 공백이 있을경우 텍스트가 깨지는 문제가 있어 주석처리.
		//strTemp = strTemp.Left ( strTemp.GetLength () - 1 );

		{
			WordText.push_back ( strTemp );
		}

		strTemp.Empty ();

		{
			WordText.push_back ( strTemp );
		}
	}
	else
	{
		//	남아있는 문자들 처리
		WordText.push_back ( strTemp );
	}
}


void CBasicTextBoxExLink::SplitMLText( CString& strOrigin, float fWidth, CD3DFontPar* pFont,
									   NS_UITEXTUTIL::MULTILINETEXT& MLText, const VEC_LINK_DATA_POS& vecLinkPos, bool bSplit, int nPrevLineLength )
{
	
	static const char cSPACE = ' ';

	if ( !pFont )
	{
		GASSERT ( pFont && "폰트가 지정되지 않았습니다." );
		return ;
	}

	SIZE Size;
	pFont->GetTextExtent ( strOrigin.GetString(), Size );

	CString strTemp;
	if ( fWidth < Size.cx )
	{
		int strLength = strOrigin.GetLength ();					
		for ( int i = 0; i < strLength; )
		{   				
			BOOL bDBCS = FALSE;				
			if ( (i + 1) < strLength )
			{
				//	한글인가?
				if ( NS_UITEXTUTIL::IsDBCS ( strOrigin[i] ) )
				{
					bDBCS = TRUE;
				}
			}

			bool bIsENGLISH = false;
			//	한글이면 하나 더 처리...
			if ( bDBCS )
			{				
				strTemp += strOrigin[i++];
				strTemp += strOrigin[i++];
			}
			//	영문
			else
			{
				char c = strOrigin[i];
				if ( isalpha ( c ) ) bIsENGLISH = true;
				strTemp += strOrigin[i];
				i++;
			}

			//	문자열이 범위를 넘어서면 셋에 등록하고,
			//	그 다음부터 계속 진행한다.
			pFont->GetTextExtent ( strTemp.GetString(), Size );
            if ( fWidth < Size.cx )
			{
				int nBreakIndex = -1;

				//	문자단위로 짜를 범위안에 있다면
				for ( int size = 0; size < static_cast<int>(vecLinkPos.size()); ++size ) 
				{
					int nStart = vecLinkPos[size].nStart;
					int nEnd = vecLinkPos[size].nEnd;
					int nGap = 0;

					// 이미 한번 분리 됐던 아이템링크 처리
					// 아이템 링크가 채팅라인 2~3줄에 애매하게 걸쳐있을때 링크가 안되는 문제가 있어 추가함.
					if( true == bSplit )
					{
						nStart -= nPrevLineLength;
						nEnd -= nPrevLineLength;
					}

					if ( i >= nStart && i <= nEnd )
					{
						nBreakIndex = nStart;
						break;
					}
				}

				if ( nBreakIndex > 0 ) 
				{
					int nStrLength = strTemp.GetLength ();
					int nWordSize = nStrLength - nBreakIndex;
					i-= nWordSize;
					strTemp = strTemp.Left ( nBreakIndex );				

					if (strTemp.GetLength() < 1)
					{
						sc::writeLogError(
                            sc::string::format(
                                "텍스트 출력창이 너무 작게 설정되어 있습니다.[%1%]", strOrigin.GetString()));
						return;
					}
				}
				else
				{
					//	오직 영문자인가만 확인한다.
					if ( bIsENGLISH )
					{
						int nIndex = strTemp.ReverseFind ( cSPACE );
						//	처음까지 가도 공백을 찾을 수 없는 경우
						//	또는 첫번째 문자가 공백인 경우
						if ( (vecLinkPos.size() == 0) && nIndex > 0 )
						{
							int nStrLength = strTemp.GetLength ();
							int nWordSize = nStrLength - nIndex;
							i-= nWordSize;
							strTemp = strTemp.Left ( nIndex );
						}
						else
						{
							if ( bDBCS )
							{
								GASSERT ( 0 && "들어온다.1" );
								i-=2;
								strTemp = strTemp.Left ( strTemp.GetLength() - 2 );
							}
							else
							{
								i--;
								strTemp = strTemp.Left ( strTemp.GetLength() - 1 );
							}
						}
					}
					else
					{
						//	영문자를 제외한 나머지.. ( 숫자, DBCS등 처리 )
						if ( bDBCS )
						{
							i-=2;
							strTemp = strTemp.Left ( strTemp.GetLength() - 2 );
						}
						else
						{
							i--;
							strTemp = strTemp.Left ( strTemp.GetLength() - 1 );
						}
					}

					if (strTemp.GetLength () < 1)
					{
						sc::writeLogError(
                            sc::string::format(
                                "텍스트 출력창이 너무 작게 설정되어 있습니다.[%1%]", strOrigin.GetString()));
						return;
					}
				}

				NS_UITEXTUTIL::SSTRING	sSTRING;
				sSTRING.strLine = strTemp;
				sSTRING.bLASTLINE = false;
				MLText.push_back ( sSTRING );
				strTemp.Empty ();
			}
		}

		//	남아있는 문자들 처리
		NS_UITEXTUTIL::SSTRING	sSTRING;
		sSTRING.strLine = strTemp;
		sSTRING.bLASTLINE = true;
		MLText.push_back ( sSTRING );
	}
	else
	{
		strTemp = strOrigin;
		NS_UITEXTUTIL::SSTRING	sSTRING;
		sSTRING.strLine = strTemp;
		sSTRING.bLASTLINE = true;
		MLText.push_back ( sSTRING );
	}	
}


void CBasicTextBoxExLink::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CBasicTextBoxEx::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( bFirstControl )
	{
		UpdateSelWordData ( x, y );
	}	
}

void CBasicTextBoxExLink::UpdateSelWordData ( int x, int y )
{
	m_nSelectWordData = nNOT_VISIBLE;
	m_strSelectWord = "";

	DWORD dwMsg = GetMessageEx ();	
	if ( !CHECK_MOUSE_IN ( dwMsg ) ) return ;

	const UIRECT& rcGlobalPos = GetGlobalPos ();

	int nSelectLine = GetSelectPos();
	if ( nSelectLine == nNOT_VISIBLE ) return;

	const STEXTPART& sPart = GetTextPart( nSelectLine );

	for ( int i = 0; i < static_cast<int>(sPart.dequeLine.size()); ++i ) 
	{
		const STEXTLINE& sLine = sPart.dequeLine[i];

		for ( int j = 0; j < static_cast<int>(sLine.vectorWord.size()); ++j )
		{
			const STEXTWORD& sWord = sLine.vectorWord[j];
			UIRECT rcWord;

			rcWord = UIRECT( sWord.x, sWord.y, static_cast<float>(sWord.strSize.cx), static_cast<float>(sWord.strSize.cy) );

			if ( (x >rcWord.left && x<rcWord.right) &&
				(y> rcWord.top && y<rcWord.bottom) )
			{
				m_nSelectWordData = sWord.nData;
				m_strSelectWord = sWord.strWord;
				return;
			}
		}
	}
}