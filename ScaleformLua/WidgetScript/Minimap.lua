
g_strMinimap_Name = ""			-- 맵 이름
g_strMinimap_Texture = ""		-- 맵 텍스처 파일 이름

g_tbMinimap_ZoneName = {}
g_tbMinimap_Gate = {}
g_tbMinimap_CTFAuthenticator = {}
g_tbMinimap_Post = {}
g_tbMinimap_Bus = {}
g_tbMinimap_Locker = {}
g_tbMinimap_StartPos = {}
g_tbMinimap_ClubMan = {}
g_tbMinimap_Npc = {}
g_tbMinimap_Move = {}

g_fMinimap_Scale = 1.0				-- 현재 미니맵 확대 비율
g_fMinimap_SclaeInterval = 0.1		-- 미니맵 확대/축소시 변화량
g_fMinimap_SclaeMax = 1.5			-- 미니맵 최대 확대 비율
g_fMinimap_SclaeMin = 0.5			-- 미니맵 최소 확대 비율

g_strMinimap_ClubName = ""
g_dwMinimap_ServerID = -1
g_dwMinimap_ClubID = -1

g_bMinimap_Fold = true
g_bMinimap_Open = false

function IsOpenMinimap()

	return g_bMinimap_Open
	
end

-- 미니맵 로드 완료
function LoadMinimap()
	-- UI 닫기 이벤트
	SetEvent( WIDGET_ID_MINIMAP, EVENT_WIDGET_CLOSE, "CloseMinimap", "", "" )
	
	-- 기본적으로 미니맵은 접어 놓도록 한다.
	Ev_Minimap_btnFold()

	-- 전체 화면 버튼 이벤트
	SetEvent( WIDGET_ID_MINIMAP, EVENT_BUTTON_CLICK, "Ev_Minimap_btnFullScreen", "", "btnFullScreen" )
	-- 미니맵 접기
	SetEvent( WIDGET_ID_MINIMAP, EVENT_BUTTON_CLICK, "Ev_Minimap_btnFold", "", "btnFold" )
	-- 미니맵 펼치기
	SetEvent( WIDGET_ID_MINIMAP, EVENT_BUTTON_CLICK, "Ev_Minimap_btnUnFold", "", "btnUnFold" )
	-- 확대 버튼 이벤트
	SetEvent( WIDGET_ID_MINIMAP, EVENT_BUTTON_CLICK, "Ev_Minimap_btnScaleUp", "", "btnScaleUp" )
	-- 축소 버튼 이벤트
	SetEvent( WIDGET_ID_MINIMAP, EVENT_BUTTON_CLICK, "Ev_Minimap_btnScaleDown", "", "btnScaleDown" )
	-- 원래 배율로 버튼 이벤트
	SetEvent( WIDGET_ID_MINIMAP, EVENT_BUTTON_CLICK, "Ev_Minimap_btnCurrent", "", "btnCurrent" )
	-- 미니맵 클릭시 캐릭터 이동
	SetEvent( WIDGET_ID_MINIMAP, EVENT_MOUSE_CLICK, "Ev_MiniMap_MapClick", "buttonIdx;stageX;stageY", "map" )
	
	-- 시크릿 맵 끄기
	SetBool( WIDGET_ID_MINIMAP, "visible", false, "mcSecretMap" )
	
	-- 채널 텍스트 설정
	local strChannel = "[" .. tostring( g_Login.m_nServerChannel ) .. "]"
	SetString( WIDGET_ID_MINIMAP, "text", strChannel, "labelChannel" )

	HitTestEnable( WIDGET_ID_MINIMAP, false, "labelPos" )

end

-- 미니맵 오픈
function OpenMinimap()
	
	g_bMinimap_Open = true
	
	local nMapID = g_MyChar:GetCurrentMapID()
	g_strMinimap_Name = GetMapName( nMapID )
	
	local strTexture = g_Minimap:GetMapImgFile( nMapID )
	if nil == strTexture or 0 == string.len( strTexture ) then
	
		g_strMinimap_Texture = ""
	
	else
	
		g_strMinimap_Texture = GetMinimapTexturePath() .. strTexture
	
	end
	
	-- 현재 맵 이름 설정
	Minimap_SetMapName()
	-- 현재 좌표 설정
	Minimap_SetPosition()
	-- 현재 시간 설정
	Minimap_SetDate()	
	-- 카메라 방향 설정
	Minimap_SetCameraDir()
	-- 아이콘 생성
	Minimap_CreateIcon()
	-- 맵 이미지 로드
	Minimap_LoadMapImg()
	-- 미니맵 내 위치 설정
	Minimap_SetMyPos()
	
	Minimap_LoadClubMarkImage()
	
	Minimap_SetCTFVictorySchool()
	
	InsertFrameMove( FrameMove_Map )
	Ev_Minimap_btnCurrent()
	-- 난사군도 승전국 설정
	Minimap_SetVictoriousCountry( g_VCountry:GetVCountryFirst( 0 ), g_VCountry:GetVCountryNameFirst( 0 ) )
	Minimpa_UpdateVictoryIconPosInfo()
	-- 장비 소모품 위치 설정
	EquipConsume_UIPosition()
	
end

-- 클로즈 이벤트
function CloseMinimap()

	g_bMinimap_Open = false
	RemoveFrameMove( FrameMove_Map )
	
end

-- 현재 맵 이름 설정
function Minimap_SetMapName()
	
	if 	nil == g_strMinimap_Name or
		"" == g_strMinimap_Name then
		return
	end
	
	SetString( WIDGET_ID_MINIMAP, "text", g_strMinimap_Name, "labelName" )

end

