// DlgSpecialActivitySet.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../../SigmaCore/String/StringFormat.h"

#include "../../MfcExLib/ExLibs/EtcFunction.h"
#include "../../RanLogic/Crow/GLCrowData.h"
#include "../../RanLogic/Crow/GLCrowDataMan.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/Activity/ActivityBase.h"
#include "../../RanLogic/Activity/ActivityVerifier.h"
#include "../../RanLogic/GLogicDataMan.h"

#include "../../RanLogic/GLUseFeatures.h"
#include "../../enginelib/GUInterface/GameTextControl.h"
#include "./DlgItem.h"
#include "./DlgMap.h"
#include "./DlgNPC.h"
#include "DlgSpecialActivitySet.h"

// DlgSpecialActivitySet 대화 상자입니다.

IMPLEMENT_DYNAMIC(DlgSpecialActivitySet, CDialog)

DlgSpecialActivitySet::DlgSpecialActivitySet(CWnd* pParent /*=NULL*/)
	: CDialog(DlgSpecialActivitySet::IDD, pParent)
    , m_Activity( 0, 0xffff, 0, 1.0f )
{
    m_bModify = false;
}

DlgSpecialActivitySet::~DlgSpecialActivitySet()
{
    m_sMapList.CleanUpMapList();
}

void DlgSpecialActivitySet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_SPEC_ACT_TARGET, m_TargetList);
}

BEGIN_MESSAGE_MAP(DlgSpecialActivitySet, CDialog)
    ON_BN_CLICKED(IDOK, OnBnClickedOk)
    ON_CBN_SELCHANGE(IDC_COMBO_SPEC_ACT_TYPE, &DlgSpecialActivitySet::OnCbnSelchangeComboSpecActType)
    ON_BN_CLICKED(IDC_BUTTON_TARGET_LIST, &DlgSpecialActivitySet::OnBnClickedButtonTargetAdd)
    ON_BN_CLICKED(IDC_BUTTON_TARGET_DEL, &DlgSpecialActivitySet::OnBnClickedButtonTargetDel)
    ON_EN_CHANGE(IDC_EDIT_SPEC_ACT_REWARD_TITLE, &DlgSpecialActivitySet::OnEnChangeRewardTitle)
END_MESSAGE_MAP()


