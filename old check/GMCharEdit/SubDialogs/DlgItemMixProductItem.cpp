#include "stdafx.h"
#include "./DlgItemMixProductItem.h"

#include "../DoCharEdit/PropPages/PropertyChaItemMix.h"

#include "../../SigmaCore/String/StringFormat.h"

#include "../../enginelib/G-Logic/GLogic.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/Product/GLProductRecipeMan.h"
#include "../../RanLogic/Item/GLItemMan.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC( CDlgItemMixProductItem, CDialog )
CDlgItemMixProductItem::CDlgItemMixProductItem( CWnd* pParent )
: CDialog( CDlgItemMixProductItem::IDD, pParent )
, m_bMultiSelect( false )
{
}

CDlgItemMixProductItem::~CDlgItemMixProductItem() { }

void CDlgItemMixProductItem::DoDataExchange( CDataExchange* pDX )
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_PRODUCT_TYPE_ID, m_viewType);
	DDX_Control(pDX, IDC_EDIT_VIEW_ID, m_viewProductID);
	DDX_Control(pDX, IDC_EDIT_VIEW_MID, m_viewProductMID);
	DDX_Control(pDX, IDC_EDIT_VIEW_SID, m_viewProductSID);
	DDX_Control(pDX, IDC_EDIT_VIEW_RESULT_ITEM_NAME, m_viewProductName);
	DDX_Control(pDX, IDC_EDIT_PRODUCT_TYPE, m_viewTypeName);
	DDX_Control(pDX, IDC_EDIT_CP, m_viewCP);
	DDX_Control(pDX, IDC_CHECK_LEARN_TYPE, m_checkLearn);
	DDX_Control(pDX, IDC_EDIT_WS, m_viewWS);
	DDX_Control(pDX, IDC_EDIT_MAX_WS, m_viewMaxWS);
	DDX_Control(pDX, IDC_EDIT_MAX_WSP, m_viewWPS);
	DDX_Control(pDX, IDC_EDIT_MAX_WSPPER, m_viewWSPPer);
	DDX_Control(pDX, IDC_BUTTON_ABLE_CP, m_CPAbleButton);
	DDX_Control(pDX, IDC_BUTTON_UNABLE_CP, m_CPUnableButton);
	DDX_Control(pDX, IDC_BUTTON_ABLE_MP, m_MPAbleButton);
	DDX_Control(pDX, IDC_BUTTON_UNABLE_MP, m_MPUnableButton);
}

void CDlgItemMixProductItem::UpdateDefaultProductItem()
{
	m_CPAbleButton.EnableWindow( TRUE );
	m_CPUnableButton.EnableWindow( TRUE );
	m_MPAbleButton.EnableWindow( TRUE );
	m_MPUnableButton.EnableWindow( TRUE );
	m_viewType.SetWindowText( "" );
	m_viewTypeName.SetWindowText( "" );
	m_checkLearn.SetCheck( BST_UNCHECKED );
	m_viewCP.SetWindowText( "" );
	m_viewWS.SetWindowText( "" );
	m_viewMaxWS.SetWindowText( "" );
	m_viewWPS.SetWindowText( "" );
	m_viewWSPPer.SetWindowText( "" );
	m_viewProductID.SetWindowText( "" );
	m_viewProductMID.SetWindowText( "" );
	m_viewProductSID.SetWindowText( "" );
	m_viewProductName.SetWindowText( "" );
}

