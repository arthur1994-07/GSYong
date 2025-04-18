
g_nGameOption_SelectPage = 0			-- 현재 선택한 페이지
g_tbGameOption_ResolutionList = {}		-- 해상도 리스트
g_nGameOption_SelectTexResolution = -1

g_bGameOption_IsChangeAlpha = false

-- 게임 옵션 UI 로드 완료
function LoadGameOption()

	local tbData = {}

	-- 탭 데이터 생성
	local strTabData = ""
	for i = 0, 3, 1 do
		strTabData = strTabData .. GetGameWordText( "HWOPTION_AV_NAME", i ) .. ";"
	end

	-- 탭 설정
	SetData( WIDGET_ID_GAMEOPTION, strTabData, "tabPage", false )
	SetInt( WIDGET_ID_GAMEOPTION, "selectedIndex", g_nGameOption_SelectPage, "tabPage" )
	SetEvent( WIDGET_ID_GAMEOPTION, EVENT_INDEX_CLICK_CHANGE, "Ev_GameOption_TabPage", "index;", "tabPage" )
	
	GameOption_UpdatePage()
	
	-- 적용/확인 버튼
	SetString( WIDGET_ID_GAMEOPTION, "label", GetGameWordText( "CHARACTER_STAT_APPLY", 0 ), "btnApply" )
	SetString( WIDGET_ID_GAMEOPTION, "label", GetGameWordText( "CHAT_OK_BUTTON", 0 ), "btnOK" )
	SetEvent( WIDGET_ID_GAMEOPTION, EVENT_BUTTON_CLICK, "Ev_GameOption_Apply", "", "btnApply" )
	SetEvent( WIDGET_ID_GAMEOPTION, EVENT_BUTTON_CLICK, "Ev_GameOption_OK", "", "btnOK" )
	
	-- 그래픽 --------------------------------------------------------------------------------------------
	
	-- 텍스트 설정
	SetString( WIDGET_ID_GAMEOPTION, "text", GetGameWordText( "HWOPTION_VIDEO_OPTION", 10 ), "pageGraphic.labelDefault" )
	SetString( WIDGET_ID_GAMEOPTION, "text", GetGameWordText( "HWOPTION_VIDEO_OPTION", 0 ), "pageGraphic.labelResolution" )
	SetString( WIDGET_ID_GAMEOPTION, "text", GetGameWordText( "HWOPTION_VIDEO_OPTION", 1 ), "pageGraphic.labelShadow" )
	SetString( WIDGET_ID_GAMEOPTION, "text", GetGameWordText( "HWOPTION_VIDEO_OPTION", 2 ), "pageGraphic.labelDetail" )
	SetString( WIDGET_ID_GAMEOPTION, "text", GetGameWordText( "HWOPTION_VIDEO_OPTION", 3 ), "pageGraphic.labelSight" )
	SetString( WIDGET_ID_GAMEOPTION, "text", GetGameWordText( "HWOPTION_VIDEO_OPTION", 17 ), "pageGraphic.labelPhysical" )
	SetString( WIDGET_ID_GAMEOPTION, "text", GetGameWordText( "HWOPTION_VIDEO_OPTION", 16 ), "pageGraphic.labelTextureFilter" )
	SetString( WIDGET_ID_GAMEOPTION, "text", GetGameWordText( "HWOPTION_VIDEO_OPTION", 18 ), "pageGraphic.labelTextureRes" )
	
	SetString( WIDGET_ID_GAMEOPTION, "text", GetGameWordText( "HWOPTION_VIDEO_OPTION", 4 ), "pageGraphic.checkBuff_text" )
	SetString( WIDGET_ID_GAMEOPTION, "text", GetGameWordText( "HWOPTION_VIDEO_OPTION", 6 ), "pageGraphic.checkReflect_text" )
	SetString( WIDGET_ID_GAMEOPTION, "text", GetGameWordText( "HWOPTION_VIDEO_OPTION", 7 ), "pageGraphic.checkDistortion_text" )
	SetString( WIDGET_ID_GAMEOPTION, "text", GetGameWordText( "HWOPTION_VIDEO_OPTION", 8 ), "pageGraphic.checkGlow_text" )
	SetString( WIDGET_ID_GAMEOPTION, "text", GetGameWordText( "HWOPTION_VIDEO_OPTION", 9 ), "pageGraphic.checkPostProc_text" )
	SetString( WIDGET_ID_GAMEOPTION, "text", GetGameWordText( "HWOPTION_VIDEO_OPTION", 11 ), "pageGraphic.checkVSync_text" )
	SetString( WIDGET_ID_GAMEOPTION, "text", GetGameWordText( "HWOPTION_VIDEO_OPTION", 12 ), "pageGraphic.checkTargetEff_text" )
	
	SetString( WIDGET_ID_GAMEOPTION, "text", GetGameWordText( "HWOPTION_VIDEO_OPTION", 13 ), "pageGraphic.checkItemEff_text" )
	SetString( WIDGET_ID_GAMEOPTION, "text", GetGameWordText( "HWOPTION_VIDEO_OPTION", 14 ), "pageGraphic.checkMapPoint_text" )
	SetString( WIDGET_ID_GAMEOPTION, "text", GetGameWordText( "HWOPTION_VIDEO_OPTION", 15 ), "pageGraphic.checkNormalmap_text" )
	
	-- 콤보박스 설정
	-- 기본설정
	for i = 0, 4, 1 do	
		tbData[ i + 1 ] = { GetGameWordText( "HWOPTION_DEFAULT_OPTION", i ) }
	end	
	SetDataEx( WIDGET_ID_GAMEOPTION, "label;", tbData, "pageGraphic.comboDefault", false )
	SetInt( WIDGET_ID_GAMEOPTION, "selectedIndex", g_DxParamSet.dwDefault, "pageGraphic.comboDefault" )
	SetEvent( WIDGET_ID_GAMEOPTION, EVENT_LIST_INDEX_CHANGE, "Ev_GameOption_ComboDefault", "index;", "pageGraphic.comboDefault" )
	
	-- 그림자
	tbData = {}
	for i = 0, 4, 1 do
		tbData[ i + 1 ] = { GetGameWordText( "HWOPTION_VIDEO_SHADOW", i ) }
	end
	SetDataEx( WIDGET_ID_GAMEOPTION, "label;", tbData, "pageGraphic.comboShadow", false )
	SetInt( WIDGET_ID_GAMEOPTION, "selectedIndex", g_DxParamSet.dwShadow, "pageGraphic.comboShadow" )
	SetEvent( WIDGET_ID_GAMEOPTION, EVENT_LIST_INDEX_CHANGE, "Ev_GameOption_OnCustom", "", "pageGraphic.comboShadow" )
	
	-- 캐릭터 디테일
	tbData = {}
	for i = 0, 2, 1 do
		tbData[ i + 1 ] = { GetGameWordText( "HWOPTION_VIDEO_SKINDETAIL", i ) }
	end
	SetDataEx( WIDGET_ID_GAMEOPTION, "label;", tbData, "pageGraphic.comboDetail", false )
	SetInt( WIDGET_ID_GAMEOPTION, "selectedIndex", g_DxParamSet.dwDetail, "pageGraphic.comboDetail" )
	SetEvent( WIDGET_ID_GAMEOPTION, EVENT_LIST_INDEX_CHANGE, "Ev_GameOption_OnCustom", "", "pageGraphic.comboDetail" )
	
	-- 시야
	tbData = {}
	for i = 0, 3, 1 do
		tbData[ i + 1 ] = { GetGameWordText( "HWOPTION_VIDEO_SIGHT", i ) }
	end
	SetDataEx( WIDGET_ID_GAMEOPTION, "label;", tbData, "pageGraphic.comboSight", false )
	SetInt( WIDGET_ID_GAMEOPTION, "selectedIndex", g_DxParamSet.dwSight, "pageGraphic.comboSight" )
	SetEvent( WIDGET_ID_GAMEOPTION, EVENT_LIST_INDEX_CHANGE, "Ev_GameOption_OnCustom", "", "pageGraphic.comboSight" )
	
	-- 물리 적용 범위
	tbData = {}
	for i = 0, 2, 1 do
		tbData[ i + 1 ] = { GetGameWordText( "HWOPTION_VIDEO_PHYSXCLOTH", i ) }
	end
	SetDataEx( WIDGET_ID_GAMEOPTION, "label;", tbData, "pageGraphic.comboPhysical", false )
	SetInt( WIDGET_ID_GAMEOPTION, "selectedIndex", g_DxParamSet.dwPhysXCloth, "pageGraphic.comboPhysical" )
	SetEvent( WIDGET_ID_GAMEOPTION, EVENT_LIST_INDEX_CHANGE, "Ev_GameOption_OnCustom", "", "pageGraphic.comboPhysical" )
	
	-- 텍스처 필터링
	tbData = {}
	for i = 0, 4, 1 do
		tbData[ i + 1 ] = { GetGameWordText( "HWOPTION_VIDEO_TEXFILTER", i ) }
	end
	SetDataEx( WIDGET_ID_GAMEOPTION, "label;", tbData, "pageGraphic.comboTextureFilter", false )
	SetInt( WIDGET_ID_GAMEOPTION, "selectedIndex", g_DxParamSet.dwTexFilter, "pageGraphic.comboTextureFilter" )
	SetEvent( WIDGET_ID_GAMEOPTION, EVENT_LIST_INDEX_CHANGE, "Ev_GameOption_OnCustom", "", "pageGraphic.comboTextureFilter" )
	
	-- 텍스처 해상도
	tbData = {}
	for i = 0, 2, 1 do
		tbData[ i + 1 ] = { GetGameWordText( "HWOPTION_VIDEO_SKINDETAIL", i ) }
	end
	SetDataEx( WIDGET_ID_GAMEOPTION, "label;", tbData, "pageGraphic.comboTextureRes", false )
	SetInt( WIDGET_ID_GAMEOPTION, "selectedIndex", g_DxParamSet.dwTexQuality, "pageGraphic.comboTextureRes" )
	SetEvent( WIDGET_ID_GAMEOPTION, EVENT_LIST_INDEX_CHANGE, "Ev_GameOption_OnCustom", "", "pageGraphic.comboTextureRes" )
	
	--해상도
	local nResolutionSelect = 0
	
	tbData = {}
	g_tbGameOption_ResolutionList = GetResolutionList()
	for i = 1, table.getn( g_tbGameOption_ResolutionList ), 1 do
	
		local tbResolution = g_tbGameOption_ResolutionList[ i ]
		local strResolution = ""
		
		strResolution = tostring( tbResolution[ "nWidth" ] ) .. "x" .. tostring( tbResolution[ "nHeight" ] ) .. "x" .. 
			tostring( tbResolution[ "nFormat" ] )
			
		if false == tbResolution[ "bRecommend" ] then
			strResolution = strResolution .. " " .. GetGameWordText( "DISCOURAGES", 0 )
		end
		
		tbData[ i ] = { strResolution }
		
		if 	g_DxParamSet.dwWidth == tbResolution[ "nWidth" ] and
			g_DxParamSet.dwHeight == tbResolution[ "nHeight" ] then
			
			if 	16 == tbResolution[ "nFormat" ] and 
				0 == g_DxParamSet.emFormat then
				nResolutionSelect = i - 1
			elseif  32 == tbResolution[ "nFormat" ] and
					1 == g_DxParamSet.emFormat  then
				nResolutionSelect = i - 1
			end
			
		end
	
	end
	SetDataEx( WIDGET_ID_GAMEOPTION, "label;", tbData, "pageGraphic.comboResolution", false )
	SetInt( WIDGET_ID_GAMEOPTION, "selectedIndex", nResolutionSelect, "pageGraphic.comboResolution" )
	-- 창모드에서도 해상도 변경할 수 있게 주석 처리함.
	--SetBool( WIDGET_ID_GAMEOPTION, "enabled", IsFullScreen(), "pageGraphic.comboResolution" )
	
	-- 체크박스 설정
	SetBool( WIDGET_ID_GAMEOPTION, "selected", g_DxParamSet.bBuff, "pageGraphic.checkBuff" )
	SetEvent( WIDGET_ID_GAMEOPTION, EVENT_BUTTON_CLICK, "Ev_GameOption_OnCustom", "", "pageGraphic.checkBuff" )
	SetBool( WIDGET_ID_GAMEOPTION, "selected", g_DxParamSet.bRealReflect, "pageGraphic.checkReflect" )
	SetEvent( WIDGET_ID_GAMEOPTION, EVENT_BUTTON_CLICK, "Ev_GameOption_OnCustom", "", "pageGraphic.checkReflect" )
	SetBool( WIDGET_ID_GAMEOPTION, "selected", g_DxParamSet.bRefract, "pageGraphic.checkDistortion" )
	SetEvent( WIDGET_ID_GAMEOPTION, EVENT_BUTTON_CLICK, "Ev_GameOption_OnCustom", "", "pageGraphic.checkDistortion" )
	SetBool( WIDGET_ID_GAMEOPTION, "selected", g_DxParamSet.bGlow, "pageGraphic.checkGlow" )
	SetEvent( WIDGET_ID_GAMEOPTION, EVENT_BUTTON_CLICK, "Ev_GameOption_OnCustom", "", "pageGraphic.checkGlow" )
	SetBool( WIDGET_ID_GAMEOPTION, "selected", g_DxParamSet.bPost, "pageGraphic.checkPostProc" )
	SetEvent( WIDGET_ID_GAMEOPTION, EVENT_BUTTON_CLICK, "Ev_GameOption_OnCustom", "", "pageGraphic.checkPostProc" )
	SetBool( WIDGET_ID_GAMEOPTION, "selected", g_DxParamSet.bFrameLimit, "pageGraphic.checkVSync" )
	--SetEvent( WIDGET_ID_GAMEOPTION, EVENT_BUTTON_CLICK, "Ev_GameOption_OnCustom", "", "pageGraphic.checkVSync" )
	SetBool( WIDGET_ID_GAMEOPTION, "selected", g_DxParamSet.bTargetEffect, "pageGraphic.checkTargetEff" )
	SetEvent( WIDGET_ID_GAMEOPTION, EVENT_BUTTON_CLICK, "Ev_GameOption_OnCustom", "", "pageGraphic.checkTargetEff" )
	SetBool( WIDGET_ID_GAMEOPTION, "selected", g_DxParamSet.bClickEffect, "pageGraphic.checkMapPoint" )
	SetEvent( WIDGET_ID_GAMEOPTION, EVENT_BUTTON_CLICK, "Ev_GameOption_OnCustom", "", "pageGraphic.checkMapPoint" )
	SetBool( WIDGET_ID_GAMEOPTION, "selected", g_DxParamSet.bMineEffect, "pageGraphic.checkItemEff" )
	SetEvent( WIDGET_ID_GAMEOPTION, EVENT_BUTTON_CLICK, "Ev_GameOption_OnCustom", "", "pageGraphic.checkItemEff" )
	
	if true == g_DxParamSet.bPossibleNormalMap then
		SetBool( WIDGET_ID_GAMEOPTION, "selected", g_DxParamSet.bUseNormalMap, "pageGraphic.checkNormalmap" )
	else
		SetBool( WIDGET_ID_GAMEOPTION, "selected", false, "pageGraphic.checkNormalmap" )
	end
	SetEvent( WIDGET_ID_GAMEOPTION, EVENT_BUTTON_CLICK, "Ev_GameOption_OnCustom", "", "pageGraphic.checkNormalmap" )
	
	
	-- 소리 ----------------------------------------------------------------------------------------------
	
	-- 체크박스
	SetString( WIDGET_ID_GAMEOPTION, "text", GetGameWordText( "HWOPTION_AUDIO", 0 ), "pageSound.checkEffect_text" )
	SetString( WIDGET_ID_GAMEOPTION, "text", GetGameWordText( "HWOPTION_AUDIO", 1 ), "pageSound.checkEnvironment_text" )
	SetString( WIDGET_ID_GAMEOPTION, "text", GetGameWordText( "HWOPTION_AUDIO", 2 ), "pageSound.checkBGM_text" )
	
	SetBool( WIDGET_ID_GAMEOPTION, "selected", GetMuteSfx(), "pageSound.checkEffect" )
	SetBool( WIDGET_ID_GAMEOPTION, "selected", GetMuteEnv(), "pageSound.checkEnvironment" )	
	SetBool( WIDGET_ID_GAMEOPTION, "selected", GetMuteBGM(), "pageSound.checkBGM" )
	
	-- 슬라이더
	SetDouble( WIDGET_ID_GAMEOPTION, "maximum", 1, "pageSound.sliderEffect" )
	SetDouble( WIDGET_ID_GAMEOPTION, "maximum", 1, "pageSound.sliderEnvironment" )
	SetDouble( WIDGET_ID_GAMEOPTION, "maximum", 1, "pageSound.sliderBGM" )	
	SetDouble( WIDGET_ID_GAMEOPTION, "minimum", 0, "pageSound.sliderEffect" )
	SetDouble( WIDGET_ID_GAMEOPTION, "minimum", 0, "pageSound.sliderEnvironment" )
	SetDouble( WIDGET_ID_GAMEOPTION, "minimum", 0, "pageSound.sliderBGM" )
	
	SetDouble( WIDGET_ID_GAMEOPTION, "value", GameOption_ToSoundPercent( GetSfxVolume() ), "pageSound.sliderEffect" )
	SetDouble( WIDGET_ID_GAMEOPTION, "value", GameOption_ToSoundPercent( GetEnvVolume() ), "pageSound.sliderEnvironment" )
	SetDouble( WIDGET_ID_GAMEOPTION, "value", GameOption_ToSoundPercent( GetBGMVolume() ), "pageSound.sliderBGM" )
	
	SetEvent( WIDGET_ID_GAMEOPTION, EVENT_SLIDER_VALUE_CHANGE, "Ev_GameOption_SliderEffect", "value", "pageSound.sliderEffect" )
	SetEvent( WIDGET_ID_GAMEOPTION, EVENT_SLIDER_VALUE_CHANGE, "Ev_GameOption_SliderEnv", "value", "pageSound.sliderEnvironment" )
	SetEvent( WIDGET_ID_GAMEOPTION, EVENT_SLIDER_VALUE_CHANGE, "Ev_GameOption_SliderBGM", "value", "pageSound.sliderBGM" )
	
	-- 게임 ----------------------------------------------------------------------------------------------

	-- 텍스트 설정
	SetString( WIDGET_ID_GAMEOPTION, "text", GetGameWordText( "HWOPTION_GAMEPLAY_OPTION", 0 ), "pageGame.checkDuel_text" )
	SetString( WIDGET_ID_GAMEOPTION, "text", GetGameWordText( "HWOPTION_GAMEPLAY_OPTION", 1 ), "pageGame.checkTrade_text" )
	SetString( WIDGET_ID_GAMEOPTION, "text", GetGameWordText( "HWOPTION_GAMEPLAY_OPTION", 2 ), "pageGame.checkParty_text" )
	SetString( WIDGET_ID_GAMEOPTION, "text", GetGameWordText( "HWOPTION_GAMEPLAY_OPTION", 4 ), "pageGame.checkFriend_text" )
	SetString( WIDGET_ID_GAMEOPTION, "text", GetGameWordText( "HWOPTION_GAMEPLAY_OPTION", 8 ), "pageGame.checkAttack_text" )
	SetString( WIDGET_ID_GAMEOPTION, "text", GetGameWordText( "HWOPTION_GAMEPLAY_OPTION", 7 ), "pageGame.checkTip_text" )
	--SetString( WIDGET_ID_GAMEOPTION, "label", GetGameWordText( "HWOPTION_GAMEPLAY_OPTION", 9 ), "pageGame.checkName" )
	SetString( WIDGET_ID_GAMEOPTION, "text", GetGameWordText( "HWOPTION_GAMEPLAY_OPTION", 11 ), "pageGame.labelCharSimple" )
	SetString( WIDGET_ID_GAMEOPTION, "text", GetGameWordText( "HWOPTION_GAMEPLAY_OPTION", 13 ), "pageGame.labelSkill" )
	SetString( WIDGET_ID_GAMEOPTION, "text", GetGameInternalText( "REBIRTH_WINDOW_AUTO_REBIRTH", 0 ), "pageGame.checkAutoResurrect_text" )
	SetString( WIDGET_ID_GAMEOPTION, "text", GetGameWordText( "HWOPTION_GAMEPLAY_OPTION", 14 ), "pageGame.checkRide_text" )
	SetString( WIDGET_ID_GAMEOPTION, "text", GetGameWordText( "HWOPTION_GAMEPLAY_OPTION", 15 ), "pageGame.checkMobHP_text" )
	SetString( WIDGET_ID_GAMEOPTION, "text", GetGameWordText( "HWOPTION_GAMEPLAY_OPTION", 16 ), "pageGame.checkMyInfo_text" )
	SetString( WIDGET_ID_GAMEOPTION, "text", GetGameWordText( "HWOPTION_GAMEPLAY_OPTION", 17 ), "pageGame.checkGroupChat_text" )
	
	-- 콤보박스 설정
	
	-- 캐릭터 단순화 등급
	tbData = {}
	for i = 0, 3, 1 do
		tbData[ i + 1 ] = { GetGameWordText( "HWOPTION_GAMEPLAY_SIMPLE", i ) }	
	end
	SetDataEx( WIDGET_ID_GAMEOPTION, "label;", tbData, "pageGame.comboCharSimple", false )
	SetString( WIDGET_ID_GAMEOPTION, "TooltipText", 
		GetGameInternalText( "CHARACTER_SIMPLIFY_TOOLTIP", 0 ), "pageGame.comboCharSimple" )
	
	-- 스킬타입 표시
	tbData = {}
	for i = 0, 2, 1 do
		tbData[ i + 1 ] = { GetGameWordText( "HWOPTION_GAMEPLAY_SKILLTYPE", i ) }
	end
	SetDataEx( WIDGET_ID_GAMEOPTION, "label;", tbData, "pageGame.comboSkill", false )
	
	local tbGameOption = GetGameOption()
	SetBool( WIDGET_ID_GAMEOPTION, "selected", tbGameOption[ "bDuel" ], "pageGame.checkDuel" )
	SetBool( WIDGET_ID_GAMEOPTION, "selected", tbGameOption[ "bTrade" ], "pageGame.checkTrade" )
	SetBool( WIDGET_ID_GAMEOPTION, "selected", tbGameOption[ "bParty" ], "pageGame.checkParty" )
	SetBool( WIDGET_ID_GAMEOPTION, "selected", tbGameOption[ "bFriend" ], "pageGame.checkFriend" )
	SetBool( WIDGET_ID_GAMEOPTION, "selected", tbGameOption[ "bAttack" ], "pageGame.checkAttack" )
	SetBool( WIDGET_ID_GAMEOPTION, "selected", tbGameOption[ "bTip" ], "pageGame.checkTip" )
	--SetBool( WIDGET_ID_GAMEOPTION, "selected", tbGameOption[ "bName" ], "pageGame.checkName" )
	SetInt( WIDGET_ID_GAMEOPTION, "selectedIndex", tbGameOption[ "nCharSimple" ], "pageGame.comboCharSimple" )
	SetInt( WIDGET_ID_GAMEOPTION, "selectedIndex", tbGameOption[ "nSkill" ], "pageGame.comboSkill" )
	SetBool( WIDGET_ID_GAMEOPTION, "selected", tbGameOption[ "bCheckAutoRebirth" ], "pageGame.checkAutoResurrect" )
	SetBool( WIDGET_ID_GAMEOPTION, "selected", tbGameOption[ "bRide" ], "pageGame.checkRide" )
	SetBool( WIDGET_ID_GAMEOPTION, "selected", tbGameOption[ "bMobHP" ], "pageGame.checkMobHP" )
	SetBool( WIDGET_ID_GAMEOPTION, "selected", tbGameOption[ "bMyInfo" ], "pageGame.checkMyInfo" )
	SetBool( WIDGET_ID_GAMEOPTION, "selected", tbGameOption[ "bGroupChat" ], "pageGame.checkGroupChat" )
	
	
	-- UI ----------------------------------------------------------------------------------------------
	
	-- 안티 앨리어싱
	SetString( WIDGET_ID_GAMEOPTION, "text", GetGameWordText( "GAMEOPTION_UI_ANTI_ALIASING", 0 ), "pageUI.labelQuality" )
	tbData = {}
	for i = 1, 2, 1 do
		tbData[ i ] = { GetGameWordText( "GAMEOPTION_UI_ANTI_ALIASING", i ) }
	end
	SetDataEx( WIDGET_ID_GAMEOPTION, "label;", tbData, "pageUI.comboQuality", false )
	SetInt( WIDGET_ID_GAMEOPTION, "selectedIndex", tbGameOption[ "nUIAntiAliasing" ], "pageUI.comboQuality" )
	
	-- 미니 HP 보기
	SetBool( WIDGET_ID_GAMEOPTION, "selected", tbGameOption[ "bMiniHP" ], "pageUI.checkMiniHP" )
	SetString( WIDGET_ID_GAMEOPTION, "TooltipText", GetGameInternalText( "UI_PERFORMANCE_WARNNING", 0 ), "pageUI.checkMiniHP_text" )
	SetString( WIDGET_ID_GAMEOPTION, "text", GetGameWordText( "HWOPTION_GAMEPLAY_OPTION", 3 ), "pageUI.checkMiniHP_text" )
	
	-- 다른 사람 데미지 보기
	SetString( WIDGET_ID_GAMEOPTION, "text", GetGameWordText( "HWOPTION_GAMEPLAY_OPTION", 20 ), "pageUI.checkShowOtherDamage_text" )
	SetString( WIDGET_ID_GAMEOPTION, "TooltipText", GetGameInternalText( "UI_PERFORMANCE_WARNNING", 0 ), "pageUI.checkShowOtherDamage_text" )
	SetBool( WIDGET_ID_GAMEOPTION, "selected", tbGameOption[ "bShowOtherDamage" ], "pageUI.checkShowOtherDamage" )
	
	-- UI 기본 위치로
	SetString( WIDGET_ID_GAMEOPTION, "label", GetGameWordText( "HWOPTION_GAMEPLAY_OPTION", 5 ), "pageUI.btnDefaultPos" )
	SetEvent( WIDGET_ID_GAMEOPTION, EVENT_BUTTON_CLICK, "Ev_GameOption_DefaultPos", "", "pageUI.btnDefaultPos" )
	
	-- 이름 보기 설정
	SetString( WIDGET_ID_GAMEOPTION, "label", GetGameWordText( "HWOPTION_GAMEPLAY_OPTION", 19 ), "pageUI.btnNameConfig" )
	SetEvent( WIDGET_ID_GAMEOPTION, EVENT_BUTTON_CLICK, "Ev_GameOption_NameConfig", "", "pageUI.btnNameConfig" )
	
	-- 채팅창 뚫기
	SetBool( WIDGET_ID_GAMEOPTION, "selected", tbGameOption[ "bChat" ], "pageUI.checkChat" )
	SetString( WIDGET_ID_GAMEOPTION, "text", GetGameWordText( "HWOPTION_GAMEPLAY_OPTION", 6 ), "pageUI.checkChat_text" )
	
	-- 이름, 데미지 디스플레이 개수 제한 해제
	SetBool( WIDGET_ID_GAMEOPTION, "selected", not tbGameOption[ "bDisplayLimit" ], "pageUI.checkDisplayLimit" )
	SetString( WIDGET_ID_GAMEOPTION, "TooltipText", GetGameInternalText( "UI_PERFORMANCE_WARNNING", 0 ), "pageUI.checkDisplayLimit_text" )
	SetString( WIDGET_ID_GAMEOPTION, "text", GetGameWordText( "HWOPTION_GAMEPLAY_OPTION", 21 ), "pageUI.checkDisplayLimit_text" )
	
	-- 전체 UI 알파값
	SetString( WIDGET_ID_GAMEOPTION, "text", "UI Aplha", "pageUI.labelUIAlphaStatic" )
	SetDouble( WIDGET_ID_GAMEOPTION, "minimum", 0.25, "pageUI.sliderUIAlpha" )
	SetDouble( WIDGET_ID_GAMEOPTION, "maximum", 1.0, "pageUI.sliderUIAlpha" )
	SetDouble( WIDGET_ID_GAMEOPTION, "value", tbGameOption[ "fUIAlpha" ], "pageUI.sliderUIAlpha" )
	SetEvent( WIDGET_ID_GAMEOPTION, EVENT_SLIDER_VALUE_CHANGE, "Ev_GameOption_UIAlpha", "value", "pageUI.sliderUIAlpha", "0" )
	Ev_GameOption_UIAlpha( tbGameOption[ "fUIAlpha" ], true )

