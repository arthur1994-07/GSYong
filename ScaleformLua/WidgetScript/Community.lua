
COMMUNITY_FRIEND_POPUP_FRIEND_MAX	= 8
COMMUNITY_FRIEND_POPUP_GROUP_MAX	= 2
COMMUNITY_FRIEND_SLOT_MAX			= 11

g_bCommunity_ShowAll = true
g_Community_FriendTab_SelectedFriendName = nil

g_nGroupPopupSelectIndex = -1

g_bCommunity_Init = false

g_bCommunity_IsOpen = false

function LoadCommunity()

	SetEvent( WIDGET_ID_COMMUNITY, EVENT_WIDGET_CLOSE, "CloseCommunity", "", "" )

	-- 탭
	local tb = 
	{
		{ GetGameWordText( "RNFRIEND_WINDOW", 1 ) },
		{ GetGameWordText( "RNFRIEND_WINDOW", 2 ) },
	}
	SetDataEx( WIDGET_ID_COMMUNITY, "label", tb, "tab", false )
	SetEvent( WIDGET_ID_COMMUNITY, EVENT_INDEX_CLICK_CHANGE, "Ev_Community_Tab", "index", "tab" )
	SetInt( WIDGET_ID_COMMUNITY, "selectedIndex", 0, "tab" )
	
	-- 친구 페이지 ----------------------------------------------------------------------------------
	
	-- 보기
	SetString( WIDGET_ID_COMMUNITY, "text", GetGameWordText( "RNFRIEND_FRIENDTAB", 2 ), "friendPage.type" )	
	-- 친구추가
	SetString( WIDGET_ID_COMMUNITY, "label", GetGameWordText( "RNFRIEND_FRIENDTAB_BTN", 0 ), "friendPage.addFriend" )
	SetEvent( WIDGET_ID_COMMUNITY, EVENT_BUTTON_CLICK, "Ev_Community_FriendTab_AddFriend", "", "friendPage.addFriend" )
	-- 그룹생성
	SetString( WIDGET_ID_COMMUNITY, "label", GetGameWordText( "RNFRIEND_FRIENDTAB_BTN", 1 ), "friendPage.addGroup" )
	SetEvent( WIDGET_ID_COMMUNITY, EVENT_BUTTON_CLICK, "Ev_Community_FriendTab_AddGroup", "", "friendPage.addGroup" )
	
	-- 보기 - 전체/온라인
	tb = 
	{
		{ GetGameWordText( "RNFRIEND_FRIENDTAB_COMBO", 0 ) },
		{ GetGameWordText( "RNFRIEND_FRIENDTAB_COMBO", 1 ) }
	}
	SetDataEx( WIDGET_ID_COMMUNITY, "label", tb, "friendPage.dropDown" )
	SetEvent( WIDGET_ID_COMMUNITY, EVENT_LIST_INDEX_CHANGE, "Ev_Community_FrieldTab_OnOff", "index", "friendPage.dropDown" )	
	SetInt( WIDGET_ID_COMMUNITY, "selectedIndex", 0, "friendPage.dropDown" )
	
	-- 친구 선택 팝업
	local tbPopupFriend = 
	{
		{ GetGameWordText( "INVITE_GROUP_CHAT_WINDOW_TITLE", 0 ) }
	}
	for i = 2, COMMUNITY_FRIEND_POPUP_FRIEND_MAX, 1 do
		tbPopupFriend[ i ] = { GetGameWordText( "RNFRIEND_FRIENDTAB_POPUP", i - 2 ) }
	end
	SetDataEx( WIDGET_ID_COMMUNITY, "label;", tbPopupFriend, "friendPage.popupFriend.main.list", false )
	SetEvent( WIDGET_ID_COMMUNITY, EVENT_LIST_ITEM_CLICK, "Ev_Community_FriendTab_FriendPopupClick",
		"buttonIdx;index;", "friendPage.popupFriend.main.list" )
	SetBool( WIDGET_ID_COMMUNITY, "visible", false, "friendPage.popupFriend" )
	SetEvent( WIDGET_ID_COMMUNITY, EVENT_LIST_ITEM_ROLLOVER, "Ev_Community_FriendTab_Popup_RollOver", 
		"index;", "friendPage.popupFriend.main.list" )
	SetEvent( WIDGET_ID_COMMUNITY, EVENT_LIST_ITEM_CLICK, "Ev_Community_FriendTab_FriendSubPopupClick",
		"buttonIdx;index;", "friendPage.popupFriend.sub.list" )
	
	-- 그룹 팝업
	local tbPopupGroup = {}
	for i = 1, COMMUNITY_FRIEND_POPUP_GROUP_MAX, 1 do
		tbPopupGroup[ i ] = { GetGameWordText("RNFRIEND_FRIENDTAB_GPOPUP", i - 1) }
	end
	SetDataEx( WIDGET_ID_COMMUNITY, "label;", tbPopupGroup, "friendPage.popupGroup.main", false )
	SetEvent( WIDGET_ID_COMMUNITY, EVENT_LIST_ITEM_CLICK, "Ev_Community_FriendTab_GroupPopupClick",
		"buttonIdx;index;", "friendPage.popupGroup.main" )
	SetBool( WIDGET_ID_COMMUNITY, "visible", false, "friendPage.popupGroup" )
	
	-- 친구 리스트 이벤트
	SetEvent( WIDGET_ID_COMMUNITY, EVENT_TREE_ITEM_EXPAND, "Ev_Community_FriendTab_ExpandList", "index", "friendPage.list" )
	SetEvent( WIDGET_ID_COMMUNITY, EVENT_LIST_ITEM_CLICK, "Ev_Community_FriendTab_ItemClick", 
		"buttonIdx;key;index;indexSub;", "friendPage.list" )
	
	-- 차단 페이지 ----------------------------------------------------------------------------------
	
	-- 차단 추가
	SetString( WIDGET_ID_COMMUNITY, "label", GetGameWordText("RNFRIEND_BLOCKTAB", 0), "blockPage.block" )
	SetEvent( WIDGET_ID_COMMUNITY, EVENT_BUTTON_CLICK, "Ev_Community_BlockTab_Block", "", "blockPage.block" )
	
	-- 차단 해제
	SetString( WIDGET_ID_COMMUNITY, "label", GetGameWordText("RNFRIEND_BLOCKTAB", 1), "blockPage.blockRelease" )
	SetEvent( WIDGET_ID_COMMUNITY, EVENT_BUTTON_CLICK, "Ev_Community_BlockTab_BlockRelease", "", "blockPage.blockRelease" )
	
	g_bCommunity_Init = false