void CDlgItemMixProductItem::UpdateProductItem( DWORD dwProductType, DWORD dwProductID )
{
	if( dwProductType < 0 || dwProductType >= Product::ETypeTotal )
		return;

	if( !dm::CDataManagerProduct::Instance()->GetTypeLearnStateInfo(
		dwProductType ) &&
		!dm::CDataManagerProduct::Instance()
		->IsOpenProductType( dwProductType ) )
	{
		m_CPAbleButton.EnableWindow( FALSE );
		m_CPUnableButton.EnableWindow( FALSE );
		m_MPAbleButton.EnableWindow( FALSE );
		m_MPUnableButton.EnableWindow( FALSE );
	}
	else
	{
		dm::CDataManagerProduct::PRODUCT_ITEM_INFO itemInfo =
			dm::CDataManagerProduct::Instance()->GetProductItemInfo(
			dwProductType, dwProductID );

		if( itemInfo.bUseMP )
		{
			if( itemInfo.bMP )
			{
				m_MPAbleButton.EnableWindow( TRUE );
				m_MPUnableButton.EnableWindow( FALSE );

				if( itemInfo.bUseCP )
				{
					if( itemInfo.bCP )
					{
						m_CPAbleButton.EnableWindow( TRUE );
						m_CPUnableButton.EnableWindow( FALSE );
					}
					else
					{
						m_CPAbleButton.EnableWindow( FALSE );
						m_CPUnableButton.EnableWindow( TRUE );
					}
				}
				else
				{
					m_CPAbleButton.EnableWindow( FALSE );
					m_CPUnableButton.EnableWindow( FALSE );
				}
			}
			else
			{
				m_MPAbleButton.EnableWindow( FALSE );
				m_MPUnableButton.EnableWindow( TRUE );

				m_CPAbleButton.EnableWindow( FALSE );
				m_CPUnableButton.EnableWindow( FALSE );
			}
		}
		else
		{
			if( itemInfo.bUseCP )
			{
				if( itemInfo.bCP )
				{
					m_CPAbleButton.EnableWindow( TRUE );
					m_CPUnableButton.EnableWindow( FALSE );
				}
				else
				{
					m_CPAbleButton.EnableWindow( FALSE );
					m_CPUnableButton.EnableWindow( TRUE );
				}
			}
			else
			{
				m_CPAbleButton.EnableWindow( FALSE );
				m_CPUnableButton.EnableWindow( FALSE );
			}

			m_MPAbleButton.EnableWindow( FALSE );
			m_MPUnableButton.EnableWindow( FALSE );
		}
	}

	m_viewType.SetWindowText(
		sc::string::format("%d", dwProductType ).c_str() );

	m_viewTypeName.SetWindowText(
		dm::CDataManagerProduct::strTypeName[ dwProductType ].c_str() );

	m_checkLearn.SetCheck(
		dm::CDataManagerProduct::Instance()
		->GetTypeLearnStateInfo( dwProductType ) ? BST_CHECKED : BST_UNCHECKED );

	if( dm::CDataManagerProduct::Instance()->IsOpenProductType( dwProductType ) )
		m_checkLearn.SetCheck( BST_CHECKED );

	m_viewCP.SetWindowText(
		sc::string::format( "%d",
		dm::CDataManagerProduct::Instance()
		->GetTypeTypeTotalExpInfo( dwProductType ) ).c_str() );

	const SProductRecipe productRecipe = GLProductRecipeMan::GetInstance()
		.m_mapKeyProductRecipe[ dwProductID ];
	//if( pProductRecipe )
	{
		m_viewWS.SetWindowText(
			sc::string::format("%d",
			productRecipe.wWs ).c_str() );

		m_viewMaxWS.SetWindowText(
			sc::string::format("%d",
			dm::CDataManagerProduct::Instance()
			->GetTypeMaxWSInfo( dwProductType ) ).c_str() );

		m_viewWPS.SetWindowText(
			sc::string::format("%d",
			productRecipe.wWsp ).c_str() );

		m_viewWSPPer.SetWindowText(
			sc::string::format("%d",
			productRecipe.wWspPer ).c_str() );

		m_viewProductID.SetWindowText(
			sc::string::format("%d", dwProductID ).c_str() );

		m_viewProductMID.SetWindowText(
			sc::string::format("%d",
			productRecipe.sResultItem.sNID.Mid() ).c_str() );

		m_viewProductSID.SetWindowText(
			sc::string::format("%d",
			productRecipe.sResultItem.sNID.Sid() ).c_str() );

		SNATIVEID sNativeID = productRecipe.sResultItem.sNID;
		SITEM* pItem = GLogicData::GetInstance().GetItem( sNativeID );
		if( pItem )
			m_viewProductName.SetWindowText( pItem->GetName() );
	}
}

void CDlgItemMixProductItem::PushProductItem( const PRODUCT_ITEM& productItem )
{
	if( productItem.dwProductType < 0 ||
		productItem.dwProductType >= Product::ETypeTotal )
		return;

	m_vecProductItem.push_back( productItem );

	if( m_vecProductItem.size() > 1 )
		m_bMultiSelect = true;
	else
		m_bMultiSelect = false;
}

