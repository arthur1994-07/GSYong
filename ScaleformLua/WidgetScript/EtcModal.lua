
-- 상호관계 UI를 통해 탑승 요청이 옴;
function Modal_VehicleWith ( szOwnerName )

	local strMsg = StringFormat( GetGameInternalText( "BIKE_TAR", 0 ), szOwnerName )
	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, strMsg, Ev_EtcModal_VehicleWith_OK, Ev_EtcModal_VehicleWith_Cancel )

end

function Ev_EtcModal_VehicleWith_OK()
	g_Client:VehicleWithAns(true)
	Event_MsgBox_Close()
end

function Ev_EtcModal_VehicleWith_Cancel()
	g_Client:VehicleWithAns(false)
	Event_MsgBox_Close()
end