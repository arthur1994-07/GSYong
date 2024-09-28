#pragma once

#include "PieceContainerPage.h"

// CCharPage 대화 상자입니다.

class CCharPage : public CPieceContainerPage
{
		DECLARE_DYNAMIC(CCharPage)

public:
		CCharPage();
virtual ~CCharPage();

		enum { IDD = IDD_DIALOG_CHARPAGE };

static  BOOL	        g_bMakeNameChfCfg;	// cfg 파일 이름을 생성시 chf 이름을 붙인다.

virtual BOOL	        OnInitDialog();

afx_msg void 	        OnBnClickedButtonNew();
afx_msg void 	        OnBnClickedButtonOpen();
afx_msg void 	        OnBnClickedButtonSave();
//afx_msg void 	        OnBnClickedButtonSaveChfBillboard();
//afx_msg void 	        OnBnClickedButtonAllSave();
afx_msg void 	        OnBnClickedCheckNocharacter();
afx_msg void 	        OnDeltaposSpinRadius(NMHDR *pNMHDR, LRESULT *pResult);
afx_msg void 	        OnDeltaposSpinHeight(NMHDR *pNMHDR, LRESULT *pResult);
afx_msg void 	        OnEnChangeEditRadius();
afx_msg void 	        OnEnChangeEditHeight();
afx_msg void 	        OnBnClickedButtonLoadEe();
afx_msg void 	        OnBnClickedButtonSaveEe();
afx_msg void 	        OnBnClickedCheckNonatkmode();
afx_msg void 	        OnBnClickedCheckOneColor();
afx_msg void 	        OnBnClickedCheckNameChfCfg();

		void	        SetSpinControl( int nRadius, int nHeight, int nColorHeight );

		BOOL	        LoadChfFile( const char *pszFileName );

		VOID	        SetBoneEditTab( bool bEdit );
		VOID	        SetBoardEditTab( bool bEdit );

		VOID	        InsertSkinPiece( const char *pszFileName );
		VOID	        InsertAttBone( const char *pszFileName );
		VOID 	        InsertSkinEff( const char *pszFileName );
		VOID 	        LoadAnimation( const char *pszFileName );
		VOID	        LoadSkinBone( const char *pszFileName ); 

        DxSkinPiece*    CreateSkinPiece(int nParts);
        void            SkinPieceClear(int nParts = -1);
        void            SetSkinPiece(DxSkinPiece* pSkinPiece);
        void            SetSkinPieceType(DxSkinPiece* pSkinPiece);

        void            SwapSkinPiece(int nParts);
        void            ReplaceSkinPiece(int nParts);

        DxCharPart*     GetCharPart();
        int             GetSkinPieceSize();
        
        bool            IsSameBoneFile(const TCHAR* pName);
        bool            HasParts(DxSkinPiece* pTempSkinPiece);
        void            ShowWindow(bool bShow);

        void            ReSetPages();

protected:
		void	        ReSetCheckButton();
		void	        ReSetEditBox();

virtual void	        DoDataExchange(CDataExchange* pDX);

        SPSkinPiece     m_aSkinPieces[PIECE_SIZE];

		DECLARE_MESSAGE_MAP()
};