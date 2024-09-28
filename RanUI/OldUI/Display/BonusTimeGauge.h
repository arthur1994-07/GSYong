#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class CInnerInterface;
class CBasicProgressBar;
class GLGaeaClient;

class CBonusTimeGauge : public CUIGroup
{

	enum
	{
		CONSUME_GAUGE,
        CHARGE_GAUGE
	};

public:
	CBonusTimeGauge(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CBonusTimeGauge();

private:
    CBasicProgressBar*	m_pConsume;
    CBasicProgressBar*  m_pCharge;
    bool m_bCharging; // ��õ �ƴϸ� �Һ� 

protected:
    CInnerInterface* m_pInterface;
    GLGaeaClient* m_pGaeaClient;

public:
	void	SetTimeConsume ( int nNOW, int nMAX );
	void	SetTimeCharge  ( int nNOW, int nMAX );
	void	CreateSubControl ();

	void	BONUS_TIME_EVENT_START( bool bCharging ); // �̺�Ʈ ����
	void	BONUS_TIME_EVENT_END(); // �̺�Ʈ ����
	void	BONUS_TIME_BUSTER_START(); // ����ġ ����
	void	BONUS_TIME_BUSTER_END(); // ����ġ ����

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

};