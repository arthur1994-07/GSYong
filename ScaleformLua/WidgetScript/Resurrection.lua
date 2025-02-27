
-- 부활 타입
g_nResurrection_Type_CheckPoint 	= 0		-- 체크 포인트
g_nResurrection_Type_CurPos 		= 1		-- 현재 위치 (귀혼주)
g_nResurrection_Type_EXP 			= 2		-- 경험치 회복
g_nResurrection_Type_Skill		 	= 3		-- 스킬 부활

g_nResurrection_Action = -1			-- 선택된 부활 타입

g_bResurrection_Skill = false		-- 스킬 부활?
g_nResurrection_Time = 0			-- 부활 시간
g_nResurrection_ActionTime = 0		-- 부활 액션 대기 시간

g_funcResurrection_Action = nil		-- 부활 동작 함수

-- 부활 UI 로드 완료
function LoadResurrection()

	-- 윈도우 드래그 막기
	SetBool( WIDGET_ID_RESURRECTION, "customDrag", true, "" )
	-- 닫기 버튼 숨기기
	SetBool( WIDGET_ID_RESURRECTION, "visible", false, "closeBtn" )
	
	-- 자동부활 체크박스
	SetString( WIDGET_ID_RESURRECTION, "text",
		GetGameInternalText( "REBIRTH_WINDOW_AUTO_REBIRTH", 0 ), "checkAuto_text" )
	SetEvent( WIDGET_ID_RESURRECTION, EVENT_BUTTON_CLICK, "Ev_Resurrtion_checkAuto", "", "checkAuto" )

	-- 아이템 광고 텍스트
	SetString( WIDGET_ID_RESURRECTION, "text",
		GetGameInternalText( "REBIRTH_WINDOW_RECOMMEND_ITEM_EXPLAIN", 0 ), "labelAD" )
	
	-- 구입
	SetString( WIDGET_ID_RESURRECTION, "label",
		GetGameWordText( "REBIRTH_WINDOW_BUY_BUTTON_TEXT", 0 ), "btnBuy" )
	SetEvent( WIDGET_ID_RESURRECTION, EVENT_BUTTON_CLICK, "Ev_Resurrection_BuyADItem", "", "btnBuy" )
	
	-- 슬롯 바인딩, 아이템 설정
	SetString( WIDGET_ID_RESURRECTION, "binding", SLOT_RESURRECTION_AD_ID, "slot" )
	SetSlot_Item( SLOT_RESURRECTION_AD_ID, g_nRESURRECT_RECOMMEND_ITEM_MID, g_nRESURRECT_RECOMMEND_ITEM_SID, 1 )
	
	if g_UserFeatures:IsUsingPointShop() == false then
		SetBool( WIDGET_ID_RESURRECTION, "enabled", false, "btnBuy" )
	end
	
end

