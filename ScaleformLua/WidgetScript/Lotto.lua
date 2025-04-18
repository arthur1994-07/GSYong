
LOTTO_GAMEMONEY_SYSTEM = 0
LOTTO_RANPOINT_SYSTEM = 1

BUY_LOTTO_FLAG_OK = 0
BUY_LOTTO_FLAG_FAIL = 1				-- 일반 실패;
BUY_LOTTO_FLAG_LOCK_FAIL = 2		-- 구매 잠금 상태로 인한 실패;
BUY_LOTTO_FLAG_LEVEL_FAIL = 3		-- 레벨 부족;
BUY_LOTTO_FLAG_MONEY_FAIL = 4		-- 구입 비용 없음;
BUY_LOTTO_FLAG_DUPLICATE_FAIL = 5	-- 중복 번호;
BUY_LOTTO_FLAG_NUM_FAIL = 6			-- 범위 밖의 숫자;
BUY_LOTTO_FLAG_ZERO_FAIL = 7		-- 숫자에 0이 들어옴;

g_nLotto_PREVWINNERLISTCOUNT = 6	-- 당첨 결과 리스트 개수
g_nLotto_NUMCOUNT = 7				-- 로또 번호 개수
g_nLotto_BUYLISTCOUNT = 5			-- 구입 목록 리스트 개수
g_nLotto_BUYNUMMIN = 1				-- 구매 입력 최소 숫자
g_nLotto_BUYNUMMAX = 15				-- 구매 입력 최대 숫자

g_nLotto_SelectTab = 0				-- 현재 선택된 탭 ( 보여줘야 할 로또 시스템 ) -> 사실 명확하지 않다
g_nLotto_SelectBuyPage = 1			-- 구입 목록 선택 페이지

g_bLotto_AutoInput = false			-- 자동 입력?

g_bLotto_Step = {}					-- 각 로또 시스템의 상태
g_bEnableBuy = {}					-- 각 로또 시스템의 구매가능 상태

