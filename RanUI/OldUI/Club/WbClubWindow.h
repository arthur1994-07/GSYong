#pragma	once

#include "../Util/UIWindowEx.h"
#include "../Util/TapSelectorEx.h"

class CInnerInterface;
class GLGaeaClient;
class CWbClubBasicInfoTab;
class CWbClubManagementTab;
class CWbClubStorageTab;

class CWbClubWindow : public CUIWindowEx, private CTapSelectorEx<DEFAULT_TAPBUTTON>
{
private:
    typedef CTapSelectorEx::BUTTON STAPBUTTON;

    enum
    {
        BACK = ET_CONTROL_NEXT,
        CLUB_BASICINFO_TAB,
        CLUB_ACTIVITY_TAB,
        CLUB_STORAGE_TAB,
        CLUB_MANAGEMENT_TAB,
        CLUB_BASICINFO_TAB_WINDOW,
        CLUB_ACTIVITY_TAB_WINDOW,
        CLUB_STORAGE_TAB_WINDOW,
        CLUB_MANAGEMENT_TAB_WINDOW,
    };

public:
    CWbClubWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual ~CWbClubWindow();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
    void CreateSubControl();

    CUIGroup* GetTapWindow(UIGUID ControlID);

    void LoadClubMemberList();
    void RefreshState();
    void RefreshGuidanceMap();
    const DWORD GetClubMemberID();
	const DWORD GetClubAllianceID();
    void ShowGradePopup();
    void ShowNotice();
    void ShowClubLog();
    void LoadClubAllianceList();
    void LoadClubBattleList();
    void ClubStorageUpdateMoney();
    void ClubUpdateNewbieNotice();

public:
    virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
    virtual void SetVisibleSingle( BOOL isVisible );
    virtual void EventSelectedTap(UIGUID ControlID) override;

private:
    int m_nPage;
    CUIGroup* m_pCurrentTap;

    CWbClubBasicInfoTab* m_pClubBasicInfoTab;
    CWbClubManagementTab* m_pClubManagementTab;
    CWbClubStorageTab* m_pClubStorageTab;

    CBasicLineBoxSmart* m_pNeedRegistClubBack;
    CUIControl* m_pNeedRegistClubLogo;
    CBasicTextBox* m_pNeedRegistText;
};








class MyWbClubWindow : public IClubWindow, private CWbClubWindow
{
public:
    MyWbClubWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );

public:
    virtual void CreateUIWindowAndRegisterOwnership();

    virtual void RefreshClubList() override;
    virtual void RefreshGuidanceMap() override;
    virtual void RefreshState() override;
    virtual const DWORD GetClubMemberID() override;
    virtual void LoadClubAllianceList() override;
    virtual void LoadClubBattleList() override;
    virtual void SetAnnouncement() override;
    virtual const DWORD GetBattleClubID() override { return 0; };
    virtual const DWORD GetUnionMemberID() override;
    virtual const UI::String GetClubMemberName() override { return ""; };
    virtual void ShowGradePopup() override;
    virtual void ShowClubLog() override;
    virtual void ClubStorageUpdateMoney() override;
    virtual void ClubUpdateNewbieNotice() override;
};