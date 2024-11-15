
local g_bOpenPopupCheck = false 

local g_EMPARTY_ITEM_FILTER_MIDTOHIGH = 2
local g_EMPARTY_ITEM_FILTER_HIGH = 3
local g_EMPARTY_ITEM_FILTER_HIGHEST = 4

-- 파티 정보 UI 로드 완료
function LoadPartyInfo()
	
	-- 아이템 옵션
	local str = ""
	for i = 1, 5, 1 do	-- 0번 사용 안함..
		str = str .. GetGameWordText( "PARTY_MODAL_ITEM_OPTION", i ) .. ";"
	end
	SetData( WIDGET_ID_PARTYINFO, str, "pageParty.comboItem", false )
	
	local nItemOp = g_Party:GetItemOption()
	-- 0번 인덱스는 사용 안하지만 리스트 인덱스는 0번부터 시작되니 값을 하나 뺀다.
	SetInt( WIDGET_ID_PARTYINFO, "selectedIndex", nItemOp - 1, "pageParty.comboItem" )
	
	-- 특정 아이템 옵션에서만 아이템 필터 옵션을 선택할 수 있다.
	if 	4 == nItemOp or		-- 마스터 분배
		5 == nItemOp then	-- 주사위
		
		SetBool( WIDGET_ID_PARTYINFO, "visible", true, "pageParty.comboItemFilter" )
		
	else
	
		SetBool( WIDGET_ID_PARTYINFO, "visible", false, "pageParty.comboItemFilter" )
		
	end
	
	-- 게임머니 옵션
	str = ""
	for i = 1, 3, 1 do	-- 0번 사용 안함..
		str = str .. GetGameWordText( "PARTY_MODAL_MONEY_OPTION", i ) .. ";"
	end
	SetData( WIDGET_ID_PARTYINFO, str, "pageParty.comboMoney", false )
	
	-- 0번 인덱스는 사용 안하지만 리스트 인덱스는 0번부터 시작되니 값을 하나 뺀다.
	SetInt( WIDGET_ID_PARTYINFO, "selectedIndex", g_Party:GetMoneyOption() - 1, "pageParty.comboMoney" )
	
	-- 아이템 필터
	str = ""
	for i = 1, 3, 1 do	-- 0번 사용안함
		str = str .. GetGameWordText( "PARTY_MODAL_ITEM_OPTION_FILTER", i ) .. ";"
	end
	SetData( WIDGET_ID_PARTYINFO, str, "pageParty.comboItemFilter", false )
	
	-- Item Filter Option 은 가장 작은 값이 g_EMPARTY_ITEM_FILTER_MIDTOHIGH 이다
	-- 그러나 List Index 는 0부터 시작하기 때문에 g_EMPARTY_ITEM_FILTER_MIDTOHIGH 를 빼줘야한다
	local nItemFilterOption = g_Party:GetItemFilterOption()
	nItemFilterOption = nItemFilterOption - g_EMPARTY_ITEM_FILTER_MIDTOHIGH
	if nItemFilterOption >= 0 then
	
		SetInt( WIDGET_ID_PARTYINFO, "selectedIndex", nItemFilterOption, "pageParty.comboItemFilter" )
			
	end

	-- 버프보기
	SetBool( WIDGET_ID_PARTYINFO, "selected", g_bParty_ShowBuff, "pageParty.checkBuff" )
	-- 파티원 정보
	SetBool( WIDGET_ID_PARTYINFO, "selected", g_bParty_ShowPartySlot, "pageParty.checkInfo" )
	SetBool( WIDGET_ID_PARTYINFO, "visible", false, "pageParty.popupParty" )

	-- 텍스트 설정
	SetString( WIDGET_ID_PARTYINFO, "text", GetGameWordText( "PARTY_RENEWAL_INFO_TEXT", 0 ), "pageParty.labelItem" )
	SetString( WIDGET_ID_PARTYINFO, "text", GetGameWordText( "PARTY_RENEWAL_INFO_TEXT", 2 ), "pageParty.labelMoney" )
	SetString( WIDGET_ID_PARTYINFO, "text", GetGameWordText( "PARTY_RENEWAL_INFO_TEXT", 4 ), "pageParty.checkBuff_text" )
	SetString( WIDGET_ID_PARTYINFO, "text", GetGameWordText( "PARTY_RENEWAL_INFO_TEXT", 12 ), "pageParty.checkInfo_text" )
	SetString( WIDGET_ID_PARTYINFO, "label", GetGameWordText( "PARTY_RENEWAL_INFO_TEXT", 5 ), "pageParty.btnExpel" )
	SetString( WIDGET_ID_PARTYINFO, "label", GetGameWordText( "PARTY_RENEWAL_INFO_TEXT", 6 ), "pageParty.btnDelegate" )
	SetString( WIDGET_ID_PARTYINFO, "label", GetGameWordText( "PARTY_RENEWAL_INFO_TEXT", 3 ), "pageParty.btnDissolve" )
	SetString( WIDGET_ID_PARTYINFO, "label", GetGameWordText( "PARTY_RENEWAL_INFO_TEXT", 8 ), "pageParty.btnLeave" )
	
	-- 도움말
	SetEvent( WIDGET_ID_PARTYINFO, EVENT_BUTTON_CLICK, "Ev_PartyInfo_ClickHelp", "", "btnHelp" )
	
	-- 이벤트
	SetEvent( WIDGET_ID_PARTYINFO, EVENT_BUTTON_CLICK, "Ev_PartyInfo_checkBuff", "", "pageParty.checkBuff" )
	SetEvent( WIDGET_ID_PARTYINFO, EVENT_BUTTON_CLICK, "Ev_PartyInfo_checkInfo", "", "pageParty.checkInfo" )
	SetEvent( WIDGET_ID_PARTYINFO, EVENT_BUTTON_CLICK, "Ev_PartyInfo_btnExpel", "", "pageParty.btnExpel" )
	SetEvent( WIDGET_ID_PARTYINFO, EVENT_BUTTON_CLICK, "Ev_PartyInfo_btnDelegate", "", "pageParty.btnDelegate" )
	SetEvent( WIDGET_ID_PARTYINFO, EVENT_BUTTON_CLICK, "Ev_PartyInfo_btnDissolve", "", "pageParty.btnDissolve" )
	SetEvent( WIDGET_ID_PARTYINFO, EVENT_BUTTON_CLICK, "Ev_PartyInfo_btnLeave", "", "pageParty.btnLeave" )
	SetEvent( WIDGET_ID_PARTYINFO, EVENT_LIST_INDEX_CHANGE, "Ev_PartyInfo_comboItem", "index", "pageParty.comboItem" )
	SetEvent( WIDGET_ID_PARTYINFO, EVENT_LIST_INDEX_CHANGE, "Ev_PartyInfo_comboMoney", "index", "pageParty.comboMoney" )
	SetEvent( WIDGET_ID_PARTYINFO, EVENT_LIST_INDEX_CHANGE, "Ev_PartyInfo_comboItemFilter", "index", "pageParty.comboItemFilter" )
	SetEvent( WIDGET_ID_PARTYINFO, EVENT_MOUSE_CLICK, "Ev_PartyInfo_MouseClick", "", "pageParty"   )
	
	for i = 0, PARTY_MAX - 1, 1 do
	
		SetEvent( WIDGET_ID_PARTYINFO, EVENT_BUTTON_CLICK,
			"Ev_PartyInfo_btnFriend", "", "pageParty.btnFriend" .. tostring( i ), tostring( i ) )
		SetEvent( WIDGET_ID_PARTYINFO, EVENT_BUTTON_CLICK,
			"Ev_PartyInfo_btnCharInfo", "", "pageParty.btnCharInfo" .. tostring( i ), tostring( i ) )
	
	end
	
	SetEvent( WIDGET_ID_PARTYINFO, EVENT_LIST_ITEM_UP, "Ev_PartyInfo_ListClick", "buttonIdx;index;", "pageParty.list" )
	
	-- 팝업
	local tbPopupText = {}
	for i = 0, 6, 1 do
		tbPopupText[i+1] = { GetGameWordText("PARTY_RENEWAL_POPUP_MENU_TEXT", i) }
	end
	SetDataEx( WIDGET_ID_PARTYINFO, "label;", tbPopupText, "pageParty.popupParty.list", false )
	SetEvent( WIDGET_ID_PARTYINFO, EVENT_LIST_ITEM_CLICK, "Ev_PartyInfo_PopupClick", "buttonIdx;index;", "pageParty.popupParty.list" )
	-----------
	
	PartyInfo_UpdateList()
	PartyInfo_UpdateOptionEnable()

