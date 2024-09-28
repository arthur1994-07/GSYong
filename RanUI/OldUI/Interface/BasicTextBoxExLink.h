#pragma once

#include "../../../EngineLib/GUInterface/BasicTextBoxEx.h"
#include "../../../RanLogic/GLChatLinkDef.h"


class CBasicTextBoxExLink : public CBasicTextBoxEx
{
public:
	CBasicTextBoxExLink(EngineDeviceMan* pEngineDevice);
	virtual ~CBasicTextBoxExLink();
public:
	void SetTextLink( const CString& strText, const VEC_LINK_TEXT_DATA& vecLinkTextData, const D3DCOLOR& TextColor );
	int	AddTextLink( const CString& strText,  const VEC_LINK_TEXT_DATA& vecLinkTextData, const D3DCOLOR& TextColor );
    virtual int	AddStringLink( int nIndex, const CString& strText,  const VEC_LINK_TEXT_DATA& vecLinkTextData, const D3DCOLOR& TextColor );

public:
	virtual	void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

protected:
	virtual void ReCalcRenderPos ();

public:
	void	UpdateSelWordData ( int x, int y );
	int		GetSelectWordData () { return m_nSelectWordData; }
	CString GetSelectWordText () { return m_strSelectWord; }

public:
	void GetMLTextWithoutCRLF ( const CString& strOrigin, float fWidth, CD3DFontPar* pFont, 
								NS_UITEXTUTIL::MULTILINETEXT& MLText, const VEC_LINK_DATA_POS& vecLinkPos, bool bSplit = false, int nPrevLineLength = 0 );
	void GetWordText( const CString &strOrigin, NS_UITEXTUTIL::STRINGVECTOR& WordText, const VEC_LINK_DATA_POS& vecLinkPos, int nStart );
	void SplitMLText( CString& strOrigin, float fWidth, CD3DFontPar* pFont, NS_UITEXTUTIL::MULTILINETEXT& MLText, const VEC_LINK_DATA_POS& vecLinkPos, bool bSplit, int nPrevLineLength );


	int IsLinkDataStart( const VEC_LINK_DATA_POS& vecLinkPos, int nStart );	// 시작지점 확인
	int IsLinkData( const VEC_LINK_DATA_POS& vecLinkPos, int nStart, int nEnd );	// 시작지점 및 끝점 확인

	int		m_nSelectWordData;
	CString m_strSelectWord;
};
