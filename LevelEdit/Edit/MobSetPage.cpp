// MobSetPage.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "../LevelEdit.h"
#include "../LevelMainFrm.h"
#include "../LevelEditDoc.h"
#include "../LevelEditView.h"

#include "../../EngineLib/DxTools/DxViewPort.h"
#include "../../EngineLib/G-Logic/GLogic.h"

#include "./MobSchedulePage.h"
#include "./LevelSheetWithTab.h"

#include "../../EngineLib/DxTools/DxMethods.h"

#include "afxdlgs.h"

#include "../../RanLogic/Crow/GLMobSchedule.h"
#include "../../RanLogic/Crow/GLCrowDataMan.h"
#include "../../MfcExLib/ExLibs/EtcFunction.h"

#include "afxtempl.h"

#include "./MobSetPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMobSetPage 대화 상자입니다.

GLMobSchedule				CMobSetPage::m_Schedule;
CMobSetPage::EM_EDIT_TYPE	CMobSetPage::m_emEditType = CMobSetPage::EM_EDIT_NONE;
BOOL						CMobSetPage::m_bMaterial = FALSE;
UINT						CMobSetPage::g_unLayer = 0;

typedef std::tr1::shared_ptr<GLMobScheduleManServer> SpMobScheduleManServer;

IMPLEMENT_DYNAMIC(CMobSetPage, CPropertyPage)
CMobSetPage::CMobSetPage() :
	CPropertyPage(CMobSetPage::IDD),
	m_bInit(FALSE)
{
}

CMobSetPage::~CMobSetPage()
{
}

void CMobSetPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MOBSET, m_ctrlMobList);
	DDX_Control(pDX, IDC_LIST_MOBSELECT, m_ctrlMobSelectList);
	DDX_Control(pDX, IDC_COMBO_MOBLAYER, m_comboMobLayer);
	DDX_Control(pDX, IDC_EDIT_ACTIVITYAREA, m_editActivityArea);
}


BEGIN_MESSAGE_MAP(CMobSetPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_CREATE, OnBnClickedButtonCreate)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnBnClickedButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_MODIFY, OnBnClickedButtonModify)
	ON_BN_CLICKED(IDC_BUTTON_SCHEDULE, OnBnClickedButtonSchedule)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_TEST, OnBnClickedButtonTest)
	ON_BN_CLICKED(IDC_BUTTON_CROWID, OnBnClickedButtonCrowid)
	ON_LBN_DBLCLK(IDC_LIST_MOBSET, OnLbnDblclkListMobset)
	ON_BN_CLICKED(IDC_BUTTON_RENDGEN, OnBnClickedButtonRendgen)
	ON_BN_CLICKED(IDC_CHECK_RENDGENPOS, OnBnClickedCheckRendgenpos)
	ON_BN_CLICKED(IDC_BUTTON_STOP, OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_GATEID_PC_REGEN, OnBnClickedButtonGateidPcRegen)
	ON_BN_CLICKED(IDC_BUTTON_BUS_LOCATION, OnBnClickedButtonBusLocation)
	ON_BN_CLICKED(IDC_BUTTON_CSV_EXPORT, OnBnClickedButtonCsvExport)
	ON_BN_CLICKED(IDC_BUTTON_CSV_IMPORT, OnBnClickedButtonCsvImport)
	ON_BN_CLICKED(IDC_WEEK_CHECK1, OnBnClickedWeekCheck)
	ON_BN_CLICKED(IDC_WEEK_CHECK2, OnBnClickedWeekCheck)
	ON_BN_CLICKED(IDC_WEEK_CHECK3, OnBnClickedWeekCheck)
	ON_BN_CLICKED(IDC_WEEK_CHECK4, OnBnClickedWeekCheck)
	ON_BN_CLICKED(IDC_WEEK_CHECK5, OnBnClickedWeekCheck)
	ON_BN_CLICKED(IDC_WEEK_CHECK6, OnBnClickedWeekCheck)
	ON_BN_CLICKED(IDC_WEEK_CHECK7, OnBnClickedWeekCheck)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_RENDGENPOS_NUM, OnDeltaposSpinRendgenposNum)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_RENDGENPOS_DIST, OnDeltaposSpinRendgenposDist)
	ON_BN_CLICKED(IDC_CHECK_LEADER, OnBnClickedCheckLeader)
	ON_BN_CLICKED(IDC_CHECK_GROUP_REGEN, OnBnClickedCheckGroupRegen)
	ON_BN_CLICKED(IDC_BUTTON_MULTIMODIFY, OnBnClickedButtonMultimodify)
	ON_BN_CLICKED(IDC_CHECK_MINIMAP, &CMobSetPage::OnBnClickedCheckMinimap)
	ON_BN_CLICKED(IDC_CHECK_EACH_REGEN, &CMobSetPage::OnBnClickedCheckEachRegen)
	ON_BN_CLICKED(IDC_BUTTON_MOBLAYER_ADD, &CMobSetPage::OnBnClickedButtonMoblayerAdd)
	ON_CBN_SELCHANGE(IDC_COMBO_MOBLAYER, &CMobSetPage::OnCbnSelchangeComboMoblayer)
	ON_BN_CLICKED(IDC_BUTTON_MOBLAYER_DEL, &CMobSetPage::OnBnClickedButtonMoblayerDel)
END_MESSAGE_MAP()


// CMobSetPage 메시지 처리기입니다.
BOOL CMobSetPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	ActiveMobSetEdit( EM_EDIT_NONE );

	m_bInit = TRUE;
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
void CMobSetPage::ActiveMobSetEdit( EM_EDIT_TYPE emType /* = EM_EDIT_NONE */ )
{
	const int nMobSingleSetCtrl[] =
	{
		IDC_STATIC_E,
		IDC_BUTTON_NAME,
		IDC_EDIT_MOBNAME,
		IDC_BUTTON_SCHEDULE,
		IDC_BUTTON_CANCEL,
		IDC_BUTTON_ADD,
		IDC_BUTTON_CROWID,
		IDC_EDIT_MID,
		IDC_EDIT_SID,
		IDC_BUTTON_REGEN,
		IDC_EDIT_REGEN_MIN,
		IDC_EDIT_REGEN_MAX,
        IDC_EDIT_SCALE,
		IDC_EDIT_ACTIVITYAREA,
		IDC_STATIC_SCALE,
		IDC_STATIC_INFLUENCEY,
		IDC_EDIT_INFLUENCEY,

		IDC_EDIT_GROUPNAME,
		IDC_BUTTON_GROUPNAME,
		IDC_CHECK_LEADER,
		IDC_CHECK_GROUP_REGEN,
		IDC_CHECK_EACH_REGEN,

		IDC_WEEK_CHECK1,
		IDC_WEEK_CHECK2,
		IDC_WEEK_CHECK3,
		IDC_WEEK_CHECK4,
		IDC_WEEK_CHECK5,
		IDC_WEEK_CHECK6,
		IDC_WEEK_CHECK7,

		IDC_BUTTON_REGEN2,
		IDC_EDIT_REGEN_TIME1,
		IDC_EDIT_REGEN_TIME2,

		IDC_CHECK_RENDGENPOS,
		IDC_BUTTON_RENDGEN,
		IDC_SPIN_RENDGENPOS_NUM,
		IDC_EDIT_RENDGENPOS_NUM,
		IDC_STATIC_RENDGENPOS_NUM,
		IDC_SPIN_RENDGENPOS_DIST,
		IDC_EDIT_RENDGENPOS_DIST,
		IDC_STATIC_RENDGENPOS_DIST,

		IDC_BUTTON_GATEID_PC_REGEN,
		IDC_EDIT_GATEID_PC_REGEN,

		IDC_BUTTON_BUS_LOCATION,
		IDC_EDIT_BUS_LOCATION,

		IDC_CHECK_MINIMAP,
		IDC_EDIT_ACTIVITYAREA,
		IDC_STATIC_SCALE2
	};

	const int nMobMultiSetCtrl[] =
	{
		IDC_LIST_MOBSELECT,
		IDC_STATIC_E,
		IDC_BUTTON_GROUPNAME,
		IDC_EDIT_GROUPNAME,

		IDC_BUTTON_CANCEL,
		IDC_BUTTON_ADD
	};
	int nSingleCtrl = sizeof(nMobSingleSetCtrl) / sizeof(int);
	int nMultiCtrl = sizeof(nMobMultiSetCtrl) / sizeof(int);



	CWnd *pWnd;
	for ( int i=0; i<nSingleCtrl; i++ )
	{
		pWnd = GetDlgItem(nMobSingleSetCtrl[i]);
		pWnd->ShowWindow ( FALSE );
	}
	for ( int i=0; i<nMultiCtrl; i++ )
	{
		pWnd = GetDlgItem(nMobMultiSetCtrl[i]);
		pWnd->ShowWindow ( FALSE );
	}

	if( emType == EM_EDIT_SINGLE )
	{
		for ( int i=0; i<nSingleCtrl; i++ )
		{
			pWnd = GetDlgItem(nMobSingleSetCtrl[i]);
			pWnd->ShowWindow ( TRUE );
		}	
	}else if( emType == EM_EDIT_MULTIPLE )
	{
		for ( int i=0; i<nMultiCtrl; i++ )
		{
			pWnd = GetDlgItem(nMobMultiSetCtrl[i]);
			pWnd->ShowWindow ( TRUE );
		}
	}

	if( emType == EM_EDIT_MULTIPLE )
	{
		SetWin_Text( this, IDC_STATIC_S, "수정중인 몬스터 리스트" );
	}else{
		SetWin_Text( this, IDC_STATIC_S, "등록된 몹 리스트" );
	}

	pWnd = GetDlgItem ( IDC_BUTTON_CREATE );
	pWnd->EnableWindow ( emType == EM_EDIT_NONE );

	pWnd = GetDlgItem ( IDC_BUTTON_MODIFY );
	pWnd->EnableWindow ( emType == EM_EDIT_NONE );

	pWnd = GetDlgItem ( IDC_BUTTON_MULTIMODIFY );
	pWnd->EnableWindow ( emType == EM_EDIT_NONE );

	pWnd = GetDlgItem ( IDC_BUTTON_DEL );
	pWnd->EnableWindow ( emType == EM_EDIT_NONE );

	SetWin_Enable( this, IDC_EDIT_GROUPNAME, !m_bMaterial );
	SetWin_Enable( this, IDC_BUTTON_GROUPNAME, !m_bMaterial );
	SetWin_Enable( this, IDC_CHECK_LEADER, !m_bMaterial );
	SetWin_Enable( this, IDC_BUTTON_SCHEDULE, !m_bMaterial );

	SetWin_Enable( this, IDC_CHECK_EACH_REGEN, m_Schedule.m_bLeaderMonster );
	SetWin_Enable( this, IDC_CHECK_GROUP_REGEN, m_Schedule.m_bLeaderMonster );

	ListingUpdate ();

	MobLayerUpdate();

	m_emEditType = emType;
}