-- 로또 로드 완료
function LoadLotto()

	local str = GetGameInternalText( "LOTTO_MAIN_UI_TITLE_TEXT", 0 )
	SetString( WIDGET_ID_LOTTO, "title", str, "" )

	-- 탭 설정
	local strParam = CreateParam( 
		GetGameWordText( "LOTTO_MAIN_UI_TAB_NAME", 0 ),
		GetGameWordText( "LOTTO_MAIN_UI_TAB_NAME", 1 ) )
	SetData( WIDGET_ID_LOTTO, strParam, "tab", false )
	SetEvent( WIDGET_ID_LOTTO, EVENT_INDEX_CLICK_CHANGE, "Ev_Lotto_SelectTab", "index", "tab" )
	SetInt( WIDGET_ID_LOTTO, "selectedIndex", g_nLotto_SelectTab, "tab" )
	
	-- 이전회차 결과보기
	str = GetGameWordText( "LOTTO_MAIN_UI_BUTTON_TEXT", 0 )
	SetString( WIDGET_ID_LOTTO, "text", str, "result.tfResult" )
	-- 순위
	str = GetGameWordText( "LOTTO_WIN_MAIN_LIST_INFO_COLUMN_NAME_TEXT", 0 )
	SetString( WIDGET_ID_LOTTO, "text", str, "result.tfRanking" )
	-- 당첨자
	str = GetGameWordText( "LOTTO_WIN_MAIN_LIST_INFO_COLUMN_NAME_TEXT", 1 )
	SetString( WIDGET_ID_LOTTO, "text", str, "result.tfName" )
	-- 당첨수
	str = GetGameWordText( "LOTTO_WIN_MAIN_LIST_INFO_COLUMN_NAME_TEXT", 2 )
	SetString( WIDGET_ID_LOTTO, "text", str, "result.tfCount" )
	-- 금액
	str = GetGameWordText( "LOTTO_WIN_MAIN_LIST_INFO_COLUMN_NAME_TEXT", 3 )
	SetString( WIDGET_ID_LOTTO, "text", str, "result.tfMoney" )
	-- 보너스
	str = GetGameInternalText( "LOTTO_BONUS_TEXT", 0 )
	SetString( WIDGET_ID_LOTTO, "text", str, "result.tfBonus" )
	
	-- 번호 입력
	str = GetGameWordText( "LOTTO_INPUTNUM_TEXT", 0 )
	SetString( WIDGET_ID_LOTTO, "text", str, "buy.tfInputNum" )
	-- 자동 입력
	str = GetGameWordText( "LOTTO_BUY_LOTTO_UI_BUTTON_TEXT", 0 )
	SetString( WIDGET_ID_LOTTO, "label", str, "buy.btnAuto" )
	-- 구입 비용
	str = GetGameInternalText( "LOTTO_TICKET_MONEY_TEXT", 0 )
	SetString( WIDGET_ID_LOTTO, "text", str, "buy.tfCostStatic" )
	-- 구입
	str = GetGameWordText( "LOTTO_BUY_LOTTO_UI_BUTTON_TEXT", 1 )
	SetString( WIDGET_ID_LOTTO, "label", str, "buy.btnBuy" )
	SetBool( WIDGET_ID_LOTTO, "enabled", false, "buy.btnBuy" )
	SetEvent( WIDGET_ID_LOTTO, EVENT_BUTTON_CLICK, "Ev_Lotto_Buy", "", "buy.btnBuy" )
	
	-- 연속 구매 수량
	str = GetGameWordText( "LOTTO_BUY_LOTTO_UI_TEXT", 0 )
	SetString( WIDGET_ID_LOTTO, "text", str, "buy.ContinuousBuyCount_text" )
	
	-- 장
	str = GetGameWordText( "LOTTO_BUY_LOTTO_UI_TEXT", 1 )
	SetString( WIDGET_ID_LOTTO, "text", str, "buy.Quantity" )
	
	-- 연속 자동 구입
	str = GetGameWordText( "LOTTO_BUY_LOTTO_UI_BUTTON_TEXT", 3 )
	SetString( WIDGET_ID_LOTTO, "label", str, "buy.btnContinuousBuy" )
	--SetBool( WIDGET_ID_LOTTO, "enabled", false, "buy.btnContinuousBuy" )
	SetEvent( WIDGET_ID_LOTTO, EVENT_BUTTON_CLICK, "Ev_Lotto_Buy_List", "", "buy.btnContinuousBuy" )
	
	-- 당첨 확인
	str = GetGameWordText( "LOTTO_MAIN_UI_BUTTON_TEXT", 1 )
	SetString( WIDGET_ID_LOTTO, "label", str, "buyList.btnCheck" )
	SetEvent( WIDGET_ID_LOTTO, EVENT_BUTTON_CLICK, "Ev_Lotto_ConfirmWin", "", "buyList.btnCheck" )
	
	-- 소지금
	str = GetGameInternalText( "LOTTO_MY_MONEY_TEXT", 0 )
	SetString( WIDGET_ID_LOTTO, "text", str, "buyList.labelMoneyStatic" )
	
	-- 누적당첨금액
	str = GetGameWordText( "LOTTO_ACCUMULATED_MONEY", 1 )
	SetString( WIDGET_ID_LOTTO, "text", str, "result.tfCumulativeResult"  )
	
	-- 추첨일.
	str = GetGameWordText( "LOTTO_DRAWING_DATE_TEXT", 0 )
	SetString( WIDGET_ID_LOTTO, "text", str, "buyList.DrawDate"  )
	
	-- 순위
	str = GetGameInternalText( "LOTTO_WIN_MAN_LIST_RANKING_TEXT", 0 )
	for i = 1, g_nLotto_PREVWINNERLISTCOUNT, 1 do
	
		local strRank = StringFormat( str, i )
		SetString( WIDGET_ID_LOTTO, "text", strRank, "result.item" .. tostring( i ) .. ".tfRank" )
		
		-- 바인딩
		SetString( WIDGET_ID_LOTTO, "binding", SLOT_LOTTO_WIN_LIST_ITEM_ID .. tostring( i ), "result.item" .. tostring( i ) .. ".slot" )
	
	end
	
	-- 이전 회차 리스트 이벤트
	SetEvent( WIDGET_ID_LOTTO, EVENT_LIST_INDEX_CHANGE, "Ev_Lotto_SelectPrevTurn", "index", "result.comboResult" )
	-- 페이지 버튼
	SetEvent( WIDGET_ID_LOTTO, EVENT_BUTTON_CLICK, "Ev_Lotto_BuyListPage", "", "buyList.stepPage.nextBtn", "1" )
	SetEvent( WIDGET_ID_LOTTO, EVENT_BUTTON_CLICK, "Ev_Lotto_BuyListPage", "", "buyList.stepPage.prevBtn", "0" )
	-- 누적 금액 새로고침 버튼
	SetEvent( WIDGET_ID_LOTTO, EVENT_BUTTON_CLICK, "Ev_Lotto_RefreshAccumMoney", "", "buyList.btnRefresh" )
	
	-- 바인딩
	SetString( WIDGET_ID_LOTTO, "binding", SLOT_LOTTO_WIN_ITEM_ID, "buyList.WinningItem" )
	
	-- 구매 입력 이벤트
	for i = 1, g_nLotto_NUMCOUNT - 1, 1 do
	
		SetEvent( WIDGET_ID_LOTTO, EVENT_KEY_UP, "Ev_Lotto_BuyNumInput",
			"keyCode", "buy.input" .. tostring( i ), tostring( i ) )
			
		SetEvent( WIDGET_ID_LOTTO, EVENT_FOCUS_OUT, "Ev_Lotto_BuyNumInputFocusOut",
			"", "buy.input" .. tostring( i ), tostring( i ) )
	
	end
	
	-- 자동 입력 이벤트
	SetEvent( WIDGET_ID_LOTTO, EVENT_BUTTON_CLICK, "Ev_Lotto_BuyAutoInput", "", "buy.btnAuto" )
	
	-- 도움말 버튼 이벤트
	SetEvent( WIDGET_ID_LOTTO, EVENT_BUTTON_CLICK, "Ev_Lotto_HelpButton_Click", "", "btnHelp" )
	
	g_bLotto_Step[ LOTTO_GAMEMONEY_SYSTEM ] = g_Msg:GetLottoSystemState ( LOTTO_GAMEMONEY_SYSTEM );
	g_bLotto_Step[ LOTTO_RANPOINT_SYSTEM ] = g_Msg:GetLottoSystemState ( LOTTO_RANPOINT_SYSTEM );
	
	Lotto_ChangeState( LOTTO_GAMEMONEY_SYSTEM, g_bLotto_Step[ LOTTO_GAMEMONEY_SYSTEM ] )
	Lotto_ChangeState( LOTTO_RANPOINT_SYSTEM, g_bLotto_Step[ LOTTO_RANPOINT_SYSTEM ] )

end

function OpenLotto ()

	-- Tab 설정
	local strParam;
	
	local nSystemCount = g_Lotto:GetLottoSystemCount()
	if 1 == nSystemCount then
	
		strParam = CreateParam( 
			GetGameWordText( "LOTTO_MAIN_UI_TAB_NAME", 0 ) )
	
	elseif 2 == nSystemCount then
	
		strParam = CreateParam( 
			GetGameWordText( "LOTTO_MAIN_UI_TAB_NAME", 0 ),
			GetGameWordText( "LOTTO_MAIN_UI_TAB_NAME", 1 ) )
			
	end
	
	SetData( WIDGET_ID_LOTTO, strParam, "tab", false )
	SetEvent( WIDGET_ID_LOTTO, EVENT_INDEX_CLICK_CHANGE, "Ev_Lotto_SelectTab", "index", "tab" )
	SetInt( WIDGET_ID_LOTTO, "selectedIndex", g_nLotto_SelectTab, "tab" )
	
end

