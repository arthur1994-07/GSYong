// SoundPage.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../CharEdit.h"
#include "SoundPage.h"

#include "SheetWithTabAnim.h"

#include "../CharEditDoc.h"
#include "../CharEditView.h"
#include "../EngineLib/DxMeshs/SkinMesh/_new/DxSkinMesh9.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinObject.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinAnimation.h"
#include "../EngineLib/DxMeshs/SkinMesh/SANIMCONINFO.h"

#include "../MainFrm.h"
#include "../EngineLib/DxSound/DxSoundMan.h"
#include "../Dialog/SoundSourceDlg.h"

// CSoundPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CSoundPage, CPropertyPage)
CSoundPage::CSoundPage()
	: CPropertyPage(CSoundPage::IDD)
{
	m_pChaSoundData = NULL;
}

CSoundPage::~CSoundPage()
{
}

void CSoundPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_STARTFRAME, m_ctrlStartFrame);
}


BEGIN_MESSAGE_MAP(CSoundPage, CPropertyPage)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnBnClickedButtonDel)
END_MESSAGE_MAP()


// CSoundPage 메시지 처리기입니다.
void CSoundPage::FrameMarking ( BOOL bReset )
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;

	if ( bReset )
	{
		pFrame->m_cDlgTimeLine.SetFrameMarking ( 0 );
	}
	else
	{
		const DxSkinAnimation* pSelectedAniCont = m_pSheetTab->GetSkinAniContrl()->GETCURANIM();
		if ( !pSelectedAniCont )	
			return;
		DWORD dwUnitTime = pSelectedAniCont->m_UNITTIME;

		WORD PlayFrame[MAXPLAYFRAME];
		for ( WORD i=0; i<m_DummyChaSoundData.m_PlayFrameCount; ++i )
		{
			PlayFrame[i] = (WORD) ( m_DummyChaSoundData.m_PlayFrame [i] / dwUnitTime );
		}

		pFrame->m_cDlgTimeLine.SetFrameMarking ( m_DummyChaSoundData.m_PlayFrameCount, PlayFrame );
	}
}

void CSoundPage::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.	

	//	<--	버블정렬	-->	//
	for ( int i = 0; i < m_DummyChaSoundData.m_PlayFrameCount; ++i )
	{
		for ( int j = 0; j < m_DummyChaSoundData.m_PlayFrameCount; ++j )
		{
			if ( m_DummyChaSoundData.m_PlayFrame[i] < m_DummyChaSoundData.m_PlayFrame[j] )
			{
				WORD Temp;
				BOOL bTemp;
				char str[128];

				//	파일 이름
				strcpy ( str, m_DummyChaSoundData.m_szFileName[i] );
				strcpy ( m_DummyChaSoundData.m_szFileName[i], m_DummyChaSoundData.m_szFileName[j] );
				strcpy ( m_DummyChaSoundData.m_szFileName[j], str );

				//	버퍼 카운트
//				Temp = m_DummyChaSoundData.m_BufferCount[i];
//				m_DummyChaSoundData.m_BufferCount[i] = m_DummyChaSoundData.m_BufferCount[j];
//				m_DummyChaSoundData.m_BufferCount[j] = Temp;

				//	루프
				bTemp = m_DummyChaSoundData.m_bLoop[i];
				m_DummyChaSoundData.m_bLoop[i] = m_DummyChaSoundData.m_bLoop[j];
				m_DummyChaSoundData.m_bLoop[j] = bTemp;

				//	재생 조건
//				Temp = m_DummyChaSoundData.m_PlayType[i];
//				m_DummyChaSoundData.m_PlayType[i] = m_DummyChaSoundData.m_PlayType[j];
//				m_DummyChaSoundData.m_PlayType[j] = Temp;                

				//	프레임
				Temp = m_DummyChaSoundData.m_PlayFrame[i];
				m_DummyChaSoundData.m_PlayFrame[i] = m_DummyChaSoundData.m_PlayFrame[j];
				m_DummyChaSoundData.m_PlayFrame[j] = Temp;                
			}
		}
	}

	for ( int i = 0; i < MAXPLAYFRAME; ++i )
	{
		if ( m_DummyChaSoundData.m_PlayFrameCount <= i )
		{
			m_DummyChaSoundData.m_PlayFrame[i] = 0xFFFF;			
		}
	}

	*m_pChaSoundData = m_DummyChaSoundData;

	DxSkinAniControl* pSkinAniControl = m_pSheetTab->GetSkinAniContrl();
	if ( !pSkinAniControl )	return;

	m_pSheetTab->SetActivePage ( ANIMSETPAGEINDEX );

	m_pSheetTab->ApplyAniContrl ();
	FrameMarking ( TRUE );
}

