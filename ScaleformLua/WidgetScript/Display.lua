
-- 이름 디스플레이 ---------------------------------------------------------------------------
g_fDisplay_ShowNameDistanceRatio = 1.0					-- 이름 출력할 거리 비율
g_strDisplay_NameMC = "NameDisplay"						-- 이름 디스플레이 Linkage
g_strDisplay_NameMC_tfName = "tfName"					-- 이름 텍스트 필드 인스턴스 네임
g_strDisplay_NameMC_mcNameBG = "mcNameBG"				-- 이름 텍스트 필드 배경
g_strDisplay_NameMC_tfClub = "tfClub"					-- 클럽 텍스트 필드 인스턴스 네임
g_strDisplay_NameMC_mcClubBG = "mcClubBG"				-- 클럽 텍스트 필드 배경
g_strDisplay_NameMC_mcSchoolIcon = "mcSchoolIcon"		-- 학원 아이콘 무비클립 인스턴스 네임
g_strDisplay_NameMC_mcCountryImg = "mcCountryImg"		-- 국가 이미지 인스턴스 네임
g_strDisplay_NameMC_mcPartyIcon = "mcParty"				-- 파티 아이콘 무비클립 인스턴스 네임
g_strDisplay_NameMC_rtClubIcon = "rtClubIcon"			-- 클럽 아이콘 렌더텍스처 인스턴스 네임
g_strDisplay_NameMC_rtClubIconMC = "ClubIcon_RT_MC"		-- 클럽 아이콘 렌더텍스처 무비클립 ID
g_strDisplay_NameMC_rtClubIconLink = "ClubIcon_RT"		-- 클럽 아이콘 비트맵 ID
g_strDisplay_NameMC_barHP = "barHP"						-- HP 바
g_nDisplay_NameMC_bgOffsetX = 4							-- 배경 오프셋 X
g_nDisplay_NameMC_bgOffsetY = 4							-- 배경 오프셋 Y
g_strDisplay_ItemNameMC = "ItemNameDisplay"				-- 아이템 이름 디스플레이 Linkage

-- 학원 아이콘 프레임 ID
g_strDisplay_NameMC_NoSchool = "NONE"
g_strDisplay_NameMC_SMSchool = "SM"
g_strDisplay_NameMC_HASchool = "HA"
g_strDisplay_NameMC_BHSchool = "BH"

-- 파티 아이콘 프레임 ID
g_strDisplay_NameMC_NoParty = "NONE"
g_strDisplay_NameMC_PartyMaster = "MASTER"
g_strDisplay_NameMC_PartyTarget = "TARGET"
g_strDisplay_NameMC_PartyGeneral = "GENERAL"

-- 헤드챗 디스플레이 --------------------------------------------------------------------------
g_strDisplay_HeadChatMC = "HeadChatDisplay"			-- 헤드챗  디스플레이 Linkage
g_strDisplay_HeadChat_tf = "textField"				-- 채팅 메시지 텍스트 필드 인스턴스 네임
g_strDisplay_HeadChat_bg = "bg"						-- 배경 무비클립 인스턴스 네임
g_fDisplay_HeadChat_LifeTime = 8.0					-- 헤드챗 보여질 될 시간

-- HP 디스플레이 ------------------------------------------------------------------------------
g_strDisplay_HpRedMC = "HpDisplayRed"					-- HP 디스플레이 Linkage
g_strDisplay_HpGreenMC = "HpDisplayGreen"				-- HP 디스플레이 Linkage
g_strDisplay_HpBlueMC = "HpDisplayBlue"					-- HP 디스플레이 Linkage
g_strDisplay_HpGreenSimpleMC = "HpDisplayGreenSimple"	-- HP 디스플레이 Linkage

g_strDisplay_Hp_tf = "textField"						-- HP 디스플레이 텍스트 필드
g_strDisplay_Hp_bar = "barHP"							-- HP 디스플에이 바

