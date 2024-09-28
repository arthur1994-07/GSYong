#include "StdAfx.h"
#include "../../../SigmaCore/DebugSet.h"

#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/DebugSet.h"
#include "../../../EngineLib/Common/DXInputString.h"
#include "../../../EngineLib/GUInterface/UIRenderQueue.h"
#include "../../../EngineLib/GUInterface/BasicCarrat.h"
#include "../../../EngineLib/DxResponseMan.h"
#include "../../../EngineLib/DxTools/RENDERPARAM.h"

#include "../../../RanLogic/RANPARAM.h"

#include "../../InnerInterface.h"
#include "UIEditBoxLink.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CUIEditBoxLink::CUIEditBoxLink(EngineDeviceMan* pEngineDevice)
	: CUIEditBox(pEngineDevice)
    , m_xInsertPosBack(0)
{
}


CUIEditBoxLink::~CUIEditBoxLink()
{
	m_vecLinkTextData.clear();
}

void CUIEditBoxLink::ClearEdit()
{
	CUIEditBox::ClearEdit();
	
	m_vecLinkTextData.clear();
}

bool CUIEditBoxLink::IsChatLink()
{
	if ( m_vecLinkTextData.size() > 0 ) return true;
	
	return false;
}

bool CUIEditBoxLink::IsPartyLink( const SLINK_DATA_BASIC& sLinkDataBasic )
{
	//	��Ƽ���� ��ũ�� �ϳ��� ���� �ؾ��Ѵ�.
	//  �ٸ� ��ũ�� �ߺ��ϱ⿡�� �����;��� �ʹ� ����.

	if ( sLinkDataBasic.sLinkType.wMType == PARTY_LINK &&
		 sLinkDataBasic.sLinkType.wSType == PARTY_LINK_RECRUIT ) 
	{
		if ( m_vecLinkTextData.size() > 0 ) return true;
	}

	for ( DWORD i = 0; i < m_vecLinkTextData.size(); ++i ) 
	{
		const SLINK_DATA_BASIC& sLinkData = m_vecLinkTextData[i].sLinkDataBasic;
		
		if ( sLinkData.sLinkType.wMType == PARTY_LINK &&
			 sLinkData.sLinkType.wSType == PARTY_LINK_RECRUIT  ) 
		{
			 return true;
		}

	}

	return false;

}

CStringW CUIEditBoxLink::MutiToWide(const CString& strText)
{
	if ( !m_pFont )
		return L"";

	int iLen = strText.GetLength();

	WCHAR wTemp[ 1024 ];
	m_pFont->MultiToWide( strText.GetString(), iLen, wTemp, 1022 );
	CStringW strTempW = wTemp;

	return strTempW;
}

void CUIEditBoxLink::ChatToLink( const SLINK_DATA_BASIC& sLinkDataBasic, const CString& strLinkName, DWORD dwColor, const SITEMCUSTOM *pItemCustom )
{
	if ( !m_pFont )
		return;

	if ( !sLinkDataBasic.VALID() ) 
		return;

	// �ִ�� �ɼ� �ִ� ��ũ�� �� �ɾ���.
	if ( m_vecLinkTextData.size() >= CHAT_LINK_MAX ) return;

	// ��Ƽ���� ��ũ�� �����ͷ� ���� �Ѱ��ۿ� �� �Ǵ�.
	if ( IsPartyLink ( sLinkDataBasic ) ) return;

	//	�Է� �������� Ȯ�� ( ��ü Edit ���� ��� ) 
	int nSize = strLinkName.GetLength();	
	if ( GetEditLength() + nSize > GetLimitInput() ) 
	{
		return;
	}

	//	���� ���� ���
	SIZE nTextSize;
	m_pFont->GetTextExtent( strLinkName, nTextSize );

	//if (strLinkName.GetLength() + 16 > static_cast<int>(GetLocalPos().sizeX))
	//	return;

	CStringW strTempW = MutiToWide( m_strText );

	DWORD nIndex = static_cast<DWORD>(m_vecLinkTextData.size());

	SLINK_TEXT_DATA_EX sLinkTextData;
	sLinkTextData.sLinkDataBasic = sLinkDataBasic;
	sLinkTextData.sLinkPos.nStart = WideToMultiPos( &strTempW, m_xInsertPosR );
	sLinkTextData.sLinkPos.nEnd = sLinkTextData.sLinkPos.nStart + strLinkName.GetLength();
	sLinkTextData.dwTextColor = dwColor;

	if (pItemCustom)
	{
		sLinkTextData.sItemcustom = *pItemCustom;
	}



	////�׷�ä�� ������ ��ũ ���� ����
	//int nSizeEditbox= 0;
	//for (int i = 0; i < m_vecLinkTextData.size(); i++)
	//{
	//	
	//	nSizeEditbox = m_vecLinkTextData[i].sLinkPos.nEnd;
	//	int aa = GetLocalPos().sizeX;

	//	if ( nSizeEditbox*8 > static_cast<int>(GetLocalPos().sizeX))
	//		return;
	//}

	m_vecLinkTextData.push_back( sLinkTextData );

	CString strBack = m_strText;

	//	���� �߰�
    InsertString( strLinkName );
	
	CalcLinkTextAdd ( nIndex );
}

