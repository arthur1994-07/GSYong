
g_tbVehicleColor_ColorListOrig = {}			-- 탈것 원본 컬러 리스트
g_tbVehicleColor_ColorList = {}				-- 탈것 변경 컬러 리스트
g_tbVehicleColor_ColorListTemp = {}			-- 탈것 변경 완료 컬러 리스트

g_nVehicleColor_ChangeColorIndex = -1		-- 현재 변경중인 컬러 인덱스
g_bVehicelColor_isSubColor = false			-- 변경중인 컬러가 메인 컬러인지 서브 컬러인지

-- 탈것 색상 변경 로드 완료
function LoadVehicleColor()

	SetEvent( WIDGET_ID_VEHICLECOLOR, EVENT_WIDGET_CLOSE, "CloseVehicleColor", "", "" )

	-- 컬러 패널 설정 ------------------------------------------------------------------------------------	
	-- 컬러 패널 숨기기
	SetBool( WIDGET_ID_VEHICLECOLOR, "visible", false, "panelColor" )
	
	-- 확인
	SetString( WIDGET_ID_VEHICLECOLOR, "label", GetGameWordText( "GAMBLE_OKCANCEL", 0 ), "panelColor.btnOK" )
	SetEvent( WIDGET_ID_VEHICLECOLOR, EVENT_BUTTON_CLICK, "Ev_VehicleColor_ColorOK", "", "panelColor.btnOK" )
	-- 취소
	SetString( WIDGET_ID_VEHICLECOLOR, "label", GetGameWordText( "GAMBLE_OKCANCEL", 1 ), "panelColor.btnCancel" )
	SetEvent( WIDGET_ID_VEHICLECOLOR, EVENT_BUTTON_CLICK, "Ev_VehicleColor_ColorCancel", "", "panelColor.btnCancel" )
	
	-- 컬러 변경 이벤트
	SetEvent( WIDGET_ID_VEHICLECOLOR, EVENT_COLOR_CHANGE, "Ev_VehicleColor_ChangeColor", "strValue", "panelColor.colorPicker" )

	-- 설정 패널 설정 ------------------------------------------------------------------------------------	
	-- 컬러 이름 설정
	for i = 0, 5, 1 do
	
		local strLineInst = "panelSetting.line" .. tostring( i + 1 )
		local strLineEv = "Line" .. tostring( i + 1 )
	
		SetString( WIDGET_ID_VEHICLECOLOR, "text",
			GetGameWordText( "CAR_COLOR_CHANGE_TEXT", i ), strLineInst .. ".labelName" )
			
		SetEvent( WIDGET_ID_VEHICLECOLOR, EVENT_BUTTON_CLICK, "Ev_VehicleColor_" .. strLineEv .. "Color1", "", 
			strLineInst .. ".color1" )
		SetEvent( WIDGET_ID_VEHICLECOLOR, EVENT_BUTTON_CLICK, "Ev_VehicleColor_" .. strLineEv .. "Color2", "", 
			strLineInst .. ".color2" )
		
	end
	
	-- 버튼 설정 -------------------------------------------------------------------	
	-- 랜덤
	SetString( WIDGET_ID_VEHICLECOLOR, "label",
		GetGameWordText( "COLORSELECTOR_PANNEL_PAGE_BUTTON", 0 ), "panelSetting.btnRandom" )
	SetEvent( WIDGET_ID_VEHICLECOLOR, EVENT_BUTTON_CLICK, "Ev_VehicleColor_btnRandom", "", "panelSetting.btnRandom" )
	-- 리셋
	SetString( WIDGET_ID_VEHICLECOLOR, "label",
		GetGameWordText( "COLORSELECTOR_PANNEL_PAGE_BUTTON", 1 ), "panelSetting.btnReset" )
	SetEvent( WIDGET_ID_VEHICLECOLOR, EVENT_BUTTON_CLICK, "Ev_VehicleColor_btnReset", "", "panelSetting.btnReset" )
	-- 취소
	SetString( WIDGET_ID_VEHICLECOLOR, "label",
		GetGameWordText( "COLORSELECTOR_PANNEL_PAGE_BUTTON", 2 ), "panelSetting.btnCancel" )
	SetEvent( WIDGET_ID_VEHICLECOLOR, EVENT_BUTTON_CLICK, "Ev_VehicleColor_btnCancel", "", "panelSetting.btnCancel" )
	-- 확인
	SetString( WIDGET_ID_VEHICLECOLOR, "label",
		GetGameWordText( "COLORSELECTOR_PANNEL_PAGE_BUTTON", 3 ), "panelSetting.btnOK" )
	SetEvent( WIDGET_ID_VEHICLECOLOR, EVENT_BUTTON_CLICK, "Ev_VehicleColor_btnOK", "", "panelSetting.btnOK" )

