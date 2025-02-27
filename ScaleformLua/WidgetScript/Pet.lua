g_nMax_PetSkillIndex = 30		-- 펫 최대 스킬 개수

PET_SKILL_SLOT_A = 1 			-- 스킬슬롯 A 인덱스(고정)
PET_SKILL_SLOT_B = 2 			-- 스킬슬롯 B 인덱스(고정)

g_tbPetSkill_AllList = {}		-- 펫 스킬 전체 리스트
g_tbPetSkill_NameA = {}			-- 펫 스킬명 ( 활성화 색상 포함 )
g_tbPetSkill_NameB = {}			-- 펫 스킬명 ( 활성화 색상 포함 )
g_tbPetSkill_Tooltip = {}		-- 펫 스킬 튤팁 WIDGET_ID_PET_SKILL에서 사용

g_nPrevActivatedIndexA 	= 0		-- 이전에 활성화 된 콤보박스 인덱스
g_nActivatedIndexA 		= 0		-- 현재 활성화 된 콤보박스 인덱스

g_nPrevActivatedIndexB 	= 0		-- 이전에 활성화 된 콤보박스 인덱스
g_nActivatedIndexB 		= 0		-- 현재 활성화 된 콤보박스 인덱스

g_dwSkill_ID_SlotA 	= -1		-- 현재 활성화 된 스킬아이디(dwID)
g_dwSkill_ID_SlotB 	= -1		-- 현재 활성화 된 스킬아이디(dwID)

g_nPetSatiation = 0				-- 펫 포만도
g_nMaxSkillSlot = 1				-- 사용가능한 스킬슬롯 (기본 1 최대 2)

function LoadPet()

	Pet_ResetUI()
	
	-- 텍스트 입력
	SetString( WIDGET_ID_PET, "label", GetGameWordText( "PET_NAME_STATIC", 0 ), "titleBtn" )
	SetString( WIDGET_ID_PET, "text", GetGameWordText( "PET_TEXT_STATIC", 0 ), "LabelName" )
	SetString( WIDGET_ID_PET, "text", GetGameWordText( "PET_TEXT_STATIC", 1 ), "LabelType" )
	SetString( WIDGET_ID_PET, "text", GetGameWordText( "PET_TEXT_STATIC", 2 ), "LabelSatiety" )
	
	--포만도 게이지 최소 최대 설정
	SetInt( WIDGET_ID_PET, "minimum", 0, "infoSatiation.barValue" )
	SetInt( WIDGET_ID_PET, "maximum", 100, "infoSatiation.barValue" )
	
	-- 슬롯 바인딩
	SetString( WIDGET_ID_PET, "binding", SLOT_PET_ID .. tostring( PET_SKILL_SLOT_A ), "PetSlot.Slot3" )
	SetString( WIDGET_ID_PET, "binding", SLOT_PET_ID .. tostring( PET_SKILL_SLOT_B ), "PetSlot.Slot4" )
	
	SetBool( WIDGET_ID_PET, "tooltipEnabled", false, "PetSlot.Slot3" )
	SetBool( WIDGET_ID_PET, "tooltipEnabled", false, "PetSlot.Slot4" )
	
	SetString( WIDGET_ID_PET, "binding", SLOT_PET_ITEM_ID .. "1", "PetSlot.Slot1" )
	SetString( WIDGET_ID_PET, "binding", SLOT_PET_ITEM_ID .. "2", "PetSlot.Slot2" )
	
	-- 이벤트
	SetEvent( WIDGET_ID_PET, EVENT_WIDGET_CLOSE, "ClosePet", "", "" )
	SetEvent( WIDGET_ID_PET, EVENT_LIST_INDEX_CHANGE, "Ev_Pet_ClickItemA", "index;", "ListA" )
	SetEvent( WIDGET_ID_PET, EVENT_LIST_INDEX_CHANGE, "Event_Pet_ClickItemB", "index;", "ListB" )
	
	HitTestEnable( WIDGET_ID_PET, false, "PetSlot.LabelSkillSlotA" )
	HitTestEnable( WIDGET_ID_PET, false, "PetSlot.LabelSkillSlotB" )
	SetString( WIDGET_ID_PET, "text", "A", "PetSlot.LabelSkillSlotA" )
	SetString( WIDGET_ID_PET, "text", "B", "PetSlot.LabelSkillSlotB" )

