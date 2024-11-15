g_nPostBoxSubX = 0 
g_nPostBoxSubY = 0
g_PostBoxSub_StageX = 0
g_PostBoxSub_StageY = 0

g_PostBoxSub_strStaicReceiveMoney = GetGameWordText( "POSTBOX_PAGE_READ_TEXTBOX", 3 )
g_PostBoxSub_strStaicBillingMoney = GetGameWordText( "POSTBOX_PAGE_READ_TEXTBOX", 4 )

POSTBOXSUB_FRIEND_COUNT_TOTAL = 14

g_tbFriendList = {}
g_tbClubMemberList = {}
g_PostBoxSub_FriendPage_nSelectTab = 1

g_PostBoxSub_ReadPage_Index = 0

function LoadPostBoxSub()

	PostBoxSub_PosInit()
	
	if g_PostBoxSub_ReadPage_Index == 1 then
	
		PostBoxSub_ReadPageOpen()
		
	elseif g_PostBoxSub_ReadPage_Index == 2 then
	
		PostBoxSub_PreviewPageOpen()
		
	elseif g_PostBoxSub_ReadPage_Index == 3 then
	
		PostBoxSub_FindPageOpen()
		
	end	
	
end

function PostBoxSub_PosInit()
	SetString( WIDGET_ID_POSTBOX_SUB, "title", GetGameWordText( "TEMPINVEN_TYPE", 1 ), "" )
	SetBool( WIDGET_ID_POSTBOX_SUB, "customDrag", true, "" )
	
	local nPostBoxX = 100
	local nPostBoxY = 100
	local nPostBoxW = 0
	if true == IsOpenWidget( WIDGET_ID_POSTBOX_SUB ) then
		nPostBoxW = GetInt( WIDGET_ID_POSTBOX, "width", "" )
		nPostBoxX = GetInt( WIDGET_ID_POSTBOX, "x", "" )
		nPostBoxY = GetInt( WIDGET_ID_POSTBOX, "y", "" )
	end	
	
	local nNewX = nPostBoxX + nPostBoxW
	
	SetInt( WIDGET_ID_POSTBOX_SUB, "x", nNewX, "" )
	SetInt( WIDGET_ID_POSTBOX_SUB, "y", nPostBoxY, "" )
	
	g_nPostBoxSubX = nNewX
	g_nPostBoxSubY = nPostBoxY
	
	SetEvent( WIDGET_ID_POSTBOX_SUB, EVENT_MOUSE_DOWN, "Ev_PostBoxSub_Drag_Start", "stageX;stageY", "titleBtn" )
	
	SetEvent( WIDGET_ID_POSTBOX_SUB, EVENT_WIDGET_CLOSE, "Ev_PostBoxSub_Close", "", "" )
	
end

function PostBoxSub_Reset_FindPage()
	g_tbFriendList = {}
	g_tbClubMemberList = {}
	g_PostBoxSub_FriendPage_nSelectTab = 1
	RemoveData( WIDGET_ID_POSTBOX_SUB, 1, "pageFriend.friendList.list", true )
end

