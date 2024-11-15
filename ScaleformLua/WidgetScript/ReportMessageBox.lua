  
-- 기존 메세지 박스와의 차이점
-- 1. 출력 후 지정한 시간동안 페이드 아웃되며 사라진다.
-- 2. 메세지 박스가 출력되어 있는 동안에도 다른 행동이 가능하다.
-- 3. 위젯이 visible off 되지 않고 종료된다.

g_FadeOut_Time 		= 5000 					-- 메세지 박스가 천천히 사라질때 사라지기 까지 시간(기본값 5000)
g_FadeOut_Delay 	= 500 					-- 메세지 박스가 천천히 사라질때 사라지는 속도(기본값 500)
g_FadeOut_Easing 	= "EASE_REGULAR_OUT"	-- 메세지 박스가 사라지는 타입 설정(기본값 EASE_REGULAR_OUT)

g_funcReportMsgBox_EnterKey = nil		-- 엔터키 이벤트
g_funcReportMsgBox_EscKey = nil			-- ESC키 이벤트

g_tbReportMsgBox_Info = nil
g_bReportBoxInputESC = false

-- 메시지 박스 로드 완료 
function LoadReportMessageBox()

	-- 키보드 이벤트
	SetEvent( WIDGET_ID_REPORTMESSAGEBOX, EVENT_KEY_UP, "Ev_ReportMsgBox_KeyUp", "keyCode", "" )
	
	local strOK = GetGameWordText( "MODAL_BUTTON", 0 )
	local strCancel = GetGameWordText( "MODAL_BUTTON", 1 )
	
	SetString( WIDGET_ID_REPORTMESSAGEBOX, "label", strOK, "msgBoxOK.btnOK" )		
	SetString( WIDGET_ID_REPORTMESSAGEBOX, "label", strCancel, "msgBoxCancel.btnCancel" )	
	SetString( WIDGET_ID_REPORTMESSAGEBOX, "label", strOK, "msgBoxOKCancel.btnOK" )
	SetString( WIDGET_ID_REPORTMESSAGEBOX, "label", strCancel, "msgBoxOKCancel.btnCancel" )
	SetString( WIDGET_ID_REPORTMESSAGEBOX, "label", strOK, "msgBoxInput.btnOK" )
	SetString( WIDGET_ID_REPORTMESSAGEBOX, "label", strCancel, "msgBoxInput.btnCancel" )
	
	SetBool( WIDGET_ID_REPORTMESSAGEBOX, "visible", false, "MB_progressBar")
	SetBool( WIDGET_ID_REPORTMESSAGEBOX, "visible", false, "closeBtn" )
	
	-- 프로그래스 숨기기
	SetBool( WIDGET_ID_REPORTMESSAGEBOX, "visible", false, "MB_progressBar" )
	
end


-- 알림 메시지 오픈
function OpenReportMessageBox()

	SetBool( WIDGET_ID_REPORTMESSAGEBOX, "visible", false, "" )

end

