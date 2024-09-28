#pragma once

#include "../../MfcExLib/ExLibs/CustomDrawTreeCtrl.h"
#include "../../MfcExLib/ExLibs/ControlSizer.h"
#include "../Util/SettingIni.h"

/**
	���� �� �ǰ� �ִ� �𵨵��� ���(�ڻ�)����
    Ʈ�� ��Ʈ�ѷ� ���� �Ѵ�.

 */
class CAssetDlg
    : public CDialog
    , public CSettingIni
{
        DECLARE_DYNAMIC(CAssetDlg)

public:
        CAssetDlg(CWnd* pParent = NULL);
virtual ~CAssetDlg();

        enum { IDD = IDD_DIALOG_ASSET };


virtual BOOL                    OnInitDialog();

        /**
            Ʈ�� ��Ʈ��, ��ư �� ���̾�α׿� ���� ��Ʈ�ѵ��� 
            ���̾�αװ� ������¡ �� �� ���� ���� �ǵ��� ���� �Ѵ�.

         */
        void                    InitSizer();

afx_msg void                    OnSize(UINT nType, int cx, int cy);
afx_msg void                    OnBnClickedExport();

        /**
            Ʈ�� ��Ʈ�ѿ� �������� �߰� �Ѵ�.

         */
        HTREEITEM               Insert(HTREEITEM hParent, const TCHAR* pName, bool bExpand = true);
        void                    Clear();

protected:

        /**
            ��������� �������� ��Ʈ���� ��� �Ѵ�.
        	
        	\param hNode ����� Ʈ�� ������
        	\param nDepth Ʈ�� ������ ��� ����
        	\param fs ��� ��Ʈ��
         */
        void                    RecursiveExport(HTREEITEM hNode, int nDepth, std::fstream& fs);

virtual void                    DoDataExchange(CDataExchange* pDX);       

        DECLARE_MESSAGE_MAP()

private:
        CCustomDrawTreeCtrl     m_kTreeCtrl;
        CButton                 m_kBtnExport;
        CControlSizer           m_kSizer;
};