BOOL DlgSpecialActivitySet::OnInitDialog()
{
	CDialog::OnInitDialog();

    m_sMapList.LoadMapList();
    m_TargetList.SetExtendedStyle(m_TargetList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    
    char* szNameColumnItem[3] =
    {
        "Mid", "Sid", "Name"
    };

    const int WidthRateItCr[3] =
    {
        15, 15, 70
    };

    RECT rectCtrl;
    m_TargetList.GetClientRect(&rectCtrl);
    int InWidth(rectCtrl.right - rectCtrl.left);

    for(unsigned int _index(0); _index < 3; ++_index)
    {
        LV_COLUMN lvComlunm;
        lvComlunm.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH ;
        lvComlunm.fmt = LVCFMT_CENTER;
        lvComlunm.pszText = szNameColumnItem[_index];
        lvComlunm.iSubItem = -1;
        lvComlunm.cx = (InWidth * WidthRateItCr[_index])/ 100;
        m_TargetList.InsertColumn(_index, &lvComlunm);
    }

    InitDefaultCtrls();
    UpdateItems();
	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void DlgSpecialActivitySet::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnOK();
}

void DlgSpecialActivitySet::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
    Activity sTempAct(m_Activity);
    InverseUpdateItems(sTempAct);

    if ( sTempAct.GetTitle().empty() ) 
    {
        AfxMessageBox( "Error Empty Title" );
        return;
    }
    if ( sTempAct.GetTitleID().empty() ) 
    {
        AfxMessageBox( "Error Empty Title ID" );
        return;
    }

    if ( sTempAct.GetDesc().empty() ) 
    {
        AfxMessageBox( "Error Empty Desc" );
        return;
    }

    if ( sTempAct.GetDescID().empty() ) 
    {
        AfxMessageBox( "Error Empty Desc ID" );
        return;
    }

    if ( sTempAct.GetRewardPoint() < 0 ) 
    {
        AfxMessageBox( "Error RewardPoint < 0 " );
        return;
    }

    if ( sTempAct.GetRewardTitle().empty() && !sTempAct.GetRewardTitleID().empty() )
    {
        AfxMessageBox( "Error Empty RewardTitle" );
        return;
    }

    if ( !sTempAct.GetRewardTitle().empty() && sTempAct.GetRewardTitleID().empty() )
    {
        AfxMessageBox( "Error Empty RewardTitle ID" );
        return;
    }

    if ( sTempAct.GetTitle().length() > TITLE_MAXLENGTH )
    {
        AfxMessageBox( "Error strTitle Length " );
        return;
    }

    if ( sTempAct.GetTitleID().length() > TITLE_ID_MAXLENGTH )
    {
        AfxMessageBox( "Error strTitle ID Length " );
        return;
    }

    if ( sTempAct.GetDesc().length() > DESC_MAXLENGTH )
    {
        AfxMessageBox( "Error strDesc Length " );
        return;
    }

    if ( sTempAct.GetDescID().length() > DESC_ID_MAXLENGTH )
    {
        AfxMessageBox( "Error strDesc ID Length " );
        return;
    }

    if ( sTempAct.GetRewardTitle().length() > REWARDTITLE_MAXLENGTH )
    {
        AfxMessageBox( "Error strRewardTitle Length " );
        return;
    }

    if ( sTempAct.GetRewardTitleID().length() > REWARDTITLE_ID_MAXLENGTH )
    {
        AfxMessageBox( "Error strRewardTitle ID Length " );
        return;
    }

    switch ( sTempAct.m_id.wMainID ) 
    {
    case ACTIVITY_MID_LEVEL:
        {
            if ( sTempAct.m_nValueBase <= 0 ) 
            {
                AfxMessageBox( "Error Input Level > 0 " );
                return;
            }
        }
        break;
	case ACTIVITY_MID_KILLMOB:  
	case ACTIVITY_MID_KILLPC:
	case ACTIVITY_MID_TAKE:  
	case ACTIVITY_MID_USEITEM:
        {
            if ( sTempAct.m_vecTargetID.empty() ) 
            {
                AfxMessageBox( "TargetID is NULL" );
                return;
            }

            if ( sTempAct.m_nValueBase <= 0 ) 
            {
                AfxMessageBox( "Input Target Count" );
                return;         
            }
        }
        break;
    case ACTIVITY_MID_MAP:
        {
            if ( sTempAct.m_vecTargetID.empty() ) 
            {
                AfxMessageBox( "TargetID is NULL" );
                return;
            }
        }
        break;
    case ACTIVITY_MID_ACTIVITY:
	case ACTIVITY_MID_QUEST:  
	case ACTIVITY_MID_PONIT:
        {
            AfxMessageBox( "Can not Select Type" );
            return;
        }
        break;
    }

    m_Activity = sTempAct;
	CDialog::OnOK();
}


