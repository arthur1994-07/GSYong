
function LoadProductConfirmMsgBox()
	-- Configure
	local strOK = GetGameWordText( "MODAL_BUTTON", 0 )
	local strCancel = GetGameWordText( "MODAL_BUTTON", 1 )
	SetString( WIDGET_ID_PRODUCTCONFIRM, "label", strOK, "msgBoxOKCancel.btnOK" )
	SetString( WIDGET_ID_PRODUCTCONFIRM, "label", strCancel, "msgBoxOKCancel.btnCancel" )
	
	-- 메시지 텍스트 설정
	SetString( WIDGET_ID_PRODUCTCONFIRM, "htmlText", GetGameInternalText( "PRODUCT_WINDOW_CONFIRM_MATERIAL_MSG", 0 ), "textField" )
	
	-- MSG_BOX_TYPE_OKCANCEL
	SetBool( WIDGET_ID_PRODUCTCONFIRM, "visible", false, "msgBoxOK" )
	SetBool( WIDGET_ID_PRODUCTCONFIRM, "visible", false, "msgBoxCancel" )
	SetBool( WIDGET_ID_PRODUCTCONFIRM, "visible", true, "msgBoxOKCancel" )
	SetBool( WIDGET_ID_PRODUCTCONFIRM, "visible", false, "msgBoxInput" )
	SetBool( WIDGET_ID_PRODUCTCONFIRM, "visible", false, "MB_progressBar")
	
	SetEvent( WIDGET_ID_PRODUCTCONFIRM, EVENT_BUTTON_CLICK, "Ev_ProductConfirmMsgBox_OK", "", "msgBoxOKCancel.btnOK" )
	SetEvent( WIDGET_ID_PRODUCTCONFIRM, EVENT_BUTTON_CLICK, "Ev_ProductConfirmMsgBox_Cancel", "", "msgBoxOKCancel.btnCancel" )
	
end

function OpenProductConfirmMsgBox()
	for i = 1, PRODUCT_MATERIAL_COUNT_TOTAL, 1 do
		Product_HighLight_ConfrimMsgBox(i)
	end
end

function Product_HighLight_ConfrimMsgBox( nIndex )

	local tbData = g_Product_tbProductData[ nIndex ]
	if nil == tbData then
		return
	end

	local tbItemID = SplitNativeID( tbData[ 1 ] )
	local tbItemPos = g_MyChar:FindInvenItemPos( tbItemID[1], tbItemID[2], 0 )
	if 2 ~= table.getn( tbItemPos ) then
		return
	end
	
	local nPageIndex = math.floor(tbItemPos[2]/g_nInvenPageYCount)+1
	Inventory_MovePage(nPageIndex)
		
	Inventory_HighlightItem( tbItemID[ 1 ], tbItemID[ 2 ], tbData[ 2 ] )
end 

function Ev_ProductConfirmMsgBox_OK()
	local dwKey = g_Product:GetSelectedProductKey( g_Product_wSelectedProduct )
	local nCount = tonumber( GetString( WIDGET_ID_PRODUCT, "text", "Goods.infoViewGroup.ProductCountEdit" ) )
	ResetProductNeedConfirm()
	ProductProgress_SetData(strConfirmItemName, nConfirmCount, fConfirmDelay)
	g_Msg:ReqProduct( dwKey, nCount, true )
	
	Inventory_SetVisibleMultiHighlight( false, -1, nil )
	CloseWidget ( WIDGET_ID_PRODUCTCONFIRM )
end

function Ev_ProductConfirmMsgBox_Cancel()
	g_Msg:ReqProductCancel()
	ResetProductNeedConfirm()
	CF_ProductEnd()
	
	Inventory_SetVisibleMultiHighlight( false, -1, nil )
	CloseWidget ( WIDGET_ID_PRODUCTCONFIRM )
end