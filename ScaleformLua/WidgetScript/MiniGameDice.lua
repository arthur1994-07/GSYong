-- WIDGET_ID_MINIGAME_DICE 

EMODDEVEN_STAGE_READY = 0;				-- 레디 상태
EMODDEVEN_STAGE_SELECT = 1;				-- 선택 상태
EMODDEVEN_STAGE_ROLL = 2;				-- 주사위가 구르는 상태
EMODDEVEN_STAGE_RESULT = 3;				-- 결과 상태
EMODDEVEN_STAGE_SELECT_REWARD = 4;		-- 보상선택 상태
EMODDEVEN_STAGE_RECEIVE_REWARD = 5;		-- 보상 받기 상태
-- Ran\RanLogic\MiniGame\OddEven\GLOddEvenDefine.h;

indexRewardSelected = -1

g_bMiniGameDice_ReserveClose = false
g_bMiniGameDice_FailedOpen = false


-- 미니게임-주사위 실행시
function OpenMiniGameDice()

	if true == g_bMiniGameDice_ReserveClose then
	
		MiniGameDice_Close()
	
		--g_Msg:ReqCloseMinigame()
		
		g_bMiniGameDice_ReserveClose = false
		
		return
		
	end
	
	local strCreateTextureParam = "MiniGame_Dice_RT_MC;MiniGame_Dice_RT;" .. tostring( EMWIDGET_TEXTURE_DICE ) .. ";"
	CallFunction( WIDGET_ID_MINIGAME_DICE, "CreateTexture", strCreateTextureParam, "mcMain.mcScene.rt" )
	
	TurnStageMiniGameDiceReady()
	
	-- 주사위창을 끌 수 없도록
	SetBool( WIDGET_ID_MINIGAME_DICE, "visible", false, "closeBtn" )
	
	SetEvent( WIDGET_ID_MINIGAME_DICE, EVENT_MOUSE_CLICK, "EventMiniGameRewardGoldClick0", "", "mcMain.mcScene.mcGoldReward0" )
	SetEvent( WIDGET_ID_MINIGAME_DICE, EVENT_MOUSE_OVER, "EventMiniGameRewardGoldOver0", "", "mcMain.mcScene.mcGoldReward0" )
	SetEvent( WIDGET_ID_MINIGAME_DICE, EVENT_MOUSE_OUT, "EventMiniGameRewardGoldOut0", "", "mcMain.mcScene.mcGoldReward0" )
	
	SetEvent( WIDGET_ID_MINIGAME_DICE, EVENT_MOUSE_CLICK, "EventMiniGameRewardGoldClick1", "", "mcMain.mcScene.mcGoldReward1" )
	SetEvent( WIDGET_ID_MINIGAME_DICE, EVENT_MOUSE_OVER, "EventMiniGameRewardGoldOver1", "", "mcMain.mcScene.mcGoldReward1" )
	SetEvent( WIDGET_ID_MINIGAME_DICE, EVENT_MOUSE_OUT, "EventMiniGameRewardGoldOut1", "", "mcMain.mcScene.mcGoldReward1" )
	
	SetEvent( WIDGET_ID_MINIGAME_DICE, EVENT_MOUSE_CLICK, "EventMiniGameRewardGoldClick2", "", "mcMain.mcScene.mcGoldReward2" )
	SetEvent( WIDGET_ID_MINIGAME_DICE, EVENT_MOUSE_OVER, "EventMiniGameRewardGoldOver2", "", "mcMain.mcScene.mcGoldReward2" )
	SetEvent( WIDGET_ID_MINIGAME_DICE, EVENT_MOUSE_OUT, "EventMiniGameRewardGoldOut2", "", "mcMain.mcScene.mcGoldReward2" )
	
	SetEvent( WIDGET_ID_MINIGAME_DICE, EVENT_MOUSE_CLICK, "EventMiniGameRewardGoldClick3", "", "mcMain.mcScene.mcGoldReward3" )	
	SetEvent( WIDGET_ID_MINIGAME_DICE, EVENT_MOUSE_OVER, "EventMiniGameRewardGoldOver3", "", "mcMain.mcScene.mcGoldReward3" )
	SetEvent( WIDGET_ID_MINIGAME_DICE, EVENT_MOUSE_OUT, "EventMiniGameRewardGoldOut3", "", "mcMain.mcScene.mcGoldReward3" )
	
	SetEvent( WIDGET_ID_MINIGAME_DICE, EVENT_MOUSE_CLICK, "EventMiniGameRewardBronzeClick", "", "mcMain.mcScene.mcBronzeReward" )
	SetEvent( WIDGET_ID_MINIGAME_DICE, EVENT_MOUSE_OVER, "EventMiniGameRewardBronzeOver", "", "mcMain.mcScene.mcBronzeReward" )
	SetEvent( WIDGET_ID_MINIGAME_DICE, EVENT_MOUSE_OUT, "EventMiniGameRewardBronzeOut", "", "mcMain.mcScene.mcBronzeReward" )
	
	-- 승리 보상 슬롯 바인딩		
	for i = 1, 4, 1 do
	
		SetString( WIDGET_ID_MINIGAME_DICE, "binding", SLOT_MINIGAME_DICE_REWARD_ID .. tostring(i), 
			"mcMain.mcScene.mcGoldReward" .. tostring(i - 1) .. ".slot")
			
	end
	
	SetString( WIDGET_ID_MINIGAME_DICE, "binding", SLOT_MINIGAME_DICE_REWARD_ID .. "0", "mcMain.mcScene.mcBronzeReward.slot")
	
	SetBool( WIDGET_ID_MINIGAME_DICE, "visible", false, "mcMain.mcScene.barTimer" )
	
