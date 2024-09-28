#ifndef SC_COLOR_CONSOLE_H_
#define SC_COLOR_CONSOLE_H_

//! Original Code from Jaded Hobo
//! http://www.codeproject.com/KB/cpp/AddColorConsole.aspx
//! Modify By Jgkim
//! - Unicode support
//! - Language codepage support
//! History
//! 2008-12-03 Jgkim 

#include <iostream>
#include <iomanip>

#ifndef _UNICODE
typedef std::ostream TOSTREAM;
#define _CONSOLE_CODE_PAGE(str) std::cout.imbue( std::locale(str) )
#else
typedef std::wostream TOSTREAM;
#define _CONSOLE_CODE_PAGE(str) std::wcout.imbue( std::locale(str) )
#endif

namespace sc {
    namespace console {
        static const WORD bgMask( BACKGROUND_BLUE      | 
            BACKGROUND_GREEN     | 
            BACKGROUND_RED       | 
            BACKGROUND_INTENSITY   );
        static const WORD fgMask( FOREGROUND_BLUE      | 
            FOREGROUND_GREEN     | 
            FOREGROUND_RED       | 
            FOREGROUND_INTENSITY   );

        static const WORD fgBlack    ( 0 ); 
        static const WORD fgLoRed    ( FOREGROUND_RED   ); 
        static const WORD fgLoGreen  ( FOREGROUND_GREEN ); 
        static const WORD fgLoBlue   ( FOREGROUND_BLUE  ); 
        static const WORD fgLoCyan   ( fgLoGreen   | fgLoBlue ); 
        static const WORD fgLoMagenta( fgLoRed     | fgLoBlue ); 
        static const WORD fgLoYellow ( fgLoRed     | fgLoGreen ); 
        static const WORD fgLoWhite  ( fgLoRed     | fgLoGreen | fgLoBlue ); 
        static const WORD fgGray     ( fgBlack     | FOREGROUND_INTENSITY ); 
        static const WORD fgHiWhite  ( fgLoWhite   | FOREGROUND_INTENSITY ); 
        static const WORD fgHiBlue   ( fgLoBlue    | FOREGROUND_INTENSITY ); 
        static const WORD fgHiGreen  ( fgLoGreen   | FOREGROUND_INTENSITY ); 
        static const WORD fgHiRed    ( fgLoRed     | FOREGROUND_INTENSITY ); 
        static const WORD fgHiCyan   ( fgLoCyan    | FOREGROUND_INTENSITY ); 
        static const WORD fgHiMagenta( fgLoMagenta | FOREGROUND_INTENSITY ); 
        static const WORD fgHiYellow ( fgLoYellow  | FOREGROUND_INTENSITY );
        static const WORD bgBlack    ( 0 ); 
        static const WORD bgLoRed    ( BACKGROUND_RED   ); 
        static const WORD bgLoGreen  ( BACKGROUND_GREEN ); 
        static const WORD bgLoBlue   ( BACKGROUND_BLUE  ); 
        static const WORD bgLoCyan   ( bgLoGreen   | bgLoBlue ); 
        static const WORD bgLoMagenta( bgLoRed     | bgLoBlue ); 
        static const WORD bgLoYellow ( bgLoRed     | bgLoGreen ); 
        static const WORD bgLoWhite  ( bgLoRed     | bgLoGreen | bgLoBlue ); 
        static const WORD bgGray     ( bgBlack     | BACKGROUND_INTENSITY ); 
        static const WORD bgHiWhite  ( bgLoWhite   | BACKGROUND_INTENSITY ); 
        static const WORD bgHiBlue   ( bgLoBlue    | BACKGROUND_INTENSITY ); 
        static const WORD bgHiGreen  ( bgLoGreen   | BACKGROUND_INTENSITY ); 
        static const WORD bgHiRed    ( bgLoRed     | BACKGROUND_INTENSITY ); 
        static const WORD bgHiCyan   ( bgLoCyan    | BACKGROUND_INTENSITY ); 
        static const WORD bgHiMagenta( bgLoMagenta | BACKGROUND_INTENSITY ); 
        static const WORD bgHiYellow ( bgLoYellow  | BACKGROUND_INTENSITY );

        static class con_dev
        {
        private:
            HANDLE                      hCon;
            DWORD                       cCharsWritten; 
            CONSOLE_SCREEN_BUFFER_INFO  csbi; 
            DWORD                       dwConSize;

