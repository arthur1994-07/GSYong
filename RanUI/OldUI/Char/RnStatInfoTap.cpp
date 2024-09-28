#include "stdafx.h"
#include "RnStatInfoTap.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/BasicVarTextBox.h"

#include "../../../RanLogic/GLogicEx.h"
#include "../../../RanLogic/GLogicDataMan.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"

#include "../Util/RnButton.h"
#include "../Item/RnInventoryWindow.h"

#include "../../InnerInterface.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace
{
	const char* ID2GAMEWORD_POINT = "포인트";
	const char* ID2GAMEWORD_INIT = "초기화";
	const char* ID2GAMEWORD_APPLY = "적용";


	const char* ID2GAMEWORD_POW = "힘";
	const char* ID2GAMEWORD_DEX = "민첩";
	const char* ID2GAMEWORD_SPI = "정신";
	const char* ID2GAMEWORD_STR = "체력";
	const char* ID2GAMEWORD_STA = "근력";

	const char* ID2GAMEWORD_ATTACK_MIN = "최소공격력";
	const char* ID2GAMEWORD_ATTACK_MAX = "최대공격력";
	const char* ID2GAMEWORD_DEFNESE = "방어력";
	const char* ID2GAMEWORD_FIGHT   = "격투치";
	const char* ID2GAMEWORD_SHOOT   = "사격치";
	const char* ID2GAMEWORD_STAMINA = "기력치";
	const char* ID2GAMEWORD_HIT	    = "명중률";
	const char* ID2GAMEWORD_AVOID   = "회피율";
	const char* ID2GAMEWORD_HP		= "HP";
	const char* ID2GAMEWORD_MP		= "MP";
	const char* ID2GAMEWORD_SP		= "SP";

	const char* NO_MARK = "-";
	const char* UP_MARK = "▲";
	const char* DOWN_MARK = "▼";

	const char* ID2GAMEWORD_FIRE_RESIST		= "화염";
	const char* ID2GAMEWORD_COLD_RESIST	    = "냉기";
	const char* ID2GAMEWORD_ELECTRIC_RESIST = "전기";
	const char* ID2GAMEWORD_POISON_RESIST   = "맹독";
	const char* ID2GAMEWORD_SPIRIT_RESIST   = "정기";


	const char* ZERO_STRING = "0";
	DWORD COLOR_GREEN = D3DCOLOR_ARGB(255,0,255,0);

// 	const char* ID2GAMEWORD_POINT_QUESTION_BUTTON_1 = "캐릭터 레벨이 증가할 때마다 3점의 능력포인트를 얻게 됩니다."
// 	"획득한 능력포인트를 아래 힘,민첩,정신,체력,근력에 "
// 	" 투자할수 있습니다";

 	const char* ID2GAMEWORD_POINT_QUESTION_BUTTON = "캐릭터 레벨이 증가할 때마다 3점의 능력포인트를 얻게 됩니다." 			
 	" 획득한 능력포인트를 아래 힘,민첩,정신,체력,근력에 투자할수 있습니다";
	

	const char* ID2GAMEWORD_POW_TOOLTIP = "힘이 증가하면, 격투치와 가격치가 증가합니다.";
	const char* ID2GAMEWORD_DEX_TOOLTIP = "민첩이 증가하면, 격투치, 사격치,기력,방어력이 증가합니다.";
	const char* ID2GAMEWORD_SPI_TOOLTIP = "정신이 증가하면, 기력과 MP가 증가합니다.";
	const char* ID2GAMEWORD_STR_TOOLTIP = "체력이 증가하면, HP가 증가 합니다.";
	const char* ID2GAMEWORD_STA_TOOLTIP = "근력이 증가하면, SP가 증가합니다.";


	const char* ID2GAMEWORD_ATTACK_MIN_TOOLTIP = "공격력(최소)은 근접 및 원거리 물리 공격 피해를 증가시킵니다.";
	const char* ID2GAMEWORD_ATTACK_MAX_TOOLTIP = "공격력(최대)은 근접 및 원거리 물리 공격 피해를 감소시킵니다.";
	const char* ID2GAMEWORD_DEFENSE_TOOLTIP = "방어력은 근접 및 원거리 물리 공격 피해를 감소시킵니다.";
	const char* ID2GAMEWORD_FIGHT_TOOLTIP = "격투치는 근접 물리 공격 피해를 증가시킵니다.";
	const char* ID2GAMEWORD_SHOOT_TOOLTIP = "사격치는 원거리 물리 공격의 피해를 증가시킵니다.";
	const char* ID2GAMEWORD_STAMINA_TOOLTIP = "기력치는 마법 공격의 피해를 증가시킵니다.";
	const char* ID2GAMEWORD_HIT_TOOLTIP = "명중률은 근접 및 원거리 물리 공격이 적에게 적중될 확률을 증가시킵니다.";
	const char* ID2GAMEWORD_AVOID_TOOLTIP = "회피율은 적의 근접 및 원거리 물리공격에 피해를 입지 않을 확률을 증가 시킵니다.";
	const char* ID2GAMEWORD_HP_TOOLTIP = "HP가 0이되면 캐릭터는 사망합니다.";
	const char* ID2GAMEWORD_MP_TOOLTIP = "MP는 스킬 사용에 필요합니다.";
	const char* ID2GAMEWORD_SP_TOOLTIP = "SP는 스킬 사용에 필요합니다.";


	/*
	const char* ID2GAMEWORD_FIRE_RESIST_TOOLTIP = "화염 속성 공격에의한 피해를 감소시키고 속성 지속시간또한 감소시킵니다.";
	const char* ID2GAMEWORD_COLD_RESIST_TOOLTIP = "냉기 속성 공격에의한 피해를 감소시키고 속성 지속시간또한 감소시킵니다.";
	const char* ID2GAMEWORD_ELECTRIC_RESIST_TOOLTIP = "전기 속성 공격에의한 피해를 감소시키고 속성 지속시간또한 감소시킵니다.";
	const char* ID2GAMEWORD_POISON_RESIST_TOOLTIP = "맹독 속성 공격에의한 피해를 감소시키고 속성 지속시간또한 감소시킵니다.";
	const char* ID2GAMEWORD_SPIRIT_RESIST_TOOLTIP = "정기 속성 공격에의한 피해를 감소시키고 속성 지속시간또한 감소시킵니다.";
	*/

	const char* ID2GAMEWORD_FIRE_RESIST_TOOLTIP     = "화염 속성 피해, 지속시간 감소 PVP(Max %1%), PVE(Max %2%)";
	const char* ID2GAMEWORD_COLD_RESIST_TOOLTIP     = "냉기 속성 피해, 지속시간 감소 PVP(Max %1%), PVE(Max %2%)";
	const char* ID2GAMEWORD_ELECTRIC_RESIST_TOOLTIP = "전기 속성 피해, 지속시간 감소 PVP(Max %1%), PVE(Max %2%)";
	const char* ID2GAMEWORD_POISON_RESIST_TOOLTIP   = "맹독 속성 피해, 지속시간 감소 PVP(Max %1%), PVE(Max %2%)";
	const char* ID2GAMEWORD_SPIRIT_RESIST_TOOLTIP   = "정기 속성 피해, 지속시간 감소 PVP(Max %1%), PVE(Max %2%)";

}