void CSoundPage::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->SetActivePage ( ANIMSETPAGEINDEX );

	FrameMarking ( TRUE );
}

void CSoundPage::OnBnClickedButtonAdd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if ( m_DummyChaSoundData.m_PlayFrameCount >= MAXPLAYFRAME - 1 )
	{
		CString	str;
		str.Format ( "사운드 시작지점 지정은 %d개까지 유효합니다.", MAXPLAYFRAME - 1 );
		MessageBox ( str.GetString()  );
		return;
	}

	WORD			PlayType;
	SSoundSource	SoundSource;
	BOOL			bLoop;

	CSoundSourceDlg	dlg;
	dlg.SetState ( &m_SoundSourceMan );
	if ( dlg.DoModal() == IDCANCEL )
	{
		return;
	}
	else
	{
		dlg.GetSelectSoundSource ( SoundSource, PlayType, bLoop );
	}

	//	파일 이름
	strcpy ( m_DummyChaSoundData.m_szFileName[m_DummyChaSoundData.m_PlayFrameCount],
		SoundSource.FileName.GetString() );
	//	버퍼
//	m_DummyChaSoundData.m_BufferCount[m_DummyChaSoundData.m_PlayFrameCount] = SoundSource.BufferCount;
	//	루프
	m_DummyChaSoundData.m_bLoop[m_DummyChaSoundData.m_PlayFrameCount] = bLoop;
	//	플레이 조건
//	m_DummyChaSoundData.m_PlayType[m_DummyChaSoundData.m_PlayFrameCount] = PlayType;

	/////////////////////////////////////////////////////////////////////////////////
	//	Note : 타임 라인 조정.
	//
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();	

	const DxSkinAnimation* pSelectedAniCont = m_pSheetTab->GetSkinAniContrl()->GETCURANIM();
	if ( !pSelectedAniCont )	
		return;

	DWORD dwThisTime = 0;
	DWORD dwUnitTime = pSelectedAniCont->m_UNITTIME;
	if ( dwUnitTime > 0 )
	{
		if ( pSelectedAniCont )
		{
			if( CCharEditView::m_emObjType == EOBJ_SKINCHAR )
			{
				dwThisTime = (DWORD) pView->GetSkinChar ()->m_spSkinAniControlThread->GETCURTIME();
			}else if( CCharEditView::m_emObjType == EOBJ_ATTBONE )
			{
				dwThisTime = (DWORD) pView->GetAttBone ()->m_spSkinAniControlThread->GETCURTIME();
			}else if( CCharEditView::m_emObjType == EOBJ_VEHICLE )
			{
				dwThisTime = (DWORD) pView->GetVehicle ()->m_spSkinAniControlThread->GETCURTIME();
			}
			

		}
	}

	//for ( int i = 0; i < m_DummyChaSoundData.m_PlayFrameCount; i++ )
	//{
	//	if ( m_DummyChaSoundData.m_PlayFrame[i] == (WORD)dwThisTime )
	//	{
	//		MessageBox ( "이미 같은 시작지점이 지정되어 있습니다." );
	//		return;
	//	}
	//}

	//	시작되는 프레임 찾아서 가져오기 해야한다.
	m_DummyChaSoundData.m_PlayFrame[m_DummyChaSoundData.m_PlayFrameCount] = (WORD)dwThisTime;	
	/////////////////////////////////////////////////////////////////////////////////

	CString	str;

	str.Format ( "F:%d/L:%d/%s", (WORD)dwThisTime / dwUnitTime,
		bLoop, SoundSource.Comment );
	
	m_ctrlStartFrame.AddString ( str );


	//	실제 의미를 가지는 카운터..
	//	이것이 증가하면, 데이타가 들어간것을 인정하는 것이다.
	m_DummyChaSoundData.m_PlayFrameCount++;


	FrameMarking ();
}

