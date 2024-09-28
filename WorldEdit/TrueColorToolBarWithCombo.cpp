#include "stdafx.h"
#include "TrueColorToolBarWithCombo.h"

struct CToolBarData
{
	WORD wVersion;
	WORD wWidth;
	WORD wHeight;
	WORD wItemCount;
	//WORD aItems[wItemCount]

	WORD* items()
	{ return (WORD*)(this+1); }
};

BOOL CTrueColorToolBarWithCombo::LoadToolBar(UINT unToolBarID, UINT unBitmapID)
{
	ASSERT_VALID(this);
	ASSERT(unToolBarID != 0);
	ASSERT(unBitmapID != 0);

	LPCTSTR lpszResourceName = MAKEINTRESOURCE(unToolBarID);
	LPCTSTR lpszBmpResourceName = MAKEINTRESOURCE(unBitmapID);

	ASSERT(lpszResourceName != NULL);
	ASSERT(lpszBmpResourceName != NULL);

	// determine location of the bitmap in resource fork
	HINSTANCE hInst = AfxFindResourceHandle(lpszResourceName, RT_TOOLBAR);
	HRSRC hRsrc = ::FindResource(hInst, lpszResourceName, RT_TOOLBAR);
	if (hRsrc == NULL)
		return FALSE;

	HGLOBAL hGlobal = LoadResource(hInst, hRsrc);
	if (hGlobal == NULL)
		return FALSE;

	CToolBarData* pData = (CToolBarData*)LockResource(hGlobal);
	if (pData == NULL)
		return FALSE;
	ASSERT(pData->wVersion == 1);

	UINT* pItems = new UINT[pData->wItemCount];
	for (int i = 0; i < pData->wItemCount; i++)
		pItems[i] = pData->items()[i];
	BOOL bResult = SetButtons(pItems, pData->wItemCount);
	delete[] pItems;

	if (bResult)
	{
		// set new sizes of the buttons
		CSize sizeImage(pData->wWidth, pData->wHeight);
		CSize sizeButton(pData->wWidth + 7, pData->wHeight + 7);
		SetSizes(sizeButton, sizeImage);

		// load bitmap now that sizes are known by the toolbar control
		bResult = LoadBitmap(lpszBmpResourceName);
	}

	UnlockResource(hGlobal);
	FreeResource(hGlobal);

	return bResult;
}

BOOL CTrueColorToolBarWithCombo::LoadBitmap(LPCTSTR lpszResourceName)
{
	CBitmap bitmap;
	CImageList imageList;

	if (bitmap.LoadBitmap(lpszResourceName) == FALSE)
	{
		return FALSE;
	}

	// 툴바의 크기와 배경색을 설정 한다.
	imageList.Create(16, 15, ILC_COLORDDB | ILC_MASK, 11, 1);
	imageList.Add(&bitmap, RGB(192,192,192));

	SendMessage(TB_SETIMAGELIST, 0, (LPARAM)imageList.m_hImageList);

	imageList.Detach();
	bitmap.Detach();

	return TRUE;
}