void CDlgItemMixProductItem::DelProductItem( const PRODUCT_ITEM& productItem )
{
	if( productItem.dwProductType < 0 || productItem.dwProductType >= Product::ETypeTotal )
		return;

	std::vector<PRODUCT_ITEM>::iterator iter = m_vecProductItem.begin();

	int count = -1;
	for( unsigned int i=0; i<m_vecProductItem.size(); ++i )
	{
		PRODUCT_ITEM tempProductItem = m_vecProductItem.at( i );

		if( tempProductItem.dwProductType == productItem.dwProductType &&
			tempProductItem.dwProductID == productItem.dwProductID )
			count = i;
	}

	if( count >= 0 )
		m_vecProductItem.erase( iter+count );

	if( m_vecProductItem.size() > 1 )
		m_bMultiSelect = true;
	else
		m_bMultiSelect = false;
};

void CDlgItemMixProductItem::ClearProductType()
{
	m_vecProductItem.clear();
}

BEGIN_MESSAGE_MAP( CDlgItemMixProductItem, CDialog )
	ON_BN_CLICKED(IDC_BUTTON_ABLE_CP, &CDlgItemMixProductItem::OnBnClickedButtonAbleCp)
	ON_BN_CLICKED(IDC_BUTTON_UNABLE_CP, &CDlgItemMixProductItem::OnBnClickedButtonUnableCp)
	ON_BN_CLICKED(IDC_BUTTON_ABLE_MP, &CDlgItemMixProductItem::OnBnClickedButtonAbleMp)
	ON_BN_CLICKED(IDC_BUTTON_UNABLE_MP, &CDlgItemMixProductItem::OnBnClickedButtonUnableMp)
END_MESSAGE_MAP()

BOOL CDlgItemMixProductItem::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;
}

void CDlgItemMixProductItem::OnBnClickedButtonAbleCp()
{
	// Change Set 등록;
	for( unsigned int i=0; i<m_vecProductItem.size(); ++i )
	{
		if( !dm::CDataManagerProduct::Instance()->GetTypeLearnStateInfo(
			m_vecProductItem.at( i ).dwProductType ) )
		{
			if( !dm::CDataManagerProduct::Instance()
				->IsOpenProductType(
				m_vecProductItem.at( i ).dwProductType ) )
				continue;
		}

		dm::CDataManagerProduct::PRODUCT_ITEM_INFO itemInfo =
			dm::CDataManagerProduct::Instance()->GetProductItemInfo(
			m_vecProductItem.at( i ).dwProductType,
			m_vecProductItem.at( i ).dwProductID );

		if( !itemInfo.bUseCP )
			continue;

		if( itemInfo.bUseMP )
		{
			if( !itemInfo.bMP )
				continue;
		}

		if( !itemInfo.bCP )
			continue;

		dm::CDataManagerProduct::PRODUCT_CHANGE_SET changeSet;
		changeSet.action = dm::CDataManagerProduct::EMPICKUP_EXP;
		changeSet.dwProductType = m_vecProductItem.at( i ).dwProductType;
		changeSet.dwProductID = m_vecProductItem.at( i ).dwProductID;
		changeSet.bState = false;
		dm::CDataManagerProduct::Instance()->AddChangeSet( changeSet );

		UpdateProductItem( changeSet.dwProductType, changeSet.dwProductID );

		CPropertyChaItemMix* pChaItemMix =
			dynamic_cast<CPropertyChaItemMix*>( m_pParentWnd );
		if( pChaItemMix )
			pChaItemMix->UpdateProductItemListChild();
	}
}

