
-- 타이틀 로드 완료
function LoadTitle()

	SetString( WIDGET_ID_TITLE, "text", GetGameWordText( "COPYRIGHT_TITLE", 0 ), "title.labelTitle" )
	SetString( WIDGET_ID_TITLE, "text", GetGameWordText( "COPYRIGHT_COMPANY", 0 ), "title.labelCopyright" )

end

-- 타이틀 오픈
function OpenTitle()

	Title_UIPosition( GetScreenWidth(), GetScreenHeight() )
	local nSP = GetServiceProvider()
	if ( nSP == SP_TAIWAN ) then
		SetBool( WIDGET_ID_TITLE, "visible", true, "title.imgTitle_taiwan" )
		SetBool( WIDGET_ID_TITLE, "visible", false, "title.imgTitle_default" )
		
	else
		SetBool( WIDGET_ID_TITLE, "visible", false, "title.imgTitle_taiwan" )
		SetBool( WIDGET_ID_TITLE, "visible", true, "title.imgTitle_default" )
		
	end
	
end

-- 위치 잡기
function Title_UIPosition( nScreenWidth, nScreenHeight )

	if false == IsOpenWidget( WIDGET_ID_TITLE ) then
		return
	end

	-- 타이틀 이미지
	local nSP = GetServiceProvider()
	local strImgTitle = "title.imgTitle_default"
	if ( nSP == SP_TAIWAN ) then
		strImgTitle = "title.imgTitle_taiwan"
	end
	
	SetInt( WIDGET_ID_TITLE, "x", nScreenWidth / 2, strImgTitle)
	SetInt( WIDGET_ID_TITLE, "y", 150, strImgTitle )
	
	-- 타이틀 텍스트
	local nWidth = GetInt( WIDGET_ID_TITLE, "width", "title.labelTitle" )	
	SetInt( WIDGET_ID_TITLE, "x", ( nScreenWidth - nWidth ) / 2, "title.labelTitle" )
	SetInt( WIDGET_ID_TITLE, "y", nScreenHeight - 50, "title.labelTitle" )
	
	-- 저작권 텍스트
	nWidth = GetInt( WIDGET_ID_TITLE, "width", "title.labelCopyright" )	
	SetInt( WIDGET_ID_TITLE, "x", ( nScreenWidth - nWidth ) / 2, "title.labelCopyright" )
	SetInt( WIDGET_ID_TITLE, "y", nScreenHeight - 34, "title.labelCopyright" )

end
