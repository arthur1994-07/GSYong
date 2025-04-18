
-- 펫 포만도 경고 시작할 수치
g_nQuickMenu_PetWarnningStartRatio = 10

-- 유동적인 퀵메뉴들중 각각에 대한 사용여부
g_Use_QuickMenu_Report		= false		-- 알림창 퀵 메뉴 사용을 결정한다.(true : 사용, false : 비사용)
g_Use_QuickMenu_Pet 		= false		-- 펫 퀵 메뉴 사용을 결정한다.(true : 사용, false : 비사용)
g_Use_QuickMenu_Vehicle		= false		-- 탈것 퀵 메뉴 사용을 결정한다.(true : 사용, false : 비사용)
g_Use_QuickMenu_Post		= false		-- 우편 퀵 메뉴 사용을 결정한다.(true : 사용, false : 비사용)
g_Use_QuickMenu_MiniParty	= false		-- 미니파티 퀵 메뉴 사용을 결정한다.(true : 사용, false : 비사용)
g_Use_QuickMenu_VehicleWith = false		-- 동승자 입장에서 내리기
g_Use_QuickMenu_PartyDistribButton = false -- 파티 아이템 분배 버튼
g_Use_QuickMenu_WeaponBoost = false		-- 무기 부스터
g_Use_QuickMenu_Macro = false			-- 매크로
g_Use_QuickMenu_Lotto = false 			-- 로또

g_IsMovedUI = false 						-- UI가 움직였는가?  

-- 드래그 관련 저장 값
g_QuickMenu_DragMouseX = 0
g_QuickMenu_DragMouseY = 0
g_QuickMenu_OrigPosX = 0
g_QuickMenu_OrigPosY = 0

-- 퀵 메뉴를 유동적으로 배치하기 위한 좌표저장
g_QuickMenu_Interval = 8

-- 퀵 메뉴중 펫 슬롯에 넣을 펫 이미지의 아이템 ID 저장용
g_QuickMenu_PetMID = 0
g_QuickMenu_PetSID = 0
-- 펫 공복도
PetHungryGauge = 0

-- 퀵 메뉴중 탈것 슬롯에 넣을 탈것 이미지의 아이템 ID 저장용
g_QuickMenu_vehicleMID = 0
g_QuickMenu_VehicleSID = 0
-- 탈것 연료
vehicleFuelGauge = 0

-- 큐박스 타입
g_QuickMenu_Qbox_Type = 0

-- 큐박스 잠금 상태
g_bQuickMenu_LockQBox = false

g_nQuickMenu_BgLeftOffset = 0
g_nQuickMenu_BgRightOffset = 0

