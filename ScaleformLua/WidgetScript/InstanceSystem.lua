g_CooperationInfo_nTime 			= 0
g_CooperationInfo_fElapsedTime 		= 0

g_CooperationInfo_bInstanceSystemUI = false

g_CooperationInfo_bStageState = false

g_CooperationInfo_bEntranceState = false
g_CooperationInfo_bStageIndicator = false
g_CooperationInfo_bStage = false
g_CooperationInfo_bMediumText = false
g_CooperationInfo_bBigText = false
g_CooperationInfo_bSmallText = false

PVE_STAGE_STATE_ONGOING		= 0         -- 진행중
PVE_STAGE_STATE_TIMEOVER	= 1         -- 스테이지 타임 오버
PVE_STAGE_STATE_STAGECLEAR	= 2         -- 스테이지 클리어
PVE_STAGE_STATE_RETRY		= 3         -- 재도전 상태
PVE_STAGE_STATE_ALLDIED		= 4         -- 전부 죽음
PVE_STAGE_STATE_DIED		= 5         -- 자신 죽음

PVE_STAGE_STATE_GAMEROVER_FIREPARTY = 6 -- 게임 종료 : 파티 종료
PVE_STAGE_STATE_GAMEROVER_TIMEOVER	= 7 -- 게임 종료 : 인던 시간 종료
PVE_STAGE_STATE_GAMEROVER_NOTENOUGH	= 8 -- 게임 종료 : 재도전 실패 - 돈 부족
PVE_STAGE_STATE_GAMEROVER_WAITTIME	= 9 -- 게임 종료 : 재도전 실패 - 대기 시간 오버
PVE_STAGE_STATE_GAMEROVER_NOTTRY	= 10-- 게임 종료 : 재도전 실패 - 파티원 중 포기
PVE_STAGE_STATE_GAMEROVER_COMPLETED = 11-- 게임 종료 : 모든 스테이지 완료
PVE_STAGE_STATE_GAMEROVER_FAILED	= 12-- 게임 종료 : 기타
PVE_STAGE_STATE_GAMEROVER_RETRY		= 13-- 게임 종료 : 기타

-- UI 가 Open 되면 실행되어져야할 Callback Function Queue
Queue_UI_CallbackFunc = {}

g_nInstanceSys_SmallGap = 0
g_nInstanceSys_MediumGap = 0
g_nInstanceSys_BigGap = 0

-- Completed Load Instance System UI
function LoadInstanceSystemUI()

	-- Widget Close Event 등록;
	SetEvent( WIDGET_ID_INSTANCE_SYSTEM_DISP, EVENT_WIDGET_CLOSE, "Ev_PVE_WidgetClose", "", "" )
	
	SetBool( WIDGET_ID_INSTANCE_SYSTEM_DISP, "visible", false, "bgSmall" )
	SetBool( WIDGET_ID_INSTANCE_SYSTEM_DISP, "visible", false, "bgMedium" )
	SetBool( WIDGET_ID_INSTANCE_SYSTEM_DISP, "visible", false, "bgBig" )
	
	g_nInstanceSys_SmallGap = GetInt( WIDGET_ID_INSTANCE_SYSTEM_DISP, "width", "bgSmall" ) - 
		GetInt( WIDGET_ID_INSTANCE_SYSTEM_DISP, "width", "SmallText" )
		
	g_nInstanceSys_MediumGap = GetInt( WIDGET_ID_INSTANCE_SYSTEM_DISP, "width", "bgMedium" ) - 
		GetInt( WIDGET_ID_INSTANCE_SYSTEM_DISP, "width", "MediumText" )
		
	g_nInstanceSys_BigGap = GetInt( WIDGET_ID_INSTANCE_SYSTEM_DISP, "height", "bgBig" ) - 
		GetInt( WIDGET_ID_INSTANCE_SYSTEM_DISP, "height", "BigText" )
	
end

function CloseInstanceSystemUI()
	
	g_CooperationInfo_bInstanceSystemUI = false
	
