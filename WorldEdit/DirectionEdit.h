#pragma once
#include "afxwin.h"


class CsheetWithTab;

#include "StCreatePos.h"

// DirectionEdit 대화 상자입니다.

class DirectionEdit : public CPropertyPage
{
	DECLARE_DYNAMIC(DirectionEdit)

protected:
	CsheetWithTab	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }
	void UpdateContrl ();

protected:
	float			m_fScale;
	float			m_ToScale;

public:
	DWORD			m_dwOldPageIndex;
	D3DXVECTOR3*	m_pvDirection;

	float			m_fOldScale;
	D3DXVECTOR3		m_pvOldDirection;

private:
	void ReDraw();

public:
	DirectionEdit();
	virtual ~DirectionEdit();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_DIRECTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CStCreatePos m_stDirection;
	CScrollBar m_ZScroll;
	CScrollBar m_YScroll;
	CScrollBar m_XScroll;

	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonCancel();
	virtual BOOL OnKillActive();
	virtual BOOL OnSetActive();
	afx_msg void OnEnChangeEditX();
	afx_msg void OnEnChangeEditY();
	afx_msg void OnEnChangeEditZ();
};

namespace NSDirectionRender
{
	void SetActive( BOOL bActive );
	void Update( const D3DXVECTOR3& vDir );
	void Render( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vLookat );
};