end

-- 탈것 색상 변경창 오픈
function OpenVehicleColor()

	-- 컬러 설정
	g_tbVehicleColor_ColorListOrig = g_MyChar:GetVehicleColor()
	
	-- 원본 컬러 테이블 복사
	g_tbVehicleColor_ColorListTemp = CopyTable( g_tbVehicleColor_ColorListOrig )
	g_tbVehicleColor_ColorList = CopyTable( g_tbVehicleColor_ColorListOrig )
	for value in pairs( g_tbVehicleColor_ColorList ) do
	
		local tbColor = g_tbVehicleColor_ColorList[ value ]
		local strLine = "panelSetting.line" .. tostring( value ) .. "."
		
		SetUInt( WIDGET_ID_VEHICLECOLOR, "color", tbColor[ "dwMainColor" ], strLine .. "color1" )
		SetUInt( WIDGET_ID_VEHICLECOLOR, "color", tbColor[ "dwSubColor" ], strLine .. "color2" )
		SetBool( WIDGET_ID_VEHICLECOLOR, "enabled", tbColor[ "bEnable" ], strLine .. "color1" )
		SetBool( WIDGET_ID_VEHICLECOLOR, "enabled", tbColor[ "bEnable" ], strLine .. "color2" )
		
	end

end

-- 닫기
function CloseVehicleColor()

	-- 탈것 원본 컬러로 돌려놓기
	g_MyChar:SetVehicleColor( g_tbVehicleColor_ColorListOrig )

end

-- 컬러 패널 활성/비활성
function VehicleColor_SetVisibleColorPanel( bVisible )

	local strColorAnimComplete = ""
	local strColorAnimAlpha = ""
	
	local strSettingAnimComplete = ""
	local strSettingAnimAlpha = ""

	if true == bVisible then
	
		SetInt( WIDGET_ID_VEHICLECOLOR, "alpha", 0, "panelColor" )
		SetBool( WIDGET_ID_VEHICLECOLOR, "visible", true, "panelColor" )
		
		strColorAnimComplete = ""
		strColorAnimAlpha = "1"
		
		strSettingAnimComplete = "Ev_VehicleColor_SettingPanelAnimVisibleOff"
		strSettingAnimAlpha = "0"
		
		-- 컬러 패널 켜질 경우 컬러 픽커 색 설정하기
		VehicleColor_SetColorPicker()
	
	else
	
		SetInt( WIDGET_ID_VEHICLECOLOR, "alpha", 0, "panelSetting" )
		SetBool( WIDGET_ID_VEHICLECOLOR, "visible", true, "panelSetting" )
	
		strColorAnimComplete = "Ev_VehicleColor_ColorPanelAnimVisibleOff"
		strColorAnimAlpha = "0"
		
		strSettingAnimComplete = ""
		strSettingAnimAlpha = "1"

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
	SetTween( WIDGET_ID_VEHICLECOLOR, tbQuickSet, "alpha", strColorAnimAlpha, "panelColor" )
	
	-- 설정 패널 애니메이션
	tbQuickSet[ "onComplete" ] = strSettingAnimComplete
	SetTween( WIDGET_ID_VEHICLECOLOR, tbQuickSet, "alpha", strSettingAnimAlpha, "panelSetting" )