-- 현재 시간 설정
function Minimap_SetDate()

	if nil == g_tbMap_Date then	
		g_tbMap_Date = GetDate()
	end

	local strDate = string.format( "%02d/%02d  %02d:%02d",
		g_tbMap_Date[ "nMonth" ], g_tbMap_Date[ "nDay" ],
		g_tbMap_Date[ "nHour" ], g_tbMap_Date[ "nMinute" ] )
		
	SetString( WIDGET_ID_MINIMAP, "text", strDate, "labelDate" )
	SetString( WIDGET_ID_MINIMAP, "text", strDate, "labelDateFold" )	

end

-- 현재 좌표 설정
function Minimap_SetPosition()

	if nil == g_tbMap_CurPos then
		g_tbMap_CurPos = g_MyChar:GetMapPosition()
	end
	
	if 	nil == g_tbMap_CurPos[ "x" ] or
		nil == g_tbMap_CurPos[ "y" ] then
		return
	end

	local strPos = string.format( "%d/%d", g_tbMap_CurPos[ "x" ], g_tbMap_CurPos[ "y" ] )
	SetString( WIDGET_ID_MINIMAP, "text", strPos, "labelPos" )
	SetString( WIDGET_ID_MINIMAP, "text", strPos, "labelPosFold" )

end

-- 플레이어 아이콘 설정
function Minimap_SetPlayerIcon()

	-- 기존 플레이어 아이콘 삭제를 위해 레이어 제거
	CallFunction( WIDGET_ID_MINIMAP, "RemoveLayer", g_strMap_LAYER_PLAYER, "map" )
	-- 플레이어 레이어 추가
	CallFunction( WIDGET_ID_MINIMAP, "AddLayer", g_strMap_LAYER_PLAYER, "map" )
	
	-- 플레이어 아이콘 추가
	local strDataToString = g_strMap_LAYER_PLAYER .. ";" .. g_strMap_ICON_PLAYER .. ";" .. "_iconPlayer" .. ";;;;;;"
	CallFunction( WIDGET_ID_MINIMAP, "AddIcon", strDataToString, "map" )
	
	-- 플레이어 방향 아이콘 추가
	strDataToString = g_strMap_LAYER_PLAYER .. ";" .. g_strMap_ICON_PLAYERDIR .. ";" .. "_iconPlayerDir" .. ";;;;;;"
	CallFunction( WIDGET_ID_MINIMAP, "AddIcon", strDataToString, "map" )
	
	strDataToString = CreateParam( g_strMap_LAYER_PLAYER, g_strMap_ICON_PLAYERDIR, "0" )
	CallFunction( WIDGET_ID_MINIMAP, "SetHitTestEnable", strDataToString, "map" )
	
	-- 플레이어 이동 목표지점 아이콘 설정
	local strParam = g_strMap_LAYER_PLAYER .. ";" .. g_strMap_ICON_PLAYERMOVE .. ";" .. "_iconPlayerMove" .. ";;;;;;"
	CallFunction( WIDGET_ID_MINIMAP, "AddIcon", strParam, "map" )

end

-- 맵 이미지 로드
function Minimap_LoadMapImg()

	if 	nil == g_strMinimap_Texture or 0 == string.len( g_strMinimap_Texture ) then
		return
	end

	CallFunction( WIDGET_ID_MINIMAP, "LoadMap", g_strMinimap_Texture, "map" )

end

-- 미니맵 내 위치 설정
function Minimap_SetMyPos()

	if nil == g_tbMap_MapMyPos then
		g_tbMap_MapMyPos = g_MyChar:GetMapPositionOffset()
	end
	
	if 	nil == g_tbMap_MapMyPos[ "x" ] or
		nil == g_tbMap_MapMyPos[ "y" ] then
		return
	end
	
	-- 맵 이미지 이동
	local nXOffset = (g_tbMap_MapMyPos[ "x" ] * g_fMinimap_Scale - (GetInt( WIDGET_ID_MINIMAP, "width", "map" ) / 2)) * -1
	local nYOffset = (g_tbMap_MapMyPos[ "y" ] * g_fMinimap_Scale - (GetInt( WIDGET_ID_MINIMAP, "height", "map" ) / 2)) * -1
	
	local strDataToString = tostring( nXOffset ) .. ";" .. tostring( nYOffset ) .. ";"
	CallFunction( WIDGET_ID_MINIMAP, "MoveMap", strDataToString, "map" )
	
	-- 내 캐릭터 아이콘 가운데로 이동
	Minimap_MoveIcon( g_strMap_LAYER_PLAYER, g_strMap_ICON_PLAYER,
		g_tbMap_MapMyPos[ "x" ], g_tbMap_MapMyPos[ "y" ], 0, 0 )
		
	-- 방향 아이콘도 같이 따라다녀야 하니 재설정
	Minimap_SetCameraDir()
	
	-- 이동 목표 지점이 있으면 목표 지점과 충분히 가까워졌을 경우 숨기기
	if nil ~= g_tbMap_MovePos then
	
		-- 두 점 사이 거리 구하기
		local nLength = math.sqrt( math.pow( g_tbMap_MapMyPos[ "x" ] - g_tbMap_MovePos[ "x" ], 2 ) + 
			math.pow( g_tbMap_MapMyPos[ "y" ] - g_tbMap_MovePos[ "y" ], 2 ) )
			
		if 2 > nLength then
		
			local strParam = g_strMap_LAYER_PLAYER .. ";" .. g_strMap_ICON_PLAYERMOVE .. ";" .. "0" .. ";"
			CallFunction( WIDGET_ID_MINIMAP, "SetVisibleIcon", strParam, "map" )
			
			g_tbMap_MovePos = nil
		
		end
		
	end