-- 타입 설정
function ReportMsgBox_SetType( nType )

	if MSG_BOX_TYPE_OK == nType then
	
		SetBool( WIDGET_ID_REPORTMESSAGEBOX, "visible", true, "msgBoxOK" )
		SetBool( WIDGET_ID_REPORTMESSAGEBOX, "visible", false, "msgBoxCancel" )
		SetBool( WIDGET_ID_REPORTMESSAGEBOX, "visible", false, "msgBoxOKCancel" )
		SetBool( WIDGET_ID_REPORTMESSAGEBOX, "visible", false, "msgBoxInput" )
	
	elseif MSG_BOX_TYPE_CANCEL == nType then
	
		SetBool( WIDGET_ID_REPORTMESSAGEBOX, "visible", false, "msgBoxOK" )
		SetBool( WIDGET_ID_REPORTMESSAGEBOX, "visible", true, "msgBoxCancel" )
		SetBool( WIDGET_ID_REPORTMESSAGEBOX, "visible", false, "msgBoxOKCancel" )
		SetBool( WIDGET_ID_REPORTMESSAGEBOX, "visible", false, "msgBoxInput" )
	
	elseif MSG_BOX_TYPE_OKCANCEL == nType then
	
		SetBool( WIDGET_ID_REPORTMESSAGEBOX, "visible", false, "msgBoxOK" )
		SetBool( WIDGET_ID_REPORTMESSAGEBOX, "visible", false, "msgBoxCancel" )
		SetBool( WIDGET_ID_REPORTMESSAGEBOX, "visible", true, "msgBoxOKCancel" )
		SetBool( WIDGET_ID_REPORTMESSAGEBOX, "visible", false, "msgBoxInput" )
	
	elseif MSG_BOX_TYPE_INPUT == nType then
	
		SetBool( WIDGET_ID_REPORTMESSAGEBOX, "visible", false, "msgBoxOK" )
		SetBool( WIDGET_ID_REPORTMESSAGEBOX, "visible", false, "msgBoxCancel" )
		SetBool( WIDGET_ID_REPORTMESSAGEBOX, "visible", false, "msgBoxOKCancel" )
		SetBool( WIDGET_ID_REPORTMESSAGEBOX, "visible", true, "msgBoxInput" )
	
	else
	
		SetBool( WIDGET_ID_REPORTMESSAGEBOX, "visible", false, "msgBoxOK" )
		SetBool( WIDGET_ID_REPORTMESSAGEBOX, "visible", false, "msgBoxCancel" )
		SetBool( WIDGET_ID_REPORTMESSAGEBOX, "visible", false, "msgBoxOKCancel" )
		SetBool( WIDGET_ID_REPORTMESSAGEBOX, "visible", false, "msgBoxInput" )
	
	end

end

-- 메시지 알림 박스 오픈
function OpenReportMsgBox( nType, strText, funcEnter, funcESC, fTime, fDelay, nEasing )
	
	ReportMsgBox_SetType( nType )
	
	g_funcReportMsgBox_EnterKey = funcEnter
	g_funcReportMsgBox_EscKey = funcESC
	
	SetBool( WIDGET_ID_REPORTMESSAGEBOX, "visible", true, "" )

	-- 페이드 아웃되기까지 시간, 딜레이, 타입값이 있을 경우 해당 값 입력
	if nil ~= fTime then
		g_FadeOut_Time = fTime
	end
	if nil ~= fDelay then
		g_FadeOut_Delay = fDelay
	end
	if nil ~= nEasing then
		g_FadeOut_Easing = nEasing
	end
	
	-- 메시지 텍스트 설정
	SetString( WIDGET_ID_REPORTMESSAGEBOX, "text", strText, "textField" )
	
	local strParam = CreateParam( "", 1, 1, g_FadeOut_Easing )
	SetEvent( WIDGET_ID_REPORTMESSAGEBOX, EVENT_MOUSE_ROLLOVER, "Event_MsgBox_FadeIn", "", "" , strParam )
	
	strParam = CreateParam( "Event_ReportMsgBox_Close", g_FadeOut_Time, g_FadeOut_Delay, g_FadeOut_Easing )
	SetEvent( WIDGET_ID_REPORTMESSAGEBOX, EVENT_MOUSE_ROLLOUT, "Event_MsgBox_FadeOut", "", "" , strParam )
	
end