-- 부활창 오픈
function OpenResurrection()

	SetBool( WIDGET_ID_RESURRECTION, "selected", GetAutoResurrection(), "checkAuto" )
	
	-- 텍스트 설정
	Resurrection_SetText()
	
	-- 타이머 설정
	Resurrection_SetTimer()
	
	-- 부활하기/경험치 회복 버튼 활성화
	SetBool( WIDGET_ID_RESURRECTION, "enabled", true, "btnResurrection" )
	SetBool( WIDGET_ID_RESURRECTION, "enabled", true, "btnAction" )
	
	-- 손실/회복 경험치
	local tbRecovery = g_MyChar:GetRsurrectionExpRecovery()
	local nDecReExp = tbRecovery[ "nDecExp" ] - tbRecovery[ "nReExp" ]
	local nLevelUpExp = g_MyChar:GetLevelUpExp()
	
	-- 부활 조건
	local tbCondition = g_MyChar:GetResurrectionCondition()
	
	-- 서브 텍스트
	local strSub = ""
	
	-- 부활버튼 툴팁 이벤트
	local tbMapList = g_Minimap:GetMapList()
	local dwStartID = g_MyChar:GetStartMapID()
	local strStartMap = ""
	
	for i = 1, #tbMapList do
	
		local tbMap = tbMapList[i]
		if dwStartID == tbMap["dwID"] then
			strStartMap = tbMap["strName"]
		end
		
	end
	
	local fDecExpRatio = ( tbRecovery[ "nDecExp" ] / g_MyChar:GetLevelUpExp() ) * 100
	
	-- 부활 타입 설정, 액션 버튼 이벤트 설정
	if false == g_bResurrection_Skill then
	
		-- 부활장소, 경험치 정수, 경험치 퍼센트
		local strToolTip_Resurre = string.format( GetGameInternalText("REBIRTH_WINDOW_INFORMATION_CHECK_POINT"), 
			strStartMap, CurrencyFormat( tbRecovery[ "nDecExp" ] ), fDecExpRatio )
		SetString( WIDGET_ID_RESURRECTION, "TooltipText", strToolTip_Resurre, "btnResurrection" )
	
		-- 부활 버튼 텍스트, 이벤트
		SetString( WIDGET_ID_RESURRECTION, "label", GetGameWordText( "REBIRTH_BUTTON", 0 ), "btnResurrection" )
		SetEvent( WIDGET_ID_RESURRECTION, EVENT_BUTTON_CLICK, "Ev_Resurrection_CheckPoint", "", "btnResurrection" )

	-- 스킬 부활
	else
	
		local str = GetGameWordText( "REBIRTH_BUTTON", g_nResurrection_Type_Skill )
		
		SetString( WIDGET_ID_RESURRECTION, "label", str, "btnResurrection" )
		SetEvent( WIDGET_ID_RESURRECTION, EVENT_BUTTON_CLICK, "Ev_Resurrection_Skill", "", "btnResurrection" )
		
		SetString( WIDGET_ID_RESURRECTION, "TooltipText", "", "btnResurrection" )
		
	end
	
	-- 귀혼주 O, 선도불가지역 x, 귀혼불가지역 x
	if 	true == tbCondition[ "bItemResurrect" ] and
		false == tbCondition[ "bGuildBattleMap" ] then
		
		g_nResurrection_Action = g_nResurrection_Type_CurPos
		
		strSub = string.format( GetGameInternalText( "REBIRTH_WINDOW_NOTIFY_CURRENT_POS_REBIRTH", 0 ),
			CurrencyFormat( tbRecovery[ "nDecExp" ] ), fDecExpRatio )
			
		g_funcResurrection_Action = Resurrection_DoActionCurPos
		
		-- 버튼 툴팁
		local strToolTip_Action = GetGameInternalText("REBIRTH_WINDOW_INFORMATION_CURRENT_POSITION")
		SetString( WIDGET_ID_RESURRECTION, "TooltipText", strToolTip_Action, "btnAction" )
		
	-- 귀혼주 x, 선도불가지역 x, 귀혼불가지역 x
	elseif 	true == tbCondition[ "bCanRecoveryExp" ] and
			false == tbCondition[ "bGuildBattleMap" ] then
			
		g_nResurrection_Action = g_nResurrection_Type_EXP
		
		local fDecReExpRatio = ( nDecReExp / g_MyChar:GetLevelUpExp() ) * 100

		strSub = string.format( GetGameInternalText( "REBIRTH_WINDOW_NOTIFY_RECOVERY_EXP", 0 ),
			CurrencyFormat( tbRecovery[ "nDecExp" ] ), fDecExpRatio, 
			CurrencyFormat( nDecReExp ), fDecReExpRatio )
			
		g_funcResurrection_Action = Resurrection_DoActionEXP
	
		-- 부활타입, 란머니 소모량, 경험치 복구량
		local strToolTip_Action = string.format( GetGameInternalText("REBIRTH_WINDOW_INFORMATION_RECOVERY_EXP"), 
			GetGameWordText( "AUTO_REBIRTH_ACTION", 0 ), 
			CurrencyFormat( tbRecovery["nMoney"] ), 
			CurrencyFormat( tbRecovery["nReExp"] ) .. " (" .. tostring( tbRecovery["nRecoveryPercent"]) .. "%)"  )
		
		SetString( WIDGET_ID_RESURRECTION, "TooltipText", strToolTip_Action, "btnAction" )
		
	else
		
		g_nResurrection_Action = g_nResurrection_Type_CheckPoint
		strSub = GetGameInternalText( "REBIRTH_WINDOW_NOTIFY_CHECK_POS_REBIRTH", 0 )
		g_funcResurrection_Action = Resurrection_DoActionCheckPoint
		
		-- 버튼 툴팁
		SetString( WIDGET_ID_RESURRECTION, "TooltipText", "", "btnAction" )	
		
	end
	
	if true == tbCondition[ "bCantUseResurrect" ] and g_bResurrection_Skill == false then
		SetBool( WIDGET_ID_RESURRECTION, "visible", false, "btnAction" )
	else
		SetBool( WIDGET_ID_RESURRECTION, "visible", true, "btnAction" )
	end
	
	
	-- 서브 텍스트 설정
	strSub = HTMLFontColor( strSub, HTML_TEXT_COLOR_ORANGE )
	SetString( WIDGET_ID_RESURRECTION, "htmlText", strSub, "labelSub" )
	
	-- 액션 버튼 설정
	SetString( WIDGET_ID_RESURRECTION, "label", 
		GetGameWordText( "REBIRTH_BUTTON", g_nResurrection_Action ), "btnAction" )
	SetEvent( WIDGET_ID_RESURRECTION, EVENT_BUTTON_CLICK, "Ev_Resurrection_ActionTimer", "", "btnAction" )