RnStatInfoTap::RnStatInfoTap( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, 
							 EngineDeviceMan* pEngineDevice) : CUIGroupHelper(pEngineDevice)
{
	m_pGaeaClient = pGaeaClient;
	m_pInterface = pInterface;

	m_pStatPointValue = NULL;

	m_pStatPowValue = NULL;
	m_pStatPowUpValue = NULL;

	m_pStatDexValue = NULL;
	m_pStatDexUpValue = NULL;

	m_pStatSpiValue = NULL;
	m_pStatSpiUpValue = NULL;

	m_pStatStrValue = NULL;
	m_pStatStrUpValue = NULL;


	InitVirtualPoint();

	//////////////////////////////////////////////////////////////////////////
	
	ID2GAMEWORD_POINT = ID2GAMEWORD("CHARACTER_STAT_POINT");
	ID2GAMEWORD_INIT  = ID2GAMEWORD("CHARACTER_STAT_INIT");
	ID2GAMEWORD_APPLY = ID2GAMEWORD("CHARACTER_STAT_APPLY");

	ID2GAMEWORD_POW = ID2GAMEWORD("CHARACTER_STAT_STATIC",0);
	ID2GAMEWORD_DEX = ID2GAMEWORD("CHARACTER_STAT_STATIC",1);
	ID2GAMEWORD_SPI = ID2GAMEWORD("CHARACTER_STAT_STATIC",2);
	ID2GAMEWORD_STR = ID2GAMEWORD("CHARACTER_STAT_STATIC",4);
	ID2GAMEWORD_STA = ID2GAMEWORD("CHARACTER_STAT_STATIC",5);

	ID2GAMEWORD_ATTACK_MIN = ID2GAMEWORD("CHARACTER_STATUS_STATIC",15);
	ID2GAMEWORD_ATTACK_MAX = ID2GAMEWORD("CHARACTER_STATUS_STATIC",14);
	ID2GAMEWORD_DEFNESE = ID2GAMEWORD("CHARACTER_STATUS_STATIC",3);
	ID2GAMEWORD_FIGHT   = ID2GAMEWORD("CHARACTER_STATUS_STATIC",4);
	ID2GAMEWORD_SHOOT   = ID2GAMEWORD("CHARACTER_STATUS_STATIC",5);
	ID2GAMEWORD_STAMINA = ID2GAMEWORD("CHARACTER_STATUS_STATIC",11);
	ID2GAMEWORD_HIT	    = ID2GAMEWORD("CHARACTER_STATUS_STATIC",9);
	ID2GAMEWORD_AVOID   = ID2GAMEWORD("CHARACTER_STATUS_STATIC",10);


	ID2GAMEWORD_FIRE_RESIST = ID2GAMEWORD("CHARACTER_RESI_STATIC", 1);
	ID2GAMEWORD_COLD_RESIST = ID2GAMEWORD("CHARACTER_RESI_STATIC", 2);
	ID2GAMEWORD_ELECTRIC_RESIST = ID2GAMEWORD("CHARACTER_RESI_STATIC", 3);
	ID2GAMEWORD_POISON_RESIST   = ID2GAMEWORD("CHARACTER_RESI_STATIC", 4);
	ID2GAMEWORD_SPIRIT_RESIST   = ID2GAMEWORD("CHARACTER_RESI_STATIC", 5);


	ID2GAMEWORD_POINT_QUESTION_BUTTON = ID2GAMEINTEXT("CHARACTER_WINDOW_POINT_QUESTION_BUTTON");

	ID2GAMEWORD_POW_TOOLTIP = ID2GAMEINTEXT("CHARACTER_WINDOW_POW_TOOLTIP");
	ID2GAMEWORD_DEX_TOOLTIP = ID2GAMEINTEXT("CHARACTER_WINDOW_DEX_TOOLTIP");
	ID2GAMEWORD_SPI_TOOLTIP = ID2GAMEINTEXT("CHARACTER_WINDOW_SPI_TOOLTIP");
	ID2GAMEWORD_STR_TOOLTIP = ID2GAMEINTEXT("CHARACTER_WINDOW_STR_TOOLTIP");
	ID2GAMEWORD_STA_TOOLTIP = ID2GAMEINTEXT("CHARACTER_WINDOW_STA_TOOLTIP");


	ID2GAMEWORD_ATTACK_MIN_TOOLTIP = ID2GAMEINTEXT("CHARACTER_WINDOW_ATTACK_MIN_TOOLTIP");
	ID2GAMEWORD_ATTACK_MAX_TOOLTIP = ID2GAMEINTEXT("CHARACTER_WINDOW_ATTACK_MAX_TOOLTIP");
	ID2GAMEWORD_DEFENSE_TOOLTIP = ID2GAMEINTEXT("CHARACTER_WINDOW_DEFENSE_TOOLTIP");
	ID2GAMEWORD_FIGHT_TOOLTIP = ID2GAMEINTEXT("CHARACTER_WINDOW_FIGHT_TOOLTIP");
	ID2GAMEWORD_SHOOT_TOOLTIP = ID2GAMEINTEXT("CHARACTER_WINDOW_SHOOT_TOOLTIP");
	ID2GAMEWORD_STAMINA_TOOLTIP = ID2GAMEINTEXT("CHARACTER_WINDOW_STAMINA_TOOLTIP");
	ID2GAMEWORD_HIT_TOOLTIP = ID2GAMEINTEXT("CHARACTER_WINDOW_HIT_TOOLTIP");
	ID2GAMEWORD_AVOID_TOOLTIP = ID2GAMEINTEXT("CHARACTER_WINDOW_AVOID_TOOLTIP");
	ID2GAMEWORD_HP_TOOLTIP = ID2GAMEINTEXT("CHARACTER_WINDOW_HP_TOOLTIP");
	ID2GAMEWORD_MP_TOOLTIP = ID2GAMEINTEXT("CHARACTER_WINDOW_MP_TOOLTIP");
	ID2GAMEWORD_SP_TOOLTIP = ID2GAMEINTEXT("CHARACTER_WINDOW_SP_TOOLTIP");

	/*
	ID2GAMEWORD_FIRE_RESIST_TOOLTIP = ID2GAMEINTEXT("CHARACTER_WINDOW_FIRE_RESIST_TOOLTIP");
	ID2GAMEWORD_COLD_RESIST_TOOLTIP = ID2GAMEINTEXT("CHARACTER_WINDOW_COLD_RESIST_TOOLTIP");
	ID2GAMEWORD_ELECTRIC_RESIST_TOOLTIP = ID2GAMEINTEXT("CHARACTER_WINDOW_ELECTRIC_RESIST_TOOLTIP");
	ID2GAMEWORD_POISON_RESIST_TOOLTIP = ID2GAMEINTEXT("CHARACTER_WINDOW_POISON_RESIST_TOOLTIP");
	ID2GAMEWORD_SPIRIT_RESIST_TOOLTIP = ID2GAMEINTEXT("CHARACTER_WINDOW_SPIRIT_RESIST_TOOLTIP");
	*/

	ID2GAMEWORD_FIRE_RESIST_TOOLTIP = ID2GAMEINTEXT("CHARACTER_WINDOW_FIRE_RESIST_TOOLTIP_C");
	ID2GAMEWORD_COLD_RESIST_TOOLTIP = ID2GAMEINTEXT("CHARACTER_WINDOW_COLD_RESIST_TOOLTIP_C");
	ID2GAMEWORD_ELECTRIC_RESIST_TOOLTIP = ID2GAMEINTEXT("CHARACTER_WINDOW_ELECTRIC_RESIST_TOOLTIP_C");
	ID2GAMEWORD_POISON_RESIST_TOOLTIP = ID2GAMEINTEXT("CHARACTER_WINDOW_POISON_RESIST_TOOLTIP_C");
	ID2GAMEWORD_SPIRIT_RESIST_TOOLTIP = ID2GAMEINTEXT("CHARACTER_WINDOW_SPIRIT_RESIST_TOOLTIP_C");

}

