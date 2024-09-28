#include "stdafx.h"
#include "WorldEditState.h"
#include "LightEditState.h"

#include "../WorldEdit.h"
#include "../MainFrm.h"
#include "../WorldEditDoc.h"
#include "../WorldEditView.h"
#include "../SheetWithTab.h"
#include "../AssetDlg.h"

#include "../enginelib/DxTools/Material/NSMaterialManager.h"
#include "../enginelib/Water/NSWaterMain.h"

#include "../enginelib/Archive/Archive.h"

CFrameEditState::CFrameEditState() : 
    CEditState(new CLightEditState)
{

}

int CFrameEditState::GetState()
{
    return FRAMEPAGEINDEX;
}

void CFrameEditState::SetFocus(int nOld)
{

}

void CFrameEditState::Reload(CParam& kParam)
{
    MIN_ASSERT(m_strPath.empty() == false && _T("���� ������ �����ϴ�."));

    CWorldEditApp* pApp			= static_cast<CWorldEditApp*>(AfxGetApp());
    CMainFrame* pFrame			= static_cast<CMainFrame*>(pApp->m_pMainWnd);
    CWorldEditView* pView		= static_cast<CWorldEditView*>(pFrame->GetActiveView());
    CWorldEditDoc* pDoc			= static_cast<CWorldEditDoc*>(pFrame->GetActiveDocument());
    DxSetLandMan* pSetLandMan	= pView->GetSetLandMan();
    DxLandMan* pLandMan			= pView->GetLandMan();
    LPDIRECT3DDEVICEQ pDevice	= pView->GetD3DDevice();

    RemoveAllTexture();
    RemoveAllPiece();

    TSTRING strCur = SetBackUpCurrentDirectory(true);    

	// x ���ϰ� psf��, ���� ���¸� psf ���Ϸ� ������ ��, �� �̸����� �ε� �� �� �ְ� �۾��Ѵ�.
	BOOL bSavePsfFile(FALSE);
	TSTRING strBackUp = m_strFile;
	TSTRING strExt = sc::string::getFileExt( m_strFile.c_str() );
	sc::string::lowerString( strExt );
	if ( strExt == _T("x") || strExt == _T("psf") )
	{
		bSavePsfFile = TRUE;

		DWORD dwTime = timeGetTime();
		TSTRINGSTREAM strStringStream;
		strStringStream << dwTime;
		m_strFile = strStringStream.str();
		pSetLandMan->SaveSet ( pView->GetD3DDevice (), m_strFile.c_str() );
	}

	NSMaterialManager::SimpleReload( pDevice );

    if (pSetLandMan != 0 && pSetLandMan->GetFrameMesh()->GetFrameRoot() != 0)
    {
        pDoc->LoadSettingFile(m_strFile.c_str());
    }
    else if (pLandMan != 0 && pLandMan->GetSerialFile().IsOpen() == TRUE)
    {
        pDoc->LoadExportFile(m_strFile.c_str());
    }

	// �ӽ÷� ������ ������ �����.
	if ( bSavePsfFile )
	{
		DeleteFile ( m_strFile.c_str() );
		m_strFile = strBackUp;
		pDoc->SetTitleAddVersion( m_strFile.c_str() );
	}

    ::SetCurrentDirectory(strCur.c_str());
}

TSTRING CFrameEditState::SetBackUpCurrentDirectory(bool bSet, const TCHAR* pFile)
{
    TCHAR temp[MAX_PATH]= {0};
    ::GetCurrentDirectory(256, temp);

    if (bSet)
    {
        ::SetCurrentDirectory(m_strPath.c_str());
    }    
    else if (_tcsicmp(m_strFile.c_str(), pFile) != 0)
    {
        m_strPath = temp;
        m_strFile = pFile;
    }

    return temp;
}

