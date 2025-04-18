
-- 분배 타입 (0 : 주사위, 1 : 마스터)
g_nPartyDistribution_Type = 0

-- 로드 완료
function LoadPartyDistribution()

	-- 입찰 외 포기
	local str = GetGameWordText( "PARTY_DICE_TENDER_WINDOW_TEXT", 1 )
	SetString( WIDGET_ID_PARTYDISTRIBUTION, "label", str, "btnGiveUp" )
	SetEvent( WIDGET_ID_PARTYDISTRIBUTION, EVENT_BUTTON_CLICK, "Ev_PartyDistribution_GiveUp", "", "btnGiveUp" )
	
	-- 입찰
	str = GetGameWordText( "PARTY_DICE_TENDER_WINDOW_TEXT", 2 )
	SetString( WIDGET_ID_PARTYDISTRIBUTION, "label", str, "btnBidding" )
	SetEvent( WIDGET_ID_PARTYDISTRIBUTION, EVENT_BUTTON_CLICK, "Ev_PartyDistribution_Bidding", "", "btnBidding" )
	
	-- 버리기
	str = GetGameWordText( "PARTY_DISTRIBUTION_WINDOW_TEXT", 0 )
	SetString( WIDGET_ID_PARTYDISTRIBUTION, "label", str, "btnDiscard" )
	SetEvent( WIDGET_ID_PARTYDISTRIBUTION, EVENT_BUTTON_CLICK, "Ev_PartyDistribution_Discard", "", "btnDiscard" )

end

-- 오픈
function OpenPartyDistribution()

	local str = ""

	-- 주사위
	if 0 == g_nPartyDistribution_Type then
	
		SetBool( WIDGET_ID_PARTYDISTRIBUTION, "visible", true, "btnGiveUp" )
		SetBool( WIDGET_ID_PARTYDISTRIBUTION, "visible", true, "btnBidding" )
		SetBool( WIDGET_ID_PARTYDISTRIBUTION, "visible", false, "btnDiscard" )
		
		-- 타이틀 텍스트
		str = GetGameWordText( "PARTY_MODAL_ITEM_OPTION", 5 ) .. "(" .. 
			GetGameWordText( "PARTY_MODAL_ITEM_OPTION_FILTER", g_Party:GetItemFilterOption() ) .. ")"
	
		
	-- 마스터
	elseif 1 == g_nPartyDistribution_Type then
	
		SetBool( WIDGET_ID_PARTYDISTRIBUTION, "visible", false, "btnGiveUp" )
		SetBool( WIDGET_ID_PARTYDISTRIBUTION, "visible", false, "btnBidding" )
		SetBool( WIDGET_ID_PARTYDISTRIBUTION, "visible", true, "btnDiscard" )
		
		-- 타이틀 텍스트
		str = GetGameWordText( "PARTY_MODAL_ITEM_OPTION", 4 )
	
	end
	
	-- 타이틀
	SetString( WIDGET_ID_PARTYDISTRIBUTION, "title", str, "" )

end

---------------------------------------------------------------------------------------------------------
-- 이벤트

-- 입찰 외 포기
function Ev_PartyDistribution_GiveUp()
end

-- 입찰
function Ev_PartyDistribution_Bidding()
end

-- 버리기
function Ev_PartyDistribution_Discard()
end