ITEM_PREVIEW_SLOT_MAX = 6

function LoadItemPreview()

	local strParam = CreateParam( "ItemPreview_RT_MC", "ItemPreview_RT", EMWIDGET_TEXTURE_ITEM_PREVIEW )
	CallFunction( WIDGET_ID_ITEM_PREVIEW, "CreateTexture", strParam, "Preview" )
	SetEvent( WIDGET_ID_ITEM_PREVIEW, EVENT_MOUSE_WHEEL, "Ev_ItemPreview_MoveViewZ", "delta", "Preview" )
	
	SetEvent( WIDGET_ID_ITEM_PREVIEW, EVENT_MOUSE_DOWN, "Ev_ItemPreview_TurnLeft", "", "TurnLeft" )
	SetEvent( WIDGET_ID_ITEM_PREVIEW, EVENT_MOUSE_UP, "Ev_ItemPreview_StopToTurn", "", "TurnLeft" )
	SetEvent( WIDGET_ID_ITEM_PREVIEW, EVENT_MOUSE_ROLLOUT, "Ev_ItemPreview_StopToTurn", "", "TurnLeft" )
	
	SetEvent( WIDGET_ID_ITEM_PREVIEW, EVENT_MOUSE_DOWN, "Ev_ItemPreview_TurnRight", "", "TruunRight" )
	SetEvent( WIDGET_ID_ITEM_PREVIEW, EVENT_MOUSE_UP, "Ev_ItemPreview_StopToTurn", "", "TruunRight" )
	SetEvent( WIDGET_ID_ITEM_PREVIEW, EVENT_MOUSE_ROLLOUT, "Ev_ItemPreview_StopToTurn", "", "TruunRight" )
	
	SetString( WIDGET_ID_ITEM_PREVIEW, "binding", SLOT_ITEM_PREVIEW_ID .. "1", "slotHat" )
	SetString( WIDGET_ID_ITEM_PREVIEW, "binding", SLOT_ITEM_PREVIEW_ID .. "2", "slotTop" )
	SetString( WIDGET_ID_ITEM_PREVIEW, "binding", SLOT_ITEM_PREVIEW_ID .. "3", "slotBottom" )
	SetString( WIDGET_ID_ITEM_PREVIEW, "binding", SLOT_ITEM_PREVIEW_ID .. "4", "slotGloves" )
	SetString( WIDGET_ID_ITEM_PREVIEW, "binding", SLOT_ITEM_PREVIEW_ID .. "5", "slotShoes" )
	SetString( WIDGET_ID_ITEM_PREVIEW, "binding", SLOT_ITEM_PREVIEW_ID .. "6", "slotWeapon" )
	SetString( WIDGET_ID_ITEM_PREVIEW, "text", GetGameWordText( "ITEMPREVIEW_TEXTBOX", 0 ), "textHat" )
	SetString( WIDGET_ID_ITEM_PREVIEW, "text", GetGameWordText( "ITEMPREVIEW_TEXTBOX", 1 ), "textTop" )
	SetString( WIDGET_ID_ITEM_PREVIEW, "text", GetGameWordText( "ITEMPREVIEW_TEXTBOX", 2 ), "textBottom" )
	SetString( WIDGET_ID_ITEM_PREVIEW, "text", GetGameWordText( "ITEMPREVIEW_TEXTBOX", 3 ), "textGloves" )
	SetString( WIDGET_ID_ITEM_PREVIEW, "text", GetGameWordText( "ITEMPREVIEW_TEXTBOX", 4 ), "textShoes" )
	SetString( WIDGET_ID_ITEM_PREVIEW, "text", GetGameWordText( "ITEMPREVIEW_TEXTBOX", 5 ), "textWeapon" )
	
	SetBool( WIDGET_ID_ITEM_PREVIEW, "holdable", false, "slotHat" )
	SetBool( WIDGET_ID_ITEM_PREVIEW, "holdable", false, "slotTop" )
	SetBool( WIDGET_ID_ITEM_PREVIEW, "holdable", false, "slotBottom" )
	SetBool( WIDGET_ID_ITEM_PREVIEW, "holdable", false, "slotGloves" )
	SetBool( WIDGET_ID_ITEM_PREVIEW, "holdable", false, "slotShoes" )
	SetBool( WIDGET_ID_ITEM_PREVIEW, "holdable", false, "slotWeapon" )
	
	g_ItemPreview:SetPreItem()
	
	-- 마우스 휠 뷰 이동 최소/최대값
	g_ItemPreview.m_fMinViewPosZ = -35.0
	g_ItemPreview.m_fMaxViewPosZ = -15.0
	
end

-- 뷰 이동
function Ev_ItemPreview_MoveViewZ( fDelta )

	g_ItemPreview:MoveViewPosZ( fDelta )

end

-- 왼쪽 회전
function Ev_ItemPreview_TurnLeft()

	g_ItemPreview:SetTurnLeft( true )
	
end

-- 오른쪽 회전
function Ev_ItemPreview_TurnRight()

	g_ItemPreview:SetTurnRight( true )
	
end

-- 회전 끝
function Ev_ItemPreview_StopToTurn()

	g_ItemPreview:SetTurnLeft( false )
	g_ItemPreview:SetTurnRight( false )
	
end

-- 열기
function ItemPreiview_Open()

	if false == IsOpenWidget( WIDGET_ID_ITEM_PREVIEW ) then
	
		-- 새로 열기 전에 슬롯 초기화
		for i = 1, ITEM_PREVIEW_SLOT_MAX, 1 do

			ResetSlot( SLOT_ITEM_PREVIEW_ID .. tostring( i ) )
			
		end
	
		OpenWidget( WIDGET_ID_ITEM_PREVIEW )
		
	end
	
end

-- 미리보기 아이템 설정
function ItemPreview_SetSlot( tbItem )

	SetSlot_Item( SLOT_ITEM_PREVIEW_ID .. tostring( tbItem[ 1 ] + 1 ), tbItem[ 2 ], tbItem[ 3 ], 1 )

end