-- 거래/귓말/파티/대련/친구등록/대련/그룹챗/정보보기
-- 퀵메뉴 컴포넌트들을 일단 다끄고, 나중에 필요할때 키도록한다;
function LoadQuickMenu()

	g_nQuickMenu_BgLeftOffset = GetInt( WIDGET_ID_QUICKMENU, "left", "QuickMenu.QuickDrag.scale9Grid" )
	g_nQuickMenu_BgRightOffset = GetInt( WIDGET_ID_QUICKMENU, "width", "QuickMenu.QuickDrag" ) - 
		GetInt( WIDGET_ID_QUICKMENU, "right", "QuickMenu.QuickDrag.scale9Grid" )
	
	-- 탈것
	SetBool( WIDGET_ID_QUICKMENU, "visible", false, "QuickMenu.vehicle" )
	SetBool( WIDGET_ID_QUICKMENU, "tooltipEnabled", false, "QuickMenu.vehicle.Buttonvehicle" )
	SetString( WIDGET_ID_QUICKMENU, "binding", SLOT_VEHICLE_IMAGE_ID, "QuickMenu.vehicle.Buttonvehicle" )
	
	-- 펫
	SetBool( WIDGET_ID_QUICKMENU, "visible", false, "QuickMenu.pet" )
	SetBool( WIDGET_ID_QUICKMENU, "tooltipEnabled", false, "QuickMenu.pet.ButtonPet" )
	SetString( WIDGET_ID_QUICKMENU, "binding", SLOT_PET_IMAGE_ID, "QuickMenu.pet.ButtonPet" )
	HitTestEnable( WIDGET_ID_QUICKMENU, false, "QuickMenu.pet.mcWarnning" )
	
	-- 무기 부스터
	SetBool( WIDGET_ID_QUICKMENU, "visible", false, "QuickMenu.WeaponBoost" )
	SetBool( WIDGET_ID_QUICKMENU, "tooltipEnabled", true, "QuickMenu.WeaponBoost.icon" )
	SetInt( WIDGET_ID_QUICKMENU, "minimum", 0, "QuickMenu.WeaponBoost.WeaponBoostStage" )
	SetInt( WIDGET_ID_QUICKMENU, "maximum", 100, "QuickMenu.WeaponBoost.WeaponBoostStage" )
	
	-- 알림 메시지
	local str = GetGameInternalText("NOTIFY_BUTTON_MESSAGE", 0)
	SetString( WIDGET_ID_QUICKMENU, "TooltipText", str, "QuickMenu.ButtonRaport" )
	SetBool( WIDGET_ID_QUICKMENU, "visible", false, "QuickMenu.ButtonRaport" )
	
	-- 우편
	str = GetGameInternalText("POSTBOX_TOOLTIP_NOTIFYBUTTON", 0)
	SetString( WIDGET_ID_QUICKMENU, "TooltipText", str, "QuickMenu.ButtonPost" )
	SetBool( WIDGET_ID_QUICKMENU, "visible", false, "QuickMenu.ButtonPost" )
	
	-- 미니파티
	SetBool( WIDGET_ID_QUICKMENU, "visible", false, "QuickMenu.ButtonMiniParty" )
	
	-- 동승
	SetBool( WIDGET_ID_QUICKMENU, "visible", false, "QuickMenu.ButtonRideWith" )
	
	-- 파티 분배
	SetBool( WIDGET_ID_QUICKMENU, "visible", false, "QuickMenu.ButtonPartyDistribution" )
	
	-- 큐박스
	str = GetGameInternalText("QBOX_BUTTON_MESSAGE", 0)
	SetString( WIDGET_ID_QUICKMENU, "TooltipText", str, "QuickMenu.qbox.ButtonQbox" )
	
	-- 드래그 버튼
	SetDouble( WIDGET_ID_QUICKMENU, "alpha", 0.0, "QuickMenu.QuickDrag" )
	
	QuickMenu_MacroDefaultSet()
	QuickMenu_ViewSet()
	
	-- 각 버튼에 마우스 클릭시 발생할 이벤트
	SetEvent(WIDGET_ID_QUICKMENU, EVENT_MOUSE_CLICK, "Click_QuickMenu_Pet", "buttonIdx", "QuickMenu.pet.ButtonPet" )
	SetEvent(WIDGET_ID_QUICKMENU, EVENT_MOUSE_CLICK, "Click_QuickMenu_vehicle", "", "QuickMenu.vehicle.Buttonvehicle" )
	SetEvent(WIDGET_ID_QUICKMENU, EVENT_BUTTON_CLICK, "Click_QuickMenu_Report", "", "QuickMenu.ButtonRaport" )
	SetEvent(WIDGET_ID_QUICKMENU, EVENT_BUTTON_CLICK, "Click_QuickMenu_Post", "", "QuickMenu.ButtonPost" )
	SetEvent(WIDGET_ID_QUICKMENU, EVENT_BUTTON_CLICK, "Click_QuickMenu_Run", "", "QuickMenu.ButtonRun" )
	SetEvent(WIDGET_ID_QUICKMENU, EVENT_BUTTON_CLICK, "Click_QuickMenu_QBox", "", "QuickMenu.qbox.ButtonQbox" )
	SetEvent(WIDGET_ID_QUICKMENU, EVENT_BUTTON_CLICK, "Click_QuickMenu_RideWith", "", "QuickMenu.ButtonRideWith" )
	SetEvent(WIDGET_ID_QUICKMENU, EVENT_BUTTON_CLICK, "Click_QuickMenu_PartyDistribution", "", "QuickMenu.ButtonPartyDistribution" )
	SetEvent(WIDGET_ID_QUICKMENU, EVENT_BUTTON_CLICK, "Click_QuickMenu_MacroOption", "", "QuickMenu.MacroSlot" )
	SetEvent( WIDGET_ID_QUICKMENU, EVENT_BUTTON_CLICK, "Click_QuickMenu_RanMobile", "", "QuickMenu.btnRanMobile" )
	SetEvent( WIDGET_ID_QUICKMENU, EVENT_BUTTON_CLICK, "Click_QuickMenu_Lotto", "", "QuickMenu.Lotto" )
	
	
	-- 마우스 오버시 발생할 이벤트
	SetEvent(WIDGET_ID_QUICKMENU, EVENT_MOUSE_OVER, "MouseOver_QuickMenu_Report", "", "QuickMenu.ButtonRaport" )
	SetEvent(WIDGET_ID_QUICKMENU, EVENT_MOUSE_OVER, "MouseOver_QuickMenu_QuickDrag", "", "" )
	-- 롤아웃 시 창닫기
	SetEvent( WIDGET_ID_QUICKMENU, EVENT_MOUSE_ROLLOUT, "MouseOut_QuickMenu_QuickDrag", "", "" )
	
	Ev_QuickMenu_HotKeyText()
	
	SetEvent( WIDGET_ID_QUICKMENU, EVENT_MOUSE_DOWN, "Ev_QuickMenu_DragStart", "stageX;stageY", "QuickMenu.QuickDrag" )
	
	if true == g_UserFeatures:IsUsingRanMobile() then
	
		local nSP = GetServiceProvider()
		-- KR 제외 use features 로만 체크
		if 	nSP ~= SP_KOREA_TEST and nSP ~= SP_KOREA then
		
			SetBool( WIDGET_ID_QUICKMENU, "visible", false, "QuickMenu.ButtonRun" )
			SetBool( WIDGET_ID_QUICKMENU, "visible", true, "QuickMenu.btnRanMobile" )
		
		-- KR은 GM 계정만 열어준다.
		else
		
			if USER_USER_GM <= g_MyChar:UserLevel() then
			
				SetBool( WIDGET_ID_QUICKMENU, "visible", false, "QuickMenu.ButtonRun" )
				SetBool( WIDGET_ID_QUICKMENU, "visible", true, "QuickMenu.btnRanMobile" )
			
			else
			
				SetBool( WIDGET_ID_QUICKMENU, "visible", true, "QuickMenu.ButtonRun" )
				SetBool( WIDGET_ID_QUICKMENU, "visible", false, "QuickMenu.btnRanMobile" )
			
			end
		
		end
	
	else
	
		SetBool( WIDGET_ID_QUICKMENU, "visible", true, "QuickMenu.ButtonRun" )
		SetBool( WIDGET_ID_QUICKMENU, "visible", false, "QuickMenu.btnRanMobile" )
	
	end
	
	if true == g_UserFeatures:IsUsingLottoSystem() then
		SetBool( WIDGET_ID_QUICKMENU, "visible", true, "QuickMenu.Lotto" )
		g_Use_QuickMenu_Lotto = true
	else
		SetBool( WIDGET_ID_QUICKMENU, "visible", false, "QuickMenu.Lotto" )
		g_Use_QuickMenu_Lotto = false	
	end
		
