
g_nPartySlot_buffDragRightOffset = 251

g_nPartySlot_buffMiniSize = 16
g_nPartySlot_buffLargeSize = 36
g_fPartySlot_buffOrigScale = 0

g_nPartySlot_MousePosX = 0
g_nPartySlot_MousePosY = 0
g_nPartySlot_PosX = 0
g_nPartySlot_PosY = 0
g_nPartySlot_DragBottom = 0

g_bPartySlot_IsOpen = false

g_nPartySlot_ShowBuffIndex = -1

-- 파티 슬롯 UI 로드 완료
function LoadPartySlot()

	SetEvent( WIDGET_ID_PARTYSLOT, EVENT_WIDGET_CLOSE, "ClosePartySlot", "", "" )
	
	-- 드래그 이벤트
	SetEvent( WIDGET_ID_PARTYSLOT, EVENT_MOUSE_DOWN, "Ev_PartySlot_DragStart", "stageX;stageY;", "" )
	
	-- 하이라이트 끄기
	HitTestEnable( WIDGET_ID_PARTYSLOT, false, "mcHighlight" )
	SetBool( WIDGET_ID_PARTYSLOT, "visible", false, "mcHighlight" )
	
	-- 원본 스케일
	g_fPartySlot_buffOrigScale = g_nPartySlot_buffMiniSize / g_nPartySlot_buffLargeSize

	for i = 0, PARTY_MAX - 1, 1 do
	
		local strInst = "partySlot" .. tostring( i )
		SetInt( WIDGET_ID_PARTYSLOT, "minimum", 0, strInst .. ".barHP" )
		SetInt( WIDGET_ID_PARTYSLOT, "maximum", 100, strInst .. ".barHP" )
		SetInt( WIDGET_ID_PARTYSLOT, "value", 0, strInst .. ".barHP" )
		
		-- 대상에게 스킬 사용
		SetEvent( WIDGET_ID_PARTYSLOT, EVENT_MOUSE_CLICK, "Ev_PartySlot_MouseClick", "buttonIdx;", strInst, tostring( i ) )
		-- 버프 보기
		SetEvent( WIDGET_ID_PARTYSLOT, EVENT_MOUSE_ROLLOVER, "Ev_PartySlot_RollOver", "", strInst, tostring( i ) )
		SetEvent( WIDGET_ID_PARTYSLOT, EVENT_MOUSE_ROLLOUT, "Ev_PartySlot_RollOut", "", strInst, tostring( i ) )
		
		HitTestEnable( WIDGET_ID_PARTYSLOT, false, strInst .. ".mcBuff" )
		
		for j = 1, PARTY_MEMBER_BUFF_SLOT_TOTAL, 1 do
		
			local strSlotInst = strInst .. ".mcBuff.slot" .. tostring( j )
			
			SetString( WIDGET_ID_PARTYSLOT, "binding", Party_MakeBuffSlotID( i, j ), strSlotInst )
			SetBool( WIDGET_ID_PARTYSLOT, "visible", false, strSlotInst )
		
		end
	
	end

end

function OpenPartySlot()

	g_bPartySlot_IsOpen = true
	
	PartySlot_SetVisibleBuff( g_bParty_ShowBuff )
	-- 새로 열렸으니 업데이트 한번 하자
	CF_Party_UpdateSlot()

end

function ClosePartySlot()

	g_bPartySlot_IsOpen = false

end

-- 파티원 버프 visible 설정
function PartySlot_SetVisibleBuff( bVisible )

	for i = 0, PARTY_MAX - 1, 1 do
	
		SetBool( WIDGET_ID_PARTYSLOT, "visible", bVisible, "partySlot" .. tostring( i ) .. ".mcBuff" )
	
	end
	
	PartySlot_UIPosition()

end

-----------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 파티 생성
function PartySlot_NewParty()

	if false == g_bParty_ShowPartySlot then
		return
	end

	if false == IsLoadWidget( WIDGET_ID_PARTYSLOT ) then
	
		OpenWidget( WIDGET_ID_PARTYSLOT )
		
	else
	
		if false == IsOpenWidget( WIDGET_ID_PARTYSLOT ) then
			OpenWidget( WIDGET_ID_PARTYSLOT )
		end
	
		PartySlot_UpdateList()
		
	end
	
