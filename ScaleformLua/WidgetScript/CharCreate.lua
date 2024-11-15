
g_nCharCreate_Select_Sex_Prev	= -1		-- 이전에 선택한 성별

g_nCharCreate_RotateBtnPosRatio = 0

-- 캐릭터 생성 UI 로드 완료 
function LoadCharCreate()

	SetBool( WIDGET_ID_CHARCREATE, "customDrag", true, "" )
	
	-- 닫기 버튼 숨기기
	SetBool( WIDGET_ID_CHARCREATE, "visible", false, "closeBtn" )

	-- 접속중인 서버, 채널 텍스트
	local strServerName = GetGameExternalText( "SERVER_NAME", g_Login.m_nServerGroup )
	local strChannel = tostring( g_Login.m_nServerChannel ) .. GetGameExternalText( "SERVER_CHANNEL", 0 )
	local strConnectServer = strServerName .. " - " .. strChannel
	
	-- 접속중인 서버, 채널 텍스트 설정
	SetString( WIDGET_ID_CHARCREATE, "text", strConnectServer, "charCreate.btns.labelServer" )
	
	-- 캐릭터 생성 버튼
	SetString( WIDGET_ID_CHARCREATE, "label", strCharCreate, "charCreate.btns.btnNewChar" )
	-- 캐릭터 선택 버튼
	SetString( WIDGET_ID_CHARCREATE, "label", GetGameWordText( "SELECT_CHARACTER_PAGE", 0 ), "charCreate.btns.btnCharSelect" )
	-- 서버 변경 버튼
	SetString( WIDGET_ID_CHARCREATE, "label", GetGameWordText( "CHANGE_SERVER", 0 ), "charCreate.btns.btnServer" )
	-- 생성 완료 버튼
	SetString( WIDGET_ID_CHARCREATE, "label", GetGameWordText( "CREATE_COMPLETE", 0 ), "charCreate.btns.btnComplete" )
	
	-- 학원 선택
	SetString( WIDGET_ID_CHARCREATE, "text", GetGameWordText( "CREATE_CHARACTER_PAGE_STATIC", 0 ),
		"charCreate.tfSchool" )
	-- 활동 부서
	SetString( WIDGET_ID_CHARCREATE, "text", GetGameWordText( "CREATE_CHARACTER_PAGE_STATIC", 1 ),
		"charCreate.tfClass" )
	-- 얼굴 모양
	SetString( WIDGET_ID_CHARCREATE, "text", GetGameWordText( "CREATE_CHARACTER_PAGE_STATIC", 2 ),
		"charCreate.styleSelect.labelFace" )
	-- 얼굴 모양
	SetString( WIDGET_ID_CHARCREATE, "text", GetGameWordText( "CREATE_CHARACTER_PAGE_STATIC", 3 ),
		"charCreate.styleSelect.labelHair" )
	-- 이름 입력
	SetString( WIDGET_ID_CHARCREATE, "text", GetGameWordText( "CREATE_CHARACTER_PAGE_STATIC", 4 ),
		"charCreate.labelName" )		
	-- 성별
	SetString( WIDGET_ID_CHARCREATE, "text", GetGameWordText( "CREATE_CHARACTER_PAGE_STATIC", 6 ),
		"charCreate.tfSex" )
	-- 기본설정
	SetString( WIDGET_ID_CHARCREATE, "text", GetGameWordText( "KEYSETTING_APPLYOK", 0 ),
		"charCreate.tfDefault" )

	-- 성별 선택 라디오버튼 비활성
	SetEnableSexRadio( false )
	-- 클래스 선택 라이오버튼 비활성
	SetEnableClassRadio( false )
	
	-- 여자 클래스 라디오 그룹 visible off
	SetBool( WIDGET_ID_CHARCREATE, "visible", false, "charCreate.radioClassF" )

	-- 얼굴/머리 모양 데이터 설정
	InitStyleData()
	
	-- 학원 라디오 버튼 클릭 이벤트
	SetEvent( WIDGET_ID_CHARCREATE, EVENT_BUTTON_CLICK, "Event_CharCreate_radioSchool_SM",
		"", "charCreate.radioSchool.radio_SM" )
	SetEvent( WIDGET_ID_CHARCREATE, EVENT_BUTTON_CLICK, "Event_CharCreate_radioSchool_HA",
		"", "charCreate.radioSchool.radio_HA" )
	SetEvent( WIDGET_ID_CHARCREATE, EVENT_BUTTON_CLICK, "Event_CharCreate_radioSchool_BH",
		"", "charCreate.radioSchool.radio_BH" )
		
	local str = GetGameExternalText( "NEW_CHAR_SELECT_SCHOOL_NAME", 0 )
	SetString( WIDGET_ID_CHARCREATE, "TooltipText", str, "charCreate.radioSchool.radio_SM" )
	
	str = GetGameExternalText( "NEW_CHAR_SELECT_SCHOOL_NAME", 1 )
	SetString( WIDGET_ID_CHARCREATE, "TooltipText", str, "charCreate.radioSchool.radio_HA" )
	
	str = GetGameExternalText( "NEW_CHAR_SELECT_SCHOOL_NAME", 2 )
	SetString( WIDGET_ID_CHARCREATE, "TooltipText", str, "charCreate.radioSchool.radio_BH" )
	
	-- 성별 라디오 버튼 클릭 이벤트
	SetEvent( WIDGET_ID_CHARCREATE, EVENT_BUTTON_CLICK, "Event_CharCreate_radioSex_Male",
		"", "charCreate.radioSex.radio_Male" )
	SetEvent( WIDGET_ID_CHARCREATE, EVENT_BUTTON_CLICK, "Event_CharCreate_radioSex_Female",
		"", "charCreate.radioSex.radio_Female" )
		
	str = GetGameWordText( "BASIC_SEX", 0 )
	SetString( WIDGET_ID_CHARCREATE, "TooltipText", str, "charCreate.radioSex.radio_Male" )
	
	str = GetGameWordText( "BASIC_SEX", 1 )
	SetString( WIDGET_ID_CHARCREATE, "TooltipText", str, "charCreate.radioSex.radio_Female" )
	
	-- 클래스 라디오 버튼 클릭 이벤트
	SetEvent( WIDGET_ID_CHARCREATE, EVENT_BUTTON_CLICK, "Event_CharCreate_SwordmanM",
		"", "charCreate.radioClassM.radioSwordman" )
	SetEvent( WIDGET_ID_CHARCREATE, EVENT_BUTTON_CLICK, "Event_CharCreate_SwordmanF",
		"", "charCreate.radioClassF.radioSwordman" )
	SetEvent( WIDGET_ID_CHARCREATE, EVENT_BUTTON_CLICK, "Event_CharCreate_BrawleM",
		"", "charCreate.radioClassM.radioBrawle" )
	SetEvent( WIDGET_ID_CHARCREATE, EVENT_BUTTON_CLICK, "Event_CharCreate_BrawleF",
		"", "charCreate.radioClassF.radioBrawle" )
	SetEvent( WIDGET_ID_CHARCREATE, EVENT_BUTTON_CLICK, "Event_CharCreate_ArcherM",
		"", "charCreate.radioClassM.radioArcher" )
	SetEvent( WIDGET_ID_CHARCREATE, EVENT_BUTTON_CLICK, "Event_CharCreate_ArcherF",
		"", "charCreate.radioClassF.radioArcher" )
	SetEvent( WIDGET_ID_CHARCREATE, EVENT_BUTTON_CLICK, "Event_CharCreate_ShamanM",
		"", "charCreate.radioClassM.radioShaman" )
	SetEvent( WIDGET_ID_CHARCREATE, EVENT_BUTTON_CLICK, "Event_CharCreate_ShamanF",
		"", "charCreate.radioClassF.radioShaman" )
	SetEvent( WIDGET_ID_CHARCREATE, EVENT_BUTTON_CLICK, "Event_CharCreate_ScientistM",
		"", "charCreate.radioClassM.radioScientist" )
	SetEvent( WIDGET_ID_CHARCREATE, EVENT_BUTTON_CLICK, "Event_CharCreate_ScientistF",
		"", "charCreate.radioClassF.radioScientist" )
	SetEvent( WIDGET_ID_CHARCREATE, EVENT_BUTTON_CLICK, "Event_CharCreate_AssassinM",
		"", "charCreate.radioClassM.radioAssassin" )
	SetEvent( WIDGET_ID_CHARCREATE, EVENT_BUTTON_CLICK, "Event_CharCreate_AssassinF",
		"", "charCreate.radioClassF.radioAssassin" )
	SetEvent( WIDGET_ID_CHARCREATE, EVENT_BUTTON_CLICK, "Event_CharCreate_TrickerM",
		"", "charCreate.radioClassM.radioTricker" )
	SetEvent( WIDGET_ID_CHARCREATE, EVENT_BUTTON_CLICK, "Event_CharCreate_TrickerF",
		"", "charCreate.radioClassF.radioTricker" )
	SetEvent( WIDGET_ID_CHARCREATE, EVENT_BUTTON_CLICK, "Event_CharCreate_ExtreamM",
		"", "charCreate.radioClassM.radioExtream" )
	SetEvent( WIDGET_ID_CHARCREATE, EVENT_BUTTON_CLICK, "Event_CharCreate_ExtreamF",
		"", "charCreate.radioClassF.radioExtream" )
	SetEvent( WIDGET_ID_CHARCREATE, EVENT_BUTTON_CLICK, "Event_CharCreate_ActorM",
		"", "charCreate.radioClassM.radioActor" )
	SetEvent( WIDGET_ID_CHARCREATE, EVENT_BUTTON_CLICK, "Event_CharCreate_ActorF",
		"", "charCreate.radioClassF.radioActor" )
		
	str = GetCharClassIndexName( CHARCLASS_IDX_ARMS_M )
	SetString( WIDGET_ID_CHARCREATE, "TooltipText", str, "charCreate.radioClassM.radioSwordman" )
	str = GetCharClassIndexName( CHARCLASS_IDX_ARMS_W )
	SetString( WIDGET_ID_CHARCREATE, "TooltipText", str, "charCreate.radioClassF.radioSwordman" )
	
	str = GetCharClassIndexName( CHARCLASS_IDX_FIGHTER_M )
	SetString( WIDGET_ID_CHARCREATE, "TooltipText", str, "charCreate.radioClassM.radioBrawle" )
	str = GetCharClassIndexName( CHARCLASS_IDX_FIGHTER_W )
	SetString( WIDGET_ID_CHARCREATE, "TooltipText", str, "charCreate.radioClassF.radioBrawle" )
	
	str = GetCharClassIndexName( CHARCLASS_IDX_ARCHER_M )
	SetString( WIDGET_ID_CHARCREATE, "TooltipText", str, "charCreate.radioClassM.radioArcher" )
	str = GetCharClassIndexName( CHARCLASS_IDX_ARCHER_W )
	SetString( WIDGET_ID_CHARCREATE, "TooltipText", str, "charCreate.radioClassF.radioArcher" )
	
	str = GetCharClassIndexName( CHARCLASS_IDX_SPIRIT_M )
	SetString( WIDGET_ID_CHARCREATE, "TooltipText", str, "charCreate.radioClassM.radioShaman" )
	str = GetCharClassIndexName( CHARCLASS_IDX_SPIRIT_W )
	SetString( WIDGET_ID_CHARCREATE, "TooltipText", str, "charCreate.radioClassF.radioShaman" )
	
	str = GetCharClassIndexName( CHARCLASS_IDX_SCIENTIST_M )
	SetString( WIDGET_ID_CHARCREATE, "TooltipText", str, "charCreate.radioClassM.radioScientist" )
	str = GetCharClassIndexName( CHARCLASS_IDX_SCIENTIST_W )
	SetString( WIDGET_ID_CHARCREATE, "TooltipText", str, "charCreate.radioClassF.radioScientist" )
	
	str = GetCharClassIndexName( CHARCLASS_IDX_ASSASSIN_M )
	SetString( WIDGET_ID_CHARCREATE, "TooltipText", str, "charCreate.radioClassM.radioAssassin" )
	str = GetCharClassIndexName( CHARCLASS_IDX_ASSASSIN_W )
	SetString( WIDGET_ID_CHARCREATE, "TooltipText", str, "charCreate.radioClassF.radioAssassin" )
	
	str = GetCharClassIndexName( CHARCLASS_IDX_TRICKER_M )
	SetString( WIDGET_ID_CHARCREATE, "TooltipText", str, "charCreate.radioClassM.radioTricker" )
	str = GetCharClassIndexName( CHARCLASS_IDX_TRICKER_W )
	SetString( WIDGET_ID_CHARCREATE, "TooltipText", str, "charCreate.radioClassF.radioTricker" )
	
	str = GetCharClassIndexName( CHARCLASS_IDX_EXTREAM_M )
	SetString( WIDGET_ID_CHARCREATE, "TooltipText", str, "charCreate.radioClassM.radioExtream" )
	
	str = str .. "\n" .. HTMLFontColor( string.format( 
		GetGameInternalText( "EXTREME_CREATE_CONDITION_TEXT", 0 ), g_wLEVEL_FOR_EXTREME ), HTML_TEXT_COLOR_RED )
	SetString( WIDGET_ID_CHARCREATE, "TooltipHtmlText", str, "charCreate.radioClassM.mcExtremeDisableTooltip" )
	
	str = GetCharClassIndexName( CHARCLASS_IDX_EXTREAM_W )
	SetString( WIDGET_ID_CHARCREATE, "TooltipText", str, "charCreate.radioClassF.radioExtream" )
	
	str = str .. "\n" .. HTMLFontColor( string.format( 
		GetGameInternalText( "EXTREME_CREATE_CONDITION_TEXT", 0 ), g_wLEVEL_FOR_EXTREME ), HTML_TEXT_COLOR_RED )
	SetString( WIDGET_ID_CHARCREATE, "TooltipHtmlText", str, "charCreate.radioClassF.mcExtremeDisableTooltip" )
	
	str = GetCharClassIndexName( CHARCLASS_IDX_ACTOR_M )
	SetString( WIDGET_ID_CHARCREATE, "TooltipText", str, "charCreate.radioClassM.radioActor" )
	
	str = str .. "\n" .. HTMLFontColor( string.format( 
		GetGameInternalText( "EXTREME_CREATE_CONDITION_TEXT", 0 ), g_wLEVEL_FOR_ACTOR ), HTML_TEXT_COLOR_RED )
	SetString( WIDGET_ID_CHARCREATE, "TooltipHtmlText", str, "charCreate.radioClassM.mcActorDisableTooltip" )
	
	str = GetCharClassIndexName( CHARCLASS_IDX_ACTOR_W )
	SetString( WIDGET_ID_CHARCREATE, "TooltipText", str, "charCreate.radioClassF.radioActor" )
	
	str = str .. "\n" .. HTMLFontColor( string.format( 
		GetGameInternalText( "EXTREME_CREATE_CONDITION_TEXT", 0 ), g_wLEVEL_FOR_ACTOR ), HTML_TEXT_COLOR_RED )
	SetString( WIDGET_ID_CHARCREATE, "TooltipHtmlText", str, "charCreate.radioClassF.mcActorDisableTooltip" )
	
	-- 얼굴 인덱스 변경 이벤트
	SetEvent( WIDGET_ID_CHARCREATE, EVENT_INDEX_CLICK_CHANGE, "Event_CharCreate_FaceIndex", 
		"lastIndex;index;", "charCreate.styleSelect.optionFace" )
	-- 헤어 인덱스 변경 이벤트
	SetEvent( WIDGET_ID_CHARCREATE, EVENT_INDEX_CLICK_CHANGE, "Event_CharCreate_HairIndex", 
		"lastIndex;index;", "charCreate.styleSelect.optionHair" )
		
	-- 캐릭터 생성 버튼 이벤트
	SetEvent( WIDGET_ID_CHARCREATE, EVENT_BUTTON_CLICK, "Event_CharCreate_btnNewChar_Click",
		"", "charCreate.btns.btnNewChar" )
	-- 캐릭터 선택 버튼 이벤트
	SetEvent( WIDGET_ID_CHARCREATE, EVENT_BUTTON_CLICK, "Event_CharCreate_btnCharSelect_Click",
		"", "charCreate.btns.btnCharSelect" )
	-- 서버변경 버튼 이벤트
	SetEvent( WIDGET_ID_CHARCREATE, EVENT_BUTTON_CLICK, "Event_CharCreate_btnServer_Click",
		"", "charCreate.btns.btnServer" )
	-- 생성완료 버튼 이벤트
	SetEvent( WIDGET_ID_CHARCREATE, EVENT_BUTTON_CLICK, "Event_CharCreate_btnComplete_Click",
		"", "charCreate.btns.btnComplete" )
		
	
	-- 클래스 제한, '0' = 과학부,인술부,마술부 제한. '1' = 인술부, 마술부 제한. '2' = 마술부 제한. '3' = 연극부, '4' = 제한없음
	if g_wENABLE_NEW_CLASS == 0 then
		SetBool( WIDGET_ID_CHARCREATE, "visible", false, "charCreate.radioClassM.radioScientist" )
		SetBool( WIDGET_ID_CHARCREATE, "visible", false, "charCreate.radioClassF.radioScientist" )
	end
	if g_wENABLE_NEW_CLASS <= 1 then
		SetBool( WIDGET_ID_CHARCREATE, "visible", false, "charCreate.radioClassM.radioAssassin" )
		SetBool( WIDGET_ID_CHARCREATE, "visible", false, "charCreate.radioClassF.radioAssassin" )
	end
	if g_wENABLE_NEW_CLASS <= 2 then
		SetBool( WIDGET_ID_CHARCREATE, "visible", false, "charCreate.radioClassM.radioTricker" )
		SetBool( WIDGET_ID_CHARCREATE, "visible", false, "charCreate.radioClassF.radioTricker" )
	end
	if g_wENABLE_NEW_CLASS <= 3 then
		SetBool( WIDGET_ID_CHARCREATE, "visible", false, "charCreate.radioClassM.radioActor" )
		SetBool( WIDGET_ID_CHARCREATE, "visible", false, "charCreate.radioClassF.radioActor" )
	end
	