void DlgSpecialActivitySet::InitDefaultCtrls ()
{
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		std::string strACTIVITY_NOTIFY[ACTIVITY_NOTIFY_SIZE], strACTIVITY_MID[ACTIVITY_MID_SIZE];
		CGameTextMan::GetInstance().LoadHelpCommentString("ACTIVITY_NOTIFY", strACTIVITY_NOTIFY, CGameTextMan::EM_COMMENT_TEXT, ACTIVITY_NOTIFY_SIZE);
		CGameTextMan::GetInstance().LoadHelpCommentString("ACTIVITY_MID", strACTIVITY_MID, CGameTextMan::EM_COMMENT_TEXT, ACTIVITY_NOTIFY_SIZE);

		SetWin_Combo_Init ( this, IDC_COMBO_SPEC_ACT_NOTIFY_LEVEL, strACTIVITY_NOTIFY, ACTIVITY_NOTIFY_SIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_SPEC_ACT_TYPE, strACTIVITY_MID, ACTIVITY_MID_SIZE );
	}
	else
	{
		SetWin_Combo_Init ( this, IDC_COMBO_SPEC_ACT_NOTIFY_LEVEL, COMMENT::ACTIVITY_NOTIFY, ACTIVITY_NOTIFY_SIZE );
		SetWin_Combo_Init ( this, IDC_COMBO_SPEC_ACT_TYPE, COMMENT::ACTIVITY_MID, ACTIVITY_MID_SIZE );
	}
    SetWin_Enable( this, IDC_COMBO_SPEC_ACT_TYPE, !m_bModify );
}

void DlgSpecialActivitySet::UpdateItems()
{
    SetWin_Num_int( this, IDC_EDIT_SPEC_ACT_MID, m_Activity.m_id.wMainID );
    SetWin_Num_int( this, IDC_EDIT_SPEC_ACT_SID, m_Activity.m_id.wSubID );
    SetWin_Check( this, IDC_CHECK_USE, m_Activity.m_bValidate );

    SetWin_Text( this, IDC_EDIT_SPEC_ACT_SUBJECT, m_Activity.GetTitle().c_str() );
    SetWin_Text( this, IDC_EDIT_SPEC_ACT_SUBJECT_ID, m_Activity.GetTitleID().c_str() );
    SetWin_Text( this, IDC_EDIT_SPEC_ACT_DESCRITION, m_Activity.GetDesc().c_str() );
    SetWin_Text( this, IDC_EDIT_SPEC_ACT_DESCRITION_ID, m_Activity.GetDescID().c_str() );

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		SetWin_Combo_Sel( this, IDC_COMBO_SPEC_ACT_NOTIFY_LEVEL, CGameTextMan::GetInstance().GetCommentText("ACTIVITY_NOTIFY", m_Activity.m_nNotifyLevel).GetString() );
		SetWin_Combo_Sel( this, IDC_COMBO_SPEC_ACT_TYPE, CGameTextMan::GetInstance().GetCommentText("ACTIVITY_MID", m_Activity.m_id.wMainID).GetString() );
	}
	else
	{
		SetWin_Combo_Sel( this, IDC_COMBO_SPEC_ACT_NOTIFY_LEVEL, COMMENT::ACTIVITY_NOTIFY[m_Activity.m_nNotifyLevel].c_str() );
		SetWin_Combo_Sel( this, IDC_COMBO_SPEC_ACT_TYPE, COMMENT::ACTIVITY_MID[m_Activity.m_id.wMainID].c_str() );
	}
    
    SetWin_Num_int( this, IDC_EDIT_SPEC_ACT_BYORDER, m_Activity.m_nByOrder );
    SetWin_Num_int( this, IDC_EDIT_SPEC_ACT_REWARD_POINT, m_Activity.GetRewardPoint() );
    SetWin_Text( this, IDC_EDIT_SPEC_ACT_REWARD_TITLE, m_Activity.GetRewardTitle().c_str() );
    SetWin_Text( this, IDC_EDIT_SPEC_ACT_REWARD_TITLE_ID, m_Activity.GetRewardTitleID().c_str() );

    UpdateConditionControl();   
    UpdateCondition();
    UpdateActivityIDControl ();

}