-- 알림창 에서 메세지박스 오픈시
function Report_MessegeBox_Open( Invitation_Index, bOpenListBox )
		
	local tb_Invitation = g_Invitation[ tonumber( Invitation_Index ) ]
	
	-- 메세지 박스가 이미 켜져있을경우 더이상 메세지 박스를 출력하지 않는다.
	--if true == IsOpenWidget( WIDGET_ID_REPORTMESSAGEBOX ) then
	--	return
	--end
	-- 메시지 박스의 알파값이 0이 되었을 떄를 위하여 알파값 1로 설정
	SetInt( WIDGET_ID_REPORTMESSAGEBOX, "alpha", 1, "" )
	-- 메세지 박스 위젯 오픈 관련
	Event_ReportBox_Close() -- 기존 UI 와 같이 메세지 박스를 출력하면 알림창을 닫아준다.
	
	-- 친구 초대
	if tb_Invitation[ "dwType" ] == INVITATION_REPORT_FRIEND then
	
		local StrMessage = StringFormat( GetGameInternalText("ADD_FRIEND_REQ", 0), tb_Invitation["dwHostName"] )
		
		OpenReportMsgBox( MSG_BOX_TYPE_OKCANCEL, StrMessage , Report_MessegeBox_OKClick_Friend, Report_MessegeBox_NOClick_Friend)
		SetEvent( WIDGET_ID_REPORTMESSAGEBOX, EVENT_BUTTON_CLICK, "Report_MessegeBox_OKClick_Friend", "", "msgBoxOKCancel.btnOK" )
		SetEvent( WIDGET_ID_REPORTMESSAGEBOX, EVENT_BUTTON_CLICK, "Report_MessegeBox_NOClick_Friend", "", "msgBoxOKCancel.btnCancel" )
		
		g_tbReportMsgBox_Info = { Invitation_Index, tb_Invitation[ "dwHostName" ] }
		
	-- 그룹채팅 초대	
	elseif tb_Invitation[ "dwType" ] == INVITATION_REPORT_GROUPCHAT then
	
		GroupChat_InviteMsgRecv( tb_Invitation["dwHostInformation"] , Invitation_Index )
		
		g_tbReportMsgBox_Info = { Invitation_Index }
		
	-- 클럽 초대
	elseif tb_Invitation[ "dwType" ] == INVITATION_REPORT_CLUB then
	
		local tbHostInFor = tb_Invitation[ "dwHostInformation" ]
		
		local StrMessage = StringFormat( GetGameInternalText("CLUB_JOIN_ASK", 0), 
			tbHostInFor["dwClubName"], tb_Invitation["dwHostName"] )
		
		OpenReportMsgBox( MSG_BOX_TYPE_OKCANCEL, StrMessage , Report_MessegeBox_OKClick_Club, Report_MessegeBox_NOClick_Club)
		SetEvent( WIDGET_ID_REPORTMESSAGEBOX, EVENT_BUTTON_CLICK, "Report_MessegeBox_OKClick_Club", "", "msgBoxOKCancel.btnOK" )
		SetEvent( WIDGET_ID_REPORTMESSAGEBOX, EVENT_BUTTON_CLICK, "Report_MessegeBox_NOClick_Club", "", "msgBoxOKCancel.btnCancel" )
		
		g_tbReportMsgBox_Info = { Invitation_Index }
	
	-- 파티 초대
	elseif tb_Invitation[ "dwType" ] == INVITATION_REPORT_PARTY then
	
		local tbHostInFor = tb_Invitation[ "dwHostInformation" ]
	
		local StrMessage = StringFormat( GetGameInternalText("PARTY_TAR", 0), tb_Invitation["dwHostName"] )
		.. "\n"
		.. tbHostInFor["strPartyInfo"]
		
		OpenReportMsgBox( MSG_BOX_TYPE_OKCANCEL, StrMessage , Report_MessegeBox_OKClick_Party, Report_MessegeBox_NOClick_Party)
		SetEvent( WIDGET_ID_REPORTMESSAGEBOX, EVENT_BUTTON_CLICK, "Report_MessegeBox_OKClick_Party", "", "msgBoxOKCancel.btnOK" )
		SetEvent( WIDGET_ID_REPORTMESSAGEBOX, EVENT_BUTTON_CLICK, "Report_MessegeBox_NOClick_Party", "", "msgBoxOKCancel.btnCancel" )
		
		g_tbReportMsgBox_Info = { Invitation_Index }
		
	else
	
		g_tbReportMsgBox_Info = nil
		
	end
	
	Event_MsgBox_FadeOut( "Event_ReportMsgBox_Close", g_FadeOut_Time, g_FadeOut_Delay, g_FadeOut_Easing )
	
end

---------------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 메시지 박스 엔터키 단축키
function ReportMsgBox_Enter()

	if false == IsOpenWidget( WIDGET_ID_REPORTMESSAGEBOX ) then
		return false
	end
	
	if false == GetBool( WIDGET_ID_REPORTMESSAGEBOX, "visible", "" ) then
	
		return false
		
	else
	
		-- 엔터키 단축키 이벤트 호출
		if nil ~= g_funcReportMsgBox_EnterKey then
			g_funcReportMsgBox_EnterKey()
		end
	
		return true
	
	end

