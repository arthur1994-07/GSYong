#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../RanLogic/Msg/GLContrlInstanceContentsMsg.h"
#include "../../InnerInterface.h"

class CBasicProgressBar;
class GLGaeaClient;
class CInnerInterface;

class CTIAuthProgressUI : public CUIGroup
{
private:
	enum
	{
		CONFT_CONFIRM = NO_ID + 1,
	};

public:
	CTIAuthProgressUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual	~CTIAuthProgressUI();

public:
	void CreateSubControl ();

	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl ) override;
	void SetAuthTime(const float _fAuthTime, const float _fCurrentTime);

protected:
	float fCurrentTime; // ���� ���� ���� �ð�;
	float fAuthTime;    // ���� �Ϸ� �ð�;

	GLGaeaClient* m_pGaeaClient;
	CInnerInterface* m_pInterface;
	CBasicProgressBar* m_pProgress;
};

class CTIProgressInfoUI : public CUIGroup
{
public:
	CTIProgressInfoUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual	~CTIProgressInfoUI();    

	void CreateSubControl ();

	void SetTopParam(const int _iParam1, const int _iParam2);
	void SetMiddleParam(const int _iParam1);
	void SetBottomParam(const float _fParam1);

	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl ) override;
protected:
	CBasicTextBox* m_pTopText;      // ��� �ؽ�Ʈ : XXX ������;
	CBasicTextBox* m_pMiddleText;   // �ߴ� �ؽ�Ʈ : ��ǥ ���� :;
	CBasicTextBox* m_pBottomText;   // �ϴ� �ؽ�Ʈ : ���� �ð� :;

	CUIControl* m_pBackGround; // ��� ȭ��;

	float m_fRemainTime;  // ���� �ð� : �ʴ��� ( 80 => 1:20 );

	GLGaeaClient* m_pGaeaClient;
	CInnerInterface* m_pInterface;
	CBasicProgressBar* m_pProgress;
};

class CTIRankInfoUI : public CUIGroup
{
public:
	CTIRankInfoUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~CTIRankInfoUI();

	void CreateSubControl ();
	void SetRankInfo(GLMSG::NET_CTI_RANK_VEC& _RankInfo);
	void SetRankMyInfo(const int _iRank, const int _iPoint);

	virtual	void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl ) override;
	virtual void SetVisibleSingle( BOOL bVisible ) override;
protected:
	std::vector<DWORD> countryRank;
	CBasicTextBox* m_pCountryRankText;    // ���� �ؽ�Ʈ;
	CBasicTextBox* m_pCountryNameText;     // Ŭ���̸� �ؽ�Ʈ;
	CBasicTextBox* m_pCountryPointText;    // ���� �ؽ�Ʈ;
	CBasicTextBox* m_pMyRankText;       // �� Ŭ�� ���� �ؽ�Ʈ;
	CBasicTextBox* m_pMyNameText;       // �� Ŭ�� �̸� �ؽ�Ʈ;
	CBasicTextBox* m_pMyPointText;      // �� Ŭ�� ���� �ؽ�Ʈ;

	CUIControl* m_pBackGround;  // ��� ȭ��;

	GLGaeaClient* m_pGaeaClient;
	CInnerInterface* m_pInterface;
	CBasicProgressBar* m_pProgress;
};












class MyCTIAuthProgressUI : public ICTIAuthProgressUI, public CTIAuthProgressUI
{
public:
	MyCTIAuthProgressUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~MyCTIAuthProgressUI() {};

public:
	virtual void CreateUIWindowAndRegisterOwnership();

	virtual void SetAuthTime(const bool _bVisible, const float _fAuthTime, const float _fCurrentTime = 0.0f);
};

class MyCTIProgressInfoUI : public ICTIProgressInfoUI, public CTIProgressInfoUI
{
public:
	MyCTIProgressInfoUI(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~MyCTIProgressInfoUI() {};

public:
	virtual void CreateUIWindowAndRegisterOwnership();

	virtual void SetProgressInfo(const NET_MSG_GENERIC* _pMessag);
};

class MyCTIRankInfoUI: public ICTIRankInfoUI, public CTIRankInfoUI
{
public:
	MyCTIRankInfoUI(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~MyCTIRankInfoUI() {};

public:
	virtual void CreateUIWindowAndRegisterOwnership();

	virtual void SetRankInfo(const NET_MSG_GENERIC* _pMessag);
	virtual void SetRankMyInfo(const NET_MSG_GENERIC* _pMessag);
};