void RnStatInfoTap::CreateSubControl()
{
	CreateLineBox( "CHARACTER_WINDOW_STAT_POINT1_REGION", "CHARACTER_WINDOW_LINEBOX_TEXTUREINFO" );
	CreateLineBox( "CHARACTER_WINDOW_STAT_POINT2_REGION", "CHARACTER_WINDOW_LINEBOX_TEXTUREINFO" );
	CreateLineBox( "CHARACTER_WINDOW_STAT_POINT3_REGION", "CHARACTER_WINDOW_LINEBOX_TEXTUREINFO" );
	CreateLineBox( "CHARACTER_WINDOW_STAT_POINT4_REGION", "CHARACTER_WINDOW_LINEBOX_TEXTUREINFO" );
	CreateLineBox( "CHARACTER_WINDOW_STAT_POINT5_REGION", "CHARACTER_WINDOW_LINEBOX_TEXTUREINFO" );
	CreateLineBox( "CHARACTER_WINDOW_STAT_POINT6_REGION", "CHARACTER_WINDOW_LINEBOX_TEXTUREINFO" );
	CreateLineBox( "CHARACTER_WINDOW_STAT_POINT7_REGION", "CHARACTER_WINDOW_LINEBOX_TEXTUREINFO" );
	CreateLineBox( "CHARACTER_WINDOW_STAT_POINT8_REGION", "CHARACTER_WINDOW_LINEBOX_TEXTUREINFO" );
	CreateLineBox( "CHARACTER_WINDOW_STAT_POINT9_REGION", "CHARACTER_WINDOW_LINEBOX_TEXTUREINFO" );


	CreateText(ID2GAMEWORD_POINT,"CHARACTER_WINDOW_STAT_1_POINT_TEXT",TEXT_ALIGN_CENTER_X|TEXT_ALIGN_TOP);
	m_pStatPointValue = CreateText(ZERO_STRING,"CHARACTER_WINDOW_STAT_1_POINT_VALUE",TEXT_ALIGN_CENTER_X|TEXT_ALIGN_TOP,POINT_VALUE);

	CreateFlipButton("CHARACTER_WINDOW_STAT_QUESTION_BUTTON","CHARACTER_WINDOW_STAT_QUESTION_BUTTON_OVER",QUESTION_BUTTON,CBasicButton::MOUSEIN_FLIP);
	
	//////////////////////////////////////////////////////////////////////////

	m_OverRectMap[STAT_POW_BASE_OVER_RECT] = CreateOverRect("CHARACTER_WINDOW_STAT_1_POW_MOUSEOVER_RECT",STAT_POW_BASE_OVER_RECT);
	m_OverRectMap[STAT_DEX_BASE_OVER_RECT] = CreateOverRect("CHARACTER_WINDOW_STAT_1_DEX_MOUSEOVER_RECT",STAT_DEX_BASE_OVER_RECT);
	m_OverRectMap[STAT_SPI_BASE_OVER_RECT] = CreateOverRect("CHARACTER_WINDOW_STAT_1_SPI_MOUSEOVER_RECT",STAT_SPI_BASE_OVER_RECT);
	m_OverRectMap[STAT_STR_BASE_OVER_RECT] = CreateOverRect("CHARACTER_WINDOW_STAT_1_STR_MOUSEOVER_RECT",STAT_STR_BASE_OVER_RECT);
	m_OverRectMap[STAT_STA_BASE_OVER_RECT] = CreateOverRect("CHARACTER_WINDOW_STAT_1_STA_MOUSEOVER_RECT",STAT_STA_BASE_OVER_RECT);

	m_OverRectMap[STAT_ATTACK_MIN_OVER_RECT] = CreateOverRect("CHARACTER_WINDOW_STAT_2_ATTACK_MIN_MOUSEOVER_RECT",STAT_ATTACK_MIN_OVER_RECT);
	m_OverRectMap[STAT_ATTACK_MAX_OVER_RECT] = CreateOverRect("CHARACTER_WINDOW_STAT_2_ATTACK_MAX_MOUSEOVER_RECT",STAT_ATTACK_MAX_OVER_RECT);
	m_OverRectMap[STAT_DEFENSE_OVER_RECT] = CreateOverRect("CHARACTER_WINDOW_STAT_2_DEFENSE_MOUSEOVER_RECT",STAT_DEFENSE_OVER_RECT);
	m_OverRectMap[STAT_FIGHT_OVER_RECT] = CreateOverRect("CHARACTER_WINDOW_STAT_2_FIGHT_MOUSEOVER_RECT",STAT_FIGHT_OVER_RECT);
	m_OverRectMap[STAT_SHOOT_OVER_RECT] = CreateOverRect("CHARACTER_WINDOW_STAT_2_SHOOT_MOUSEOVER_RECT",STAT_SHOOT_OVER_RECT);
	m_OverRectMap[STAT_STAMINA_OVER_RECT] = CreateOverRect("CHARACTER_WINDOW_STAT_2_STAMINA_MOUSEOVER_RECT",STAT_STAMINA_OVER_RECT);
	m_OverRectMap[STAT_HIT_OVER_RECT] = CreateOverRect("CHARACTER_WINDOW_STAT_2_HIT_MOUSEOVER_RECT",STAT_HIT_OVER_RECT);
	m_OverRectMap[STAT_AVOID_OVER_RECT] = CreateOverRect("CHARACTER_WINDOW_STAT_2_AVOID_MOUSEOVER_RECT",STAT_AVOID_OVER_RECT);
	m_OverRectMap[STAT_HP_OVER_RECT] = CreateOverRect("CHARACTER_WINDOW_STAT_2_HP_MOUSEOVER_RECT",STAT_HP_OVER_RECT);
	m_OverRectMap[STAT_MP_OVER_RECT] = CreateOverRect("CHARACTER_WINDOW_STAT_2_MP_MOUSEOVER_RECT",STAT_MP_OVER_RECT);
	m_OverRectMap[STAT_SP_OVER_RECT] = CreateOverRect("CHARACTER_WINDOW_STAT_2_SP_MOUSEOVER_RECT",STAT_SP_OVER_RECT);

	m_OverRectMap[STAT_FIRE_RESIST_RECT] = CreateOverRect("CHARACTER_WINDOW_STAT_3_FIRE_RESIST_MOUSEOVER_RECT",STAT_FIRE_RESIST_RECT);
	m_OverRectMap[STAT_COLD_RESIST_RECT] = CreateOverRect("CHARACTER_WINDOW_STAT_3_COLD_RESIST_MOUSEOVER_RECT",STAT_COLD_RESIST_RECT);
	m_OverRectMap[STAT_ELECTRIC_RESIST_RECT] = CreateOverRect("CHARACTER_WINDOW_STAT_3_ELECTRIC_RESIST_MOUSEOVER_RECT",STAT_ELECTRIC_RESIST_RECT);
	m_OverRectMap[STAT_POISON_RESIST_RECT] = CreateOverRect("CHARACTER_WINDOW_STAT_3_POISON_RESIST_MOUSEOVER_RECT",STAT_POISON_RESIST_RECT);
	m_OverRectMap[STAT_SPIRIT_RESIST_RECT] = CreateOverRect("CHARACTER_WINDOW_STAT_3_SPIRIT_RESIST_MOUSEOVER_RECT",STAT_SPIRIT_RESIST_RECT);

	//////////////////////////////////////////////////////////////////////////

	CreateText(ID2GAMEWORD_POW,"CHARACTER_WINDOW_STAT_1_POW_TEXT",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP );
	CreateText(ID2GAMEWORD_DEX,"CHARACTER_WINDOW_STAT_1_DEX_TEXT",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);
	CreateText(ID2GAMEWORD_SPI,"CHARACTER_WINDOW_STAT_1_SPI_TEXT",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);
	CreateText(ID2GAMEWORD_STR,"CHARACTER_WINDOW_STAT_1_STR_TEXT",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);
	CreateText(ID2GAMEWORD_STA,"CHARACTER_WINDOW_STAT_1_STA_TEXT",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);

	//////////////////////////////////////////////////////////////////////////

	m_pStatPowValue = CreateText(ZERO_STRING,"CHARACTER_WINDOW_STAT_1_POW_VALUE",TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP,STAT_POW_BASE_VALUE);
	m_pStatDexValue = CreateText(ZERO_STRING,"CHARACTER_WINDOW_STAT_1_DEX_VALUE",TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP,STAT_DEX_BASE_VALUE);
	m_pStatSpiValue = CreateText(ZERO_STRING,"CHARACTER_WINDOW_STAT_1_SPI_VALUE",TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP,STAT_SPI_BASE_VALUE);
	m_pStatStrValue = CreateText(ZERO_STRING,"CHARACTER_WINDOW_STAT_1_STR_VALUE",TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP,STAT_STR_BASE_VALUE);
	m_pStatStaValue = CreateText(ZERO_STRING,"CHARACTER_WINDOW_STAT_1_STA_VALUE",TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP,STAT_STA_BASE_VALUE);
	//////////////////////////////////////////////////////////////////////////
	std::string ZERO_STRING2 = 	std::string("(") + ZERO_STRING + ")";

	m_pStatPowUpValue = CreateText(ZERO_STRING2,"CHARACTER_WINDOW_STAT_1_POW_UP_VALUE",TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP,STAT_POW_UP_VALUE);
	m_pStatDexUpValue = CreateText(ZERO_STRING2,"CHARACTER_WINDOW_STAT_1_DEX_UP_VALUE",TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP,STAT_DEX_UP_VALUE);
	m_pStatSpiUpValue = CreateText(ZERO_STRING2,"CHARACTER_WINDOW_STAT_1_SPI_UP_VALUE",TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP,STAT_SPI_UP_VALUE);
	m_pStatStrUpValue = CreateText(ZERO_STRING2,"CHARACTER_WINDOW_STAT_1_STR_UP_VALUE",TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP,STAT_STR_UP_VALUE);
	m_pStatStaUpValue = CreateText(ZERO_STRING2,"CHARACTER_WINDOW_STAT_1_STA_UP_VALUE",TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP,STAT_STA_UP_VALUE);

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	RnButton* pStatBtn = NULL;
	RnButton::CreateArg arg;
	//////////////////////////////////////////////////////////////////////////
	arg.defaultTextureKey = "CHARACTER_WINDOW_STAT_PLUS_MINUS_BUTTON_NORMAL_TEXTUREINFO";
	arg.mouseOverTextureKey = "CHARACTER_WINDOW_STAT_PLUS_MINUS_BUTTON_MOUSEOVER_TEXTUREINFO";
	arg.mouseClickTextureKey = "CHARACTER_WINDOW_STAT_PLUS_MINUS_BUTTON_MOUSECLICK_TEXTUREINFO";
//	arg.textKey = "CHARACTER_WINDOW_STAT_BUTTON_TEXT_POS";
	arg.pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 11, _DEFAULT_FONT_SHADOW_FLAG );

	//////////////////////////////////////////////////////////////////////////
	arg.text = "+";
	//////////////////////////////////////////////////////////////////////////
	float EventPressedTime = 0.05f;
	pStatBtn = new RnButton(m_pEngineDevice);
	pStatBtn->CreateSub(this,"CHARACTER_WINDOW_STAT_POW_PLUS_BUTTON",UI_FLAG_DEFAULT,STAT_POW_PLUS_BUTTON);
	pStatBtn->CreateSubControl(arg);
	pStatBtn->SetPressedEventTime(EventPressedTime);
	RegisterControl(pStatBtn);
	m_PlusMinusButtonVec.push_back(pStatBtn);

	pStatBtn = new RnButton(m_pEngineDevice);
	pStatBtn->CreateSub(this,"CHARACTER_WINDOW_STAT_DEX_PLUS_BUTTON",UI_FLAG_DEFAULT,STAT_DEX_PLUS_BUTTON);
	pStatBtn->CreateSubControl(arg);
	pStatBtn->SetPressedEventTime(EventPressedTime);
	RegisterControl(pStatBtn);
	m_PlusMinusButtonVec.push_back(pStatBtn);

	pStatBtn = new RnButton(m_pEngineDevice);
	pStatBtn->CreateSub(this,"CHARACTER_WINDOW_STAT_SPI_PLUS_BUTTON",UI_FLAG_DEFAULT,STAT_SPI_PLUS_BUTTON);
	pStatBtn->CreateSubControl(arg);
	pStatBtn->SetPressedEventTime(EventPressedTime);
	RegisterControl(pStatBtn);
	m_PlusMinusButtonVec.push_back(pStatBtn);

	pStatBtn = new RnButton(m_pEngineDevice);
	pStatBtn->CreateSub(this,"CHARACTER_WINDOW_STAT_STR_PLUS_BUTTON",UI_FLAG_DEFAULT,STAT_STR_PLUS_BUTTON);
	pStatBtn->CreateSubControl(arg);
	pStatBtn->SetPressedEventTime(EventPressedTime);
	RegisterControl(pStatBtn);
	m_PlusMinusButtonVec.push_back(pStatBtn);

	pStatBtn = new RnButton(m_pEngineDevice);
	pStatBtn->CreateSub(this,"CHARACTER_WINDOW_STAT_STA_PLUS_BUTTON",UI_FLAG_DEFAULT,STAT_STA_PLUS_BUTTON);
	pStatBtn->CreateSubControl(arg);
	pStatBtn->SetPressedEventTime(EventPressedTime);
	RegisterControl(pStatBtn);
	m_PlusMinusButtonVec.push_back(pStatBtn);


	//////////////////////////////////////////////////////////////////////////
	arg.text = "-";
	//////////////////////////////////////////////////////////////////////////

	pStatBtn = new RnButton(m_pEngineDevice);
	pStatBtn->CreateSub(this,"CHARACTER_WINDOW_STAT_POW_MINUS_BUTTON",UI_FLAG_DEFAULT,STAT_POW_MINUS_BUTTON);
	pStatBtn->CreateSubControl(arg);
	pStatBtn->SetPressedEventTime(EventPressedTime);
	RegisterControl(pStatBtn);
	m_PlusMinusButtonVec.push_back(pStatBtn);

	pStatBtn = new RnButton(m_pEngineDevice);
	pStatBtn->CreateSub(this,"CHARACTER_WINDOW_STAT_DEX_MINUS_BUTTON",UI_FLAG_DEFAULT,STAT_DEX_MINUS_BUTTON);
	pStatBtn->CreateSubControl(arg);
	pStatBtn->SetPressedEventTime(EventPressedTime);
	RegisterControl(pStatBtn);
	m_PlusMinusButtonVec.push_back(pStatBtn);

	pStatBtn = new RnButton(m_pEngineDevice);
	pStatBtn->CreateSub(this,"CHARACTER_WINDOW_STAT_SPI_MINUS_BUTTON",UI_FLAG_DEFAULT,STAT_SPI_MINUS_BUTTON);
	pStatBtn->CreateSubControl(arg);
	pStatBtn->SetPressedEventTime(EventPressedTime);
	RegisterControl(pStatBtn);
	m_PlusMinusButtonVec.push_back(pStatBtn);

	pStatBtn = new RnButton(m_pEngineDevice);
	pStatBtn->CreateSub(this,"CHARACTER_WINDOW_STAT_STR_MINUS_BUTTON",UI_FLAG_DEFAULT,STAT_STR_MINUS_BUTTON);
	pStatBtn->CreateSubControl(arg);
	pStatBtn->SetPressedEventTime(EventPressedTime);
	RegisterControl(pStatBtn);
	m_PlusMinusButtonVec.push_back(pStatBtn);

	pStatBtn = new RnButton(m_pEngineDevice);
	pStatBtn->CreateSub(this,"CHARACTER_WINDOW_STAT_STA_MINUS_BUTTON",UI_FLAG_DEFAULT,STAT_STA_MINUS_BUTTON);
	pStatBtn->CreateSubControl(arg);
	pStatBtn->SetPressedEventTime(EventPressedTime);
	RegisterControl(pStatBtn);
	m_PlusMinusButtonVec.push_back(pStatBtn);


	arg.pFont = NULL;
	//////////////////////////////////////////////////////////////////////////

	arg.defaultTextureKey = "CHARACTER_WINDOW_STAT_RED_BUTTON_NORMAL_TEXTUREINFO";
	arg.mouseOverTextureKey = "CHARACTER_WINDOW_STAT_RED_BUTTON_MOUSEOVER_TEXTUREINFO";
	arg.mouseClickTextureKey = "CHARACTER_WINDOW_STAT_RED_BUTTON_MOUSECLICK_TEXTUREINFO";
	arg.inactiveTextureKey = "CHARACTER_WINDOW_STAT_BUTTON_INACTIVE_TEXTUREINFO";

	arg.text = ID2GAMEWORD_INIT;

	pStatBtn = new RnButton(m_pEngineDevice);
	pStatBtn->CreateSub(this,"CHARACTER_WINDOW_STAT_INIT_BUTTON",UI_FLAG_DEFAULT,STAT_INIT_BUTTON);
	pStatBtn->CreateSubControl(arg);
	pStatBtn->SetEnable(false);
	RegisterControl(pStatBtn);
	m_StatInitButton = pStatBtn;

	arg.text = ID2GAMEWORD_APPLY;

	pStatBtn = new RnButton(m_pEngineDevice);
	pStatBtn->CreateSub(this,"CHARACTER_WINDOW_STAT_APPLY_BUTTON",UI_FLAG_DEFAULT,STAT_APPLY_BUTTON);
	pStatBtn->CreateSubControl(arg);
	pStatBtn->SetEnable(false);
	RegisterControl(pStatBtn);
	m_StatApplyButton = pStatBtn;

	
	//////////////////////////////////////////////////////////////////////////
	CreateText(ID2GAMEWORD_ATTACK_MIN,"CHARACTER_WINDOW_STAT_2_ATTACK_MIN_TEXT",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP );
	CreateText(ID2GAMEWORD_ATTACK_MAX,"CHARACTER_WINDOW_STAT_2_ATTACK_MAX_TEXT",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP );
	CreateText(ID2GAMEWORD_DEFNESE,"CHARACTER_WINDOW_STAT_2_DEFENSE_TEXT",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP );
	CreateText(ID2GAMEWORD_FIGHT,"CHARACTER_WINDOW_STAT_2_FIGHT_TEXT",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);
	CreateText(ID2GAMEWORD_SHOOT,"CHARACTER_WINDOW_STAT_2_SHOOT_TEXT",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);
	CreateText(ID2GAMEWORD_STAMINA,"CHARACTER_WINDOW_STAT_2_STAMINA_TEXT",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);
	CreateText(ID2GAMEWORD_HIT,"CHARACTER_WINDOW_STAT_2_HIT_TEXT",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);
	CreateText(ID2GAMEWORD_AVOID,"CHARACTER_WINDOW_STAT_2_AVOID_TEXT",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);
	CreateText(ID2GAMEWORD_HP,"CHARACTER_WINDOW_STAT_2_HP_TEXT",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);
	CreateText(ID2GAMEWORD_MP,"CHARACTER_WINDOW_STAT_2_MP_TEXT",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);
	CreateText(ID2GAMEWORD_SP,"CHARACTER_WINDOW_STAT_2_SP_TEXT",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);

	m_pStatAttackMinValue = CreateText(ZERO_STRING,"CHARACTER_WINDOW_STAT_2_ATTACK_MIN_VALUE",TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP );
	m_pStatAttackMaxValue = CreateText(ZERO_STRING,"CHARACTER_WINDOW_STAT_2_ATTACK_MAX_VALUE",TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP );
	m_pStatDefenseValue = CreateText(ZERO_STRING,"CHARACTER_WINDOW_STAT_2_DEFENSE_VALUE",TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP );
	m_pStatFightValue   = CreateText(ZERO_STRING,"CHARACTER_WINDOW_STAT_2_FIGHT_VALUE",TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP);
	m_pStatShootValue   = CreateText(ZERO_STRING,"CHARACTER_WINDOW_STAT_2_SHOOT_VALUE",TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP);
	m_pStatStaminaValue = CreateText(ZERO_STRING,"CHARACTER_WINDOW_STAT_2_STAMINA_VALUE",TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP);
	m_pStatHitValue     = CreateText(ZERO_STRING,"CHARACTER_WINDOW_STAT_2_HIT_VALUE",TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP);
	m_pStatAvoidValue   = CreateText(ZERO_STRING,"CHARACTER_WINDOW_STAT_2_AVOID_VALUE",TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP);
	m_pStatHpValue      = CreateText(ZERO_STRING,"CHARACTER_WINDOW_STAT_2_HP_VALUE",TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP);
	m_pStatMpValue      = CreateText(ZERO_STRING,"CHARACTER_WINDOW_STAT_2_MP_VALUE",TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP);
	m_pStatSpValue      = CreateText(ZERO_STRING,"CHARACTER_WINDOW_STAT_2_SP_VALUE",TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP);

	m_pStatAttackMinMarkImage = CreateText("","CHARACTER_WINDOW_STAT_2_ATTACK_MIN_MARK_IMAGE",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP );
	m_pStatAttackMaxMarkImage = CreateText("","CHARACTER_WINDOW_STAT_2_ATTACK_MAX_MARK_IMAGE",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP );
	m_pStatDefenseMarkImage	= CreateText("","CHARACTER_WINDOW_STAT_2_DEFENSE_MARK_IMAGE",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP );
	m_pStatFightMarkImage  	= CreateText("","CHARACTER_WINDOW_STAT_2_FIGHT_MARK_IMAGE",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);
	m_pStatShootMarkImage  	= CreateText("","CHARACTER_WINDOW_STAT_2_SHOOT_MARK_IMAGE",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);
	m_pStatStaminaMarkImage	= CreateText("","CHARACTER_WINDOW_STAT_2_STAMINA_MARK_IMAGE",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);
	m_pStatHitMarkImage    	= CreateText("","CHARACTER_WINDOW_STAT_2_HIT_MARK_IMAGE",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);
	m_pStatAvoidMarkImage  	= CreateText("","CHARACTER_WINDOW_STAT_2_AVOID_MARK_IMAGE",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);
	m_pStatHpMarkImage     	= CreateText("","CHARACTER_WINDOW_STAT_2_HP_MARK_IMAGE",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);
	m_pStatMpMarkImage     	= CreateText("","CHARACTER_WINDOW_STAT_2_MP_MARK_IMAGE",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);
	m_pStatSpMarkImage     	= CreateText("","CHARACTER_WINDOW_STAT_2_SP_MARK_IMAGE",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);

	m_pStatAttackMinMark = CreateText(NO_MARK,"CHARACTER_WINDOW_STAT_2_ATTACK_MIN_MARK",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP );
	m_pStatAttackMaxMark = CreateText(NO_MARK,"CHARACTER_WINDOW_STAT_2_ATTACK_MAX_MARK",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP );
	m_pStatDefenseMark	= CreateText(NO_MARK,"CHARACTER_WINDOW_STAT_2_DEFENSE_MARK",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP );
	m_pStatFightMark  	= CreateText(NO_MARK,"CHARACTER_WINDOW_STAT_2_FIGHT_MARK",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);
	m_pStatShootMark  	= CreateText(NO_MARK,"CHARACTER_WINDOW_STAT_2_SHOOT_MARK",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);
	m_pStatStaminaMark	= CreateText(NO_MARK,"CHARACTER_WINDOW_STAT_2_STAMINA_MARK",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);
	m_pStatHitMark    	= CreateText(NO_MARK,"CHARACTER_WINDOW_STAT_2_HIT_MARK",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);
	m_pStatAvoidMark  	= CreateText(NO_MARK,"CHARACTER_WINDOW_STAT_2_AVOID_MARK",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);
	m_pStatHpMark     	= CreateText(NO_MARK,"CHARACTER_WINDOW_STAT_2_HP_MARK",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);
	m_pStatMpMark     	= CreateText(NO_MARK,"CHARACTER_WINDOW_STAT_2_MP_MARK",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);
	m_pStatSpMark     	= CreateText(NO_MARK,"CHARACTER_WINDOW_STAT_2_SP_MARK",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);

	m_pStatAttackMinUpValue	= CreateText(ZERO_STRING,"CHARACTER_WINDOW_STAT_2_ATTACK_MIN_UP_VALUE",TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP );
	m_pStatAttackMaxUpValue	= CreateText(ZERO_STRING,"CHARACTER_WINDOW_STAT_2_ATTACK_MAX_UP_VALUE",TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP );
	m_pStatDefenseUpValue	= CreateText(ZERO_STRING,"CHARACTER_WINDOW_STAT_2_DEFENSE_UP_VALUE",TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP );
	m_pStatFightUpValue  	= CreateText(ZERO_STRING,"CHARACTER_WINDOW_STAT_2_FIGHT_UP_VALUE",TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP);
	m_pStatShootUpValue  	= CreateText(ZERO_STRING,"CHARACTER_WINDOW_STAT_2_SHOOT_UP_VALUE",TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP);
	m_pStatStaminaUpValue	= CreateText(ZERO_STRING,"CHARACTER_WINDOW_STAT_2_STAMINA_UP_VALUE",TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP);
	m_pStatHitUpValue    	= CreateText(ZERO_STRING,"CHARACTER_WINDOW_STAT_2_HIT_UP_VALUE",TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP);
	m_pStatAvoidUpValue  	= CreateText(ZERO_STRING,"CHARACTER_WINDOW_STAT_2_AVOID_UP_VALUE",TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP);
	m_pStatHpUpValue     	= CreateText(ZERO_STRING,"CHARACTER_WINDOW_STAT_2_HP_UP_VALUE",TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP);
	m_pStatMpUpValue     	= CreateText(ZERO_STRING,"CHARACTER_WINDOW_STAT_2_MP_UP_VALUE",TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP);
	m_pStatSpUpValue     	= CreateText(ZERO_STRING,"CHARACTER_WINDOW_STAT_2_SP_UP_VALUE",TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP);


	CreateText(ID2GAMEWORD_FIRE_RESIST,"CHARACTER_WINDOW_STAT_3_FIRE_RESIST_TEXT",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP );
	CreateText(ID2GAMEWORD_COLD_RESIST,"CHARACTER_WINDOW_STAT_3_COLD_RESIST_TEXT",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);
	CreateText(ID2GAMEWORD_ELECTRIC_RESIST,"CHARACTER_WINDOW_STAT_3_ELECTRIC_RESIST_TEXT",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);
	CreateText(ID2GAMEWORD_POISON_RESIST,"CHARACTER_WINDOW_STAT_3_POISON_RESIST_TEXT",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);
	CreateText(ID2GAMEWORD_SPIRIT_RESIST,"CHARACTER_WINDOW_STAT_3_SPIRIT_RESIST_TEXT",TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);

	m_pFireResistValue = CreateText(ZERO_STRING,"CHARACTER_WINDOW_STAT_3_FIRE_RESIST_VALUE",TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP );
	m_pColdResistValue = CreateText(ZERO_STRING,"CHARACTER_WINDOW_STAT_3_COLD_RESIST_VALUE",TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP);
	m_pElectricResistValue = CreateText(ZERO_STRING,"CHARACTER_WINDOW_STAT_3_ELECTRIC_RESIST_VALUE",TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP);
	m_pPoisonResistValue = CreateText(ZERO_STRING,"CHARACTER_WINDOW_STAT_3_POISON_RESIST_VALUE",TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP);
	m_pSpiritResistValue = CreateText(ZERO_STRING,"CHARACTER_WINDOW_STAT_3_SPIRIT_RESIST_VALUE",TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP);
	
}


