
-- 서버 상태값 정의
SERVER_STATE_SMOOTH 	= 0
SERVER_STATE_NORMAL 	= 1
SERVER_STATE_CONGEST 	= 2
SERVER_STATE_NOVACANCY 	= 3

-- 채널 리스트 데이터
g_tbChannelSelect_ChannelDataList = {}

-- 접속 시도한 채널 인덱스
g_nServerChannelSelect_SelectIndex = -1

-- 서버 채널 선택 UI 로드 완료
function LoadServerChannelSelect()

	-- 드래그 끄기
	SetBool( WIDGET_ID_SERVERCHANNELSELECT, "customDrag", true, "" )

	-- 닫기 버튼 숨기기
	SetBool( WIDGET_ID_SERVERCHANNELSELECT, "visible", false, "closeBtn" )
	
	-- 접속 버튼 텍스트 설정
	local strConnect = GetGameWordText( "SELECT_SERVER_CONNECTQUIT", 0 )
	SetString( WIDGET_ID_SERVERCHANNELSELECT, "label", strConnect, "btnConnect" )
	-- 접속 버튼 이벤트 설정
	SetEvent( WIDGET_ID_SERVERCHANNELSELECT, EVENT_BUTTON_CLICK, "Event_ChannelSelect_Connect", "", "btnConnect" )
	
	-- 채널 리스트 이벤트 설정
	SetEvent( WIDGET_ID_SERVERCHANNELSELECT, EVENT_LIST_ITEM_DOUBLE_CLICK, "Event_ChannelSelect_listChannel_DClick", "index;", "listChannel" )

end

-- 서버 채널 선택 UI 오픈
function OpenServerChannelSelect()

	ServerChannelSelect_UIPosition()
	
	SetInt( WIDGET_ID_SERVERCHANNELSELECT, "selectedIndex", -1, "listChannel" )

	-- 채널 리스트 초기화
	InitChannelList()

end

function ServerChannelSelect_UIPosition()

	if false == IsOpenWidget( WIDGET_ID_SERVERCHANNELSELECT ) then
		return
	end
	
	local nPosX = 0
	local nPosY = 0
	
	if true == IsOpenWidget( WIDGET_ID_SERVERSELECT ) then
	
		local nX = GetInt( WIDGET_ID_SERVERSELECT, "x", "" )
		local nY = GetInt( WIDGET_ID_SERVERSELECT, "y", "" )
		local nW = GetInt( WIDGET_ID_SERVERSELECT, "width", "" )
		
		nPosX = nX + nW
		nPosY = nY
	
	end
	
	SetInt( WIDGET_ID_SERVERCHANNELSELECT, "x", nPosX, "" )
	SetInt( WIDGET_ID_SERVERCHANNELSELECT, "y", nPosY, "" )

end

