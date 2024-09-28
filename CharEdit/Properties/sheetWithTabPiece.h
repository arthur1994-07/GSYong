#pragma once

#include <boost/tr1/memory.hpp>
#include "../../enginelib/DxEffect/Char/DxEffChar.h"

class   CPropertyPage;
class   CSkinPiecePage;
typedef std::tr1::shared_ptr<CSkinPiecePage> SPSkinPiecePage;
typedef std::tr1::shared_ptr<CPropertyPage> SPPropertyPage;

/**
	조각 편집 시트를 의미 한다.
    조각은 캐릭터, 비어클, 어태치본 조각이 있다.

    기존에 캐릭터들과 조각을 따로 랜더링 함으로 인해 동기화 문제가 있었다.
    즉 캐릭터에 사용되는 조각과 개별 조각이 동일한데 따로 렌더링 되므로
    이펙터, LOD 등을 서로 간에 동기화 시켜야 했다.

    이를 타파하고자 캐릭터와 조각의 통합 과정에서 새롭게 만든 클래스
	
 */
class CsheetWithTabPiece
    : public CPropertySheet
{
	    DECLARE_DYNAMIC(CsheetWithTabPiece)

public:
	    CsheetWithTabPiece(CRect& rect, CRect& rectAll, CWnd* pParentWnd);
virtual ~CsheetWithTabPiece();

        enum
        {
            /* 캐릭터 이펙터 외에 툴에서
               사용하는 페이지도 있음에 주의 한다. */

            EMEFFPAGE_MATERIAL = EMEFFCHAR_SIZE,
            EMEFFPAGE_LINE2BONED,
			EMEFFPAGE_USER_BACK_SLOT,
            EMEFFPAGE_SIZE,

            PIECEPAGE = EMEFFPAGE_SIZE,
        };

virtual BOOL                OnInitDialog();

        void                NoTabs() { m_bNoTabs = TRUE; }
        void                SetDistanceFromTop(int iDistance) { m_iDistanceFromTop = iDistance; }

        BOOL                SetActivePage(int nPage);

        void				SetSpinControl(int nColorHeight);
        void				ResetDialog();
        void				UpdateKeyBoardMouse();

        void                ReSetPages();
        void                ResetPiece();
        
        void                SetSkinPiece(DxSkinPiece* pSkinPiece);

        void                SwapSkinPiece(int nParts);
        void                ReplaceSkinPiece(int nParts);

        void                FullWindowsSize(bool bFull);
        SPSkinPiecePage     GetSkinPiecePage() { return m_SkinPiecePage; }

protected:
        void                CreateEffect();
        void                CreateEffect(CPropertyPage* pPage);

afx_msg HBRUSH              CtlColor(CDC* pDc, UINT uCtlColor);	

        DECLARE_MESSAGE_MAP()

private:
        CRect               m_Rect;
        CRect               m_RectAll;
        BOOL                m_bNoTabs;
        int                 m_iDistanceFromTop;
        CWnd*			    m_pWndParent;

        SPSkinPiecePage	    m_SkinPiecePage;        
        SPPropertyPage	    m_spEffList[EMEFFPAGE_SIZE];    
};