void RnStatInfoTap::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if(!m_apCharData.get()) 
		m_apCharData.reset(new GLCHARLOGIC);	

	// 이걸 매번 실시간으로 업데이트 해야하는지는 좀 의심이 가지만 
	// 기존 코드 형태가 이러니 그냥 가져다 쓰자
	if( GetPoint() )
	{
		for (size_t i=0; i < m_PlusMinusButtonVec.size(); ++i )
		{
			m_PlusMinusButtonVec[i]->SetVisibleSingle(true);
		}

		m_StatInitButton->SetEnable(true);
		m_StatApplyButton->SetEnable(true);

		
	}
	else
	{
		for (size_t i=0; i < m_PlusMinusButtonVec.size(); ++i )
		{
			m_PlusMinusButtonVec[i]->SetVisibleSingle(false);
		}

		m_StatInitButton->SetEnable(false);
		m_StatApplyButton->SetEnable(false);

		InitVirtualPoint();
	}

	GLCHARLOGIC* pCharData = &m_pGaeaClient->GetCharacterLogic();
    GLCharacter* pChar = m_pGaeaClient->GetCharacter();

	SetPoint(*pCharData);
	SetPow(*pCharData);
	SetDex(*pCharData);
	SetSpi(*pCharData);
	SetStr(*pCharData);
	SetSta(*pCharData);

	//if( IsUsedVirtualPoint() )
	{
 		*m_apCharData = *pCharData;

		m_apCharData->m_sStats.wPow += m_VirtualPowPoint;
		m_apCharData->m_sStats.wDex += m_VirtualDexPoint;
		m_apCharData->m_sStats.wSpi += m_VirtualSpiPoint;
		m_apCharData->m_sStats.wStr += m_VirtualStrPoint;
		m_apCharData->m_sStats.wSta += m_VirtualStaPoint;

		BYTE   nDurabilityApply[DURABILITY_APPLY_COUNT] = {  1, 1, 1, 1, 1, 1  };

		// 백업
		for( int i = 0; i < DURABILITY_APPLY_COUNT; ++i )
		{
			nDurabilityApply[i] = GLCONST_CHAR::nDurabilityApply[i];

			GLCONST_CHAR::nDurabilityApply[i] = 0;
		}

		// 인벤토리 아이템에 마우스를 대면 착용한 것 처럼 정보 표시
		ApplyInvenSlotItem( m_apCharData.get() );

		m_apCharData->INIT_DATA( FALSE, FALSE );
		m_apCharData->UPDATE_DATA( 1, 1 );

		//복구
		for( int i = 0; i < DURABILITY_APPLY_COUNT; ++i )
		{
			GLCONST_CHAR::nDurabilityApply[i] = nDurabilityApply[i];
		}
	}

	SetAttackMin(*pCharData,*m_apCharData);
	SetAttackMax(*pCharData,*m_apCharData);
	SetDefense(*pCharData,*m_apCharData);
	SetFight(*pCharData,*m_apCharData);
	SetShoot(*pCharData,*m_apCharData);
	SetStamina(*pCharData,*m_apCharData);
	SetHit(*pCharData,*m_apCharData);
	SetAvoid(*pCharData,*m_apCharData);
	SetHP(*pCharData,*m_apCharData);
	SetMP(*pCharData,*m_apCharData);
	SetSP(*pCharData,*m_apCharData);


	//////////////////////////////////////////////////////////////////////////
	SetFireResist(*pCharData);
	SetColdResist(*pCharData);
	SetElectricResist(*pCharData);
	SetPoisonResist(*pCharData);
	SetSpiritResist(*pCharData);


	CUIGroupHelper::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}


