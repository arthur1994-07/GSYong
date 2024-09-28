
#include "stdafx.h"
#include "../../EngineLib/DxTools/DxViewPort.h"
#include "../../EngineLib/DxTools/NSCharAniSequence.h"
#include "../../EngineLib/Common/SUBPATH.h"
#include "../../MfcExLib/ExLibs/EtcFunction.h"
#include "../CharEdit.h"
#include "SheetWithTabChar.h"
#include "SheetWithTabAnim.h"

#include "../Dialog/ResolutionDlg.h"
#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"
#include "FlyCameraEditPage.h"

IMPLEMENT_DYNAMIC(CFlyCameraEditPage, CPropertyPage)
CFlyCameraEditPage::CFlyCameraEditPage(void)
: CPropertyPage(CFlyCameraEditPage::IDD)
, m_pSheetTabAnim(NULL)
{
}

CFlyCameraEditPage::~CFlyCameraEditPage(void)
{
}

void CFlyCameraEditPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CAMERAANCHOR, m_listCameraAnchor);
}

void CFlyCameraEditPage::PostNcDestroy()
{
	SAFE_DELETE(m_pSheetTabAnim);
	CPropertyPage::PostNcDestroy();
}

BEGIN_MESSAGE_MAP(CFlyCameraEditPage, CPropertyPage)
		ON_BN_CLICKED(IDC_BUTTON_FLYCAM_LOAD, &CFlyCameraEditPage::OnBnClickedBtnLoad)
		ON_BN_CLICKED(IDC_BUTTON_FLYCAM_SAVE, &CFlyCameraEditPage::OnBnClickedBtnSave)
		ON_BN_CLICKED(IDC_BUTTON_ANCHORINSERT, &CFlyCameraEditPage::OnBnClickedBtnAnchorInsert)
		ON_BN_CLICKED(IDC_BUTTON_ANCHORDELETE, &CFlyCameraEditPage::OnBnClickedBtnAnchorDelete)
		ON_BN_CLICKED(IDC_BUTTON_PLAY, &CFlyCameraEditPage::OnBnClickedBtnPlay)
		ON_BN_CLICKED(IDC_STATIC_FLYCAM_EDITGROUP_APPLY, &CFlyCameraEditPage::OnBnClickedBtnApply)
		ON_NOTIFY(NM_CLICK, IDC_LIST_CAMERAANCHOR, &CFlyCameraEditPage::OnNMClickListCameraAnchor)
		ON_NOTIFY(NM_DBLCLK, IDC_LIST_CAMERAANCHOR, &CFlyCameraEditPage::OnDMClickListCameraAnchor)
END_MESSAGE_MAP()

BOOL CFlyCameraEditPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// 애니메이션 시트 초기화;
	{
		CWnd *pWnd;
		CRect SheetRect;

		pWnd = GetDlgItem ( IDC_STATIC_SHEETTAP2 );
		pWnd->GetWindowRect ( SheetRect );
		ScreenToClient ( &SheetRect );

		m_pSheetTabAnim = new CsheetWithTabAnim ( SheetRect, this );
		m_pSheetTabAnim->Create ( this, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0 );
	}


	// 앵커 리스트 헤더 컬럼 설정;
	RECT listRect;
	{
		m_listCameraAnchor.GetClientRect(&listRect);
		int nListWidth = ( listRect.right - listRect.left );
		int nVScrollBarWidth = ::GetSystemMetrics(SM_CXVSCROLL); 
		int nColWidth[2] = { int(nListWidth*0.5f), int(nListWidth*0.5f) };

		m_listCameraAnchor.SetExtendedStyle ( m_listCameraAnchor.GetExtendedStyle() |
			LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

		m_listCameraAnchor.InsertColumn ( 0, _T("Index"),	LVCFMT_CENTER, nColWidth[0] );
		m_listCameraAnchor.InsertColumn ( 1, _T("Frame"),	LVCFMT_CENTER, nColWidth[1] );
	}

	return TRUE;
}

BOOL CFlyCameraEditPage::OnSetActive()
{
	// 애니메이션 리스트 초기화;
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();
	m_pSheetTabAnim->ReSetPages();
	m_pSheetTabAnim->SetPages(pView->GetSkinCharData()->GetSkinAniControlData().get()
		, pView->GetSkinChar()->m_spSkinAniControlThread.get());

	return CPropertyPage::OnSetActive();
}

