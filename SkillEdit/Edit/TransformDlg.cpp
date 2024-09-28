// TransformDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"

#include "../SkillEdit.h"
#include "../../EngineLib/DxEffect/Char/DxEffCharData.h"
#include "../../MfcExLib/ExLibs/EtcFunction.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinPieceContainer.h"
#include "./TransformDlg.h"


// CTransformDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CTransformDlg, CDialog)
CTransformDlg::CTransformDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTransformDlg::IDD, pParent)
{
	m_sSPECIAL_SKILL.ResetAll();
}



CTransformDlg::~CTransformDlg()
{
}

void CTransformDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTransformDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_M_OPEN1, OnBnClickedButtonMOpen1)
	ON_BN_CLICKED(IDC_BUTTON_M_OPEN2, OnBnClickedButtonMOpen2)
	ON_BN_CLICKED(IDC_BUTTON_M_OPEN3, OnBnClickedButtonMOpen3)
	ON_BN_CLICKED(IDC_BUTTON_M_OPEN4, OnBnClickedButtonMOpen4)
	ON_BN_CLICKED(IDC_BUTTON_M_OPEN5, OnBnClickedButtonMOpen5)
	ON_BN_CLICKED(IDC_BUTTON_M_OPEN6, OnBnClickedButtonMOpen6)
	ON_BN_CLICKED(IDC_BUTTON_M_OPEN7, OnBnClickedButtonMOpen7)
	ON_BN_CLICKED(IDC_BUTTON_M_OPEN8, OnBnClickedButtonMOpen8)
	ON_BN_CLICKED(IDC_BUTTON_W_OPEN1, OnBnClickedButtonWOpen1)
	ON_BN_CLICKED(IDC_BUTTON_W_OPEN2, OnBnClickedButtonWOpen2)
	ON_BN_CLICKED(IDC_BUTTON_W_OPEN3, OnBnClickedButtonWOpen3)
	ON_BN_CLICKED(IDC_BUTTON_W_OPEN4, OnBnClickedButtonWOpen4)
	ON_BN_CLICKED(IDC_BUTTON_W_OPEN5, OnBnClickedButtonWOpen5)
	ON_BN_CLICKED(IDC_BUTTON_W_OPEN6, OnBnClickedButtonWOpen6)
	ON_BN_CLICKED(IDC_BUTTON_W_OPEN7, OnBnClickedButtonWOpen7)
	ON_BN_CLICKED(IDC_BUTTON_W_OPEN8, OnBnClickedButtonWOpen8)


	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_SELFZONEEFFECT4, OnBnClickedButtonSelfzoneeffect4)
END_MESSAGE_MAP()


// CTransformDlg �޽��� ó�����Դϴ�.

BOOL CTransformDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	for( BYTE i = 0; i < SKILL::EMTRANSFORM_NSIZE; i++ )
	{
		SetWin_Text( this, IDC_EDIT_M_PART1+i, m_sSPECIAL_SKILL.strTransform_Man[i].c_str() );
		SetWin_Text( this, IDC_EDIT_W_PART1+i, m_sSPECIAL_SKILL.strTransform_Woman[i].c_str() );
	}

	SetWin_Num_int( this, IDC_EDIT_ENDTIMEEFFECT, m_sSPECIAL_SKILL.dwRemainSecond );
	SetWin_Text( this, IDC_EDIT_SELFZONEEFFECT3, m_sSPECIAL_SKILL.strEffectName.c_str() );



	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


void CTransformDlg::OnBnClickedButtonMOpen1()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "Cps File (*.cps)|*.cps|";

	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(TRUE,".cps",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		this);

	dlg.m_ofn.lpstrInitialDir = DxSkinPieceContainer::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text( this, IDC_EDIT_M_PART1, dlg.GetFileName().GetString() );		
	}
}

void CTransformDlg::OnBnClickedButtonMOpen2()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "Character Piece File (*.cps)|*.cps|";

	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(TRUE,".cps",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		this);

	dlg.m_ofn.lpstrInitialDir = DxSkinPieceContainer::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text( this, IDC_EDIT_M_PART2, dlg.GetFileName().GetString() );		
	}
}

void CTransformDlg::OnBnClickedButtonMOpen3()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "Character Piece File (*.cps)|*.cps|";

	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(TRUE,".cps",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		this);

	dlg.m_ofn.lpstrInitialDir = DxSkinPieceContainer::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text( this, IDC_EDIT_M_PART3, dlg.GetFileName().GetString() );		
	}
}

void CTransformDlg::OnBnClickedButtonMOpen4()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "Character Piece File (*.cps)|*.cps|";

	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(TRUE,".cps",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		this);

	dlg.m_ofn.lpstrInitialDir = DxSkinPieceContainer::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text( this, IDC_EDIT_M_PART4, dlg.GetFileName().GetString() );		
	}
}

void CTransformDlg::OnBnClickedButtonMOpen5()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "Character Piece File (*.cps)|*.cps|";

	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(TRUE,".cps",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		this);

	dlg.m_ofn.lpstrInitialDir = DxSkinPieceContainer::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text( this, IDC_EDIT_M_PART5, dlg.GetFileName().GetString() );		
	}
}

