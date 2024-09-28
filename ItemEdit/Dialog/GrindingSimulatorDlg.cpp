// GrindingSimulatorDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../ItemEdit.h"
#include "GrindingSimulatorDlg.h"

#include "../../SigmaCore/String/StringFormat.h"

#include "../../EngineLib/G-Logic/GLogic.h"
#include "../../MfcExLib/ExLibs/EtcFunction.h"

#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/Item/GLItemGrindingScript.h"
#include "../../RanLogic/Item/GLItemCustom.h"
#include "../../RanLogic/GLogicEx.h"

#include <boost/tr1/memory.hpp>

// CGrindingSimulatorDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CGrindingSimulatorDlg, CDialog)

CGrindingSimulatorDlg::CGrindingSimulatorDlg(CWnd* pParent /*=NULL*/)
: CDialog(CGrindingSimulatorDlg::IDD, pParent)
, m_pGrinder(NULL)
{

}

CGrindingSimulatorDlg::~CGrindingSimulatorDlg()
{
}

void CGrindingSimulatorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LOG, m_LogList);
}


BEGIN_MESSAGE_MAP(CGrindingSimulatorDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CGrindingSimulatorDlg::OnBnClickedButtonClose)
	ON_BN_CLICKED(IDC_BUTTON_START, &CGrindingSimulatorDlg::OnBnClickedButtonStart)
END_MESSAGE_MAP()


// CGrindingSimulatorDlg 메시지 처리기입니다.

void CGrindingSimulatorDlg::OnBnClickedButtonClose()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//OnClose();
	OnOK();
}

void CGrindingSimulatorDlg::OnBnClickedButtonStart()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nMid = GetWin_Num_int( this, IDC_EDIT_MID );
	int nSid = GetWin_Num_int( this, IDC_EDIT_SID );

	int nNum   = GetWin_Num_int( this, IDC_EDIT_NUM );
	int nCount = GetWin_Num_int( this, IDC_EDIT_COUNT );

	UINT nOption = 0;
	
	if ( IsDlgButtonChecked(IDC_CHECK_INC_GRINDING) )
	{
		nOption |= EMGRINDINGOPTION_INC_GRINDINGRATE;
	}

	if ( IsDlgButtonChecked(IDC_CHECK_ANTIRESET) )
	{
		nOption |= EMGRINDINGOPTION_ANTIRESET;
	}

	if ( IsDlgButtonChecked(IDC_CHECK_ANTITERMINATE) )
	{
		nOption |= EMGRINDINGOPTION_ANTIDISAPPEAR;
	}

	DoSimulate ( SNATIVEID(static_cast<WORD>(nMid), static_cast<WORD>(nSid)) 
		,static_cast<UINT>(nNum) 
		,static_cast<UINT>(nCount)
		,nOption );

}

struct SWAP_GRINDINGOPT
{
	SWAP_GRINDINGOPT( CItemNode* _pNode, SGRINDING_OPT *_pOrg ) 
		: pNode(_pNode)
		, pOrg(_pOrg)
	{
	}

	void operator()(SGRINDING_OPT*)
	{
		pNode->m_rGRINDING_SET = pOrg;
	}

	CItemNode*		pNode;
	SGRINDING_OPT*  pOrg;
};

