
-- 미니맵, 전체맵 공용 전역

g_tbMap_Date = nil			-- 현재 시간
g_tbMap_CurPos = nil		-- 현재 좌표
g_tbMap_MapMyPos = nil		-- 미니맵 내 위치
g_tbMap_CameraDir = nil		-- 카메라 방향
g_tbMap_MovePos = nil		-- 맵 이동 목표 지점
g_fMap_CameraThetaY = 0		-- 카메라 회전값

g_strMap_LAYER_PLAYER 		= "layer0"		-- 플레이어 레이어
g_strMap_LAYER_TEXT 		= "layer1"		-- 지역명 텍스트 레이어
g_strMap_LAYER_GATE 		= "layer2"		-- 게이트 아이콘 레이어
g_strMap_LAYER_POST 		= "layer3"		-- 우편함 아이콘 레이어
g_strMap_LAYER_BUS 			= "layer4"		-- 버스 아이콘 레이어
g_strMap_LAYER_LOCKER 		= "layer5"		-- 라커 아이콘 레이어
g_strMap_LAYER_STARTPOS		= "layer6"		-- 시작위치 아이콘 레이어
g_strMap_LAYER_CLUBMAN		= "layer7"		-- 클럽 매니저 아이콘 레이어
g_strMap_LAYER_NPC			= "layer8"		-- NPC 아이콘 레이어
g_strMap_LAYER_PARTY		= "layer9"		-- 파티 아이콘 레이어
g_strMap_LAYER_MOB			= "layerMob"	-- 몹 아이콘 레이어
g_strMap_LAYER_CTF			= "layerCTF"		 -- CTF 단말기
g_strMap_LAYER_CTF_ALLIANCE = "layerCTFAlliance" -- CTF 동맹( 같은 학원 )
g_strMap_LAYER_AUTH			= "layerAuth"	-- 인증기

g_tbMap_MobLayer 			= {}			-- 몹 아이디별 레이어 테이블

g_strMap_ICON_PLAYER 		= "iconPlayer"			-- 플레이어 아이콘 ID
g_strMap_ICON_PLAYERDIR 	= "iconPlayerDir"		-- 플레이어 방향 아이콘 ID
g_strMap_ICON_PLAYERMOVE	= "iconPlayerMove"		-- 플레이어 이동 목표지점 아이콘 ID
g_strMap_ICON_SEARCHCHAR	= "iconSearchChar"		-- 검색한 캐릭터 아이콘 ID
g_strMap_ICON_PARTY			= "iconParty"			-- 파티 아이콘 ID

g_bMap_VisibleAll			= true		-- 표시 설정
g_bMap_VisibleGate 			= true		-- 게이트 표시 여부
g_bMap_VisibleNpc 			= true		-- NPC 표시 여부
g_bMap_VisibleZoneName 		= true		-- 지역명 표시 여부
g_bMap_VisibleBus 			= true		-- 버스 정류장 표시 여부
g_bMap_VisibleLocker 		= true		-- 라커 관리인 표시 여부
g_bMap_VisibleStartPos 		= true		-- 시작위치 표시 여부
g_bMap_VisibleClubMan 		= true		-- 클럽 매니저 표시 여부
g_bMap_VisiblePost 			= true		-- 우편함 표시 여부
g_bMap_VisibleParty 		= true		-- 파티원 표시 여부

g_bMap_Action				= false		-- 동작 설정
g_bMap_ActionVehicle		= false		-- 지도 클릭시 자동 탈것 타기

g_bMap_SecretMap			= false		-- 미니맵 없는 맵?

-- 인증기 타입
g_nAuthType_CTI 		= 0		-- 난사군도
g_nAuthType_Guidance	= 1		-- 선도전

g_fMap_CTFAlliance_Time 	= 0
g_fMap_CTFAlliance_MaxTime 	= 1


-- 업데이트
function FrameMove_Map( fElspTime )

	local bOpenMinimap = IsOpenMinimap()
	local bOpenLargeMap = IsOpenLargeMap()
	
	-- 분단위가 바뀌었으면 현재 시간 재설정
	if nil ~= g_tbMap_Date then
	
		local tbDate = GetDate()
		if g_tbMap_Date[ "nMinute" ] ~= tbDate[ "nMinute" ] then
		
			g_tbMap_Date = tbDate
			
			if true == bOpenMinimap then
			
				Minimap_SetDate()
							
			end
		
		end
	
	end
	
	-- 좌표가 변경되었으면 현재 좌표 재설정
	if nil ~= g_tbMap_CurPos then
	
		local tbPos = g_MyChar:GetMapPosition()
		if 	g_tbMap_CurPos[ "x" ] ~= tbPos[ "x" ] or
			g_tbMap_CurPos[ "y" ] ~= tbPos[ "y" ] then
			
			g_tbMap_CurPos = tbPos
			
			if true == bOpenMinimap then
				Minimap_SetPosition()
			end
			
		end
	
	end
	
	-- 시크릿 맵이 아닐때만 업데이트하자
	if false == g_bMap_SecretMap then
	
		-- 맵 위치 변경되었으면 현재 위치 재설정
		if nil ~= g_tbMap_MapMyPos then
		
			local tbPos = g_MyChar:GetMapPositionOffset()
			if 	g_tbMap_MapMyPos[ "x" ] ~= tbPos[ "x" ] or
				g_tbMap_MapMyPos[ "y" ] ~= tbPos[ "y" ] then
				
				g_tbMap_MapMyPos = tbPos
				
				if false == g_bMinimap_Fold then
					Minimap_SetMyPos()
				end
				
				if true == bOpenLargeMap then

					LargeMap_SetMyPos()
					
				end
		
			end
			
			
			if true == bOpenLargeMap then
				
				g_fMap_CTFAlliance_Time = g_fMap_CTFAlliance_Time + fElspTime
				
				if g_fMap_CTFAlliance_Time > g_fMap_CTFAlliance_MaxTime then
					g_fMap_CTFAlliance_Time = 0
					LargeMap_UpdateAllanceIcon()
				end
				
			end
			
		end
		
		-- 카메라 방향도 변경되었다면 재설정 (y값은 계산식에서 사용하지 않으니 무시한다.)
		if nil ~= g_tbMap_CameraDir then

			local tbDir = GetCameraDir()
			if	20 < math.abs( g_tbMap_CameraDir[ "x" ] - tbDir[ "x" ] ) or
				20 < math.abs( g_tbMap_CameraDir[ "z" ] - tbDir[ "z" ] ) then
				
				g_tbMap_CameraDir = tbDir
				
				-- 회전 위치 계산 ------------------
				local fLengthXZ = math.sqrt( tbDir[ "x" ] * tbDir[ "x" ] + tbDir[ "z" ] * tbDir[ "z" ] )
				if 0 == fLengthXZ then
					fLengthXZ = 0.001
				end
				
				g_fMap_CameraThetaY = math.acos( tbDir[ "x" ] / fLengthXZ );
				if 0 < tbDir[ "z" ] then
					g_fMap_CameraThetaY = g_fMap_CameraThetaY * -1
				end
				
				g_fMap_CameraThetaY = ( 180 * g_fMap_CameraThetaY / math.pi ) + 180
				---------------------------------
				
				if false == g_bMinimap_Fold then
					Minimap_SetCameraDir()
				end
				
				if true == bOpenLargeMap then
					LargeMap_SetCameraDir()
				end
				
			end
		
		end
	
	end

--	if true == bOpenLargeMap then
--		CF_LargeMap_HideMoveIcon()
--	end
	
end