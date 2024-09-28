#pragma once

struct SNATIVEID;
class GLGaeaClient;

namespace LoadingThread
{
    void SetData(
	    const CString& strTextureName,
	    SNATIVEID sMapID,
	    bool bFirst = false,
		bool bOVER15 = false ,
		BOOL bFullSizeLoadingImage = FALSE );

	//void Start();
	//void End();

    void Start_DirectRender( BOOL bShowWinnerCountryFlag );
    void End_DirectRender();
    void FrameMove();
	void Render();
}