-- 편지읽기 페이지 열기
function PostBoxSub_ReadPageOpen( )
	
	SetBool( WIDGET_ID_POSTBOX_SUB, "visible", true, "" )

	SetBool( WIDGET_ID_POSTBOX_SUB, "visible", true, "pageRead" )
	SetBool( WIDGET_ID_POSTBOX_SUB, "visible", false, "pageFriend" )
	SetBool( WIDGET_ID_POSTBOX_SUB, "visible", false, "pagePreview" )
	
	CallFunction( WIDGET_ID_POSTBOX_SUB, "gotoAndStop", "design"..g_PostBox_tbReceivePageSelectedPost["design"], "pageRead.design" )
	
	SetString( WIDGET_ID_POSTBOX_SUB, "binding", SLOT_POSTBOX_READPAGE_ID, "pageRead.slot" )
	
	SetString( WIDGET_ID_POSTBOX_SUB, "text", GetGameWordText( "POSTBOX_PAGE_READ_TEXTBOX", 0 ), "pageRead.staticDate" )
	SetString( WIDGET_ID_POSTBOX_SUB, "text", GetGameWordText( "POSTBOX_PAGE_READ_TEXTBOX", 1 ), "pageRead.staticFrom" )
	SetString( WIDGET_ID_POSTBOX_SUB, "text", GetGameWordText( "POSTBOX_PAGE_READ_TEXTBOX", 2 ), "pageRead.staticReceiveItem" )
	SetString( WIDGET_ID_POSTBOX_SUB, "text", GetGameWordText( "POSTBOX_PAGE_READ_TEXTBOX", 5 ), "pageRead.staticMyMoney" )
	SetString( WIDGET_ID_POSTBOX_SUB, "text", GetGameWordText( "POSTBOX_PAGE_READ_TEXTBOX", 6 ), "pageRead.staticReceiveNonItem" )
	SetString( WIDGET_ID_POSTBOX_SUB, "text", GetGameWordText( "POSTBOX_PAGE_READ_TEXTBOX", 7 ), "pageRead.staticAttachItem" )
	
	SetString( WIDGET_ID_POSTBOX_SUB, "label", GetGameWordText( "POSTBOX_PAGE_READ_BUTTON", 2 ), "pageRead.deleteButton" )
	SetString( WIDGET_ID_POSTBOX_SUB, "label", GetGameWordText( "POSTBOX_PAGE_READ_BUTTON", 3 ), "pageRead.replyButton" )
	SetString( WIDGET_ID_POSTBOX_SUB, "label", GetGameWordText( "POSTBOX_BUTTON", 3 ), "pageRead.closeButton" ) 
	
	local strFromText = HTMLFontColor( g_PostBox_tbReceivePageSelectedPost["from"], HTML_TEXT_COLOR_SKYBLUE )
	local strLeftTimeText = HTMLFontColor( g_PostBox_tbReceivePageSelectedPost["leftDay"], HTML_TEXT_COLOR_YELLOW )
	
	SetString( WIDGET_ID_POSTBOX_SUB, "text", g_PostBox_tbReceivePageSelectedPost["date"], "pageRead.date" )
	SetString( WIDGET_ID_POSTBOX_SUB, "htmlText", strFromText, "pageRead.from" )
	SetString( WIDGET_ID_POSTBOX_SUB, "htmlText", strLeftTimeText, "pageRead.leftTime" )
	SetString( WIDGET_ID_POSTBOX_SUB, "text", g_PostBox_tbReceivePageSelectedPost["subject"], "pageRead.subject" )
	SetString( WIDGET_ID_POSTBOX_SUB, "text", g_PostBox_tbReceivePageSelectedPost["contents"], "pageRead.contents" )
	
	--SetString( WIDGET_ID_POSTBOX_SUB, "text", g_PostBox_tbReceivePageSelectedPost["staticMoney"], "pageRead.staticMoney" )
	SetString( WIDGET_ID_POSTBOX_SUB, "text", CurrencyFormat( g_MyChar:GetInvenMoney() ), "pageRead.myMoney" )
	
	local strMoney = "0"
	if g_PostBox_tbReceivePageSelectedPost["money"] ~= nil then
		strMoney = CurrencyFormat( g_PostBox_tbReceivePageSelectedPost["money"] )
	end
	
	-- 대금 청구 일때
	if g_PostBox_tbReceivePageSelectedPost["billing"] then
	
		strMoney = CurrencyFormat( g_PostBox_tbReceivePageSelectedPost["money"] * -1 )
		strMoney = HTMLFontColor( strMoney, HTML_TEXT_COLOR_RED )
		
		SetString( WIDGET_ID_POSTBOX_SUB, "label", GetGameWordText( "POSTBOX_PAGE_READ_BUTTON", 4 ), "pageRead.takeButton" )
		SetString( WIDGET_ID_POSTBOX_SUB, "label", GetGameWordText( "POSTBOX_PAGE_READ_BUTTON", 5 ), "pageRead.returnButton" )
		SetString( WIDGET_ID_POSTBOX_SUB, "text", GetGameWordText( "POSTBOX_PAGE_READ_TEXTBOX", 4 ), "pageRead.staticMoney" )
		
	-- 대금 청구가 아닐때
	else
	
		if strMoney == "0" then
			CallFunction( WIDGET_ID_POSTBOX_SUB, "gotoAndStop", "typeEnable", "pageRead.moneyType" )			
		else
			CallFunction( WIDGET_ID_POSTBOX_SUB, "gotoAndStop", "typeDefault", "pageRead.moneyType" )			
		end	
		
		strMoney = CurrencyFormat( g_PostBox_tbReceivePageSelectedPost["money"] )
		strMoney = HTMLFontColor( strMoney, HTML_TEXT_COLOR_WHITE )
		
		SetString( WIDGET_ID_POSTBOX_SUB, "label", GetGameWordText( "POSTBOX_PAGE_READ_BUTTON", 0 ), "pageRead.takeButton" )
		SetString( WIDGET_ID_POSTBOX_SUB, "label", GetGameWordText( "POSTBOX_PAGE_READ_BUTTON", 1 ), "pageRead.returnButton" )
		SetString( WIDGET_ID_POSTBOX_SUB, "text", GetGameWordText( "POSTBOX_PAGE_READ_TEXTBOX", 3 ), "pageRead.staticMoney" )
		
	end
	
	SetString( WIDGET_ID_POSTBOX_SUB, "htmlText", strMoney, "pageRead.money" )
	
	if g_PostBox_tbReceivePageSelectedPost["receive"] then
		SetBool( WIDGET_ID_POSTBOX_SUB, "visible", true, "pageRead.takeButton" )
	else
		SetBool( WIDGET_ID_POSTBOX_SUB, "visible", false, "pageRead.takeButton" )
	end
	
	if g_PostBox_tbReceivePageSelectedPost["returnAble"] then
		SetBool( WIDGET_ID_POSTBOX_SUB, "visible", true, "pageRead.returnButton" )
	else
		SetBool( WIDGET_ID_POSTBOX_SUB, "visible", false, "pageRead.returnButton" )
	end
	
	if g_PostBox_tbReceivePageSelectedPost["deleteAble"] then
		SetBool( WIDGET_ID_POSTBOX_SUB, "visible", true, "pageRead.deleteButton" )
	else
		SetBool( WIDGET_ID_POSTBOX_SUB, "visible", false, "pageRead.deleteButton" )
	end
	
	--SetSlot_Item_DWORD( SLOT_POSTBOX_READPAGE_ID, g_PostBox_tbReceivePageSelectedPost["itemId"], g_PostBox_tbReceivePageSelectedPost["turnNum"] )
	SetSlot_ReceivePost( SLOT_POSTBOX_READPAGE_ID , g_PostBox_tbReceivePageSelectedPost["Index"], g_PostBox_tbReceivePageSelectedPost["turnNum"] )
	
	SetEvent( WIDGET_ID_POSTBOX_SUB, EVENT_BUTTON_CLICK, "Ev_PostBoxSub_ReadPage_DeleteButton", "", "pageRead.deleteButton" )
	SetEvent( WIDGET_ID_POSTBOX_SUB, EVENT_BUTTON_CLICK, "Ev_PostBoxSub_ReadPage_TakeButton", "", "pageRead.takeButton" )
	SetEvent( WIDGET_ID_POSTBOX_SUB, EVENT_BUTTON_CLICK, "Ev_PostBoxSub_ReadPage_ReturnButton", "", "pageRead.returnButton" )
	SetEvent( WIDGET_ID_POSTBOX_SUB, EVENT_BUTTON_CLICK, "Ev_PostBoxSub_ReadPage_ReplyButton", "", "pageRead.replyButton" )
	SetEvent( WIDGET_ID_POSTBOX_SUB, EVENT_BUTTON_CLICK, "Ev_PostBoxSub_FindPage_CloseButton", "", "pageRead.closeButton" )	
	
	g_PostBoxSub_ReadPage_Index = 0

