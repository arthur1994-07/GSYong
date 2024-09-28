#include "stdafx.h"

#include "../../VisualMaterialLib/Command/DxVMCommand_Coordinates.h"

#include "./CustomItem.h"

#include "PropertyGridCoordinates.h"



//---------------------------------------------------------------------------------------------------------- PropertyGridPanner
PropertyGridPanner::PropertyGridPanner()
{
}

PropertyGridPanner::~PropertyGridPanner()
{
}

void PropertyGridPanner::CreatePropertyGrid( CXTPPropertyGrid& wndPropertyGrid, DxVMCommand* pVMCommand )
{
    CString	strEdit;

    // 기본적인 셋팅
    m_rVMCommand = dynamic_cast<DxVMCommand_Panner*>( pVMCommand );

    // 리셋
    wndPropertyGrid.ResetContent();

    CXTPPropertyGridItem* pChild;

    // create document settings category.
    CXTPPropertyGridItem* pCategory  = wndPropertyGrid.AddCategory( _T("MaterialExpressionConstant") );
    strEdit.Format ( _T("%f"), m_rVMCommand->m_sData.m_fSpeedX );
    pChild = pCategory->AddChildItem( new CCustomItemEditFloat( _T("Speed X"), &m_rVMCommand->m_sData.m_fSpeedX, strEdit.GetString() ) );

    strEdit.Format ( _T("%f"), m_rVMCommand->m_sData.m_fSpeedY );
    pChild = pCategory->AddChildItem( new CCustomItemEditFloat( _T("Speed Y"), &m_rVMCommand->m_sData.m_fSpeedY, strEdit.GetString() ) );

    pCategory  = wndPropertyGrid.AddCategory( _T("MaterialExpression") );
    pChild = pCategory->AddChildItem( new CCustomItemEdit(_T("Desc"), &m_rVMCommand->m_strDesc ) );
    pChild->SetTooltip( _T("A Description that level designers can add (shows in the material editer UI).") );
}


//---------------------------------------------------------------------------------------------------------- PropertyGridRotator
PropertyGridRotator::PropertyGridRotator()
{
}

PropertyGridRotator::~PropertyGridRotator()
{
}

void PropertyGridRotator::CreatePropertyGrid( CXTPPropertyGrid& wndPropertyGrid, DxVMCommand* pVMCommand )
{
    CString	strEdit;

    // 기본적인 셋팅
    m_rVMCommand = dynamic_cast<DxVMCommand_Rotator*>( pVMCommand );

    // 리셋
    wndPropertyGrid.ResetContent();

    CXTPPropertyGridItem* pChild;

    // create document settings category.
    CXTPPropertyGridItem* pCategory  = wndPropertyGrid.AddCategory( _T("MaterialExpressionConstant") );
    strEdit.Format ( _T("%f"), m_rVMCommand->m_sData.m_fCenterX );
    pChild = pCategory->AddChildItem( new CCustomItemEditFloat( _T("Center X"), &m_rVMCommand->m_sData.m_fCenterX, strEdit.GetString() ) );

    strEdit.Format ( _T("%f"), m_rVMCommand->m_sData.m_fCenterY );
    pChild = pCategory->AddChildItem( new CCustomItemEditFloat( _T("Center Y"), &m_rVMCommand->m_sData.m_fCenterY, strEdit.GetString() ) );

    strEdit.Format ( _T("%f"), m_rVMCommand->m_sData.m_fSpeed );
    pChild = pCategory->AddChildItem( new CCustomItemEditFloat( _T("Speed"), &m_rVMCommand->m_sData.m_fSpeed, strEdit.GetString() ) );

    pCategory  = wndPropertyGrid.AddCategory( _T("MaterialExpression") );
    pChild = pCategory->AddChildItem( new CCustomItemEdit(_T("Desc"), &m_rVMCommand->m_strDesc ) );
    pChild->SetTooltip( _T("A Description that level designers can add (shows in the material editer UI).") );
}


//---------------------------------------------------------------------------------------------------------- PropertyGridTextureCoordinate
PropertyGridTextureCoordinate::PropertyGridTextureCoordinate()
{
}

PropertyGridTextureCoordinate::~PropertyGridTextureCoordinate()
{
}

void PropertyGridTextureCoordinate::CreatePropertyGrid( CXTPPropertyGrid& wndPropertyGrid, DxVMCommand* pVMCommand )
{
    CString	strEdit;

    // 기본적인 셋팅
    m_rVMCommand = dynamic_cast<DxVMCommand_TextureCoordinate*>( pVMCommand );

    // 리셋
    wndPropertyGrid.ResetContent();

    CXTPPropertyGridItem* pChild;

    std::string strEditDisable = "Disable";

    // create document settings category.
    CXTPPropertyGridItem* pCategory  = wndPropertyGrid.AddCategory( _T("MaterialExpressionTextureCoordinate") );
    pChild = pCategory->AddChildItem( new CCustomItemEdit( _T("Coordinate Index (Disable)"), &strEditDisable ) );
    pChild->SetTooltip( _T("Texture Coordinate Index") );

    strEdit.Format ( _T("%f"), m_rVMCommand->m_fUTiling );
    pChild = pCategory->AddChildItem( new CCustomItemEditFloat( _T("UTiling"), &m_rVMCommand->m_fUTiling, strEdit.GetString() ) );

    strEdit.Format ( _T("%f"), m_rVMCommand->m_fVTiling );
    pChild = pCategory->AddChildItem( new CCustomItemEditFloat( _T("VTiling"), &m_rVMCommand->m_fVTiling, strEdit.GetString() ) );

    pChild = pCategory->AddChildItem( new CCustomItemEdit( _T("Un Mirror U? (Disable)"), &strEditDisable ) );
    pChild = pCategory->AddChildItem( new CCustomItemEdit( _T("Un Mirror V? (Disable)"), &strEditDisable ) );

    pCategory  = wndPropertyGrid.AddCategory( _T("MaterialExpression") );
    pChild = pCategory->AddChildItem( new CCustomItemEdit(_T("Desc"), &m_rVMCommand->m_strDesc ) );
    pChild->SetTooltip( _T("A Description that level designers can add (shows in the material editer UI).") );
}