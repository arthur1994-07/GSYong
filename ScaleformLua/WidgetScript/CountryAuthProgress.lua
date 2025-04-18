
g_fCountryAuthProgress_Time = 0.0

-- 난사군도 인증기 로드
function LoadCountryAuthProgress()

	HitTestEnable( WIDGET_ID_COUNTRYAUTHPROGRESS, false, "" )

end

-- 오픈
function OpenCountryAuthProgress()

	CountryAuthProgress_SetProgress()

end

-- 진행바 설정
function CountryAuthProgress_SetProgress()
	SetInt( WIDGET_ID_COUNTRYAUTHPROGRESS, "value", 0, "bar" )	-- 시작값;
	
	local tbQuickSet = 
	{
		time = g_fCountryAuthProgress_Time * 1000,
		delay = 0,
		ease = EASE_NONE,
		loop = false,
		onComplete = "Ev_CountryAuthProgress_EndTimer"
	}
	SetTween( WIDGET_ID_COUNTRYAUTHPROGRESS, tbQuickSet, "value", "10", "bar" )	-- 플래쉬 세팅 최대치 기본값 10으로 이동;

end

---------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 인증기 열기/닫기
function CountryAuthProgress_Open( bVisible, fTime )

	g_fCountryAuthProgress_Time = fTime

	if false == bVisible then
	
		CloseWidget( WIDGET_ID_COUNTRYAUTHPROGRESS )
		
	else
	
		if false == IsOpenWidget( WIDGET_ID_COUNTRYAUTHPROGRESS ) then
		
			OpenWidget( WIDGET_ID_COUNTRYAUTHPROGRESS )
		
		else
		
			CountryAuthProgress_SetProgress()
		
		end
	
	end

end

---------------------------------------------------------------------------------------------------
-- 이벤트

-- 시간 종료
function Ev_CountryAuthProgress_EndTimer()

	CloseWidget( WIDGET_ID_COUNTRYAUTHPROGRESS )

end