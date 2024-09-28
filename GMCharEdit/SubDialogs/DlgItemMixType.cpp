#include "stdafx.h"
#include "./DlgItemMixType.h"
#include "../DoCharEdit/PropPages/PropertyChaItemMix.h"

#include "../../SigmaCore/String/StringFormat.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC( CDlgItemMixType, CDialog )
CDlgItemMixType::CDlgItemMixType( CWnd* pParent )
: CDialog( CDlgItemMixType::IDD, pParent )
, m_bMultiSelect( false )
{
}

CDlgItemMixType::~CDlgItemMixType() { }

void CDlgItemMixType::DoDataExchange( CDataExchange* pDX )
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_PRODUCT_TYPE_ID, m_viewType);
	DDX_Control(pDX, IDC_EDIT_PRODUCT_TYPE, m_viewTypeName);
	DDX_Control(pDX, IDC_EDIT_VIEW_TYPE0, m_viewTypes[0]);
	DDX_Control(pDX, IDC_EDIT_VIEW_TYPE1, m_viewTypes[1]);
	DDX_Control(pDX, IDC_EDIT_VIEW_TYPE2, m_viewTypes[2]);
	DDX_Control(pDX, IDC_EDIT_VIEW_TYPE3, m_viewTypes[3]);
	DDX_Control(pDX, IDC_EDIT_VIEW_TYPE4, m_viewTypes[4]);
	DDX_Control(pDX, IDC_EDIT_VIEW_TYPE5, m_viewTypes[5]);
	DDX_Control(pDX, IDC_EDIT_VIEW_TYPE6, m_viewTypes[6]);
	DDX_Control(pDX, IDC_EDIT_VIEW_TYPE7, m_viewTypes[7]);
	DDX_Control(pDX, IDC_EDIT_VIEW_TYPE8, m_viewTypes[8]);
	DDX_Control(pDX, IDC_EDIT_VIEW_TYPE9, m_viewTypes[9]);
	DDX_Control(pDX, IDC_EDIT_VIEW_TYPE10, m_viewTypes[10]);
	DDX_Control(pDX, IDC_EDIT_VIEW_TYPE11, m_viewTypes[11]);
	DDX_Control(pDX, IDC_EDIT_VIEW_LEARN0, m_viewLearns[0]);
	DDX_Control(pDX, IDC_EDIT_VIEW_LEARN1, m_viewLearns[1]);
	DDX_Control(pDX, IDC_EDIT_VIEW_LEARN2, m_viewLearns[2]);
	DDX_Control(pDX, IDC_EDIT_VIEW_LEARN3, m_viewLearns[3]);
	DDX_Control(pDX, IDC_EDIT_VIEW_LEARN4, m_viewLearns[4]);
	DDX_Control(pDX, IDC_EDIT_VIEW_LEARN5, m_viewLearns[5]);
	DDX_Control(pDX, IDC_EDIT_VIEW_LEARN6, m_viewLearns[6]);
	DDX_Control(pDX, IDC_EDIT_VIEW_LEARN7, m_viewLearns[7]);
	DDX_Control(pDX, IDC_EDIT_VIEW_LEARN8, m_viewLearns[8]);
	DDX_Control(pDX, IDC_EDIT_VIEW_LEARN9, m_viewLearns[9]);
	DDX_Control(pDX, IDC_EDIT_VIEW_LEARN10, m_viewLearns[10]);
	DDX_Control(pDX, IDC_EDIT_VIEW_LEARN11, m_viewLearns[11]);
	DDX_Control(pDX, IDC_EDIT_VIEW_CP0, m_viewCPs[0]);
	DDX_Control(pDX, IDC_EDIT_VIEW_CP1, m_viewCPs[1]);
	DDX_Control(pDX, IDC_EDIT_VIEW_CP2, m_viewCPs[2]);
	DDX_Control(pDX, IDC_EDIT_VIEW_CP3, m_viewCPs[3]);
	DDX_Control(pDX, IDC_EDIT_VIEW_CP4, m_viewCPs[4]);
	DDX_Control(pDX, IDC_EDIT_VIEW_CP5, m_viewCPs[5]);
	DDX_Control(pDX, IDC_EDIT_VIEW_CP6, m_viewCPs[6]);
	DDX_Control(pDX, IDC_EDIT_VIEW_CP7, m_viewCPs[7]);
	DDX_Control(pDX, IDC_EDIT_VIEW_CP8, m_viewCPs[8]);
	DDX_Control(pDX, IDC_EDIT_VIEW_CP9, m_viewCPs[9]);
	DDX_Control(pDX, IDC_EDIT_VIEW_CP10, m_viewCPs[10]);
	DDX_Control(pDX, IDC_EDIT_VIEW_CP11, m_viewCPs[11]);
	DDX_Control(pDX, IDC_EDIT_CP, m_viewCP);
	DDX_Control(pDX, IDC_CHECK_LEARN_TYPE, m_checkLearn);
	DDX_Control(pDX, IDC_EDIT_MAX_WS, m_viewMaxWS);
	DDX_Control(pDX, IDC_EDIT_LEARN_COUNT, m_viewLearnCount);
}