end

function OpenCharCreate()

	OpenWidget( WIDGET_ID_CHARCREATETOOLTIP )
	--OpenWidget( WIDGET_ID_CHARCREATEROTATE )
	
	SetString( WIDGET_ID_CHARCREATE, "text", "", "charCreate.inputName" )
	
	SetBool( WIDGET_ID_CHARCREATE, "selected", false, "charCreate.radioSchool.radio_SM" )
	SetBool( WIDGET_ID_CHARCREATE, "selected", false, "charCreate.radioSchool.radio_HA" )
	SetBool( WIDGET_ID_CHARCREATE, "selected", false, "charCreate.radioSchool.radio_BH" )
	
	SetBool( WIDGET_ID_CHARCREATE, "selected", false, "charCreate.radioSex.radio_Male" )
	SetBool( WIDGET_ID_CHARCREATE, "selected", false, "charCreate.radioSex.radio_Female" )
	
	SetBool( WIDGET_ID_CHARCREATE, "selected", false, "charCreate.radioClassM.radioSwordman" )
	SetBool( WIDGET_ID_CHARCREATE, "selected", false, "charCreate.radioClassM.radioBrawle" )
	SetBool( WIDGET_ID_CHARCREATE, "selected", false, "charCreate.radioClassM.radioArcher" )
	SetBool( WIDGET_ID_CHARCREATE, "selected", false, "charCreate.radioClassM.radioShaman" )
	SetBool( WIDGET_ID_CHARCREATE, "selected", false, "charCreate.radioClassM.radioScientist" )
	SetBool( WIDGET_ID_CHARCREATE, "selected", false, "charCreate.radioClassM.radioAssassin" )
	SetBool( WIDGET_ID_CHARCREATE, "selected", false, "charCreate.radioClassM.radioTricker" )
	SetBool( WIDGET_ID_CHARCREATE, "selected", false, "charCreate.radioClassM.radioExtream" )

	SetBool( WIDGET_ID_CHARCREATE, "selected", false, "charCreate.radioClassF.radioSwordman" )
	SetBool( WIDGET_ID_CHARCREATE, "selected", false, "charCreate.radioClassF.radioBrawle" )
	SetBool( WIDGET_ID_CHARCREATE, "selected", false, "charCreate.radioClassF.radioArcher" )
	SetBool( WIDGET_ID_CHARCREATE, "selected", false, "charCreate.radioClassF.radioShaman" )
	SetBool( WIDGET_ID_CHARCREATE, "selected", false, "charCreate.radioClassF.radioScientist" )
	SetBool( WIDGET_ID_CHARCREATE, "selected", false, "charCreate.radioClassF.radioAssassin" )
	SetBool( WIDGET_ID_CHARCREATE, "selected", false, "charCreate.radioClassF.radioTricker" )
	SetBool( WIDGET_ID_CHARCREATE, "selected", false, "charCreate.radioClassF.radioExtream" )
	
	SetInt( WIDGET_ID_CHARCREATE, "selectedIndex", 0, "charCreate.styleSelect.optionFace" )
	SetInt( WIDGET_ID_CHARCREATE, "selectedIndex", 0, "charCreate.styleSelect.optionHair" )
	
	SetEnableSexRadio( false )
	SetEnableClassRadio( false )
	
	Ev_CharCreate_ToolTip_Close( 2 )
	Ev_CharCreate_ToolTip_Close( 1 )
	
	local str = GetGameWordText( "SELECT_CHARACTER_PAGE_BUTTON", 0 ) .. 
		"(" .. tostring( g_Login:GetCharRemain() ) .. ")"
	SetString( WIDGET_ID_CHARCREATE, "label", str, "charCreate.btns.btnNewChar" )
	SetBool( WIDGET_ID_CHARCREATE, "enabled", false, "charCreate.btns.btnNewChar" )

	-- 선택 상태 초기화
	g_CharCreate.m_nSelectSchool = -1
	g_CharCreate.m_nSelectSex = -1
	g_CharCreate.m_nSelectClass = -1
	
	-- 극강부 비활성시 툴팁용 컴포넌트
	SetBool( WIDGET_ID_CHARCREATE, "visible", false, "charCreate.radioClassM.mcExtremeDisableTooltip" )
	SetBool( WIDGET_ID_CHARCREATE, "visible", false, "charCreate.radioClassF.mcExtremeDisableTooltip" )
	-- 변술부 비활성시 툴팁용 컴포넌트
	SetBool( WIDGET_ID_CHARCREATE, "visible", false, "charCreate.radioClassM.mcActorDisableTooltip" )
	SetBool( WIDGET_ID_CHARCREATE, "visible", false, "charCreate.radioClassF.mcActorDisableTooltip" )
	