end

-- 마우스 클릭시 
function Ev_PartyInfo_MouseClick()
	
	if 	g_bOpenPopupCheck == true then 
		g_bOpenPopupCheck = false
		return 
	end
	
	SetBool( WIDGET_ID_PARTYINFO, "visible", false, "pageParty.popupParty" )
	
end

-- 파티 마스터인지 확인하고 마스터 기능 On/off
function PartyInfo_UpdateOptionEnable()

	local bIsMaster = g_Party:IsMyCharMaster()
	
	SetBool( WIDGET_ID_PARTYINFO, "enabled", bIsMaster, "pageParty.comboItem" )
	SetBool( WIDGET_ID_PARTYINFO, "enabled", bIsMaster, "pageParty.comboMoney" )
	SetBool( WIDGET_ID_PARTYINFO, "enabled", bIsMaster, "pageParty.comboItemFilter" )
	SetBool( WIDGET_ID_PARTYINFO, "visible", bIsMaster, "pageParty.btnExpel" )
	SetBool( WIDGET_ID_PARTYINFO, "visible", bIsMaster, "pageParty.btnDelegate" )
	SetBool( WIDGET_ID_PARTYINFO, "visible", bIsMaster, "pageParty.btnDissolve" )
	
end

-- 사용 안하는 리스트 설정
function PartyInfo_DisableList( nIndex )

	-- 캐릭터 이미지
	CallListItemFunc( WIDGET_ID_PARTYINFO, nIndex, "imgChar", "gotoAndStop", "NONE", "pageParty.list" )
	
	-- 학원 이미지
	CallListItemFunc( WIDGET_ID_PARTYINFO, nIndex, "mcSchool", "gotoAndStop", "none", "pageParty.list" )
	
	-- 레벨
	SetListItemString( WIDGET_ID_PARTYINFO, nIndex, "tfLevel", "htmlText", "", "pageParty.list" )
	
	-- 맵
	SetListItemString( WIDGET_ID_PARTYINFO, nIndex, "tfMap", "htmlText", "", "pageParty.list" )
	
	-- 위치
	SetListItemString( WIDGET_ID_PARTYINFO, nIndex, "tfPos", "htmlText", "", "pageParty.list" )
	
	-- 사망여부
	local strInst = "pageParty.LIRChar" .. tostring( nIndex + 1 ) .. ".imgChar.mcDead"
	SetBool( WIDGET_ID_PARTYINFO, "visible", false, strInst )
	
	-- 인덱스 숨기기
	if 0 == nIndex then
		SetBool( WIDGET_ID_PARTYINFO, "visible", false, "pageParty.idxMaster" )
	else
		SetBool( WIDGET_ID_PARTYINFO, "visible", false, "pageParty.idx" .. tostring( nIndex ) )
	end
	
	-- 친구 추가
	SetBool( WIDGET_ID_PARTYINFO, "visible", false, "pageParty.btnFriend" .. tostring( nIndex ) )
	-- 캐릭터 정보보기 
	SetBool( WIDGET_ID_PARTYINFO, "visible", false, "pageParty.btnCharInfo" .. tostring( nIndex ) )

