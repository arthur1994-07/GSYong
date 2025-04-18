
g_nHairColor_ColorOrig = 0		-- 원본 컬러
g_nHairColor_ColorTemp = 0		-- 변경 완료된 컬러
g_nHairColor_Color = 0			-- 변경중인 컬러

-- 윈도우 사이즈 애니메이션을 위해 각 패널별 사이즈 저장
g_nHairColor_SettingWidth = 0 
g_nHairColor_SettingHeight = 0

g_nHairColor_ColorWidth = 0
g_nHairColor_ColorHeight = 0

-- 로드 완료
function LoadHairColor()

	SetEvent( WIDGET_ID_HAIRCOLOR, EVENT_WIDGET_CLOSE, "CloseHairColor", "", "" )
	
	-- 컬러 패널 설정 --------------------------------------------------------------------------------------	
	-- 윈도우 사이즈 애니메이션을 위해 Width/Height 저장
	g_nHairColor_ColorWidth = GetInt( WIDGET_ID_HAIRCOLOR, "width", "panelColor" ) +  10
	g_nHairColor_ColorHeight = GetInt( WIDGET_ID_HAIRCOLOR, "height", "panelColor" ) + 29
	
	-- 컬러 패널 숨기기
	SetBool( WIDGET_ID_HAIRCOLOR, "visible", false, "panelColor" )
	
	-- 확인
	SetString( WIDGET_ID_HAIRCOLOR, "label", GetGameWordText( "GAMBLE_OKCANCEL", 0 ), "panelColor.btnOK" )
	SetEvent( WIDGET_ID_HAIRCOLOR, EVENT_BUTTON_CLICK, "Ev_HairColor_ColorOK", "", "panelColor.btnOK" )
	-- 취소
	SetString( WIDGET_ID_HAIRCOLOR, "label", GetGameWordText( "GAMBLE_OKCANCEL", 1 ), "panelColor.btnCancel" )
	SetEvent( WIDGET_ID_HAIRCOLOR, EVENT_BUTTON_CLICK, "Ev_HairColor_ColorCancel", "", "panelColor.btnCancel" )
	
	-- 컬러 변경 이벤트
	SetEvent( WIDGET_ID_HAIRCOLOR, EVENT_COLOR_CHANGE, "Ev_HairColor_ChangeColor", "strValue", "panelColor.colorPicker" )
	
	
	-- 설정 패널 설정 --------------------------------------------------------------------------------------	
	-- 윈도우 사이즈 애니메이션을 위해 Width/Height 저장
	g_nHairColor_SettingWidth = GetInt( WIDGET_ID_HAIRCOLOR, "width", "panelSetting" ) + 10
	g_nHairColor_SettingHeight = GetInt( WIDGET_ID_HAIRCOLOR, "height", "panelSetting" ) + 29
	
	-- 컬러 패널이 설정 패널보다 커서 최초 윈도우 사이즈가 크게 잡히니 설정 패널 사이즈로 강제 설정
	SetInt( WIDGET_ID_HAIRCOLOR, "customWidth", g_nHairColor_SettingWidth, "" )
	SetInt( WIDGET_ID_HAIRCOLOR, "customHeight", g_nHairColor_SettingHeight, "" )
	
	-- 머리색
	SetString( WIDGET_ID_HAIRCOLOR, "text", GetGameInternalText( "HAIR_COLOR_LABEL", 0 ), "panelSetting.labelName" )
	
	-- 컬러 뷰 클릭
	SetEvent( WIDGET_ID_HAIRCOLOR, EVENT_BUTTON_CLICK, "Ev_HairColor_SelectColor", "", "panelSetting.colorView" )
	
	-- 버튼 설정 ---------------------
	-- 랜덤
	SetString( WIDGET_ID_HAIRCOLOR, "label",
		GetGameWordText( "COLORSELECTOR_PANNEL_PAGE_BUTTON", 0 ), "panelSetting.btnRandom" )
	SetEvent( WIDGET_ID_HAIRCOLOR, EVENT_BUTTON_CLICK, "Ev_HairColor_btnRandom", "", "panelSetting.btnRandom" )
	-- 리셋
	SetString( WIDGET_ID_HAIRCOLOR, "label",
		GetGameWordText( "COLORSELECTOR_PANNEL_PAGE_BUTTON", 1 ), "panelSetting.btnReset" )
	SetEvent( WIDGET_ID_HAIRCOLOR, EVENT_BUTTON_CLICK, "Ev_HairColor_btnReset", "", "panelSetting.btnReset" )
	-- 취소
	SetString( WIDGET_ID_HAIRCOLOR, "label",
		GetGameWordText( "COLORSELECTOR_PANNEL_PAGE_BUTTON", 2 ), "panelSetting.btnCancel" )
	SetEvent( WIDGET_ID_HAIRCOLOR, EVENT_BUTTON_CLICK, "Ev_HairColor_btnCancel", "", "panelSetting.btnCancel" )
	-- 확인
	SetString( WIDGET_ID_HAIRCOLOR, "label",
		GetGameWordText( "COLORSELECTOR_PANNEL_PAGE_BUTTON", 3 ), "panelSetting.btnOK" )
	SetEvent( WIDGET_ID_HAIRCOLOR, EVENT_BUTTON_CLICK, "Ev_HairColor_btnOK", "", "panelSetting.btnOK" )