void CMobSetPage::SetSelectSchedule ( GLMobSchedule* pMobSch )
{
	std::string strSchduleInfo;
	
	if ( pMobSch )
	{
		if( pMobSch->m_strGroupName == "" ) 
			strSchduleInfo = "null";
		else
		{												
			strSchduleInfo = pMobSch->m_strGroupName;
			if( pMobSch->m_bLeaderMonster )
			{
				strSchduleInfo += "*";
			}
		}

		strSchduleInfo += " - ";

		strSchduleInfo += pMobSch->m_szName;

		SetWIn_Multi_ListBox_Sel ( this, IDC_LIST_MOBSET, strSchduleInfo.c_str() );
	}
	else
	{

		m_ctrlMobList.SetSel ( LB_ERR );
	}
}

void CMobSetPage::CutMobSch()
{
	m_bCut = true;

	int nIndex = m_ctrlMobList.GetCurSel();
	if( nIndex == LB_ERR )
	{
		m_strCopyMobName = "";
	}
	else
	{
		CString StrTemp;
		m_ctrlMobList.GetText( nIndex, StrTemp );
		SetWin_ListBox_Sel( this, IDC_LIST_MOBSET, StrTemp );

		char szTemp[64] = {0,};
		strcpy( szTemp, &strstr( StrTemp.GetString(), " - " )[3] );
		m_strCopyMobName = szTemp;
	}

	SetWin_Text( this, IDC_EDIT_COPY_MOBNAME, m_strCopyMobName );
}

void CMobSetPage::CopyMobSch()
{
	m_bCut = false;

	int nIndex = m_ctrlMobList.GetCurSel();
	if( nIndex == LB_ERR )
	{
		m_strCopyMobName = "";
	}
	else
	{
		CString StrTemp;
		m_ctrlMobList.GetText( nIndex, StrTemp );
		SetWin_ListBox_Sel( this, IDC_LIST_MOBSET, StrTemp );

		char szTemp[64] = {0,};
		strcpy( szTemp, &strstr( StrTemp.GetString(), " - " )[3] );
		m_strCopyMobName = szTemp;
	}

	SetWin_Text( this, IDC_EDIT_COPY_MOBNAME, m_strCopyMobName );
}

void CMobSetPage::PasteMobSch()
{
    CLevelMainFrame* pFrame = (CLevelMainFrame*) AfxGetMainWnd();
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	SpMobScheduleManServer spMobSchMan(pGLLandMan->GetMobSchMan());
	NavigationMesh* pNavi = pGLLandMan->GetNavi().get();

	// 기존 자료
	MOBSCHEDULENODE* pNode = spMobSchMan->FindMobSch( m_strCopyMobName.GetString() );
	if( !pNode )
		return;

	CLevelEditView *pView = (CLevelEditView*)pFrame->GetActiveView();

	// 새로운 좌표
	DXAFFINEPARTS Affine;
    Affine = *pNode->Data->m_pAffineParts;
	Affine.vTrans = pView->GetCollisionPos();

	GLMobSchedule* pSchedule = pNode->Data;

	PCROWDATA pCrow = GLCrowDataMan::GetInstance().GetCrowData ( pSchedule->m_CrowID );
	BOOL bMaterial = ( pCrow->m_emCrow == CROW_MATERIAL );
	if ( bMaterial != m_bMaterial )	return;

	if( m_bCut )
	{
		// 새로운 위치, 셋팅, 스케줄/리젠 리스트 좌표 보정
		{
			D3DXVECTOR3 vOffset = pNode->Data->m_pAffineParts->vTrans - Affine.vTrans;

            pNode->Data->SetAffineValue( &Affine );
			pNode->Data->GenerateRendGenPos( pNavi );
			pNode->Data->ModifierSchList( &vOffset, pNavi );
		}

		SetWin_ListBox_Sel( this, IDC_LIST_MOBSET, m_strCopyMobName );
	}
	else
	{
		// 기존 자료 복사
		m_Schedule = *pNode->Data;

		// 리더가 둘 이상인 것을 방지
		m_Schedule.m_bLeaderMonster = FALSE;

		// 새로운 이름 부여;
        // 새로운 이름을 부여할때는 기존의 copy된 몹의 이름뒤에 [숫자] 형식으로 이름을 붙인다.;
		CString strTemp;
		{
			int i=0;
			do
			{
				strTemp.Format( "%s[%d]", m_strCopyMobName, i++ );
			}
			while( spMobSchMan->FindMobSch( strTemp.GetString() ) );

			StringCchCopy( m_Schedule.m_szName, GLMobSchedule::EM_MAXSZNAME, strTemp.GetString() );
		}

		// 새로운 위치 셋팅, 스케줄/리젠 리스트 좌표 보정
		{
			D3DXVECTOR3 vOffset = pNode->Data->m_pAffineParts->vTrans - Affine.vTrans;

			m_Schedule.SetAffineValue( &Affine );
			m_Schedule.GenerateRendGenPos( pNavi );
			m_Schedule.ModifierSchList( &vOffset, pNavi );
		}

		// 스케줄 등록, 다이얼로그 갱신
		{
			spMobSchMan->AddMobSch( m_Schedule );
			UpdatePage();
		}

		SetWin_ListBox_Sel( this, IDC_LIST_MOBSET, strTemp );
	}
}

