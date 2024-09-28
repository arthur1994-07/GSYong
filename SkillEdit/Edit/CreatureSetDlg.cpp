// CreatureSetDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "../SkillEdit.h"
#include "../../EngineLib/DxEffect/Char/DxEffCharData.h"
#include "../../MfcExLib/ExLibs/EtcFunction.h"
#include "./CreatureSetDlg.h"


// CCreatureSetDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CCreatureSetDlg, CDialog)
CCreatureSetDlg::CCreatureSetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCreatureSetDlg::IDD, pParent)
{
	m_sSPECIAL_SKILL.ResetAll();
	m_nSelectLevel = 0;
}

CCreatureSetDlg::~CCreatureSetDlg()
{
}

void CCreatureSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCreatureSetDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_GATEEFFECT, OnBnClickedButtonGateeffect)
END_MESSAGE_MAP()


// CCreatureSetDlg 메시지 처리기입니다.
BOOL CCreatureSetDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if ( m_nSelectLevel < 0 )
	{
		m_nSelectLevel = 0;
	}
	else if ( m_nSelectLevel >= SKILL::MAX_LEVEL )
	{
		m_nSelectLevel = SKILL::MAX_LEVEL - 1;
	}

	// Note : 사용할 스킬 레벨+1을 한다.
	SetWin_Num_int( this, IDC_EDIT_USESKILLLEVEL,  m_sSPECIAL_SKILL.sCREATURE_LVL[m_nSelectLevel].wUSESKILLLEVEL + 1 );

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	SetWin_Num_float( this, IDC_EDIT_RECALLDELAY,  m_sSPECIAL_SKILL.sCREATURE_LVL[m_nSelectLevel].fSEC_RECALLDELAY );
	SetWin_Num_float( this, IDC_EDIT_ACTIVITYTIME, m_sSPECIAL_SKILL.sCREATURE_LVL[m_nSelectLevel].fSEC_ACTIVETIME );
	SetWin_Num_int( this, IDC_EDIT_CREATUREMID,    m_sSPECIAL_SKILL.sCREATURE_LVL[m_nSelectLevel].sID.wMainID );
	SetWin_Num_int( this, IDC_EDIT_CREATURESID,    m_sSPECIAL_SKILL.sCREATURE_LVL[m_nSelectLevel].sID.wSubID );
	SetWin_Num_int( this, IDC_EDIT_MAXCREATURE,    m_sSPECIAL_SKILL.sCREATURE_LVL[m_nSelectLevel].dwMAX );
	SetWin_Num_int( this, IDC_EDIT_AITYPE,			m_sSPECIAL_SKILL.sCREATURE_LVL[m_nSelectLevel].dwType );
	m_sSPECIAL_SKILL.sCREATURE_LVL[m_nSelectLevel].bEventSkill ?
		CheckDlgButton( IDC_CHECK_ENABLE_EVENT_SKILL, TRUE ) :
		CheckDlgButton( IDC_CHECK_ENABLE_EVENT_SKILL, FALSE );
	SetWin_Text( this, IDC_EDIT_GATEEFFECT, m_sSPECIAL_SKILL.strEffectName.c_str() );


	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CCreatureSetDlg::OnBnClickedOk()
{
	// Note : 사용할 스킬 레벨에 -1을 한다.
	WORD wUseSkillLevel = GetWin_Num_int( this, IDC_EDIT_USESKILLLEVEL );

	if ( wUseSkillLevel <= 0 )
	{
		wUseSkillLevel = 1;
	}
	else if ( wUseSkillLevel >= SKILL::MAX_LEVEL )
	{
		wUseSkillLevel = SKILL::MAX_LEVEL - 1;
	}
	else
	{
		wUseSkillLevel = wUseSkillLevel - 1;
	}

	m_sSPECIAL_SKILL.sCREATURE_LVL[m_nSelectLevel].wUSESKILLLEVEL = wUseSkillLevel;

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_sSPECIAL_SKILL.sCREATURE_LVL[m_nSelectLevel].fSEC_RECALLDELAY  = GetWin_Num_float( this, IDC_EDIT_RECALLDELAY );
	m_sSPECIAL_SKILL.sCREATURE_LVL[m_nSelectLevel].fSEC_ACTIVETIME   = GetWin_Num_float( this, IDC_EDIT_ACTIVITYTIME );
	m_sSPECIAL_SKILL.sCREATURE_LVL[m_nSelectLevel].sID.wMainID       = GetWin_Num_int( this, IDC_EDIT_CREATUREMID );
	m_sSPECIAL_SKILL.sCREATURE_LVL[m_nSelectLevel].sID.wSubID        = GetWin_Num_int( this, IDC_EDIT_CREATURESID );
	m_sSPECIAL_SKILL.sCREATURE_LVL[m_nSelectLevel].dwMAX             = GetWin_Num_int( this, IDC_EDIT_MAXCREATURE );
	m_sSPECIAL_SKILL.sCREATURE_LVL[m_nSelectLevel].dwType             = GetWin_Num_int( this, IDC_EDIT_AITYPE );
	m_sSPECIAL_SKILL.sCREATURE_LVL[m_nSelectLevel].bEventSkill		= static_cast< bool >( IsDlgButtonChecked( IDC_CHECK_ENABLE_EVENT_SKILL ) );
	m_sSPECIAL_SKILL.strEffectName                                   = GetWin_Text( this, IDC_EDIT_GATEEFFECT );

	OnOK();
}

void CCreatureSetDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnCancel();
}

void CCreatureSetDlg::OnBnClickedButtonGateeffect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Effect File (*.egp)|*.egp|";

	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".egp",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CCreatureSetDlg*)this);

	dlg.m_ofn.lpstrInitialDir = DxEffSinglePropGMan::GetInstance().GetPath().c_str();
	if ( dlg.DoModal() == IDOK )
	{
		GetDlgItem ( IDC_EDIT_GATEEFFECT )->SetWindowText( dlg.GetFileName().GetString() );		
	}
}