end


-- 얼굴/머리 모양 데이터 설정
function InitStyleData()

	local strFaceData = ""
	local strHairData = ""
	
	local nHeadNum, nHairNum, nCharClass = 0, 0, 0
	if g_CharCreate.m_nSelectClass ~= -1 then
		nCharClass = GetCharClassToSexFlag(g_CharCreate.m_nSelectClass)
		nHeadNum = GetHeadNum(g_CharCreate.m_nSelectClass)
		nHairNum = GetHairNum(g_CharCreate.m_nSelectClass)
	end
	
	if 0 == nHeadNum then
		strFaceData = " ;"
	end
	
	if 0 == nHairNum then
		strHairData = " ;"
	end
	
	for i = 0, nHeadNum-1, 1 do
		strFaceData = strFaceData .. GetGameWordText( "FACE_NAME_FIGHTER", i ) .. ";"
	end

	for i = 0, nHairNum-1, 1 do		
		strHairData = strHairData .. GetGameWordText( "HAIR_NAME_FIGHTER", i ) .. ";"
	end
	
	SetData( WIDGET_ID_CHARCREATE, strFaceData, "charCreate.styleSelect.optionFace" )
	SetData( WIDGET_ID_CHARCREATE, strHairData, "charCreate.styleSelect.optionHair" )
	
	SetInt( WIDGET_ID_CHARCREATE, "selectedIndex", 0, "charCreate.styleSelect.optionFace" )
	SetInt( WIDGET_ID_CHARCREATE, "selectedIndex", 0, "charCreate.styleSelect.optionHair" )
