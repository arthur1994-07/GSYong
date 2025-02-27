
-- 생성된 보상 슬롯 개수
g_nRanMobile_CreatedSlotCount = 0

-- 로그 완료
function LoadRanMobile()

	local str = ""

	-- 최상위 탭 설정
	local tbData = 
	{
		{ GetGameWordText( "RANMOBILE_TITLE", 1 ) },
		{ GetGameWordText( "RANMOBILE_TITLE", 2 ) },
		{ GetGameWordText( "RANMOBILE_TITLE", 3 ) },
	}
	SetDataEx( WIDGET_ID_RANMOBILE, "label", tbData, "tabTitle", false )
	SetEvent( WIDGET_ID_RANMOBILE, EVENT_INDEX_CLICK_CHANGE, "Ev_RanMobile_ChangeTitle", "index", "tabTitle" )
	SetInt( WIDGET_ID_RANMOBILE, "selectedIndex", 0, "tabTitle" )
	
	-----------------------------------------------------------------
	-- intro
	
	-- 탭 설정
	tbData = 
	{
		{ GetGameWordText( "RANMOBILE_TITLE", 4 ) },
		{ GetGameWordText( "RANMOBILE_TITLE", 8 ) },
		{ GetGameWordText( "RANMOBILE_TITLE", 5 ) },
	}
	SetDataEx( WIDGET_ID_RANMOBILE, "label", tbData, "intro.tab", false )
	SetEvent( WIDGET_ID_RANMOBILE, EVENT_INDEX_CLICK_CHANGE, "Ev_RanMobile_ChangeIntroTab", "index", "intro.tab" )
	SetInt( WIDGET_ID_RANMOBILE, "selectedIndex", 0, "intro.tab" )
	
	-- 란 인더 포켓이란 -------------
	str = GetGameInternalText( "RANMOBILE_INFO_TEXT1", 0 )
	SetString( WIDGET_ID_RANMOBILE, "text", str, "intro.intro.tfText1" )
	
	str = GetGameInternalText( "RANMOBILE_INFO_TEXT2", 0 ) .. 
		GetGameInternalText( "RANMOBILE_INFO_TEXT3", 0 ) .. 
		GetGameInternalText( "RANMOBILE_INFO_TEXT4", 0 )
	SetString( WIDGET_ID_RANMOBILE, "text", str, "intro.intro.tfText2" )
	
	str = GetGameWordText( "RANMOBILE_ASSET", 9 )
	SetString( WIDGET_ID_RANMOBILE, "label", str, "intro.intro.btnHomePage" )
	SetEvent( WIDGET_ID_RANMOBILE, EVENT_BUTTON_CLICK, "Ev_RanMobile_HomePage", "", "intro.intro.btnHomePage" )
	
	str = GetGameWordText( "RANMOBILE_ASSET", 10 )
	SetString( WIDGET_ID_RANMOBILE, "label", str, "intro.intro.btnDownload1" )
	SetEvent( WIDGET_ID_RANMOBILE, EVENT_BUTTON_CLICK, "Ev_RanMobile_AndroidDownload", "", "intro.intro.btnDownload1" )
	
	SetString( WIDGET_ID_RANMOBILE, "label", str, "intro.intro.btnDownload2" )
	SetEvent( WIDGET_ID_RANMOBILE, EVENT_BUTTON_CLICK, "Ev_RanMobile_IOSDownload", "", "intro.intro.btnDownload2" )
	
	-- 연동 방식 -------------
	str = GetGameInternalText( "RANMOBILE_INFO_TEXT5", 0 )
	SetString( WIDGET_ID_RANMOBILE, "text", str, "intro.recognize.tfText1" )
	
	-- 혜택 제공 -------------
	str = GetGameInternalText( "RANMOBILE_INFO_TEXT6", 0 )
	SetString( WIDGET_ID_RANMOBILE, "text", str, "intro.reward.tfText1" )
	
	-----------------------------------------------------------------
	-- charInfo
	
	-- 탭 설정
	tbData = 
	{
		{ GetGameWordText( "RANMOBILE_TITLE", 6 ) },
	}
	SetDataEx( WIDGET_ID_RANMOBILE, "label", tbData, "charInfo.tab", false )
	SetInt( WIDGET_ID_RANMOBILE, "selectedIndex", 0, "charInfo.tab" )
	
	-- 보석 전송
	str = GetGameWordText( "RANMOBILE_ASSET", 0 )
	SetString( WIDGET_ID_RANMOBILE, "label", str, "charInfo.info.btnSend" )
	SetEvent( WIDGET_ID_RANMOBILE, EVENT_BUTTON_CLICK, "Ev_RanMobile_SendGem", "", "charInfo.info.btnSend" )
	
	-- 일일 보상 받기
	str = GetGameWordText( "RANMOBILE_ASSET", 1 )
	SetString( WIDGET_ID_RANMOBILE, "label", str, "charInfo.info.btnReceive" )
	SetEvent( WIDGET_ID_RANMOBILE, EVENT_BUTTON_CLICK, "Ev_RanMobile_ReceiveReward", "", "charInfo.info.btnReceive" )
	
	-- 게임 머니 연동
	str = GetGameWordText( "RANMOBILE_ASSET", 2 )
	SetString( WIDGET_ID_RANMOBILE, "label", str, "charInfo.info.recognize.btnRecognize" )
	SetEvent( WIDGET_ID_RANMOBILE, EVENT_BUTTON_CLICK, "Ev_RanMobile_Recognize", "", "charInfo.info.recognize.btnRecognize" )
	
	-- 모바일 전용 닉네임
	str = GetGameWordText( "RANMOBILE_ASSET", 3 )
	SetString( WIDGET_ID_RANMOBILE, "text", str, "charInfo.info.info.tfMobileNameStatic" )
	
	-- 남은 연동 시간
	str = GetGameWordText( "RANMOBILE_ASSET", 4 )
	SetString( WIDGET_ID_RANMOBILE, "text", str, "charInfo.info.info.tfRemainTimeStatic" )
	
	-- 연장 금액
	str = GetGameWordText( "RANMOBILE_ASSET", 5 )
	SetString( WIDGET_ID_RANMOBILE, "text", str, "charInfo.info.info.tfPriceStatic" )
	
	local strColor = TranslateHTMLColor( GetMoneyColor( g_RanMobile:GetExtendPrice() ) )
	str = HTMLFontColor( CurrencyFormat( g_RanMobile:GetExtendPrice() ), strColor )
	SetString( WIDGET_ID_RANMOBILE, "htmlText", str, "charInfo.info.info.tfPrice" )
	
	-- 연장하기
	str = GetGameWordText( "RANMOBILE_ASSET", 6 )
	SetString( WIDGET_ID_RANMOBILE, "label", str, "charInfo.info.info.btnExtend" )
	SetEvent( WIDGET_ID_RANMOBILE, EVENT_BUTTON_CLICK, "Ev_RanMobile_Extend", "", "charInfo.info.info.btnExtend" )
	
	-- 정보 텍스트
	str = GetGameInternalText( "RANMOBILE_TEXT2", 0 )
	SetString( WIDGET_ID_RANMOBILE, "text", str, "charInfo.info.tfInfo" )
	
	-- 연동 정보 텍스트
	str = string.format( GetGameInternalText( "RANMOBILE_TEXT7", 0 ), CurrencyFormat( g_RanMobile:GetRecognizePrice() ) )
	SetString( WIDGET_ID_RANMOBILE, "text", str, "charInfo.info.recognize.tfRecognizeInfo" )
	
	-- 클래스 이미지
	SetBool( WIDGET_ID_RANMOBILE, "visible", false, "charInfo.info.info.mcClass.mcDead" )
	
	-----------------------------------------------------------------
	-- reward
	
	-- 탭 설정
	tbData = 
	{
		{ GetGameWordText( "RANMOBILE_TITLE", 7 ) },
	}
	SetDataEx( WIDGET_ID_RANMOBILE, "label", tbData, "reward.tab", false )
	SetInt( WIDGET_ID_RANMOBILE, "selectedIndex", 0, "reward.tab" )
	
	-- 소유한 아이템 개수
	str = GetGameWordText( "RANMOBILE_ASSET", 7 )
	SetString( WIDGET_ID_RANMOBILE, "text", str, "reward.reward.tfItemCountStatic" )
	SetString( WIDGET_ID_RANMOBILE, "text", "0", "reward.reward.tfItemCount" )
	
	-- 정보 텍스트
	str = GetGameInternalText( "RANMOBILE_TEXT1", 0 )
	SetString( WIDGET_ID_RANMOBILE, "text", str, "reward.reward.tfInfo" )
	
	-- MPoint 새로고침
	SetEvent( WIDGET_ID_RANMOBILE, EVENT_BUTTON_CLICK, "Ev_RanMobile_RefreshMPoint", "", "reward.reward.btnRefresh" )
	
	-- 보상 아이템 업데이트
	RanMobile_UpdateReward()