end

function OpenPet()

	Pet_Update( true )
	
	SetInt( WIDGET_ID_PET, "selectedIndex", g_nActivatedIndexA, "ListA" )
	SetInt( WIDGET_ID_PET, "selectedIndex", g_nActivatedIndexB, "ListB" )

end

-- 펫 닫기
function ClosePet()

	-- 펫창을 닫을 경우 펫 퀵 아이콘을 활성화 시킨다.
	SetBool( WIDGET_ID_QUICKMENU, "selected", "true", "QuickMenu.pet.ButtonPet" )
	
end

function Pet_GetSatiation()

	g_nPetSatiation = g_Pet:GetSatiation()
	
	local strSatiation = string.format( "%.2f%%", g_nPetSatiation / 10.0 )
	SetString( WIDGET_ID_PET, "text", strSatiation, "infoSatiation.tfValue" )
	SetDouble( WIDGET_ID_PET, "value", g_nPetSatiation / 10.0, "infoSatiation.barValue" )
	
	-- 퀵메뉴 펫 공복도 게이지를 조절
	QuickMenu_Pet_Hungry_Gauge()
	
end

function Pet_SetActivatedSkill( nIndexA, nIndexB )

	--클라이언트로 변경된 인덱스 정보 전송(현재 함수가 호출되었다면 UI에 변경사항이 있다는 의미)
	local tbActivatedSkill = {}
	
	local tbActiveSkillA  = {}
	local tbActiveSkillB  = {}

	tbActiveSkillA[ "dwID" ] = g_tbPetSkill_NameA[ nIndexA + 1 ][ 2 ]
	table.insert( tbActivatedSkill, tbActiveSkillA )
	
	tbActiveSkillB[ "dwID" ] = g_tbPetSkill_NameB[ nIndexB + 1 ][ 2 ]
	table.insert( tbActivatedSkill, tbActiveSkillB )
	
	g_Msg:SetActivatedSkillPet( tbActivatedSkill )

end

