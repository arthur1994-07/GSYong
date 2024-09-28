#pragma once

#include "ColorPropertyDefine.h"


namespace MiniGame
{	
	class ColorBox : public CUIGroupHelper
	{
		enum
		{
			COLOR_BOX_CONTROL = NO_ID + 1,
		};
	public:
		void CreateSubControl(void);
		void SetImage(const char* szControlKeyword);
		void SetDiffuseColor(const D3DCOLOR& _diffuseColor);
		const DWORD GetDiffuseColor(void) const;

	public:
		ColorBox(EngineDeviceMan* pEngineDevice);
		virtual ~ColorBox(void);

	private:
		// need name Box here;
		CUIControl* pControl;
		CSwapImage* pSwapImage;
	};

	

	class ColorLight : public CUIGroupHelper
	{
		enum
		{
			COLOR_LIGHT_NONE = NO_ID,
			COLOR_LIGHT_DIFFUSE,
			COLOR_LIGHT_AMBIENT,
			COLOR_LIGHT_SPECULAR,

			COLOR_LIGHT_VECTOR_X,
			COLOR_LIGHT_VECTOR_Y,
			COLOR_LIGHT_VECTOR_Z,
		};
	public:
		virtual void TranslateUIMessage(UIGUID controlID, DWORD dwMessage) override;
		virtual void Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl) override;
		void CreateSubControl(const DWORD _index);
		void SetColor(const DWORD _color);
		void bindLightProperty(D3DLIGHT9* _pLight);

		const bool isValidColor(void) const;
		const DWORD GetColor(void) const;

	public:
		ColorLight(EngineDeviceMan* _pEngineDevice);
		virtual ~ColorLight(void);

	private:
		CBasicTextBox* pTitleText;

		CBasicTextBox* pColorText[3];
		CBasicTextBox* pValueText[3];
		ColorBox* pColorBox[3];

		CBasicTextBox* pDirectText[3];
		CBasicLineBoxSmart* pLineBoxDirect[3];
		CUIEditBox* pEditBoxDirect[3];

		DWORD dwSelected;
		D3DXVECTOR3 directLight;
		D3DLIGHT9* pLight;
	};

	class ColorMaterial : public CUIGroupHelper
	{
		enum
		{
			COLOR_MATERIAL_NONE = NO_ID,
			COLOR_MATERIAL_DIFFUSE,
			COLOR_MATERIAL_AMBIENT,
			COLOR_MATERIAL_SPECULAR,
			COLOR_MATERIAL_EMISSIVE,
			COLOR_MATERIAL_SPECULAR_POWER,
		};
	public:
		virtual void TranslateUIMessage(UIGUID controlID, DWORD dwMessage) override;
		virtual void Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl) override;
		void CreateSubControl(void);
		void SetColor(const DWORD _color);		
		void bindMaterialProperty(D3DMATERIAL9* _pMaterial);

		const bool isValidColor(void) const;
		const DWORD GetColor(void) const;

	public:
		ColorMaterial(EngineDeviceMan* _pEngineDevice);
		virtual ~ColorMaterial(void);

	private:
		CBasicTextBox* pTitleText;

		CBasicTextBox* pValueText[4];
		CBasicTextBox* pColorText[4];
		ColorBox* pColorBox[4];

		CBasicTextBox* pPowerText;
		CBasicLineBoxSmart* pLineBoxPower;
		CUIEditBox* pEditBoxPower;

		DWORD dwSelected;
		D3DMATERIAL9* pMaterial;
	};

	class ColorProperty : public IColorProperty, public CUIWindowEx
	{		
	public:	
		virtual void Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl) override;
		virtual void TranslateUIMessage(UIGUID controlID, DWORD dwMessage) override;
		virtual void CreateUIWindowAndRegisterOwnership(void) override;	
		virtual void bindLightProperty(D3DLIGHT9* _pLight, const DWORD _index = 0) override;
		virtual void bindMaterialProperty(D3DMATERIAL9* _pMaterail) override;
		virtual void resetProperty(void) override;		

	public:
		ColorProperty(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
		virtual ~ColorProperty();

	protected:
		enum
		{
			COLOR_PROPERTY_NONE = ET_CONTROL_NEXT,
			COLOR_PROPERTY_LIGHT_0,
			COLOR_PROPERTY_LIGHT_1,
			COLOR_PROPERTY_LIGHT_2,

			COLOR_PROPERTY_MATERIAL,

			COLOR_PROPERTY_PICKER,

			COLOR_PROPERTY_BUTTON_OK,
			COLOR_PROPERTY_BUTTON_CANCEL,
		};
	private:
		GLGaeaClient* pGaeaClient;
		ColorLight* pColorLight[3];
		ColorMaterial* pMaterial;

		DWORD dwSelected;

		CUIColorPickerGroup* pColorPicker;

		CUITextButton* pButtonCancel;
		CUITextButton* pButtonOK;
	};

}

