#include "stdafx.h"

#include "../../VisualMaterialLib/Command/DxVMCommand_Parameters.h"

#include "./CustomItem.h"

#include "PropertyGridParameters.h"



//---------------------------------------------------------------------------------------------------------- PropertyGridVectorParameter
PropertyGridVectorParameter::PropertyGridVectorParameter()
{
}

PropertyGridVectorParameter::~PropertyGridVectorParameter()
{
}

void PropertyGridVectorParameter::CreatePropertyGrid( CXTPPropertyGrid& wndPropertyGrid, DxVMCommand* pVMCommand )
{
    CString	strEdit;

    // 기본적인 셋팅
    m_rVMCommand = dynamic_cast<DxVMCommand_VectorParameter*>( pVMCommand );

    // 리셋
    wndPropertyGrid.ResetContent();

    CXTPPropertyGridItem* pChild;

    // create document settings category.
    CXTPPropertyGridItem* pCategory  = wndPropertyGrid.AddCategory( _T("MaterialExpressionVectorParameter") );
    strEdit.Format ( _T("%f"), m_rVMCommand->m_sData.m_fR );
    pChild = pCategory->AddChildItem( new CCustomItemEditFloat( _T("R"), &m_rVMCommand->m_sData.m_fR, strEdit.GetString() ) );

    strEdit.Format ( _T("%f"), m_rVMCommand->m_sData.m_fG );
    pChild = pCategory->AddChildItem( new CCustomItemEditFloat( _T("G"), &m_rVMCommand->m_sData.m_fG, strEdit.GetString() ) );

    strEdit.Format ( _T("%f"), m_rVMCommand->m_sData.m_fB );
    pChild = pCategory->AddChildItem( new CCustomItemEditFloat( _T("B"), &m_rVMCommand->m_sData.m_fB, strEdit.GetString() ) );

    pCategory  = wndPropertyGrid.AddCategory( _T("MaterialExpressionParameter") );
    pChild = pCategory->AddChildItem( new CCustomItemEdit(_T("Parameter Name"), &m_rVMCommand->m_sData.m_strParameter ) );
    pChild->SetTooltip( _T("The name of the parameter.") );

    pCategory  = wndPropertyGrid.AddCategory( _T("MaterialExpression") );
    pChild = pCategory->AddChildItem( new CCustomItemEdit(_T("Desc"), &m_rVMCommand->m_strDesc ) );
    pChild->SetTooltip( _T("A Description that level designers can add (shows in the material editer UI).") );
}