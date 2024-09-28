
// bjju.sns

#pragma	once

#include "../EngineLib/GUInterface/UIGroup.h"

class CUIPage;
class CUIWindowObject;

class CUIPageFrame : public CUIGroup
{
protected:
	enum
	{
		PAGE_FRAME_NONE = NO_ID + 1,
		PAGE_FRAME,
	};

public:
	enum
	{
		PAGE_ANIMATION_NONE  = 0,
		PAGE_ANIMATION_ALPHA = 1,
	};

public:
	CUIPageFrame(EngineDeviceMan* pEngineDevice);
	virtual ~CUIPageFrame();

public:
	void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	void PageOpen  ( CUIWindowObject* pParentWindow, CUIPage* pPage, BOOL bAnimation=FALSE );
	void PageClose ();

public:
	void BeginSubControl ();
	void OpenSubControl  ();
	void CloseSubControl ();
	void EndSubControl	 ();

protected:
	void BeginOpenAnimation ( CUIWindowObject* pParentWindow, CUIPage* pPage );
	void UpdateAnimation	( float fElapsedTime );

public:
	CUIPage* m_pPage;

public:
	DWORD	 m_dwAniType;
	FLOAT	 m_fAniSpeedPerSec;

private:
	CUIPage* m_pAniPageDest;
	FLOAT	 m_fAniTime;
	BOOL	 m_bAniPlay;
};
