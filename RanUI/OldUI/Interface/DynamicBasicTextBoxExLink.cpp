#include "StdAfx.h"
#include "DynamicBasicTextBoxExLink.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CDynamicBasicTextBoxExLink::CDynamicBasicTextBoxExLink(EngineDeviceMan* pEngineDevice) :
    CBasicTextBoxExLink(pEngineDevice)
{

}

int CDynamicBasicTextBoxExLink::AddStringLink(int nIndex, const CString& strText, const VEC_LINK_TEXT_DATA& vecLinkTextData, const D3DCOLOR& TextColor)
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

    // 현재 문자열을 링크 단위로 짤라 
    // 그냥 붙여 버린다. 재조합 시킬것이기 때문에 
    // 링크 정보와 색상 정보만 보존 되면 된다.

    VEC_LINK_DATA_POS vecLinkPos;

    for (DWORD i = 0; i < vecLinkTextData.size(); ++i) 
    {
        vecLinkPos.push_back(vecLinkTextData[i].sLinkPos);
    }

    int nStart = 0, nEnd = 0;
    NS_UITEXTUTIL::ClearWordText();
    NS_UITEXTUTIL::STRINGVECTOR WordText;
    GetWordText(strText, WordText, vecLinkPos, nStart);

    STEXTPART& sTextPart = m_TextBox[nIndex];

    BOOST_FOREACH(CString& sWord, WordText)
    {
        STEXTWORD sTextWord;
        sTextWord.strWord = sWord;
        sTextWord.dwColor = TextColor;

        nEnd += sWord.GetLength();

        int nLink = IsLinkData(vecLinkPos, nStart, nEnd);

        if (nLink >= 0)
        {
            sTextWord.dwColor = vecLinkTextData[nLink].dwTextColor;
            sTextWord.nData   = vecLinkTextData[nLink].nLinkTypeIndex;
        }

        nStart = nEnd;
        sTextPart.dequeLine.back().vectorWord.push_back(sTextWord);
    }

    ReCalcTextPart(sTextPart.dequeLine);    
    ReCalcEndLind();
    ReCalcRenderPos();

    return (int) m_TextBox.size() - 1;
}

void CDynamicBasicTextBoxExLink::SetCurLine( int nCurLine )
{
    if (nCurLine < nDEFAULT_STARTLINE)
    {
        nCurLine = nDEFAULT_STARTLINE;
    }

    if (m_nVisibleLine < m_nTotalLine)
    {
        const int nEndStartIndex = m_nTotalLine - m_nVisibleLine;

        if (nEndStartIndex < nCurLine)
        {
            nCurLine = nEndStartIndex;
        }
    }

    if (nCurLine == m_nCurLine)
    {
        return;
    }

    m_nCurLine = nCurLine;

    ReCalcVisibleLine();
    ReCalcTextPart();
    ReCalcRenderPos();
}

void CDynamicBasicTextBoxExLink::AlignSubControl(const UIRECT& rcParentOldPos, const UIRECT& rcParentNewPos)
{
    CUIGroup::AlignSubControl(rcParentOldPos, rcParentNewPos);

    ReCalcVisibleLine();
    ReCalcTextPart();

    // 보통 텍스트 박스 크기를 조정 한 후
    // SetGlobalPos() 로 이동 시킴으로
    // SetGlobalPos 에서 ReCalcRenderPos()를 호출 함으로 제거
    //ReCalcRenderPos();
}

float CDynamicBasicTextBoxExLink::ReCalcVisibleLine()
{
    //	뿌릴 문자열 위치 확인	
    if ( !(m_nAlign & TEXT_ALIGN_BOTTOM) )
    {
        if ( !FindStartIndex(m_nPartIndex, m_nLineIndex, m_nCurLine) )
        {
            return 0;
        }
    }

    //	아이콘 리셋
    for (int nImage = 0; nImage < (int)m_ImageList.size (); ++nImage)
    {
        CUIControl* pImage = FindControl(m_ImageList[nImage]);

        if (pImage)
        {
            pImage->SetVisibleSingle(FALSE);
        }
    }

    //	뿌릴 최대 Y 사이즈 계산
    float fTextHeight = 0.0f;

    if (m_nAlign & TEXT_ALIGN_BOTTOM)
    {
        CalcMaxHEIGHT(&fTextHeight, &m_nVisibleLine);

        int nEND_LINE = m_nTotalLine - m_nVisibleLine;

        if (nEND_LINE < m_nCurLine)
        {            
            m_nCurLine = nEND_LINE;
        }

        CalcMaxHEIGHT(&fTextHeight, &m_nVisibleLine, m_nPartIndex, m_nLineIndex);

        if ( !FindStartIndex (m_nPartIndex, m_nLineIndex, m_nCurLine) )
        {
            return 0;
        }
    }
    else
    {
        CalcMaxHEIGHT(&fTextHeight, &m_nVisibleLine, m_nPartIndex, m_nLineIndex);
    }

    return fTextHeight;
}

