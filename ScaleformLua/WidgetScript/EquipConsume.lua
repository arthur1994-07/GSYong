
g_EquipConsume_Type = -1
g_EquipConsume_Count = 0

function LoadEquipConsume()

	SetEvent( WIDGET_ID_EQUIPCONSUME, EVENT_WIDGET_CLOSE, "CloseEquipConsume", "", "" )
	
end

function OpenEquipConsume()

	if -1 == g_EquipConsume_Type then
	
		EquipConsume_Check()
	
	else
	
		EquipConsume_Update( g_EquipConsume_Count, g_EquipConsume_Type )
	
	end

	EquipConsume_UIPosition()

end

function CloseEquipConsume()

	g_EquipConsume_Type = -1
	g_EquipConsume_Count = 0

end

function EquipConsume_UIPosition()

	if false == IsOpenWidget( WIDGET_ID_EQUIPCONSUME ) then
		return
	end

	-- �Ҹ� ���������� ���� ����ǥ�� UI �� �̴ϸ� �Ʒ��� ��ġ�ϵ��� �Ѵ�.
	local bMiniMapFold = false
	if true == IsOpenWidget( WIDGET_ID_MINIMAP ) then
		bMiniMapFold = GetBool( WIDGET_ID_MINIMAP, "visible", "bgFold" )
	end

	local nPosX = GetInt( WIDGET_ID_MINIMAP, "x", "" )
	local nPosY = GetInt( WIDGET_ID_MINIMAP, "y", "" )
	
	local nHeight = 0
	if true == bMiniMapFold then
		nHeight = GetInt( WIDGET_ID_MINIMAP, "height", "bgFold" )
	else
		nHeight = GetInt( WIDGET_ID_MINIMAP, "height", "bg" )
	end
	
	SetInt( WIDGET_ID_EQUIPCONSUME, "x", nPosX, "" )
	SetInt( WIDGET_ID_EQUIPCONSUME, "y", nHeight + nPosY, "" )

end

-------------------------------------------------------------------------------------------
-- Ŭ���̾�Ʈ ��

-- ��� ���� Ȯ���ϰ� �Ҹ�ǰ ���������� UI ����
function EquipConsume_Check()

	local tbItem = g_MyChar:GetEquipConsume()
	
	-- ������ �ƴϴ�.
	if -1 == tbItem[ 1 ] and -1 == tbItem[ 2 ] then
	
		CloseWidget( WIDGET_ID_EQUIPCONSUME )
	
	else
	
		EquipConsume_Update( tbItem[ 1 ], tbItem[ 2 ] )
	
	end

end

-- �Ҹ�ǰ ���� ������Ʈ
function EquipConsume_Update( nCount, nType )

	if 0 == nCount then
	
		CloseWidget( WIDGET_ID_EQUIPCONSUME )
		return
		
	end

	g_EquipConsume_Type = nType
	g_EquipConsume_Count = nCount
	
	if false == IsOpenWidget( WIDGET_ID_EQUIPCONSUME ) then
	
		OpenWidget( WIDGET_ID_EQUIPCONSUME )
		return
		
	end

	local str = ""
	if ITEMTYPE_ARROW == nType then
	
		str = GetGameWordText( "ARROW_TYPE", 0 ) .. " : " .. nCount
		
	elseif ITEMTYPE_CHARM == nType then
	
		str = GetGameWordText( "ARROW_TYPE", 1 ) .. " : " .. nCount
		
	elseif ITEMTYPE_BULLET == nType then
	
		str = GetGameWordText( "ARROW_TYPE", 2 ) .. " : " .. nCount
		
	end
	
	SetString( WIDGET_ID_EQUIPCONSUME, "text", str, "PutOnCountText" )
	
end