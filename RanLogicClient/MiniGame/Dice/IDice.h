#pragma once

#include "IDiceDefine.h"

namespace MiniGame
{
	class IDice
	{
	public:
		virtual const MiniGame::EMDICE_GAME_STATE update(const float _fTime, const float _fElapsedTime) = 0;

		virtual void grab(const float _fPositionX, const float _fPositionY, const float _fPositionZ = -5.0f) = 0;
		virtual void holdOut(const unsigned int* const pDetermineNumber, const float _fPositionX, const float _fPositionY, const float _fPositionZ = -5.0f) = 0;		
		virtual void reset(void) = 0;
		virtual void lock(void) = 0;

		virtual const D3DXMATRIX* const getDiceWorldMatrix(const unsigned int _nIndex) = 0;

		const unsigned int getNDice(void) const;		

	public:
		IDice(const unsigned int _nDice, const unsigned int _nStageWidth, const unsigned int _nStageHeight);
		virtual ~IDice(void);

	protected:
		const unsigned int nDice;
		const unsigned int nStageWidth;
		const unsigned int nStageHeight;
		MiniGame::EMDICE_GAME_STATE emCurrentState;
	};
}