end

-- 메시지 박스 ESC키 단축키
function ReportMsgBox_ESC()

	if false == IsOpenWidget( WIDGET_ID_REPORTMESSAGEBOX ) then
		return false
	end

	if false == GetBool( WIDGET_ID_REPORTMESSAGEBOX, "visible", "" ) then
	
		return false
		
	else
	
		-- ESC키 단축키 이벤트 호출
		if nil ~= g_funcReportMsgBox_EscKey then
			g_bReportBoxInputESC = true
			g_funcReportMsgBox_EscKey()
		end
		
		return true
		
	end

end

---------------------------------------------------------------------------------------------------------------
-- 이벤트

-- 키보드 이벤트 (단축키 체크는 캐릭터 접속까지 끝나야한다. 게임 외부에서 처리를 위해 이벤트로 처리해주자.)
function Ev_ReportMsgBox_KeyUp( nKeyCode )

	if KEYCODE_ESCAPE == nKeyCode then
	
		ReportMsgBox_ESC()	
	
	elseif KEYCODE_ENTER == nKeyCode then
	
		ReportMsgBox_Enter()
	
	end

end

-- 닫기 이벤트
function Event_ReportMsgBox_Close()
	
	Event_ReportBox_StageClick()
	
	g_funcReportBox_EnterKey = nil
	g_funcReportBox_EscKey = nil
	g_FadeOut_Time 		= 5000 
	g_FadeOut_Delay 	= 500 
	g_FadeOut_Easing 	= "EASE_REGULAR_OUT"

	-- 메시지 박스가 사라지면서 알림창의 스테이지 클릭 이벤트도 사라지도록 하기 위함
	RemoveStageMouseClick( Event_ReportBox_StageClick )
	RemoveStageMouseClick( Event_MsgReportBox_StageClick )
	
	SetBool( WIDGET_ID_REPORTMESSAGEBOX, "visible", false, "" )
	
end

------ 알림창 관련 함수
-- 친구 초대 신청 확인
function Report_MessegeBox_OKClick_Friend()

	if nil == g_tbReportMsgBox_Info then
	
		Event_ReportMsgBox_Close()
		return
		
	end
	
	local tbInvitation = g_Invitation[ g_tbReportMsgBox_Info[ 1 ] ]
	g_Msg:ReqFriendLureAns_throughAlarm( g_tbReportMsgBox_Info[ 2 ], true, tbInvitation["MGuid"] )
	
	Invitation_Table_Remove( g_tbReportMsgBox_Info[ 1 ] )
	Event_ReportMsgBox_Close()
	
end

-- 친구 초대 신청 취소
function Report_MessegeBox_NOClick_Friend()

	if nil == g_tbReportMsgBox_Info then
	
		Event_ReportMsgBox_Close()
		return
		
	end
	
	local tbInvitation = g_Invitation[ g_tbReportMsgBox_Info[ 1 ] ]

	if g_bReportBoxInputESC == false then
		g_Msg:ReqFriendLureAns_throughAlarm( g_tbReportMsgBox_Info[ 2 ], false, tbInvitation["MGuid"] )
		Invitation_Table_Remove( g_tbReportMsgBox_Info[ 1 ] )
	end	
	
	g_bReportBoxInputESC = false
	
	Event_ReportMsgBox_Close()
end

-- 클럽 초대 신청 확인
function Report_MessegeBox_OKClick_Club()

	if nil == g_tbReportMsgBox_Info then
	
		Event_ReportMsgBox_Close()
		return
		
	end

	local tbInvitation = g_Invitation[ g_tbReportMsgBox_Info[ 1 ] ]
	local tbHostInFor = tbInvitation[ "dwHostInformation" ]
	
	g_Msg:ReqClubJoinAns(tbHostInFor["dwMasterID"], true)
	
	Invitation_Table_Remove( g_tbReportMsgBox_Info[ 1 ] )
	Event_ReportMsgBox_Close()
	
end