end

-- 카메라 방향 설정
function Minimap_SetCameraDir()

	if nil == g_tbMap_CameraDir then
		g_tbMap_CameraDir = GetCameraDir()
	end
	
	if nil == g_tbMap_MapMyPos then
		return
	end
	
	if 	nil == g_tbMap_MapMyPos[ "x" ] or
		nil == g_tbMap_MapMyPos[ "y" ] then
		return
	end
	
	-- 캐릭터 위치로 이동
	Minimap_MoveIcon( g_strMap_LAYER_PLAYER, g_strMap_ICON_PLAYERDIR, 
		g_tbMap_MapMyPos[ "x" ], g_tbMap_MapMyPos[ "y" ], 0, 0 )
		
	-- 아이콘 회전
	local strParam = CreateParam( g_strMap_LAYER_PLAYER, g_strMap_ICON_PLAYERDIR, g_fMap_CameraThetaY )
	CallFunction( WIDGET_ID_MINIMAP, "RotateIcon", strParam, "map" )

end

-- 지역명 텍스트 설정
function Minimap_SetZoneName()

	-- 기존 지역명 데이터 삭제를 위해 레이어 제거
	CallFunction( WIDGET_ID_MINIMAP, "RemoveLayer", g_strMap_LAYER_TEXT, "map" )
	-- 지역명 추가를 위해 다시 레이어 추가
	CallFunction( WIDGET_ID_MINIMAP, "AddLayer", g_strMap_LAYER_TEXT, "map" )
	
	-- 현재 맵에 설정된 지역명 정보를 가져와 텍스트 아이콘 추가
	g_tbMinimap_ZoneName = g_Minimap:GetMapZoneName( g_MyChar:GetCurrentMapID() )
	for value in pairs( g_tbMinimap_ZoneName ) do
	
		local tbZoneData = g_tbMinimap_ZoneName[ value ]
	
		local strTextID = "zone" .. tostring( value )
		local strDataToString = g_strMap_LAYER_TEXT .. ";" .. strTextID .. ";" .. "MapTextYellow" .. ";" .. 
			tbZoneData[ "strName" ]
		CallFunction( WIDGET_ID_MINIMAP, "AddTextIcon", strDataToString, "map" )
		
		-- 텍스트 이동
		Minimap_MoveIcon( g_strMap_LAYER_TEXT, strTextID, tbZoneData[ "x" ], tbZoneData[ "y" ], 0, 0 )
	
	end

end

-- 게이트 아이콘 설정
function Minimap_SetGateIcon()

	-- 기존 게이트 아이콘 삭제를 위해 레이어 제거
	CallFunction( WIDGET_ID_MINIMAP, "RemoveLayer", g_strMap_LAYER_GATE, "map" )
	-- 게이트 추가를 위해 다시 레이어 추가
	CallFunction( WIDGET_ID_MINIMAP, "AddLayer", g_strMap_LAYER_GATE, "map" )
	
	-- 현재 맵 게이트 정보를 가져와 아이콘 추가
	g_tbMinimap_Gate = g_Minimap:GetMapGateName( g_MyChar:GetCurrentMapID() )
	for value in pairs( g_tbMinimap_Gate ) do
	
		local tbGateData = g_tbMinimap_Gate[ value ]
		
		local strGateID = "gate" .. tostring( value )
		local strDataToString = g_strMap_LAYER_GATE .. ";" .. strGateID .. ";" .. 
			"_iconGate" .. ";" .. tbGateData[ "strName" ] .. ";"
		CallFunction( WIDGET_ID_MINIMAP, "AddIcon", strDataToString, "map" )
		
		-- 아이콘 이동
		Minimap_MoveIcon( g_strMap_LAYER_GATE, strGateID, tbGateData[ "x" ], tbGateData[ "y" ], 0, 0 )
	
	end

end

-- CTF 단말기 아이콘 설정
function Minimap_SetAuthenticatorIcon()	
	
	g_tbMinimap_CTFAuthenticator = g_Minimap:GetAuthenticatorInfo(g_MyChar:GetCurrentMapID())
--	if false == IsOpenWidget( WIDGET_ID_MINIMAP ) then
--		return
--	end
	
	-- 기존 게이트 아이콘 삭제를 위해 레이어 제거
	CallFunction( WIDGET_ID_MINIMAP, "RemoveLayer", g_strMap_LAYER_CTF, "map" )
	CallFunction( WIDGET_ID_MINIMAP, "AddLayer", g_strMap_LAYER_CTF, "map" )
	
	for value in pairs( g_tbMinimap_CTFAuthenticator ) do
		local tbAuth = g_tbMinimap_CTFAuthenticator[ value ]
		local strAuthactorID = "authenticator"..tostring( value )
		local strParam = g_strMap_LAYER_CTF .. ";" .. strAuthactorID .. ";" 
						.. "_iconCTFCaptureInfo" .. ";" .. ""
		CallFunction( WIDGET_ID_MINIMAP, "AddIcon", strParam, "map" )
		
		strParam = g_strMap_LAYER_CTF..";"..strAuthactorID..";"..tostring(tbAuth["emSchool"]+1)
		CallFunction( WIDGET_ID_MINIMAP, "gotoAndStopIcon", strParam, "map")
		-- 아이콘 이동
		Minimap_MoveIcon( g_strMap_LAYER_CTF, strAuthactorID, tbAuth[ "PosX" ], tbAuth[ "PosY" ], 0, 0 )
	end
end