end

-- 성별 라디오 버튼 활성 설정
function SetEnableSexRadio( bEnable )

	SetBool( WIDGET_ID_CHARCREATE, "enabled", bEnable, "charCreate.radioSex.radio_Male" )
	SetBool( WIDGET_ID_CHARCREATE, "enabled", bEnable, "charCreate.radioSex.radio_Female" )

end

-- 클래스 라디오 버튼 활성 설정
function SetEnableClassRadio( bEnable )

	SetBool( WIDGET_ID_CHARCREATE, "enabled", bEnable, "charCreate.radioClassM.radioSwordman" )
	SetBool( WIDGET_ID_CHARCREATE, "enabled", bEnable, "charCreate.radioClassF.radioSwordman" )
	
	SetBool( WIDGET_ID_CHARCREATE, "enabled", bEnable, "charCreate.radioClassM.radioBrawle" )
	SetBool( WIDGET_ID_CHARCREATE, "enabled", bEnable, "charCreate.radioClassF.radioBrawle" )
	
	SetBool( WIDGET_ID_CHARCREATE, "enabled", bEnable, "charCreate.radioClassM.radioArcher" )
	SetBool( WIDGET_ID_CHARCREATE, "enabled", bEnable, "charCreate.radioClassF.radioArcher" )
	
	SetBool( WIDGET_ID_CHARCREATE, "enabled", bEnable, "charCreate.radioClassM.radioShaman" )
	SetBool( WIDGET_ID_CHARCREATE, "enabled", bEnable, "charCreate.radioClassF.radioShaman" )
	
	SetBool( WIDGET_ID_CHARCREATE, "enabled", bEnable, "charCreate.radioClassM.radioScientist" )
	SetBool( WIDGET_ID_CHARCREATE, "enabled", bEnable, "charCreate.radioClassF.radioScientist" )
	
	SetBool( WIDGET_ID_CHARCREATE, "enabled", bEnable, "charCreate.radioClassM.radioAssassin" )
	SetBool( WIDGET_ID_CHARCREATE, "enabled", bEnable, "charCreate.radioClassF.radioAssassin" )
	
	SetBool( WIDGET_ID_CHARCREATE, "enabled", bEnable, "charCreate.radioClassM.radioTricker" )
	SetBool( WIDGET_ID_CHARCREATE, "enabled", bEnable, "charCreate.radioClassF.radioTricker" )
	
	-- 극강부 생성 불가능하면 극강부 버튼 비활성화
	if true == g_Login:GetCharRemainEx() then
	
		SetBool( WIDGET_ID_CHARCREATE, "visible", false, "charCreate.radioClassF.mcExtremeDisableTooltip" )
		SetBool( WIDGET_ID_CHARCREATE, "visible", false, "charCreate.radioClassM.mcExtremeDisableTooltip" )
		
		SetBool( WIDGET_ID_CHARCREATE, "enabled", bEnable, "charCreate.radioClassM.radioExtream" )
		SetBool( WIDGET_ID_CHARCREATE, "enabled", bEnable, "charCreate.radioClassF.radioExtream" )
		
	else
	
		SetBool( WIDGET_ID_CHARCREATE, "visible", true, "charCreate.radioClassF.mcExtremeDisableTooltip" )
		SetBool( WIDGET_ID_CHARCREATE, "visible", true, "charCreate.radioClassM.mcExtremeDisableTooltip" )
		
		SetBool( WIDGET_ID_CHARCREATE, "enabled", false, "charCreate.radioClassF.radioExtream" )
		SetBool( WIDGET_ID_CHARCREATE, "enabled", false, "charCreate.radioClassM.radioExtream" )
	
	end
	
	-- 변술부 생성 불가능하면 변술부 버튼 비활성화
	if true == g_Login:GetCharRemainActor() then
	
		SetBool( WIDGET_ID_CHARCREATE, "visible", false, "charCreate.radioClassF.mcActorDisableTooltip" )
		SetBool( WIDGET_ID_CHARCREATE, "visible", false, "charCreate.radioClassM.mcActorDisableTooltip" )
	
		SetBool( WIDGET_ID_CHARCREATE, "enabled", bEnable, "charCreate.radioClassM.radioActor" )
		SetBool( WIDGET_ID_CHARCREATE, "enabled", bEnable, "charCreate.radioClassF.radioActor" )
	
	else
	
		SetBool( WIDGET_ID_CHARCREATE, "visible", true, "charCreate.radioClassF.mcActorDisableTooltip" )
		SetBool( WIDGET_ID_CHARCREATE, "visible", true, "charCreate.radioClassM.mcActorDisableTooltip" )
	
		SetBool( WIDGET_ID_CHARCREATE, "enabled", false, "charCreate.radioClassM.radioActor" )
		SetBool( WIDGET_ID_CHARCREATE, "enabled", false, "charCreate.radioClassF.radioActor" )
		
	end

