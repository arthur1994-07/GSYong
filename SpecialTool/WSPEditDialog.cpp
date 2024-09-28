// WSPEditDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SpecialTool.h"
#include "WSPEditDialog.h"
#include "../MfcExLib/ExLibs/EtcFunction.h"

#include "../enginelib/GUInterface/GameTextLoader.h"
#include "../enginelib/GUInterface/GameTextControl.h"
#include "../RanLogic/GLUseFeatures.h"
#include "../RanLogic/Product/GLProduct.h"
// CWSPEditDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(CWSPEditDialog, CDialog)

CWSPEditDialog::CWSPEditDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CWSPEditDialog::IDD, pParent)
{

}

CWSPEditDialog::~CWSPEditDialog()
{
}

void CWSPEditDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TypeList, m_TypeList);
}

BOOL CWSPEditDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetWin_Num_int( this, IDC_EDIT_WS, m_wWs);
	SetWin_Num_int( this, IDC_EDIT_WSP, m_wWsp);
	SetWin_Num_int( this, IDC_EDIT_PER, m_wWspPer);
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		for ( int i = 0; i < Product::ETypeTotal ; i++ )
		{
			m_TypeList.AddString( CGameTextMan::GetInstance().GetCommentText("PRODUCT_TYPE_NAME",i).GetString());
		}
	}
	else
	{
		for ( int i = 0; i < Product::ETypeTotal ; i++ )
		{
			m_TypeList.AddString( COMMENT::PRODUCT_TYPE_NAME[i].c_str() );
		}
	}
    m_TypeList.SetCurSel( m_ProductType );

	return TRUE;  // return TRUE unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CWSPEditDialog, CDialog)
	ON_BN_CLICKED(IDOK, &CWSPEditDialog::OnBnClickedOk)
	ON_LBN_SELCHANGE(IDC_TypeList, &CWSPEditDialog::OnLbnSelchangeTypelist)
END_MESSAGE_MAP()


// CWSPEditDialog 메시지 처리기입니다.

void CWSPEditDialog::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_wWs = (WORD)GetWin_Num_int( this, IDC_EDIT_WS );
	m_wWsp = (WORD)GetWin_Num_int( this, IDC_EDIT_WSP );
	m_wWspPer = (WORD)GetWin_Num_int( this, IDC_EDIT_PER );
	OnOK();
}

void CWSPEditDialog::OnLbnSelchangeTypelist()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	const int SelectType = m_TypeList.GetCurSel();
	if ( SelectType < 0)
	{
		return;
	}
	m_ProductType = (WORD)SelectType;

	CString strTypeName;
	m_TypeList.GetText( SelectType, strTypeName );
	SetWin_Text( this, IDC_EDIT_TYPE, strTypeName.GetBuffer() );
}
