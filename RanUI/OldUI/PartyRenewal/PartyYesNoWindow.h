#pragma once

#include <boost/signal.hpp>
#include "../../InnerInterface.h"
#include "../Util/UIWindowEx.h"
#include "PartyYesNoWindowDefine.h"

class GLGaeaClient;
class PartyYesNoWindow : public CUIWindowEx
{
	typedef std::vector<PartyYesNoNode> YESNO_NODE_VECTOR;
	typedef YESNO_NODE_VECTOR::const_iterator YESNO_NODE_VECTOR_CITER;

	enum
	{
		BUTTON_YES = ET_CONTROL_NEXT + 1,		
		BUTTON_NO,
		TEXT_DESCRIPTION,
	};

public:
	virtual void Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl) override;
	virtual void TranslateUIMessage(UIGUID ControlID, DWORD dwMsg) override;

private:
	void Execute(const bool bYes);

protected:
	const DWORD AddComponent(const std::string& strDescription, PARTY_YESNO_FUNCTION PartyYesNoFunction); // return ID;
	void CreateSubControl(void);
	void SetID(const DWORD nID);

private:

public:
	PartyYesNoWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~PartyYesNoWindow(void);

private:
	GLGaeaClient* m_pGaeaClient;

	RnButton* m_pButtonYes;
	RnButton* m_pButtonNo;
	CBasicTextBox* m_pTextDescription;

	YESNO_NODE_VECTOR m_YesNoNodeVector;
	DWORD m_nID;
};

class MyPartyYesNoWindow : public PartyYesNoWindow, IPartyYesNoWindow
{
public:
	virtual const DWORD AddComponent(const std::string& strDescription, PARTY_YESNO_FUNCTION PartyYesNoFunction) override; // return ID;
	virtual void CreateUIWindowAndRegisterOwnership(void) override;
	virtual void SetID(const DWORD nID) override;	

public:
	MyPartyYesNoWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~MyPartyYesNoWindow(void){};
};