--클라이언트에서 받아온 정보를 UI에 사용 가능하도록 변환
function Pet_UpdateInfo()
 	
	g_tbPetSkill_AllList = {}		-- 펫 스킬 전체 리스트
	g_tbPetSkill_NameA = {}			-- 펫 스킬명 ( 활성화 색상 포함 )
	g_tbPetSkill_NameB = {}			-- 펫 스킬명 ( 활성화 색상 포함 )
	
	--리스트 초기화
	local tbDataName = {}
	tbDataName[ 1 ] = GetGameWordText( "PET_TEXT_STATIC", 5 )
	tbDataName[ 2 ] = -1
	
	table.insert( g_tbPetSkill_NameA, tbDataName )
	table.insert( g_tbPetSkill_NameB, tbDataName )
	
	g_nActivatedIndexA = 0		-- 현재 활성화 된 콤보박스 인덱스
	g_nActivatedIndexB = 0		-- 현재 활성화 된 콤보박스 인덱스
	
	g_dwSkill_ID_SlotA = -1		-- 현재 활성화 된 스킬아이디(dwID)
	g_dwSkill_ID_SlotB = -1		-- 현재 활성화 된 스킬아이디(dwID)
	
	--데이터 획득 및 편집
	for i = 1, g_nMax_PetSkillIndex, 1 do
	
		-- 펫 스킬 정보 획득
		table.insert( g_tbPetSkill_AllList, g_Pet:GetPetSkill( i - 1 ) )	
		
		if nil ~= g_tbPetSkill_AllList[ i ] then
			
			local tbPetSkill = CopyTable( g_tbPetSkill_AllList[ i ] )

			if nil ~= table.getn( tbPetSkill ) then
			
				local tbDataNameA = {}
				local tbDataNameB = {}
				local tbDataTooltip = {}
				
				-- 스킬 명칭이 있는지 확인 없으면 스킬이 존재 안함
				if nil ~= tbPetSkill[ "strName" ] then
					
					if 	1 == tbPetSkill["nActivated"] then
						
						tbDataNameA[ 1 ] = tbPetSkill[ "strName" ]
						tbDataNameA[ 2 ] = tbPetSkill[ "dwID" ]
						
						tbDataNameB[ 1 ] = HTMLFontColor( tbPetSkill[ "strName" ], HTML_TEXT_COLOR_RED ) 
						tbDataNameB[ 2 ] = tbPetSkill[ "dwID" ]
						
						g_nActivatedIndexA = table.getn( g_tbPetSkill_NameA )
						g_dwSkill_ID_SlotA = tbPetSkill[ "dwID" ]
						
					elseif 2 == tbPetSkill["nActivated"] then
					
						tbDataNameA[ 1 ] = HTMLFontColor( tbPetSkill[ "strName" ], HTML_TEXT_COLOR_RED )
						tbDataNameA[ 2 ] = tbPetSkill[ "dwID" ]
						
						tbDataNameB[ 1 ] = tbPetSkill[ "strName" ] 
						tbDataNameB[ 2 ] = tbPetSkill[ "dwID" ]
						
						g_nActivatedIndexB = table.getn( g_tbPetSkill_NameB )
						g_dwSkill_ID_SlotB = tbPetSkill[ "dwID" ]

					else
					
						tbDataNameA[ 1 ] = tbPetSkill[ "strName" ]
						tbDataNameA[ 2 ] = tbPetSkill[ "dwID" ]
						
						tbDataNameB[ 1 ] = tbPetSkill[ "strName" ]
						tbDataNameB[ 2 ] = tbPetSkill[ "dwID" ]
						
					end
					
					table.insert( g_tbPetSkill_NameA, tbDataNameA )
					table.insert( g_tbPetSkill_NameB, tbDataNameB )

					-- 펫 스킬용 튤팁(전체 리스트를 받기 위함)
					local strSkillTooltip = "[" ..  tbPetSkill[ "strName" ] .. "]"
					
					if 1 == tbPetSkill[ "bPassive" ] then
						strSkillTooltip = strSkillTooltip
					end 
					
					-- 펫 스킬용 튤팁(전체 리스트를 받기 위함)
					local strSkillTooltip = "[" ..  tbPetSkill[ "strName" ] .. "]" .. 
						GetGameWordText ( "SKILL_SLOT", 0 ).. "." .. tbPetSkill[ "dwGrade" ]
					
					if 1 == tbPetSkill[ "bPassive" ] then
						strSkillTooltip = strSkillTooltip .. GetGameWordText ( "SKILL_SLOT", 1 );
					end 					
					
					tbDataTooltip[ 1 ] = strSkillTooltip
					table.insert( g_tbPetSkill_Tooltip, tbDataTooltip )
					
					-- 콤보 박스용(사용중인 스킬 붉은색 활성화)
					if 0 == tbPetSkill[ "bLearn" ] then
					
						-- 왜 1은 (1번 슬롯에서 사용 중)을 넣어야 회색이 되는지 의문?
						tbDataNameA[ 1 ] = HTMLFontColor( tbPetSkill[ "strName" ], HTML_TEXT_COLOR_GRAY )
						tbDataNameB[ 1 ] = HTMLFontColor( tbPetSkill[ "strName" ], HTML_TEXT_COLOR_GRAY ) 
					
					end
					
				end -- if nil ~= tbPetSkill[ "strName" ] then
				
			end
			
		end

	end --end for
	
end

-- 펫 아이템 슬롯 업데이트
function Pet_UpdateItemSlot()

	local tbItemList = g_Pet:GetPetItemList()
	
	for nIndex in pairs( tbItemList ) do
	
		local tbItem = tbItemList[ nIndex ]
		if nil ~= tbItem then
			SetSlot_Pet( SLOT_PET_ITEM_ID .. tostring( nIndex ), tbItem[ "wMID" ], tbItem[ "wSID" ], nIndex )
		end
		
	end
	
end

