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
    	페이지가 활성화 될때 호출 된다.
        우리는 활성화 될 때 최신 정보로 갱신 한다.

     */
	virtual void BeginSubControl() { Update(); }

protected:
    /**
    	PVE 인던 리스트 박스의 인던을 선택 한다.

     */
	void                        SetCurSel(int nIndex);

    /**
        페이지 정보를 설정 한다.

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
        PVE 인던 리스트 박스의 아이템을 추가 한다.

     */
	void SetInstanceMap(const TCHAR* pTitle, const std::string strEnterNum, int nLevel, bool bPossible, _TIME_TYPE sec, int nIndex = -1);

    /**
        입력되는 맵ID를 가지는 맵을 추출 한다.

     */
	bool FindTable(const SNATIVEID mid, const int nLevel, MAP_TABLE*& pTable);
	bool FindTable(const DWORD dwChrNum, const SNATIVEID mid, const int nLevel, MAP_TABLE*& pTable);

    /**
        현재 리스트박스 에서 선택된 맵 정보를 추출 한다.

     */
	bool GetSelCurInfo(TriggerMapInfo*& info, int& level);
	bool GetSelCurTable(MAP_TABLE*& pTable);

    /**
    	내부 자료 구조를 외부와 동기화 한다.

     */
	void UpdateSelCur();
	void UpdateListBox();

    /**
        외부 인터페이스

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
		PVE 로직 처리기

	 */
	GLPVEClient*		m_pLogic;

	CBasicTextBox* CreateStaticControl(const char* szConatrolKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID = NO_ID);
};
