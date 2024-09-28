// WearingDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../ItemEdit.h"
#include "WearingDlg.h"
#include "../../MfcExLib/ExLibs/EtcFunction.h"
#include "../../Enginelib/DxMeshs/SkinMesh/DxSkinPieceContainer.h"
#include "../../RanLogic/Item/GLItemDef.h"
#include "../../RanLogic/Item/GlItem.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"

// CWearingDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CWearingDlg, CDialog)
CWearingDlg::CWearingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWearingDlg::IDD, pParent)
	, m_emSuit( SUIT_HEADGEAR )
{
}

CWearingDlg::~CWearingDlg()
{
}

void CWearingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CWearingDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_WEARINGFILE_C1, OnBnClickedButtonWearingfileC1)
	ON_BN_CLICKED(IDC_BUTTON_WEARINGFILE_C2, OnBnClickedButtonWearingfileC2)
	ON_BN_CLICKED(IDC_BUTTON_WEARINGFILE_C3, OnBnClickedButtonWearingfileC3)
	ON_BN_CLICKED(IDC_BUTTON_WEARINGFILE_C4, OnBnClickedButtonWearingfileC4)
	ON_BN_CLICKED(IDC_BUTTON_WEARINGFILE_C5, OnBnClickedButtonWearingfileC5)
	ON_BN_CLICKED(IDC_BUTTON_WEARINGFILE_C6, OnBnClickedButtonWearingfileC6)
	ON_BN_CLICKED(IDC_BUTTON_WEARINGFILE_C7, OnBnClickedButtonWearingfileC7)
	ON_BN_CLICKED(IDC_BUTTON_WEARINGFILE_C8, OnBnClickedButtonWearingfileC8)
	ON_BN_CLICKED(IDC_BUTTON_WEARINGFILE_C9, OnBnClickedButtonWearingfileC9)
	ON_BN_CLICKED(IDC_BUTTON_WEARINGFILE_C10, OnBnClickedButtonWearingfileC10)
	ON_BN_CLICKED(IDC_BUTTON_WEARINGFILE_C11, OnBnClickedButtonWearingfileC11)
	ON_BN_CLICKED(IDC_BUTTON_WEARINGFILE_C12, OnBnClickedButtonWearingfileC12)
	ON_BN_CLICKED(IDC_BUTTON_WEARINGFILE_C13, OnBnClickedButtonWearingfileC13)
    ON_BN_CLICKED(IDC_BUTTON_WEARINGFILE_C14, OnBnClickedButtonWearingfileC14)
    ON_BN_CLICKED(IDC_BUTTON_WEARINGFILE_C15, OnBnClickedButtonWearingfileC15)
    ON_BN_CLICKED(IDC_BUTTON_WEARINGFILE_C16, OnBnClickedButtonWearingfileC16)
    ON_BN_CLICKED(IDC_BUTTON_WEARINGFILE_C17, OnBnClickedButtonWearingfileC17)

	ON_BN_CLICKED(IDC_BUTTON_WEARINGFILE_C20, OnBnClickedButtonWearingfileC20)
	ON_BN_CLICKED(IDC_BUTTON_WEARINGFILE_C21, OnBnClickedButtonWearingfileC21)

	ON_BN_CLICKED(IDC_BUTTON_WEARINGFILE_EX_C1, OnBnClickedButtonWearingfileExC1)
	ON_BN_CLICKED(IDC_BUTTON_WEARINGFILE_EX_C2, OnBnClickedButtonWearingfileExC2)
	ON_BN_CLICKED(IDC_BUTTON_WEARINGFILE_EX_C3, OnBnClickedButtonWearingfileExC3)
	ON_BN_CLICKED(IDC_BUTTON_WEARINGFILE_EX_C4, OnBnClickedButtonWearingfileExC4)
	ON_BN_CLICKED(IDC_BUTTON_WEARINGFILE_EX_C5, OnBnClickedButtonWearingfileExC5)
	ON_BN_CLICKED(IDC_BUTTON_WEARINGFILE_EX_C6, OnBnClickedButtonWearingfileExC6)
	ON_BN_CLICKED(IDC_BUTTON_WEARINGFILE_EX_C7, OnBnClickedButtonWearingfileExC7)
	ON_BN_CLICKED(IDC_BUTTON_WEARINGFILE_EX_C8, OnBnClickedButtonWearingfileExC8)
	ON_BN_CLICKED(IDC_BUTTON_WEARINGFILE_EX_C9, OnBnClickedButtonWearingfileExC9)
	ON_BN_CLICKED(IDC_BUTTON_WEARINGFILE_EX_C10, OnBnClickedButtonWearingfileExC10)
	ON_BN_CLICKED(IDC_BUTTON_WEARINGFILE_EX_C12, OnBnClickedButtonWearingfileExC12)
	ON_BN_CLICKED(IDC_BUTTON_WEARINGFILE_EX_C13, OnBnClickedButtonWearingfileExC13)
    ON_BN_CLICKED(IDC_BUTTON_WEARINGFILE_EX_C14, OnBnClickedButtonWearingfileExC14)
    ON_BN_CLICKED(IDC_BUTTON_WEARINGFILE_EX_C15, OnBnClickedButtonWearingfileExC15)
    ON_BN_CLICKED(IDC_BUTTON_WEARINGFILE_EX_C16, OnBnClickedButtonWearingfileExC16)
    ON_BN_CLICKED(IDC_BUTTON_WEARINGFILE_EX_C17, OnBnClickedButtonWearingfileExC17)

	ON_BN_CLICKED(IDC_BUTTON_WEARINGFILE_EX_C20, OnBnClickedButtonWearingfileExC20)
	ON_BN_CLICKED(IDC_BUTTON_WEARINGFILE_EX_C21, OnBnClickedButtonWearingfileExC21)