end

function OpenGameOption()

	g_nGameOption_SelectTexResolution = GetInt( WIDGET_ID_GAMEOPTION, "selectedIndex", "pageGraphic.comboTextureRes" )

end

-- 페이지 업데이트
function GameOption_UpdatePage()

	-- 그래픽
	if 0 == g_nGameOption_SelectPage then
		
		SetBool( WIDGET_ID_GAMEOPTION, "visible", true, "pageGraphic" )
		SetBool( WIDGET_ID_GAMEOPTION, "visible", false, "pageSound" )
		SetBool( WIDGET_ID_GAMEOPTION, "visible", false, "pageGame" )
		SetBool( WIDGET_ID_GAMEOPTION, "visible", false, "pageUI" )
	
	-- 소리
	elseif 1 == g_nGameOption_SelectPage then
	
		SetBool( WIDGET_ID_GAMEOPTION, "visible", false, "pageGraphic" )
		SetBool( WIDGET_ID_GAMEOPTION, "visible", true, "pageSound" )
		SetBool( WIDGET_ID_GAMEOPTION, "visible", false, "pageGame" )
		SetBool( WIDGET_ID_GAMEOPTION, "visible", false, "pageUI" )
	
	-- 게임
	elseif 2 == g_nGameOption_SelectPage then
	
		SetBool( WIDGET_ID_GAMEOPTION, "visible", false, "pageGraphic" )
		SetBool( WIDGET_ID_GAMEOPTION, "visible", false, "pageSound" )
		SetBool( WIDGET_ID_GAMEOPTION, "visible", true, "pageGame" )
		SetBool( WIDGET_ID_GAMEOPTION, "visible", false, "pageUI" )
		
	-- UI
	elseif 3 == g_nGameOption_SelectPage then
	
		SetBool( WIDGET_ID_GAMEOPTION, "visible", false, "pageGraphic" )
		SetBool( WIDGET_ID_GAMEOPTION, "visible", false, "pageSound" )
		SetBool( WIDGET_ID_GAMEOPTION, "visible", false, "pageGame" )
		SetBool( WIDGET_ID_GAMEOPTION, "visible", true, "pageUI" )
	
	end