-- 인증기 아이콘 설정
function Minimap_SetAuth( tbList, nType )

	-- 기존 아이콘 삭제를 위해 레이어 제거
	CallFunction( WIDGET_ID_MINIMAP, "RemoveLayer", g_strMap_LAYER_AUTH, "map" )
	CallFunction( WIDGET_ID_MINIMAP, "AddLayer", g_strMap_LAYER_AUTH, "map" )
	
	for value in pairs( tbList ) do
	
		local tbData = tbList[ value ]
		
		local strIconLinkID = ""
		
		if g_nAuthType_CTI == nType then
		
			if 0 == tbData[ 1 ] then
			
				strIconLinkID = "_iconCTINone"
			
			elseif 1 == tbData[ 1 ] then
			
				strIconLinkID = "_iconCTIMine"
			
			elseif 2 == tbData[ 1 ] then
			
				strIconLinkID = "_iconCTIOther"
			
			elseif 3 == tbData[ 1 ] then
			
				strIconLinkID = "_iconCTIAccelerator"
				
			end
		
		elseif g_nAuthType_Guidance == nType then
		
			if 0 == tbData[ 1 ] then
			
				strIconLinkID = "_iconGuidanceNone"
			
			elseif 1 == tbData[ 1 ] then
			
				strIconLinkID = "_iconGuidanceMine"
				
			elseif 2 == tbData[ 1 ] then
			
				strIconLinkID = "_iconGuidanceNeutrality"
			
			elseif 3 == tbData[ 1 ] then
			
				strIconLinkID = "_iconGuidanceOther"
			
			elseif 4 == tbData[ 1 ] then
			
				strIconLinkID = "_iconGuidanceAccelerator"
				
			end
		
		end
	
		local strIconID = "iconAuth" .. tostring( value )
		
		local strParam = CreateParam( g_strMap_LAYER_AUTH, strIconID, strIconLinkID )
		CallFunction( WIDGET_ID_MINIMAP, "AddIcon", strParam, "map" )
		
		-- 아이콘 이동
		Minimap_MoveIcon( g_strMap_LAYER_AUTH, strIconID, tbData[ 2 ], tbData[ 3 ], 0, 0 )
	
	end

end

-- 난사군도 보급품, 인증기 아이콘 설정
function Minimap_SetCTIAuth( tbList )

	if false == g_bMinimap_Open then
		return
	end
	
	Minimap_SetAuth( tbList, g_nAuthType_CTI )

end

-- 선도전 인증기, 가속기 아이콘 설정
function Minimap_SetGuidanceAuth( tbList )

	if false == g_bMinimap_Open then
		return
	end
	
	Minimap_SetAuth( tbList, g_nAuthType_Guidance )

end

-- 우편함 아이콘 설정
function Minimap_SetPostIcon()

	-- 기존 우편함 아이콘 삭제를 위해 레이어 제거
	CallFunction( WIDGET_ID_MINIMAP, "RemoveLayer", g_strMap_LAYER_POST, "map" )
	-- 우편함 추가를 위해 다시 레이어 추가
	CallFunction( WIDGET_ID_MINIMAP, "AddLayer", g_strMap_LAYER_POST, "map" )
	
	-- 현재 우편함 정보를 가져와 아이콘 추가
	g_tbMinimap_Post = g_Minimap:GetMapPost( g_MyChar:GetCurrentMapID() )
	for value in pairs( g_tbMinimap_Post ) do
	
		local tbPostData = g_tbMinimap_Post[ value ]
		
		local strPostID = "post" .. tostring( value )
		local strDataToString = g_strMap_LAYER_POST .. ";" .. strPostID .. ";" .. 
			"_iconPost" .. ";" .. tbPostData[ "strName" ] .. ";"
		CallFunction( WIDGET_ID_MINIMAP, "AddIcon", strDataToString, "map" )
		
		-- 아이콘 이동
		Minimap_MoveIcon( g_strMap_LAYER_POST, strPostID, tbPostData[ "x" ], tbPostData[ "y" ], 0, 0 )
	
	end

end

-- NPC 아이콘 설정
function Minimap_SetNpcIcon()

	-- 기존 NPC 아이콘 삭제를 위해 레이어 제거
	CallFunction( WIDGET_ID_MINIMAP, "RemoveLayer", g_strMap_LAYER_NPC, "map" )
	-- NPC 추가를 위해 다시 레이어 추가
	CallFunction( WIDGET_ID_MINIMAP, "AddLayer", g_strMap_LAYER_NPC, "map" )
	
	-- 현재 NPC 정보를 가져와 아이콘 추가
	g_tbMinimap_Npc = g_Minimap:GetMapNormalNpc( g_MyChar:GetCurrentMapID() )
	for value in pairs( g_tbMinimap_Npc ) do

		local tbNpcData = g_tbMinimap_Npc[ value ]
		
		local strIconID = "npc" .. tostring( value )
		local strDataToString = g_strMap_LAYER_NPC .. ";" .. strIconID .. ";"
		
		-- 퀘스트 타입 정보가 있다면 퀘스트 NPC
		if nil ~= tbNpcData[ "nQuestType" ] then
		
			if MAP_NPC_QUEST_START == tbNpcData[ "nQuestType" ] then
			
				strDataToString = strDataToString .. "_iconQuestStart" .. ";"
			
			elseif MAP_NPC_QUEST_STEP == tbNpcData[ "nQuestType" ] then
			
				strDataToString = strDataToString .. "_iconQuestStep" .. ";"
				
			else
			
				strDataToString = strDataToString .. "_iconQuestFinish" .. ";"
			
			end
		
		-- 마켓 정보가 있고 마켓 이용 가능하다면 마켓 NPC
		elseif 	nil ~= tbNpcData[ "bMarket" ] and
				true == tbNpcData[ "bMarket" ] then
			
			strDataToString = strDataToString .. "_iconMarket" .. ";"
			
		-- 그 외 대화 가능한 NPC는 일반 NPC
		else
		
			strDataToString = strDataToString .. "_iconNormal" .. ";"
		
		end
		
		strDataToString = strDataToString .. tbNpcData[ "strName" ] .. ";"
		CallFunction( WIDGET_ID_MINIMAP, "AddIcon", strDataToString, "map" )
		
		-- 아이콘 이동
		Minimap_MoveIcon( g_strMap_LAYER_NPC, strIconID, tbNpcData[ "x" ], tbNpcData[ "y" ], 0, 0 )
	
	end

