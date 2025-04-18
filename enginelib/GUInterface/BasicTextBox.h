//	BasicTextBox 클래스
//
//	최초 작성자 : 성기엽
//	이후 수정자 :
//	로그
//		[2004.02.02]
//			@ 필요 없는 기능 삭제
//			@ 의미별 기본 기능 강화
//			@ 텍스트박스 전체영역 계산 오류 수정
//		[2003.11.20]
//			@ 문단 정렬 추가
//		[2003.11.19]
//			@ 최초제작

#pragma	once

#include "UIGroup.h"
#include "UITextUtil.h"
#include <deque>

//	문단 정렬
const int TEXT_ALIGN_LEFT		= 0x01;
const int TEXT_ALIGN_CENTER_X	= 0x02;
const int TEXT_ALIGN_RIGHT		= 0x04;
const int TEXT_ALIGN_TOP		= 0x08;
const int TEXT_ALIGN_CENTER_Y	= 0x10;
const int TEXT_ALIGN_BOTTOM		= 0x20;
const int TEXT_ALIGN_BOTH_X		= 0x40;

class	CD3DFontPar;
class	CBasicTextBox : public CUIGroup
{
private:
// 	enum
// 	{
// 		BASE_IMAGE_CONTROLID = 10000,
// 		BASE_IMAGE_CONTROLID_END = BASE_IMAGE_CONTROLID + 1000,
// 	};

public:
static	const	float	fDEFAULT_LINE_INTERVAL;
static	const	float	fDEFAULT_PART_INTERVAL;

protected:
static	const	int		nDEFAULT_STARTLINE;
static	const	int		nDEFAULT_VISIBLE_LINE;
static	const	int		nDEFAULT_LINE_LIMIT;

static	const	UIGUID	nID_IMAGELIST;

protected:
	SIZE	BLANKSIZE;

public:
	struct	STEXTWORD	//	단어 ( 색상 조절 가능 )
	{
		float		x;			//	뿌릴 위치
		float		y;		
		CString		strWord;	//	뿌릴 내용
		D3DCOLOR	dwColor;	//	색상
		SIZE		strSize;	//	사이즈
		int			nData;		//	데이터 링크

		STEXTWORD()
			: x( 0.0f )
			, y( 0.0f )
			, dwColor ( UINT_MAX )
			, nData ( -1 )
		{
			strSize.cx = 0;
			strSize.cy = 0;
		}
	};
	typedef	std::vector<STEXTWORD>	VECTORTEXTWORD;

	struct	STEXTLINE	//	한 줄 ( 의미 없음, 단지 줄 단위로 자르기 위함 )
	{
		VECTORTEXTWORD	vectorWord;	
		SIZE			strSize;		//	사이즈
		bool			bLASTLINE;

		STEXTLINE()
			: bLASTLINE( false )
		{
			strSize.cx = 0;
			strSize.cy = 0;
		}
	};
	typedef	std::deque<STEXTLINE>	DEQUETEXTLINE;
	typedef	DEQUETEXTLINE::iterator	DEQUETEXTLINE_ITER;

	struct	STEXTPART	//	AddText, SetText가 한 번 호출된 단위
	{
		DEQUETEXTLINE	dequeLine;
		
		BOOL			m_bUseTextColor;
		D3DCOLOR		m_dwColor;
		DWORD_PTR		m_dwData;		//	데이타
		DWORD_PTR		m_dwDataEx;		//	확장 데이타		
		CUIControl*		m_pImage;
		UIRECT			m_rcPart;

		STEXTPART()
			: m_bUseTextColor( FALSE )
			, m_dwColor( UINT_MAX )
			, m_dwData( UINT_MAX )
			, m_dwDataEx( UINT_MAX )
			, m_pImage( NULL )
		{
		}
	};

public:
	typedef std::deque<STEXTPART>	STEXTBOX;
	typedef STEXTBOX::iterator			STEXTBOX_ITER;	
	typedef std::deque<UIGUID>			SUIGUID;
	typedef SUIGUID::iterator				SUIGUID_ITER;	
public:
	CBasicTextBox (EngineDeviceMan* pEngineDevice);
	virtual	~CBasicTextBox ();

public:
	virtual HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );

protected:
	virtual HRESULT	RenderText ();
	virtual HRESULT RenderPart ( DEQUETEXTLINE& dequeTextLine );
	virtual HRESULT RenderLine ( VECTORTEXTWORD& vectorTextWord );
	virtual HRESULT	RenderWord ( STEXTWORD& sTextWord );

public:
	virtual void SetGlobalPos ( const UIRECT& rcPos );
	virtual void SetGlobalPos ( const D3DXVECTOR2& vPos );
	virtual	void AlignSubControl ( const UIRECT& rcParentOldPos, const UIRECT& rcParentNewPos );

public:	
	virtual void SetFont ( CD3DFontPar* pFont );
	void SetTextAlign ( int nAlign );
	void SetLineInterval ( const float& fLineInterval );
	void SetPartInterval ( const float& fPartInterval );

public:
	const STEXTPART& GetTextPart ( const int& nIndex );

protected:
	virtual void ReCalcRenderPos ();

public:
	virtual	void ClearText ();

public:	//	삭제
	void SetOneLineText ( const CString& strText, const D3DCOLOR& TextColor = NS_UITEXTUTIL::ENABLE );

public:
	int AddTextNoSplit ( const CString& strText, const D3DCOLOR& TextColor = NS_UITEXTUTIL::ENABLE );
	int SetTextNoSplit ( const CString& strText, const D3DCOLOR& TextColor = NS_UITEXTUTIL::ENABLE );

public:	
	// 영단어를 뭉쳐서 잘라내는 기능을 없애기 위해 함수 확장;
	void SetText( const CString& strText, const D3DCOLOR& TextColor = NS_UITEXTUTIL::ENABLE, bool bCutWord = true );	// Text 세팅;
	int	AddText( const CString& strText, const D3DCOLOR& TextColor = NS_UITEXTUTIL::ENABLE, bool bCutWord = true );		// Text 다음라인에 추가;
	int	AddOneLineText( const CString& strText, bool bUseEllipsis = TRUE, const D3DCOLOR& TextColor = NS_UITEXTUTIL::ENABLE );
	int AddTextWithCRLF( const CString& strText, const D3DCOLOR& TextColor = NS_UITEXTUTIL::ENABLE );
	int	AddString( int nIndex, const CString& strText, const D3DCOLOR& TextColor = NS_UITEXTUTIL::ENABLE );
	void ReplaceWord( int nIndex, const CString& strTypeField, const CString& strWord, const D3DCOLOR& TextColor = NS_UITEXTUTIL::ENABLE );
	void ReplaceText( int nIndex, const CString& strTypeField, const CString& strText, const D3DCOLOR& TextColor = NS_UITEXTUTIL::ENABLE );
	void SetTextToLimit( const CString& strText, bool bUseEllipsis = FALSE, const D3DCOLOR& TextColor = NS_UITEXTUTIL::ENABLE );
	int	AddStringToLimit( int nIndex, const CString& strText, const D3DCOLOR& TextColor = NS_UITEXTUTIL::ENABLE );

//public:
	//void PopFrontText()								{ m_TextBox.pop_front (); }
	//void PopFrontTextLine( int nIndex );

public:
	void SetOverColor( int nIndex, D3DCOLOR TextColor );
	D3DCOLOR GetOverColor( int nIndex )				{ return m_dwOverColor; }

	void SetUseOverColor( BOOL bUseOverColor )		{ m_bUseOverColor = bUseOverColor; }
	BOOL IsUseOverColor()							{ return m_bUseOverColor; }

	void SetTextImage( int nIndex, const CString& strKeyword, int nXpos = 0 );
	CUIControl*	GetTextImage( int nIndex )			{ return NULL; }

public:
	void SetTextColor ( int nIndex, D3DCOLOR dwTextColor );
	D3DCOLOR GetTextColor ( int nIndex );

	void SetUseTextColor ( int nIndex, BOOL bUseTextColor );
	BOOL IsUseTextColor ( int nIndex );

public:
	bool GetPartInfo ( const int nIndex, UIRECT& rcPart );

public:
    virtual void SetDiffuseAlpha ( WORD wAlpha );