-- 클럽 초대 신청 취소
function Report_MessegeBox_NOClick_Club()		

	if nil == g_tbReportMsgBox_Info then
	
		Event_ReportMsgBox_Close()
		return
		
	end

	local tbInvitation = g_Invitation[ g_tbReportMsgBox_Info[ 1 ] ]
	local tbHostInFor = tbInvitation[ "dwHostInformation" ]
	
	if g_bReportBoxInputESC == false then
		g_Msg:ReqClubJoinAns(tbHostInFor["dwMasterID"], false)
	
		Invitation_Table_Remove( g_tbReportMsgBox_Info[ 1 ] )
	end
	
	g_bReportBoxInputESC = false
	Event_ReportMsgBox_Close()
end

-- 파티 초대 신청 확인
function Report_MessegeBox_OKClick_Party()

	if nil == g_tbReportMsgBox_Info then
	
		Event_ReportMsgBox_Close()
		return
		
	end

	local tbInvitation = g_Invitation[ g_tbReportMsgBox_Info[ 1 ] ]
	local tbHostInFor = tbInvitation[ "dwHostInformation" ]
	
	g_Msg:ReqPartyInviteAns( true, tbHostInFor["dwPartyID"], tbHostInFor["dwHashKey"], tbHostInFor["dwMasterID"], tbHostInFor["dwTimeKey"] )

	Invitation_Table_Remove( g_tbReportMsgBox_Info[ 1 ] )
	Event_ReportMsgBox_Close()
	
end

-- 파티 초대 신청 취소
function Report_MessegeBox_NOClick_Party()

	if nil == g_tbReportMsgBox_Info then
	
		Event_ReportMsgBox_Close()
		return
		
	end

	local tbInvitation = g_Invitation[ g_tbReportMsgBox_Info[ 1 ] ]
	local tbHostInFor = tbInvitation[ "dwHostInformation" ]
	
	if g_bReportBoxInputESC == false then
		g_Msg:ReqPartyInviteAns( false, tbHostInFor["dwPartyID"], tbHostInFor["dwHashKey"], tbHostInFor["dwMasterID"], tbHostInFor["dwTimeKey"] )

		Invitation_Table_Remove( g_tbReportMsgBox_Info[ 1 ] )
	end
	
	g_bReportBoxInputESC = false
	Event_ReportMsgBox_Close()
end

-- 메세지 알림 박스가 생기도록 하기 위한 함수
-- ([페이드인 후 이동할 함수], [페이드인 되는 총 시간], [페이드인 속도], [페이드인 타입])
function Event_MsgBox_FadeIn(goFunc, nTime, nDelay, nEasing)
	RemoveStageMouseClick( Event_MsgReportBox_StageClick )
	local strPtop = "alpha;"
	local strValue = "1;"
	local tbQuickSet = 
	{
		time = tonumber(nTime),
		 delay = tonumber( nDelay ),
		 ease = tostring( nEasing ),
		 loop = false,
		 onComplete = goFunc

	}
	SetTween( WIDGET_ID_REPORTMESSAGEBOX, tbQuickSet, strPtop, strValue, "" ) 
	
end
-- 메세지 알림 박스를 서서히 사라지도록 하기 위한 함수
-- ([페이드 아웃 후 이동할 함수], [페이드 아웃되는 총 시간], [페이드 아웃 속도], [페이드 아웃 타입])
function Event_MsgBox_FadeOut(goFunc, nTime, nDelay, nEasing)
	InsertStageMouseClickLater( Event_MsgReportBox_StageClick )
	local strPtop = "alpha;"
	local strValue = "0;"
	local tbQuickSet = 
	{
		time = tonumber(nTime),
		 delay = tonumber( nDelay ),
		 ease = tostring( nEasing ),
		 loop = false,
		 onComplete = goFunc

	}
	SetTween( WIDGET_ID_REPORTMESSAGEBOX, tbQuickSet, strPtop, strValue, "" ) 
	
end
-- 스테이지 클릭시 창이 닫히도록 하기 위한 함수
function Event_MsgReportBox_StageClick( )	
	if true == IsOpenWidget(WIDGET_ID_REPORTMESSAGEBOX) then
		Event_ReportMsgBox_Close()
	end		
end