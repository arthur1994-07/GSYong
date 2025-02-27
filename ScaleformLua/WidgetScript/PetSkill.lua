g_nSkill_MaxDisplay = 4			-- 한 페이지당 보일 스킬 갯수

function LoadPetSkill()
	
	--WIDGET_ID_PET 에 귀속적인 윈도우이므로 해당 윈도우가 정상오픈 되어 있어야 한다
	if false == IsOpenWidget( WIDGET_ID_PET ) then
		return
	end
	
	--초기 출력 위치 설정
	local nInvenX = GetInt( WIDGET_ID_PET, "x", "" )
	local nInvenY = GetInt( WIDGET_ID_PET, "y", "" )
	local nInvenW = GetInt( WIDGET_ID_PET, "width", "" )
	
	local nWidth = GetInt( WIDGET_ID_PET_SKILL, "width", "" )
	
	local nNewX = nInvenX - nWidth
	if 0 > nNewX then
		nNewX = nInvenX + nInvenW
	end
	
	SetInt( WIDGET_ID_PET_SKILL, "x", nNewX, "" )
	SetInt( WIDGET_ID_PET_SKILL, "y", nInvenY, "" )
		
	-- 텍스트 입력
	SetString( WIDGET_ID_PET_SKILL, "title", GetGameWordText( "PET_NAME_STATIC", 1 ), "" )
	
	
	-- 슬롯 바인딩
	for i = 1, g_nSkill_MaxDisplay, 1 do	
		SetString( WIDGET_ID_PET_SKILL, "binding", SLOT_PET_SKILL_ID .. tostring( i ), "slot" .. tostring( i ) )	
	end

	-- 리스트 스크롤 이벤트
	SetEvent( WIDGET_ID_PET_SKILL, EVENT_SCROLL, "Ev_PetSkill_ScrollList", "", "list" )
	
	PetSkill_UpdateListState( 0 )

end

function PetSkill_Update ()

	if false == IsOpenWidget( WIDGET_ID_PET_SKILL ) then
		return
	end
	
	Ev_PetSkill_ScrollList()
	
end

-- 리스트 상태 업데이트
function PetSkill_UpdateListState( nScrollPos )

	local tbSkillList = g_tbPetSkill_AllList
	local nOffset = nScrollPos
	
	for i = 1, g_nSkill_MaxDisplay, 1 do
	
		local nIndex = i + nOffset
	
		if nIndex <= table.getn( tbSkillList ) then
		
			-- 슬롯 업데이트
			local tbSkill = CopyTable( tbSkillList[ nIndex ] )
			
			if nil ~= tbSkill[ "dwID" ] then
				SetSlot_Skill( SLOT_PET_SKILL_ID .. tostring( i ), tbSkill[ "dwID" ] )	
			else 
				ResetSlot(SLOT_PET_SKILL_ID .. tostring( i ))
			end
		end
		
	end
	
	SetDataEx( WIDGET_ID_PET_SKILL, "label;", g_tbPetSkill_Tooltip, "list", false )

end

-- 리스트 스크롤 이벤트
function Ev_PetSkill_ScrollList()

	-- 현재 스크롤 위치
	local nScrollPos = GetInt( WIDGET_ID_PET_SKILL, "scrollPosition", "list" )
	
	PetSkill_UpdateListState( nScrollPos )

end


