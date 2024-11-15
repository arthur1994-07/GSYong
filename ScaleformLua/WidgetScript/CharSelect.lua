
g_tbCharSelect_List = {}

g_bCharSelect_RecvRanMobileInfo = false

g_bCharSelect_InitList = false
g_nCharSelect_CharCount = 0		-- 캐릭터 개수
g_nCharSelect_CharMax = 16		-- 캐릭터 생성 최대 개수

g_bFirstLogin = true

-- 캐릭터 선택 UI 로드 완료
function LoadCharSelect()

	SetBool( WIDGET_ID_CHARSELECT, "customDrag", true, "" )
	-- 닫기 버튼 숨기기
	SetBool( WIDGET_ID_CHARSELECT, "visible", false, "closeBtn" )

	-- 캐릭터 선택
	--local strCharSelect = "< " .. GetGameWordText( "SELECT_CHARACTER_PAGE", 0 ) .. " >"	
	-- exp
	local strExp = GetGameWordText( "CHARACTER_EXP_STATIC", 0 )
	-- 성향
	local strAtb = GetGameWordText( "SELECT_CHARACTER_STATIC", 1 )
	-- 삭제
	local strDelete = GetGameWordText( "SELECT_CHARACTER_PAGE_BUTTON", 2 )
	-- 서버변경
	local strServerChange = GetGameWordText( "CHANGE_SERVER", 0 )
	-- 게임시작
	local strGameStart = GetGameWordText( "SELECT_CHARACTER_PAGE_BUTTON", 1 )
	
	-- 접속중인 서버, 채널 텍스트
	local strServerName = GetGameExternalText( "SERVER_NAME", g_Login.m_nServerGroup )
	local strChannel = tostring( g_Login.m_nServerChannel ) .. GetGameExternalText( "SERVER_CHANNEL", 0 )
	local strConnectServer = strServerName .. " - " .. strChannel
	
	-- 타이틀 텍스트 설정
	--SetString( WIDGET_ID_CHARSELECT, "text", strCharSelect, "labelTitle" )
	--SetString( WIDGET_ID_CHARSELECT, "text", GetGameWordText( "SELECT_CHARACTER_PAGE", 0 ), "charInfo.tfTitle" )
	-- exp 텍스트 설정
	SetString( WIDGET_ID_CHARSELECT, "text", strExp, "charInfo.labelExp" )
	-- 성향 텍스트 설정
	SetString( WIDGET_ID_CHARSELECT, "text", strAtb, "charInfo.labelTendency" )
	
	-- 삭제 버튼 텍스트 설정
	SetString( WIDGET_ID_CHARSELECT, "label", strDelete, "charInfo.btns.btnDelete" )
	-- 서버변경 버튼 텍스트 설정
	SetString( WIDGET_ID_CHARSELECT, "label", strServerChange, "charInfo.btns.btnServer" )
	-- 게임시작 버튼 텍스트 설정
	SetString( WIDGET_ID_CHARSELECT, "label", strGameStart, "charInfo.btns.btnStart" )
	
	-- 접속중인 서버, 채널 텍스트 설정
	SetString( WIDGET_ID_CHARSELECT, "text", strConnectServer, "charInfo.btns.labelServer" )
	
	-- exp 바 최대/최소 값 설정
	SetDouble( WIDGET_ID_CHARSELECT, "maximum", 1, "charInfo.barExp" )
	SetDouble( WIDGET_ID_CHARSELECT, "minimum", 0, "charInfo.barExp" )
	
	-- 성향 바 최대/최소 값 설정
	SetDouble( WIDGET_ID_CHARSELECT, "maximum", 200, "charInfo.barTendency" )
	SetDouble( WIDGET_ID_CHARSELECT, "minimum", -200, "charInfo.barTendency" )	
	SetDouble( WIDGET_ID_CHARSELECT, "value", 0, "charInfo.barTendency" )
	
	-- 캐릭터 리스트 선택 이벤트 설정
	SetEvent( WIDGET_ID_CHARSELECT, EVENT_LIST_INDEX_CHANGE, "Event_CharSelect_listChar_Index",
		"index;", "charInfo.listChar" )
	-- 캐릭터 리스트 더블클릭 이벤트 설정
	SetEvent( WIDGET_ID_CHARSELECT, EVENT_LIST_ITEM_DOUBLE_CLICK, "Event_CharSelect_listChar_DBClick",
		"index;", "charInfo.listChar" )
	-- 새캐릭터 버튼 클릭 이벤트 설정
	SetEvent( WIDGET_ID_CHARSELECT, EVENT_BUTTON_CLICK, "Event_CharSelect_btnNewChar_Click", "",
		"charInfo.btns.btnNewChar" )
	-- 삭제 버튼 클릭 이벤트 설정
	SetEvent( WIDGET_ID_CHARSELECT, EVENT_BUTTON_CLICK, "Ev_CharSelect_BtnDel_SecondPassOpen", "",
		"charInfo.btns.btnDelete" )
	-- 서버변경 버튼 클릭 이벤트 설정
	SetEvent( WIDGET_ID_CHARSELECT, EVENT_BUTTON_CLICK, "Event_CharSelect_btnServer_Click", "",
		"charInfo.btns.btnServer" )
	-- 게임시작 버튼 클릭 이벤트 설정
	SetEvent( WIDGET_ID_CHARSELECT, EVENT_BUTTON_CLICK, "Event_CharSelect_btnStart_Click", "",
		"charInfo.btns.btnStart" )
		
	-- 란 모바일 정보 요청
	if true == g_UserFeatures:IsUsingRanMobile() then
	
		g_bCharSelect_RecvRanMobileInfo = false
		g_Msg:ReqRanMobileInfoAll()
		
	end

