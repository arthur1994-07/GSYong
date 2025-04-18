
g_nPetColor_ColorOrig = 0		-- 원본 컬러
g_nPetColor_ColorTemp = 0		-- 변경 완료된 컬러
g_nPetColor_Color = 0			-- 변경중인 컬러

-- 윈도우 사이즈 애니메이션을 위해 각 패널별 사이즈 저장
g_nPetColor_SettingWidth = 0 
g_nPetColor_SettingHeight = 0

g_nPetColor_ColorWidth = 0
g_nPetColor_ColorHeight = 0

-- 펫 컬러 변경 로드 완료
function LoadPetColor()

	SetEvent( WIDGET_ID_PETCOLOR, EVENT_WIDGET_CLOSE, "ClosePetColor", "", "" )
	
	-- 컬러 패널 설정 --------------------------------------------------------------------------------------	
	-- 윈도우 사이즈 애니메이션을 위해 Width/Height 저장
	g_nPetColor_ColorWidth = GetInt( WIDGET_ID_PETCOLOR, "width", "panelColor" ) +  10
	g_nPetColor_ColorHeight = GetInt( WIDGET_ID_PETCOLOR, "height", "panelColor" ) + 29
	
	-- 컬러 패널 숨기기
	SetBool( WIDGET_ID_PETCOLOR, "visible", false, "panelColor" )
	
	-- 확인
	SetString( WIDGET_ID_PETCOLOR, "label", GetGameWordText( "GAMBLE_OKCANCEL", 0 ), "panelColor.btnOK" )
	SetEvent( WIDGET_ID_PETCOLOR, EVENT_BUTTON_CLICK, "Ev_PetColor_ColorOK", "", "panelColor.btnOK" )
	-- 취소
	SetString( WIDGET_ID_PETCOLOR, "label", GetGameWordText( "GAMBLE_OKCANCEL", 1 ), "panelColor.btnCancel" )
	SetEvent( WIDGET_ID_PETCOLOR, EVENT_BUTTON_CLICK, "Ev_PetColor_ColorCancel", "", "panelColor.btnCancel" )
	
	-- 컬러 변경 이벤트
	SetEvent( WIDGET_ID_PETCOLOR, EVENT_COLOR_CHANGE, "Ev_PetColor_ChangeColor", "strValue", "panelColor.colorPicker" )
	
	
	-- 설정 패널 설정 --------------------------------------------------------------------------------------	
	-- 윈도우 사이즈 애니메이션을 위해 Width/Height 저장
	g_nPetColor_SettingWidth = GetInt( WIDGET_ID_PETCOLOR, "width", "panelSetting" ) + 10
	g_nPetColor_SettingHeight = GetInt( WIDGET_ID_PETCOLOR, "height", "panelSetting" ) + 29
	
	-- 컬러 패널이 설정 패널보다 커서 최초 윈도우 사이즈가 크게 잡히니 설정 패널 사이즈로 강제 설정
	SetInt( WIDGET_ID_PETCOLOR, "customWidth", g_nPetColor_SettingWidth, "" )
	SetInt( WIDGET_ID_PETCOLOR, "customHeight", g_nPetColor_SettingHeight, "" )
	
	-- 펫 컬러
	SetString( WIDGET_ID_PETCOLOR, "text", GetGameInternalText( "PET_COLOR_LABEL", 0 ), "panelSetting.labelName" )
	
	-- 컬러 뷰 클릭
	SetEvent( WIDGET_ID_PETCOLOR, EVENT_BUTTON_CLICK, "Ev_PetColor_SelectColor", "", "panelSetting.colorView" )
	
	-- 버튼 설정 ---------------------
	-- 랜덤
	SetString( WIDGET_ID_PETCOLOR, "label",
		GetGameWordText( "COLORSELECTOR_PANNEL_PAGE_BUTTON", 0 ), "panelSetting.btnRandom" )
	SetEvent( WIDGET_ID_PETCOLOR, EVENT_BUTTON_CLICK, "Ev_PetColor_btnRandom", "", "panelSetting.btnRandom" )
	-- 리셋
	SetString( WIDGET_ID_PETCOLOR, "label",
		GetGameWordText( "COLORSELECTOR_PANNEL_PAGE_BUTTON", 1 ), "panelSetting.btnReset" )
	SetEvent( WIDGET_ID_PETCOLOR, EVENT_BUTTON_CLICK, "Ev_PetColor_btnReset", "", "panelSetting.btnReset" )
	-- 취소
	SetString( WIDGET_ID_PETCOLOR, "label",
		GetGameWordText( "COLORSELECTOR_PANNEL_PAGE_BUTTON", 2 ), "panelSetting.btnCancel" )
	SetEvent( WIDGET_ID_PETCOLOR, EVENT_BUTTON_CLICK, "Ev_PetColor_btnCancel", "", "panelSetting.btnCancel" )
	-- 확인
	SetString( WIDGET_ID_PETCOLOR, "label",
		GetGameWordText( "COLORSELECTOR_PANNEL_PAGE_BUTTON", 3 ), "panelSetting.btnOK" )
	SetEvent( WIDGET_ID_PETCOLOR, EVENT_BUTTON_CLICK, "Ev_PetColor_btnOK", "", "panelSetting.btnOK" )