end

-- 받는사람 찾기 페이지 열기
function PostBoxSub_FindPageOpen()

	SetBool( WIDGET_ID_POSTBOX_SUB, "visible", true, "" )
	
	SetBool( WIDGET_ID_POSTBOX_SUB, "visible", false, "pageRead" )
	SetBool( WIDGET_ID_POSTBOX_SUB, "visible", true, "pageFriend" )
	SetBool( WIDGET_ID_POSTBOX_SUB, "visible", false, "pagePreview" )
	
	local strParams = CreateParam( 
		GetGameWordText( "POSTBOX_PAGE_FRIENDFIND_BUTTON", 0 ),
		GetGameWordText( "POSTBOX_PAGE_FRIENDFIND_BUTTON", 1 ) )		
	SetData( WIDGET_ID_POSTBOX_SUB, strParams, "pageFriend.tabType" )
	SetInt( WIDGET_ID_POSTBOX_SUB, "selectedIndex", 0, "pageFriend.tabType" )
	SetEvent( WIDGET_ID_POSTBOX_SUB, EVENT_INDEX_CLICK_CHANGE, "Ev_PostBoxSub_pageFriendTabChange", "index", "pageFriend.tabType" )
	
	SetString( WIDGET_ID_POSTBOX_SUB, "label", GetGameWordText( "POSTBOX_PAGE_FRIENDFIND_BUTTON", 2 ), "pageFriend.okButton" )
	SetString( WIDGET_ID_POSTBOX_SUB, "label", GetGameWordText( "POSTBOX_BUTTON", 3 ), "pageFriend.closeButton" )	
	
	SetEvent( WIDGET_ID_POSTBOX_SUB, EVENT_BUTTON_CLICK, "Ev_PostBoxSub_FindPage_AttachButton", "", "pageFriend.okButton" )
	SetEvent( WIDGET_ID_POSTBOX_SUB, EVENT_BUTTON_CLICK, "Ev_PostBoxSub_FindPage_CloseButton", "", "pageFriend.closeButton" )	
	SetEvent( WIDGET_ID_POSTBOX_SUB, EVENT_SCROLL, "Ev_PostBoxSub_FindPage_UpdateClubList", "", "pageFriend.friendList.list" )
	
	PostBoxSub_FindPage_FrirndTap()

	g_PostBoxSub_ReadPage_Index = 0