end

-- 캐릭터 선택 오픈
function OpenCharSelect()

	-- 새캐릭터
	local str = GetGameWordText( "SELECT_CHARACTER_PAGE_BUTTON", 0 ) .. 
		"(" .. tostring( g_Login:GetCharRemain() ) .. ")"
	SetString( WIDGET_ID_CHARSELECT, "label", str, "charInfo.btns.btnNewChar" )
	
	if true == g_bCharSelect_InitList then
		CharSelect_InitSelectCharList()
	end
	
	g_bCharSelect_InitList = true

end

function CharSelect_SetEnable ( _bEnabled )

	SetBool( WIDGET_ID_CHARSELECT, "enabled", _bEnabled, "charInfo.btns.btnNewChar" )
	SetBool( WIDGET_ID_CHARSELECT, "enabled", _bEnabled, "charInfo.btns.btnDelete" )
	SetBool( WIDGET_ID_CHARSELECT, "enabled", _bEnabled, "charInfo.btns.btnServer" )
	SetBool( WIDGET_ID_CHARSELECT, "enabled", _bEnabled, "charInfo.btns.btnStart" )
	
	CountrySelect_SetEnable( _bEnabled )
		
end

-- 리셋
function CharSelect_Reset()

	g_bCharSelect_InitList = false

end

-- 란 모바일 계정 정보 설정
function CharSelect_SetRanMobileInfo()

	-- 모바일 정보 아직 못받았다.
	if false == g_bCharSelect_RecvRanMobileInfo then
		return
	end

	local tbRanMobile = g_RanMobile:GetCharInfoAll()
	local nIndex = 1

	for value in pairs( g_tbCharSelect_List ) do
	
		local strInst = "charInfo.listChar.mcItemRenderer.item" .. (value - 1)
		local tbCharInfo = g_tbCharSelect_List[ value ]
		
		for value2 in pairs( tbRanMobile ) do
		
			local tbMobileInfo = tbRanMobile[ value2 ]
			if tbMobileInfo[ 3 ] == tbCharInfo[ 11 ] then
			
				local str = string.format( "%s : %s\n%s : %s",
					GetGameWordText( "RANMOBILE_ASSET", 3 ), tbMobileInfo[ 1 ], 
					GetGameWordText( "RANMOBILE_ASSET", 4 ), tbMobileInfo[ 4 ] )
			
				-- 닉네임, 만료 시간 툴팁
				SetString( WIDGET_ID_CHARSELECT, "TooltipText", str, strInst .. ".mcMobileImg" )
				-- 만료 시간 게이지 설정
				SetDouble( WIDGET_ID_CHARSELECT, "value", tbMobileInfo[ 2 ], strInst .. ".mcMobileImg" )
				
				break
			
			end
		
		end
	
	end