end

-- 파티 슬롯 리스트 업데이트
function PartySlot_UpdateList()

	if false == IsLoadWidget( WIDGET_ID_PARTYSLOT ) then
		return
	end
	
	local nHeight = GetInt( WIDGET_ID_PARTYSLOT, "height", "partySlot0" )
	g_nPartySlot_DragBottom = 0
	
	for i = 0, PARTY_MAX - 1, 1 do
	
		local tbMember = g_tbParty_MemberList[ i + 1 ]
		if nil ~= tbMember then
		
			local strInst = "partySlot" .. tostring( i ) .. "."
			
			PartySlot_LabelUpdate( i + 1 )
			
			-- 캐릭터 이미지
			CallFunction( WIDGET_ID_PARTYSLOT, "gotoAndStop", Party_GetCharImgParam( tbMember ), strInst .. "imgChar" )
			-- 학원 이미지
			CallFunction( WIDGET_ID_PARTYSLOT, "gotoAndStop", 
				Party_GetSchoolImgFrameID( tbMember[ "wSchool" ] ), strInst .. "mcSchool" )			
			
			SetBool( WIDGET_ID_PARTYSLOT, "visible", true, "partySlot" .. tostring( i ) )
			
		else
		
			SetBool( WIDGET_ID_PARTYSLOT, "visible", false, "partySlot" .. tostring( i ) )
			g_nPartySlot_DragBottom = g_nPartySlot_DragBottom + nHeight
		
		end
	
	end
	
	local nScreenHeight = GetScreenHeight()
	local nY = GetInt( WIDGET_ID_PARTYSLOT, "y", "" )
	
	nHeight = GetInt( WIDGET_ID_PARTYSLOT, "height", "" )
	
	if nScreenHeight + g_nPartySlot_DragBottom < nY + nHeight then
	
		nY = nScreenHeight - nHeight + g_nPartySlot_DragBottom
		
		SetInt( WIDGET_ID_PARTYSLOT, "y", nY, "" )
		
	end

end

-- 파티 해산
function PartySlot_Dissolve()
	CloseWidget( WIDGET_ID_PARTYSLOT )
end

-- 파티원 맵 이동
function PartySlot_MemberMoveMap( dwIndex )

	if false == IsLoadWidget( WIDGET_ID_PARTYSLOT ) then
		return
	end

	local tbMember = g_tbParty_MemberList[ dwIndex + 1 ]
	if nil == tbMember then
		return
	end
	
	local strInst = "partySlot" .. tostring( dwIndex ) .. "."
	
	-- 캐릭터 이미지
	CallFunction( WIDGET_ID_PARTYSLOT, "gotoAndStop", Party_GetCharImgParam( tbMember ), strInst .. "imgChar" )
	
	PartySlot_LabelUpdate( dwIndex + 1 )

end

function PartySlot_LabelUpdate( nIndex )

	local tbMember = g_tbParty_MemberList[ nIndex ]
	if nil == tbMember then
		return
	end
	
	local strInst = "partySlot" .. tostring( nIndex - 1 ) .. "."
	
	local strName = tbMember[ "strName" ]
	local strHP = tostring( tbMember[ "dwNowHP" ] ) .. "/" .. tostring( tbMember[ "dwMaxHP" ] )
	local nHPRatio = math.floor( tbMember[ "dwNowHP" ] / tbMember[ "dwMaxHP" ] * 100 )
	local bDie = false
	local bSameMap = true
	
	local tbMyChar = Party_FindMyChar()
	if tbMember[ "dwNowHP" ] == 0 then
	
		strHP = HTMLFontColor( strHP, HTML_TEXT_COLOR_RED )
		strName = HTMLFontColor( strName, HTML_TEXT_COLOR_RED )
		
		bDie = true
		bSameMap = Party_IsSameMap( tbMyChar, tbMember )
		
	else
	
		if false == Party_IsSameMap( tbMyChar, tbMember ) then
		
			strName = HTMLFontColor( strName, HTML_TEXT_COLOR_GRAY )
			nHPRatio = 100
			bSameMap = false
			
		else
		
			strHP = HTMLFontColor( strHP, HTML_TEXT_COLOR_WHITE )
			strName = HTMLFontColor( strName, HTML_TEXT_COLOR_WHITE )
			
		end
		
	end
	
	SetString( WIDGET_ID_PARTYSLOT, "htmlText", strName, strInst .. "labelName" )
	
	SetBool( WIDGET_ID_PARTYSLOT, "visible", bSameMap, strInst .. "labelHP" )
	SetString( WIDGET_ID_PARTYSLOT, "htmlText", strHP, strInst .. "labelHP" )
	SetDouble( WIDGET_ID_PARTYSLOT, "value", nHPRatio, strInst .. "barHP" )
	
	SetBool( WIDGET_ID_PARTYSLOT, "visible", bDie, strInst .. "imgChar.mcDead" )
	