end

-- Completed Open Instance System UI
function OpenInstanceSystemUI()
	-- 모든 Instance System UI 의 Data 를 초기화한다;
	ResetInstanceSystemUI()
	
	-- FrameMove 를 등록한다;
	InsertFrameMove( FrameMove_CooperationInfo )
	
	-- 초기화 완료를 알린다;
	g_CooperationInfo_bInstanceSystemUI = true
	
	-- 마지막에 실행되어야 한다;
	-- Callback 실행;
	for key, value in pairs ( Queue_UI_CallbackFunc ) do
		if ( value.paramCount == 0 ) then
			value.func()
		elseif ( value.paramCount == 1 ) then
			value.func( value.param[1] )
		elseif ( value.paramCount == 2 ) then
			value.func( value.param[1], value.param[2] )
		elseif ( value.paramCount == 3 ) then
			value.func( value.param[1], value.param[2], value.param[3] )
		elseif ( value.paramCount == 4 ) then
			value.func( value.param[1], value.param[2], value.param[3], value.param[4] )
		elseif ( value.paramCount == 5 ) then
			value.func( value.param[1], value.param[2], value.param[3], value.param[4], value.param[5] )
		else
		end
	end
	
	-- Callback Function 초기화;
	for key, value in pairs ( Queue_UI_CallbackFunc ) do
		Queue_UI_CallbackFunc[key] = nil
	end
	
end

function PVESetEntranceState(nCur, nAll, nSec, bShow)
	-- Instance System UI Open 이 완료되지 않았다면;
	if ( false == g_CooperationInfo_bInstanceSystemUI ) then
		
		UI_CallbackFunc = {}
		UI_CallbackFunc.paramCount = 4
		UI_CallbackFunc.func = PVESetEntranceState
		UI_CallbackFunc.param = {}
		table.insert( UI_CallbackFunc.param, nCur )
		table.insert( UI_CallbackFunc.param, nAll )
		table.insert( UI_CallbackFunc.param, nSec )
		table.insert( UI_CallbackFunc.param, bShow )
		
		Queue_UI_CallbackFunc[ "PVESetEntranceState" ] = UI_CallbackFunc
		return
	end
	
	-- 실행이 되었기 때문에 기존에 등록된 Callback을 제거한다;
	Queue_UI_CallbackFunc[ "PVESetEntranceState" ] = nil
	
	-- 메시지 방식이 아니기 때문에.. Lua에서 컨트롤 한다.
	if g_CooperationInfo_bEntranceState == false and bShow == true then
		ResetInstanceSystemUI()
		-- UI가 비활성화고 최초로 켜지는 경우
		g_CooperationInfo_bEntranceState = true

		SetBool( WIDGET_ID_INSTANCE_SYSTEM_DISP, "visible", true, "ProgressBar")
		PVESetTimer(nSec)
		
	elseif g_CooperationInfo_bEntranceState == true and bShow == false then
		-- UI가 활성화 상태이며 끄려고 하는경우
		g_CooperationInfo_bEntranceState = false
		PVESetSmallText("")
	end

	if bShow == true then
		PVESetSmallText(string.format(GetGameWordText("COMPETITION_PVE", 6 ), nCur, nAll))
	end
end

