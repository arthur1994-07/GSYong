#include "pch.h"
#include "IDice.h"

namespace MiniGame
{
	IDice::IDice(const unsigned int _nDice, const unsigned int _nStageWidth, const unsigned int _nStageHeight)
		: emCurrentState(MiniGame::EMDICE_GAME_STATE_IDLE)
		, nDice(_nDice)
		, nStageWidth(_nStageWidth)
		, nStageHeight(_nStageHeight)
	{
	}
	IDice::~IDice(void)
	{
	}

	const unsigned int IDice::getNDice(void) const
	{
		return this->nDice;
	}
}