BOOL CMobSetPage::UpdateDataSchedule ( BOOL bGetData, BOOL bNew )
{
	CWnd *pWnd;
	CButton *pButton;

    CLevelMainFrame* pFrame      = (CLevelMainFrame*) AfxGetMainWnd();
    GLGaeaServer* pGaeaServer    = pFrame->GetGLGaeaServer();
    GLLandMan* pGLLandMan        = pGaeaServer->GetRootMap();
	SpMobScheduleManServer spMobSchMan(pGLLandMan->GetMobSchMan ());
	NavigationMesh* pNavi        = pGLLandMan->GetNavi ().get();

	CString strTemp;
	if ( !bGetData )
	{
		if ( bNew )
		{
			int i=0;
			do
			{
				strTemp.Format ( "CROW[%d]", i++);
			} while ( spMobSchMan->FindMobSch(strTemp.GetString()) );

			strcpy ( m_Schedule.m_szName,strTemp.GetString() );
		}

		pWnd = GetDlgItem ( IDC_EDIT_MOBNAME );
		pWnd->SetWindowText ( m_Schedule.m_szName );
		m_strBackMobName = m_Schedule.m_szName;		//	이름 백업 ( 이름 수정 가능하게 하기 위해 ) 

		strTemp.Format ( "%d", m_Schedule.m_CrowID.wMainID );
		pWnd = GetDlgItem ( IDC_EDIT_MID );
		pWnd->SetWindowText ( strTemp );

		strTemp.Format ( "%d", m_Schedule.m_CrowID.wSubID );
		pWnd = GetDlgItem ( IDC_EDIT_SID );
		pWnd->SetWindowText ( strTemp );

		
		strTemp.Format ( "%d", m_Schedule.m_nRegenHour );
		SetDlgItemText( IDC_EDIT_REGEN_TIME1, strTemp.GetString() );

		strTemp.Format ( "%d", m_Schedule.m_nRegenMin );
		SetDlgItemText( IDC_EDIT_REGEN_TIME2, strTemp.GetString() );

		for( int i = 0; i < 7; i++ )
			SetWin_Check( this, IDC_WEEK_CHECK1 + i, m_Schedule.m_bDayOfWeek[i] );

		if( m_Schedule.GetUseRegenTimeStamp() ) 
		{
			SetWin_Enable( this, IDC_EDIT_REGEN_MIN, FALSE );
			SetWin_Enable( this, IDC_EDIT_REGEN_MAX, FALSE );
		}else{
			SetWin_Enable( this, IDC_EDIT_REGEN_MIN, TRUE );
			SetWin_Enable( this, IDC_EDIT_REGEN_MAX, TRUE );
		}

		strTemp.Format ( "%f", m_Schedule.m_fReGenTimeMin );
		pWnd = GetDlgItem ( IDC_EDIT_REGEN_MIN );
		pWnd->SetWindowText ( strTemp );

		strTemp.Format ( "%f", m_Schedule.m_fReGenTimeMax );
		pWnd = GetDlgItem ( IDC_EDIT_REGEN_MAX );
		pWnd->SetWindowText ( strTemp );

		pButton = (CButton*) GetDlgItem ( IDC_CHECK_RENDGENPOS );
		pButton->SetCheck ( m_Schedule.m_bRendGenPos );

		SetWin_Enable( this, IDC_SPIN_RENDGENPOS_NUM, m_Schedule.m_bRendGenPos );
		SetWin_Num_int( this, IDC_EDIT_RENDGENPOS_NUM, m_Schedule.m_wRendGenPosNum );
		SetWin_Enable( this, IDC_SPIN_RENDGENPOS_DIST, m_Schedule.m_bRendGenPos );
		SetWin_Num_int( this, IDC_EDIT_RENDGENPOS_DIST, m_Schedule.m_wRendGenPosDist );

		pButton = (CButton*) GetDlgItem ( IDC_CHECK_MINIMAP );
		pButton->SetCheck ( m_Schedule.m_dwFlags&GLMobSchedule::EMMF_DISPLAY_MAP_POINT );

		strTemp.Format ( "%d", m_Schedule.m_dwPC_REGEN_GATEID );
		pWnd = GetDlgItem ( IDC_EDIT_GATEID_PC_REGEN );
		pWnd->SetWindowText ( strTemp );

		SetWin_Text ( this, IDC_EDIT_BUS_LOCATION, m_Schedule.m_strBUSLOCATION.c_str() );

		SetWin_Check( this, IDC_CHECK_LEADER, m_Schedule.m_bLeaderMonster );
		SetWin_Check( this, IDC_CHECK_EACH_REGEN, m_Schedule.m_bEachregen );
		SetWin_Check( this, IDC_CHECK_GROUP_REGEN, m_Schedule.m_bGroupRegen );
		SetWin_Text( this, IDC_EDIT_GROUPNAME, m_Schedule.m_strGroupName.c_str() );

        SetWin_Num_float( this, IDC_EDIT_SCALE, m_Schedule.m_fScale );
		
		SetWin_Num_float( this, IDC_EDIT_ACTIVITYAREA, m_Schedule.m_fActivityArea );
		SetWin_Num_float( this, IDC_EDIT_INFLUENCEY, m_Schedule.m_fInfluenceY );

		if( strstr( m_Schedule.m_strGroupName.c_str(), " - " ) != NULL )
		{
			MessageBox ( "ERROR : Invalid group name. Can not use ' - '", "ERROR" );
			return FALSE;
		}

		if( m_Schedule.m_bLeaderMonster )
		{
			GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
			SpMobScheduleManServer spMobSchMan(pGLLandMan->GetMobSchMan());
			const MOBSCHEDULELIST *pMobScheduleList = spMobSchMan->GetMobSchList();
			MOBSCHEDULENODE *pHead = pMobScheduleList->m_pHead;

			while(pHead)
			{

				GLMobSchedule *pSchdule = pHead->Data;
                if( strcmp( pSchdule->m_szName, m_Schedule.m_szName ) == 0 ) 
				{
					pHead = pHead->pNext;
					continue;				
				}

				if( pSchdule->m_strGroupName == m_Schedule.m_strGroupName &&
					pSchdule->m_bLeaderMonster )
				{

					char szTempChar[256];
					sprintf( szTempChar, "Change \"%s\" group leader. %s -> %s", 
									m_Schedule.m_strGroupName.c_str(),  
									pSchdule->m_szName, m_Schedule.m_szName );
					MessageBox( szTempChar, "Message" );
					pHead->Data->m_bLeaderMonster = FALSE;
					break;
				}
				pHead = pHead->pNext;
			}

		}

		return TRUE;
	}
	else
	{
		pWnd = GetDlgItem ( IDC_EDIT_MOBNAME );
		pWnd->GetWindowText ( strTemp );
		
		if ( strTemp.GetLength() > GLMobSchedule::EM_MAXSZNAME )
		{
			MessageBox ( "ERROR : Name of MOB is too long. enter under 64 characters." );
			return FALSE;
		}
		
		strcpy ( m_Schedule.m_szName, strTemp.GetString() );

		pWnd = GetDlgItem ( IDC_EDIT_MID );
		pWnd->GetWindowText ( strTemp );
		m_Schedule.m_CrowID.wMainID = atoi ( strTemp.GetString() );

		pWnd = GetDlgItem ( IDC_EDIT_SID );
		pWnd->GetWindowText ( strTemp );
		m_Schedule.m_CrowID.wSubID = atoi ( strTemp.GetString() );

	
		strTemp = GetWin_Text( this, IDC_EDIT_REGEN_TIME1 );
		m_Schedule.m_nRegenHour = atoi(strTemp.GetString());

		strTemp = GetWin_Text( this, IDC_EDIT_REGEN_TIME2 );
		m_Schedule.m_nRegenMin = atoi(strTemp.GetString());

		for( int i = 0; i < 7; i++ )
			m_Schedule.m_bDayOfWeek[i] = GetWin_Check( this, IDC_WEEK_CHECK1 + i );

		if( !m_Schedule.GetUseRegenTimeStamp() )
		{
			pWnd = GetDlgItem ( IDC_EDIT_REGEN_MIN );
			pWnd->GetWindowText ( strTemp );
			m_Schedule.m_fReGenTimeMin = (float) atof ( strTemp.GetString() );

			pWnd = GetDlgItem ( IDC_EDIT_REGEN_MAX );
			pWnd->GetWindowText ( strTemp );
			m_Schedule.m_fReGenTimeMax = (float) atof ( strTemp.GetString() );
		}

		pButton = (CButton*) GetDlgItem ( IDC_CHECK_RENDGENPOS );
		m_Schedule.m_bRendGenPos = pButton->GetCheck();

		if ( m_Schedule.m_bRendGenPos )
		{
			m_Schedule.m_wRendGenPosNum = GetWin_Num_int( this, IDC_EDIT_RENDGENPOS_NUM );
			if ( m_Schedule.m_wRendGenPosNum > GLMobSchedule::EM_MAXRENDDISTRANGE ) m_Schedule.m_wRendGenPosNum = GLMobSchedule::EM_MAXRENDDISTRANGE;
			if ( m_Schedule.m_wRendGenPosNum < 1 ) m_Schedule.m_wRendGenPosNum = 1;

			m_Schedule.m_wRendGenPosDist = GetWin_Num_int( this, IDC_EDIT_RENDGENPOS_DIST );
			if ( m_Schedule.m_wRendGenPosDist > GLMobSchedule::EM_MAXDISTFACTOR ) m_Schedule.m_wRendGenPosDist = GLMobSchedule::EM_MAXDISTFACTOR;
			if ( m_Schedule.m_wRendGenPosDist < GLMobSchedule::EM_MINDISTFACTOR ) m_Schedule.m_wRendGenPosDist = GLMobSchedule::EM_MINDISTFACTOR;

			if ( m_Schedule.m_vectorRendGenPos.empty() )
				m_Schedule.GenerateRendGenPos(pNavi);
		}

		pButton = (CButton*) GetDlgItem ( IDC_CHECK_MINIMAP );
		if ( pButton->GetCheck() )
		{
			m_Schedule.m_dwFlags |= GLMobSchedule::EMMF_DISPLAY_MAP_POINT;
		}
		else
		{
			m_Schedule.m_dwFlags &= ~GLMobSchedule::EMMF_DISPLAY_MAP_POINT;
		}

		pWnd = GetDlgItem ( IDC_EDIT_GATEID_PC_REGEN );
		pWnd->GetWindowText ( strTemp );
		m_Schedule.m_dwPC_REGEN_GATEID = (DWORD) atoi ( strTemp.GetString() );

		m_Schedule.m_strBUSLOCATION = GetWin_Text ( this, IDC_EDIT_BUS_LOCATION );

		m_Schedule.m_bLeaderMonster = GetWin_Check( this, IDC_CHECK_LEADER );
		m_Schedule.m_bEachregen = GetWin_Check( this, IDC_CHECK_EACH_REGEN );
		m_Schedule.m_bGroupRegen = GetWin_Check( this, IDC_CHECK_GROUP_REGEN );
		m_Schedule.m_strGroupName   = GetWin_Text( this, IDC_EDIT_GROUPNAME ).GetString();

        m_Schedule.m_fScale = GetWin_Num_float( this, IDC_EDIT_SCALE );

		m_Schedule.m_fActivityArea = GetWin_Num_float( this, IDC_EDIT_ACTIVITYAREA );
		m_Schedule.m_fInfluenceY = GetWin_Num_float( this, IDC_EDIT_INFLUENCEY );

		if( strstr( m_Schedule.m_strGroupName.c_str(), " - " ) != NULL )
		{
			MessageBox ( "ERROR : Invalid group name. Can not use ' - '", "ERROR" );
			return FALSE;
		}

		if( m_Schedule.m_bLeaderMonster )
		{
			GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
			SpMobScheduleManServer spMobSchMan(pGLLandMan->GetMobSchMan());
			const MOBSCHEDULELIST *pMobScheduleList = spMobSchMan->GetMobSchList();
			MOBSCHEDULENODE *pHead = pMobScheduleList->m_pHead;

			while(pHead)
			{

				GLMobSchedule *pSchdule = pHead->Data;
				if( strcmp( pSchdule->m_szName, m_Schedule.m_szName ) == 0 ) 
				{
					pHead = pHead->pNext;
					continue;				
				}

				if( pSchdule->m_strGroupName == m_Schedule.m_strGroupName &&
					pSchdule->m_bLeaderMonster )
				{

					char szTempChar[256];
					sprintf( szTempChar, "Change \"%s\" group leader. %s -> %s", 
										m_Schedule.m_strGroupName.c_str(),  
										pSchdule->m_szName, m_Schedule.m_szName );
					MessageBox( szTempChar, "Message" );
					pHead->Data->m_bLeaderMonster = FALSE;
					break;
				}
				pHead = pHead->pNext;
			}

		}
	}

	return TRUE;
}

