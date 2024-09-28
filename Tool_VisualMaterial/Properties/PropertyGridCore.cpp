#include "stdafx.h"

#include "../../VisualMaterialLib/Command/DxVMCommand_Core.h"
#include "../../VisualMaterialLib/Preview/NSVM_PREVIEW.h"

#include "./CustomItem.h"

#include "PropertyGridCore.h"


PropertyGridCore::PropertyGridCore()
{
}

PropertyGridCore::~PropertyGridCore()
{
}

void PropertyGridCore::CreatePropertyGrid( CXTPPropertyGrid& wndPropertyGrid, DxVMCommand* pVMCommand )
{
    CString	strEdit;

    // 기본적인 셋팅
    m_rVMCommand = dynamic_cast<DxVMCommand_Core*>( pVMCommand );

    // 리셋
    wndPropertyGrid.ResetContent();

    // create document settings category.
    CXTPPropertyGridItem* pCategory;
    CXTPPropertyGridItemBool* pItemBool;
    CXTPPropertyGridItem* pChild;
    CXTPPropertyGridItemConstraints* pList;

    // Material
    {
        pCategory = wndPropertyGrid.AddCategory( _T("Material") );


        strEdit.Format ( _T("%f"), m_rVMCommand->m_fOpacityMaskClipValue );
        pChild = pCategory->AddChildItem( new CCustomItemEditFloat( _T("Opacity Mask Clip Value"), &m_rVMCommand->m_fOpacityMaskClipValue, strEdit.GetString() ) );
        pChild->SetTooltip( _T("If BlendMode is BLEND_Masked, the surface is not rendered where OpacityMask < OpacityMaskClipVaule.") );


        pChild = pCategory->AddChildItem( new CCustomItemComboBox( _T("Blend Mode"), 
                                                                    &m_rVMCommand->m_nBlendMode, 
                                                                    NSVISUAL_MATERIAL::g_strBlendMode[m_rVMCommand->m_nBlendMode].c_str() ) );
		pList = pChild->GetConstraints();
        for ( DWORD i=0; i<VM_BLENDMODE_SIZE; ++i )
        {
            pList->AddConstraint( NSVISUAL_MATERIAL::g_strBlendMode[i].c_str() );
        }
        pChild->SetTooltip( _T("Determines how the material's color is blended with background colors.") );



        pChild = pCategory->AddChildItem( new CCustomItemComboBox( _T("Two Side Mode"), 
                                                                    &m_rVMCommand->m_nTwoSidedMode, 
                                                                    NSVISUAL_MATERIAL::g_strTwoSideMode[m_rVMCommand->m_nTwoSidedMode].c_str() ) );
		pList = pChild->GetConstraints();
        for ( DWORD i=0; i<VM_TWOSIDEDMODE_SIZE; ++i )
        {
            pList->AddConstraint( NSVISUAL_MATERIAL::g_strTwoSideMode[i].c_str() );
        }
        pChild->SetTooltip( _T("TwoSide - Indicated that the material should be rendered without backface culling and normal should be filpped for backfaces \r\n \
TwoSideSeperatePass - Indicated that the material should be rendered in its own pass. Used for hair rendering") );



        pItemBool = dynamic_cast<CXTPPropertyGridItemBool*>( pCategory->AddChildItem( new CXTPPropertyGridItemBool( _T("Wireframe"), 
                                                                                                    m_rVMCommand->m_bWireframe, 
                                                                                                    &m_rVMCommand->m_bWireframe ) ) );
        pItemBool->SetCheckBoxStyle();



        pItemBool = dynamic_cast<CXTPPropertyGridItemBool*>( pCategory->AddChildItem( new CXTPPropertyGridItemBool( _T("CharUV_MIRROR"), 
                                                                                                    m_rVMCommand->m_bCharUV_MIRROR, 
                                                                                                    &m_rVMCommand->m_bCharUV_MIRROR ) ) );
        pItemBool->SetCheckBoxStyle();



        pItemBool = dynamic_cast<CXTPPropertyGridItemBool*>( pCategory->AddChildItem( new CXTPPropertyGridItemBool( _T("UserColorOverlay"), 
                                                                                                    m_rVMCommand->m_bUserColorOverlay, 
                                                                                                    &m_rVMCommand->m_bUserColorOverlay ) ) );
        pItemBool->SetCheckBoxStyle();
    }

    // MaterialColorPower
    {
        pCategory  = wndPropertyGrid.AddCategory( _T("MaterialColorPower") );


        strEdit.Format ( _T("%f"), m_rVMCommand->m_fDiffusePower );
        pChild = pCategory->AddChildItem( new CCustomItemEditFloat( _T("DiffusePower"), &m_rVMCommand->m_fDiffusePower, strEdit.GetString() ) );
        pChild->SetTooltip( _T("Diffuse Power") );


        strEdit.Format ( _T("%f"), m_rVMCommand->m_fAmbientPower );
        pChild = pCategory->AddChildItem( new CCustomItemEditFloat( _T("AmbientPower"), &m_rVMCommand->m_fAmbientPower, strEdit.GetString() ) );
        pChild->SetTooltip( _T("Ambient Power") );


        strEdit.Format ( _T("%f"), m_rVMCommand->m_fSpecularPower );
        pChild = pCategory->AddChildItem( new CCustomItemEditFloat( _T("SpecularPower"), &m_rVMCommand->m_fSpecularPower, strEdit.GetString() ) );
        pChild->SetTooltip( _T("Specular Power") );


        pItemBool = dynamic_cast<CXTPPropertyGridItemBool*>( pCategory->AddChildItem( new CXTPPropertyGridItemBool( _T("ShininessPower Use Texture"), 
                                                                                                    m_rVMCommand->m_bShininessPowerTexture, 
                                                                                                    &m_rVMCommand->m_bShininessPowerTexture ) ) );
        pItemBool->SetCheckBoxStyle();
        pItemBool->SetTooltip( _T("광택도 계산을 Texture를 사용할 것인가~? 아니면 함수를 사용할 것인가~? \r\n \
TRUE - Used Texture ( 입력 범위 : 0 ~ 200 ), FALSE - pow 함수 사용 ( 입력 범위 : 0 ~ 제한없음 ) \r\n \
TRUE - 속도가 빠르다.                        FALSE - 속도가 느리다. \r\n \
TRUE - 품질저하가 있다.                      FALSE - 품질이 좋다.") );



        strEdit.Format ( _T("%f"), m_rVMCommand->m_fShininessPower );
        pChild = pCategory->AddChildItem( new CCustomItemEditFloat( _T("ShininessPower"), &m_rVMCommand->m_fShininessPower, strEdit.GetString() ) );
        pChild->SetTooltip( _T("Shininess Power") );

        strEdit.Format ( _T("%f"), m_rVMCommand->m_fLevelPower );
        pChild = pCategory->AddChildItem( new CCustomItemEditFloat( _T("LevelPower"), &m_rVMCommand->m_fLevelPower, strEdit.GetString() ) );
        pChild->SetTooltip( _T("Ran-Online의 Item Level에서 사용하는 수치\r\n \
Ambient 값의 세기를 조정한다.") );

        strEdit.Format ( _T("%f"), m_rVMCommand->m_fLevelFlowPower );
        pChild = pCategory->AddChildItem( new CCustomItemEditFloat( _T("LevelFlowPower"), &m_rVMCommand->m_fLevelFlowPower, strEdit.GetString() ) );
        pChild->SetTooltip( _T("Ran-Online의 Item Level에서 사용하는 수치\r\n \
4,5 단계에서 볼수있는 흐르는 효과의 Effect 세기를 조정한다.") );
    }

    // Translucency
    {
        pCategory  = wndPropertyGrid.AddCategory( _T("Translucency") );


        pItemBool = dynamic_cast<CXTPPropertyGridItemBool*>( pCategory->AddChildItem( new CXTPPropertyGridItemBool( _T("Disable Depth Test?"), 
                                                                                                    m_rVMCommand->m_bDisableDepthTest, 
                                                                                                    &m_rVMCommand->m_bDisableDepthTest ) ) );
        pItemBool->SetCheckBoxStyle();
        pItemBool->SetTooltip( _T("Allows the material to disable depth tests, which is only meaningful with translucent blend mode \r\n \
Disalbing depth tests will make rendering significantly slower since no occlued pixels can get zculled") );
    }

    // UserColor
    {
        pCategory  = wndPropertyGrid.AddCategory( _T("UserColor Test") );


        strEdit.Format ( _T("%f"), NSVM_PREVIEW::g_vUserColor1.x );
        pChild = pCategory->AddChildItem( new CCustomItemEditFloat( _T("UserColor1_R"), &NSVM_PREVIEW::g_vUserColor1.x, strEdit.GetString() ) );
        pChild->SetTooltip( _T("Diffuse Power") );

        strEdit.Format ( _T("%f"), NSVM_PREVIEW::g_vUserColor1.y );
        pChild = pCategory->AddChildItem( new CCustomItemEditFloat( _T("UserColor1_G"), &NSVM_PREVIEW::g_vUserColor1.y, strEdit.GetString() ) );
        pChild->SetTooltip( _T("Ambient Power") );

        strEdit.Format ( _T("%f"), NSVM_PREVIEW::g_vUserColor1.z );
        pChild = pCategory->AddChildItem( new CCustomItemEditFloat( _T("UserColor1_B"), &NSVM_PREVIEW::g_vUserColor1.z, strEdit.GetString() ) );
        pChild->SetTooltip( _T("Specular Power") );


        strEdit.Format ( _T("%f"), NSVM_PREVIEW::g_vUserColor2.x );
        pChild = pCategory->AddChildItem( new CCustomItemEditFloat( _T("UserColor2_R"), &NSVM_PREVIEW::g_vUserColor2.x, strEdit.GetString() ) );
        pChild->SetTooltip( _T("Diffuse Power") );

        strEdit.Format ( _T("%f"), NSVM_PREVIEW::g_vUserColor2.y );
        pChild = pCategory->AddChildItem( new CCustomItemEditFloat( _T("UserColor2_G"), &NSVM_PREVIEW::g_vUserColor2.y, strEdit.GetString() ) );
        pChild->SetTooltip( _T("Ambient Power") );

        strEdit.Format ( _T("%f"), NSVM_PREVIEW::g_vUserColor2.z );
        pChild = pCategory->AddChildItem( new CCustomItemEditFloat( _T("UserColor2_B"), &NSVM_PREVIEW::g_vUserColor2.z, strEdit.GetString() ) );
        pChild->SetTooltip( _T("Specular Power") );
    }

    // MaterialInterface
    {
        pCategory  = wndPropertyGrid.AddCategory( _T("MaterialInterface") );


        pChild = pCategory->AddChildItem( new CCustomItemFileBox( _T("Preview Mesh"), EMFM_XFILE, &m_rVMCommand->m_strPreviewMesh, pVMCommand ) );
        pChild->SetTooltip( _T("The mesh used by the material editer to preview the material") );
    }
}