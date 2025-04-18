
-- 파티 멤버 리스트
g_tbParty_MemberList = {}

-- 파티슬롯 보기?
g_bParty_ShowPartySlot = true
-- 버프 보기?
g_bParty_ShowBuff = false

-- 파티멤버 리스트 업데이트
function Party_UpdateList()

	g_tbParty_MemberList = {}
	
	for i = 0, PARTY_MAX - 1, 1 do
	
		local tbMember = g_Party:GetMember( i )
		if nil == tbMember[ "dwGaeaID" ] then
			break
		end	
		
		-- 버프 테이블 초기화
		tbMember[ "tbBuff" ] = {}
		
		-- 멤버 정보 설정
		g_tbParty_MemberList[ i + 1 ] = tbMember
		
		-- 버프 정보 추가
		local tbBuff = g_Client:SfReqPartyMemberGetSkillFactInfo( tbMember[ "dwGaeaID" ] )
		for value in pairs( tbBuff ) do
		
			local tb = tbBuff[ value ]
			Party_AddBuff( tbMember[ "dwGaeaID" ], tb[ 1 ], tb[ 2 ] )
	
		end
	
	end

end

-- 파티 해산
function Party_Dissolve()
	g_tbParty_MemberList = {}
end

-- 파티원 맵 이동
function Party_MemberMoveMap( nIndex, nMapID, dwGaeaMapID )

	if nIndex + 1 > table.getn( g_tbParty_MemberList ) then
		return
	end
	
	local tbMember = g_tbParty_MemberList[ nIndex + 1 ]
	tbMember[ "dwMapID" ] = nMapID
	tbMember[ "dwGaeaMapID" ] = dwGaeaMapID

end

-- 파티원 정보 업데이트
function Party_UpdateMember( nIndex, nPosX, nPosY, nHPMax, nHPNow, nLevel, tbPos )

	if nIndex + 1 > table.getn( g_tbParty_MemberList ) then
		return
	end
	
	local tbMember = g_tbParty_MemberList[ nIndex + 1 ]
	tbMember[ "nPosX" ] = nPosX
	tbMember[ "nPosY" ] = nPosY
	tbMember[ "dwMaxHP" ] = nHPMax
	tbMember[ "dwNowHP" ] = nHPNow
	tbMember[ "nLevel" ] = nLevel
	tbMember[ "tbPosition" ] = tbPos

end

-- 파티원 이름 변경
function Party_MemberRename( nIndex, strName )

	if nIndex + 1 > table.getn( g_tbParty_MemberList ) then
		return
	end
	
	local tbMember = g_tbParty_MemberList[ nIndex + 1 ]
	tbMember[ "strName" ] = strName

end

-- 내 캐릭터 인덱스
function Party_FindMyCharIndex()

	local nMyCharDBNum = g_MyChar:GetDBNum()
	
	for value in pairs( g_tbParty_MemberList ) do
	
		local tbMember = g_tbParty_MemberList[ value ]
		if nMyCharDBNum == tbMember[ "dwDbNum" ] then
			return value - 1
		end
		
	end
	
	return -1

end

-- 내 캐릭터 찾기
function Party_FindMyChar()

	local nMyCharDBNum = g_MyChar:GetDBNum()
	
	for value in pairs( g_tbParty_MemberList ) do
	
		local tbMember = g_tbParty_MemberList[ value ]
		if nMyCharDBNum == tbMember[ "dwDbNum" ] then
			return tbMember
		end
	
	end
	
	return nil

end

-- 같은 맵인지 확인
function Party_IsSameMap( tbMyChar, tbMember )

	if  tbMyChar[ "dwMapID" ] == tbMember[ "dwMapID" ] and
		tbMyChar[ "dwGaeaMapID" ] == tbMember[ "dwGaeaMapID" ] then
		return true
	else
		return false
	end
	
	return false
	
end