--UI에서 갱신을 해야하는 함수의 변화를 확인
function Pet_CheckChangeInfo()

	if	g_nPrevActivatedIndexA ~= g_nActivatedIndexA or
		g_nPrevActivatedIndexB ~= g_nActivatedIndexB or
		g_nMaxSkillSlot ~= g_Pet:GetMaxSkillSlot() or
		g_nPetSatiation ~= g_Pet:GetSatiation() then
		
		return true
		
	end
	
	return false
	
end

--UI 정보 갱신
function Update_PetUI()

	--펫 이름
	SetString( WIDGET_ID_PET, "text", g_Pet:GetName(), "LabelPetName")
	
	--펫 타입
	SetString( WIDGET_ID_PET, "text", g_Pet:GetTypeString(), "LabelPetType")
	
	-- 펫 포만도 설정
	Pet_GetSatiation()
	
	-- 슬롯 설정
	g_nMaxSkillSlot = g_Pet:GetMaxSkillSlot()
	
	if 2 == g_nMaxSkillSlot then
	
		SetBool( WIDGET_ID_PET, "enabled", 1, "ListA" )
		SetBool( WIDGET_ID_PET, "enabled", 1, "ListB" )
		
	else
	
		SetBool( WIDGET_ID_PET, "enabled", 1, "ListA" )
		SetBool( WIDGET_ID_PET, "enabled", 0, "ListB" )
		
	end
	
	--List
	g_nPrevActivatedIndexA = g_nActivatedIndexA
	g_nPrevActivatedIndexB = g_nActivatedIndexB
	
	SetDataEx( WIDGET_ID_PET, "label;SkillID;", g_tbPetSkill_NameA, "ListA", false )
	SetDataEx( WIDGET_ID_PET, "label;SkillID;", g_tbPetSkill_NameB, "ListB", false )
	
	--Slot
	SetSlot_Skill( SLOT_PET_ID .. tostring( PET_SKILL_SLOT_A ), g_dwSkill_ID_SlotA )
	SetSlot_Skill( SLOT_PET_ID .. tostring( PET_SKILL_SLOT_B ), g_dwSkill_ID_SlotB )

end

function Pet_ResetUI()

	g_nMax_PetSkillIndex = 80		-- 펫 최대 스킬 갯수

	g_tbPetSkill_AllList = {}		-- 펫 스킬 전체 리스트
	g_tbPetSkill_NameA = {}			-- 펫 스킬명 ( 활성화 색상 포함 )
	g_tbPetSkill_NameB = {}			-- 펫 스킬명 ( 활성화 색상 포함 )
	g_tbPetSkill_Tooltip = {}		-- 펫 스킬 튤팁 WIDGET_ID_PET_SKILL에서 사용 <-펫 창에서 사용되도록 수정 필요.
	
	--리스트 초기화
	local tbDataName = {}
	tbDataName[ 1 ] = GetGameWordText( "PET_TEXT_STATIC", 5 )
	tbDataName[ 2 ] = -1
	
	table.insert( g_tbPetSkill_NameA, tbDataName )
	table.insert( g_tbPetSkill_NameB, tbDataName )
	
	g_nPrevActivatedIndexA 	= 0		-- 이전에 활성화 된 콤보박스 인덱스
	g_nActivatedIndexA 		= 0		-- 현재 활성화 된 콤보박스 인덱스

	g_nPrevActivatedIndexB 	= 0		-- 이전에 활성화 된 콤보박스 인덱스
	g_nActivatedIndexB 		= 0		-- 현재 활성화 된 콤보박스 인덱스
	
	g_SkillSlotA 	= -1			-- 현재 활성화 된 스킬아이디(dwID)
	g_SkillSlotB 	= -1			-- 현재 활성화 된 스킬아이디(dwID)

	g_MaxSkillSlot = 0				-- 사용가능한 스킬슬롯 (기본 1 아이템으로 최대 2)
	
end