end

-- 미니게임-주사위 종료
function EventMiniGameDiceClose()

	g_Msg:ReqCloseMinigame();
	
end

-- 미니게임 주사위 준비상태
function TurnStageMiniGameDiceReady()

	local _strGuide;
	local _indexStepCurrent = g_ManagerMiniGame:getStepCurrent()
	local _sizeStepMaximum = g_ManagerMiniGame:getStepMaximum()
	local _moneyCost = g_ManagerMiniGame:getMoneyCost()
	
	if _indexStepCurrent == 0 then
	
		_strGuide = GetGameInternalText( "MINIGAME_ODDEVEN_PROCESS_BEGIN" ) .. 
			CurrencyFormat( _moneyCost ) .. GetGameInternalText( "MINIGAME_ODDEVEN_PROCESS_BEGIN_END" )
			
	elseif _indexStepCurrent == _sizeStepMaximum then	
	
		_strGuide = GetGameInternalText( "MINIGAME_ODDEVEN_PROCESS_END" )
		
	else
	
		_strGuide = GetGameInternalText( "MINIGAME_ODDEVEN_PROCESS_REBEGIN" ) ..
			CurrencyFormat( _moneyCost ) .. GetGameInternalText( "MINIGAME_ODDEVEN_PROCESS_BEGIN_END" )
		
	end
	
	SetString( WIDGET_ID_MINIGAME_DICE, "text", _strGuide, "mcMain.textFieldGuide" )
	
	SetString( WIDGET_ID_MINIGAME_DICE, "text", CurrencyFormat(g_MyChar:GetInvenMoney()), "mcMain.inputFieldMoney" )
	
	CallFunction( WIDGET_ID_MINIGAME_DICE, "gotoAndStop", "stage_ready", "mcMain.mcScene" )
	SetBool( WIDGET_ID_MINIGAME_DICE, "visible", true, "mcMain.mcScene.yes_btn" )
	SetBool( WIDGET_ID_MINIGAME_DICE, "visible", true, "mcMain.mcScene.no_btn" )
	SetString( WIDGET_ID_MINIGAME_DICE, "label", "Yes", "mcMain.mcScene.yes_btn" )
	SetString( WIDGET_ID_MINIGAME_DICE, "label", "No", "mcMain.mcScene.no_btn" )
	
	-- 보상받기 버튼 출력되지 않도록
	SetBool( WIDGET_ID_MINIGAME_DICE, "visible", false, "mcMain.mcScene.GetReward_TextBtn" )
	
	-- Yse/No 버튼 클릭 이벤트
	SetEvent( WIDGET_ID_MINIGAME_DICE, EVENT_BUTTON_CLICK, "EventMiniGameDiceYesClick", "", "mcMain.mcScene.yes_btn" )
	SetEvent( WIDGET_ID_MINIGAME_DICE, EVENT_BUTTON_CLICK, "EventMiniGameDiceClose", "", "mcMain.mcScene.no_btn" )
	
	-- 연승 횟수 출력
	local SuccessiveWin = string.format( GetGameWordText( "MINIGAME_ODDEVEN_TEXT", 10 ), _indexStepCurrent )
	local SuccessiveMax = string.format( GetGameWordText( "MINIGAME_ODDEVEN_TEXT", 9 ), _sizeStepMaximum )
	SetString( WIDGET_ID_MINIGAME_DICE, "text", SuccessiveWin .. SuccessiveMax, "mcMain.inputState2" )
	
	-- 홀짝 선택 정보란 비우기
	SetString( WIDGET_ID_MINIGAME_DICE, "text", "", "mcMain.inputState1" )
	
	SetBool( WIDGET_ID_MINIGAME_DICE, "visible", false, "mcMain.mcScene.mcGoldReward0" )
	SetBool( WIDGET_ID_MINIGAME_DICE, "visible", false, "mcMain.mcScene.mcGoldReward1" )
	SetBool( WIDGET_ID_MINIGAME_DICE, "visible", false, "mcMain.mcScene.mcGoldReward2" )
	SetBool( WIDGET_ID_MINIGAME_DICE, "visible", false, "mcMain.mcScene.mcGoldReward3" )
	SetBool( WIDGET_ID_MINIGAME_DICE, "visible", false, "mcMain.mcScene.mcBronzeReward" )
	
