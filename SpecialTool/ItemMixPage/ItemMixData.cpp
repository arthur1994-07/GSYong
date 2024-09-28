#include "stdafx.h"

#include "../../RanLogic/GLogicData.h"
#include "../../MfcExLib/ExLibs/EtcFunction.h"

#include "../SpecialTool.h"
#include "./ItemMixData.h"
#include "../ItemDialog.h"
#include "../WSPEditDialog.h"

IMPLEMENT_DYNAMIC(CItemMixData, CDialog)

BEGIN_MESSAGE_MAP(CItemMixData, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_MAT01_EDIT, OnBnClickedButtonMat01Edit)
	ON_BN_CLICKED(IDC_BUTTON_MAT02_EDIT, OnBnClickedButtonMat02Edit)
	ON_BN_CLICKED(IDC_BUTTON_MAT03_EDIT, OnBnClickedButtonMat03Edit)
	ON_BN_CLICKED(IDC_BUTTON_MAT04_EDIT, OnBnClickedButtonMat04Edit)
	ON_BN_CLICKED(IDC_BUTTON_MAT05_EDIT, OnBnClickedButtonMat05Edit)
	ON_BN_CLICKED(IDC_BUTTON_RESULT_EDIT, OnBnClickedButtonResultEdit)
	ON_BN_CLICKED(IDC_WSPEDIT, OnBnClickedButtonWspEdit )
END_MESSAGE_MAP()

CItemMixData::CItemMixData(CWnd* pParent /*=NULL*/)
	: CDialog(CItemMixData::IDD, pParent)
{
}

CItemMixData::~CItemMixData()
{
}

void CItemMixData::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

void CItemMixData::OnBnClickedOk()
{
	OnOK();
}

