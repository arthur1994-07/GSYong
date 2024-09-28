// LearnSkillDialog.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"

#include "../NpcAction.h"
#include "./SkillDialog.h"
#include "../../RanLogic/Skill/GLSkill.h"
#include "../../MfcExLib/ExLibs/EtcFunction.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// CLearnSkillDialog ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CSkillDialog, CDialog)
CSkillDialog::CSkillDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSkillDialog::IDD, pParent)
{
	m_bConfirm = FALSE;
}

CSkillDialog::~CSkillDialog()
{
}

void CSkillDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSkillDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_FIND, OnBnClickedButtonFind)
END_MESSAGE_MAP()


// CLearnSkillDialog �޽��� ó�����Դϴ�.

void CSkillDialog::OnBnClickedButtonOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_bConfirm = TRUE;

	CComboBox* pComboBox = (CComboBox*) GetDlgItem ( IDC_COMBO_SKILL_LEVEL );
	int nIndex = pComboBox->GetCurSel ();
	if ( nIndex==LB_ERR )	return;

	m_Condition.wLevel = (WORD) pComboBox->GetItemData ( nIndex );

	CListBox * pListBox = (CListBox *) GetDlgItem ( IDC_LIST_SKILL );
	nIndex = pListBox->GetCurSel ();
	if ( nIndex==LB_ERR )	return;

	m_Condition.dwNID = (DWORD) pListBox->GetItemData ( nIndex );

	OnOK ();
}

SSkillCondition	CSkillDialog::GetSkillCondition ()
{
	assert ( m_bConfirm && "�����Ⱚ �Ѿ�ϴ�." );

	return m_Condition;
}

void CSkillDialog::OnBnClickedButtonCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.	
	OnCancel ();
}

BOOL CSkillDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_bConfirm = FALSE;

	//	���� ������ �巯�� �����۵� ��� �ε��ϱ�
	CString strTemp;

	CComboBox* pComboBox = (CComboBox*) GetDlgItem ( IDC_COMBO_SKILL_LEVEL );
	for ( int i = 1; i <= SKILL::MAX_LEVEL; i++ )
	{
		strTemp.Format ( "%d", i );
		int nIndex = pComboBox->AddString ( strTemp );
		pComboBox->SetItemData ( nIndex, i );
	}
	pComboBox->SetCurSel ( 0 );

	CListBox * pListBox = (CListBox *) GetDlgItem ( IDC_LIST_SKILL );
	for ( WORD i=0; i<EMSKILLCLASS_NSIZE; ++i )
	for ( WORD j=0; j<GLSkillMan::MAX_CLASSSKILL; ++j )
	{
		PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( i, j );
		if ( !pSkill )	continue;

		CString strTemp;
		strTemp.Format ( "[%03d/%03d] %s", pSkill->m_sBASIC.sNATIVEID.wMainID,
			pSkill->m_sBASIC.sNATIVEID.wSubID, pSkill->GetName() );
		
		int nIndex = pListBox->AddString ( strTemp.GetString() );
		pListBox->SetItemData ( nIndex, pSkill->m_sBASIC.sNATIVEID.dwID );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CSkillDialog::OnBnClickedButtonFind()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int nMainID, nSubID;

	nMainID = GetWin_Num_int( this, IDC_EDIT_FINDMID );
	nSubID = GetWin_Num_int( this, IDC_EDIT_FINDSID );

	CListBox* pListBox = (CListBox*) GetDlgItem ( IDC_LIST_SKILL );
	int nSize = pListBox->GetCount();

	SNATIVEID sNativeID;

	for ( int i = 0; i < nSize; ++i )
	{
		sNativeID = (DWORD) pListBox->GetItemData( i );

		if ( nMainID == sNativeID.wMainID && nSubID == sNativeID.wSubID )
		{
			pListBox->SetCurSel( i );
			return;
		}
	}
}