-- 스테이지 텍스트 설정
function PVESetStage(nStage, bShow)
	-- Instance System UI Open 이 완료되지 않았다면;
	if ( false == g_CooperationInfo_bInstanceSystemUI ) then
		
		UI_CallbackFunc = {}
		UI_CallbackFunc.paramCount = 2
		UI_CallbackFunc.func = PVESetStage
		UI_CallbackFunc.param = {}
		table.insert( UI_CallbackFunc.param, nStage )
		table.insert( UI_CallbackFunc.param, bShow )
		
		Queue_UI_CallbackFunc[ "PVESetStage" ] = UI_CallbackFunc
		return
	end
	
	-- 실행이 되었기 때문에 기존에 등록된 Callback을 제거한다;
	Queue_UI_CallbackFunc[ "PVESetStage" ] = nil
	
	-- 메시지 방식이 아니기 때문에.. Lua에서 컨트롤 한다.
	if g_CooperationInfo_bStage == false and bShow == true then
		-- UI가 비활성화고 최초로 켜지는 경우
		g_CooperationInfo_bStage = true
		
		
		ordinal = nStage % 10
		 
		-- 예외 처리
		if nStage == 11 or nStage == 12 or nStage == 13 or 4 <= ordinal then
			ordinal = 0
		end
		
		PVESetMediumText("")
		
	elseif g_CooperationInfo_bStage == true and bShow == false then
		-- UI가 활성화 상태이며 끄려고 하는경우
		g_CooperationInfo_bStage = false
	end
	
end

function PVESetMessage(szText, bShow)
	-- Instance System UI Open 이 완료되지 않았다면;
	if ( false == g_CooperationInfo_bInstanceSystemUI ) then
		
		UI_CallbackFunc = {}
		UI_CallbackFunc.paramCount = 2
		UI_CallbackFunc.func = PVESetMessage
		UI_CallbackFunc.param = {}
		table.insert( UI_CallbackFunc.param, szText )
		table.insert( UI_CallbackFunc.param, bShow )
		
		Queue_UI_CallbackFunc[ "PVESetMessage" ] = UI_CallbackFunc
		return
	end
	
	-- 실행이 되었기 때문에 기존에 등록된 Callback을 제거한다;
	Queue_UI_CallbackFunc[ "PVESetMessage" ] = nil

	-- 큰 텍스트 표시
	if g_CooperationInfo_bBigText == false and bShow == true then
		g_CooperationInfo_bBigText = true
		
		PVESetBigText(GetGameInternalText(szText))
		
	elseif g_CooperationInfo_bBigText == true and bShow == false then
		g_CooperationInfo_bBigText = false
		
		PVESetBigText("")
		
	end
end

function PVEMonsterIndicator(nCur, nAll, bShow)
	-- Instance System UI Open 이 완료되지 않았다면;
	if ( false == g_CooperationInfo_bInstanceSystemUI ) then
		
		UI_CallbackFunc = {}
		UI_CallbackFunc.paramCount = 3
		UI_CallbackFunc.func = PVEMonsterIndicator
		UI_CallbackFunc.param = {}
		table.insert( UI_CallbackFunc.param, nCur )
		table.insert( UI_CallbackFunc.param, nAll )
		table.insert( UI_CallbackFunc.param, bShow )
		
		Queue_UI_CallbackFunc[ "PVEMonsterIndicator" ] = UI_CallbackFunc
		return
	end
	
	-- 실행이 되었기 때문에 기존에 등록된 Callback을 제거한다;
	Queue_UI_CallbackFunc[ "PVEMonsterIndicator" ] = nil

	-- 작은 텍스트 표시
	if g_CooperationInfo_bSmallText == false and bShow == true then
		g_CooperationInfo_bSmallText = true

	elseif g_CooperationInfo_bSmallText == true and bShow == false then
		g_CooperationInfo_bSmallText = false
		
		PVESetSmallText("")
	end
	
	
	if bShow == true then
		PVESetSmallText(string.format(GetGameWordText("COMPETITION_PVE", 15 ), nCur, nAll))
	end

end

