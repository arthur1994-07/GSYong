
-- HP바 최소 사이즈
g_nCursorTargetInfo_MINHPWIDTH = 190

-- 텍스트
g_strCursorTargetInfo_Text = ""
-- HP 보여야하는가?
g_bCursorTargetInfo_ShowHP = false
-- 열렸나?
g_bCursorTargetInfo_IsOpen = false

-- 몹 HP 정보 (Now, Max prevNow, prevMax)
g_tbCursorTargetInfo_HpInfo = { 0, 0, 0, 0 }
g_fCursorTargetInfo_ElspTime = 0

-- 커서 타겟 정보 로드 완료
function LoadCursorTargetInfo()

	SetEvent( WIDGET_ID_CURSOR_TARGET_INFO, EVENT_WIDGET_CLOSE, "CloseCursorTargetInfo", "", "" )
	SetBool( WIDGET_ID_CURSOR_TARGET_INFO, "visible", false, "" )

end

-- 커서 타겟 정보 오픈
function OpenCursorTargetInfo()

	SetBool( WIDGET_ID_CURSOR_TARGET_INFO, "visible", false, "" )
	
	g_bCursorTargetInfo_IsOpen = true

end

function CloseCursorTargetInfo()

	g_bCursorTargetInfo_IsOpen = false

end

function CursorTargetInfo_FrameMove( fElspTime )

	g_fCursorTargetInfo_ElspTime = g_fCursorTargetInfo_ElspTime + fElspTime
	if 0.25 > g_fCursorTargetInfo_ElspTime then
		return
	end
	
	g_fCursorTargetInfo_ElspTime = 0
	
	if 	g_tbCursorTargetInfo_HpInfo[ 3 ] == g_tbCursorTargetInfo_HpInfo[ 1 ] and
		g_tbCursorTargetInfo_HpInfo[ 4 ] == g_tbCursorTargetInfo_HpInfo[ 2 ] then
		return
	end

	local fHpRatio = g_tbCursorTargetInfo_HpInfo[ 1 ] / g_tbCursorTargetInfo_HpInfo[ 2 ] * 100
	local strHP = string.format( "%d/%d(%.0f%%)",
		g_tbCursorTargetInfo_HpInfo[ 1 ],
		g_tbCursorTargetInfo_HpInfo[ 2 ],
		fHpRatio )
	
	SetDouble( WIDGET_ID_CURSOR_TARGET_INFO, "value", fHpRatio, "bar" )
	SetString( WIDGET_ID_CURSOR_TARGET_INFO, "text", strHP, "labelHP" )

end

-- 정보 설정
function CursorTargetInfo_SetInfo()

	SetBool( WIDGET_ID_CURSOR_TARGET_INFO, "visible", true, "" )

	-- HP 바 visible
	SetBool( WIDGET_ID_CURSOR_TARGET_INFO, "visible", g_bCursorTargetInfo_ShowHP, "bar" )
	SetBool( WIDGET_ID_CURSOR_TARGET_INFO, "visible", g_bCursorTargetInfo_ShowHP, "labelHP" )

	SetInt( WIDGET_ID_CURSOR_TARGET_INFO, "width", 300, "textField" )
	
	SetString( WIDGET_ID_CURSOR_TARGET_INFO, "htmlText", g_strCursorTargetInfo_Text, "textField" )
	
	local nTextWidth = GetInt( WIDGET_ID_CURSOR_TARGET_INFO, "textWidth", "textField" )
	local nTextHeight = GetInt( WIDGET_ID_CURSOR_TARGET_INFO, "textHeight", "textField" )
	
	local nBgWidth = nTextWidth + 20
	local nBgHeight = nTextHeight + 15
	
	if true == g_bCursorTargetInfo_ShowHP then
	
		nBgHeight = nBgHeight + GetInt( WIDGET_ID_CURSOR_TARGET_INFO, "height", "bar" )
		
		local nBarWidth = g_nCursorTargetInfo_MINHPWIDTH
		if nBgWidth < g_nCursorTargetInfo_MINHPWIDTH then
		
			nBgWidth = g_nCursorTargetInfo_MINHPWIDTH + 10
			
		else
		
			nBarWidth = nTextWidth + 10
		
		end
		
		SetString( WIDGET_ID_CURSOR_TARGET_INFO, "width", nBarWidth, "bar" )
		SetString( WIDGET_ID_CURSOR_TARGET_INFO, "width", nBarWidth, "labelHP" )
		
		SetInt( WIDGET_ID_CURSOR_TARGET_INFO, "y", nTextHeight + 12, "bar" )
		SetInt( WIDGET_ID_CURSOR_TARGET_INFO, "y", nTextHeight + 11, "labelHP" )
	
	end
	
	SetInt( WIDGET_ID_CURSOR_TARGET_INFO, "width", nBgWidth, "bg" )
	SetInt( WIDGET_ID_CURSOR_TARGET_INFO, "height", nBgHeight, "bg" )
	
	SetInt( WIDGET_ID_CURSOR_TARGET_INFO, "width", nTextWidth + 10, "textField" )	
	SetInt( WIDGET_ID_CURSOR_TARGET_INFO, "height", nTextHeight + 10, "textField" )
	
	local nScreenWidth = GetScreenWidth()
	local nScreenHeight = GetScreenHeight()
	
	SetInt( WIDGET_ID_CURSOR_TARGET_INFO, "x", nScreenWidth - nBgWidth - 10, "" )
	SetInt( WIDGET_ID_CURSOR_TARGET_INFO, "y", nScreenHeight - nBgHeight - 40, "" )