end

------------------------------------------------------------------------------------------------------

-- 캐릭터 리스트 선택 이벤트
function Event_CharSelect_listChar_Index( nIndex )

	if nIndex == -1 then
			
		-- exp 바 값 설정
		SetDouble( WIDGET_ID_CHARSELECT, "value", 0, "charInfo.barExp" )
		SetString( WIDGET_ID_CHARSELECT, "text", " ", "charInfo.tfExp" )
		
		-- 성향 바 값 설정
		SetDouble( WIDGET_ID_CHARSELECT, "value", -200, "charInfo.barTendency" )
			
		return ;
	end
	
	-- exp/성향 바 수치 설정
	local fExp = GetDataDouble( WIDGET_ID_CHARSELECT, nIndex, "exp", "charInfo.listChar" )
	local fAtb = GetDataDouble( WIDGET_ID_CHARSELECT, nIndex, "atb", "charInfo.listChar" )	
	SetDouble( WIDGET_ID_CHARSELECT, "value", fExp, "charInfo.barExp" )
	SetDouble( WIDGET_ID_CHARSELECT, "value", fAtb, "charInfo.barTendency" )
	
	if fExp > 1 then
		fExp = 1
	end	
	
	local strExp = string.format( "%.2f", fExp * 100 ) .. "%"
	SetString( WIDGET_ID_CHARSELECT, "text", strExp, "charInfo.tfExp" )
		
	-- 캐릭터 정보 설정
	local strName = GetDataString( WIDGET_ID_CHARSELECT, nIndex, "name", "charInfo.listChar" )
	local strLevel = "Lv." .. GetDataString( WIDGET_ID_CHARSELECT, nIndex, "level", "charInfo.listChar" )
	local strSchool = GetDataString( WIDGET_ID_CHARSELECT, nIndex, "school", "charInfo.listChar" )
	local strClass = GetCharClassName( GetDataInt( WIDGET_ID_CHARSELECT, nIndex, "class", "charInfo.listChar" ) )
	local strMap = GetGameWordText( "SELECT_CHARACTER_STATIC", 2 ) .. " : " .. 
		GetDataString( WIDGET_ID_CHARSELECT, nIndex, "map", "charInfo.listChar" )
	
	local strInfo = strName .. " " .. strLevel .. "\n" .. strSchool .. "/" .. strClass .. "\n" .. strMap
	
	
	-- 클럽 이름, 랭크 설정
	local strClubName = GetDataString( WIDGET_ID_CHARSELECT, nIndex, "clubName", "charInfo.listChar" )
	local strClubRank = GetDataString( WIDGET_ID_CHARSELECT, nIndex, "clubRank", "charInfo.listChar" )
	
	local strClubInfo = ""
	if 0 == string.len( strClubName ) then
		strInfo = strInfo .. "\n" .. GetGameWordText( "CHAT_OPTION", 3 ) .. " : No Club."
	else
		strInfo = strInfo .. "\n" .. GetGameWordText( "CHAT_OPTION", 3 ) .. " : " .. strClubName .. "(" .. strClubRank .. ")"
	end
	
	SetString( WIDGET_ID_CHARSELECT, "text", strInfo, "charInfo.tfSelectInfo" )
	
	-- 로비 캐릭터 선택
	local nSelectPage = math.floor( nIndex / 3 )
	local nSelectNum = nIndex % 3
	g_CharSelect:SelectLobyChar( nSelectPage, nSelectNum, nIndex )
	
end

-- 캐릭터 리스트 더블클릭 이벤트
function Event_CharSelect_listChar_DBClick( nIndex )

	-- 로비 캐릭터 선택
	local nSelectPage = math.floor( nIndex / 3 )
	local nSelectNum = nIndex % 3
	g_CharSelect:SelectLobyChar( nSelectPage, nSelectNum, nIndex )

	g_Msg:JoinGame( nIndex )

