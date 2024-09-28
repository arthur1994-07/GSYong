

#ifndef _mrcSig_h_
#define _mrcSig_h_

#include "mrcConfig.h"

namespace mrcSig
{
	using namespace mrc;

	void Init(void* device,HWND hWnd);
	void Update(real elapsedTime);
	void Render(real elapsedTime);

	void Release();

	
}








#endif