function PVEPositioningIndicator(nX, nY, nNow, nNeed, bShow)
	-- Instance System UI Open 이 완료되지 않았다면;
	if ( false == g_CooperationInfo_bInstanceSystemUI ) then
		
		UI_CallbackFunc = {}
		UI_CallbackFunc.paramCount = 5
		UI_CallbackFunc.func = PVEPositioningIndicator
		UI_CallbackFunc.param = {}
		table.insert( UI_CallbackFunc.param, nX )
		table.insert( UI_CallbackFunc.param, nY )
		table.insert( UI_CallbackFunc.param, nNow )
		table.insert( UI_CallbackFunc.param, nNeed )
		table.insert( UI_CallbackFunc.param, bShow )
		
		Queue_UI_CallbackFunc[ "PVEPositioningIndicator" ] = UI_CallbackFunc
		return
	end
	
	-- 실행이 되었기 때문에 기존에 등록된 Callback을 제거한다;
	Queue_UI_CallbackFunc[ "PVEPositioningIndicator" ] = nil
	
	-- 작은 텍스트 표시
	if g_CooperationInfo_bSmallText == false and bShow == true then
		g_CooperationInfo_bSmallText = true
		
		--sc::string::format( ID2GAMEWORD(_T("COMPETITION_PVE"), 25), nX, nY, nNow, nNeed );
		--PVESetSmallText(StringFormat(GetGameWordText("COMPETITION_PVE", 25 ), nX, nY, nNow, nNeed))
	elseif g_CooperationInfo_bSmallText == true and bShow == false then
		g_CooperationInfo_bSmallText = false
		PVESetSmallText("")
	end
	
	if bShow == true then
		PVESetSmallText(StringFormat(GetGameWordText("COMPETITION_PVE", 25 ), nX, nY, nNow, nNeed))
	end
	
end

function PVESetStageIndicator(nStage, nEnd, nSec, bBlank, bShow)
	-- Instance System UI Open 이 완료되지 않았다면;
	if ( false == g_CooperationInfo_bInstanceSystemUI ) then
		
		UI_CallbackFunc = {}
		UI_CallbackFunc.paramCount = 5
		UI_CallbackFunc.func = PVESetStageIndicator
		UI_CallbackFunc.param = {}
		table.insert( UI_CallbackFunc.param, nStage )
		table.insert( UI_CallbackFunc.param, nEnd )
		table.insert( UI_CallbackFunc.param, nSec )
		table.insert( UI_CallbackFunc.param, bBlank )
		table.insert( UI_CallbackFunc.param, bShow )
		
		Queue_UI_CallbackFunc[ "PVESetStageIndicator" ] = UI_CallbackFunc
		return
	end
	
	-- 실행이 되었기 때문에 기존에 등록된 Callback을 제거한다;
	Queue_UI_CallbackFunc[ "PVESetStageIndicator" ] = nil
	
	-- 우선순위인 UI가 존재한다.
	if g_CooperationInfo_bStageState == true then
		return
	end
	
	-- 메시지 방식이 아니기 때문에.. Lua에서 컨트롤 한다.
	if g_CooperationInfo_bStageIndicator == false and bShow == true then
		
		--ResetInstanceSystemUI()
		-- UI가 비활성화고 최초로 켜지는 경우
		g_CooperationInfo_bStageIndicator = true
		g_CooperationInfo_fElapsedTime = 0
		
		SetBool( WIDGET_ID_INSTANCE_SYSTEM_DISP, "visible", true, "ProgressBar")
		PVESetTimer(nSec)
		
		if ( nEnd > 0 ) then
			SetBool( WIDGET_ID_INSTANCE_SYSTEM_DISP, "visible", true, "StageDisp" )
			
			nBefore = nStage - 1
			nNow = nStage
			nLast = nEnd
				
			if nBefore < 1 then
				nBefore = 1
				CallFunction( WIDGET_ID_INSTANCE_SYSTEM_DISP, "gotoAndStop", "Normal"		, "StageDisp.Before" )
			else
				CallFunction( WIDGET_ID_INSTANCE_SYSTEM_DISP, "gotoAndStop", "Clear"		, "StageDisp.Before" )
			end
			
			CallFunction( WIDGET_ID_INSTANCE_SYSTEM_DISP, "gotoAndStop", "Now"		, "StageDisp.Now" )
			CallFunction( WIDGET_ID_INSTANCE_SYSTEM_DISP, "gotoAndStop", "Last"			, "StageDisp.Last" )
		
			SetString( WIDGET_ID_INSTANCE_SYSTEM_DISP, "text", tostring(nBefore)	, "StageDisp.Before.TextBox")
			SetString( WIDGET_ID_INSTANCE_SYSTEM_DISP, "text", tostring(nNow)		, "StageDisp.Now.TextBox")	
			SetString( WIDGET_ID_INSTANCE_SYSTEM_DISP, "text", tostring(nLast)	, "StageDisp.Last.TextBox")
			SetBool( WIDGET_ID_INSTANCE_SYSTEM_DISP, "visible", true, "ProgressBar")		
			SetString( WIDGET_ID_INSTANCE_SYSTEM_DISP, "visible", true, "TimeText" )
			SetBool( WIDGET_ID_INSTANCE_SYSTEM_DISP, "visible", true, "StageDisp")
		end	
		
	elseif g_CooperationInfo_bStageIndicator == true and bShow == false then
		-- UI가 활성화 상태이며 끄려고 하는경우
		
		g_CooperationInfo_bStageIndicator = false
		
		
		SetBool( WIDGET_ID_INSTANCE_SYSTEM_DISP, "visible", false, "ProgressBar")		
		SetString( WIDGET_ID_INSTANCE_SYSTEM_DISP, "visible", false, "TimeText" )
		SetBool( WIDGET_ID_INSTANCE_SYSTEM_DISP, "visible", false, "StageDisp" )

	end