end

-- 새캐릭터 버튼 클릭 이벤트
function Event_CharSelect_btnNewChar_Click()
	
	if 0 == g_Login:GetCharRemain() then
		OpenMsgBox( MSG_BOX_TYPE_OK, GetGameExternalText( "CHARACTER_NOT_ENOUGH_CHARREMAIN", 0 ), Event_MsgBox_Close )
		return
	end
	
	if g_nCharSelect_CharCount == g_nCharSelect_CharMax then
		OpenMsgBox( MSG_BOX_TYPE_OK, GetGameExternalText( "CHARACTER_THIS_SERVER_SLOT_FULL", 0 ), Event_MsgBox_Close )
		return
	end
	
	ToCharacterCreateStage()

end

-- 삭제 버튼 클릭 이벤트
function Event_CharSelect_btnDelete_Click()

	local tbSelect = GetArray( WIDGET_ID_CHARSELECT, "arrSelectedIndex", "charInfo.listChar" )
	if 0 == table.getn( tbSelect ) then
		OpenMsgBox( MSG_BOX_TYPE_OK, GetGameExternalText( "CHARACTERSTAGE_NOT_SELECTED", 0 ), Event_MsgBox_Close )
		return
	end
	
	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, GetGameExternalText( "CHARACTERSTAGE_CAUTION_DELETE_DAUM", 0 ), 
		Event_CharSelect_btnDelete_DeleteCount, Event_MsgBox_Close )
	
end

-- 2차비번을 사용하면 비번확인창을 사용하고 2차비번을 사용하지 않는경우 무조건 삭제
function Ev_CharSelect_BtnDel_SecondPassOpen()

	Event_MsgBox_Close()
	
	local tbSelect = GetArray( WIDGET_ID_CHARSELECT, "arrSelectedIndex", "charInfo.listChar" )
	if 0 == table.getn( tbSelect ) then
		OpenMsgBox( MSG_BOX_TYPE_OK, GetGameExternalText( "CHARACTERSTAGE_NOT_SELECTED", 0 ), Event_MsgBox_Close )
		return
	end
	
	if g_CharSelect:Get2ndPassType() == 1 then
		
		SecondPassword_Open(3)
		
	elseif g_CharSelect:Get2ndPassType() == 0 then
	
		Event_CharSelect_btnDelete_DeleteCount()
		
	end
	
end

-- 삭제 확인창에서 확인 버튼 클릭시 10초 카운트 이벤트
function Event_CharSelect_btnDelete_DeleteCount()

	Event_MsgBox_Close()
	
	OpenMsgBox( MSG_BOX_TYPE_CANCEL, GetGameExternalText( "WAITSERVER_DEL_CHARACTER", 0 ),
		nil, Event_MsgBox_Close, nil, nil, 10, Event_CharSelect_DelCharComplete )
	
end

-- 서버변경 버튼 클릭 이벤트
function Event_CharSelect_btnServer_Click()

	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, GetGameExternalText( "CHARACTERSTAGE_6", 0 ), 
		Event_CharSelect_Disconnect_Server, Event_MsgBox_Close )

end

-- 게임시작 버튼 클릭 이벤트
function Event_CharSelect_btnStart_Click()
	
	local tbSelect = GetArray( WIDGET_ID_CHARSELECT, "arrSelectedIndex", "charInfo.listChar" )
	if 0 == table.getn( tbSelect ) then
	
		OpenMsgBox( MSG_BOX_TYPE_OK, GetGameExternalText( "CHARACTERSTAGE_8", 0 ),
			Event_MsgBox_Close, Event_MsgBox_Close )
	
		return
	
	end
	
	local nIndex = tbSelect[ 1 ]
	
	-- 로비 캐릭터 선택
	local nSelectPage = math.floor( nIndex / 3 )
	local nSelectNum = nIndex % 3
	g_CharSelect:SelectLobyChar( nSelectPage, nSelectNum, nIndex )
	
	g_Msg:JoinGame( nIndex )
	
end