void CMobSetPage::OnBnClickedButtonCreate()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nIndex = m_ctrlMobList.GetCurSel ();

	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
	CLevelEditView *pView = (CLevelEditView*) pFrame->GetActiveView ();    
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();

	m_Schedule = GLMobSchedule();
	UpdateDataSchedule ( FALSE );

	const D3DXVECTOR3& vCollPos = pView->GetCollisionPos();

	DXAFFINEPARTS Affine;
	Affine.vTrans = vCollPos;
	m_Schedule.SetAffineValue ( &Affine );
	pView->ActiveEditMatrix ( &m_Schedule );

	ActiveMobSetEdit ( EM_EDIT_SINGLE ); 

	pGaeaServer->ClearDropObj ();
	pGaeaServer->SetUpdate ( FALSE );

	// Note : 예전에 찍어놓은 곳으로 다시 돌아간다.
	if (nIndex == LB_ERR)
        return;
	m_ctrlMobList.SetCurSel( nIndex );
}

void CMobSetPage::OnBnClickedButtonDel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nIndex = m_ctrlMobList.GetCurSel ();
	if (nIndex == LB_ERR)
        return;

	int nCount = m_ctrlMobList.GetSelCount();

	CString strMessageBox;
	strMessageBox.Format( "Really Delete [%d] Item?", nCount );

	if (MessageBox( strMessageBox, "Warning", MB_YESNO) != IDYES)
		return;

	CLevelMainFrame* pFrame = (CLevelMainFrame*) AfxGetMainWnd();
	GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
	pGaeaServer->ClearDropObj ();
	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	SpMobScheduleManServer spMobSchMan(pGLLandMan->GetMobSchMan ());

	if( nCount == 1 ) 
	{
		CString strName;
		m_ctrlMobList.GetText ( nIndex, strName );

		char szTemp[64] = {0,};
		strcpy( szTemp, &strstr( strName.GetString(), " - " )[3] );

		spMobSchMan->DelMobSch ( szTemp );
		m_ctrlMobList.DeleteString ( nIndex );

		ActiveMobSetEdit ( EM_EDIT_NONE );

		// Note : 예전에 찍어놓은 곳으로 다시 돌아간다.
		if( !m_ctrlMobList.GetCount() )	{}				// 선택할 것이 없다.
		else if( m_ctrlMobList.GetCount() == nIndex )	// 마지막 것을 지웠다.
		{
			m_ctrlMobList.SetCurSel( nIndex-1 );
		}
		else
		{
			m_ctrlMobList.SetCurSel( nIndex );
		}

		CLevelEditView *pView = (CLevelEditView*) pFrame->GetActiveView ();
		pView->DeActiveEditMatrix ();

		pGaeaServer->GetRootMap()->ClearDropObj ();
		pGaeaServer->SetUpdate ( FALSE );

	}
	else
	{
		CArray<int,int> aryListBoxSel;
		aryListBoxSel.SetSize(nCount);

		m_ctrlMobList.GetSelItems( nCount, aryListBoxSel.GetData() );

		std::vector<CString> vecDelList;
		for( int i = 0; i < nCount; i++ )
		{
			CString strTemp;
			m_ctrlMobList.GetText ( aryListBoxSel[i], strTemp );
			vecDelList.push_back(strTemp);
		}

		for( int i = 0; i < nCount; i++ )
		{
			CString& strTemp = vecDelList[i];


			char szTemp[64] = {0,};
			strcpy( szTemp, &strstr( strTemp.GetString(), " - " )[3] );

			spMobSchMan->DelMobSch ( szTemp );
			m_ctrlMobList.DeleteString ( nIndex );
	
		}
		ActiveMobSetEdit ( EM_EDIT_NONE );

		CLevelEditView *pView = (CLevelEditView*) pFrame->GetActiveView ();
		pView->DeActiveEditMatrix ();

		pGaeaServer->GetRootMap()->ClearDropObj ();
		pGaeaServer->SetUpdate ( FALSE );

	}

}

