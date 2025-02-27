
-- 링크 아이템 최대 개수
g_nGroupChat_MAXLINKITEM = 3

-- 채팅방 정보 - !! 멀티 UI SubID와 테이블 Index는 같은 값이다.
--[[
{
	{
		nKey 		: 그룹 채팅 키
		strOwner 	: 방장 이름
		nLogID 		: 채팅 로그 ID
		
		nMouseX		: 드래그 마우스 X
		nMouseY		: 드래그 마우스 Y
		bMinimize	: 최소화중?
		strMenuChar	: 메뉴 연 캐릭터 이름
	}
	...
}
]]
g_tbGroupChat_RoomInfo = {}

-- 포커스 없는 상태?
g_bGroupChat_FocusClean = false

-- 드래그 중인 최소화 타이틀 SubID
g_nGroupChat_DraggingSubID = 0

-- 그룹 채팅 로드 완료
function LoadGroupChat( nSubID )

	local tbInfo = g_tbGroupChat_RoomInfo[ nSubID ]
	if nil == tbInfo then
		return
	end

	-- 로그 ID 설정
	SetInt( WIDGET_ID_GROUPCHAT, "logID", tbInfo[ "nLogID" ], "chat.log", nSubID )
	
	-- 이벤트 전달용 SubID string
	local strSubID = tostring( nSubID )
	
	-- 닫기
	SetEvent( WIDGET_ID_GROUPCHAT, EVENT_WIDGET_CLOSE, "CloseGroupChat", "", "", strSubID, nSubID )
	-- 닫기 버튼 툴팁
	--SetString( WIDGET_ID_GROUPCHAT, "TooltipText", GetGameInternalText("INVITE_GROUP_CHAT_WINDOW_CLOSEBUTTON"), "closeBtn", strSubID, nSubID )	
	
	-- 초대하기
	local str = GetGameWordText( "MAX_GROUP_CHAT_INVITE", 0 )
	SetString( WIDGET_ID_GROUPCHAT, "label", str, "chat.btnInvite", nSubID )
	SetEvent( WIDGET_ID_GROUPCHAT, EVENT_BUTTON_CLICK, "Ev_GroupChat_Invite", "", "chat.btnInvite", strSubID, nSubID )
	
	-- 링크 아이템 뷰 visible 끄기
	for i = 1, g_nGroupChat_MAXLINKITEM, 1 do
	
		local strInst = "chat.link" .. tostring( i )
		local strParam = CreateParam( strSubID, i )
	
		SetBool( WIDGET_ID_GROUPCHAT, "visible", false, strInst, nSubID )
		SetEvent( WIDGET_ID_GROUPCHAT, EVENT_BUTTON_CLICK, "Ev_GroupChat_RemoveLinkItem", 
			"", strInst .. ".btnClose", strParam, nSubID )
	
	end
	
	-- 입력 이벤트
	SetEvent( WIDGET_ID_GROUPCHAT, EVENT_KEY_UP, "Ev_GroupChat_KeyUp", "keyCode", "chat.input", strSubID, nSubID )
	
	-- 리스트 아이템 클릭
	SetEvent( WIDGET_ID_GROUPCHAT, EVENT_LIST_ITEM_CLICK, 
		"Ev_GroupChat_ListClick", "index", "chat.list", strSubID, nSubID )
		
	-- 메뉴 설정 ---------------------------------
	-- 정보보기
	str = GetGameWordText( "CHAT_GROUP_POP_UP_MENU_INFO", 0 )
	local strParam = CreateParam( strSubID, 1 )
	SetString( WIDGET_ID_GROUPCHAT, "label", str, "menu.btnRefer", nSubID )
	SetEvent( WIDGET_ID_GROUPCHAT, EVENT_BUTTON_CLICK, "Ev_GroupChat_Menu", "", "menu.btnRefer", strParam, nSubID )
	
	-- 친구 등록
	str = GetGameWordText( "CHAT_GROUP_POP_UP_MENU_FRIEND_REGISTER", 0 )
	strParam = CreateParam( strSubID, 2 )
	SetString( WIDGET_ID_GROUPCHAT, "label", str, "menu.btnFriend", nSubID )
	SetEvent( WIDGET_ID_GROUPCHAT, EVENT_BUTTON_CLICK, "Ev_GroupChat_Menu", "", "menu.btnFriend", strParam, nSubID )
	
	-- 차단 등록
	str = GetGameWordText( "CHAT_GROUP_POP_UP_MENU_BLOCK_REGISTER", 0 )
	strParam = CreateParam( strSubID, 3 )
	SetString( WIDGET_ID_GROUPCHAT, "label", str, "menu.btnAddBlock", nSubID )
	SetEvent( WIDGET_ID_GROUPCHAT, EVENT_BUTTON_CLICK, "Ev_GroupChat_Menu", "", "menu.btnAddBlock", strParam, nSubID )
	
	-- 차단 해제
	str = GetGameWordText( "CHAT_GROUP_POP_UP_MENU_BLOCK_RELEASE", 0 )
	strParam = CreateParam( strSubID, 4 )
	SetString( WIDGET_ID_GROUPCHAT, "label", str, "menu.btnRemoveBlock", nSubID )
	SetEvent( WIDGET_ID_GROUPCHAT, EVENT_BUTTON_CLICK, "Ev_GroupChat_Menu", "", "menu.btnRemoveBlock", strParam, nSubID )
	
	-- 파티 초대
	str = GetGameWordText( "CHAT_GROUP_POP_UP_MENU_INVITE", 0 )
	strParam = CreateParam( strSubID, 5 )
	SetString( WIDGET_ID_GROUPCHAT, "label", str, "menu.btnParty", nSubID )
	SetEvent( WIDGET_ID_GROUPCHAT, EVENT_BUTTON_CLICK, "Ev_GroupChat_Menu", "", "menu.btnParty", strParam, nSubID )
	
	-- 강제 퇴출 -- 자신이 마스터가 아닌경우에는 버튼활성을 막는다.
	if g_MyChar:GetName() == tbInfo["strOwner"] then 
		str = GetGameWordText( "CHAT_GROUP_POP_UP_MENU_GET_OUT_ROOM", 0 )
		strParam = CreateParam( strSubID, 6 )
		SetString( WIDGET_ID_GROUPCHAT, "label", str, "menu.btnSecede", nSubID )
		SetEvent( WIDGET_ID_GROUPCHAT, EVENT_BUTTON_CLICK, "Ev_GroupChat_Menu", "", "menu.btnSecede", strParam, nSubID )
	else
		str = GetGameWordText( "CHAT_GROUP_POP_UP_MENU_GET_OUT_ROOM", 0 )
		SetString( WIDGET_ID_GROUPCHAT, "label", str, "menu.btnSecede", nSubID )
		SetBool( WIDGET_ID_GROUPCHAT, "enabled", false, "menu.btnSecede", nSubID )	
	end

	-- 최소화 타이틀 visible 끄기
	SetBool( WIDGET_ID_GROUPCHAT, "visible", false, "mini", nSubID )
	
	-- 최소화
	SetString( WIDGET_ID_GROUPCHAT, "TooltipText", GetGameWordText("INVITE_GROUP_CHAT_WINDOW_MINIMIZE", 0), "chat.btnMinimize", strSubID, nSubID )	-- 최소화 버튼 툴팁
	SetEvent( WIDGET_ID_GROUPCHAT, EVENT_BUTTON_CLICK, "Ev_GroupChat_Minimize", "", "chat.btnMinimize", strSubID, nSubID )
	-- 최대화
	SetString( WIDGET_ID_GROUPCHAT, "TooltipText", GetGameWordText("INVITE_GROUP_CHAT_WINDOW_MINIMIZE", 1), "mini.btnMaximize", strSubID, nSubID )	-- 최소화 버튼 툴팁
	SetEvent( WIDGET_ID_GROUPCHAT, EVENT_BUTTON_CLICK, "Ev_GroupChat_Maximize", "", "mini.btnMaximize", strSubID, nSubID )
	
	-- 최소화 타이틀 드래그
	SetEvent( WIDGET_ID_GROUPCHAT, EVENT_MOUSE_DOWN, "Ev_GroupChat_DragStart", "stageX;stageY", "mini.title", strSubID, nSubID )
	
	-- 저장된 채팅 로그 출력
	--AddStoredChat( tbInfo[ "nLogID" ] )
	-- 오픈시 저장된 채팅 로그 출력하도록 수정 
	
