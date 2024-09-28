#include "stdafx.h"

#include "../../VisualMaterialLib/Command/DxVMCommand_Texture.h"

#include "./CustomItem.h"

#include "PropertyGridTexture.h"


PropertyGridTextureSample::PropertyGridTextureSample()
{
}

PropertyGridTextureSample::~PropertyGridTextureSample()
{
}

void PropertyGridTextureSample::CreatePropertyGrid( CXTPPropertyGrid& wndPropertyGrid, DxVMCommand* pVMCommand )
{
    // 기본적인 셋팅
    m_rVMCommand = dynamic_cast<DxVMCommand_TextureSample*>( pVMCommand );

    // 리셋
    wndPropertyGrid.ResetContent();

    CXTPPropertyGridItem* pChild;
    CXTPPropertyGridItemBool* pItemBool;

    // create document settings category.
	CXTPPropertyGridItem* pCategory = wndPropertyGrid.AddCategory( _T("MaterialExpressionTextureSample") );
    pChild = pCategory->AddChildItem( new CCustomItemFileBox( _T("Texture"), EMFM_TEXTURE, &m_rVMCommand->m_strTexture, pVMCommand ) );


    //pItemBool = dynamic_cast<CXTPPropertyGridItemBool*>( pCategory->AddChildItem( new CXTPPropertyGridItemBool( _T("CubeTexture"), 
    //                                                                                                m_rVMCommand->m_bCubeTexture, 
    //                                                                                                &m_rVMCommand->m_bCubeTexture ) ) );
    pItemBool = dynamic_cast<CXTPPropertyGridItemBool*>( pCategory->AddChildItem( new CCustomItemCheckBox( _T("CubeTexture"), 
                                                                                                            m_rVMCommand->m_bCubeTexture, 
                                                                                                            &m_rVMCommand->m_bCubeTexture, 
                                                                                                            pVMCommand ) ) );
    pItemBool->SetCheckBoxStyle();
    pItemBool->SetTooltip( _T("Texture2D - Disable Check, TextureCube - Enable Check") );

    

    pCategory  = wndPropertyGrid.AddCategory( _T("MaterialExpression") );
    pChild = pCategory->AddChildItem( new CCustomItemEdit( _T("Desc"), &m_rVMCommand->m_strDesc ) );
    pChild->SetTooltip( _T("A Description that level designers can add (shows in the material editer UI).") );
}