BOOL CItemMixData::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWin_Num_int( this, IDC_EDIT_ID, m_sProductRecipe.dwKey );

	int nID[3*(ITEMMIX_ITEMNUM+1)] = {
		IDC_EDIT_MAT01_MID, IDC_EDIT_MAT01_SID, IDC_EDIT_MAT01_NUM, 
		IDC_EDIT_MAT02_MID, IDC_EDIT_MAT02_SID, IDC_EDIT_MAT02_NUM, 
		IDC_EDIT_MAT03_MID, IDC_EDIT_MAT03_SID, IDC_EDIT_MAT03_NUM, 
		IDC_EDIT_MAT04_MID, IDC_EDIT_MAT04_SID, IDC_EDIT_MAT04_NUM, 
		IDC_EDIT_MAT05_MID, IDC_EDIT_MAT05_SID, IDC_EDIT_MAT05_NUM, 
		IDC_EDIT_RESULT_MID, IDC_EDIT_RESULT_SID, IDC_EDIT_RESULT_NUM };

    int i = 0;
	for ( i = 0; i < ITEMMIX_ITEMNUM; ++i ) 
	{
		SetWin_Num_int( this, nID[(i*3)], m_sProductRecipe.sMeterialItem[i].sNID.wMainID );
		SetWin_Num_int( this, nID[(i*3)+1], m_sProductRecipe.sMeterialItem[i].sNID.wSubID );
		SetWin_Num_int( this, nID[(i*3)+2], m_sProductRecipe.sMeterialItem[i].nNum );
	}

	SetWin_Num_int( this, nID[(i*3)], m_sProductRecipe.sResultItem.sNID.wMainID );
	SetWin_Num_int( this, nID[(i*3)+1], m_sProductRecipe.sResultItem.sNID.wSubID );
	SetWin_Num_int( this, nID[(i*3)+2], m_sProductRecipe.sResultItem.nNum );

	SetWin_Num_int( this, IDC_EDIT_RATE, m_sProductRecipe.dwRate );
	SetWin_Num_int( this, IDC_EDIT_PRICE, m_sProductRecipe.dwPrice );
	SetWin_Num_float( this, IDC_EDIT_DELAY, m_sProductRecipe.fDelaySec );

	SetWin_Check( this, IDC_NEED_LEARN, m_sProductRecipe.bNeedLearn );
	SetWin_Check( this, IDC_RANDOM_OPTION, m_sProductRecipe.bRandomOption );

	SetWin_Num_int( this, IDC_EDIT_ATTACK, m_sProductRecipe.nGradeAttack );
	SetWin_Num_int( this, IDC_EDIT_DEFENSE, m_sProductRecipe.nGradeDefense );

	SetWin_Num_int( this, IDC_EDIT_LEVEL, m_sProductRecipe.wLevel );
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CItemMixData::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	SetWin_Num_int( this, IDC_EDIT_ID, m_sProductRecipe.dwKey );

	int nID[ITEMMIX_ITEMNUM] = {
		IDC_EDIT_MAT01_NUM, IDC_EDIT_MAT02_NUM, 
		IDC_EDIT_MAT03_NUM, IDC_EDIT_MAT04_NUM, 
		IDC_EDIT_MAT05_NUM };

	for ( int i = 0; i < ITEMMIX_ITEMNUM; ++i ) 
	{
		if ( m_sProductRecipe.sMeterialItem[i].sNID == NATIVEID_NULL() ) 
			m_sProductRecipe.sMeterialItem[i].nNum = 0;
		else
			m_sProductRecipe.sMeterialItem[i].nNum = GetWin_Num_int( this, nID[i] );
	}

	m_sProductRecipe.sResultItem.nNum = GetWin_Num_int( this, IDC_EDIT_RESULT_NUM );

	m_sProductRecipe.dwRate = GetWin_Num_int( this, IDC_EDIT_RATE );
	m_sProductRecipe.dwRate = min( m_sProductRecipe.dwRate, 100 );
	m_sProductRecipe.dwRate = max( m_sProductRecipe.dwRate, 0 );

	m_sProductRecipe.dwPrice = GetWin_Num_int( this, IDC_EDIT_PRICE );
	m_sProductRecipe.fDelaySec = GetWin_Num_float( this, IDC_EDIT_DELAY );

	m_sProductRecipe.bNeedLearn = GetWin_Check( this, IDC_NEED_LEARN );
	m_sProductRecipe.bRandomOption = GetWin_Check( this, IDC_RANDOM_OPTION );

	m_sProductRecipe.nGradeAttack = (BYTE)GetWin_Num_int( this, IDC_EDIT_ATTACK );
	m_sProductRecipe.nGradeAttack = min( m_sProductRecipe.nGradeAttack, GRADE_HIGH );
	m_sProductRecipe.nGradeAttack = max( m_sProductRecipe.nGradeAttack, 0 );

	m_sProductRecipe.nGradeDefense = (BYTE)GetWin_Num_int( this, IDC_EDIT_DEFENSE );
	m_sProductRecipe.nGradeDefense = min( m_sProductRecipe.nGradeDefense, GRADE_HIGH );
	m_sProductRecipe.nGradeDefense = max( m_sProductRecipe.nGradeDefense, 0 );

	m_sProductRecipe.wLevel			= (WORD)GetWin_Num_int( this, IDC_EDIT_LEVEL );

	//	첫번째 재료의 유효성 체크 
	if( m_sProductRecipe.sMeterialItem[0].sNID == NATIVEID_NULL() )
	{
		MessageBox( "The first Material item must be present" );
		return;
	}

	// 결과 아이템 유효성 체크
	if( m_sProductRecipe.sResultItem.sNID == NATIVEID_NULL() )
	{
		MessageBox( "The Result item must be present" );
		return;
	}

	for ( int i = 0; i < ITEMMIX_ITEMNUM; ++i ) 
	{
		if( m_sProductRecipe.sMeterialItem[i].sNID != NATIVEID_NULL() &&
			m_sProductRecipe.sMeterialItem[i].nNum == 0 ) 
		{
			MessageBox( "Set the number of items" );
			return;
		}		
	}

	if( m_sProductRecipe.sResultItem.sNID != NATIVEID_NULL() &&
		m_sProductRecipe.sResultItem.nNum == 0 )
	{
		MessageBox( "Set the number of items" );
		return;
	}

	CDialog::OnOK();
}