void CTransformDlg::OnBnClickedButtonMOpen6()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "Character Piece File (*.cps)|*.cps|";

	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(TRUE,".cps",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		this);

	dlg.m_ofn.lpstrInitialDir = DxSkinPieceContainer::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text( this, IDC_EDIT_M_PART6, dlg.GetFileName().GetString() );		
	}
}

void CTransformDlg::OnBnClickedButtonMOpen7()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "Character Piece File (*.cps)|*.cps|";

	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(TRUE,".cps",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		this);

	dlg.m_ofn.lpstrInitialDir = DxSkinPieceContainer::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text( this, IDC_EDIT_M_PART7, dlg.GetFileName().GetString() );		
	}
}

void CTransformDlg::OnBnClickedButtonMOpen8()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "Character Piece File (*.cps)|*.cps|";

	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(TRUE,".cps",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		this);

	dlg.m_ofn.lpstrInitialDir = DxSkinPieceContainer::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text( this, IDC_EDIT_M_PART8, dlg.GetFileName().GetString() );		
	}
}

void CTransformDlg::OnBnClickedButtonWOpen1()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "Cps File (*.cps)|*.cps|";

	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(TRUE,".cps",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		this);

	dlg.m_ofn.lpstrInitialDir = DxSkinPieceContainer::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text( this, IDC_EDIT_W_PART1, dlg.GetFileName().GetString() );		
	}
}

void CTransformDlg::OnBnClickedButtonWOpen2()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "Character Piece File (*.cps)|*.cps|";

	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(TRUE,".cps",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		this);

	dlg.m_ofn.lpstrInitialDir = DxSkinPieceContainer::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text( this, IDC_EDIT_W_PART2, dlg.GetFileName().GetString() );		
	}
}

void CTransformDlg::OnBnClickedButtonWOpen3()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "Character Piece File (*.cps)|*.cps|";

	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(TRUE,".cps",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		this);

	dlg.m_ofn.lpstrInitialDir = DxSkinPieceContainer::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text( this, IDC_EDIT_W_PART3, dlg.GetFileName().GetString() );		
	}
}

void CTransformDlg::OnBnClickedButtonWOpen4()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "Character Piece File (*.cps)|*.cps|";

	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(TRUE,".cps",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		this);

	dlg.m_ofn.lpstrInitialDir = DxSkinPieceContainer::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text( this, IDC_EDIT_W_PART4, dlg.GetFileName().GetString() );		
	}
}

void CTransformDlg::OnBnClickedButtonWOpen5()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "Character Piece File (*.cps)|*.cps|";

	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(TRUE,".cps",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		this);

	dlg.m_ofn.lpstrInitialDir = DxSkinPieceContainer::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text( this, IDC_EDIT_W_PART5, dlg.GetFileName().GetString() );		
	}
}

void CTransformDlg::OnBnClickedButtonWOpen6()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "Character Piece File (*.cps)|*.cps|";

	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(TRUE,".cps",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		this);

	dlg.m_ofn.lpstrInitialDir = DxSkinPieceContainer::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text( this, IDC_EDIT_W_PART6, dlg.GetFileName().GetString() );		
	}
}

void CTransformDlg::OnBnClickedButtonWOpen7()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "Character Piece File (*.cps)|*.cps|";

	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(TRUE,".cps",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		this);

	dlg.m_ofn.lpstrInitialDir = DxSkinPieceContainer::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text( this, IDC_EDIT_W_PART7, dlg.GetFileName().GetString() );		
	}
}

void CTransformDlg::OnBnClickedButtonWOpen8()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "Character Piece File (*.cps)|*.cps|";

	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(TRUE,".cps",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		this);

	dlg.m_ofn.lpstrInitialDir = DxSkinPieceContainer::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text( this, IDC_EDIT_W_PART8, dlg.GetFileName().GetString() );		
	}
}


void CTransformDlg::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	for( BYTE i = 0; i < SKILL::EMTRANSFORM_NSIZE; i++ )
	{
		m_sSPECIAL_SKILL.strTransform_Man[i]   = GetWin_Text( this, IDC_EDIT_M_PART1+i );
		m_sSPECIAL_SKILL.strTransform_Woman[i] = GetWin_Text( this, IDC_EDIT_W_PART1+i );
	}

	m_sSPECIAL_SKILL.dwRemainSecond = GetWin_Num_int( this, IDC_EDIT_ENDTIMEEFFECT );
	m_sSPECIAL_SKILL.strEffectName  = GetWin_Text( this, IDC_EDIT_SELFZONEEFFECT3 );

	OnOK();
}

void CTransformDlg::OnBnClickedButtonSelfzoneeffect4()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "Effect File (*.egp)|*.egp|";

	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(TRUE,".egp",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CTransformDlg*)this);

	dlg.m_ofn.lpstrInitialDir = DxEffSinglePropGMan::GetInstance().GetPath().c_str();
	if ( dlg.DoModal() == IDOK )
	{
		GetDlgItem ( IDC_EDIT_SELFZONEEFFECT3 )->SetWindowText( dlg.GetFileName().GetString() );		
	}
}