end

-- 텍스트 설정
function CursorTargetInfo_SetText( strText )

	g_strCursorTargetInfo_Text = strText
	
	CursorTargetInfo_SetInfo()
	
end

-- 이름 텍스트 생성
function CursorTargetInfo_CreateNameText( strName, nLevel, strColor )

	local strText = ""
	
	if nil ~= nLevel then
		strText = "Lv." .. tostring( nLevel ) .. " " .. strName
	else
		strText = strName
	end
	
	return HTMLFontColor( strText, strColor )

end

-- 학원 텍스트 생성
function CursorTargetInfo_CreateSchoolText( nSchool, strColor )

	return HTMLFontColor( GetGameWordText( "FULL_ACADEMY_NAME"  , nSchool ) .. " " .. GetGameWordText( "ACADEMY_NAME_BELONG"  , 0 ) , strColor )


end

-- 직업 텍스트 생성
function CursorTargetInfo_CreateClassText( nClassIndex, strColor )

	return HTMLFontColor( GetGameWordText( "CLASS_NAME_NO_SEX"  , GetCharClassToClassIndex( nClassIndex ) ), strColor )
	--return HTMLFontColor( GetCharClassIndexName( nClassIndex ), strColor )

end

	


-- 파티 텍스트 생성
function CursorTargetInfo_CreatePartyText( nPartyState, strColor )

	local strParty = ""
	if 1 == nPartyState then
		strParty = GetGameInternalText( "CURSOR_TARGET_PARTYMASTER", 0 )
	elseif 2 == nPartyState then
		strParty = GetGameInternalText( "CURSOR_TARGET_PARTYMEMBER", 0 )
	else
		strParty = GetGameInternalText( "CURSOR_TARGET_PARTYNO", 0 )
	end
		
	return HTMLFontColor( strParty, strColor )

end

-- 클럽 텍스트 생성
function CursorTargetInfo_CreateClubText( strClubName, strColor )

	return HTMLFontColor( strClubName, strColor )

end

----------------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 커서 타겟 정보 (NONE)
function CursorTargetInfo_SetNone()

	SetBool( WIDGET_ID_CURSOR_TARGET_INFO, "visible", false, "" )
	
	RemoveFrameMove( CursorTargetInfo_FrameMove )
	
end