end

-- 컬러 픽커 색 설정
function VehicleColor_SetColorPicker()

	-- 변경하기로 선택한 컬러
	local tbColor = g_tbVehicleColor_ColorList[ g_nVehicleColor_ChangeColorIndex ]
	if nil == tbColor then
		return
	end
	
	-- 메인 컬러? 서브 컬러?
	local nColor = 0
	if false == g_bVehicelColor_isSubColor then
		nColor = tbColor[ "dwMainColor" ]
	else
		nColor = tbColor[ "dwSubColor" ]
	end
	
	-- 해당 컬러로 컬러 픽커 색 설정
	SetUInt( WIDGET_ID_VEHICLECOLOR, "color", nColor, "panelColor.colorPicker" )

end

--------------------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 탈것 색상 변경 카드 사용
function VehicleColor_ItemUse( strName )

	local str = StringFormat( GetGameInternalText( "MODAL_VEHICLE_COLOR_INFO", 0 ), strName )
	OpenMsgBox( MSG_BOX_TYPE_OK, str, Ev_VechicleColor_UseOK )

end

--------------------------------------------------------------------------------------------------------------------
-- 이벤트

-- 탈것 색상 변경 카드 사용 OK
function Ev_VechicleColor_UseOK()

	if false == IsOpenWidget( WIDGET_ID_VEHICLECOLOR ) then
		OpenWidget( WIDGET_ID_VEHICLECOLOR )
	end
	
	Event_MsgBox_Close()

end

-- 컬러 패널 visible off 애니메이션 완료
function Ev_VehicleColor_ColorPanelAnimVisibleOff()
	SetBool( WIDGET_ID_VEHICLECOLOR, "visible", false, "panelColor" )
end

-- 설정 패널 visible off 애니메이션 완료
function Ev_VehicleColor_SettingPanelAnimVisibleOff()
	SetBool( WIDGET_ID_VEHICLECOLOR, "visible", false, "panelSetting" )
end

-- 랜덤 버튼
function Ev_VehicleColor_btnRandom()

	-- 랜덤 컬러 설정
	for value in pairs( g_tbVehicleColor_ColorListTemp ) do
	
		local tbColor = g_tbVehicleColor_ColorListTemp[ value ]
		
		-- 컬러 변경 가능한 놈만
		if true == tbColor[ "bEnable" ] then
		
			local strLine = "panelSetting.line" .. tostring( value ) .. "."
			
			-- 랜덤 컬러
			tbColor[ "dwMainColor" ] = GetRandomColor( false )
			tbColor[ "dwSubColor" ] = GetRandomColor( false )
		
			-- 컬러 뷰, 색 다시 설정
			SetUInt( WIDGET_ID_VEHICLECOLOR, "color", tbColor[ "dwMainColor" ], strLine .. "color1" )
			SetUInt( WIDGET_ID_VEHICLECOLOR, "color", tbColor[ "dwSubColor" ], strLine .. "color2" )
		
		end
		
	end
	
	-- 변경중인 컬러도 랜덤 컬러 적용
	g_tbVehicleColor_ColorList = CopyTable( g_tbVehicleColor_ColorListTemp )
	
	-- 탈것 컬러 변경
	g_MyChar:SetVehicleColor( g_tbVehicleColor_ColorListTemp )

end

-- 리셋 버튼
function Ev_VehicleColor_btnReset()

	-- 탈것 원본 컬러로 돌려놓기
	g_MyChar:SetVehicleColor( g_tbVehicleColor_ColorListOrig )
	
	-- 변경 완료 컬러도 원본으로 초기화
	g_tbVehicleColor_ColorListTemp = CopyTable( g_tbVehicleColor_ColorListOrig )
	
	-- 컬러 뷰, 원본 컬러로 다시 설정
	for value in pairs( g_tbVehicleColor_ColorListOrig ) do
	
		local tbColor = g_tbVehicleColor_ColorListOrig[ value ]
		local strLine = "panelSetting.line" .. tostring( value ) .. "."
		
		SetUInt( WIDGET_ID_VEHICLECOLOR, "color", tbColor[ "dwMainColor" ], strLine .. "color1" )
		SetUInt( WIDGET_ID_VEHICLECOLOR, "color", tbColor[ "dwSubColor" ], strLine .. "color2" )
		
	end

