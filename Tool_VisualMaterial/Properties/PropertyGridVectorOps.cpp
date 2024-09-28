#include "stdafx.h"

#include "../../VisualMaterialLib/Command/DxVMCommand_VectorOps.h"

#include "./CustomItem.h"

#include "PropertyGridVectorOps.h"



//---------------------------------------------------------------------------------------------------------- PropertyGridFresnel
PropertyGridFresnel::PropertyGridFresnel()
{
}

PropertyGridFresnel::~PropertyGridFresnel()
{
}

void PropertyGridFresnel::CreatePropertyGrid( CXTPPropertyGrid& wndPropertyGrid, DxVMCommand* pVMCommand )
{
    CString	strEdit;

    // 기본적인 셋팅
    m_rVMCommand = dynamic_cast<DxVMCommand_Fresnel*>( pVMCommand );

    // 리셋
    wndPropertyGrid.ResetContent();

    CXTPPropertyGridItem* pChild;

    // create document settings category.
    CXTPPropertyGridItem* pCategory  = wndPropertyGrid.AddCategory( _T("MaterialExpressionConstant") );
    strEdit.Format ( _T("%f"), m_rVMCommand->m_fExponent );
    pChild = pCategory->AddChildItem( new CCustomItemEditFloat( _T("Exponent"), &m_rVMCommand->m_fExponent, strEdit.GetString() ) );

    pCategory  = wndPropertyGrid.AddCategory( _T("MaterialExpression") );
    pChild = pCategory->AddChildItem( new CCustomItemEdit(_T("Desc"), &m_rVMCommand->m_strDesc ) );
    pChild->SetTooltip( _T("A Description that level designers can add (shows in the material editer UI).") );
}