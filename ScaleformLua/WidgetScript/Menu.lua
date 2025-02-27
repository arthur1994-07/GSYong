
g_Menu_bOpen = false --메뉴가 닫여 있는 상태

function LoadMenu()
	
	-- 팝업 visible 끄기
	SetBool( WIDGET_ID_MENU, "visible", false, "Popup_Bt" )

	-- 전장UI(U) 
	local str = GetGameWordText( "COMPETITION_WINDOW", 0 ) .. "(U)"
	SetString( WIDGET_ID_MENU, "label", str, "Popup_Bt.Menu.competition" )
	SetEvent( WIDGET_ID_MENU, EVENT_BUTTON_CLICK, "Ev_Menu_Competition", "", "Popup_Bt.Menu.competition" )

	-- 생활 기록부 (특별활동/출석부) (Y) <-단축키 스트링 추가
	str = GetGameWordText( "STUDENTRECORD_WINDOW_TITLE", 0 ) .. "(Y)"
	SetString( WIDGET_ID_MENU, "label", str, "Popup_Bt.Menu.studentrecord" )
	SetEvent( WIDGET_ID_MENU, EVENT_BUTTON_CLICK, "Ev_Menu_Studentrecord", "", "Popup_Bt.Menu.studentrecord" )
	
	-- 제작(;)   <-단축키 스트링 추가
	local bProductUsable = g_UserFeatures:IsUsingRenewProduct()
	if bProductUsable == true then
		str = GetGameWordText( "ITEM_MIX_BUTTON", 0 ) .. "(;)"
		SetString( WIDGET_ID_MENU, "label", str, "Popup_Bt.Menu.product" )
		SetEvent( WIDGET_ID_MENU, EVENT_BUTTON_CLICK, "Ev_Menu_Product", "", "Popup_Bt.Menu.product" )
	else
		SetBool( WIDGET_ID_MENU, "visible", false, "Popup_Bt.Menu.product" )
	end
	
	-- 게임옵션(Esc) <-단축키 스트링 추가
	str = GetGameWordText( "HWOPTION_WINDOW_NAME_STATIC", 0 ) .. "(ESC)"
	SetString( WIDGET_ID_MENU, "label", str, "Popup_Bt.Menu.option" )
	SetEvent( WIDGET_ID_MENU, EVENT_BUTTON_CLICK, "Ev_Menu_Option", "", "Popup_Bt.Menu.option" )
	
	-- 클럽(G)	 
	str = GetGameWordText( "GAMEMENU", 5 )
	SetString( WIDGET_ID_MENU, "label", str, "Popup_Bt.Menu.guild" )
	SetEvent( WIDGET_ID_MENU, EVENT_BUTTON_CLICK, "Ev_Menu_Guild", "", "Popup_Bt.Menu.guild" )
	
	-- 파티(P)	 
	str = GetGameWordText( "GAMEMENU", 3 )
	SetString( WIDGET_ID_MENU, "label", str, "Popup_Bt.Menu.party" )
	SetEvent( WIDGET_ID_MENU, EVENT_BUTTON_CLICK, "Ev_Menu_Party", "", "Popup_Bt.Menu.party" )
	
	-- 친구(F)	
	str = GetGameWordText( "GAMEMENU", 6 )
	SetString( WIDGET_ID_MENU, "label", str, "Popup_Bt.Menu.friend" )
	SetEvent( WIDGET_ID_MENU, EVENT_BUTTON_CLICK, "Ev_Menu_Friend", "", "Popup_Bt.Menu.friend" )
	
	-- 포인트 상점(|)
	str = GetGameWordText( "GAMEMENU", 10 )
	SetString( WIDGET_ID_MENU, "label", str, "Popup_Bt.Menu.itemshop" )
	SetEvent( WIDGET_ID_MENU, EVENT_BUTTON_CLICK, "Ev_Menu_Itemshop", "", "Popup_Bt.Menu.itemshop" )
	if g_UserFeatures:IsUsingPointShop() == false then
		SetBool( WIDGET_ID_MENU, "enabled", false, "Popup_Bt.Menu.itemshop" )
	end
	
	-- 퀘스트(D/T)
	str = GetGameWordText( "QUEST_WINDOW_STATIC", 0 ) .. "(D/T)"
	SetString( WIDGET_ID_MENU, "label", str, "Popup_Bt.Menu.quest" )
	SetEvent( WIDGET_ID_MENU, EVENT_BUTTON_CLICK, "Ev_Menu_Quest", "", "Popup_Bt.Menu.quest" )
	
	-- 스킬(K)	
	str = GetGameWordText( "GAMEMENU", 2 )
	SetString( WIDGET_ID_MENU, "label", str, "Popup_Bt.Menu.skill" )
	SetEvent( WIDGET_ID_MENU, EVENT_BUTTON_CLICK, "Ev_Menu_Skill", "", "Popup_Bt.Menu.skill" )
	
	-- 인벤토리(I) 
	str = GetGameWordText( "GAMEMENU", 0 )
	SetString( WIDGET_ID_MENU, "label", str, "Popup_Bt.Menu.inventory" )
	SetEvent( WIDGET_ID_MENU, EVENT_BUTTON_CLICK, "Ev_Menu_Inventory", "", "Popup_Bt.Menu.inventory" )
	
	-- 캐릭터(C) 
	str = GetGameWordText( "GAMEMENU", 1 )
	SetString( WIDGET_ID_MENU, "label", str, "Popup_Bt.Menu.character" )
	SetEvent( WIDGET_ID_MENU, EVENT_BUTTON_CLICK, "Ev_Menu_Character", "", "Popup_Bt.Menu.character" )
	
	-- 전체맵
	str = GetGameWordText( "GAMEMENU", 7 )
	SetString( WIDGET_ID_MENU, "label", str, "Popup_Bt.Menu.btnLargeMap" )
	SetEvent( WIDGET_ID_MENU, EVENT_BUTTON_CLICK, "Ev_Menu_LargeMap", "", "Popup_Bt.Menu.btnLargeMap" )
	
	local bLottoUsable = g_UserFeatures:IsUsingLottoSystem()
	if bLottoUsable == true then
		-- 로또
		str = GetGameWordText( "GAMEMENU", 15 )
		SetString( WIDGET_ID_MENU, "label", str, "Popup_Bt.Menu.btnLotto" )
		SetEvent( WIDGET_ID_MENU, EVENT_BUTTON_CLICK, "Ev_Menu_Lotto", "", "Popup_Bt.Menu.btnLotto" )
	else
		SetBool( WIDGET_ID_MENU, "visible", false, "Popup_Bt.Menu.btnLotto" )
	end
	
	if bLottoUsable == false or bProductUsable == false then
	
		-- 위에서 부터 순서대로 넣고 정렬한다;
		-- 모든 버튼 사이즈를 같다고 생각한다;
		local tbAlignMenuList = {}
		local nLottoIdx, nProductIdx, nBtnHeight = 3, 5, GetInt( WIDGET_ID_MENU, "height", "Popup_Bt.Menu.btnLotto" )
		table.insert(tbAlignMenuList, {0, "Popup_Bt.Menu.option"} )
		table.insert(tbAlignMenuList, {0, "Popup_Bt.Menu.itemshop"} )
		table.insert(tbAlignMenuList, {0, "Popup_Bt.Menu.btnLotto"} )
		table.insert(tbAlignMenuList, {0, "Popup_Bt.Menu.competition"} )
		table.insert(tbAlignMenuList, {0, "Popup_Bt.Menu.product"} )	
		
		local nBackgroundTrans = 0
		
		-- 로또 사용여부 설정;
		if false == bLottoUsable then
		
			for i = nLottoIdx-1, 1, -1 do
				tbAlignMenuList[i][1] = tbAlignMenuList[i][1] - nBtnHeight
			end
			nBackgroundTrans = nBackgroundTrans - nBtnHeight
			
		end
		
		if bProductUsable == false then
			
			for i = nProductIdx-1, 1, -1 do
				tbAlignMenuList[i][1] = tbAlignMenuList[i][1] - nBtnHeight
			end
			nBackgroundTrans = nBackgroundTrans - nBtnHeight
			
		end
		
		-- 실제 위치 수정;
		for i = nProductIdx - 1, 1, -1 do
			local nCurrentY = GetInt( WIDGET_ID_MENU, "y", tbAlignMenuList[i][2] )
			SetInt( WIDGET_ID_MENU, "y", nCurrentY - tbAlignMenuList[i][1], tbAlignMenuList[i][2] )
		end
		
		-- 배경 설정;
		local nBGH, nBGY = GetInt( WIDGET_ID_MENU, "height", "Popup_Bt.Menu.Background" )
					, GetInt( WIDGET_ID_MENU, "y", "Popup_Bt.Menu.Background" )
		SetInt( WIDGET_ID_MENU, "y", nBGY - nBackgroundTrans, "Popup_Bt.Menu.Background" )
		SetInt( WIDGET_ID_MENU, "height", nBGH + nBackgroundTrans, "Popup_Bt.Menu.Background" )
		
	end