end

-- 그룹 채팅 오픈
function OpenGroupChat( nSubID )

	local tbInfo = g_tbGroupChat_RoomInfo[ nSubID ]	
	if nil == tbInfo then
		return
	end
		
	-- 링크 아이템 뷰 visible 끄기
	for i = 1, g_nGroupChat_MAXLINKITEM, 1 do
	
		-- 이벤트 전달용 SubID string
		local strSubID = tostring( nSubID )
	
		local strInst = "chat.link" .. tostring( i )
		local strParam = CreateParam( strSubID, i )
	
		SetBool( WIDGET_ID_GROUPCHAT, "visible", false, strInst, nSubID )
		SetEvent( WIDGET_ID_GROUPCHAT, EVENT_BUTTON_CLICK, "Ev_GroupChat_RemoveLinkItem", 
			"", strInst .. ".btnClose", strParam, nSubID )
	
	end
	
	
	-- 메뉴 visible off
	SetBool( WIDGET_ID_GROUPCHAT, "visible", false, "menu", nSubID )
	
	-- 최소화 타이틀 텍스트 설정
	SetString( WIDGET_ID_GROUPCHAT, "text", tbInfo[ "strOwner" ], "mini.title.label", nSubID )
	
	-- 채팅 멤버 설정
	GroupChat_SetMember( nSubID )
	
	-- 오픈은 항상 최대화 상태로
	GroupChat_Maximize( true, nSubID )
	
	-- 저장된 채팅 로그 출력
	AddStoredChat( tbInfo[ "nLogID" ] )
	-- 로드시 저장된 채팅을 출력할경우 UI로드 이후에는 작동하지 않아 
	-- 오픈시 저장된 채팅로그 출력하다록 수정 	
	
	-- 강제 퇴출 -- 자신이 마스터가 아닌경우에는 버튼활성을 막는다.
	if g_MyChar:GetName() == tbInfo["strOwner"] then 
		SetBool( WIDGET_ID_GROUPCHAT, "enabled", true, "menu.btnSecede", nSubID )	
	else
		SetBool( WIDGET_ID_GROUPCHAT, "enabled", false, "menu.btnSecede", nSubID )	
	end

	