-- 캐릭터 삭제 타이머 완료 이벤트
function Event_CharSelect_DelCharComplete()

	local tbSelect = GetArray( WIDGET_ID_CHARSELECT, "arrSelectedIndex", "charInfo.listChar" )
	if 0 == table.getn( tbSelect ) then
		return
	end
	
	g_CharSelect:DeleteChar( tbSelect[ 1 ], g_strSecondPassword )	
	
	Event_MsgBox_Close()
	
end

-- pk 성향 오류, 서버 접속 끊기
function Event_CharSelect_Disconnect_Server()

	g_CharSelect:DisconnectServer()
	ToServerSelectStage()
	
	-- 서버 연결을 끊어버리니 연결 시도해본다.
	TryConnectLoginServer()
	
end

-- 캐릭터 접속 에러 메시지 박스 오픈
function OpenMsgBox_CharSelect( strText, funcEvent )

	Event_MsgBox_Close()

	OpenMsgBox( MSG_BOX_TYPE_OK, strText, funcEvent )
	
end

-- 캐릭터 접속 취소 이벤트
function Event_CharSelect_JoinCancel()

	Event_MsgBox_Close()
	g_CharSelect:DisconnectServer()

end

-- 캐릭터 접속 타이머 이벤트
function Event_CharSelect_JoinTimer()

	Event_MsgBox_Close()

	OpenMsgBox( MSG_BOX_TYPE_OK, GetGameExternalText( "CHARACTERSTAGE_DAUM_GAME_JOIN_FAIL", 0 ), 
		Event_CharSelect_JoinCancel, Event_CharSelect_JoinCancel )
	
end