end

function OpenCommunity()

	g_bCommunity_IsOpen = true

	if false == g_bCommunity_Init then
	
		Community_UpdateFriendList( true )
		g_bCommunity_Init = true
		
	end

	Community_UpdateCount()
	Community_UpdateBlock()

end

function CloseCommunity()

	g_bCommunity_IsOpen = false

end

function ResetCommunity()

	g_bCommunity_Init = false

end

-- 친구, 그룹 수 업데이트
function Community_UpdateCount()

	if false == g_bCommunity_IsOpen then
		return
	end

	-- 친구 등록 인원수
	local str = GetGameWordText( "RNFRIEND_FRIENDTAB", 0 ) .. g_Friend:GetFriendCount() .. "/" .. g_Friend:GetMaxFriend()
	SetString( WIDGET_ID_COMMUNITY, "text", str, "friendPage.count" )
	
	-- 그룹 개수
	local tbGroupList = g_Friend:GetGroupList()
	str = GetGameWordText( "RNFRIEND_FRIENDTAB", 1 ) .. table.getn( tbGroupList ).. "/" .. g_Friend:GetMaxGroup()
	SetString( WIDGET_ID_COMMUNITY, "text", str, "friendPage.groupCount" )

end

-- 그룹 편집 리스트 업데이트
function Community_UpdateGroupPopup()

	if false == g_bCommunity_IsOpen then
		return
	end

	local tbGroupList = g_Friend:GetGroupList()
	local nGroup = table.getn( tbGroupList )
	
	local instPopupSub = "friendPage.popupFriend.sub.item"
	local nGroupMax = g_Friend:GetMaxGroup()
	
	for i = 1, nGroup, 1 do
		SetBool( WIDGET_ID_COMMUNITY, "visible", true, instPopupSub .. i  )
	end	

	for i = nGroup + 1, nGroupMax, 1 do
		SetBool( WIDGET_ID_COMMUNITY, "visible", false, instPopupSub .. i  )
	end
	
	local nHeight = nGroup * GetInt( WIDGET_ID_COMMUNITY, "height", "friendPage.popupFriend.sub.item1" )
	SetDataEx( WIDGET_ID_COMMUNITY, "groupnum;label;", tbGroupList, "friendPage.popupFriend.sub.list", false )

end