end

-- 오픈
function OpenRanMobile()

	local str = ""
	
	-- 클래스 이미지
	str = RanMobile_ClassToFrameID( g_MyChar.nClassIndex )
	CallFunction( WIDGET_ID_RANMOBILE, "gotoAndStop", str, "charInfo.info.info.mcClass" )
	
	-- 캐릭터 레벨
	str = "Lv. " .. g_MyChar:GetLevel()
	SetString( WIDGET_ID_RANMOBILE, "text", str, "charInfo.info.info.tfLevel" )
	
	-- 클래스 이름
	str = GetCharClassIndexName( g_MyChar.nClassIndex )
	SetString( WIDGET_ID_RANMOBILE, "text", str, "charInfo.info.info.tfClass" )
	
	-- 캐릭터 이름
	str = g_MyChar:GetName()
	SetString( WIDGET_ID_RANMOBILE, "text", str, "charInfo.info.info.tfName" )
	
	RanMobile_MsgInfo( 1 )

end

function RanMobile_Reset()

	g_nRanMobile_CreatedSlotCount = 0

end

-- 클래스 인덱스 -> 프레임 ID
function RanMobile_ClassToFrameID( nIndex )

	if nIndex == CHARCLASS_IDX_FIGHTER_M then
		return "M_BRAWLER_E"
	elseif nIndex == CHARCLASS_IDX_ARMS_M then
		return "M_SWORD_E"
	elseif nIndex == CHARCLASS_IDX_ARCHER_W then
		return "W_ARCHER_E"
	elseif nIndex == CHARCLASS_IDX_SPIRIT_W then
		return "W_SHAMAN_E"
	elseif nIndex == CHARCLASS_IDX_EXTREAM_M then
		return "M_EXTREAM_E"
	elseif nIndex == CHARCLASS_IDX_EXTREAM_W then
		return "W_EXTREAM_E"
	elseif nIndex == CHARCLASS_IDX_FIGHTER_W then
		return "W_BRAWLER_E"
	elseif nIndex == CHARCLASS_IDX_ARMS_W then
		return "W_SWORD_E"
	elseif nIndex == CHARCLASS_IDX_ARCHER_M then
		return "M_ARCHER_E"
	elseif nIndex == CHARCLASS_IDX_SPIRIT_M then
		return "M_SHAMAN_E"
	elseif nIndex == CHARCLASS_IDX_SCIENTIST_M then
		return "M_SCIENTIST_E"
	elseif nIndex == CHARCLASS_IDX_SCIENTIST_W then
		return "W_SCIENTIST_E"
	elseif nIndex == CHARCLASS_IDX_ASSASSIN_M then
		return "M_ASSASSIN_E"
	elseif nIndex == CHARCLASS_IDX_ASSASSIN_W then
		return "W_ASSASSIN_E"
	elseif nIndex == CHARCLASS_IDX_TRICKER_M then
		return "M_MAGICIAN_E"
	elseif nIndex == CHARCLASS_IDX_TRICKER_W then
		return "W_MAGICIAN_E"
	elseif nIndex == CHARCLASS_IDX_ACTOR_M then
		return "M_DRAMA_D"
	elseif nIndex == CHARCLASS_IDX_ACTOR_W then
		return "W_DRAMA_D"
	else
		return "NONE"
	end

