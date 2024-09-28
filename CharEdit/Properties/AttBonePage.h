#pragma once

#include "PieceContainerPage.h"

// CAttBonePage 대화 상자입니다.

class CAttBonePage : public CPieceContainerPage
{
	    DECLARE_DYNAMIC(CAttBonePage)

public:
        CAttBonePage();
virtual ~CAttBonePage();

        // 대화 상자 데이터입니다.
        enum { IDD = IDD_DIALOG_ATTBONEPAGE };

virtual BOOL        OnInitDialog();

	    BOOL	    LoadAbfFile( const char *pszFileName );

        DxSkinPiece*CreateSkinPiece(int nParts);
        void        SkinPieceClear(int nParts = -1);
        void        SetSkinPiece(DxSkinPiece* pSkinPiece);
        void        SetSkinPieceType(DxSkinPiece* pSkinPiece);

        void        SwapSkinPiece(int nParts);
        void        ReplaceSkinPiece(int nParts);

        DxCharPart* GetCharPart();
        int         GetSkinPieceSize();

        bool        IsSameBoneFile(const TCHAR* pName);
        bool        HasParts(DxSkinPiece* pTempSkinPiece);
        void        ShowWindow(bool bShow);

        void        ReSetPages();

afx_msg void        OnBnClickedButtonNew();
afx_msg void        OnBnClickedButtonOpen();
afx_msg void        OnBnClickedButtonSave();
afx_msg void        OnBnClickedCheckNocharacter();
afx_msg void        OnEnChangeEditRadius();
afx_msg void        OnEnChangeEditHeight();

protected:
	    VOID	    InsertSkinPiece( const char *pszFileName );
	    VOID 	    InsertSkinEff( const char *pszFileName );
	    VOID 	    LoadAnimation( const char *pszFileName );
	    VOID	    LoadSkinBone( const char *pszFileName ); 

        void	    ReSetCheckButton();
        void	    ReSetEditBox();        

virtual void        DoDataExchange(CDataExchange* pDX);

        SPSkinPiece m_aSkinPieces[ATTBONE_SIZE];

	    DECLARE_MESSAGE_MAP()
};