end

-- 홀/짝 선택
function TurnStageMiniGameDiceSelect()

	indexRewardSelected = -1
	SetString( WIDGET_ID_MINIGAME_DICE, "text", GetGameInternalText("MINIGAME_ODDEVEN_PROCESS_SELECT_ODDEVEN"), "mcMain.textFieldGuide" )
	SetEvent( WIDGET_ID_MINIGAME_DICE, EVENT_BUTTON_CLICK, "EventMiniGameSelectOddClick", "", "mcMain.mcScene.yes_btn" )
	SetEvent( WIDGET_ID_MINIGAME_DICE, EVENT_BUTTON_CLICK, "EventMiniGameSelectEvenClick", "", "mcMain.mcScene.no_btn" )
	
end

-- 승리시 함수
function TurnStageMiniGameDiceResultWin()

	CallFunction( WIDGET_ID_MINIGAME_DICE, "gotoAndStop", "stage_result_win", "mcMain.mcScene" )
	SetBool( WIDGET_ID_MINIGAME_DICE, "visible", false, "mcMain.mcScene.yes_btn" )
	SetBool( WIDGET_ID_MINIGAME_DICE, "visible", false, "mcMain.mcScene.no_btn" )
	
	local _indexStepCurrent = g_ManagerMiniGame:getStepCurrent()
	SetString( WIDGET_ID_MINIGAME_DICE, "text", StringFormatEx(GetGameInternalText("MINIGAME_ODDEVEN_PROCESS_REWARD_WIN"), _indexStepCurrent + 1), "mcMain.textFieldGuide" )
	
	SetBool( WIDGET_ID_MINIGAME_DICE, "visible", true, "mcMain.mcScene.mcGoldReward0" )
	SetBool( WIDGET_ID_MINIGAME_DICE, "visible", true, "mcMain.mcScene.mcGoldReward1" )
	SetBool( WIDGET_ID_MINIGAME_DICE, "visible", true, "mcMain.mcScene.mcGoldReward2" )
	SetBool( WIDGET_ID_MINIGAME_DICE, "visible", true, "mcMain.mcScene.mcGoldReward3" )
	
	CallFunction( WIDGET_ID_MINIGAME_DICE, "gotoAndStop", "boxIdle", "mcMain.mcScene.mcGoldReward0" )
	CallFunction( WIDGET_ID_MINIGAME_DICE, "gotoAndStop", "boxIdle", "mcMain.mcScene.mcGoldReward1" )
	CallFunction( WIDGET_ID_MINIGAME_DICE, "gotoAndStop", "boxIdle", "mcMain.mcScene.mcGoldReward2" )
	CallFunction( WIDGET_ID_MINIGAME_DICE, "gotoAndStop", "boxIdle", "mcMain.mcScene.mcGoldReward3" )
	
	local fLimitTime = g_ManagerMiniGame:getTimeLimit()
	SetDouble( WIDGET_ID_MINIGAME_DICE, "maximum", 100, "mcMain.mcScene.barTimer" )
	SetDouble( WIDGET_ID_MINIGAME_DICE, "minimum", 0, "mcMain.mcScene.barTimer" )
	SetDouble( WIDGET_ID_MINIGAME_DICE, "value", 100, "mcMain.mcScene.barTimer" )
	
	local tbQuickSet = 
	{
		time = fLimitTime * 1000,
		delay = 0,
		ease = EASE_NONE,
		loop = false,
		onComplete = "Ev_MinigameDice_TimeOverRewardWin"
	}
	SetTween( WIDGET_ID_MINIGAME_DICE, tbQuickSet, "value", "0", "mcMain.mcScene.barTimer" )
	SetBool( WIDGET_ID_MINIGAME_DICE, "visible", true, "mcMain.mcScene.barTimer" )
	