void CUIEditBoxLink::CalcLinkTextAdd( int nAdd )
{
	if ( nAdd < 0 ) return;
	if ( nAdd != m_vecLinkTextData.size()-1 ) return;

	int nDiffText = m_vecLinkTextData[nAdd].sLinkPos.nEnd - m_vecLinkTextData[nAdd].sLinkPos.nStart;
	for ( DWORD i = 0; i < m_vecLinkTextData.size()-1; ++i ) 
	{		
		if ( m_vecLinkTextData[nAdd].sLinkPos.nStart <= m_vecLinkTextData[i].sLinkPos.nStart )
		{		
			CString strDiffText = m_strText.Mid( m_vecLinkTextData[nAdd].sLinkPos.nStart, nDiffText );

			m_vecLinkTextData[i].sLinkPos.nStart += nDiffText;
			m_vecLinkTextData[i].sLinkPos.nEnd += nDiffText;
		}
	}

	std::sort( m_vecLinkTextData.begin(), m_vecLinkTextData.end() );

}

void CUIEditBoxLink::CalcLinkText()
{
    DXInputString* pInputString = m_pEngineDevice->GetInputString();
	CString strInput(pInputString->GetString());

	if (m_strText == strInput)
        return;

	int nSize = strInput.GetLength();
	int nSize2 = m_strText.GetLength();

	// ���� Ŀ���� �����ͼ� �߰��� ���� Ȯ���ϰ� 
	// ���� Ŀ���� �����ͼ� ������ ���� Ȯ������

	//	���ڰ� �߰��Ǿ��� ���
	if ( nSize > nSize2 ) 
	{
		int nDiffText = nSize - nSize2;

		//	���� Ŀ����ġ�� �����Ǿ����Ƿ� 
		//	���� Ŀ������ �ٲ� ��ġ��ŭ ���ڸ� �����´�.
		CStringW strTempW = MutiToWide( m_strText );
		int nPos = WideToMultiPos( &strTempW, m_xInsertPos );
		
		CString strDiffText = strInput.Mid( nPos, nDiffText );	
		
		for ( DWORD i = 0; i < m_vecLinkTextData.size(); ++i ) 
		{		
			if ( m_vecLinkTextData[i].sLinkPos.nStart < nPos ) continue;

			m_vecLinkTextData[i].sLinkPos.nStart += nDiffText;
			m_vecLinkTextData[i].sLinkPos.nEnd += nDiffText;
		}
	}
	//	���ڰ� �����Ǿ��� ���
	else if ( nSize2 > nSize ) 
	{
		int nDiffText = nSize2 - nSize;

		GetInsertPos();
		CStringW strTempW = MutiToWide( m_strText );
		int nPos = WideToMultiPos( &strTempW, m_xInsertPos );

		//	��ũ �׸��� �����Ȱ��
		int nDelete = IsDeleteLinkText( nPos, nDiffText );		
		
		if ( nDelete > -1 )
		{
			nDiffText = m_vecLinkTextData[nDelete].sLinkPos.nEnd - m_vecLinkTextData[nDelete].sLinkPos.nStart;
			CString strDiffText = m_strText.Mid ( m_vecLinkTextData[nDelete].sLinkPos.nStart, nDiffText );
			m_strText.Delete( m_vecLinkTextData[nDelete].sLinkPos.nStart, nDiffText );

			int xInsertPos = m_vecLinkTextData[nDelete].sLinkPos.nStart;
			
			for ( DWORD i = nDelete+1; i < m_vecLinkTextData.size(); ++i ) 
			{			
				m_vecLinkTextData[i].sLinkPos.nStart -= nDiffText;
				m_vecLinkTextData[i].sLinkPos.nEnd -= nDiffText;
			}

			m_vecLinkTextData.erase( m_vecLinkTextData.begin()+nDelete );
			std::sort( m_vecLinkTextData.begin(), m_vecLinkTextData.end() );

			SET_STRING_BUFFER ( m_strText );

			m_xInsertPos = MultiToWidePos( &m_strText, xInsertPos );
			SetInsertPos();

		}
		//	��ġ�� �̵��� ���
		else
		{
			CString strDiffText = m_strText.Mid( nPos, nDiffText);
			
			for ( DWORD i = 0; i < m_vecLinkTextData.size(); ++i ) 
			{		
				if ( m_vecLinkTextData[i].sLinkPos.nStart < nPos ) continue;

				m_vecLinkTextData[i].sLinkPos.nStart -= nDiffText;
				m_vecLinkTextData[i].sLinkPos.nEnd -= nDiffText;
			}
		}
			
	}
}

