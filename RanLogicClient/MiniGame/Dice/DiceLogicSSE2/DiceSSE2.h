#pragma once

#include "../../../../SigmaCore/SSE2/SSE2.h"

namespace MiniGame
{
	class DiceSSE2
	{	
	private:
		__forceinline const __m128_4& getMatrix(void) const { return this->xWorld; }

		void holdOut(const unsigned int _nDetNum, const __m128& _xHoldOutPosition);
		void grab(const __m128& _xGrabPosition, const float _fWidth, const float _fHeight);

		void moveBlock(void);
		const bool isCollision(DiceSSE2& _targetDice); // 주사위 간의 충돌 검사;
		const bool isCollision(const float _fWidth, const float _fHeight); // 테이블 충돌 검사;
		const int updateBlock(void);

		void reset(void);

	private:
		void _setRotateAxis(void);

		void _determineMatrix(void);
		void _interpolateMatrix(void);

		const bool _isGrab(void);

	private:
		friend class DiceLogicSSE2;
		void* operator new(const size_t _nSize);
		void* operator new[](const size_t _nSize);
		void operator delete(void* _pThis);
		void operator delete[](void* _pThis);
		DiceSSE2(void);
		~DiceSSE2(void);

	private:
		__m128 xMoveDirect; // 현재 이동 방향;
		__m128 xGrabPosition; // 잡은 위치; 현재 위치와의 보간에 사용;
		__m128 xRollingAxis; // 주사위의 회전 축;
		__m128 xPosition; // 현재 위치;

		__m128_4 xDetermineWorld; // 미리 정해진 값에 대한 주사위 행렬;		
		__m128_4 xWorld; // 주사위의 행렬;

		unsigned int nDetermineNumber;	// 결정 번호;
		unsigned int nState;

		float fRollingSpeed;	// 회전 속도;
		float fMovingSpeed;	// 이동 속도;		
	};

	namespace DiceConstNumber
	{
		const __m128_4 xConstNumber[6] = 
		{
			__m128_4( // 1;
			-1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, -1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
			),
			__m128_4( // 2;
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			-1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
			),
			__m128_4( // 3;
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, -1.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
			),
			__m128_4( // 4;
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, -1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
			),
			__m128_4( // 5;
			0.0f, 0.0f, -1.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
			),		
			__m128_4( // 6;
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
			)
		};
	}
}
