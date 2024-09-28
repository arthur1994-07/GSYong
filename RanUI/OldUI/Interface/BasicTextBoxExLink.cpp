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
		GASSERT ( 0 && "��Ʈ�� ���Դϴ�. SetFont()�� �̿��Ͽ� ��Ʈ���� ���� �����ʽÿ�." );
		return 0;
	}

	VEC_LINK_DATA_POS vecLinkPos;

	for ( DWORD i = 0; i < vecLinkTextData.size(); ++i ) 
	{
		vecLinkPos.push_back( vecLinkTextData[i].sLinkPos );
	}

	//	���ڿ� �ڸ���
	NS_UITEXTUTIL::MULTILINETEXT MLText;
	GetMLTextWithoutCRLF( strText, GetLocalPos().sizeX, m_pFont, MLText, vecLinkPos );

	int nSplitLine = (int)MLText.size ();
	
	STEXTPART sTextPart;

	//	����Ÿ�� ����ֱ�
	int nStart = 0;
	for ( int i = 0; i < nSplitLine; ++i )
	{		
		STEXTLINE sTextLine;

		if ( (i - 1 == nSplitLine) && !MLText[i].strLine.GetLength () )
		{
			//	������������ �� ���.
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

	//	���� ���� ������ ���� ���, �߶�
	ReCalcEndLind();
	
	ReCalcRenderPos ();

	return (int)m_TextBox.size () - 1;
}

int CBasicTextBoxExLink::AddStringLink( int nIndex, const CString& strText, const VEC_LINK_TEXT_DATA& vecLinkTextData, const D3DCOLOR& TextColor )
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
    //            std::string ErrMsg = sc::string::format("Parent Control Name = '%s'�� \"%s\"�� 'AddString'�� �� �����ϴ�.", strParent.c_str(), strText.GetString());
				//AfxMessageBox(ErrMsg.c_str(), MB_OK);
				//return -1;
    //        }

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
        //	������ ���ڿ��� ���� ��ŭ�� �ڸ���
        NS_UITEXTUTIL::MULTILINETEXT MLText;
        GetMLTextWithoutCRLF( strTextCopy, GetLocalPos().sizeX, m_pFont, MLText, vecLinkPos, true, nPrevLineLength );

        int nSplitLine = (int)MLText.size ();

        //	����Ÿ�� ����ֱ�
        for ( int i = 0; i < nSplitLine; ++i )
        {
            STEXTLINE sTextLine;

            SIZE strSize;
            m_pFont->GetTextExtent( MLText[i].strLine.GetString(), strSize );

            sTextLine.strSize = strSize;
            sTextLine.bLASTLINE = MLText[i].bLASTLINE;

            if ( (i - 1 == nSplitLine) && !MLText[i].strLine.GetLength () )
            {
                //	������������ �� ���.
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

                    //	������ word�� �� �ǹ̸� ������ ���Ѵ�.
                    //	���� �׻� 1���� �����Ѵٰ� �����ϰ�, 0�� �ε����� ����Ѵٰ�
                    //	����Ѵ�.
                    sTextLine.vectorWord.push_back( sTextWord );

                    nStart += WordText[k].GetLength();
                }

                sTextPart.dequeLine.push_back( sTextLine );

                m_nTotalLine++;

                //	���� ������ ���ǰ�, �ɸ��� ���
                //			if ( m_nLimitLine && m_nLimitLine == i )	break;
            }
        }	
    }

    //	m_TextBox.push_back ( sTextString );

	//	���� ���� ������ ���� ���, �߶�
	ReCalcEndLind();

    ReCalcRenderPos();

    return (int) m_TextBox.size() - 1;
}

