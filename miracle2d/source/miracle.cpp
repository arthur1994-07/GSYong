

#include "mrcPrecompiled.h"

#include "miracle.h"

#if (defined(_WIN32) || defined(_WIN64))
#include <MMSystem.h>
#endif


#include "mrcScript.h"
#include "mrcTextureMgr.h"
#include "mrcLayerMgr.h"
#include "mrcKeyboard.h"
#include "mrcRenderer.h"

namespace mrc
{

	int defaultPrint(const CHAR* s, ... )
	{
		static SQChar temp[2048];
		va_list vl;
		va_start(vl,s);
		_vsntprintf(temp,sizeof(temp),s,vl);
		_putts(temp);
		va_end(vl);

		return 0;
	}

	static F_Print sFuncStandardPrint = &defaultPrint;
	static bool sisShowedAssertMessage = false;

	void setStandardPrint(F_Print funcPrint)
	{
		sFuncStandardPrint = funcPrint;
	}

	F_Print getStandardPrint()
	{
		return sFuncStandardPrint;
	}

	bool isRunningScript()
	{
		return ScriptMain::getInstance().isRunningScript();
	}

	bool isShowedAssertMessage()
	{
		return sisShowedAssertMessage;
	}

	void setShowedAssertMessage(bool isShowed)
	{
		sisShowedAssertMessage = isShowed;
	}

#if (defined(_WIN32) || defined(_WIN64))

	HWND makeWindows(uint16 startPosX,uint16 startPosY,uint16 width,uint16 height,
		BaseMsgProc msgProc)
	{
		timeBeginPeriod(1);

		WNDCLASS WndClass;

		WndClass.cbClsExtra=0;
		WndClass.cbWndExtra=0;
		WndClass.hbrBackground = (HBRUSH) GetStockObject(GRAY_BRUSH);
		WndClass.hCursor = LoadCursor(NULL,IDC_ARROW);
		WndClass.hIcon = LoadIcon(NULL,IDI_APPLICATION);
		WndClass.hInstance = NULL;
		WndClass.lpfnWndProc = msgProc;
		WndClass.lpszClassName = _mrcT("mircale test windows");
		WndClass.lpszMenuName =  NULL;
		WndClass.style = CS_HREDRAW | CS_VREDRAW;
		RegisterClass(&WndClass);

		HWND hWnd = NULL;
		//		if( NoCation )
		{
			hWnd = CreateWindow(WndClass.lpszClassName, _mrcT("mircale temp window"),
				WS_POPUP, 0, 0, width, height,NULL, NULL, NULL, NULL);
		}
		// 		else
		// 		{
		// client 영역을 width x height 로 맞추기위해, non-client 영역이 차지하는 만큼 늘려준다.
		// 1024x768 -> 1032x794
		// 			DWORD style = WS_OVERLAPPED | WS_CAPTION;
		// 
		// 				Width  += GetSystemMetrics(SM_CXFRAME) * 2;
		// 				Height += GetSystemMetrics(SM_CYSIZE) + GetSystemMetrics(SM_CYFRAME) * 2;
		// 			
		//  			hWnd = CreateWindow(WndClass.lpszClassName, strTitle.c_str(),
		//  				style   , 0, 0, Width, Height,hParent, NULL,hInstance, NULL);
		// 		}

		ShowWindow(hWnd, SW_SHOWDEFAULT);
		MoveWindow(hWnd,0, 0, width,height, TRUE); 

		UpdateWindow(hWnd);

		IRenderer::InitArg initArg;
		RenderPasing::setInitArgAttribute(initArg, _mrcT("DirectX9"),true );
		RenderPasing::setInitArgAttribute(initArg, _mrcT("WindowHandle"),hWnd );
		RenderPasing::setInitArgAttribute(initArg, _mrcT("Width"), width );
		RenderPasing::setInitArgAttribute(initArg, _mrcT("Height"),height );

		IRenderer::getInstance().init(initArg);

		return hWnd;
	}

	void loadTexture()
	{
		TextureDataMgr::getInstance().setRootPath( _mrcT("res_graphic") );
		TextureDataMgr::getInstance().loadXml( _mrcT("texture_data.xml") );
	}

	void loadScript(const String& path)
	{
		ScriptMain::getInstance().init(path.c_str(),false);
		ScriptMain::getInstance().loadScript();
	}


#endif


	

	void printLayers()
	{
		LayerMgr::getInstance().printLayers();
	}

	void updateLayer( real elapsedTime )
	{
		LayerMgr::getInstance().update(elapsedTime);	
	}

	void renderLayer()
	{
		IRenderer::getInstance().clear(true,false,false);
		LayerMgr::getInstance().render();
		IRenderer::getInstance().present();
	}

	void clear()
	{
		IRenderer::getInstance().clear(true,false,false);
	}

	void present()
	{
		IRenderer::getInstance().present();
	}

	void messageProcedureLayer( const Message& msg )
	{
		if( isShowedAssertMessage() )
			return;

		if( msg.mMsgID == WM_KEYDOWN || msg.mMsgID == WM_KEYUP)
		{
			if( msg.mwParam <= 127)
				Keyboard::getInstance().pushKey((int32)msg.mwParam,msg.mMsgID == WM_KEYDOWN,0);
		}

		LayerMgr::getInstance().messageProcedure(msg);
	}

	void resetAll()
	{
		TextureDataMgr::getInstance().reload();
		// 스크립트를 재로딩하면 내부적으로 LayerMgr도 클리어함
		ScriptMain::getInstance().loadScript();
		
	}

	void reloadScript()
	{
		ScriptMain::getInstance().loadScript();
	}

	void release()
	{
		ScriptMain::getInstance().release();
		LayerMgr::release();
		// 매니저는 외부에서 다시 세팅될수 있으므로 따로 이렇게 제거해줘야함
		// ScriptMain에서 LayerMgr를 사용함으로 마지막에....
	}

	void* getDevice()
	{
		return IRenderer::getInstance().getDevice();		
	}




}



