function LoadFullScreen()
	-- ��ü ȭ�� ��ư �̺�Ʈ
	SetEvent( WIDGET_ID_FULLSCREEN, EVENT_BUTTON_CLICK, "Ev_FullScreen_btnFullScreen", "", "FullScreen" )	
end


-- �̺�Ʈ

-- ��üȭ�� ��ư �̺�Ʈ
function Ev_FullScreen_btnFullScreen()

	ChangeScreenMode()

end