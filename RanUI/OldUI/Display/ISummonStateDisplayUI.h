#pragma once

#include "../../../enginelib/GUInterface/UIWindow.h"

#include "../Util/MessageBox.h"

class ISummonStateDisplayUI
{
public:
	virtual ~ISummonStateDisplayUI() { };

public:
	virtual void CreateUIWindowAndRegisterOwnership() = 0;

	virtual bool IsVisible() = 0;
};