
-- 서버 리스트 초기화가 끝났나?
g_bInitServerList = false

-- 서버 정보 가져오는 중?
g_bServerSelect_GettingServerInfo = false

-- 서버 선택 UI 로드 완료
function LoadedServerSelect()

	-- 드래그 끄기
	SetBool( WIDGET_ID_SERVERSELECT, "customDrag", true, "" )
	
	-- 닫기 버튼 숨기기
	SetBool( WIDGET_ID_SERVERSELECT, "visible", false, "closeBtn" )
	
	-- 서버 리스트 선택 이벤트 설정
	SetEvent( WIDGET_ID_SERVERSELECT, EVENT_LIST_INDEX_CHANGE, "Event_ServerSelect_Select", "index;", "listServer" )
	
	-- 종료 버튼 텍스트 설정
	local strExit = GetGameWordText( "SELECT_SERVER_CONNECTQUIT", 1 )
	SetString( WIDGET_ID_SERVERSELECT, "label", strExit, "btnExit" )
	-- 종료 버튼 이벤트 설정
	SetEvent( WIDGET_ID_SERVERSELECT, EVENT_BUTTON_CLICK, "Event_ServerSelect_Exit", "", "btnExit" )
	
	-- 클로즈 이벤트
	SetEvent( WIDGET_ID_SERVERSELECT, EVENT_WIDGET_CLOSE, "Ev_ServerSelect_WidgetClose", "", "" )

end

-- 서버 선택 UI 오픈
function OpenServerSelect()

	SetInt( WIDGET_ID_SERVERSELECT, "selectedIndex", -1, "listServer" )
	
	ServerSelect_Reset()
	InsertFrameMove( FrameMove_ServerSelect )

end

-- 리셋
function ServerSelect_Reset()

	g_bInitServerList = false

end

-- 업데이트
function FrameMove_ServerSelect( fElapsedTime )

	-- UI가 꺼져있으면 종료
	if false == IsOpenWidget( WIDGET_ID_SERVERSELECT ) then
		return;
	end
	
	-- 서버 정보 가져오고 있다.
	if true == g_bServerSelect_GettingServerInfo then
	
		if true == g_Login:IsGameServerInfoEnd() then
		
			g_bServerSelect_GettingServerInfo = false
		
			MsgBox_Reset()
			Event_ServerSelect_MsgClose()
			
		end
		
		return
	
	end
	
	-- 온라인 상태가 아니라면 다시 초기화할 수 있도록 설정
	if false == g_Login:IsOnline() then
	
		ServerSelect_Reset()
	
	end
	
	-- 서버 리스트 초기화
	if false == g_bInitServerList then
	
		InitServerList()
		
	end
	
end