end

-- 보상 리스트 업데이트
function RanMobile_UpdateReward()

	local tbCharInfo = g_RanMobile:GetCharInfo( 0 )
	local tbListData = {}
	
	--[[
	
		tb.set( 1, sReward.sID.Mid() );
		tb.set( 2, sReward.sID.Sid() );
		tb.set( 3, pItem->GetName() );
		tb.set( 4, ITEM_INFOR_COLOR::dwItemRank[ pItem->sBasicOp.emLevel ] );
		tb.set( 5, sReward.wCount );
		tb.set( 6, sReward.dwNecessaryMPoint );
	
	]]

	local tbRewardList = g_RanMobile:GetRewardItem()
	for value in pairs( tbRewardList ) do
	
		local tbReward = tbRewardList[ value ]
		
		-- MPoint 소지 개수보다 크면 빨간색 텍스트
		local strMPointColor = HTML_TEXT_COLOR_WHITE
		if nil == tbCharInfo[ 3 ] or tbCharInfo[ 3 ] < tbReward[ 6 ] then
			strMPointColor = HTML_TEXT_COLOR_RED
		end
		
		local tb = 
		{
			HTMLFontColor( tbReward[ 3 ], TranslateHTMLColor( tbReward[ 4 ] ) ),	-- 아이템 이름
			HTMLFontColor( tbReward[ 6 ], strMPointColor ),							-- MPoint
		}
		table.insert( tbListData, tb )
	
	end
	
	local strParam = CreateParam( "label1", "label2" )
	SetDataEx( WIDGET_ID_RANMOBILE, strParam, tbListData, "reward.reward.list" )
	
	-- 렌더러 설정
	for value in pairs( tbRewardList ) do

		local strInst = "reward.reward.list.mcItemRenderer.item" .. (value - 1)
		local strSlotID = SLOT_RANMOBILE_ID .. value
		
		-- 처음 생성된 렌더러다.
		if value > g_nRanMobile_CreatedSlotCount then
		
			g_nRanMobile_CreatedSlotCount = value
		
			-- 슬롯 등록 및 바인딩
			RegSlot( SLOT_ITEMBASE, strSlotID )
		
		end
		
		local tbReward = tbRewardList[ value ]
		
		-- 슬롯 아이템 설정
		SetString( WIDGET_ID_RANMOBILE, "binding", strSlotID, strInst .. ".slot" )
		SetSlot_RanMobileReward( strSlotID, value - 1 )
		
		-- 교환 버튼
		local str = GetGameWordText( "RANMOBILE_ASSET", 8 )
		SetString( WIDGET_ID_RANMOBILE, "label", str, strInst .. ".btn" )
		SetBool( WIDGET_ID_RANMOBILE, "enabled", nil ~= tbCharInfo[ 3 ] and tbCharInfo[ 3 ] >= tbReward[ 6 ], strInst .. ".btn" )
		SetEvent( WIDGET_ID_RANMOBILE, EVENT_BUTTON_CLICK, "Ev_RanMobile_TradeWidthMPoint", "", strInst .. ".btn", value - 1 )
	
	end