END_MESSAGE_MAP()


BOOL CWearingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();	

	UpdateList();

	SetReadMode();

	
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

// CWearingDlg 메시지 처리기입니다.

void CWearingDlg::UpdateList()
{
	if( m_emSuit >= SUIT_VEHICLE_PARTS_A &&
		m_emSuit <= SUIT_VEHICLE_PARTS_F )
	{
		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			if( m_emVehicleType == VEHICLE_TYPE_BIKE )
			{
				SetWin_Text (this, IDC_BUTTON_WEARINGFILE_C1, CGameTextMan::GetInstance().GetCommentText("BIKETYPE", 0));
				SetWin_Text (this, IDC_BUTTON_WEARINGFILE_C2, CGameTextMan::GetInstance().GetCommentText("BIKETYPE", 1));
				SetWin_Text (this, IDC_BUTTON_WEARINGFILE_C3, CGameTextMan::GetInstance().GetCommentText("BIKETYPE", 2));
				SetWin_Text (this, IDC_BUTTON_WEARINGFILE_C4, CGameTextMan::GetInstance().GetCommentText("BIKETYPE", 3));
				SetWin_Text (this, IDC_BUTTON_WEARINGFILE_C5, CGameTextMan::GetInstance().GetCommentText("BIKETYPE", 4));
				SetWin_Text (this, IDC_BUTTON_WEARINGFILE_C6, CGameTextMan::GetInstance().GetCommentText("BIKETYPE", 5));
				SetWin_Text (this, IDC_BUTTON_WEARINGFILE_C7, CGameTextMan::GetInstance().GetCommentText("BIKETYPE", 6));
				SetWin_Text (this, IDC_BUTTON_WEARINGFILE_C8, CGameTextMan::GetInstance().GetCommentText("BIKETYPE", 7));
				SetWin_Text (this, IDC_BUTTON_WEARINGFILE_C9, CGameTextMan::GetInstance().GetCommentText("BIKETYPE", 8));
				SetWin_Text (this, IDC_BUTTON_WEARINGFILE_C10,CGameTextMan::GetInstance().GetCommentText("BIKETYPE", 9));
			}
			else if( m_emVehicleType == VEHICLE_TYPE_BOARD )
			{
				SetWin_Text ( this, IDC_BUTTON_WEARINGFILE_C1, CGameTextMan::GetInstance().GetCommentText("BOARDTYPE", 0));
				SetWin_Text ( this, IDC_BUTTON_WEARINGFILE_C2, CGameTextMan::GetInstance().GetCommentText("BOARDTYPE", 1));
				SetWin_Text ( this, IDC_BUTTON_WEARINGFILE_C3, CGameTextMan::GetInstance().GetCommentText("BOARDTYPE", 2));
				SetWin_Text ( this, IDC_BUTTON_WEARINGFILE_C4, CGameTextMan::GetInstance().GetCommentText("BOARDTYPE", 3));
				SetWin_Text ( this, IDC_BUTTON_WEARINGFILE_C5, CGameTextMan::GetInstance().GetCommentText("BOARDTYPE", 4));
				SetWin_Text ( this, IDC_BUTTON_WEARINGFILE_C6, CGameTextMan::GetInstance().GetCommentText("BOARDTYPE", 5));
				SetWin_Text ( this, IDC_BUTTON_WEARINGFILE_C7, CGameTextMan::GetInstance().GetCommentText("BOARDTYPE", 6));
				SetWin_Text ( this, IDC_BUTTON_WEARINGFILE_C8, CGameTextMan::GetInstance().GetCommentText("BOARDTYPE", 7));
				SetWin_Text ( this, IDC_BUTTON_WEARINGFILE_C9, CGameTextMan::GetInstance().GetCommentText("BOARDTYPE", 8));
				SetWin_Text ( this, IDC_BUTTON_WEARINGFILE_C10,CGameTextMan::GetInstance().GetCommentText("BOARDTYPE", 9) );
			}

		}
		else
		{
			if( m_emVehicleType == VEHICLE_TYPE_BIKE )
			{
				SetWin_Text ( this, IDC_BUTTON_WEARINGFILE_C1, COMMENT::BIKETYPE[ 0 ].c_str() );
				SetWin_Text ( this, IDC_BUTTON_WEARINGFILE_C2, COMMENT::BIKETYPE[ 1 ].c_str() );
				SetWin_Text ( this, IDC_BUTTON_WEARINGFILE_C3, COMMENT::BIKETYPE[ 2 ].c_str() );
				SetWin_Text ( this, IDC_BUTTON_WEARINGFILE_C4, COMMENT::BIKETYPE[ 3 ].c_str() );
				SetWin_Text ( this, IDC_BUTTON_WEARINGFILE_C5, COMMENT::BIKETYPE[ 4 ].c_str() );
				SetWin_Text ( this, IDC_BUTTON_WEARINGFILE_C6, COMMENT::BIKETYPE[ 5 ].c_str() );
				SetWin_Text ( this, IDC_BUTTON_WEARINGFILE_C7, COMMENT::BIKETYPE[ 6 ].c_str() );
				SetWin_Text ( this, IDC_BUTTON_WEARINGFILE_C8, COMMENT::BIKETYPE[ 7 ].c_str() );
				SetWin_Text ( this, IDC_BUTTON_WEARINGFILE_C9, COMMENT::BIKETYPE[ 8 ].c_str() );
				SetWin_Text ( this, IDC_BUTTON_WEARINGFILE_C10, COMMENT::BIKETYPE[ 9 ].c_str() );
			}
			else if( m_emVehicleType == VEHICLE_TYPE_BOARD )
			{
				SetWin_Text ( this, IDC_BUTTON_WEARINGFILE_C1, COMMENT::BOARDTYPE[ 0 ].c_str() );
				SetWin_Text ( this, IDC_BUTTON_WEARINGFILE_C2, COMMENT::BOARDTYPE[ 1 ].c_str() );
				SetWin_Text ( this, IDC_BUTTON_WEARINGFILE_C3, COMMENT::BOARDTYPE[ 2 ].c_str() );
				SetWin_Text ( this, IDC_BUTTON_WEARINGFILE_C4, COMMENT::BOARDTYPE[ 3 ].c_str() );
				SetWin_Text ( this, IDC_BUTTON_WEARINGFILE_C5, COMMENT::BOARDTYPE[ 4 ].c_str() );
				SetWin_Text ( this, IDC_BUTTON_WEARINGFILE_C6, COMMENT::BOARDTYPE[ 5 ].c_str() );
				SetWin_Text ( this, IDC_BUTTON_WEARINGFILE_C7, COMMENT::BOARDTYPE[ 6 ].c_str() );
				SetWin_Text ( this, IDC_BUTTON_WEARINGFILE_C8, COMMENT::BOARDTYPE[ 7 ].c_str() );
				SetWin_Text ( this, IDC_BUTTON_WEARINGFILE_C9, COMMENT::BOARDTYPE[ 8 ].c_str() );
				SetWin_Text ( this, IDC_BUTTON_WEARINGFILE_C10, COMMENT::BOARDTYPE[ 9 ].c_str() );
			}
		}

		SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C1, m_strWearingFile[0].c_str() ); // 격투남 	
		SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C2, m_strWearingFile[1].c_str() ); // 격투여
		SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C3, m_strWearingFile[2].c_str() ); // 검도남
		SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C4, m_strWearingFile[3].c_str() ); // 검도여
		SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C5, m_strWearingFile[4].c_str() ); // 양궁남
		SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C6, m_strWearingFile[5].c_str() ); // 양궁여	
		SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C7, m_strWearingFile[6].c_str() ); // 기예남
		SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C8, m_strWearingFile[7].c_str() ); // 기예여
		SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C9, m_strWearingFile[8].c_str() ); // 극강남
		SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C10, m_strWearingFile[9].c_str() );	//	극강여
		SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C12, m_strWearingFile[10].c_str() );	// 과학남
		SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C13, m_strWearingFile[11].c_str() );	// 과학여
		SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C14, m_strWearingFile[12].c_str() );	// 인술남
		SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C15, m_strWearingFile[13].c_str() );	// 인술여
		SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C16, m_strWearingFile[14].c_str() );	// 마술남
		SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C17, m_strWearingFile[15].c_str() );	// 마술여
		SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C18, m_strWearingFile[16].c_str() );	// 기타남
		SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C19, m_strWearingFile[17].c_str() );	// 기타여
		SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C20, m_strWearingFile[18].c_str() );	// 연극남
		SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C21, m_strWearingFile[19].c_str() );	// 연극여
		
		SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C11, m_strPetWearingFile.c_str() );	
	}
	else
	{
		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			SetWin_Text ( this, IDC_BUTTON_WEARINGFILE_C1, CGameTextMan::GetInstance().GetCommentText("CHARCLASS", GLCI_FIGHTER_M)); // 격투남;
			SetWin_Text ( this, IDC_BUTTON_WEARINGFILE_C2, CGameTextMan::GetInstance().GetCommentText("CHARCLASS", GLCI_FIGHTER_W)); // 격투여;
			SetWin_Text ( this, IDC_BUTTON_WEARINGFILE_C3, CGameTextMan::GetInstance().GetCommentText("CHARCLASS", GLCI_ARMS_M)); // 검도남;
			SetWin_Text ( this, IDC_BUTTON_WEARINGFILE_C4, CGameTextMan::GetInstance().GetCommentText("CHARCLASS", GLCI_ARMS_W)); // 검도여;
			SetWin_Text ( this, IDC_BUTTON_WEARINGFILE_C5, CGameTextMan::GetInstance().GetCommentText("CHARCLASS", GLCI_ARCHER_M)); // 양궁남;
			SetWin_Text ( this, IDC_BUTTON_WEARINGFILE_C6, CGameTextMan::GetInstance().GetCommentText("CHARCLASS", GLCI_ARCHER_W)); // 양궁여;	
			SetWin_Text ( this, IDC_BUTTON_WEARINGFILE_C7, CGameTextMan::GetInstance().GetCommentText("CHARCLASS", GLCI_SPIRIT_M)); // 기예남;
			SetWin_Text ( this, IDC_BUTTON_WEARINGFILE_C8, CGameTextMan::GetInstance().GetCommentText("CHARCLASS", GLCI_SPIRIT_W)); // 기예여;
			SetWin_Text ( this, IDC_BUTTON_WEARINGFILE_C9, CGameTextMan::GetInstance().GetCommentText("CHARCLASS", GLCI_EXTREME_M)); // 극강남;
			SetWin_Text ( this, IDC_BUTTON_WEARINGFILE_C10, CGameTextMan::GetInstance().GetCommentText("CHARCLASS", GLCI_EXTREME_W)); // 극강여;
		}
		else
		{
			SetWin_Text ( this, IDC_BUTTON_WEARINGFILE_C1, COMMENT::CHARCLASS[ GLCI_FIGHTER_M ].c_str() ); // 격투남 	
			SetWin_Text ( this, IDC_BUTTON_WEARINGFILE_C2, COMMENT::CHARCLASS[ GLCI_FIGHTER_W ].c_str()  ); // 격투여
			SetWin_Text ( this, IDC_BUTTON_WEARINGFILE_C3, COMMENT::CHARCLASS[ GLCI_ARMS_M ].c_str()  ); // 검도남
			SetWin_Text ( this, IDC_BUTTON_WEARINGFILE_C4, COMMENT::CHARCLASS[ GLCI_ARMS_W ].c_str()  ); // 검도여
			SetWin_Text ( this, IDC_BUTTON_WEARINGFILE_C5, COMMENT::CHARCLASS[ GLCI_ARCHER_M ].c_str()  ); // 양궁남
			SetWin_Text ( this, IDC_BUTTON_WEARINGFILE_C6, COMMENT::CHARCLASS[ GLCI_ARCHER_W ].c_str()  ); // 양궁여	
			SetWin_Text ( this, IDC_BUTTON_WEARINGFILE_C7, COMMENT::CHARCLASS[ GLCI_SPIRIT_M ].c_str()  ); // 기예남
			SetWin_Text ( this, IDC_BUTTON_WEARINGFILE_C8, COMMENT::CHARCLASS[ GLCI_SPIRIT_W ].c_str()  ); // 기예여
			SetWin_Text ( this, IDC_BUTTON_WEARINGFILE_C9, COMMENT::CHARCLASS[ GLCI_EXTREME_M ].c_str()  ); // 극강남
			SetWin_Text ( this, IDC_BUTTON_WEARINGFILE_C10, COMMENT::CHARCLASS[ GLCI_EXTREME_W ].c_str()  ); // 극강여
		}

		SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C1, m_strWearingFile[GLCI_FIGHTER_M].c_str() ); // 격투남 	
		SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C2, m_strWearingFile[GLCI_FIGHTER_W].c_str() ); // 격투여
		SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C3, m_strWearingFile[GLCI_ARMS_M].c_str() ); // 검도남
		SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C4, m_strWearingFile[GLCI_ARMS_W].c_str() ); // 검도여
		SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C5, m_strWearingFile[GLCI_ARCHER_M].c_str() ); // 양궁남
		SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C6, m_strWearingFile[GLCI_ARCHER_W].c_str() ); // 양궁여	
		SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C7, m_strWearingFile[GLCI_SPIRIT_M].c_str() ); // 기예남
		SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C8, m_strWearingFile[GLCI_SPIRIT_W].c_str() ); // 기예여

		SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C9, m_strWearingFile[GLCI_EXTREME_M].c_str() ); // 극강남
		SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C10, m_strWearingFile[GLCI_EXTREME_W].c_str() );	//	극강여
		SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C12, m_strWearingFile[GLCI_SCIENTIST_M].c_str() );	// 과학남
		SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C13, m_strWearingFile[GLCI_SCIENTIST_W].c_str() );	// 과학여
        SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C14, m_strWearingFile[GLCI_ASSASSIN_M].c_str() );	// 인술남
        SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C15, m_strWearingFile[GLCI_ASSASSIN_W].c_str() );	// 인술여
		SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C16, m_strWearingFile[GLCI_TRICKER_M].c_str() );	// 마술남
		SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C17, m_strWearingFile[GLCI_TRICKER_W].c_str() );	// 마술여
		SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C18, m_strWearingFile[GLCI_ETC_M].c_str() );	// 기타남
		SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C19, m_strWearingFile[GLCI_ETC_W].c_str() );	// 기타여
		SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C20, m_strWearingFile[GLCI_ACTOR_M].c_str() );	// 연극남
		SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C21, m_strWearingFile[GLCI_ACTOR_W].c_str() );	// 연극여

		SetWin_Text ( this, IDC_EDIT_WEARINGFILE_C11, m_strPetWearingFile.c_str() );	
	}

	SetWin_Text ( this, IDC_EDIT_WEARINGFILE_EX_C1, m_strWearingFileEx[GLCI_FIGHTER_M].c_str() ); // 격투남 	
	SetWin_Text ( this, IDC_EDIT_WEARINGFILE_EX_C2, m_strWearingFileEx[GLCI_FIGHTER_W].c_str() ); // 격투여
	SetWin_Text ( this, IDC_EDIT_WEARINGFILE_EX_C3, m_strWearingFileEx[GLCI_ARMS_M].c_str() ); // 검도남
	SetWin_Text ( this, IDC_EDIT_WEARINGFILE_EX_C4, m_strWearingFileEx[GLCI_ARMS_W].c_str() ); // 검도여
	SetWin_Text ( this, IDC_EDIT_WEARINGFILE_EX_C5, m_strWearingFileEx[GLCI_ARCHER_M].c_str() ); // 양궁남
	SetWin_Text ( this, IDC_EDIT_WEARINGFILE_EX_C6, m_strWearingFileEx[GLCI_ARCHER_W].c_str() ); // 양궁여	
	SetWin_Text ( this, IDC_EDIT_WEARINGFILE_EX_C7, m_strWearingFileEx[GLCI_SPIRIT_M].c_str() ); // 기예남
	SetWin_Text ( this, IDC_EDIT_WEARINGFILE_EX_C8, m_strWearingFileEx[GLCI_SPIRIT_W].c_str() ); // 기예여

	SetWin_Text ( this, IDC_EDIT_WEARINGFILE_EX_C9, m_strWearingFileEx[GLCI_EXTREME_M].c_str() ); // 극강남
	SetWin_Text ( this, IDC_EDIT_WEARINGFILE_EX_C10, m_strWearingFileEx[GLCI_EXTREME_W].c_str() );	//	극강여
	SetWin_Text ( this, IDC_EDIT_WEARINGFILE_EX_C12, m_strWearingFileEx[GLCI_SCIENTIST_M].c_str() );	// 과학남
	SetWin_Text ( this, IDC_EDIT_WEARINGFILE_EX_C13, m_strWearingFileEx[GLCI_SCIENTIST_W].c_str() );	// 과학여
    SetWin_Text ( this, IDC_EDIT_WEARINGFILE_EX_C14, m_strWearingFileEx[GLCI_ASSASSIN_M].c_str() );	// 인술남
    SetWin_Text ( this, IDC_EDIT_WEARINGFILE_EX_C15, m_strWearingFileEx[GLCI_ASSASSIN_W].c_str() );	// 인술여
	SetWin_Text ( this, IDC_EDIT_WEARINGFILE_EX_C16, m_strWearingFileEx[GLCI_TRICKER_M].c_str() );	// 마술남
	SetWin_Text ( this, IDC_EDIT_WEARINGFILE_EX_C17, m_strWearingFileEx[GLCI_TRICKER_W].c_str() );	// 마술여
	SetWin_Text ( this, IDC_EDIT_WEARINGFILE_EX_C18, m_strWearingFileEx[GLCI_ETC_M].c_str() );	// 기타남
	SetWin_Text ( this, IDC_EDIT_WEARINGFILE_EX_C19, m_strWearingFileEx[GLCI_ETC_W].c_str() );	// 기타여
	SetWin_Text ( this, IDC_EDIT_WEARINGFILE_EX_C20, m_strWearingFileEx[GLCI_ACTOR_M].c_str() );	// 연극남
	SetWin_Text ( this, IDC_EDIT_WEARINGFILE_EX_C21, m_strWearingFileEx[GLCI_ACTOR_W].c_str() );	// 연극여

	SetWin_Text ( this, IDC_EDIT_SUBICONFILE_C1, m_strArrInventoryFile[GLCI_FIGHTER_M].c_str() ); // 격투남 	
	SetWin_Text ( this, IDC_EDIT_SUBICONFILE_C2, m_strArrInventoryFile[GLCI_FIGHTER_W].c_str() ); // 격투여
	SetWin_Text ( this, IDC_EDIT_SUBICONFILE_C3, m_strArrInventoryFile[GLCI_ARMS_M].c_str() ); // 검도남
	SetWin_Text ( this, IDC_EDIT_SUBICONFILE_C4, m_strArrInventoryFile[GLCI_ARMS_W].c_str() ); // 검도여
	SetWin_Text ( this, IDC_EDIT_SUBICONFILE_C5, m_strArrInventoryFile[GLCI_ARCHER_M].c_str() ); // 양궁남
	SetWin_Text ( this, IDC_EDIT_SUBICONFILE_C6, m_strArrInventoryFile[GLCI_ARCHER_W].c_str() ); // 양궁여	
	SetWin_Text ( this, IDC_EDIT_SUBICONFILE_C7, m_strArrInventoryFile[GLCI_SPIRIT_M].c_str() ); // 기예남
	SetWin_Text ( this, IDC_EDIT_SUBICONFILE_C8, m_strArrInventoryFile[GLCI_SPIRIT_W].c_str() ); // 기예여

	SetWin_Text ( this, IDC_EDIT_SUBICONFILE_C9, m_strArrInventoryFile[GLCI_EXTREME_M].c_str() ); // 극강남
	SetWin_Text ( this, IDC_EDIT_SUBICONFILE_C10, m_strArrInventoryFile[GLCI_EXTREME_W].c_str() );	//	극강여
	SetWin_Text ( this, IDC_EDIT_SUBICONFILE_C12, m_strArrInventoryFile[GLCI_SCIENTIST_M].c_str() );	// 과학남
	SetWin_Text ( this, IDC_EDIT_SUBICONFILE_C13, m_strArrInventoryFile[GLCI_SCIENTIST_W].c_str() );	// 과학여
	SetWin_Text ( this, IDC_EDIT_SUBICONFILE_C14, m_strArrInventoryFile[GLCI_ASSASSIN_M].c_str() );	// 인술남
	SetWin_Text ( this, IDC_EDIT_SUBICONFILE_C15, m_strArrInventoryFile[GLCI_ASSASSIN_W].c_str() );	// 인술여
	SetWin_Text ( this, IDC_EDIT_SUBICONFILE_C16, m_strArrInventoryFile[GLCI_TRICKER_M].c_str() );	// 마술남
	SetWin_Text ( this, IDC_EDIT_SUBICONFILE_C17, m_strArrInventoryFile[GLCI_TRICKER_W].c_str() );	// 마술여
	SetWin_Text ( this, IDC_EDIT_SUBICONFILE_C18, m_strArrInventoryFile[GLCI_ETC_M].c_str() );	// 기타남
	SetWin_Text ( this, IDC_EDIT_SUBICONFILE_C19, m_strArrInventoryFile[GLCI_ETC_W].c_str() );	// 기타여
	SetWin_Text ( this, IDC_EDIT_SUBICONFILE_C20, m_strArrInventoryFile[GLCI_ACTOR_M].c_str() );	// 연극남
	SetWin_Text ( this, IDC_EDIT_SUBICONFILE_C21, m_strArrInventoryFile[GLCI_ACTOR_W].c_str() );	// 연극여
}