void RnStatInfoTap::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	if( ControlID >= STAT_POW_BASE_OVER_RECT && ControlID <= STAT_SPIRIT_RESIST_RECT)
	{
		OverRectMap::iterator it = m_OverRectMap.find(ControlID);
		if( m_OverRectMap.end() != it )
		{
			CUIControl* pControl = it->second;
			pControl->SetUseRender(CHECK_MOUSE_IN( dwMsg ));
		}
	}

	switch(ControlID)
	{
	case QUESTION_BUTTON:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
			{
				// 버튼 클릭
// 				m_pToolTip->SetVisibleSingle(TRUE);
// 				m_pToolTip->ClearText();
// 				m_pToolTip->SetText(ID2GAMEWORD_POINT_QUESTION_BUTTON_1,NS_UITEXTCOLOR::LIGHTSKYBLUE);
				CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
				m_pInterface->BEGIN_COMMON_LINEINFO_MULTI(TEXT_ALIGN_LEFT|TEXT_ALIGN_TOP);

				NS_UITEXTUTIL::MULTILINETEXT multiLine = NS_UITEXTUTIL::GetMLTextWithoutCRLF(ID2GAMEWORD_POINT_QUESTION_BUTTON,350,pFont);

				for (size_t i=0; i < multiLine.size(); ++i)
					m_pInterface->ADD_COMMON_LINEINFO_MULTI(multiLine[i].strLine, NS_UITEXTCOLOR::LIGHTSKYBLUE);

				m_pInterface->END_COMMON_LINEINFO_MULTI();

  		//		m_pInterface->SHOW_COMMON_LINEINFO(ID2GAMEWORD_POINT_QUESTION_BUTTON_2,NS_UITEXTCOLOR::LIGHTSKYBLUE );	
		//		m_pToolTip->AddText(ID2GAMEWORD_POINT_QUESTION_BUTTON_3,NS_UITEXTCOLOR::LIGHTSKYBLUE );	
			}
			break;
		}
	case STAT_POW_BASE_OVER_RECT:
		{
			if( CHECK_MOUSE_IN( dwMsg ) )
				m_pInterface->SHOW_COMMON_LINEINFO(ID2GAMEWORD_POW_TOOLTIP,NS_UITEXTCOLOR::LIGHTSKYBLUE);
			break;
		}
	case STAT_DEX_BASE_OVER_RECT:
		{
			if( CHECK_MOUSE_IN( dwMsg ) )
				m_pInterface->SHOW_COMMON_LINEINFO(ID2GAMEWORD_DEX_TOOLTIP,NS_UITEXTCOLOR::LIGHTSKYBLUE);
			break;
		}
	case STAT_SPI_BASE_OVER_RECT:
		{
			if( CHECK_MOUSE_IN( dwMsg ) )
				m_pInterface->SHOW_COMMON_LINEINFO(ID2GAMEWORD_SPI_TOOLTIP,NS_UITEXTCOLOR::LIGHTSKYBLUE);
			break;
		}
	case STAT_STR_BASE_OVER_RECT:
		{
			if( CHECK_MOUSE_IN( dwMsg ) )
				m_pInterface->SHOW_COMMON_LINEINFO(ID2GAMEWORD_STR_TOOLTIP,NS_UITEXTCOLOR::LIGHTSKYBLUE);
			break;
		}
	case STAT_STA_BASE_OVER_RECT:
		{
			if( CHECK_MOUSE_IN( dwMsg ) )
				m_pInterface->SHOW_COMMON_LINEINFO(ID2GAMEWORD_STA_TOOLTIP,NS_UITEXTCOLOR::LIGHTSKYBLUE);
			break;
		}
	case STAT_ATTACK_MIN_OVER_RECT:
		{
			if( CHECK_MOUSE_IN( dwMsg ) )
				m_pInterface->SHOW_COMMON_LINEINFO(ID2GAMEWORD_ATTACK_MIN_TOOLTIP,NS_UITEXTCOLOR::LIGHTSKYBLUE);
			break;
		}
	case STAT_ATTACK_MAX_OVER_RECT:
		{
			if( CHECK_MOUSE_IN( dwMsg ) )
				m_pInterface->SHOW_COMMON_LINEINFO(ID2GAMEWORD_ATTACK_MAX_TOOLTIP,NS_UITEXTCOLOR::LIGHTSKYBLUE);
			break;
		}
	case STAT_DEFENSE_OVER_RECT:
		{
			if( CHECK_MOUSE_IN( dwMsg ) )
				m_pInterface->SHOW_COMMON_LINEINFO(ID2GAMEWORD_DEFENSE_TOOLTIP,NS_UITEXTCOLOR::LIGHTSKYBLUE);
			break;
		}
	case STAT_FIGHT_OVER_RECT:
		{
			if( CHECK_MOUSE_IN( dwMsg ) )
				m_pInterface->SHOW_COMMON_LINEINFO(ID2GAMEWORD_FIGHT_TOOLTIP,NS_UITEXTCOLOR::LIGHTSKYBLUE);
			break;
		}
	case STAT_SHOOT_OVER_RECT:
		{
			if( CHECK_MOUSE_IN( dwMsg ) )
				m_pInterface->SHOW_COMMON_LINEINFO(ID2GAMEWORD_SHOOT_TOOLTIP,NS_UITEXTCOLOR::LIGHTSKYBLUE);
			break;
		}
	case STAT_STAMINA_OVER_RECT:
		{
			if( CHECK_MOUSE_IN( dwMsg ) )
				m_pInterface->SHOW_COMMON_LINEINFO(ID2GAMEWORD_STAMINA_TOOLTIP,NS_UITEXTCOLOR::LIGHTSKYBLUE);
			break;
		}
	case STAT_HIT_OVER_RECT:
		{
			if( CHECK_MOUSE_IN( dwMsg ) )
			{
				const CString strHitRate = sc::string::format( "%.2f", m_apCharData->GetHitRate() ).c_str();
				const CString strHitTooltip = sc::string::format(std::string(ID2GAMEWORD_HIT_TOOLTIP), strHitRate.GetString()).c_str();
				m_pInterface->SHOW_COMMON_LINEINFO(strHitTooltip.GetString(),NS_UITEXTCOLOR::LIGHTSKYBLUE);
			}
			break;
		}
	case STAT_AVOID_OVER_RECT:
		{
			if( CHECK_MOUSE_IN( dwMsg ) )
			{
				const CString strAvoidRate = sc::string::format( "%.2f", m_apCharData->GetAvoidRate() ).c_str();
				const CString strAvoidTooltip = sc::string::format(std::string(ID2GAMEWORD_AVOID_TOOLTIP), strAvoidRate.GetString()).c_str();
				m_pInterface->SHOW_COMMON_LINEINFO(strAvoidTooltip.GetString(),NS_UITEXTCOLOR::LIGHTSKYBLUE);
			}
			break;
		}
	case STAT_HP_OVER_RECT:
		{
			if( CHECK_MOUSE_IN( dwMsg ) )
				m_pInterface->SHOW_COMMON_LINEINFO(ID2GAMEWORD_HP_TOOLTIP,NS_UITEXTCOLOR::LIGHTSKYBLUE);
			break;
		}
	case STAT_MP_OVER_RECT:
		{
			if( CHECK_MOUSE_IN( dwMsg ) )
				m_pInterface->SHOW_COMMON_LINEINFO(ID2GAMEWORD_MP_TOOLTIP,NS_UITEXTCOLOR::LIGHTSKYBLUE);
			break;
		}
	case STAT_SP_OVER_RECT:
		{
			if( CHECK_MOUSE_IN( dwMsg ) )
				m_pInterface->SHOW_COMMON_LINEINFO(ID2GAMEWORD_SP_TOOLTIP,NS_UITEXTCOLOR::LIGHTSKYBLUE);
			break;
		}
	case STAT_FIRE_RESIST_RECT:
		{
			if( CHECK_MOUSE_IN( dwMsg ) )
			{
				int iPvP = static_cast<int>( GLCONST_CHAR::fREGIST_MAX_G/GLCONST_CHAR::fRESIST_G*100 );
				int iPvE = static_cast<int>( GLCONST_CHAR::fRESIST_MAX_CROW/GLCONST_CHAR::fRESIST_G*100 );

				std::string strText = sc::string::format( ID2GAMEWORD_FIRE_RESIST_TOOLTIP, iPvP, iPvE );

				m_pInterface->SHOW_COMMON_LINEINFO(strText.c_str(),NS_UITEXTCOLOR::LIGHTSKYBLUE);
			}
			break;
		}
	case STAT_COLD_RESIST_RECT:
		{
			if( CHECK_MOUSE_IN( dwMsg ) )
			{
				int iPvP = static_cast<int>( GLCONST_CHAR::fREGIST_MAX_G/GLCONST_CHAR::fRESIST_G*100 );
				int iPvE = static_cast<int>( GLCONST_CHAR::fRESIST_MAX_CROW/GLCONST_CHAR::fRESIST_G*100 );

				std::string strText = sc::string::format( ID2GAMEWORD_COLD_RESIST_TOOLTIP, iPvP, iPvE );

				m_pInterface->SHOW_COMMON_LINEINFO(strText.c_str(),NS_UITEXTCOLOR::LIGHTSKYBLUE);
			}
			break;
		}
	case STAT_ELECTRIC_RESIST_RECT:
		{
			if( CHECK_MOUSE_IN( dwMsg ) )
			{
				int iPvP = static_cast<int>( GLCONST_CHAR::fREGIST_MAX_G/GLCONST_CHAR::fRESIST_G*100 );
				int iPvE = static_cast<int>( GLCONST_CHAR::fRESIST_MAX_CROW/GLCONST_CHAR::fRESIST_G*100 );

				std::string strText = sc::string::format( ID2GAMEWORD_ELECTRIC_RESIST_TOOLTIP, iPvP, iPvE );

				m_pInterface->SHOW_COMMON_LINEINFO(strText.c_str(),NS_UITEXTCOLOR::LIGHTSKYBLUE);
			}
			break;
		}
	case STAT_POISON_RESIST_RECT:
		{
			if( CHECK_MOUSE_IN( dwMsg ) )
			{
				int iPvP = static_cast<int>( GLCONST_CHAR::fREGIST_MAX_G/GLCONST_CHAR::fRESIST_G*100 );
				int iPvE = static_cast<int>( GLCONST_CHAR::fRESIST_MAX_CROW/GLCONST_CHAR::fRESIST_G*100 );

				std::string strText = sc::string::format( ID2GAMEWORD_POISON_RESIST_TOOLTIP, iPvP, iPvE );

				m_pInterface->SHOW_COMMON_LINEINFO(strText.c_str(),NS_UITEXTCOLOR::LIGHTSKYBLUE);
			}
			break;
		}
	case STAT_SPIRIT_RESIST_RECT:
		{
			if( CHECK_MOUSE_IN( dwMsg ) )
			{
				int iPvP = static_cast<int>( GLCONST_CHAR::fREGIST_MAX_G/GLCONST_CHAR::fRESIST_G*100 );
				int iPvE = static_cast<int>( GLCONST_CHAR::fRESIST_MAX_CROW/GLCONST_CHAR::fRESIST_G*100 );

				std::string strText = sc::string::format( ID2GAMEWORD_SPIRIT_RESIST_TOOLTIP, iPvP, iPvE );

				m_pInterface->SHOW_COMMON_LINEINFO(strText.c_str(),NS_UITEXTCOLOR::LIGHTSKYBLUE);
			}
			break;
		}

	case STAT_POW_PLUS_BUTTON:
		{
			if( (dwMsg & UIMSG_RNBUTTON_CLICK) || (dwMsg & UIMSG_RNBUTTON_EVENT_PRESSED) )
				PlusVirtualPoint(m_VirtualPowPoint);
			break;
		}
	case STAT_POW_MINUS_BUTTON:
		{
			if( (dwMsg & UIMSG_RNBUTTON_CLICK) || (dwMsg & UIMSG_RNBUTTON_EVENT_PRESSED) )
				MinusVirtualPoint(m_VirtualPowPoint);
			break;
		}
	case STAT_DEX_PLUS_BUTTON:
		{
			if( (dwMsg & UIMSG_RNBUTTON_CLICK) || (dwMsg & UIMSG_RNBUTTON_EVENT_PRESSED) )
				PlusVirtualPoint(m_VirtualDexPoint);
			break;
		}
	case STAT_DEX_MINUS_BUTTON:
		{
			if( (dwMsg & UIMSG_RNBUTTON_CLICK) || (dwMsg & UIMSG_RNBUTTON_EVENT_PRESSED) )
				MinusVirtualPoint(m_VirtualDexPoint);
			break;
		}
	case STAT_SPI_PLUS_BUTTON:
		{
			if( (dwMsg & UIMSG_RNBUTTON_CLICK) || (dwMsg & UIMSG_RNBUTTON_EVENT_PRESSED) )
				PlusVirtualPoint(m_VirtualSpiPoint);
			break;
		}
	case STAT_SPI_MINUS_BUTTON:
		{
			if( (dwMsg & UIMSG_RNBUTTON_CLICK) || (dwMsg & UIMSG_RNBUTTON_EVENT_PRESSED) )
				MinusVirtualPoint(m_VirtualSpiPoint);
			break;
		}
	case STAT_STR_PLUS_BUTTON:
		{
			if( (dwMsg & UIMSG_RNBUTTON_CLICK) || (dwMsg & UIMSG_RNBUTTON_EVENT_PRESSED) )
				PlusVirtualPoint(m_VirtualStrPoint);
			break;
		}
	case STAT_STR_MINUS_BUTTON:
		{
			if( (dwMsg & UIMSG_RNBUTTON_CLICK) || (dwMsg & UIMSG_RNBUTTON_EVENT_PRESSED) )
				MinusVirtualPoint(m_VirtualStrPoint);
			break;
		}
	case STAT_STA_PLUS_BUTTON:
		{
			if( (dwMsg & UIMSG_RNBUTTON_CLICK) || (dwMsg & UIMSG_RNBUTTON_EVENT_PRESSED) )
				PlusVirtualPoint(m_VirtualStaPoint);
			break;
		}
	case STAT_STA_MINUS_BUTTON:
		{
			if( (dwMsg & UIMSG_RNBUTTON_CLICK) || (dwMsg & UIMSG_RNBUTTON_EVENT_PRESSED) )
				MinusVirtualPoint(m_VirtualStaPoint);
			break;
		}
	case STAT_INIT_BUTTON:
		{
			if( dwMsg & UIMSG_RNBUTTON_CLICK)
			{
				InitVirtualPoint();

			}
			break;
		}
	case STAT_APPLY_BUTTON:
		{
			if( dwMsg & UIMSG_RNBUTTON_CLICK)
			{
				WORD emSTATS[EMSIZE] = {0,};
				emSTATS[EMPOW] = (WORD)m_VirtualPowPoint;
				emSTATS[EMDEX] = (WORD)m_VirtualDexPoint;
				emSTATS[EMSPI] = (WORD)m_VirtualSpiPoint;
				emSTATS[EMSTR] = (WORD)m_VirtualStrPoint;
				emSTATS[EMSTA] = (WORD)m_VirtualStaPoint;
				m_pGaeaClient->GetCharacter()->ReqStatsUp(emSTATS);

				InitVirtualPoint();
			}
			break;
		}
	}
}