end

-----------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 파티 리스트 업데이트
function PartyInfo_UpdateList()

	if false == IsLoadWidget( WIDGET_ID_PARTYINFO ) then
		return
	end
	-- 0번 인덱스는 사용 안하지만 리스트 인덱스는 0번부터 시작되니 값을 하나 뺀다.
	SetInt( WIDGET_ID_PARTYINFO, "selectedIndex", g_Party:GetItemOption() - 1, "pageParty.comboItem" )
	SetInt( WIDGET_ID_PARTYINFO, "selectedIndex", g_Party:GetMoneyOption() - 1, "pageParty.comboMoney" )
	
	-- 팝업버튼 지우기
	SetBool( WIDGET_ID_PARTYINFO, "visible", false, "pageParty.popupParty" )
	
	-- 옵션 콤보박스 활성/비활성
	PartyInfo_UpdateOptionEnable()
	
	local tbMyChar = Party_FindMyChar()
	
	local tbDataList = {}
	for i = 1, table.getn( g_tbParty_MemberList ), 1 do
	
		local tbMember = g_tbParty_MemberList[ i ]
		
		local tbData = {}
		if true == Party_IsSameMap( tbMyChar, tbMember ) then
			tbData[ 1 ] = HTMLFontColor( tbMember[ "strName" ], HTML_TEXT_COLOR_WHITE )
		else			
			tbData[ 1 ] = HTMLFontColor( tbMember[ "strName" ], HTML_TEXT_COLOR_GRAY )
		end
		
		tbData[ 2 ] = tostring( tbMember[ "dwDbNum" ] )
		
		tbDataList[ i ] = tbData
		
	end
	
	SetDataEx( WIDGET_ID_PARTYINFO, "label;dbNum;", tbDataList, "pageParty.list", false )
	
	for i = 1, PARTY_MAX, 1 do
	
		local nIndex = i - 1
		local tbMember = g_tbParty_MemberList[ i ]
		if nil ~= tbMember then
		
			-- 죽었는지 상태
			local bDie = false
			if 0 >= tbMember[ "dwNowHP" ] then
				bDie = true
			end
			SetBool( WIDGET_ID_PARTYINFO, "visible", bDie, "pageParty.LIRChar" .. i .. ".imgChar.mcDead" )
			
			-- 다른맵이면 회색
			local strColor = ""
			if true == Party_IsSameMap( tbMyChar, tbMember ) then
				strColor = HTML_TEXT_COLOR_WHITE
			else
				strColor = HTML_TEXT_COLOR_GRAY
			end
			
			-- 인덱스 보이기
			if 0 == nIndex then
				SetBool( WIDGET_ID_PARTYINFO, "visible", true, "pageParty.idxMaster" )
			else
				SetBool( WIDGET_ID_PARTYINFO, "visible", true, "pageParty.idx" .. tostring( nIndex ) )
			end
			
			-- 캐릭터 이미지
			CallListItemFunc( WIDGET_ID_PARTYINFO, nIndex, "imgChar", "gotoAndStop", 
				Party_GetCharImgParam( tbMember ), "pageParty.list" )
				
			-- 학원 아이콘
			CallListItemFunc( WIDGET_ID_PARTYINFO, nIndex, "mcSchool", "gotoAndPlay",
				Party_GetSchoolImgFrameID( tbMember[ "wSchool" ] ), "pageParty.list" )
				
			-- 레벨
			SetListItemString( WIDGET_ID_PARTYINFO, nIndex, "tfLevel", "htmlText", 
				HTMLFontColor( "Lv " .. tostring( tbMember[ "nLevel" ] ), strColor ), "pageParty.list" )
				
			-- 맵
			SetListItemString( WIDGET_ID_PARTYINFO, nIndex, "tfMap", "htmlText", 
				HTMLFontColor( GetMapName( tbMember[ "dwMapID" ] ), strColor ), "pageParty.list" )
				
			-- 위치
			local strPos = tostring( tbMember[ "nPosX" ] ) .. "/" .. tostring( tbMember[ "nPosY" ] )
			SetListItemString( WIDGET_ID_PARTYINFO, nIndex, "tfPos", "htmlText", 
				HTMLFontColor( strPos, strColor ), "pageParty.list" )
			
			-- 친구 추가 버튼 visible
			local bFriend = not( g_MyChar:GetDBNum() == tbMember[ "dwDbNum" ] or true == g_Friend:IsFriend( tbMember[ "strName" ] ) )
			SetBool( WIDGET_ID_PARTYINFO, "visible", bFriend, "pageParty.btnFriend" .. tostring( nIndex ) )

			-- 캐릭터 정보보기 
			SetBool( WIDGET_ID_PARTYINFO, "visible", true, "pageParty.btnCharInfo" .. tostring( nIndex ) )
		
		else
		
			-- 사용안하는 리스트
			PartyInfo_DisableList( nIndex )
		
		end	
	
	end
	
	-- 파티중인지 여부에 따라 체크박스 활성
	SetBool( WIDGET_ID_PARTYINFO, "enabled", g_Party:IsValid(), "pageParty.checkBuff" )
	SetBool( WIDGET_ID_PARTYINFO, "enabled", g_Party:IsValid(), "pageParty.checkInfo" )
		


	end