-- 친구 리스트 업데이트
function Community_UpdateFriendList( bInit )

	if false == g_bCommunity_IsOpen then
		return
	end

	-- 헤드 데이터
	local tbHeadData = {}
	-- 키 데이터
	local tbKey = {}
	-- 트리 데이터
	local tbTreeData = {}
	
	local tbGroupList = g_Friend:GetGroupList()
	for value in pairs( tbGroupList ) do
	
		local tbGroupInfo = tbGroupList[ value ]
		
		-- 헤드
		local tb = 
		{
			"key" .. tbGroupInfo[ 1 ],	-- 키 (그룹 번호로 설정하자)
			"false",			-- 펼침 상태
			tbGroupInfo[ 1 ],	-- 그룹 번호
			tbGroupInfo[ 2 ],	-- 그룹 이름
		}
		
		if false == bInit then
		
			local tbHead = GetHeadData( WIDGET_ID_COMMUNITY, value - 1, "friendPage.list" )
			if nil ~= tbHead and true == tbHead[ "expand" ] then
			
				tb[ 2 ] = "true"
			
			end
			
		end
		table.insert( tbHeadData, tb )
		
		-- 키
		table.insert( tbKey, "key" .. tbGroupInfo[ 1 ] )
		
		-- 트리 데이터
		tb = {}
		
		local tbFriendList = g_Friend:GetFriendListInGroup( tbGroupInfo[ 1 ] )
		for value2 in pairs( tbFriendList ) do
		
			local tbFriend = tbFriendList[ value2 ]
			
			-- 오프라인, 전체 보기
			if false == tbFriend[ 6 ] and true == g_bCommunity_ShowAll then
			
				local tbTemp = 
				{
					HTMLFontColor( "Lv" .. tbFriend[ 2 ], HTML_TEXT_COLOR_GRAY ),	-- 레벨
					HTMLFontColor( tbFriend[ 3 ], HTML_TEXT_COLOR_GRAY ),			-- 이름
					" ",
					" ",
				}
				table.insert( tb, tbTemp )
			
			-- 온라인
			elseif true == tbFriend[ 6 ] then
			
				local tbTemp = 
				{
					"Lv" .. tbFriend[ 2 ],		-- 레벨
					tbFriend[ 3 ],				-- 이름
					tbFriend[ 4 ],				-- 맵 이름
					tbFriend[ 5 ],				-- 좌표
				}
				table.insert( tb, tbTemp )

			end
		
		end
		
		table.insert( tbTreeData, tb )
	
	end
	
	local strParam = CreateParam( "key", "expand", "groupnum", "label1" )
	SetHeadData( WIDGET_ID_COMMUNITY, strParam, tbHeadData, "friendPage.list" )
	
	local strParam = CreateParam( "label1", "label2", "label3", "label4" )
	SetTreeData( WIDGET_ID_COMMUNITY, tbKey, strParam, tbTreeData, "friendPage.list" )
	
	CallFunction( WIDGET_ID_COMMUNITY, "validateNow", "", "friendPage.list" )

end

