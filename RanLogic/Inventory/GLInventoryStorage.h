#pragma once

#include "./GLInventory.h"

enum
{
	EStorageX			= 6,
	EStorageY			= 4,

	EStorageTotalSlot	= EStorageX * EStorageY,
};

class GLInventoryStorage : public GLInventory
{
public:
	GLInventoryStorage()
		: GLInventory( EStorageX, EStorageY )
	{
	}

	GLInventoryStorage( WORD _sizeX, WORD _sizeY )
		: GLInventory( _sizeX, _sizeY )
	{
	}

	virtual ~GLInventoryStorage() {}

private:
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<GLInventory>(*this);
	}
};