-- 탭 정보 설정
function Lotto_SetTabInfo()

	-- Lotto Total Num 설정
	g_nLotto_BUYNUMMAX = g_Lotto:GetTotalNum( g_nLotto_SelectTab )

	-- 현재 회차 누적 금액
	Lotto_AccumulateMoney( g_nLotto_SelectTab, g_Lotto:GetAccumulateMoney( g_nLotto_SelectTab ), false )
	
	-- 현재 회차
	str = StringFormat( GetGameInternalText( "LOTTO_WIN_MAN_LIST_TURN_TEXT", 0 ),
		g_Lotto:GetCurTurnNum( g_nLotto_SelectTab ) + 1 )
	SetString( WIDGET_ID_LOTTO, "text", str, "buyList.labelTurn" )
	
	-- 당첨 아이템 보기
	str = StringFormat( GetGameInternalText( "LOTTO_WIN_ITEM_TEXT", 0 ),
		g_Lotto:GetCurTurnNum( g_nLotto_SelectTab ) + 1 )
	SetString( WIDGET_ID_LOTTO, "text", str, "buyList.WinningItemText" )
	
	-- 당첨 아이템 세팅
	SetSlot_LottoWinItem( SLOT_LOTTO_WIN_ITEM_ID, g_nLotto_SelectTab )
	
	-- 구입 가능 상태 설정
	SetBool( WIDGET_ID_LOTTO, "enabled", g_bEnableBuy[ g_nLotto_SelectTab ], "buy.btnBuy" )
	SetBool( WIDGET_ID_LOTTO, "enabled", g_bEnableBuy[ g_nLotto_SelectTab ], "buy.btnAuto" )
	SetBool( WIDGET_ID_LOTTO, "enabled", g_bEnableBuy[ g_nLotto_SelectTab ], "buy.btnContinuoutBuy" )
	CallFunction( WIDGET_ID_LOTTO, "gotoAndPlay", "none", "buy.mcAuto" )
	
	for i = 1, g_nLotto_NUMCOUNT - 1, 1 do
	
		SetBool( WIDGET_ID_LOTTO, "enabled", g_bEnableBuy[ g_nLotto_SelectTab ], "buy.input" .. tostring( i ) )
		SetString( WIDGET_ID_LOTTO, "text", "", "buy.input" .. tostring( i ) )
	
	end
	
	-- 추첨 시간
	local EnableBuyText = ""
	if ( g_bLotto_Step[ g_nLotto_SelectTab ] == 0 ) or ( g_bLotto_Step[ g_nLotto_SelectTab ] == 3 ) then
		EnableBuyText = HTMLFontColor( "(" .. GetGameWordText( "LOTTO_DRAWING_DATE_STATE", 0 ) .. ")", HTML_TEXT_COLOR_GREEN )
	elseif g_bLotto_Step[ g_nLotto_SelectTab ] == 1 then
		EnableBuyText = HTMLFontColor( "(" .. GetGameWordText( "LOTTO_DRAWING_DATE_STATE", 1 ) .. ")", HTML_TEXT_COLOR_RED )
	elseif g_bLotto_Step[ g_nLotto_SelectTab ] == 2 then
		EnableBuyText = HTMLFontColor( "(" .. GetGameWordText( "LOTTO_DRAWING_DATE_STATE", 2 ) .. ")", HTML_TEXT_COLOR_RED )
	end
	local tb = g_Lotto:GetStartTime( g_nLotto_SelectTab )
	str = StringFormat( GetGameInternalText( "LOTTO_DRAWING_DATE_DAY_TEXT", 0 ), tb[ 1 ] ) .. " " ..
		string.format( "%02d:%02d", tb[ 2 ], tb[ 3 ] ) .. EnableBuyText
	SetString( WIDGET_ID_LOTTO, "htmlText", str, "buyList.labelTime" )
	
	-- 구입 비용
	str = CurrencyFormat( g_Lotto:GetBuyCost( g_nLotto_SelectTab ) )
	SetString( WIDGET_ID_LOTTO, "text", str, "buy.tfCost" )
	
	-- 소지금
	if g_nLotto_SelectTab == 1 then
	
		str = GetGameInternalText( "LOTTO_MY_POINT_TEXT" )
		SetString( WIDGET_ID_LOTTO, "text", str, "buyList.labelMoneyStatic" )
		
	else
	
		str = GetGameInternalText( "LOTTO_MY_MONEY_TEXT" )
		SetString( WIDGET_ID_LOTTO, "text", str, "buyList.labelMoneyStatic" )
		
	end
	
	-- 소지금 설정
	Lotto_UpdateMoney()
	-- 이전 회차 리스트 설정
	Lotto_SetPrevTurnList()
	-- 이전 회차 당첨자 리스트 설정 
	Lotto_SetPrevTurnWinner()
	-- 페이지 텍스트 설정
	Lotto_SetPageText()
	-- 구입 목록 텍스트 설정
	Lotto_SetBuyListText()
	-- 구입 목록 설정
	Lotto_SetBuyList()
	-- 구입 상태 설정
	Lotto_SetBuyState()
	
	-- 당첨확인 버튼 설정
	local tbConfirmList = g_Lotto:GetBuyList( g_nLotto_SelectTab, true )
	local bConfirm = false
	if ( g_bLotto_Step[ g_nLotto_SelectTab ] == 0 ) or ( g_bLotto_Step[ g_nLotto_SelectTab ] == 3 ) then
		bConfirm = true
	end
	
	if ( 0 == #tbConfirmList ) then
		bConfirm = false
	end
	SetBool( WIDGET_ID_LOTTO, "enabled", bConfirm, "buyList.btnCheck" )
end

-- 이전 회차 리스트 설정
function Lotto_SetPrevTurnList()

	local tbList = {}

	local tbPrevTurn = g_Lotto:GetPrevTurnList( g_nLotto_SelectTab )
	for value in pairs( tbPrevTurn ) do
	
		local tbData =
		{
			StringFormat( GetGameInternalText( "LOTTO_WIN_MAN_LIST_TURN_TEXT", 0 ), tbPrevTurn[ value ] + 1 ),
			tbPrevTurn[ value ]
		}
		
		table.insert( tbList, tbData )
	
	end
	
	SetDataEx( WIDGET_ID_LOTTO, "label;turn;", tbList, "result.comboResult", false )
	SetInt( WIDGET_ID_LOTTO, "selectedIndex", 0, "result.comboResult" )
	
	-- 테이블이 하나라도 있으면 첫번째 회차 정보 서버에 요청
	if 0 ~= #tbPrevTurn then
	
		g_Msg:ReqPreWinNumLotto( g_nLotto_SelectTab, tbPrevTurn[ 1 ] )
		g_Msg:ReqPreWinManListLotto( g_nLotto_SelectTab, tbPrevTurn[ 1 ] )
		g_Msg:ReqPreAccumulateMoneyLotto( g_nLotto_SelectTab, tbPrevTurn[ 1 ] )
		
		-- 당첨 번호
		local str = StringFormat( GetGameInternalText( "LOTTO_WIN_NUM_TEXT", 0 ), tbPrevTurn[ 1 ] + 1 )
		SetString( WIDGET_ID_LOTTO, "text", str, "result.tfWinnerNum" )
		
	else
	
		-- 당첨 번호
		for i = 1, g_nLotto_NUMCOUNT, 1 do
	
			SetString( WIDGET_ID_LOTTO, "text", "0", "result.tfNum" .. tostring( i ) )
			CallFunction( WIDGET_ID_LOTTO, "gotoAndPlay", "Ball0", "result.Lotto_ball_slot" .. tostring( i ) )
		
		end
	
		-- 당첨 번호 타이틀
		local str = StringFormat( GetGameInternalText( "LOTTO_WIN_NUM_TEXT", 0 ), 0 )
		SetString( WIDGET_ID_LOTTO, "text", str, "result.tfWinnerNum" )
		
		-- 누적당첨금
		SetString( WIDGET_ID_LOTTO, "text", 0, "result.labelCumulateMoney" )
		
		-- 리스트
		local tbData =
		{
			StringFormat( GetGameInternalText( "LOTTO_WIN_MAN_LIST_TURN_TEXT", 0 ), 0 ),
			0
		}
		
		table.insert( tbList, tbData )
		
		SetDataEx( WIDGET_ID_LOTTO, "label;turn;", tbList, "result.comboResult", false )
		
	end

end

-- 이전 회차 당첨자 정보 리스트 설정
function Lotto_SetPrevTurnWinner()

	--{
	--	{
	--		1 : 당첨자 순위		number
	--		2 : 이름			string
	--		3 : 당첨수			number
	--		4 : 당첨금액			number
	--	}
	--	...
	--}
	local tbPrevWinnerList = g_Lotto:GetPrevTurnWinnerList( g_nLotto_SelectTab )
	
	local tbInsertRank = {}
	for i = 1, g_nLotto_PREVWINNERLISTCOUNT, 1 do
		table.insert( tbInsertRank, nil )
	end
	
	for value in pairs( tbPrevWinnerList ) do
	
		local tbWinner = tbPrevWinnerList[ value ]
		tbInsertRank[ tbWinner[ 1 ] ] = tbWinner
	
	end
	
	for i = 1, g_nLotto_PREVWINNERLISTCOUNT, 1 do
	
		local strInst = "result.item" .. tostring( i )
	
		local tbWinner = tbInsertRank[ i ]
		if nil == tbWinner then
		
			SetString( WIDGET_ID_LOTTO, "text", "-", strInst .. ".tfName" )
			SetString( WIDGET_ID_LOTTO, "text", "0", strInst .. ".tfCount" )
			SetString( WIDGET_ID_LOTTO, "text", "0", strInst .. ".tfMoney" )
			
			SetSlot_PreWinListItem( SLOT_LOTTO_WIN_LIST_ITEM_ID .. tostring( i ), g_nLotto_SelectTab, i )
		
		else
		
			if 0 == string.len( tbWinner[ 2 ] ) then
				SetString( WIDGET_ID_LOTTO, "text", "-", strInst .. ".tfName" )
			else
				local strName = ""
				if ( tbWinner[ 3 ] > 1 ) then
					strName = StringFormat( GetGameInternalText( "LOTTO_WIN_MAN_LIST_NAME_TEXT", 0 ),
					string.sub( tbWinner[ 2 ], 0, g_Lotto:GetViewNumCharName() ) .. "******" )
				else
					strName = string.sub( tbWinner[ 2 ], 0, g_Lotto:GetViewNumCharName() ) .. "******"
				end
				
				SetString( WIDGET_ID_LOTTO, "text", strName, strInst .. ".tfName" )
			end

			SetString( WIDGET_ID_LOTTO, "text", tostring( tbWinner[ 3 ] ), strInst .. ".tfCount" )
			SetString( WIDGET_ID_LOTTO, "text", CurrencyFormat( tbWinner[ 4 ] ), strInst .. ".tfMoney" )
			
			SetSlot_PreWinListItem( SLOT_LOTTO_WIN_LIST_ITEM_ID .. tostring( i ), g_nLotto_SelectTab, i )
		
		end
	
	end

end

-- 페이지 텍스트 설정
function Lotto_SetPageText()

	local nCount = g_Lotto:GetBuyCount( g_nLotto_SelectTab, false )
	if 0 == nCount then
		nCount = 1
	end
	
	local nMaxPage = math.ceil( nCount / g_nLotto_BUYLISTCOUNT )
	
	local str = tostring( g_nLotto_SelectBuyPage ) .. "/" .. tostring( nMaxPage )
	
	SetString( WIDGET_ID_LOTTO, "text", str, "buyList.stepPage.textField" )

end

-- 구입 목록 텍스트 설정
function Lotto_SetBuyListText()

	-- 구입 목록 텍스트
	str = StringFormat( GetGameInternalText( "LOTTO_MY_BUY_LIST_TEXT", 0 ),
		g_Lotto:GetBuyCount( g_nLotto_SelectTab, false ), g_Lotto:GetBuyCountMax( g_nLotto_SelectTab ) )
	SetString( WIDGET_ID_LOTTO, "text", str, "buyList.labelList" )

end

-- 구입 목록 설정
function Lotto_SetBuyList()

	--{
	--	{
	--		1 : 자동?			bool
	--		2 : 숫자 테이블		table
	--	}
	--	...
	--}
	local tbList = g_Lotto:GetBuyList( g_nLotto_SelectTab, false )
	
	-- 현재 보고 있는 페이지에 해당하는 숫자만 설정한다.
	local nStartIndex = (g_nLotto_SelectBuyPage - 1)  * g_nLotto_BUYLISTCOUNT + 1	
	local nIndex = 1
	
	for i = nStartIndex, nStartIndex + g_nLotto_BUYLISTCOUNT - 1, 1 do
	
		local strInst = "buyList.num" .. tostring( nIndex )
		nIndex = nIndex + 1
	
		local tbBuy = tbList[ i ]
		if nil == tbBuy then
		
			CallFunction( WIDGET_ID_LOTTO, "gotoAndPlay", "none", strInst .. ".mcAuto" )
			
			for j = 1, g_nLotto_NUMCOUNT - 1, 1 do
			
				SetString( WIDGET_ID_LOTTO, "text", "", strInst .. ".tfNum" .. tostring( j ) )
				
			end
		
		else
		
			-- 자동?
			if true == tbBuy[ 1 ] then
			
				CallFunction( WIDGET_ID_LOTTO, "gotoAndPlay", "auto", strInst .. ".mcAuto" )
			
			else
			
				CallFunction( WIDGET_ID_LOTTO, "gotoAndPlay", "manual", strInst .. ".mcAuto" )
			
			end
			
			-- 숫자 설정
			local tbNum = tbBuy[ 2 ]
			for j = 1, g_nLotto_NUMCOUNT - 1, 1 do
			
				SetString( WIDGET_ID_LOTTO, "text", tostring( tbNum[ j ] ), 
					strInst .. ".tfNum" .. tostring( j ) )
			
			end
		
		end
	
	end

end

-- 구매 상태 설정
function Lotto_SetBuyState()

	local bBuyAble = not g_Lotto:GetLockBuyState( g_nLotto_SelectTab )
	
	SetBool( WIDGET_ID_LOTTO, "enabled", bBuyAble, "buy.btnAuto" )
	
	for i = 1, g_nLotto_NUMCOUNT - 1, 1 do
	
		SetBool( WIDGET_ID_LOTTO, "enabled", bBuyAble, "buy.input" .. tostring( i ) )
	
	end

end

-- 입력 번호 중복 체크
function Lotto_CheckDuplicateInputNum( nIndex, nNum )

	if nil == nNum then
		return true
	end

	for i = 1, g_nLotto_NUMCOUNT - 1, 1 do
	
		if i ~= nIndex then
		
			local strOtherInst = "buy.input" .. tostring( i )
		
			local str = GetString( WIDGET_ID_LOTTO, "text", strOtherInst )
			
			-- 중복된 번호가 이미 입력되어 있는지 확인
			local nInputNum = tonumber( str )
			if nNum == nInputNum then
			
				local strInst = "buy.input" .. tostring( nIndex )
				SetString( WIDGET_ID_LOTTO, "text", "", strInst )
				
				-- 번호 중복됨. 메시지 박스 띄우기
				str = StringFormat( GetGameInternalText( "LOTTO_BUY_LOTTO_CONFIRM_TEXT2", 0 ), g_nLotto_NUMCOUNT - 1 )
				OpenMsgBox( MSG_BOX_TYPE_OK, str, Event_MsgBox_Close )
				
				return true
			
			end
			
		end
	
	end
	
	return false

end

----------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 당첨자 업데이트
function Lotto_UpdateWinnerList( nSystemID )

	if false == IsLoadWidget( WIDGET_ID_LOTTO ) then
		return
	end

	-- 업데이트할 탭과 현재 보고 있는 탭이 다르면 안한다.
	if g_nLotto_SelectTab ~= nSystemID then
		return
	end
	
	Lotto_SetPrevTurnWinner()

end

-- 당첨 번호 업데이트
function Lotto_UpdateWinNum( nSystemID, tbNum )

	if false == IsLoadWidget( WIDGET_ID_LOTTO ) then
		return
	end

	-- 업데이트할 탭과 현재 보고 있는 탭이 다르면 안한다.
	if g_nLotto_SelectTab ~= nSystemID then
		return
	end
	
	for i = 1, g_nLotto_NUMCOUNT, 1 do
	
		if nil ~= tbNum[ i ] then
		
			SetString( WIDGET_ID_LOTTO, "text", tostring( tbNum[ i ] ), "result.tfNum" .. tostring( i ) )
			
			if 1 <= tbNum[ i ] and 10 >= tbNum[ i ] then
			
				CallFunction( WIDGET_ID_LOTTO, "gotoAndPlay", "Ball01_10", "result.Lotto_ball_slot" .. tostring( i ) )
			
			elseif 11 <= tbNum[ i ] and 20 >= tbNum[ i ] then
			
				CallFunction( WIDGET_ID_LOTTO, "gotoAndPlay", "Ball11_20", "result.Lotto_ball_slot" .. tostring( i ) )
			
			elseif 21 <= tbNum[ i ] and 30 >= tbNum[ i ] then
			
				CallFunction( WIDGET_ID_LOTTO, "gotoAndPlay", "Ball21_30", "result.Lotto_ball_slot" .. tostring( i ) )
			
			elseif 31 <= tbNum[ i ] and 40 >= tbNum[ i ] then
			
				CallFunction( WIDGET_ID_LOTTO, "gotoAndPlay", "Ball31_40", "result.Lotto_ball_slot" .. tostring( i ) )
			
			elseif 41 <= tbNum[ i ] and 45 >= tbNum[ i ] then
			
				CallFunction( WIDGET_ID_LOTTO, "gotoAndPlay", "Ball41_45", "result.Lotto_ball_slot" .. tostring( i ) )
			
			else
			
				CallFunction( WIDGET_ID_LOTTO, "gotoAndPlay", "Ball0", "result.Lotto_ball_slot" .. tostring( i ) )
			
			end
			
		end
	
	end

end

-- 회차 누적 금액 설정
function Lotto_AccumulateMoney( nSystemID, llMoney, bPrev )

	if false == IsLoadWidget( WIDGET_ID_LOTTO ) then
		return
	end

	-- 업데이트할 탭과 현재 보고 있는 탭이 다르면 안한다.
	if g_nLotto_SelectTab ~= nSystemID then
		return
	end
	
	-- 이전 회차 누적 금액
	if true == bPrev then
		SetString( WIDGET_ID_LOTTO, "text", CurrencyFormat( llMoney ), "result.labelCumulateMoney" )
	-- 현재 회차 누적 금액
	else
		-- 집계상태일 경우
		if ( g_bLotto_Step[ g_nLotto_SelectTab ] == 3 ) then
			SetString( WIDGET_ID_LOTTO, "text", GetGameInternalText( "LOTTO_MAIN_UI_ACCUMULATE_MONEY_TEXT1", 0 ), "buyList.labelCurCumulateMoney" )
		else
			SetString( WIDGET_ID_LOTTO, "text", CurrencyFormat( llMoney ), "buyList.labelCurCumulateMoney" )
		end
	end

end

-- 로또 구매 리스트 업데이트
function Lotto_UpdateBuyList( nSystemID )

	if false == IsLoadWidget( WIDGET_ID_LOTTO ) then
		return
	end

	-- 업데이트할 탭과 현재 보고 있는 탭이 다르면 안한다.
	if g_nLotto_SelectTab ~= nSystemID then
		return
	end
	
	local nCount = g_Lotto:GetBuyCount( g_nLotto_SelectTab, false )
	if 0 == nCount then
		nCount = 1
	end
	
	local nMaxPage = math.ceil( nCount / g_nLotto_BUYLISTCOUNT )
	
	g_nLotto_SelectBuyPage = nMaxPage
	
	-- 페이지 텍스트 설정
	Lotto_SetPageText()
	-- 구입 목록 설정
	Lotto_SetBuyList()
	-- 구입 목록 텍스트 설정
	Lotto_SetBuyListText()
	
	
	-- 당첨확인 버튼 설정
	local tbConfirmList = g_Lotto:GetBuyList( g_nLotto_SelectTab, true )
	local bConfirm = false
	if ( g_bLotto_Step[ g_nLotto_SelectTab ] == 0 ) or ( g_bLotto_Step[ g_nLotto_SelectTab ] == 3 ) then
		bConfirm = true
	end
	
	if ( 0 == #tbConfirmList ) then
		bConfirm = false
	end
	SetBool( WIDGET_ID_LOTTO, "enabled", bConfirm, "buyList.btnCheck" )

end

-- 로또 다음턴 진행
function Lotto_NextTurn( nSystemID )

	if false == IsLoadWidget( WIDGET_ID_LOTTO ) then
		return
	end

	-- 업데이트할 탭과 현재 보고 있는 탭이 다르면 안한다.
	if g_nLotto_SelectTab ~= nSystemID then
		return
	end
	
	-- 구입 목록 페이지 세팅
	g_nLotto_SelectBuyPage = 1
	
	-- 탭 정보 설정
	Lotto_SetTabInfo()

end

-- 소지금 업데이트
function Lotto_UpdateMoney()

	if false == IsLoadWidget( WIDGET_ID_LOTTO ) then
		return
	end

	-- 내 돈
	if 0 == g_nLotto_SelectTab then
		str = CurrencyFormat( g_MyChar:GetInvenMoney() )
	else
		str = CurrencyFormat( g_MyChar:GetPointShopPoint() )
	end
	
	SetString( WIDGET_ID_LOTTO, "text", str, "buyList.tfMoney" )

end

-- 상태 업데이트
function Lotto_ChangeState( nSystemID, nState )

	if false == IsLoadWidget( WIDGET_ID_LOTTO ) then
		return
	end
	
	-- 업데이트할 탭과 현재 보고 있는 탭이 다르면 안한다.
	--if g_nLotto_SelectTab ~= nSystemID then
	--	return
	--end

	-- 기본 구입 가능 상태
	if 0 == nState then
	
		g_bEnableBuy[ nSystemID ] = true
		g_bLotto_Step[ nSystemID ] = 0
	
	-- 추첨 대기 상태
	elseif 1 == nState then
	
		g_bEnableBuy[ nSystemID ] = false
		g_bLotto_Step[ nSystemID ] = 1
	
	-- 추첨 상태
	elseif 2 == nState then
	
		g_bEnableBuy[ nSystemID ] = false
		g_bLotto_Step[ nSystemID ] = 2
	
		CF_PrintMsg_SystemMsg( StringFormat( 
			GetGameInternalText( "LOTTO_DRAWING_STATE_START_TEXT", 0 ), g_Lotto:GetCurTurnNum( nSystemID ) + 1 ),
			TEXT_COLOR_LIGHTSKYBLUE )
	
	-- 집계 상태
	elseif 3 == nState then
	
		g_bEnableBuy[ nSystemID ] = false
		g_bLotto_Step[ nSystemID ] = 3
	
		CF_PrintMsg_SystemMsg( StringFormat( 
			GetGameInternalText( "LOTTO_DRAWING_STATE_END_TEXT", 0 ), g_Lotto:GetCurTurnNum( nSystemID ) ),
			TEXT_COLOR_LIGHTSKYBLUE )
	
	end
	
	Lotto_SetTabInfo()

end

-----------------------------------------------------------------------------------------------------------
-- 이벤트

-- 탭 선택
function Ev_Lotto_SelectTab( nIndex )

	g_nLotto_SelectTab = nIndex
	
	if 0 == nIndex then
	
		SetBool( WIDGET_ID_LOTTO, "visible", false, "buyList.iconPoint" )
		SetBool( WIDGET_ID_LOTTO, "visible", true, "buyList.iconMoney" )
	
	else
	
		SetBool( WIDGET_ID_LOTTO, "visible", true, "buyList.iconPoint" )
		SetBool( WIDGET_ID_LOTTO, "visible", false, "buyList.iconMoney" )
	
	end
	
	-- 구입 목록 페이지 세팅
	local nCount = g_Lotto:GetBuyCount( g_nLotto_SelectTab, false )
	if 0 == nCount then
		nCount = 1
	end
	
	local nMaxPage = math.ceil( nCount / g_nLotto_BUYLISTCOUNT )
	
	g_nLotto_SelectBuyPage = nMaxPage
	
	-- 탭 정보 설정
	Lotto_SetTabInfo()

end

-- 이전 회차 선택
function Ev_Lotto_SelectPrevTurn( nIndex )

	local nTurn = GetDataInt( WIDGET_ID_LOTTO, nIndex, "turn", "result.comboResult" )
	
	-- 리스트 텍스트 초기화
	for i = 1, g_nLotto_PREVWINNERLISTCOUNT, 1 do
	
		local strInst = "result.item" .. tostring( i )
		SetString( WIDGET_ID_LOTTO, "text", "-", strInst .. ".tfName" )
		SetString( WIDGET_ID_LOTTO, "text", "0", strInst .. ".tfCount" )
		SetString( WIDGET_ID_LOTTO, "text", "0", strInst .. ".tfMoney" )
		
	end
	
	-- 당첨 번호
	local str = StringFormat( GetGameInternalText( "LOTTO_WIN_NUM_TEXT", 0 ), nTurn + 1 )
	SetString( WIDGET_ID_LOTTO, "text", str, "result.tfWinnerNum" )
	
	-- 서버에 이전 회차 정보 요청
	g_Msg:ReqPreWinNumLotto( g_nLotto_SelectTab, nTurn )
	g_Msg:ReqPreWinManListLotto( g_nLotto_SelectTab, nTurn )
	g_Msg:ReqPreAccumulateMoneyLotto( g_nLotto_SelectTab, nTurn )

end

-- 페이지 버튼 클릭
function Ev_Lotto_BuyListPage( strPrev )

	local nPrev = tonumber( strPrev )
	if 0 == nPrev then
	
		if 1 >= g_nLotto_SelectBuyPage then
			return
		end
		
		g_nLotto_SelectBuyPage = g_nLotto_SelectBuyPage - 1
	
	else
	
		local nCount = g_Lotto:GetBuyCount( g_nLotto_SelectTab, false )
		if 0 == nCount then
			nCount = 1
		end
		
		local nMaxPage = math.ceil( nCount / g_nLotto_BUYLISTCOUNT )
		if nMaxPage <= g_nLotto_SelectBuyPage then
			return
		end
		
		g_nLotto_SelectBuyPage = g_nLotto_SelectBuyPage + 1
	
	end
	
	-- 페이지 텍스트 설정
	Lotto_SetPageText()
	-- 구입 목록 설정
	Lotto_SetBuyList()

end

-- 구매 숫자 입력
function Ev_Lotto_BuyNumInput( nKeyCode, strIndex )

	local nIndex = tonumber( strIndex )
	
	-- 탭키 포커스 이동
	if KEYCODE_TAB == nKeyCode then
	
		-- 첫번째로
		if g_nLotto_NUMCOUNT - 1 == nIndex then
		
			SetFocus( WIDGET_ID_LOTTO, "buy.input1" )
		
		-- 다음 포커스 이동
		else
		
			nIndex = nIndex + 1
			SetFocus( WIDGET_ID_LOTTO, "buy.input" .. tostring( nIndex ) )
		
		end
	
	else
		
		local strInst = "buy.input" .. strIndex
	
		-- 수동 입력
		g_bLotto_AutoInput = false
		
		CallFunction( WIDGET_ID_LOTTO, "gotoAndPlay", "manual", "buy.mcAuto" )
	
		local nNum = tonumber( GetString( WIDGET_ID_LOTTO, "text", strInst ) )
		
		-- 입력된 번호가 없다.
		if nil == nNum then
		
			-- 구입 버튼 비활성화 시키고 종료
			SetBool( WIDGET_ID_LOTTO, "enabled", false, "buy.btnBuy" )
			return
			
		end
		
		-- 번호 최소/최대값 안에 있는지 확인
		if g_nLotto_BUYNUMMAX < nNum then
		
			nNum = g_nLotto_BUYNUMMAX
			SetString( WIDGET_ID_LOTTO, "text", tostring( nNum ), strInst )
			
		elseif g_nLotto_BUYNUMMIN > nNum then
		
			nNum = g_nLotto_BUYNUMMIN
			SetString( WIDGET_ID_LOTTO, "text", tostring( nNum ), strInst )
		
		end
		
		local bBuyAble = true
		
		for i = 1, g_nLotto_NUMCOUNT - 1, 1 do
		
			if nIndex ~= i then
			
				local strOtherInst = "buy.input" .. tostring( i )
				
				-- 모든 번호가 입력되어 있지 않으면 구입 버튼 비활성화
				local str = GetString( WIDGET_ID_LOTTO, "text", strOtherInst )
				if 0 == string.len( str ) then
				
					bBuyAble = false
					break
					
				end
			
			end
		
		end
		
		-- 구입 버튼 활성 설정
		SetBool( WIDGET_ID_LOTTO, "enabled", bBuyAble, "buy.btnBuy" )
	
	end

end

-- 구매 입력 포커스 아웃
function Ev_Lotto_BuyNumInputFocusOut( strIndex )

	local nIndex = tonumber( strIndex )
	local nNum = tonumber( GetString( WIDGET_ID_LOTTO, "text", "buy.input" .. strIndex ) )

	-- 중복된 숫자 있는지 확인
	if true == Lotto_CheckDuplicateInputNum( nIndex, nNum ) then
	
		-- 있으면 구입 버튼 비활성화
		SetBool( WIDGET_ID_LOTTO, "enabled", false, "buy.btnBuy" )
		return
		
	end

end

-- 자동 입력
function Ev_Lotto_BuyAutoInput()

	g_bLotto_AutoInput = true

	-- 입력 상태 설정
	CallFunction( WIDGET_ID_LOTTO, "gotoAndPlay", "auto", "buy.mcAuto" )
	
	local tbNum = g_Lotto:GetAutoNum( g_nLotto_SelectTab )
	for value in pairs( tbNum ) do
	
		if value > g_nLotto_NUMCOUNT - 1 then
			break
		end
	
		local strInst = "buy.input" .. tostring( value )
		SetString( WIDGET_ID_LOTTO, "text", tostring( tbNum[ value ] ), strInst )
	
	end
	
	-- 구입 버튼 활성화
	SetBool( WIDGET_ID_LOTTO, "enabled", true, "buy.btnBuy" )

end

-- 구입
function Ev_Lotto_Buy()

	local tbNum = {}
	for i = 1, g_nLotto_NUMCOUNT - 1, 1 do
	
		local strInst = "buy.input" .. tostring( i )
		
		local nNum = tonumber( GetString( WIDGET_ID_LOTTO, "text", strInst ) )
		
		-- 숫자가 하나라도 없으면 그냥 종료
		if nil == nNum then
		
			BasicChat_PrintSystemMsg(CHAT_SYSTEM, GetGameInternalText("LOTTO_MAIN_UI_BUY_BUTTON_EXCEPTION_TEXT6", 0), TEXT_COLOR_RED)
			
			return
			
		end
		
		table.insert( tbNum, nNum )
		
		-- 입력된 번호 지우기
		SetString( WIDGET_ID_LOTTO, "text", "", strInst )
	
	end
	
	-- 입력 상태 설정
	CallFunction( WIDGET_ID_LOTTO, "gotoAndPlay", "none", "buy.mcAuto" )
	
	-- 구입 버튼 비활성화
	SetBool( WIDGET_ID_LOTTO, "enabled", false, "buy.btnBuy" )
	
	local nBuyCount = g_Lotto:GetBuyCount( g_nLotto_SelectTab, true )
	if nBuyCount ~= 0 then
	
		BasicChat_PrintSystemMsg(CHAT_SYSTEM, GetGameInternalText("LOTTO_MAIN_UI_BUY_BUTTON_EXCEPTION_TEXT2"), TEXT_COLOR_RED)
		return
		
	end	
	
	local nMaxBuyCount = g_Lotto:GetBuyCountMax( g_nLotto_SelectTab )
	local nCurBuyCount = g_Lotto:GetBuyCount( g_nLotto_SelectTab, false )
	local nRemainBuyCount = nMaxBuyCount - nCurBuyCount
	if ( nRemainBuyCount <= 0 ) then
	
		BasicChat_PrintSystemMsg(CHAT_SYSTEM, GetGameInternalText("LOTTO_MAIN_UI_BUY_BUTTON_EXCEPTION_TEXT6"), TEXT_COLOR_RED)
		return
		
	end
	
	-- 구입 요청
	local nFlag = g_Msg:ReqBuyLotto( g_nLotto_SelectTab, tbNum, g_bLotto_AutoInput )
	
	if ( BUY_LOTTO_FLAG_LOCK_FAIL == nFlag ) then
		
		BasicChat_PrintSystemMsg(CHAT_SYSTEM, GetGameInternalText("LOTTO_MAIN_UI_BUY_BUTTON_EXCEPTION_TEXT5"), TEXT_COLOR_RED)
		
	elseif ( BUY_LOTTO_FLAG_MONEY_FAIL == nFlag ) then
	
		BasicChat_PrintSystemMsg(CHAT_SYSTEM, GetGameInternalText("LOTTO_BUY_LOTTO_EXCEPTION_LACK_MONEY_TEXT", 0), TEXT_COLOR_RED)
		
	elseif ( BUY_LOTTO_FLAG_OK ~= nFlag ) then
	
		BasicChat_PrintSystemMsg(CHAT_SYSTEM, GetGameInternalText("LOTTO_MAIN_UI_BUY_BUTTON_EXCEPTION_TEXT6", 0), TEXT_COLOR_RED)
		
	end
end

-- 여러장 구입
function Ev_Lotto_Buy_List ( )

	local nCount = tonumber( GetString( WIDGET_ID_LOTTO, "text", "buy.ContinuousBuyCount" ) )
	
	if nil == nCount then
	
		BasicChat_PrintSystemMsg(CHAT_SYSTEM, GetGameInternalText("LOTTO_MAIN_UI_BUY_BUTTON_EXCEPTION_TEXT6", 0), TEXT_COLOR_RED)
		
		return
		
	end
	
	local nBuyCount = g_Lotto:GetBuyCount( g_nLotto_SelectTab, true )
	if nBuyCount ~= 0 then
	
		BasicChat_PrintSystemMsg(CHAT_SYSTEM, GetGameInternalText("LOTTO_MAIN_UI_BUY_BUTTON_EXCEPTION_TEXT2"), TEXT_COLOR_RED)
		return
		
	end	
	
	local nMaxBuyCount = g_Lotto:GetBuyCountMax( g_nLotto_SelectTab )
	local nCurBuyCount = g_Lotto:GetBuyCount( g_nLotto_SelectTab, false )
	local nRemainBuyCount = nMaxBuyCount - nCurBuyCount
	if ( nRemainBuyCount < nCount ) then
	
		nCount = nRemainBuyCount
		SetString( WIDGET_ID_LOTTO, "text", tostring( nRemainBuyCount ), "buy.ContinuousBuyCount" )
	
	end
	
	local nFlag = g_Msg:ReqBuyLottoList( g_nLotto_SelectTab, nCount )
	
	if ( BUY_LOTTO_FLAG_LOCK_FAIL == nFlag ) then
	
		BasicChat_PrintSystemMsg(CHAT_SYSTEM, GetGameInternalText("LOTTO_MAIN_UI_BUY_BUTTON_EXCEPTION_TEXT5"), TEXT_COLOR_RED)
		
	elseif ( BUY_LOTTO_FLAG_MONEY_FAIL == nFlag ) then
	
		BasicChat_PrintSystemMsg(CHAT_SYSTEM, GetGameInternalText("LOTTO_BUY_LOTTO_EXCEPTION_LACK_MONEY_TEXT", 0), TEXT_COLOR_RED)
		
	elseif ( BUY_LOTTO_FLAG_OK ~= nFlag ) then
	
		BasicChat_PrintSystemMsg(CHAT_SYSTEM, GetGameInternalText("LOTTO_MAIN_UI_BUY_BUTTON_EXCEPTION_TEXT6", 0), TEXT_COLOR_RED)
		
	end
	
end

-- 누적 금액 새로고침
function Ev_Lotto_RefreshAccumMoney()

	g_Msg:ReqPreAccumulateMoneyLotto( g_nLotto_SelectTab, g_Lotto:GetCurTurnNum( g_nLotto_SelectTab ) )

end

-- 당첨 확인
function Ev_Lotto_ConfirmWin()

	if ( IsOpenWidget( WIDGET_ID_LOTTOCONFIRM ) ) then
		
		CloseWidget( WIDGET_ID_LOTTOCONFIRM )
	
	else
	
		OpenWidget( WIDGET_ID_LOTTOCONFIRM )
		
	end

end

-- 도움말 버튼 클릭
function Ev_Lotto_HelpButton_Click()
	local str = GetGameInternalText("LOTTO_MAIN_UI_HELP_TEXT1") .. "\n\n\n" ..
			GetGameInternalText("LOTTO_MAIN_UI_HELP_TEXT2") .. "\n\n\n" ..
			GetGameInternalText("LOTTO_MAIN_UI_HELP_TEXT3") .. "\n"

	HelpTooltip_SetText( str )
end