end

-- 미리보기 페이지 열기
function PostBoxSub_PreviewPageOpen( )

	SetBool( WIDGET_ID_POSTBOX_SUB, "visible", true, "" )

	local nSelectDesign = GetInt( WIDGET_ID_POSTBOX, "selectedIndex", "pageSend.sendDesign" )
	CallFunction( WIDGET_ID_POSTBOX_SUB, "gotoAndStop", "design"..nSelectDesign, "pagePreview.design" )

	SetString( WIDGET_ID_POSTBOX_SUB, "binding", SLOT_POSTBOX_PREVIEWPAGE_ID, "pagePreview.slot" )

	local tbPreivew = g_Post:GetPreviewPagePost()	

	--SetSlot_Item_DWORD( SLOT_POSTBOX_PREVIEWPAGE_ID, tbPreivew[3], tbPreivew[4] )
	SetSlot_PostBoxSub_PreviewPageOpen( SLOT_POSTBOX_PREVIEWPAGE_ID )
	
	local strFromText = HTMLFontColor( tbPreivew[2], HTML_TEXT_COLOR_SKYBLUE )
	
	SetString( WIDGET_ID_POSTBOX_SUB, "text", tbPreivew[1], "pagePreview.date" )
	SetString( WIDGET_ID_POSTBOX_SUB, "htmlText", strFromText, "pagePreview.from" )
	
	SetBool( WIDGET_ID_POSTBOX_SUB, "visible", false, "pageRead" )
	SetBool( WIDGET_ID_POSTBOX_SUB, "visible", false, "pageFriend" )
	SetBool( WIDGET_ID_POSTBOX_SUB, "visible", true, "pagePreview" )
	
	SetString( WIDGET_ID_POSTBOX_SUB, "text", GetGameWordText( "POSTBOX_PAGE_READ_TEXTBOX", 0 ), "pagePreview.staticDate" )
	SetString( WIDGET_ID_POSTBOX_SUB, "text", GetGameWordText( "POSTBOX_PAGE_READ_TEXTBOX", 1 ), "pagePreview.staticFrom" )
	SetString( WIDGET_ID_POSTBOX_SUB, "text", GetGameWordText( "POSTBOX_PAGE_READ_TEXTBOX", 2 ), "pagePreview.staticReceiveItem" )
	SetString( WIDGET_ID_POSTBOX_SUB, "text", "", "pagePreview.staticMoney" )
	SetString( WIDGET_ID_POSTBOX_SUB, "text", GetGameWordText( "POSTBOX_PAGE_PREVIEW_TEXTBOX", 0 ), "pagePreview.staticMyMoney" )
	SetString( WIDGET_ID_POSTBOX_SUB, "text", GetGameWordText( "POSTBOX_PAGE_READ_TEXTBOX", 6 ), "pagePreview.staticReceiveNonItem" )
	SetString( WIDGET_ID_POSTBOX_SUB, "text", GetGameWordText( "POSTBOX_PAGE_READ_TEXTBOX", 7 ), "pagePreview.staticAttachItem" )
	SetString( WIDGET_ID_POSTBOX_SUB, "label", GetGameWordText( "POSTBOX_BUTTON", 3 ), "pagePreview.closeButton" )

	local strMyMoney = ""
	local strMoney = ""
	--local nMyMoney = g_MyChar:GetInvenMoney() - PostBox_SendPage_GetCommission()
	local nMyMoney = Calculate_Minus( g_MyChar:GetInvenMoney(), PostBox_SendPage_GetCommission() )
	local nMoney = 0	
		
	if GetString( WIDGET_ID_POSTBOX, "text", "pageSend.sendBilling" ) == "" then
		SetString( WIDGET_ID_POSTBOX_SUB, "text", g_PostBoxSub_strStaicReceiveMoney, "pagePreview.staticMoney" )
		strMoney = GetString( WIDGET_ID_POSTBOX, "text", "pageSend.sendMoney" )
		if strMoney == "" then
			strMoney = "0"
		end		
		nMyMoney = Calculate_Minus( nMyMoney, tonumber( strMoney ) )
		strMoney = CurrencyFormat( tonumber( strMoney ) )
	else
		SetString( WIDGET_ID_POSTBOX_SUB, "text", g_PostBoxSub_strStaicBillingMoney, "pagePreview.staticMoney" )
		strMoney = GetString( WIDGET_ID_POSTBOX, "text", "pageSend.sendBilling" )
		if strMoney == "" then
			strMoney = "0"
		else
			--nMyMoney = nMyMoney - tonumber( strMoney )
			strMoney = CurrencyFormat( tonumber( strMoney ) )
		end		
	end	
	
	if strMoney == "" then
		strMoney = "0"
	end	
	
	strMyMoney = CurrencyFormat( tonumber( nMyMoney ) )	
	--strMoney = CurrencyFormat( tonumber( -strMoney ) )

	SetString( WIDGET_ID_POSTBOX_SUB, "text", strMoney, "pagePreview.money" )
	SetString( WIDGET_ID_POSTBOX_SUB, "text", strMyMoney, "pagePreview.myMoney" )
	SetString( WIDGET_ID_POSTBOX_SUB, "text", GetString( WIDGET_ID_POSTBOX, "text", "pageSend.sendSubject" ), "pagePreview.subject" )
	SetString( WIDGET_ID_POSTBOX_SUB, "text", GetString( WIDGET_ID_POSTBOX, "text", "pageSend.sendContents" ), "pagePreview.contents" )
	
	SetEvent( WIDGET_ID_POSTBOX_SUB, EVENT_BUTTON_CLICK, "Ev_PostBoxSub_FindPage_CloseButton", "", "pagePreview.closeButton" )

	g_PostBoxSub_ReadPage_Index = 0
