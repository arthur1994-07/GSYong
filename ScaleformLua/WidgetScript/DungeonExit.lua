
g_nDungeonExit_OutTime = 0

-- 로드 완료
function LoadDungeonExit()

	SetEvent( WIDGET_ID_DUNGEONEXIT, EVENT_BUTTON_CLICK, "Ev_DungeonExit_Exit", "", "btnExit" )
	
	local str = GetGameWordText( "CTF_RESULT_BUTTON", 4 )
	SetString( WIDGET_ID_DUNGEONEXIT, "label", str, "btnExit" )

end

-- 오픈
function OpenDungeonExit()

	DungeonExit_SetTime()

end

-- 타이머 텍스트 설정
function DungeonExit_SetTime()

	local nMin = math.floor( g_nDungeonExit_OutTime / 60 )
	local nSec = g_nDungeonExit_OutTime % 60

	local strResult = GetGameWordText( "CTF_RESULT_TEXT", 0 ) .. " " .. 
		string.format( GetGameWordText( "CTF_RESULT_TEXT", 1 ), nMin, nSec ) .. ">"
	SetString( WIDGET_ID_DUNGEONEXIT, "text", strResult, "labelInfo" )
	
	if 0 == g_nDungeonExit_OutTime then
		return
	end
	
	local tbQuickSet = 
	{
		time = 1000,
		delay = 0,
		ease = EASE_NONE,
		loop = false,
		onComplete = "DungeonExit_SetTime"
	}
	SetTween( WIDGET_ID_DUNGEONEXIT, tbQuickSet, "", "", "" )
	
	g_nDungeonExit_OutTime = g_nDungeonExit_OutTime - 1

end

-------------------------------------------------------------------------------------------------
-- 클라이언트 콜

function DungeonExit_Show( fTime )

	g_nDungeonExit_OutTime = fTime
	
	OpenWidget( WIDGET_ID_DUNGEONEXIT )

end

-------------------------------------------------------------------------------------------------
-- 이벤트

-- 나가기
function Ev_DungeonExit_Exit()

	-- 인던 나가기 요청
	g_Msg:InstanceManRequestClose()

end