#include "pch.h"
#include "../../../../EngineLib/DxSound/DxSoundLib.h"
#include "../../../../SigmaCore/SSE2/SSE2.h"
#include "DiceSSE2.h"
#include "DiceLogicSSE2.h"

namespace MiniGame
{
	DiceLogicSSE2::DiceLogicSSE2(const unsigned int _nDice, const unsigned int _nStageWidth, const unsigned int _nStageHeight)
		: IDice(_nDice, _nStageWidth, _nStageHeight)
	{
		//::srand((unsigned int)::time(0));

		if ( _nDice == 0 )
			return;

		if ( _nDice == 1 )
			this->pDice = new DiceSSE2;
		else
			this->pDice = new DiceSSE2[_nDice];

		const __m128& _xSize = SSE2::xmm(3.0f, 3.0f, 3.0f);
		const float _fReferenceAngle = (DEFNUMBER_PI * 2.0f / float(_nDice - 1));

		this->pDice[_nDice - 1].xGrabPosition = SSE2::xmm(0.0f, 0.0f, 0.0f);
		this->pDice[_nDice - 1].xPosition = SSE2::xmm(0.0f, 0.0f, 0.0f);

		for ( unsigned int _i = _nDice - 1; _i; --_i )
		{
			const float _fAngle = _fReferenceAngle * float(_i - 1);
			const float _fSin = ::sin(_fAngle);
			const float _fCos = ::cos(_fAngle);
			const __m128& _xPosition = SSE2::xmm(_fCos, _fSin, 0.0f) * _xSize;
			this->pDice[_i - 1].xGrabPosition = _xPosition;
			this->pDice[_i - 1].xPosition = _xPosition;
		}
	}

	DiceLogicSSE2::~DiceLogicSSE2(void)
	{
		if ( this->pDice != 0 )
		{
			if ( this->nDice == 1 )
				delete this->pDice;
			else
				delete[] this->pDice;
		}		
	}

	/*
	 * 주사위를 집어 들고 있는 좌표로 호출;  
	 * 집어 든 상태로 움직이려면 계속 호출 해줘야 함;
	*/
	void DiceLogicSSE2::grab(const float _fPositionX, const float _fPositionY, const float _fPositionZ)
	{
		switch ( this->emCurrentState )
		{
		case MiniGame::EMDICE_GAME_STATE_IDLE:
		case MiniGame::EMDICE_GAME_STATE_HOLD:
			{
				this->emCurrentState = MiniGame::EMDICE_GAME_STATE_HOLD;

				const float _fPosition[2] = {_fPositionX * float(this->nStageWidth), _fPositionY * float(this->nStageHeight)};
				const __m128& _xGrabPosition = SSE2::xmm(_fPosition[0], _fPosition[1], _fPositionZ, 0.0f);
				for ( unsigned int _i = this->nDice; _i; --_i )		
					this->pDice[_i - 1].grab(_xGrabPosition, float(this->nStageWidth >> 1), float(this->nStageHeight >> 1));
			}
			break;
		}
	}	

	void DiceLogicSSE2::holdOut(const unsigned int* const _pDetermineNumber, const float _fPositionX, const float _fPositionY, const float _fPositionZ)
	{
		if ( this->emCurrentState != MiniGame::EMDICE_GAME_STATE_HOLD )
			return;

		this->emCurrentState = EMDICE_GAME_STATE_ROLL;

		const float _fPosition[2] = {_fPositionX * float(this->nStageWidth), _fPositionY * float(this->nStageHeight)};
		const __m128& _xGrabPosition = SSE2::xmm(_fPosition[0], _fPosition[1], _fPositionZ, 0.0f);
		for ( unsigned int _i = this->nDice; _i; --_i )			
			this->pDice[_i - 1].holdOut(_pDetermineNumber[_i - 1], _xGrabPosition);
	}

	void DiceLogicSSE2::reset(void)
	{
		this->emCurrentState = MiniGame::EMDICE_GAME_STATE_IDLE;
		for ( unsigned int _i = this->nDice; _i; --_i )
			this->pDice[_i - 1].reset();
	}
	void DiceLogicSSE2::lock(void)
	{
		this->emCurrentState = MiniGame::EMDICE_GAME_STATE_LOCK;
		for ( unsigned int _i = this->nDice; _i; --_i )
			this->pDice[_i - 1].reset();
	}	

	const D3DXMATRIX* const DiceLogicSSE2::getDiceWorldMatrix(const unsigned int _nIndex)
	{
		if ( _nIndex < this->nDice )
			return (D3DXMATRIX*)&this->pDice[_nIndex].getMatrix();

		return 0;
	}

	void DiceLogicSSE2::_moveBlock(void)
	{
		for ( unsigned int _i = this->nDice; _i; --_i )
			this->pDice[_i - 1].moveBlock();
	}
	const bool DiceLogicSSE2::_collisionCheck(void)
	{
		unsigned int _nCollision = 0;

		const unsigned int _nDice = this->nDice;
		for ( unsigned int _i = 0; _i < _nDice; ++_i )
		{
			DiceSSE2& _dice = this->pDice[_i];
			_nCollision += (_dice.isCollision(float(this->nStageWidth >> 1), float(this->nStageHeight >> 1)) ? 1 : 0);
		}

		for ( unsigned int _i = 0; _i < _nDice; ++_i )
		{
			DiceSSE2& _dice = this->pDice[_i];			
			for ( unsigned int _j = _i + 1; _j < this->nDice; ++_j )	
				_nCollision += (_dice.isCollision(this->pDice[_j]) ? 1 : 0);
		}

		return _nCollision ? true : false;
	}
	const EMDICE_GAME_STATE DiceLogicSSE2::_updateBlock(void)
	{
		int _nResult = 0;
		for ( unsigned int _i = this->nDice; _i; --_i )
			_nResult += this->pDice[_i - 1].updateBlock();		
		
		this->emCurrentState = _nResult == 0 ? MiniGame::EMDICE_GAME_STATE_FINISH : this->emCurrentState;
		return this->emCurrentState;
	}

	const EMDICE_GAME_STATE DiceLogicSSE2::update(const float _fTime, const float _fElapsedTime)
	{
		DxSoundLib* const pSound = DxSoundLib::GetInstance();
		DiceLogicSSE2::_moveBlock();
		if ( DiceLogicSSE2::_collisionCheck() == true )
			pSound->PlaySound("DICE_ROLL");
		return DiceLogicSSE2::_updateBlock();
	}	
}

