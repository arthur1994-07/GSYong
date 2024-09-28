#pragma once
#include "afxcmn.h"
#include "../../RanLogic/Item/ItemSet/GLItemSet.h"
#include "../../enginelib/G-Logic/TypeDefine.h"

//������ ������ �ִ°� �ƴ϶� "��Ʈ������" ������

// SetItemInfo ��ȭ �����Դϴ�.
struct SSETITEM;
struct SNATIVEID;
struct SITEM;
typedef struct SETITEMLIST SITLIST;


class SetItemInfo : public CDialog
{
	DECLARE_DYNAMIC(SetItemInfo)

public:
	SetItemInfo(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~SetItemInfo();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_SETITEMINFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	virtual BOOL OnInitDialog();
	//SNATIVEID�� ���ؼ� �˻��� �ʱ�ȭ��
	//�� �������� ���� ��������� �˻�� ��Ʈ����Ʈ Dialog���� �����ϴ� ID�� ���� �˻� �� ���� ��� ���

	DECLARE_MESSAGE_MAP()

public:
	void IDListSetting();
	void BonusListSetting();

	afx_msg void OnBnClickedSelectButton();
public:
	void SetNativeID(const SNATIVEID *Item_ID); 
	void FindItemSetInfo();
	void OutPutNoList();
	
private:
	CListCtrl			m_cIDList;
	CListCtrl			m_cBonus;

	SSETITEM		m_SetItem;
	SITLIST			m_SetList;
	SNATIVEID		m_NativeID;

public:
	afx_msg void OnBnClickedReloadButton();
};
