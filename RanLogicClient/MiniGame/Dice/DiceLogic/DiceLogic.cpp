#include "pch.h"
#include "../../../../EngineLib/DxSound/DxSoundLib.h"
#include "Dice.h"
#include "DiceLogic.h"

namespace MiniGame
{
	DiceLogic::DiceLogic(const unsigned int _nDice, const unsigned int _nStageWidth, const unsigned int _nStageHeight)
		: IDice(_nDice, _nStageWidth, _nStageHeight)
	{
		//::srand((unsigned int)::time(0));

		if ( _nDice == 0 )
			return;

		if ( _nDice == 1 )
			this->pDice = new Dice;
		else
			this->pDice = new Dice[_nDice];

		const float _fSize = 3.0f;
		const float _fAngle = (D3DX_PI * 2.0f / float(_nDice - 1));

		this->pDice[_nDice - 1].vGrabPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		this->pDice[_nDice - 1].vPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

		for ( unsigned int _i = _nDice - 1; _i; --_i )
		{
			const float _fSin = ::sin(_fAngle * float(_i - 1));
			const float _fCos = ::cos(_fAngle * float(_i - 1));
			const D3DXVECTOR3 _vPosition(D3DXVECTOR3(_fCos, _fSin, 0.0f) * _fSize);
			this->pDice[_i - 1].vGrabPosition = _vPosition;
			this->pDice[_i - 1].vPosition = _vPosition;
		}
	}

	DiceLogic::~DiceLogic(void)
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
	void DiceLogic::grab(const float _fPositionX, const float _fPositionY, const float _fPositionZ)
	{
		switch ( this->emCurrentState )
		{
		case MiniGame::EMDICE_GAME_STATE_IDLE:
		case MiniGame::EMDICE_GAME_STATE_HOLD:
			{
				this->emCurrentState = MiniGame::EMDICE_GAME_STATE_HOLD;

				const float _fPosition[2] = {_fPositionX * float(this->nStageWidth), _fPositionY * float(this->nStageHeight)};
				const D3DXVECTOR3 _vGrabPosition(_fPosition[0], _fPosition[1], _fPositionZ);
				for ( unsigned int _i = this->nDice; _i; --_i )		
					this->pDice[_i - 1].grab(_vGrabPosition, float(this->nStageWidth >> 1), float(this->nStageHeight >> 1));
			}
			break;
		}
	}	

	void DiceLogic::holdOut(const unsigned int* const _pDetermineNumber, const float _fPositionX, const float _fPositionY, const float _fPositionZ)
	{
		if ( this->emCurrentState != EMDICE_GAME_STATE_HOLD )
			return;

		this->emCurrentState = EMDICE_GAME_STATE_ROLL;

		const float _fPosition[2] = {_fPositionX * float(this->nStageWidth), _fPositionY * float(this->nStageHeight)};
		const D3DXVECTOR3 _vGrabPosition(_fPosition[0], _fPosition[1], _fPositionZ);
		for ( unsigned int _i = this->nDice; _i; --_i )			
			this->pDice[_i - 1].holdOut(_pDetermineNumber[_i - 1], _vGrabPosition);
	}

	void DiceLogic::reset(void)
	{
		this->emCurrentState = MiniGame::EMDICE_GAME_STATE_IDLE;
		for ( unsigned int _i = this->nDice; _i; --_i )
			this->pDice[_i - 1].reset();
	}
	void DiceLogic::lock(void)
	{
		this->emCurrentState = MiniGame::EMDICE_GAME_STATE_LOCK;
		for ( unsigned int _i = this->nDice; _i; --_i )
			this->pDice[_i - 1].reset();
	}	

	const D3DXMATRIXA16* const DiceLogic::getDiceWorldMatrix(const unsigned int _nIndex)
	{
		if ( _nIndex < this->nDice )
			return &this->pDice[_nIndex].getMatrix();

		return 0;
	}

	void DiceLogic::_moveBlock(void)
	{
		for ( unsigned int _i = this->nDice; _i; --_i )
			this->pDice[_i - 1].moveBlock();
	}
	const bool DiceLogic::_collisionCheck(void)
	{
		unsigned int _nCollision = 0;

		const unsigned int _nDice = this->nDice;
		for ( unsigned int _i = 0; _i < _nDice; ++_i )
		{
			Dice& _dice = this->pDice[_i];
			_nCollision += (_dice.isCollision(float(this->nStageWidth >> 1), float(this->nStageHeight >> 1)) ? 1 : 0);
		}

		for ( unsigned int _i = 0; _i < _nDice; ++_i )
		{
			Dice& _dice = this->pDice[_i];			
			for ( unsigned int _j = _i + 1; _j < this->nDice; ++_j )	
				_nCollision += (_dice.isCollision(this->pDice[_j]) ? 1 : 0);
		}

		return _nCollision ? true : false;
	}

	const EMDICE_GAME_STATE DiceLogic::_updateBlock(void)
	{
		int _nResult = 0;
		for ( unsigned int _i = this->nDice; _i; --_i )
			_nResult += this->pDice[_i - 1].updateBlock();		

		this->emCurrentState = _nResult == 0 ? MiniGame::EMDICE_GAME_STATE_FINISH : this->emCurrentState;
		return this->emCurrentState;
	}

	
	const EMDICE_GAME_STATE DiceLogic::update(const float _fTime, const float _fElapsedTime)
	{
		DxSoundLib* const pSound = DxSoundLib::GetInstance();
		DiceLogic::_moveBlock();
		if ( DiceLogic::_collisionCheck() == true )
			pSound->PlaySound("DICE_ROLL");
		return DiceLogic::_updateBlock();
	}
}