void CUIEditBoxLink::GetInsertPos()
{
	CUIEditBox::GetInsertPos();
	m_xInsertPosBack = m_xInsertPos;
}

void CUIEditBoxLink::SetInsertPos()
{
	CUIEditBox::SetInsertPos();

	m_xInsertPosBack = m_xInsertPos;
}

void CUIEditBoxLink::SetRenderText(const CString& strRenderText)
{
	m_strRenderText = strRenderText;
}

int	CUIEditBoxLink::IsDeleteLinkText( int nPos, int nDiffText )
{
	//	��ũ �׸��� �����Ǿ����� Ȯ��	
	for ( DWORD i = 0; i < m_vecLinkTextData.size(); ++i ) 
	{		
		if ( nPos >= m_vecLinkTextData[i].sLinkPos.nStart && 
			(nPos + nDiffText) <= m_vecLinkTextData[i].sLinkPos.nEnd )
		{
			return i;
		}
	}

	return -1;
}

void CUIEditBoxLink::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if (!IsVisible() || !m_pFont || !m_pCarrat)
        return;

	CUIGroup::Update(x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl);

	for (DWORD i = 0; i < m_vecLinkTextData.size(); ++i) 
	{
		CDebugSet::ToView(6, i , "nStartPos %d, nEndPos %d", m_vecLinkTextData[i].sLinkPos.nStart, m_vecLinkTextData[i].sLinkPos.nEnd);
	}
	
	if (IsBegin()) 
	{
        DXInputString* pInputString = m_pEngineDevice->GetInputString();
		if (!pInputString->IsOn())
		{
			EndEdit ();
			return;
		}

		SetMODE_NATIVE(pInputString->IsNativeMode() ? true : false);

		switch (RENDERPARAM::emSERVICE_TYPE)
		{
		case RENDERPARAM::EMSERVICE_INDONESIA:
		case RENDERPARAM::EMSERVICE_MALAYSIA_EN:
		case RENDERPARAM::EMSERVICE_PHILIPPINES:
//		case EMSERVICE_VIETNAM:
			if( IsMODE_NATIVE() )
			{
				pInputString->ConverseNativeToEng();
			}
			break;
		};

		if (!m_bKeyInput)
			pInputString->ConverseFullToHalf();

		CString strInput = pInputString->GetString();
		int xInsertPos = pInputString->GetInsertPos();		

		// 	�����Է� �̿ܿ� ������� Ű�� ������ ���, ��� ����
		m_bVALID_INPUT = true;
		if( !m_bMinusSign && ( strInput[0] == '-' ) ) m_bVALID_INPUT = false;
		if( DxInputDevice::GetInstance().GetKeyState ( DIK_LCONTROL ) & DXKEY_DOWNED ) m_bVALID_INPUT = false;
		if( DxInputDevice::GetInstance().GetKeyState ( DIK_LMENU ) & DXKEY_DOWNED ) m_bVALID_INPUT = false;
		if( DxInputDevice::GetInstance().GetKeyState ( DIK_RCONTROL ) & DXKEY_DOWNED ) m_bVALID_INPUT = false;
		if( DxInputDevice::GetInstance().GetKeyState ( DIK_RMENU ) & DXKEY_DOWNED ) m_bVALID_INPUT = false;
		if( !m_bVALID_INPUT && m_strText != strInput )
		{
			SET_STRING_BUFFER ( m_strText );
			SetInsertPos();
			return;
		}

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

		// ���ڰ� ����Ǿ����� LinkText ��ġ�� �����Ѵ�.
		if ( IsChatLink() )	
		{	
			CalcLinkText();		
			strInput = pInputString->GetString();
			xInsertPos = pInputString->GetInsertPos();
		}
		

		// �Է� ���� ���ڰ� ��ȿ�ϹǷ� m_xInsertPos �����Ѵ�
		m_xInsertPos = xInsertPos;
		m_xInsertPosR = m_xInsertPos;

		CDebugSet::ToView ( 6, 4, "xInsertPos : %d", xInsertPos );
		
		m_strText = strInput;
		SetRenderText ( m_strText );
		UpdateInsertPos();
	}	

	UpdateRenderText();
	UpdatePosition();

	// �ؽ��Ŀ� ���ڸ� �������ϴ� ������� ����
	m_pFont->PushText( (TCHAR*)m_strRenderTextR.GetString() );	
}

