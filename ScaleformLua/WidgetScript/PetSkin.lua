
g_nPetSkin_ItemID = -1
g_nPetSkin_PosX = -1
g_nPetSkin_PosY = -1
g_nPetSkin_HoldPosX = -1
g_nPetSkin_HoldPosY = -1

-- 로드 완료
function LoadPetSkin()

	HitTestEnable( WIDGET_ID_PETSKIN, false, "" )

end

-- 오픈
function OpenPetSkin()

	CallFunction( WIDGET_ID_PETSKIN, "gotoAndPlay", "Mix", "Petmix" )
	
	local tbQuickSet = 
	{
		time = 1000,
		delay = 0,
		ease = EASE_NONE,
		loop = false,
		onComplete = "Ev_PetSkin_ComplateAnim"
	}
	SetTween( WIDGET_ID_PETSKIN, tbQuickSet, "", "", "" )

end

-----------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- 펫 스킨 카드 사용
function PetSkin_UseCard( dwItemID, wPosX, wPosY, wHoldPosX, wHoldPosY )

	g_nPetSkin_ItemID = dwItemID
	g_nPetSkin_PosX = wPosX
	g_nPetSkin_PosY = wPosY
	g_nPetSkin_HoldPosX = wHoldPosX
	g_nPetSkin_HoldPosY = wHoldPosY

	OpenWidget( WIDGET_ID_PETSKIN )

end

-----------------------------------------------------------------------------------------------------------
-- 이벤트

-- 애니메이션 끝
function Ev_PetSkin_ComplateAnim()

	g_Msg:ReqPetSkin( g_nPetSkin_ItemID, g_nPetSkin_PosX, g_nPetSkin_PosY, g_nPetSkin_HoldPosX, g_nPetSkin_HoldPosY )
	
	CloseWidget( WIDGET_ID_PETSKIN )

end