end

-- 파티원 정보 업데이트
function PartySlot_UpdateMember( dwIndex )

	if false == IsLoadWidget( WIDGET_ID_PARTYSLOT ) then
		return
	end

	local tbMember = g_tbParty_MemberList[ dwIndex + 1 ]
	if nil == tbMember then
		return
	end
	
	PartySlot_LabelUpdate( dwIndex + 1 )

	-- 버프 정보 추가
	local tbBuff = g_Client:SfReqPartyMemberGetSkillFactInfo( tbMember[ "dwGaeaID" ] )
	for value in pairs( tbBuff ) do
	
		local tb = tbBuff[ value ]
		Party_AddBuff( tbMember[ "dwGaeaID" ], tb[ 1 ], tb[ 2 ] )
	
	end
end

-- 파티원 이름 변경
function PartySlot_MemberRename( dwIndex )

	if false == IsLoadWidget( WIDGET_ID_PARTYSLOT ) then
		return
	end

	local tbMember = g_tbParty_MemberList[ dwIndex + 1 ]
	if nil == tbMember then
		return
	end
	
	SetString( WIDGET_ID_PARTYSLOT, "text", tbMember[ "strName" ], 
		"partySlot" .. tostring( dwIndex ) .. ".labelName" )

end

-- 파티원 버프 추가
function PartySlot_AddBuff( nMemberIndex, nBuffIndex, tbBuff )

	if false == g_bPartySlot_IsOpen then
		return
	end
	
	local strSlotInst = "partySlot" .. tostring( nMemberIndex - 1 ) .. ".mcBuff.slot" .. tostring( nBuffIndex )
	
	SetBool( WIDGET_ID_PARTYSLOT, "visible", true, strSlotInst )
	SetSlot_Skill( Party_MakeBuffSlotID( nMemberIndex - 1, nBuffIndex ), tbBuff[ 1 ] )

end

-- 파티원 버프 제거
function PartySlot_RemoveBuff( nMemberIndex, nBuffIndex, tbBuffList )

	if false == g_bPartySlot_IsOpen then
		return
	end
	
	-- 마지막 버프가 아닐 경우 뒤에 등록된 버프 한칸씨 당겨주기
	if nBuffIndex ~= table.getn( tbBuffList ) + 1 then
	
		for i = nBuffIndex, table.getn( tbBuffList ), 1 do
	
			local strSlotInst = "partySlot" .. tostring( nMemberIndex - 1 ) .. ".mcBuff.slot" .. tostring( i )
			SetBool( WIDGET_ID_PARTYSLOT, "visible", true, strSlotInst )
			
			local tbBuff = tbBuffList[ i ]
			SetSlot_Skill( Party_MakeBuffSlotID( nMemberIndex - 1, i ), tbBuff[ 1 ] )
		
		end
		
		-- 마지막 슬롯 초기화를 위해 남은 버프 테이블 개수에서 하나 더하기
		nBuffIndex = table.getn( tbBuffList ) + 1
	
	end
	
	local strSlotInst = "partySlot" .. tostring( nMemberIndex - 1 ) .. ".mcBuff.slot" .. tostring( nBuffIndex )
	
	SetBool( WIDGET_ID_PARTYSLOT, "visible", false, strSlotInst )
	ResetSlot( Party_MakeBuffSlotID( nMemberIndex - 1, nBuffIndex ) )

