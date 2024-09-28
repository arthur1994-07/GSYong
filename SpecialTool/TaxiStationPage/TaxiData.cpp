// TaxiData.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "../SpecialTool.h"
#include "./TaxiData.h"
#include "../../MfcExLib/ExLibs/EtcFunction.h"
#include "../../EngineLib/G-Logic/GLDefine.h"

#include "../../RanLogic/GLUseFeatures.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"

#include "../NpcDialog.h"
#include "../MapDialog.h"


// CTaxiData 대화 상자입니다.

IMPLEMENT_DYNAMIC(CTaxiData, CDialog)
CTaxiData::CTaxiData(CWnd* pParent /*=NULL*/)
	: CDialog(CTaxiData::IDD, pParent)
{
}

CTaxiData::~CTaxiData()
{
}

void CTaxiData::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTaxiData, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_MAP_EDIT, OnBnClickedButtonMapEdit)
	ON_BN_CLICKED(IDC_BUTTON_NPC_EDIT, OnBnClickedButtonNpcEdit)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CTaxiData 메시지 처리기입니다.

BOOL CTaxiData::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	SetWin_Num_int( this, IDC_EDIT_ID, m_sTaxiStation.dwLINKID );

	SetWin_Check( this, IDC_CHECK_USE, m_sTaxiStation.bUsed );
	
	const SNATIVEID sMapNativeID = m_sTaxiStation.dwMAPID;
	SetWin_Num_int( this, IDC_EDIT_MAP_MID, sMapNativeID.wMainID );
	SetWin_Num_int( this, IDC_EDIT_MAP_SID, sMapNativeID.wSubID );
	
	SetWin_Num_int( this, IDC_EDIT_MAP_XPOS, m_sTaxiStation.wPosX );
	SetWin_Num_int( this, IDC_EDIT_MAP_YPOS, m_sTaxiStation.wPosY );

	const SNATIVEID sNPCNativeID = m_sTaxiStation.dwNPCID;
	SetWin_Num_int( this, IDC_EDIT_NPC_MID, sNPCNativeID.wMainID );
	SetWin_Num_int( this, IDC_EDIT_NPC_SID, sNPCNativeID.wSubID );

	SetWin_Num_int( this, IDC_EDIT_CHARGE, m_sTaxiStation.dwMapCharge );

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() == TRUE )
	{
		CString strFind;
		strFind.Format("CN_%03d_%03d", sNPCNativeID.wMainID, sNPCNativeID.wSubID);
		SetWin_Text(this, IDC_EDIT_NPC_NAME, CGameTextMan::GetInstance().GetCrowText(strFind.GetString()).GetString());
		strFind.Format("MAP_%d_%d", sMapNativeID.wMainID, sMapNativeID.wSubID);
		SetWin_Text(this, IDC_EDIT_MAP_NAME, CGameTextMan::GetInstance().GetGameInText(strFind.GetString()).GetString());
	}
	else
	{
		SetWin_Text( this, IDC_EDIT_MAP_NAME, m_sTaxiStation.strMAP.c_str() );
		SetWin_Text( this, IDC_EDIT_NPC_NAME, m_sTaxiStation.strSTATION.c_str() );	
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CTaxiData::OnBnClickedButtonMapEdit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMapDialog dlg;

	dlg.m_sMapID =	m_sTaxiStation.dwMAPID;

	if ( dlg.DoModal() )
	{
		m_sTaxiStation.dwMAPID = dlg.m_sMapID.dwID;
		SetWin_Num_int( this, IDC_EDIT_MAP_MID, dlg.m_sMapID.wMainID );
		SetWin_Num_int( this, IDC_EDIT_MAP_SID, dlg.m_sMapID.wSubID );
	}

}

void CTaxiData::OnBnClickedButtonNpcEdit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CNpcDialog dlg;

	dlg.m_sNpcID =	m_sTaxiStation.dwNPCID;

	if ( dlg.DoModal() )
	{
		m_sTaxiStation.dwNPCID = dlg.m_sNpcID.dwID;
		SetWin_Num_int( this, IDC_EDIT_NPC_MID, dlg.m_sNpcID.wMainID );
		SetWin_Num_int( this, IDC_EDIT_NPC_SID, dlg.m_sNpcID.wSubID );
	}
}

void CTaxiData::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_sTaxiStation.bUsed = GetWin_Check( this, IDC_CHECK_USE );

	if ( m_sTaxiStation.dwMAPID == UINT_MAX ) 
	{
		MessageBox ( "The Map was not selected.", "Caution", MB_OK );
		return;
	}

	m_sTaxiStation.wPosX = GetWin_Num_int ( this, IDC_EDIT_MAP_XPOS );
	m_sTaxiStation.wPosY = GetWin_Num_int ( this, IDC_EDIT_MAP_YPOS );

	if ( m_sTaxiStation.dwNPCID == UINT_MAX ) 
	{
		MessageBox ( "The Map was not selected.", "Caution", MB_OK );
		return;
	}

	m_sTaxiStation.dwMapCharge = GetWin_Num_int ( this, IDC_EDIT_CHARGE );
	m_sTaxiStation.strMAP = GetWin_Text( this, IDC_EDIT_MAP_NAME );
	m_sTaxiStation.strSTATION = GetWin_Text( this, IDC_EDIT_NPC_NAME );

	if ( m_sTaxiStation.strMAP.empty() )
	{
		MessageBox ( "The MapName was not input.", "Caution", MB_OK );
		return;
	}

	if ( m_sTaxiStation.strSTATION.empty() )
	{
		MessageBox ( "The NpcName was not input.", "Caution", MB_OK );
		return;
	}

	OnOK();
}