end

function Ev_MinigameDice_TimeOverRewardWin()

	local nRand = math.random( 0, 3 )

	if 0 == nRand then
		EventMiniGameRewardGoldClick0()
	elseif 1 == nRand then
		EventMiniGameRewardGoldClick1()
	elseif 2 == nRand then
		EventMiniGameRewardGoldClick2()
	elseif 3 == nRand then
		EventMiniGameRewardGoldClick3()
	end

end

-- 패배시 함수
function TurnStageMiniGameDiceResultLose()

	CallFunction( WIDGET_ID_MINIGAME_DICE, "gotoAndStop", "stage_result_lose", "mcMain.mcScene" )
	SetBool( WIDGET_ID_MINIGAME_DICE, "visible", false, "mcMain.mcScene.yes_btn" )
	SetBool( WIDGET_ID_MINIGAME_DICE, "visible", false, "mcMain.mcScene.no_btn" )
	
	SetString( WIDGET_ID_MINIGAME_DICE, "text", GetGameInternalText("MINIGAME_ODDEVEN_PROCESS_REWARD_LOSE"), "mcMain.textFieldGuide" )
	
	SetBool( WIDGET_ID_MINIGAME_DICE, "visible", true, "mcMain.mcScene.mcBronzeReward" )
	CallFunction( WIDGET_ID_MINIGAME_DICE, "gotoAndStop", "boxIdle", "mcMain.mcScene.mcBronzeReward" )
	
	local fLimitTime = g_ManagerMiniGame:getTimeLimit()
	SetDouble( WIDGET_ID_MINIGAME_DICE, "maximum", 100, "mcMain.mcScene.barTimer" )
	SetDouble( WIDGET_ID_MINIGAME_DICE, "minimum", 0, "mcMain.mcScene.barTimer" )
	SetDouble( WIDGET_ID_MINIGAME_DICE, "value", 100, "mcMain.mcScene.barTimer" )
	
	local tbQuickSet = 
	{
		time = fLimitTime * 1000,
		delay = 0,
		ease = EASE_NONE,
		loop = false,
		onComplete = "Ev_MinigameDice_TimeOverRewardLose"
	}
	SetTween( WIDGET_ID_MINIGAME_DICE, tbQuickSet, "value", "0", "mcMain.mcScene.barTimer" )
	SetBool( WIDGET_ID_MINIGAME_DICE, "visible", true, "mcMain.mcScene.barTimer" )
	
end

function Ev_MinigameDice_TimeOverRewardLose()

	EventMiniGameRewardBronzeClick()

end