public:
	// 리턴값 원형 그대로 사용하자. 참조로 받고 그걸 사용하면 문제될 수 있다.
	CString	GetText ( int nIndex );

	void SetTextData ( int nIndex, DWORD dwData );
	DWORD GetTextData ( int nIndex );

	void SetTextDataEx ( int nIndex, DWORD dwData );
	DWORD GetTextDataEx ( int nIndex );
	
	int	GetLineCount ( int nIndex );
	//void RemoveLine ( int nIndex, int nLine );

protected:
	 /**
    	문자열을 추가한 후 제한 문자열 보다 많다면
        처음 입력된 문자열을 제거해 주어야 한다.
        이를 수행 한다.

     */
	void ReCalcEndLind();

	// TexBox - Index 삭제
	void EraseTextBoxIndex( int nIndex );

public:	
	virtual	void SetCurLine( int nCurLine );
	int	GetCurLine()					{ return m_nCurLine; }

public:
	long	GetLongestLine ();
	long	GetLongestPart ();

public:
	int	GetCount()						{ return static_cast<int>( m_TextBox.size() ); }
	int	GetVisibleLine()				{ return m_nVisibleLine; }
	bool empty()						{ return m_TextBox.empty(); }

public:
	void CalcMaxHEIGHT( float* pHEIGHT, int* pVISIBLELINE, int nPartIndexCOPY, int nLineIndexCOPY );
	void CalcMaxHEIGHT( float* pHEIGHT, int* pVISIBLELINE );
	float CalcMaxHEIGHT( int nBeginLine, int nEndLine );

public:
	int GetTotalLine()						{ return m_nTotalLine; }

	void SetLimitLine( int nLimitLine )		{ m_nLimitLine = nLimitLine; }
	int GetLimitLine()						{ return m_nLimitLine; }
	
	const float& GetLineInterval () const	{ return m_fLineInterval; }
	const float& GetPartInterval () const	{ return m_fPartInterval; }
    void SetAutoChangeFontSize( bool bEnable )  { m_bAutoChangeFontSize = bEnable; }

	BOOL FindStartIndex( int& nPartIndex, int& nLineIndex, const int nCurLine );

protected:
    void AutoChangeFontSize( const CString& strText, NS_UITEXTUTIL::MULTILINETEXT& MLText, int& nSplitLine, bool bCRLF );
    void PushText( const CString& strText, D3DCOLOR TextColor, bool bCutWord = true );	
	STEXTWORD MakeTextWord( const CString& strWord, const D3DCOLOR TextColor );
	STEXTLINE MakeTextLine( const CString& strWord, const D3DCOLOR TextColor, bool bLastLine );

protected:
	STEXTBOX	m_TextBox;

	int			m_nRenderdLine;		//	출력 완료한 라인수
	int			m_nRenderPartIndex;	//	출력중인 파트 인덱스
	int			m_nRenderLineIndex;	//	출력중인 라인 인덱스
	BOOL		m_bRenderTextColor;	//	마우스 오버시 텍스트 칼라 사용
	D3DCOLOR	m_dwRenderTextColor;//	마우스 오버시 텍스트 칼라

	float	m_fLineInterval;
	float	m_fPartInterval;

	int		m_nAlign;

	int		m_nVisibleLine;

	int		m_nCurLine;
	int		m_nPartIndex;
	int		m_nLineIndex;

	int		m_nLimitLine;
	int		m_nTotalLine;

	SUIGUID		m_ImageList;
//	UIGUID	m_nImageGenID;

	LPDIRECT3DDEVICEQ	m_pd3dDevice;

	D3DCOLOR	m_dwOverColor;	//	마우스 오버시 컬러	
	int			m_nOverIndex;
	BOOL		m_bUseOverColor;
    bool m_bAutoChangeFontSize; // 텍스트박스 크기가 작을 경우 font 크기를 자동으로 줄여서, 텍스트를 모두 출력할 수 있게 한다. default = false;
    CD3DFontPar* m_pFontOriginTemp; // 원래 font를 저장하기 위한 변수.
};

CBasicTextBox* CreateBasicTextControl(CUIGroup* pParent, const char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID WndID = NO_ID);