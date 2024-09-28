//	ä�� �ڽ�
//
//	���� �ۼ��� : ���⿱
//	���� ������ : 
//	�α�
//		[2003.11.25]
//			@ ���� �ۼ�
//

#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class CInnerInterface;
class CBasicTextBox;
class GLGaeaClient;

class CSimpleMessageMan : public CUIGroup
{
private:
static	const	float	fLIFE_TIME;

public:
	CSimpleMessageMan(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CSimpleMessageMan ();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void	CreateSubControl ();

public:
	void	ClearText ();
	void	AddText ( CString strMessage, D3DCOLOR dwMessageColor );

public:
	virtual void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

private:
	CBasicTextBox*	m_pInfoDisplay;	
	CUIControl*		m_pInfoDisplayDummy;

private:
	float	m_fLifeTime;
	std::vector<CString>	m_vecMESSAGE;

public:
	float GetLifeTime ()	{ return m_fLifeTime; }

protected:
	CInnerInterface* m_pInterface;
};