end

-- 편지읽기 페이지 재설정(서브윈도우 열린상태)
function PostBoxSub_ReadPageReload()
	
	SetBool( WIDGET_ID_POSTBOX_SUB, "visible", true, "pageRead" )
	SetBool( WIDGET_ID_POSTBOX_SUB, "visible", false, "pageFriend" )
	
	if nil ~= g_PostBox_tbReceivePageSelectedPost["design"] then
		CallFunction( WIDGET_ID_POSTBOX_SUB, "gotoAndStop", "design"..g_PostBox_tbReceivePageSelectedPost["design"], "pageRead.design" )
	end
	
	local strFromText = HTMLFontColor( g_PostBox_tbReceivePageSelectedPost["from"], HTML_TEXT_COLOR_SKYBLUE )
	local strLeftTimeText = HTMLFontColor( g_PostBox_tbReceivePageSelectedPost["leftDay"], HTML_TEXT_COLOR_YELLOW )

	SetString( WIDGET_ID_POSTBOX_SUB, "text", g_PostBox_tbReceivePageSelectedPost["date"], "pageRead.date" )
	SetString( WIDGET_ID_POSTBOX_SUB, "htmlText", strFromText, "pageRead.from" )
	SetString( WIDGET_ID_POSTBOX_SUB, "htmlText", strLeftTimeText, "pageRead.leftTime" )
	SetString( WIDGET_ID_POSTBOX_SUB, "text", g_PostBox_tbReceivePageSelectedPost["subject"], "pageRead.subject" )
	SetString( WIDGET_ID_POSTBOX_SUB, "text", g_PostBox_tbReceivePageSelectedPost["contents"], "pageRead.contents" )
	
	SetString( WIDGET_ID_POSTBOX_SUB, "text", CurrencyFormat( g_MyChar:GetInvenMoney() ), "pageRead.myMoney" )
	
	local strMoney = "0"
	if g_PostBox_tbReceivePageSelectedPost["money"] ~= nil then
		strMoney = CurrencyFormat( g_PostBox_tbReceivePageSelectedPost["money"] )
	end
	if g_PostBox_tbReceivePageSelectedPost["billing"] then
		strMoney = HTMLFontColor( strMoney, HTML_TEXT_COLOR_RED )
		SetString( WIDGET_ID_POSTBOX_SUB, "label", GetGameWordText( "POSTBOX_PAGE_READ_BUTTON", 4 ), "pageRead.takeButton" )
		SetString( WIDGET_ID_POSTBOX_SUB, "label", GetGameWordText( "POSTBOX_PAGE_READ_BUTTON", 5 ), "pageRead.returnButton" )
		SetString( WIDGET_ID_POSTBOX_SUB, "text", GetGameWordText( "POSTBOX_PAGE_READ_TEXTBOX", 4 ), "pageRead.staticMoney" )
		CallFunction( WIDGET_ID_POSTBOX_SUB, "gotoAndStop", "typeBilling", "pageRead.moneyType" )
	else
		if strMoney == "0" then
			CallFunction( WIDGET_ID_POSTBOX_SUB, "gotoAndStop", "typeEnable", "pageRead.moneyType" )			
		else
			CallFunction( WIDGET_ID_POSTBOX_SUB, "gotoAndStop", "typeDefault", "pageRead.moneyType" )			
		end	
		strMoney = HTMLFontColor( strMoney, HTML_TEXT_COLOR_WHITE )
		SetString( WIDGET_ID_POSTBOX_SUB, "label", GetGameWordText( "POSTBOX_PAGE_READ_BUTTON", 0 ), "pageRead.takeButton" )
		SetString( WIDGET_ID_POSTBOX_SUB, "label", GetGameWordText( "POSTBOX_PAGE_READ_BUTTON", 1 ), "pageRead.returnButton" )
		SetString( WIDGET_ID_POSTBOX_SUB, "text", GetGameWordText( "POSTBOX_PAGE_READ_TEXTBOX", 3 ), "pageRead.staticMoney" )
	end
	SetString( WIDGET_ID_POSTBOX_SUB, "htmlText", strMoney, "pageRead.money" )
	
	if g_PostBox_tbReceivePageSelectedPost["receive"] then
		SetBool( WIDGET_ID_POSTBOX_SUB, "visible", true, "pageRead.takeButton" )
	else
		SetBool( WIDGET_ID_POSTBOX_SUB, "visible", false, "pageRead.takeButton" )
	end
	
	if g_PostBox_tbReceivePageSelectedPost["returnAble"] then
		SetBool( WIDGET_ID_POSTBOX_SUB, "visible", true, "pageRead.returnButton" )
	else
		SetBool( WIDGET_ID_POSTBOX_SUB, "visible", false, "pageRead.returnButton" )
	end
	
	if g_PostBox_tbReceivePageSelectedPost["deleteAble"] then
		SetBool( WIDGET_ID_POSTBOX_SUB, "visible", true, "pageRead.deleteButton" )
	else
		SetBool( WIDGET_ID_POSTBOX_SUB, "visible", false, "pageRead.deleteButton" )
	end
	
	--SetSlot_Item_DWORD( SLOT_POSTBOX_READPAGE_ID, g_PostBox_tbReceivePageSelectedPost["itemId"], g_PostBox_tbReceivePageSelectedPost["turnNum"] )
	SetSlot_ReceivePost( SLOT_POSTBOX_READPAGE_ID, g_PostBox_tbReceivePageSelectedPost["Index"], g_PostBox_tbReceivePageSelectedPost["turnNum"] )
	