end

function OpenQuickMenu()

	if g_IsMovedUI == false then

		local nInitInterval = GetInt(WIDGET_ID_QUICKMENU, "width", "QuickMenu.QuickDrag") 
		local nInitPosX = GetInt(WIDGET_ID_MENUBUTTON, "x", "") - nInitInterval
		SetInt( WIDGET_ID_QUICKMENU, "x", nInitPosX, "")
	
	end
		
	local tbWeaponBoostInfo = g_MyChar:GetWeaponBoostInitData()
	if tbWeaponBoostInfo ~= nil and table.getn(tbWeaponBoostInfo) ~= 0 then
		QuickMenu_SetWeaponBoostInfo(tbWeaponBoostInfo[1], tbWeaponBoostInfo[2], tbWeaponBoostInfo[3])
	end
	
	SetInt( WIDGET_ID_QUICKMENU, "value", 0, "QuickMenu.qbox.QBoxStage" )
	
end

function ResetQuickMenuPostion( bResetUI )

	if bResetUI == true then 
		g_IsMovedUI = false
	end
	
	if g_IsMovedUI == false then
	
		local nInitInterval = GetInt(WIDGET_ID_QUICKMENU, "width", "QuickMenu.QuickDrag")
		local nInitPosX = GetInt(WIDGET_ID_MENUBUTTON, "x", "") - nInitInterval
		SetInt( WIDGET_ID_QUICKMENU, "x", nInitPosX, "")
	
	end
		
end

function QuickMenu_IsMovedUI()

	return g_IsMovedUI;
	
end

function QuickMenu_SetMovedUI( bMovedUI )

	g_IsMovedUI = bMovedUI;

end

function QuickMenu_Close( )
	CloseWidget(WIDGET_ID_QUICKMENU)
end

function QuickMenu_Reset()

	g_Use_QuickMenu_Report = false
	g_Use_QuickMenu_Pet = false
	g_Use_QuickMenu_Vehicle = false
	g_Use_QuickMenu_Post = false
	g_Use_QuickMenu_MiniParty = false
	g_Use_QuickMenu_VehicleWith = false
	g_Use_QuickMenu_PartyDistribButton = false
	g_Use_QuickMenu_WeaponBoost = false
	g_Use_QuickMenu_Macro = false

end

-- 달리기 단축키 설명 내용 변경
function Ev_QuickMenu_HotKeyText()
	-- 달리기 단축키 값 입력(키타입에 따라 단축키가 변하기 때문)
	local RunHotKey = GetGameWordText("KEYSETTING_MENUSHOTCUT", 11) .. "(" .. tostring(g_InputMan:GetShortcutsToString( g_tbHotKeyCfg_IndexType[ 29 ] )) .. ")"
	SetString( WIDGET_ID_QUICKMENU, "TooltipText", RunHotKey, "QuickMenu.ButtonRun" )
end

-- 우편 버튼 출력 여부를 결정하는 함수(받은 우편개수가 1개 이상일 경우 출력)
function QuickMenu_ViewSet_Post()
	local GetNewPostNum = g_Post:GetNewPost()	-- 우편함에 저장된 읽지않은 우편 리스트를 읽어들여 테이블 형태로 저장
	if 0 < GetNewPostNum then
		g_Use_QuickMenu_Post = true
	elseif 0 == GetNewPostNum then
		g_Use_QuickMenu_Post = false
	end
end

-- 알림 버튼 출력 여부를 결정하는 함수(받은 알림개수가 1개 이상일 경우 출력)
function QuickMenu_ViewSet_Report()
	local Invitation_Data_Count = table.getn(g_Invitation)
	if 0 < Invitation_Data_Count then
		g_Use_QuickMenu_Report = true
	elseif 0 == Invitation_Data_Count then
		g_Use_QuickMenu_Report = false
	end
end
-- 펫 소환상태에 따라 버튼 출력 여부를 결정하는 함수
function QuickMenu_ViewSet_Pet()
	if true == g_Pet:IsVALID() then
		g_Use_QuickMenu_Pet = true
		local PetCardId = g_Pet:GetPetCardID()
		SetSlot_Item( SLOT_PET_IMAGE_ID, PetCardId["wMID"], PetCardId["wSID"], 1 )	-- 펫 카드 이미지를 퀵 펫 슬롯에 출력
		QuickMenu_Pet_Hungry_Gauge()
	elseif false == g_Pet:IsVALID() then
		g_Use_QuickMenu_Pet = false
	end
end
-- 펫 포만도 계산 함수
function QuickMenu_Pet_Hungry()
	PetHungryGauge = g_Pet:GetSatiation()
	PetHungryGauge = PetHungryGauge / 10.0