end

-- 그룹 채팅 클로즈
function CloseGroupChat( strSubID )

	local tbInfo = g_tbGroupChat_RoomInfo[ tonumber( strSubID ) ]
	if nil == tbInfo then
		return
	end
	
	-- 채팅방 나가기 요청
	g_Msg:ReqQuitGroupChat( tbInfo[ "nKey" ] )

end

function GroupChat_Reset()

	g_tbGroupChat_RoomInfo = {}

end

-- 채팅 메시지 만들기
function GroupChat_MakeChatMsg( strName, strMsg )

	local str = "(" .. strName .. ") : " .. strMsg
	return str

end

-- 채팅방 최소/최대화 
function GroupChat_Maximize( bMaximize, nSubID )

	local tbInfo = g_tbGroupChat_RoomInfo[ nSubID ]	
	if nil == tbInfo then
		return
	end
	
	-- 최소화 상태 저장
	tbInfo[ "bMinimize" ] = not bMaximize

	SetBool( WIDGET_ID_GROUPCHAT, "visible", not bMaximize, "mini", nSubID )
	SetBool( WIDGET_ID_GROUPCHAT, "visible", bMaximize, "background", nSubID )
	SetBool( WIDGET_ID_GROUPCHAT, "visible", bMaximize, "titleBtn", nSubID )
	SetBool( WIDGET_ID_GROUPCHAT, "visible", bMaximize, "closeBtn", nSubID )
	SetBool( WIDGET_ID_GROUPCHAT, "visible", bMaximize, "mcFocus", nSubID )
	SetBool( WIDGET_ID_GROUPCHAT, "visible", bMaximize, "hit", nSubID )
	SetBool( WIDGET_ID_GROUPCHAT, "visible", bMaximize, "bound", nSubID )
	SetBool( WIDGET_ID_GROUPCHAT, "visible", bMaximize, "chat", nSubID )

end

-- 채팅 방 인덱스 검색
function GroupChat_FindIndex( nKey )

	local nIndex = -1

	for value in pairs( g_tbGroupChat_RoomInfo ) do
	
		if nKey == g_tbGroupChat_RoomInfo[ value ][ "nKey" ] then
			
			nIndex = value
			break
			
		end
	
	end
	
	return nIndex

end

