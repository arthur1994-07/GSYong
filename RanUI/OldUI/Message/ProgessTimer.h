#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class CInnerInterface;
class GLGaeaClient;
class CBasicTextBox;
class CBasicLineBoxSmart;

class ProgessTimer : public CUIGroup
{
public:
	ProgessTimer( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~ProgessTimer() {};

private:
protected:
	GLGaeaClient* m_pGaeaClient;
	CInnerInterface* m_pInterface;

private:
	CBasicTextBox* m_pProgessText;

	CUIControl* m_pWatchIcon;
	CBasicLineBoxSmart* m_pBlank;
	CBasicLineBoxSmart* m_pGauge;

private:
	bool m_bForceRebirth;

	float m_fTime;
	float m_fMaxTime;
	std::string m_strText;
	int m_nTextIndex;

public:
	void CreateSubControl();
	void SetTime( UINT iTime );
	void SetProgessTimer(UINT iTime,float fPosX, float fPosY, const char* szText, int nTextIndex);

public:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
};

class MyProgessTimer: public IProgessTimer, private ProgessTimer
{
public:
	MyProgessTimer( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
		:ProgessTimer( pGaeaClient, pInterface, pEngineDevice )
	{

	}
	virtual ~MyProgessTimer()   {};

public:
	virtual void CreateUIWindowAndRegisterOwnership();
	virtual void SetProgessTimer( UINT iTime,float fPosX, float fPosY, const char* szText, int nTextIndex );
};