void CSoundPage::OnBnClickedButtonDel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    int nIndex = m_ctrlStartFrame.GetCurSel ();
	if ( -1 == nIndex )
	{
		return ;
	}

	m_ctrlStartFrame.DeleteString ( nIndex );	

	m_DummyChaSoundData.m_PlayFrameCount--;
	int TempIndex = 0;
	for ( int i = 0; i < m_DummyChaSoundData.m_PlayFrameCount - nIndex; ++i )
	{
		TempIndex = i+nIndex;
		
		strcpy ( m_DummyChaSoundData.m_szFileName[TempIndex], m_DummyChaSoundData.m_szFileName[TempIndex+1] );		
//		m_DummyChaSoundData.m_BufferCount[TempIndex] = m_DummyChaSoundData.m_BufferCount[TempIndex+1];
		m_DummyChaSoundData.m_bLoop[TempIndex] = m_DummyChaSoundData.m_bLoop[TempIndex+1];
//		m_DummyChaSoundData.m_PlayType[TempIndex] = m_DummyChaSoundData.m_PlayType[TempIndex+1];
		m_DummyChaSoundData.m_PlayFrame[TempIndex] = m_DummyChaSoundData.m_PlayFrame[TempIndex+1];        
	}

	FrameMarking ();
}

void CSoundPage::SetSound ( SChaSoundData* pChaSoundData )
{
	if ( !pChaSoundData )
	{
		MessageBox ( "웃기는 짬뽕이군.. 이런 오류는 나면 안되는 거다." );
		return ;
	}

	 m_pChaSoundData = pChaSoundData;

	 m_DummyChaSoundData = *m_pChaSoundData;

	//	<--	이전 내용물 삭제	-->	//
	while ( m_ctrlStartFrame.GetCount () )
	{
		m_ctrlStartFrame.DeleteString ( 0 );
	}

	if ( !m_DummyChaSoundData.m_PlayFrameCount )
	{
		return;
	}

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView ();	

	const DxSkinAnimation* pSelectedAniCont = m_pSheetTab->GetSkinAniContrl()->GETCURANIM();
	if ( !pSelectedAniCont )	
		return;
	
	DWORD dwUnitTime = pSelectedAniCont->m_UNITTIME;
	WORD BufferCount = 0;

	for ( int i = 0; i < m_DummyChaSoundData.m_PlayFrameCount; ++i )
	{
		CString str;
		CString	Comment;
		BufferCount = 0;

		//	파일 이름으로 정보 얻어오기
		SSoundSource* pSoundSource = NULL;
		m_SoundSourceMan.GoHead();
		while ( !m_SoundSourceMan.IsEnd() )
		{
			pSoundSource = m_SoundSourceMan.GetRecord ( );

			if ( !strcmp ( pSoundSource->FileName.GetString(), m_DummyChaSoundData.m_szFileName[i] ) )
			{
				Comment = pSoundSource->Comment;
//				BufferCount = pSoundSource->BufferCount;
			}
			m_SoundSourceMan.GoNext();
		}

//		BufferCount = m_DummyChaSoundData.m_BufferCount[i];

		str.Format ( "F:%d/L:%d/%s", m_DummyChaSoundData.m_PlayFrame[i] / dwUnitTime,
			m_DummyChaSoundData.m_bLoop[i], Comment );
		m_ctrlStartFrame.AddString ( str );
	}

	FrameMarking ();
}

BOOL CSoundPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	std::string WavTableFile( DxSoundMan::GetInstance().GetPath() );
    WavTableFile += "WavTable.sg";
	m_SoundSourceMan.LoadFile( WavTableFile.c_str() );	

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