void CWearingDlg::OnBnClickedOk()
{
	if( m_emSuit >= SUIT_VEHICLE_PARTS_A &&
		m_emSuit <= SUIT_VEHICLE_PARTS_F )
	{
		m_strWearingFile[ 0 ] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_C1 ).GetString();
		m_strWearingFile[ 1 ] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_C2 ).GetString();
		m_strWearingFile[ 2 ] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_C3 ).GetString();
		m_strWearingFile[ 3 ] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_C4 ).GetString();
		m_strWearingFile[ 4 ] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_C5 ).GetString();
		m_strWearingFile[ 5 ] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_C6 ).GetString();
		m_strWearingFile[ 6 ] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_C7 ).GetString();
		m_strWearingFile[ 7 ] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_C8 ).GetString();
		m_strWearingFile[ 8 ] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_C9 ).GetString();
		m_strWearingFile[ 9 ] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_C10 ).GetString();
	}
	else
	{
		m_strWearingFile[GLCI_FIGHTER_M] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_C1 ).GetString();
		m_strWearingFile[GLCI_FIGHTER_W] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_C2 ).GetString();
		m_strWearingFile[GLCI_ARMS_M] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_C3 ).GetString();
		m_strWearingFile[GLCI_ARMS_W] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_C4 ).GetString();
		m_strWearingFile[GLCI_ARCHER_M] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_C5 ).GetString();
		m_strWearingFile[GLCI_ARCHER_W] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_C6 ).GetString();
		m_strWearingFile[GLCI_SPIRIT_M] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_C7 ).GetString();	
		m_strWearingFile[GLCI_SPIRIT_W] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_C8 ).GetString();	
		m_strWearingFile[GLCI_EXTREME_M] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_C9 ).GetString();
		m_strWearingFile[GLCI_EXTREME_W] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_C10 ).GetString();
		m_strWearingFile[GLCI_SCIENTIST_M] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_C12 ).GetString();
		m_strWearingFile[GLCI_SCIENTIST_W] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_C13 ).GetString();
        m_strWearingFile[GLCI_ASSASSIN_M] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_C14 ).GetString();
        m_strWearingFile[GLCI_ASSASSIN_W] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_C15 ).GetString();
        m_strWearingFile[GLCI_TRICKER_M] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_C16 ).GetString();
        m_strWearingFile[GLCI_TRICKER_W] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_C17 ).GetString();
		m_strWearingFile[GLCI_ETC_M] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_C18 ).GetString();
		m_strWearingFile[GLCI_ETC_W] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_C19 ).GetString();
		m_strWearingFile[GLCI_ACTOR_M] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_C20 ).GetString();
		m_strWearingFile[GLCI_ACTOR_W] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_C21 ).GetString();
	}

	m_strWearingFileEx[GLCI_FIGHTER_M] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_EX_C1 ).GetString();
	m_strWearingFileEx[GLCI_FIGHTER_W] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_EX_C2 ).GetString();
	m_strWearingFileEx[GLCI_ARMS_M] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_EX_C3 ).GetString();
	m_strWearingFileEx[GLCI_ARMS_W] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_EX_C4 ).GetString();
	m_strWearingFileEx[GLCI_ARCHER_M] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_EX_C5 ).GetString();
	m_strWearingFileEx[GLCI_ARCHER_W] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_EX_C6 ).GetString();
	m_strWearingFileEx[GLCI_SPIRIT_M] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_EX_C7 ).GetString();	
	m_strWearingFileEx[GLCI_SPIRIT_W] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_EX_C8 ).GetString();	
	m_strWearingFileEx[GLCI_EXTREME_M] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_EX_C9 ).GetString();
	m_strWearingFileEx[GLCI_EXTREME_W] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_EX_C10 ).GetString();
	m_strWearingFileEx[GLCI_SCIENTIST_M] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_EX_C12 ).GetString();
	m_strWearingFileEx[GLCI_SCIENTIST_W] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_EX_C13 ).GetString();
    m_strWearingFileEx[GLCI_ASSASSIN_M] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_EX_C14 ).GetString();
    m_strWearingFileEx[GLCI_ASSASSIN_W] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_EX_C15 ).GetString();
	m_strWearingFileEx[GLCI_TRICKER_M] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_EX_C16 ).GetString();
	m_strWearingFileEx[GLCI_TRICKER_W] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_EX_C17 ).GetString();
    m_strWearingFileEx[GLCI_ETC_M] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_EX_C18 ).GetString();
    m_strWearingFileEx[GLCI_ETC_W] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_EX_C19 ).GetString();
	m_strWearingFileEx[GLCI_ACTOR_M] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_EX_C20 ).GetString();
	m_strWearingFileEx[GLCI_ACTOR_W] = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_EX_C21 ).GetString();

	m_strArrInventoryFile[GLCI_FIGHTER_M] = GetWin_Text ( this, IDC_EDIT_SUBICONFILE_C1 ).GetString();
	m_strArrInventoryFile[GLCI_FIGHTER_W] = GetWin_Text ( this, IDC_EDIT_SUBICONFILE_C2 ).GetString();
	m_strArrInventoryFile[GLCI_ARMS_M] = GetWin_Text ( this, IDC_EDIT_SUBICONFILE_C3 ).GetString();
	m_strArrInventoryFile[GLCI_ARMS_W] = GetWin_Text ( this, IDC_EDIT_SUBICONFILE_C4 ).GetString();
	m_strArrInventoryFile[GLCI_ARCHER_M] = GetWin_Text ( this, IDC_EDIT_SUBICONFILE_C5 ).GetString();
	m_strArrInventoryFile[GLCI_ARCHER_W] = GetWin_Text ( this, IDC_EDIT_SUBICONFILE_C6 ).GetString();
	m_strArrInventoryFile[GLCI_SPIRIT_M] = GetWin_Text ( this, IDC_EDIT_SUBICONFILE_C7 ).GetString();	
	m_strArrInventoryFile[GLCI_SPIRIT_W] = GetWin_Text ( this, IDC_EDIT_SUBICONFILE_C8 ).GetString();	
	m_strArrInventoryFile[GLCI_EXTREME_M] = GetWin_Text ( this, IDC_EDIT_SUBICONFILE_C9 ).GetString();
	m_strArrInventoryFile[GLCI_EXTREME_W] = GetWin_Text ( this, IDC_EDIT_SUBICONFILE_C10 ).GetString();
	m_strArrInventoryFile[GLCI_SCIENTIST_M] = GetWin_Text ( this, IDC_EDIT_SUBICONFILE_C12 ).GetString();
	m_strArrInventoryFile[GLCI_SCIENTIST_W] = GetWin_Text ( this, IDC_EDIT_SUBICONFILE_C13 ).GetString();
	m_strArrInventoryFile[GLCI_ASSASSIN_M] = GetWin_Text ( this, IDC_EDIT_SUBICONFILE_C14 ).GetString();
	m_strArrInventoryFile[GLCI_ASSASSIN_W] = GetWin_Text ( this, IDC_EDIT_SUBICONFILE_C15 ).GetString();
	m_strArrInventoryFile[GLCI_TRICKER_M] = GetWin_Text ( this, IDC_EDIT_SUBICONFILE_C16 ).GetString();
	m_strArrInventoryFile[GLCI_TRICKER_W] = GetWin_Text ( this, IDC_EDIT_SUBICONFILE_C17 ).GetString();
	m_strArrInventoryFile[GLCI_ETC_M] = GetWin_Text ( this, IDC_EDIT_SUBICONFILE_C18 ).GetString();
	m_strArrInventoryFile[GLCI_ETC_W] = GetWin_Text ( this, IDC_EDIT_SUBICONFILE_C19 ).GetString();
	m_strArrInventoryFile[GLCI_ACTOR_M] = GetWin_Text ( this, IDC_EDIT_SUBICONFILE_C20 ).GetString();
	m_strArrInventoryFile[GLCI_ACTOR_W] = GetWin_Text ( this, IDC_EDIT_SUBICONFILE_C21 ).GetString();

	m_strPetWearingFile = GetWin_Text ( this, IDC_EDIT_WEARINGFILE_C11 );

	OnOK();
}