end

-- 오픈
function OpenHairColor()

	-- 컬러 값 설정
	g_nHairColor_ColorOrig = ColorWORDtoDWORD( g_MyChar.m_wHairColor )
	g_nHairColor_ColorTemp = g_nHairColor_ColorOrig
	g_nHairColor_Color = g_nHairColor_ColorOrig
	
	SetUInt( WIDGET_ID_HAIRCOLOR, "color", g_nHairColor_ColorOrig, "panelSetting.colorView" )

end

-- 닫기
function CloseHairColor()

	-- 원본 컬러로 되돌리기
	g_MyChar:HairColorChange( ColorDWORDtoWORD( g_nHairColor_ColorOrig ) )

end

-- 컬러 패널 활성/비활성
function HairColor_SetVisibleColorPanel( bVisible )

	local strColorAnimComplete = ""
	local strColorAnimAlpha = ""
	
	local strSettingAnimComplete = ""
	local strSettingAnimAlpha = ""
	
	local strNewSize = ""
	
	if true == bVisible then
	
		SetBool( WIDGET_ID_HAIRCOLOR, "alpha", 0, "panelColor" )
		SetBool( WIDGET_ID_HAIRCOLOR, "visible", true, "panelColor" )
		
		strColorAnimComplete = ""
		strColorAnimAlpha = "1"
		
		strSettingAnimComplete = "Ev_HairColor_SettingPanelVisibleOff"
		strSettingAnimAlpha = "0"
		
		strNewSize = tostring( g_nHairColor_ColorWidth ) .. ";" .. tostring( g_nHairColor_ColorHeight )
		
		SetUInt( WIDGET_ID_HAIRCOLOR, "color", g_nHairColor_ColorTemp, "panelColor.colorPicker" )
	
	else
	
		SetBool( WIDGET_ID_HAIRCOLOR, "alpha", 0, "panelSetting" )
		SetBool( WIDGET_ID_HAIRCOLOR, "visible", true, "panelSetting" )
		
		strColorAnimComplete = "Ev_HairColor_ColorPanelVisibleOff"
		strColorAnimAlpha = "0"
		
		strSettingAnimComplete = ""
		strSettingAnimAlpha = "1"
		
		strNewSize = tostring( g_nHairColor_SettingWidth ) .. ";" .. tostring( g_nHairColor_SettingHeight )
	
	end
	
	-- 컬러 패널 애니메이션
	local tbQuickSet = 
	{
		time = 500,
		delay = 0,
		ease = EASE_REGULAR_OUT,
		loop = false,
		onComplete = strColorAnimComplete
	}
	SetTween( WIDGET_ID_HAIRCOLOR, tbQuickSet, "alpha", strColorAnimAlpha, "panelColor" )
	
	-- 설정 패널 애니메이션
	tbQuickSet[ "onComplete" ] = strSettingAnimComplete
	SetTween( WIDGET_ID_HAIRCOLOR, tbQuickSet, "alpha", strSettingAnimAlpha, "panelSetting" )
	
	-- 윈도우 사이즈 애니메이션	
	tbQuickSet[ "time" ] = 250
	tbQuickSet[ "onComplete" ] = ""
	SetTween( WIDGET_ID_HAIRCOLOR, tbQuickSet, "customWidth;customHeight;", strNewSize, "" )