end

-- 메뉴 닫기
function Menu_Close()

	-- 팝업 visible 끄기
	SetBool( WIDGET_ID_MENU, "visible", false, "Popup_Bt" )

	RemoveStageMouseClick( Ev_Menu_CheckClick )
	g_Menu_bOpen = false

end

-- 메뉴 열기/닫기
function Menu_Show()
	
	if false == g_Menu_bOpen then
	
		SetBool( WIDGET_ID_MENU, "visible", true, "Popup_Bt" )

		local bPrison = g_Client:IsCharPrison()
		
		if true == bPrison then
			SetBool( WIDGET_ID_MENU, "enabled", false, "Popup_Bt.Menu.competition" )
		else		
			SetBool( WIDGET_ID_MENU, "enabled", true, "Popup_Bt.Menu.competition" )
		end
		--메뉴 UI가 닫여 있을 경우 닫여 있는 BTUP프레임으로 이동하고 g_Menu_bOpen을 true로 변경한다. 
		CallFunction( WIDGET_ID_MENU, "gotoAndPlay", "BTUP", "Popup_Bt" )
		
		InsertStageMouseClickLater( Ev_Menu_CheckClick )
		g_Menu_bOpen = true
	
	else
	
		Menu_Close()
		
	end
	
end

-------------버튼 클릭 시 이벤트-------------