-- 파티 해산
function PartyInfo_Dissolve()

	if false == IsLoadWidget( WIDGET_ID_PARTYINFO ) then
		return
	end

	for i = 1, PARTY_MAX, 1 do
		PartyInfo_DisableList( i - 1 )
	end
	


	RemoveData( WIDGET_ID_PARTYINFO, 0, "pageParty.list", true )
	
	SetBool( WIDGET_ID_PARTYINFO, "enabled", false, "pageParty.comboItem" )
	SetBool( WIDGET_ID_PARTYINFO, "enabled", false, "pageParty.comboMoney" )
	SetBool( WIDGET_ID_PARTYINFO, "enabled", false, "pageParty.comboItemFilter" )
	SetBool( WIDGET_ID_PARTYINFO, "visible", false, "pageParty.btnExpel" )
	SetBool( WIDGET_ID_PARTYINFO, "visible", false, "pageParty.btnDelegate" )
	SetBool( WIDGET_ID_PARTYINFO, "visible", false, "pageParty.btnDissolve" )
	
	-- 파티 아이템 필터 제거 
	SetBool( WIDGET_ID_PARTYINFO, "visible", false, "pageParty.comboItemFilter" )
	
	-- 0번 인덱스는 사용 안하지만 리스트 인덱스는 0번부터 시작되니 값을 하나 뺀다.
	SetInt( WIDGET_ID_PARTYINFO, "selectedIndex", g_Party:GetItemOption() - 1, "pageParty.comboItem" )
	SetInt( WIDGET_ID_PARTYINFO, "selectedIndex", g_Party:GetMoneyOption() - 1, "pageParty.comboMoney" )
	
	-- 파티중인지 여부에 따라 체크박스 활성
	SetBool( WIDGET_ID_PARTYINFO, "enabled", g_Party:IsValid(), "pageParty.checkBuff" )
	SetBool( WIDGET_ID_PARTYINFO, "enabled", g_Party:IsValid(), "pageParty.checkInfo" )

	--PartyInfo_UpdateOptionEnable()