-- 서버 리스트 초기화
function InitServerList()

	g_bInitServerList = true;
	
	-- 온라인 상태인가?
	if true == g_Login:IsOnline() then 
	
		-- 서버 리스트 테이블
		local tbServerList = {}
	
		-- 게임 서버 정보를 모두 받았나?
		if false == g_Login:IsGameServerInfoEnd() then
		
			if true == IsLoadWidget( WIDGET_ID_MESSAGEBOX ) then
			
				-- 서버에서 정보를 가져오고 있습니다.
				local strMsg = GetGameExternalText( "SERVERSTAGE_1", 0 )
				
				-- 메시지 박스 출력
				OpenMsgBox( MSG_BOX_TYPE_OK, strMsg, Event_ServerSelect_MsgClose )
			
			end
		
			g_bServerSelect_GettingServerInfo = true
			
			return
			
		else
		
			-- 서버 데이터 리스트 가져오기
			-- { { 그룹 인덱스, 신규 서버 여부 }, ... }
			local tbServerDataList = g_Login:GetGameServerList()
			
			--서버 정보 가져오기 실패
			if 0 == table.getn( tbServerDataList ) then
			
				-- 서버 정보 가져오기 실패 텍스트
				local strMsg = GetGameExternalText( "SERVER_INFO_RECV_FAILED", 0 )
				
				-- 메시지 박스 출력
				OpenMsgBox( MSG_BOX_TYPE_OK, strMsg, Event_ServerSelect_MsgClose )				
				
				return
				
			-- 서버 정보 가져오기 성공
			else
				
				local nServerListIndex = 1	-- 서버 리스트 추가할 인덱스
				
				-- 서버 리스트 테이블 생성
				for value in pairs( tbServerDataList ) do

					-- 각 항목별 데이터
					local tbData = tbServerDataList[ value ]
					
					-- 서버 이름
					local strLabel = GetGameExternalText( "SERVER_NAME", tbData[ 1 ] )
					
					-- 신규 서버라면 New 아이콘 추가
					if true == tbData[ 2 ] then
						
						CallFunction( WIDGET_ID_SERVERSELECT, "gotoAndPlay", "new", "item" .. tostring( value ) .. ".mcNewImg" )
						
					-- 아니라면 텍스트 라인을 맞추기 위해 탭 문자 추가
					else
					
						CallFunction( WIDGET_ID_SERVERSELECT, "gotoAndPlay", "none", "item" .. tostring( value ) .. ".mcNewImg" )
					
					end
					
					-- 서버 항목 생성
					local tbServer = {}
					tbServer[ 1 ] = strLabel
					tbServer[ 2 ] = tostring( tbData[ 1 ] )
					
					-- 서버 리스트 테이블에 추가
					tbServerList[ nServerListIndex ] = tbServer
					nServerListIndex = nServerListIndex + 1
					
				end
				
			end
			
			-- 메시지 박스 떠있다면 끄기
			if true == IsOpenWidget( WIDGET_ID_MESSAGEBOX ) then
				Event_MsgBox_Close()
			end
			
			-- 서버 리스트 설정
			SetDataEx( WIDGET_ID_SERVERSELECT, "label;groupIndex;", tbServerList, "listServer", false )
			
		end
		
	-- 네트워크 연결 실패
	else
	
		if false == IsOpenMsgBox() then
		
			-- 네트워크 연결이 실패하였습니다. 게임을 종료하시겠습니까? 취소를 누르면 재연결을 시도합니다.
			local strMsg = GetGameExternalText( "SERVERSTAGE_4", 0 )
			
			-- 메시지 박스 출력
			OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, strMsg, TryConnectLoginServer, Event_MsgBox_CloseGame )
		
		end
		
	end

end

-- 재접속 시도
function TryConnectLoginServer()

	-- 로그인 서버 접속 시도
	local bConnectLoginServer = g_Msg:ConnectLoginServer()
	if false == bConnectLoginServer then
		
		-- 실패했다면 게임 종료 메시지 출력
		Event_ServerSelect_Exit()
		
	else
	
		Event_MsgBox_Close()
		InitServerList()
		
	end
	
end

---------------------------------------------------------------------------------------------------------------------------

-- 서버 리스트 선택 이벤트
function Event_ServerSelect_Select( nIndex )

	-- 잘못된 선택이면 종료
	if 0 > nIndex then
		return
	end
	
	-- 선택한 서버 인덱스 설정
	g_Login.m_nSelectServerIndex = nIndex
	
	if false == IsOpenWidget( WIDGET_ID_SERVERCHANNELSELECT ) then
		
		-- 채널 선택 UI 열기
		OpenWidget( WIDGET_ID_SERVERCHANNELSELECT )
	
	else
	
		-- 이미 열려 있으면 채널 리스트 초기화
		InitChannelList()
	
	end

end

-- 종료 버튼 클릭 이벤트
function Event_ServerSelect_Exit()

	-- 게임을 종료하시겠습니까?
	local strMsg = GetGameExternalText( "SERVERSTAGE_3", 0 )
	
	-- 메시지 박스 출력
	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, strMsg, Event_MsgBox_CloseGame, Event_MsgBox_Close )

end

-- 메시지 박스 닫기
function Event_ServerSelect_MsgClose()

	ServerSelect_Reset()
	Event_MsgBox_Close()

end

-- 클로즈 이벤트
function Ev_ServerSelect_WidgetClose()

	RemoveFrameMove( FrameMove_ServerSelect )

end