end

function PVESetStageState(nState, bShow)
	-- Instance System UI Open 이 완료되지 않았다면;
	if ( false == g_CooperationInfo_bInstanceSystemUI ) then
		
		UI_CallbackFunc = {}
		UI_CallbackFunc.paramCount = 2
		UI_CallbackFunc.func = PVESetStageState
		UI_CallbackFunc.param = {}
		table.insert( UI_CallbackFunc.param, nState )
		table.insert( UI_CallbackFunc.param, bShow )
		
		Queue_UI_CallbackFunc[ "PVESetStageState" ] = UI_CallbackFunc
		return
	end
	
	-- 실행이 되었기 때문에 기존에 등록된 Callback을 제거한다;
	Queue_UI_CallbackFunc[ "PVESetStageState" ] = nil
	
	if g_CooperationInfo_bStageState == false and bShow == true then
		g_CooperationInfo_bStageState = true
	elseif g_CooperationInfo_bStageState == true and bShow == false then
		g_CooperationInfo_bStageState = false
		PVESetMediumText("")
	end
	if bShow == true then
		if nState == PVE_STAGE_STATE_STAGECLEAR or nState == PVE_STAGE_STATE_GAMEROVER_COMPLETED then
			ResetInstanceSystemUI()
			PVESetMediumText(GetGameWordText("COMPETITION_PVE", 12))
		elseif nState == PVE_STAGE_STATE_TIMEOVER then
			ResetInstanceSystemUI()
			PVESetMediumText(GetGameWordText("COMPETITION_PVE", 13))
		elseif nState == PVE_STAGE_STATE_ONGOING or nState == PVE_STAGE_STATE_RETRY then
		elseif nState == PVE_STAGE_STATE_ALLDIED or nState == PVE_STAGE_STATE_DIED then
			ResetInstanceSystemUI()
			PVESetMediumText(GetGameWordText("COMPETITION_PVE", 14))
		else
--		else
--			ResetInstanceSystemUI()
--			PVESetMediumText(GetGameWordText("COMPETITION_PVE", 14))
		end
	end
end

function SetNewInstanceRetry ( llMoney, nTime, bShow )

	if true == bShow and 0 < nTime then
	
		local strMsg = string.format(GetGameInternalText("COMPETITION_PVE_RETRY_TEXT", 0), llMoney )
		OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, strMsg, 
			Ev_Competition_Retray_MsgBox_btnOk, Ev_Competition_Retray_MsgBox_btnCancel,
			nil, nil, 
			nTime, Ev_Competition_Retray_MsgBox_btnCancel )
		
	else
	
		Event_MsgBox_Close()
		
	end
	