end

-- 파티원 맵 이동
function PartyInfo_MemberMoveMap( dwIndex )

	PartyInfo_UpdateList()

end

-- 파티원 정보 업데이트
function PartyInfo_UpdateMember( dwIndex )

	if false == IsLoadWidget( WIDGET_ID_PARTYINFO ) then
		return
	end

	local tbMember = g_tbParty_MemberList[ dwIndex + 1 ]
	if nil == tbMember then
		return
	end
	
	local tbMyChar = Party_FindMyChar()
	
	-- 내 캐릭터와 같은 맵인지 확인
	local bIsSameMap = true
	local strColor = ""
	if true == Party_IsSameMap( tbMyChar, tbMember ) then
		strColor = HTML_TEXT_COLOR_WHITE
		bIsSameMap = true
	else
		strColor = HTML_TEXT_COLOR_GRAY
		bIsSameMap = false
	end
	
	-- 레벨
	SetListItemString( WIDGET_ID_PARTYINFO, dwIndex, "tfLevel", "htmlText", 
		HTMLFontColor( "Lv " .. tostring( tbMember[ "nLevel" ] ), strColor ), "pageParty.list" )
		
	-- 위치
	local strPos = tostring( tbMember[ "nPosX" ] ) .. "/" .. tostring( tbMember[ "nPosY" ] )
	SetListItemString( WIDGET_ID_PARTYINFO, dwIndex, "tfPos", "htmlText", 
		HTMLFontColor( strPos, strColor ), "pageParty.list" )
	
	-- 죽었는지 상태
	local bDie = false
	if 0 >= tbMember[ "dwNowHP" ] then
		bDie = true
	end
	SetBool( WIDGET_ID_PARTYINFO, "visible", bDie, 
		"pageParty.LIRChar" .. tostring( dwIndex + 1 ) .. ".imgChar.mcDead" )
	
