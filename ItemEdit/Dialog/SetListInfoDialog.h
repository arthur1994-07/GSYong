#pragma once

#include "../../RanLogic/Item/ItemSet/GLItemSet.h"
#include "../../enginelib/G-Logic/TypeDefine.h"

// SetListInfoDialog 대화 상자입니다.

class SetListInfoDialog : public CDialog
{
	DECLARE_DYNAMIC(SetListInfoDialog)

public:
	SetListInfoDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~SetListInfoDialog();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_SETLIST_INFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

public:
	void OutPutSetList();
	SNATIVEID GetReturnID() {return m_GiveToSetItemInfoDlg;}
private:
	CListCtrl m_SetITListList;							//세트리스트 - 리스트 컨트롤
	std::vector<SSETITEM>		m_vecSetIteminfoList;		//세트아이템 정보 리스트
	SNATIVEID m_GiveToSetItemInfoDlg;					//세트아이템 info dlg에 넘기기 위한 용도

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonSelect();
};
