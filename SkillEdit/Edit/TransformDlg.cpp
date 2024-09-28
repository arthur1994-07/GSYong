// TransformDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "../SkillEdit.h"
#include "../../EngineLib/DxEffect/Char/DxEffCharData.h"
#include "../../MfcExLib/ExLibs/EtcFunction.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinPieceContainer.h"
#include "./TransformDlg.h"


// CTransformDlg 대화 상자입니다.

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


// CTransformDlg 메시지 처리기입니다.

BOOL CTransformDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	for( BYTE i = 0; i < SKILL::EMTRANSFORM_NSIZE; i++ )
	{
		SetWin_Text( this, IDC_EDIT_M_PART1+i, m_sSPECIAL_SKILL.strTransform_Man[i].c_str() );
		SetWin_Text( this, IDC_EDIT_W_PART1+i, m_sSPECIAL_SKILL.strTransform_Woman[i].c_str() );
	}

	SetWin_Num_int( this, IDC_EDIT_ENDTIMEEFFECT, m_sSPECIAL_SKILL.dwRemainSecond );
	SetWin_Text( this, IDC_EDIT_SELFZONEEFFECT3, m_sSPECIAL_SKILL.strEffectName.c_str() );



	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CTransformDlg::OnBnClickedButtonMOpen1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Cps File (*.cps)|*.cps|";

	//	Note : 파일 오픈 다이알로그를 만듬.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Character Piece File (*.cps)|*.cps|";

	//	Note : 파일 오픈 다이알로그를 만듬.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Character Piece File (*.cps)|*.cps|";

	//	Note : 파일 오픈 다이알로그를 만듬.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Character Piece File (*.cps)|*.cps|";

	//	Note : 파일 오픈 다이알로그를 만듬.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Character Piece File (*.cps)|*.cps|";

	//	Note : 파일 오픈 다이알로그를 만듬.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Character Piece File (*.cps)|*.cps|";

	//	Note : 파일 오픈 다이알로그를 만듬.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Character Piece File (*.cps)|*.cps|";

	//	Note : 파일 오픈 다이알로그를 만듬.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Character Piece File (*.cps)|*.cps|";

	//	Note : 파일 오픈 다이알로그를 만듬.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Cps File (*.cps)|*.cps|";

	//	Note : 파일 오픈 다이알로그를 만듬.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Character Piece File (*.cps)|*.cps|";

	//	Note : 파일 오픈 다이알로그를 만듬.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Character Piece File (*.cps)|*.cps|";

	//	Note : 파일 오픈 다이알로그를 만듬.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Character Piece File (*.cps)|*.cps|";

	//	Note : 파일 오픈 다이알로그를 만듬.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Character Piece File (*.cps)|*.cps|";

	//	Note : 파일 오픈 다이알로그를 만듬.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Character Piece File (*.cps)|*.cps|";

	//	Note : 파일 오픈 다이알로그를 만듬.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Character Piece File (*.cps)|*.cps|";

	//	Note : 파일 오픈 다이알로그를 만듬.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Character Piece File (*.cps)|*.cps|";

	//	Note : 파일 오픈 다이알로그를 만듬.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Effect File (*.egp)|*.egp|";

	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".egp",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CTransformDlg*)this);

	dlg.m_ofn.lpstrInitialDir = DxEffSinglePropGMan::GetInstance().GetPath().c_str();
	if ( dlg.DoModal() == IDOK )
	{
		GetDlgItem ( IDC_EDIT_SELFZONEEFFECT3 )->SetWindowText( dlg.GetFileName().GetString() );		
	}
}
