
-- 파티 멤버 리스트
--{
--	{
--		1 : 캐릭터 ID
--		2 : 클래스 ID
--		3 : 캐릭터 이름
--	}
--	...
--}
g_tbPartyRecruit_MemberList = {}
-- 이미 가입요청 했나?
g_bPartyRecruit_Once = false
-- 파티 링크 인덱스
g_nPartyRecruit_LinkIndex = -1

-- 파티 지원 답장
g_tbPartyRecruit_AnswerQueue = {}

-- 파티 모집 로드 완료
function LoadPartyRecruit()

	-- 구분
	local str = GetGameWordText( "PARTY_LINK_WINDOW", 1 )
	SetString( WIDGET_ID_PARTYRECRUIT, "text", str, "labelType" )
	
	-- 부서
	str = GetGameWordText( "PARTY_LINK_WINDOW", 2 )
	SetString( WIDGET_ID_PARTYRECRUIT, "text", str, "labelClass" )
	
	-- 이름
	str = GetGameWordText( "PARTY_LINK_WINDOW", 3 )
	SetString( WIDGET_ID_PARTYRECRUIT, "text", str, "labelName" )
	
	-- 레벨
	str = GetGameWordText( "PRIVATE_MARKET_ITEM_REQUIRE", 1 )
	SetString( WIDGET_ID_PARTYRECRUIT, "text", str, "labelLevel" )

	-- 파티 지원
	str = GetGameWordText( "PARTY_LINK_WINDOW", 4 )
	SetString( WIDGET_ID_PARTYRECRUIT, "label", str, "btn" )
	SetEvent( WIDGET_ID_PARTYRECRUIT, EVENT_BUTTON_CLICK, "Ev_PartyRecruit_Request", "", "btn" )

end

-- 파티 모집 오픈
function OpenPartyRecruit()

	PartyRecruit_SetInfo()

end

-- 파티 링크 정보 설정
function PartyRecruit_SetInfo()

	-- 가입 요청 버튼 활성 설정
	SetBool( WIDGET_ID_PARTYRECRUIT, "enabled", not g_bPartyRecruit_Once, "btn" )
	
	-- 리스트 설정
	local tbListData = {}
	for i = 1, PARTY_MAX, 1 do
	
		local strItem = "item" .. tostring( i )
		
		local tbMember = g_tbPartyRecruit_MemberList[ i ]
		if nil == tbMember then
		
			SetString( WIDGET_ID_PARTYRECRUIT, "text", "", strItem .. ".tfClass" )
			SetString( WIDGET_ID_PARTYRECRUIT, "text", "", strItem .. ".tfName" )
			SetString( WIDGET_ID_PARTYRECRUIT, "text", "", strItem .. ".tfLevel" )
		
		else
		
			-- 클래스 이름
			SetString( WIDGET_ID_PARTYRECRUIT, "text", GetCharClassName( tbMember[ 2 ] ), strItem .. ".tfClass" )
			-- 캐릭터 이름
			SetString( WIDGET_ID_PARTYRECRUIT, "text", tbMember[ 3 ], strItem .. ".tfName" )
			-- 레벨
			SetString( WIDGET_ID_PARTYRECRUIT, "text", "Lv " .. tostring( tbMember[ 4 ] ), strItem .. ".tfLevel" )
			
			local tbData = {}
			
			-- 첫번째 멤버는 항상 마스터다.
			if 1 == i then
			
				table.insert( tbData, "M" )
				
			-- 나머지 멤버 넘버링
			else
			
				table.insert( tbData, tostring( i - 1 ) )
			
			end
			table.insert( tbListData, tbData )
		
		end
	
	end
	
	SetDataEx( WIDGET_ID_PARTYRECRUIT, "label", tbListData, "list", false )

end

------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 파티 링크 클릭
function PartyRecruit_Open( tbMemberList, bOnce, nLinkIndex )

	g_tbPartyRecruit_MemberList = tbMemberList
	g_bPartyRecruit_Once = bOnce
	g_nPartyRecruit_LinkIndex = nLinkIndex
	
	if false == IsOpenWidget( WIDGET_ID_PARTYRECRUIT ) then
	
		OpenWidget( WIDGET_ID_PARTYRECRUIT )
		
	else
	
		PartyRecruit_SetInfo()
	
	end

end

-- 파티 링크 지원 요청 받음
function PartyRecruit_RequestRecv( dwGaeaID, nClass, strName, dwHashKey, dwTimeKey, dwMasterID )

	local str = GetCharClassName( nClass ) .. "/" .. strName
	
	local tbData = 
	{
		dwHashKey,
		dwTimeKey,
		dwGaeaID
	}
	table.insert( g_tbPartyRecruit_AnswerQueue, tbData )
	
	local tbOkData = { true }
	local tbCancelData = { false }
	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, str, Ev_PartyRecruit_RequestAnswer, tbOkData, tbCancelData )

end

------------------------------------------------------------------------------------------------------
-- 이벤트

-- 파티 지원 요청
function Ev_PartyRecruit_Request()

	if 0 == table.getn( g_tbPartyRecruit_MemberList ) then
		return
	end

	-- 파티 마스터
	local tbMember = g_tbPartyRecruit_MemberList[ 1 ]

	-- 파티 요청
	g_Msg:ReqPartyLinkJoin( tbMember[ 1 ], g_nPartyRecruit_LinkIndex )
	
	g_bPartyRecruit_Once = true
	
	CloseWidget( WIDGET_ID_PARTYRECRUIT )

end

-- 지원 요청 답변
function Ev_PartyRecruit_RequestAnswer( tbData )

	-- 데이터 없다.
	if 0 == table.getn( g_tbPartyRecruit_AnswerQueue ) then
		return
	end

	local nHashKey = g_tbPartyRecruit_AnswerQueue[ 1 ][ 1 ]
	local nTimeKey = g_tbPartyRecruit_AnswerQueue[ 1 ][ 2 ]
	local nGaeaID = g_tbPartyRecruit_AnswerQueue[ 1 ][ 3 ]
	
	g_Msg:ReqPartyLinkLure( tbData[ 1 ], nHashKey, nTimeKey, nGaeaID )
	Event_MsgBox_Close()
	
	table.remove( g_tbPartyRecruit_AnswerQueue, 1 )

end