end

--------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 헤어 컬러 변경 카드 사용
function HairColor_ItemUse()

	OpenMsgBox( MSG_BOX_TYPE_OK, GetGameInternalText( "MODAL_HAIRCOLOR_INFO", 0 ), Ev_HairColor_UseOK )

end

--------------------------------------------------------------------------------------------------------
-- 이벤트

-- 헤어 컬러 변경 카드 사용 OK
function Ev_HairColor_UseOK()

	if false == IsOpenWidget( WIDGET_ID_HAIRCOLOR ) then
		OpenWidget( WIDGET_ID_HAIRCOLOR )
	end
	
	Event_MsgBox_Close()

end

--  컬러 패널 확인
function Ev_HairColor_ColorOK()

	HairColor_SetVisibleColorPanel( false )
	
	-- 변경된 컬러 저장
	g_nHairColor_ColorTemp = g_nHairColor_Color
	g_MyChar:HairColorChange( ColorDWORDtoWORD( g_nHairColor_Color ) )
	
	SetUInt( WIDGET_ID_HAIRCOLOR, "color", g_nHairColor_ColorTemp, "panelSetting.colorView" )

end

-- 컬러 패널 취소
function Ev_HairColor_ColorCancel()

	HairColor_SetVisibleColorPanel( false )
	
	-- 변경 전 컬러로 되돌리기
	g_nHairColor_Color = g_nHairColor_ColorTemp
	g_MyChar:HairColorChange( ColorDWORDtoWORD( g_nHairColor_Color ) )
	
	SetUInt( WIDGET_ID_HAIRCOLOR, "color", g_nHairColor_ColorTemp, "panelSetting.colorView" )

end

-- 컬러 변경
function Ev_HairColor_ChangeColor( strColor )

	-- 변경중인 컬러 저장하고 헤어 컬러 변경
	g_nHairColor_Color = tonumber( strColor )
	g_MyChar:HairColorChange( ColorDWORDtoWORD( g_nHairColor_Color ) )

end

-- 설정 패널 visible off
function Ev_HairColor_SettingPanelVisibleOff()

	SetBool( WIDGET_ID_HAIRCOLOR, "visible", false, "panelSetting" )
	
end

-- 컬러 패널 visible off
function Ev_HairColor_ColorPanelVisibleOff()

	SetBool( WIDGET_ID_HAIRCOLOR, "visible", false, "panelColor" )
	
end

-- 컬러 뷰 클릭
function Ev_HairColor_SelectColor()

	HairColor_SetVisibleColorPanel( true )

end

-- 랜덤 컬러
function Ev_HairColor_btnRandom()

	-- 랜덤 컬러 가져와 헤어 컬러 변경
	g_nHairColor_ColorTemp = GetRandomColor( false )
	g_nHairColor_Color = g_nHairColor_ColorTemp
	
	g_MyChar:HairColorChange( ColorDWORDtoWORD( g_nHairColor_ColorTemp ) )
	
	SetUInt( WIDGET_ID_HAIRCOLOR, "color", g_nHairColor_ColorTemp, "panelSetting.colorView" )

end

-- 컬러 초기화
function Ev_HairColor_btnReset()

	-- 원본 컬러로 초기화
	g_nHairColor_ColorTemp = g_nHairColor_ColorOrig
	g_nHairColor_Color = g_nHairColor_ColorOrig
	
	-- 헤어 컬러 변경
	g_MyChar:HairColorChange( ColorDWORDtoWORD( g_nHairColor_ColorOrig ) )
	
	SetUInt( WIDGET_ID_HAIRCOLOR, "color", g_nHairColor_ColorOrig, "panelSetting.colorView" )

end

-- 취소
function Ev_HairColor_btnCancel()

	CloseWidget( WIDGET_ID_HAIRCOLOR )

end

-- 확인
function Ev_HairColor_btnOK()

	-- 설정한 컬러로 서버에 변경 요청하고 UI 끄기
	g_Msg:ReqInvenHairColorChange( ColorDWORDtoWORD( g_nHairColor_ColorTemp ) )
	CloseWidget( WIDGET_ID_HAIRCOLOR )

end