-- 채팅 멤버 설정
function GroupChat_SetMember( nIndex )

	local tbInfo = g_tbGroupChat_RoomInfo[ nIndex ]	
	if nil == tbInfo then
		return
	end

	local tbMember = g_GroupChat:GetMember( tbInfo[ "nKey" ] )
	local strMyCharName = g_MyChar:GetName()
	
	local tbList = {}
	for value in pairs( tbMember ) do
		
		local strIsFriend
		
		if strMyCharName ~= tbMember[ value ] then
		
			if g_Friend:IsFriend(tbMember[ value ]) == true then
				strIsFriend = GetGameWordText("MAX_GROUP_CHAT_STATE_FRIEND")
			else
				strIsFriend = GetGameWordText("MAX_GROUP_CHAT_STATE_NORMAL")
			end

		else
			strIsFriend = ""
		end
		
		local tbData = 
		{
			tbMember[ value ]		-- 캐릭터 이름
			, strIsFriend
		}
		
		table.insert( tbList, tbData )
	
	end
	
		
	local strParam = CreateParam( "label", "TooltipText" )
	SetDataEx( WIDGET_ID_GROUPCHAT, strParam, tbList, "chat.list", false, nIndex )
	
	-- 타이틀 설정
	local str = StringFormat( GetGameWordText( "MAX_GROUP_CHAT_ROOM_MASTER", 0 ), 
		tbInfo[ "strOwner" ], table.getn( tbMember ), g_GroupChat:GetMaxGroupChatTO() )
	SetString( WIDGET_ID_GROUPCHAT, "title", str, "", nIndex )
end

--------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 빈방인가?
function GroupChat_IsRoomFree( nIndex )

	-- 해당 인덱스는 사용중인 방이 아니다.
	if nil == g_tbGroupChat_RoomInfo[ nIndex + 1 ] then
	
		return true
		
	end
	
	return false

end

-- 채팅 방 개설
function GroupChat_CreateRoom( nIndex, nKey, strOwnerName )

	nIndex = nIndex + 1

	-- 채팅방 정보 생성
	local tbInfo =
	{
		nKey 		= nKey,							-- 키
		strOwner 	= strOwnerName,					-- 방잠 이름
		nLogID 		= CHAT_LOG_GROUPCHAT + nIndex,	-- 로그 ID
		
		nMouseX		= 0,		-- 드래그 마우스 X
		nMouseY		= 0,		-- 드래그 마우스 Y
		bMinimize	= false,	-- 최소화중?
		strMenuChar = ""		-- 메뉴 연 캐릭터 이름
	}

	-- 채팅방 정보 저장
	g_tbGroupChat_RoomInfo[ nIndex ] = tbInfo
	
	OpenWidget( WIDGET_ID_GROUPCHAT, nIndex )

end

-- 채팅 방 삭제
function GroupChat_DeleteRoom( nKey )

	local nIndex = GroupChat_FindIndex( nKey )
	if -1 == nIndex then
		return
	end
	
	-- 채팅 로그 클리어
	ClearStoredChat( g_tbGroupChat_RoomInfo[ nIndex ][ "nLogID" ] )
	CallFunction( WIDGET_ID_GROUPCHAT, "Clear", "", "chat.log", nIndex )
	
	-- 채팅방 정보 삭제
	g_tbGroupChat_RoomInfo[ nIndex ] = nil
	
	-- 채팅방 닫기
	CloseWidget( WIDGET_ID_GROUPCHAT, nIndex )

end

-- 키 가져오기
function GroupChat_GetKey( nIndex )

	local tbInfo = g_tbGroupChat_RoomInfo[ nIndex + 1 ]
	if nil == tbInfo then
	
		return -1
		
	end
	
	return tbInfo[ "nKey" ]

end

-- 채팅 멤버 업데이트
function GroupChat_UpdateMember( nKey, strName, bJoin, bSelf )

	local nIndex = GroupChat_FindIndex( nKey )
	if -1 == nIndex then
		return
	end
	
	-- 자기자신한테 보내는 메세지인가?
	if true == bSelf then
		if true == bJoin then
			local strMsg = ""
			strMsg = StringFormat( AnsiToUTF8( GetGameWordText( "MAX_GROUP_CHAT_JOIN_SECURITY_MSG", 0 ) ), strName )
			
			local tbInfo = g_tbGroupChat_RoomInfo[ nIndex ]	
			if nil == tbInfo then
				return
			end
			
			AddChatMessage( tbInfo[ "nLogID" ], CHAT_ALL, false, strMsg, TEXT_COLOR_YELLOW, {}, {} )
		end
	end
	
	local strJoin_Out = ""
		
	if bJoin == true then
		strJoin_Out = StringFormat( GetGameWordText( "MAX_GROUP_CHAT_JOIN_MSG", 0 ), strName )
	else
		strJoin_Out = StringFormat( GetGameWordText( "MAX_GROUP_CHAT_OUT_MSG", 0 ), strName )
	end
	strJoin_Out = AnsiToUTF8(strJoin_Out)
		
	local tbInfo = g_tbGroupChat_RoomInfo[ nIndex ]	
	if nil == tbInfo then
		return
	end
		
	AddChatMessage( tbInfo[ "nLogID" ], CHAT_ALL, false, strJoin_Out, TEXT_COLOR_GREENYELLOW, {}, {} )

	
	-- 멤버 설정
	GroupChat_SetMember( nIndex )