end

-- 친구 탭 선택
function PostBoxSub_FindPage_FrirndTap()

	g_tbFriendList = {}
	g_tbFriendList = g_Friend:GetFriendList()
	local nFriendList = table.getn( g_tbFriendList )
	
	if nFriendList == 0 then
		RemoveData( WIDGET_ID_POSTBOX_SUB, 1, "pageFriend.friendList.list", true )
		return
	end
	
	local strFriend = ""
	for index = 1, nFriendList, 1 do	
		local tbFriend = g_tbFriendList[index]
		strFriend = strFriend .. tbFriend[1] .. ";"
	end
	SetData(WIDGET_ID_POSTBOX_SUB, strFriend, "pageFriend.friendList.list", false)
	
	g_PostBoxSub_FriendPage_nSelectTab = 1
	
	Ev_PostBoxSub_FindPage_UpdateFriendList()

end

function Ev_PostBoxSub_FindPage_UpdateFriendList()
	
	for i = 1, POSTBOXSUB_FRIEND_COUNT_TOTAL, 1 do
		local strItemInst = "pageFriend.friendList.item" .. tostring( i )
		local nIndex = i
		
		if nIndex <= table.getn( g_tbFriendList ) then
			local tbItem = CopyTable( g_tbFriendList[ nIndex ] )
			if tbItem[2] == false  then
				SetTextFormat( WIDGET_ID_POSTBOX_SUB, 0x888888, false, 0, 0, strItemInst )
			elseif tbItem[2] == true then
				SetTextFormat( WIDGET_ID_POSTBOX_SUB, 0xFFFFFF, false, 0, 0, strItemInst )
			end
		end
	end
end

