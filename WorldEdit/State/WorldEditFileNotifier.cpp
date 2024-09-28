#include "stdafx.h"
#include "WorldEditFileNotifier.h"
#include "WorldStateMachine.h"

#include "../WorldEdit.h"
#include "../MainFrm.h"
#include "../WorldEditDoc.h"
#include "../WorldEditView.h"
#include "../SheetWithTab.h"

#include "../../enginelib/DxLand/DxPiece/DxPieceContainer.h"
#include "../../enginelib/DxLand/DxPiece/DxSingleTexMesh.h"
#include "../../enginelib/DxLand/DxPiece/DxPieceAABB.h"

bool CFrameMeshXFileNotifier::Init()
{
	DxSetLandMan* pSetLandMan = 0;	
	Param() >> pSetLandMan >> Reset();
    return SetFile(GetFile(pSetLandMan));
}

int CFrameMeshXFileNotifier::OnNotify()
{
	DxSetLandMan* pSetLandMan = 0;
	CsheetWithTab* pSheetWithTab;
	Param() >> pSetLandMan >> pSheetWithTab >> Reset();

	pSheetWithTab->m_FramePage.LoadXFile(m_strFile.c_str());

	return RETURNCODE_EXIT;
}

TSTRING CFrameMeshXFileNotifier::GetFile(DxSetLandMan* pSetLandMan)
{
    DxFrameMesh* pFrameMesh = pSetLandMan->GetFrameMesh();

    if (pFrameMesh != 0 && pFrameMesh->GetFileName() != 0)
    {
        return pFrameMesh->GetFileName();
    }

    return _T("");
}

bool CTextureFileNotifier::Init()
{
	const TCHAR* pFile = 0;
	Param() >> pFile >> Reset();

    TSTRING strFile = pFile;
    TextureManager::GetInstance().FindFilePath(strFile);

	return SetFile(strFile);
}

int CTextureFileNotifier::OnNotify()
{
	const TCHAR* pFile = 0;
	TextureManager::Resource* pRes = 0;
	Param() >> pFile >> pRes >> Reset();

	TextureManager::GetInstance().ReLoadTexture(GetFileName(), *pRes, false,
		TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D, 0, D3DFMT_UNKNOWN, TEXTURE_RESIZE_NONE, TEXTURE_ETC, false, FALSE);

	return RETURNCODE_SUCCESS;
}

bool CTextureFileNotifier::GetFile(TSTRING& strFile)
{
    return TextureManager::GetInstance().FindFilePath(strFile);
}

TSTRING CTextureFileNotifier::GetFile(SMATERIAL_PIECE* pMtlPiece, DXMATERIALEX* pDxMtl)
{
    TSTRING strFile;

    if (pMtlPiece->m_textureRes.IsValid())
    {
        strFile = pMtlPiece->m_textureRes.GetFileName();
    }
    else
    {
        strFile = pDxMtl->m_textureRes.GetFileName();
    }

    GetFile(strFile);

    assert(strFile.empty() == false);
    return strFile;
}

bool CPieceFileNotifier::Init()
{
	DxPieceManagerPSF::MATRIXPISNAME* pPieceInfo = 0;
	Param() >> pPieceInfo >> Reset();

	TSTRING strPath = DxPieceContainer::GetInstance().GetPath();
	strPath += pPieceInfo->m_pPisName;

	return SetFile(strPath);
}

int CPieceFileNotifier::OnNotify()
{
	DxPieceManagerPSF::MATRIXPISNAME* pPieceInfo = 0;
	Param() >> pPieceInfo >> Reset();

	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity(&matIdentity);
	LPDIRECT3DDEVICEQ pDevice = GetDevice();

	DxPieceEdit* pPieceEdit = 
		DxPieceContainer::GetInstance().ReLoadPiece(pDevice, pPieceInfo->m_pPiece, pPieceInfo->m_pPisName);

	if (pPieceEdit != 0)
	{
		pPieceInfo->m_pPiece->Import
		(
			pDevice, 
			pPieceEdit, 
			pPieceInfo->m_pPiece->m_matWorld, 
			matIdentity, 
			pPieceInfo->m_pPiece->GetVisibleRadius(), 
			pPieceInfo->m_pPiece->GetInvisibleRadius(),
			pPieceInfo->m_pPiece->IsExceptLowOption() 
		);
	}

	return RETURNCODE_SUCCESS;
}

LPDIRECT3DDEVICEQ CPieceFileNotifier::GetDevice()
{
	CWorldEditApp* pApp	= static_cast<CWorldEditApp*>(AfxGetApp());    
	CMainFrame* pFrame = static_cast<CMainFrame*>(pApp->m_pMainWnd);    
	CWorldEditView* pView = static_cast<CWorldEditView*>(pFrame->GetActiveView());    
	return pView->GetD3DDevice();
}

TSTRING CPieceFileNotifier::GetFile(const TCHAR* pPisName)
{
    TSTRING strPath = DxPieceContainer::GetInstance().GetPath();
    strPath += pPisName;
    return strPath;
}

bool CPieceWldFileNotifier::Init()
{
	DxPiece* pPiece = 0;
	Param() >> pPiece >> Reset();

	if ( !pPiece )
		return false;

	TSTRING strPath = DxPieceContainer::GetInstance().GetPath();
	strPath += pPiece->m_strName;

	return SetFile(strPath);
}

int CPieceWldFileNotifier::OnNotify()
{
	DxPiece* pPiece = 0;
	Param() >> pPiece >> Reset();

	if ( !pPiece )
		return RETURNCODE_FAIL;

	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity(&matIdentity);
	LPDIRECT3DDEVICEQ pDevice = GetDevice();

	DxPieceEdit* pPieceEdit = 
		DxPieceContainer::GetInstance().ReLoadPiece(pDevice, pPiece, pPiece->m_strName.c_str());

	if (pPieceEdit != 0)
	{
		pPiece->Import(pDevice, pPieceEdit, pPiece->m_matWorld, matIdentity, pPiece->GetVisibleRadius(), pPiece->GetInvisibleRadius(), pPiece->IsExceptLowOption() );
	}

	return RETURNCODE_SUCCESS;
}

LPDIRECT3DDEVICEQ CPieceWldFileNotifier::GetDevice()
{
	CWorldEditApp* pApp	= static_cast<CWorldEditApp*>(AfxGetApp());    
	CMainFrame* pFrame = static_cast<CMainFrame*>(pApp->m_pMainWnd);    
	CWorldEditView* pView = static_cast<CWorldEditView*>(pFrame->GetActiveView());    
	return pView->GetD3DDevice();
}

TSTRING CPieceWldFileNotifier::GetFile(const TCHAR* pPisName)
{
	TSTRING strPath = DxPieceContainer::GetInstance().GetPath();
	strPath += pPisName;
	return strPath;
}
