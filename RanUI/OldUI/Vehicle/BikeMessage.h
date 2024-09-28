#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class	CBasicTextBox;

class CBikeMessageWindow : public CUIGroup
{
public:
	CBikeMessageWindow(EngineDeviceMan* pEngineDevice);
	virtual	~CBikeMessageWindow ();

public:
	void	CreateSubControl ();

private:
	CBasicTextBox*		m_pTextBox;
};