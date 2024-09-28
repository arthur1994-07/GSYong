#pragma once

#include "../IDice.h"

namespace MiniGame
{
	class Dice;
	class DiceLogic : public IDice
	{
	public:
		virtual const MiniGame::EMDICE_GAME_STATE update(const float _fTime, const float _fElapsedTime) override;

		virtual void grab(const float _fPositionX, const float _fPositionY, const float _fPositionZ = -3.0f) override;		
		virtual void holdOut(const unsigned int* const pDetermineNumber, const float _fPositionX, const float _fPositionY, const float _fPositionZ = -3.0f) override;
		virtual void reset(void) override;
		virtual void lock(void) override;

		virtual const D3DXMATRIXA16* const getDiceWorldMatrix(const unsigned int _nIndex) override;

	private:
		void _moveBlock(void);
		const bool _collisionCheck(void);
		const MiniGame::EMDICE_GAME_STATE _updateBlock(void);

	public:
		DiceLogic(const unsigned int _nDice, const unsigned int _nStageWidth, const unsigned int _nStageHeight);
		virtual ~DiceLogic(void);

	private:
		Dice* pDice;
	};
}