void CDynamicBasicTextBoxExLink::ReCalcTextPart()
{
    size_t nPartIndex = m_nPartIndex;
    size_t nStartLine = m_nLineIndex;
    size_t nEndLine = 0;
    size_t nPartSize = m_TextBox.size ();
    size_t nCurrentLine = 0;

    for (size_t i = nPartIndex; i < nPartSize; ++i)
    {
        CString strText;
        DEQUETEXTLINE& sDequeLines = m_TextBox[i].dequeLine;
        size_t nLineSize = sDequeLines.size();

        for (nEndLine = nStartLine; nEndLine < nLineSize; ++nEndLine)
        {
            if (nCurrentLine + nEndLine > static_cast<size_t>(m_nVisibleLine))
            {
                // 끝내기 위함
                i = nPartSize;
                break;
            }

            STEXTLINE& sTextLine = sDequeLines[nEndLine];

            BOOST_FOREACH(STEXTWORD& sTextWord, sTextLine.vectorWord)
            {
                strText += sTextWord.strWord;
            }
        }

        if (nStartLine != nEndLine)
        {
            nCurrentLine += ReCreateTextPart(sDequeLines, strText, nStartLine, nEndLine);
        }

        nStartLine = 0;
    }

    // 동적으로 텍스트 박스가 변경 되어 한계 라인수를 넘더라도
    // 사이즈를 변경할 떄는 삭제하지 않는다. 
    // 삭제는 문자열을 추가할 떄 삭제되어 진다.
}

void CDynamicBasicTextBoxExLink::ReCalcTextPart(DEQUETEXTLINE& sDequeLines)
{
    CString strText;

    BOOST_FOREACH(STEXTLINE& sTextLine, sDequeLines)
    {
        BOOST_FOREACH(STEXTWORD& sTextWord, sTextLine.vectorWord)
        {
            strText += sTextWord.strWord;
        }

        ReCreateTextPart(sDequeLines, strText, 0, sDequeLines.size());
    }
}

size_t CDynamicBasicTextBoxExLink::ReCreateTextPart(DEQUETEXTLINE& sDequeLines, CString& strText, size_t nStartLine, size_t nEndLine)
{
    NS_UITEXTUTIL::MULTILINETEXT& MLText = NS_UITEXTUTIL::GetMLTextWithoutCRLF(strText, GetLocalPos().sizeX, m_pFont);

    m_nTotalLine += MLText.size() - (nEndLine - nStartLine);

    size_t nLine = nStartLine, nWord = 0, nChar = 0, nCount = 0;

    BOOST_FOREACH(NS_UITEXTUTIL::SSTRING& sMultiLine, MLText)
    {
        SIZE strSize;
        m_pFont->GetTextExtent(sMultiLine.strLine.GetString(), strSize);

        STEXTLINE sTextLine;
        sTextLine.strSize   = strSize;
        sTextLine.bLASTLINE = sMultiLine.bLASTLINE;

        ReCreateTextWord(sDequeLines, nLine, nWord, nChar, sTextLine, sMultiLine.strLine);
        
        sDequeLines.insert(sDequeLines.begin() + nEndLine + nCount++, sTextLine);
    }

    sDequeLines.erase(sDequeLines.begin() + nStartLine, sDequeLines.begin() + nEndLine);

    return nCount;
}

