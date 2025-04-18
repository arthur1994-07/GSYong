-- 캐릭터가 받은 초대 종류(해당 변수명과 값은 변경되어서는 않된다.)(1. 친구초대; 2. 그룹쳇팅; 3. 클럽초대; 4. 파티초대)
--INVITATION_REPORT_FRIEND			= 1
--INVITATION_REPORT_GROUPCHAT		= 2
--INVITATION_REPORT_CLUB			= 3
--INVITATION_REPORT_PARTY			= 4 

-- 전역 변수 지정
g_Invitation = {}			-- 캐릭터가 받은 초대 데이터 저장용 테이블
g_ReportPoint = 0			-- 알림창 메세지 박스의 현재 위치를 저장하기 위한 변수

-- 알림정보 창 로드 완료
function LoadReportBox()

	InsertStageMouseClick( Event_ReportBox_StageClick )
	
	-- 알림창에 마우스 롤 오버 혹은 롤 아웃시 셋팅
	SetEvent( WIDGET_ID_REPORTBOX, EVENT_MOUSE_ROLLOVER, "Ev_ReportBox_RollOver", "", "")
	SetEvent( WIDGET_ID_REPORTBOX, EVENT_MOUSE_ROLLOUT, "Ev_ReportBox_RollOut", "", "" )	
	-- 온픈함수
	OpenReportBox()	
	-- 리스트 스크롤 이벤트 설정(위젯 ID, 스크롤 이벤트 선언, 이벤트 발생시 이동할 함수, 반환받을 프로퍼티 종류, 인스턴스 경로)
	SetEvent( WIDGET_ID_REPORTBOX, EVENT_SCROLL, "Event_ReportBox_list_Scroll", "", "ReportBoxList" )
	-- 이벤트 발생시 이동할 함수의 경우 클릭 이벤트로 반환되는 위치는 0이 최소값 이므로 스크립트에 사용하기 위해 최소값을 1로 변환하는 함수를 만들어 해당 함수로 이동시킴
	SetEvent( WIDGET_ID_REPORTBOX, EVENT_LIST_ITEM_CLICK, "IndexPlusOne", "index", "ReportBoxList" )
	
end
-- 알림창 오픈시 발생될 함수
function OpenReportBox()
	local Invitation_Data_Text = {}								-- 알림창의 초대 텍스트 내용을 담을 배열
	local Invitation_Data_Count = table.getn(g_Invitation)		-- 보유한 초대의 갯수를 담을 변수
	
	RemoveStageMouseClick( Event_MsgReportBox_StageClick )
	
	-- for 문 사용, i 를 1 에서 Invitation_Data_Count 까지 1씩 증가시킨다.(감소시키고 싶으면 -1 사용)
	-- GetGameWordText 는 gameword.xml의 "" 안의 값을 가져온다.
	-- 알림창 레이블에 표시할 텍스트를 설정하는 for문
	for i = 1, Invitation_Data_Count, 1 do
		local tb_Invitation = g_Invitation[ i ]
		if tb_Invitation[ "dwType" ] == INVITATION_REPORT_FRIEND then
			Invitation_Data_Text[ i ] = { tb_Invitation[ "dwDate" ] .. GetGameWordText( "NOTIFY_TYPE", 1 ) .. "            " .. tb_Invitation[ "dwHostName" ] }	-- 테이블을 삽입할때 .. 을통해 값을 연결시킬 수 있다.
		elseif tb_Invitation[ "dwType" ] == INVITATION_REPORT_GROUPCHAT then
			Invitation_Data_Text[ i ] = { tb_Invitation[ "dwDate" ] .. GetGameWordText( "NOTIFY_TYPE", 2 ) .. "            " .. tb_Invitation[ "dwHostName" ] }
		elseif tb_Invitation[ "dwType" ] == INVITATION_REPORT_CLUB then
			Invitation_Data_Text[ i ] = { tb_Invitation[ "dwDate" ] .. GetGameWordText( "NOTIFY_TYPE", 3 ) .. "            " .. tb_Invitation[ "dwHostName" ] }
		elseif tb_Invitation[ "dwType" ] == INVITATION_REPORT_PARTY then
			Invitation_Data_Text[ i ] = { tb_Invitation[ "dwDate" ] .. GetGameWordText( "NOTIFY_TYPE", 4 ) .. "            " .. tb_Invitation[ "dwHostName" ] }
		end		
	end
	
	-- 알림창 타이틀 이름 설정(위젯 ID, 스트링 출력위치[현재는 위젯의 타이틀], 출력할 텍스트, 인스턴스 경로[타이틀의 경우 따로 경로가 없으므로 비워준다.])
	SetString( WIDGET_ID_REPORTBOX, "title", GetGameWordText( "NOTIFY_WINDOW_TITLE", 0 ) .. " ( " .. Invitation_Data_Count .. " )", "" )
	-- 알림창 라벨 데이터값 설정(위젯 ID, 테이블 오브젝트, 테이블 데이터값[보통은 배열로 사용], 인스턴스 경로, 추가여부)
	SetDataEx( WIDGET_ID_REPORTBOX, "label;", Invitation_Data_Text, "ReportBoxList", false )
