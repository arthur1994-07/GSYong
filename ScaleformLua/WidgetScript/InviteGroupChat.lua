
-- 리스트 아이템 개수
INVITEGROUPCHAT_LIST_COUNT = 8

g_tbInviteGroupChat_FriendList = {}			-- 친구 리스트
g_tbInviteGroupChat_ClubList = {}			-- 클럽 리스트

g_tbInviteGroupChat_InviteFriend = {}		-- 초대할 친구 리스트
g_tbInviteGroupChat_InviteClub = {}			-- 초대할 클럽 리스트

-- 이전 하이라이트 리스트 아이템 인덱스
g_nInviteGroupChat_HighlightIndex = 0

-- 이름 검색 성공 후 스크롤 이동시 스크롤 이벤트가 날라와 하이라이트가 꺼지니 무시하기
g_bInviteGroupChat_IgnoreScrollEv = false

-- 그룹채팅 초대 로드 완료
function LoadInviteGroupChat()

	-- 탭 설정
	local strParams = CreateParam( 
		GetGameWordText( "MAX_GROUP_CHAT_STATE_FRIEND", 0 ),
		GetGameWordText( "MAX_GROUP_CHAT_STATE_CLUBMEMBER", 0 ) )		
	SetData( WIDGET_ID_INVITEGROUPCHAT, strParams, "tabType" )
	SetEvent( WIDGET_ID_INVITEGROUPCHAT, EVENT_INDEX_CLICK_CHANGE, "Ev_InviteGroupChat_SelectTab", "index", "tabType" )
	
	-- 검색 설정
	SetEvent( WIDGET_ID_INVITEGROUPCHAT, EVENT_KEY_UP, "Ev_InviteGroupChat_KeyUp", "keyCode", "inputSearch" )
	SetEvent( WIDGET_ID_INVITEGROUPCHAT, EVENT_BUTTON_CLICK, "Ev_InviteGroupChat_Search", "", "btnSearch" )
	
	-- 리스트 설정
	SetEvent( WIDGET_ID_INVITEGROUPCHAT, EVENT_LIST_ITEM_CLICK, "Ev_InviteGroupChat_listFriendSelect", "index", "exListFriend.list" )
	SetEvent( WIDGET_ID_INVITEGROUPCHAT, EVENT_LIST_ITEM_CLICK, "Ev_InviteGroupChat_listClubSelect", "index", "exListClub.list" )
	SetEvent( WIDGET_ID_INVITEGROUPCHAT, EVENT_SCROLL, "Ev_InviteGroupChat_listFriendScroll", "", "exListFriend.list" )
	SetEvent( WIDGET_ID_INVITEGROUPCHAT, EVENT_SCROLL, "Ev_InviteGroupChat_listClubScroll", "", "exListClub.list" )

	-- 초대 버튼 설정
	SetString( WIDGET_ID_INVITEGROUPCHAT, "label", GetGameWordText( "MAX_GROUP_CHAT_INVITE", 0 ), "btnInvite" )
	SetEvent( WIDGET_ID_INVITEGROUPCHAT, EVENT_BUTTON_CLICK, "Ev_InviteGroupChat_Invite", "", "btnInvite" )
	
	-- 검색 하이라이트 끄기
	for i = 1, INVITEGROUPCHAT_LIST_COUNT, 1 do
	
		SetBool( WIDGET_ID_INVITEGROUPCHAT, "visible", false, "exListFriend.item" .. tostring( i ) .. ".highlight" )
		SetBool( WIDGET_ID_INVITEGROUPCHAT, "visible", false, "exListClub.item" .. tostring( i ) .. ".highlight" )
	
	end
	
end

-- 그룹채팅 초대 오픈
function OpenInviteGroupChat()

	-- 친구탭 선택
	SetInt( WIDGET_ID_INVITEGROUPCHAT, "selectedIndex", 0, "tabType" )
	Ev_InviteGroupChat_SelectTab( 0 )

	InviteGroupChat_SetFriendList()
	InviteGroupChat_SetClubList()

end