HRESULT CUIEditBoxLink::Render ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;

	if ( !m_pFont )
		return hr;

	hr = CUIGroup::Render ( pd3dDevice );
	if ( FAILED ( hr ) ) return hr;

	CUIRenderQueue::Get()->RenderImmediately();

	//	��ũ �׸��� ����� ȭ�鿡 �Ѹ���.
	{

		CStringW strTempW = MutiToWide( m_strText );
		int nViewPos = WideToMultiPos( &strTempW, m_xViewPos );
		int nSize = m_strRenderTextR.GetLength();
		int nViewPosEnd = nViewPos + nSize;


		
		D3DXVECTOR2 vRenderPos = m_vRenderPos;
		CString strText, strRenderTextL;
		SIZE TextSize;
		
		// �Ϲ� ���ڵ��� �ѷ��ش�.
		int nStart = nViewPos;
		int nEnd = nViewPosEnd;

		if ( m_vecLinkTextData.size() ) 
		{
			if ( nViewPosEnd < m_vecLinkTextData[0].sLinkPos.nStart ) 
				nEnd = nViewPosEnd;
			else nEnd = m_vecLinkTextData[0].sLinkPos.nStart;
		}
		
		if ( nStart < nEnd ) 
		{		
			CString strText = m_strRenderTextR.Mid( nStart-nViewPos, nEnd-nStart );
				
			hr = m_pFont->DrawText( vRenderPos.x, vRenderPos.y, m_dwTextColor, strText.GetString(), 0L, FALSE );	
			if ( FAILED ( hr ) ) return hr;
		}

		// �Ϲ� ���ڸ� �ѷ��ش�.
		DWORD index = 0;
		
		do 
		{
			nStart = nViewPos;
			nEnd = nViewPosEnd;
			
			if ( m_vecLinkTextData.size() > index ) 
			{
				if ( nViewPosEnd < m_vecLinkTextData[index].sLinkPos.nStart ) 
					nEnd = nViewPosEnd;
				else nEnd = m_vecLinkTextData[index].sLinkPos.nStart;
			}

			if ( index > 0 && nViewPos < m_vecLinkTextData[index-1].sLinkPos.nEnd ) 
			{
				nStart = m_vecLinkTextData[index-1].sLinkPos.nEnd;
			}

			if ( nStart < nEnd ) 
			{
				strRenderTextL = m_strRenderTextR.Mid ( 0, nStart - nViewPos );
				m_pFont->GetTextExtent ( strRenderTextL.GetString(), TextSize );		
				vRenderPos.x = m_vRenderPos.x + TextSize.cx;
				vRenderPos.y = m_vRenderPos.y;			
				
				CString strText = m_strRenderTextR.Mid( nStart-nViewPos, nEnd-nStart );
					
				hr = m_pFont->DrawText( vRenderPos.x, vRenderPos.y, m_dwTextColor, strText.GetString(), 0L, FALSE );	
				if ( FAILED ( hr ) ) return hr;		
			}

			index++;
		}
		while ( index <= m_vecLinkTextData.size() );

        
		//	��ũ �׸��� �ѷ��ش�.
		for ( DWORD i = 0; i < m_vecLinkTextData.size(); ++i ) 
		{			
			nStart = m_vecLinkTextData[i].sLinkPos.nStart;
			nEnd = m_vecLinkTextData[i].sLinkPos.nEnd;

			if ( nStart < nViewPos )	nStart = nViewPos;
			if ( nEnd > nViewPosEnd ) nEnd = nViewPosEnd;
			if ( nStart < nEnd ) 
			{
				strRenderTextL = m_strRenderTextR.Mid ( 0, nStart - nViewPos );
				m_pFont->GetTextExtent ( strRenderTextL.GetString(), TextSize );		
				vRenderPos.x = m_vRenderPos.x + TextSize.cx;
				vRenderPos.y = m_vRenderPos.y;			
				
				CString strText = m_strRenderTextR.Mid( nStart-nViewPos, nEnd-nStart );
					
				hr = m_pFont->DrawText( vRenderPos.x, vRenderPos.y, m_vecLinkTextData[i].dwTextColor, strText.GetString(), 0L, FALSE );	
				if ( FAILED ( hr ) ) return hr;		
			}
		}
	}


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

		m_pFont->DrawText( vRenderPos.x, vRenderPos.y, NS_UITEXTUTIL::BLACK , strRenderText.GetString(), 0L, FALSE );	
	}

	return S_OK;
}

