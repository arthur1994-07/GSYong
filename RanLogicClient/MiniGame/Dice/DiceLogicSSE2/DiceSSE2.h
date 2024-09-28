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
		const bool isCollision(DiceSSE2& _targetDice); // �ֻ��� ���� �浹 �˻�;
		const bool isCollision(const float _fWidth, const float _fHeight); // ���̺� �浹 �˻�;
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
		__m128 xMoveDirect; // ���� �̵� ����;
		__m128 xGrabPosition; // ���� ��ġ; ���� ��ġ���� ������ ���;
		__m128 xRollingAxis; // �ֻ����� ȸ�� ��;
		__m128 xPosition; // ���� ��ġ;

		__m128_4 xDetermineWorld; // �̸� ������ ���� ���� �ֻ��� ���;		
		__m128_4 xWorld; // �ֻ����� ���;

		unsigned int nDetermineNumber;	// ���� ��ȣ;
		unsigned int nState;

		float fRollingSpeed;	// ȸ�� �ӵ�;
		float fMovingSpeed;	// �̵� �ӵ�;		
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