end

-- 학원 선택 후 동작
function AtferSchoolSelect()

	CloseWidget( WIDGET_ID_CHARCREATEROTATE )

	-- 이미지 프레임 ID
	local strImgParam = ""	
	if g_CharCreate.m_nSelectSchool == 0 then
		strImgParam = "SM"
	elseif g_CharCreate.m_nSelectSchool == 1 then
		strImgParam = "HA"
	elseif g_CharCreate.m_nSelectSchool == 2 then
		strImgParam = "BH"
	else
		return
	end
	
	local strDec = "NEW_CHAR_SELECT_SCHOOL_DEC" .. tostring( g_CharCreate.m_nSelectSchool )
	strDec = GetGameExternalText( strDec, 0 )
	
	local bMale = GetBool( WIDGET_ID_CHARCREATE, "selected", "charCreate.radioSex.radio_Male" )
	local bFemale = GetBool( WIDGET_ID_CHARCREATE, "selected", "charCreate.radioSex.radio_Female" )
	
	local bClassEnable = false
	if true == bMale or true == bFemale then
		bClassEnable = true
	end
	
	Ev_CharCreate_ToolTip_Open( 1, strImgParam, strDec )
	
	-- 성별 라디오 버튼 활성
	SetEnableSexRadio( true )
	-- 클래스 라디오 비활성
	SetEnableClassRadio( bClassEnable )
	
	-- 캐릭터 학원 선택
	g_CharCreate:SelectCharSchool( g_CharCreate.m_nSelectSchool )
	
	if 	-1 ~= g_CharCreate.m_nSelectSchool and
		-1 ~= g_CharCreate.m_nSelectSex and
		-1 ~= g_CharCreate.m_nSelectClass then
		
		UnselectClassM()
		UnselectClassF()
		
		SetBool( WIDGET_ID_CHARCREATE, "selected", false, "charCreate.radioSex.radio_Male" )
		SetBool( WIDGET_ID_CHARCREATE, "selected", false, "charCreate.radioSex.radio_Female" )
		
		g_CharCreate.m_nSelectSex = -1
		g_CharCreate.m_nSelectClass = -1
		
		g_CharCreate:SetStyleStep( false )
		
	end

end

-- 성별 선택 후 동작
function AfterSexSelect()

	CloseWidget( WIDGET_ID_CHARCREATEROTATE )
	
	-- 이미지 프레임 ID
	local strImgParam = ""	
	
	-- 선택한 성별이 남자라면 남자 클래스 라디오 그룹 visible on
	if g_CharCreate.m_nSelectSex == 0 then
	
		strImgParam = "MALE"
		SetBool( WIDGET_ID_CHARCREATE, "visible", true, "charCreate.radioClassM" )
		SetBool( WIDGET_ID_CHARCREATE, "visible", false, "charCreate.radioClassF" )
		SetInt( WIDGET_ID_CHARCREATE, "selectedIndex", 0, "charCreate.styleSelect.optionFace" )
		SetInt( WIDGET_ID_CHARCREATE, "selectedIndex", 0, "charCreate.styleSelect.optionHair" )
		
	-- 선택한 성별이 여자라면 여자 클래스 라디오 그룹 visible on
	elseif g_CharCreate.m_nSelectSex == 1 then
	
		strImgParam = "FEMALE"
		SetBool( WIDGET_ID_CHARCREATE, "visible", false, "charCreate.radioClassM" )
		SetBool( WIDGET_ID_CHARCREATE, "visible", true, "charCreate.radioClassF" )
		SetInt( WIDGET_ID_CHARCREATE, "selectedIndex", 0, "charCreate.styleSelect.optionFace" )
		SetInt( WIDGET_ID_CHARCREATE, "selectedIndex", 0, "charCreate.styleSelect.optionHair" )
		
	else
		return
		
	end
	
	-- 클래스 라디오 버튼 활성
	SetEnableClassRadio( true )
	
	-- 클래스가 이미 선택되어 있고 성별이 변경 되었다면 캐릭터 숨기기
	if -1 ~= g_CharCreate.m_nSelectClass and
	   -1 ~= g_nCharCreate_Select_Sex_Prev and
	   g_nCharCreate_Select_Sex_Prev ~= g_CharCreate.m_nSelectSex then
	   
		if 1 == g_CharCreate.m_nSelectSex then
			UnselectClassM()
		else
			UnselectClassF()
		end
	
		g_CharCreate.m_nSelectClass = -1
		g_CharCreate:SetStyleStep( false )
	
	end