int RnStatInfoTap::GetVirtualPow()
{
	return m_pGaeaClient->GetCharacterLogic().m_sSUMSTATS_SKILL.wPow + m_VirtualPowPoint;
}

int RnStatInfoTap::GetVirtualDex()
{
	return m_pGaeaClient->GetCharacterLogic().m_sSUMSTATS_SKILL.wDex + m_VirtualDexPoint;
}

int RnStatInfoTap::GetVirtualSpi()
{
	return m_pGaeaClient->GetCharacterLogic().m_sSUMSTATS_SKILL.wSpi + m_VirtualSpiPoint;
}

int RnStatInfoTap::GetVirtualStr()
{
	return m_pGaeaClient->GetCharacterLogic().m_sSUMSTATS_SKILL.wStr + m_VirtualStrPoint;
}

int RnStatInfoTap::GetVirtualSta()
{
	return m_pGaeaClient->GetCharacterLogic().m_sSUMSTATS_SKILL.wSta + m_VirtualStaPoint;
}

void RnStatInfoTap::SetPoint( const GLCHARLOGIC& sCharData )
{
	CString strCombine;
	strCombine.Format( "%d", GetRestVirtualPoint() );
	m_pStatPointValue->SetOneLineText ( strCombine, NS_UITEXTCOLOR::GOLD );
}

