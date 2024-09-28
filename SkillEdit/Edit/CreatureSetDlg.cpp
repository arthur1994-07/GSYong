// CreatureSetDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"

#include "../SkillEdit.h"
#include "../../EngineLib/DxEffect/Char/DxEffCharData.h"
#include "../../MfcExLib/ExLibs/EtcFunction.h"
#include "./CreatureSetDlg.h"


// CCreatureSetDlg ��ȭ �����Դϴ�.

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


// CCreatureSetDlg �޽��� ó�����Դϴ�.
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

	// Note : ����� ��ų ����+1�� �Ѵ�.
	SetWin_Num_int( this, IDC_EDIT_USESKILLLEVEL,  m_sSPECIAL_SKILL.sCREATURE_LVL[m_nSelectLevel].wUSESKILLLEVEL + 1 );

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
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
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CCreatureSetDlg::OnBnClickedOk()
{
	// Note : ����� ��ų ������ -1�� �Ѵ�.
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

	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	OnCancel();
}

void CCreatureSetDlg::OnBnClickedButtonGateeffect()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "Effect File (*.egp)|*.egp|";

	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(TRUE,".egp",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CCreatureSetDlg*)this);

	dlg.m_ofn.lpstrInitialDir = DxEffSinglePropGMan::GetInstance().GetPath().c_str();
	if ( dlg.DoModal() == IDOK )
	{
		GetDlgItem ( IDC_EDIT_GATEEFFECT )->SetWindowText( dlg.GetFileName().GetString() );		
	}
}