void CFlyCameraEditPage::OnBnClickedBtnLoad()
{
	//	파일 오픈 다이알로그를 만듬;
	CFileDialog dlg(TRUE, ".lua", "Open"
		, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT
		, "Flycamera keyframe list (*.lua)|*.lua|"
		, (CFlyCameraEditPage*)this);

	//  Root Dir 설정;
	CCharEditApp* pApp = (CCharEditApp*) AfxGetApp();
	const SUBPATH* pPath = pApp->GetSubPath();
	std::string strAppPath = pPath->AppRoot();

	CString strFullPath = strAppPath.c_str();
	strFullPath += pPath->GLogicPath().c_str();

	dlg.m_ofn.lpstrInitialDir = strFullPath;
	if ( dlg.DoModal() == IDOK )
	{
		m_FlyCameControl.Load(dlg.GetPathName());
	}
	UpdateList();
}

void CFlyCameraEditPage::OnBnClickedBtnSave()
{
	//	파일 오픈 다이알로그를 만듬;
	CFileDialog dlg(TRUE,".lua", "Save"
		, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT
		, "Flycamera keyframe list (*.lua)|*.lua|"
		, (CFlyCameraEditPage*)this);

	//  Root Dir 설정;
	CCharEditApp* pApp = (CCharEditApp*) AfxGetApp();
	const SUBPATH* pPath = pApp->GetSubPath();
	std::string strAppPath = pPath->AppRoot();

	CString strFullPath = strAppPath.c_str();
	strFullPath += pPath->GLogicPath().c_str();

	dlg.m_ofn.lpstrInitialDir = strFullPath;
	if ( dlg.DoModal() == IDOK )
	{
		m_FlyCameControl.Save(dlg.GetPathName());
	}
}


void CFlyCameraEditPage::OnBnClickedBtnAnchorInsert()
{
	const GLFlyCameraKeyFrameControl::KEYFRAME_VEC& vecKeylist = m_FlyCameControl.GetKeyFrameList();
	
	// 다음 프레임은 마지막 키 프레임의 + FPS만큼으로 설정한다;
	const INT nAnchorSize(vecKeylist.size())
		, nNextFrame(nAnchorSize == 0 ? 0 : vecKeylist[nAnchorSize-1].nKey+m_FlyCameControl.GetFPS());
	
	// 현재 카메라 정보로 세팅;
	FlyCameraControl::SMsg msg(FlyCameraControl::EMFLYCAMERA_ADD_KEYFRAMESLOT, nNextFrame);
	m_FlyCameControl.AddMessage(&msg);

	D3DXVECTOR3 vPos(DxViewPort::GetInstance().GetFromPt()), vLook(DxViewPort::GetInstance().GetLookatPt());
	msg.type	= FlyCameraControl::EMFLYCAMERA_SET_KEYPOSITION;
	msg.dwMsg	= 0;
	msg.vec		= D3DXVECTOR4(vPos.x, vPos.y, vPos.z, nNextFrame);
	m_FlyCameControl.AddMessage(&msg);

	msg.type	= FlyCameraControl::EMFLYCAMERA_SET_KEYLOOKAT;
	msg.dwMsg	= 0;
	msg.vec		= D3DXVECTOR4(vLook.x, vLook.y, vLook.z, nNextFrame);
	m_FlyCameControl.AddMessage(&msg);
	
	// 메세지 방식임으로 프로시져가 돌지않으면 값갱신이 안된다;
	// 콜백을 만들어주면 가능하다;
	m_FlyCameControl.MsgProcedures();
	UpdateList();
}

void CFlyCameraEditPage::OnBnClickedBtnAnchorDelete()
{
	POSITION pos = m_listCameraAnchor.GetFirstSelectedItemPosition();
	int nIndex = m_listCameraAnchor.GetNextSelectedItem( pos );
	if ( nIndex==LB_ERR )	return;

	const int nSelKeyFrame(m_FlyCameControl.GetKeyFrame(nIndex));
	FlyCameraControl::SMsg msg(FlyCameraControl::EMFLYCAMERA_DEL_KEYFRAMESLOT,nSelKeyFrame);
	m_FlyCameControl.AddMessage(&msg);

	// 메세지 방식임으로 프로시져가 돌지않으면 값갱신이 안된다;
	// 콜백을 만들어주면 가능하다;
	m_FlyCameControl.MsgProcedures();
	UpdateList();
}