void RnStatInfoTap::SetPow( const GLCHARLOGIC& sCharData )
{
	SetStatHelper(m_pStatPowValue,m_pStatPowUpValue,sCharData.m_sSUMSTATS_SKILL.wPow,GetVirtualPow());
}

void RnStatInfoTap::SetDex( const GLCHARLOGIC& sCharData )
{
	SetStatHelper(m_pStatDexValue,m_pStatDexUpValue,sCharData.m_sSUMSTATS_SKILL.wDex,GetVirtualDex());
}

void RnStatInfoTap::SetSpi( const GLCHARLOGIC& sCharData )
{
	SetStatHelper(m_pStatSpiValue,m_pStatSpiUpValue,sCharData.m_sSUMSTATS_SKILL.wSpi,GetVirtualSpi());
}

void RnStatInfoTap::SetStr( const GLCHARLOGIC& sCharData )
{
	SetStatHelper(m_pStatStrValue,m_pStatStrUpValue,sCharData.m_sSUMSTATS_SKILL.wStr,GetVirtualStr());
}

void RnStatInfoTap::SetSta( const GLCHARLOGIC& sCharData )
{
	SetStatHelper(m_pStatStaValue,m_pStatStaUpValue,sCharData.m_sSUMSTATS_SKILL.wSta,GetVirtualSta() );
}

WORD RnStatInfoTap::GetPoint()
{
	const GLCHARLOGIC& sCharData = m_pGaeaClient->GetCharacterLogic();	
	return sCharData.m_wStatsPoint;
}

void RnStatInfoTap::SetAttackMin( const GLCHARLOGIC& originData,const GLCHARLOGIC& changeUpData  )
{
	ChangeUpHelper(m_pStatAttackMinValue,m_pStatAttackMinMark,m_pStatAttackMinUpValue,originData.GETFORCE_LOW(),
		changeUpData.GETFORCE_LOW() - originData.GETFORCE_LOW(), m_pStatAttackMinMarkImage);
}

void RnStatInfoTap::SetAttackMax( const GLCHARLOGIC& originData,const GLCHARLOGIC& changeUpData  )
{
	ChangeUpHelper(m_pStatAttackMaxValue,m_pStatAttackMaxMark,m_pStatAttackMaxUpValue,originData.GETFORCE_HIGH(),
		changeUpData.GETFORCE_HIGH() - originData.GETFORCE_HIGH(), m_pStatAttackMaxMarkImage);
}

void RnStatInfoTap::SetDefense( const GLCHARLOGIC& originData,const GLCHARLOGIC& changeUpData  )
{
	ChangeUpHelper(m_pStatDefenseValue,m_pStatDefenseMark,m_pStatDefenseUpValue,originData.GETDEFENSE(),
		changeUpData.GETDEFENSE() - originData.GETDEFENSE(), m_pStatDefenseMarkImage);
}

