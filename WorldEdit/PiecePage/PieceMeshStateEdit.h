#pragma once

struct DxFrame;
class CsheetWithTab;

// CPieceMeshStateEdit 대화 상자입니다.

class CPieceMeshStateEdit : public CPropertyPage
{
	DECLARE_DYNAMIC(CPieceMeshStateEdit)

protected:
	CsheetWithTab	*m_pSheetTab;

public:
	void SetSheetTab( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

private:
	DxFrame* m_pDxFrame;

public:
	void SetInstance ( DxFrame *pDxFrame );

public:
	CPieceMeshStateEdit();
	virtual ~CPieceMeshStateEdit();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_PIECE_MESH_STATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	virtual BOOL OnSetActive();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedCheckEnableLightmap();
};
