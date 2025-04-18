
-- 2개 에서 10개 까지 팝업 버튼 타입
POPUP_BUTTON_TYPE_10	= "BT10"
POPUP_BUTTON_TYPE_9		= "BT9"
POPUP_BUTTON_TYPE_8		= "BT8"
POPUP_BUTTON_TYPE_7		= "BT7"
POPUP_BUTTON_TYPE_6		= "BT6"
POPUP_BUTTON_TYPE_5		= "BT5"
POPUP_BUTTON_TYPE_4		= "BT4"
POPUP_BUTTON_TYPE_3		= "BT3"
POPUP_BUTTON_TYPE_2		= "BT2"
POPUP_BUTTON_TYPE_1		= "BT1"

g_strPopupButton_Type = ""	-- 팝업 버튼 타입
g_tbPopupButton_Label = {}  -- 팝업 버튼 텍스트
g_tbPopupButton_Event = {}  -- 팝업 버튼 이벤트

-- 로드 완료
function LoadPopupButton()
	
	-- 클릭시 창닫기
	InsertStageMouseClick( PopupButtonClose )
	
end

-- 오픈
function OpenPopupButton()

	PopupButton_Init()

end

-- 초기화
function PopupButton_Init()

	local nStringTableLen = table.getn( g_tbPopupButton_Label )
	local nEventTableLen = table.getn( g_tbPopupButton_Event )
	if 	0 == nStringTableLen or
		nStringTableLen ~= nEventTableLen then
		return
	end
	
	CallFunction( WIDGET_ID_POPUP_BUTTON, "gotoAndStop", g_strPopupButton_Type, "btns" )

	-- 위치 잡기
	local MousePos = GetMousePos()
	local MousePosX, MousePosY = PreventOutsideDrag( WIDGET_ID_POPUP_BUTTON, 
		MousePos[ "x" ] - 10, MousePos[ "y" ] - 10, 0, 0, 0, 0 )
	SetInt( WIDGET_ID_POPUP_BUTTON, "x", MousePosX, "" )
	SetInt( WIDGET_ID_POPUP_BUTTON, "y", MousePosY, "" )
	
	for value in pairs( g_tbPopupButton_Label ) do
	
		SetString( WIDGET_ID_POPUP_BUTTON, "label", g_tbPopupButton_Label[ value ], "btns.btn" .. value )
		SetEvent( WIDGET_ID_POPUP_BUTTON, EVENT_BUTTON_CLICK, g_tbPopupButton_Event[ value ], "", "btns.btn" .. value )
	
	end
	
end

-- 팝업 버튼 리스트 오픈
function PopupButton_Open( strType, tbString, tbEvent )
	
	-- 팝업 버튼 타입
	g_strPopupButton_Type = strType
	-- 팝업 버튼 텍스트
	g_tbPopupButton_Label = tbString
	-- 팝업 버튼 이벤트
	g_tbPopupButton_Event = tbEvent
	
	if false == IsOpenWidget( WIDGET_ID_POPUP_BUTTON ) then
		OpenWidget( WIDGET_ID_POPUP_BUTTON )
	else
		PopupButton_Init()
	end

end

---------------------------------------------------------------------------------------------------------------
-- 이벤트

function PopupButtonClose()
	
	CloseWidget( WIDGET_ID_POPUP_BUTTON )
	
end