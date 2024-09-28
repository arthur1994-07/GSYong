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
	float fCurrentTime; // 현재 인증 진행 시간;
	float fAuthTime;    // 인증 완료 시간;

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
	CBasicTextBox* m_pTopText;      // 상단 텍스트 : XXX 진행중;
	CBasicTextBox* m_pMiddleText;   // 중단 텍스트 : 목표 점수 :;
	CBasicTextBox* m_pBottomText;   // 하단 텍스트 : 남은 시간 :;

	CUIControl* m_pBackGround; // 배경 화면;

	float m_fRemainTime;  // 남은 시간 : 초단위 ( 80 => 1:20 );

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
	CBasicTextBox* m_pCountryRankText;    // 순위 텍스트;
	CBasicTextBox* m_pCountryNameText;     // 클럽이름 텍스트;
	CBasicTextBox* m_pCountryPointText;    // 점수 텍스트;
	CBasicTextBox* m_pMyRankText;       // 내 클럽 순위 텍스트;
	CBasicTextBox* m_pMyNameText;       // 내 클럽 이름 텍스트;
	CBasicTextBox* m_pMyPointText;      // 내 클럽 점수 텍스트;

	CUIControl* m_pBackGround;  // 배경 화면;

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