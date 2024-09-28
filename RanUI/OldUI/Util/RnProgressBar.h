#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class CBasicLineBoxSmart;

class RnProgressBar : public CUIGroup
{
public:
    enum
    {
        HORIZONTAL,
        VERTICAL,
    };

	struct CreateArg
    {
        std::string controlName;
        std::string backgroundTextureName;
        std::string overTextureName;
    };

private:
    BYTE m_eType;

	DWORD m_nNow;
	DWORD m_nMax;
	float m_fRate;

	CBasicLineBoxSmart* m_pDefault;
    CUIControl* m_pOverPos;
    CBasicLineBoxSmart* m_pOver;

public:
    RnProgressBar( EngineDeviceMan* pEngineDevice );
    virtual ~RnProgressBar();

public:
    void CreateSubControl( const CreateArg& arg );
    void CreateSubControl( std::string controlName );
    void SetAlignFlag( WORD wFlag );
    virtual void SetGlobalPos( const UIRECT& rcPos );
    virtual void SetGlobalPos( const D3DXVECTOR2& vPos );
    virtual	void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );		

public:
	void UpdateProgress();
	bool IsEndProgress();

private:
	void CalculateRate();
	void CalculateValue();

public:
	void SetType( BYTE eType ) { m_eType = eType; }
	void SetNow( DWORD nNow ) { m_nNow = nNow; CalculateRate(); }
	void SetMax( DWORD nMax ) { m_nMax = nMax; CalculateRate(); }
	void SetRate( float fRate )	{m_fRate = fRate; CalculateValue();}

public:
	BYTE GetType() { return m_eType; }
	DWORD GetNow() { return m_nNow; }
	DWORD GetMax() { return m_nMax; }
	float GetRate() { return m_fRate; }
};