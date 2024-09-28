#pragma	once

#include "../Util/UIWindowEx.h"
#include "../Util/TapSelectorEx.h"

#include "RnCharacterWindowInterface.h"

class RnCharacterAdditionalWindow;
class RnCharacterInfoTap;
class RnVehicleInfoTap;
class RnPetInfoTap;

class GLGaeaClient;
class CBasicVarTextBox;


class RnCharacterWindow : public CUIWindowEx , private CTapSelectorEx<DEFAULT_TAPBUTTON> ,
	public RnCharacterWindowInterface
{
public:
	typedef CTapSelectorEx::BUTTON STAPBUTTON;

	enum
	{
		CHARACTER_INFO_TAP = ET_CONTROL_NEXT + 1,
		VEHICLE_INFO_TAP,
		PET_INFO_TAP,

		CHARACTER_INFO_TAP_WINDOW, 
		VEHICLE_INFO_TAP_WINDOW, 
		PET_INFO_TAP_WINDOW, 
	};

	RnCharacterWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);

	void CreateSubControl();

	virtual void DeleteUIMeshData();

	virtual	void UpdateTitleData();

	virtual	void SetArmSwapTabButton();
	
	// �̰� â�� ������  ���ο� ĳ���� �� Ŭ������ �ٽ� �ε������ �ϱ� ������ 
	virtual	void SetVisibleSingle( BOOL bVisible );

	virtual void SetStorageItemToVehicleTapWindow(const std::vector<DWORD>& vehicleIDVec);

private:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

	virtual void SetFocusControl();
	virtual void ResetFocusControl();
		  
	CUIGroup* GetTapWindow(UIGUID controlID);
	void      EventSelectedTap(UIGUID controlID);

	virtual void SetVisibleCharacterAdditionalWindow( bool bVisible );
private:
	RnCharacterAdditionalWindow* m_pCharacterAdditionalAbility;

	RnCharacterInfoTap* m_pCharacterInfoTap;
	RnVehicleInfoTap*   m_pVehicleInfoTap;
	RnPetInfoTap*		m_pPetInfoTap;

	CUIGroup*			m_pCurrentTap;

	GLGaeaClient*       m_pGaeaClient;
	CInnerInterface*    m_pInterface;

	UIRECT	m_rcBasicPos;
	UIRECT	m_rcChangePos;	// �߰� �ɷ�â ǥ��� ���û�����
};