end

-- 파티원 이름 변경
function PartyInfo_MemberRename( dwIndex )

	if false == IsLoadWidget( WIDGET_ID_PARTYINFO ) then
		return
	end

	local tbMember = g_tbParty_MemberList[ dwIndex + 1 ]
	if nil == tbMember then
		return
	end
	
	local tbMyChar = Party_FindMyChar()
	
	-- 내 캐릭터와 같은 맵인지 확인
	local strColor = ""
	if true == Party_IsSameMap( tbMyChar, tbMember ) then
		strColor = HTML_TEXT_COLOR_WHITE
	else
		strColor = HTML_TEXT_COLOR_GRAY
	end
	
	SetListItemString( WIDGET_ID_PARTYINFO, dwIndex, "textField", "htmlText", 
		HTMLFontColor( tbMember[ "strName" ], strColor ), "pageParty.list" )

end

-- 파티 옵션 변경
function PartyInfo_ChangeOption( nItemOp, nMoneyOp, nItemFilterOp )

	if false == IsLoadWidget( WIDGET_ID_PARTYINFO ) then
		return
	end
	
	-- 특정 아이템 옵션에서만 아이템 필터 옵션을 선택할 수 있다.
	if 	4 == nItemOp or		-- 마스터 분배
		5 == nItemOp then	-- 주사위
		
		SetBool( WIDGET_ID_PARTYINFO, "visible", true, "pageParty.comboItemFilter" )
		
	else
	
		SetBool( WIDGET_ID_PARTYINFO, "visible", false, "pageParty.comboItemFilter" )
		
	end
	
	-- 0번 인덱스는 사용 안하지만 리스트 인덱스는 0번부터 시작되니 값을 하나 뺀다.
	SetInt( WIDGET_ID_PARTYINFO, "selectedIndex", nItemOp-1, "pageParty.comboItem" )
	SetInt( WIDGET_ID_PARTYINFO, "selectedIndex", nMoneyOp-1, "pageParty.comboMoney" )
	SetInt( WIDGET_ID_PARTYINFO, "selectedIndex", nItemFilterOp-1, "pageParty.comboItemFilter" )

end

-- 파티원 추방
function PartyInfo_Expel()

	local nSelect = GetInt( WIDGET_ID_PARTYINFO, "selectedIndex", "pageParty.list" )
	g_Msg:SecedeParty( nSelect )
	Event_MsgBox_Close()

end

-- 파티 마스터 위임
function PartyInfo_Delegate()

	local nSelect = GetInt( WIDGET_ID_PARTYINFO, "selectedIndex", "pageParty.list" )
	g_Msg:AuthorityParty( nSelect )
	Event_MsgBox_Close()

end

-- 파티 해산
function PartyInfo_DissolveParty()
	g_Msg:DissolveParty()
	Event_MsgBox_Close()
end

-- 파티 탈퇴
function PartyInfo_Leave()
	g_Msg:SecedeParty( Party_FindMyCharIndex() )
	
	
	
	Event_MsgBox_Close()
end

--------------------------------------------------------------------------------------------------------------
-- 이벤트

-- 버프보기
function Ev_PartyInfo_checkBuff()

	g_bParty_ShowBuff = GetBool( WIDGET_ID_PARTYINFO, "selected", "pageParty.checkBuff" )
	PartySlot_SetVisibleBuff( g_bParty_ShowBuff )

end

-- 파티원 정보
function Ev_PartyInfo_checkInfo()

	g_bParty_ShowPartySlot = GetBool( WIDGET_ID_PARTYINFO, "selected", "pageParty.checkInfo" )
	
	if true == g_bParty_ShowPartySlot then
		OpenWidget( WIDGET_ID_PARTYSLOT )
	else
		CloseWidget( WIDGET_ID_PARTYSLOT )
	end
	