        public:
            con_dev() 
            { 
                hCon = GetStdHandle( STD_OUTPUT_HANDLE );
            }
        private:
            void GetInfo()
            {
                GetConsoleScreenBufferInfo( hCon, &csbi );
                dwConSize = csbi.dwSize.X * csbi.dwSize.Y; 
            }
        public:
            void Clear()
            {
                COORD coordScreen = { 0, 0 };

                GetInfo(); 
                FillConsoleOutputCharacter( hCon, TEXT(' '),
                    dwConSize, 
                    coordScreen,
                    &cCharsWritten ); 
                GetInfo(); 
                FillConsoleOutputAttribute( hCon,
                    csbi.wAttributes,
                    dwConSize,
                    coordScreen,
                    &cCharsWritten ); 
                SetConsoleCursorPosition( hCon, coordScreen ); 
            }
            void SetColor( WORD wRGBI, WORD Mask )
            {
                GetInfo();
                csbi.wAttributes &= Mask; 
                csbi.wAttributes |= wRGBI; 
                SetConsoleTextAttribute( hCon, csbi.wAttributes );
            }
        } console;

        inline TOSTREAM& clr( TOSTREAM& os )
        {
            os.flush();
            console.Clear();
            return os;
        };

        inline TOSTREAM& fg_red( TOSTREAM& os )
        {
            os.flush();
            console.SetColor( fgHiRed, bgMask );

            return os;
        }

        inline TOSTREAM& fg_green( TOSTREAM& os )
        {
            os.flush();
            console.SetColor( fgHiGreen, bgMask );

            return os;
        }

        inline TOSTREAM& fg_blue( TOSTREAM& os )
        {
            os.flush();
            console.SetColor( fgHiBlue, bgMask );

            return os;
        }

        inline TOSTREAM& fg_white( TOSTREAM& os )
        {
            os.flush();
            console.SetColor( fgHiWhite, bgMask );

            return os;
        }

        inline TOSTREAM& fg_cyan( TOSTREAM& os )
        {
            os.flush();
            console.SetColor( fgHiCyan, bgMask );

            return os;
        }

        inline TOSTREAM& fg_magenta( TOSTREAM& os )
        {
            os.flush();
            console.SetColor( fgHiMagenta, bgMask );

            return os;
        }

        inline TOSTREAM& fg_yellow( TOSTREAM& os )
        {
            os.flush();
            console.SetColor( fgHiYellow, bgMask );

            return os;
        }

        inline TOSTREAM& fg_black( TOSTREAM& os )
        {
            os.flush();
            console.SetColor( fgBlack, bgMask );

            return os;
        }

        inline TOSTREAM& fg_gray( TOSTREAM& os )
        {
            os.flush();
            console.SetColor( fgGray, bgMask );

            return os;
        }

        inline TOSTREAM& bg_red( TOSTREAM& os )
        {
            os.flush();
            console.SetColor( bgHiRed, fgMask );

            return os;
        }

        inline TOSTREAM& bg_green( TOSTREAM& os )
        {
            os.flush();
            console.SetColor( bgHiGreen, fgMask );

            return os;
        }

        inline TOSTREAM& bg_blue( TOSTREAM& os )
        {
            os.flush();
            console.SetColor( bgHiBlue, fgMask );

            return os;
        }

        inline TOSTREAM& bg_white( TOSTREAM& os )
        {
            os.flush();
            console.SetColor( bgHiWhite, fgMask );

            return os;
        }

        inline TOSTREAM& bg_cyan( TOSTREAM& os )
        {
            os.flush();
            console.SetColor( bgHiCyan, fgMask );

            return os;
        }

        inline TOSTREAM& bg_magenta( TOSTREAM& os )
        {
            os.flush();
            console.SetColor( bgHiMagenta, fgMask );

            return os;
        }

        inline TOSTREAM& bg_yellow( TOSTREAM& os )
        {
            os.flush();
            console.SetColor( bgHiYellow, fgMask );

            return os;
        }

        inline TOSTREAM& bg_black( TOSTREAM& os )
        {
            os.flush();
            console.SetColor( bgBlack, fgMask );

            return os;
        }

        inline TOSTREAM& bg_gray( TOSTREAM& os )
        {
            os.flush();
            console.SetColor( bgGray, fgMask );

            return os;
        }

    } // namespace CORE_COMMON
} // namespace COLOR_CONSOLE
//------------------------------------------------------------------------------
#endif // SC_COLOR_CONSOLE_H_