void CWearingDlg::LoadWearingfile( int nID )
{
	// 호버보드는 착용 아이템이없음;
	// 탈것에 장착하는 아이템일 경우;
	if(  m_emVehicleType != VEHICLE_TYPE_BOARD && 
		m_emSuit >= SUIT_VEHICLE_PARTS_A && 
		m_emSuit <= SUIT_VEHICLE_PARTS_F )
	{
		CString szFilter = "Wearing File (*.vps)|*.vps";

		//	Note : 파일 오픈 다이알로그를 만듬.
		CFileDialog dlg(TRUE,".vps",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
			(CWearingDlg*)this);

		dlg.m_ofn.lpstrInitialDir = DxSkinPieceContainer::GetInstance().GetPath ();
		if ( dlg.DoModal() == IDOK )
		{
			SetWin_Text ( this, nID, dlg.GetFileName().GetString() );
		}
	}

	// 탈것 & 호버보드를 제외한 착용 오브젝트;
	else if( m_emVehicleType != VEHICLE_TYPE_BOARD &&
		m_emSuit == SUIT_VEHICLE )
	{
		CString szFilter = "Wearing File (*.vcf)|*.vcf";

		//	Note : 파일 오픈 다이알로그를 만듬.
		CFileDialog dlg(TRUE,".vcf",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
			(CWearingDlg*)this);

		dlg.m_ofn.lpstrInitialDir = DxSkinPieceContainer::GetInstance().GetPath ();
		if ( dlg.DoModal() == IDOK )
		{
			SetWin_Text ( this, nID, dlg.GetFileName().GetString() );
		}
	}
	// 호버보드 및 캐릭터 착용 아이템;
	else
	{
		CString szFilter = "Wearing File (*.cps;*.abl)|*.cps;*.abl|";

		//	Note : 파일 오픈 다이알로그를 만듬.
		CFileDialog dlg(TRUE,".cps",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
			(CWearingDlg*)this);

		dlg.m_ofn.lpstrInitialDir = DxSkinPieceContainer::GetInstance().GetPath ();
		if ( dlg.DoModal() == IDOK )
		{
			SetWin_Text ( this, nID, dlg.GetFileName().GetString() );
		}
	}
}