-- 데미지 디스플레이 ---------------------------------------------------------------------------
g_strDisplay_DmgMC = "DamageDisplay"	-- 데미지 무비클립
g_strDisplay_DmgLifeTime = 0.7			-- 데미지 리셋 시간
g_strDisplay_DmgRandOffsetRange = 20.0	-- 데미지 위치 랜덤 오프셋 범위
g_strDisplay_DmgHeadOffsetX = 0.0		-- 데미지 위치 머리로부터 오프셋 X
g_strDisplay_DmgHeadOffsetY = -20.0		-- 데미지 위치 머리로부터 오프셋 Y

-- 개인상점 디스플레이 -----------------------------------------------------------------------------
g_strDisplay_PrivateMarket = "PrivateMarketPopup"
g_strDisplay_PrivateMarket_Button = "btnMarket"
g_strDisplay_PrivateMarket_TextPM = "textPrivateMarket"
g_strDisplay_PrivateMarket_TextName = "textName"
g_strDisplay_PrivateMarket_TextPMName = "textPMName"

-- 이름/데미지 디스플레이 제한 개수
g_nDisplayLimitCount = 20


-- 디스플레이 국가 이미지 프레임 ID
function Display_GetCountryFrameID( nCountryID )

	local strFrameID = ""
	
	-- 중국
	if 6 == nCountryID then
	
		strFrameID = "china"
		
	-- 인도네시아
	elseif 11 == nCountryID then
	
		strFrameID = "indonesia"
	
	-- 말레이시아
	elseif 17 == nCountryID then
	
		strFrameID = "malaysia"
	
	-- 필리핀
	elseif 24 == nCountryID then
	
		strFrameID = "philippines"
	
	-- 대만
	elseif 29 == nCountryID then
	
		strFrameID = "taiwan"
	
	-- 태국
	elseif 31 == nCountryID then
	
		strFrameID = "thailand"
		
	-- 베트남
	elseif 35 == nCountryID then
	
		strFrameID = "vietnam"
	
	-- 연합	
	elseif 36 == nCountryID then
	
		strFrameID = "union"
		
	-- 한국	
	elseif 27 == nCountryID then
	
		strFrameID = "korea"
		
	-- 일본
	elseif 12 == nCountryID then
	
		strFrameID = "japan"
		
	-- 없음
	else
	
		strFrameID = "none"
		
	end
	
	return strFrameID

end

-- 디스플레이 국가 이미지 프레임 ID
function Display_GetCountryFrameIDByName( strName )

	local strFrameID = ""
	
	-- 중국
	if "China" == strName then
	
		strFrameID = "china"
		
	-- 인도네시아
	elseif "Indonesia" == strName then
	
		strFrameID = "indonesia"
	
	-- 말레이시아
	elseif "Malaysia" == strName then
	
		strFrameID = "malaysia"
	
	-- 필리핀
	elseif "Philippines" == strName then
	
		strFrameID = "philippines"
	
	-- 대만
	elseif "Taiwan" == strName then
	
		strFrameID = "taiwan"
	
	-- 태국
	elseif "Thailand" == strName then
	
		strFrameID = "thailand"
		
	-- 베트남
	elseif "Vietnam" == strName then
	
		strFrameID = "vietnam"
		
	-- 연합
	elseif "Union" == strName then
	
		strFrameID = "union"
		
	-- 한국	
	elseif "South Korea" == strName then
	
		strFrameID = "korea"
		
	-- 일본
	elseif "Japan" == strName then
	
		strFrameID = "japan"
		
	-- 없음
	else
	
		strFrameID = "none"
		
	end
	
	return strFrameID

end

---------------------------------------------------------------------------------------------------

-- !!!! 속도 문제로 디스플레이는 클라이언트에서 직접 처리한다. 위 정의값만 잘 설정해준다.
-- !!!! 혹시 몰라 일단 함수는 만들어 놓았다.

-- 디스플레이 로드 완료
function LoadDisplay()
end

