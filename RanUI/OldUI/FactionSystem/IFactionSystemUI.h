#pragma once

#include "../../../enginelib/GUInterface/UIWindow.h"

#include "../Util/MessageBox.h"

#include "./FactionSystemUIDataType.h"


class IFactionInfoWindow
{
public:
	virtual ~IFactionInfoWindow ( ) { };

public:
	virtual void CreateUIWindowAndRegisterOwnership ( ) = 0;
};