void DlgSpecialActivitySet::UpdateConditionControl()
{
    int nType = GetWin_Combo_Sel( this, IDC_COMBO_SPEC_ACT_TYPE );

    //  조건 관련 설정 UI 모두 감추고 필요한것만 다시 보이게 한다.
    SetWin_ShowWindow( this, IDC_STATIC_LEVEL, FALSE );
    SetWin_ShowWindow( this, IDC_EDIT_SPEC_ACT_LEVEL, FALSE );
    SetWin_ShowWindow( this, IDC_STATIC_TARGET, FALSE );
    SetWin_ShowWindow( this, IDC_LIST_SPEC_ACT_TARGET, FALSE );

    SetWin_ShowWindow( this, IDC_BUTTON_TARGET_LIST, FALSE );
    SetWin_ShowWindow( this, IDC_BUTTON_TARGET_DEL, FALSE );
    
    SetWin_ShowWindow( this, IDC_STATIC_COUNT, FALSE );
    SetWin_ShowWindow( this, IDC_EDIT_SPEC_ACT_COUNT_NUM, FALSE );
    
    switch ( nType )
    {
    case ACTIVITY_MID_LEVEL:
        {
            SetWin_ShowWindow( this, IDC_STATIC_LEVEL, TRUE );
            SetWin_ShowWindow( this, IDC_EDIT_SPEC_ACT_LEVEL, TRUE );
        }
        break;
    case ACTIVITY_MID_KILLMOB:  
	case ACTIVITY_MID_KILLPC:
	case ACTIVITY_MID_TAKE:  
	case ACTIVITY_MID_USEITEM:  
        {
            SetWin_ShowWindow( this, IDC_STATIC_TARGET, TRUE );
            SetWin_ShowWindow( this, IDC_LIST_SPEC_ACT_TARGET, TRUE );            

            SetWin_ShowWindow( this, IDC_BUTTON_TARGET_LIST, TRUE );
            SetWin_ShowWindow( this, IDC_BUTTON_TARGET_DEL, TRUE );

            SetWin_ShowWindow( this, IDC_STATIC_COUNT, TRUE );
            SetWin_ShowWindow( this, IDC_EDIT_SPEC_ACT_COUNT_NUM, TRUE );
        }
        break;
	case ACTIVITY_MID_MAP:  
        {
            SetWin_ShowWindow( this, IDC_STATIC_TARGET, TRUE );
            SetWin_ShowWindow( this, IDC_LIST_SPEC_ACT_TARGET, TRUE );

            SetWin_ShowWindow( this, IDC_BUTTON_TARGET_LIST, TRUE );
            SetWin_ShowWindow( this, IDC_BUTTON_TARGET_DEL, TRUE );
        }
        break;
	case ACTIVITY_MID_ACTIVITY:  
	case ACTIVITY_MID_QUEST:  
	case ACTIVITY_MID_PONIT:  
        break;
    }   

    //  Static 관련 텍스트 수정
    switch ( nType )
    {
    case ACTIVITY_MID_LEVEL:
        break;
    case ACTIVITY_MID_KILLMOB:  
        {
            SetWin_Text( this, IDC_STATIC_TARGET, "MOB" );
            SetWin_Text( this, IDC_STATIC_COUNT, "KILL" );
        }
        break;
	case ACTIVITY_MID_KILLPC:
        {
            SetWin_Text( this, IDC_STATIC_TARGET, "MAP" );
            SetWin_Text( this, IDC_STATIC_COUNT, "KILL" );
        }
        break;
	case ACTIVITY_MID_MAP:  
        {
            SetWin_Text( this, IDC_STATIC_TARGET, "MAP" );
        }
        break;
	case ACTIVITY_MID_TAKE:  
	case ACTIVITY_MID_USEITEM:  
        {
            SetWin_Text( this, IDC_STATIC_TARGET, "ITEM" );
            SetWin_Text( this, IDC_STATIC_COUNT, "NUM" );
        }
        break;
	case ACTIVITY_MID_ACTIVITY:  
	case ACTIVITY_MID_QUEST:  
	case ACTIVITY_MID_PONIT:  
        break;
    }
}

