#include "stdafx.h"

#include "../../VisualMaterialLib/Command/DxVMCommand_Math.h"

#include "./CustomItem.h"

#include "PropertyGridMath.h"


//---------------------------------------------------------------------------------------------------------- PropertyGridSine
PropertyGridSine::PropertyGridSine()
{
}

PropertyGridSine::~PropertyGridSine()
{
}

void PropertyGridSine::CreatePropertyGrid( CXTPPropertyGrid& wndPropertyGrid, DxVMCommand* pVMCommand )
{
    CString	strEdit;

    // �⺻���� ����
    m_rVMCommand = dynamic_cast<DxVMCommand_Sine*>( pVMCommand );

    // ����
    wndPropertyGrid.ResetContent();

    CXTPPropertyGridItem* pChild;

    // create document settings category.
    CXTPPropertyGridItem* pCategory  = wndPropertyGrid.AddCategory( _T("MaterialExpressionConstant") );
    strEdit.Format ( _T("%f"), m_rVMCommand->m_sData.m_fPeriod );
    pChild = pCategory->AddChildItem( new CCustomItemEditFloat( _T("Period"), &m_rVMCommand->m_sData.m_fPeriod, strEdit.GetString() ) );

    pCategory  = wndPropertyGrid.AddCategory( _T("MaterialExpression") );
    pChild = pCategory->AddChildItem( new CCustomItemEdit(_T("Desc"), &m_rVMCommand->m_strDesc ) );
    pChild->SetTooltip( _T("A Description that level designers can add (shows in the material editer UI).") );
}
