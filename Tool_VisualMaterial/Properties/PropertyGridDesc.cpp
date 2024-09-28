#include "stdafx.h"

#include "../../VisualMaterialLib/Command/DxVMCommand.h"

#include "./CustomItem.h"

#include "PropertyGridDesc.h"


//---------------------------------------------------------------------------------------------------------- PropertyGridDesc
PropertyGridDesc::PropertyGridDesc()
{
}

PropertyGridDesc::~PropertyGridDesc()
{
}

void PropertyGridDesc::CreatePropertyGrid( CXTPPropertyGrid& wndPropertyGrid, DxVMCommand* pVMCommand )
{
    CString	strEdit;

    // 기본적인 셋팅
    TSTRING* pstrDesc = pVMCommand->GetDescPointer();

    // 리셋
    wndPropertyGrid.ResetContent();

    CXTPPropertyGridItem* pChild;

    // create document settings category.
    CXTPPropertyGridItem* pCategory  = wndPropertyGrid.AddCategory( _T("MaterialExpressionConstant") );

    pCategory  = wndPropertyGrid.AddCategory( _T("MaterialExpression") );
    pChild = pCategory->AddChildItem( new CCustomItemEdit(_T("Desc"), pstrDesc ) );
    pChild->SetTooltip( _T("A Description that level designers can add (shows in the material editer UI).") );
}