-- 캐릭터 이미지 파라미터
function Party_GetCharImgParam( tbMember )

	local strImgParam = ""

	if tbMember[ "emClass" ] == CHARCLASS_ARMS_M then
		strImgParam = "M_SWORD_"
	elseif tbMember[ "emClass" ] == CHARCLASS_ARMS_W then
		strImgParam = "W_SWORD_"
	elseif tbMember[ "emClass" ] == CHARCLASS_FIGHTER_M then
		strImgParam = "M_BRAWLER_"
	elseif tbMember[ "emClass" ] == CHARCLASS_FIGHTER_W then
		strImgParam = "W_BRAWLER_"
	elseif tbMember[ "emClass" ] == CHARCLASS_ARCHER_M then
		strImgParam = "M_ARCHER_"
	elseif tbMember[ "emClass" ] == CHARCLASS_ARCHER_W then
		strImgParam = "W_ARCHER_"
	elseif tbMember[ "emClass" ] == CHARCLASS_SPIRIT_M then
		strImgParam = "M_SHAMAN_"
	elseif tbMember[ "emClass" ] == CHARCLASS_SPIRIT_W then
		strImgParam = "W_SHAMAN_"
	elseif tbMember[ "emClass" ] == CHARCLASS_SCIENTIST_M then
		strImgParam = "M_SCIENTIST_"
	elseif tbMember[ "emClass" ] == CHARCLASS_SCIENTIST_W then
		strImgParam = "W_SCIENTIST_"
	elseif tbMember[ "emClass" ] == CHARCLASS_ASSASSIN_M then
		strImgParam = "M_ASSASSIN_"
	elseif tbMember[ "emClass" ] == CHARCLASS_ASSASSIN_W then
		strImgParam = "W_ASSASSIN_"
	elseif tbMember[ "emClass" ] == CHARCLASS_EXTREAM_M then
		strImgParam = "M_EXTREAM_"
	elseif tbMember[ "emClass" ] == CHARCLASS_EXTREAM_W then
		strImgParam = "W_EXTREAM_"
	elseif tbMember[ "emClass" ] == CHARCLASS_TRICKER_M then
		strImgParam = "M_MAGICIAN_"
	elseif tbMember[ "emClass" ] == CHARCLASS_TRICKER_W then
		strImgParam = "W_MAGICIAN_"
	elseif tbMember[ "emClass" ] == CHARCLASS_ACTOR_M then
		strImgParam = "M_DRAMA_"
	elseif tbMember[ "emClass" ] == CHARCLASS_ACTOR_W then
		strImgParam = "W_DRAMA_"
	else
		return
	end
	
	if true == Party_IsSameMap( Party_FindMyChar(), tbMember ) then
		strImgParam = strImgParam .. "E"
	else
		strImgParam = strImgParam .. "D"
	end
	
	return strImgParam

end

-- 학원 이미지
function Party_GetSchoolImg( tbMember )

	local strSchoolIcon = ""
	if 0 == tbMember[ "wSchool" ] then
		strSchoolIcon = HTMLImage( "PARTY_INFO_SCHOOL_SM", 16, 11 )
	elseif 1 == tbMember[ "wSchool" ] then
		strSchoolIcon = HTMLImage( "PARTY_INFO_SCHOOL_HA", 16, 11 )
	elseif 2 == tbMember[ "wSchool" ] then
		strSchoolIcon = HTMLImage( "PARTY_INFO_SCHOOL_BH", 16, 11 )
	end
	
	return strSchoolIcon

end

-- 학원 이미지 프레임 ID
function Party_GetSchoolImgFrameID( nSchool )

	local strID = ""
	if 0 == nSchool then
		strID = "sm"
	elseif 1 == nSchool then
		strID = "ha"
	elseif 2 == nSchool then
		strID = "bh"
	else
		strID = "none"
	end
	
	return strID

end

-- 학원이미지
function Party_GetSchoolImg_NotbMember( tbSchool )

	local strSchoolIcon = ""
	if 0 == tbSchool then
		strSchoolIcon = HTMLImage( "PARTY_INFO_SCHOOL_SM", 16, 11 )
	elseif 1 == tbSchool then
		strSchoolIcon = HTMLImage( "PARTY_INFO_SCHOOL_HA", 16, 11 )
	elseif 2 == tbSchool then
		strSchoolIcon = HTMLImage( "PARTY_INFO_SCHOOL_BH", 16, 11 )
	end
	
	return strSchoolIcon

end

-- 파티원 버프 슬롯 ID 생성
function Party_MakeBuffSlotID( nPartyIndex, nSlotIndex )

	return SLOT_PARTY_MEMBER_BUFF_ID .. nPartyIndex .. "_" .. nSlotIndex

end