-- 클럽 탭 선택
function PostBoxSub_FindPage_ClubTap()

	g_tbClubMemberList = {}
	g_tbClubMemberList = g_Club:GetClubMemberList()
	local nClubMember = table.getn( g_tbClubMemberList )
	
	if nClubMember == 0 then
		RemoveData( WIDGET_ID_POSTBOX_SUB, 1, "pageFriend.friendList.list", true )
		return
	end
	
	local strMember = ""
	for index = 1, nClubMember, 1 do	
		local tbMember = g_tbClubMemberList[index]
		strMember = strMember .. tbMember[1] .. ";"
	end
	SetData(WIDGET_ID_POSTBOX_SUB, strMember, "pageFriend.friendList.list", false)
	
	g_PostBoxSub_FriendPage_nSelectTab = 2	
	
	Ev_PostBoxSub_FindPage_UpdateClubList()
end

-------------------------------------------------------------------------------------------------------
-- 이벤트

function Ev_PostBoxSub_Drag_Start( nStageX, nStageY )
	InsertStageMouseMove( Ev_PostBoxSub_Drag )
	InsertStageMouseUp( Ev_PostBoxSub_Drag_End )
	
	g_PostBoxSub_StageX = nStageX
	g_PostBoxSub_StageY = nStageY
	
	g_PostBoxSub_X = GetInt( WIDGET_ID_POSTBOX_SUB, "x", "" )
	g_PostBoxSub_Y = GetInt( WIDGET_ID_POSTBOX_SUB, "y", "" )
end

function Ev_PostBoxSub_Drag_End( stageX, stageY )
	RemoveStageMouseMove( Ev_PostBoxSub_Drag )
	RemoveStageMouseUp( Ev_PostBoxSub_Drag_End )
end

function Ev_PostBoxSub_Drag( nStageX, nStageY )

	local nOffsetX = 0
	local nOffsetY = 0
	nOffsetX = nStageX - g_PostBoxSub_StageX
	nOffsetY = nStageY - g_PostBoxSub_StageY
	
	local nPostSubW = GetInt( WIDGET_ID_POSTBOX_SUB, "width", "" )
	local nPostW = 0
	
	if true == IsOpenWidget( WIDGET_ID_POSTBOX ) then
	
		nPostW = GetInt( WIDGET_ID_POSTBOX, "width", "" )
		
	end	
	
	local nNewX = 0
	local nNewY = 0
	nNewX, nNewY = PreventOutsideDrag( WIDGET_ID_POSTBOX,
		g_PostBoxSub_X + nOffsetX, g_PostBoxSub_Y + nOffsetY, 
		0, nPostW, 0, 0 )
	
	SetInt( WIDGET_ID_POSTBOX_SUB, "x", nNewX, "" )
	SetInt( WIDGET_ID_POSTBOX_SUB, "y", nNewY, "" )

	if true == IsOpenWidget( WIDGET_ID_POSTBOX ) then
	
		SetInt( WIDGET_ID_POSTBOX, "x", nNewX - nPostW, "" )
		SetInt( WIDGET_ID_POSTBOX, "y", nNewY, "" )
		
	end
	
end

function Ev_PostBoxSub_Close()
	RemoveStageMouseMove( PostBoxSub_Move )
	PostBox_Sub_Close()
end

-- 우편 삭제버튼
function Ev_PostBoxSub_ReadPage_DeleteButton()

	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, GetGameInternalText("MODAL_POSTBOX", 0),
		Ev_PostBox_ReceivePage_SelectedPostDelete, Event_MsgBox_Close )
		
end

-- 우편물 받기버튼
function Ev_PostBoxSub_ReadPage_TakeButton()
		
	if g_PostBox_tbReceivePageSelectedPost["billing"] == true then
		local strMoney = CurrencyFormat( g_PostBox_tbReceivePageSelectedPost["money"] * -1 )
		local strMsgBox = GetGameInternalText("MODAL_POSTBOX_2005", 1)		
		
		strMsgBox = StringFormat( strMsgBox, strMoney )		
		OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, strMsgBox, Ev_PostBox_TakePost, Event_MsgBox_Close )
		return
	end	

	local nSelectedItem = GetInt( WIDGET_ID_POSTBOX, "selectedIndex", "pageReceive.list" )
	g_Msg:ReqSelectPostTakeItem( nSelectedItem )
	g_PostBox_tbReceivePageSelectedPost = g_Post:GetReceivePageSelectPost( g_PostBox_nSelectedItem )
	PostBoxSub_ReadPageReload()	--여기
end