end

----------------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 계정 연동 결과
function RanMobile_MsgRecognize( nResult )

	-- 성공
	if 1 == nResult then
	
		-- 모바일 정보 재요청
		g_Msg:ReqRanMobileInfo()
		
	elseif -1 == nResult then
	
		local str = GetGameInternalText( "RanMobile_MsgRecognize - DB Error", 0 )
		CF_PrintMsg_SystemMsgBox( str, TEXT_COLOR_RED )
		
	-- 닉네임 중복
	elseif -2 == nResult then
	
		local str = GetGameInternalText( "RANMOBILE_TEXT10", 0 )
		CF_PrintMsg_SystemMsgBox( str, TEXT_COLOR_RED )
	
	-- 연장 시간 에러
	elseif -3 == nResult then
	-- 계정 정보 없음
	elseif -4 == nResult then
	
		local str = GetGameInternalText( "RANMOBILE_TEXT11", 0 )
		CF_PrintMsg_SystemMsgBox( str, TEXT_COLOR_RED )
	
	-- 소지금 부족
	elseif -5 == nResult then
	-- 연동권 없음
	elseif -6 == nResult then
	
	end

end

-- 계정 정보
function RanMobile_MsgInfo( nResult )

	if false == IsLoadWidget( WIDGET_ID_RANMOBILE ) then
		return
	end

	local tbCharInfo = g_RanMobile:GetCharInfo( 0 )
	if 1 == nResult and 0 ~= table.getn( tbCharInfo ) then
	
		SetBool( WIDGET_ID_RANMOBILE, "visible", true, "charInfo.info.info" )
		SetBool( WIDGET_ID_RANMOBILE, "visible", false, "charInfo.info.recognize" )
		SetBool( WIDGET_ID_RANMOBILE, "enabled", true, "charInfo.info.btnSend" )
		SetBool( WIDGET_ID_RANMOBILE, "enabled", true, "charInfo.info.btnReceive" )
		
		SetString( WIDGET_ID_RANMOBILE, "text", tbCharInfo[ 1 ], "charInfo.info.info.tfMobileName" )
		SetString( WIDGET_ID_RANMOBILE, "text", tbCharInfo[ 2 ], "charInfo.info.info.tfRemainTime" )
		SetString( WIDGET_ID_RANMOBILE, "text", tostring( tbCharInfo[ 3 ] ), "reward.reward.tfItemCount" )
		
		RanMobile_UpdateReward()
	
	else
	
		-- 연동한 캐릭터 이미 있을 경우 연동권으로만 가능하다.
		SetBool( WIDGET_ID_RANMOBILE, "enabled", 0 == g_RanMobile:GetTotalCharCount(), "charInfo.info.recognize.btnRecognize" )
	
		SetBool( WIDGET_ID_RANMOBILE, "visible", false, "charInfo.info.info" )
		SetBool( WIDGET_ID_RANMOBILE, "visible", true, "charInfo.info.recognize" )
		SetBool( WIDGET_ID_RANMOBILE, "enabled", false, "charInfo.info.btnSend" )
		SetBool( WIDGET_ID_RANMOBILE, "enabled", false, "charInfo.info.btnReceive" )
	
	end