-- 채널 리스트 초기화
function InitChannelList()

	-- 선택한 서버가 없다면 종료
	if 0 > g_Login.m_nSelectServerIndex then
		return
	end
	
	-- 선택한 서버의 그룹 인덱스
	local nGroupIndex = GetDataInt( WIDGET_ID_SERVERSELECT, g_Login.m_nSelectServerIndex, "groupIndex", "listServer" )
	
	-- 타이틀 설정
	local strServerName = GetGameExternalText( "SERVER_NAME", nGroupIndex )
	SetString( WIDGET_ID_SERVERCHANNELSELECT, "label", strServerName, "titleBtn" )
	
	-- 채널 데이터 리스트 얻기
	-- { { 채널 인덱스, PK 여부, 서버 상태 }, ... }
	g_tbChannelSelect_ChannelDataList = g_Login:GetServerChannelList( nGroupIndex )
	
	-- 채널 텍스트
	local strChannel = GetGameExternalText( "SERVER_CHANNEL", 0 )
	-- PK 텍스트
	local strPK = GetGameExternalText( "SERVER_PK" , 0 )
	
	-- 채널 리스트
	local tbChannelList = {}
	local nChannelListIndex = 1		-- 추가할 채널 인덱스
	
	-- 채널 리스트 생성
	for value in pairs( g_tbChannelSelect_ChannelDataList ) do
	
		-- 각 항목별 데이터
		local tbData = g_tbChannelSelect_ChannelDataList[ value ]
		
		-- 리스트 텍스트
		local strLabel = tostring( tbData[ 1 ] ) .. " " .. strChannel
		
		-- PK 서버라면 PK 문자 추가
		if true == tbData[ 2 ] then
			
			strLabel = strLabel .. strPK
			strLabel = PrepareHTMLData( strLabel )
			
		-- 아니면 텍스트 정렬을 위해 탭 문자 추가
		else
			
			strLabel = PrepareHTMLData( strLabel )
			
		end
		
		-- 서버 상태
		local nServerState = SERVER_STATE_SMOOTH
	
		-- 원할
		if tbData[ 3 ] < g_nSERVER_NORMAL * 0.01 then
		
			nServerState = SERVER_STATE_SMOOTH
			
		-- 보통
		elseif tbData[ 3 ] < g_nSERVER_CONGEST * 0.01 then
		
			nServerState = SERVER_STATE_NORMAL
			
		-- 혼잡
		elseif tbData[ 3 ] < g_nSERVER_NOVACANCY * 0.01 then
		
			nServerState = SERVER_STATE_CONGEST
			
		-- 접속 불가
		else
		
			nServerState = SERVER_STATE_NOVACANCY
			
		end
		
		-- 서버 상태 텍스트
		local strServerState = ""
		-- 원할
		if tbData[ 3 ] < g_nSERVER_NORMAL * 0.01 then
		
			strServerState = HTMLFontColor( GetGameWordText( "SERVER_STATE", 0 ), HTML_TEXT_COLOR_PALEGREEN )
			
		-- 보통
		elseif tbData[ 3 ] < g_nSERVER_CONGEST * 0.01 then
		
			strServerState = HTMLFontColor( GetGameWordText( "SERVER_STATE", 1 ), HTML_TEXT_COLOR_GOLD )
		
		-- 혼잡
		elseif tbData[ 3 ] < g_nSERVER_NOVACANCY * 0.01 then
		
			strServerState = HTMLFontColor( GetGameWordText( "SERVER_STATE", 2 ), HTML_TEXT_COLOR_RED )
			
		-- 접속 불가
		else
		
			strServerState = HTMLFontColor( GetGameWordText( "SERVER_STATE", 3 ), HTML_TEXT_COLOR_RED )
			
		end
		
		local tbChannel = {}
		tbChannel[ 1 ] = strLabel					-- 리스트 텍스트
		tbChannel[ 2 ] = strServerState				-- 서버 상태 텍스트
		tbChannel[ 3 ] = tostring( tbData[ 1 ] )	-- 채널 인덱스
		tbChannel[ 4 ] = tostring( nServerState )	-- 서버 상태
		
		-- 채널 리스트에 추가
		tbChannelList[ nChannelListIndex ] = tbChannel
		nChannelListIndex = nChannelListIndex + 1
	
	end
	
	-- 채널 리스트가 하나도 없다면 종료
	if 0 == table.getn( tbChannelList ) then
		return
	end
	
	-- 채널 리스트 설정
	local strParam = CreateParam( "label1", "label2", "channelIndex", "serverState" )
	SetDataEx( WIDGET_ID_SERVERCHANNELSELECT, strParam, tbChannelList, "listChannel", false )

end

