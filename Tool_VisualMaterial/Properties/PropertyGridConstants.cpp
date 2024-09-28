#include "stdafx.h"

#include "../../VisualMaterialLib/Command/DxVMCommand_Constants.h"

#include "./CustomItem.h"

#include "PropertyGridConstants.h"


//---------------------------------------------------------------------------------------------------------- PropertyGridConstant
PropertyGridConstant::PropertyGridConstant()
{
}

PropertyGridConstant::~PropertyGridConstant()
{
}

void PropertyGridConstant::CreatePropertyGrid( CXTPPropertyGrid& wndPropertyGrid, DxVMCommand* pVMCommand )
{
    CString	strEdit;

    // 기본적인 셋팅
    m_rVMCommand = dynamic_cast<DxVMCommand_Constant*>( pVMCommand );

    // 리셋
    wndPropertyGrid.ResetContent();

    CXTPPropertyGridItem* pChild;

    // create document settings category.
    CXTPPropertyGridItem* pCategory  = wndPropertyGrid.AddCategory( _T("MaterialExpressionConstant") );
    strEdit.Format ( _T("%f"), m_rVMCommand->m_sData.m_fR );
    pChild = pCategory->AddChildItem( new CCustomItemEditFloat( _T("R"), &m_rVMCommand->m_sData.m_fR, strEdit.GetString() ) );

    pCategory  = wndPropertyGrid.AddCategory( _T("MaterialExpression") );
    pChild = pCategory->AddChildItem( new CCustomItemEdit(_T("Desc"), &m_rVMCommand->m_strDesc ) );
    pChild->SetTooltip( _T("A Description that level designers can add (shows in the material editer UI).") );
}


//---------------------------------------------------------------------------------------------------------- PropertyGridConstant2Vector
PropertyGridConstant2Vector::PropertyGridConstant2Vector()
{
}

PropertyGridConstant2Vector::~PropertyGridConstant2Vector()
{
}

void PropertyGridConstant2Vector::CreatePropertyGrid( CXTPPropertyGrid& wndPropertyGrid, DxVMCommand* pVMCommand )
{
    CString	strEdit;

    // 기본적인 셋팅
    m_rVMCommand = dynamic_cast<DxVMCommand_Constant2Vector*>( pVMCommand );

    // 리셋
    wndPropertyGrid.ResetContent();

    CXTPPropertyGridItem* pChild;

    // create document settings category.
    CXTPPropertyGridItem* pCategory  = wndPropertyGrid.AddCategory( _T("MaterialExpressionConstant") );
    strEdit.Format ( _T("%f"), m_rVMCommand->m_sData.m_fR );
    pChild = pCategory->AddChildItem( new CCustomItemEditFloat( _T("R"), &m_rVMCommand->m_sData.m_fR, strEdit.GetString() ) );

    strEdit.Format ( _T("%f"), m_rVMCommand->m_sData.m_fG );
    pChild = pCategory->AddChildItem( new CCustomItemEditFloat( _T("G"), &m_rVMCommand->m_sData.m_fG, strEdit.GetString() ) );

    pCategory  = wndPropertyGrid.AddCategory( _T("MaterialExpression") );
    pChild = pCategory->AddChildItem( new CCustomItemEdit(_T("Desc"), &m_rVMCommand->m_strDesc ) );
    pChild->SetTooltip( _T("A Description that level designers can add (shows in the material editer UI).") );
}


//---------------------------------------------------------------------------------------------------------- PropertyGridConstant3Vector
PropertyGridConstant3Vector::PropertyGridConstant3Vector()
{
}

PropertyGridConstant3Vector::~PropertyGridConstant3Vector()
{
}

void PropertyGridConstant3Vector::CreatePropertyGrid( CXTPPropertyGrid& wndPropertyGrid, DxVMCommand* pVMCommand )
{
    CString	strEdit;

    // 기본적인 셋팅
    m_rVMCommand = dynamic_cast<DxVMCommand_Constant3Vector*>( pVMCommand );

    // 리셋
    wndPropertyGrid.ResetContent();

    CXTPPropertyGridItem* pChild;

    // create document settings category.
    CXTPPropertyGridItem* pCategory  = wndPropertyGrid.AddCategory( _T("MaterialExpressionConstant") );
    strEdit.Format ( _T("%f"), m_rVMCommand->m_sData.m_fR );
    pChild = pCategory->AddChildItem( new CCustomItemEditFloat( _T("R"), &m_rVMCommand->m_sData.m_fR, strEdit.GetString() ) );

    strEdit.Format ( _T("%f"), m_rVMCommand->m_sData.m_fG );
    pChild = pCategory->AddChildItem( new CCustomItemEditFloat( _T("G"), &m_rVMCommand->m_sData.m_fG, strEdit.GetString() ) );

    strEdit.Format ( _T("%f"), m_rVMCommand->m_sData.m_fB );
    pChild = pCategory->AddChildItem( new CCustomItemEditFloat( _T("B"), &m_rVMCommand->m_sData.m_fB, strEdit.GetString() ) );

    pCategory  = wndPropertyGrid.AddCategory( _T("MaterialExpression") );
    pChild = pCategory->AddChildItem( new CCustomItemEdit(_T("Desc"), &m_rVMCommand->m_strDesc ) );
    pChild->SetTooltip( _T("A Description that level designers can add (shows in the material editer UI).") );
}


//---------------------------------------------------------------------------------------------------------- PropertyGridConstant4Vector
PropertyGridConstant4Vector::PropertyGridConstant4Vector()
{
}

PropertyGridConstant4Vector::~PropertyGridConstant4Vector()
{
}

void PropertyGridConstant4Vector::CreatePropertyGrid( CXTPPropertyGrid& wndPropertyGrid, DxVMCommand* pVMCommand )
{
    CString	strEdit;

    // 기본적인 셋팅
    m_rVMCommand = dynamic_cast<DxVMCommand_Constant4Vector*>( pVMCommand );

    // 리셋
    wndPropertyGrid.ResetContent();

    CXTPPropertyGridItem* pChild;

    // create document settings category.
    CXTPPropertyGridItem* pCategory  = wndPropertyGrid.AddCategory( _T("MaterialExpressionConstant") );
    strEdit.Format ( _T("%f"), m_rVMCommand->m_sData.m_fR );
    pChild = pCategory->AddChildItem( new CCustomItemEditFloat( _T("R"), &m_rVMCommand->m_sData.m_fR, strEdit.GetString() ) );

    strEdit.Format ( _T("%f"), m_rVMCommand->m_sData.m_fG );
    pChild = pCategory->AddChildItem( new CCustomItemEditFloat( _T("G"), &m_rVMCommand->m_sData.m_fG, strEdit.GetString() ) );

    strEdit.Format ( _T("%f"), m_rVMCommand->m_sData.m_fB );
    pChild = pCategory->AddChildItem( new CCustomItemEditFloat( _T("B"), &m_rVMCommand->m_sData.m_fB, strEdit.GetString() ) );

    strEdit.Format ( _T("%f"), m_rVMCommand->m_sData.m_fA );
    pChild = pCategory->AddChildItem( new CCustomItemEditFloat( _T("A"), &m_rVMCommand->m_sData.m_fA, strEdit.GetString() ) );

    pCategory  = wndPropertyGrid.AddCategory( _T("MaterialExpression") );
    pChild = pCategory->AddChildItem( new CCustomItemEdit(_T("Desc"), &m_rVMCommand->m_strDesc ) );
    pChild->SetTooltip( _T("A Description that level designers can add (shows in the material editer UI).") );
}