end
-- 퀵창에 펫 공복도 게이지를 조절하기 위한 함수
function QuickMenu_Pet_Hungry_Gauge()

	QuickMenu_Pet_Hungry()
	SetInt( WIDGET_ID_QUICKMENU, "minimum", 0, "QuickMenu.pet.PetHungryStage" )
	SetInt( WIDGET_ID_QUICKMENU, "maximum", 100, "QuickMenu.pet.PetHungryStage" )
	SetDouble( WIDGET_ID_QUICKMENU, "value", PetHungryGauge, "QuickMenu.pet.PetHungryStage" )
	
	-- 포만도 33% 밑으로 떨어지면 경고 표시
	local bWarnning = false
	if g_nQuickMenu_PetWarnningStartRatio > PetHungryGauge then
		bWarnning = true
	end
	SetBool( WIDGET_ID_QUICKMENU, "visible", bWarnning, "QuickMenu.pet.mcWarnning" )
	
	local str = GetGameInternalText("PET_HUNGRY", 0)
	str = string.format( str, PetHungryGauge )
	str = str .. " [" .. GetGameWordText("KEYSETTING_MENUSHOTCUT", 13) .. " : L]"
	SetString( WIDGET_ID_QUICKMENU, "TooltipText", str, "QuickMenu.pet.ButtonPet" )
	
end

-- 탈것탑승 상태에 따라 버튼 출력 여부를 결정하는 함수
function QuickMenu_ViewSet_Vehicle( bEquip, wMID, wSID, nFuel )
	if true == bEquip then
		g_Use_QuickMenu_Vehicle = true
		g_QuickMenu_vehicleMID = wMID
		g_QuickMenu_VehicleSID = wSID
		QuickMenu_vehicle_Fuel_Gauge()
	elseif false == bEquip then
		g_Use_QuickMenu_Vehicle = false
	end
	
	QuickMenu_Vehicle_View()
end
-- 탈것 아이콘 출력
function QuickMenu_Vehicle_View()
	SetBool( WIDGET_ID_QUICKMENU, "visible", g_Use_QuickMenu_Vehicle, "QuickMenu.vehicle.Buttonvehicle")
	SetBool( WIDGET_ID_QUICKMENU, "visible", g_Use_QuickMenu_Vehicle, "QuickMenu.vehicle.vehicleFuelStage")
	
	SetSlot_Item( SLOT_VEHICLE_IMAGE_ID, g_QuickMenu_vehicleMID, g_QuickMenu_VehicleSID, 1 )	-- 펫 카드 이미지를 퀵 펫 슬롯에 출력
end

-- 탈것 연료 계산 함수
function QuickMenu_vehicle_Fuel()
	vehicleFuelGauge = g_MyChar:VehicleFull()
	vehicleFuelGauge = vehicleFuelGauge / 10.0
end
-- 퀵창에 탈것 연료 게이지를 조절하기 위한 함수
function QuickMenu_vehicle_Fuel_Gauge()

	QuickMenu_vehicle_Fuel()
	SetInt( WIDGET_ID_QUICKMENU, "minimum", 0, "QuickMenu.vehicle.vehicleFuelStage" )
	SetInt( WIDGET_ID_QUICKMENU, "maximum", 100, "QuickMenu.vehicle.vehicleFuelStage" )
	SetDouble( WIDGET_ID_QUICKMENU, "value", vehicleFuelGauge, "QuickMenu.vehicle.vehicleFuelStage" )
	
	local str = GetGameWordText("VEHICLE_TEXT_STATIC", 2) .. " : " .. tostring( math.floor( vehicleFuelGauge ) ) .. "%"
	str = str .. "[" .. GetGameWordText("KEYSETTING_MENUSHOTCUT", 17) .. " : N]"
	
	SetString( WIDGET_ID_QUICKMENU, "TooltipText", str, "QuickMenu.vehicle.Buttonvehicle" )
	
end

