#pragma once

#include "./PropPages/PropertyChaBase.h"
#include "./PropPages/PropertyChaInven.h"
#include "./PropPages/PropertyChaPutOnItem.h"
#include "./PropPages/PropertyChaSkill.h"
#include "./PropPages/PropertyChaUserInven.h"
#include "./PropPages/PropertyChaQuest.h"
#include "./PropPages/PropertyVehicle.h"
#include "./PropPages/PropertyChaItemMix.h"
#include "./PropPages/PropertyChaClubInven.h"

#include "../../RanLogic/GLogicEx.h"
#include "../../RanLogic/Character/GLCharData.h"

namespace gmce
{
	class IDBManager;
}

class CPropertySheetCharEdit : public CPropertySheet
{
	DECLARE_DYNAMIC(CPropertySheetCharEdit)

public:
	CPropertySheetCharEdit(int nChaNum, int nUserNumber, UINT nIDCaption,    CWnd* pParentWnd = NULL, UINT iSelectPage = 0, bool ShowRestore = false);
	CPropertySheetCharEdit(int nChaNum, int nUserNumber, LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0, bool ShowRestore = false);
	virtual ~CPropertySheetCharEdit();

public:
    int m_nChaNum;
    int m_nUserNumber;
    GLCHARLOGIC* m_pChar;
	std::vector<SITEMCUSTOM>	m_vecModifyItems;	// 랜덤옵션을 변경한 아이템들

	gmce::IDBManager *m_pDBMan;

    bool m_ShowRestore;

protected:
	DECLARE_MESSAGE_MAP()

    CPropertyChaBase        m_ChaBase;
    CPropertyChaInven       m_ChaInven;
    CPropertyChaPutOnItem   m_ChaPutOnItem;
    CPropertyChaSkill       m_ChaSkill;    
    CPropertyChaUserInven   m_ChaUserInven0;
    CPropertyChaUserInven   m_ChaUserInven1;
    CPropertyChaUserInven   m_ChaUserInven2;
    CPropertyChaUserInven   m_ChaUserInven3;
    CPropertyChaUserInven   m_ChaUserInvenP;
    CPropertyChaQuest       m_ChaQuest;
	CPropertyChaItemMix		m_ChaItemMix;
	CPropertyChaClubInven	m_ChaClubInven;


	//
	//mjeon.ado
	//
	// newly added features will be only supported by ADO.
	//
	CPropertyVehicle		m_ChaVehicle;


    void AddAllPages();

public:
    virtual BOOL OnInitDialog();
};