void DlgSpecialActivitySet::UpdateCondition()
{
    int nType = GetWin_Combo_Sel( this, IDC_COMBO_SPEC_ACT_TYPE );    

    m_TargetList.DeleteAllItems();

    //  값을 셋팅한다.
    switch ( nType )
    {
    case ACTIVITY_MID_LEVEL:
        {
            SetWin_Num_int( this, IDC_EDIT_SPEC_ACT_LEVEL, m_Activity.m_nValueBase );
        }
        break;
    case ACTIVITY_MID_KILLMOB:  
	    {
            CString Strtemp;
            for ( size_t i = 0; i < m_Activity.m_vecTargetID.size(); i++ )
            {
                int InsertIndex = m_TargetList.InsertItem(i, Strtemp);

                SNATIVEID targetID(m_Activity.m_vecTargetID[i]);
                Strtemp.Format(_T("%d"), (int)targetID.wMainID);
                m_TargetList.SetItem(InsertIndex, 0, LVIF_TEXT, Strtemp, 0, 0, 0, 0);
                Strtemp.Format(_T("%d"), (int)targetID.wSubID);
                m_TargetList.SetItem(InsertIndex, 1, LVIF_TEXT, Strtemp, 0, 0, 0, 0);

                SCROWDATA* pCROW = GLCrowDataMan::GetInstance().GetCrowData ( targetID );
                if ( pCROW )
                {
                    m_TargetList.SetItem(InsertIndex, 2, LVIF_TEXT, pCROW->GetName(), 0, 0, 0, 0);
                }
            }
            SetWin_Num_int( this, IDC_EDIT_SPEC_ACT_COUNT_NUM, m_Activity.m_nValueBase );
        }
        break;
    
    case ACTIVITY_MID_TAKE:  
    case ACTIVITY_MID_USEITEM:  
        {
            CString Strtemp;
            for ( size_t i = 0; i < m_Activity.m_vecTargetID.size(); i++ )
            {
                int InsertIndex = m_TargetList.InsertItem(i, Strtemp);

                SNATIVEID targetID(m_Activity.m_vecTargetID[i]);
                Strtemp.Format(_T("%d"), (int)targetID.wMainID);
                m_TargetList.SetItem(InsertIndex, 0, LVIF_TEXT, Strtemp, 0, 0, 0, 0);
                Strtemp.Format(_T("%d"), (int)targetID.wSubID);
                m_TargetList.SetItem(InsertIndex, 1, LVIF_TEXT, Strtemp, 0, 0, 0, 0);

                const SITEM* pITEM = GLogicData::GetInstance().GetItem ( targetID );
                if ( pITEM )
                {
                    m_TargetList.SetItem(InsertIndex, 2, LVIF_TEXT, pITEM->GetName(), 0, 0, 0, 0);
                }
            }
            SetWin_Num_int( this, IDC_EDIT_SPEC_ACT_COUNT_NUM, m_Activity.m_nValueBase );
        }
        break;

    case ACTIVITY_MID_KILLPC:
	case ACTIVITY_MID_MAP:  
        {
            CString Strtemp;
            for ( size_t i = 0; i < m_Activity.m_vecTargetID.size(); i++ )
            {
                int InsertIndex = m_TargetList.InsertItem(i, Strtemp);

                SNATIVEID targetID(m_Activity.m_vecTargetID[i]);
                Strtemp.Format(_T("%d"), (int)targetID.wMainID);
                m_TargetList.SetItem(InsertIndex, 0, LVIF_TEXT, Strtemp, 0, 0, 0, 0);
                Strtemp.Format(_T("%d"), (int)targetID.wSubID);
                m_TargetList.SetItem(InsertIndex, 1, LVIF_TEXT, Strtemp, 0, 0, 0, 0);

                SMAPNODE* pNODE = m_sMapList.FindMapNode ( targetID );
                if ( pNODE )
                {
                    m_TargetList.SetItem(InsertIndex, 2, LVIF_TEXT, pNODE->m_MapID.c_str(), 0, 0, 0, 0);
                }
            }
            SetWin_Num_int( this, IDC_EDIT_SPEC_ACT_COUNT_NUM, m_Activity.m_nValueBase );
        }
        break;
	case ACTIVITY_MID_ACTIVITY:  
	case ACTIVITY_MID_QUEST:  
	case ACTIVITY_MID_PONIT:  
        break;
    }
}