-- 대금청구 우편물 받기
function Ev_PostBox_TakePost()
	local nMoney = g_MyChar:GetInvenMoney() + g_PostBox_tbReceivePageSelectedPost["money"]
	
	if nMoney < 0  then
		CF_PrintMsg_SystemMsg( GetGameInternalText("POSTBOX_MSG_TAKE_CHAR_MONEY_LACK"), TEXT_COLOR_RED )
		return
	end
	
	Event_MsgBox_Close()
	
	local nSelectedItem = GetInt( WIDGET_ID_POSTBOX, "selectedIndex", "pageReceive.list" )
	g_Msg:ReqSelectPostTakeItem( nSelectedItem )
	
	g_PostBox_tbReceivePageSelectedPost = g_Post:GetReceivePageSelectPost( g_PostBox_nSelectedItem )
	PostBoxSub_ReadPageReload()	
	
end 

-- 우편 반환버튼
function Ev_PostBoxSub_ReadPage_ReturnButton()

	local nSelectedItem = GetInt( WIDGET_ID_POSTBOX, "selectedIndex", "pageReceive.list" )
	local tbSelectedPost = g_PostBox_tbReceivePageList[ nSelectedItem+1 ]

	for k,v in pairs(tbSelectedPost) do
	end

	local str = ""
	-- 대금청구를 거부하시겠습니까?
	if tbSelectedPost["type"] == 2 then
		str = GetGameInternalText("MODAL_POSTBOX", 7)
	-- 반송하시겠습니까?
	else
		str = GetGameInternalText("MODAL_POSTBOX", 1)
	end
	
	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, str, Ev_PostBox_ReceivePage_SelectedPostReturn, Event_MsgBox_Close )

end

-- 답장하기 버튼
function Ev_PostBoxSub_ReadPage_ReplyButton()
	SetInt( WIDGET_ID_POSTBOX, "selectedIndex", -1, "pageReceive.list" )
	g_PostBox_nSelectedItem = -1
	
	local strFrom = g_PostBox_tbReceivePageSelectedPost["from"]
	SetInt( WIDGET_ID_POSTBOX, 1, "selectedIndex", "tab" )
	SetString( WIDGET_ID_POSTBOX, "text", strFrom, "pageSend.sendTo" )
	Ev_PostBox_ChangeTab( 1 )
	CloseWidget( WIDGET_ID_POSTBOX_SUB )
end

-- 친구 페이지 탭 변경
function Ev_PostBoxSub_pageFriendTabChange( nIndex )

	-- 친구 탭 선택
	if 0 == nIndex then
	
		PostBoxSub_FindPage_FrirndTap()
	
	-- 클럽 탭 선택
	elseif 1 == nIndex then
	
		PostBoxSub_FindPage_ClubTap()
	
	end

end

function Ev_PostBoxSub_FindPage_UpdateClubList()
	
	for i = 1, POSTBOXSUB_FRIEND_COUNT_TOTAL, 1 do
		local strItemInst = "pageFriend.friendList.item" .. tostring( i )
		local nIndex = i
		
		if nIndex <= table.getn( g_tbClubMemberList ) then
			local tbItem = CopyTable( g_tbClubMemberList[ nIndex ] )
			if tbItem[2] == false then
				SetTextFormat( WIDGET_ID_POSTBOX_SUB, 0x888888, false, 0, 0, strItemInst )
			elseif tbItem[2] == true then
				SetTextFormat( WIDGET_ID_POSTBOX_SUB, 0xFFFFFF, false, 0, 0, strItemInst )
			end
		end
	end
end

function Ev_PostBoxSub_FindPage_AttachButton()

	local nSelected = GetInt( WIDGET_ID_POSTBOX_SUB, "selectedIndex", "pageFriend.friendList.list" ) + 1
	local strSelect = ""
	
	if g_tbFriendList == nil or nSelected <= 0 then 
		return ;
	end
	
	if g_PostBoxSub_FriendPage_nSelectTab == 1 then
		if nSelected <= table.getn( g_tbFriendList ) then
			local tbItem = CopyTable( g_tbFriendList[ nSelected ] )
			strSelect = tbItem[1]
		end
	elseif  g_PostBoxSub_FriendPage_nSelectTab == 2 then
		if nSelected <= table.getn( g_tbClubMemberList ) then
			local tbItem = CopyTable( g_tbClubMemberList[ nSelected ] )
			strSelect = tbItem[1]
		end
	end
	
	SetString( WIDGET_ID_POSTBOX, "text", strSelect, "pageSend.sendTo" )
	Ev_PostBoxSub_FindPage_CloseButton()

end

function Ev_PostBoxSub_FindPage_CloseButton()
	PostBoxSub_Reset_FindPage()
	
	CloseWidget( WIDGET_ID_POSTBOX_SUB )
end