end

-- 파티원 모든 버프 제거
function PartySlot_RemoveAllBuff( nMemberIndex )

	if false == IsLoadWidget( WIDGET_ID_PARTYSLOT ) then
		return
	end
	
	for i = 1, PARTY_MEMBER_BUFF_SLOT_TOTAL, 1 do
		
		local strSlotInst = "partySlot" .. tostring( nMemberIndex - 1 ) .. ".mcBuff.slot" .. tostring( i )
		
		SetBool( WIDGET_ID_PARTYSLOT, "visible", false, strSlotInst )
		ResetSlot( Party_MakeBuffSlotID( nMemberIndex - 1, i ) )
	
	end

end

-- 위치 잡기
function PartySlot_UIPosition( nX, nY )

	if nil == nX then
		nX = GetInt( WIDGET_ID_PARTYSLOT, "x", "" )
	end
	
	if nil == nY then
		nY = GetInt( WIDGET_ID_PARTYSLOT, "y", "" )
	end

	local nWidth = GetInt( WIDGET_ID_PARTYSLOT, "width", "partySlot0.bg" )
	local nHeight = GetInt( WIDGET_ID_PARTYSLOT, "height", "" )

	if true == g_bParty_ShowBuff then
		nWidth = nWidth + g_nPartySlot_buffDragRightOffset
	end
	
	local nLeft, nTop, nBottom, nRight = 0, 0, g_nPartySlot_DragBottom, 0
	
	local nScreenWidth = GetScreenWidth()
	local nScreenHeight = GetScreenHeight()
	
	if nLeft > nX then
		nX = nLeft
	end
	
	if nTop > nY then
		nY = nTop
	end
	
	if nScreenWidth + nRight < nX + nWidth then
		nX = nScreenWidth - nWidth + nRight
	end
	
	if nScreenHeight + nBottom < nY + nHeight then
		nY = nScreenHeight - nHeight + nBottom
	end
	
	SetInt( WIDGET_ID_PARTYSLOT, "x", nX, "" )
	SetInt( WIDGET_ID_PARTYSLOT, "y", nY, "" )
	
end

-----------------------------------------------------------------------------------------------------------
-- 이벤트

-- 드래그 시작
function Ev_PartySlot_DragStart( nStageX, nStageY )

	InsertStageMouseMove( Ev_PartySlot_Dragging )
	InsertStageMouseUp( Ev_PartySlot_DragEnd )
	
	g_nPartySlot_MousePosX = nStageX
	g_nPartySlot_MousePosY = nStageY
	
	g_nPartySlot_PosX = GetInt( WIDGET_ID_PARTYSLOT, "x", "" )
	g_nPartySlot_PosY = GetInt( WIDGET_ID_PARTYSLOT, "y", "" )

end

-- 드래그 중
function Ev_PartySlot_Dragging( nStageX, nStageY )

	local nOffsetX = nStageX - g_nPartySlot_MousePosX
	local nOffsetY = nStageY - g_nPartySlot_MousePosY
	PartySlot_UIPosition( g_nPartySlot_PosX + nOffsetX, g_nPartySlot_PosY + nOffsetY )

end

-- 드래그 끝
function Ev_PartySlot_DragEnd( nStageX, nStageY )

	RemoveStageMouseMove( Ev_PartySlot_Dragging )
	RemoveStageMouseUp( Ev_PartySlot_DragEnd )
	
end

-- 대상에게 스킬 사용
function Ev_PartySlot_MouseClick( nbutton, strIndex )

	if nbutton ~= 1 then
		return
	end
	
	local nIndex = tonumber( strIndex )
	
	local tbMember = g_tbParty_MemberList[ nIndex + 1 ]
	g_MyChar:PartyMemberSkillReaction( tbMember[ "dwGaeaID" ] )
	
end