end

-- 채팅창 인풋 포커스 설정
function GroupChat_SetFocus()

	if true == g_bGroupChat_FocusClean then
	
		g_bGroupChat_FocusClean = false
		return true
		
	end

	for value in pairs( g_tbGroupChat_RoomInfo ) do
	
		local tbInfo = g_tbGroupChat_RoomInfo[ value ]
		
		-- 최소화된 창은 무시
		if false == tbInfo[ "bMinimize" ] then
		
			SetFocus( WIDGET_ID_GROUPCHAT, "chat.input", value )
			return true
			
		end
	
	end
	
	return false

end

-- 메시지 수신
function GroupChat_RecvMsg( nKey, strName, strMsg, tbLinkItem )

	local nIndex = GroupChat_FindIndex( nKey )
	if -1 == nIndex then
		return
	end
	
	local tbInfo = g_tbGroupChat_RoomInfo[ nIndex ]	
	if nil == tbInfo then
		return
	end
	
	-- 아이템 링크 없다.
	if nil == tbLinkItem then
	
		local strNewMsg = "(" .. AnsiToUTF8(strName) .. ") : " .. strMsg
		AddChatMessage( tbInfo[ "nLogID" ], CHAT_ALL, false, strNewMsg, TEXT_COLOR_WHITE, {}, {} )
		
	else
	
		local str = "(" .. AnsiToUTF8(strName) .. ") : " .. strMsg
		local nLength = GetStringLen( str )
		
		for value in pairs( tbLinkItem ) do
		
			local tbItem = tbLinkItem[ value ]
			
			-- 아이템 이름 추가
			local strItem = "<" .. tbItem[ "strName" ] .. ">"
			str = str .. strItem
			
			-- 아이템 링크 텍스트 위치 만들기
			tbItem[ "nStart" ] = nLength
			tbItem[ "nEnd" ] = nLength + GetStringLen( strItem )
			
			nLength = tbItem[ "nEnd" ]
		
		end
		
		AddLinkMessage( tbInfo[ "nLogID" ], CHAT_ALL, false, AnsiToUTF8(str), TEXT_COLOR_WHITE, {}, tbLinkItem, {} )
	
	end
	
	if true == tbInfo[ "bMinimize" ] then
	
		CallFunction( WIDGET_ID_GROUPCHAT, "gotoAndPlay", "notify", "mini.title", nIndex )
	
	end

end

-- 활성된 채팅방 키
function GroupChat_GetEnableRoomKey()

	local tbInfo = nil
	
	for value in pairs( g_tbGroupChat_RoomInfo ) do
	
		if false == g_tbGroupChat_RoomInfo[ value ][ "bMinimize" ] then
			
			tbInfo = g_tbGroupChat_RoomInfo[ value ]
			break
			
		end
	
	end
	
	if nil == tbInfo then
		return -1
	end
	
	return tbInfo[ "nKey" ]

end

-- 아이템 링크 업데이트
function GroupChat_UpdateLinkItem( nKey )

	local nIndex = GroupChat_FindIndex( nKey )
	if -1 == nIndex then
		return
	end
	
	local tbInfo = g_tbGroupChat_RoomInfo[ nIndex ]	
	if nil == tbInfo then
		return
	end
	
	--[[ 아이템 링크 정보 가져오기
	{
		{
			1 : 아이템 이름
			2 : 아이템 컬러
		}
		...
	}
	]]
	local tbLinkItem = g_GroupChat:GetLinkItemInfo( nKey )
	for i = 1, g_nGroupChat_MAXLINKITEM, 1 do
	
		local tbItem = tbLinkItem[ i ]
		local strInst = "chat.link" .. tostring( i )
		
		if nil == tbItem then
		
			SetBool( WIDGET_ID_GROUPCHAT, "visible", false, strInst, nIndex )
		
		else
		
			SetBool( WIDGET_ID_GROUPCHAT, "visible", true, strInst, nIndex )
			
			local str = HTMLFontColor( tbItem[ 1 ], TranslateHTMLColor( tbItem[ 2 ] ) )
			SetString( WIDGET_ID_GROUPCHAT, "htmlText", str, strInst .. ".label", nIndex )
		
		end
	
	end

end

--------------------------------------------------------------------------------------------------------
-- 이벤트

