// CamcollPage.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "WorldEdit.h"
#include "CamcollPage.h"

#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"

#include "../EngineLib/DxMeshs/FrameMesh/DxFrameMesh.h"
#include "../EngineLib/DxMeshs/FrameMesh/DxReplaceContainer.h"
#include "../MfcExLib/ExLibs/EtcFunction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CCamcollPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CCamcollPage, CPropertyPage)
CCamcollPage::CCamcollPage()
	: CPropertyPage(CCamcollPage::IDD),
	m_bDlgInit(FALSE),
	m_nBufferCel(NULL),
	m_nMaxCel(0)
{
}

CCamcollPage::~CCamcollPage()
{
	SAFE_DELETE_ARRAY(m_nBufferCel);
}

void CCamcollPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CAMCOLL_DISABLE, m_ListDisable);
	DDX_Control(pDX, IDC_LIST_CAMCOLL_ENABLE, m_ListEnable);
}

BEGIN_MESSAGE_MAP(CCamcollPage, CPropertyPage)
	ON_LBN_SELCHANGE(IDC_LIST_CAMCOLL_DISABLE, OnLbnSelchangeListCamcollDisable)
	ON_LBN_SETFOCUS(IDC_LIST_CAMCOLL_DISABLE, OnLbnSetFocusListCamcollDisable) 
	ON_LBN_SELCHANGE(IDC_LIST_CAMCOLL_ENABLE, OnLbnSelchangeListCamcollEnable)
	ON_LBN_SETFOCUS(IDC_LIST_CAMCOLL_ENABLE, OnLbnSetFocusListCamcollEnable) 
	ON_BN_CLICKED(IDC_BUTTON_CAMCOLL_UP, OnBnClickedButtonCamcollUp)
	ON_BN_CLICKED(IDC_BUTTON_CAMCOLL_DOWN, OnBnClickedButtonCamcollDown)
END_MESSAGE_MAP()


void CCamcollPage::UpdateContrl ()
{
	if ( !m_bDlgInit )	return;

	OnInitDialog();		// 다시 초기화를 시킨다.
}

void CCamcollPage::SetList( DxFrame *pFrameRoot )
{
	m_ListDisable.ResetContent ();
	m_ListEnable.ResetContent ();

	m_MapCamColl.clear();
	m_nMaxCel = 0;

	if ( pFrameRoot )
	{
		AddList( pFrameRoot );

		if ( m_nMaxCel )
		{
			SAFE_DELETE_ARRAY(m_nBufferCel);

			m_nBufferCel = new INT32[m_nMaxCel];
		}
	}

}

void CCamcollPage::AddList( DxFrame *pFrameCur )
{
	DxMeshes* pMesh = pFrameCur->pmsMeshs;

	while( pMesh )
	{
		if ( !pMesh->m_pLocalMesh )
        {
			pMesh = pMesh->m_pMeshNext;		
			continue;
		}

		if ( 1<pMesh->m_cMaterials )
		{
            // V570 The 'pMesh->m_cMaterials' variable is assigned to itself.
			pMesh->m_cMaterials = pMesh->m_cMaterials;
		}

		for ( DWORD i=0; i<pMesh->m_cMaterials; i++ )
		{
			const char *szTexture = pMesh->m_strTextureFiles[i].GetString();
		
			if( szTexture && strlen(szTexture) > 0 )
			{
                std::pair<MULTIMAPCAMCOLLITER, MULTIMAPCAMCOLLITER> iter_pair;

				iter_pair = m_MapCamColl.equal_range( szTexture );

				//	Note : 리스트에 없을경우 추가
				//
				if ( iter_pair.first == iter_pair.second )
				{
					if ( pMesh->m_exMaterials[i].bCamColl )
					{
						int nIndex = m_ListEnable.AddString ( szTexture );
					}
					else
					{
						int nIndex = m_ListDisable.AddString ( szTexture );
					}

					++m_nMaxCel;
				}

				CAMCOLLINFO cCamCollInfo;

				cCamCollInfo.pMesh    = pMesh;
				cCamCollInfo.pCamColl = &pMesh->m_exMaterials[i].bCamColl;
				cCamCollInfo.dwSubset = i;

                m_MapCamColl.insert( std::pair<std::string, CAMCOLLINFO>(szTexture, cCamCollInfo) );
			}
		}

		pMesh = pMesh->m_pMeshNext;
	}

	//	Note : 자식들을 재귀적으로 모두 탐색.
	//

	DxFrame *pFrameChild = pFrameCur->pframeFirstChild;
	while (pFrameChild != NULL)
	{
		AddList ( pFrameChild );
		pFrameChild = pFrameChild->pframeSibling;
	}
}