-- 파티원 버프 추가
function Party_AddBuff( dwGaeaID, dwSkillID, dwSlot )

	-- 멤버 인덱스
	local nMemberIndex = 0
	-- 버프 인덱스
	local nBuffIndex = 0
	
	-- 버프 정보
	local tbBuff = {}
	
	-- 내 캐릭터
	local nMyCharDBNum = g_MyChar:GetDBNum()
	
	-- 해당 멤버 찾아서 버프 정보 추가하기
	for value in pairs( g_tbParty_MemberList ) do
	
		local tbMember = g_tbParty_MemberList[ value ]
		
		-- 내 캐릭터는 버프 추가할 필요 없다.
		if nMyCharDBNum == tbMember[ "dwDbNum" ] then
			continue
		end
		
		if dwGaeaID == tbMember[ "dwGaeaID" ] then
		
			-- 같은 자리에 같은 버프가 이미 등록되어 있으면 또 할 필요 없다.
			for value2 in pairs( tbMember[ "tbBuff" ] ) do
			
				if 	tbMember[ "tbBuff" ][ value2 ][ 1 ] == dwSkillID and
					tbMember[ "tbBuff" ][ value2 ][ 2 ] == dwSlot then
					return
				end
			
			end
			
			-- 버프 정보 생성
			tbBuff = 
			{
				dwSkillID,	-- 스킬 ID
				dwSlot		-- 클라이언트 실제 버프 위치
			}
			
			-- 버프 정보 추가
			table.insert( tbMember[ "tbBuff" ], tbBuff )
			
			nMemberIndex = value
			nBuffIndex = table.getn( tbMember[ "tbBuff" ] )
			
			break
		
		end
	
	end
	
	-- 멤버 못찾음
	if 0 == nMemberIndex then
		return
	end
	
	-- 슬롯 활성화
	PartySlot_AddBuff( nMemberIndex, nBuffIndex, tbBuff )

end

-- 파티원 버프 삭제
function Party_RemoveBuff( dwGaeaID, dwSkillID, dwSlot )

	-- 멤버 인덱스
	local nMemberIndex = 0
	-- 버프 인덱스
	local nBuffIndex = 0
	
	-- 해당 멤버 버프 리스트
	local tbBuffList = {}
	
	-- 내 캐릭터
	local nMyCharDBNum = g_MyChar:GetDBNum()
	
	-- 해당 멤버와 버프 찾기
	for value in pairs( g_tbParty_MemberList ) do
	
		local tbMember = g_tbParty_MemberList[ value ]
		
		-- 내 캐릭터는 버프 정보 없다.
		if nMyCharDBNum == tbMember[ "dwDbNum" ] then
			continue
		end
		
		if dwGaeaID == tbMember[ "dwGaeaID" ] then
		
			tbBuffList = tbMember[ "tbBuff" ]
			for value2 in pairs( tbBuffList ) do
			
				local tbBuff = tbBuffList[ value2 ]
				if 	tbBuff[ 1 ] == dwSkillID and
					tbBuff[ 2 ] == dwSlot then
					
					table.remove( tbMember[ "tbBuff" ], value2 )
					
					nMemberIndex = value
					nBuffIndex = value2
					
					break
					
				end
			
			end
			
			-- 버프 찾음
			if 0 ~= nMemberIndex then
				break
			end
		
		end
		
	end
	
	-- 멤버 못찾음
	if 0 == nMemberIndex then
		return
	end

	-- 슬롯 비활성화
	PartySlot_RemoveBuff( nMemberIndex, nBuffIndex, tbBuffList )
	
end

-- 파티원 모든 버프 삭제
function Party_RemoveAllBuff( dwGaeaID )

	-- 멤버 인덱스
	local nMemberIndex = 0
	
	-- 해당 멤버 찾아서 버프 정보 날리기
	for value in pairs( g_tbParty_MemberList ) do
	
		local tbMember = g_tbParty_MemberList[ value ]
		if dwGaeaID == tbMember[ "dwGaeaID" ] then
		
			tbMember[ "tbBuff" ] = {}
			nMemberIndex = value
			
			break
		
		end
		
	end
	
	-- 멤버 못찾음
	if 0 == nMemberIndex then
		return
	end

	-- 슬롯 초기화
	PartySlot_RemoveAllBuff( nMemberIndex )
	
end

-- 맵 이동함. 파티원 데이터 리프레쉬
function Party_Refresh()

	-- 파티 유효하면 파티 슬롯 켜기
	if true == g_Party:IsValid() then
	
		Party_UpdateList()
	
		if true == g_bParty_ShowPartySlot then
			OpenWidget( WIDGET_ID_PARTYSLOT )
		else
			CloseWidget( WIDGET_ID_PARTYSLOT )
		end
	
	end
	
	-- 다른 파티원 현재 맵과 같은 맵인지 확인
	for value in pairs( g_tbParty_MemberList ) do
	
		PartySlot_MemberMoveMap( value - 1 )
		PartyInfo_MemberMoveMap( value - 1 )
	
	end

end