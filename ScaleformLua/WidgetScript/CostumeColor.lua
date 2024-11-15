
g_tbCostumeColor_ColorOrig = {}		-- 코스튬 원본 컬러
g_tbCostumeColor_ColorTemp = {}		-- 변경 완료된 컬러
g_tbCostumeColor_Color = {}			-- 변경중인 컬러

g_bCostumeColor_Restrict = false	-- 정해진 컬러만 변경 가능? (사용안함)
g_bCostumeColor_isSubColor = false	-- 서브 컬러 변경중?

-- 윈도우 사이즈 애니메이션을 위해 각 패널별 사이즈 저장
g_nCostumeColor_SettingWidth = 0
g_nCostumeColor_SettingHeight = 0

g_nCostumeColor_ColorWidth = 0
g_nCostumeColor_ColorHeight = 0

-- 코스튬 컬러 변경 로드 완료
function LoadCostumeColor()

	SetEvent( WIDGET_ID_COSTUMECOLOR, EVENT_WIDGET_CLOSE, "CloseCostumeColor", "", "" )
	
	-- 컬러 패널 설정 -----------------------------------------------------------------------------------
	-- 윈도우 사이즈 애니메이션을 위해 Width/Height 저장
	g_nCostumeColor_ColorWidth = GetInt( WIDGET_ID_COSTUMECOLOR, "width", "panelColor" ) + 10
	g_nCostumeColor_ColorHeight= GetInt( WIDGET_ID_COSTUMECOLOR, "height", "panelColor" ) + 29
	
	-- 컬러 패널 숨기기
	SetBool( WIDGET_ID_COSTUMECOLOR, "visible", false, "panelColor" )
	
	-- 확인
	SetString( WIDGET_ID_COSTUMECOLOR, "label", GetGameWordText( "GAMBLE_OKCANCEL", 0 ), "panelColor.btnOK" )
	SetEvent( WIDGET_ID_COSTUMECOLOR, EVENT_BUTTON_CLICK, "Ev_CostumeColor_ColorOK", "", "panelColor.btnOK" )
	-- 취소
	SetString( WIDGET_ID_COSTUMECOLOR, "label", GetGameWordText( "GAMBLE_OKCANCEL", 1 ), "panelColor.btnCancel" )
	SetEvent( WIDGET_ID_COSTUMECOLOR, EVENT_BUTTON_CLICK, "Ev_CostumeColor_ColorCancel", "", "panelColor.btnCancel" )
	
	-- 컬러 변경 이벤트
	SetEvent( WIDGET_ID_COSTUMECOLOR, EVENT_COLOR_CHANGE, "Ev_CostumeColor_ChangeColor", "strValue", "panelColor.colorPicker" )
	
	-- 설정 패널 설정 -----------------------------------------------------------------------------------
	-- 윈도우 사이즈 애니메이션을 위해 Width/Height 저장
	g_nCostumeColor_SettingWidth = GetInt( WIDGET_ID_COSTUMECOLOR, "width", "panelSetting" ) + 10
	g_nCostumeColor_SettingHeight = GetInt( WIDGET_ID_COSTUMECOLOR, "height", "panelSetting" ) + 29
	
	-- 컬러 패널이 설정 패널보다 커서 최초 윈도우 사이즈가 크게 잡히니 설정 패널 사이즈로 강제 설정
	SetInt( WIDGET_ID_COSTUMECOLOR, "customWidth", g_nCostumeColor_SettingWidth, "" )
	SetInt( WIDGET_ID_COSTUMECOLOR, "customHeight", g_nCostumeColor_SettingHeight, "" )
	
	-- 이름
	SetString( WIDGET_ID_COSTUMECOLOR, "text", 
		GetGameInternalText( "COSTUM_COLOR_LABEL", 0 ), "panelSetting.labelName" )
		
	SetEvent( WIDGET_ID_COSTUMECOLOR, EVENT_BUTTON_CLICK, "Ev_CostumeColor_colorView1Click", "", "panelSetting.color1" )
	SetEvent( WIDGET_ID_COSTUMECOLOR, EVENT_BUTTON_CLICK, "Ev_CostumeColor_colorView2Click", "", "panelSetting.color2" )
	
	-- 버튼 설정 ----------------------------------
	-- 랜덤
	SetString( WIDGET_ID_COSTUMECOLOR, "label",
		GetGameWordText( "COLORSELECTOR_PANNEL_PAGE_BUTTON", 0 ), "panelSetting.btnRandom" )
	SetEvent( WIDGET_ID_COSTUMECOLOR, EVENT_BUTTON_CLICK, "Ev_CostumeColor_btnRandom", "", "panelSetting.btnRandom" )
	-- 리셋
	SetString( WIDGET_ID_COSTUMECOLOR, "label",
		GetGameWordText( "COLORSELECTOR_PANNEL_PAGE_BUTTON", 1 ), "panelSetting.btnReset" )
	SetEvent( WIDGET_ID_COSTUMECOLOR, EVENT_BUTTON_CLICK, "Ev_CostumeColor_btnReset", "", "panelSetting.btnReset" )
	-- 취소
	SetString( WIDGET_ID_COSTUMECOLOR, "label",
		GetGameWordText( "COLORSELECTOR_PANNEL_PAGE_BUTTON", 2 ), "panelSetting.btnCancel" )
	SetEvent( WIDGET_ID_COSTUMECOLOR, EVENT_BUTTON_CLICK, "Ev_CostumeColor_btnCancel", "", "panelSetting.btnCancel" )
	-- 확인
	SetString( WIDGET_ID_COSTUMECOLOR, "label",
		GetGameWordText( "COLORSELECTOR_PANNEL_PAGE_BUTTON", 3 ), "panelSetting.btnOK" )
	SetEvent( WIDGET_ID_COSTUMECOLOR, EVENT_BUTTON_CLICK, "Ev_CostumeColor_btnOK", "", "panelSetting.btnOK" )

