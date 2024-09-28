#pragma once

#include "../../MfcExLib/ExLibs/CustomDrawTreeCtrl.h"
#include "../../MfcExLib/ExLibs/ControlSizer.h"
#include "../Util/SettingIni.h"

/**
	현재 뷰 되고 있는 모델들의 어셋(자산)들을
    트리 컨트롤로 나열 한다.

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
            트리 컨트롤, 버튼 등 다이얼로그에 붙은 컨트롤들이 
            다이얼로그가 리사이징 될 때 같이 갱신 되도록 설정 한다.

         */
        void                    InitSizer();

afx_msg void                    OnSize(UINT nType, int cx, int cy);
afx_msg void                    OnBnClickedExport();

        /**
            트리 컨트롤에 아이템을 추가 한다.

         */
        HTREEITEM               Insert(HTREEITEM hParent, const TCHAR* pName, bool bExpand = true);
        void                    Clear();

protected:

        /**
            재귀적으로 아이템을 스트림에 출력 한다.
        	
        	\param hNode 출력할 트리 아이템
        	\param nDepth 트리 아이템 출력 깊이
        	\param fs 출력 스트림
         */
        void                    RecursiveExport(HTREEITEM hNode, int nDepth, std::fstream& fs);

virtual void                    DoDataExchange(CDataExchange* pDX);       

        DECLARE_MESSAGE_MAP()

private:
        CCustomDrawTreeCtrl     m_kTreeCtrl;
        CButton                 m_kBtnExport;
        CControlSizer           m_kSizer;
};