-- 선택한 채널에 접속
function ConnectChannel( nIndex )

	g_nServerChannelSelect_SelectIndex = nIndex

	-- 서버 상태
	local nServerState = GetDataInt( WIDGET_ID_SERVERCHANNELSELECT, nIndex, "serverState", "listChannel" )
	
	-- 선택한 서버 접속 불가, 메시지 박스 출력
	if nServerState == SERVER_STATE_NOVACANCY then
	
		local strMsg = GetGameExternalText( "LOGINSTAGE_FULL", 0 )
		OpenMsgBox( MSG_BOX_TYPE_OK, strMsg, Event_MsgBox_Close )
		return
		
	end
	
	-- 선택한 서버 인덱스
	local nServerIndex = GetInt( WIDGET_ID_SERVERSELECT, "selectedIndex", "listServer" )
	-- 서버 그룹
	local nServerGroup = GetDataInt( WIDGET_ID_SERVERSELECT, nServerIndex, "groupIndex", "listServer" )
	-- 채널 인덱스
	local nChannelIndex = GetDataInt( WIDGET_ID_SERVERCHANNELSELECT, nIndex, "channelIndex", "listChannel" )
	
	-- 서버 채널에 연결
	if false == g_Msg:ConnectServerChannel( nServerIndex, nServerGroup, nChannelIndex ) then
	
		-- 접속 실패 메시지 출력
		local strMsg = GetGameExternalText( "SERVERSTAGE_5", 0 )
		OpenMsgBox( MSG_BOX_TYPE_OK, strMsg, Event_MsgBox_Close )
		return
	
	end
	
	local nSP = GetServiceProvider()
	if SP_OFFICE_TEST == nSP then
	
		-- 서버 넘버 셋팅
		if true == g_Login:SetServerNumber( nServerIndex ) then
			
			-- 성공 했다면 로그인 스테이지로
			ToLoginStage()
			
			g_Login.m_nServerGroup = nServerGroup
			g_Login.m_nServerChannel = nChannelIndex
			
		end
	
	else
	
		-- 일부 서비스 프로바이더에선 웹에서 이미 로그인 했다.
		nSP = GetRenderServiceProvider()
		if 	EMSERVICE_KOREA == nSP or
			EMSERVICE_MALAYSIA_CN == nSP or
			EMSERVICE_MALAYSIA_EN == nSP or
			EMSERVICE_PHILIPPINES == nSP or
			EMSERVICE_VIETNAM == nSP then
			
			-- 서버 넘버 셋팅
			g_Login:SetServerNumber( nServerIndex )
			
			g_Login.m_nServerGroup = nServerGroup
			g_Login.m_nServerChannel = nChannelIndex
			
			-- 서버에서 복호화 키 받을 때까지 잠시 대기
			local str = GetGameExternalText( "LOGINSTAGE_6", 0 )
			OpenMsgBox( MSG_BOX_TYPE_INFO, str, nil, nil, nil, nil, 1, Ev_ServerChannelSelect_WaitKeyDelay )
			
		else
		
			-- 서버 넘버 셋팅
			if true == g_Login:SetServerNumber( nServerIndex ) then
				
				-- 성공 했다면 로그인 스테이지로
				ToLoginStage()
				
				g_Login.m_nServerGroup = nServerGroup
				g_Login.m_nServerChannel = nChannelIndex
				
			end
			
		end
	
	end

end

-----------------------------------------------------------------------------------------------------------------------
-- 이벤트

-- 접속 버튼 클릭 이벤트
function Event_ChannelSelect_Connect()

	-- 선택한 채널 리스트 인덱스
	local nIndex = GetInt( WIDGET_ID_SERVERCHANNELSELECT, "selectedIndex", "listChannel" )

	-- 채널에 접속
	ConnectChannel( nIndex )

end

-- 채널 리스트 아이템 더블 클릭 이벤트
function Event_ChannelSelect_listChannel_DClick( nIndex )

	if -1 == nIndex then
		return
	end

	-- 채널에 접속
	ConnectChannel( nIndex )

end

-- 서버 키 대기 완료
function Ev_ServerChannelSelect_WaitKeyDelay()

	Event_MsgBox_Close()

	-- 서버에서 복호화 키 받았으면 로그인 시도
	if true == g_Msg:WaitKeyAndTryLogin() then

		-- 로그인 대기 메시지
		local str = GetGameExternalText( "DAUM_LOGIN_WAIT", 0 )
		OpenMsgBox( MSG_BOX_TYPE_CANCEL, str, Event_MsgBox_Close, Event_MsgBox_Close, 
			nil, nil, 10, Ev_ServerChannelSelect_NoResponse )

	else

		-- 키 아직 못받았다.
		Ev_ServerChannelSelect_NoResponse()

	end

end

-- 서버 접속 실패 - 응답 없음.
function Ev_ServerChannelSelect_NoResponse()

	Event_MsgBox_Close()

	local str = GetGameExternalText( "DAUM_LOGIN_WAIT_SERVERFULL", 0 )
	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, str, Ev_ServerChannelSelect_RetryConnect, Event_MsgBox_Close )

end

-- 서버 접속 다시 시도
function Ev_ServerChannelSelect_RetryConnect()

	Event_MsgBox_Close()
	ConnectChannel( g_nServerChannelSelect_SelectIndex )

end