end

-- 사운드 백분위를 데시벨로 컨버팅
function GameOption_ToSoundDecibel( fPercent )

	local nVolume = math.floor( 2000 * fPercent )
	if 1 > nVolume then
		nVolume = -10000
	else
		nVolume = nVolume - 2000
	end

	return nVolume
	
end

-- 데시벨을 백분위로 컨버팅
function GameOption_ToSoundPercent( nDecibel )

	local fPercent = 0
	if -10000 ~= nDecibel then
		fPercent = ( nDecibel + 2000 ) / 2000
	end

	return fPercent
end

-----------------------------------------------------------------------------------------------------------------
-- 이벤트

-- 탭 인덱스 변경
function Ev_GameOption_TabPage( nIndex )

	g_nGameOption_SelectPage = nIndex
	GameOption_UpdatePage()

end

-- 적용
function Ev_GameOption_Apply()

	if 0 == g_nGameOption_SelectPage then
	
		local nResolutionSelect = GetInt( WIDGET_ID_GAMEOPTION, "selectedIndex", "pageGraphic.comboResolution" ) + 1
		local tbResolution = g_tbGameOption_ResolutionList[ nResolutionSelect ]
		
		if nil ~= tbResolution then
			g_DxParamSet.dwWidth = tbResolution[ "nWidth" ]
			g_DxParamSet.dwHeight = tbResolution[ "nHeight" ]
			if 32 == tbResolution[ "nFormat" ] then
				g_DxParamSet.emFormat = 1
			else
				g_DxParamSet.emFormat = 0
			end
			
		end
		
		nResolutionSelect = GetInt( WIDGET_ID_GAMEOPTION, "selectedIndex", "pageGraphic.comboTextureRes" )
		if g_nGameOption_SelectTexResolution ~= nResolutionSelect then
		
			local str = GetGameInternalText( "OPTION_CHANGEMAP_APPLY", 0 )
			OpenMsgBox( MSG_BOX_TYPE_OK, str, Event_MsgBox_Close )
		
		end
		
		g_DxParamSet.dwDefault = GetInt( WIDGET_ID_GAMEOPTION, "selectedIndex", "pageGraphic.comboDefault" )
		g_DxParamSet.dwShadow = GetInt( WIDGET_ID_GAMEOPTION, "selectedIndex", "pageGraphic.comboShadow" )
		g_DxParamSet.dwDetail = GetInt( WIDGET_ID_GAMEOPTION, "selectedIndex", "pageGraphic.comboDetail" )
		g_DxParamSet.dwSight = GetInt( WIDGET_ID_GAMEOPTION, "selectedIndex", "pageGraphic.comboSight" )
		g_DxParamSet.dwPhysXCloth = GetInt( WIDGET_ID_GAMEOPTION, "selectedIndex", "pageGraphic.comboPhysical" )
		g_DxParamSet.dwTexFilter = GetInt( WIDGET_ID_GAMEOPTION, "selectedIndex", "pageGraphic.comboTextureFilter" )
		g_DxParamSet.dwTexQuality = GetInt( WIDGET_ID_GAMEOPTION, "selectedIndex", "pageGraphic.comboTextureRes" )
		
		g_DxParamSet.bBuff = GetBool( WIDGET_ID_GAMEOPTION, "selected", "pageGraphic.checkBuff" )
		g_DxParamSet.bRealReflect = GetBool( WIDGET_ID_GAMEOPTION, "selected", "pageGraphic.checkReflect" )
		g_DxParamSet.bRefract = GetBool( WIDGET_ID_GAMEOPTION, "selected", "pageGraphic.checkDistortion" )
		g_DxParamSet.bGlow = GetBool( WIDGET_ID_GAMEOPTION, "selected", "pageGraphic.checkGlow" )
		g_DxParamSet.bPost = GetBool( WIDGET_ID_GAMEOPTION, "selected", "pageGraphic.checkPostProc" )
		g_DxParamSet.bFrameLimit = GetBool( WIDGET_ID_GAMEOPTION, "selected", "pageGraphic.checkVSync" )
		g_DxParamSet.bTargetEffect = GetBool( WIDGET_ID_GAMEOPTION, "selected", "pageGraphic.checkTargetEff" )
		g_DxParamSet.bClickEffect = GetBool( WIDGET_ID_GAMEOPTION, "selected", "pageGraphic.checkMapPoint" )
		g_DxParamSet.bMineEffect = GetBool( WIDGET_ID_GAMEOPTION, "selected", "pageGraphic.checkItemEff" )
		g_DxParamSet.bUseNormalMap = GetBool( WIDGET_ID_GAMEOPTION, "selected", "pageGraphic.checkNormalmap" )
		
		g_DxParamSet:VideoOK()
	
	elseif 1 == g_nGameOption_SelectPage then
	
		SetMuteSfx( GetBool( WIDGET_ID_GAMEOPTION, "selected", "pageSound.checkEffect" ) )
		SetMuteEnv( GetBool( WIDGET_ID_GAMEOPTION, "selected", "pageSound.checkEnvironment" ) )
		SetMuteBGM( GetBool( WIDGET_ID_GAMEOPTION, "selected", "pageSound.checkBGM" ) )
		
		g_DxParamSet:AudioOK()
	
	elseif 2 == g_nGameOption_SelectPage then
	
		local tbGameOption = GetGameOption()
		tbGameOption[ "bDuel" ] = GetBool( WIDGET_ID_GAMEOPTION, "selected", "pageGame.checkDuel" )
		tbGameOption[ "bTrade" ] = GetBool( WIDGET_ID_GAMEOPTION, "selected", "pageGame.checkTrade" )
		tbGameOption[ "bParty" ] = GetBool( WIDGET_ID_GAMEOPTION, "selected", "pageGame.checkParty" )
		tbGameOption[ "bFriend" ] = GetBool( WIDGET_ID_GAMEOPTION, "selected", "pageGame.checkFriend" )
		tbGameOption[ "bAttack" ] = GetBool( WIDGET_ID_GAMEOPTION, "selected", "pageGame.checkAttack" )
		tbGameOption[ "bTip" ] = GetBool( WIDGET_ID_GAMEOPTION, "selected", "pageGame.checkTip" )
		--tbGameOption[ "bName" ] = GetBool( WIDGET_ID_GAMEOPTION, "selected", "pageGame.checkName" )
		tbGameOption[ "nCharSimple" ] = GetInt( WIDGET_ID_GAMEOPTION, "selectedIndex", "pageGame.comboCharSimple" )
		tbGameOption[ "nSkill" ] = GetInt( WIDGET_ID_GAMEOPTION, "selectedIndex", "pageGame.comboSkill" )
		tbGameOption[ "bCheckAutoRebirth" ] = GetBool( WIDGET_ID_GAMEOPTION, "selected", "pageGame.checkAutoResurrect" )
		tbGameOption[ "bRide" ] = GetBool( WIDGET_ID_GAMEOPTION, "selected", "pageGame.checkRide" )
		tbGameOption[ "bMobHP" ] = GetBool( WIDGET_ID_GAMEOPTION, "selected", "pageGame.checkMobHP" )
		tbGameOption[ "bMyInfo" ] = GetBool( WIDGET_ID_GAMEOPTION, "selected", "pageGame.checkMyInfo" )
		tbGameOption[ "bGroupChat" ] = GetBool( WIDGET_ID_GAMEOPTION, "selected", "pageGame.checkGroupChat" )
		
		SetGameOption( tbGameOption )
		
		-- 정보보기 공개 여부 설정
		g_Msg:NotifyReferCharPublic()
		
	elseif 3 == g_nGameOption_SelectPage then
	
		local tbGameOption = GetGameOption()
		tbGameOption[ "bMiniHP" ] = GetBool( WIDGET_ID_GAMEOPTION, "selected", "pageUI.checkMiniHP" )
		tbGameOption[ "bShowOtherDamage" ] = GetBool( WIDGET_ID_GAMEOPTION, "selected", "pageUI.checkShowOtherDamage" )
		tbGameOption[ "bChat" ] = GetBool( WIDGET_ID_GAMEOPTION, "selected", "pageUI.checkChat" )
		tbGameOption[ "fUIAlpha" ] = GetDouble( WIDGET_ID_GAMEOPTION, "value", "pageUI.sliderUIAlpha" )
		tbGameOption[ "nUIAntiAliasing" ] = GetInt( WIDGET_ID_GAMEOPTION, "selectedIndex", "pageUI.comboQuality" )
		tbGameOption[ "bDisplayLimit" ] = not GetBool( WIDGET_ID_GAMEOPTION, "selected", "pageUI.checkDisplayLimit" )
	
		-- UI 알파값 설정
		if true == g_bGameOption_IsChangeAlpha then
		
			SetUIAlpha( tbGameOption[ "fUIAlpha" ] )
			g_bGameOption_IsChangeAlpha = false
		
		end
		
		SetGameOption( tbGameOption )
	
	end