void CDlgItemMixProductItem::OnBnClickedButtonUnableCp()
{
	// Change Set 등록;
	for( unsigned int i=0; i<m_vecProductItem.size(); ++i )
	{
		if( !dm::CDataManagerProduct::Instance()->GetTypeLearnStateInfo(
			m_vecProductItem.at( i ).dwProductType ) )
		{
			if( !dm::CDataManagerProduct::Instance()
				->IsOpenProductType(
				m_vecProductItem.at( i ).dwProductType ) )
				continue;
		}

		dm::CDataManagerProduct::PRODUCT_ITEM_INFO itemInfo =
			dm::CDataManagerProduct::Instance()->GetProductItemInfo(
			m_vecProductItem.at( i ).dwProductType,
			m_vecProductItem.at( i ).dwProductID );

		if( !itemInfo.bUseCP )
			continue;

		if( itemInfo.bUseMP )
		{
			if( !itemInfo.bMP )
				continue;
		}

		if( itemInfo.bCP )
			continue;

		dm::CDataManagerProduct::PRODUCT_CHANGE_SET changeSet;
		changeSet.action = dm::CDataManagerProduct::EMPICKUP_EXP;
		changeSet.dwProductType = m_vecProductItem.at( i ).dwProductType;
		changeSet.dwProductID = m_vecProductItem.at( i ).dwProductID;
		changeSet.bState = true;
		dm::CDataManagerProduct::Instance()->AddChangeSet( changeSet );

		UpdateProductItem( changeSet.dwProductType, changeSet.dwProductID );

		CPropertyChaItemMix* pChaItemMix =
			dynamic_cast<CPropertyChaItemMix*>( m_pParentWnd );
		if( pChaItemMix )
			pChaItemMix->UpdateProductItemListChild();
	}
}

void CDlgItemMixProductItem::OnBnClickedButtonAbleMp()
{
	// Change Set 등록;
	for( unsigned int i=0; i<m_vecProductItem.size(); ++i )
	{
		if( !dm::CDataManagerProduct::Instance()->GetTypeLearnStateInfo(
			m_vecProductItem.at( i ).dwProductType ) )
		{
			if( !dm::CDataManagerProduct::Instance()
				->IsOpenProductType(
				m_vecProductItem.at( i ).dwProductType ) )
				continue;
		}

		dm::CDataManagerProduct::PRODUCT_ITEM_INFO itemInfo =
			dm::CDataManagerProduct::Instance()->GetProductItemInfo(
			m_vecProductItem.at( i ).dwProductType,
			m_vecProductItem.at( i ).dwProductID );

		if( itemInfo.bUseMP )
		{
			if( !itemInfo.bMP )
				continue;
		}
		else
			continue;

		dm::CDataManagerProduct::PRODUCT_CHANGE_SET changeSet;
		changeSet.action = dm::CDataManagerProduct::EMLEARN_BOOK;
		changeSet.dwProductType = m_vecProductItem.at( i ).dwProductType;
		changeSet.dwProductID = m_vecProductItem.at( i ).dwProductID;
		changeSet.bState = false;
		dm::CDataManagerProduct::Instance()->AddChangeSet( changeSet );

		UpdateProductItem( changeSet.dwProductType, changeSet.dwProductID );

		CPropertyChaItemMix* pChaItemMix =
			dynamic_cast<CPropertyChaItemMix*>( m_pParentWnd );
		if( pChaItemMix )
			pChaItemMix->UpdateProductItemListChild();
	}
}

void CDlgItemMixProductItem::OnBnClickedButtonUnableMp()
{
	// Change Set 등록;
	for( unsigned int i=0; i<m_vecProductItem.size(); ++i )
	{
		if( !dm::CDataManagerProduct::Instance()->GetTypeLearnStateInfo(
			m_vecProductItem.at( i ).dwProductType ) )
		{
			if( !dm::CDataManagerProduct::Instance()
				->IsOpenProductType(
				m_vecProductItem.at( i ).dwProductType ) )
				continue;
		}

		dm::CDataManagerProduct::PRODUCT_ITEM_INFO itemInfo =
			dm::CDataManagerProduct::Instance()->GetProductItemInfo(
			m_vecProductItem.at( i ).dwProductType,
			m_vecProductItem.at( i ).dwProductID );

		if( itemInfo.bUseMP )
		{
			if( itemInfo.bMP )
				continue;
		}
		else
			continue;

		dm::CDataManagerProduct::PRODUCT_CHANGE_SET changeSet;
		changeSet.action = dm::CDataManagerProduct::EMLEARN_BOOK;
		changeSet.dwProductType = m_vecProductItem.at( i ).dwProductType;
		changeSet.dwProductID = m_vecProductItem.at( i ).dwProductID;
		changeSet.bState = true;
		dm::CDataManagerProduct::Instance()->AddChangeSet( changeSet );

		UpdateProductItem( changeSet.dwProductType, changeSet.dwProductID );

		CPropertyChaItemMix* pChaItemMix =
			dynamic_cast<CPropertyChaItemMix*>( m_pParentWnd );
		if( pChaItemMix )
			pChaItemMix->UpdateProductItemListChild();
	}
}
