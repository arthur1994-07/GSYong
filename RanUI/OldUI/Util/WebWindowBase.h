#pragma	once

#include "./UIWindowEx.h"

class CInnerInterface;

class CWebWindowBase : public CUIWindowEx
{
public:
    static BOOL s_bSkipVisibleWeb;

protected:
    INT m_iWebID;
    TSTRING m_strAddress;
	TSTRING m_strPostArgument;

    INT m_iAlignLeft;
    INT m_iAlignTop;
    INT m_iAlignRight;
    INT m_iAlignBottom;

public:
	CWebWindowBase( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );

public:
    virtual VOID SetAddress( const TCHAR* pszAddress );
	virtual VOID AlignPosition();
	virtual VOID SetPostArgument( const TCHAR* pszPOST );

public:
	virtual VOID SetVisibleSingle( BOOL bVisible );
	virtual VOID Update( INT x, INT y, BYTE LB, BYTE MB, BYTE RB, INT nScroll, FLOAT fElapsedTime, BOOL bFirstControl );
	virtual VOID* GetWebBrowser();
	virtual BOOL IsCompleteLoad();
	virtual VOID SetCompleteLoad( BOOL bCompleteLoad );

public:
	INT GetWebID() { return m_iWebID; }
	const TCHAR* GetAddress() { return m_strAddress.c_str(); }
	const TCHAR* GetPostArgument() { return m_strPostArgument.c_str(); }
};