-- 친구 리스트 상태 업데이트
function Community_UpdateState( nGroupNum )

	if false == g_bCommunity_IsOpen then
		return
	end

	local tbGroupList = g_Friend:GetGroupList()
	local nIndex = -1
	
	for value in pairs( tbGroupList ) do
	
		local tbGroupInfo = tbGroupList[ value ]
		if tbGroupInfo[ 1 ] == nGroupNum then
		
			nIndex = value - 1
			break
			
		end
	
	end
	
	if -1 == nIndex then
		return
	end
	
	local tbFriendList = g_Friend:GetFriendListInGroup( nGroupNum )
	for value in pairs( tbFriendList ) do
	
		local strInst = "friendPage.list.mcItemRenderer.item" .. nIndex .. "_" .. (value - 1)
		local tbFriend = tbFriendList[ value ]
		
		local tbTemp = {}
		
		-- 오프라인, 전체 보기
		if false == tbFriend[ 6 ] and true == g_bCommunity_ShowAll then
		
			SetBool( WIDGET_ID_COMMUNITY, "visible", false, strInst .. ".online" )
			SetBool( WIDGET_ID_COMMUNITY, "visible", true, strInst .. ".offline" )
			
			CallFunction( WIDGET_ID_COMMUNITY, "gotoAndStop", 
				Community_ConvertFrameToClass( tbFriend[ 1 ] ), strInst .. ".offline" )
				
			tbTemp = 
			{
				HTMLFontColor( "Lv" .. tbFriend[ 2 ], HTML_TEXT_COLOR_GRAY ),	-- 레벨
				HTMLFontColor( tbFriend[ 3 ], HTML_TEXT_COLOR_GRAY ),			-- 이름
				" ",
				" ",
			}
		
		-- 온라인
		elseif true == tbFriend[ 6 ] then
		
			SetBool( WIDGET_ID_COMMUNITY, "visible", true, strInst .. ".online" )
			SetBool( WIDGET_ID_COMMUNITY, "visible", false, strInst .. ".offline" )
			
			CallFunction( WIDGET_ID_COMMUNITY, "gotoAndStop", 
				Community_ConvertFrameToClass( tbFriend[ 1 ] ), strInst .. ".online" )
		
			tbTemp = 
			{
				"Lv" .. tbFriend[ 2 ],		-- 레벨
				tbFriend[ 3 ],				-- 이름
				tbFriend[ 4 ],				-- 맵 이름
				tbFriend[ 5 ],				-- 좌표
			}

		end
		
		if 0 ~= table.getn( tbTemp ) then
		
			local strParam = CreateParam( "label1", "label2", "label3", "label4" )
			ChangeTreeData( WIDGET_ID_COMMUNITY, "key" .. nGroupNum, value - 1, strParam, tbTemp, "friendPage.list" )
		
		end
	
	end
	
	CallFunction( WIDGET_ID_COMMUNITY, "updateData", "", "friendPage.list" )
	CallFunction( WIDGET_ID_COMMUNITY, "invalidateRenderer", "", "friendPage.list" )

end

-- 차단 리스트 업데이트
function Community_UpdateBlock()

	if false == g_bCommunity_IsOpen then
		return
	end

	local tbBlock = g_Friend:GetBlockList()
	if 0 == table.getn( tbBlock ) then
		RemoveData( WIDGET_ID_COMMUNITY, 0, "blockPage.list", true )
	else
		SetDataEx( WIDGET_ID_COMMUNITY, "label1;", tbBlock, "blockPage.list", false )
	end
	
	
	local strCount = " " .. table.getn( tbBlock ) .."/".. g_Friend:GetMaxBlock()
	SetString( WIDGET_ID_COMMUNITY, "text", strCount, "blockPage.count" )

end

function Community_ConvertFrameToClass( nClassIndex )

	if nClassIndex == CHARCLASS_IDX_FIGHTER_M then
		return "FIGHTER_M"
	elseif nClassIndex == CHARCLASS_IDX_ARMS_M then
		return "ARMS_M"
	elseif nClassIndex == CHARCLASS_IDX_ARCHER_W then
		return "ARCHER_W"
	elseif nClassIndex == CHARCLASS_IDX_SPIRIT_W then
		return "SPIRIT_W"
	elseif nClassIndex == CHARCLASS_IDX_EXTREAM_M then
		return "EXTREME_M"
	elseif nClassIndex == CHARCLASS_IDX_EXTREAM_W then
		return "EXTREME_W"
	elseif nClassIndex == CHARCLASS_IDX_FIGHTER_W then
		return "FIGHTER_W"
	elseif nClassIndex == CHARCLASS_IDX_ARMS_W then
		return "ARMS_W"
	elseif nClassIndex == CHARCLASS_IDX_ARCHER_M then
		return "ARCHER_M"
	elseif nClassIndex == CHARCLASS_IDX_SPIRIT_M then
		return "SPIRIT_M"
	elseif nClassIndex == CHARCLASS_IDX_SCIENTIST_M then
		return "SCIENTIST_M"
	elseif nClassIndex == CHARCLASS_IDX_SCIENTIST_W then
		return "SCIENTIST_W"
	elseif nClassIndex == CHARCLASS_IDX_ASSASSIN_M then
		return "ASSASSIN_M"
	elseif nClassIndex == CHARCLASS_IDX_ASSASSIN_W then
		return "ASSASSIN_W"
	elseif nClassIndex == CHARCLASS_IDX_TRICKER_M then
		return "MAGICIAN_M"
	elseif nClassIndex == CHARCLASS_IDX_TRICKER_W then
		return "MAGICIAN_W"
	elseif nClassIndex == CHARCLASS_IDX_ACTOR_M then
		return "ActorM"
	elseif nClassIndex == CHARCLASS_IDX_ACTOR_W then
		return "ActorF"
	end
	
	return nil
	
end