void DlgSpecialActivitySet::UpdateActivityIDControl()
{
    if ( m_bModify )
    {
        return;
    }

    WORD wType = static_cast<WORD>(GetWin_Combo_Sel( this, IDC_COMBO_SPEC_ACT_TYPE ));
    SNATIVEID sID ( FindValidActivityID (wType) );
    m_Activity.m_id = sID;

    SetWin_Num_int( this, IDC_EDIT_SPEC_ACT_MID, static_cast<int>(sID.wMainID) );
    SetWin_Num_int( this, IDC_EDIT_SPEC_ACT_SID, static_cast<int>(sID.wSubID) );
    // 아래의 COMMNET는 데이터에서 읽어오지 않는다;
	SetWin_Text( this, IDC_EDIT_SPEC_ACT_SUBJECT_ID, sc::string::format( COMMENT::ACTIVITY_TEXTID[ENUM_ACTIVITY_TEXTID_TITLE], sID.wMainID, sID.wSubID ).c_str() );
	SetWin_Text( this, IDC_EDIT_SPEC_ACT_DESCRITION_ID, sc::string::format( COMMENT::ACTIVITY_TEXTID[ENUM_ACTIVITY_TEXTID_DESC], sID.wMainID, sID.wSubID ).c_str() );
	if ( !GetWin_Text( this, IDC_EDIT_SPEC_ACT_REWARD_TITLE ).IsEmpty() )
		SetWin_Text( this, IDC_EDIT_SPEC_ACT_REWARD_TITLE_ID, sc::string::format( COMMENT::ACTIVITY_TEXTID[ENUM_ACTIVITY_TEXTID_REWARDTITLE], sID.wMainID, sID.wSubID ).c_str() );
	else
		SetWin_Text( this, IDC_EDIT_SPEC_ACT_REWARD_TITLE_ID, "" );
}

void DlgSpecialActivitySet::InverseUpdateItems( Activity& OutActv )
{
    OutActv.m_bValidate = GetWin_Check( this, IDC_CHECK_USE );

    OutActv.SetTitle( GetWin_Text( this, IDC_EDIT_SPEC_ACT_SUBJECT ).GetString() );
    OutActv.SetTitleID( GetWin_Text( this, IDC_EDIT_SPEC_ACT_SUBJECT_ID ).GetString() );
    OutActv.SetDesc( GetWin_Text( this, IDC_EDIT_SPEC_ACT_DESCRITION ).GetString() );
    OutActv.SetDescID( GetWin_Text( this, IDC_EDIT_SPEC_ACT_DESCRITION_ID ).GetString() );

    OutActv.m_nNotifyLevel = static_cast<ENUM_ACTIVITY_NOTIFY_LEVEL>( GetWin_Combo_Sel( this, IDC_COMBO_SPEC_ACT_NOTIFY_LEVEL ));
    OutActv.m_id.wMainID = static_cast<WORD>( GetWin_Combo_Sel( this, IDC_COMBO_SPEC_ACT_TYPE ) );

    OutActv.SetRewardPoint( GetWin_Num_int( this, IDC_EDIT_SPEC_ACT_REWARD_POINT ) );
    OutActv.SetRewardTitle( GetWin_Text( this, IDC_EDIT_SPEC_ACT_REWARD_TITLE ).GetString() );
    OutActv.SetRewardTitleID( GetWin_Text( this, IDC_EDIT_SPEC_ACT_REWARD_TITLE_ID ).GetString() );

    OutActv.m_nByOrder = GetWin_Num_int( this, IDC_EDIT_SPEC_ACT_BYORDER );

    switch( OutActv.m_id.wMainID )
    {
    case ACTIVITY_MID_LEVEL:
        {
            OutActv.m_vecTargetID.clear();
            OutActv.m_nValueBase = GetWin_Num_int( this, IDC_EDIT_SPEC_ACT_LEVEL );
        }
        break;
    case ACTIVITY_MID_KILLMOB:  
	case ACTIVITY_MID_KILLPC:
	case ACTIVITY_MID_TAKE:  
	case ACTIVITY_MID_USEITEM:  
        {
            OutActv.m_nValueBase = GetWin_Num_int( this, IDC_EDIT_SPEC_ACT_COUNT_NUM );
        }
        break;
    case ACTIVITY_MID_MAP:  
        {
            OutActv.m_nValueBase = 0;
        }
        break;
    case ACTIVITY_MID_ACTIVITY:  
	case ACTIVITY_MID_QUEST:  
	case ACTIVITY_MID_PONIT:  
        {
            OutActv.m_vecTargetID.clear();
            OutActv.m_nValueBase = 0;
        }
        break;
    }

    
}

