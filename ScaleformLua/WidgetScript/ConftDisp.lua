
g_nConftDisp_Index = -10

-- 대련 디스플레이 로드 완료
function LoadConftDisp()

	HitTestEnable( WIDGET_ID_CONFT_DISP, false, "conft" )
	
	if -10 == g_nConftDisp_Index then
	
		SetBool( WIDGET_ID_CONFT_DISP, "visible", false, "conft" )
		
	else
	
		ConftDisplay_MoveFrame( g_nConftDisp_Index )
	
	end

end

-- 출력할 이미지로 프레임 이동
function ConftDisplay_MoveFrame( nIndex )

	local strParam = ""

	if -1 == nIndex then
	
		strParam = "WIN"
	
	elseif -2 == nIndex then
	
		strParam = "LOSE"
	
	elseif -3 == nIndex then
	
		strParam = "DRAW"
	
	elseif 0 == nIndex then
	
		strParam = "FIGHT"
	
	else
	
		strParam = "COUNT" .. tostring( nIndex )
	
	end
	
	CallFunction( WIDGET_ID_CONFT_DISP, "gotoAndPlay", strParam, "conft" )
	
	SetBool( WIDGET_ID_CONFT_DISP, "visible", true, "conft" )
	
	local tbQuickSet = 
	{
		time = 1000,
		delay = 0,
		ease = EASE_NONE,
		loop = false,
		onComplete = "Ev_ConftDisp_EndDisp"
	}
	SetTween( WIDGET_ID_CONFT_DISP, tbQuickSet, "", "", "conft" )
	
	ConftDisp_UIPosition( GetScreenWidth() )

end

-- 위치 잡기
function ConftDisp_UIPosition( nScreenWidth )

	if false == IsOpenWidget( WIDGET_ID_CONFT_DISP ) then
		return
	end

	local nWidth = GetInt( WIDGET_ID_CONFT_DISP, "width", "conft" )
	
	SetInt( WIDGET_ID_CONFT_DISP, "x", ( nScreenWidth - nWidth ) / 2, "" )
	SetInt( WIDGET_ID_CONFT_DISP, "y", 100, "" )

end

------------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

function ConftDisplay( nIndex )

	g_nConftDisp_Index = nIndex
	
	if false == IsOpenWidget( WIDGET_ID_CONFT_DISP ) then
	
		OpenWidget( WIDGET_ID_CONFT_DISP )
		
	else
	
		ConftDisplay_MoveFrame( g_nConftDisp_Index )
	
	end

end

------------------------------------------------------------------------------------------------------------
-- 이벤트

-- 디스플레이 끄기
function Ev_ConftDisp_EndDisp()

	SetBool( WIDGET_ID_CONFT_DISP, "visible", false, "conft" )

end