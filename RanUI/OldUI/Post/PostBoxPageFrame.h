
#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class CPostBoxPage;
class CPostBoxWindowObject;

class CPostBoxPageFrame : public CUIGroup
{
protected:
	enum
	{
		POSTBOX_PAGE_FRAME_NONE = NO_ID + 1,
		POSTBOX_PAGE_FRAME,
	};

public:
	CPostBoxPageFrame(EngineDeviceMan* pEngineDevice);
	virtual ~CPostBoxPageFrame ();

public:
	void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	void PageOpen  ( CPostBoxWindowObject* pParentWindow, CPostBoxPage* pPage );
	void PageClose ();

public:
	void BeginSubControl ();
	void OpenSubControl  ();
	void CloseSubControl ();
	void EndSubControl	 ();

public:
	CPostBoxPage* m_pPage;

};