void CWearingDlg::OnBnClickedButtonWearingfileC1()
{
	LoadWearingfile( IDC_EDIT_WEARINGFILE_C1 );
}

void CWearingDlg::OnBnClickedButtonWearingfileC2()
{
	LoadWearingfile( IDC_EDIT_WEARINGFILE_C2 );
}

void CWearingDlg::OnBnClickedButtonWearingfileC3()
{
	LoadWearingfile( IDC_EDIT_WEARINGFILE_C3 );
}

void CWearingDlg::OnBnClickedButtonWearingfileC4()
{
	LoadWearingfile( IDC_EDIT_WEARINGFILE_C4 );
}

void CWearingDlg::OnBnClickedButtonWearingfileC5()
{
	LoadWearingfile( IDC_EDIT_WEARINGFILE_C5 );
}

void CWearingDlg::OnBnClickedButtonWearingfileC6()
{
	LoadWearingfile( IDC_EDIT_WEARINGFILE_C6 );
}

void CWearingDlg::OnBnClickedButtonWearingfileC7()
{
	LoadWearingfile( IDC_EDIT_WEARINGFILE_C7 );
}

void CWearingDlg::OnBnClickedButtonWearingfileC8()
{
	LoadWearingfile( IDC_EDIT_WEARINGFILE_C8 );
}