-- 초대하기
function Ev_GroupChat_Invite( strSubID )

	local nSubID = tonumber( strSubID )

	local tbInfo = g_tbGroupChat_RoomInfo[ nSubID ]
	if nil == tbInfo then
		return
	end
	--if g_MyChar:GetName() ~= tbInfo["strOwnder"] then
	if g_MyChar:GetDBNum() ~= tbInfo["nKey"] then
		CF_PrintMsg_SystemMsg( GetGameWordText( "MAX_GROUP_CHAT_INVITE_BUTTON_MSG", 0 ), TEXT_COLOR_RED )
		return
	end
	
	OpenWidget( WIDGET_ID_INVITEGROUPCHAT )

end

-- 최소화
function Ev_GroupChat_Minimize( strSubID )

	local nSubID = tonumber( strSubID )
	
	-- 최소화 타이틀 위치 원래대로
	SetInt( WIDGET_ID_GROUPCHAT, "x", 0, "mini", nSubID )
	SetInt( WIDGET_ID_GROUPCHAT, "y", 0, "mini", nSubID )

	GroupChat_Maximize( false, nSubID )
	SetBool( WIDGET_ID_GROUPCHAT, "visible", false, "menu", nSubID )
end

-- 최대화
function Ev_GroupChat_Maximize( strSubID )

	local nSubID = tonumber( strSubID )
	
	-- 최대화시 필요한 변수 구함 ( 최소화 상태의 위치값을 받기 위해 )
	local miniX = GetInt( WIDGET_ID_GROUPCHAT, "x", "mini", nSubID )
	local miniY = GetInt( WIDGET_ID_GROUPCHAT, "y", "mini", nSubID )

	local originalX = GetInt( WIDGET_ID_GROUPCHAT, "x", "", nSubID )
	local originalY = GetInt( WIDGET_ID_GROUPCHAT, "y", "", nSubID )
	
	GroupChat_Maximize( true, nSubID )

	SetInt( WIDGET_ID_GROUPCHAT, "x", originalX + miniX, "", nSubID )
	SetInt( WIDGET_ID_GROUPCHAT, "y", originalY + miniY, "", nSubID )
	
	CallFunction( WIDGET_ID_GROUPCHAT, "gotoAndPlay", "default", "mini.title", nSubID )

end


-- 입력 이벤트
function Ev_GroupChat_KeyUp( nKeyCode, strSubID )

	if KEYCODE_ENTER ~= nKeyCode then
		return
	end

	local nSubID = tonumber( strSubID )

	local tbInfo = g_tbGroupChat_RoomInfo[ nSubID ]
	if nil == tbInfo then
		return
	end
	
	local tbLinkItem = g_GroupChat:GetLinkItemInfo( tbInfo[ "nKey" ] )
	local tbItem = tbLinkItem[ 1 ]
	
	local strMsg = GetString( WIDGET_ID_GROUPCHAT, "text", "chat.input", nSubID )
	
	-- 텍스트 안썼으면 인풋 포커스 없애기 -- tbItem 이 존재한다면 아이템 링크를 보내야함
	if 0 == string.len( strMsg ) and tbItem == nil then
		SetFocus( -1, "" )
		g_bGroupChat_FocusClean = true
		return
	end
	
	-- 인풋 텍스트 비우기
	SetString( WIDGET_ID_GROUPCHAT, "text", "", "chat.input", nSubID )
	
	-- 메시지 보내기
	g_Msg:SendChatGroupChat( tbInfo[ "nKey" ], strMsg )
	
	-- 아이템 링크 업데이트
	GroupChat_UpdateLinkItem( tbInfo[ "nKey" ] )

end

-- 아이템 링크 제거
function Ev_GroupChat_RemoveLinkItem( strSubID, nIndex )

	local tbInfo = g_tbGroupChat_RoomInfo[ tonumber(strSubID) ]
	if nil == tbInfo then
		return
	end
		
	-- 아이템 링크 제거
	g_GroupChat:RemoveLinkItem( tbInfo[ "nKey" ], nIndex - 1 )
	-- 아이템 링크 업데이트
	GroupChat_UpdateLinkItem( tbInfo[ "nKey" ] )

end