end

-- 텍스트 설정
function Resurrection_SetText()

	local strResurrect = ""
	if false == GetAutoResurrection() then
	
		strResurrect = GetGameWordText( "AUTO_REBIRTH_ACTION", g_nResurrection_Action )
	
	else
	
		strResurrect = GetGameWordText( "AUTO_REBIRTH_ACTION", 0 )
	
	end
	
	local strMain = string.format( GetGameInternalText( "REBIRTH_WINDOW_QUESTION", 0 ), 
		g_nResurrection_Time, strResurrect )
		
	SetString( WIDGET_ID_RESURRECTION, "text", strMain, "labelMain" )

end

-- 타이머 설정
function Resurrection_SetTimer()

	local tbQuickSet = 
	{
		time = 1000,
		delay = 0,
		ease = EASE_NONE,
		loop = false,
		onComplete = "Ev_Resurrection_ElpaseTime"
	}
	SetTween( WIDGET_ID_RESURRECTION, tbQuickSet, "", "", "" )

end

-- 체크 포인트 부활
function Resurrection_DoActionCheckPoint()

	g_Msg:ResurrectCheckPoint()
	CloseWidget( WIDGET_ID_RESURRECTION )
	
end

-- 귀혼주 부활
function Resurrection_DoActionCurPos()

	g_Msg:ResurrectCurPos()
	CloseWidget( WIDGET_ID_RESURRECTION )
	
end

-- 경험치 회복
function Resurrection_DoActionEXP()

	g_Msg:ResurrectExp()
	CloseWidget( WIDGET_ID_RESURRECTION )
	
end

-- 스킬 부활
function Resurrection_DoActionSkill()

	g_Msg:ResurrectSkill()
	CloseWidget( WIDGET_ID_RESURRECTION )
	
end

--------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 캐릭터 부활 정보 수신
function Resurrection_ReceiveInfo( bResurrectionSkill, fResurrectTime )

	-- 시간이 0 이면 defaultchar 데이터에서 시간 초기화
	if 0 == fResurrectTime then
		g_nResurrection_Time = g_nAUTO_REBIRTH_TIMER
	else
		g_nResurrection_Time = fResurrectTime
	end
	
	-- 스킬로 부활?
	g_bResurrection_Skill = bResurrectionSkill
	
	-- 부활 액션 대기시간 초기화
	g_nResurrection_ActionTime = 5
	
	if false == IsOpenWidget( WIDGET_ID_RESURRECTION ) then
	
		OpenWidget( WIDGET_ID_RESURRECTION )
		return
		
	end
	
	-- 정보갱신;
	OpenResurrection()