end

-- 클래스 선택 후 동작
function AtferClassSelect()

	if -1 == g_CharCreate.m_nSelectClass then
		CloseWidget( WIDGET_ID_CHARCREATEROTATE )
		return
	end
	
	OpenWidget( WIDGET_ID_CHARCREATEROTATE )
	
	-- 이미지 프레임 ID
	local strImgParam = ""
	-- 팁 텍스트
	local strDec = ""
	
	if g_CharCreate.m_nSelectClass == CHARCLASS_ARMS_M then
		strImgParam = "SwordmanM"
		strDec = GetGameExternalText( "NEW_CHAR_SELECT_CLASS_DEC1", 0 )
	elseif g_CharCreate.m_nSelectClass == CHARCLASS_ARMS_W then
		strImgParam = "SwordmanF"
		strDec = GetGameExternalText( "NEW_CHAR_SELECT_CLASS_DEC1", 0 )
	elseif g_CharCreate.m_nSelectClass == CHARCLASS_FIGHTER_M then
		strImgParam = "BrawleM"
		strDec = GetGameExternalText( "NEW_CHAR_SELECT_CLASS_DEC3", 0 )
	elseif g_CharCreate.m_nSelectClass == CHARCLASS_FIGHTER_W then
		strImgParam = "BrawleF"
		strDec = GetGameExternalText( "NEW_CHAR_SELECT_CLASS_DEC3", 0 )
	elseif g_CharCreate.m_nSelectClass == CHARCLASS_ARCHER_M then
		strImgParam = "ArcherM"
		strDec = GetGameExternalText( "NEW_CHAR_SELECT_CLASS_DEC2", 0 )
	elseif g_CharCreate.m_nSelectClass == CHARCLASS_ARCHER_W then
		strImgParam = "ArcherF"
		strDec = GetGameExternalText( "NEW_CHAR_SELECT_CLASS_DEC2", 0 )
	elseif g_CharCreate.m_nSelectClass == CHARCLASS_SPIRIT_M then
		strImgParam = "ShamanM"
		strDec = GetGameExternalText( "NEW_CHAR_SELECT_CLASS_DEC4", 0 )
	elseif g_CharCreate.m_nSelectClass == CHARCLASS_SPIRIT_W then
		strImgParam = "ShamanF"
		strDec = GetGameExternalText( "NEW_CHAR_SELECT_CLASS_DEC4", 0 )
	elseif g_CharCreate.m_nSelectClass == CHARCLASS_SCIENTIST_M then
		strImgParam = "ScientistM"
		strDec = GetGameExternalText( "NEW_CHAR_SELECT_CLASS_DEC5", 0 )
	elseif g_CharCreate.m_nSelectClass == CHARCLASS_SCIENTIST_W then
		strImgParam = "ScientistF"
		strDec = GetGameExternalText( "NEW_CHAR_SELECT_CLASS_DEC5", 0 )
	elseif g_CharCreate.m_nSelectClass == CHARCLASS_ASSASSIN_M then
		strImgParam = "AssassinM"
		strDec = GetGameExternalText( "NEW_CHAR_SELECT_CLASS_DEC6", 0 )
	elseif g_CharCreate.m_nSelectClass == CHARCLASS_ASSASSIN_W then
		strImgParam = "AssassinF"
		strDec = GetGameExternalText( "NEW_CHAR_SELECT_CLASS_DEC6", 0 )
	elseif g_CharCreate.m_nSelectClass == CHARCLASS_TRICKER_M then
		strImgParam = "AssassinM"
		strDec = GetGameExternalText( "NEW_CHAR_SELECT_CLASS_DEC7", 0 )
	elseif g_CharCreate.m_nSelectClass == CHARCLASS_TRICKER_W then
		strImgParam = "AssassinF"
		strDec = GetGameExternalText( "NEW_CHAR_SELECT_CLASS_DEC7", 0 )
	elseif g_CharCreate.m_nSelectClass == CHARCLASS_EXTREAM_M then
		strImgParam = "ExtreamM"
		strDec = GetGameExternalText( "NEW_CHAR_SELECT_CLASS_DEC0", 0 )
	elseif g_CharCreate.m_nSelectClass == CHARCLASS_EXTREAM_W then
		strImgParam = "ExtreamF"
		strDec = GetGameExternalText( "NEW_CHAR_SELECT_CLASS_DEC0", 0 )
	elseif g_CharCreate.m_nSelectClass == CHARCLASS_ACTOR_M then
		strImgParam = "ActorM"
		strDec = GetGameExternalText( "NEW_CHAR_SELECT_CLASS_DEC9", 0 )
	elseif g_CharCreate.m_nSelectClass == CHARCLASS_ACTOR_W then
		strImgParam = "ActorF"
		strDec = GetGameExternalText( "NEW_CHAR_SELECT_CLASS_DEC9", 0 )
	else
		return
	end
	
	InitStyleData()
	
	-- 클래스 팁 visible on
	Ev_CharCreate_ToolTip_Open( 2, strImgParam, strDec)
	
	-- 캐릭터 클래스 선택
	g_CharCreate:SelectCharClass( g_CharCreate.m_nSelectClass )
	g_CharCreate:SelectCharFace( GetInt( WIDGET_ID_CHARCREATE, "selectedIndex", "charCreate.styleSelect.optionFace" ) )
	g_CharCreate:SelectCharHair( GetInt( WIDGET_ID_CHARCREATE, "selectedIndex", "charCreate.styleSelect.optionHair" ) )

	-- 완성된 캐릭터 보이기
	g_CharCreate:SetStyleStep( true )
	
end

-- 클래스 남 라디오 버튼 선택 해제
function UnselectClassM()

	SetBool( WIDGET_ID_CHARCREATE, "selected", false, "charCreate.radioClassM.radioSwordman" )
	SetBool( WIDGET_ID_CHARCREATE, "selected", false, "charCreate.radioClassM.radioBrawle" )
	SetBool( WIDGET_ID_CHARCREATE, "selected", false, "charCreate.radioClassM.radioArcher" )
	SetBool( WIDGET_ID_CHARCREATE, "selected", false, "charCreate.radioClassM.radioShaman" )
	SetBool( WIDGET_ID_CHARCREATE, "selected", false, "charCreate.radioClassM.radioScientist" )
	SetBool( WIDGET_ID_CHARCREATE, "selected", false, "charCreate.radioClassM.radioAssassin" )
	SetBool( WIDGET_ID_CHARCREATE, "selected", false, "charCreate.radioClassM.radioTricker" )
	SetBool( WIDGET_ID_CHARCREATE, "selected", false, "charCreate.radioClassM.radioExtream" )
	SetBool( WIDGET_ID_CHARCREATE, "selected", false, "charCreate.radioClassM.radioActor" )
	
	Ev_CharCreate_ToolTip_Close( 2 )
	