end

-- NPC 대화 액션 타입에 따른 아이콘 설정
function Minimap_SetTalkNpcIcon( strLayer, strIcon, strIconClass, nTalkType )

	-- 기존 아이콘 삭제를 위해 레이어 제거
	CallFunction( WIDGET_ID_MINIMAP, "RemoveLayer", strLayer, "map" )
	-- 아이콘 추가를 위해 다시 레이어 추가
	CallFunction( WIDGET_ID_MINIMAP, "AddLayer", strLayer, "map" )
	
	-- 현재 정보를 가져와 아이콘 추가
	local tbNpcTalk = g_Minimap:GetMapTalkNpc( g_MyChar:GetCurrentMapID(), nTalkType )
	for value in pairs( tbNpcTalk ) do
	
		local tbNpcTalkData = tbNpcTalk[ value ]
		
		local strIconID = strIcon .. tostring( value )
		local strDataToString = strLayer .. ";" .. strIconID .. ";" .. strIconClass .. ";" .. 
			tbNpcTalkData[ "strName" ] .. ";"
		CallFunction( WIDGET_ID_MINIMAP, "AddIcon", strDataToString, "map" )
		
		-- 아이콘 이동
		Minimap_MoveIcon( strLayer, strIconID, tbNpcTalkData[ "x" ], tbNpcTalkData[ "y" ], 0, 0 )
	
	end
	
	return tbNpcTalk

end

-- 아이콘 위치 이동
function Minimap_MoveIcon( strLayer, strIcon, nPosX, nPosY, nOffsetX, nOffsetY )

	local strDataToString = strLayer .. ";" .. strIcon .. ";" .. 
		tostring( nPosX * g_fMinimap_Scale + nOffsetX ) .. ";" .. 
		tostring( nPosY * g_fMinimap_Scale + nOffsetY ) .. ";"
	CallFunction( WIDGET_ID_MINIMAP, "MoveIcon", strDataToString, "map" )
	
end

-- 아이콘 생성 및 위치 잡아주기
function Minimap_CreateIcon()

	-- 게이트 아이콘 설정
	Minimap_SetGateIcon()
	-- 우편함 아이콘 설정
	Minimap_SetPostIcon()
	-- NPC 아이콘 설정
	Minimap_SetNpcIcon()
	-- 버스 아이콘 설정
	g_tbMinimap_Bus = Minimap_SetTalkNpcIcon( g_strMap_LAYER_BUS, "bus", "_iconBus", EM_BUSSTATION )
	-- 라커 아이콘 설정
	g_tbMinimap_Locker = Minimap_SetTalkNpcIcon( g_strMap_LAYER_LOCKER, "locker", "_iconLocker", EM_STORAGE )
	-- 시작위치 아이콘 설정
	g_tbMinimap_StartPos = Minimap_SetTalkNpcIcon( g_strMap_LAYER_STARTPOS, "startPos", "_iconStartPos", EM_STARTPOINT )
	-- 클럽 매니저 아이콘 설정
	g_tbMinimap_ClubMan = Minimap_SetTalkNpcIcon( g_strMap_LAYER_CLUBMAN, "clubMan", "_iconClub", EM_CLUB_STORAGE )
	-- 플레이어 아이콘 설정
	Minimap_SetPlayerIcon()	
	-- 지역명 설정
	Minimap_SetZoneName()
	-- CTF 단말기
	Minimap_SetAuthenticatorIcon()
	
	-- 표시 설정 체크 상태에 따라 레이어 visible of/off
	Minimap_SetLayerVisible( g_strMap_LAYER_GATE, g_bMap_VisibleGate )
	Minimap_SetLayerVisible( g_strMap_LAYER_NPC, g_bMap_VisibleNpc )
	Minimap_SetLayerVisible( g_strMap_LAYER_TEXT, g_bMap_VisibleZoneName )
	Minimap_SetLayerVisible( g_strMap_LAYER_BUS, g_bMap_VisibleBus )
	Minimap_SetLayerVisible( g_strMap_LAYER_LOCKER, g_bMap_VisibleLocker )
	Minimap_SetLayerVisible( g_strMap_LAYER_STARTPOS, g_bMap_VisibleStartPos )
	Minimap_SetLayerVisible( g_strMap_LAYER_CLUBMAN, g_bMap_VisibleClubMan )
	Minimap_SetLayerVisible( g_strMap_LAYER_POST, g_bMap_VisiblePost )
	--!! 파티 레이어 추가
	--Minimap_SetLayerVisible( g_strMap_LAYER_, g_bMap_VisibleParty )

end

-- 해당 레이어 모든 아이콘 위치 업데이트
function Minimap_UpdateIconPos( tbIcon, strLayer, strIcon )

	for value in pairs( tbIcon ) do
		local tbData = tbIcon[ value ]
		Minimap_MoveIcon( strLayer, strIcon .. tostring( value ), tbData[ "x" ], tbData[ "y" ], 0, 0 )
	end

end

