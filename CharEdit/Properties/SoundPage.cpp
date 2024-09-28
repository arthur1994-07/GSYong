// SoundPage.cpp : ���� �����Դϴ�.
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

// CSoundPage ��ȭ �����Դϴ�.

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


// CSoundPage �޽��� ó�����Դϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.	

	//	<--	��������	-->	//
	for ( int i = 0; i < m_DummyChaSoundData.m_PlayFrameCount; ++i )
	{
		for ( int j = 0; j < m_DummyChaSoundData.m_PlayFrameCount; ++j )
		{
			if ( m_DummyChaSoundData.m_PlayFrame[i] < m_DummyChaSoundData.m_PlayFrame[j] )
			{
				WORD Temp;
				BOOL bTemp;
				char str[128];

				//	���� �̸�
				strcpy ( str, m_DummyChaSoundData.m_szFileName[i] );
				strcpy ( m_DummyChaSoundData.m_szFileName[i], m_DummyChaSoundData.m_szFileName[j] );
				strcpy ( m_DummyChaSoundData.m_szFileName[j], str );

				//	���� ī��Ʈ
//				Temp = m_DummyChaSoundData.m_BufferCount[i];
//				m_DummyChaSoundData.m_BufferCount[i] = m_DummyChaSoundData.m_BufferCount[j];
//				m_DummyChaSoundData.m_BufferCount[j] = Temp;

				//	����
				bTemp = m_DummyChaSoundData.m_bLoop[i];
				m_DummyChaSoundData.m_bLoop[i] = m_DummyChaSoundData.m_bLoop[j];
				m_DummyChaSoundData.m_bLoop[j] = bTemp;

				//	��� ����
//				Temp = m_DummyChaSoundData.m_PlayType[i];
//				m_DummyChaSoundData.m_PlayType[i] = m_DummyChaSoundData.m_PlayType[j];
//				m_DummyChaSoundData.m_PlayType[j] = Temp;                

				//	������
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_pSheetTab->SetActivePage ( ANIMSETPAGEINDEX );

	FrameMarking ( TRUE );
}

void CSoundPage::OnBnClickedButtonAdd()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if ( m_DummyChaSoundData.m_PlayFrameCount >= MAXPLAYFRAME - 1 )
	{
		CString	str;
		str.Format ( "���� �������� ������ %d������ ��ȿ�մϴ�.", MAXPLAYFRAME - 1 );
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

	//	���� �̸�
	strcpy ( m_DummyChaSoundData.m_szFileName[m_DummyChaSoundData.m_PlayFrameCount],
		SoundSource.FileName.GetString() );
	//	����
//	m_DummyChaSoundData.m_BufferCount[m_DummyChaSoundData.m_PlayFrameCount] = SoundSource.BufferCount;
	//	����
	m_DummyChaSoundData.m_bLoop[m_DummyChaSoundData.m_PlayFrameCount] = bLoop;
	//	�÷��� ����
//	m_DummyChaSoundData.m_PlayType[m_DummyChaSoundData.m_PlayFrameCount] = PlayType;

	/////////////////////////////////////////////////////////////////////////////////
	//	Note : Ÿ�� ���� ����.
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
	//		MessageBox ( "�̹� ���� ���������� �����Ǿ� �ֽ��ϴ�." );
	//		return;
	//	}
	//}

	//	���۵Ǵ� ������ ã�Ƽ� �������� �ؾ��Ѵ�.
	m_DummyChaSoundData.m_PlayFrame[m_DummyChaSoundData.m_PlayFrameCount] = (WORD)dwThisTime;	
	/////////////////////////////////////////////////////////////////////////////////

	CString	str;

	str.Format ( "F:%d/L:%d/%s", (WORD)dwThisTime / dwUnitTime,
		bLoop, SoundSource.Comment );
	
	m_ctrlStartFrame.AddString ( str );


	//	���� �ǹ̸� ������ ī����..
	//	�̰��� �����ϸ�, ����Ÿ�� ������ �����ϴ� ���̴�.
	m_DummyChaSoundData.m_PlayFrameCount++;


	FrameMarking ();
}

void CSoundPage::OnBnClickedButtonDel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
		MessageBox ( "����� «���̱�.. �̷� ������ ���� �ȵǴ� �Ŵ�." );
		return ;
	}

	 m_pChaSoundData = pChaSoundData;

	 m_DummyChaSoundData = *m_pChaSoundData;

	//	<--	���� ���빰 ����	-->	//
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

		//	���� �̸����� ���� ������
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

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	std::string WavTableFile( DxSoundMan::GetInstance().GetPath() );
    WavTableFile += "WavTable.sg";
	m_SoundSourceMan.LoadFile( WavTableFile.c_str() );	

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}