--- 전장UI(U)
function Ev_Menu_Competition()

	ToggleUI( WIDGET_ID_COMPETITION )
	Menu_Close()
	
end

--- 특별활동/출석부
function Ev_Menu_Studentrecord()

	ToggleUI( WIDGET_ID_STUDENT_RECORD )
	Menu_Close()
	
end

--- 제작
function Ev_Menu_Product()

	ToggleUI( WIDGET_ID_PRODUCT )
	Menu_Close()
	
end

--- 게임옵션(Esc) 
function Ev_Menu_Option()

	ToggleUI( WIDGET_ID_ESC )
	Menu_Close()
	
end

--- 클럽(G)
function Ev_Menu_Guild()

	ToggleUI( WIDGET_ID_CLUB )
	Menu_Close()
	
end

--- 파티(P)	
function Ev_Menu_Party()

	ToggleUI( WIDGET_ID_PARTYINFO )
	Menu_Close()
	
end

--- 커뮤니티 창 --친구(F)
function Ev_Menu_Friend()

	ToggleUI( WIDGET_ID_COMMUNITY )
	Menu_Close()
	
end

--- 포인트 상점
function Ev_Menu_Itemshop()

	ToggleUI( WIDGET_ID_POINTSHOP )
	Menu_Close()
	
end

--- 퀘스트(T)
function Ev_Menu_Quest()

	ToggleUI( WIDGET_ID_QUEST )
	Menu_Close()
	
end

--- 스킬(K)	
function Ev_Menu_Skill()

	ToggleUI( WIDGET_ID_SKILL )
	Menu_Close()
	
end

--- 인벤토리(I)	
function Ev_Menu_Inventory()

	ToggleUI( WIDGET_ID_INVENTORY )
	Menu_Close()
	
end

--- 캐릭터(C)
function Ev_Menu_Character()

	ToggleUI( WIDGET_ID_CHARINFO )
	Menu_Close()
	
end

-- 전체맵
function Ev_Menu_LargeMap()

	ToggleUI( WIDGET_ID_LARGEMAP )
	Menu_Close()

end

-- 로또
function Ev_Menu_Lotto()

	ToggleUI( WIDGET_ID_LOTTO )
	Menu_Close()

end

---메뉴 영역 밖을 클릭 했을 때 메뉴 닫기.
function Ev_Menu_CheckClick( nStageX, nStageY )

	local nX = GetInt( WIDGET_ID_MENU, "x", "" )
	local nW = GetInt( WIDGET_ID_MENU, "width", "" )
	local nY = GetInt( WIDGET_ID_MENU, "y", "" )
	local nH = GetInt( WIDGET_ID_MENU, "height", "" )
	
	if 	nX + nW > nStageX and
		nX < nStageX  and 
		nY + nH > nStageY and
		nY < nStageY then
	
		return
		
	end	
	
	Menu_Close()

end