end

-- 펫 컬러 오픈
function OpenPetColor()

	-- 컬러 값 설정
	g_nPetColor_ColorOrig = g_Pet:GetColor()
	g_nPetColor_ColorTemp = g_nPetColor_ColorOrig
	g_nPetColor_Color = g_nPetColor_ColorOrig
	
	SetUInt( WIDGET_ID_PETCOLOR, "color", g_nPetColor_ColorOrig, "panelSetting.colorView" )

end

-- 닫기
function ClosePetColor()

	-- 원본 컬러로 되돌리기
	g_Pet:SetColor( g_nPetColor_ColorOrig )

end

-- 컬러 패널 활성/비활성
function PetColor_SetVisibleColorPanel( bVisible )

	local strColorAnimComplete = ""
	local strColorAnimAlpha = ""
	
	local strSettingAnimComplete = ""
	local strSettingAnimAlpha = ""
	
	local strNewSize = ""
	
	if true == bVisible then
	
		SetBool( WIDGET_ID_PETCOLOR, "alpha", 0, "panelColor" )
		SetBool( WIDGET_ID_PETCOLOR, "visible", true, "panelColor" )
		
		strColorAnimComplete = ""
		strColorAnimAlpha = "1"
		
		strSettingAnimComplete = "Ev_PetColor_SettingPanelVisibleOff"
		strSettingAnimAlpha = "0"
		
		strNewSize = tostring( g_nPetColor_ColorWidth ) .. ";" .. tostring( g_nPetColor_ColorHeight )
		
		SetUInt( WIDGET_ID_PETCOLOR, "color", g_nPetColor_ColorTemp, "panelColor.colorPicker" )
	
	else
	
		SetBool( WIDGET_ID_PETCOLOR, "alpha", 0, "panelSetting" )
		SetBool( WIDGET_ID_PETCOLOR, "visible", true, "panelSetting" )
		
		strColorAnimComplete = "Ev_PetColor_ColorPanelVisibleOff"
		strColorAnimAlpha = "0"
		
		strSettingAnimComplete = ""
		strSettingAnimAlpha = "1"
		
		strNewSize = tostring( g_nPetColor_SettingWidth ) .. ";" .. tostring( g_nPetColor_SettingHeight )
	
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
	SetTween( WIDGET_ID_PETCOLOR, tbQuickSet, "alpha", strColorAnimAlpha, "panelColor" )
	
	-- 설정 패널 애니메이션
	tbQuickSet[ "onComplete" ] = strSettingAnimComplete
	SetTween( WIDGET_ID_PETCOLOR, tbQuickSet, "alpha", strSettingAnimAlpha, "panelSetting" )
	
	-- 윈도우 사이즈 애니메이션	
	tbQuickSet[ "time" ] = 250
	tbQuickSet[ "onComplete" ] = ""
	SetTween( WIDGET_ID_PETCOLOR, tbQuickSet, "customWidth;customHeight;", strNewSize, "" )