-- 친구 리스트 데이터 설정
function InviteGroupChat_SetFriendList()

	local tbFriendList = g_Friend:GetFriendList()
	
	local tbListData = {}
	for value in pairs( tbFriendList ) do
	
		local tbFriend = tbFriendList[ value ]
		if true == tbFriend[ 2 ] then
			
			local tbData = 
			{
				tbFriend[ 1 ],		-- 이름
				tbFriend[ 3 ],		-- ID
				0,					-- multiSelected
			}
			
			table.insert( tbListData, tbData )
			table.insert( g_tbInviteGroupChat_FriendList, tbData )
			
		end
	
	end
	
	SetDataEx( WIDGET_ID_INVITEGROUPCHAT, "label;charID;multiSelected;", tbListData, "exListFriend.list", false )

end

-- 클럽 리스트 데이터 설정
function InviteGroupChat_SetClubList()

	local strMyCharName = g_MyChar:GetName()
	local tbClubList = g_Club:GetClubMemberList()
	
	local tbListData = {}
	for value in pairs( tbClubList ) do
	
		local tbClub = tbClubList[ value ]
		if true == tbClub[ 2 ] then
		
			if strMyCharName ~= tbClub[ 1 ] then
			
				local tbData = 
				{
					tbClub[ 1 ],	-- 이름
					tbClub[ 3 ],	-- ID
					0,				-- multiSelected
				}
				
				table.insert( tbListData, tbData )
				table.insert( g_tbInviteGroupChat_ClubList, tbData )
			
			end
		
		end
	
	end
	
	SetDataEx( WIDGET_ID_INVITEGROUPCHAT, "label;charID;multiSelected;", tbListData, "exListClub.list", false )

end

-- 리스트 검색
function InviteGroupChat_SearchList( strName )

	local nTab = GetInt( WIDGET_ID_INVITEGROUPCHAT, "selectedIndex", "tabType" )
	
	local strInst = ""
	
	-- 검색할 리스트
	local tbTargetList = nil
	-- 친구탭
	if 0 == nTab then
	
		tbTargetList = g_tbInviteGroupChat_FriendList
		strInst = "exListFriend"
	
	-- 클럽탭
	elseif 1 == nTab then
	
		tbTargetList = g_tbInviteGroupChat_ClubList
		strInst = "exListClub"
	
	end
	
	--검색할 리스트가 없으면 종료
	if nil == tbTargetList then
		return
	end
	
	-- 인덱스 찾기
	local nFindIndex = 0
	for value in pairs( tbTargetList ) do
	
		local tbChar = tbTargetList[ value ]
		if tbChar[ 1 ] == strName then
		
			nFindIndex = value
			break
			
		end
	
	end
	
	-- 인덱스를 못찾았으면 에러메시지 출력하고 종료
	if 0 == nFindIndex then
	
		local str = StringFormat( GetGameInternalText( "GROUPCHAT_FAIL_SEARCHCHAR", 0 ), strName )
		CF_PrintMsg_SystemMsg( str, TEXT_COLOR_RED )
		return
		
	end
	
	nFindIndex = nFindIndex - 1
	
	-- 스크롤 위치 이동
	SetInt( WIDGET_ID_INVITEGROUPCHAT, "scrollPosition", nFindIndex, strInst .. ".list" )
	
	-- 하이라이트
	if nFindIndex > table.getn( tbTargetList ) - INVITEGROUPCHAT_LIST_COUNT then
	
		g_nInviteGroupChat_HighlightIndex = InviteGroupChat_ClacItemIndex( nFindIndex, table.getn( tbTargetList ) )
	
		SetBool( WIDGET_ID_INVITEGROUPCHAT, "visible", true, 
			strInst .. ".item" .. tostring( g_nInviteGroupChat_HighlightIndex ) .. ".highlight" )
	
	else
	
		g_nInviteGroupChat_HighlightIndex = 1
	
		SetBool( WIDGET_ID_INVITEGROUPCHAT, "visible", true, strInst .. ".item1.highlight" )
	
	end
	
	-- 스크롤 이동시 스크롤 이벤트가 날라와 하이라이트가 꺼지니 무시하기
	g_bInviteGroupChat_IgnoreScrollEv = true
	
	-- 인풋 텍스트 비우기
	SetString( WIDGET_ID_INVITEGROUPCHAT, "text", "", "inputSearch" )