void CMobSetPage::OnBnClickedButtonModify()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nIndex = m_ctrlMobList.GetCurSel ();
	if ( nIndex == LB_ERR )		return;

	if( m_ctrlMobList.GetSelCount() != 1 ) 
	{
		MessageBox( "수정시엔 1개만 선택하셔야 합니다", "알림" );
		return;
	}


	CString StrTemp;
	m_ctrlMobList.GetText ( nIndex, StrTemp );

    CLevelMainFrame* pFrame = (CLevelMainFrame*) AfxGetMainWnd();
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
    pGaeaServer->ClearDropObj();
	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	SpMobScheduleManServer spMobSchMan(pGLLandMan->GetMobSchMan ());

	char szTemp[64] = {0,};
	strcpy( szTemp, &strstr( StrTemp.GetString(), " - " )[3] );

	MOBSCHEDULENODE *pNode = spMobSchMan->FindMobSch ( szTemp );
	if ( !pNode )	return;


	GLMobSchedule *pMobSchedule = pNode->Data;

	m_Schedule = *pMobSchedule;
	m_Schedule.m_dwVolatFlags |= GLMobSchedule::EM_EDIT;


	UpdateDataSchedule ( FALSE, FALSE );

	CLevelEditView *pView = (CLevelEditView*) pFrame->GetActiveView ();

	pView->ActiveEditMatrix ( &m_Schedule );

	ActiveMobSetEdit ( EM_EDIT_SINGLE );

	// Note : 예전에 찍어놓은 곳으로 다시 돌아간다.
	m_ctrlMobList.SetCurSel( nIndex );

	pGaeaServer->GetRootMap()->ClearDropObj ();
	pGaeaServer->SetUpdate ( FALSE );

	DxViewPort::GetInstance().CameraJump ( pMobSchedule->m_pAffineParts->vTrans );
}

void CMobSetPage::OnBnClickedButtonSchedule()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
	CLevelEditView *pView = (CLevelEditView*) pFrame->GetActiveView ();
	
	pView->SetCurType (_CUR_SELECT );
	pView->DeActiveEditMatrix ();

	m_pSheetTab->ActiveMobSchedulePage ( &m_Schedule.m_sMobActList );	
}

void CMobSetPage::OnBnClickedButtonAdd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData ( TRUE );

	if( m_emEditType != EM_EDIT_MULTIPLE )
        if ( !UpdateDataSchedule(TRUE) )
            return;

    CLevelMainFrame* pFrame = (CLevelMainFrame*) AfxGetMainWnd();
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
    pGaeaServer->ClearDropObj ();
	pGaeaServer->SetUpdate ( FALSE );

	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	SpMobScheduleManServer spMobSchMan(pGLLandMan->GetMobSchMan ());

	if ( m_Schedule.m_bRendGenPos )
	{
		m_Schedule.GenerateRendGenPos ( pGLLandMan->GetNavi().get() );
	}

	// 여러개 편집시
	if( m_emEditType == EM_EDIT_MULTIPLE )
	{
		int nCount = m_ctrlMobSelectList.GetCount();

		CString strGroupName   = GetWin_Text( this, IDC_EDIT_GROUPNAME );
  //      float fScale = GetWin_Num_float( this, IDC_EDIT_SCALE );

		//float fActivityArea = GetWin_Num_float( this, IDC_EDIT_ACTIVITYAREA );


		CString strName;
		for( int i = 0; i < nCount; i++ )
		{	

			m_ctrlMobSelectList.GetText ( i, strName );

			char szTemp[64] = {0,};
			strcpy( szTemp, &strstr( strName.GetString(), " - " )[3] );

			MOBSCHEDULENODE* pNode = spMobSchMan->FindMobSch ( szTemp );
			if ( !pNode ) continue;					

			pNode->Data->m_strGroupName = strGroupName.GetString();
   //         pNode->Data->m_fScale = fScale;
			//pNode->Data->m_fActivityArea = fActivityArea;
		}


		CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
		CLevelEditView *pView = (CLevelEditView*) pFrame->GetActiveView ();
		pView->DeActiveEditMatrix ();

		ActiveMobSetEdit ( EM_EDIT_NONE );

		//	Note : 새로 등록되는 놈일때
		//
	}else if ( !(m_Schedule.m_dwVolatFlags&GLMobSchedule::EM_EDIT) )
	{
		if ( !spMobSchMan->FindMobSch ( m_Schedule.m_szName ) )
		{

			PCROWDATA pCrow = GLCrowDataMan::GetInstance().GetCrowData ( m_Schedule.m_CrowID );
			BOOL bMaterial = ( pCrow->m_emCrow == CROW_MATERIAL );
			if ( bMaterial != m_bMaterial )
			{
				MessageBox ( "ERROR : Crow Type Is Not Correct" );
				return;
			}

			spMobSchMan->AddMobSch ( m_Schedule );

			CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
			CLevelEditView *pView = (CLevelEditView*) pFrame->GetActiveView ();
			pView->DeActiveEditMatrix ();

			ActiveMobSetEdit ( EM_EDIT_NONE );

			// Note : 마지막 위치를 찍는다.
			m_ctrlMobList.SetCurSel( m_ctrlMobList.GetCount()-1 );

			CLevelEditView::SetCurType ( _CUR_SEL_MOBSCH );
			return;
		}
		else
		{
			MessageBox ( "ERROR : Same name is already registered." );
		}
	}
	//	Note : 기존것 한개만 편집중이였을때.
	//
	else if( m_emEditType == EM_EDIT_SINGLE )
	{
		//	이름변경 여부
		if ( !strcmp( m_strBackMobName, m_Schedule.m_szName ) )
		{
			MOBSCHEDULENODE* pNode = spMobSchMan->FindMobSch ( m_Schedule.m_szName );
			if ( !pNode ) return;

			int nIndex = m_ctrlMobList.GetCurSel ();
			if ( nIndex == LB_ERR )	return;

			*pNode->Data = m_Schedule;

			CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
			CLevelEditView *pView = (CLevelEditView*) pFrame->GetActiveView ();
			pView->DeActiveEditMatrix ();

			ActiveMobSetEdit ( EM_EDIT_NONE );

			// Note : 예전에 찍어놓은 곳으로 다시 돌아간다.
			m_ctrlMobList.SetCurSel( nIndex );

			CLevelEditView::SetCurType ( _CUR_SEL_MOBSCH );

		}
		else
		{
			if ( spMobSchMan->FindMobSch ( m_Schedule.m_szName ) )
			{
				MessageBox ( "ERROR : Same name is already registered." );
				return;
			}		

			if ( !spMobSchMan->DelMobSch ( m_strBackMobName ) ) return;

			int nIndex = m_ctrlMobList.GetCurSel ();
			if ( nIndex == LB_ERR )	return;

			spMobSchMan->AddMobSch ( m_Schedule );

			CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
			CLevelEditView *pView = (CLevelEditView*) pFrame->GetActiveView ();
			pView->DeActiveEditMatrix ();

			ActiveMobSetEdit ( EM_EDIT_NONE );

			// Note : 마지막 위치를 찍는다.
			m_ctrlMobList.SetCurSel( m_ctrlMobList.GetCount()-1 );

			CLevelEditView::SetCurType ( _CUR_SEL_MOBSCH );
		}
	}
	
}

void CMobSetPage::OnBnClickedButtonCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData ( TRUE );

	int nIndex = m_ctrlMobList.GetCurSel ();

	m_Schedule = GLMobSchedule();

	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
	CLevelEditView *pView = (CLevelEditView*) pFrame->GetActiveView ();
	pView->DeActiveEditMatrix ();

	ActiveMobSetEdit ( EM_EDIT_NONE );

	CLevelEditView::SetCurType ( _CUR_SEL_MOBSCH );

	// Note : 예전에 찍어놓은 곳으로 다시 돌아간다.
	if ( nIndex == LB_ERR )	return;
	m_ctrlMobList.SetCurSel( nIndex );
}

