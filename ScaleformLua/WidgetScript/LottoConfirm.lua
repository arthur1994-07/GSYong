
g_nLottoConfirm_SelectBuyPage = 1			-- 구입 목록 선택 페이지

-- 로또 당첨 확인 로드 완료
function LoadLottoConfirm()

	-- 타이틀
	local str = GetGameWordText( "LOTTO_MAIN_UI_BUTTON_TEXT", 1 )
	SetString( WIDGET_ID_LOTTOCONFIRM, "title", str, "" )
	
	-- 확인
	str = GetGameWordText( "LOTTO_MAIN_UI_BUTTON_TEXT", 1 )
	SetString( WIDGET_ID_LOTTOCONFIRM, "label", str, "btnOK" )
	SetEvent( WIDGET_ID_LOTTOCONFIRM, EVENT_BUTTON_CLICK, "Ev_LottoConfirm_OK", "", "btnOK" )
	
	-- 닫기
	str = GetGameWordText( "LOTTO_MAIN_UI_BUTTON_TEXT", 2 )
	SetString( WIDGET_ID_LOTTOCONFIRM, "label", str, "btnCancel" )
	SetEvent( WIDGET_ID_LOTTOCONFIRM, EVENT_BUTTON_CLICK, "Ev_LottoConfirm_Cancel", "", "btnCancel" )
	
	-- 페이지 버튼
	SetEvent( WIDGET_ID_LOTTOCONFIRM, EVENT_BUTTON_CLICK, "Ev_LottoConfirm_BuyListPage", "", "stepPage.nextBtn", "1" )
	SetEvent( WIDGET_ID_LOTTOCONFIRM, EVENT_BUTTON_CLICK, "Ev_LottoConfirm_BuyListPage", "", "stepPage.prevBtn", "0" )

end

-- 로또 당첨 확인 오픈
function OpenLottoConfirm()

	g_nLottoConfirm_SelectBuyPage = 1
	
	LottoConfirm_SetPageText()
	LottoConfirm_SetBuyListText()
	LottoConfirm_SetBuyList()
	
	-- 부모의 위치정보 가져오기 ( Lotto 창 )
	local nParentX = GetInt( WIDGET_ID_LOTTO, "x", "" )
	local nParentY = GetInt( WIDGET_ID_LOTTO, "y", "" )
	local nParentWidth = GetInt( WIDGET_ID_LOTTO, "width", "" )

	-- 당첨확인 로또창의 새로운 위치 세팅하기
	local nNewX = nParentX + nParentWidth
	local nNewY = nParentY
	
	nNewX, nNewY = PreventOutsideDrag( WIDGET_ID_LOTTOCONFIRM, nNewX, nNewY, 0, 0, 0, 0 )

	SetInt( WIDGET_ID_LOTTOCONFIRM, "x", nNewX, "" )
	SetInt( WIDGET_ID_LOTTOCONFIRM, "y", nNewY, "" )

end

-- 페이지 텍스트 설정
function LottoConfirm_SetPageText()

	local nCount = g_Lotto:GetBuyCount( g_nLotto_SelectTab, true )
	if 0 == nCount then
		nCount = 1
	end
	
	local nMaxPage = math.ceil( nCount / g_nLotto_BUYLISTCOUNT )
	local str = tostring( g_nLottoConfirm_SelectBuyPage ) .. "/" .. tostring( nMaxPage )
	
	SetString( WIDGET_ID_LOTTOCONFIRM, "text", str, "stepPage.textField" )

end

-- 구입 목록 텍스트 설정
function LottoConfirm_SetBuyListText()

	-- 구입 목록 텍스트
	str = StringFormat( GetGameInternalText( "LOTTO_CONFIRM_BUY_LIST_TEXT", 0 ),
		g_Lotto:GetBuyCount( g_nLotto_SelectTab, true ), g_Lotto:GetBuyCountMax( g_nLotto_SelectTab ) )
	SetString( WIDGET_ID_LOTTOCONFIRM, "text", str, "label" )

end

