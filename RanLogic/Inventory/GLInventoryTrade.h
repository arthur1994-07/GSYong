#pragma once

#include "./GLInventory.h"

enum
{
	ETradeX			= 6,
	ETradeY			= 2,

	ETradeTotalSlot	= ETradeX * ETradeY,
};

class GLInventoryTrade : public GLInventory
{
public:
	GLInventoryTrade()
		: GLInventory( ETradeX, ETradeY )
	{
	}

	GLInventoryTrade( WORD _sizeX, WORD _sizeY )
		: GLInventory( _sizeX, _sizeY )
	{
	}
};