#pragma once

#include <boost/tr1/memory.hpp>
#include "../../enginelib/DxEffect/Char/DxEffChar.h"

class   CPropertyPage;
class   CSkinPiecePage;
typedef std::tr1::shared_ptr<CSkinPiecePage> SPSkinPiecePage;
typedef std::tr1::shared_ptr<CPropertyPage> SPPropertyPage;

/**
	���� ���� ��Ʈ�� �ǹ� �Ѵ�.
    ������ ĳ����, ���Ŭ, ����ġ�� ������ �ִ�.

    ������ ĳ���͵�� ������ ���� ������ ������ ���� ����ȭ ������ �־���.
    �� ĳ���Ϳ� ���Ǵ� ������ ���� ������ �����ѵ� ���� ������ �ǹǷ�
    ������, LOD ���� ���� ���� ����ȭ ���Ѿ� �ߴ�.

    �̸� Ÿ���ϰ��� ĳ���Ϳ� ������ ���� �������� ���Ӱ� ���� Ŭ����
	
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
            /* ĳ���� ������ �ܿ� ������
               ����ϴ� �������� ������ ���� �Ѵ�. */

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