#pragma once

#include <emmintrin.h>

namespace SSE2
{	
#define DEFNUMBER_PI 3.141592654f
#define xmmShuffle1(_xmm, _0, _1, _2, _3) ::_mm_shuffle_ps(_xmm, _xmm, _MM_SHUFFLE(_3, _2, _1, _0))
#define xmmShuffle2(_xmm0, xmm1, _0, _1, _2, _3) ::_mm_shuffle_ps(_xmm0, xmm1, _MM_SHUFFLE(_3, _2, _1, _0))

	class CPUInformation
	{
	public:
		bool bSSE;		// Streaming SIMD Extension;
		bool bSSE2;	// Streaming SIMD Extension 2;
		bool b3DNOW;	// 3DNow!
		bool bMMX;		// MMX

		bool bExt;				// 확장된 기능이 사용가능한가?;
		bool bMMX_Ext;		// MMX 
		bool b3DNOW_Ext;	// 3DNow!

		bool bIntelCPU;		// Intel/AMD;

		unsigned int nCore;

		char	cVendorName[13];    // 생산자 이름;
	}; 
}