-- 리스트 클릭 메뉴 열기
function Ev_GroupChat_ListClick( nIndex, strSubID )

	local nSubID = tonumber( strSubID )
	
	local tbInfo = g_tbGroupChat_RoomInfo[ nSubID ]
	if nil == tbInfo then
		return
	end
	
	local strName = GetDataString( WIDGET_ID_GROUPCHAT, nIndex, "label", "chat.list", nSubID )
	-- 내 캐릭터이거나 이미 클릭한 놈이면 메뉴 끄고 종료
	if  strName == g_MyChar:GetName() or
		strName == tbInfo[ "strMenuChar" ] then
		
		tbInfo[ "strMenuChar" ] = ""
	
		SetBool( WIDGET_ID_GROUPCHAT, "visible", false, "menu", nSubID )
		SetInt( WIDGET_ID_GROUPCHAT, "selectedIndex", -1, "chat.list", nSubID )
		return
		
	end
	
	-- 메뉴 연 캐릭터 이름 저장
	tbInfo[ "strMenuChar" ] = strName
	
	-- 메뉴 visible 켜기
	SetBool( WIDGET_ID_GROUPCHAT, "visible", true, "menu", nSubID )
	
	-- 메뉴 위치 설정
	local nScrollPos = GetInt( WIDGET_ID_GROUPCHAT, "scrollPosition", "chat.list", nSubID )
	local nListX = GetInt( WIDGET_ID_GROUPCHAT, "x", "chat.list", nSubID )
	local nListY = GetInt( WIDGET_ID_GROUPCHAT, "y", "chat.list", nSubID )
	local nListW = GetInt( WIDGET_ID_GROUPCHAT, "width", "chat.list", nSubID )
	
	local nX = nListX + nListW
	local nY = 24 * (nIndex - nScrollPos)
	
	SetInt( WIDGET_ID_GROUPCHAT, "x", nX, "menu", nSubID )
	SetInt( WIDGET_ID_GROUPCHAT, "y", nY, "menu", nSubID )

end

-- 메뉴 클릭
function Ev_GroupChat_Menu( strSubID, strType )

	local nType = tonumber( strType )
	local nSubID = tonumber( strSubID )
	
	local tbInfo = g_tbGroupChat_RoomInfo[ nSubID ]
	if nil == tbInfo then
		return
	end
	
	local strName = tbInfo[ "strMenuChar" ]
	if 0 == string.len( strName ) then
		return
	end
	
	-- 정보 보기
	if 1 == nType then
	
		g_Msg:ReqReferChar( strName )
		g_ReferChar_Name = strName
	
		
	-- 친구 등록
	elseif 2 == nType then
	
		g_Msg:ReqFriendAdd( strName )
	
	-- 차단 등록
	elseif 3 == nType then
	
		g_Msg:ReqFriendBlock( strName, true )
	
	-- 차단 해제
	elseif 4 == nType then
	
		g_Msg:ReqFriendBlock( strName, false )
	
	-- 파티 초대
	elseif 5 == nType then
	
		g_Msg:LureNameParty( strName )
	
	-- 강제 퇴출
	elseif 6 == nType then
	
		g_Msg:ReqKickGroupChat( strName )
	
	end
	
	-- 메뉴 visible 끄기
	SetBool( WIDGET_ID_GROUPCHAT, "visible", false, "menu", nSubID )

end

--------------------------------------------------------------------------------------------------------
-- 메시지 박스

-- 그룹 채팅 초대 메시지 발신
function GroupChat_InviteMsg( strName )

	local strMsg = StringFormat( GetGameWordText( "INVITE_GROUP_CHAT_WINDOW_INVITE_MSG", 0 ), strName )
	
	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, strMsg, Ev_GroupChat_InviteMsgOK, Event_MsgBox_Close )
	
end

-- 그룹 채팅 요청 발신 확인
function Ev_GroupChat_InviteMsgOK() 

	if false == g_Msg:ReqGroupChat( g_strInteractMenu_Name, GROUPCHAT_INVITE_PTOP ) then
	
		--CF_PrintMsg_SystemMsg( GetGameInternalText( "PTOPMENUREJECT_MESSAGE5", 0 ), TEXT_COLOR_RED )
		
	end
	
	CloseWidget( WIDGET_ID_INTERACT_MENU )
	Event_MsgBox_Close()
	
end

-- 그룹 채팅 요청 수신
function GroupChat_InviteMsgRecv( strName, nIndex )

	local strMsg = StringFormat( GetGameWordText( "GROUP_MODAL_WINDOW_INVITE_MSG", 0 ), strName )
	local strParam = tostring( nIndex )
	
	OpenReportMsgBox( MSG_BOX_TYPE_OKCANCEL, strMsg, Ev_GroupChat_InviteMsgRecvOK, Ev_GroupChat_InviteMsgRecvCancel )
	
	SetEvent( WIDGET_ID_REPORTMESSAGEBOX, EVENT_BUTTON_CLICK, 
		"Ev_GroupChat_InviteMsgRecvOK", "", "msgBoxOKCancel.btnOK", strParam )
	SetEvent( WIDGET_ID_REPORTMESSAGEBOX, EVENT_BUTTON_CLICK, 
		"Ev_GroupChat_InviteMsgRecvCancel", "", "msgBoxOKCancel.btnCancel", strParam )
	
