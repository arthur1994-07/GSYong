
nCTFieldHP_Height = 0
nCTFieldHP_SM = 0
nCTFieldHP_HA = 0
nCTFieldHP_BH = 0

g_bCTFieldHPDisplayOpen = false

function LoadCTFieldHpDisplay()

	-- UI 닫기 이벤트
	SetEvent( WIDGET_ID_CTFIELD_HPDISPLAY, EVENT_WIDGET_CLOSE, "CloseCTFieldHpDisplay", "", "" )
	
	HitTestEnable( WIDGET_ID_CTFIELD_HPDISPLAY, false, "" )
	
	nCTFieldHP_Height = GetInt( WIDGET_ID_CTFIELD_HPDISPLAY, "height", "mcHpSM" ) * 0.5
	
end

function OpenCTFieldHpDisplay()

	g_bCTFieldHPDisplayOpen = true
	
	CTFieldHpDisplay_SetAuthenticHP( nCTFieldHP_SM, nCTFieldHP_HA, nCTFieldHP_BH, 0, 0 )
	
end

function CloseCTFieldHpDisplay()

	g_bCTFieldHPDisplayOpen = false
	
end

-- Client call
function CTFieldHpDisplay_SetAuthenticHP( nSM, nHA, nBH, posX, posY )

	if g_bCTFieldHPDisplayOpen == false then
	
		nCTFieldHP_SM = nSM
		nCTFieldHP_HA = nHA
		nCTFieldHP_BH = nBH
		OpenWidget( WIDGET_ID_CTFIELD_HPDISPLAY )
		
		return
		
	end
	
	if 0 ~= nSM and 0 ~= nHA and 0 ~= nBH then
	
		SetBool( WIDGET_ID_CTFIELD_HPDISPLAY, "visible", false, "mcWinner" )
	
	else
	
		SetBool( WIDGET_ID_CTFIELD_HPDISPLAY, "visible", true, "mcWinner" )
		
		local nX, nY = 0, 0
		if 0 == nSM then
		
			nX = GetInt( WIDGET_ID_CTFIELD_HPDISPLAY, "x", "bgSM" )
			nY = GetInt( WIDGET_ID_CTFIELD_HPDISPLAY, "y", "bgSM" )
		
		elseif 0 == nHA then
		
			nX = GetInt( WIDGET_ID_CTFIELD_HPDISPLAY, "x", "bgHA" )
			nY = GetInt( WIDGET_ID_CTFIELD_HPDISPLAY, "y", "bgHA" )
		
		elseif 0 == nBH then
		
			nX = GetInt( WIDGET_ID_CTFIELD_HPDISPLAY, "x", "bgBH" )
			nY = GetInt( WIDGET_ID_CTFIELD_HPDISPLAY, "y", "bgBH" )
		
		end
		
		SetInt( WIDGET_ID_CTFIELD_HPDISPLAY, "x", nX, "mcWinner" )
		SetInt( WIDGET_ID_CTFIELD_HPDISPLAY, "y", nY, "mcWinner" )
		
	end
	
	SetInt( WIDGET_ID_CTFIELD_HPDISPLAY, "y", nCTFieldHP_Height * nSM, "mcHpSM.Mask" )
	SetInt( WIDGET_ID_CTFIELD_HPDISPLAY, "y", nCTFieldHP_Height * nHA, "mcHpHA.Mask" )
	SetInt( WIDGET_ID_CTFIELD_HPDISPLAY, "y", nCTFieldHP_Height * nBH, "mcHpBH.Mask" )
	
	SetInt( WIDGET_ID_CTFIELD_HPDISPLAY, "x", posX, "" )
	SetInt( WIDGET_ID_CTFIELD_HPDISPLAY, "y", posY, "" )
	
end