void CFrameEditState::Insert(CParam& kParam)
{
    CWorldEditApp* pApp			= static_cast<CWorldEditApp*>(AfxGetApp());
    CMainFrame* pFrame			= static_cast<CMainFrame*>(pApp->m_pMainWnd);
    CWorldEditView* pView		= static_cast<CWorldEditView*>(pFrame->GetActiveView());
    CWorldEditDoc* pDoc			= static_cast<CWorldEditDoc*>(pFrame->GetActiveDocument());
    DxLandMan* pLandMan	        = pView->GetLandMan();
    DxSetLandMan* pSetLandMan	= pView->GetSetLandMan();
    DxFrameMesh* pFrameMesh		= pSetLandMan->GetFrameMesh();
    //PLANDSKINOBJ pSkinObj       = pSetLandMan->GetLandSkinObjList();    

    SetBackUpCurrentDirectory( false, pDoc->GetTitleOrig().c_str() );

    if (pFrameMesh != 0 && pFrameMesh->GetFrameRoot() != 0)
    {
        // ������ x ���� ���(x)
        InsertFrameMeshX(pSetLandMan, pFrame->m_cDlgEdit.GetSheetChar());

        // ������ x ������ �ؽ�ó ���� ���(dds or mxf)
        InsertMeshTexture(pFrameMesh->GetFrameRoot());

		DxPieceManagerPSF* pPieceMng= pSetLandMan->GetPieceManager();
		if (pPieceMng != 0)
		{
			// ġȯ�� ���� ���� ���(pie)
			InsertReplacePiece(pPieceMng);

			// ��ġ�� ���� ���� ���(pie)
			InsertPiece(pPieceMng);
		}
    }
    else
    {
        //pSkinObj = pLandMan->GetLandSkinObjList();

		DxPieceManagerWLD* pPieceMng = pLandMan->GetPieceManager();
		if (pPieceMng != 0)
		{
			// ��ġ�� ���� ���� ���(pie)
			InsertPiece(pPieceMng);
		}
    }
}

void CFrameEditState::Archive(CParam& kParam)
{
    CWorldEditApp* pApp			= static_cast<CWorldEditApp*>(AfxGetApp());
    CMainFrame* pFrame			= static_cast<CMainFrame*>(pApp->m_pMainWnd);
    CWorldEditView* pView		= static_cast<CWorldEditView*>(pFrame->GetActiveView());

    DxLandMan* pLandMan	        = pView->GetLandMan();
    LPDIRECT3DDEVICEQ pDevice	= pView->GetD3DDevice();
    DxSetLandMan* pSetLandMan	= pView->GetSetLandMan();
    DxFrameMesh* pFrameMesh		= pSetLandMan->GetFrameMesh();

    const TCHAR* pFolder = 0;
    kParam >> pFolder;

    Archive::PreArchive(pDevice, pFrame->m_szAppPath, pFolder);

    if (pFrameMesh == 0 || pFrameMesh->GetFrameRoot() == 0)
    {
        Archive::Archive(pLandMan);
    }
    else
    {
        Archive::Archive(pSetLandMan, m_strFile.c_str());
    }
}