function QuickMenu_ViewSet()	
	
	QuickMenu_ViewSet_Post()	-- 우편 버튼 출력 여부를 결정하는 함수
	QuickMenu_ViewSet_Report()	-- 알림 버튼 출력 여부를 결정하는 함수
	QuickMenu_ViewSet_Pet()		-- 펫 버튼 출력 여부를 결정하는 함수	
	
	-- priority은 사실상 의미는 없지만 가독성상 넣어둠;
	local tbVisibleSort = 
	{
		{
			priority = 0, instanceName = "QuickMenu.Lotto",
			visible = g_Use_QuickMenu_Lotto, width = 35
		},
	
		{	
			priority = 1, instanceName = "QuickMenu.pet", 
			visible = g_Use_QuickMenu_Pet, width = 43
		},
		{
			priority = 2, instanceName = "QuickMenu.vehicle", 
			visible = g_Use_QuickMenu_Vehicle, width = 43
		},
		{
			priority = 3, instanceName = "QuickMenu.ButtonRaport", 
			visible = g_Use_QuickMenu_Report, width = 36
		},
		{
			priority = 4, instanceName = "QuickMenu.ButtonPost", 
			visible = g_Use_QuickMenu_Post,	width = 36
		},
		{
			priority = 5, instanceName = "QuickMenu.ButtonMiniParty", 
			visible = g_Use_QuickMenu_MiniParty,	width = 36
		},
		{
			priority = 6, instanceName = "QuickMenu.ButtonRideWith", 
			visible = g_Use_QuickMenu_VehicleWith,width = 36
		},
		{
			priority = 7, instanceName = "QuickMenu.ButtonPartyDistribution", 
			visible = g_Use_QuickMenu_PartyDistribButton, width = 36
		},
		{
			priority = 8, instanceName = "QuickMenu.WeaponBoost",
			visible = g_Use_QuickMenu_WeaponBoost, width = 43
		},
		{
			priority = 9, instanceName = "QuickMenu.MacroSlot",
			visible = g_Use_QuickMenu_Macro, width = 35
		},
		
		--[[{
			priority = 10, instanceName = "QuickMenu.ButtonRun",
			visible = true, width = 36
		},
		
		{
			priority = 11, instanceName = "QuickMenu.qbox",
			visible = true, width = 43
		},]]
		

	}
	
	if true == g_UserFeatures:IsUsingRanMobile() then
	
		local nSP = GetServiceProvider()
		-- KR 제외 use features 로만 체크
		if 	nSP ~= SP_KOREA_TEST and nSP ~= SP_KOREA then
		
			local tb = 
			{
				priority = 10, instanceName = "QuickMenu.btnRanMobile",
				visible = true, width = 36
			}
			table.insert( tbVisibleSort, tb )
		
		-- KR은 GM 계정만 열어준다.
		else
		
			if USER_USER_GM <= g_MyChar:UserLevel() then
			
				local tb = 
				{
					priority = 10, instanceName = "QuickMenu.btnRanMobile",
					visible = true, width = 36
				}
				table.insert( tbVisibleSort, tb )
			
			else
			
				local tb = 
				{
					priority = 10, instanceName = "QuickMenu.ButtonRun",
					visible = true, width = 36
				}
				table.insert( tbVisibleSort, tb )
			
			end
		
		end
	
	else
	
		local tb = 
		{
			priority = 10, instanceName = "QuickMenu.ButtonRun",
			visible = true, width = 36
		}
		table.insert( tbVisibleSort, tb )
	
	end
	
	local tb = 
	{
		priority = 11, instanceName = "QuickMenu.qbox",
		visible = true, width = 43
	}
	table.insert( tbVisibleSort, tb )
	
	local nBeforeWidth = GetInt( WIDGET_ID_QUICKMENU, "width", "QuickMenu.QuickDrag" )
	local nBeforeX = GetInt( WIDGET_ID_QUICKMENU, "x", "" )
	
	local nAccumPos = g_nQuickMenu_BgLeftOffset
	local nScaleWidth = 0
	local nVisibleCount = 0
	
	for i in pairs( tbVisibleSort ) do
	
		local tb = tbVisibleSort[ i ]
		if tb.visible == true then	---> 보이는것만 정렬이 되어야한다;
		
			SetInt( WIDGET_ID_QUICKMENU, "x", nAccumPos, tb.instanceName )
		
			nScaleWidth = nAccumPos + tb.width
			nAccumPos = nScaleWidth + g_QuickMenu_Interval
			nVisibleCount = nVisibleCount + 1
			
		end
		
		SetBool( WIDGET_ID_QUICKMENU, "visible", tb.visible, tb.instanceName )
		
		-- 우편함 버튼이면 새로 받은 우편 개수 출력
		if tb.instanceName == "QuickMenu.ButtonPost" then
		
			SetString( WIDGET_ID_QUICKMENU, "text", g_Post:GetNewPost(), "QuickMenu.ButtonPost.PostNumText" )
		
		end
		
	end
	
	nScaleWidth = nScaleWidth + g_nQuickMenu_BgRightOffset
	SetInt( WIDGET_ID_QUICKMENU, "width", nScaleWidth, "QuickMenu.QuickDrag" )
	

	-- 오른쪽 기준으로 사이즈가 늘어나면 왼쪽으로 커져야한다;
	if nBeforeWidth ~= nScaleWidth then
	
		local nNewPosX = nBeforeX + (nBeforeWidth - nScaleWidth)
		SetInt( WIDGET_ID_QUICKMENU, "x", nNewPosX, "" )
		
	end	

end

-- 알림창을 On/Off 한다.
function Click_QuickMenu_Report()
	ToggleUI(WIDGET_ID_REPORTBOX)		-- (WIDGET_ID_REPORTBOX) UI 를 On/Off 한다.	
end

-- 펫 UI를 오픈
function Click_QuickMenu_Pet( buttonIdx ) 

	if 0 ~= buttonIdx then
		return
	end
	
	ToggleUI( WIDGET_ID_PET )
	
end

-- 탈것 UI를 오픈
function Click_QuickMenu_vehicle()
	ToggleUI(WIDGET_ID_VEHICLE)
end
-- 우편창을 On/Off 한다.
function Click_QuickMenu_Post()
	if false == IsOpenWidget( WIDGET_ID_POSTBOX ) then
		g_Msg:ReqUseInvenPostBoxCardPost()
		local tbInvenItem = g_MyChar:GetItemPosInInvenByTable( ITEMTYPE_POSTBOX_CARD )
		if false == tbInvenItem[1] then
			BasicChat_PrintSystemMsg(CHAT_SYSTEM,GetGameInternalText("EMINVEN_POSTBOX_OPEN_CARD_FB_NOITEM", 0),TEXT_COLOR_RED)
		end
	else
		CloseWidget( WIDGET_ID_POSTBOX )
	end
end

-- 걷기/달리기를 토글시킨다.	
function Click_QuickMenu_Run()
	g_Msg:ReqToggleRun()				-- 걷기 달리기 토글
end

-- Q-Box 습득여부를 On/Off 한다.
function Click_QuickMenu_QBox()
	local bSelected = GetBool( WIDGET_ID_QUICKMENU, "selected", "QuickMenu.qbox.ButtonQbox" )
	-- 경과시간에 따라 보내면 UI와의 오차가 생길 수 있음;
	if g_MyChar:ReqQBoxEnableState(not bSelected) == true then
		QuickMenu_QBox_TypeChange()
	else
		SetBool( WIDGET_ID_QUICKMENU, "selected", not bSelected, "QuickMenu.qbox.ButtonQbox" )	
	end