void CWearingDlg::OnBnClickedButtonWearingfileC9()
{
	LoadWearingfile( IDC_EDIT_WEARINGFILE_C9 );
}

void CWearingDlg::OnBnClickedButtonWearingfileC10()
{
	LoadWearingfile( IDC_EDIT_WEARINGFILE_C10 );
}

void CWearingDlg::OnBnClickedButtonWearingfileC11()
{
	LoadWearingfile( IDC_EDIT_WEARINGFILE_C11 );
}

void CWearingDlg::OnBnClickedButtonWearingfileC12()
{
	LoadWearingfile( IDC_EDIT_WEARINGFILE_C12 );
}

void CWearingDlg::OnBnClickedButtonWearingfileC13()
{
	LoadWearingfile( IDC_EDIT_WEARINGFILE_C13 );
}

void CWearingDlg::OnBnClickedButtonWearingfileC14()
{
    LoadWearingfile( IDC_EDIT_WEARINGFILE_C14 );
}

void CWearingDlg::OnBnClickedButtonWearingfileC15()
{
    LoadWearingfile( IDC_EDIT_WEARINGFILE_C15 );
}

void CWearingDlg::OnBnClickedButtonWearingfileC16()
{
    LoadWearingfile( IDC_EDIT_WEARINGFILE_C16 );
}