-- 버프 보여주기
function Ev_PartySlot_RollOver( strIndex )

	g_nPartySlot_ShowBuffIndex = tonumber( strIndex )
	
	-- 하이라이트도 켜기
	SetBool( WIDGET_ID_PARTYSLOT, "visible", true, "mcHighlight" )
	
	local nY = GetInt( WIDGET_ID_PARTYSLOT, "y", "partySlot" .. tostring( g_nPartySlot_ShowBuffIndex ) )
	SetInt( WIDGET_ID_PARTYSLOT, "y", nY, "mcHighlight" )
	
	-- 타이머 설정
	local tbQuickSet = 
	{
		time = 250,
		delay = 0,
		ease = EASE_NONE,
		loop = false,
		onComplete = "Ev_PartySlot_ShowBuff"
	}
	SetTween( WIDGET_ID_PARTYSLOT, tbQuickSet, "", "", "" )

end

-- 버프 보여주기
function Ev_PartySlot_ShowBuff()

	if -1 == g_nPartySlot_ShowBuffIndex then
		return
	end
	
	-- 버프 보기
	local strInst = "partySlot" .. tostring( g_nPartySlot_ShowBuffIndex ) .. ".mcBuff"
	SetBool( WIDGET_ID_PARTYSLOT, "visible", true, strInst )
	
	-- 버프 사이즈 크게
	for i = 1, PARTY_MEMBER_BUFF_SLOT_TOTAL, 1 do
	
		local strSlotInst = strInst .. ".slot" .. tostring( i )
		
		-- 스케일 기본 비율로
		SetDouble( WIDGET_ID_PARTYSLOT, "scaleX", 1.0, strSlotInst )
		SetDouble( WIDGET_ID_PARTYSLOT, "scaleY", 1.0, strSlotInst )
		
		-- 사이즈 변했으니 위치 다시 잡아준다.
		local nNewX = ((i - 1) % PARTY_MEMBER_BUFF_SLOT_COL) * g_nPartySlot_buffLargeSize
		local nNewY = math.floor( (i - 1) / PARTY_MEMBER_BUFF_SLOT_COL ) * g_nPartySlot_buffLargeSize
		SetInt( WIDGET_ID_PARTYSLOT, "x", nNewX, strSlotInst )
		SetInt( WIDGET_ID_PARTYSLOT, "y", nNewY, strSlotInst )
	
	end

end

-- 버프 보기 끄기
function Ev_PartySlot_RollOut( strIndex )

	g_nPartySlot_ShowBuffIndex = -1
	
	-- 하이라이트도 끄기
	SetBool( WIDGET_ID_PARTYSLOT, "visible", false, "mcHighlight" )

	-- 타이머 초기화
	local tbQuickSet = 
	{
		time = 0,
		delay = 0,
		ease = EASE_NONE,
		loop = false,
		onComplete = ""
	}
	SetTween( WIDGET_ID_PARTYSLOT, tbQuickSet, "", "", "" )
	
	-- 버프 보기 상태 원래대로
	local strInst = "partySlot" .. strIndex .. ".mcBuff"
	SetBool( WIDGET_ID_PARTYSLOT, "visible", g_bParty_ShowBuff, strInst )
	
	-- 버프 사이즈 작게
	for i = 1, PARTY_MEMBER_BUFF_SLOT_TOTAL, 1 do
	
		local strSlotInst = strInst .. ".slot" .. tostring( i )
		
		-- 스케일 원래 비율로
		SetDouble( WIDGET_ID_PARTYSLOT, "scaleX", g_fPartySlot_buffOrigScale, strSlotInst )
		SetDouble( WIDGET_ID_PARTYSLOT, "scaleY", g_fPartySlot_buffOrigScale, strSlotInst )

		-- 사이즈 변했으니 위치 다시 잡아준다.
		local nNewX = ((i - 1) % PARTY_MEMBER_BUFF_SLOT_COL) * g_nPartySlot_buffMiniSize
		local nNewY = math.floor( (i - 1) / PARTY_MEMBER_BUFF_SLOT_COL ) * g_nPartySlot_buffMiniSize
		SetInt( WIDGET_ID_PARTYSLOT, "x", nNewX, strSlotInst )
		SetInt( WIDGET_ID_PARTYSLOT, "y", nNewY, strSlotInst )
	
	end

end