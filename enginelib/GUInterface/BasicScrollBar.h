#pragma	once

#include "UIGroup.h"

class CBasicScrollThumbFrame;

class	CBasicScrollBar : public CUIGroup
{
private:
public:
	enum	//	기본 윈도우 ID ( 외부로 공개되지 않음 )
	{
		ET_CONTROL_UP = NO_ID + 1,
		ET_CONTROL_DN,
	};

public:
	CBasicScrollBar (EngineDeviceMan* pEngineDevice);
	virtual	~CBasicScrollBar ();

public:
	void CreateThumb ( char* szFrame, char* szThumb, char* szBodyValue, char* szUpValue, char* szDnValue, const bool& bVERTICAL = true );
	void CreateButton ( char* szUpValue, char* szUpFlipValue, char* szDnValue, char* szDnFlipValue, const bool& bVERTICAL = true );
	void CreateButtonEX ( char* szUpValue, char* szUpOverValue, char* szUpFlipValue, char* szDnValue, char* szDnOverValue,char* szDnFlipValue, const bool& bVERTICAL );
	CBasicScrollThumbFrame* const GetThumbFrame () const;
	void SetVisibleThumb( bool bVisible );

public:
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

private:
	CBasicScrollThumbFrame*	m_pThumbFrame;
};