end

-- 리스트 인덱스를 현재 보여지고 있는 리스트 아이템 인덱스로 변환
function InviteGroupChat_ClacItemIndex( nIndex, nTableCount )

	return INVITEGROUPCHAT_LIST_COUNT - ( nTableCount - nIndex ) + 1

end

-------------------------------------------------------------------------------------------------------------------------
-- 이벤트

-- 탭 변경
function Ev_InviteGroupChat_SelectTab( nIndex )

	if 0 == nIndex then
	
		SetBool( WIDGET_ID_INVITEGROUPCHAT, "visible", true, "exListFriend" )
		SetBool( WIDGET_ID_INVITEGROUPCHAT, "visible", false, "exListClub" )
	
	elseif 1 == nIndex then
	
		SetBool( WIDGET_ID_INVITEGROUPCHAT, "visible", false, "exListFriend" )
		SetBool( WIDGET_ID_INVITEGROUPCHAT, "visible", true, "exListClub" )
	
	end

end

-- 검색 인풋 입력
function Ev_InviteGroupChat_KeyUp( nKeyCode )

	if KEYCODE_ENTER == nKeyCode then
	
		Ev_InviteGroupChat_Search()
	
	end

end

-- 검색 버튼 클릭
function Ev_InviteGroupChat_Search()

	if 0 ~= g_nInviteGroupChat_HighlightIndex then
	
		SetBool( WIDGET_ID_INVITEGROUPCHAT, "visible", false, 
			"exListFriend.item" .. tostring( g_nInviteGroupChat_HighlightIndex ) .. ".highlight" )
		SetBool( WIDGET_ID_INVITEGROUPCHAT, "visible", false, 
			"exListClub.item" .. tostring( g_nInviteGroupChat_HighlightIndex ) .. ".highlight" )
	
	end

	local strName = GetString( WIDGET_ID_INVITEGROUPCHAT, "text", "inputSearch" )
	InviteGroupChat_SearchList( strName )

end

-- 친구 리스트 선택
function Ev_InviteGroupChat_listFriendSelect( nIndex )

	-- 캐릭터 ID
	local nSelectID = GetDataInt( WIDGET_ID_INVITEGROUPCHAT, nIndex, "charID", "exListFriend.list" )

	-- 초대 리스트 추가?
	local bInsert = true
	
	-- 초대 리스트에 있으면 제거
	for value in pairs( g_tbInviteGroupChat_InviteFriend ) do
	
		local nID = g_tbInviteGroupChat_InviteFriend[ value ][ 2 ]
		if nSelectID == nID then
		
			table.remove( g_tbInviteGroupChat_InviteFriend, value )
			bInsert = false
			
			break
			
		end
	
	end
	
	-- 초대 리스트 추가
	if true == bInsert then
		table.insert( g_tbInviteGroupChat_InviteFriend, g_tbInviteGroupChat_FriendList[ nIndex + 1 ] )
	end
	
	-- 하이라이트 끄기
	if 0 ~= g_nInviteGroupChat_HighlightIndex then
	
		local nHighlight = 0
		if nIndex > table.getn( g_tbInviteGroupChat_FriendList ) - INVITEGROUPCHAT_LIST_COUNT then
	
			nHighlight = InviteGroupChat_ClacItemIndex( nIndex, table.getn( g_tbInviteGroupChat_FriendList ) )
			
		else
		
			nHighlight = 1
		
		end
		
		if nHighlight == g_nInviteGroupChat_HighlightIndex then
		
			SetBool( WIDGET_ID_INVITEGROUPCHAT, "visible", false, 
				"exListFriend.item" .. tostring( g_nInviteGroupChat_HighlightIndex ) .. ".highlight" )
		
		end
	
	end

end

