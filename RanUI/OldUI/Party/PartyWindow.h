//	파티 윈도우
//
//	최초 작성자 : 성기엽
//	이후 수정자 : 
//	로그
//		[2003.11.27]
//			@ 작성
//

#pragma	once

#include "../Util/UIWindowEx.h"
#include "../../InnerInterface.h"

class CInnerInterface;
class CPartySlot;
class CBasicTextBox;
class GLGaeaClient;
struct GLPartySlotClient;

class CPartyWindow : public CUIWindowEx
{
protected:
	enum
	{
		PARTY_SLOT_MEMBER0 = ET_CONTROL_NEXT,
		PARTY_SLOT_MEMBER1,
		PARTY_SLOT_MEMBER2,
		PARTY_SLOT_MEMBER3,
		PARTY_SLOT_MEMBER4,
		PARTY_SLOT_MEMBER5,
		PARTY_SLOT_MEMBER6,
		PARTY_SLOT_MEMBER7,
	};

public:
	CPartyWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CPartyWindow();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
    void	CreateSubControl();

public:	
	void SetPartyInfo(const DWORD dwPartyID, const DWORD dwHashKey, const DWORD dwMasterID, const DWORD dwTimeKey);
	
	const DWORD GetPartyID(void) const;
	const DWORD GetHashKey(void) const;
	const DWORD GetMasterID(void) const;
	const DWORD GetTimeKey(void) const;

private:
	GLPartySlotClient* const FindSelfClient(void);
	BOOL	IsSameMap( GLPartySlotClient *pSelf, GLPartySlotClient *pMember );

private:
	void	SetSlotMember( CPartySlot* pPartySlot, GLPartySlotClient *pMember, BOOL bSameMap, BOOL bQuitEnable, BOOL bAuthEnable );

public:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

private:
	CPartySlot*		m_pPartySlot[MAXPARTY];
	CBasicTextBox*	m_pConditionTextBox;

private:
	SPartyOption	m_sPartyOption;

private:
	DWORD	m_dwPartyID;
	DWORD	m_dwHashKey;
	DWORD	m_dwMasterID;
	DWORD	m_dwTimeKey;

};

class MyPartyWindow : public IPartyWindow, private CPartyWindow
{
public:
    MyPartyWindow( GLGaeaClient* pGaeaCleint, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~MyPartyWindow()    {};

public:
    virtual void CreateUIWindowAndRegisterOwnership();

	virtual const DWORD GetPartyID(void) const override;
	virtual const DWORD GetHashKey(void) const override;
    virtual const DWORD GetMasterID(void) const override;
	virtual const DWORD GetTimeKey(void) const override;
    virtual void SetPartyInfo(const DWORD dwPartyID, const DWORD dwHashKey, const DWORD dwMasterID, const DWORD dwTimeKey) override;    
};