SNATIVEID DlgSpecialActivitySet::FindValidActivityID( const WORD wMid )
{
    SNATIVEID sID( wMid, USHRT_MAX );

    //  유휴 아이디 찾기
    bool m_bFindID = false;
    for ( WORD i = 0; i < ACTIVITYSID_MAX; ++i )
    {
        sID.wSubID = i;
        Activity* pActivity = ActivityBaseData::GetInstance()->GetActivity( sID );
        if ( !pActivity )
        {
            m_bFindID = true;
            break;
        }
    }      

    if ( !m_bFindID ) 
    {
        AfxMessageBox( "No Find ID" );
        return SNATIVEID(false);
    }      

    return sID;
}     

void DlgSpecialActivitySet::OnCbnSelchangeComboSpecActType()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    int nIndex = GetWin_Combo_Sel( this, IDC_COMBO_SPEC_ACT_TYPE );

	switch ( nIndex )
    {
    case ACTIVITY_MID_LEVEL:
    case ACTIVITY_MID_KILLMOB:  
	case ACTIVITY_MID_KILLPC:
	case ACTIVITY_MID_TAKE:  
	case ACTIVITY_MID_USEITEM:
	case ACTIVITY_MID_MAP:
        {
            m_Activity.m_nValueBase = 0;
            m_Activity.m_vecTargetID.clear();
        }
	    break;
    case ACTIVITY_MID_ACTIVITY:  
	case ACTIVITY_MID_QUEST:  
	case ACTIVITY_MID_PONIT:
        {
            AfxMessageBox( "Can not Select Type" );
            SetWin_Combo_Sel( this, IDC_COMBO_SPEC_ACT_TYPE, ACTIVITY_MID_LEVEL );

            m_Activity.m_nValueBase = 0;
            m_Activity.m_vecTargetID.clear();
        }
        break;
    }

    UpdateConditionControl();
    UpdateCondition();
    UpdateActivityIDControl();
}