void CGrindingSimulatorDlg::DoSimulate( const SNATIVEID dstID, const UINT nGrinderNum, const UINT nGrindingCount, const UINT nGrindingOption )
{
	if ( !m_pGrinder )
	{
		MessageBox( "grinder is null", "Error" );
		return;
	}

	CItemNode* pNode = GLItemMan::GetInstance().GetItemNode( 
		m_pGrinder->sBasicOp.sNativeID.wMainID, 
		m_pGrinder->sBasicOp.sNativeID.wSubID );
	
	if ( !pNode )
	{
		MessageBox( "itemnode is null", "Error" );
		return;
	}

	//! 스크립트를 로드
	std::tr1::shared_ptr<SGRINDING_OPT> pOptFile( GLItemMan::GetInstance().LoadGrindingOptData( 
		m_pGrinder->sGrindingOp.strGRINDER_OPT_FILE, false ));

	if ( !pOptFile )
	{
		MessageBox( "failed to load script", "Error" );
		return;
	}

	// 새로운 옵션파일과 임시로 바꿔치기 한다.
	std::tr1::shared_ptr<SGRINDING_OPT> pPreOptFile(pNode->m_rGRINDING_SET, 
		SWAP_GRINDINGOPT(pNode, pNode->m_rGRINDING_SET));

	pNode->m_rGRINDING_SET = pOptFile.get();

	const SITEM* pDstItem = GLItemMan::GetInstance().GetItem(dstID);

	if ( !pDstItem )
	{
		MessageBox( sc::string::format("not found dest item(%1%/%2%)", dstID.wMainID, dstID.wSubID).c_str(), "Error" );
		return;
	}

	ClearLog();

	SITEMCUSTOM sGrinder(m_pGrinder->sBasicOp.sNativeID);
	SITEMCUSTOM sDestItem(dstID);

	sGrinder.wTurnNum = static_cast<WORD>(nGrinderNum);

	GLCHARLOGIC cChar;

	for ( UINT i=0; i<nGrindingCount; ++i )
	{
		if ( sGrinder.wTurnNum == 0 )
		{
			DoLog( "grinder is empty" );
			return;
		}

		// 조건 체크
		EMGRINDINGCONDITION emCondition = cChar.GRINDING_CONDITION_CHECK( sDestItem, m_pGrinder, sGrinder );
		switch ( emCondition )
		{
		default:
		case EMGRINDINGCONDITION_FAILED: DoLog( "condition check failed (EMGRINDINGCONDITION_FAILED)" );			return;

		case EMGRINDINGCONDITION_SUCCESS:
			{
				//Blank
			}
			break;

		case EMGRINDINGCONDITION_NOITEM:  	  DoLog( "condition check failed (EMGRINDINGCONDITION_NOITEM)" );		return;
		case EMGRINDINGCONDITION_MAX:		  DoLog( "condition check failed (EMGRINDINGCONDITION_MAX)" );			return;
		case EMGRINDINGCONDITION_NOTBEST:	  DoLog( "condition check failed (EMGRINDINGCONDITION_NOTBEST)" );		return;
		case EMGRINDINGCONDITION_NOTHIGH:	  DoLog( "condition check failed (EMGRINDINGCONDITION_NOTHIGH)" );		return;
		case EMGRINDINGCONDITION_NOTBESTITEM: DoLog( "condition check failed (EMGRINDINGCONDITION_NOTBESTITEM)" );	return;
		case EMGRINDINGCONDITION_NOTNUM:	  DoLog( "condition check failed (EMGRINDINGCONDITION_NUM)" );			return;
		case EMGRINDINGCONDITION_DEFCLASS:	  DoLog( "condition check failed (EMGRINDINGCONDITION_DEFCLASS)" );		return;
		};

		EMGRINDING_TYPE emType = m_pGrinder->sGrindingOp.emTYPE;
		WORD wNum = cChar.GRINDING_CONSUME_NUM( sDestItem, m_pGrinder, sGrinder );
        BYTE ResultGrade = 0; // 연마결과수치
		DWORD dwResult = cChar.DOGRINDING(sDestItem, m_pGrinder, sGrinder, static_cast<DWORD>(nGrindingOption), ResultGrade);

		if ( sGrinder.wTurnNum < wNum )
		{
			sGrinder.wTurnNum = 0;
		}
		else
		{
			sGrinder.wTurnNum -= wNum;
		}

		if ( dwResult&EMGRINDINGRESULT_TERMINATE )
		{
			DoLog( sc::string::format("failed grinding, item is destroyed...(left : %1%, grade = %2%)", 
				sGrinder.wTurnNum, static_cast<UINT>(sDestItem.GETGRADE(emType))) );

			return;
		}
		else if ( dwResult&EMGRINDINGRESULT_ANTIDISAPPEAR )
		{
			DoLog( sc::string::format("failed grinding, failed grinding. but, Item is protected to destroyed (left : %1%, grade = %2%)", 
				sGrinder.wTurnNum, static_cast<UINT>(sDestItem.GETGRADE(emType))) );
		}
		else if ( dwResult&EMGRINDINGRESULT_ANTIRESET )
		{
			DoLog( sc::string::format("failed grinding, failed grinding. but, Item is protected to Reset (left : %1%, grade = %2%)", 
				sGrinder.wTurnNum, static_cast<UINT>(sDestItem.GETGRADE(emType))) );
		}
		else if ( dwResult&EMGRINDINGRESULT_RESET )
		{
			DoLog( sc::string::format("failed grinding, grade is reset(left : %1%, grade = %2%)", 
				sGrinder.wTurnNum, static_cast<UINT>(sDestItem.GETGRADE(emType))) );
		}
		else if ( dwResult&EMGRINDINGRESULT_SUCCESS )
		{
			DoLog( sc::string::format("successful grinding(left = %1%, grade = %2%)", 
				sGrinder.wTurnNum, static_cast<UINT>(sDestItem.GETGRADE(emType))) );
		}
		else
		{
			DoLog( sc::string::format("failed grinding(left = %1%, grade = %2%)", 
				sGrinder.wTurnNum, static_cast<UINT>(sDestItem.GETGRADE(emType))) );
		}
	}
}

void CGrindingSimulatorDlg::ClearLog()
{
	m_LogList.ResetContent();
}

void CGrindingSimulatorDlg::DoLog( const std::string& strLog )
{
	m_LogList.AddString( strLog.c_str() );
}