void CBasicTextBoxExLink::ReCalcRenderPos ()
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
//					fPosX += BLANKSIZE.cx;

					//	NOTE
					//		���������� CR/LF�� ���� ����Ƿ��� �ϴ°�?
					//		bLASTLINE�� true�� ���, ������ �ڽ��� ������ �ʰ�
					//		����ȴٴ� ���� �ǹ��Ѵ�.
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

		if ( i != (nPartSize-1) ) fTEXT_POS_Y += GetPartInterval ();	//	��Ʈ ���͹�

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


			//	�ѱ��ΰ�?
			if ( NS_UITEXTUTIL::IsDBCS ( strOrigin[i] ) )
			{
				bDBCS = TRUE;
			}
		}

		//	����
		{
			strTemp += strOrigin[i];
			i++;
		}

		//	�ѱ��̸� �ϳ� �� ó��...
		if ( bDBCS )
		{				
			strTemp += strOrigin[i];
			i++;
		}
	}

	if ( strTemp.GetLength() && strTemp[strTemp.GetLength()-1] == ' ' )
	{
		// ���� �����ϴ� �κ��ε� ��� ������ ������ ��ũ�� ���̿� ������ ������� �ؽ�Ʈ�� ������ ������ �־� �ּ�ó��.
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
		//	�����ִ� ���ڵ� ó��
		WordText.push_back ( strTemp );
	}
}


void CBasicTextBoxExLink::SplitMLText( CString& strOrigin, float fWidth, CD3DFontPar* pFont,
									   NS_UITEXTUTIL::MULTILINETEXT& MLText, const VEC_LINK_DATA_POS& vecLinkPos, bool bSplit, int nPrevLineLength )
{
	
	static const char cSPACE = ' ';

	if ( !pFont )
	{
		GASSERT ( pFont && "��Ʈ�� �������� �ʾҽ��ϴ�." );
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
				//	�ѱ��ΰ�?
				if ( NS_UITEXTUTIL::IsDBCS ( strOrigin[i] ) )
				{
					bDBCS = TRUE;
				}
			}

			bool bIsENGLISH = false;
			//	�ѱ��̸� �ϳ� �� ó��...
			if ( bDBCS )
			{				
				strTemp += strOrigin[i++];
				strTemp += strOrigin[i++];
			}
			//	����
			else
			{
				char c = strOrigin[i];
				if ( isalpha ( c ) ) bIsENGLISH = true;
				strTemp += strOrigin[i];
				i++;
			}

			//	���ڿ��� ������ �Ѿ�� �¿� ����ϰ�,
			//	�� �������� ��� �����Ѵ�.
			pFont->GetTextExtent ( strTemp.GetString(), Size );
            if ( fWidth < Size.cx )
			{
				int nBreakIndex = -1;

				//	���ڴ����� ¥�� �����ȿ� �ִٸ�
				for ( int size = 0; size < static_cast<int>(vecLinkPos.size()); ++size ) 
				{
					int nStart = vecLinkPos[size].nStart;
					int nEnd = vecLinkPos[size].nEnd;
					int nGap = 0;

					// �̹� �ѹ� �и� �ƴ� �����۸�ũ ó��
					// ������ ��ũ�� ä�ö��� 2~3�ٿ� �ָ��ϰ� ���������� ��ũ�� �ȵǴ� ������ �־� �߰���.
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
                                "�ؽ�Ʈ ���â�� �ʹ� �۰� �����Ǿ� �ֽ��ϴ�.[%1%]", strOrigin.GetString()));
						return;
					}
				}
				else
				{
					//	���� �������ΰ��� Ȯ���Ѵ�.
					if ( bIsENGLISH )
					{
						int nIndex = strTemp.ReverseFind ( cSPACE );
						//	ó������ ���� ������ ã�� �� ���� ���
						//	�Ǵ� ù��° ���ڰ� ������ ���
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
								GASSERT ( 0 && "���´�.1" );
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
						//	�����ڸ� ������ ������.. ( ����, DBCS�� ó�� )
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
                                "�ؽ�Ʈ ���â�� �ʹ� �۰� �����Ǿ� �ֽ��ϴ�.[%1%]", strOrigin.GetString()));
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

		//	�����ִ� ���ڵ� ó��
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