end

-- 클래스 여 라디오 버튼 선택 해제
function UnselectClassF()

	SetBool( WIDGET_ID_CHARCREATE, "selected", false, "charCreate.radioClassF.radioSwordman" )
	SetBool( WIDGET_ID_CHARCREATE, "selected", false, "charCreate.radioClassF.radioBrawle" )
	SetBool( WIDGET_ID_CHARCREATE, "selected", false, "charCreate.radioClassF.radioArcher" )
	SetBool( WIDGET_ID_CHARCREATE, "selected", false, "charCreate.radioClassF.radioShaman" )
	SetBool( WIDGET_ID_CHARCREATE, "selected", false, "charCreate.radioClassF.radioScientist" )
	SetBool( WIDGET_ID_CHARCREATE, "selected", false, "charCreate.radioClassF.radioAssassin" )
	SetBool( WIDGET_ID_CHARCREATE, "selected", false, "charCreate.radioClassF.radioTricker" )
	SetBool( WIDGET_ID_CHARCREATE, "selected", false, "charCreate.radioClassF.radioExtream" )
	
	Ev_CharCreate_ToolTip_Close( 2 )

end

------------------------------------------------------------------------------------------------------------------

-- 성문 학원 클릭
function Event_CharCreate_radioSchool_SM()
	g_CharCreate.m_nSelectSchool = 0
	AtferSchoolSelect()
end

-- 현암 학원 클릭
function Event_CharCreate_radioSchool_HA()
	g_CharCreate.m_nSelectSchool = 1
	AtferSchoolSelect()
end

-- 봉황 학원 클릭
function Event_CharCreate_radioSchool_BH()
	g_CharCreate.m_nSelectSchool = 2
	AtferSchoolSelect()
end

-- 남자 클릭
function Event_CharCreate_radioSex_Male()

	if -1 ~= g_CharCreate.m_nSelectSex then
		g_nCharCreate_Select_Sex_Prev = g_CharCreate.m_nSelectSex
	end
	
	g_CharCreate.m_nSelectSex = 0
	AfterSexSelect()
end

-- 여자 클릭
function Event_CharCreate_radioSex_Female()

	if -1 ~= g_CharCreate.m_nSelectSex then
		g_nCharCreate_Select_Sex_Prev = g_CharCreate.m_nSelectSex
	end
	
	g_CharCreate.m_nSelectSex = 1
	AfterSexSelect()
end

-- 검도부 남 클릭
function Event_CharCreate_SwordmanM()
	g_CharCreate.m_nSelectClass = CHARCLASS_ARMS_M
	AtferClassSelect()
end

-- 검도부 여 클릭
function Event_CharCreate_SwordmanF()
	g_CharCreate.m_nSelectClass = CHARCLASS_ARMS_W
	AtferClassSelect()
end

-- 격투부 남 클릭
function Event_CharCreate_BrawleM()
	g_CharCreate.m_nSelectClass = CHARCLASS_FIGHTER_M
	AtferClassSelect()
end

-- 격투부 여 클릭
function Event_CharCreate_BrawleF()
	g_CharCreate.m_nSelectClass = CHARCLASS_FIGHTER_W
	AtferClassSelect()
end

-- 양궁부 남 클릭
function Event_CharCreate_ArcherM()
	g_CharCreate.m_nSelectClass = CHARCLASS_ARCHER_M
	AtferClassSelect()
end

-- 양궁부 여 클릭
function Event_CharCreate_ArcherF()
	g_CharCreate.m_nSelectClass = CHARCLASS_ARCHER_W
	AtferClassSelect()
end

-- 기예부 남 클릭
function Event_CharCreate_ShamanM()
	g_CharCreate.m_nSelectClass = CHARCLASS_SPIRIT_M
	AtferClassSelect()
end

-- 기예부 여 클릭
function Event_CharCreate_ShamanF()
	g_CharCreate.m_nSelectClass = CHARCLASS_SPIRIT_W
	AtferClassSelect()
end

-- 과학부 남 클릭
function Event_CharCreate_ScientistM()
	g_CharCreate.m_nSelectClass = CHARCLASS_SCIENTIST_M
	AtferClassSelect()
end

-- 과학부 여 클릭
function Event_CharCreate_ScientistF()
	g_CharCreate.m_nSelectClass = CHARCLASS_SCIENTIST_W
	AtferClassSelect()
end

-- 인술부 남 클릭
function Event_CharCreate_AssassinM()
	g_CharCreate.m_nSelectClass = CHARCLASS_ASSASSIN_M
	AtferClassSelect()
end

-- 인술부 여 클릭
function Event_CharCreate_AssassinF()
	g_CharCreate.m_nSelectClass = CHARCLASS_ASSASSIN_W
	AtferClassSelect()
end

-- 마술부 남 클릭
function Event_CharCreate_TrickerM()
	g_CharCreate.m_nSelectClass = CHARCLASS_TRICKER_M
	AtferClassSelect()
end

-- 마술부 여 클릭
function Event_CharCreate_TrickerF()
	g_CharCreate.m_nSelectClass = CHARCLASS_TRICKER_W
	AtferClassSelect()
end

-- 극강부 남 클릭
function Event_CharCreate_ExtreamM()
	g_CharCreate.m_nSelectClass = CHARCLASS_EXTREAM_M
	AtferClassSelect()
end

-- 극강부 여 클릭
function Event_CharCreate_ExtreamF()
	g_CharCreate.m_nSelectClass = CHARCLASS_EXTREAM_W
	AtferClassSelect()
end

-- 연극부 남 클릭
function Event_CharCreate_ActorM()
	g_CharCreate.m_nSelectClass = CHARCLASS_ACTOR_M
	AtferClassSelect()
end

-- 연극부 여 클릭
function Event_CharCreate_ActorF()
	g_CharCreate.m_nSelectClass = CHARCLASS_ACTOR_W
	AtferClassSelect()
end

-- 얼굴 인덱스 변경 이벤트
function Event_CharCreate_FaceIndex( nLastIndex, nIndex )

	local bRight = false
	if nLastIndex < nIndex then
		bRight = true
	end

	g_CharCreate:ShiftCharFace( bRight )

end

-- 헤어 인덱스 변경 이벤트
function Event_CharCreate_HairIndex( nLastIndex, nIndex )

	local bRight = false
	if nLastIndex < nIndex then
		bRight = true
	end

	g_CharCreate:ShiftCharHair( bRight )
	
end

-- 캐릭터 생성 버튼 이벤트
function Event_CharCreate_btnNewChar_Click()
end

-- 캐릭터 선택 버튼 이벤트
function Event_CharCreate_btnCharSelect_Click()

	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, GetGameExternalText( "CHARACTERSTAGE_13"),
		Event_CharCreate_btnCharSelect_OK, Event_MsgBox_Close )