void CItemMixData::OnBnClickedButtonMat01Edit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CItemDialog dlg;
	dlg.m_nidITEM = m_sProductRecipe.sMeterialItem[0].sNID;

	if ( dlg.DoModal() )
	{
		m_sProductRecipe.sMeterialItem[0].sNID = dlg.m_nidITEM;
		SetWin_Num_int( this, IDC_EDIT_MAT01_MID, dlg.m_nidITEM.wMainID );
		SetWin_Num_int( this, IDC_EDIT_MAT01_SID, dlg.m_nidITEM.wSubID );
	}
}

void CItemMixData::OnBnClickedButtonMat02Edit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CItemDialog dlg;
	dlg.m_nidITEM = m_sProductRecipe.sMeterialItem[1].sNID;

	if ( dlg.DoModal() )
	{
		m_sProductRecipe.sMeterialItem[1].sNID = dlg.m_nidITEM;
		SetWin_Num_int( this, IDC_EDIT_MAT02_MID, dlg.m_nidITEM.wMainID );
		SetWin_Num_int( this, IDC_EDIT_MAT02_SID, dlg.m_nidITEM.wSubID );
	}
}

void CItemMixData::OnBnClickedButtonMat03Edit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CItemDialog dlg;
	dlg.m_nidITEM = m_sProductRecipe.sMeterialItem[2].sNID;

	if ( dlg.DoModal() )
	{
		m_sProductRecipe.sMeterialItem[2].sNID = dlg.m_nidITEM;
		SetWin_Num_int( this, IDC_EDIT_MAT03_MID, dlg.m_nidITEM.wMainID );
		SetWin_Num_int( this, IDC_EDIT_MAT03_SID, dlg.m_nidITEM.wSubID );
	}
}

void CItemMixData::OnBnClickedButtonMat04Edit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CItemDialog dlg;
	dlg.m_nidITEM = m_sProductRecipe.sMeterialItem[3].sNID;

	if ( dlg.DoModal() )
	{
		m_sProductRecipe.sMeterialItem[3].sNID = dlg.m_nidITEM;
		SetWin_Num_int( this, IDC_EDIT_MAT04_MID, dlg.m_nidITEM.wMainID );
		SetWin_Num_int( this, IDC_EDIT_MAT04_SID, dlg.m_nidITEM.wSubID );
	}
}

void CItemMixData::OnBnClickedButtonMat05Edit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CItemDialog dlg;
	dlg.m_nidITEM = m_sProductRecipe.sMeterialItem[4].sNID;

	if ( dlg.DoModal() )
	{
		m_sProductRecipe.sMeterialItem[4].sNID = dlg.m_nidITEM;
		SetWin_Num_int( this, IDC_EDIT_MAT05_MID, dlg.m_nidITEM.wMainID );
		SetWin_Num_int( this, IDC_EDIT_MAT05_SID, dlg.m_nidITEM.wSubID );
	}
}

void CItemMixData::OnBnClickedButtonResultEdit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CItemDialog dlg;
	dlg.m_nidITEM = m_sProductRecipe.sResultItem.sNID;

	if ( dlg.DoModal() )
	{
		m_sProductRecipe.sResultItem.sNID = dlg.m_nidITEM;
		SetWin_Num_int( this, IDC_EDIT_RESULT_MID, dlg.m_nidITEM.wMainID );
		SetWin_Num_int( this, IDC_EDIT_RESULT_SID, dlg.m_nidITEM.wSubID );
	}
}

void CItemMixData::OnBnClickedButtonWspEdit()
{
	CWSPEditDialog dlg;
    dlg.m_ProductType = m_sProductRecipe.eProductType;
	dlg.m_wWs = m_sProductRecipe.wWs;
	dlg.m_wWsp = m_sProductRecipe.wWsp;
	dlg.m_wWspPer = m_sProductRecipe.wWspPer;
	if (dlg.DoModal() )
	{
        m_sProductRecipe.eProductType = dlg.m_ProductType;
		m_sProductRecipe.wWs = dlg.m_wWs;
		m_sProductRecipe.wWsp = dlg.m_wWsp;
		m_sProductRecipe.wWspPer = dlg.m_wWspPer;
	}
}