

#ifndef _miracle_h_
#define _miracle_h_

#include "mrcConfig.h"
#include "mrcAssert.h"
#include "mrcAllocatedObject.h"

#include "mrcStdContainer.h"
#include "mrcString.h"
#include "mrcAllocatedObject.h"

#include "mrcUtil/mrcPolymorphFactory.h"
//#include "mrcTextureMgr.h"
#include "mrcMath.h"

#include "mrcMatrix4.h"
#include "mrcVector2.h"
#include "mrcVector3.h"
#include "mrcRenderer.h"



namespace mrc
{

#if (defined(_WIN32) || defined(_WIN64))
	typedef LRESULT  (WINAPI *BaseMsgProc)(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	HWND makeWindows(uint16 startPosX,uint16 startPosY,uint16 width,uint16 height,
		BaseMsgProc msgProc);
#endif

//	void initDefault();
	void loadTexture();
	void loadScript(const String& path = _mrcT("script"));

	void release();

	void* getDevice();

	void printLayers();
	void messageProcedureLayer(const Message& msg);
	void updateLayer(real elapsedTime);
	void renderLayer();

	void clear();
	void present();

	void resetAll();

	void reloadScript();


	///  mrcRegisterScriptObject 파일에 구현이 있음
	void registerScriptObject(funcRegisterScriptObject func);
}




#endif



