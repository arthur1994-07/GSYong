#pragma once
// GenItemRevise ��ȭ �����Դϴ�.
#include "../RanLogic/GenItemTable/GenItemManager.h"

class GenItemRevise : public CDialog
{
		DECLARE_DYNAMIC(GenItemRevise)

	public:
		static GenItemRevise& GetInstance()
		{
			static GenItemRevise m_Instance;
			return m_Instance;
		} //Searchpage�� ToolDialogue Ŭ�������� �ҷ��� �ϹǷ� �̱������� ����
		virtual ~GenItemRevise();

	private:
		GenItemRevise(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.

	// ��ȭ ���� �������Դϴ�.
		enum { IDD = IDD_DIALOG_REVISE };

	public:
		struct TypeNLine
		{	
			int m_Selection;
			GenItem::GenItemEditData::GenFileEditType m_Type;

			TypeNLine()
			:m_Selection(-1), m_Type(GenItem::GenItemEditData::NONE)
			{}

			TypeNLine(const TypeNLine &_Ref)
			{
				m_Selection = _Ref.m_Selection;
				m_Type = _Ref.m_Type;
			}		
		};

	private:
		CListCtrl m_ListGenItemFileData;
		CListCtrl m_Listannotation;
		CListCtrl m_ListSpecIDItem;

	private:
		std::string m_FileName; //���� ���� ���� ���� �̸�
		TypeNLine m_TNL;   //���õ� �����͸� �����ϴ� ��찡 �ƴϸ� -1�� �����ؾ���	
		bool m_CheckSkillFact;
		GenItem::GenItemEditData m_EditData;	
		GenItem::ItemInfo m_ItemInfo;
		std::vector<GenItem::ItemInfo> m_vecItInfo;
		std::vector<std::string> m_vecStr;

	public:
		BOOL OnInitDialog();
		void UpdateGenDataItems();
		void UpdateItemdataItems();
		void UpdateAnnotation();
		void SetFileName(std::string &_strName)
		{
			m_FileName = _strName;
		}
		void GetFileName(std::string &_strName)
		{
			_strName = m_FileName;
		}
		void Clear()
		{		
			m_TNL.m_Selection = -1;
			m_TNL.m_Type = GenItem::GenItemEditData::NONE;
			m_CheckSkillFact = false;
			m_ItemInfo.m_ItemID.wMainID = 65535;
			m_ItemInfo.m_ItemID.wSubID = 65535;
			m_ItemInfo.m_ItemName = "";
			m_ItemInfo.m_SpecID = -1;

			std::vector<GenItem::ItemInfo> obj;
			std::vector<std::string> obj2;
			obj.swap(m_vecItInfo);
			obj2.swap(m_vecStr);		
			m_EditData.erase();
		}
		GenItem::GenItemEditData::NumRate* GetNumRateFromList(int _Line);
		GenItem::GenItemEditData::SpIDnRate* GetSpIDnRate(int _Line);
		void CheckStateSkillFact()
		{
			m_CheckSkillFact  = m_CheckSkillFact ? false : true;
		}

		void GetDataOriginal();
		void FindClickData(GenItem::GenItemEditData::GenFileEditType &_dataType, int &_Line, int _SetItem);
		void DlgUseCheck();

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

		DECLARE_MESSAGE_MAP()
	public:
		afx_msg void OnNMDblclkListInfoGenitemfile(NMHDR *pNMHDR, LRESULT *pResult);
		afx_msg void OnBnClickedButtonReviseInputGenrate();
		afx_msg void OnBnClickedButtonReviseInputGennum();
		afx_msg void OnBnClickedButtonReviseInputGendata();
		afx_msg void OnBnClickedCheckReviseSkillfact();
		afx_msg void OnBnClickedReviseDelete();
		afx_msg void OnBnClickedReviseClear();
		afx_msg void OnBnClickedReviseSave();
		afx_msg void OnBnClickedButtonReviseFind();
		afx_msg void OnBnClickedButtonReviseSearchItem();
		afx_msg void OnEnChangeEditSpecidSingle();
		afx_msg void OnEnChangeEditReviseTemp();
		afx_msg void OnNMCustomdrawListInfoGenitemfile(NMHDR *pNMHDR, LRESULT *pResult);
		afx_msg void OnBnClickedButtonReviseInputAnnotation();
		afx_msg void OnNMDblclkListInfoGenitemfileAnnotation(NMHDR *pNMHDR, LRESULT *pResult);
		afx_msg void OnNMCustomdrawListInfoGenitemfileAnnotation(NMHDR *pNMHDR, LRESULT *pResult);
		afx_msg void OnNMClickListInfoGenitemfile(NMHDR *pNMHDR, LRESULT *pResult);
		afx_msg void OnNMClickListInfoGenitemfileAnnotation(NMHDR *pNMHDR, LRESULT *pResult);
};