end

-- 확인
function Ev_GameOption_OK()

	Ev_GameOption_Apply()
	CloseWidget( WIDGET_ID_GAMEOPTION )
	
end

-- 기본설정 콤보박스 이벤트
function Ev_GameOption_ComboDefault( nIndex )

	if 1 == nIndex then
	
		SetInt( WIDGET_ID_GAMEOPTION, "selectedIndex", 0, "pageGraphic.comboShadow" )
		SetInt( WIDGET_ID_GAMEOPTION, "selectedIndex", 0, "pageGraphic.comboDetail" )
		SetInt( WIDGET_ID_GAMEOPTION, "selectedIndex", 2, "pageGraphic.comboSight" )
		SetInt( WIDGET_ID_GAMEOPTION, "selectedIndex", 0, "pageGraphic.comboPhysical" )
		SetInt( WIDGET_ID_GAMEOPTION, "selectedIndex", 0, "pageGraphic.comboTextureFilter" )
		SetInt( WIDGET_ID_GAMEOPTION, "selectedIndex", 0, "pageGraphic.comboTextureRes" )
	
		SetBool( WIDGET_ID_GAMEOPTION, "selected", false, "pageGraphic.checkBuff" )
		SetBool( WIDGET_ID_GAMEOPTION, "selected", false, "pageGraphic.checkReflect" )
		SetBool( WIDGET_ID_GAMEOPTION, "selected", false, "pageGraphic.checkDistortion" )
		SetBool( WIDGET_ID_GAMEOPTION, "selected", false, "pageGraphic.checkGlow" )
		SetBool( WIDGET_ID_GAMEOPTION, "selected", false, "pageGraphic.checkPostProc" )
		--SetBool( WIDGET_ID_GAMEOPTION, "selected", false, "pageGraphic.checkVSync" )
		SetBool( WIDGET_ID_GAMEOPTION, "selected", false, "pageGraphic.checkTargetEff" )
		SetBool( WIDGET_ID_GAMEOPTION, "selected", false, "pageGraphic.checkMapPoint" )
		SetBool( WIDGET_ID_GAMEOPTION, "selected", false, "pageGraphic.checkItemEff" )
		SetBool( WIDGET_ID_GAMEOPTION, "selected", false, "pageGraphic.checkNormalmap" )
	
	elseif 2 == nIndex then
	
		SetInt( WIDGET_ID_GAMEOPTION, "selectedIndex", 1, "pageGraphic.comboShadow" )
		SetInt( WIDGET_ID_GAMEOPTION, "selectedIndex", 1, "pageGraphic.comboDetail" )
		SetInt( WIDGET_ID_GAMEOPTION, "selectedIndex", 2, "pageGraphic.comboSight" )
		SetInt( WIDGET_ID_GAMEOPTION, "selectedIndex", 1, "pageGraphic.comboPhysical" )
		SetInt( WIDGET_ID_GAMEOPTION, "selectedIndex", 1, "pageGraphic.comboTextureFilter" )
		SetInt( WIDGET_ID_GAMEOPTION, "selectedIndex", 1, "pageGraphic.comboTextureRes" )
	
		SetBool( WIDGET_ID_GAMEOPTION, "selected", true, "pageGraphic.checkBuff" )
		SetBool( WIDGET_ID_GAMEOPTION, "selected", false, "pageGraphic.checkReflect" )
		SetBool( WIDGET_ID_GAMEOPTION, "selected", false, "pageGraphic.checkDistortion" )
		SetBool( WIDGET_ID_GAMEOPTION, "selected", false, "pageGraphic.checkGlow" )
		SetBool( WIDGET_ID_GAMEOPTION, "selected", false, "pageGraphic.checkPostProc" )
		--SetBool( WIDGET_ID_GAMEOPTION, "selected", false, "pageGraphic.checkVSync" )
		SetBool( WIDGET_ID_GAMEOPTION, "selected", true, "pageGraphic.checkTargetEff" )
		SetBool( WIDGET_ID_GAMEOPTION, "selected", true, "pageGraphic.checkMapPoint" )
		SetBool( WIDGET_ID_GAMEOPTION, "selected", false, "pageGraphic.checkItemEff" )
		SetBool( WIDGET_ID_GAMEOPTION, "selected", false, "pageGraphic.checkNormalmap" )
	
	elseif 3 == nIndex then
	
		SetInt( WIDGET_ID_GAMEOPTION, "selectedIndex", 2, "pageGraphic.comboShadow" )
		SetInt( WIDGET_ID_GAMEOPTION, "selectedIndex", 1, "pageGraphic.comboDetail" )
		SetInt( WIDGET_ID_GAMEOPTION, "selectedIndex", 3, "pageGraphic.comboSight" )
		SetInt( WIDGET_ID_GAMEOPTION, "selectedIndex", 2, "pageGraphic.comboPhysical" )
		SetInt( WIDGET_ID_GAMEOPTION, "selectedIndex", 3, "pageGraphic.comboTextureFilter" )
		SetInt( WIDGET_ID_GAMEOPTION, "selectedIndex", 2, "pageGraphic.comboTextureRes" )
	
		SetBool( WIDGET_ID_GAMEOPTION, "selected", true, "pageGraphic.checkBuff" )
		SetBool( WIDGET_ID_GAMEOPTION, "selected", false, "pageGraphic.checkReflect" )
		SetBool( WIDGET_ID_GAMEOPTION, "selected", true, "pageGraphic.checkDistortion" )
		SetBool( WIDGET_ID_GAMEOPTION, "selected", true, "pageGraphic.checkGlow" )
		SetBool( WIDGET_ID_GAMEOPTION, "selected", true, "pageGraphic.checkPostProc" )
		--SetBool( WIDGET_ID_GAMEOPTION, "selected", true, "pageGraphic.checkVSync" )
		SetBool( WIDGET_ID_GAMEOPTION, "selected", true, "pageGraphic.checkTargetEff" )
		SetBool( WIDGET_ID_GAMEOPTION, "selected", true, "pageGraphic.checkMapPoint" )
		SetBool( WIDGET_ID_GAMEOPTION, "selected", true, "pageGraphic.checkItemEff" )
		SetBool( WIDGET_ID_GAMEOPTION, "selected", true, "pageGraphic.checkNormalmap" )
	
	elseif 4 == nIndex then
	
		SetInt( WIDGET_ID_GAMEOPTION, "selectedIndex", 4, "pageGraphic.comboShadow" )
		SetInt( WIDGET_ID_GAMEOPTION, "selectedIndex", 2, "pageGraphic.comboDetail" )
		SetInt( WIDGET_ID_GAMEOPTION, "selectedIndex", 3, "pageGraphic.comboSight" )
		SetInt( WIDGET_ID_GAMEOPTION, "selectedIndex", 2, "pageGraphic.comboPhysical" )
		SetInt( WIDGET_ID_GAMEOPTION, "selectedIndex", 4, "pageGraphic.comboTextureFilter" )
		SetInt( WIDGET_ID_GAMEOPTION, "selectedIndex", 2, "pageGraphic.comboTextureRes" )
	
		SetBool( WIDGET_ID_GAMEOPTION, "selected", true, "pageGraphic.checkBuff" )
		SetBool( WIDGET_ID_GAMEOPTION, "selected", true, "pageGraphic.checkReflect" )
		SetBool( WIDGET_ID_GAMEOPTION, "selected", true, "pageGraphic.checkDistortion" )
		SetBool( WIDGET_ID_GAMEOPTION, "selected", true, "pageGraphic.checkGlow" )
		SetBool( WIDGET_ID_GAMEOPTION, "selected", true, "pageGraphic.checkPostProc" )
		--SetBool( WIDGET_ID_GAMEOPTION, "selected", true, "pageGraphic.checkVSync" )
		SetBool( WIDGET_ID_GAMEOPTION, "selected", true, "pageGraphic.checkTargetEff" )
		SetBool( WIDGET_ID_GAMEOPTION, "selected", true, "pageGraphic.checkMapPoint" )
		SetBool( WIDGET_ID_GAMEOPTION, "selected", true, "pageGraphic.checkItemEff" )
		SetBool( WIDGET_ID_GAMEOPTION, "selected", true, "pageGraphic.checkNormalmap" )
		
	end

