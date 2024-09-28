//	��ų �̵�
//
//	���� �ۼ��� : ���⿱
//	���� ������ : 
//	�α�
//		[2003.12.8]
//			@ �ۼ�
//

#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class CInnerInterface;
class CBasicTextBox;
class GLGaeaClient;

class CSystemMessageWindow : public CUIGroup
{
public:
	CSystemMessageWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CSystemMessageWindow ();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void	CreateSubControl ();

public:
	void	SetSystemMessage ( const CString& strMessage, const D3DCOLOR& dwColor );

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

private:
	CBasicTextBox*		m_pTextBox;

private:
	float	m_fElapsedTime;

protected:
	CInnerInterface* m_pInterface;

};