end

------------------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 펫 컬러 변경 카드 사용
function PetColor_ItemUse()

	OpenMsgBox( MSG_BOX_TYPE_OK, GetGameInternalText( "MODAL_PETCOLOR_INFO", 0 ), Ev_PetColor_UseOK )

end

------------------------------------------------------------------------------------------------------------------
-- 이벤트

-- 펫 컬러 변경 카드 사용 OK
function Ev_PetColor_UseOK()

	if false == IsOpenWidget( WIDGET_ID_PETCOLOR ) then
		OpenWidget( WIDGET_ID_PETCOLOR )
	end
	
	Event_MsgBox_Close()

end

-- 컬러 패널 확인
function Ev_PetColor_ColorOK()

	PetColor_SetVisibleColorPanel( false )
	
	-- 변경된 컬러 저장
	g_nPetColor_ColorTemp = g_nPetColor_Color
	g_Pet:SetColor( g_nPetColor_Color )
	
	SetUInt( WIDGET_ID_PETCOLOR, "color", g_nPetColor_ColorTemp, "panelSetting.colorView" )
	
end

-- 컬러 패널 취소
function Ev_PetColor_ColorCancel()

	PetColor_SetVisibleColorPanel( false )
	
	-- 변경 전 컬러로 되돌리기
	g_nPetColor_Color = g_nPetColor_ColorTemp
	g_Pet:SetColor( g_nPetColor_Color )
	
	SetUInt( WIDGET_ID_PETCOLOR, "color", g_nPetColor_ColorTemp, "panelSetting.colorView" )
	
end

-- 컬러 변경
function Ev_PetColor_ChangeColor( strColor )

	-- 변경중인 컬러 저장하고 펫 컬러 변경 
	g_nPetColor_Color = tonumber(strColor)
	g_Pet:SetColor( g_nPetColor_Color )

end

-- 설정 패널 visible off
function Ev_PetColor_SettingPanelVisibleOff()
	SetBool( WIDGET_ID_PETCOLOR, "visible", false, "panelSetting" )
end

-- 컬러 패널 visible off
function Ev_PetColor_ColorPanelVisibleOff()
	SetBool( WIDGET_ID_PETCOLOR, "visible", false, "panelColor" )
end

-- 컬러 뷰 클릭
function Ev_PetColor_SelectColor()
	PetColor_SetVisibleColorPanel( true )
end

-- 랜덤
function Ev_PetColor_btnRandom()

	-- 랜덤 컬러 가져와 펫 컬러 변경
	g_nPetColor_ColorTemp = GetRandomColor( false )
	g_nPetColor_Color = g_nPetColor_ColorTemp
	
	g_Pet:SetColor( g_nPetColor_ColorTemp )
	
	SetUInt( WIDGET_ID_PETCOLOR, "color", g_nPetColor_ColorTemp, "panelSetting.colorView" )

end

-- 리셋
function Ev_PetColor_btnReset()

	-- 원본 컬러로 초기화
	g_nPetColor_ColorTemp = g_nPetColor_ColorOrig
	g_nPetColor_Color = g_nPetColor_ColorOrig
	
	-- 펫 컬러 변경
	g_Pet:SetColor( g_nPetColor_ColorOrig )
	
	SetUInt( WIDGET_ID_PETCOLOR, "color", g_nPetColor_ColorOrig, "panelSetting.colorView" )

end

-- 취소
function Ev_PetColor_btnCancel()

	CloseWidget( WIDGET_ID_PETCOLOR )

end

-- 확인
function Ev_PetColor_btnOK()

	-- 설정한 컬러로 서버에 변경 요청하고 UI 끄기
	g_Msg:CommitColorPet( g_nPetColor_ColorTemp )
	CloseWidget( WIDGET_ID_PETCOLOR )

end