#pragma once

//#include <vector>
#include "UIDataType.h"

namespace GUIBase
{
	/// Image Data Á¤º¸;
	struct UI_IMAGE_DATA
	{
		/// Basic Image Data
		int					imgWidth;
		int					imgHeight;
		DWORD*				imgData;
		POINT				viewSize;

		/// Scroll Image Data
		RECT				scrollRect;
		POINT				scrollOffset;

		/// Update Position Data
		RECT				updateRectBounds;
		UIRECT*				updateRects;
		UINT				updateRectsCount;
		//std::vector<UIRECT>	updateRects;

		UI_IMAGE_DATA()
		{
			imgWidth = 0;
			imgHeight = 0;
			imgData = NULL;

			viewSize.x = 0;
			viewSize.y = 0;
			SetRect( &scrollRect, 0, 0, 0, 0 );
			scrollOffset.x = 0;
			scrollOffset.y = 0;
			SetRect( &updateRectBounds, 0, 0, 0, 0 );
			updateRects = NULL;
			updateRectsCount = 0;
		}
		UI_IMAGE_DATA( int width, int height, void* data )
		{
			imgWidth = width;
			imgHeight = height;
			imgData = new DWORD[ width*height ];
			memcpy( imgData, data, width*height*4 );

			viewSize.x = 0;
			viewSize.y = 0;
			SetRect( &scrollRect, 0, 0, 0, 0 );
			scrollOffset.x = 0;
			scrollOffset.y = 0;
			SetRect( &updateRectBounds, 0, 0, 0, 0 );
			updateRects = NULL;
			updateRectsCount = 0;
		}
		~UI_IMAGE_DATA()
		{
			if( imgData )
			{
				delete[] imgData;
				imgData = NULL;
			}

			if( updateRects )
			{
				delete[] updateRects;
				updateRects = NULL;
			}
		}
	};

}