-- 친구팝업 열기
function Community_FriendTab_OpenFriendPopup()

	SetBool( WIDGET_ID_COMMUNITY, "visible", true, "friendPage.popupFriend" )
	SetInt( WIDGET_ID_COMMUNITY, "selectedIndex", -1, "friendPage.popupFriend.main.list" )
	
	SetBool( WIDGET_ID_COMMUNITY, "visible", false, "friendPage.popupFriend.sub" )
	
	local tbMousePos = GetMousePos()
	
	local nX = tbMousePos[ "x" ] - GetInt( WIDGET_ID_COMMUNITY, "x", "" )
	local nY = tbMousePos[ "y" ] - GetInt( WIDGET_ID_COMMUNITY, "y", "" )	
	nX = nX- GetInt( WIDGET_ID_COMMUNITY, "x", "friendPage" )
	nY = nY- GetInt( WIDGET_ID_COMMUNITY, "y", "friendPage" )

	SetInt( WIDGET_ID_COMMUNITY, "x", nX, "friendPage.popupFriend" )
	SetInt( WIDGET_ID_COMMUNITY, "y", nY, "friendPage.popupFriend" )

end

-- 그룹 채팅 초대
function Community_FriendTab_InviteGroupChat()

	g_Msg:ReqGroupChat( g_Community_FriendTab_SelectedFriendName, GROUPCHAT_INVITE_FRIEND )

end

-- 귓속말 등록
function Community_FriendTab_Whisper()

	BasicChat_SetWhisper( g_Community_FriendTab_SelectedFriendName )

end

-- 정보보기 
function Community_FriendTab_ShowFriendInfo()

	g_Msg:ReqReferChar( g_Community_FriendTab_SelectedFriendName )

end

-- 파티 초대
function Community_FriendTab_InviteParty()

	local strMsg = StringFormat( GetGameInternalText( "PARTY_QUESTION" ), g_Community_FriendTab_SelectedFriendName )
	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, strMsg, Ev_Community_FriendTab_Invite_Ok, Event_MsgBox_Close )

end

-- 파티 초대 확인
function Ev_Community_FriendTab_Invite_Ok()

	g_Msg:ReqFriendInviteParty( g_Community_FriendTab_SelectedFriendName )
	Event_MsgBox_Close()	

end

-- 친구 이동
function Community_FriendTab_MoveToFriend()

	local tbInvenItem = g_MyChar:GetItemPosInInvenByTable( ITEMTYPE_2FRIEND )
	if tbInvenItem[ 1 ] == false then
	
		CF_PrintMsg_SystemMsg( GetGameInternalText("EM2FRIEND_FB_NO_ITEM"), TEXT_COLOR_RED )
		return
		
	end
	
	OpenMsgBoxModaless( MSG_BOX_TYPE_CANCEL, GetGameInternalText( "WAITSERVER_MESSAGE" ), 
		Event_MsgBoxModaless_Close, Event_MsgBoxModaless_Close, nil, nil, 10, Community_FriendTab_MoveToFriend_Move )

end

-- 친구 이동 확인
function Community_FriendTab_MoveToFriend_Move()

	g_Msg:ReqMoveToFriend( g_Community_FriendTab_SelectedFriendName )
	Event_MsgBoxModaless_Close()

end

-- 차단
function Community_FriendTab_BlockFriend()

	local strMsg = StringFormat( GetGameInternalText( "FRIEND_BLOCK_ADD" ), g_Community_FriendTab_SelectedFriendName )
	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, strMsg, Ev_Community_FriendTab_BlockFriend_Ok, Event_MsgBox_Close )

end

-- 차단 확인
function Ev_Community_FriendTab_BlockFriend_Ok()

	g_Msg:ReqFriendBlock( g_Community_FriendTab_SelectedFriendName, true )
	Event_MsgBox_Close()

end

-- 삭제
function Community_FriendTab_DeleteFriend()

	local strMsg = StringFormat( GetGameInternalText( "FRIEND_DEL" ), g_Community_FriendTab_SelectedFriendName )
	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, strMsg, Ev_Community_FriendTab_DeleteFriend_Ok, Event_MsgBox_Close )

end

-- 삭제 확인
function Ev_Community_FriendTab_DeleteFriend_Ok()

	g_Msg:ReqFriendDel( g_Community_FriendTab_SelectedFriendName )
	Event_MsgBox_Close()

end