---------------------------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 캐릭터 리스트 초기화
function CharSelect_InitSelectCharList()

	if false == IsLoadWidget( WIDGET_ID_CHARSELECT ) then
	
		g_bCharSelect_InitList = true
		return
		
	end

	g_tbCharSelect_List = {}

	-- 캐릭터 리스트
	-- { { 이름, 레벨, 학원, 클래스, 종료지역, exp, 성향, 클럽이름, 클럽랭크 }, ... }
	g_tbCharSelect_List = g_CharSelect:GetLobyCharInfoTable()
	
	if g_bFirstLogin == true then 
	
		-- 캐릭터가 하나도 없다면 캐릭터 생성 스테이지로 이동
		if 0 == table.getn( g_tbCharSelect_List ) then
		
			g_bFirstLogin = false
			ToCharacterCreateStage()
			
			return
			
		end
		
	end
	
	if 0 == table.getn( g_tbCharSelect_List ) then
		
		SetString( WIDGET_ID_CHARSELECT, "text", "", "charInfo.tfSelectInfo" )
		RemoveData( WIDGET_ID_CHARSELECT, 0, "charInfo.listChar", true )
		
		return
		
	end
	
	local tbCharListData = {}
	for value in pairs( g_tbCharSelect_List ) do
	
		-- 캐릭터 정보
		local tbCharInfo = g_tbCharSelect_List[ value ]
		
		local tbCharInfoData = {}
		tbCharInfoData[ 1 ] = "Lv." .. tostring( tbCharInfo[ 2 ] ) .. "   " .. GetCharClassName( tbCharInfo[ 4 ] ) .. 
			"\n" .. tbCharInfo[ 1 ]
		tbCharInfoData[ 2 ] = tostring( tbCharInfo[ 1 ] )							-- 이름
		tbCharInfoData[ 3 ] = tostring( tbCharInfo[ 2 ]	)							-- 레벨
		tbCharInfoData[ 4 ] = GetGameWordText( "ACADEMY_NAME", tbCharInfo[ 3 ] )	-- 학원
		tbCharInfoData[ 5 ] = tostring( tbCharInfo[ 4 ] )							-- 클랙스
		tbCharInfoData[ 6 ] = tostring( tbCharInfo[ 5 ]	)							-- 종료지역
		tbCharInfoData[ 7 ] = tostring( tbCharInfo[ 6 ]	)							-- exp
		tbCharInfoData[ 8 ] = tostring( tbCharInfo[ 7 ]	)							-- 성향
		tbCharInfoData[ 9 ] = tostring( tbCharInfo[ 8 ]	)							-- 클럽이름
		tbCharInfoData[ 10 ] = tostring( tbCharInfo[ 9 ] )							-- 클럽랭크
		tbCharInfoData[ 11 ] = tostring( tbCharInfo[ 10 ] )							-- 모바일 연동 여부
		table.insert( tbCharListData, tbCharInfoData )
		
	end
	
	-- 리스트 데이터 설정
	SetDataEx( WIDGET_ID_CHARSELECT, "label1;name;level;school;class;map;exp;atb;clubName;clubRank;isMobile",
		tbCharListData, "charInfo.listChar", false )
		
	CallFunction( WIDGET_ID_CHARSELECT, "SelectClear", "", "charInfo.listChar" )
	CallFunction( WIDGET_ID_CHARSELECT, "validateNow", "", "charInfo.listChar" )
	
	for value in pairs( g_tbCharSelect_List ) do
	
		local strInst = "charInfo.listChar.mcItemRenderer.item" .. (value - 1)
		
		local tbCharInfo = g_tbCharSelect_List[ value ]
		
		local nClass = tbCharInfo[ 4 ]
		local bNone = false
		
		-- 해당 클래스에 맞는 프레임으로 이동
		local strFrameID = ""
		if nClass == CHARCLASS_FIGHTER_M then
			strFrameID = "_BR_M_"
		elseif nClass == CHARCLASS_FIGHTER_W then
			strFrameID = "_BR_F_"
		elseif nClass == CHARCLASS_ARMS_M then
			strFrameID = "_SW_M_"
		elseif nClass == CHARCLASS_ARMS_W then
			strFrameID = "_SW_F_"
		elseif nClass == CHARCLASS_ARCHER_M then
			strFrameID = "_AR_M_"
		elseif nClass == CHARCLASS_ARCHER_W then
			strFrameID = "_AR_F_"
		elseif nClass == CHARCLASS_SPIRIT_M then
			strFrameID = "_SH_M_"
		elseif nClass == CHARCLASS_SPIRIT_W then
			strFrameID = "_SH_F_"
		elseif nClass == CHARCLASS_EXTREAM_M then
			strFrameID = "_EX_M_"
		elseif nClass == CHARCLASS_EXTREAM_W then
			strFrameID = "_EX_F_"
		elseif nClass == CHARCLASS_SCIENTIST_M then
			strFrameID = "_SC_M_"
		elseif nClass == CHARCLASS_SCIENTIST_W then
			strFrameID = "_SC_F_"
		elseif nClass == CHARCLASS_ASSASSIN_M then
			strFrameID = "_AS_M_"
		elseif nClass == CHARCLASS_ASSASSIN_W then
			strFrameID = "_AS_F_"
		elseif nClass == CHARCLASS_TRICKER_M then
			strFrameID = "_TR_M_"
		elseif nClass == CHARCLASS_TRICKER_W then
			strFrameID = "_TR_F_"
		elseif nClass == CHARCLASS_ACTOR_M then
			strFrameID = "_DR_M_"
		elseif nClass == CHARCLASS_ACTOR_W then
			strFrameID = "_DR_F_"
		else
			strFrameID = "none"
			bNone = true
		end

		if false == bNone then
		
			strFrameID = strFrameID .. "on"
			
		end
		
		CallFunction( WIDGET_ID_CHARSELECT, "gotoAndStop", strFrameID, strInst .. ".charImg" )
		
		if true == g_UserFeatures:IsUsingRanMobile() then
		
			local nIsMobile = tbCharInfo[ 10 ]
			SetBool( WIDGET_ID_CHARSELECT, "visible", 1 == nIsMobile, strInst .. ".mcMobileImg" )
			
		else
		
			SetBool( WIDGET_ID_CHARSELECT, "visible", false, strInst .. ".mcMobileImg" )
		
		end
	
	end
	
	-- 새캐릭터
	local str = GetGameWordText( "SELECT_CHARACTER_PAGE_BUTTON", 0 ) .. 
		"(" .. tostring( g_Login:GetCharRemain() ) .. ")"
	SetString( WIDGET_ID_CHARSELECT, "label", str, "charInfo.btns.btnNewChar" )
	
	-- 란 모바일 정보 설정
	CharSelect_SetRanMobileInfo()