end
-- 인덱스 값을 +1 시켜줄 함수
function IndexPlusOne( nIndex )	
	local IndexPlusOne = nIndex + 1	
	Report_MessegeBox_Open( IndexPlusOne, true )	
end

-- 리스트 스크롤 이벤트
function Event_ReportBox_list_Scroll()
	-- 현재 스크롤 위치 저장
	local nScrollPos = GetInt( WIDGET_ID_REPORTBOX, "scrollPosition", "ReportBoxList" )
end

-- 초대 메세지를 전달받을 시 확인창이 출력되도록 만들기 위한 함수(해당 함수는 WidgetClientCall.lua 에서 사용된다)
-- (텍스트 축력타입, 메세지박스 지속시간, 메세지박스 사라지는 속도, 사라진 메세지 박스가 재 등장 하지 않기 위한 문)(원용)
function Report_MessegeBox_Invitation(nIndex)
	QuickMenu_ViewSet()		-- 알림창 출력 여부 결정용(알림창에 항목이 하나라도 있을 경우 출력)
	Report_MessegeBox_Open(nIndex, false )
end

-- 알림 목록 저장 테이블 삭제용 함수
function Invitation_Table_Remove(nIndex)
	table.remove(g_Invitation, nIndex )
	QuickMenu_ViewSet()		-- 알림창 출력 여부 결정용(알림창에 항목이 하나라도 있을 경우 출력)
end


--------------------------------- 클라이언트 콜
-- 알림창에서 받을 초대 타입 구분용 함수
function Insert_Invitation_Table(Get_Invitation_Val, Get_HostName, Get_HostInfor_Val, tbMGuid )

	local nIndex = 0
	
	-- 알림창 알림이 중복안되게 하는 부분 기획의도에 맞게 제거.
	--for value in pairs( g_Invitation ) do
	
		--local tb = g_Invitation[ value ]
		--if Get_Invitation_Val == tb[ "dwType" ] then
		
		--	nIndex = value
		--	break
		
		--end
	
	--end

	local tbInvitation = {}
	local tbDate = GetDate()	--tbDate[ "nMonth" ], 	tbDate[ "nDay" ], 	tbDate[ "nHour" ], 	tbDate[ "nMinute" ]
	
	-- 받은 초대 메세지 타입을 넣기 위한 문
	tbInvitation["dwType"] = Get_Invitation_Val															-- 캐릭터가 받은 초대 타입 저장용 테이블
	tbInvitation["dwDate"] = string.format( "[%d : %d]   ", tbDate[ "nHour" ], tbDate[ "nMinute" ] )	-- 초대를 받은 연-월-일 을 저장하기 위한 테이블
	tbInvitation["dwHostName"] = Get_HostName															-- 초대한 캐릭터 이름을 담을 테이블
	tbInvitation["dwHostInformation"] = Get_HostInfor_Val												-- 초대한 캐릭터 정보를 담을 테이블
	tbInvitation["MGuid"] = tbMGuid
	
	if 0 == nIndex then
		nIndex = table.getn( g_Invitation)	+ 1
	end
	
	g_Invitation [ nIndex ] = tbInvitation

	g_ReportPoint = nIndex
end

----------------------------------------------------- 이벤트
-- 닫기 이벤트
function Event_ReportBox_Close()
	RemoveStageMouseClick( Event_ReportBox_StageClick )
	CloseWidget(WIDGET_ID_REPORTBOX)
	SetBool(WIDGET_ID_QUICKMENU, "selected", false, "QuickMenu.ButtonRaport")
	
end

function Ev_ReportBox_RollOver()
	RemoveStageMouseClick( Event_ReportBox_StageClick )
end

function Ev_ReportBox_RollOut()
	InsertStageMouseClick( Event_ReportBox_StageClick )
end

-- 스테이지 클릭시 창이 닫히도록 하기 위한 함수
function Event_ReportBox_StageClick()
	if true == IsOpenWidget( WIDGET_ID_REPORTBOX ) then
		Event_ReportBox_Close()
	end	
end