void CMobSetPage::ListingUpdate ()
{
	m_ctrlMobList.ResetContent ();
	m_mapGroupLead.clear();

    CLevelMainFrame* pFrame = (CLevelMainFrame*) AfxGetMainWnd();
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
    GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	SpMobScheduleManServer spMobSchMan(pGLLandMan->GetMobSchMan ());
	if ( spMobSchMan == NULL )
		return;

	const MOBSCHEDULELIST *pMobScheduleList = spMobSchMan->GetMobSchList();
	MOBSCHEDULENODE *pHead = pMobScheduleList->m_pHead;
	std::string strSchduleInfo;


	/// 그룹 멤버 업데이트
	while ( pHead )
	{
		GLMobSchedule* pSchedule = pHead->Data;

		if( pSchedule->m_strGroupName.size() != 0 )
		{
			if( m_mapGroupLead.find(pSchedule->m_strGroupName) == m_mapGroupLead.end() )
				m_mapGroupLead[pSchedule->m_strGroupName] = "";

			if( pSchedule->m_bLeaderMonster )
			{		
				m_mapGroupLead[pSchedule->m_strGroupName] = pSchedule->m_szName;
			}
		}

		pHead = pHead->pNext;

	}

	// 그룹 리더 체크 
	MAP_GROUPLEAD_ITER iter = m_mapGroupLead.begin();
	CString strMsg;
	pHead = pMobScheduleList->m_pHead;
	for( ; iter != m_mapGroupLead.end(); ++iter )
	{
		if( iter->second == "" )
		{
			while ( pHead )
			{
				GLMobSchedule* pSchedule = pHead->Data;

				if( pSchedule->m_strGroupName == iter->first )
				{
					pSchedule->m_bLeaderMonster = TRUE;
					strMsg.Format( "%s 그룹의 리더가 설정되지 않아 %s를 그룹의 리더로 설정합니다", pSchedule->m_strGroupName.c_str(), pSchedule->m_szName );
					MessageBox( strMsg.GetString(), "알림" );
					break;
				}

				pHead = pHead->pNext;
			}
		}
	}


	pHead = pMobScheduleList->m_pHead;
	while ( pHead )
	{
		GLMobSchedule* pSchedule = pHead->Data;
		
		strSchduleInfo = "";

		if( pHead->Data->m_strGroupName == "" ) 
			strSchduleInfo = "null";
		else{												
			strSchduleInfo =pSchedule->m_strGroupName;

			if( pSchedule->m_bLeaderMonster )
			{
				strSchduleInfo += "*";
			}
		}

		strSchduleInfo += " - ";

		strSchduleInfo += pSchedule->m_szName;

		PCROWDATA pCrow = GLCrowDataMan::GetInstance().GetCrowData ( pSchedule->m_CrowID );
		if ( pCrow )
		{	
			BOOL bMaterial = ( pCrow->m_emCrow == CROW_MATERIAL );
			if ( bMaterial == m_bMaterial )	m_ctrlMobList.AddString ( strSchduleInfo.c_str() );
		}

		pHead = pHead->pNext;
	}

	
}

void CMobSetPage::OnBnClickedButtonTest()
{
	HRESULT hr;
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
	CLevelEditView *pView = (CLevelEditView*) pFrame->GetActiveView ();    
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
	
	pGaeaServer->ClearDropObj ();

	hr = pGaeaServer->GetRootMap()->SetMap( TRUE );
	if ( FAILED(hr) )
	{
		MessageBox ( "FAILURE : Test procedure is stopped because of map initialization failure." );
		return;
	}
	pGaeaServer->GetRootMap()->SetMobSchManRegenLayer(g_unLayer);

	pGaeaServer->SetUpdate ( TRUE );
}

void CMobSetPage::OnBnClickedButtonStop()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    CLevelMainFrame* pFrame = (CLevelMainFrame*) AfxGetMainWnd();
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
	pGaeaServer->ClearDropObj ();
	pGaeaServer->SetUpdate ( FALSE );
}

void CMobSetPage::OnBnClickedButtonCrowid()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->ActiveCrowTreePage ( MOBSETPAGEINDEX, m_Schedule.m_CrowID );
}

void CMobSetPage::SetNewID ( WORD MID, WORD SID )
{
	m_Schedule.m_CrowID.wMainID = MID;
	m_Schedule.m_CrowID.wSubID = SID;

	CWnd* pWnd = NULL;
	CString str;

	str.Format ( "%d", MID );
	pWnd = GetDlgItem ( IDC_EDIT_MID );
	pWnd->SetWindowText ( str );	

	str.Format ( "%d", SID );
	pWnd = GetDlgItem ( IDC_EDIT_SID );
	pWnd->SetWindowText ( str );
}

void CMobSetPage::SetPC_GATE_REGEN ( DWORD dwGateID )
{
	m_Schedule.m_dwPC_REGEN_GATEID = dwGateID;

	CWnd* pWnd = NULL;
	CString str;

	str.Format ( "%d", dwGateID );
	pWnd = GetDlgItem ( IDC_EDIT_GATEID_PC_REGEN );
	pWnd->SetWindowText ( str );
}

void CMobSetPage::UpdatePage ()
{
	if ( !m_bInit )		return;

	m_Schedule = GLMobSchedule();
	UpdateDataSchedule ( FALSE );

	ListingUpdate ();
}

void CMobSetPage::OnLbnDblclkListMobset()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nIndex = m_ctrlMobList.GetCurSel ();
	if (nIndex == LB_ERR)
        return;

    CLevelMainFrame* pFrame = (CLevelMainFrame*) AfxGetMainWnd();
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
    GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	SpMobScheduleManServer spMobSchMan(pGLLandMan->GetMobSchMan ());

	CString StrTemp;
	m_ctrlMobList.GetText ( nIndex, StrTemp );

	char szTemp[64] = {0,};
	strcpy( szTemp, &strstr( StrTemp.GetString(), " - " )[3] );

	MOBSCHEDULENODE *pNode = spMobSchMan->FindMobSch ( szTemp );
	if ( !pNode )	return;

	GLMobSchedule *pMobSchedule = pNode->Data;
	DxViewPort::GetInstance().CameraJump ( pMobSchedule->m_pAffineParts->vTrans );
}

void CMobSetPage::OnBnClickedButtonRendgen()
{
    CLevelMainFrame* pFrame = (CLevelMainFrame*) AfxGetMainWnd();
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	NavigationMesh* pNavi = pGLLandMan->GetNavi ().get();

	CButton* pButton = (CButton*) GetDlgItem ( IDC_CHECK_RENDGENPOS );
	m_Schedule.m_bRendGenPos = pButton->GetCheck();	

	if ( m_Schedule.m_bRendGenPos )
		m_Schedule.GenerateRendGenPos(pNavi);
}

void CMobSetPage::OnBnClickedCheckRendgenpos()
{
	CButton* pButton = (CButton*) GetDlgItem ( IDC_CHECK_RENDGENPOS );

    CLevelMainFrame* pFrame = (CLevelMainFrame*) AfxGetMainWnd();
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
    GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	NavigationMesh* pNavi = pGLLandMan->GetNavi ().get();

	m_Schedule.m_bRendGenPos = pButton->GetCheck();
	
	SetWin_Enable( this, IDC_SPIN_RENDGENPOS_NUM, m_Schedule.m_bRendGenPos );
	SetWin_Enable( this, IDC_SPIN_RENDGENPOS_DIST, m_Schedule.m_bRendGenPos );

	if ( m_Schedule.m_bRendGenPos )
	{
		m_Schedule.m_wRendGenPosNum = GLMobSchedule::EM_MAXRENDDISTRANGE;
		SetWin_Num_int( this, IDC_EDIT_RENDGENPOS_NUM, m_Schedule.m_wRendGenPosNum );
		m_Schedule.m_wRendGenPosDist = GLMobSchedule::EM_MINDISTFACTOR;
		SetWin_Num_int( this, IDC_EDIT_RENDGENPOS_DIST, m_Schedule.m_wRendGenPosDist );
		
		m_Schedule.GenerateRendGenPos(pNavi);
	}
	else
	{
		SetWin_Num_int( this, IDC_EDIT_RENDGENPOS_NUM, m_Schedule.m_wRendGenPosNum );
		SetWin_Num_int( this, IDC_EDIT_RENDGENPOS_DIST, m_Schedule.m_wRendGenPosDist );
	}
}


void CMobSetPage::OnBnClickedButtonGateidPcRegen()
{
	m_pSheetTab->ActiveGateListPage ( MOBSETPAGEINDEX, m_Schedule.m_dwPC_REGEN_GATEID );
}

void CMobSetPage::OnBnClickedButtonBusLocation()
{
	CString szFilter = "BUS Destination List (*.busloc)|*.busloc|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".busloc",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, this);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		SetWin_Text ( this, IDC_EDIT_BUS_LOCATION, dlg.GetFileName().GetString() );
	}
}

