#include "StdAfx.h"


#include "mrcInit.h"


namespace mrcTest
{
	static bool g_gamePlay = true;

	LRESULT WINAPI MyMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		static bool Keyboardkey[255] = {false,};
		try
		{
			switch( msg )
			{
			case WM_DESTROY:
				PostQuitMessage(0);
				break;

			case WM_MOUSEMOVE:
			case WM_LBUTTONDOWN:  
			case WM_LBUTTONUP:
			case WM_LBUTTONDBLCLK:
			case WM_RBUTTONDOWN:  
			case WM_RBUTTONUP:
				{
					float RealSize = 1;
					POINT pt = { LONG(short(LOWORD(lParam)) / RealSize), LONG(short(HIWORD(lParam)) / RealSize) };
					lParam = MAKELONG(pt.x,pt.y);

					mrc::messageProcedureLayer(mrc::Message(hWnd,msg,wParam,lParam));
					//					LayerMgr::getInstance().messageProcedure( Message(hWnd,msg,wParam,lParam) );
					break;
				}

			case WM_CHAR:
				{
					// 					if( Laputan::Utility::Is_WideChar_For_WM_CHAR((int)wParam) )
					// 						return 1;
					// 
					// 					Laputan::MsgProc(hWnd,msg,wParam,lParam);
					break;
				}
			case WM_KEYUP:
				{
					switch(wParam)
					{
					case VK_SPACE:
						{
							break;
						}
					case VK_ESCAPE:
					case VK_F1:
					case VK_F2:
					case VK_F3:
					case VK_F4:
					case VK_F5:
					case VK_F6:
					case VK_F7:
					case VK_F8:
					case VK_F9:
					case VK_F10:
					case VK_F11:
					case VK_F12:
						break;
					default:
						{
							mrc::messageProcedureLayer(mrc::Message(hWnd,msg,wParam,lParam));
							break;
						}
					}

					break;
				}

			case WM_KEYDOWN:
				{
					switch(wParam)
					{
					case VK_SPACE:
						{
							break;
						}
					case VK_ESCAPE:
						{
							PostQuitMessage(0);
							break;
						}
					case VK_F1:
						{
							break;
						}
					case VK_F2:
						{
							break;
						}
					case VK_F3:
						{
							break;
						}
					case VK_F5:
						{
							g_gamePlay = !g_gamePlay;
							break;
						}
					case VK_F6:
						{
							mrc::updateLayer((mrc::real)0.017);
							break;
						}
					case VK_F7:
						{
							break;
						}
					case VK_F8:
						{
							mrc::resetAll();
							break;
						}
					case VK_F9:
						{
							break;
						}
					case VK_F10:
						{

							break;
						}


					default:
						{ 
							mrc::messageProcedureLayer(mrc::Message(hWnd,msg,wParam,lParam));
							//////////////////////////////////////////////////////////////////////////
							break;
						}
					}



					// 밑에 메세지를 넘긴다
					//					break;
				}

			case WM_INPUTLANGCHANGE:
			case WM_IME_SETCONTEXT:
			case WM_IME_STARTCOMPOSITION:
			case WM_IME_COMPOSITION:
			case WM_IME_ENDCOMPOSITION:
			case WM_IME_NOTIFY:
				{
					break;
				}

			}
		}
		catch (const mrc::Exception& )
		{
		}

		return DefWindowProc( hWnd, msg, wParam, lParam );
	}

	bool isPlay()
	{
		return g_gamePlay;
	}

}

