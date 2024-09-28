#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class CInnerInterface;
class GLGaeaClient;

class CWbClubStoragePage;
class CWbClubStorageUseRecordPage;
class CWbClubStorageInputEdit;

class CWbClubStorageTab : public CUIGroup
{
private:
    static	const	int nMAX_STORAGE_CHANNEL = 6;

private:
    enum
    {
        BACK = NO_ID + 1,
        STORAGE_PAGE_START = BACK + 1,
        STORAGE_PAGE_END = STORAGE_PAGE_START + nMAX_STORAGE_CHANNEL - 1,
        PROFITS_BTN = STORAGE_PAGE_END + 1,
        DEPOSIT_BTN = STORAGE_PAGE_END + 2,
        WITHDRAW_BTN = STORAGE_PAGE_END + 3,
        INPUT_EIDT = STORAGE_PAGE_END + 4,
    };

public:
    CWbClubStorageTab(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual ~CWbClubStorageTab();

private:
    GLGaeaClient* m_pGaeaClient;
    CInnerInterface* m_pInterface;

public:
    void CreateSubControl();

    void ClubStorageUpdateMoney();
    void ClubStorageUpdateLog();

private:
    void Refresh();
    void ClearInfo();

public:
    virtual	void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
    virtual void SetVisibleSingle( BOOL bVisible );

private:
    CWbClubStoragePage* m_pPage[nMAX_STORAGE_CHANNEL];
    CWbClubStorageUseRecordPage* m_pStorageUseRecordPage;

    CBasicTextBox* m_pProfitsText;
    RnButton* m_pProfitsBtn;

    CBasicTextBox* m_pKeepMoneyText;
    RnButton* m_pDepositBtn;
    RnButton* m_pWithDrawBtn;

    CWbClubStorageInputEdit* m_pInputEdit;
};