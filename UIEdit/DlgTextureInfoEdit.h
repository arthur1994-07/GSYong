#pragma once

#include "Resource.h"
#include "ViewDialog.h"
#include "afxwin.h"

class CSimpleImage;

class CDlgTextureInfoEdit : public CDialog
{
	DECLARE_DYNAMIC(CDlgTextureInfoEdit)

public:
	CDlgTextureInfoEdit(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgTextureInfoEdit();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEXTEDIT_DLG };

protected:
	CViewDialog		m_cDlgView;			// 텍스쳐 뷰어
	LPDIRECT3DTEXTUREQ m_pTextureImage;
	D3DXIMAGE_INFO	m_stImageInfo;
	DWORD*			m_pImageData;
	D3DLOCKED_RECT	m_stLocked;

	CString			m_strTextureFile;	// 텍스쳐 파일명

	INT				m_nPicWidth;		// 텍스쳐 넓이
	INT				m_nPicHeight;		// 텍스쳐 높이

	float			m_fMultiply;

	BOOL			m_bAlignRect;		// 마우스 선택 영역 보정 여부

	CPoint			m_bDNPoint;			// 좌측마우스 클릭된 위치
	CPoint			m_bUPPoint;			// 좌측마우스 놓은 위치
	CRect			m_cSelectRect;		// 마우스 선택 영역

	BOOL			m_bMoveTexture;		// 텍스쳐 이동중
	CPoint			m_bMovePoint;		// 뿌려줄 위치
	CPoint			m_bMoveDNPoint;		// 우측마우스 클릭된 위치
	CPoint			m_bMoveUPPoint;		// 우측마우스 놓은 위치


	INT				m_nControlType;		// 컨트롤 종류

	int				m_nAniIndex;		// 애니메이션 콘트롤 인덱스
	HBRUSH			m_bgBrush;			// 배경 색상 스태틱 컨트롤 색상

	CButton			m_checkBox_A;

protected:
	virtual VOID DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

public:
	VOID	SetAniIndex( int nIndex ) { m_nAniIndex = nIndex; }
	VOID	SetTextureInfo( CString strText, FLOAT fLeft, FLOAT ftop, FLOAT fRight, FLOAT fBottom );
	VOID	ClearBoxInfo();
	VOID	ClearMouseInfo();
	VOID	UpdateDlgCtrlRect();
	VOID	MarkSelectRect();
	VOID	AlignSelectRect();
	VOID	AlignTextureRect();
	D3DXCOLOR GetTexelData( INT iX, INT iY );

public:
	VOID	SetTextureMode( INT nMode )	{ m_nControlType = nMode; }

public:
	afx_msg VOID OnTimer(UINT_PTR nIDEvent);
	afx_msg VOID OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg VOID OnMouseMove(UINT nFlags, CPoint point);
	afx_msg VOID OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg VOID OnBnClickedBtnOPEN();
	afx_msg VOID OnBnClickedCheckAlign();
	afx_msg VOID OnBnClickedBtnOK();
	afx_msg VOID OnBnClickedBtnCANCEL();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedCheckScale1();
	afx_msg void OnBnClickedCheckScale2();
	afx_msg void OnBnClickedCheckScale3();
	afx_msg void OnCbnEditchangeComboZoom();
	afx_msg void OnCbnSelchangeComboZoom();
	afx_msg void OnStnClickedStaticBgcolor();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};