void CDlgItemMixType::UpdateDefaultProductType()
{
	m_viewType.SetWindowText( "" );
	m_viewTypeName.SetWindowText( "" );
	m_checkLearn.SetCheck( BST_UNCHECKED );
	m_checkLearn.EnableWindow( TRUE );
	m_viewCP.SetWindowText( "" );
	m_viewMaxWS.SetWindowText( "" );
	m_viewLearnCount.SetWindowText( "" );

	for( unsigned int i=0; i<Product::ETypeTotal; ++i )
	{
		m_viewTypes[ i ].SetWindowText(
			sc::string::format( "%u", i ).c_str() );

		m_viewLearns[ i ].SetWindowText(
			dm::CDataManagerProduct::Instance()
			->GetTypeLearnStateInfo( i ) ?
			"ON" : "OFF" );

		if( dm::CDataManagerProduct::Instance()
			->IsOpenProductType( i ) )
			m_viewLearns[ i ].SetWindowText( "Basic" );

		m_viewCPs[ i ].SetWindowText(
			sc::string::format( "%d",
			dm::CDataManagerProduct::Instance()
			->GetTypeTypeTotalExpInfo( i ) ).c_str() );
	}
}

void CDlgItemMixType::UpdateLearnAllType()
{
	m_viewType.SetWindowText( "" );
	m_viewTypeName.SetWindowText( "" );
	m_checkLearn.SetCheck( BST_CHECKED );
	m_checkLearn.EnableWindow( TRUE );
	m_viewCP.SetWindowText( "" );
	m_viewMaxWS.SetWindowText( "" );
	m_viewLearnCount.SetWindowText( "" );

	for( unsigned int i=0; i<Product::ETypeTotal; ++i )
	{
		m_viewTypes[ i ].SetWindowText(
			sc::string::format( "%u", i ).c_str() );

		m_viewLearns[ i ].SetWindowText(
			dm::CDataManagerProduct::Instance()
			->GetTypeLearnStateInfo( i ) ?
			"ON" : "OFF" );

		if( dm::CDataManagerProduct::Instance()
			->IsOpenProductType( i ) )
			m_viewLearns[ i ].SetWindowText( "Basic" );

		m_viewCPs[ i ].SetWindowText(
			sc::string::format( "%d",
			dm::CDataManagerProduct::Instance()
			->GetTypeTypeTotalExpInfo( i ) ).c_str() );
	}
}

