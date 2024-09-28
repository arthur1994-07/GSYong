// MFC_UI_TEST.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
//

#include "stdafx.h"
#include "MFC_UI_TEST.h"

#include "mrcInit.h"
#include "./mrcSigma/mrcSig.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//#ifdef _DEBUG
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
//#endif


// CMFC_UI_TESTApp

BEGIN_MESSAGE_MAP(CMFC_UI_TESTApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CMFC_UI_TESTApp ����

CMFC_UI_TESTApp::CMFC_UI_TESTApp()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	// InitInstance�� ��� �߿��� �ʱ�ȭ �۾��� ��ġ�մϴ�.
}


// ������ CMFC_UI_TESTApp ��ü�Դϴ�.

CMFC_UI_TESTApp theApp;


// CMFC_UI_TESTApp �ʱ�ȭ

BOOL CMFC_UI_TESTApp::InitInstance()
{
	// ���� ���α׷� �Ŵ��佺Ʈ�� ComCtl32.dll ���� 6 �̻��� ����Ͽ� ���־� ��Ÿ����
	// ����ϵ��� �����ϴ� ���, Windows XP �󿡼� �ݵ�� InitCommonControlsEx()�� �ʿ��մϴ�.
	// InitCommonControlsEx()�� ������� ������ â�� ���� �� �����ϴ�.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ���� ���α׷����� ����� ��� ���� ��Ʈ�� Ŭ������ �����ϵ���
	// �� �׸��� �����Ͻʽÿ�.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// ǥ�� �ʱ�ȭ
	// �̵� ����� ������� �ʰ� ���� ���� ������ ũ�⸦ ���̷���
	// �Ʒ����� �ʿ� ���� Ư�� �ʱ�ȭ
	// ��ƾ�� �����ؾ� �մϴ�.
	// �ش� ������ ����� ������Ʈ�� Ű�� �����Ͻʽÿ�.
	// TODO: �� ���ڿ��� ȸ�� �Ǵ� ������ �̸��� ����
	// ������ �������� �����ؾ� �մϴ�.
	SetRegistryKey(_T("���� ���� ���α׷� �����翡�� ������ ���� ���α׷�"));

	HWND hWnd = mrc::initWindows(0,0,800,600,&mrcTest::MyMsgProc);

	mrcSig::Init(mrc::getDevice(),hWnd);

	boost::timer timer;
	timer.restart();

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT )
	{
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			try
			{
				static double time  = timer.elapsed();
				double boostTime = timer.elapsed();
				double elapsedTime =  boostTime - time;
				time = boostTime;

				if( elapsedTime > 1 )
					elapsedTime = 1;

				if( mrcTest::isPlay() )
				{
					mrc::updateLayer((mrc::real)elapsedTime);
					mrcSig::Update((mrc::real)elapsedTime);
				}


				mrc::clear();
				mrc::renderLayer();

//				mrc::present();


			}
			catch (const mrc::Exception& E)
			{
				mrc::ShowMessage( ("error : ") + E.getMessage() +"\n" );
				mrc::resetAll();
			}

		}
	}
	mrcSig::Release();
	mrc::release();

	// ��ȭ ���ڰ� �������Ƿ� ���� ���α׷��� �޽��� ������ �������� �ʰ�  ���� ���α׷��� ���� �� �ֵ��� FALSE��
	// ��ȯ�մϴ�.
	return FALSE;
}