end


-- ///////////////////////////////////////////////////////////////////////////////////////////////
-- Event /////////////////////////////////////////////////////////////////////////////////////////
-- ///////////////////////////////////////////////////////////////////////////////////////////////

-- New Instance 재도전 확인;
function Ev_Competition_Retray_MsgBox_btnOk()
	g_Msg:ReqNewInstanceRetry(true)
	Event_MsgBox_Close()
end

-- New Instance 재도전 취소;
function Ev_Competition_Retray_MsgBox_btnCancel()
	Event_MsgBox_Close()
	g_Msg:ReqNewInstanceRetry(false)	
end

function Event_InfinityCooperation_Retray_MsgBox_btnOk()
	g_Msg:ReqPVERetry(true)
	Event_MsgBox_Close()
end

function Event_InfinityCooperation_Retray_MsgBox_btnCancel()
	Event_MsgBox_Close()
	g_Msg:ReqPVERetry(false)	
end

-- Instance System UI 가 닫힐 때의 Event
function Ev_PVE_WidgetClose()
	RemoveFrameMove( FrameMove_CooperationInfo )
end


-- ///////////////////////////////////////////////////////////////////////////////////////////////
-- ///////////////////////////////////////////////////////////////////////////////////////////////
-- ///////////////////////////////////////////////////////////////////////////////////////////////


-- FrameMove
function FrameMove_CooperationInfo( fElapsedTime )

	-- 다른 UI의 요소가 켜져있는경우 창을 열자

	if IsOpenWidget( WIDGET_ID_INSTANCE_SYSTEM_DISP ) == false then
		if ( g_CooperationInfo_bEntranceState == true ) or ( g_CooperationInfo_bStageIndicator == true ) then
			OpenWidget( WIDGET_ID_INSTANCE_SYSTEM_DISP )
		else
			CloseWidget( WIDGET_ID_INSTANCE_SYSTEM_DISP )
		end
		return
	end

	g_CooperationInfo_fElapsedTime = g_CooperationInfo_fElapsedTime + fElapsedTime
		
	-- 타이머 돌리기
	if 0 < g_CooperationInfo_nTime then
	
		local nRemainTime =	g_CooperationInfo_nTime - g_CooperationInfo_fElapsedTime;
		if nRemainTime < 0 then
			nRemainTime=0;
		end
		
		local strTemp = string.format("%02d:%02d", nRemainTime/60, nRemainTime%60 )
		SetString( WIDGET_ID_INSTANCE_SYSTEM_DISP, "text", strTemp, "TimeText" )
	end
	
end

-- Reset Instance System UI
function ResetInstanceSystemUI()
	g_CooperationInfo_nStage =0
	g_CooperationInfo_nEnd =0
	g_CooperationInfo_nTime =0
	g_CooperationInfo_fElapsedTime =0
	
	g_CooperationInfo_bEntranceState = false
	g_CooperationInfo_bStageIndicator = false
	g_CooperationInfo_bStage = false
	g_CooperationInfo_bBigText = false
	g_CooperationInfo_bSmallText = false
	
	PVESetTimer(0)
	PVESetMediumText("")
	PVESetSmallText("")
	PVESetBigText("")
	SetString( WIDGET_ID_INSTANCE_SYSTEM_DISP, "text", "", "TimeText" )
	SetBool( WIDGET_ID_INSTANCE_SYSTEM_DISP, "visible", false, "ProgressBar")
	SetBool( WIDGET_ID_INSTANCE_SYSTEM_DISP, "visible", false, "StageDisp" )

end


-- ///////////////////////////////////////////////////////////////////////////////////////////////
-- Wrapper ///////////////////////////////////////////////////////////////////////////////////////
-- ///////////////////////////////////////////////////////////////////////////////////////////////