end

-- 그룹 채팅 요청 수신 확인
function Ev_GroupChat_InviteMsgRecvOK( nIndex )

	-- 수락
	g_Msg:ReqGroupChatFB( true )
	
	Invitation_Table_Remove( nIndex )
	Event_ReportMsgBox_Close()
	
end

-- 그룹 채팅 요청 수신 거절
function Ev_GroupChat_InviteMsgRecvCancel( nIndex )

	if g_bReportBoxInputESC == false then
	
		-- 거절
		g_Msg:ReqGroupChatFB( false )
		
		Invitation_Table_Remove( nIndex )

	end
	
	g_bReportBoxInputESC = false
	Event_ReportMsgBox_Close()
end

-------------------------------------------------------------------------------------
-- 드래그 이벤트

-- 드래그 시작
function Ev_GroupChat_DragStart( nStageX, nStageY, strSubID )

	local nSubID = tonumber( strSubID )

	local tbInfo = g_tbGroupChat_RoomInfo[ nSubID ]
	if nil == tbInfo then
		return
	end
	
	-- 드래그 마우스 위치 저장
	tbInfo[ "nMouseX" ] = nStageX
	tbInfo[ "nMouseY" ] = nStageY
	
	g_nGroupChat_DraggingSubID = nSubID
	
	InsertStageMouseMove( Ev_GroupChat_Drag )
	InsertStageMouseUp( Ev_GroupChat_DragEnd )

end

-- 드래그
function Ev_GroupChat_Drag( nStageX, nStageY )

	local tbInfo = g_tbGroupChat_RoomInfo[ g_nGroupChat_DraggingSubID ]
	if nil == tbInfo then
		return
	end
	
	local nX = GetInt( WIDGET_ID_GROUPCHAT, "x", "mini", g_nGroupChat_DraggingSubID )
	local nY = GetInt( WIDGET_ID_GROUPCHAT, "y", "mini", g_nGroupChat_DraggingSubID )
	local nW = GetInt( WIDGET_ID_GROUPCHAT, "width", "mini", g_nGroupChat_DraggingSubID )
	local nH = GetInt( WIDGET_ID_GROUPCHAT, "height", "mini", g_nGroupChat_DraggingSubID )
	
	local nWindowX = GetInt( WIDGET_ID_GROUPCHAT, "x", "", g_nGroupChat_DraggingSubID )
	local nWindowY = GetInt( WIDGET_ID_GROUPCHAT, "y", "", g_nGroupChat_DraggingSubID )
	local nWindowW = GetInt( WIDGET_ID_GROUPCHAT, "width", "", g_nGroupChat_DraggingSubID )
	local nWindowH = GetInt( WIDGET_ID_GROUPCHAT, "height", "", g_nGroupChat_DraggingSubID )
	
	local nScreenW = GetScreenWidth()
	local nScreenH = GetScreenHeight()
	
	local nNewX = nX + ( nStageX - tbInfo[ "nMouseX" ] )
	local nNewY = nY + ( nStageY - tbInfo[ "nMouseY" ] )
	
	if nWindowX + nNewX < 0 then
	
		nNewX = nWindowX * -1
		
	elseif nWindowX + nNewX + nW > nScreenW then
	
		nNewX = nScreenW - nWindowX - nW
	
	end
	
	if nWindowY + nNewY < 0 then
	
		nNewY = nWindowY * -1
		
	elseif nWindowY + nNewY + nH > nScreenH then
	
		nNewY = nScreenH - nWindowY - nH
		
	end
	
	-- 드래그 마우스 위치 저장
	tbInfo[ "nMouseX" ] = nStageX
	tbInfo[ "nMouseY" ] = nStageY
	
	SetInt( WIDGET_ID_GROUPCHAT, "x", nNewX, "mini", g_nGroupChat_DraggingSubID )
	SetInt( WIDGET_ID_GROUPCHAT, "y", nNewY, "mini", g_nGroupChat_DraggingSubID )

end

-- 드래그 끝
function Ev_GroupChat_DragEnd( nStageX, nStageY )

	RemoveStageMouseMove( Ev_GroupChat_Drag )
	RemoveStageMouseUp( Ev_GroupChat_DragEnd )
	
	g_nGroupChat_DraggingSubID = 0

end