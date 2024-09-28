#include "stdafx.h"

#include "../../EngineLib/Common/SubPath.h"
#include "../../EngineLib/DxTools/TextureManager.h"
#include "../../VisualMaterialLib/Command/DxVMCommand.h"
#include "Tool_VisualMaterial.h"

#include "CustomItem.h"

extern TSTRING g_strAppPath;

//------------------------------------------------------------------------------------------------------ CCustomItemFileBox
CCustomItemFileBox::CCustomItemFileBox(CString strCaption, 
                                       const TSTRING& strDefExt, 
                                       const TSTRING& strFilter, 
                                       const TSTRING& strPath, 
                                       TSTRING* pstrEdit,
                                       DxVMCommand* pVMCommand )
	: CXTPPropertyGridItem( strCaption, pstrEdit->c_str() )
    , m_strDefExt(strDefExt)
    , m_strFilter(strFilter)
    , m_strPath(strPath)
    , m_rstrEdit(pstrEdit)
    , m_rVMCommand(pVMCommand)
{
	m_nFlags = xtpGridItemHasExpandButton|xtpGridItemHasEdit;
}

CCustomItemFileBox::CCustomItemFileBox( CString strCaption, 
                                        FILE_TYPE emFileType, 
                                        TSTRING* pstrEdit,
                                        DxVMCommand* pVMCommand )
    : CXTPPropertyGridItem( strCaption, pstrEdit->c_str() )
    , m_rstrEdit(pstrEdit)
    , m_rVMCommand(pVMCommand)
{
    m_nFlags = xtpGridItemHasExpandButton|xtpGridItemHasEdit;

    CTool_VisualMaterialApp* pApp = (CTool_VisualMaterialApp*) AfxGetApp();
    const SUBPATH* pPath = pApp->GetSubPath();

    switch( emFileType )
    {
    case EMFM_TEXTURE:
        m_strDefExt = _T(".dds");
        m_strFilter = _T("Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|");
        m_strPath = TextureManager::GetInstance().GetPath().c_str();
        break;

    case EMFM_XFILE:
        m_strDefExt = _T(".x");
        m_strFilter = _T("X - file  (*.x)|*.x|");
        m_strPath = g_strAppPath;
	    m_strPath += pPath->ObjFilePieceEdit(); //SUBPATH::OBJ_FILE_PIECEEDIT;
        break;
    };
}

void CCustomItemFileBox::OnInplaceButtonDown(CXTPPropertyGridInplaceButton* /*pButton*/)
{
	CFileDialog dlg( TRUE, m_strDefExt.c_str(),  GetValue(), OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, m_strFilter.c_str(), m_pGrid);
    dlg.m_ofn.lpstrInitialDir = m_strPath.c_str();
	if ( dlg.DoModal( ) == IDOK )
	{
		OnValueChanged( dlg.GetFileName() );
		m_pGrid->Invalidate( FALSE );
	}
}

void CCustomItemFileBox::OnValueChanged(CString strValue) 
{
	CXTPPropertyGridItem::OnValueChanged(strValue);

    *m_rstrEdit = strValue.GetString();

    // 모든 정보를 재 정리한다.
    m_rVMCommand->Refresh();
}


//------------------------------------------------------------------------------------------------------ CCustomItemEdit
CCustomItemEdit::CCustomItemEdit( CString strCaption, TSTRING* pstrEdit )
	: CXTPPropertyGridItem( strCaption, pstrEdit->c_str() )
    , m_rstrEdit(pstrEdit)
{
	m_nFlags = xtpGridItemHasEdit;
}

void CCustomItemEdit::OnValueChanged(CString strValue) 
{
	CXTPPropertyGridItem::OnValueChanged(strValue);

    *m_rstrEdit = strValue.GetString();
}


//------------------------------------------------------------------------------------------------------ CCustomItemEditFloat
CCustomItemEditFloat::CCustomItemEditFloat( CString strCaption, float* pfValue, const TCHAR* pEdit )
	: CXTPPropertyGridItem( strCaption, pEdit )
    , m_rfValue(pfValue)
{
	m_nFlags = xtpGridItemHasEdit;
}

void CCustomItemEditFloat::OnValueChanged(CString strValue) 
{
	CXTPPropertyGridItem::OnValueChanged(strValue);

    *m_rfValue = static_cast<float>( _tstof ( strValue.GetString() ) );
}


//------------------------------------------------------------------------------------------------------ CCustomItemComboBox
CCustomItemComboBox::CCustomItemComboBox( CString strCaption, int* pnValue, const TCHAR* pEdit )
	: CXTPPropertyGridItem( strCaption, pEdit )
    , m_rnValue(pnValue)
{
	m_nFlags = xtpGridItemHasComboButton;
}

void CCustomItemComboBox::OnValueChanged(CString strValue) 
{
    CXTPPropertyGridItemConstraints* pList = GetConstraints();

    for ( int i=0; i<pList->GetCount(); ++i )
    {
        if ( strValue == pList->GetConstraintAt(i)->m_strConstraint )
        {
            *m_rnValue = i;
            CXTPPropertyGridItem::OnValueChanged(strValue);
            return;
        }
    }
    
    CXTPPropertyGridItem::OnValueChanged(strValue);
}



//------------------------------------------------------------------------------------------------------ CCustomItemCheckBox
CCustomItemCheckBox::CCustomItemCheckBox( CString strCaption, BOOL bValue, BOOL* pbValue, DxVMCommand* pVMCommand )
	: CXTPPropertyGridItemBool( strCaption, bValue, pbValue )
    , m_rVMCommand(pVMCommand)
{
}

void CCustomItemCheckBox::SetBool(BOOL bValue)
{
    CXTPPropertyGridItemBool::SetBool( bValue );

    // 모든 정보를 재 정리한다.
    m_rVMCommand->Refresh();
}