-- 구입 리스트 업데이트
function LottoConfirm_SetBuyList()

	--{
	--	{
	--		1 : 자동?			bool
	--		2 : 숫자 테이블		table
	--	}
	--	...
	--}
	local tbList = g_Lotto:GetBuyList( g_nLotto_SelectTab, true )
	
	-- 확인할 번호가 하나도 없을 때 당첨 확인 버튼 누르면 현재 회차 구매 목록이 날아간다. 버그인가?
	-- 일단 버튼 클릭 못하도록 막는다.
	if 0 == table.getn( tbList ) then
	
		SetBool( WIDGET_ID_LOTTOCONFIRM, "enabled", false, "btnOK" )
	
	else
	
		SetBool( WIDGET_ID_LOTTOCONFIRM, "enabled", true, "btnOK" )
	
	end
	
	-- 당첨 번호
	local tbWinNum = g_Lotto:GetConfirmNum( g_nLotto_SelectTab )
	
	-- 현재 보고 있는 페이지에 해당하는 숫자만 설정한다.
	local nStartIndex = (g_nLottoConfirm_SelectBuyPage - 1)  * g_nLotto_BUYLISTCOUNT + 1	
	local nIndex = 1
	
	for i = nStartIndex, nStartIndex + g_nLotto_BUYLISTCOUNT - 1, 1 do
	
		local strInst = "num" .. tostring( nIndex )
		nIndex = nIndex + 1
	
		local tbBuy = tbList[ i ]
		if nil == tbBuy then
		
			CallFunction( WIDGET_ID_LOTTOCONFIRM, "gotoAndPlay", "none", strInst .. ".mcAuto" )
			
			for j = 1, g_nLotto_NUMCOUNT - 1, 1 do
			
				SetString( WIDGET_ID_LOTTOCONFIRM, "text", "", strInst .. ".tfNum" .. tostring( j ) )
				CallFunction( WIDGET_ID_LOTTOCONFIRM, "gotoAndPlay", "Lotto_stay", strInst .. ".Num" .. tostring( j ) )
				
			end
		
		else
		
			-- 자동?
			if true == tbBuy[ 1 ] then
			
				CallFunction( WIDGET_ID_LOTTOCONFIRM, "gotoAndPlay", "auto", strInst .. ".mcAuto" )
			
			else
			
				CallFunction( WIDGET_ID_LOTTOCONFIRM, "gotoAndPlay", "manual", strInst .. ".mcAuto" )
			
			end
			
			-- 숫자 설정
			local tbNum = tbBuy[ 2 ]
			for j = 1, g_nLotto_NUMCOUNT - 1, 1 do
			
				SetString( WIDGET_ID_LOTTOCONFIRM, "text", tostring( tbNum[ j ] ), 
					strInst .. ".tfNum" .. tostring( j ) )
					
				-- 당첨 번호 체크
				if true == Lotto_CheckNumber( tbWinNum, tbNum[ j ] ) then
				
					CallFunction( WIDGET_ID_LOTTOCONFIRM, "gotoAndPlay", "Lotto_O", strInst .. ".Num" .. tostring( j ) )
				
				else
				
					CallFunction( WIDGET_ID_LOTTOCONFIRM, "gotoAndPlay", "Lotto_X", strInst .. ".Num" .. tostring( j ) )
				
				end
			
			end
		
		end
	
	end

end

-- 당첨 번호 체크
function Lotto_CheckNumber( tbNum, nNum )

	for value in pairs( tbNum ) do
	
		if tbNum[ value ] == nNum then
			return true
		end
	
	end
	
	return false

end

------------------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 업데이트
function LottoConfirm_Update( nSystemID )

	if false == IsOpenWidget( WIDGET_ID_LOTTOCONFIRM ) then
		return
	end
	
	-- 업데이트할 탭과 현재 보고 있는 탭이 다르면 안한다.
	if g_nLotto_SelectTab ~= nSystemID then
		return
	end
	
	g_nLottoConfirm_SelectBuyPage = 1
	
	LottoConfirm_SetPageText()
	LottoConfirm_SetBuyListText()
	LottoConfirm_SetBuyList()

end

------------------------------------------------------------------------------------------------------------------
-- 이벤트

-- 확인
function Ev_LottoConfirm_OK()

	g_Msg:ReqConfirmBuyListLotto( g_nLotto_SelectTab )
	
	-- Main Lotto UI 의 당첨확인 버튼을 비활성화시킨다
	SetBool( WIDGET_ID_LOTTO, "enabled", false, "buyList.btnCheck" )
	
	CloseWidget( WIDGET_ID_LOTTOCONFIRM )

end

-- 닫기
function Ev_LottoConfirm_Cancel()

	CloseWidget( WIDGET_ID_LOTTOCONFIRM )

end

-- 페이지 버튼 클릭
function Ev_LottoConfirm_BuyListPage( strPrev )

	local nPrev = tonumber( strPrev )
	if 0 == nPrev then
	
		if 1 >= g_nLottoConfirm_SelectBuyPage then
			return
		end
		
		g_nLottoConfirm_SelectBuyPage = g_nLottoConfirm_SelectBuyPage - 1
	
	else
	
		local nCount = g_Lotto:GetBuyCount( g_nLotto_SelectTab, true )
		if 0 == nCount then
			nCount = 1
		end
		
		local nMaxPage = math.ceil( nCount / g_nLotto_BUYLISTCOUNT )
		if nMaxPage <= g_nLottoConfirm_SelectBuyPage then
			return
		end
		
		g_nLottoConfirm_SelectBuyPage = g_nLottoConfirm_SelectBuyPage + 1
	
	end
	
	-- 페이지 텍스트 설정
	LottoConfirm_SetPageText()
	-- 구입 목록 설정
	LottoConfirm_SetBuyList()

end