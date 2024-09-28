#pragma once

#include "../RanLogic/GenItemTable/GenItemManager.h"

// LinkLostDatainfo 대화 상자입니다.

class LinkLostDatainfo : public CDialog
{
		DECLARE_DYNAMIC(LinkLostDatainfo)

	public:
		LinkLostDatainfo(CWnd* pParent = NULL);   // 표준 생성자입니다.
		virtual ~LinkLostDatainfo();

	// 대화 상자 데이터입니다.
		enum 
		{
			ELEMENT_SIZE = 12,
			IDD = IDD_DIALOG_LINKLOST 
		};

		enum LinkLostCount
		{
			LINKLOST_ITEM = 0,
			LINKLOST_SPECID,
			LINKLOST_GENITEMFILE,
			LINKLOST_CROW
		};

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

		DECLARE_MESSAGE_MAP()		

	public: 
		BOOL OnInitDialog();
		void UpDateItems();
		void SetCtrlListItemCase(const std::vector<GenItem::LinkLostData*> &_Data);
		void SetCtrlNoUseSpecID(const std::vector<int> &_vecNoUse);
		int FindIndexFromData(const std::vector<GenItem::LinkLostData*> &_LinkLostData, 
							LinkLostCount _Case, const GenItem::LinkLostData &_Source);
		HRESULT SaveExcelFile(CWnd* _pWnd);

	private:
		
		CListCtrl m_ListLinkLostItem;
		CListCtrl m_ListLinkLostSpecID;
		CListCtrl m_ListLinkLostGenItemFile;
		CListCtrl m_ListLinkLostCrow;
		CListCtrl m_ListNoUseSpecID;

		std::vector<GenItem::LinkLostData*> m_LinkLostData;
		std::vector<int> m_NoUseSpecID;
		unsigned int CountLinkLost[4];		
public:
	afx_msg void OnBnClickedButtonLinklostItemFind();
	afx_msg void OnBnClickedButtonLinklostSpecidFind();
	afx_msg void OnBnClickedButtonLinklostGenitemfileFind();
	afx_msg void OnBnClickedButtonLinklostCrowFind();
	afx_msg void OnBnClickedButtonReload();
	afx_msg void OnBnClickedButtonLinklostCsv();
};
