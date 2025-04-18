
g_nCountrSelect_labelBgGap = 0

-- 국가 선택 UI 로드 완료
function LoadCountrySelect()

	-- 국가 선택 버튼
	SetString( WIDGET_ID_COUNTRYSELECT, "label", GetGameWordText( "SELECT_COUNTRY_MODAL_OK_BUTTON", 0 ), "btnSelect" )
	SetEvent( WIDGET_ID_COUNTRYSELECT, EVENT_BUTTON_CLICK, "Ev_CountrySelect_ClickSelectBtn", "", "btnSelect" )
	
	g_nCountrSelect_labelBgGap = GetInt( WIDGET_ID_COUNTRYSELECT, "width", "bg" ) - 
		( GetInt( WIDGET_ID_COUNTRYSELECT, "x", "tfCountry" ) + 
		  GetInt( WIDGET_ID_COUNTRYSELECT, "width", "tfCountry" ) )

end

-- 오픈
function OpenCountrySelect()

	-- 선택한 국가 이미지 설정
	CountrySelect_SetImage( g_Client:GetSelectedCountryID() )

end

function CountrySelect_SetEnable ( _bEnabled )

	SetBool( WIDGET_ID_COUNTRYSELECT, "enabled", _bEnabled, "btnSelect" )
		
end

-- 선택한 국가 이미지 설정
-- 해당 부분은 .\Data\Glogic\Country..과 동기화가 이루어져야한다;
function CountrySelect_SetImage( nCountryID )

	local strFrameID = ""
	local bVisible = false
	
	
	--[[
	-- 중국
	if 6 == nCountryID then
	
		strFrameID = "china"
	
	-- 말레이시아
	elseif 17 == nCountryID then
	
		strFrameID = "malaysia"
	
	-- 필리핀
	elseif 24 == nCountryID then
	
		strFrameID = "philippines"
	
	-- 대만
	elseif 29 == nCountryID then
	
		strFrameID = "taiwan"
	
	-- 태국
	elseif 31 == nCountryID then
	
		strFrameID = "thailand"
		
	-- 없음
	else
	
		strFrameID = "none"
		bVisible = true
		
	end
	]]--
	strFrameID = Display_GetCountryFrameID(nCountryID)
	if strFrameID == "none"  then
		bVisible = true
	end
	
	CallFunction( WIDGET_ID_COUNTRYSELECT, "gotoAndPlay", strFrameID, "mcCountryImg" )
	
	SetBool( WIDGET_ID_COUNTRYSELECT, "visible", bVisible, "btnSelect" )
	SetBool( WIDGET_ID_COUNTRYSELECT, "visible", not bVisible, "tfCountry" )
	
	-- 버튼 visible 꺼져 있으면 현재 선택한 국가 이름 출력
	if false == bVisible then
	
		local strName = g_Client:GetSelectedCountryName()
		SetString( WIDGET_ID_COUNTRYSELECT, "text", strName, "tfCountry" )
		
		local nX = GetInt( WIDGET_ID_COUNTRYSELECT, "x", "tfCountry" )
		local nTextWidth = GetInt( WIDGET_ID_COUNTRYSELECT, "textWidth", "tfCountry" )
		local nWidth = nX + nTextWidth + g_nCountrSelect_labelBgGap
			
		if nWidth > GetInt( WIDGET_ID_COUNTRYSELECT, "width", "bg" ) then
		
			SetInt( WIDGET_ID_COUNTRYSELECT, "width", nTextWidth + 4, "tfCountry" )
			SetInt( WIDGET_ID_COUNTRYSELECT, "width", nWidth, "bg" )
		
		end
	
	end

end

----------------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 국가 선택 업데이트
function CountrySelect_Update( nCountryID )

	if false == IsOpenWidget( WIDGET_ID_COUNTRYSELECT ) then
		return
	end

	CountrySelect_SetImage( nCountryID )

end

----------------------------------------------------------------------------------------------------------------
-- 이벤트

-- 국가 선택 버튼 클릭
function Ev_CountrySelect_ClickSelectBtn()

	local str = GetGameExternalText( "SELECT_COUNTRY_NOTIFY", 0 )
	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, str, Ev_CountrySelect_OK, Event_MsgBox_Close )

end

-- 국가 선택 메시지 박스 확인
function Ev_CountrySelect_OK()

	Event_MsgBox_Close()
	
	-- 국가 선택 이미 되어 있는지 확인
	if false == g_Client:IsSelectCountry() then
	
		OpenWidget( WIDGET_ID_COUNTRYSELECTLIST )
	
	else
	
		local str = GetGameExternalText( "SELECT_COUNTRY_ALREADY", 0 )
		OpenMsgBox( MSG_BOX_TYPE_OK, str, Event_MsgBox_Close, Event_MsgBox_Close )
	
	end

end