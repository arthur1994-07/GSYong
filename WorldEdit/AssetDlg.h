#pragma once

#include "../MfcExLib/ExLibs/CustomDrawTreeCtrl.h"
#include "../MfcExLib/ExLibs/ControlSizer.h"
#include "SettingIni.h"

/**
	���� �� �ǰ� �ִ� �𵨵��� ���(�ڻ�)����
    Ʈ�� ��Ʈ�ѷ� ���� �Ѵ�.

    ������ ����� Ŭ������ �� �ش� ��µ��� �������� �̵� �Ǹ�
    ���� �Ǿ� ����.

 */
class CAssetDlg
    : public CDialog, 
      public CSettingIni
{
        DECLARE_DYNAMIC(CAssetDlg)

public:
        CAssetDlg(CWnd* pParent = NULL);
virtual ~CAssetDlg();

        enum { IDD = IDD_DIALOG_ASSET };
        
        enum
        {
            TREEITEMTYPE_FRAMEMESH,
            TREEITEMTYPE_PIECE,            
        };

virtual BOOL                    OnInitDialog();

        /**
            Ʈ�� ��Ʈ��, ��ư �� ���̾�α׿� ���� ��Ʈ�ѵ��� 
            ���̾�αװ� ������¡ �� �� ���� ���� �ǵ��� ���� �Ѵ�.

         */
        void                    InitSizer();

        /**
            ������ �޽��� ó�� �Լ���
         */
afx_msg void                    OnSize(UINT nType, int cx, int cy);
afx_msg void                    OnBnClickedExport();
afx_msg void                    OnTvnSelchangedTreeAsset(NMHDR *pNMHDR, LRESULT *pResult);

        /**
        	���� �Էµ� �������� ��Ʈ�� ��� �Ѵ�.
            ��ü ��Ʈ�� �ƴ϶� �������� ��Ʈ�ӿ� ���� �Ѵ�.

         */
        HTREEITEM               GetTypeTreeItem(HTREEITEM hItem);

        /**
            �������� ���� �Ѵ�.
            ���� �信�� ������ �޽��� �����ϸ� ȣ�� �Ǿ�
            Ʈ�� ����Ʈ���� �ش� �������� �����ϰ� �� �ش�.

         */
        void                    SelectFrame(DxFrame* pFrame);

        /**
        	Ʈ�� ��Ʈ�ѿ� �������� �߰� �Ѵ�.

        	\param hParent �߰��� �θ� �ڵ�
        	\param pName �߰��� ������ ���ڿ�
        	\param bExpand �������� Ȯ������ ����
        	\param lParam �߰��� ������ ������
        	\param bFrontInsert �߰��� �θ� �ڵ��� ������ ��
                                �ش� �θ��� �ڽĵ� �� ���� ó���� �߰����� ����
        	\return �߰��� ������ �ڵ�
         */
        HTREEITEM               Insert(HTREEITEM hParent, const TCHAR* pName, bool bExpand = true, DWORD lParam = 0, bool bFrontInsert = false);

        /**
            �������� Ž�� �Ѵ�.

         */
        HTREEITEM               Find(const TCHAR* pName);
        HTREEITEM               GetChild(HTREEITEM hItem);
        HTREEITEM               GetNextSibling(HTREEITEM hItem);

        /**
            ���� ����(Sibling) �� Ʈ�� �������� ����� ��� �Ѵ�.

         */
        int                     GetSiblingSize(HTREEITEM hItem);

        /**
            ���� ����(Sibling) ���� ���õ� �������� ���° ����(Index) �� ����Ѵ�.

         */
        DWORD                   GetSiblingIndexFromSelected();

        /**
            �������� �ؽ�Ʈ�� ����, ��� �Ѵ�.

         */
        bool                    IsItemText(HTREEITEM hItem, const TCHAR* pText);
        bool                    IsSelectItemText(const TCHAR* pText);
        void                    SetItemText(HTREEITEM hItem, const TCHAR* pText);

        /**
            �������� �����͸� ���� �Ѵ�.

         */
        void                    SetItemData(HTREEITEM hItem, DWORD dwData);

        /**
            Ʈ�� ��Ʈ���� ���� �Ѵ�.

         */
        bool                    Delete(const TCHAR* pName);
        void                    Delete(HTREEITEM hItem);

        /**
            �ԷµǴ� Ʈ���� ��� ���� �������� ���� �Ѵ�.
            �Ϲ������� Ʈ�� �������� �����ϸ� ����� ����������
            �츮�� �θ�� �״�� �ΰ� ���길 �����ϰ� ���� �� ��� �Ѵ�.

         */
        void                    DeleteSub(HTREEITEM hItem);

        /**
            ��� Ʈ�� ��Ʈ���� ���� �Ѵ�.

         */
        void                    Clear();

        /**
        	���� ���õ� ���°� Ʈ���� �����Ͽ� ���õ� ��������
            �� ȭ���� ������Ʈ�� Ŭ���Ͽ� ���õ� �������� üũ �Ѵ�.

        	\return true : Ʈ���� Ŭ���� ����, false : ȭ���� Ŭ���Ͽ� ���õ� ����(����Ʈ��)
         */
        bool                    IsClick2Tree() { return m_bSelectTree; }

protected:

        /** <���� �Լ�>
            �ش� �̸��� ���� ù��° �������� ��ü Ʈ�� ��������
            ���鼭 ã�´�.

         */
        HTREEITEM               RecursiveFind(HTREEITEM hItem, const TCHAR* pName);

        /** <���� �Լ�>
            ��������� �������� ��Ʈ���� ��� �Ѵ�.
        	
        	\param hNode ����� Ʈ�� ������
        	\param nDepth Ʈ�� ������ ��� ����
        	\param fs ��� ��Ʈ��
         */
        void                    RecursiveExport(HTREEITEM hNode, int nDepth, std::fstream& fs);

        /** <���� �Լ�>
            ������ ������ Ʈ���������� ������ ���Ѵ�.            

            \param hItem ���� Ʈ�� ������
            \param nCount ������ ������ Ʈ�������� ����
                          ���� ������ �� ������ ��� �ȴ�.
         */
        void                    RecursiveSiblingSize(HTREEITEM hItem, int& nCount);

        /** <���� �Լ�>
            2���� Ʈ�� �������� ��ü �Ѵ�.
            ���������� �ؽ�Ʈ�� �������� ���� �����͸� ��ü �Ѵ�.
            ���� �߰����� ������ ���� �� ���� �ϵ��� �Ѵ�.

         */
        void                    SwapItem(HTREEITEM hNode, HTREEITEM hNode2);

virtual void                    DoDataExchange(CDataExchange* pDX);       

        DECLARE_MESSAGE_MAP()

private:
        CCustomDrawTreeCtrl     m_kTreeCtrl;
        CButton                 m_kBtnExport;
        CControlSizer           m_kSizer;
        bool                    m_bSelectTree;
};