-- 모든 아이콘 다시 이동 (맵 스케일 변경 등으로 인해..)
function Minimap_MoveIconAll()
	
	Minimap_UpdateIconPos( g_tbMinimap_ZoneName, g_strMap_LAYER_TEXT, "zone" )
	Minimap_UpdateIconPos( g_tbMinimap_Gate, g_strMap_LAYER_GATE, "gate" )
	Minimap_UpdateIconPos( g_tbMinimap_Post, g_strMap_LAYER_POST, "post" )
	Minimap_UpdateIconPos( g_tbMinimap_Bus, g_strMap_LAYER_BUS, "bus" )
	Minimap_UpdateIconPos( g_tbMinimap_Locker, g_strMap_LAYER_LOCKER, "locker" )
	Minimap_UpdateIconPos( g_tbMinimap_StartPos, g_strMap_LAYER_STARTPOS, "startPos" )
	Minimap_UpdateIconPos( g_tbMinimap_ClubMan, g_strMap_LAYER_CLUBMAN, "clubMan" )
	Minimap_UpdateIconPos( g_tbMinimap_Npc, g_strMap_LAYER_NPC, "npc" )
	
	if nil ~= g_tbMap_MovePos then
		
		Minimap_MoveIcon( g_strMap_LAYER_PLAYER, g_strMap_ICON_PLAYERMOVE, g_tbMap_MovePos["x"], g_tbMap_MovePos["y"], 0, 0 )
		
	end
	
end

-- 레이어 visible 설정
function Minimap_SetLayerVisible( strLayer, bVisible )

	local strParam = strLayer .. ";"
	if false == bVisible then
		strParam = strParam .. "0"
	else
		strParam = strParam .. "1"
	end
	
	CallFunction( WIDGET_ID_MINIMAP, "SetVisibleLayer", strParam, "map" )

end

-------------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 맵 이동
function Minimap_MoveActiveMap( strMapName, strMapTexture, bSecretMap, dwServerID, dwClubID, strClubName )

	if nil == strMapTexture or 0 == string.len( strMapTexture ) then
	
		bSecretMap = true
		g_strMinimap_Texture = ""
		
	else
	
		g_strMinimap_Texture = GetMinimapTexturePath() .. strMapTexture
	
	end

	g_bMap_SecretMap = bSecretMap

	g_strMinimap_Name = strMapName
	
	g_strMinimap_ClubName = strClubName
	
	g_dwMinimap_ServerID = dwServerID
	g_dwMinimap_ClubID = dwClubID
	
	if false == IsOpenMinimap() then
		return
	end
	
	if false == g_bMap_SecretMap then
	
		SetBool( WIDGET_ID_MINIMAP, "visible", false, "mcSecretMap" )
	
		Minimap_LoadMapImg()
		Minimap_CreateIcon()
	
	end
	
	Minimap_SetMapName()
	Minimap_LoadClubMarkImage()
	MiniMap_Update()
	
	Minimpa_UpdateVictoryIconPosInfo()
	
end

-- 난사군도 승전국 설정
function Minimap_SetVictoriousCountry( wCountryID, strCountryName )
	local str = Display_GetCountryFrameID( wCountryID )
	CallFunction( WIDGET_ID_MINIMAP, "gotoAndPlay", str, "victoryIconInfo.mcCountryImg" )
	
	if str == "none" then	
		str = ""		
	else	
		str = strCountryName .. "\n" .. GetGameInternalText( "MINIMAP_INFO_CTIMARK", 0 )
	end
	
	SetBool( WIDGET_ID_MINIMAP, "visible", str ~= "", "victoryIconInfo.mcCountryImg")
	SetString( WIDGET_ID_MINIMAP, "TooltipText", str, "victoryIconInfo.mcCountryImg" )
end

function Minimap_SetCTFVictorySchool()
	local dwSchool = g_CTField:GetCTFCaptureSchool()
	local bVisible = true
	if dwSchool == SCHOOL_SUNGMOON then
		CallFunction( WIDGET_ID_MINIMAP, "gotoAndStop", "SCHOOL0", "victoryIconInfo.imgSchool" )
	elseif dwSchool == SCHOOL_HYUNAM then
		CallFunction( WIDGET_ID_MINIMAP, "gotoAndStop", "SCHOOL1", "victoryIconInfo.imgSchool" )
	elseif dwSchool == SCHOOL_BONGHWANG then
		CallFunction( WIDGET_ID_MINIMAP, "gotoAndStop", "SCHOOL2", "victoryIconInfo.imgSchool" )
	else
		CallFunction( WIDGET_ID_MINIMAP, "gotoAndStop", "NONE", "victoryIconInfo.imgSchool" )
		bVisible = false
	end
	SetBool( WIDGET_ID_MINIMAP, "visible", bVisible, "victoryIconInfo.imgSchool")
	SetString( WIDGET_ID_MINIMAP, "TooltipText"
			, GetGameInternalText( "MINIMAP_INFO_CTFMARK", 0 ), "victoryIconInfo.imgSchool" )
end

-- 클럽 마크 이미지 로드
function Minimap_LoadClubMarkImage()
	local tbGuidClubInfo = g_Client:GetCurrentMapGuidClubInfo()
	g_dwMinimap_ClubID = tbGuidClubInfo[1]
	g_strMinimap_ClubName = tbGuidClubInfo[2]
	g_dwMinimap_ServerID = g_MyChar.dwServerID
	
	if "" == g_strMinimap_ClubName then
		SetString( WIDGET_ID_MINIMAP, "TooltipText", "", "victoryIconInfo.imgClub" )
	else
		local strParam = CreateParam( "ClubIcon_RT_MC", "ClubIcon_RT", g_dwMinimap_ServerID, g_dwMinimap_ClubID, 16, 11 )
		UpdateClubMark( WIDGET_ID_MINIMAP, strParam, "victoryIconInfo.rtClubIcon" )
		SetString( WIDGET_ID_MINIMAP, "TooltipText", g_strMinimap_ClubName, "victoryIconInfo.rtClubIcon" )
	end

	local bVisible = g_strMinimap_ClubName ~= ""
	SetBool( WIDGET_ID_MINIMAP, "visible", bVisible, "victoryIconInfo.rtClubIcon" )
	SetBool( WIDGET_ID_MINIMAP, "visible", bVisible, "victoryIconInfo.imgClub" )