end

-- 취소 버튼
function Ev_VehicleColor_btnCancel()

	CloseWidget( WIDGET_ID_VEHICLECOLOR )

end

-- 확인 버튼
function Ev_VehicleColor_btnOK()

	-- 서버에 탈것 컬러 변경 요청하고 UI 끄기
	g_Msg:CommitVehicleColor( g_tbVehicleColor_ColorListTemp )
	CloseWidget( WIDGET_ID_VEHICLECOLOR )

end

-- 컬러 변경
function Ev_VehicleColor_ChangeColor( strColor )

	if -1 == g_nVehicleColor_ChangeColorIndex then
		return
	end
	
	-- 변경하려고 선택한 컬러
	local tbColor = g_tbVehicleColor_ColorList[ g_nVehicleColor_ChangeColorIndex ]
	if nil == tbColor then
		return
	end
	
	-- 메인 컬러?
	if false == g_bVehicelColor_isSubColor then
		tbColor[ "dwMainColor" ] = tonumber(strColor) 
	-- 서브 컬러?
	else
		tbColor[ "dwSubColor" ] = tonumber(strColor)
	end
	
	-- 탈것 색 변경하기
	g_MyChar:SetVehicleColor( g_tbVehicleColor_ColorList )

end

-- 컬러 선택 확인
function Ev_VehicleColor_ColorOK()

	-- 컬러 패널 끄기
	VehicleColor_SetVisibleColorPanel( false )
	
	-- 컬러 변경이 끝났으니 저장
	g_tbVehicleColor_ColorListTemp = CopyTable( g_tbVehicleColor_ColorList )
	
	-- 탈것 색 변경하기
	g_MyChar:SetVehicleColor( g_tbVehicleColor_ColorListTemp )
	
	if -1 == g_nVehicleColor_ChangeColorIndex then
		return
	end
	
	-- 컬러 뷰 색 변경된 색으로 변경
	local tbColor = g_tbVehicleColor_ColorListTemp[ g_nVehicleColor_ChangeColorIndex ]
	local strLine = "panelSetting.line" .. tostring( g_nVehicleColor_ChangeColorIndex ) .. "."
	
	if false == g_bVehicelColor_isSubColor then
		SetUInt( WIDGET_ID_VEHICLECOLOR, "color", tbColor[ "dwMainColor" ], strLine .. "color1" )
	else
		SetUInt( WIDGET_ID_VEHICLECOLOR, "color", tbColor[ "dwSubColor" ], strLine .. "color2" )
	end
	
end

-- 컬러 선택 취소
function Ev_VehicleColor_ColorCancel()

	-- 컬러 패널 끄기
	VehicleColor_SetVisibleColorPanel( false )	
	
	-- 컬러 변경이 취소되었으니 이전 완료된 변경 컬러 카피
	g_tbVehicleColor_ColorList = CopyTable( g_tbVehicleColor_ColorListTemp )
	
	-- 이전 컬러로 탈것 색 변경하기
	g_MyChar:SetVehicleColor( g_tbVehicleColor_ColorList )
	
end

-- 본체 컬러1
function Ev_VehicleColor_Line1Color1()

	-- 변경할 컬러 인덱스, 메인 컬러 여부 설정
	g_nVehicleColor_ChangeColorIndex = 1
	g_bVehicelColor_isSubColor = false
	
	-- 컬러 패널 켜기
	VehicleColor_SetVisibleColorPanel( true )

end