end

-- 코스튬 컬러 변경창 오픈
function OpenCostumeColor()

	-- 코스튬 컬러 가져오기
	g_tbCostumeColor_ColorOrig = g_MyChar:GetCostumeColor()
	g_tbCostumeColor_ColorTemp = CopyTable( g_tbCostumeColor_ColorOrig )
	g_tbCostumeColor_Color = CopyTable( g_tbCostumeColor_ColorOrig )
	
	SetUInt( WIDGET_ID_COSTUMECOLOR, "color", g_tbCostumeColor_ColorOrig[ "dwMainColor" ], "panelSetting.color1" )
	SetUInt( WIDGET_ID_COSTUMECOLOR, "color", g_tbCostumeColor_ColorOrig[ "dwSubColor" ], "panelSetting.color2" )

end

-- 닫기
function CloseCostumeColor()

	-- 원래 컬러로 적용
	g_MyChar:SetCostumeColor( g_tbCostumeColor_ColorOrig )

end

-- 컬러 패널 활성/비활성
function CostumeColor_SetVisibleColorPanel( bVisible )

	local strColorAnimComplete = ""
	local strColorAnimAlpha = ""
	
	local strSettingAnimComplete = ""
	local strSettingAnimAlpha = ""
	
	local strNewSize = ""
	
	if true == bVisible then
	
		SetBool( WIDGET_ID_COSTUMECOLOR, "alpha", 0, "panelColor" )
		SetBool( WIDGET_ID_COSTUMECOLOR, "visible", true, "panelColor" )
		
		strColorAnimComplete = ""
		strColorAnimAlpha = "1"
		
		strSettingAnimComplete = "Ev_CostumeColor_SettingPanelVisibleOff"
		strSettingAnimAlpha = "0"
		
		strNewSize = tostring( g_nCostumeColor_ColorWidth ) .. ";" .. tostring( g_nCostumeColor_ColorHeight )
		
		g_tbCostumeColor_Color = CopyTable( g_tbCostumeColor_ColorTemp )
		if false == g_bCostumeColor_isSubColor then
			SetUInt( WIDGET_ID_COSTUMECOLOR, "color", g_tbCostumeColor_Color[ "dwMainColor" ],
				"panelColor.colorPicker" )
		else
			SetUInt( WIDGET_ID_COSTUMECOLOR, "color", g_tbCostumeColor_Color[ "dwSubColor" ],
				"panelColor.colorPicker" )
		end
	
	else
	
		SetBool( WIDGET_ID_COSTUMECOLOR, "alpha", 0, "panelSetting" )
		SetBool( WIDGET_ID_COSTUMECOLOR, "visible", true, "panelSetting" )
		
		strColorAnimComplete = "Ev_CostumeColor_ColorPanelVisibleOff"
		strColorAnimAlpha = "0"
		
		strSettingAnimComplete = ""
		strSettingAnimAlpha = "1"
		
		strNewSize = tostring( g_nCostumeColor_SettingWidth ) .. ";" .. tostring( g_nCostumeColor_SettingHeight )
	
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
	SetTween( WIDGET_ID_COSTUMECOLOR, tbQuickSet, "alpha", strColorAnimAlpha, "panelColor" )
	
	-- 설정 패널 애니메이션
	tbQuickSet[ "onComplete" ] = strSettingAnimComplete
	SetTween( WIDGET_ID_COSTUMECOLOR, tbQuickSet, "alpha", strSettingAnimAlpha, "panelSetting" )
	
	-- 윈도우 사이즈 애니메이션	
	tbQuickSet[ "time" ] = 250
	tbQuickSet[ "onComplete" ] = ""
	SetTween( WIDGET_ID_COSTUMECOLOR, tbQuickSet, "customWidth;customHeight;", strNewSize, "" )