-- 커서 타겟 정보 (PC)
function CursorTargetInfo_SetPC( strName, nLevel, nSchool, nClassIndex,
	nPartyState, dwPartyID, bPartyValid, dwClubID, strClubName, bNotPkTime )
	
	-- HP 보일 필요 없다.
	g_bCursorTargetInfo_ShowHP = false

	local strText = ""
	
	if g_MyChar:GetSchool() == nSchool then
	
		strText = CursorTargetInfo_CreateNameText( strName, nLevel, HTML_TEXT_COLOR_PALEGREEN ) .. "\n" .. 
		CursorTargetInfo_CreateClassText( nClassIndex, HTML_TEXT_COLOR_WHITE ) .. "\n" .. 
		CursorTargetInfo_CreatePartyText( nPartyState, HTML_TEXT_COLOR_SKYBLUE ) .. "\n" .. 		
		CursorTargetInfo_CreateSchoolText( nSchool, HTML_TEXT_COLOR_WHITE )
		
	else
		if g_MyChar:CheckIsValidClub(g_MyChar:GetClubID()) == true and g_MyChar:CheckIsValidClub(dwClubID) == true and g_MyChar:GetClubID() == dwClubID then
		strText = CursorTargetInfo_CreateNameText( strName, nLevel, HTML_TEXT_COLOR_SKYBLUE ) .. "\n" .. 
		 CursorTargetInfo_CreateClassText( nClassIndex, HTML_TEXT_COLOR_WHITE )  .. "\n" .. 
		 CursorTargetInfo_CreateClubText( strClubName, HTML_TEXT_COLOR_PALEGREEN )
		
		elseif  g_Party:IsValid() == true and bPartyValid == true and g_Party:GetPartyID() == dwPartyID then

		strText = CursorTargetInfo_CreateNameText( strName, nLevel, HTML_TEXT_COLOR_SKYBLUE ) .. "\n" .. 
				CursorTargetInfo_CreateClassText( nClassIndex, HTML_TEXT_COLOR_WHITE )
		else
		
			local strNameColor = ""
			if g_MyChar:GetLevel() + 30 < nLevel then
				strNameColor = HTML_TEXT_COLOR_RED
			else
				strNameColor = HTML_TEXT_COLOR_WHITE
			end
			
			local strPK = ""
			if true == bNotPkTime then
				strPK = "\n" .. HTMLFontColor( GetGameInternalText( "CURSOR_TARGET_NONFREEPK", 0 ), HTML_TEXT_COLOR_WHITE )
			end
			
			strText = CursorTargetInfo_CreateNameText( strName, nLevel, strNameColor ) .. "\n" .. 
				CursorTargetInfo_CreateSchoolText( nSchool, HTML_TEXT_COLOR_RED ) .. "\n" .. 
				CursorTargetInfo_CreateClassText( nClassIndex, HTML_TEXT_COLOR_WHITE ) ..
				strPK
		
		end
	
	end
	
	strText = strText .. "\n" .. 
		HTMLFontColor( GetGameInternalText( "CURSOR_TARGET_SHIFTCLICK", 0 ), HTML_TEXT_COLOR_YELLOW )
	
	CursorTargetInfo_SetText( strText )

end

-- 커서 타겟 정보 (NPC)
function CursorTargetInfo_SetNPC( strName, tbTalk, bQuestStart, bQuestStep )

	-- HP 보일 필요 없다.
	g_bCursorTargetInfo_ShowHP = false

	local strText = CursorTargetInfo_CreateNameText( strName, nil, HTML_TEXT_COLOR_WHITE )
	
	for value in pairs( tbTalk ) do
		strText = strText .. "\n" .. HTMLFontColor( tbTalk[ value ], HTML_TEXT_COLOR_YELLOW )
	end
	
	if true == bQuestStart then
		strText = strText .. "\n" .. 
			HTMLFontColor( GetGameInternalText( "CURSOR_TARGET_QUESTSTART", 0 ), HTML_TEXT_COLOR_SKYBLUE )
	end
	
	if true == bQuestStep then
		strText = strText .. "\n" .. 
			HTMLFontColor( GetGameInternalText( "CURSOR_TARGET_QUESTSTEP", 0 ), HTML_TEXT_COLOR_GREEN )
	end
	
	CursorTargetInfo_SetText( strText )

end

-- 커서 타겟 정보 (Mob)
function CursorTargetInfo_SetMob( strName, nLevel, bNonDmg )

	g_bCursorTargetInfo_ShowHP = true

	local strNameColor = ""
	if g_MyChar:GetLevel() + 30 < nLevel then
		strNameColor = HTML_TEXT_COLOR_RED
	else
		strNameColor = HTML_TEXT_COLOR_WHITE
	end
	
	local strText = CursorTargetInfo_CreateNameText( strName, nLevel, strNameColor )
	
	if true == bNonDmg then
	
		strText = strText .. "\n" .. 
			HTMLFontColor( GetGameInternalText( "CANT_ATTACK_CROW", 0 ), HTML_TEXT_COLOR_RED )
	
	end
	
	CursorTargetInfo_SetText( strText )
	
	InsertFrameMove( CursorTargetInfo_FrameMove )

end

-- 커서 타겟 HP
function CursorTargetInfo_SetHP( dwNow, dwMax )

	if false == g_bCursorTargetInfo_IsOpen then
		return
	end
	
	if 	dwNow == g_tbCursorTargetInfo_HpInfo[ 1 ] and
		dwMax == g_tbCursorTargetInfo_HpInfo[ 2 ] then
		return
	end

	g_tbCursorTargetInfo_HpInfo[ 3 ] = g_tbCursorTargetInfo_HpInfo[ 1 ]
	g_tbCursorTargetInfo_HpInfo[ 4 ] = g_tbCursorTargetInfo_HpInfo[ 2 ]
	
	g_tbCursorTargetInfo_HpInfo[ 1 ] = dwNow
	g_tbCursorTargetInfo_HpInfo[ 2 ] = dwMax

end