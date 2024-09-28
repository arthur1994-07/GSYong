#pragma once

#include "../MfcExLib/ExLibs/CustomDrawTreeCtrl.h"
#include "../MfcExLib/ExLibs/ControlSizer.h"
#include "SettingIni.h"

/**
	현재 뷰 되고 있는 모델들의 어셋(자산)들을
    트리 컨트롤로 나열 한다.

    나열된 어셋을 클릭했을 때 해당 어셋들의 페이지로 이동 되며
    선택 되어 진다.

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
            트리 컨트롤, 버튼 등 다이얼로그에 붙은 컨트롤들이 
            다이얼로그가 리사이징 될 때 같이 갱신 되도록 설정 한다.

         */
        void                    InitSizer();

        /**
            윈도우 메시지 처리 함수들
         */
afx_msg void                    OnSize(UINT nType, int cx, int cy);
afx_msg void                    OnBnClickedExport();
afx_msg void                    OnTvnSelchangedTreeAsset(NMHDR *pNMHDR, LRESULT *pResult);

        /**
        	현재 입력된 아이템의 루트를 출력 한다.
            전체 루트가 아니라 아이템의 루트임에 주의 한다.

         */
        HTREEITEM               GetTypeTreeItem(HTREEITEM hItem);

        /**
            프레임을 선택 한다.
            툴의 뷰에서 프레임 메쉬를 선택하며 호출 되어
            트리 리스트에서 해당 프레임을 선택하게 해 준다.

         */
        void                    SelectFrame(DxFrame* pFrame);

        /**
        	트리 컨트롤에 아이템을 추가 한다.

        	\param hParent 추가할 부모 핸들
        	\param pName 추가할 아이템 문자열
        	\param bExpand 아이템을 확장할지 유무
        	\param lParam 추가할 아이템 데이터
        	\param bFrontInsert 추가할 부모 핸들이 존재할 떄
                                해당 부모의 자식들 중 가장 처음에 추가할지 유무
        	\return 추가한 아이템 핸들
         */
        HTREEITEM               Insert(HTREEITEM hParent, const TCHAR* pName, bool bExpand = true, DWORD lParam = 0, bool bFrontInsert = false);

        /**
            아이템을 탐색 한다.

         */
        HTREEITEM               Find(const TCHAR* pName);
        HTREEITEM               GetChild(HTREEITEM hItem);
        HTREEITEM               GetNextSibling(HTREEITEM hItem);

        /**
            동일 깊이(Sibling) 의 트리 아이템이 몇개인지 출력 한다.

         */
        int                     GetSiblingSize(HTREEITEM hItem);

        /**
            동일 깊이(Sibling) 에서 선택된 아이템이 몇번째 인지(Index) 를 출력한다.

         */
        DWORD                   GetSiblingIndexFromSelected();

        /**
            아이템의 텍스트를 편집, 출력 한다.

         */
        bool                    IsItemText(HTREEITEM hItem, const TCHAR* pText);
        bool                    IsSelectItemText(const TCHAR* pText);
        void                    SetItemText(HTREEITEM hItem, const TCHAR* pText);

        /**
            아이템의 데이터를 설정 한다.

         */
        void                    SetItemData(HTREEITEM hItem, DWORD dwData);

        /**
            트리 컨트롤을 삭제 한다.

         */
        bool                    Delete(const TCHAR* pName);
        void                    Delete(HTREEITEM hItem);

        /**
            입력되는 트리의 모든 서브 아이템을 삭제 한다.
            일반적으로 트리 아이템을 삭제하면 서브는 삭제되지만
            우리는 부모는 그대로 두고 서브만 삭제하고 싶을 때 사용 한다.

         */
        void                    DeleteSub(HTREEITEM hItem);

        /**
            모든 트리 컨트롤을 삭제 한다.

         */
        void                    Clear();

        /**
        	현재 선택된 상태가 트리를 선택하여 선택된 상태인지
            뷰 화면의 오브젝트를 클릭하여 선택된 상태인지 체크 한다.

        	\return true : 트리를 클릭한 상태, false : 화면을 클릭하여 선택된 상태(디폴트값)
         */
        bool                    IsClick2Tree() { return m_bSelectTree; }

protected:

        /** <내부 함수>
            해당 이름을 가진 첫번째 아이템을 전체 트리 아이템을
            돌면서 찾는다.

         */
        HTREEITEM               RecursiveFind(HTREEITEM hItem, const TCHAR* pName);

        /** <내부 함수>
            재귀적으로 아이템을 스트림에 출력 한다.
        	
        	\param hNode 출력할 트리 아이템
        	\param nDepth 트리 아이템 출력 깊이
        	\param fs 출력 스트림
         */
        void                    RecursiveExport(HTREEITEM hNode, int nDepth, std::fstream& fs);

        /** <내부 함수>
            동일한 깊이의 트리아이템의 갯수를 구한다.            

            \param hItem 시작 트리 아이템
            \param nCount 동일한 깊이의 트리아이템 갯수
                          최종 갯수가 이 참조로 출력 된다.
         */
        void                    RecursiveSiblingSize(HTREEITEM hItem, int& nCount);

        /** <내부 함수>
            2개의 트리 아이템을 교체 한다.
            내부적으로 텍스트와 아이템이 가진 데이터만 교체 한다.
            향후 추가적인 사항이 있을 시 구현 하도록 한다.

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