end

-- 로또창을 On/Off 한다.
function Click_QuickMenu_Lotto()
	ToggleUI(WIDGET_ID_LOTTO)
end

-- 동승 해제 버튼
function Click_QuickMenu_RideWith()
	g_Client:Vehicle_GetoffwithVehicle()
	SetBool( WIDGET_ID_QUICKMENU, "visible", false, "QuickMenu.ButtonRideWith")
	g_Use_QuickMenu_VehicleWith = false
	QuickMenu_ViewSet()
end

-- 파티 분배 버튼
function Click_QuickMenu_PartyDistribution()
	--ToggleUI(WIDGET_ID_PARTYDISTRIBUTION_DICE)
	ToggleDistributionUI()
	DelSpeechBubble(WIDGET_ID_QUICKMENU, "QuickMenu.ButtonPartyDistribution")
	SetBool(WIDGET_ID_QUICKMENU, "visible", true, "QuickMenu.ButtonPartyDistribution.PartyDistributionIcon" )
end

-- 매크로 버튼
function Click_QuickMenu_MacroOption()
	ToggleUI( WIDGET_ID_MACROOPTION )
end

-- 란모바일
function Click_QuickMenu_RanMobile()

	ToggleUI( WIDGET_ID_RANMOBILE )

end

--------------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 큐박스 잠금 상태 설정
function QuickMenu_SetQBoxLock( bEnable, bPartyMasterMsg )

	g_bQuickMenu_LockQBox = not bEnable
	SetBool( WIDGET_ID_QUICKMENU, "selected", g_bQuickMenu_LockQBox, "QuickMenu.qbox.ButtonQbox" )
	
	QuickMenu_QBox_TypeChange( )
end

-- 큐박스 잠금 상태 반환
function QuickMenu_GetQBoxLock()

	return (not g_bQuickMenu_LockQBox)
	
end

-- 큐박스 토글
function QuickMenu_ToggleQBox()
	SetBool( WIDGET_ID_QUICKMENU, "selected", not g_bQuickMenu_LockQBox, "QuickMenu.qbox.ButtonQbox" )
	QuickMenu_QBox_TypeChange()
	g_MyChar:ReqQBoxEnableState(not g_bQuickMenu_LockQBox)
end

-- 큐박스 습득
function QuickMenu_QBox_Take( nType, fTime )
	-- Exp Get, Boom, Oh.No, Heal 의 경우 큐박스 아이콘을 변경하지 않음
	if nType == 5 or nType == 7 or nType == 8 or nType == 12 then
		return
	end
	g_QuickMenu_Qbox_Type = nType
	QuickMenu_QBox_TypeChange()
	if fTime == 0 then
		return
	end
	
	-- SetTween 에 사용될 최대치 및 시작값 설정
	SetInt( WIDGET_ID_QUICKMENU, "maximum", fTime, "QuickMenu.qbox.QBoxStage" ) -- 최대치
	SetInt( WIDGET_ID_QUICKMENU, "value", fTime, "QuickMenu.qbox.QBoxStage" ) 	-- 시작값

	local tbQuickSet = 
	{
		time = fTime * 1000,
		delay = 0,
		ease = EASE_NONE,
		loop = false,
		onComplete = ""
	}
	SetTween( WIDGET_ID_QUICKMENU, tbQuickSet, "value", "0", "QuickMenu.qbox.QBoxStage" )	-- value 에서 0 으로 이동
	
end

-- 큐박스 시간 종료
function QuickMenu_QBox_Reset()

	g_QuickMenu_Qbox_Type = 0
	CallFunction( WIDGET_ID_QUICKMENU, "gotoAndStop", "default", "QuickMenu.qbox.ButtonQbox.QboxIcon" )
	
	local tbQuickSet = 
	{
		time = 0,
		delay = 0,
		ease = EASE_NONE,
		loop = false,
		onComplete = ""
	}
	SetTween( WIDGET_ID_QUICKMENU, tbQuickSet, "value", "0", "QuickMenu.qbox.QBoxStage" )
	SetInt( WIDGET_ID_QUICKMENU, "value", 0, "QuickMenu.qbox.QBoxStage" )
	
end

-- 큐박스 타입변환
function QuickMenu_QBox_TypeChange()

	g_bQuickMenu_LockQBox = GetBool( WIDGET_ID_QUICKMENU, "selected", "QuickMenu.qbox.ButtonQbox" )
	local strImgParam = ""
	
	if Q_BOX_SPEED_UP == g_QuickMenu_Qbox_Type then			-- 스피드 업
		strImgParam = "SpeedUp"
	elseif Q_BOX_CRAZY_TIME == g_QuickMenu_Qbox_Type then	-- 크레이지 타임
		strImgParam = "Crazy"
	elseif Q_BOX_POWER_UP == g_QuickMenu_Qbox_Type then		-- 파워 업
		strImgParam = "PowerUp"
	elseif Q_BOX_EXP_TIME == g_QuickMenu_Qbox_Type then		-- EXP 타임
		strImgParam = "ExpTime"
	elseif Q_BOX_LUCKY == g_QuickMenu_Qbox_Type then		-- 럭키
		strImgParam = "Lucky"
	elseif Q_BOX_SPEED_UP_MAX == g_QuickMenu_Qbox_Type then	-- 스피드 업 맥스
		strImgParam = "SpeedUpMax"
	elseif Q_BOX_MADNESS_TIME == g_QuickMenu_Qbox_Type then	-- 매드니스 타임
		strImgParam = "Madness"
	elseif Q_BOX_POWER_UP_MAX == g_QuickMenu_Qbox_Type then	-- 파워 업 맥스
		strImgParam = "PowerUpMax"
	else
		strImgParam = "default"
	end

	-- 잠금 해제 상태
	CallFunction( WIDGET_ID_QUICKMENU, "gotoAndStop", strImgParam, "QuickMenu.qbox.ButtonQbox.QboxIcon" )

