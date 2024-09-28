#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class GLGaeaClient;
class CInnerInterface;

class CSelectClubListField;

class CSelectClubList : public CUIGroup
{
private:
    enum
    {

        BACK = NO_ID + 1,
        NAME_FIELD = BACK + 1,
        RANK_FIELD = BACK + 2,
        LEVEL_FIELD = BACK + 3,
        MASTER_FIELD = BACK + 4,
        MEMBER_FIELD = BACK + 5,
    };

public:
    CSelectClubList(GLGaeaClient* pGaeaClient, COuterInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual	~CSelectClubList();

protected:
    GLGaeaClient* m_pGaeaClient;
    COuterInterface* m_pInterface;

public:
    virtual	void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

public:
    void CreateSubControl();

    void SetClubListOrder( wb::search::EM_ORDER_TYPE Type, wb::search::EM_ORDER_ASC_DSC Order );
    void SearchClub();

    void SelectClubList( int nIndex );

    const std::vector<wb::CLUB_INFO_BASE>& GetClubList();

    void SetSearchName( std::string& strName );

private:
    CSelectClubListField* m_pNameField;
    CSelectClubListField* m_pRankField;
    CSelectClubListField* m_pLevelField;
    CSelectClubListField* m_pMasterField;
    CSelectClubListField* m_pMemberField;
    CBasicTextBox* m_pClubListText;
    CBasicScrollBarEx* m_pListScroll;

private:
    std::vector<wb::CLUB_INFO_BASE> m_vecClubInfo;
    std::string strSearchName;
    wb::search::EM_ORDER_TYPE m_OrderType;
    wb::search::EM_ORDER_ASC_DSC m_OrderASC_DSC;
};