end
-- 캐릭터 선택 화면으로 이동 이벤트
function Event_CharCreate_btnCharSelect_OK()
	g_CharCreate:SetStyleStep( false )
	ToCharacterSelectStage()
end

-- 서버변경 버튼 이벤트
function Event_CharCreate_btnServer_Click()

	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, GetGameExternalText( "CHARACTERSTAGE_6"),
		Event_CharCreate_btnServer_OK, Event_MsgBox_Close )

end

-- 서버 선택 화면으로 이동 이벤트
function Event_CharCreate_btnServer_OK()

	g_CharCreate:SetStyleStep( false )
	ToServerSelectStage()
	
	-- 서버 연결을 끊어버리니 연결 시도해본다.
	TryConnectLoginServer()
	
end

-- 생성완료 버튼 이벤트
function Event_CharCreate_btnComplete_Click()

	if -1 == g_CharCreate.m_nSelectSchool then
	
		OpenMsgBox( MSG_BOX_TYPE_OK, GetGameExternalText( "NEW_CHAR_SELECT_SCHOOL_ERROR", 0 ), Event_MsgBox_Close )
		return
		
	elseif -1 == g_CharCreate.m_nSelectSex then
	
		OpenMsgBox( MSG_BOX_TYPE_OK, GetGameExternalText( "NEW_CHAR_SELECT_SEX_ERROR", 0 ), Event_MsgBox_Close )
		return
		
	elseif -1 == g_CharCreate.m_nSelectClass then
	
		OpenMsgBox( MSG_BOX_TYPE_OK, GetGameExternalText( "NEW_CHAR_SELECT_CLASS_ERROR", 0 ), Event_MsgBox_Close )
		return
		
	end
	
	-- 캐릭터 이름
	local strCharName 
	
	local nSP = GetServiceProvider()
	if nSP == SP_OFFICE_TEST or 
		nSP == SP_KOREA_TEST or 
		nSP == SP_KOREA then
		
		strCharName 	= GetString( WIDGET_ID_CHARCREATE, "text", "charCreate.inputName" )
		
	else
	
		strCharName 	= GetString( WIDGET_ID_CHARCREATE, "text", "charCreate.inputName" )

	end
	
	if 0 == string.len( strCharName )then
		OpenMsgBox( MSG_BOX_TYPE_OK, GetGameExternalText( "CREATESTAGE_2", 0 ), Event_MsgBox_Close )
		return
	end
	
	-- 캐릭터 생성
	local nResult = g_Msg:CreateChar( strCharName )
	
	if 1 == nResult then
		OpenMsgBox( MSG_BOX_TYPE_OK, GetGameExternalText( "CREATE_CHARACTER_NAME_ERROR", 0 ), Event_MsgBox_Close )
	elseif 2 == nResult then
		OpenMsgBox( MSG_BOX_TYPE_OK, GetGameExternalText( "CREATE_CHARACTER_NAME_ERROR_WB", 0 ), Event_MsgBox_Close )
	elseif 3 == nResult then
		OpenMsgBox( MSG_BOX_TYPE_OK, GetGameExternalText( "CREATESTAGE_9", 0 ), Event_MsgBox_Close )
	elseif 4 == nResult then
		OpenMsgBox( MSG_BOX_TYPE_OK, GetGameExternalText( "CREATESTAGE_10", 0 ), Event_MsgBox_Close )
	elseif 5 == nResult then
		OpenMsgBox( MSG_BOX_TYPE_OK, GetGameExternalText( "CREATE_CHARACTER_NAME_ERROR", 0 ), Event_MsgBox_Close )
	elseif 6 == nResult then
		OpenMsgBox( MSG_BOX_TYPE_OK, GetGameExternalText( "CHARACTER_BADNAME", 0 ), Event_MsgBox_Close )
	end

end

-- 캐릭터 생성 완료 메시지 OK
function Event_MsgBox_NewCharSucceed()
	g_CharCreate:SetStyleStep( false )
	ToCharacterSelectStage()
end

------------------------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 캐릭터 생성 결과
function CharCreate_NewCharResult( nResult )

	-- 캐릭터 생성 성공
	if 1 == nResult then
		OpenMsgBox( MSG_BOX_TYPE_OK, GetGameExternalText( "CREATESTAGE_4", 0 ), Event_MsgBox_NewCharSucceed )
	
	-- 같은 이름의 캐릭터가 이미 존재함
	elseif 2 == nResult then
		OpenMsgBox( MSG_BOX_TYPE_OK, GetGameExternalText( "CREATESTAGE_5", 0 ), Event_MsgBox_Close )
		
	-- 캐릭터 글자수 제한
	elseif 3 == nResult then
		OpenMsgBox( MSG_BOX_TYPE_OK, GetGameExternalText( "NEWCHAR_NAME_TOO_SHORT", 0 ), Event_MsgBox_Close )
		
	-- 더이상캐릭터 만들수 없음(최대캐릭터 숫자 초과)
	elseif 4 == nResult then
		OpenMsgBox( MSG_BOX_TYPE_OK, GetGameExternalText( "CREATESTAGE_6", 0 ), Event_MsgBox_Close )
		
	-- 에러발생 만들수 없음(시스템에러, 허용되지 않는 이름)
	elseif 5 == nResult then
		OpenMsgBox( MSG_BOX_TYPE_OK, GetGameExternalText( "CREATESTAGE_7", 0 ), Event_MsgBox_Close )
		
	-- 태국어문자 완성되지 않은 문자 조합 에러
	elseif 6 == nResult then
		OpenMsgBox( MSG_BOX_TYPE_OK, GetGameExternalText( "CREATESTAGE_9", 0 ), Event_MsgBox_Close )
		
	-- 베트남 문자 조합 에러
	elseif 7 == nResult then
		OpenMsgBox( MSG_BOX_TYPE_OK, GetGameExternalText( "CREATESTAGE_10", 0 ), Event_MsgBox_Close )
		
	-- 문자 오류
	elseif 8 == nResult then
		OpenMsgBox( MSG_BOX_TYPE_OK, GetGameExternalText( "CHARACTER_BADNAME", 0 ), Event_MsgBox_Close )
		
	-- 한국문자 조합 오류
	elseif 9 == nResult then
		OpenMsgBox( MSG_BOX_TYPE_OK, GetGameExternalText( "CREATE_CHARACTER_NAME_ERROR_KOREA", 0 ), Event_MsgBox_Close )
		
	-- 캐릭터의 이름은 영문과 숫자의 조합으로만 이루어져야 합니다.
	elseif 10 == nResult then
		OpenMsgBox( MSG_BOX_TYPE_OK, GetGameExternalText( "CREATE_CHARACTER_NAME_ERROR_WB", 0 ), Event_MsgBox_Close )
		
	end

end