end

-- M포인트 요청 결과
function RanMobile_MsgGetPoint( nResult )

	-- 성공
	if 1 == nResult then
	elseif -1 == nResult then
		local str = GetGameInternalText( "RanMobile_MsgGetPoint - DB Error", 0 )
		CF_PrintMsg_SystemMsgBox( str, TEXT_COLOR_RED )
	-- MPoint 값 잘못됨
	elseif -2 == nResult then
	-- 총 MPoint 보다 큰 값
	elseif -3 == nResult then
	-- DB Insert 에러
	elseif -4 == nResult then
	end

end

-- 시간 연장 결과
function RanMobile_MsgExtendTime( nResult )

	-- 성공
	if 1 == nResult then
	elseif -1 == nResult then
		local str = GetGameInternalText( "RanMobile_MsgExtendTime - DB Error", 0 )
		CF_PrintMsg_SystemMsgBox( str, TEXT_COLOR_RED )
	-- 연장 시간 잘못됨
	elseif -2 == nResult then
	-- DB Insert 에러
	elseif -3 == nResult then
	-- 캐릭터 정보 없음
	elseif -4 == nResult then
	-- 캐릭터 정보 없음
	elseif -5 == nResult then
	-- 소지금 부족
	elseif -6 == nResult then
	end

end

-- 보상 교환 결과
function RanMobile_MsgChangeItem( nResult )

	if 1 == nResult then
	
		local str = GetGameInternalText( "RANMOBILE_TEXT12", 0 )
		CF_PrintMsg_SystemMsgBox( str, TEXT_COLOR_LIGHTSKYBLUE )
	
		-- 모바일 정보 재요청
		g_Msg:ReqRanMobileInfo()
		
	elseif -1 == nResult then
	
		local str = GetGameInternalText( "RanMobile_MsgChangeItem - DB Error", 0 )
		CF_PrintMsg_SystemMsgBox( str, TEXT_COLOR_RED )
		
	-- 교환 데이터 없음
	elseif -2 == nResult then
	-- MPoint 부족
	elseif -3 == nResult then
	-- 인벤토리 가득참
	elseif -4 == nResult then
	
		local str = GetGameInternalText( "RANMOBILE_TEXT13", 0 )
		CF_PrintMsg_SystemMsgBox( str, TEXT_COLOR_RED )
	
	end

end