-- 그룹팝업 열기
function Community_FriendTab_OpenGroupPopup( nIndex )

	SetBool( WIDGET_ID_COMMUNITY, "visible", true, "friendPage.popupGroup" )
	SetInt( WIDGET_ID_COMMUNITY, "selectedIndex", -1, "friendPage.popupGroup.main" )
	
	local tbMousePos = GetMousePos()
	
	local nX = tbMousePos[ "x" ] - GetInt( WIDGET_ID_COMMUNITY, "x", "" )
	local nY = tbMousePos[ "y" ] - GetInt( WIDGET_ID_COMMUNITY, "y", "" )	
	nX = nX- GetInt( WIDGET_ID_COMMUNITY, "x", "friendPage" )
	nY = nY- GetInt( WIDGET_ID_COMMUNITY, "y", "friendPage" )

	SetInt( WIDGET_ID_COMMUNITY, "x", nX, "friendPage.popupGroup" )
	SetInt( WIDGET_ID_COMMUNITY, "y", nY, "friendPage.popupGroup" )
	
	g_nGroupPopupSelectIndex = nIndex
	
end

-- 그룹 이름변경
function Community_FriendTab_GroupNameEdit()

	local tbHead = GetHeadData( WIDGET_ID_COMMUNITY, g_nGroupPopupSelectIndex, "friendPage.list" )
	if nil == tbHead or nil == tbHead[ "groupnum" ] then
		return
	end
	
	local strName = GetMsgBoxInput()
	if strName ~= GetGameWordText( "RNFRIEND_FRIENDTAB_SLOT_NORMAL", 0 ) then
	
		g_Msg:ReqFriendGroupRename( tbHead[ "groupnum" ], strName )
	
	end
	
	Event_MsgBox_Close()
	
end

-- 그룹삭제
function Community_FriendTab_DeleteGroup()

	local tbHead = GetHeadData( WIDGET_ID_COMMUNITY, g_nGroupPopupSelectIndex, "friendPage.list" )
	if nil == tbHead or nil == tbHead[ "groupnum" ] then
		return
	end
	
	g_Msg:ReqFriendGroupDelete( tbHead[ "groupnum" ] )
	
end

----------------------------------------------------------------------------------
-- 이벤트

-- 탭 변경
function Ev_Community_Tab( nIndex )

	if 0 == nIndex then
	
		SetBool( WIDGET_ID_COMMUNITY, "visible", true, "friendPage" )
		SetBool( WIDGET_ID_COMMUNITY, "visible", false, "blockPage" )
	
	elseif 1 == nIndex then
	
		SetBool( WIDGET_ID_COMMUNITY, "visible", false, "friendPage" )
		SetBool( WIDGET_ID_COMMUNITY, "visible", true, "blockPage" )
	
	end

end

-- 친구 추가
function Ev_Community_FriendTab_AddFriend()

	local strInput = GetString( WIDGET_ID_COMMUNITY, "text", "friendPage.friendInput" )
	if GetServiceProvider() == SP_GS then
		strInput = UTF8ToAnsi(strInput)
	end
	
	if strInput == "" then
		OpenMsgBox( MSG_BOX_TYPE_OK, GetGameInternalText( "RNFRIEND_FRIEND_INPUTEMPTY" ), Event_MsgBox_Close )
		return
	end
	
	local strMsg = StringFormat( GetGameInternalText( "FRIEND_ADD" ), strInput )
	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, strMsg, Ev_Community_FriendTab_AddFriend_Ok, Event_MsgBox_Close )

end

-- 친구 추가 OK
function Ev_Community_FriendTab_AddFriend_Ok()

	local strInput = GetString( WIDGET_ID_COMMUNITY, "text", "friendPage.friendInput" )
	if GetServiceProvider() == SP_GS then
		strInput = UTF8ToAnsi(strInput)
	end
	
	g_Msg:ReqFriendAdd( strInput )
	Event_MsgBox_Close()
	
end

-- 그룹 생성
function Ev_Community_FriendTab_AddGroup()

	local strInput = GetString( WIDGET_ID_COMMUNITY, "text", "friendPage.friendInput" )
	if strInput == "" then
		OpenMsgBox( MSG_BOX_TYPE_OK, GetGameInternalText( "RNFRIEND_FRIEND_INPUTEMPTY" ), Event_MsgBox_Close )
		return
	end
	
	if haveSymbolCha( strInput ) then
		OpenMsgBox( MSG_BOX_TYPE_OK, GetGameInternalText( "RNFRIEND_FRIEND_GROUPEDIT_ERROR" ), Event_MsgBox_Close )
		return
	end
	
	if GetServiceProvider() == SP_GS then
		strInput = UTF8ToAnsi( strInput )
	end

	strInput = StringFormat( GetGameInternalText( "RNFRIEND_FRIEND_ADDGROUP" ), strInput )
	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, strInput, Ev_Community_FriendTab_AddGroup_Ok, Event_MsgBox_Close )