void CDlgItemMixType::UpdateProductType( DWORD dwProductType )
{
	if( dwProductType < 0 || dwProductType >= Product::ETypeTotal )
		return;

	m_viewType.SetWindowText(
		sc::string::format("%d", dwProductType ).c_str() );

	m_viewTypeName.SetWindowText(
		dm::CDataManagerProduct::strTypeName[ dwProductType ].c_str() );

	m_checkLearn.SetCheck(
		dm::CDataManagerProduct::Instance()
		->GetTypeLearnStateInfo( dwProductType ) ? BST_CHECKED : BST_UNCHECKED );

	if( dm::CDataManagerProduct::Instance()->IsOpenProductType( dwProductType ) )
	{
		m_checkLearn.SetCheck( BST_CHECKED );
		m_checkLearn.EnableWindow( FALSE );
	}
	else
		m_checkLearn.EnableWindow( TRUE );

	m_viewCP.SetWindowText(
		sc::string::format( "%d",
		dm::CDataManagerProduct::Instance()
		->GetTypeTypeTotalExpInfo( dwProductType ) ).c_str() );

	m_viewMaxWS.SetWindowText(
		sc::string::format("%d",
		dm::CDataManagerProduct::Instance()
		->GetTypeMaxWSInfo( dwProductType ) ).c_str() );

	m_viewLearnCount.SetWindowText(
		sc::string::format( "%d",
		dm::CDataManagerProduct::Instance()
		->GetExpCountType( dwProductType ) ).c_str() );

	for( unsigned int i=0; i<Product::ETypeTotal; ++i )
	{
		m_viewTypes[ i ].SetWindowText(
			sc::string::format( "%u", i ).c_str() );

		m_viewLearns[ i ].SetWindowText(
			dm::CDataManagerProduct::Instance()
			->GetTypeLearnStateInfo( i ) ?
			"ON" : "OFF" );

		if( dm::CDataManagerProduct::Instance()
			->IsOpenProductType( i ) )
			m_viewLearns[ i ].SetWindowText( "Basic" );

		m_viewCPs[ i ].SetWindowText(
			sc::string::format( "%d",
			dm::CDataManagerProduct::Instance()
			->GetTypeTypeTotalExpInfo( i ) ).c_str() );
	}
}

void CDlgItemMixType::PushProductType( DWORD dwProductType )
{
	if( dwProductType < 0 || dwProductType >= Product::ETypeTotal )
		return;

	m_vecProductType.push_back( dwProductType );

	if( m_vecProductType.size() > 1 )
		m_bMultiSelect = true;
	else
		m_bMultiSelect = false;
}

void CDlgItemMixType::DelProductType( DWORD dwProductType )
{
	if( dwProductType < 0 || dwProductType >= Product::ETypeTotal )
		return;

	std::vector<DWORD>::iterator iter = m_vecProductType.begin();
	
	int count = -1;
	for( unsigned int i=0; i<m_vecProductType.size(); ++i )
	{
		DWORD tempProductType = m_vecProductType.at( i );

		if( tempProductType == dwProductType )
			count = i;
	}

	if( count >= 0 )
		m_vecProductType.erase( iter+count );

	if( m_vecProductType.size() > 1 )
		m_bMultiSelect = true;
	else
		m_bMultiSelect = false;
}

void CDlgItemMixType::ClearProductType()
{
	m_vecProductType.clear();
}

bool CDlgItemMixType::GetProductType( DWORD dwProductType )
{
	for( unsigned int i=0; i<m_vecProductType.size(); ++i )
	{
		DWORD tempProductType = m_vecProductType.at( i );
		if( tempProductType == dwProductType )
			return true;
	}

	return false;
}

BEGIN_MESSAGE_MAP( CDlgItemMixType, CDialog )
	ON_BN_CLICKED(IDC_CHECK_LEARN_TYPE, &CDlgItemMixType::OnBnClickedCheckLearnType)
END_MESSAGE_MAP()

BOOL CDlgItemMixType::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;
}

void CDlgItemMixType::OnBnClickedCheckLearnType()
{
	int bCurCheckLearn = m_checkLearn.GetCheck();
	for( unsigned int i=0; i<m_vecProductType.size(); ++i )
	{
		DWORD dwProductType = m_vecProductType.at( i );

		if( dm::CDataManagerProduct::Instance()
			->IsOpenProductType( dwProductType ) )
			continue;

		bool bState = false;
		if( m_vecProductType.size() == 1 )
		{
			bState = !dm::CDataManagerProduct::Instance()
				->GetTypeLearnStateInfo( dwProductType );
		}
		else
			bState = ( bCurCheckLearn == BST_CHECKED );
		

		dm::CDataManagerProduct::PRODUCT_CHANGE_SET changeSet;
		changeSet.action = dm::CDataManagerProduct::EMLEARN_TYPE;
		changeSet.dwProductType = dwProductType;
		changeSet.bState = bState;

		// Change Set µî·Ï;
		dm::CDataManagerProduct::Instance()->AddChangeSet( changeSet );

		UpdateProductType( dwProductType );

		CPropertyChaItemMix* pChaItemMix =
			dynamic_cast<CPropertyChaItemMix*>( m_pParentWnd );
		if( pChaItemMix )
			pChaItemMix->UpdateProductItemListChild();
	}
}