-- 보석 전송 결과
function RanMobile_MsgSendJewel( nResult )

	-- 성공
	if 1 == nResult then
	
		local str = GetGameInternalText( "RANMOBILE_TEXT5", 0 )
		CF_PrintMsg_SystemMsgBox( str, TEXT_COLOR_LIGHTSKYBLUE )
	
	elseif -1 == nResult then
	
		local str = GetGameInternalText( "RanMobile_MsgSendJewel - DB Error", 0 )
		CF_PrintMsg_SystemMsgBox( str, TEXT_COLOR_RED )
		
	-- 보석 개수 잘못됨
	elseif -2 == nResult then
	-- DB Insert 에러
	elseif -3 == nResult then
	-- 아이템 없음
	elseif -4 == nResult then
	
		local str = GetGameInternalText( "RANMOBILE_TEXT14", 0 )
		CF_PrintMsg_SystemMsgBox( str, TEXT_COLOR_RED )
	
	end

end

-- 일일 보상 결과
function RanMobile_MsgDailyReward( nResult )

	-- 성공
	if 1 == nResult then
	
		local str = GetGameInternalText( "RANMOBILE_TEXT16", 0 )
		CF_PrintMsg_SystemMsgBox( str, TEXT_COLOR_LIGHTSKYBLUE )
	
	elseif -1 == nResult then
	
		local str = GetGameInternalText( "RanMobile_MsgDailyReward - DB Error", 0 )
		CF_PrintMsg_SystemMsgBox( str, TEXT_COLOR_RED )
		
	-- DB 업데이트 에러
	elseif -2 == nResult then
	-- 인벤토리 가득참
	elseif -3 == nResult then
	-- 이미 받음
	elseif -4 == nResult then
	
		local str = GetGameInternalText( "RANMOBILE_TEXT15", 0 )
		CF_PrintMsg_SystemMsgBox( str, TEXT_COLOR_RED )
	
	end

end

-- 란 모바일 연동권 사용
function RanMobile_UseRecognizeItem()

	local tbCharInfo = g_RanMobile:GetCharInfo( 0 )
	if 0 ~= table.getn( tbCharInfo ) then
	
		local str = GetGameInternalText( "RANMOBILE_TEXT11", 0 )
		CF_PrintMsg_SystemMsgBox( str, TEXT_COLOR_RED )
		
		return
	
	end

	local str = GetGameInternalText( "RANMOBILE_TEXT6", 0 )
	OpenMsgBox( MSG_BOX_TYPE_INPUT, str, Ev_RanMobile_UseRecognizeItemOK, Event_MsgBox_Close )

end

-- 란 모바일 연동권 사용 OK
function Ev_RanMobile_UseRecognizeItemOK()

	local strName = GetMsgBoxInput()
	
	local nLen = string.len( strName )
	if 4 > nLen or 12 < nLen then
		return
	end
	
	g_Msg:ReqRanMobileCreateCash( strName )
	Event_MsgBox_Close()

end

----------------------------------------------------------------------------------------------------------------
-- 이벤트

-- 최상위 탭 변경
function Ev_RanMobile_ChangeTitle( nIndex )

	if 0 == nIndex then
	
		SetBool( WIDGET_ID_RANMOBILE, "visible", true, "intro" )
		SetBool( WIDGET_ID_RANMOBILE, "visible", false, "charInfo" )
		SetBool( WIDGET_ID_RANMOBILE, "visible", false, "reward" )
	
	elseif 1 == nIndex then
	
		SetBool( WIDGET_ID_RANMOBILE, "visible", false, "intro" )
		SetBool( WIDGET_ID_RANMOBILE, "visible", true, "charInfo" )
		SetBool( WIDGET_ID_RANMOBILE, "visible", false, "reward" )
	
	elseif 2 == nIndex then
	
		SetBool( WIDGET_ID_RANMOBILE, "visible", false, "intro" )
		SetBool( WIDGET_ID_RANMOBILE, "visible", false, "charInfo" )
		SetBool( WIDGET_ID_RANMOBILE, "visible", true, "reward" )
	
	end

end