-- 레이어 추가
function Display_AddLayer( nLayerID )
	CallFunction( WIDGET_ID_DISPLAY, "AddLayer", tostring( nLayerID ), "ds" )
end

-- 디스플레이 오브젝트 추가
function Display_AddObject( nLayerID, nDisplayID, strClassName )

	local strParam = tostring( nLayerID ) .. ";" .. 
		tostring( nDisplayID ) .. ";" .. 
		strClassName

	CallFunction( WIDGET_ID_DISPLAY, "AddDisplay", strParam, "ds" )	

end

-- 디스플레이 오브젝트 프로퍼티 설정
function Display_SetProp( nLayerID, nDisplayID, strProp, value, strInst )

	local strParam = tostring( nLayerID ) .. ";" .. 
		tostring( nDisplayID ) .. ";" .. 
		strInst .. ";" .. 
		strProp .. ";" .. 
		tostring( value ) .. ";"
	
	CallFunction( WIDGET_ID_DISPLAY, "SetDisplayProp", strParam, "ds" )

end

-- 디스플레이 오브젝트 텍스트 컬러 설정
function Display_SetTextColor( nLayerID, nDisplayID, nColor, strInst )

	local strParam = tostring( nLayerID ) .. ";" .. 
		tostring( nDisplayID ) .. ";" .. 
		strInst .. ";" .. 
		tostring( nColor ) .. ";"

	CallFunction( WIDGET_ID_DISPLAY, "SetDisplayTextColor", strParam, "ds" )

end

-- 디스플레이 오브젝트 위치 설정
function Display_SetPosition( nLayerID, nDisplayID, nPosX, nPosY )

	local strParam = tostring( nLayerID ) .. ";" .. 
		tostring( nDisplayID ) .. ";" .. 
		tostring( nPosX ) .. ";" .. 
		tostring( nPosY ) .. ";"

	CallFunction( WIDGET_ID_DISPLAY, "SetPosition", strParam, "ds" )

end

-- 디스플레이 오브젝트 스케일 설정
function Display_SetScale( nLayerID, nDisplayID, fRatio )

	local strParam = tostring( nLayerID ) .. ";" .. 
		tostring( nDisplayID ) .. ";" .. 
		tostring( fRatio ) .. ";"
		
	CallFunction( WIDGET_ID_DISPLAY, "SetScale", strParam, "ds" )

end

-- 디스플레이 오브젝트 라이프타임 설정 (밀리세컨드 단위)
function Display_SetLifeTime( nLayerID, nDisplayID, nLifeTime, nFadeTime )

	local strParam = tostring( nLayerID ) .. ";" .. 
		tostring( nDisplayID ) .. ";" .. 
		tostring( nLifeTime ) .. ";" .. 
		tostring( nFadeTime ) .. ";"

	CallFunction( WIDGET_ID_DISPLAY, "SetLifeTime", strParam, "ds" )

end

-- 헤드쳇 출력
function Display_PrintHeadChat( nLayerID, nDisplayID, strName, nNameColor, strText, nTextColor, strInst, strBgInst )

	local strParam =  tostring( nLayerID ) .. ";" .. 
		tostring( nDisplayID ) .. ";" .. 
		strInst .. ";" .. 
		strBgInst .. ";" .. 
		strName .. ";" .. 
		tostring( nNameColor ) .. ";" .. 
		strText .. ";" .. 
		tostring( nTextColor ) .. ";"
		
	CallFunction( WIDGET_ID_DISPLAY, "SetChat", strParam, "ds" )

end

-- 데미지 스프라이트 출력
function Display_PrintDamage( nLayerID, nDisplayID, nValue, strClassName, strInst )

	local strParam = tostring( nLayerID ) .. ";" .. 
		tostring( nDisplayID ) .. ";" .. 
		strInst .. ";" .. 
		tostring( nValue ) .. ";" .. 
		strClassName  .. ";"

	CallFunction( WIDGET_ID_DISPLAY, "SetDamage", strParam, "ds" )
	
end
