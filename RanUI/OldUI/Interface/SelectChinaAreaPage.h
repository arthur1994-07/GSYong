#pragma	once

#include "../Util/UIOuterWindow.h"

class COuterInterface;
class GLGaeaClient;

class CSelectChinaAreaPage : public CUIOuterWindow
{
	enum
	{
		SELECT_AREA_QUIT = ET_CONTROL_NEXT,
		SELECT_AREA_BUTTON_00,
		SELECT_AREA_BUTTON_01,
		SELECT_AREA_BUTTON_02
//		SELECT_AREA_BUTTON_03,
//		SELECT_AREA_BUTTON_04,
//		SELECT_AREA_BUTTON_05 // �߱����� �߰�
	};

public:
	CSelectChinaAreaPage(GLGaeaClient* pGaeaClient, COuterInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CSelectChinaAreaPage ();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void CreateSubControl ();

public:
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

private:
	void SetParamChinaRegion( INT nIndex );
	void ConnectLoginServer();
};