end

-- 달리기 설정
function QuickMenu_SetRun( bRun )
	--[[
	if bRun == true then
		DelSpeechBubble(WIDGET_ID_QUICKMENU, "QuickMenu.ButtonRun")
	else
		SetSpeechBubble(WIDGET_ID_QUICKMENU, "QuickMenu.ButtonRun", "Run run")
	end
	]]--
	SetBool( WIDGET_ID_QUICKMENU, "selected", not bRun, "QuickMenu.ButtonRun" )

end

-- 파티 결성 및 파티 해재( false == 해제, true == 결성 )
function CF_QuickMenu_Party( bParty )
	--[[if bParty == false then
		g_Use_QuickMenu_MiniParty = false
	elseif bParty == true then
		g_Use_QuickMenu_MiniParty = true
	end
	QuickMenu_ViewSet()]]
end

-- 탈것에 탑승시 클콜
function CF_QuickMenu_RideVehicle( bHost )
	if bHost == false then
		SetBool( WIDGET_ID_QUICKMENU, "visible", true, "QuickMenu.ButtonRideWith")
		g_Use_QuickMenu_VehicleWith = true
	end
	QuickMenu_ViewSet()
end
-- 탈것에 하차시 클콜
function CF_QuickMenu_GetoffVehicle( bHost )
	SetBool( WIDGET_ID_QUICKMENU, "visible", false, "QuickMenu.ButtonRideWith")
	g_Use_QuickMenu_VehicleWith = false
	QuickMenu_ViewSet()
end
-- 동승 요청결과 클콜
function CF_QuickMenu_VehicleReqTargetAnswer( bAnswer )
	if bAnswer == true then
		SetBool( WIDGET_ID_QUICKMENU, "visible", true, "QuickMenu.ButtonRideWith")
		g_Use_QuickMenu_VehicleWith = true
	end
	QuickMenu_ViewSet()
end

function CF_QuickMenu_UpdateRideWithState(bVisible)
	SetBool( WIDGET_ID_QUICKMENU, "visible", bVisible, "QuickMenu.ButtonRideWith")
	g_Use_QuickMenu_VehicleWith = bVisible
	QuickMenu_ViewSet()
end

---------------------- 마우스 오버 이벤트 -------------------
function MouseOver_QuickMenu_Report() 	-- 알림창	

	if false == IsOpenWidget( WIDGET_ID_REPORTBOX ) then
		SetBool(WIDGET_ID_QUICKMENU, "selected", "true", "QuickMenu.ButtonRaport")
	end
	
end

-- 퀵 드래그
function MouseOver_QuickMenu_QuickDrag()

	SetInt( WIDGET_ID_QUICKMENU, "alpha", 1, "QuickMenu.QuickDrag" )
	--SetInt( WIDGET_ID_QUICKMENU, "alpha", 1, "QuickMenu.mcDragLeft" )
	--SetInt( WIDGET_ID_QUICKMENU, "alpha", 1, "QuickMenu.mcDragRight" )
	
end

---------------------- 마우스 롤 아웃 이벤트 -------------------
-- 퀵 드래그
function MouseOut_QuickMenu_QuickDrag()

	SetInt( WIDGET_ID_QUICKMENU, "alpha", 0, "QuickMenu.QuickDrag" )
	--SetInt( WIDGET_ID_QUICKMENU, "alpha", 0, "QuickMenu.mcDragLeft" )
	--SetInt( WIDGET_ID_QUICKMENU, "alpha", 0, "QuickMenu.mcDragRight" )
	
end
------------------------ 드래그 이벤트 -------------------------------
-- 드래그 시작 이벤트
function Ev_QuickMenu_DragStart( nStageX, nStageY )

	-- 언락일 경우만 드래그 허용
	InsertStageMouseMove( Ev_QuickMenu_Dragging )
	InsertStageMouseUp( Ev_QuickMenu_DragEnd )
	
	g_QuickMenu_DragMouseX = nStageX
	g_QuickMenu_DragMouseY = nStageY
	
	g_QuickMenu_OrigPosX = GetInt( WIDGET_ID_QUICKMENU, "x", "" )
	g_QuickMenu_OrigPosY = GetInt( WIDGET_ID_QUICKMENU, "y", "" )

end

-- 드래그 이동
function Ev_QuickMenu_Dragging( nStageX, nStageY )

	local nOffsetX = nStageX - g_QuickMenu_DragMouseX
	local nOffsetY = nStageY - g_QuickMenu_DragMouseY
	
	local nNewX = nOffsetX + g_QuickMenu_OrigPosX
	local nNewY = nOffsetY + g_QuickMenu_OrigPosY
	QuickMenu_UIPosition( nNewX, nNewY )
	
	g_IsMovedUI = true
	
end