-- 클럽 리스트 선택
function Ev_InviteGroupChat_listClubSelect( nIndex )

	-- 캐릭터 ID
	local nSelectID = GetDataInt( WIDGET_ID_INVITEGROUPCHAT, nIndex, "charID", "exListClub.list" )

	-- 초대 리스트 추가?
	local bInsert = true
	
	-- 초대 리스트에 있으면 제거
	for value in pairs( g_tbInviteGroupChat_InviteClub ) do
	
		local nID = g_tbInviteGroupChat_InviteClub[ value ][ 2 ]
		if nSelectID == nID then
		
			table.remove( g_tbInviteGroupChat_InviteClub, value )
			bInsert = false
			
			break
			
		end
	
	end
	
	-- 초대 리스트 추가
	if true == bInsert then
		table.insert( g_tbInviteGroupChat_InviteClub, g_tbInviteGroupChat_ClubList[ nIndex + 1 ] )
	end
	
	-- 하이라이트 끄기
	if 0 ~= g_nInviteGroupChat_HighlightIndex then
	
		local nHighlight = 0
		if nIndex > table.getn( g_tbInviteGroupChat_ClubList ) - INVITEGROUPCHAT_LIST_COUNT then
	
			nHighlight = InviteGroupChat_ClacItemIndex( nIndex, table.getn( g_tbInviteGroupChat_ClubList ) )
			
		else
		
			nHighlight = 1
		
		end
		
		if nHighlight == g_nInviteGroupChat_HighlightIndex then
		
			SetBool( WIDGET_ID_INVITEGROUPCHAT, "visible", false, 
				"exListClub.item" .. tostring( g_nInviteGroupChat_HighlightIndex ) .. ".highlight" )
		
		end
	
	end

end

-- 친구 리스트 스크롤
function Ev_InviteGroupChat_listFriendScroll()

	-- 검색 후 강제 스크롤 됨. 하이라이트 끄지말고 종료
	if true == g_bInviteGroupChat_IgnoreScrollEv then
	
		g_bInviteGroupChat_IgnoreScrollEv = false
		return
		
	end
	
	-- 하이라이트 끄기
	if 0 ~= g_nInviteGroupChat_HighlightIndex then
	
		SetBool( WIDGET_ID_INVITEGROUPCHAT, "visible", false, 
			"exListFriend.item" .. tostring( g_nInviteGroupChat_HighlightIndex ) .. ".highlight" )
	
	end
	
	g_nInviteGroupChat_HighlightIndex = 0

end

-- 클럽 리스트 스크롤
function Ev_InviteGroupChat_listClubScroll()

	-- 검색 후 강제 스크롤 됨. 하이라이트 끄지말고 종료
	if true == g_bInviteGroupChat_IgnoreScrollEv then
		
		g_bInviteGroupChat_IgnoreScrollEv = false
		return
		
	end

	-- 하이라이트 끄기
	if 0 ~= g_nInviteGroupChat_HighlightIndex then
	
		SetBool( WIDGET_ID_INVITEGROUPCHAT, "visible", false, 
			"exListClub.item" .. tostring( g_nInviteGroupChat_HighlightIndex ) .. ".highlight" )
	
	end
	
	g_nInviteGroupChat_HighlightIndex = 0

end

-- 초대하기
function Ev_InviteGroupChat_Invite()

	-- 친구 초대
	for value in pairs( g_tbInviteGroupChat_InviteFriend ) do
	
		local tbFriend = g_tbInviteGroupChat_InviteFriend[ value ]
		if false == g_Msg:ReqGroupChat( tbFriend[ 1 ], GROUPCHAT_INVITE_FRIEND ) then
		
			--!! 초대 실패 메시지 출력
			--CF_PrintMsg_SystemMsg( GetGameInternalText( "PTOPMENUREJECT_MESSAGE5", 0 ), TEXT_COLOR_RED )
		
		end
	
	end

	-- 클럽원 초대
	for value in pairs( g_tbInviteGroupChat_InviteClub ) do
	
		local tbClub = g_tbInviteGroupChat_InviteClub[ value ]
		if false == g_Msg:ReqGroupChat( tbClub[ 1 ], GROUPCHAT_INVITE_CLUBMEMBER ) then
		
			--!! 초대 실패 메시지 출력
			--CF_PrintMsg_SystemMsg( GetGameInternalText( "PTOPMENUREJECT_MESSAGE5", 0 ), TEXT_COLOR_RED )
		
		end
	
	end
	
	CloseWidget( WIDGET_ID_INVITEGROUPCHAT )

end