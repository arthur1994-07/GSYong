#include "StdAfx.h"

#include "../../Util/UIWindowObject.h"
#include "../../Util/UIPageFrame.h"
#include "../../Util/UIPage.h"
#include "../../Util/UIColorPannelPage.h"
#include "../../Util/UIColorPickerGroup.h"
#include "../../Util/UITextButton.h"

#include "../../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../../EngineLib/GUInterface/UIEditBox.h"
#include "../../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../../EngineLib/DeviceMan.h"

#include "../../../../RanLogicClient/GLGaeaClient.h"

#include "ColorProperty.h"

namespace MiniGame
{
	ColorBox::ColorBox(EngineDeviceMan* _pEngineDevice)
		: CUIGroupHelper(_pEngineDevice)
	{
	}
	ColorBox::~ColorBox(void)
	{
	}

	void ColorBox::CreateSubControl(void)
	{
		this->pControl = new CUIControl(m_pEngineDevice);
		this->pControl->CreateSub(this, "COLOR_BOX_BACKGROUND", UI_FLAG_DEFAULT, COLOR_BOX_CONTROL);
		this->pControl->SetUseRender(TRUE);
		this->pControl->SetDiffuse(D3DCOLOR_RGBA(255, 0, 0, 255));
		this->pControl->SetVisibleSingle(TRUE);
		RegisterControl(this->pControl);

		this->pSwapImage = new CSwapImage(m_pEngineDevice);
		this->pSwapImage->CreateSub(this, "COLOR_BOX", UI_FLAG_DEFAULT);
		this->pSwapImage->SetImage("COLORPANNEL_PAGE_INACTIVE_0");
		this->pSwapImage->SetVisibleSingle(TRUE);
		RegisterControl(this->pSwapImage);
	}

	void ColorBox::SetImage(const char* szControlKeyword)
	{
		this->pSwapImage->SetImage(szControlKeyword);		
	}	

	void ColorBox::SetDiffuseColor(const D3DCOLOR& _diffuseColor)
	{		
		this->pControl->SetDiffuse(_diffuseColor);
	}

	const DWORD ColorBox::GetDiffuseColor(void) const
	{
		return this->pControl->GetDiffuse();
	}
	

		



	const D3DCOLOR D3DXCOLOR2D3DCOLOR(const D3DXCOLOR& _d3dxColor)
	{
		const DWORD _alpha(DWORD(_d3dxColor.a * 255.0f));
		const DWORD _red(DWORD(_d3dxColor.r * 255.0f));
		const DWORD _green(DWORD(_d3dxColor.g * 255.0f));
		const DWORD _blue(DWORD(_d3dxColor.b * 255.0f));
		return (_alpha << 24) + (_red << 16) + (_green << 8) + (_blue << 0);
	}
	const D3DXCOLOR D3DCOLOR2D3DXCOLOR(const D3DCOLOR& _d3dColor)
	{
		const float _fFactor(1.0f / 255.0f);
		const float _alpha(float((_d3dColor >> 24) & 0xff) * _fFactor);
		const float _red(float((_d3dColor >> 16) & 0xff) * _fFactor);
		const float _green(float((_d3dColor >> 8) & 0xff) * _fFactor);
		const float _blue(float((_d3dColor >> 0) & 0xff) * _fFactor);

		return D3DXCOLOR(_red, _green, _blue, _alpha);
	}

	ColorLight::ColorLight(EngineDeviceMan* _pEngineDevice)
		: CUIGroupHelper(_pEngineDevice)
		, pLight(0)
		, dwSelected(-1)
	{		
	}
	ColorLight::~ColorLight(void)
	{
	}
	void ColorLight::CreateSubControl(const DWORD _index)
	{
		CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
		CD3DFontPar* pFont11 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 11, _DEFAULT_FONT_FLAG );