void RnStatInfoTap::SetFight( const GLCHARLOGIC& originData,const GLCHARLOGIC& changeUpData  )
{
	ChangeUpHelper(m_pStatFightValue,m_pStatFightMark,m_pStatFightUpValue,originData.m_sumPowerAttribute[SKILL::EMAPPLY_MELEE],
		changeUpData.m_sumPowerAttribute[SKILL::EMAPPLY_MELEE]  - originData.m_sumPowerAttribute[SKILL::EMAPPLY_MELEE], m_pStatFightMarkImage);
}

void RnStatInfoTap::SetShoot( const GLCHARLOGIC& originData,const GLCHARLOGIC& changeUpData  )
{
	ChangeUpHelper(m_pStatShootValue,m_pStatShootMark,m_pStatShootUpValue,originData.m_sumPowerAttribute[SKILL::EMAPPLY_RANGE],
		changeUpData.m_sumPowerAttribute[SKILL::EMAPPLY_RANGE] - originData.m_sumPowerAttribute[SKILL::EMAPPLY_RANGE], m_pStatShootMarkImage);
}

void RnStatInfoTap::SetStamina( const GLCHARLOGIC& originData,const GLCHARLOGIC& changeUpData  )
{
	ChangeUpHelper(m_pStatStaminaValue,m_pStatStaminaMark,m_pStatStaminaUpValue,originData.m_sumPowerAttribute[SKILL::EMAPPLY_MAGIC],
		changeUpData.m_sumPowerAttribute[SKILL::EMAPPLY_MAGIC] - originData.m_sumPowerAttribute[SKILL::EMAPPLY_MAGIC], m_pStatStaminaMarkImage);
}

void RnStatInfoTap::SetHit( const GLCHARLOGIC& originData,const GLCHARLOGIC& changeUpData  )
{
	ChangeUpHelper(m_pStatHitValue,m_pStatHitMark,m_pStatHitUpValue,originData.GETHIT(),
		changeUpData.GETHIT() - originData.GETHIT(), m_pStatHitMarkImage);
}

void RnStatInfoTap::SetAvoid( const GLCHARLOGIC& originData,const GLCHARLOGIC& changeUpData  )
{
	ChangeUpHelper(m_pStatAvoidValue,m_pStatAvoidMark,m_pStatAvoidUpValue,originData.GETAVOID(),
		changeUpData.GETAVOID() - originData.GETAVOID(), m_pStatAvoidMarkImage);	
}

void RnStatInfoTap::SetHP( const GLCHARLOGIC& originData,const GLCHARLOGIC& changeUpData  )
{
	ChangeUpHelper(m_pStatHpValue,m_pStatHpMark,m_pStatHpUpValue,originData.GETMAXHP(),
		changeUpData.GETMAXHP() - originData.GETMAXHP(), m_pStatHpMarkImage);

}

void RnStatInfoTap::SetMP( const GLCHARLOGIC& originData,const GLCHARLOGIC& changeUpData  )
{
	ChangeUpHelper(m_pStatMpValue,m_pStatMpMark,m_pStatMpUpValue,originData.GETMAXMP(),
		changeUpData.GETMAXMP() - originData.GETMAXMP(), m_pStatMpMarkImage);
}

void RnStatInfoTap::SetSP( const GLCHARLOGIC& originData,const GLCHARLOGIC& changeUpData  )
{
	ChangeUpHelper(m_pStatSpValue,m_pStatSpMark,m_pStatSpUpValue,originData.GETMAXSP(),
		changeUpData.GETMAXSP() - originData.GETMAXSP(), m_pStatSpMarkImage);
}

void RnStatInfoTap::InitVirtualPoint()
{
	m_VirtualPowPoint = 0;
	m_VirtualDexPoint = 0;
	m_VirtualSpiPoint = 0;
	m_VirtualStrPoint = 0;
	m_VirtualStaPoint = 0;
}

int RnStatInfoTap::GetRestVirtualPoint()
{
	return GetPoint() - m_VirtualPowPoint - m_VirtualDexPoint - m_VirtualSpiPoint
		- m_VirtualStrPoint - m_VirtualStaPoint;
}

void RnStatInfoTap::PlusVirtualPoint( int& virtualPoint )
{
	if( !GetRestVirtualPoint() )
		return;

	virtualPoint++;
}

void RnStatInfoTap::MinusVirtualPoint( int& virtualPoint )
{
	if( virtualPoint > 0 )
		virtualPoint--;
}

bool RnStatInfoTap::IsUsedVirtualPoint()
{
	return (m_VirtualPowPoint + m_VirtualDexPoint + m_VirtualSpiPoint + m_VirtualStrPoint + m_VirtualStaPoint) > 0;
}

void RnStatInfoTap::ApplyInvenSlotItem( GLCHARLOGIC* pChar )
{
	if( !pChar )
		return;

	IInventory* pInventory = m_pInterface->GetInventory();
	if( !pInventory )
		return;

	if( pInventory->GetTapSelected() != CRnInventoryWindow::ETapInven )
		return;

	// RM# 989: 포장된 아이템은 캐릭터 정보창에서 아이템의 능력치 비교가 되지않도록 수정요청이 옴;
	const SINVENITEM* pInvenItem = pInventory->GetFocusItem();
	if( !pInvenItem || pInvenItem->GetNativeID() == NATIVEID_NULL() || pInvenItem->CustomItem().IsBoxWrapped())
		return;

	SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->GetNativeID() );
	if( !pItem )
		return;

	if( !pItem->IsSuitable() )
		return;

	// 조건은 오로지 부서만 확인한다
	if( pItem->sBasicOp.dwReqCharClass & pChar->m_emClass )
	{
		for( int i=0; i<SLOT_TSIZE; ++i )
		{
			if( EMSLOTCHECK_OK == pChar->CHECKSLOT_ITEM( pInvenItem->GetNativeID(), EMSLOT( i ) ) )
			{
				pChar->m_PutOnItems[ i ] = pInvenItem->sItemCustom;
				break;
			}
		}
	}
}

void RnStatInfoTap::ChangeUpHelper(CBasicTextBox* pValueText,CBasicTextBox* pMarkText,CBasicTextBox* pChangeUpText,
								   int originValue,int changeUpValue, CBasicTextBox* pMarkTextImage )
{
	CString strCombine;
	strCombine.Format ( "%d", originValue );
	pValueText->SetText(strCombine);

	strCombine.Format ( "%d", changeUpValue);
	if( changeUpValue )
	{
		pMarkTextImage->ClearText();
		
		if( strCombine.Find("-") >= 0 )
		{
			pMarkText->ClearText();
			pChangeUpText->SetText(strCombine,NS_UITEXTCOLOR::RED);
			pMarkTextImage->AddText("");
			if (pMarkTextImage)
				pMarkTextImage->SetTextImage(0,"LOWER_ARROW");
		}
		else
		{
			pMarkText->ClearText();
			pChangeUpText->SetText(strCombine,COLOR_GREEN);
			pMarkTextImage->AddText("");
			if (pMarkTextImage)
				pMarkTextImage->SetTextImage(0,"UPPER_ARROW");
		}
	}
	else
	{
		pMarkText->SetText(NO_MARK);
		pMarkTextImage->ClearText();
		pChangeUpText->SetText(strCombine);
	}
}

void RnStatInfoTap::SetStatHelper( CBasicTextBox* pValueText,CBasicTextBox* pUpValueText,int originValue,int changeUp )
{
	pValueText->ClearText();
	pUpValueText->ClearText();

	CString strCombine;

	if( GetPoint() )
	{
		strCombine.Format( "%d", originValue );
		pValueText->SetText( strCombine );

		//////////////////////////////////////////////////////////////////////////
		strCombine.Format("(%d)",changeUp);
		pUpValueText->SetText(strCombine,originValue != changeUp ? NS_UITEXTCOLOR::PLUSOPTION : NS_UITEXTCOLOR::WHITE);
	}
	else
	{
		strCombine.Format("%d",originValue);
		pUpValueText->SetText(strCombine);
	}
}

void RnStatInfoTap::SetFireResist( const GLCHARLOGIC& sCharData )
{
	CString strCombine;
	strCombine.Format( "%d", sCharData.m_sSUMRESIST_SKILL.nFire );
	m_pFireResistValue->SetText(strCombine);
}

void RnStatInfoTap::SetColdResist( const GLCHARLOGIC& sCharData )
{
	CString strCombine;
	strCombine.Format( "%d", sCharData.m_sSUMRESIST_SKILL.nIce );
	m_pColdResistValue->SetText(strCombine);
}

void RnStatInfoTap::SetElectricResist( const GLCHARLOGIC& sCharData )
{
	CString strCombine;
	strCombine.Format( "%d", sCharData.m_sSUMRESIST_SKILL.nElectric );
	m_pElectricResistValue->SetText(strCombine);
}

void RnStatInfoTap::SetPoisonResist( const GLCHARLOGIC& sCharData )
{
	CString strCombine;
	strCombine.Format( "%d", sCharData.m_sSUMRESIST_SKILL.nPoison );
	m_pPoisonResistValue->SetText(strCombine);
}

void RnStatInfoTap::SetSpiritResist( const GLCHARLOGIC& sCharData )
{
	CString strCombine;
	strCombine.Format( "%d", sCharData.m_sSUMRESIST_SKILL.nSpirit );
	m_pSpiritResistValue->SetText(strCombine);
}