end

--------------------------------------------------------------------------------------------
-- 이벤트

-- 타이머 시간 흐름
function Ev_Resurrection_ElpaseTime()

	-- 1초씩 빼주기
	g_nResurrection_Time = g_nResurrection_Time - 1
	
	if 0 < g_nResurrection_Time then
	
		-- 타이머 설정
		Resurrection_SetTimer()
		
	-- 타이머 끝났으니 부활 요청
	else
		
		-- 자동 부활하기?
		if false == GetAutoResurrection() then
			-- 액션 타입에 맞게 부활 요청
			if g_nResurrection_Type_CheckPoint == g_nResurrection_Action then
				Resurrection_DoActionCheckPoint()
			elseif g_nResurrection_Type_CurPos == g_nResurrection_Action then
				Resurrection_DoActionCurPos()
			elseif g_nResurrection_Type_EXP == g_nResurrection_Action then
				Resurrection_DoActionEXP()
			elseif g_nResurrection_Type_Skill == g_nResurrection_Action then
				Resurrection_DoActionSkill()
			end
			
		else
		
			Resurrection_DoActionCheckPoint()
			
		end
	
	end
	
	-- 텍스트 재설정
	Resurrection_SetText()

end

-- 부활하기 버튼 이벤트
function Ev_Resurrection_CheckPoint()

	g_funcResurrection_Action = Resurrection_DoActionCheckPoint
	
	Ev_Resurrection_ActionTimer()

end

-- 스킬 부활하기
function Ev_Resurrection_Skill()

	g_funcResurrection_Action = Resurrection_DoActionSkill
	
	Ev_Resurrection_ActionTimer()

end

-- 부활 액션 타이머
function Ev_Resurrection_ActionTimer()

	-- 잠시만 기다려 주십시오. x초
	local strMain = GetGameInternalText( "WAITSERVER_MESSAGE", 0 ) .. "\n" ..
		tostring( g_nResurrection_ActionTime ) .. GetGameWordText( "WAITSERVER_TIMELEFT_UNIT", 0 )
	
	SetString( WIDGET_ID_RESURRECTION, "text", strMain, "labelMain" )
	SetString( WIDGET_ID_RESURRECTION, "text", "", "labelSub" )
	
	SetBool( WIDGET_ID_RESURRECTION, "enabled", false, "btnResurrection" )
	SetBool( WIDGET_ID_RESURRECTION, "enabled", false, "btnAction" )

	local tbQuickSet = 
	{
		time = 1000,
		delay = 0,
		ease = EASE_NONE,
		loop = false,
		onComplete = "Ev_Resurrection_ActionTimerUpdate"
	}
	SetTween( WIDGET_ID_RESURRECTION, tbQuickSet, "", "", "" )

end

-- 부활 액션 타이머 업데이트
function Ev_Resurrection_ActionTimerUpdate()

	g_nResurrection_ActionTime = g_nResurrection_ActionTime - 1
	
	if 0 < g_nResurrection_ActionTime then
	
		Ev_Resurrection_ActionTimer()
		
	else
	
		if nil ~= g_funcResurrection_Action then
			g_funcResurrection_Action()
		end
	
	end

end

-- 자동 부활 체크 옵션 설정
function Ev_Resurrtion_checkAuto()

	local bCheck = GetBool( WIDGET_ID_RESURRECTION, "selected", "checkAuto" )
	SetAutoResurrection( bCheck )
	Resurrection_SetText()
end

-- 광고 아이템 구입
function Ev_Resurrection_BuyADItem()

	g_tbNpcTalk_AD = g_nRESURRECT_RECOMMEND_ITEM_SEARCHSTRING
	OpenWidget( WIDGET_ID_POINTSHOP )

end