void CUIEditBoxLink::UpdateInsertPos()
{
	//	Ŀ���� ��ũ ������ ���� ���̿� �̵��� ���´�.

	if ( m_xInsertPosBack != m_xInsertPosR )
	{

		CStringW strTempW = MutiToWide( m_strText );
		int nInsertPos = WideToMultiPos( &strTempW, m_xInsertPosR );
		int nInsertPosBack =  WideToMultiPos( &strTempW, m_xInsertPosBack );		

		for ( DWORD i = 0; i < m_vecLinkTextData.size(); ++i ) 
		{
			if ( m_vecLinkTextData[i].sLinkPos.nStart < nInsertPos && nInsertPos < m_vecLinkTextData[i].sLinkPos.nEnd ) 
			{
				//	�������� �°Ÿ� �������� �������� �°Ÿ� ��������
				if ( nInsertPosBack < nInsertPos )	nInsertPos = m_vecLinkTextData[i].sLinkPos.nEnd;
				else nInsertPos = m_vecLinkTextData[i].sLinkPos.nStart;

				m_xInsertPos = MultiToWidePos( &m_strText, nInsertPos );
				SetInsertPos();
			}
		}
		
		m_xInsertPosBack = m_xInsertPosR;
	}
}


void CUIEditBoxLink::UpdateRenderText()
{
	m_strRenderTextR = m_strRenderText;

	if( DXInputString::m_bCaratMove == FALSE )
		return;

	if ( !m_pFont )
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
		WCHAR wTemp[ 1024 ];
		m_pFont->MultiToWide( m_strRenderTextR.GetString(), iLen, wTemp, 1022 );
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

		iViewPosM = max( iViewPosM, 0 );
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

	CDebugSet::ToView ( 6, 5, "iViewPosM : %d", iViewPosM );

	// ������ ��ġ iViewPosM ( ��Ƽ ����Ʈ ) <= ����ȭ => m_xViewPos ( �����ڵ� )
	m_xViewPos = MultiToWidePos( &m_strRenderTextR, iViewPosM );

	// ĳ�� ��ġ ���
	{
		strTemp = m_strRenderTextR.Mid( iViewPosM, iInsertPosRM - iViewPosM );
		m_pFont->GetTextExtent ( strTemp.GetString(), sizeTemp );
		m_fCarratPos = (float)sizeTemp.cx;
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

void CUIEditBoxLink::UpdatePosition ()
{
	if ( !m_pFont )
		return;

	if( DXInputString::m_bCaratMove )
	{
		SIZE TextSize;
		m_pFont->GetTextExtent ( m_strRenderTextR.GetString(), TextSize );

		//	���� �Ѹ� ��ġ
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
			m_pCarrat->SetGlobalPos ( rcCarratPos );
		}
	}
	else
	{
		const long lEditWidth = (long)(GetGlobalPos().sizeX - fLINE_END_GAP);

		//	���ڿ��� ����Ʈ�ڽ��� �Ѿ ��� �պκ� �߶󳻱�
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

		//	���� �Ѹ� ��ġ
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
			m_pCarrat->SetGlobalPos ( rcCarratPos );
		}
	}

	{ // �ѱ���� ���� �����϶� ĳ���� Ȯ���Ѵ�.
		UIRECT rcCaretOldPos = m_pCarrat->GetGlobalPos();
		UIRECT rcCaretNewPos(rcCaretOldPos);

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
