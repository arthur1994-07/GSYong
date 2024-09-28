#pragma once

#include "Resource.h"
#include "ViewDialog.h"
#include "afxwin.h"

class CSimpleImage;

class CDlgTextureInfoEdit : public CDialog
{
	DECLARE_DYNAMIC(CDlgTextureInfoEdit)

public:
	CDlgTextureInfoEdit(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgTextureInfoEdit();

	// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_TEXTEDIT_DLG };

protected:
	CViewDialog		m_cDlgView;			// �ؽ��� ���
	LPDIRECT3DTEXTUREQ m_pTextureImage;
	D3DXIMAGE_INFO	m_stImageInfo;
	DWORD*			m_pImageData;
	D3DLOCKED_RECT	m_stLocked;

	CString			m_strTextureFile;	// �ؽ��� ���ϸ�

	INT				m_nPicWidth;		// �ؽ��� ����
	INT				m_nPicHeight;		// �ؽ��� ����

	float			m_fMultiply;

	BOOL			m_bAlignRect;		// ���콺 ���� ���� ���� ����

	CPoint			m_bDNPoint;			// �������콺 Ŭ���� ��ġ
	CPoint			m_bUPPoint;			// �������콺 ���� ��ġ
	CRect			m_cSelectRect;		// ���콺 ���� ����

	BOOL			m_bMoveTexture;		// �ؽ��� �̵���
	CPoint			m_bMovePoint;		// �ѷ��� ��ġ
	CPoint			m_bMoveDNPoint;		// �������콺 Ŭ���� ��ġ
	CPoint			m_bMoveUPPoint;		// �������콺 ���� ��ġ


	INT				m_nControlType;		// ��Ʈ�� ����

	int				m_nAniIndex;		// �ִϸ��̼� ��Ʈ�� �ε���
	HBRUSH			m_bgBrush;			// ��� ���� ����ƽ ��Ʈ�� ����

	CButton			m_checkBox_A;

protected:
	virtual VOID DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
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