		{ // text Box;
			CBasicTextBox* _pTextBox(NULL);
			
			_pTextBox = new CBasicTextBox(m_pEngineDevice);
			_pTextBox->CreateSub(this, "COLOR_TEXT_TITLE", UI_FLAG_DEFAULT);
			_pTextBox->SetFont(pFont11);
			_pTextBox->SetTextAlign ( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );	
			_pTextBox->SetText(sc::string::format("[Light %1%]", _index).c_str(), NS_UITEXTCOLOR::ORANGE);
			RegisterControl(_pTextBox);
			this->pTitleText = _pTextBox;




			_pTextBox = new CBasicTextBox(m_pEngineDevice);
			_pTextBox->CreateSub ( this, "COLOR_TEXT_DIFFUSE", UI_FLAG_DEFAULT );
			_pTextBox->SetFont ( pFont9 );
			_pTextBox->SetTextAlign ( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );	
			_pTextBox->SetText("Diffuse :", NS_UITEXTCOLOR::GOLD);
			RegisterControl(_pTextBox);
			this->pColorText[0] = _pTextBox;
			_pTextBox = new CBasicTextBox(m_pEngineDevice);
			_pTextBox->CreateSub ( this, "COLOR_TEXT_DIFFUSE_VALUE", UI_FLAG_DEFAULT );
			_pTextBox->SetFont ( pFont9 );
			_pTextBox->SetTextAlign ( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );	
			_pTextBox->SetText("0/0/0", NS_UITEXTCOLOR::WHITE);
			RegisterControl(_pTextBox);
			this->pValueText[0] = _pTextBox;

			_pTextBox = new CBasicTextBox(m_pEngineDevice);
			_pTextBox->CreateSub(this, "COLOR_TEXT_AMBIENT", UI_FLAG_DEFAULT);
			_pTextBox->SetFont(pFont9);
			_pTextBox->SetTextAlign(TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y);
			_pTextBox->SetText("Ambient :", NS_UITEXTCOLOR::GOLD);
			RegisterControl(_pTextBox);
			this->pColorText[1] = _pTextBox;
			_pTextBox = new CBasicTextBox(m_pEngineDevice);
			_pTextBox->CreateSub ( this, "COLOR_TEXT_AMBIENT_VALUE", UI_FLAG_DEFAULT );
			_pTextBox->SetFont ( pFont9 );
			_pTextBox->SetTextAlign ( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );	
			_pTextBox->SetText("0/0/0", NS_UITEXTCOLOR::WHITE);
			RegisterControl(_pTextBox);
			this->pValueText[1] = _pTextBox;

			_pTextBox = new CBasicTextBox(m_pEngineDevice);
			_pTextBox->CreateSub(this, "COLOR_TEXT_SPECULAR", UI_FLAG_DEFAULT);
			_pTextBox->SetFont ( pFont9 );
			_pTextBox->SetTextAlign ( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );	
			_pTextBox->SetText("Specular:", NS_UITEXTCOLOR::GOLD);
			RegisterControl(_pTextBox);
			this->pColorText[2] = _pTextBox;
			_pTextBox = new CBasicTextBox(m_pEngineDevice);
			_pTextBox->CreateSub ( this, "COLOR_TEXT_SPECULAR_VALUE", UI_FLAG_DEFAULT );
			_pTextBox->SetFont ( pFont9 );
			_pTextBox->SetTextAlign ( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );	
			_pTextBox->SetText("0/0/0", NS_UITEXTCOLOR::WHITE);
			RegisterControl(_pTextBox);
			this->pValueText[2] = _pTextBox;




			_pTextBox = new CBasicTextBox(m_pEngineDevice);
			_pTextBox->CreateSub(this, "COLOR_TEXT_DIRECT_X", UI_FLAG_DEFAULT);
			_pTextBox->SetFont(pFont9);
			_pTextBox->SetTextAlign(TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y);
			_pTextBox->SetText("Direct X :", NS_UITEXTCOLOR::GOLD);
			RegisterControl(_pTextBox);
			this->pDirectText[0] = _pTextBox;

			_pTextBox = new CBasicTextBox(m_pEngineDevice);
			_pTextBox->CreateSub(this, "COLOR_TEXT_DIRECT_Y", UI_FLAG_DEFAULT);
			_pTextBox->SetFont(pFont9);
			_pTextBox->SetTextAlign(TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y);
			_pTextBox->SetText("Y :", NS_UITEXTCOLOR::GOLD);
			RegisterControl(_pTextBox);
			this->pDirectText[0] = _pTextBox;

			_pTextBox = new CBasicTextBox(m_pEngineDevice);
			_pTextBox->CreateSub ( this, "COLOR_TEXT_DIRECT_Z", UI_FLAG_DEFAULT );
			_pTextBox->SetFont(pFont9);
			_pTextBox->SetTextAlign(TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y);
			_pTextBox->SetText("Z :", NS_UITEXTCOLOR::GOLD);
			RegisterControl(_pTextBox);
			this->pDirectText[2] = _pTextBox;
		}

		{ // diffuse/ambient/specular color Box;
			ColorBox* _pColorBox(0);

			_pColorBox = new ColorBox(m_pEngineDevice);
			_pColorBox->CreateSub(this, "COLOR_BOX_DIFFUSE", UI_FLAG_DEFAULT, COLOR_LIGHT_DIFFUSE);
			_pColorBox->CreateSubControl();
			_pColorBox->SetVisibleSingle(TRUE);
			RegisterControl(_pColorBox);
			this->pColorBox[0] = _pColorBox;

			_pColorBox = new ColorBox(m_pEngineDevice);
			_pColorBox->CreateSub(this, "COLOR_BOX_AMBIENT", UI_FLAG_DEFAULT, COLOR_LIGHT_AMBIENT);
			_pColorBox->CreateSubControl();
			_pColorBox->SetVisibleSingle(TRUE);
			RegisterControl(_pColorBox);
			this->pColorBox[1] = _pColorBox;

			_pColorBox = new ColorBox(m_pEngineDevice);
			_pColorBox->CreateSub(this, "COLOR_BOX_SPECULAR", UI_FLAG_DEFAULT, COLOR_LIGHT_SPECULAR);
			_pColorBox->CreateSubControl();
			_pColorBox->SetVisibleSingle(TRUE);
			RegisterControl(_pColorBox);
			this->pColorBox[2] = _pColorBox;
		}		

