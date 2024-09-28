#pragma once

#include "PieceContainerPage.h"

/**
	탈것 설정 페이지

	메인 콤보 박스에서 <Vehicle 파일> 을 선택하면 활성화 된다.

 */
class CVehiclePage : public CPieceContainerPage
{
		DECLARE_DYNAMIC(CVehiclePage)

public:
		CVehiclePage();
		virtual ~CVehiclePage();
		
		enum    { IDD = IDD_DIALOG_VEHICLEPAGE };		

		void        SetBoardEditTab( bool bEdit );

virtual BOOL        OnInitDialog();
afx_msg void        OnBnClickedButtonNew();
afx_msg void        OnBnClickedButtonOpen();
afx_msg void        OnBnClickedButtonSave();
afx_msg void        OnBnClickedCheckNocharacter();
afx_msg void        OnEnChangeEditRadius();
afx_msg void        OnEnChangeEditHeight();
afx_msg void		OnBnClickedButtonFootColl();

		BOOL        LoadVcfFile( const char *pszFileName );

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

protected:

virtual void        DoDataExchange(CDataExchange* pDX);

		void        ReSetCheckButton();
		void        ReSetEditBox();

		VOID        InsertSkinPiece( const char *pszFileName );
		VOID        InsertSkinEff( const char *pszFileName );
		VOID        LoadAnimation( const char *pszFileName );
		VOID        LoadSkinBone( const char *pszFileName ); 

        SPSkinPiece m_aSkinPieces[PART_V_SIZE];

		DECLARE_MESSAGE_MAP()
};