------------------------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- UI 관련 데이터 갱신
function Pet_Update( bInit )

	-- 퀵메뉴 펫 공복도 게이지 설정
	QuickMenu_Pet_Hungry_Gauge()
	
	if false == IsOpenWidget( WIDGET_ID_PET ) then
		return
	end

	--클라이언트에서 받아온 정보를 UI에 사용 가능하도록 변환
	Pet_UpdateInfo()
	Pet_UpdateItemSlot()
	
	--업데이트 부하를 줄이기 위하여 변화 조건 검사
	if true == bInit or true == Pet_CheckChangeInfo() then
	
		--UI 정보 갱신
		Update_PetUI()
		
		--펫 스킬창 업데이트
		PetSkill_Update()
		
	end
	
	SetDataEx( WIDGET_ID_PET, "label;SkillID;", g_tbPetSkill_NameA, "ListA", false )
	SetDataEx( WIDGET_ID_PET, "label;SkillID;", g_tbPetSkill_NameB, "ListB", false )
end

-- 펫 UI 끄기
function Pet_CloseWindow()

	CloseWidget( WIDGET_ID_PET )
	
end

-- 펫 이름 변경
function Pet_Rename()

	OpenMsgBox( MSG_BOX_TYPE_INPUT, GetGameInternalText( "INVEN_CHAR_RENAME", 0 ), Ev_Pet_Rename, Event_MsgBox_Close )

end

------------------------------------------------------------------------------------------------------------------
-- 이벤트

--스킬A 드롭 박스 클릭시 스킬 조건 검색 
function Ev_Pet_ClickItemA( nIndex )
	--현재 활성화된 콤보박스 인덱스와 다르면 아래 조건 검사 
	if nIndex ~= g_nActivatedIndexA then
	
		if 0 < nIndex then
		
			-- 조건검사 시작
			local tbPetSkill = g_tbPetSkill_AllList[ nIndex ]
			
			-- 1. 선택된 펫 스킬이 사용 가능한지 판단
			if nil ~= tbPetSkill then			
				-- 배우지 않았다면;
				if 0 == tbPetSkill[ "bLearn" ] then
					g_nActivatedIndexA = g_nPrevActivatedIndexA
					SetInt( WIDGET_ID_PET, "selectedIndex", g_nPrevActivatedIndexA, "ListA" )
					CF_PrintMsg_SystemMsg( GetGameInternalText( "PET_SYSTEM_DISABLED_MESSAGE", 2 ), TEXT_COLOR_RED )					
					return
				end
				-- 2.1 펫 스킬이 사용 가능하다면( 배운 스킬 )
				--슬롯 B에 사용된 슬롯과 같으면,  현재 스킬 슬롯과 이전 스킬 슬롯으로 표기
				if ( nIndex ~= 0 and nIndex == g_nActivatedIndexB ) then
				
					g_nActivatedIndexA = g_nPrevActivatedIndexA
					
					SetInt( WIDGET_ID_PET, "selectedIndex", g_nPrevActivatedIndexA, "ListA" )
					CF_PrintMsg_SystemMsg( GetGameInternalText( "PET_SYSTEM_DUPLICATE_MESSAGE", 1 ), TEXT_COLOR_RED )
					
					return
					
				end
				
			else
			
				-- 2.2 펫 스킬이 사용 불가라면
				--슬롯 B에 사용된 슬롯과 같으면,  현재 스킬 슬롯과 이전 스킬 슬롯으로 표기
				if nIndex ~= 0 and nIndex == g_nActivatedIndexB then
				
					g_nActivatedIndexA = g_nPrevActivatedIndexA
					
					SetInt( WIDGET_ID_PET, "selectedIndex", g_nPrevActivatedIndexA, "ListA" )
					CF_PrintMsg_SystemMsg( GetGameInternalText( "PET_SYSTEM_DISABLED_MESSAGE", 0 ), TEXT_COLOR_RED )
					
					return
					
				end
				
			end --if nil ~= tbPetSkill then
			
		else --if 0 < nIndex then
		
			--오류처리
			--슬롯 B에 사용된 슬롯과 같으면,  현재 스킬 슬롯과 이전 스킬 슬롯으로 표기
			if nIndex ~= 0 and nIndex == g_nActivatedIndexB then
			
				g_nActivatedIndexA = g_nPrevActivatedIndexA				
				
				SetInt( WIDGET_ID_PET, "selectedIndex", g_nPrevActivatedIndexA, "ListA" )
				CF_PrintMsg_SystemMsg( GetGameInternalText( "PET_SYSTEM_DUPLICATE_MESSAGE", 1 ), TEXT_COLOR_RED )
				
				return
				
			end
			
		end -- if 0 < nIndex then
		
		--정상 조건
		g_nActivatedIndexA = nIndex
		
		if 0 == nIndex then
		
			g_dwSkill_ID_SlotA = -1	
			
		else
		
			--현재 활성화딘 콤보박스 인덱스와 다르면 스킬변경.
			local tbActiveSkillA = CopyTable( g_tbPetSkill_NameA[ g_nActivatedIndexA + 1 ] )
			g_dwSkill_ID_SlotA = tbActiveSkillA[ 2 ]
			
		end
		
		--활성화 시킬 스킬 ID를 입력한다
		Pet_SetActivatedSkill( nIndex, g_nActivatedIndexB )
		
	end