		{ // light direct edit Box;
			CBasicLineBoxSmart* _pLineBox(0);
			CUIEditBox* _pEditBox(0);

			_pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
			_pLineBox->CreateSub(this, "COLOR_LIGHT_LINE_BOX_DIRECT_X", UI_FLAG_XSIZE | UI_FLAG_YSIZE);
			_pLineBox->CreateSubControl("COLORPICKER_COLORINFO_EDIT_LINEBOX_TEXINFO");
			RegisterControl (_pLineBox);
			this->pLineBoxDirect[0] = _pLineBox;

			_pEditBox = new CUIEditBox(m_pEngineDevice);
			_pEditBox->CreateSub(this, "COLOR_LIGHT_EDIT_BOX_DIRECT_X", UI_FLAG_XSIZE | UI_FLAG_BOTTOM, COLOR_LIGHT_VECTOR_X);
			_pEditBox->CreateCarrat("WRITE_NOTE_CARRAT", TRUE, UINT_MAX);
			_pEditBox->SetFont(pFont9);
			_pEditBox->SetLimitInput(6);
			_pEditBox->DoUSE_DECIMALNUMBER(true);
			_pEditBox->SetMinusSign(TRUE);
			_pEditBox->SetAlign(EDIT_RIGHT_ALIGN);
			_pEditBox->SetTextColor(NS_UITEXTCOLOR::GOLD);
			RegisterControl(_pEditBox);
			this->pEditBoxDirect[0] = _pEditBox;


			_pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
			_pLineBox->CreateSub(this, "COLOR_LIGHT_LINE_BOX_DIRECT_Y", UI_FLAG_XSIZE | UI_FLAG_YSIZE);
			_pLineBox->CreateSubControl("COLORPICKER_COLORINFO_EDIT_LINEBOX_TEXINFO");
			RegisterControl (_pLineBox);
			this->pLineBoxDirect[1] = _pLineBox;

			_pEditBox = new CUIEditBox(m_pEngineDevice);
			_pEditBox->CreateSub(this, "COLOR_LIGHT_EDIT_BOX_DIRECT_Y", UI_FLAG_XSIZE | UI_FLAG_BOTTOM, COLOR_LIGHT_VECTOR_Y);
			_pEditBox->CreateCarrat("WRITE_NOTE_CARRAT", TRUE, UINT_MAX);
			_pEditBox->SetFont(pFont9);
			_pEditBox->SetLimitInput(6);
			_pEditBox->DoUSE_DECIMALNUMBER(true);
			_pEditBox->SetMinusSign(TRUE);
			_pEditBox->SetAlign(EDIT_RIGHT_ALIGN);
			_pEditBox->SetTextColor(NS_UITEXTCOLOR::GOLD);
			RegisterControl(_pEditBox);
			this->pEditBoxDirect[1] = _pEditBox;


			_pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
			_pLineBox->CreateSub(this, "COLOR_LIGHT_LINE_BOX_DIRECT_Z", UI_FLAG_XSIZE | UI_FLAG_YSIZE);
			_pLineBox->CreateSubControl("COLORPICKER_COLORINFO_EDIT_LINEBOX_TEXINFO");
			RegisterControl (_pLineBox);
			this->pLineBoxDirect[2] = _pLineBox;

			_pEditBox = new CUIEditBox(m_pEngineDevice);
			_pEditBox->CreateSub(this, "COLOR_LIGHT_EDIT_BOX_DIRECT_Z", UI_FLAG_XSIZE | UI_FLAG_BOTTOM, COLOR_LIGHT_VECTOR_Z);
			_pEditBox->CreateCarrat("WRITE_NOTE_CARRAT", TRUE, UINT_MAX);
			_pEditBox->SetFont(pFont9);
			_pEditBox->SetLimitInput(6);
			_pEditBox->DoUSE_DECIMALNUMBER(true);
			_pEditBox->SetMinusSign(TRUE);
			_pEditBox->SetAlign(EDIT_RIGHT_ALIGN);
			_pEditBox->SetTextColor(NS_UITEXTCOLOR::GOLD);
			RegisterControl(_pEditBox);
			this->pEditBoxDirect[2] = _pEditBox;
		}
	}
	void ColorLight::TranslateUIMessage(UIGUID controlID, DWORD dwMessage)
	{
		CUIGroupHelper::TranslateUIMessage(controlID, dwMessage);

		switch ( controlID )
		{		
		case COLOR_LIGHT_DIFFUSE:
		case COLOR_LIGHT_AMBIENT:
		case COLOR_LIGHT_SPECULAR:
			if ( CHECK_MOUSEIN_LBUPLIKE(dwMessage) == TRUE )
			{
				const DWORD _dwID(controlID - COLOR_LIGHT_DIFFUSE);
				const DWORD _dwSelected(this->dwSelected);
				if ( (_dwSelected != -1) && (ColorLight::isValidColor() == true) )
					this->pColorBox[_dwSelected]->SetImage("COLORPANNEL_PAGE_INACTIVE_0");

				this->pColorBox[_dwID]->SetImage("COLORPANNEL_PAGE_ACTIVE_0");
				this->dwSelected = _dwID;
			}
			break;
		case COLOR_LIGHT_VECTOR_X:
		case COLOR_LIGHT_VECTOR_Y:
		case COLOR_LIGHT_VECTOR_Z:
			{
				const DWORD _index(controlID - COLOR_LIGHT_VECTOR_X);
				if ( CHECK_MOUSE_IN_LBDOWNLIKE(dwMessage) == TRUE )
				{
					this->pEditBoxDirect[_index]->EndEdit();
					this->pEditBoxDirect[_index]->BeginEdit();

					const DWORD _dwID(controlID - COLOR_LIGHT_DIFFUSE);
					this->dwSelected = _dwID;
				}
				else if ( CHECK_LB_DOWN_LIKE(dwMessage) == TRUE )
					this->pEditBoxDirect[_index]->EndEdit();
			}
			break;
		}
	}
	void ColorLight::Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl)
	{
		CUIGroupHelper::Update(x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl);

		if ( this->pLight != 0 )
		{
			float _fDirect[3] = {this->directLight.x, this->directLight.y, this->directLight.z};
			for ( DWORD _i(3); _i--; )
			{
				if ( this->pEditBoxDirect[_i]->IsBegin() )
				{
					const std::string _stringDirectX(this->pEditBoxDirect[_i]->GetEditString().GetString());
					const float _fDirectElement(float(::atof(_stringDirectX.c_str())));
					_fDirect[_i] = _fDirectElement;
				}
			}
			D3DXVECTOR3 _direct(_fDirect[0], _fDirect[1], _fDirect[2]);
			this->directLight = _direct;
			::D3DXVec3Normalize(&_direct, &_direct);
			this->pLight->Direction = _direct;
		}		
	}
	void ColorLight::SetColor(const DWORD _color)
	{
		const DWORD _dwSelected(this->dwSelected);
		const UIGUID _controlID(_dwSelected + COLOR_LIGHT_DIFFUSE);
		switch ( _controlID )
		{
		case COLOR_LIGHT_DIFFUSE:
			if ( this->pLight != 0 )
				this->pLight->Diffuse = D3DCOLOR2D3DXCOLOR(_color);
			this->pColorBox[_dwSelected]->SetDiffuseColor(D3DCOLOR(_color));
			this->pValueText[_dwSelected]->SetText(sc::string::format("R%1%/G%2%/B%3%", (_color >> 16) & 0xff, (_color >> 8) & 0xff, (_color >> 0) & 0xff).c_str(), NS_UITEXTCOLOR::WHITE);
			break;
		case COLOR_LIGHT_AMBIENT:
			if ( this->pLight != 0 )
				this->pLight->Ambient = D3DCOLOR2D3DXCOLOR(_color);
			this->pColorBox[_dwSelected]->SetDiffuseColor(D3DCOLOR(_color));
			this->pValueText[_dwSelected]->SetText(sc::string::format("R%1%/G%2%/B%3%", (_color >> 16) & 0xff, (_color >> 8) & 0xff, (_color >> 0) & 0xff).c_str(), NS_UITEXTCOLOR::WHITE);
			break;
		case COLOR_LIGHT_SPECULAR:
			if ( this->pLight != 0 )
				this->pLight->Specular = D3DCOLOR2D3DXCOLOR(_color);
			this->pColorBox[_dwSelected]->SetDiffuseColor(D3DCOLOR(_color));
			this->pValueText[_dwSelected]->SetText(sc::string::format("R%1%/G%2%/B%3%", (_color >> 16) & 0xff, (_color >> 8) & 0xff, (_color >> 0) & 0xff).c_str(), NS_UITEXTCOLOR::WHITE);
			break;		
		}
	}

	const bool ColorLight::isValidColor(void) const
	{
		const UIGUID _controlID(this->dwSelected + COLOR_LIGHT_DIFFUSE);
		switch ( _controlID )
		{
		case COLOR_LIGHT_DIFFUSE:
		case COLOR_LIGHT_AMBIENT:
		case COLOR_LIGHT_SPECULAR:
			return true;
		}

		return false;
	}
	const DWORD ColorLight::GetColor(void) const
	{
		const DWORD _dwSelected(this->dwSelected);
		if ( _dwSelected == -1 )
			return 0;

		return this->pColorBox[_dwSelected]->GetDiffuseColor();
	}
	void ColorLight::bindLightProperty(D3DLIGHT9* _pLight)
	{
		this->pLight = _pLight;		

		if ( _pLight == NULL )
			SetVisibleSingle(FALSE);
		else
		{
			const DWORD _colorArray[3] = 
			{
				D3DXCOLOR2D3DCOLOR(_pLight->Diffuse),
				D3DXCOLOR2D3DCOLOR(_pLight->Ambient),
				D3DXCOLOR2D3DCOLOR(_pLight->Specular)
			};
			const float _fDirection[3] = 
			{
				_pLight->Direction.x, _pLight->Direction.y, _pLight->Direction.z
			};
			for ( DWORD _i(3); _i--; )
			{
				const DWORD _color(_colorArray[_i]);
				this->pColorBox[_i]->SetDiffuseColor(_color);
				this->pValueText[_i]->SetText(sc::string::format("R%1%/G%2%/B%3%", (_color >> 16) & 0xff, (_color >> 8) & 0xff, (_color >> 0) & 0xff).c_str(), NS_UITEXTCOLOR::WHITE);
				this->pEditBoxDirect[_i]->SetEditString(sc::string::format("%1%", _fDirection[_i]).c_str());
				this->directLight = _pLight->Direction;
			}
			SetVisibleSingle(TRUE);
		}
	}
	
	



	ColorMaterial::ColorMaterial(EngineDeviceMan* _pEngineDevice)
		: CUIGroupHelper(_pEngineDevice)
		, pMaterial(0)
		, dwSelected(-1)		
	{
	}
	ColorMaterial::~ColorMaterial(void)
	{
	}
	void ColorMaterial::CreateSubControl(void)
	{
		CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
		CD3DFontPar* pFont11 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 11, _DEFAULT_FONT_FLAG );

		{ // text Box;
			CBasicTextBox* _pTextBox(NULL);

			_pTextBox = new CBasicTextBox(m_pEngineDevice);
			_pTextBox->CreateSub(this, "COLOR_TEXT_TITLE", UI_FLAG_DEFAULT);
			_pTextBox->SetFont(pFont11);
			_pTextBox->SetTextAlign ( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );	
			_pTextBox->SetText("[Material]", NS_UITEXTCOLOR::ORANGE);
			RegisterControl(_pTextBox);
			this->pTitleText = _pTextBox;



			_pTextBox = new CBasicTextBox(m_pEngineDevice);
			_pTextBox->CreateSub ( this, "COLOR_TEXT_DIFFUSE", UI_FLAG_DEFAULT );
			_pTextBox->SetFont ( pFont9 );
			_pTextBox->SetTextAlign ( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );	
			_pTextBox->SetText("Diffuse :", NS_UITEXTCOLOR::GOLD);
			RegisterControl(_pTextBox);
			this->pColorText[0] = _pTextBox;
			_pTextBox = new CBasicTextBox(m_pEngineDevice);
			_pTextBox->CreateSub ( this, "COLOR_TEXT_DIFFUSE_VALUE", UI_FLAG_DEFAULT );
			_pTextBox->SetFont ( pFont9 );
			_pTextBox->SetTextAlign ( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );	
			_pTextBox->SetText("0/0/0", NS_UITEXTCOLOR::WHITE);
			RegisterControl(_pTextBox);
			this->pValueText[0] = _pTextBox;

			_pTextBox = new CBasicTextBox(m_pEngineDevice);
			_pTextBox->CreateSub ( this, "COLOR_TEXT_AMBIENT", UI_FLAG_DEFAULT );
			_pTextBox->SetFont ( pFont9 );
			_pTextBox->SetTextAlign ( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );	
			_pTextBox->SetText("Ambient :", NS_UITEXTCOLOR::GOLD);
			RegisterControl(_pTextBox);
			this->pColorText[1] = _pTextBox;
			_pTextBox = new CBasicTextBox(m_pEngineDevice);
			_pTextBox->CreateSub ( this, "COLOR_TEXT_AMBIENT_VALUE", UI_FLAG_DEFAULT );
			_pTextBox->SetFont ( pFont9 );
			_pTextBox->SetTextAlign ( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );	
			_pTextBox->SetText("0/0/0", NS_UITEXTCOLOR::WHITE);
			RegisterControl(_pTextBox);
			this->pValueText[1] = _pTextBox;

			_pTextBox = new CBasicTextBox(m_pEngineDevice);
			_pTextBox->CreateSub ( this, "COLOR_TEXT_SPECULAR", UI_FLAG_DEFAULT );
			_pTextBox->SetFont ( pFont9 );
			_pTextBox->SetTextAlign ( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );	
			_pTextBox->SetText("Specular:", NS_UITEXTCOLOR::GOLD);
			RegisterControl(_pTextBox);
			this->pColorText[2] = _pTextBox;
			_pTextBox = new CBasicTextBox(m_pEngineDevice);
			_pTextBox->CreateSub ( this, "COLOR_TEXT_SPECULAR_VALUE", UI_FLAG_DEFAULT );
			_pTextBox->SetFont ( pFont9 );
			_pTextBox->SetTextAlign ( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );	
			_pTextBox->SetText("0/0/0", NS_UITEXTCOLOR::WHITE);
			RegisterControl(_pTextBox);
			this->pValueText[2] = _pTextBox;

			_pTextBox = new CBasicTextBox(m_pEngineDevice);
			_pTextBox->CreateSub ( this, "COLOR_TEXT_EMISSIVE", UI_FLAG_DEFAULT );
			_pTextBox->SetFont ( pFont9 );
			_pTextBox->SetTextAlign ( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );	
			_pTextBox->SetText("Emissive:", NS_UITEXTCOLOR::GOLD);
			RegisterControl(_pTextBox);
			this->pColorText[3] = _pTextBox;
			_pTextBox = new CBasicTextBox(m_pEngineDevice);
			_pTextBox->CreateSub ( this, "COLOR_TEXT_EMISSIVE_VALUE", UI_FLAG_DEFAULT );
			_pTextBox->SetFont ( pFont9 );
			_pTextBox->SetTextAlign ( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );	
			_pTextBox->SetText("0/0/0", NS_UITEXTCOLOR::WHITE);
			RegisterControl(_pTextBox);
			this->pValueText[3] = _pTextBox;



			_pTextBox = new CBasicTextBox(m_pEngineDevice);
			_pTextBox->CreateSub ( this, "COLOR_TEXT_SPECULAR_POWER", UI_FLAG_DEFAULT );
			_pTextBox->SetFont ( pFont9 );
			_pTextBox->SetTextAlign ( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );	
			_pTextBox->SetText("Specular Power :", NS_UITEXTCOLOR::GOLD);
			RegisterControl(_pTextBox);
			this->pPowerText = _pTextBox;
		}
		{ // Diffuse/Ambient/Specular/Emissive color Box;
			ColorBox* _pColorBox(NULL);
			_pColorBox = new ColorBox(m_pEngineDevice);
			_pColorBox->CreateSub(this, "COLOR_BOX_DIFFUSE", UI_FLAG_DEFAULT, COLOR_MATERIAL_DIFFUSE);
			_pColorBox->CreateSubControl();
			_pColorBox->SetVisibleSingle(TRUE);
			RegisterControl(_pColorBox);
			this->pColorBox[0] = _pColorBox;

			_pColorBox = new ColorBox(m_pEngineDevice);
			_pColorBox->CreateSub(this, "COLOR_BOX_AMBIENT", UI_FLAG_DEFAULT, COLOR_MATERIAL_AMBIENT);
			_pColorBox->CreateSubControl();
			_pColorBox->SetVisibleSingle(TRUE);
			RegisterControl(_pColorBox);
			this->pColorBox[1] = _pColorBox;

			_pColorBox = new ColorBox(m_pEngineDevice);
			_pColorBox->CreateSub(this, "COLOR_BOX_SPECULAR", UI_FLAG_DEFAULT, COLOR_MATERIAL_SPECULAR);
			_pColorBox->CreateSubControl();
			_pColorBox->SetVisibleSingle(TRUE);
			RegisterControl(_pColorBox);
			this->pColorBox[2] = _pColorBox;

			_pColorBox = new ColorBox(m_pEngineDevice);
			_pColorBox->CreateSub(this, "COLOR_BOX_EMISSIVE", UI_FLAG_DEFAULT, COLOR_MATERIAL_EMISSIVE);
			_pColorBox->CreateSubControl();
			_pColorBox->SetVisibleSingle(TRUE);
			RegisterControl(_pColorBox);
			this->pColorBox[3] = _pColorBox;
		}

		{ // specular Power edit Box;
			CBasicLineBoxSmart* _pLineBox(0);
			CUIEditBox* _pEditBox(0);

			_pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
			_pLineBox->CreateSub(this, "COLOR_MATERIAL_LINE_BOX_SPECULAR_POWER", UI_FLAG_XSIZE | UI_FLAG_YSIZE);
			_pLineBox->CreateSubControl("COLORPICKER_COLORINFO_EDIT_LINEBOX_TEXINFO");
			RegisterControl(_pLineBox);
			this->pLineBoxPower = _pLineBox;

			_pEditBox = new CUIEditBox(m_pEngineDevice);
			_pEditBox->CreateSub(this, "COLOR_MATERIAL_EDIT_BOX_SPECULAR_POWER", UI_FLAG_XSIZE | UI_FLAG_BOTTOM, COLOR_MATERIAL_SPECULAR_POWER);
			_pEditBox->CreateCarrat("WRITE_NOTE_CARRAT", TRUE, UINT_MAX);
			_pEditBox->SetFont(pFont9);
			_pEditBox->SetLimitInput(4);
			_pEditBox->DoUSE_DECIMALNUMBER(true);
			_pEditBox->SetMinusSign(FALSE);
			_pEditBox->SetAlign(EDIT_RIGHT_ALIGN);
			_pEditBox->SetTextColor(NS_UITEXTCOLOR::GOLD);
			RegisterControl(_pEditBox);
			this->pEditBoxPower = _pEditBox;
		}
	}
	void ColorMaterial::TranslateUIMessage(UIGUID controlID, DWORD dwMessage)
	{
		CUIGroupHelper::TranslateUIMessage(controlID, dwMessage);

		switch ( controlID )
		{		
		case COLOR_MATERIAL_DIFFUSE:
		case COLOR_MATERIAL_AMBIENT:
		case COLOR_MATERIAL_SPECULAR:
		case COLOR_MATERIAL_EMISSIVE:
			if ( CHECK_MOUSEIN_LBUPLIKE(dwMessage) == TRUE )
			{
				const DWORD _dwID(controlID - COLOR_MATERIAL_DIFFUSE);
				const DWORD _dwSelected(this->dwSelected);
				if ( (_dwSelected != -1) && (ColorMaterial::isValidColor() == true) )
					this->pColorBox[_dwSelected]->SetImage("COLORPANNEL_PAGE_INACTIVE_0");

				this->pColorBox[_dwID]->SetImage("COLORPANNEL_PAGE_ACTIVE_0");
				this->dwSelected = _dwID;
			}
			break;
		case COLOR_MATERIAL_SPECULAR_POWER:
			{
				if ( CHECK_MOUSE_IN_LBDOWNLIKE(dwMessage) == TRUE )
				{					
					this->pEditBoxPower->EndEdit();
					this->pEditBoxPower->BeginEdit();

					const DWORD _dwID(controlID - COLOR_MATERIAL_DIFFUSE);
					this->dwSelected = _dwID;
				}
				else if ( CHECK_LB_DOWN_LIKE(dwMessage) == TRUE )
					this->pEditBoxPower->EndEdit();
			}
			break;
		}
	}
	void ColorMaterial::Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl)
	{
		CUIGroupHelper::Update(x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl);

		if ( this->pEditBoxPower->IsBegin() )
		{
			const std::string _stringPower(pEditBoxPower->GetEditString().GetString());
			const float _fPower(float(::atof(_stringPower.c_str())));

			const DWORD _nLength(_stringPower.length());			

			if ( _fPower < 0.0f || _fPower > 100.0f )
				this->pEditBoxPower->SetEditString("0.0");
			
			if ( this->pMaterial != 0 )
				this->pMaterial->Power = _fPower;
		}
	}
	void ColorMaterial::SetColor(const DWORD _color)
	{
		const DWORD _dwSelected(this->dwSelected);
		const UIGUID _controlID(_dwSelected + COLOR_MATERIAL_DIFFUSE);

		switch ( _controlID )
		{
		case COLOR_MATERIAL_DIFFUSE:
			this->pMaterial->Diffuse = D3DCOLOR2D3DXCOLOR(_color);
			this->pColorBox[_dwSelected]->SetDiffuseColor(D3DCOLOR(_color));
			this->pValueText[_dwSelected]->SetText(sc::string::format("R%1%/G%2%/B%3%", (_color >> 16) & 0xff, (_color >> 8) & 0xff, (_color >> 0) & 0xff).c_str(), NS_UITEXTCOLOR::WHITE);
			break;
		case COLOR_MATERIAL_AMBIENT:
			this->pMaterial->Ambient = D3DCOLOR2D3DXCOLOR(_color);
			this->pColorBox[_dwSelected]->SetDiffuseColor(D3DCOLOR(_color));
			this->pValueText[_dwSelected]->SetText(sc::string::format("R%1%/G%2%/B%3%", (_color >> 16) & 0xff, (_color >> 8) & 0xff, (_color >> 0) & 0xff).c_str(), NS_UITEXTCOLOR::WHITE);
			break;
		case COLOR_MATERIAL_SPECULAR:
			this->pMaterial->Specular = D3DCOLOR2D3DXCOLOR(_color);
			this->pColorBox[_dwSelected]->SetDiffuseColor(D3DCOLOR(_color));
			this->pValueText[_dwSelected]->SetText(sc::string::format("R%1%/G%2%/B%3%", (_color >> 16) & 0xff, (_color >> 8) & 0xff, (_color >> 0) & 0xff).c_str(), NS_UITEXTCOLOR::WHITE);
			break;
		case COLOR_MATERIAL_EMISSIVE:
			this->pMaterial->Emissive = D3DCOLOR2D3DXCOLOR(_color);
			this->pColorBox[_dwSelected]->SetDiffuseColor(D3DCOLOR(_color));
			this->pValueText[_dwSelected]->SetText(sc::string::format("R%1%/G%2%/B%3%", (_color >> 16) & 0xff, (_color >> 8) & 0xff, (_color >> 0) & 0xff).c_str(), NS_UITEXTCOLOR::WHITE);
			break;
		}
	}

	const bool ColorMaterial::isValidColor(void) const
	{
		const UIGUID _controlID(this->dwSelected + COLOR_MATERIAL_DIFFUSE);
		switch ( _controlID )
		{
		case COLOR_MATERIAL_DIFFUSE:
		case COLOR_MATERIAL_AMBIENT:
		case COLOR_MATERIAL_SPECULAR:
		case COLOR_MATERIAL_EMISSIVE:
			return true;
		}

		return false;
	}


	const DWORD ColorMaterial::GetColor(void) const
	{
		const DWORD _dwSelected(this->dwSelected);
		if ( _dwSelected == -1 )
			return 0;

		return this->pColorBox[_dwSelected]->GetDiffuseColor();
	}
	void ColorMaterial::bindMaterialProperty(D3DMATERIAL9* _pMaterial)
	{
		this->pMaterial = _pMaterial;		

		if ( _pMaterial == NULL )
			SetVisibleSingle(FALSE);
		else
		{
			const DWORD _colorArray[4] = 
			{
				D3DXCOLOR2D3DCOLOR(_pMaterial->Diffuse),
				D3DXCOLOR2D3DCOLOR(_pMaterial->Ambient),
				D3DXCOLOR2D3DCOLOR(_pMaterial->Specular),
				D3DXCOLOR2D3DCOLOR(_pMaterial->Emissive)
			};
			
			for ( DWORD _i(4); _i--; )
			{
				const DWORD _color(_colorArray[_i]);
				this->pColorBox[_i]->SetDiffuseColor(_color);
				this->pValueText[_i]->SetText(sc::string::format("R%1%/G%2%/B%3%", (_color >> 16) & 0xff, (_color >> 8) & 0xff, (_color >> 0) & 0xff).c_str(), NS_UITEXTCOLOR::WHITE);
			}

			this->pEditBoxPower->SetEditString(sc::string::format("%1%", _pMaterial->Power).c_str());
			SetVisibleSingle(TRUE);
		}
	}


	

	ColorProperty::ColorProperty(GLGaeaClient* _pGaeaClient, CInnerInterface* _pInterface, EngineDeviceMan* _pEngineDevice)
		: CUIWindowEx(_pInterface, _pEngineDevice)		
		, pGaeaClient(_pGaeaClient)
		, dwSelected(-1)
	{
	}

	ColorProperty::~ColorProperty(void)
	{
	}

	void ColorProperty::CreateUIWindowAndRegisterOwnership(void)
	{
		Create( COLOR_PROPERTY_WINDOW, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		CreateBaseWindowLightGray( "COLOR_PROPERTY_WINDOW", "property");

		{ // control;
			this->pColorLight[0] = new ColorLight(m_pEngineDevice);
			this->pColorLight[0]->CreateSub(this, "COLOR_PROPERTY_LIGHT_0", UI_FLAG_DEFAULT, COLOR_PROPERTY_LIGHT_0);
			this->pColorLight[0]->CreateSubControl(0);
			this->pColorLight[0]->bindLightProperty(NULL);
			RegisterControl(this->pColorLight[0]);

			this->pColorLight[1] = new ColorLight(m_pEngineDevice);
			this->pColorLight[1]->CreateSub(this, "COLOR_PROPERTY_LIGHT_1", UI_FLAG_DEFAULT, COLOR_PROPERTY_LIGHT_1);
			this->pColorLight[1]->CreateSubControl(1);
			this->pColorLight[1]->bindLightProperty(NULL);
			RegisterControl(this->pColorLight[1]);

			this->pColorLight[2] = new ColorLight(m_pEngineDevice);
			this->pColorLight[2]->CreateSub(this, "COLOR_PROPERTY_LIGHT_2", UI_FLAG_DEFAULT, COLOR_PROPERTY_LIGHT_2);
			this->pColorLight[2]->CreateSubControl(2);
			this->pColorLight[2]->bindLightProperty(NULL);
			RegisterControl(this->pColorLight[2]);


			this->pMaterial = new ColorMaterial(m_pEngineDevice);
			this->pMaterial->CreateSub(this, "COLOR_PROPERTY_MATERIAL", UI_FLAG_DEFAULT, COLOR_PROPERTY_MATERIAL);
			this->pMaterial->CreateSubControl();
			this->pMaterial->bindMaterialProperty(NULL);
			RegisterControl(this->pMaterial);
		}
		{ // picker;
			this->pColorPicker = new CUIColorPickerGroup(m_pEngineDevice);
			this->pColorPicker->CreateSub(this, "COLOR_PROPERTY_PICKER", UI_FLAG_DEFAULT, COLOR_PROPERTY_PICKER);
			this->pColorPicker->CreateSubControl();
			this->pColorPicker->SetVisibleSingle(FALSE);
			RegisterControl(this->pColorPicker);
		}

		{ // Button;
// 			CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
// 
// 			this->pButtonOK = new CUITextButton(m_pEngineDevice);
// 			this->pButtonOK->CreateSub(this, "COLOR_PROPERTY_BUTTON_OK", UI_FLAG_DEFAULT, COLOR_PROPERTY_BUTTON_OK);
// 			this->pButtonOK->CreateSubControl("COLORSELECTOR_WINDOW_BUTTON_DEFAULT", "COLORSELECTOR_WINDOW_BUTTON_CLICK", pFont);
// 			this->pButtonOK->SetOneLineText(ID2GAMEWORD("COLORSELECTOR_PANNEL_PAGE_BUTTON", 3));
// 			RegisterControl(this->pButtonOK);
// 
// 			this->pButtonCancel = new CUITextButton(m_pEngineDevice);
// 			this->pButtonCancel->CreateSub(this, "COLOR_PROPERTY_BUTTON_CANCEL", UI_FLAG_DEFAULT, COLOR_PROPERTY_BUTTON_CANCEL);
// 			this->pButtonCancel->CreateSubControl("COLORSELECTOR_WINDOW_BUTTON_DEFAULT", "COLORSELECTOR_WINDOW_BUTTON_CLICK", pFont);
// 			this->pButtonCancel->SetOneLineText(ID2GAMEWORD("COLORSELECTOR_PANNEL_PAGE_BUTTON", 2));
// 			RegisterControl(this->pButtonCancel);
		}


//		SetAlignFlag(UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		m_pInterface->UiRegisterControl(this);
		m_pInterface->UiShowGroupFocus( COLOR_PROPERTY_WINDOW );		
	}

	void ColorProperty::Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl)
	{
		CUIWindowEx::Update(x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl);

		switch ( this->dwSelected )
		{
		case COLOR_PROPERTY_LIGHT_0:
		case COLOR_PROPERTY_LIGHT_1:
		case COLOR_PROPERTY_LIGHT_2:
			{
				const DWORD _index(this->dwSelected - COLOR_PROPERTY_LIGHT_0);
				if ( this->pColorLight[_index]->isValidColor() == true )
				{
					const DWORD _color(this->pColorPicker->GetColor());
					this->pColorLight[_index]->SetColor(_color);
				}
			}
			break;
		case COLOR_PROPERTY_MATERIAL:
			if ( this->pMaterial->isValidColor() == true )
			{
				const DWORD _color(this->pColorPicker->GetColor());
				this->pMaterial->SetColor(_color);
			}
			break;
		}
		
		const UIRECT _rectReference(this->pColorLight[0]->GetLocalPos());
		const float _fLeft(float(_rectReference.left));
		const float _heightBlock(_rectReference.sizeY + 10.0f);

		float _fHeightReference(float(_rectReference.top));
		for ( DWORD _i(0); _i < 3; ++_i )
		{
			if ( this->pColorLight[_i]->IsVisible() == TRUE )
			{
				this->pColorLight[_i]->SetLocalPos(D3DXVECTOR2(_fLeft, _fHeightReference));
				_fHeightReference += _heightBlock;
			}
		}		

		this->pMaterial->SetLocalPos(D3DXVECTOR2(_fLeft, _fHeightReference));
		_fHeightReference += _heightBlock;

		if ( this->pColorPicker->IsVisible() == TRUE )
		{
			this->pColorPicker->SetLocalPos(D3DXVECTOR2(_fLeft, _fHeightReference));
			_fHeightReference += this->pColorPicker->GetLocalPos().sizeY;
		}
	}

	void ColorProperty::TranslateUIMessage(UIGUID controlID, DWORD dwMessage)
	{
		CUIWindowEx::TranslateUIMessage(controlID, dwMessage);

		if ( CHECK_MOUSEIN_LBUPLIKE(dwMessage) == FALSE )
			return;

		switch ( controlID )
		{
		case COLOR_PROPERTY_LIGHT_0:
		case COLOR_PROPERTY_LIGHT_1:
		case COLOR_PROPERTY_LIGHT_2:			
			{
				const DWORD _index(controlID - COLOR_PROPERTY_LIGHT_0);
				if ( this->pColorLight[_index]->isValidColor() == true )
				{
					const DWORD _color(this->pColorLight[_index]->GetColor());

					this->dwSelected = controlID;
					this->pColorPicker->SetColor(_color);
					this->pColorPicker->SetVisibleSingle(TRUE);
				}
				else
					this->pColorPicker->SetVisibleSingle(FALSE);
			}
			break;
		case COLOR_PROPERTY_MATERIAL:
			if ( this->pMaterial->isValidColor() == true )
			{
				const DWORD _color(this->pMaterial->GetColor());

				this->dwSelected = COLOR_PROPERTY_MATERIAL;
				this->pColorPicker->SetColor(_color);
				this->pColorPicker->SetVisibleSingle(TRUE);
			}
			else
				this->pColorPicker->SetVisibleSingle(FALSE);
			break;
		case COLOR_PROPERTY_PICKER:
			break;
// 		case COLOR_PROPERTY_BUTTON_OK:
// 			break;
// 		case COLOR_PROPERTY_BUTTON_CANCEL:
// 			break;
// 		default:
// 			this->pColorPicker->SetVisibleSingle(FALSE);
			break;
		}
	}	

	void ColorProperty::bindLightProperty(D3DLIGHT9* _pLight, const DWORD _index)
	{
		if ( _index >= 3 )
			return;

		this->pColorLight[_index]->bindLightProperty(_pLight);
	}
	void ColorProperty::bindMaterialProperty(D3DMATERIAL9* _pMaterial)
	{
		this->pMaterial->bindMaterialProperty(_pMaterial);
	}
	void ColorProperty::resetProperty(void)
	{
		for ( DWORD _i(3); _i--; )
			this->pColorLight[_i]->bindLightProperty(NULL);
		this->pMaterial->bindMaterialProperty(NULL);
	}
}