void CDynamicBasicTextBoxExLink::ReCreateTextWord(DEQUETEXTLINE& dequeLine, size_t& nLine, size_t& nWord, size_t& nChar, STEXTLINE& sTextLine, CString& strLine)
{
    STEXTWORD sNewWord;
    STEXTWORD& sWord = dequeLine[nLine].vectorWord[nWord];
    sNewWord.dwColor = sWord.dwColor;
    sNewWord.nData = sWord.nData;

    int nCur = 0;
    bool bEnd = false;
    int nLimit = strLine.GetLength();

    for ( ; nLine < dequeLine.size(); ++nLine)
    {
        for ( ; nWord < dequeLine[nLine].vectorWord.size(); ++nWord)
        {
            sWord = dequeLine[nLine].vectorWord[nWord];

            if (sWord.dwColor != sNewWord.dwColor || sWord.nData != sNewWord.nData)
            {
                // 색상과 데이터가 같은 것끼리 묶어 준다.
                ReSetWord(sWord, sNewWord, sTextLine);
            }

            int nWordSize = sWord.strWord.GetLength();

            // nCur 과 nLimit 가 짝을 이뤄 새로 만들 문장의 시작과 끝 위치를 가리킨다.
            // 아래 그림에서 C = nCur, L = nLimit 가 된다.
            // nStart 와 nEnd 가 짝을 이뤄 이전 문장의 시작과 끝 위치를 가리킨다.
            // 아래 그림에서 S = nStart, L = nEnd 가 된다.
            //  
            //                          S           E
            //                          |___________| _________
            // 예전문장(sWord)          |           ||         |
            //                           -----------  ---------
            //                          C               L
            //                          |---------------| -----
            // 새로만들 문장(sNewWord)  |               ||     |
            //                           ---------------  -----
            // 
            // 사각형은 하나의 라인을 의미 한다.
            // 임의의 라인 2개로 이뤄진 문장을 변환 한다고 가정한 그림이다.
            // 
            // 2개는(예전문장과 새로만들 문장) 반드시 동일한 길이를 가진다.
            // 경우의 수는 위의 그림 같은 경우와 반대의 경우가 존재 한다.
            // 이정도의 기본 지식을 가지고 분석하면 될 거라 생각 한다.

            int nStart = nChar;
            int nEnd = nWordSize;

            int nRemain = nEnd - nStart;

            if (nCur + nRemain > nLimit)
            {
                nEnd = nStart + nLimit - nCur;
                bEnd = true;
            }

            sNewWord.strWord += sWord.strWord.Mid(nStart, nEnd - nStart);

            if (bEnd)
            {
                nChar = nEnd;
                goto END;
            }

            nCur += nRemain;
            nChar = 0;
        }

        nWord = 0;
    }

END:
    ReSetWord(sWord, sNewWord, sTextLine);
}

void CDynamicBasicTextBoxExLink::ReSetWord(STEXTWORD& sOrg, STEXTWORD& sNewWord, STEXTLINE& sTextLine)
{
    // 사이즈 및 텍스처 할당
    const TCHAR* pText = sNewWord.strWord.GetString();
    m_pFont->GetTextExtent(pText, sNewWord.strSize);
    m_pFont->PushText(pText);

    // 최종 삽입
    sTextLine.vectorWord.push_back(sNewWord);

    // 색상 및 추가 데이터
    sNewWord.nData   = sOrg.nData;
    sNewWord.dwColor = sOrg.dwColor;

    // 초기화
    sNewWord.strWord = _T("");
}

void CDynamicBasicTextBoxExLink::ReCalcRenderPos ()
{	
    const UIRECT& rcGlobalPos = GetGlobalPos ();

    float fTextHeight = ReCalcVisibleLine();

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
                    fPosX += BLANKSIZE.cx;

                    //	NOTE
                    //		실질적으로 CR/LF를 만나 개행되려고 하는가?
                    //		bLASTLINE이 true인 경우, 문장이 박스에 꽉차지 않고
                    //		종료된다는 것을 의미한다.
                    if ( !sTextLine.bLASTLINE )
                    {
                        if ( 0 < lGAP )
                        {
                            //	만약, 마지막 줄이면 '갭'을 다 소비한다.
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

        if ( i != (nPartSize-1) ) fTEXT_POS_Y += GetPartInterval ();	//	파트 인터벌

        rcPart = UIRECT ( vBEGIN.x, vBEGIN.y, vSIZE.x, vSIZE.y );

        nLineIndex = 0;
    }
}
