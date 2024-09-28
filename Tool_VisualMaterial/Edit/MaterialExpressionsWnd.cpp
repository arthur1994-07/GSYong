
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "../../VisualMaterialLib/ViewPort/DxViewPortPreview.h"
#include "../../VisualMaterialLib/Preview/NSVM_PREVIEW.h"
#include "../../VisualMaterialLib/DxVisualMaterial.h"

#include "../Tool_VisualMaterialView.h"
#include "../MainFrm.h"

#include "MaterialExpressionsWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CMaterialExpressionsWnd::CMaterialExpressionsWnd()
{
}

CMaterialExpressionsWnd::~CMaterialExpressionsWnd()
{
}

BEGIN_MESSAGE_MAP(CMaterialExpressionsWnd, CTreeCtrl)
    ON_WM_CREATE()
    ON_WM_LBUTTONDOWN()
    ON_WM_SETCURSOR()
END_MESSAGE_MAP()

int CMaterialExpressionsWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CTreeCtrl::OnCreate(lpCreateStruct) == -1)
        return -1;

    // TODO:  여기에 특수화된 작성 코드를 추가합니다.
    HTREEITEM htiParent;

    // Note : Add Command
    // add the children of the parent item
    htiParent = InsertItem( _T("Constants") );
    SetItemState( htiParent, TVIS_BOLD, TVIS_BOLD );
    InsertItem( NSVISUAL_MATERIAL::g_strVisualMaterialCommand[VMC_CONSTANT].c_str(), htiParent );
    InsertItem( NSVISUAL_MATERIAL::g_strVisualMaterialCommand[VMC_CONSTANT2VECTOR].c_str(), htiParent );
    InsertItem( NSVISUAL_MATERIAL::g_strVisualMaterialCommand[VMC_CONSTANT3VECTOR].c_str(), htiParent );
    InsertItem( NSVISUAL_MATERIAL::g_strVisualMaterialCommand[VMC_CONSTANT4VECTOR].c_str(), htiParent );
    Expand(htiParent, TVE_EXPAND);

    htiParent = InsertItem( _T("Coordinates") );
    SetItemState( htiParent, TVIS_BOLD, TVIS_BOLD );
    InsertItem( NSVISUAL_MATERIAL::g_strVisualMaterialCommand[VMC_PANNER].c_str(), htiParent );
    InsertItem( NSVISUAL_MATERIAL::g_strVisualMaterialCommand[VMC_ROTATOR].c_str(), htiParent );
    InsertItem( NSVISUAL_MATERIAL::g_strVisualMaterialCommand[VMC_TEXTURE_COORDINATE].c_str(), htiParent );
    Expand(htiParent, TVE_EXPAND);

    htiParent = InsertItem( _T("Math") );
    SetItemState( htiParent, TVIS_BOLD, TVIS_BOLD );
    InsertItem( NSVISUAL_MATERIAL::g_strVisualMaterialCommand[VMC_ADD].c_str(), htiParent );
    InsertItem( NSVISUAL_MATERIAL::g_strVisualMaterialCommand[VMC_MULTIPLY].c_str(), htiParent );
    InsertItem( NSVISUAL_MATERIAL::g_strVisualMaterialCommand[VMC_SINE].c_str(), htiParent );
    InsertItem( NSVISUAL_MATERIAL::g_strVisualMaterialCommand[VMC_SUBTRACT].c_str(), htiParent );
    Expand(htiParent, TVE_EXPAND);

    htiParent = InsertItem( _T("Parameters") );
    SetItemState( htiParent, TVIS_BOLD, TVIS_BOLD );
    InsertItem( NSVISUAL_MATERIAL::g_strVisualMaterialCommand[VMC_VECTOR_PARAMETER].c_str(), htiParent );
    Expand(htiParent, TVE_EXPAND);

    htiParent = InsertItem( _T("Texture") );
    SetItemState( htiParent, TVIS_BOLD, TVIS_BOLD );
    InsertItem( NSVISUAL_MATERIAL::g_strVisualMaterialCommand[VMC_TEXTURESAMPLE].c_str(), htiParent );
	//InsertItem( NSVISUAL_MATERIAL::g_strVisualMaterialCommand[VMC_TEXTURE_REFRACT].c_str(), htiParent );
    Expand(htiParent, TVE_EXPAND);

    htiParent = InsertItem( _T("Utility") );
    SetItemState( htiParent, TVIS_BOLD, TVIS_BOLD );
    InsertItem( NSVISUAL_MATERIAL::g_strVisualMaterialCommand[VMC_CLAMP].c_str(), htiParent );
    InsertItem( NSVISUAL_MATERIAL::g_strVisualMaterialCommand[VMC_LINEAR_INTERPOLATE].c_str(), htiParent );
    InsertItem( NSVISUAL_MATERIAL::g_strVisualMaterialCommand[VMC_TIME].c_str(), htiParent );
    Expand(htiParent, TVE_EXPAND);

    htiParent = InsertItem( _T("VectorOps") );
    SetItemState( htiParent, TVIS_BOLD, TVIS_BOLD );
    InsertItem( NSVISUAL_MATERIAL::g_strVisualMaterialCommand[VMC_FRESNEL].c_str(), htiParent );
    Expand(htiParent, TVE_EXPAND);

    htiParent = InsertItem( _T("Vectors") );
    SetItemState( htiParent, TVIS_BOLD, TVIS_BOLD );
    InsertItem( NSVISUAL_MATERIAL::g_strVisualMaterialCommand[VMC_REFLECTION_VECTOR].c_str(), htiParent );
    Expand(htiParent, TVE_EXPAND);

    return 0;
}

void CMaterialExpressionsWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
     
    NSVISUAL_MATERIAL::g_nSelectCommandVM = -1;

    HTREEITEM htreeitemCur = HitTest(point);
    if ( htreeitemCur )
    {
        CString sSelectedFrame = GetItemText( htreeitemCur );

        for ( DWORD i=0; i<VMC_SIZE; ++i )
        {
            if ( NSVISUAL_MATERIAL::g_strVisualMaterialCommand[i] == sSelectedFrame.GetString() )
            {
                NSVISUAL_MATERIAL::g_nSelectCommandVM = i;
            }
        }
    }

    CTreeCtrl::OnLButtonDown(nFlags, point);
}

BOOL CMaterialExpressionsWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

    if ( NSVISUAL_MATERIAL::g_nSelectCommandVM == -1 )
    {
        SetCursor( LoadCursor( NULL, IDC_ARROW ) );
    }
    else
    {
        SetCursor( LoadCursor( NULL, IDC_HAND ) );
    }

    return TRUE;

    //return CTreeCtrl::OnSetCursor(pWnd, nHitTest, message);
}
