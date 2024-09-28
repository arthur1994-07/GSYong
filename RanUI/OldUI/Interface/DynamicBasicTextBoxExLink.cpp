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
        GASSERT ( 0 && "��Ʈ�� ���Դϴ�. SetFont()�� �̿��Ͽ� ��Ʈ���� ���� �����ʽÿ�." );
        return -1;
    }

    if ( nIndex < 0 || (int) m_TextBox.size () <= nIndex )
    {
        GASSERT ( 0 && "�ε����� ������ �Ѿ�ϴ�." );
        return -1;
    }

    if ( !strText.GetLength () )
    {
        GASSERT ( 0 && "���ڿ��� �����ϴ�." );
        return -1;
    }

    // ���� ���ڿ��� ��ũ ������ ©�� 
    // �׳� �ٿ� ������. ������ ��ų���̱� ������ 
    // ��ũ ������ ���� ������ ���� �Ǹ� �ȴ�.

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

    // ���� �ؽ�Ʈ �ڽ� ũ�⸦ ���� �� ��
    // SetGlobalPos() �� �̵� ��Ŵ����
    // SetGlobalPos ���� ReCalcRenderPos()�� ȣ�� ������ ����
    //ReCalcRenderPos();
}

float CDynamicBasicTextBoxExLink::ReCalcVisibleLine()
{
    //	�Ѹ� ���ڿ� ��ġ Ȯ��	
    if ( !(m_nAlign & TEXT_ALIGN_BOTTOM) )
    {
        if ( !FindStartIndex(m_nPartIndex, m_nLineIndex, m_nCurLine) )
        {
            return 0;
        }
    }

    //	������ ����
    for (int nImage = 0; nImage < (int)m_ImageList.size (); ++nImage)
    {
        CUIControl* pImage = FindControl(m_ImageList[nImage]);

        if (pImage)
        {
            pImage->SetVisibleSingle(FALSE);
        }
    }

    //	�Ѹ� �ִ� Y ������ ���
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
                // ������ ����
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

    // �������� �ؽ�Ʈ �ڽ��� ���� �Ǿ� �Ѱ� ���μ��� �Ѵ���
    // ����� ������ ���� �������� �ʴ´�. 
    // ������ ���ڿ��� �߰��� �� �����Ǿ� ����.
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
                // ����� �����Ͱ� ���� �ͳ��� ���� �ش�.
                ReSetWord(sWord, sNewWord, sTextLine);
            }

            int nWordSize = sWord.strWord.GetLength();

            // nCur �� nLimit �� ¦�� �̷� ���� ���� ������ ���۰� �� ��ġ�� ����Ų��.
            // �Ʒ� �׸����� C = nCur, L = nLimit �� �ȴ�.
            // nStart �� nEnd �� ¦�� �̷� ���� ������ ���۰� �� ��ġ�� ����Ų��.
            // �Ʒ� �׸����� S = nStart, L = nEnd �� �ȴ�.
            //  
            //                          S           E
            //                          |___________| _________
            // ��������(sWord)          |           ||         |
            //                           -----------  ---------
            //                          C               L
            //                          |---------------| -----
            // ���θ��� ����(sNewWord)  |               ||     |
            //                           ---------------  -----
            // 
            // �簢���� �ϳ��� ������ �ǹ� �Ѵ�.
            // ������ ���� 2���� �̷��� ������ ��ȯ �Ѵٰ� ������ �׸��̴�.
            // 
            // 2����(��������� ���θ��� ����) �ݵ�� ������ ���̸� ������.
            // ����� ���� ���� �׸� ���� ���� �ݴ��� ��찡 ���� �Ѵ�.
            // �������� �⺻ ������ ������ �м��ϸ� �� �Ŷ� ���� �Ѵ�.

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
    // ������ �� �ؽ�ó �Ҵ�
    const TCHAR* pText = sNewWord.strWord.GetString();
    m_pFont->GetTextExtent(pText, sNewWord.strSize);
    m_pFont->PushText(pText);

    // ���� ����
    sTextLine.vectorWord.push_back(sNewWord);

    // ���� �� �߰� ������
    sNewWord.nData   = sOrg.nData;
    sNewWord.dwColor = sOrg.dwColor;

    // �ʱ�ȭ
    sNewWord.strWord = _T("");
}

void CDynamicBasicTextBoxExLink::ReCalcRenderPos ()
{	
    const UIRECT& rcGlobalPos = GetGlobalPos ();

    float fTextHeight = ReCalcVisibleLine();

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
