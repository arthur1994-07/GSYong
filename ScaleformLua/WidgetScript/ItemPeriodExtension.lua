
-- 기능 연장
function PeriodExConfirm(wPE_HoldPosX, wPE_HoldPosY, wPE_InvenPosX, wPE_InvenPosY, strText )
	
	local tbData = { wPE_HoldPosX, wPE_HoldPosY, wPE_InvenPosX, wPE_InvenPosY }
	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, strText, Event_PE_Reconfirm, Event_MsgBox_Close, tbData )
	
end

function Event_PE_Confirm( tbData )
	
	g_Client:ReqPeriodExtension( tbData[ 1 ], tbData[ 2 ], tbData[ 3 ], tbData[ 4 ] )
	Event_MsgBox_Close()
	
end

-- 재확인 메세지 박스 출력;
function PeriodExReconfirmOpenMsgBox(wPE_HoldPosX, wPE_HoldPosY, wPE_InvenPosX, wPE_InvenPosY)
	
	local tbData = { wPE_HoldPosX, wPE_HoldPosY, wPE_InvenPosX, wPE_InvenPosY }
	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, GetGameInternalText( "ITEM_PERIODEX_CHECK_EXPIRATIONDATE" ), 
		Event_PE_Reconfirm, Event_MsgBox_Close, tbData )
	
end

function Event_PE_Reconfirm( tbData )

	g_Client:PeriodExtensionReconfirm( tbData[ 1 ], tbData[ 2 ], tbData[ 3 ], tbData[ 4 ] )
	Event_MsgBox_Close()
	
end