function InstanceSys_SetTextBgSize( strTextInst, strBgInst, nGap, bWidth )

	local strPropW = ""
	local strPropTW = ""
	local strPropX = ""
	if true == bWidth then
	
		strPropW = "width"
		strPropTW = "textWidth"
		strPropX = "x"
	
	else
	
		strPropW = "height"
		strPropTW = "textHeight"
		strPropX = "y"
	
	end
	
	SetBool( WIDGET_ID_INSTANCE_SYSTEM_DISP, "visible", true, strBgInst )

	-- 사이즈 잡기 ------------------------------
	local nTextWidth = GetInt( WIDGET_ID_INSTANCE_SYSTEM_DISP, strPropTW, strTextInst ) + 4
	local nBgWidth = nTextWidth + nGap
	
	SetInt( WIDGET_ID_INSTANCE_SYSTEM_DISP, strPropW, nTextWidth, strTextInst )
	SetInt( WIDGET_ID_INSTANCE_SYSTEM_DISP, strPropW, nBgWidth, strBgInst )
	
	-- 화면 가운데로 정렬 ------------------------
	if true == bWidth then
	
		local nWidgetX = GetInt( WIDGET_ID_INSTANCE_SYSTEM_DISP, strPropX, "" )
		
		local nNewX = (GetScreenWidth() - nBgWidth) / 2 - nWidgetX
		SetInt( WIDGET_ID_INSTANCE_SYSTEM_DISP, strPropX, nNewX + nGap / 2, strTextInst )
		SetInt( WIDGET_ID_INSTANCE_SYSTEM_DISP, strPropX, nNewX, strBgInst )
	
	end

end


function PVESetMediumText( strMediumText )

	SetString( WIDGET_ID_INSTANCE_SYSTEM_DISP, "text", strMediumText, "MediumText" )
	
	if 0 == string.len( strMediumText ) then
	
		SetBool( WIDGET_ID_INSTANCE_SYSTEM_DISP, "visible", false, "bgMedium" )
	
	else
	
		InstanceSys_SetTextBgSize( "MediumText", "bgMedium", g_nInstanceSys_MediumGap, true )
	
	end
	
end

function PVESetSmallText( strSmallText )

	SetString( WIDGET_ID_INSTANCE_SYSTEM_DISP, "text", strSmallText, "SmallText" )
	
	if 0 == string.len( strSmallText ) then
	
		SetBool( WIDGET_ID_INSTANCE_SYSTEM_DISP, "visible", false, "bgSmall" )
	
	else
	
		InstanceSys_SetTextBgSize( "SmallText", "bgSmall", g_nInstanceSys_SmallGap, true )
		
	end
	
end

function PVESetBigText( strBigText )

	SetString( WIDGET_ID_INSTANCE_SYSTEM_DISP, "text", strBigText, "BigText" )
	
	if 0 == string.len( strBigText ) then
	
		SetBool( WIDGET_ID_INSTANCE_SYSTEM_DISP, "visible", false, "bgBig" )
	
	else
	
		InstanceSys_SetTextBgSize( "BigText", "bgBig", g_nInstanceSys_BigGap, false )
		
	end
	
end

function PVESetItemStageNum( nItemNum , nStageNum )
	SetString( WIDGET_ID_INSTANCE_SYSTEM_DISP, "text", tostring(nStageNum), "StageDisp.item"..nItemNum..".TextBox" )
end

function PVESetTimer( nLimiteTime )
	SetInt( WIDGET_ID_INSTANCE_SYSTEM_DISP, "value", 100, "ProgressBar" ) 
	g_CooperationInfo_nTime = nLimiteTime
	
	local tbQuickSet = 
	{
		-- 1/1000으로 변환
		time = nLimiteTime * 1000,
		delay = 0,
		ease = EASE_NONE,
		loop = false,
		onComplete = ""
	}
	
	SetTween( WIDGET_ID_INSTANCE_SYSTEM_DISP, tbQuickSet, "value", "0", "ProgressBar" )
end