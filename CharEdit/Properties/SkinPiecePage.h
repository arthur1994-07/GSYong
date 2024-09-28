#pragma once


#include "PieceRelativePage.h"
#include "../MfcExLib/ExLibs/Separator.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxPieceDefine.h"

class CsheetWithTabChar;
class CsheetWithTabPiece;
class CPieceContainerPage;

class CSkinPiecePage : public CPieceRelativePage// : public CPropertyPage
{
	DECLARE_DYNAMIC(CSkinPiecePage)

public:
	CSkinPiecePage();
	virtual ~CSkinPiecePage();

	enum				{ IDD = IDD_DIALOG_SKINPIECEPAGE };

	virtual BOOL 				OnInitDialog();
	void				SetSheetTab ( CsheetWithTabChar *pSheetTab )		{ m_pSheetTabParent = pSheetTab; }

	void 				ShowMeshSetContrl ( BOOL bOk );
	void 				SetMeshSetContrl ();
	void 				SetComboWeaponWhereBack();

	void                InitPage(bool bResetPiece = true);
	void 				EffectListting ();
	void 				VertListting ();
	void                SetFileName();
	void                MeshListting();

	void                InitSkinPiece();
	bool                SetSkinPiece(DxSkinPiece* pSkinPiece);
	HRESULT 			LoadSkinPiece( const char *pszFileName );
	void				SetEditFileName( const char *pszFileName, BOOL bBone );
	void                SetPieceType(DxSkinPiece* pSkinPiece);

	bool                CheckBone(struct DxSkinPiece* pTempSkin);
	bool                CheckParts(DxSkinPiece* pTempSkin);

	void				HatHairOptionWnd();

	CsheetWithTabPiece* GetParent();
	CPieceContainerPage*GetGrandParent();

	afx_msg void 				OnBnClickedButtonSkinfile();
	afx_msg void 				OnBnClickedButtonBonefile();
	afx_msg void 				OnBnClickedButtonNew();
	afx_msg void 				OnBnClickedButtonOpen();
	afx_msg void 				OnBnClickedButtonSave();
	afx_msg void 				OnBnClickedButtonAllSave();
	afx_msg void 				OnBnClickedButtonMakeobj();
	afx_msg void 				OnBnClickedButtonSkinmeshselect();
	afx_msg void 				OnCbnSelchangeComboSkinmeshtype();
	afx_msg void 				OnBnClickedButtonEffinsert();
	afx_msg void 				OnBnClickedButtonEffstate();
	afx_msg void 				OnBnClickedButtonEffdel();
	afx_msg void 				OnBnClickedButtonVertinsert();
	afx_msg void 				OnBnClickedButtonVertdel();
	afx_msg void 				OnBnClickedCheckVertDraw();
	afx_msg void 				OnBnClickedButtonEffSave();
	afx_msg void 				OnBnClickedButtonEffLoad();
	afx_msg void 				OnBnClickedButtonMaterial();
	afx_msg void 				OnLvnItemchangedListVertics(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void 				OnCbnSelchangeComboWhereback();
	afx_msg void 				OnLbnDblclkListEfflist();
	afx_msg void 				OnCbnSelchangeComboType();
	afx_msg void 				OnBnClickedButtonHairNone();
	afx_msg void 				OnBnClickedButtonHairBase();
	afx_msg void 				OnBnClickedButtonHairNew();
	afx_msg void 				OnBnClickedButtonAddHairCpsLoad();

	CComboBox			m_comboMeshs;
	CSeparator			m_stSetLine;
	CComboBox			m_comboSkinMeshType;
	CListCtrl			m_listVertics;
	CComboBox			m_EffType;
	CListBox			m_EffList;
	CComboBox			m_comboWhereBack;
	CComboBox			m_combo_Type;
	EMPICETYPE			m_emPieceType;
	TSTRING             m_strPiece;

private:
	void EnableWnd_HairRenderOff();

protected:
	CsheetWithTabChar	*m_pSheetTabParent;		
	virtual void				DoDataExchange(CDataExchange* pDX);
	virtual void				PostNcDestroy();

	DECLARE_MESSAGE_MAP();
public:
	afx_msg void OnBnClickedButtonSlotWeapon();
	afx_msg void OnBnClickedButtonDocument();
};
