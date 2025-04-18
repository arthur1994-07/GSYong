
-- 국가 선택 리스트 UI 로드 완료
function LoadCountrySelectList()

	-- 확인 버튼
	SetString( WIDGET_ID_COUNTRYSELECTLIST, "label", GetGameWordText( "MODAL_BUTTON", 0 ), "btnOK" )
	SetEvent( WIDGET_ID_COUNTRYSELECTLIST, EVENT_BUTTON_CLICK, "Ev_CountrySelectList_OK", "", "btnOK" )
	
	-- 취소 버튼
	SetString( WIDGET_ID_COUNTRYSELECTLIST, "label", GetGameWordText( "MODAL_BUTTON", 1 ), "btnCancel" )
	SetEvent( WIDGET_ID_COUNTRYSELECTLIST, EVENT_BUTTON_CLICK, "Ev_CountrySelectList_Cancel", "", "btnCancel" )
	
	-- 대륙 선택
	SetString( WIDGET_ID_COUNTRYSELECTLIST, "text", GetGameWordText( "SELECT_COUNTRY_LIST_UI", 0 ), "labelContinent" )
	-- 국가 선택
	SetString( WIDGET_ID_COUNTRYSELECTLIST, "text", GetGameWordText( "SELECT_COUNTRY_LIST_UI", 1 ), "labelCountry" )
	
	-- 대룩 선택 이벤트
	SetEvent( WIDGET_ID_COUNTRYSELECTLIST, EVENT_LIST_INDEX_CHANGE, 
		"Ev_CountrySelectList_SelectContinent", "index", "listContinent" )
	
	-- 대륙 리스트 설정
	CountrySelectList_SetContinentList()

end

-- 오픈
function OpenCountrySelectList()

	-- 대륙 리스트 선택 상태 초기화
	SetInt( WIDGET_ID_COUNTRYSELECTLIST, "selectedIndex", -1, "listContinent" )

end

-- 대륙 리스트 설정
function CountrySelectList_SetContinentList()

	--[[
	{
		{
			1 : 대륙 ID
			2 : 대륙 이름
		}
		...
	}
	]]
	local tbList = g_Country:GetContinentList()
	
	local strParam = CreateParam( "id", "label" )
	SetDataEx( WIDGET_ID_COUNTRYSELECTLIST, strParam, tbList, "listContinent", false )
	
	-- 선택 상태 초기화
	SetInt( WIDGET_ID_COUNTRYSELECTLIST, "selectedIndex", -1, "listContinent" )

end

-- 국가 리스트 설정
function CountrySelectList_SetCountryList( nContinentID )

	--[[
	{
		{
			1 : 국가 ID
			2 : 국가 이름
		}
	}
	]]
	local tbList = g_Country:GetCountryList( nContinentID )
	
	local strParam = CreateParam( "id", "label" )
	SetDataEx( WIDGET_ID_COUNTRYSELECTLIST, strParam, tbList, "listCountry", false )
	
	-- 선택 상태 초기화
	SetInt( WIDGET_ID_COUNTRYSELECTLIST, "selectedIndex", -1, "listCountry" )

end

---------------------------------------------------------------------------------------------------------------------
-- 이벤트

-- 확인 버튼 클릭
function Ev_CountrySelectList_OK()

	local nSelect = GetInt( WIDGET_ID_COUNTRYSELECTLIST, "selectedIndex", "listCountry" )
	local strCountry = GetDataString( WIDGET_ID_COUNTRYSELECTLIST, nSelect, "label", "listCountry" )

	local str = StringFormat( GetGameExternalText( "SELECT_COUNTRY_CONFIRM", 0 ), strCountry )
	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, str, Ev_CountrySelectList_SelectConfirm, Event_MsgBox_Close )

end

-- 취소 버튼 클릭
function Ev_CountrySelectList_Cancel()

	CloseWidget( WIDGET_ID_COUNTRYSELECTLIST )

end

-- 국가 선택 확인
function Ev_CountrySelectList_SelectConfirm()

	Event_MsgBox_Close()
	
	local nContinentSelect = GetInt( WIDGET_ID_COUNTRYSELECTLIST, "selectedIndex", "listContinent" )
	local nCountrySelect = GetInt( WIDGET_ID_COUNTRYSELECTLIST, "selectedIndex", "listCountry" )
	
	local nContinentID = GetDataInt( WIDGET_ID_COUNTRYSELECTLIST, nContinentSelect, "id", "listContinent" )
	local nCountryID = GetDataInt( WIDGET_ID_COUNTRYSELECTLIST, nCountrySelect, "id", "listCountry" )
	
	-- 서버로 국가 변경 요청
	g_Msg:ReqChangeCountry( nContinentID, nCountryID )
	
	CloseWidget( WIDGET_ID_COUNTRYSELECTLIST )

end

-- 대륙 리스트 선택
function Ev_CountrySelectList_SelectContinent( nIndex )

	-- 선택한 대륙이 없다. 국가 리스트 초기화
	if -1 == nIndex then
	
		RemoveData( WIDGET_ID_COUNTRYSELECTLIST, 0, "listCountry", true )
		return
	
	end
	
	-- 현재 선택한 대륙 ID
	local nContinentID = GetDataInt( WIDGET_ID_COUNTRYSELECTLIST, nIndex, "id", "listContinent" )
	-- 국가 리스트 설정
	CountrySelectList_SetCountryList( nContinentID )

end