end

-- 게임옵션 커스터마이징
function Ev_GameOption_OnCustom()
	SetInt( WIDGET_ID_GAMEOPTION, "selectedIndex", 0, "pageGraphic.comboDefault" )
end

-- 효과음 슬라이더
function Ev_GameOption_SliderEffect( fValue )

	SetSfxVolume( GameOption_ToSoundDecibel( fValue ) )

end

-- 환경음 슬라이더
function Ev_GameOption_SliderEnv( fValue )

	SetEnvVolume( GameOption_ToSoundDecibel( fValue ) )

end

-- 배경음 슬라이더
function Ev_GameOption_SliderBGM( fValue )

	SetBGMVolume( GameOption_ToSoundDecibel( fValue ) )

end

-- 게임스테이지 위치 초기화 여부 묻는 메시지박스 출력
function Ev_GameOption_DefaultPos()

	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, GetGameInternalText( "DEFAULT_POSITION" ), 
		Ev_GameOption_DefaultPos_OkClick, Event_MsgBox_Close )
	
end

-- 이름 보기 설정
function Ev_GameOption_NameConfig()

	ToggleUI( WIDGET_ID_NAMEDISPLAYCONFIG )

end

-- 메시지 박스에서 확인버튼 클릭시 게임스테이지 위치 초기화
function Ev_GameOption_DefaultPos_OkClick()

	Event_MsgBox_Close()
	
	ResetWidgetPosition()
	
end

-- 전체 UI 알파값
function Ev_GameOption_UIAlpha( value, bInit )

	local str = string.format( "%.2f", value )
	SetString( WIDGET_ID_GAMEOPTION, "text", str, "pageUI.labelUIAlpha" )	
	
	if true ~= bInit then
		g_bGameOption_IsChangeAlpha = true
	end

end