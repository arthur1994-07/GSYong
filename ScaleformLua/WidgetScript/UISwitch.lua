

function LoadUISwitch()
	
	-- ����Ʈ ������ ����
	UISwitch_SetList()
	
	-- ����Ʈ Ŭ�� �̺�Ʈ ����
	SetEvent( WIDGET_ID_UISWITCH, EVENT_LIST_ITEM_CLICK, "Ev_UISwitch_SelectList", "index", "UISwitchList" )
	-- ��ü ���� ���� ��ư Ŭ�� �̺�Ʈ ����
	SetEvent( WIDGET_ID_UISWITCH, EVENT_MOUSE_CLICK, "Ev_UISwitch_UnCheckAllBtn", "", "UnCheckAllBtn" )
	-- ���� UI ��� ��ư Ŭ�� �̺�Ʈ ����
	SetEvent( WIDGET_ID_UISWITCH, EVENT_MOUSE_CLICK, "Ev_UISwitch_InnerUIOnBtn", "", "InnerUIOnBtn" )
	SetEvent( WIDGET_ID_UISWITCH, EVENT_MOUSE_CLICK, "Ev_UISwitch_InnerUIOffBtn", "", "InnerUIOffBtn" )
	
end

-- ����Ʈ ������ ������ ����
function UISwitch_SetList()

	local tbSwitchList = {}	-- ����Ʈ ���� ���̺�
	
	for value in pairs( WIDGETS_LIST ) do
	
		local tbInfo = WIDGETS_LIST[ value ]
		
		-- ������ �ִ� UI�� üũ���·� ����� ����
		local IsWidgetOpen = 0
		if IsOpenWidget( tbInfo[ "wWidgetID" ] ) == true then
			IsWidgetOpen = 1
		end
		
		-- ����Ʈ ������ ������ ����
		local tblist = 
		{
			"[" .. tbInfo[ "wWidgetID" ] .. "] " .. tbInfo[ "strDebuggingName" ],
			IsWidgetOpen
		}
		table.insert( tbSwitchList, tblist )
	
	end

	-- ������ ������ ������ ����( "�ؽ�Ʈ �ʵ�", "��Ƽ ����Ʈ ����" )
	local strParam = CreateParam( "label", "multiSelected" )
	SetDataEx( WIDGET_ID_UISWITCH, strParam, tbSwitchList, "UISwitchList", false )

end

----------------------------------------------------------------------------------------------------------------
-- �̺�Ʈ

-- ����Ʈ Ŭ�� �̺�Ʈ
function Ev_UISwitch_SelectList( nIndex )

	-- üũ���°� ����Ǳ� ���� ���� ����ȴ�.(Ŭ���ϱ� �� ������ üũ����)
	local bCheck = GetDataBool(WIDGET_ID_UISWITCH, nIndex, "multiSelected", "UISwitchList")
	
	local tbInfo = WIDGETS_LIST[ nIndex + 1 ]
	if nil == tbInfo then
		return
	end
	
	if false == bCheck then
	
		OpenWidget( tbInfo[ "wWidgetID" ] )
		
	else
	
		CloseWidget( tbInfo[ "wWidgetID" ] )
		
	end
	
end

-- ��üũ ��ư Ŭ�� �̺�Ʈ
function Ev_UISwitch_UnCheckAllBtn()

	for value in pairs( WIDGETS_LIST ) do
	
		local tbInfo = WIDGETS_LIST[ value ]
		CloseWidget( tbInfo[ "wWidgetID" ] )
		
	end
	
end

-- ���� UI On ��ư Ŭ�� �̺�Ʈ
function Ev_UISwitch_InnerUIOnBtn()
	
	-- �Է� �ؽ�Ʈ �߰�(�Է� �ؽ�Ʈ�� Ŀ��Ʈ�ܼ� â�� �������� �ʴ��� �Է��� �����ϴ�.)
	local strInputText = "dspname 1"
	AddChatMessage( CHAT_LOG_COMMAND_CONSOLE, CHAT_SYMBOL_NORMAL, false, strInputText, TEXT_COLOR_WHITE, {}, {} )	--Ŀ��Ʈ �ܼ�â�� reloadwidget 2 �ؽ�Ʈ ���
	
	-- Ŀ�ǵ� ���� �� ��� �ؽ�Ʈ �߰�
	local strResultText = DoCommand( strInputText )
	AddChatMessage( CHAT_LOG_COMMAND_CONSOLE, CHAT_SYMBOL_NORMAL, false, strResultText, TEXT_COLOR_RED, {}, {} )	--reloadwidget 2 ����
	
end

-- ���� UI Off ��ư Ŭ�� �̺�Ʈ
function Ev_UISwitch_InnerUIOffBtn()
	
	-- �Է� �ؽ�Ʈ �߰�(�Է� �ؽ�Ʈ�� Ŀ��Ʈ�ܼ� â�� �������� �ʴ��� �Է��� �����ϴ�.)
	local strInputText = "dspname 0"
	AddChatMessage( CHAT_LOG_COMMAND_CONSOLE, CHAT_SYMBOL_NORMAL, false, strInputText, TEXT_COLOR_WHITE, {}, {} )	--Ŀ��Ʈ �ܼ�â�� reloadwidget 2 �ؽ�Ʈ ���
	
	-- Ŀ�ǵ� ���� �� ��� �ؽ�Ʈ �߰�
	local strResultText = DoCommand( strInputText )
	AddChatMessage( CHAT_LOG_COMMAND_CONSOLE, CHAT_SYMBOL_NORMAL, false, strResultText, TEXT_COLOR_RED, {}, {} )	--reloadwidget 2 ����
	
end