void CFrameEditState::Asset(CParam& kParam)
{
    CWorldEditApp* pApp			= static_cast<CWorldEditApp*>(AfxGetApp());
    CMainFrame* pFrame			= static_cast<CMainFrame*>(pApp->m_pMainWnd);
    CWorldEditView* pView		= static_cast<CWorldEditView*>(pFrame->GetActiveView());

    DxSetLandMan* pSetLandMan	= pView->GetSetLandMan();
    CAssetDlg* pAssetDlg        = &pFrame->m_cDlgAsset;

    // SetLandMan
    DxFrameMesh* pFrameMesh		= pSetLandMan->GetFrameMesh();
    DxPieceManagerPSF* pPieceMng= pSetLandMan->GetPieceManager();
    DxLightMan* pLightMan       = pSetLandMan->GetLightMan();
    DxWeatherMan* pWeatherMan   = pSetLandMan->GetWeatherMan();
    DxCamAniMan* pCameraMan     = pSetLandMan->GetCameraAniMan();
    CStaticSoundMan* pSoundMan  = pSetLandMan->GetStaticSoundMan();
    PLANDSKINOBJ pSkinObj       = pSetLandMan->GetLandSkinObjList();
    DxLandGateMan* pLandGateMan = pSetLandMan->GetLandGateMan();    
    SBGMDATA* pBGMMan           = pSetLandMan->GetBGMDATA();
    FOG_PROPERTY& kFogMan       = pSetLandMan->GetFogProperty();
    SKY_PROPERTY& kSkyBoxMan    = pSetLandMan->GetSkyProperty();
    CCollisionMap* pCollMan     = pSetLandMan->GetCollisionMap();
    DXLANDEFF* pEffectList      = pSetLandMan->GetLandEffList();

    pAssetDlg->Clear();

    if (pFrameMesh == 0 || pFrameMesh->GetFrameRoot() == 0)
    {
        pAssetDlg->Insert(0, _T("Export file does not support."));
        return;
    }

    // ������ �޽�
    InsertFrame(pFrameMesh->GetFrameRoot(), pAssetDlg, 0);

    // ��
    if (pLightMan)
    {
        InsertLight(pLightMan, pAssetDlg, 0);
    }

    // ����
    if (pWeatherMan)
    {
        InserWeather(pWeatherMan, pAssetDlg, 0);
    }

    // ī�޶�
    if (pCameraMan)
    {
        InserCamera(pCameraMan, pAssetDlg, 0);
    }

    // ����
    if (pSoundMan)
    {
        InserSound(pSoundMan, pAssetDlg, 0);
    }

    // ����Ʈ
    InserEffect(pEffectList, pAssetDlg, 0);    

    // ��Ų ������Ʈ
    InserSkinObj(pSkinObj, pAssetDlg, 0);    

    // ���� ����Ʈ
    if (pLandGateMan)
    {
        InserLandGate(pLandGateMan, pAssetDlg, 0);
    }

    // BGM
    if (pBGMMan)
    {
        InserBGM(pBGMMan, pAssetDlg, 0);
    }

    // ����
    InserFog(&kFogMan, pAssetDlg, 0);    

    // ��ī�� �ڽ�
    InserSkyBox(&kSkyBoxMan, pAssetDlg, 0);

    // ����
    if (pPieceMng)
    {
        InsertPiece(pPieceMng, pAssetDlg, 0);
        InsertReplacePiece(pPieceMng, pAssetDlg, 0);
    }

    // ī�޶� �浹 �ؽ�ó
    if (pCollMan)
    {
        InserCollision(pCollMan, pAssetDlg, 0);
    }

    // �ĵ� ����
    InserWater( NSWaterMainPSF::GetWaterSurf(), pAssetDlg, 0 );
}

void CFrameEditState::SelectAsset(CParam& kParam)
{
    CWorldEditApp* pApp			= static_cast<CWorldEditApp*>(AfxGetApp());
    CMainFrame* pFrame			= static_cast<CMainFrame*>(pApp->m_pMainWnd);
    CWorldEditView* pView		= static_cast<CWorldEditView*>(pFrame->GetActiveView());

    DxSetLandMan* pSetLandMan	= pView->GetSetLandMan();
    DxFrameMesh* pFrameMesh		= pSetLandMan->GetFrameMesh();
    CAssetDlg* pAssetDlg        = &pFrame->m_cDlgAsset;

    DxFrame* pDxFrame = 0;
    kParam >> pDxFrame;
    
    if (pDxFrame)
    {
        if (pAssetDlg->IsClick2Tree())
        {
            D3DXVECTOR3 vPos = pDxFrame->GetTreeCenter();

            if (pDxFrame->pmsMeshs == 0)
            {
                // ��ǥ�� ǥ���� �ش�.
                static D3DXMATRIX mat;
                D3DXMatrixIdentity(&mat);
                D3DXMatrixTranslation(&mat, vPos.x, vPos.y, vPos.z);
                pView->ActiveEditMatrix2(&mat);
            }
            else
            {
                // ���̾����������� ǥ���� �ش�.
                pView->ActiveEditMatrix2(0);
            }

            pView->SetCollisionPos(vPos);
            DxViewPort::GetInstance().CameraJump(vPos);
        }

        pFrameMesh->SetCollisionDetectedFrame(pDxFrame);
    }
}