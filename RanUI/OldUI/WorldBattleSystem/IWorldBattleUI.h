#pragma once

class IWorldBattleButton
{
public:
	virtual ~IWorldBattleButton() { };

public:
	virtual void CreateUIWindowAndRegisterOwnership() = 0;

	virtual bool IsVisible() = 0;

	virtual void ToggleWorldBattleButton(bool bOn) = 0;
};