void CCamcollPage::SetCamCollItem(const std::string& strItemName, const BOOL bEnable)
{
    std::pair<MULTIMAPCAMCOLLITER, MULTIMAPCAMCOLLITER> iter_pair;

	iter_pair = m_MapCamColl.equal_range( strItemName );

	MULTIMAPCAMCOLLITER iter;

	for( iter = iter_pair.first; iter != iter_pair.second; iter++ )
	{
		if ( iter->second.pCamColl )
		{
			*(iter->second.pCamColl) = bEnable;
		}
	}
}

void CCamcollPage::SetCamCollRender( const std::string& strItemName, const BOOL bEnable )
{
	std::pair<MULTIMAPCAMCOLLITER, MULTIMAPCAMCOLLITER> iter_pair;

	iter_pair = m_MapCamColl.equal_range( strItemName );

	MULTIMAPCAMCOLLITER iter;

	for( iter = iter_pair.first; iter != iter_pair.second; iter++ )
	{
		if ( iter->second.pMesh )
		{
			(iter->second.pMesh)->m_exMaterials[iter->second.dwSubset].bSubSetTriRend = bEnable;
		}
	}
}

void CCamcollPage::SelectMesh ( const std::string& strItemName )
{
	MULTIMAPCAMCOLLITER iter;

	for( iter = m_MapCamColl.begin(); iter != m_MapCamColl.end(); iter++ )
	{
		if ( iter->second.pMesh )
		{
			(iter->second.pMesh)->m_exMaterials[iter->second.dwSubset].bSubSetTriRend = FALSE;
		}
	}

	m_ListDisable.SetSel(-1, FALSE);
	m_ListEnable .SetSel(-1, FALSE);

	INT32 nSelect = m_ListDisable.FindString(0, strItemName.c_str());

	if ( nSelect != LB_ERR )
	{
		m_ListDisable.SetSel(nSelect);

		SetCamCollRender( strItemName, TRUE );

		return;
	}

	 nSelect = m_ListEnable.FindString(0, strItemName.c_str());
	
	 if ( nSelect != LB_ERR )
	 {
		 m_ListEnable.SetSel(nSelect);

		 SetCamCollRender( strItemName, TRUE );

		 return;
	 }
}

// CCamcollPage 메시지 처리기입니다.

BOOL CCamcollPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	CMainFrame*     pFrame      = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView       = (CWorldEditView*) pFrame->GetActiveView ();
	DxFrameMesh*    pFrameMesh  = pView->GetFrameMesh();

	if ( pFrameMesh )
	{
		SetList ( pFrameMesh->GetFrameRoot() );
	}

	m_bDlgInit = TRUE;

	return TRUE;
}

BOOL CCamcollPage::OnSetActive()
{
	CMainFrame* pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	if( pFrame )
	{
		CWorldEditView* pView = (CWorldEditView*) pFrame->GetActiveView();
		if( pView )		pView->m_ViewType = CAMCOLL_VIEW;
	}

	return CPropertyPage::OnSetActive();
}

BOOL CCamcollPage::OnKillActive()
{
	MULTIMAPCAMCOLLITER iter;

	for( iter = m_MapCamColl.begin(); iter != m_MapCamColl.end(); iter++ )
	{
		if ( iter->second.pMesh )
		{
			(iter->second.pMesh)->m_exMaterials[iter->second.dwSubset].bSubSetTriRend = FALSE;
		}
	}

	return CPropertyPage::OnKillActive();
}

