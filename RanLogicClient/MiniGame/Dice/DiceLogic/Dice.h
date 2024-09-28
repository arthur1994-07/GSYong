#pragma once

namespace MiniGame
{
	class Dice
	{		
	private:
		__forceinline const D3DXMATRIXA16& getMatrix(void) const { return this->mWorld; }

		void holdOut(const unsigned int _nDetNum, const D3DXVECTOR3& _vHoldOutPosition);
		void grab(const D3DXVECTOR3& _vGrabPosition, const float _fWidth, const float _fHeight);

		void moveBlock(void);
		const bool isCollision(Dice& _targetDice); // 주사위 간의 충돌 검사;
		const bool isCollision(const float _fWidth, const float _fHeight); // 테이블 충돌 검사;
		const int updateBlock(void);

		void reset(void);

	private:
		void _setRotateAxis(void);

		void _determineMatrix(void);
		void _interpolateMatrix(void);

		const bool _isGrab(void);

	private:
		friend class DiceLogic;
		Dice(void);
		~Dice(void);

	private:		
		D3DXVECTOR3 vMoveDirect; // 현재 이동 방향;
		D3DXVECTOR3 vGrabPosition; // 잡은 위치; 현재 위치와의 보간에 사용;
		D3DXVECTOR3 vRollingAxis; // 주사위의 회전 축;
		D3DXVECTOR3 vPosition; // 현재 위치;

		D3DXMATRIXA16 mDetermineWorld; // 미리 정해진 값에 대한 주사위 행렬;
		D3DXMATRIXA16 mWorld; // 주사위의 행렬;

		unsigned int nDetermineNumber;	// 결정 번호;
		unsigned int nState;

		float fRollingSpeed; // 회전 속도;
		float fMovingSpeed; // 이동 속도;
	};

	namespace DiceConstNumber
	{
		const D3DXMATRIXA16 mConstNumber[6] = 
		{
			D3DXMATRIXA16( // 1
			-1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, -1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
			),
			D3DXMATRIXA16( // 2
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			-1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
			),
			D3DXMATRIXA16( // 3
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, -1.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
			),
			D3DXMATRIXA16( // 4
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, -1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
			),
			D3DXMATRIXA16( // 5
			0.0f, 0.0f, -1.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
			),		
			D3DXMATRIXA16( // 6
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
			)
		};
	}
}