end

function Minimpa_UpdateVictoryIconPosInfo()
	-- sequence->   country-school-club
	-- width 16, term 1
	local strInstName = {
		"victoryIconInfo.mcCountryImg",
		"victoryIconInfo.imgSchool",
		"victoryIconInfo.rtClubIcon"		
	}
	
	local strSubInstName = {
		"",
		"",
		"victoryIconInfo.imgClub"
	}
	local bVictoryInfo = {
		GetBool(WIDGET_ID_MINIMAP, "visible", "victoryIconInfo.mcCountryImg"),
		GetBool(WIDGET_ID_MINIMAP, "visible", "victoryIconInfo.imgSchool"),
		GetBool(WIDGET_ID_MINIMAP, "visible", "victoryIconInfo.rtClubIcon")
	}
--	for k,v in pairs(bVictoryInfo) do
--	end
	local nWidth = 0
	for i = 1, 3, 1 do
		if bVictoryInfo[i] == true then
			SetInt(WIDGET_ID_MINIMAP, "x", nWidth, strInstName[i])
			if strSubInstName[i] ~= nil and strSubInstName[i] ~= "" then
				SetInt(WIDGET_ID_MINIMAP, "x", nWidth, strSubInstName[i])
			end
			nWidth = nWidth + 17
		end
	end
	nWidth = nWidth + 1
	local nMapNameTbPosX =  GetInt(WIDGET_ID_MINIMAP, "x", "victoryIconInfo") + nWidth
	SetInt( WIDGET_ID_MINIMAP, "x", nMapNameTbPosX, "labelName")
end


-------------------------------------------------------------------------------------------------------------
-- 이벤트

-- 전체화면 버튼 이벤트
function Ev_Minimap_btnFullScreen()

	-- 튜토리얼 중인지 확인
	if true == g_BaseTutorial:IsTutorial() then
	
		-- 튜토리얼 중에는 사용할 수 없습니다.
		CF_PrintMsg_SystemMsgBox( GetGameInternalText( "TUTORIAL_CANNOT_USE", 0 ), TEXT_COLOR_RED )
		
	-- 아니라면 화면모드 전환
	else
	
		ChangeScreenMode()
		
	end

end

-- 미니맵 접기
function Ev_Minimap_btnFold()

	SetBool( WIDGET_ID_MINIMAP, "visible", false, "labelDate" )
	SetBool( WIDGET_ID_MINIMAP, "visible", false, "btnScaleUp" )
	SetBool( WIDGET_ID_MINIMAP, "visible", false, "btnScaleDown" )
	SetBool( WIDGET_ID_MINIMAP, "visible", false, "btnCurrent" )
	SetBool( WIDGET_ID_MINIMAP, "visible", false, "labelPos" )
	SetBool( WIDGET_ID_MINIMAP, "visible", false, "map" )
	SetBool( WIDGET_ID_MINIMAP, "visible", false, "bg" )
	SetBool( WIDGET_ID_MINIMAP, "visible", false, "bgMap" )
	SetBool( WIDGET_ID_MINIMAP, "visible", false, "btnFold" )
	SetBool( WIDGET_ID_MINIMAP, "visible", false, "btn_img" )
	
	SetBool( WIDGET_ID_MINIMAP, "visible", true, "bgFold" )
	SetBool( WIDGET_ID_MINIMAP, "visible", true, "labelDateFold" )
	SetBool( WIDGET_ID_MINIMAP, "visible", true, "labelPosFold" )
	SetBool( WIDGET_ID_MINIMAP, "visible", true, "btnUnFold" )	
	
	g_bMinimap_Fold = true
	
	EquipConsume_UIPosition()
	MiniMap_Update()
	
end

-- 미니맵 펼치기
function Ev_Minimap_btnUnFold()

	SetBool( WIDGET_ID_MINIMAP, "visible", true, "labelDate" )
	SetBool( WIDGET_ID_MINIMAP, "visible", true, "btnScaleUp" )
	SetBool( WIDGET_ID_MINIMAP, "visible", true, "btnScaleDown" )
	SetBool( WIDGET_ID_MINIMAP, "visible", true, "btnCurrent" )
	SetBool( WIDGET_ID_MINIMAP, "visible", true, "labelPos" )
	SetBool( WIDGET_ID_MINIMAP, "visible", true, "map" )
	SetBool( WIDGET_ID_MINIMAP, "visible", true, "bg" )
	SetBool( WIDGET_ID_MINIMAP, "visible", true, "bgMap" )
	SetBool( WIDGET_ID_MINIMAP, "visible", true, "btnFold" )
	SetBool( WIDGET_ID_MINIMAP, "visible", true, "btn_img" )
	
	SetBool( WIDGET_ID_MINIMAP, "visible", false, "bgFold" )
	SetBool( WIDGET_ID_MINIMAP, "visible", false, "labelDateFold" )
	SetBool( WIDGET_ID_MINIMAP, "visible", false, "labelPosFold" )
	SetBool( WIDGET_ID_MINIMAP, "visible", false, "btnUnFold" )	
	
	g_bMinimap_Fold = false
	
	EquipConsume_UIPosition()
	MiniMap_Update()
	
	Minimap_SetMyPos()
	Minimap_SetCameraDir()
	
