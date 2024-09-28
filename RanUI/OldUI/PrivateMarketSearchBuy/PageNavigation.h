#pragma once
#include "../../InnerInterface.h"
#include "../Util/UIWindowEx.h"
#include <map>

class CBasicLineBoxSmart;
class CBasicTextBoxExLink;
class GLGaeaClient;
class CCheckBox;
class RnButton;

const int MAX_PAGE = 10; // 한단위에 표시되는 최대 페이지수 ex) 1 2 3 4 5 6 7 8 9 10

struct INDEXNUMBER
{
	std::string strText;
	int nNumber;
};

class Pagenavigation : public CUIWindowEx
{
private:
	enum
	{
		ID_ITEM_IMAGE = ET_CONTROL_NEXT + 1,
		ID_ITEM_NAME,
		ID_ISUSE,
		ID_SELLER_NAME,
		ID_SELLER_POS,
		ID_ITEM_COST,
		ID_ITEM_COUNT,

		ID_INDEX_LEFT_BUTTON,
		ID_INDEX_RIGHT_BUTTON,
		ID_INDEX_LEFT_JUMP_BUTTON,
		ID_INDEX_RIGHT_JUMP_BUTTON,

		ID_INDEX_1,
		ID_INDEX_2,
		ID_INDEX_3,
		ID_INDEX_4,
		ID_INDEX_5,
		ID_INDEX_6,
		ID_INDEX_7,
		ID_INDEX_8,
		ID_INDEX_9,
		ID_INDEX_10,
		ID_INDEX_START = ID_INDEX_1,
		ID_INDEX_END   = ID_INDEX_10,

		START_INDEX = ID_INDEX_1 - ID_INDEX_START,
		END_INDEX = ID_INDEX_END - ID_INDEX_START,

	};

public:
	Pagenavigation( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	~Pagenavigation();
	void	            CreateSubControl();
	virtual void        Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void        TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	void                LoadItemInfo();
	void				SetCheckBoxVisible( BOOL bBool );
	void				SortTextBox( int nIndex = 1 );
	void				SortTapButton( int nIndex = 1 );
	void				PagenavigationUpdate( int nIndex = 1 );
	void				ClickIndex( UIGUID ControlID );
	int					GetIndex();
	int					GetPageIndex();
	BOOL				IsClickEvent(); //페이지네비게이션에서 이벤트가 발생하면 알려준다
	void				SetPage(int nPage);
	void				SetCountPage(float fCountPage);
	//void				SortTextBox();

private:
	UIRECT				m_StarBoxPox;
	GLGaeaClient*       m_pGaeaClient;
	CInnerInterface*    m_pInterface;

	CUIControl*			m_pITEM_IMAGE;
	std::vector<CBasicTextBoxExLink*> m_pPageNumber;
	CCheckBox*			m_pCheckBox;
	float				m_fWaiteTime;
	int					m_count;
	int					m_nIndex;
	int					m_nCountBox; 
	float				m_fCountPage;
	std::map<CString, int>	m_mapIndex;

	RnButton*			m_pLeftTapButton;
	RnButton*			m_pRightTapButton;
	RnButton*			m_pLeftTapButtonEnd;
	RnButton*			m_pRightTapButtonEnd;

	BOOL				m_IsClickEnevt;

};