-- 소개탭 변경
function Ev_RanMobile_ChangeIntroTab( nIndex )

	if 0 == nIndex then
	
		SetBool( WIDGET_ID_RANMOBILE, "visible", true, "intro.intro" )
		SetBool( WIDGET_ID_RANMOBILE, "visible", false, "intro.recognize" )
		SetBool( WIDGET_ID_RANMOBILE, "visible", false, "intro.reward" )
	
	elseif 1 == nIndex then
	
		SetBool( WIDGET_ID_RANMOBILE, "visible", false, "intro.intro" )
		SetBool( WIDGET_ID_RANMOBILE, "visible", true, "intro.recognize" )
		SetBool( WIDGET_ID_RANMOBILE, "visible", false, "intro.reward" )
	
	elseif 2 == nIndex then
	
		SetBool( WIDGET_ID_RANMOBILE, "visible", false, "intro.intro" )
		SetBool( WIDGET_ID_RANMOBILE, "visible", false, "intro.recognize" )
		SetBool( WIDGET_ID_RANMOBILE, "visible", true, "intro.reward" )
	
	end

end

-- 홈페이지 이동
function Ev_RanMobile_HomePage()

	local strURL = g_RanMobile:GetHomePageURL()
	OpenWebPage( strURL )

end

-- 안드로이드 다운로드 페이지 이동
function Ev_RanMobile_AndroidDownload()

	local strURL = g_RanMobile:GetAndroidDownlaodURL()
	OpenWebPage( strURL )

end

-- IOS 다운로드 페이지 이동
function Ev_RanMobile_IOSDownload()

	local strURL = g_RanMobile:GetIOSDownloadURL()
	OpenWebPage( strURL )

end

-- 보석 전송
function Ev_RanMobile_SendGem()

	local str = string.format( GetGameInternalText( "RANMOBILE_TEXT4", 0 ),
		1, g_RanMobile:GetMaxSendJewelCount() )
	OpenMsgBox( MSG_BOX_TYPE_INPUT, str, Ev_RanMobile_SendGemOK, Event_MsgBox_Close )

end

-- 보석 전송 확인
function Ev_RanMobile_SendGemOK()

	local nCount = tonumber( GetMsgBoxInput() )
	if nil == nCount or 0 >= nCount or g_RanMobile:GetMaxSendJewelCount() < nCount then
		return
	end
	
	g_Msg:ReqRanMobileSendJewel( nCount )
	Event_MsgBox_Close()

end

-- 일일 보상 받기
function Ev_RanMobile_ReceiveReward()

	local str = GetGameInternalText( "RANMOBILE_TEXT3", 0 )
	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, str, Ev_RanMobile_ReceiveRewardOK, Event_MsgBox_Close )

end

-- 일일 보상 받기 확인
function Ev_RanMobile_ReceiveRewardOK()

	g_Msg:ReqRanMobileGetDailyItem()
	Event_MsgBox_Close()

end

-- 게임 머니 연동
function Ev_RanMobile_Recognize()

	local str = GetGameInternalText( "RANMOBILE_TEXT6", 0 )
	OpenMsgBox( MSG_BOX_TYPE_INPUT, str, Ev_RanMobile_RecognizeOK, Event_MsgBox_Close )

end

-- 연동 확인
function Ev_RanMobile_RecognizeOK()

	local strName = GetMsgBoxInput()
	
	local nLen = string.len( strName )
	if 4 > nLen or 12 < nLen then
	
		local str = GetGameInternalText( "RANMOBILE_TEXT9", 0 )
		CF_PrintMsg_SystemMsgBox( str, TEXT_COLOR_RED )
	
		return
	end
	
	g_Msg:ReqRanMobileCreateMoney( strName )
	Event_MsgBox_Close()

end

-- 연장하기
function Ev_RanMobile_Extend()

	local str = GetGameInternalText( "RANMOBILE_TEXT8", 0 )
	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, str, Ev_RanMobile_ExtendOK, Event_MsgBox_Close )

end

-- 연장하기 OK
function Ev_RanMobile_ExtendOK()

	g_Msg:ReqRanMobileExtendTime()
	Event_MsgBox_Close()

end

-- MPoint 새로고침
function Ev_RanMobile_RefreshMPoint()

	g_Msg:ReqRanMobileInfo()

end

-- 보상탭 교환하기
function Ev_RanMobile_TradeWidthMPoint( strIndex )

	local nIndex = tonumber( strIndex )
	g_Msg:ReqRanMobileChangeItem( nIndex )

end