end

-- 확대 버튼 이벤트
function Ev_Minimap_btnScaleUp()

	g_fMinimap_Scale = g_fMinimap_Scale + g_fMinimap_SclaeInterval
	if g_fMinimap_SclaeMax <= g_fMinimap_Scale then
	
		g_fMinimap_Scale = g_fMinimap_SclaeMax
		SetBool( WIDGET_ID_MINIMAP, "enabled", false, "btnScaleUp" )
	
	end
	
	SetBool( WIDGET_ID_MINIMAP, "enabled", true, "btnScaleDown" )
	SetDouble( WIDGET_ID_MINIMAP, "scaleX", g_fMinimap_Scale, "map.map" )
	SetDouble( WIDGET_ID_MINIMAP, "scaleY", g_fMinimap_Scale, "map.map" )
	
	Minimap_MoveIconAll()
	Minimap_SetMyPos()

end

-- 축소 버튼 이벤트
function Ev_Minimap_btnScaleDown()

	g_fMinimap_Scale = g_fMinimap_Scale - g_fMinimap_SclaeInterval
	if g_fMinimap_Scale <= g_fMinimap_SclaeMin then
	
		g_fMinimap_Scale = g_fMinimap_SclaeMin
		SetBool( WIDGET_ID_MINIMAP, "enabled", false, "btnScaleDown" )
	
	end
	
	SetBool( WIDGET_ID_MINIMAP, "enabled", true, "btnScaleUp" )
	SetDouble( WIDGET_ID_MINIMAP, "scaleX", g_fMinimap_Scale, "map.map" )
	SetDouble( WIDGET_ID_MINIMAP, "scaleY", g_fMinimap_Scale, "map.map" )
	
	Minimap_MoveIconAll()
	Minimap_SetMyPos()

end

-- 원래 배율로 버튼 이벤트
function Ev_Minimap_btnCurrent()

	g_fMinimap_Scale = 1.0				-- 현재 미니맵 확대 비율
	
	SetBool( WIDGET_ID_MINIMAP, "enabled", true, "btnScaleUp" )
	SetBool( WIDGET_ID_MINIMAP, "enabled", true, "btnScaleDown" )
	
	SetDouble( WIDGET_ID_MINIMAP, "scaleX", g_fMinimap_Scale, "map.map" )
	SetDouble( WIDGET_ID_MINIMAP, "scaleY", g_fMinimap_Scale, "map.map" )
	
	Minimap_MoveIconAll()
	Minimap_SetMyPos()

end

-- 미니맵 클릭
function Ev_MiniMap_MapClick( nButtonIndex, nStageX, nStageY )

	if 0 ~= nButtonIndex then
		return
	end

	local nX, nY = MiniMap_GetMapImgPos( nStageX, nStageY )

	nX = nX / g_fMinimap_Scale
	nY = nY / g_fMinimap_Scale
	
	local nMapID = g_MyChar:GetCurrentMapID()
	
	-- 클릭한 위치로 이동
	if true == g_Msg:MoveToMap( nMapID, nX, nY ) then
	
		g_tbMap_MovePos =
		{
			x = nX,
			y = nY
		}
	
		-- 이동 가능하다면 이동 목표지점 아이콘 이동 및 보여주기
		local strParam = g_strMap_LAYER_PLAYER .. ";" .. g_strMap_ICON_PLAYERMOVE .. ";" .. "1" .. ";"
		CallFunction( WIDGET_ID_MINIMAP, "SetVisibleIcon", strParam, "map" )
		
		Minimap_MoveIcon( g_strMap_LAYER_PLAYER, g_strMap_ICON_PLAYERMOVE, g_tbMap_MovePos["x"], g_tbMap_MovePos["y"], 0, 0 )
	end
	
end

-- 마우스 위치에 따른 맵 이미지 위치
function MiniMap_GetMapImgPos( nStageX, nStageY )

	nStageX = math.floor( nStageX )
	nStageY = math.floor( nStageY )

	-- 윈도우 XY
	local nX = GetInt( WIDGET_ID_MINIMAP, "x", "" )
	local nY = GetInt( WIDGET_ID_MINIMAP, "y", "" )
	-- 맵 컴포넌트 XY
	local nMapX = GetInt( WIDGET_ID_MINIMAP, "x", "map" )
	local nMapY = GetInt( WIDGET_ID_MINIMAP, "y", "map" )
	-- 맵 이미지 XY
	local nMapDragX = GetInt( WIDGET_ID_MINIMAP, "x", "map.map" ) * -1
	local nMapDragY = GetInt( WIDGET_ID_MINIMAP, "y", "map.map" ) * -1
	
	nStageX = nStageX - nX - nMapX + nMapDragX
	nStageY = nStageY - nY - nMapY + nMapDragY
	
	return nStageX, nStageY

end

-- 미니맵 캐릭터 이동 아이콘 끄기
function MiniMap_HideMoveIcon()

	if false == IsOpenMinimap() then
		return
	end

	local strParam = CreateParam( g_strMap_LAYER_PLAYER, g_strMap_ICON_PLAYERMOVE, 0 )
	CallFunction( WIDGET_ID_MINIMAP, "SetVisibleIcon", strParam, "map" )

end

function MiniMap_Update()

	if g_bMap_SecretMap == true and g_bMinimap_Fold == false then
	
		SetBool( WIDGET_ID_MINIMAP, "visible", true, "mcSecretMap" )
		
	else
	
		SetBool( WIDGET_ID_MINIMAP, "visible", false, "mcSecretMap" )
		
	end
	
end