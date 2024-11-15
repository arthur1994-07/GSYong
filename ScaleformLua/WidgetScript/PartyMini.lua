 
-- 파티 미니 UI 로드 완료
function LoadPartyMini()

	PartyMini_UpdateList()
	SetEvent( WIDGET_ID_PARTYMINI, EVENT_LIST_ITEM_UP, "Ev_PartyMini_MouseClick", "buttonIdx;index;", "list" )
	SetEvent( WIDGET_ID_PARTYMINI, EVENT_LIST_ITEM_ROLLOVER, "Ev_PartyMini_Rollover", "index;", "list" )
	SetEvent( WIDGET_ID_PARTYMINI, EVENT_LIST_ITEM_ROLLOUT,  "Ev_PartyMini_Rollout",  "index;", "list" )
	
end

function Ev_PartyMini_MouseClick(nbutton, nIndex)

	if nbutton ~= 1 then
		return
	end
	
	local tbMember = g_tbParty_MemberList[nIndex+1]
	g_MyChar:PartyMemberSkillReaction(tbMember["dwGaeaID"])
	
end

function Ev_PartyMini_Rollover(nIndex)
	local tbMember = g_tbParty_MemberList[nIndex+1]
	BMP_SetPartyMemberInfo(tbMember)
end

function Ev_PartyMini_Rollout(nIndex)
	CloseWidget(WIDGET_ID_BUFFMINIPARTY)
end

-----------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 파티 생성
function PartyMini_NewParty()

	if false == IsLoadWidget( WIDGET_ID_PARTYMINI ) then
	
		OpenWidget( WIDGET_ID_PARTYMINI )
		
	else
	
		if false == IsOpenWidget( WIDGET_ID_PARTYMINI ) then
			OpenWidget( WIDGET_ID_PARTYMINI )
		end
		
		PartyMini_UpdateList()
		
	end
	
end

-- 파티멤버 리스트 업데이트
function PartyMini_UpdateList()

	if false == IsLoadWidget( WIDGET_ID_PARTYMINI ) then
		return
	end
	
	local tbData = {}
	
	-- 아이콘 설정
	for i = 1, PARTY_MAX, 1 do
	
		local strInst = "item" .. i
	
		local tbMember = g_tbParty_MemberList[ i ]
		if nil == tbMember then
		
			SetString( WIDGET_ID_PARTYMINI, "text", "", strInst .. ".tfPartyIcon" )
			SetString( WIDGET_ID_PARTYMINI, "text", "", strInst .. ".tfSchoolIcon" )
			SetString( WIDGET_ID_PARTYMINI, "text", "", strInst .. ".tfLevel" )
		
		else
		
			local tb = 
			{
				tbMember[ "strName" ]
			}
			table.insert( tbData, tb )

			local strLevel = "Lv" .. tbMember[ "nLevel" ]
			local strIcon = ""
			
			if 1 ~= i then
				strIcon = " " .. tostring( i - 1 )
			end
			
			SetString( WIDGET_ID_PARTYMINI, "text", strIcon, strInst .. ".tfPartyIcon" )
			SetString( WIDGET_ID_PARTYMINI, "htmlText", Party_GetSchoolImg( tbMember ), strInst .. ".tfSchoolIcon" )
			SetString( WIDGET_ID_PARTYMINI, "text", strLevel, strInst .. ".tfLevel" )
		
		end

	end
	
	SetDataEx( WIDGET_ID_PARTYMINI, "label", tbData, "list", false )

end

-- 파티 해산
function PartyMini_Dissolve()

	CloseWidget( WIDGET_ID_PARTYMINI )
	
end