void CWearingDlg::OnBnClickedButtonWearingfileC17()
{
    LoadWearingfile( IDC_EDIT_WEARINGFILE_C17 );
}

void CWearingDlg::OnBnClickedButtonWearingfileC18()
{
	LoadWearingfile( IDC_EDIT_WEARINGFILE_C18 );
}

void CWearingDlg::OnBnClickedButtonWearingfileC19()
{
	LoadWearingfile( IDC_EDIT_WEARINGFILE_C19 );
}

void CWearingDlg::OnBnClickedButtonWearingfileC20()
{
	LoadWearingfile( IDC_EDIT_WEARINGFILE_C20 );
}

void CWearingDlg::OnBnClickedButtonWearingfileC21()
{
	LoadWearingfile( IDC_EDIT_WEARINGFILE_C21 );
}

void CWearingDlg::SetReadMode ()
{
//  읽기전용 모드일때만 실행한다. 
//  리소스 추가시 수정 요망 
#ifdef READTOOL_PARAM

	const int nSkipNum =21;
	const int nSkipID[nSkipNum] = { IDOK, IDCANCEL };

	int nID = 0;
	bool bOK = false;

	CWnd* pChildWnd = GetWindow(GW_CHILD);

	while (pChildWnd)
	{
		bOK = false;
		nID = pChildWnd->GetDlgCtrlID();	

		for ( int i = 0; i < nSkipNum; ++i )
		{
			if ( nID == nSkipID[i] )
			{
				bOK = true;	
				break;
			}
		}

		if ( !bOK )	pChildWnd->EnableWindow( FALSE );

		pChildWnd = pChildWnd->GetWindow(GW_HWNDNEXT);
	}

#endif


}
INT_PTR CWearingDlg::DoModal()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	//UpdateList();

	return CDialog::DoModal();
}