end

-- 그룹 생성-OK
function Ev_Community_FriendTab_AddGroup_Ok()

	local strInput = GetString( WIDGET_ID_COMMUNITY, "text", "friendPage.friendInput" )
			
	if GetServiceProvider() == SP_GS then
		strInput = UTF8ToAnsi( strInput )	
	end
		
	g_Msg:ReqFriendGroupAdd( strInput )
	Event_MsgBox_Close()
	
end

-- 전체/온라인
function Ev_Community_FrieldTab_OnOff( nIndex )

	if 0 == nIndex then
	
		g_bCommunity_ShowAll = true
		
	else
	
		g_bCommunity_ShowAll = false
	
	end
	
	Community_UpdateFriendList( false )

end

-- 친구 팝업 선택
function Ev_Community_FriendTab_FriendPopupClick( nButtonIndex, nIndex )

	SetBool( WIDGET_ID_COMMUNITY, "visible", false, "friendPage.popupFriend" )
	
	if 0 ~= nButtonIndex then
		return
	end
	
	if nil == g_Community_FriendTab_SelectedFriendName then
		return
	end
	
	if nIndex == 0 then
		Community_FriendTab_InviteGroupChat()
	elseif nIndex == 1 then
		Community_FriendTab_Whisper()
	elseif nIndex == 2 then
		Community_FriendTab_ShowFriendInfo()
	elseif nIndex == 3 then
		Community_FriendTab_InviteParty()
	elseif nIndex == 5 then
		Community_FriendTab_MoveToFriend()
	elseif nIndex == 6 then
		Community_FriendTab_BlockFriend()
	elseif nIndex == 7 then
		Community_FriendTab_DeleteFriend()
	end	

	SetBool( WIDGET_ID_COMMUNITY, "visible", false, "friendPage.popupFriend" )

end

-- 그룹 팝업 선택
function Ev_Community_FriendTab_GroupPopupClick( nButtonIndex, nIndex )

	SetBool( WIDGET_ID_COMMUNITY, "visible", false, "friendPage.popupGroup" )
	
	if 0 ~= nButtonIndex then
		return
	end
	
	if nIndex == 0 then
	
		OpenMsgBox( MSG_BOX_TYPE_INPUT, GetGameWordText("RNFRIEND_FRIENDTAB_POPUP", 3), 
			Community_FriendTab_GroupNameEdit, Event_MsgBox_Close )
			
	elseif nIndex == 1 then
	
		Community_FriendTab_DeleteGroup()
		
	end		
	
	SetBool( WIDGET_ID_COMMUNITY, "visible", false, "friendPage.popupGroup" )

end

-- 친구 팝업 롤오버
function Ev_Community_FriendTab_Popup_RollOver( nIndex )

	if nIndex == 4 then
	
		Community_UpdateGroupPopup()
		
		SetBool( WIDGET_ID_COMMUNITY, "visible", true, "friendPage.popupFriend.sub" )
		SetInt( WIDGET_ID_COMMUNITY, "selectedIndex", -1, "friendPage.popupFriend.sub.list" )
		
	else
	
		SetBool( WIDGET_ID_COMMUNITY, "visible", false, "friendPage.popupFriend.sub" )
		
	end	

end

-- 친구 서브 팝업 클릭
function Ev_Community_FriendTab_FriendSubPopupClick( nButtonIndex, nIndex )

	SetInt( WIDGET_ID_COMMUNITY, "selectedIndex", -1, "friendPage.popupFriend.sub.list" )
	
	if nButtonIndex ~= 0 then
		return
	end
	
	local strFriend = g_Community_FriendTab_SelectedFriendName
	if strFriend == nil then
		return
	end
	
	local nGroupNum = GetDataInt( WIDGET_ID_COMMUNITY, nIndex, "groupnum", "friendPage.popupFriend.sub.list" )
	g_Msg:ReqFriendMoveGroup( strFriend, nGroupNum )
	SetBool( WIDGET_ID_COMMUNITY, "visible", false, "friendPage.popupFriend" )

end

