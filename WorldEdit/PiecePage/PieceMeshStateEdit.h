#pragma once

struct DxFrame;
class CsheetWithTab;

// CPieceMeshStateEdit ��ȭ �����Դϴ�.

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

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_PIECE_MESH_STATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	virtual BOOL OnSetActive();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedCheckEnableLightmap();
};
