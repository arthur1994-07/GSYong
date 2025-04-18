#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../InnerInterface.h"

class CInnerInterface;

class CConftDisplayMan : public CUIGroup
{
private:
	enum
	{
		MAX_NUMBER_COUNT = 6,
		MAX_RESULT = 3
	};

private:
static	const	float	fCHAR_HEIGHT;
static	const	float	fNUMBER_FADETIME;
static	const	float	fFIGHT_FADETIME;
static	const	float	fRESULT_FADETIME;
static	const	float	fGROUP_HIDE_DEAY;
static	const	int		nBUFFER;

private:
	enum
	{
		MAPMOVE_OKBUTTON = NO_ID + 1,
	};

public:
	CConftDisplayMan(CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CConftDisplayMan();

public:
	void	CreateSubControl ();

public:
	void	SetFightBegin( const int nCount );
	void	SetFightEnd( const int nResult );

public:
	float	GetFadeTime();

private:
	void	SetHideAll();

public:
	virtual	void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual HRESULT FinalCleanup();

private:
	CUIControl*		CreateControl( const char* szControl );

private:	
	//	인덱스 0번이 Fight,
	//	1부터는 순서대로 그 번호이다.
	CUIControl*		m_pNumber[MAX_NUMBER_COUNT];
	CUIControl*		m_pResult[MAX_RESULT];

	float	m_fFadeTime;

protected:
	CInnerInterface* m_pInterface;

};

class MyConftDisplayMan : public IConftDisplayMan, private CConftDisplayMan
{
public:
    MyConftDisplayMan( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~MyConftDisplayMan()    {};

public:
    virtual void CreateUIWindowAndRegisterOwnership();

    virtual void SetFightBegin( const int nCount );
    virtual void SetFightEnd( const int nResult );
};