void CWearingDlg::OnBnClickedButtonWearingfileExC1()
{
	LoadWearingfile( IDC_EDIT_WEARINGFILE_EX_C1 );
}

void CWearingDlg::OnBnClickedButtonWearingfileExC2()
{
	LoadWearingfile( IDC_EDIT_WEARINGFILE_EX_C2 );
}

void CWearingDlg::OnBnClickedButtonWearingfileExC3()
{
	LoadWearingfile( IDC_EDIT_WEARINGFILE_EX_C3 );
}

void CWearingDlg::OnBnClickedButtonWearingfileExC4()
{
	LoadWearingfile( IDC_EDIT_WEARINGFILE_EX_C4 );
}

void CWearingDlg::OnBnClickedButtonWearingfileExC5()
{
	LoadWearingfile( IDC_EDIT_WEARINGFILE_EX_C5 );
}

void CWearingDlg::OnBnClickedButtonWearingfileExC6()
{
	LoadWearingfile( IDC_EDIT_WEARINGFILE_EX_C6 );
}

void CWearingDlg::OnBnClickedButtonWearingfileExC7()
{
	LoadWearingfile( IDC_EDIT_WEARINGFILE_EX_C7 );
}

void CWearingDlg::OnBnClickedButtonWearingfileExC8()
{
	LoadWearingfile( IDC_EDIT_WEARINGFILE_EX_C8 );
}

void CWearingDlg::OnBnClickedButtonWearingfileExC9()
{
	LoadWearingfile( IDC_EDIT_WEARINGFILE_EX_C9 );
}

void CWearingDlg::OnBnClickedButtonWearingfileExC10()
{
	LoadWearingfile( IDC_EDIT_WEARINGFILE_EX_C10 );
}

void CWearingDlg::OnBnClickedButtonWearingfileExC12()
{
	LoadWearingfile( IDC_EDIT_WEARINGFILE_EX_C12 );
}

void CWearingDlg::OnBnClickedButtonWearingfileExC13()
{
	LoadWearingfile( IDC_EDIT_WEARINGFILE_EX_C13 );
}

void CWearingDlg::OnBnClickedButtonWearingfileExC14()
{
    LoadWearingfile( IDC_EDIT_WEARINGFILE_EX_C14 );
}

void CWearingDlg::OnBnClickedButtonWearingfileExC15()
{
    LoadWearingfile( IDC_EDIT_WEARINGFILE_EX_C15 );
}

void CWearingDlg::OnBnClickedButtonWearingfileExC16()
{
    LoadWearingfile( IDC_EDIT_WEARINGFILE_EX_C16 );
}

void CWearingDlg::OnBnClickedButtonWearingfileExC17()
{
    LoadWearingfile( IDC_EDIT_WEARINGFILE_EX_C17 );
}

void CWearingDlg::OnBnClickedButtonWearingfileExC18()
{
	LoadWearingfile( IDC_EDIT_WEARINGFILE_EX_C18 );
}
void CWearingDlg::OnBnClickedButtonWearingfileExC19()
{
	LoadWearingfile( IDC_EDIT_WEARINGFILE_EX_C19 );
}

void CWearingDlg::OnBnClickedButtonWearingfileExC20()
{
	LoadWearingfile( IDC_EDIT_WEARINGFILE_EX_C20 );
}
void CWearingDlg::OnBnClickedButtonWearingfileExC21()
{
	LoadWearingfile( IDC_EDIT_WEARINGFILE_EX_C21 );
}