void CMobSetPage::OnBnClickedButtonCsvExport()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog dlg( FALSE, ".csv", NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		"csv file (*.csv)|*.csv|", (CWnd*)this );

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() != IDOK ) return;

	std::fstream streamFILE;
	streamFILE.open ( dlg.GetPathName().GetString(), std::ios_base::out );

    CLevelMainFrame* pFrame = (CLevelMainFrame*) AfxGetMainWnd();
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
    GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	SpMobScheduleManServer spMobSchMan(pGLLandMan->GetMobSchMan ());
	const MOBSCHEDULELIST * GLMobSchList = spMobSchMan->GetMobSchList();

	//////////////////////////////////////////////////////////////////////////
	// SAVE CSV HEAD
	//////////////////////////////////////////////////////////////////////////
	streamFILE << "Name" << ",";
	streamFILE << "Group" << ",";
	streamFILE << "wMainID" << ",";
	streamFILE << "wSubID" << ",";
	streamFILE << "Location" << ",";
	streamFILE << "LocationNum" << ",";
	streamFILE << "LocationDist" << ",";
	
	streamFILE << "Regen Time Min" << ",";
	streamFILE << "Regen Time Max" << ",";
	streamFILE << "Regen Min" << ",";
	streamFILE << "Regen Hour" << ",";
	streamFILE << "Regen Monday" << ",";
	streamFILE << "Regen Tuesday" << ",";
	streamFILE << "Regen Wednesday" << ",";
	streamFILE << "Regen Thursday" << ",";
	streamFILE << "Regen Friday" << ",";
	streamFILE << "Regen Saturday" << ",";
	streamFILE << "Regen Sunday" << ",";

	streamFILE << "Display MapPoint" << ",";
	
	streamFILE << "ActivityArea" << ","; // 행동 반경

	streamFILE << std::endl;

	sc::SGLNODE<GLMobSchedule*>* pCur = GLMobSchList->m_pHead;
	while ( pCur )
	{
		streamFILE << pCur->Data->m_szName << ",";
		streamFILE << pCur->Data->m_strGroupName.c_str() << ",";
		streamFILE << pCur->Data->m_CrowID.wMainID << ",";
		streamFILE << pCur->Data->m_CrowID.wSubID << ",";
		streamFILE << pCur->Data->m_bRendGenPos << ",";
		streamFILE << pCur->Data->m_wRendGenPosNum << ",";
		streamFILE << pCur->Data->m_wRendGenPosDist << ",";
		streamFILE << pCur->Data->m_fReGenTimeMin << ",";
		streamFILE << pCur->Data->m_fReGenTimeMax << ",";

		streamFILE << pCur->Data->m_nRegenHour << ",";
		streamFILE << pCur->Data->m_nRegenMin << ",";
		streamFILE << pCur->Data->m_bDayOfWeek[0] << ",";
		streamFILE << pCur->Data->m_bDayOfWeek[1] << ",";
		streamFILE << pCur->Data->m_bDayOfWeek[2] << ",";
		streamFILE << pCur->Data->m_bDayOfWeek[3] << ",";
		streamFILE << pCur->Data->m_bDayOfWeek[4] << ",";
		streamFILE << pCur->Data->m_bDayOfWeek[5] << ",";
		streamFILE << pCur->Data->m_bDayOfWeek[6] << ",";

		streamFILE << (pCur->Data->m_dwFlags&GLMobSchedule::EMMF_DISPLAY_MAP_POINT) << ",";

		streamFILE << pCur->Data->m_fActivityArea << ","; // 행동 반경

		streamFILE << std::endl;
		
		pCur = pCur->pNext;
	}

	streamFILE.close();
}

void CMobSetPage::OnBnClickedButtonCsvImport()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    CLevelMainFrame* pFrame = (CLevelMainFrame*)AfxGetMainWnd();
    CLevelEditView* pView = (CLevelEditView*)pFrame->GetActiveView();
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
    CFileDialog dlg(
        TRUE,
        ".csv",
        NULL,
        OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
        "csv file (*.csv)|*.csv|",
        AfxGetApp()->m_pMainWnd );

    dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath();
    if ( dlg.DoModal() != IDOK )
        return;

    sc::CStringFile StrFile( 10240 );
    if( !StrFile.Open ( dlg.GetPathName().GetString() ) )
        return;

    GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
    SpMobScheduleManServer spMobSchMan(pGLLandMan->GetMobSchMan ());

    STRUTIL::ClearSeparator();
    STRUTIL::RegisterSeparator( "," );

    CString strLine;
    CStringArray StrArray;
    std::string strTemp;
    char szMobName[ GLMobSchedule::EM_MAXSZNAME ];

    // Skip Csv Head
    StrFile.GetNextLine( strLine );
    //////////////////////////////////////////////////////////////////////////
    // LOAD CSV
    //////////////////////////////////////////////////////////////////////////
    while ( StrFile.GetNextLine( strLine ) )
    {
        int iCsvCur = 0;
        STRUTIL::StringSeparate( strLine, StrArray, true );

        GLMobSchedule sNewMobSch;

        STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strTemp );
        StringCchCopy(szMobName, GLMobSchedule::EM_MAXSZNAME, strTemp.c_str() );

        MOBSCHEDULENODE* pNode = spMobSchMan->FindMobSch( szMobName );
        if ( !pNode ) continue;

		CString strGroupName(StrArray[iCsvCur++]);
		pNode->Data->m_strGroupName = strGroupName.GetString();//StrArray[iCsvCur++];
        pNode->Data->m_CrowID.wMainID = (WORD)atoi( StrArray[ iCsvCur++ ] );
        pNode->Data->m_CrowID.wSubID = (WORD)atoi( StrArray[ iCsvCur++ ] );
        pNode->Data->m_bRendGenPos = (0!=atoi( StrArray[ iCsvCur++ ] ));
        pNode->Data->m_wRendGenPosNum = (WORD)atoi( StrArray[ iCsvCur++ ] );
        pNode->Data->m_wRendGenPosDist = (WORD)atoi( StrArray[ iCsvCur++ ] );
        pNode->Data->m_fReGenTimeMin = (float)atof( StrArray[ iCsvCur++ ] );
		pNode->Data->m_fReGenTimeMax = (float)atof( StrArray[ iCsvCur++ ] );

		if ( pNode->Data->m_bRendGenPos )
		{
			pNode->Data->GenerateRendGenPos ( pGLLandMan->GetNavi().get() );
		}
        pNode->Data->m_nRegenHour = atoi( StrArray[ iCsvCur++ ] );
        pNode->Data->m_nRegenMin = atoi( StrArray[ iCsvCur++ ] );
        pNode->Data->m_bDayOfWeek[0] = (0!=atoi( StrArray[ iCsvCur++ ] ));
        pNode->Data->m_bDayOfWeek[1] = (0!=atoi( StrArray[ iCsvCur++ ] ));
        pNode->Data->m_bDayOfWeek[2] = (0!=atoi( StrArray[ iCsvCur++ ] ));
        pNode->Data->m_bDayOfWeek[3] = (0!=atoi( StrArray[ iCsvCur++ ] ));
        pNode->Data->m_bDayOfWeek[4] = (0!=atoi( StrArray[ iCsvCur++ ] ));
        pNode->Data->m_bDayOfWeek[5] = (0!=atoi( StrArray[ iCsvCur++ ] ));
        pNode->Data->m_bDayOfWeek[6] = (0!=atoi( StrArray[ iCsvCur++ ] ));

		if ( 0!=atoi( StrArray[ iCsvCur++ ] ) )
		{
			pNode->Data->m_dwFlags |= GLMobSchedule::EMMF_DISPLAY_MAP_POINT;
		}
		else
		{
			pNode->Data->m_dwFlags &= ~GLMobSchedule::EMMF_DISPLAY_MAP_POINT;
		}

		// 행동 반경
		pNode->Data->m_fActivityArea = (float)atof( StrArray[ iCsvCur++ ] );

    }

    int nIndex = m_ctrlMobList.GetCurSel();

    UpdatePage();

    pView->DeActiveEditMatrix();
    ActiveMobSetEdit( EM_EDIT_NONE );

    // Note : 예전에 찍어놓은 곳으로 다시 돌아간다.
    if ( nIndex != LB_ERR )
        m_ctrlMobList.SetCurSel( nIndex );

    CLevelEditView::SetCurType( _CUR_SEL_MOBSCH );
}

void CMobSetPage::OnBnClickedWeekCheck()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( GetWin_Check( this, IDC_WEEK_CHECK1 ) || GetWin_Check( this, IDC_WEEK_CHECK2 ) || 
		GetWin_Check( this, IDC_WEEK_CHECK3 ) || GetWin_Check( this, IDC_WEEK_CHECK4 ) || 
		GetWin_Check( this, IDC_WEEK_CHECK5 ) || GetWin_Check( this, IDC_WEEK_CHECK6 ) || 
		GetWin_Check( this, IDC_WEEK_CHECK7 ) )
	{
		SetWin_Enable( this, IDC_BUTTON_REGEN, FALSE );
		SetWin_Enable( this, IDC_EDIT_REGEN_MIN, FALSE );
		SetWin_Enable( this, IDC_EDIT_REGEN_MAX, FALSE );
	}else{
		SetWin_Enable( this, IDC_BUTTON_REGEN, TRUE );
		SetWin_Enable( this, IDC_EDIT_REGEN_MIN, TRUE );
		SetWin_Enable( this, IDC_EDIT_REGEN_MAX, TRUE );
	}
	SetWin_Text( this, IDC_EDIT_REGEN_MIN, "0" );
	SetWin_Text( this, IDC_EDIT_REGEN_MAX, "0" );
}