void CFlyCameraEditPage::OnBnClickedBtnPlay()
{
	FlyCameraControl::SMsg msg(FlyCameraControl::EMFLYCAMERA_PLAY_ANIMATION, -1);
	m_FlyCameControl.AddMessage(&msg);
	const GLFlyCameraKeyFrameControl::KEYFRAME_VEC& vecKeylist = m_FlyCameControl.GetKeyFrameList();
	if ( vecKeylist.empty() == false )
	{
		// FlyCamera는 기본적으로 움직일시 상대적으로 움직인다;
		// 이 구간이 빠지면 시작위치부터 상대적으로 움직인다;
		DxViewPort::GetInstance().SetViewTrans(
			vecKeylist[0].sMoveToTargetID.vPos
			, vecKeylist[0].sLookToTargetID.vPos
			, D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	}
}

void CFlyCameraEditPage::OnBnClickedBtnApply()
{
	// 리스트 아이템 클릭시 해당 정보를 IDC_ANCHORINFO에 출력해준다;
	POSITION pos = m_listCameraAnchor.GetFirstSelectedItemPosition();
	int nIndex = m_listCameraAnchor.GetNextSelectedItem( pos );
	if ( nIndex==LB_ERR )	return;

	const int nSelKeyFrame(m_FlyCameControl.GetKeyFrame(nIndex));

	const INT nNewKey(GetWin_Num_int( this, IDC_EDIT_FRAME ));

	FlyCameraControl::SMsg msg(FlyCameraControl::EMFLYCAMERA_EDIT_KEYFRAME, nSelKeyFrame, nNewKey);
	m_FlyCameControl.AddMessage(&msg);

	// 메세지 방식임으로 프로시져가 돌지않으면 값갱신이 안된다;
	// 콜백을 만들어주면 가능하다;
	m_FlyCameControl.MsgProcedures();

	UpdateList();
}

void CFlyCameraEditPage::UpdateList(void)
{
	m_listCameraAnchor.DeleteAllItems();
	
	const GLFlyCameraKeyFrameControl::KEYFRAME_VEC& vecKeylist = m_FlyCameControl.GetKeyFrameList();

	CString strTemp;
	const INT nAnchorSize(vecKeylist.size());
	for ( INT i = 0; i < nAnchorSize; ++i )
	{
		const FlyCameraControl::FlyCameraKey& valueKey = vecKeylist[i];
		strTemp.Format( _T("%d"), i );
		m_listCameraAnchor.InsertItem(i, strTemp.GetString());

		strTemp.Format( _T("%3.1f"), (float)valueKey.nKey );
		m_listCameraAnchor.SetItemText(i, 1, strTemp.GetString());
	}
}

void CFlyCameraEditPage::OnNMClickListCameraAnchor(NMHDR *pNMHDR, LRESULT *pResult)
{
	// 리스트 아이템 클릭시 해당 정보를 IDC_ANCHORINFO에 출력해준다;
	POSITION pos = m_listCameraAnchor.GetFirstSelectedItemPosition();
	int nIndex = m_listCameraAnchor.GetNextSelectedItem( pos );
	if ( nIndex==LB_ERR )	return;

	const int nSelKeyFrame(m_FlyCameControl.GetKeyFrame(nIndex));

	const FlyCameraControl::FlyCameraKey& sKeyData = m_FlyCameControl.GetKeyData(nSelKeyFrame);
	const D3DXVECTOR3& vPos = sKeyData.sMoveToTargetID.vPos
		,& vLookat = sKeyData.sLookToTargetID.vPos;

	CString strFormat;
	strFormat.Format("Index : %d", nIndex);

	CWnd *pWnd;
	pWnd = GetDlgItem ( IDC_STATIC_FLYCAM_EDITGROUP_INDEX );
	pWnd->SetWindowText ( strFormat.GetString() );
	
	strFormat.Format("Position : %3.2f, %3.2f, %3.2f", vPos.x, vPos.y, vPos.z);
	pWnd = GetDlgItem ( IDC_STATIC_FLYCAM_EDITGROUP_POS );
	pWnd->SetWindowText ( strFormat.GetString() );

	strFormat.Format("Look at : %3.2f, %3.2f, %3.2f", vLookat.x, vLookat.y, vLookat.z);
	pWnd = GetDlgItem ( IDC_STATIC_FLYCAM_EDITGROUP_LOOKAT );
	pWnd->SetWindowText ( strFormat.GetString() );

	strFormat.Format("%d", nSelKeyFrame);
	pWnd = GetDlgItem ( IDC_EDIT_FRAME );
	pWnd->SetWindowText ( strFormat.GetString() );
}

void CFlyCameraEditPage::OnDMClickListCameraAnchor(NMHDR *pNMHDR, LRESULT *pResult)
{
	// 더블 클릭시 해당 아이템의 앵커 위치로 카메라를 이동시킨다;
	POSITION pos = m_listCameraAnchor.GetFirstSelectedItemPosition();
	int nIndex = m_listCameraAnchor.GetNextSelectedItem( pos );
	if ( nIndex==LB_ERR )	return;

	const int nSelKeyFrame(m_FlyCameControl.GetKeyFrame(nIndex));
	const FlyCameraControl::FlyCameraKey& sKeyData = m_FlyCameControl.GetKeyData(nSelKeyFrame);
	DxViewPort::GetInstance().SetViewTrans(
		sKeyData.sMoveToTargetID.vPos
		, sKeyData.sLookToTargetID.vPos
		, D3DXVECTOR3(0.0f, 1.0f, 0.0f));
}