-- 승리보상 함수
function TurnStageMiniGameDiceReceiveReward()

	if indexRewardSelected == 0 then
	
		CallFunction( WIDGET_ID_MINIGAME_DICE, "gotoAndPlay", "boxSelected", "mcMain.mcScene.mcGoldReward0" )
		CallFunction( WIDGET_ID_MINIGAME_DICE, "gotoAndPlay", "boxNonSelected", "mcMain.mcScene.mcGoldReward1" )
		CallFunction( WIDGET_ID_MINIGAME_DICE, "gotoAndPlay", "boxNonSelected", "mcMain.mcScene.mcGoldReward2" )
		CallFunction( WIDGET_ID_MINIGAME_DICE, "gotoAndPlay", "boxNonSelected", "mcMain.mcScene.mcGoldReward3" )
		
	elseif indexRewardSelected == 1 then
	
		CallFunction( WIDGET_ID_MINIGAME_DICE, "gotoAndPlay", "boxNonSelected", "mcMain.mcScene.mcGoldReward0" )
		CallFunction( WIDGET_ID_MINIGAME_DICE, "gotoAndPlay", "boxSelected", "mcMain.mcScene.mcGoldReward1" )
		CallFunction( WIDGET_ID_MINIGAME_DICE, "gotoAndPlay", "boxNonSelected", "mcMain.mcScene.mcGoldReward2" )
		CallFunction( WIDGET_ID_MINIGAME_DICE, "gotoAndPlay", "boxNonSelected", "mcMain.mcScene.mcGoldReward3" )
		
	elseif indexRewardSelected == 2 then
	
		CallFunction( WIDGET_ID_MINIGAME_DICE, "gotoAndPlay", "boxNonSelected", "mcMain.mcScene.mcGoldReward0" )
		CallFunction( WIDGET_ID_MINIGAME_DICE, "gotoAndPlay", "boxNonSelected", "mcMain.mcScene.mcGoldReward1" )
		CallFunction( WIDGET_ID_MINIGAME_DICE, "gotoAndPlay", "boxSelected", "mcMain.mcScene.mcGoldReward2" )
		CallFunction( WIDGET_ID_MINIGAME_DICE, "gotoAndPlay", "boxNonSelected", "mcMain.mcScene.mcGoldReward3" )
		
	elseif indexRewardSelected == 3 then
	
		CallFunction( WIDGET_ID_MINIGAME_DICE, "gotoAndPlay", "boxNonSelected", "mcMain.mcScene.mcGoldReward0" )
		CallFunction( WIDGET_ID_MINIGAME_DICE, "gotoAndPlay", "boxNonSelected", "mcMain.mcScene.mcGoldReward1" )
		CallFunction( WIDGET_ID_MINIGAME_DICE, "gotoAndPlay", "boxNonSelected", "mcMain.mcScene.mcGoldReward2" )
		CallFunction( WIDGET_ID_MINIGAME_DICE, "gotoAndPlay", "boxSelected", "mcMain.mcScene.mcGoldReward3" )
		
	elseif indexRewardSelected == 4 then
	
		CallFunction( WIDGET_ID_MINIGAME_DICE, "gotoAndPlay", "boxSelected", "mcMain.mcScene.mcBronzeReward" )
		
	end
	
	if indexRewardSelected == 4 then
	
		SetSlot_MiniGame_Dice( SLOT_MINIGAME_DICE_REWARD_ID .. "0", g_ManagerMiniGame:getItemIDReward(0), 0 )
		
	else
	
		for i = 1, 4, 1 do
		
			SetSlot_MiniGame_Dice( SLOT_MINIGAME_DICE_REWARD_ID .. tostring(i), g_ManagerMiniGame:getItemIDReward(i - 1), i )
			
		end
		
	end
	
	-- 보상선택후 스테이지 클릭시 화면전환
	SetEvent( WIDGET_ID_MINIGAME_DICE, EVENT_MOUSE_CLICK, "EventMiniGameRewardClickCallback", "", "")
	
	local tbQuickSet = 
	{
		time = 0,
		delay = 0,
		ease = EASE_NONE,
		loop = false,
		onComplete = ""
	}
	SetTween( WIDGET_ID_MINIGAME_DICE, tbQuickSet, "value", "0", "mcMain.mcScene.barTimer" )
	--SetBool( WIDGET_ID_MINIGAME_DICE, "visible", false, "mcMain.mcScene.barTimer" )
	
end

-- 스테이지 넘어갈때 호출
function TurnStageMiniGameDice()	

	local stage = g_ManagerMiniGame:getStageCurrent()
	
	SetBool( WIDGET_ID_MINIGAME_DICE, "visible", false, "mcMain.mcScene.barTimer" )
	
	if stage == EMODDEVEN_STAGE_READY then	
	
		TurnStageMiniGameDiceReady()
		
	elseif stage == EMODDEVEN_STAGE_SELECT then
	
		TurnStageMiniGameDiceSelect()
		
		SetString( WIDGET_ID_MINIGAME_DICE, "text", CurrencyFormat(g_MyChar:GetInvenMoney()), "mcMain.inputFieldMoney" )
		
	elseif stage == EMODDEVEN_STAGE_SELECT_REWARD then
	
		if g_ManagerMiniGame:isWin() == true then
			TurnStageMiniGameDiceResultWin()
		else
			TurnStageMiniGameDiceResultLose()
		end
		
	elseif stage == EMODDEVEN_STAGE_RECEIVE_REWARD then
	
		TurnStageMiniGameDiceReceiveReward()
		
	end
	
end

