#pragma once
#include "afxcmn.h"
#include "../../RanLogic/Item/ItemSet/GLItemSet.h"
#include "../../enginelib/G-Logic/TypeDefine.h"

//아이템 정보를 넣는게 아니라 "세트아이템" 정보임

// SetItemInfo 대화 상자입니다.
struct SSETITEM;
struct SNATIVEID;
struct SITEM;
typedef struct SETITEMLIST SITLIST;


class SetItemInfo : public CDialog
{
	DECLARE_DYNAMIC(SetItemInfo)

public:
	SetItemInfo(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~SetItemInfo();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_SETITEMINFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();
	//SNATIVEID를 통해서 검사후 초기화함
	//탭 아이템의 정보 기반으로한 검사와 세트리스트 Dialog에서 리턴하는 ID를 통한 검사 두 종류 모두 사용

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