-- 본체 컬러2
function Ev_VehicleColor_Line1Color2()

	-- 변경할 컬러 인덱스, 메인 컬러 여부 설정
	g_nVehicleColor_ChangeColorIndex = 1
	g_bVehicelColor_isSubColor = true
	
	-- 컬러 패널 켜기
	VehicleColor_SetVisibleColorPanel( true )

end

-- 라이트 컬러1
function Ev_VehicleColor_Line2Color1()

	-- 변경할 컬러 인덱스, 메인 컬러 여부 설정
	g_nVehicleColor_ChangeColorIndex = 2
	g_bVehicelColor_isSubColor = false
	
	-- 컬러 패널 켜기
	VehicleColor_SetVisibleColorPanel( true )

end

-- 라이트 컬러2
function Ev_VehicleColor_Line2Color2()

	-- 변경할 컬러 인덱스, 메인 컬러 여부 설정
	g_nVehicleColor_ChangeColorIndex = 2
	g_bVehicelColor_isSubColor = true
	
	-- 컬러 패널 켜기
	VehicleColor_SetVisibleColorPanel( true )

end

-- 타이어 컬러1
function Ev_VehicleColor_Line3Color1()

	-- 변경할 컬러 인덱스, 메인 컬러 여부 설정
	g_nVehicleColor_ChangeColorIndex = 3
	g_bVehicelColor_isSubColor = false
	
	-- 컬러 패널 켜기
	VehicleColor_SetVisibleColorPanel( true )

end

-- 타이어 컬러2
function Ev_VehicleColor_Line3Color2()

	-- 변경할 컬러 인덱스, 메인 컬러 여부 설정
	g_nVehicleColor_ChangeColorIndex = 3
	g_bVehicelColor_isSubColor = true
	
	-- 컬러 패널 켜기
	VehicleColor_SetVisibleColorPanel( true )

end

-- 배기통 컬러1
function Ev_VehicleColor_Line4Color1()

	-- 변경할 컬러 인덱스, 메인 컬러 여부 설정
	g_nVehicleColor_ChangeColorIndex = 4
	g_bVehicelColor_isSubColor = false
	
	-- 컬러 패널 켜기
	VehicleColor_SetVisibleColorPanel( true )

end

-- 배기통 컬러2
function Ev_VehicleColor_Line4Color2()

	-- 변경할 컬러 인덱스, 메인 컬러 여부 설정
	g_nVehicleColor_ChangeColorIndex = 4
	g_bVehicelColor_isSubColor = true
	
	-- 컬러 패널 켜기
	VehicleColor_SetVisibleColorPanel( true )

end

-- 보조석 컬러1
function Ev_VehicleColor_Line5Color1()

	-- 변경할 컬러 인덱스, 메인 컬러 여부 설정
	g_nVehicleColor_ChangeColorIndex = 5
	g_bVehicelColor_isSubColor = false
	
	-- 컬러 패널 켜기
	VehicleColor_SetVisibleColorPanel( true )

end

-- 보조석 컬러2
function Ev_VehicleColor_Line5Color2()

	-- 변경할 컬러 인덱스, 메인 컬러 여부 설정
	g_nVehicleColor_ChangeColorIndex = 5
	g_bVehicelColor_isSubColor = true
	
	-- 컬러 패널 켜기
	VehicleColor_SetVisibleColorPanel( true )

end

-- 악세서리 컬러1
function Ev_VehicleColor_Line6Color1()

	-- 변경할 컬러 인덱스, 메인 컬러 여부 설정
	g_nVehicleColor_ChangeColorIndex = 6
	g_bVehicelColor_isSubColor = false
	
	-- 컬러 패널 켜기
	VehicleColor_SetVisibleColorPanel( true )

end

-- 악세서리 컬러2
function Ev_VehicleColor_Line6Color2()

	-- 변경할 컬러 인덱스, 메인 컬러 여부 설정
	g_nVehicleColor_ChangeColorIndex = 6
	g_bVehicelColor_isSubColor = true
	
	-- 컬러 패널 켜기
	VehicleColor_SetVisibleColorPanel( true )

end