function MiniGameDice_CheckMoney()

	if g_MyChar:GetInvenMoney() < g_ManagerMiniGame:getMoneyCost() then
	
		local str = GetGameInternalText( "MINIGAME_ODDEVEN_NOT_ENOUGH_MONEY", 0 )
		CF_PrintMsg_SystemMsg( str, TEXT_COLOR_RED )
			
		return false
		
	end
	
	return true

end

-- 열기
function MiniGameDice_Open()

	if ( false == IsOpenWidget( WIDGET_ID_MINIGAME_DICE ) ) and
		( false == g_bMiniGameDice_ReserveClose ) and
		( MiniGameDice_CheckMoney() ) then
	
		OpenWidget( WIDGET_ID_MINIGAME_DICE )
		
		
		g_bMiniGameDice_FailedOpen = false
		
	else
	
		g_Msg:ReqCloseMinigame()
		
		g_bMiniGameDice_FailedOpen = true
	
	end

end

-- 닫기
function MiniGameDice_Close()

	if ( false == IsOpenWidget( WIDGET_ID_MINIGAME_DICE ) ) and
		( false == g_bMiniGameDice_FailedOpen ) then
	
		
		g_bMiniGameDice_ReserveClose = true
		
	else
	
		
		g_bMiniGameDice_ReserveClose = false
		CloseWidget( WIDGET_ID_MINIGAME_DICE ) 
		
	end

end

---------------------------------------------------------------------------------------------------
-- 이벤트

-- 미니게임 시작 버튼 클릭(Yes 버튼 클릭)
function EventMiniGameDiceYesClick()	

	-- 현재 스테이지 정보
	local stage = g_ManagerMiniGame:getStageCurrent()
	
	-- 준비 상태
	if stage == EMODDEVEN_STAGE_READY then
	
		if false == MiniGameDice_CheckMoney() then
			
			g_Msg:ReqCloseMinigame()
			
			return
			
		end
	
		CallFunction( WIDGET_ID_MINIGAME_DICE, "gotoAndStop", "stage_select", "mcMain.mcScene" )
		SetBool( WIDGET_ID_MINIGAME_DICE, "visible", true, "mcMain.mcScene.yes_btn" )
		SetBool( WIDGET_ID_MINIGAME_DICE, "visible", true, "mcMain.mcScene.no_btn" )
		SetString( WIDGET_ID_MINIGAME_DICE, "label", "Odd", "mcMain.mcScene.yes_btn" )
		SetString( WIDGET_ID_MINIGAME_DICE, "label", "Even", "mcMain.mcScene.no_btn" )
	
		g_Msg:ReqBeginMinigame()
		
	end
	
end

function EventMiniGameSelectClick()

	CallFunction( WIDGET_ID_MINIGAME_DICE, "gotoAndStop", "stage_roll", "mcMain.mcScene" )
	SetBool( WIDGET_ID_MINIGAME_DICE, "visible", false, "mcMain.mcScene.yes_btn" )
	SetBool( WIDGET_ID_MINIGAME_DICE, "visible", false, "mcMain.mcScene.no_btn" )
	
end

function EventMiniGameSelectOddClick()

	EventMiniGameSelectClick()
	g_Msg:ReqSelectOddEvenMinigame(true)
	
	SetString( WIDGET_ID_MINIGAME_DICE, "text", GetGameWordText( "MINIGAME_ODDEVEN_TEXT", 7), "mcMain.inputState1" )
	
end

function EventMiniGameSelectEvenClick()

	EventMiniGameSelectClick()
	
	g_Msg:ReqSelectOddEvenMinigame(false)
	SetString( WIDGET_ID_MINIGAME_DICE, "text", GetGameWordText( "MINIGAME_ODDEVEN_TEXT", 8), "mcMain.inputState1" )
	
end

-- 최종 보상을 받기위하여 들어오는 함수
function EventMiniGameRewardClickCallback()	

	local stage = g_ManagerMiniGame:getStageCurrent()
	if stage == 5 then
	
		g_Msg:ReqReceiveRewardMinigame()
		
	end
	
end

-- 보상상자 마우스 좌 클릭시 최종적으로 들어오는 함수
function EventMiniGameRewardClick(indexBox)
	
	if indexRewardSelected ~= -1 then
		return
	end
	
	indexRewardSelected = indexBox
	
	if indexBox == 4 then
		g_Msg:ReqSelectRewardMinigame(0)
	else
		g_Msg:ReqSelectRewardMinigame(indexBox)
	end
	
