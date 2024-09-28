#pragma once

#include "./GLInventory.h"

enum
{
	EQuestX			= 8,
	EQuestY			= 1,

	EQuestTotalSlot	= EQuestX * EQuestY,
};

class GLInventoryQuest : public GLInventory
{
public:
	GLInventoryQuest()
		: GLInventory( EQuestX, EQuestY )
	{
	}

	GLInventoryQuest( WORD _sizeX, WORD _sizeY )
		: GLInventory( _sizeX, _sizeY )
	{
	}

private:
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<GLInventory>(*this);
	}
};