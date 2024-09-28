#pragma once

#include "RnCompetitionObject.h"
#include "../Util/UIPage.h"
#include "../../../RanLogic/GLCompetition.h"
#include "../../../RanLogic/Character/GLCharDefine.h"
#include <boost/function.hpp>

#include "CompetitionPage.h"
#include "../Util/UIPage.h"
#include "../../../RanLogic/GLCompetition.h"
#include "../../../RanLogic/Character/GLCharDefine.h"
#include "../../../RanLogic/PVE/MapTable.h"
#include "../../../RanLogicClient/TriggerSystem/ClientMapInfoMessage.h"

//class CUIListBox;
class CSwapImage;
class GLGaeaClient;
class CBasicTextBox;
class CBasicTextButton;
class CBasicTextBoxEx;
class CBasicScrollBarEx;
class CInnerInterface;
class EngineDeviceMan;
class GLPVEClient;

typedef TriggerSystem::SClientTriggerMapInfo TriggerMapInfo;
typedef TriggerSystem::TriggerMapInfoList TriggerMapInfoList;

class RnWailBuinding : public RnCompetitionObject
{
	enum
	{
		CONTROL_ID_NONE = RNCOMPETITION_NEXT,
		CONTROL_ID_BUTTON_ENTRANCE,
		CONTROL_ID_BUTTON_CLEAROFBELONGING,
		CONTROL_ID_LISTBOX_INSTANCEMAP,
	};
public:
	RnWailBuinding(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	~RnWailBuinding();

	void Init();
	virtual void CreateSubControl();
	virtual void Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl);
	virtual	void TranslateUIMessage(UIGUID ControlID, DWORD dwMsg);
	void Update();
	void OnEntrance();
	void OnClearOfBelonging();

    /**
    	�������� Ȱ��ȭ �ɶ� ȣ�� �ȴ�.
        �츮�� Ȱ��ȭ �� �� �ֽ� ������ ���� �Ѵ�.

     */
	virtual void BeginSubControl() { Update(); }

protected:
    /**
    	PVE �δ� ����Ʈ �ڽ��� �δ��� ���� �Ѵ�.

     */
	void                        SetCurSel(int nIndex);

    /**
        ������ ������ ���� �Ѵ�.

     */
    void SetTitle(const TCHAR* pTitle);
	void SetMapObject(const TCHAR* pTitle);
    void SetMapInfo(const TCHAR* pMapInfo);
    void SetLevel(int nMin, int nMax);
    void SetPerson(int nNum);
    void SetNumber(int nNum);
    void SetRemNumber(int nNum);
    void SetMapImage(SNATIVEID mid);
    void SetReward(const TCHAR* pReward);

    /**
        PVE �δ� ����Ʈ �ڽ��� �������� �߰� �Ѵ�.

     */
	void SetInstanceMap(const TCHAR* pTitle, const std::string strEnterNum, int nLevel, bool bPossible, _TIME_TYPE sec, int nIndex = -1);

    /**
        �ԷµǴ� ��ID�� ������ ���� ���� �Ѵ�.

     */
	bool FindTable(const SNATIVEID mid, const int nLevel, MAP_TABLE*& pTable);
	bool FindTable(const DWORD dwChrNum, const SNATIVEID mid, const int nLevel, MAP_TABLE*& pTable);

    /**
        ���� ����Ʈ�ڽ� ���� ���õ� �� ������ ���� �Ѵ�.

     */
	bool GetSelCurInfo(TriggerMapInfo*& info, int& level);
	bool GetSelCurTable(MAP_TABLE*& pTable);

    /**
    	���� �ڷ� ������ �ܺο� ����ȭ �Ѵ�.

     */
	void UpdateSelCur();
	void UpdateListBox();

    /**
        �ܺ� �������̽�

     */
	CInnerInterface*	m_pInterface;
	GLGaeaClient*	m_pGaeaClient;

    /**
        UI 

     */
	CBasicTextButton*	m_pEntranceButton;
	CBasicTextButton*	m_pClearOfBelongingButton;
	CBasicTextBox*		m_pText_Title;
	CBasicTextBox*		m_pText_Info;
	CBasicTextBox*		m_pListbox_Title;
	//CUIListBox*			m_pListbox;
	//CSwapImage*			m_pMap;

	/**
		PVE ���� ó����

	 */
	GLPVEClient*		m_pLogic;

	CBasicTextBox* CreateStaticControl(const char* szConatrolKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID = NO_ID);
};