end

function Event_Pet_ClickItemB( nIndex )
	
	--현재 활성화딘 콤보박스 인덱스와 다르면 아래 조건 검사 
	if nIndex ~= g_nActivatedIndexB then
	
		if 0 < nIndex then
		
			local tbPetSkill = g_tbPetSkill_AllList[ nIndex ]
			
			if nil ~= tbPetSkill then
			
				-- 배우지 않았다면;
				if 0 == tbPetSkill[ "bLearn" ] then
					g_nActivatedIndexB = g_nPrevActivatedIndexB
					SetInt( WIDGET_ID_PET, "selectedIndex", g_nPrevActivatedIndexB, "ListB" )
					CF_PrintMsg_SystemMsg( GetGameInternalText( "PET_SYSTEM_DISABLED_MESSAGE", 2 ), TEXT_COLOR_RED )
					return					
				end
				
				--인덱스 변경 안되는 조건
				if ( nIndex ~= 0 and nIndex == g_nActivatedIndexA ) then
				
					g_nActivatedIndexB = g_nPrevActivatedIndexB
					
					SetInt( WIDGET_ID_PET, "selectedIndex", g_nPrevActivatedIndexB, "ListB" )
					--선택 불가시 시스템 메시지 출력
					CF_PrintMsg_SystemMsg( GetGameInternalText( "PET_SYSTEM_DUPLICATE_MESSAGE", 1 ), TEXT_COLOR_RED )
					
					return
					
				end
			else
			
				if ( nIndex ~= 0 and nIndex == g_nActivatedIndexA ) or 0 == tbPetSkill[ "bLearn" ] then
				
					g_nActivatedIndexB = g_nPrevActivatedIndexB
					
					SetInt( WIDGET_ID_PET, "selectedIndex", g_nPrevActivatedIndexB, "ListB" )
					--선택 불가시 시스템 메시지 출력
					CF_PrintMsg_SystemMsg( GetGameInternalText( "RNCOMPETITION_SUB_TYPE_CDM", 0 ), TEXT_COLOR_RED )
					
					return
					
				end
				
			end
			
		else
		
			--오류처리
			if nIndex ~= 0 and nIndex == g_nActivatedIndexA then
			
				g_nActivatedIndexB = g_nPrevActivatedIndexB
				
				SetInt( WIDGET_ID_PET, "selectedIndex", g_nPrevActivatedIndexB, "ListB" )
				--선택 불가시 시스템 메시지 출력
				CF_PrintMsg_SystemMsg( GetGameInternalText( "RNCOMPETITION_SUB_TYPE_CDM", 0 ), TEXT_COLOR_RED )
				
				return
				
			end	
			
		end
		
		--정상 조건
		g_nActivatedIndexB = nIndex
		
		if 0 == nIndex then
		
			g_dwSkill_ID_SlotB = -1	
			
		else
		
			local tbActiveSkillB = CopyTable( g_tbPetSkill_NameA[ g_nActivatedIndexB + 1 ] )
			g_dwSkill_ID_SlotB = tbActiveSkillB[ 2 ]
			
		end
		
		Pet_SetActivatedSkill( g_nActivatedIndexA, nIndex )
		
	end
end

-- 펫 이름 변경 확인
function Ev_Pet_Rename()

	Event_MsgBox_Close()
	
	local strName = GetMsgBoxInput()
	g_Msg:ReqRenamePet( strName )

end