end

-- 컬러 픽커 색 설정
function CostumeColor_SetColorPicker()

	-- 메인 컬러? 서브 컬러?
	local nColor = 0
	if false == g_bCostumeColor_isSubColor then
		nColor = g_tbCostumeColor_Color[ "dwMainColor" ]
	else
		nColor = g_tbCostumeColor_Color[ "dwSubColor" ]
	end
	
	-- 해당 컬러로 컬러 픽커 색 설정
	SetUInt( WIDGET_ID_COSTUMECOLOR, "color", nColor, "panelColor.colorPicker" )

end

-- 컬러 뷰 색 설정
function CostumeColor_SetColorView()

	-- 메인
	SetUInt( WIDGET_ID_COSTUMECOLOR, "color", 
		g_tbCostumeColor_ColorTemp[ "dwMainColor" ], "panelSetting.color1" )
		
	-- 서브
	SetUInt( WIDGET_ID_COSTUMECOLOR, "color", 
		g_tbCostumeColor_ColorTemp[ "dwSubColor" ], "panelSetting.color2" )

end

------------------------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 코스튬 색상 변경 카드 사용
function CostumeColor_ItemUse( bRestrict )

	local strMsg = ""
	
	g_bCostumeColor_Restrict = bRestrict
	if true == g_bCostumeColor_Restrict then	
		strMsg = GetGameInternalText( "MODAL_COSTUM_INFO_RESTRICT", 0 )
	else
		strMsg = GetGameInternalText( "MODAL_COSTUM_INFO", 0 )
	end
	
	-- 메시지 박스 띄우기
	OpenMsgBox( MSG_BOX_TYPE_OK, strMsg, Ev_CostumeColor_UseOK )

end

------------------------------------------------------------------------------------------------------------------------
-- 이벤트

-- 코스튬 색상 변경 카드 사용 OK
function Ev_CostumeColor_UseOK()

	if false == IsOpenWidget( WIDGET_ID_COSTUMECOLOR ) then
		OpenWidget( WIDGET_ID_COSTUMECOLOR )
	end
	
	Event_MsgBox_Close()

end

-- 컬러 패널 확인
function Ev_CostumeColor_ColorOK()

	-- 컬러 패널 닫기
	CostumeColor_SetVisibleColorPanel( false )
	
	-- 변경 완료
	g_tbCostumeColor_ColorTemp = CopyTable( g_tbCostumeColor_Color )
	-- 컬러 적용
	g_MyChar:SetCostumeColor( g_tbCostumeColor_ColorTemp )
	
	-- 컬러 뷰 색도 같이 변경
	CostumeColor_SetColorView()