end

-- 추방
function Ev_PartyInfo_btnExpel()

	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, GetGameInternalText( "PARTY_EXPULSION_TEXT", 0 ),
		PartyInfo_Expel, Event_MsgBox_Close )

end

-- 위임
function Ev_PartyInfo_btnDelegate()

	SetBool( WIDGET_ID_PARTYINFO, "visible", false, "pageParty.popupParty" )

	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, GetGameInternalText( "PARTY_DELEGATE_TEXT", 0 ),
		PartyInfo_Delegate, Event_MsgBox_Close )
	
end

-- 파티 해산
function Ev_PartyInfo_btnDissolve()
	
	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, GetGameInternalText( "PARTY_RELEASE_PARTY_TEXT", 0 ),
		PartyInfo_DissolveParty, Event_MsgBox_Close )
	
end

-- 탈퇴
function Ev_PartyInfo_btnLeave()

	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, GetGameInternalText( "PARTY_SECEDE_TEXT", 0 ),
		PartyInfo_Leave, Event_MsgBox_Close )

end

-- 아이템 옵션 콤보박스
function Ev_PartyInfo_comboItem( nIndex )

	-- 0번 인덱스는 사용 안하지만 리스트 인덱스는 0번부터 시작되니 값을 하나 더한다.	
	local nMoneyIndex = GetInt( WIDGET_ID_PARTYINFO, "selectedIndex", "pageParty.comboMoney" ) + 1
	local nFilterIndex = GetInt( WIDGET_ID_PARTYINFO, "selectedIndex", "pageParty.comboItemFilter" ) + 1
	g_Msg:ChangeOptionParty( nIndex+1, nMoneyIndex, nFilterIndex )
	
	-- 특정 아이템 옵션에서만 아이템 필터 옵션을 선택할 수 있다.
	if 	4 == nIndex or		-- 마스터 분배
		5 == nIndex then	-- 주사위
		
		SetBool( WIDGET_ID_PARTYINFO, "visible", true, "pageParty.comboItemFilter" )
		
	else
	
		SetBool( WIDGET_ID_PARTYINFO, "visible", false, "pageParty.comboItemFilter" )
		
	end

end

-- 게임머니 옵션 콤보박스
function Ev_PartyInfo_comboMoney( nIndex )

	-- 0번 인덱스는 사용 안하지만 리스트 인덱스는 0번부터 시작되니 값을 하나 더한다.
	local nItemIndex = GetInt( WIDGET_ID_PARTYINFO, "selectedIndex", "pageParty.comboItem" ) + 1
	local nFilterIndex = GetInt( WIDGET_ID_PARTYINFO, "selectedIndex", "pageParty.comboItemFilter" ) + 1
	g_Msg:ChangeOptionParty( nItemIndex, nIndex + 1, nFilterIndex )

end

-- 아이템 필터 옵션 콤보박스
function Ev_PartyInfo_comboItemFilter( nIndex )

	-- Item Filter 는 최소값이 g_EMPARTY_ITEM_FILTER_MIDTOHIGH 이다
	-- 그러나 Index는 0부터 시작하기 때문에 g_EMPARTY_ITEM_FILTER_MIDTOHIGH 를 더해줘야한다
	local nItemIndex = GetInt( WIDGET_ID_PARTYINFO, "selectedIndex", "pageParty.comboItem" ) + g_EMPARTY_ITEM_FILTER_MIDTOHIGH
	if nItemIndex < g_EMPARTY_ITEM_FILTER_MIDTOHIGH then
		
		nItemIndex = g_EMPARTY_ITEM_FILTER_MIDTOHIGH
		
	end
	
	local nMoneyIndex = GetInt( WIDGET_ID_PARTYINFO, "selectedIndex", "pageParty.comboMoney" ) + 1
	g_Msg:ChangeOptionParty( nItemIndex, nMoneyIndex, nIndex + 1)

end

-- 친구 추가
function Ev_PartyInfo_btnFriend( strIndex )

	local nIndex = tonumber( strIndex )	
	
	local tbMember = g_tbParty_MemberList[ nIndex + 1 ]
	if nil == tbMember then
		return
	end
	
	g_Msg:ReqFriendAdd( tbMember[ "strName" ] )