void CCamcollPage::OnLbnSelchangeListCamcollDisable()
{
	MULTIMAPCAMCOLLITER iter;

	for( iter = m_MapCamColl.begin(); iter != m_MapCamColl.end(); iter++ )
	{
		if ( iter->second.pMesh )
		{
			(iter->second.pMesh)->m_exMaterials[iter->second.dwSubset].bSubSetTriRend = FALSE;
		}
	}

	INT32 nMaxSelect = m_ListDisable.GetSelItems(m_nMaxCel, m_nBufferCel);

	if ( nMaxSelect != LB_ERR )
	{
		char strItem[256];

		for ( INT32 i=nMaxSelect-1; i>=0; --i )
		{
			m_ListDisable.GetText(m_nBufferCel[i], strItem);

			SetCamCollRender( strItem, TRUE );
		}
	}
}

void CCamcollPage::OnLbnSelchangeListCamcollEnable()
{
	MULTIMAPCAMCOLLITER iter;

	for( iter = m_MapCamColl.begin(); iter != m_MapCamColl.end(); iter++ )
	{
		if ( iter->second.pMesh )
		{
			(iter->second.pMesh)->m_exMaterials[iter->second.dwSubset].bSubSetTriRend = FALSE;
		}
	}

	INT32 nMaxSelect = m_ListEnable.GetSelItems(m_nMaxCel, m_nBufferCel);

	if ( nMaxSelect != LB_ERR )
	{
		char strItem[256];

		for ( INT32 i=nMaxSelect-1; i>=0; --i )
		{
			m_ListEnable.GetText(m_nBufferCel[i], strItem);

			SetCamCollRender( strItem, TRUE );
		}
	}
}

void CCamcollPage::OnLbnSetFocusListCamcollDisable()
{
	m_ListEnable.SetSel(-1, FALSE);
}

void CCamcollPage::OnLbnSetFocusListCamcollEnable()
{
	m_ListDisable.SetSel(-1, FALSE);
}

void CCamcollPage::OnBnClickedButtonCamcollUp()
{
	INT32 nMaxSelect = m_ListEnable.GetSelItems(m_nMaxCel, m_nBufferCel);

	if ( nMaxSelect != LB_ERR )
	{
		char strItem[256];

		for ( INT32 i=nMaxSelect-1; i>=0; --i )
		{
			m_ListEnable.GetText(m_nBufferCel[i], strItem);
			m_ListEnable.DeleteString(m_nBufferCel[i]);

			SetCamCollItem( strItem, FALSE );

			m_ListDisable.AddString(strItem);

			INT32 nSelect = m_ListDisable.FindString(0, strItem);

			if ( nSelect != LB_ERR )
			{
				m_ListDisable.SetSel(nSelect);
			}
		}
	}
}

void CCamcollPage::OnBnClickedButtonCamcollDown()
{	
	INT32 nMaxSelect = m_ListDisable.GetSelItems(m_nMaxCel, m_nBufferCel);

	INT32 nMax0 = m_ListDisable.GetCount();
	INT32 nMax1 = m_ListDisable.GetSelCount();

	if ( nMaxSelect != LB_ERR )
	{
		char strItem[256];

		for ( INT32 i=nMaxSelect-1; i>=0; --i )
		{
			m_ListDisable.GetText(m_nBufferCel[i], strItem);
			m_ListDisable.DeleteString(m_nBufferCel[i]);

			SetCamCollItem( strItem, TRUE );

			m_ListEnable.AddString(strItem);

			INT32 nSelect = m_ListEnable.FindString(0, strItem);

			if ( nSelect != LB_ERR )
			{
				m_ListEnable.SetSel(nSelect);
			}
			else
			{
				nSelect = nSelect;
			}
		}
	}
}