end

-- 컬러 패널 취소
function Ev_CostumeColor_ColorCancel()

	-- 컬러 패널 닫기
	CostumeColor_SetVisibleColorPanel( false )
	
	-- 이전 변경 완료된 컬러로 초기화
	g_tbCostumeColor_Color = CopyTable( g_tbCostumeColor_ColorTemp )
	-- 컬러 적용
	g_MyChar:SetCostumeColor( g_tbCostumeColor_Color )

end

-- 컬러 패널 컬러 변경
function Ev_CostumeColor_ChangeColor( strColor )

	if false == g_bCostumeColor_isSubColor then
		g_tbCostumeColor_Color[ "dwMainColor" ] = tonumber( strColor )
	else
		g_tbCostumeColor_Color[ "dwSubColor" ] = tonumber( strColor )
	end
	
	-- 컬러 적용
	g_MyChar:SetCostumeColor( g_tbCostumeColor_Color )

end

-- 애니메이션 끝, 설정 패널 visible 끄기
function Ev_CostumeColor_SettingPanelVisibleOff()
	SetBool( WIDGET_ID_COSTUMECOLOR, "visible", false, "panelSetting" )
end

-- 애니메이션 끝, 컬러 패널 visible 끄기
function Ev_CostumeColor_ColorPanelVisibleOff()
	SetBool( WIDGET_ID_COSTUMECOLOR, "visible", false, "panelColor" )
end

-- 컬러뷰 1 클릭
function Ev_CostumeColor_colorView1Click()

	-- 메인 컬러
	g_bCostumeColor_isSubColor = false
	-- 컬러 패널 켜기
	CostumeColor_SetVisibleColorPanel( true )

end

-- 컬러뷰 2 클릭
function Ev_CostumeColor_colorView2Click()

	-- 서브 컬러
	g_bCostumeColor_isSubColor = true
	-- 컬러 패널 켜기
	CostumeColor_SetVisibleColorPanel( true )

end

-- 랜덤
function Ev_CostumeColor_btnRandom()

	-- 메인/서브 컬러 활성인지 확인하고 랜덤 컬러 가져오기
	if true == g_tbCostumeColor_ColorTemp[ "bMainEnable" ] then
		g_tbCostumeColor_ColorTemp[ "dwMainColor" ] = GetRandomColor( g_bCostumeColor_Restrict )
	end
	
	if true == g_tbCostumeColor_ColorTemp[ "bSubEnable" ] then
		g_tbCostumeColor_ColorTemp[ "dwSubColor" ] = GetRandomColor( g_bCostumeColor_Restrict )
	end
	
	-- 컬러 적용
	g_MyChar:SetCostumeColor( g_tbCostumeColor_ColorTemp )
	-- 컬러 뷰 색 변경
	CostumeColor_SetColorView()

end

-- 리셋
function Ev_CostumeColor_btnReset()

	-- 원래 컬러로 되돌리기
	g_tbCostumeColor_ColorTemp = CopyTable( g_tbCostumeColor_ColorOrig )
	g_tbCostumeColor_Color = CopyTable( g_tbCostumeColor_ColorOrig )
	
	-- 원래 컬러로 컬러 적용
	g_MyChar:SetCostumeColor( g_tbCostumeColor_ColorOrig )
	-- 컬러 뷰 색 설정
	CostumeColor_SetColorView()

end

-- 취소
function Ev_CostumeColor_btnCancel()

	CloseWidget( WIDGET_ID_COSTUMECOLOR )

end

-- 확인
function Ev_CostumeColor_btnOK()

	-- 변경 완료된 컬러 변경 서버에 요청하고 UI 닫기
	g_Msg:CommitCostumeColor( g_tbCostumeColor_ColorTemp )
	CloseWidget( WIDGET_ID_COSTUMECOLOR )

end