end

-- 캐릭터 접속 에러 메시지 
function CharSelect_JoinFail( nError )

	-- 시스템 에러
	if 0 == nError then
	
		OpenMsgBox_CharSelect( GetGameExternalText( "CHARACTERSTAGE_11", 0 ), Event_MsgBox_CloseGame )
		
	-- 접속중
	elseif 1 == nError then
	
		OpenMsgBox_CharSelect( GetGameExternalText( "CHARACTERSTAGE_10", 0 ), Event_MsgBox_CloseGame )
		
	-- pk 성향 오류
	elseif 2 == nError then
	
		OpenMsgBox_CharSelect( GetGameExternalText( "CHARACTERSTAGE_12", 0 ), Event_CharSelect_Disconnect_Server )
		
	-- 아직 저장중인 캐릭터가 있음.
	elseif 3 == nError then
	
		OpenMsgBox_CharSelect( GetGameExternalText( "CHARACTERSTAGE_14", 0 ), Event_MsgBox_Close )
	
	-- 잠시 후 다시 시도해주세요
	elseif 4 == nError then
	
		OpenMsgBox_CharSelect( GetGameExternalText( "CHARACTERSTAGE_15", 0 ), Event_MsgBox_Close )
		
	-- 아직 저장중인 캐릭터가 있어 접속 불가
	elseif 5 == nError then
	
		OpenMsgBox_CharSelect( GetGameExternalText( "CHARACTERSTAGE_14", 0 ), Event_MsgBox_Close )
	
	-- 랜덤옵션에 문제 있는 캐릭터
	elseif 6 == nError then
	
		OpenMsgBox_CharSelect( GetGameExternalText( "CHARACTERSTAGE_RANDOMOPTION_ERROR", 0 ), Event_MsgBox_Close )
	
	-- GS 전용, 변술부 게임 접속 제어
	elseif 7 == nError then
	
		OpenMsgBox_CharSelect( GetGameExternalText( "CHARACTERSTAGE_BLOCK_CLASS", 0 ), Event_MsgBox_Close )
		
	end	

end

-- 캐릭터 삭제 성공
function CharSelect_SucceedDelChar()

	OpenMsgBox_CharSelect( GetGameExternalText( "CHAR_DEL_SUCCEED", 0 ), Event_MsgBox_Close )
	CharSelect_InitSelectCharList()
	
end

-- 캐릭터 삭제 실패
function CharSelect_FailDelChar( bClub )
	local str = ""
	if true == bClub then
	
		str = GetGameExternalText( "CHAR_DEL_CLUB_FAILED", 0 )
	
	else
	
		str = GetGameExternalText( "CHAR_DEL_FAILED", 0 )
	
	end
	
	OpenMsgBox( MSG_BOX_TYPE_OK, str, Event_MsgBox_Close, Event_MsgBox_Close )

end

-- 캐릭터 직접 선택
function CharSelect_SetSelectCharIndex( nCharSelect, nCharStart )
	
	CallFunction( WIDGET_ID_CHARSELECT, "Select", tostring( nCharStart + nCharSelect ), "charInfo.listChar" )
	CallFunction( WIDGET_ID_CHARSELECT, "ScrollToSelect", "", "charInfo.listChar" )

end

-- 캐릭터 더블클릭으로 접속
function CharSelect_DClickJoin()

	OpenMsgBox( MSG_BOX_TYPE_CANCEL, GetGameExternalText( "CHARACTERSTAGE_GAME_JOIN", 0 ), 
		nil, Event_CharSelect_JoinCancel, nil, nil, 60, Event_CharSelect_JoinTimer )

end

-- 란 모바일 계정 정보 받음
function CharSelect_RecvRanMobileInfo()

	g_bCharSelect_RecvRanMobileInfo = true
	
	if 0 == table.getn( g_tbCharSelect_List ) then
		return
	end
	
	CharSelect_SetRanMobileInfo()

end