-- 친구 리스트 트리 펼침
function Ev_Community_FriendTab_ExpandList( nIndex )

	local tbGroupList = g_Friend:GetGroupList()
	
	local tbGroupInfo = tbGroupList[ nIndex + 1 ]
	if nil == tbGroupInfo then
		return
	end
	
	Community_UpdateState( tbGroupInfo[ 1 ] )

end

-- 리스트 아이템 클릭
function Ev_Community_FriendTab_ItemClick( nButtonIndex, strKey, nIndex, nIndexSub )

	if 1 ~= nButtonIndex then
	
		SetBool( WIDGET_ID_COMMUNITY, "visible", false, "friendPage.popupGroup" )
		SetBool( WIDGET_ID_COMMUNITY, "visible", false, "friendPage.popupFriend" )
		return
		
	end
	
	-- 그룹
	if -1 == nIndexSub then
	
		SetBool( WIDGET_ID_COMMUNITY, "visible", false, "friendPage.popupFriend" )
		Community_FriendTab_OpenGroupPopup( nIndex )
	
	-- 친구
	else
	
		local tbData = GetTreeData( WIDGET_ID_COMMUNITY, strKey, nIndexSub, "friendPage.list" )
		if nil == tbData or nil == tbData[ "label2" ] then
			return
		end
	
		g_Community_FriendTab_SelectedFriendName = TrimHTMLTag( tbData[ "label2" ] )
	
		SetBool( WIDGET_ID_COMMUNITY, "visible", false, "friendPage.popupGroup" )
		Community_FriendTab_OpenFriendPopup()
	
	end

end

-- 차단 추가
function Ev_Community_BlockTab_Block()

	local strInput = GetString( WIDGET_ID_COMMUNITY, "text", "blockPage.friendInput" )
	
	-- 입력란에 입력된 텍스트가 없을 경우 메시지 박스 출력하지 않음
	if strInput == "" then
	
		OpenMsgBox( MSG_BOX_TYPE_OK, GetGameInternalText( "RNFRIEND_FRIEND_INPUTEMPTY" ), Event_MsgBox_Close )
		return
		
	end
	
	if strInput ~= nil then
	
		local strMsg = StringFormat( GetGameInternalText( "FRIEND_BLOCK_ADD" ), strInput )
		OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, strMsg, Ev_Community_BlockTab_Block_Ok, Event_MsgBox_Close )
		
	end

end

-- 차단 추가 확인
function Ev_Community_BlockTab_Block_Ok()

	local strInput = GetString( WIDGET_ID_COMMUNITY, "text", "blockPage.friendInput" )
	if strInput ~= g_MyChar:GetName() then
	
		g_Msg:ReqFriendBlock( strInput, true )
		
	end
	
	SetString( WIDGET_ID_COMMUNITY, "text", "", "blockPage.friendInput" )
	Event_MsgBox_Close()

end

-- 차단 해제
function Ev_Community_BlockTab_BlockRelease()

	local tbSelect = GetArray( WIDGET_ID_COMMUNITY, "arrSelectedIndex", "blockPage.list" )
	if 0 == table.getn( tbSelect ) then
		return
	end
	
	local strBlock = GetDataString( WIDGET_ID_COMMUNITY, tbSelect[ 1 ], "label1", "blockPage.list" )
	-- 입력란에 입력된 텍스트가 없을 경우 메시지 박스 출력하지 않음
	if strBlock == "" then
	
		OpenMsgBox( MSG_BOX_TYPE_OK, GetGameInternalText( "RNFRIEND_FRIEND_INPUTEMPTY" ), Event_MsgBox_Close )
		return
		
	end
	
	if strBlock ~= nil then
	
		local strMsg = StringFormat( GetGameInternalText( "RNFRIEND_FRIEND_DELBLOCK" ), strBlock )
		OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, strMsg, Ev_Community_BlockTab_BlockRelease_Ok, Event_MsgBox_Close )
		
	end

end

-- 차단 해제 확인
function Ev_Community_BlockTab_BlockRelease_Ok()

	local tbSelect = GetArray( WIDGET_ID_COMMUNITY, "arrSelectedIndex", "blockPage.list" )
	if 0 == table.getn( tbSelect ) then
		return
	end
	
	local strBlock = GetDataString( WIDGET_ID_COMMUNITY, tbSelect[ 1 ], "label1", "blockPage.list" )
	if strBlock ~= nil then
	
		g_Msg:ReqFriendDel( strBlock )
		
	end
	
	SetString( WIDGET_ID_COMMUNITY, "text", "", "blockPage.friendInput" )
	Event_MsgBox_Close()

end