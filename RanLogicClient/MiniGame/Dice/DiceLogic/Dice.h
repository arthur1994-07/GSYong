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
		const bool isCollision(Dice& _targetDice); // �ֻ��� ���� �浹 �˻�;
		const bool isCollision(const float _fWidth, const float _fHeight); // ���̺� �浹 �˻�;
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
		D3DXVECTOR3 vMoveDirect; // ���� �̵� ����;
		D3DXVECTOR3 vGrabPosition; // ���� ��ġ; ���� ��ġ���� ������ ���;
		D3DXVECTOR3 vRollingAxis; // �ֻ����� ȸ�� ��;
		D3DXVECTOR3 vPosition; // ���� ��ġ;

		D3DXMATRIXA16 mDetermineWorld; // �̸� ������ ���� ���� �ֻ��� ���;
		D3DXMATRIXA16 mWorld; // �ֻ����� ���;

		unsigned int nDetermineNumber;	// ���� ��ȣ;
		unsigned int nState;

		float fRollingSpeed; // ȸ�� �ӵ�;
		float fMovingSpeed; // �̵� �ӵ�;
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

