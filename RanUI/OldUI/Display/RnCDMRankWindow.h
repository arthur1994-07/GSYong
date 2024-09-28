#pragma	once

#include "../Util/UIWindowEx.h"
//#include "../../../RanLogicClient/InterfaceBaseDefine.h"
#include "../../../EngineLib/GUInterface/InterfaceBaseDefine.h"

#include "../../InnerInterface.h"
#include "../../../RanLogic/Msg/GLContrlInstanceContentsMsg.h"

class CInnerInterface;
class GLGaeaClient;
class CBasicLineBoxSmart;
class CBasicTextButton;
class CBasicTextBox;
class RnCDMRankList;
class CUIStringListBox;

using namespace InstanceSystem;


/************************************************************************/
//////////////////////////////////////////////////////////////////////////
//       아쉽게도 사용하지 않는다!										//
//          사용금지!													//
//               궁금하면 직접 찾아보길..								//
//////////////////////////////////////////////////////////////////////////
/************************************************************************/


class RnCDMRankWindow : public CUIWindowEx
{
protected:
	enum
	{
		RNCDM_RESULT_WINDOW_NONE = ET_CONTROL_NEXT,
		RNCDM_RESULT_SORTBUTTON_TOTAL,

		RNCDM_RESULT_RANKING,

		RNCDM_RESULT_BUTTON_CLOSE,

		RNCDM_INFO,
	};

protected:
	struct SORTBUTTON : public CUIGroup
	{
		CBasicLineBoxSmart* ACTIVE;
		CBasicLineBoxSmart* INACTIVE;
		CBasicTextBox*      TEXTBOX;
		bool                ENABLE;

		SORTBUTTON(EngineDeviceMan* pEngineDevice)
			: CUIGroup(pEngineDevice)
			, ACTIVE(NULL)
			, INACTIVE(NULL)
			, TEXTBOX(NULL)
			, ENABLE(FALSE)
		{
		}

		void CreateSubControl ( const std::string& strLabel );
		void SetEnable        ( const bool bEnable );
	};	

public:
	RnCDMRankWindow ( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual	~RnCDMRankWindow ();

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl ) override;
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg ) override;
    virtual void SetVisibleSingle(BOOL visible) override;

public:
	void CreateSubControl ();

public:
	void Show();
	void Update();
	void UpdateTime();
	void SelectSortButton( const UIGUID cID );
	RNCDM_RANK_SLOT GetRankSlotData(int nIndex);

protected:
	CBasicLineBoxSmart* CreateLineBox ( const std::string& strControlName, const std::string& strTexInfo, const UIGUID cID=NO_ID );

private:
	SORTBUTTON*	m_pSortButton_Total;
	SORTBUTTON*	m_pSortButton_SM;
	SORTBUTTON*	m_pSortButton_HA;
	SORTBUTTON*	m_pSortButton_BH;

	RnCDMRankList* m_pRanking;
	CBasicTextBox* m_pText_Time;

	CBasicTextButton* m_pCloseButton;
	CUIStringListBox* m_pListbox;

protected:
	GLGaeaClient* m_pGaeaClient;
	std::vector<RNCDM_RANK_SLOT>	m_vecRnCdmRankSlot;

	void SetRankSloatData(RNCDM_RANK_SLOT vecRnCdmRankSlot);
	void RankListUpdate();

};

class MyRnCDMRankWindow  : public IRnCDMRankWindow, private RnCDMRankWindow
{
public:
	MyRnCDMRankWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~MyRnCDMRankWindow() {};

public:
	virtual void CreateUIWindowAndRegisterOwnership();
	virtual void Show();
	virtual void Update();
	virtual void SetRankSloatData(RNCDM_RANK_SLOT vecRnCdmRankSlot);
	virtual RNCDM_RANK_SLOT GetRankSlotData(int nIndex);
	virtual void RankListUpdate();
};