end

-- 패배 보상 아이콘에 마우스 좌 클릭시
function EventMiniGameRewardBronzeClick()

	EventMiniGameRewardClick(4)
	
end

-- 패배 보상 아이콘에 마우스 오버시
function EventMiniGameRewardBronzeOver()
	
	if indexRewardSelected ~= -1 then
		return
	end
	
	CallFunction( WIDGET_ID_MINIGAME_DICE, "gotoAndPlay", "boxOver", "mcMain.mcScene.mcBronzeReward" )
	
end

-- 패배 보상 아이콘에 마우스 아웃시
function EventMiniGameRewardBronzeOut()

	if indexRewardSelected ~= -1 then
		return
	end
	
	CallFunction( WIDGET_ID_MINIGAME_DICE, "gotoAndPlay", "boxIdle", "mcMain.mcScene.mcBronzeReward" )
	
end

-- 승리 보상 첫번째 상자 클릭
function EventMiniGameRewardGoldClick0()

	EventMiniGameRewardClick(0)
	
end

-- 승리 보상 첫번째 상자 마우스 오버
function EventMiniGameRewardGoldOver0()

	if indexRewardSelected ~= -1 then
		return
	end
	
	CallFunction( WIDGET_ID_MINIGAME_DICE, "gotoAndStop", "boxOver", "mcMain.mcScene.mcGoldReward0" )
	
end

-- 승리 보상 첫번째 상자 마우스 오버 아웃
function EventMiniGameRewardGoldOut0()

	if indexRewardSelected ~= -1 then
		return
	end
	
	CallFunction( WIDGET_ID_MINIGAME_DICE, "gotoAndStop", "boxIdle", "mcMain.mcScene.mcGoldReward0" )
	
end

-- 승리 보상 두번째 상자 클릭
function EventMiniGameRewardGoldClick1()

	EventMiniGameRewardClick(1)
	
end

-- 승리 보상 두번째 상자 마우스 오버
function EventMiniGameRewardGoldOver1()

	if indexRewardSelected ~= -1 then
		return
	end
	
	CallFunction( WIDGET_ID_MINIGAME_DICE, "gotoAndStop", "boxOver", "mcMain.mcScene.mcGoldReward1" )
	
end

-- 승리 보상 두번째 상자 마우스 오버 아웃
function EventMiniGameRewardGoldOut1()

	if indexRewardSelected ~= -1 then
		return
	end
	
	CallFunction( WIDGET_ID_MINIGAME_DICE, "gotoAndStop", "boxIdle", "mcMain.mcScene.mcGoldReward1" )
	
end

-- 승리 보상 세번째 상자 클릭
function EventMiniGameRewardGoldClick2()

	EventMiniGameRewardClick(2)
	
end

-- 승리 보상 세번째 상자 마우스 오버
function EventMiniGameRewardGoldOver2()

	if indexRewardSelected ~= -1 then
		return
	end
	
	CallFunction( WIDGET_ID_MINIGAME_DICE, "gotoAndStop", "boxOver", "mcMain.mcScene.mcGoldReward2" )
	
end

-- 승리 보상 세번째 상자 마우스 오버 아웃
function EventMiniGameRewardGoldOut2()

	if indexRewardSelected ~= -1 then
		return
	end
	
	CallFunction( WIDGET_ID_MINIGAME_DICE, "gotoAndStop", "boxIdle", "mcMain.mcScene.mcGoldReward2" )
	
end

-- 승리 보상 네번째 상자 클릭
function EventMiniGameRewardGoldClick3()

	EventMiniGameRewardClick(3)
	
end

-- 승리 보상 네번째 상자 마우스 오버
function EventMiniGameRewardGoldOver3()

	if indexRewardSelected ~= -1 then
		return
	end
	
	CallFunction( WIDGET_ID_MINIGAME_DICE, "gotoAndStop", "boxOver", "mcMain.mcScene.mcGoldReward3" )
	
end

-- 승리 보상 네번째 상자 마우스 오버 아웃
function EventMiniGameRewardGoldOut3()

	if indexRewardSelected ~= -1 then
		return
	end
	
	CallFunction( WIDGET_ID_MINIGAME_DICE, "gotoAndStop", "boxIdle", "mcMain.mcScene.mcGoldReward3" )
	
end