end

-- 캐릭터 정보
function Ev_PartyInfo_btnCharInfo( strIndex )

	local nIndex = tonumber( strIndex )	
	
	local tbMember = g_tbParty_MemberList[ nIndex + 1 ]
	if nil == tbMember then
		return
	end
	
	g_Msg:ReqReferChar( tbMember[ "strName" ] )

end

-- 헬프창 출력
function Ev_PartyInfo_ClickHelp()

	local str = GetGameInternalText( "PARTY_RENEWAL_HELP" )
	HelpTooltip_SetText( str )
	
end

function Ev_PartyInfo_ListClick( nbutton, nIndex )

	SetInt( WIDGET_ID_PARTYINFO, "selectedIndex", -1, "pageParty.popupParty.list" )

	local bVisible = false
	if Party_FindMyCharIndex() == nIndex then
	
		SetInt( WIDGET_ID_PARTYINFO, "selectedIndex", nIndex, "pageParty.list" )
		SetBool( WIDGET_ID_PARTYINFO, "visible", bVisible, "pageParty.popupParty" )
		g_bOpenPopupCheck = bVisible

		return
		
	end

	local bIsMaster = g_Party:IsMyCharMaster()
	SetBool( WIDGET_ID_PARTYINFO, "enabled", bIsMaster, "pageParty.popupParty.item1" )
	SetBool( WIDGET_ID_PARTYINFO, "enabled", bIsMaster, "pageParty.popupParty.item7" )
	
	bVisible = nbutton == 1 and g_tbParty_MemberList[ nIndex + 1 ] ~= nil
	SetBool( WIDGET_ID_PARTYINFO, "visible", bVisible, "pageParty.popupParty" )
	g_bOpenPopupCheck = bVisible
	
	if true == bVisible then
	
		local tbMousePos = GetMousePos()
	
		local nX = tbMousePos[ "x" ] - GetInt( WIDGET_ID_PARTYINFO, "x", "" ) - GetInt( WIDGET_ID_PARTYINFO, "x", "pageParty" )
		local nY = tbMousePos[ "y" ] - GetInt( WIDGET_ID_PARTYINFO, "y", "" ) - GetInt( WIDGET_ID_PARTYINFO, "y", "pageParty" )
		SetInt( WIDGET_ID_PARTYINFO, "x", nX, "pageParty.popupParty" )
		SetInt( WIDGET_ID_PARTYINFO, "y", nY, "pageParty.popupParty" )
	
	end
	
	if nbutton == 1 then	-- right

		SetInt( WIDGET_ID_PARTYINFO, "selectedIndex", nIndex, "pageParty.list" )
		
	end

end

function Ev_PartyInfo_PopupClick( nbutton, nIndex )

	SetBool( WIDGET_ID_PARTYINFO, "visible", false, "pageParty.popupParty" )
	if nbutton ~= 0 then
		return
	end
	
	local nSelectMember = GetInt( WIDGET_ID_PARTYINFO, "selectedIndex", "pageParty.list" )
	local tbMember = g_tbParty_MemberList[ nSelectMember + 1 ]
	if tbMember == nil then
		return
	end
	
	if nIndex == 0 then	-- 파티장 위임
		g_Msg:AuthorityParty(nSelectMember)
	elseif nIndex == 1 then -- 귓속말
		BasicChat_SetWhisper(tbMember["strName"])
	elseif nIndex == 2 then -- 정보 보기
		g_Msg:ReqReferChar(tbMember["strName"])
	elseif nIndex == 3 then -- 친구 등록
		Ev_PartyInfo_btnFriend(tostring(nSelectMember))
	elseif nIndex == 4 then -- 차단 등록
		g_Msg:ReqFriendBlock(tbMember["strName"], true )
	elseif nIndex == 5 then -- 차단 해제
		g_Msg:ReqFriendBlock(tbMember["strName"], false )
	elseif nIndex == 6 then -- 추방
		g_Msg:SecedeParty( nSelectMember )
	end	
	
end