void DlgSpecialActivitySet::OnBnClickedButtonTargetAdd()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    int nIndex = GetWin_Combo_Sel( this, IDC_COMBO_SPEC_ACT_TYPE );

    switch ( nIndex ) 
    {
    case ACTIVITY_MID_LEVEL:
        break;
    case ACTIVITY_MID_KILLMOB:  
        {
            SNATIVEID sID(false);
	        CDlgNPC sDlgNpc;
	        sDlgNpc.m_bMob = true;
            sDlgNpc.m_nidNPC = sID;    
            
            if ( IDOK==sDlgNpc.DoModal() )
	        {
		        sID = sDlgNpc.m_nidNPC;
                PCROWDATA pCROW = GLCrowDataMan::GetInstance().GetCrowData ( sID );
	            if ( !pCROW ) return;
            
                for( size_t i =0; i < m_Activity.m_vecTargetID.size(); i++ )
                {
                    if ( m_Activity.m_vecTargetID[i] == sID.dwID )
                    {
                        AfxMessageBox( "Error Target Already Exist" );
                        return;
                    }
                }
                m_Activity.m_vecTargetID.push_back(sID.dwID);
            }
        }
        break;
	case ACTIVITY_MID_TAKE:  
	case ACTIVITY_MID_USEITEM:
        {
            SNATIVEID sID(false);
            CDlgItem sDlgItem;
            sDlgItem.m_nidITEM = sID;

	        if ( IDOK == sDlgItem.DoModal () )
	        {
		        sID = sDlgItem.m_nidITEM;
		        const SITEM* pITEM = GLogicData::GetInstance().GetItem ( sID );
		        if ( !pITEM )	return;

                for( size_t i =0; i < m_Activity.m_vecTargetID.size(); i++ )
                {
                    if ( m_Activity.m_vecTargetID[i] == sID.dwID )
                    {
                        AfxMessageBox( "Error Target Already Exist" );
                        return;
                    }
                }
                m_Activity.m_vecTargetID.push_back(sID.dwID);
	        }
        }
        break;
	case ACTIVITY_MID_KILLPC:
	case ACTIVITY_MID_MAP:
        {
            SNATIVEID sID(false);
	        CDlgMap sDlgMap;
            sDlgMap.m_pMapList = &m_sMapList;
            sDlgMap.m_nidMAP = sID;
	        
            if ( IDOK==sDlgMap.DoModal() )
	        {
		        sID = sDlgMap.m_nidMAP;
                SMAPNODE* pNODE = m_sMapList.FindMapNode ( sID );
                if ( !pNODE ) return;

                for( size_t i =0; i < m_Activity.m_vecTargetID.size(); i++ )
                {
                    if ( m_Activity.m_vecTargetID[i] == sID.dwID )
                    {
                        AfxMessageBox( "Error Target Already Exist" );
                        return;
                    }
                }
                m_Activity.m_vecTargetID.push_back(sID.dwID);
	        }
        }
        break;
    case ACTIVITY_MID_ACTIVITY:  
	case ACTIVITY_MID_QUEST:  
	case ACTIVITY_MID_PONIT:
        break;
    }

    UpdateCondition();
}

void DlgSpecialActivitySet::OnBnClickedButtonTargetDel()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    SNATIVEID sNativeID(false);

    m_Activity.m_vecTargetID.clear();
    m_Activity.m_nValueBase = 0;
    UpdateCondition();    
}


void DlgSpecialActivitySet::OnEnChangeRewardTitle()
{
    if ( !GetWin_Text( this, IDC_EDIT_SPEC_ACT_REWARD_TITLE ).IsEmpty() 
        && GetWin_Text( this, IDC_EDIT_SPEC_ACT_REWARD_TITLE_ID ).IsEmpty() )
    {
		// 아래의 COMMNET는 데이터에서 읽어오지 않는다;
        SetWin_Text( this, IDC_EDIT_SPEC_ACT_REWARD_TITLE_ID, 
            sc::string::format( COMMENT::ACTIVITY_TEXTID[ENUM_ACTIVITY_TEXTID_REWARDTITLE], m_Activity.m_id.Mid(), m_Activity.m_id.Sid() ).c_str() );
    }
    else if ( GetWin_Text( this, IDC_EDIT_SPEC_ACT_REWARD_TITLE ).IsEmpty()  
        && !GetWin_Text( this, IDC_EDIT_SPEC_ACT_REWARD_TITLE_ID ).IsEmpty() )
    {
        SetWin_Text( this, IDC_EDIT_SPEC_ACT_REWARD_TITLE_ID, "" );
    }
}