function QuickMenu_UIPosition( nNewX, nNewY )

	local nBgX = GetInt( WIDGET_ID_QUICKMENU, "x", "QuickMenu.QuickDrag" )
	local nBgY = GetInt( WIDGET_ID_QUICKMENU, "y", "QuickMenu.QuickDrag" )
	local nBgW = GetInt( WIDGET_ID_QUICKMENU, "width", "QuickMenu.QuickDrag" )
	local nBgH = GetInt( WIDGET_ID_QUICKMENU, "height", "QuickMenu.QuickDrag" )
	
	local nScreenW = GetScreenWidth()
	local nScreenH = GetScreenHeight()
	
	if nNewX + nBgX < 0 then
		nNewX = nBgX * -1
	end
	
	if nNewY + nBgY < 0 then
		nNewY = nBgY * -1
	end
	
	if nNewX + nBgX + nBgW > nScreenW then
		nNewX = nScreenW - nBgW - nBgX
	end
	
	if nNewY + nBgY + nBgH > nScreenH then
		nNewY = nScreenH - nBgH - nBgY
	end
	
	SetInt( WIDGET_ID_QUICKMENU, "x", nNewX, "" )
	SetInt( WIDGET_ID_QUICKMENU, "y", nNewY, "" )

end

-- 드래그 끝
function Ev_QuickMenu_DragEnd( nStageX, nStageY )

	RemoveStageMouseMove( Ev_QuickMenu_Dragging )
	RemoveStageMouseUp( Ev_QuickMenu_DragEnd )

end

-- 파티 아이템 분배 UI Open/close
function QuickMenu_OpenPartyTenderNotifyButton(bNotify)
	-- 말풍선;
	if bNotify == true then
		SetBool(WIDGET_ID_QUICKMENU, "visible", false, "QuickMenu.ButtonPartyDistribution.PartyDistributionIcon" )
		-- 말풍선;
		SetSpeechBubble(WIDGET_ID_QUICKMENU, "QuickMenu.ButtonPartyDistribution", GetGameWordText("PARTY_TENDER_NOTIFY_TEXT", 0))
	end
	g_Use_QuickMenu_PartyDistribButton = bNotify
	QuickMenu_ViewSet()
end

-- 주의: 수치상으로는 내구도가 깍이는 형식이 아니라 차오르는 형식
-- 1- 일정 비율 이하시 퀵메뉴에 알리기위한 부스트비율
-- 2- 현재 남아있는 부스트양
-- 3- 부스트 총양
function QuickMenu_SetWeaponBoostInfo(fDurabilityAlarmRemainPer, fDurability, fDurabilityMax)
	local fDurabilityPer = 1 - fDurability/fDurabilityMax
	if 0.0 > fDurability or fDurabilityAlarmRemainPer < fDurabilityPer then
--		if g_Use_QuickMenu_WeaponBoost == true then
--			SetBool(WIDGET_ID_QUICKMENU, "visible", false, "QuickMenu.WeaponBoost")
--		end
		g_Use_QuickMenu_WeaponBoost = false
		QuickMenu_ViewSet()
	else
		local fDurabilityAlarmValue = fDurabilityMax * fDurabilityAlarmRemainPer
		local fRate = (fDurabilityMax-fDurability)/fDurabilityAlarmValue
		
--		if g_Use_QuickMenu_WeaponBoost == false then
--			SetBool(WIDGET_ID_QUICKMENU, "visible", true, "QuickMenu.WeaponBoost")
--		end
		g_Use_QuickMenu_WeaponBoost = true
		--SetDouble( WIDGET_ID_QUICKMENU, "value", fRate*100, "QuickMenu.WeaponBoost.WeaponBoostStage" )
		
		local strTooltip = StringFormat(GetGameInternalText("QUICKMENU_WEAPONBOOST_TOOLTIP", 0), SetFloatToString(fDurabilityPer*100, 0))
		SetDouble( WIDGET_ID_QUICKMENU, "value", fRate * 100, "QuickMenu.WeaponBoost.WeaponBoostStage" )
		SetString( WIDGET_ID_QUICKMENU, "TooltipText", strTooltip, "QuickMenu.WeaponBoost.icon" )
		
		QuickMenu_ViewSet()
		
	end
	
end

function QuickMenu_MacroDefaultSet()
	g_Use_QuickMenu_Macro = g_UserFeatures:IsUsingMacro()
	if g_Use_QuickMenu_Macro == false then
		return
	end
	
	QuickMenu_MacroViewSet()
end

function QuickMenu_MacroViewSet()

	if false == IsLoadWidget( WIDGET_ID_QUICKMENU ) then
		return
	end

	local strActiveState = ""
	if g_Macro:IsActive() == true then
		CallFunction( WIDGET_ID_QUICKMENU, "gotoAndStop", "play", "QuickMenu.MacroSlot.MacroBtn.macroIcon" )
	else
		CallFunction( WIDGET_ID_QUICKMENU, "gotoAndStop", "stop", "QuickMenu.MacroSlot.MacroBtn.macroIcon" )
	end
	
	if bUseLimitTime == true then
		local fRemainMacroTime = g_MyChar:GetRemainMacroTime()
		QuickMenu_SetMacroTime(fRemainMacroTime)
	end
	
	SetBool( WIDGET_ID_QUICKMENU, "visible", bUseLimitTime, "QuickMenu.MacroSlot.MacroBtn.remainTime" )
	
end

function QuickMenu_SetMacroTime(fTime)
	local nHour, nMinute = fTime/3600, (fTime%3600)/60
	local strTime =  string.format( "%02d:%02d", tostring(nHour), tostring(nMinute) )
	SetString(WIDGET_ID_QUICKMENU, "text", strTime, "QuickMenu.MacroSlot.MacroBtn.remainTime")
end