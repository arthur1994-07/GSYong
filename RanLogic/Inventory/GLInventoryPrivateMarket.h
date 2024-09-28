#pragma once

#include "./GLInventory.h"

enum
{
	EPrivateMarketX			= 6,
	EPrivateMarketY			= 4,

	EPrivateMarketTotalSlot	= EPrivateMarketX * EPrivateMarketY,
};

class GLInventoryPrivateMarket : public GLInventory
{
public:
	GLInventoryPrivateMarket()
		: GLInventory( EPrivateMarketX, EPrivateMarketY )
	{
	}

	GLInventoryPrivateMarket( WORD _sizeX, WORD _sizeY )
		: GLInventory( _sizeX, _sizeY )
	{
	}
};