void CMobSetPage::OnDeltaposSpinRendgenposNum(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

    CLevelMainFrame* pFrame = (CLevelMainFrame*) AfxGetMainWnd();
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
    GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	NavigationMesh* pNavi = pGLLandMan->GetNavi ().get();

	if ( pNMUpDown->iDelta < 0 )
	{
		m_Schedule.m_wRendGenPosNum++;
		if ( m_Schedule.m_wRendGenPosNum > GLMobSchedule::EM_MAXRENDDISTRANGE )
			m_Schedule.m_wRendGenPosNum = GLMobSchedule::EM_MAXRENDDISTRANGE;
		m_Schedule.GenerateRendGenPos(pNavi);
		SetWin_Num_int( this, IDC_EDIT_RENDGENPOS_NUM, m_Schedule.m_wRendGenPosNum );

	}
	else
	{
		m_Schedule.m_wRendGenPosNum--;
		if ( m_Schedule.m_wRendGenPosNum <= 0  )
			m_Schedule.m_wRendGenPosNum = 1;
		m_Schedule.GenerateRendGenPos(pNavi);
		SetWin_Num_int( this, IDC_EDIT_RENDGENPOS_NUM, m_Schedule.m_wRendGenPosNum );
	}

	*pResult = 0;
}

void CMobSetPage::OnDeltaposSpinRendgenposDist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

    CLevelMainFrame* pFrame = (CLevelMainFrame*) AfxGetMainWnd();
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
    GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();
	NavigationMesh* pNavi = pGLLandMan->GetNavi ().get();

	if ( pNMUpDown->iDelta < 0 )
	{
		m_Schedule.m_wRendGenPosDist += 10;
		if ( m_Schedule.m_wRendGenPosDist > GLMobSchedule::EM_MAXDISTFACTOR )
			m_Schedule.m_wRendGenPosDist = GLMobSchedule::EM_MAXDISTFACTOR;
		m_Schedule.GenerateRendGenPos(pNavi);
		SetWin_Num_int( this, IDC_EDIT_RENDGENPOS_DIST, m_Schedule.m_wRendGenPosDist );

	}
	else
	{
		m_Schedule.m_wRendGenPosDist -= 10;
		if ( m_Schedule.m_wRendGenPosDist < GLMobSchedule::EM_MINDISTFACTOR  )
			m_Schedule.m_wRendGenPosDist = GLMobSchedule::EM_MINDISTFACTOR;
		m_Schedule.GenerateRendGenPos(pNavi);
		SetWin_Num_int( this, IDC_EDIT_RENDGENPOS_DIST, m_Schedule.m_wRendGenPosDist );
	}

	*pResult = 0;
}

void CMobSetPage::OnBnClickedCheckLeader()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CButton* pButton = (CButton*) GetDlgItem ( IDC_CHECK_LEADER );

	m_Schedule.m_bLeaderMonster = pButton->GetCheck();

	SetWin_Enable( this, IDC_CHECK_GROUP_REGEN, m_Schedule.m_bLeaderMonster );
	SetWin_Enable( this, IDC_CHECK_EACH_REGEN, m_Schedule.m_bLeaderMonster );

	m_Schedule.m_bGroupRegen = FALSE;
	SetWin_Check( this, IDC_CHECK_GROUP_REGEN, FALSE );

	m_Schedule.m_bEachregen = FALSE;
	SetWin_Check( this, IDC_CHECK_EACH_REGEN, FALSE );
}

void CMobSetPage::OnBnClickedCheckGroupRegen()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CButton* pButton = (CButton*) GetDlgItem ( IDC_CHECK_GROUP_REGEN );

	m_Schedule.m_bGroupRegen = pButton->GetCheck();
}

void CMobSetPage::OnBnClickedButtonMultimodify()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if( m_ctrlMobList.GetSelCount() == 1 ) 
	{
		MessageBox( "다중 수정시엔 1개이상 선택하셔야 합니다", "알림" );
		return;
	}


	int nCount = m_ctrlMobList.GetSelCount();
	CArray<int,int> aryListBoxSel;
	aryListBoxSel.SetSize(nCount);

	m_ctrlMobList.GetSelItems( nCount, aryListBoxSel.GetData() );

	m_ctrlMobSelectList.ResetContent();


	CString StrTemp;
	for( int i = 0; i < nCount; i++ )
	{		
		m_ctrlMobList.GetText ( aryListBoxSel[i], StrTemp );
		m_ctrlMobSelectList.AddString( StrTemp.GetString() );
	}

	

	UpdateDataSchedule ( FALSE, FALSE );
	

	ActiveMobSetEdit( EM_EDIT_MULTIPLE );
}

void CMobSetPage::OnBnClickedCheckMinimap()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton* pButton = (CButton*) GetDlgItem ( IDC_CHECK_MINIMAP );

	if ( pButton->GetCheck() )
	{
		m_Schedule.m_dwFlags |= GLMobSchedule::EMMF_DISPLAY_MAP_POINT;
	}
	else
	{
		m_Schedule.m_dwFlags &= ~GLMobSchedule::EMMF_DISPLAY_MAP_POINT;
	}
}

void CMobSetPage::OnBnClickedCheckEachRegen()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton* pButton = (CButton*) GetDlgItem ( IDC_CHECK_EACH_REGEN );

	m_Schedule.m_bEachregen= pButton->GetCheck();
}

void CMobSetPage::MobLayerUpdate()
{
	m_comboMobLayer.ResetContent();	

	CLevelMainFrame* pFrame = (CLevelMainFrame*) AfxGetMainWnd();
	GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();

	if (pGLLandMan)
	{
		UINT count = pGLLandMan->GetMobSchManCount();

		TCHAR num[5] = {0};

		for (UINT i = 0; i < count; ++i)
		{
			_sntprintf_s(num, 5, _T("%d"), i);
			m_comboMobLayer.InsertString(i, num);
		}

		m_comboMobLayer.SetCurSel(g_unLayer);
	}
}

void CMobSetPage::OnBnClickedButtonMoblayerAdd()
{
	CLevelMainFrame* pFrame = (CLevelMainFrame*) AfxGetMainWnd();
	GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();

	if (pGLLandMan)
	{
		pGLLandMan->AddMobSchMan(++g_unLayer);
		ActiveMobSetEdit();

		// Efffect 를 Rendering 하기 위한 작업.
		pGLLandMan->SetDeActiveAllLayer();
		pGLLandMan->SetActiveLayer( g_unLayer );
	}
}

void CMobSetPage::OnCbnSelchangeComboMoblayer()
{
	int nSelected = m_comboMobLayer.GetCurSel();

	if (nSelected != g_unLayer)
	{
		CLevelMainFrame* pFrame = (CLevelMainFrame*) AfxGetMainWnd();
		GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
		GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();

		if (pGLLandMan)
		{
			g_unLayer = nSelected;
			pGLLandMan->SetMobSchMan(g_unLayer);
			ActiveMobSetEdit();

			// Efffect 를 Rendering 하기 위한 작업.
			pGLLandMan->SetDeActiveAllLayer();
			pGLLandMan->SetActiveLayer( g_unLayer );
		}
	}	
}

void CMobSetPage::OnBnClickedButtonMoblayerDel()
{
	CLevelMainFrame* pFrame = (CLevelMainFrame*) AfxGetMainWnd();
	GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();

	if (pGLLandMan)
	{
		pGLLandMan->DelMobSchMan(g_unLayer);
		pGLLandMan->GetMobSchManCount() == g_unLayer ? --g_unLayer : 0 ;
		assert(pGLLandMan->GetMobSchManCount() > g_unLayer);
		pGLLandMan->SetMobSchMan(g_unLayer);

		ActiveMobSetEdit();

		// Efffect 를 Rendering 하기 위한 작업.
		pGLLandMan->SetDeActiveAllLayer();
		pGLLandMan->SetActiveLayer( g_unLayer );
	}
}
