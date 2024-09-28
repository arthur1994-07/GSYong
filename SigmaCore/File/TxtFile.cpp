#include "stdafx.h"
#include "../File/FileUtil.h"
#include "TxtFile.h"

// ----------------------------------------------------------------------------
#include "../DebugInclude.h"
// ----------------------------------------------------------------------------

namespace sc
{
// To run agent server from the same folder as the field server
// The reason is a log file is being hogged by field server, while the agent server wants to access that file.


//TxtFile::TxtFile(const std::string& FileName, bool AutoRename, ENCODING Encoding)
//    : m_hFile(INVALID_HANDLE_VALUE)
//{
//	/**
//	*	Whoever it is that is dumb enough to implement a method for checking existing files and renaming 
//	*	them to get the desired filename instead of checking for other available filename
//	*	and use it is a lazy mother fcker and should not be a programmer at all!
//	*/
//	std::string newfcker = FileName;
//	if(sc::file::isExist(FileName))
//	{
//		// designator declaration
//		// WARNING! This loop can lead to a long blocking operation
//		for(UINT i = 1; i < UINT_MAX; i++)
//		{
//			std::string fckyou, fckthis;
//
//			// split file name with the ext
//			static const std::basic_string <char>::size_type urmom = -1;
//			std::basic_string <char>::size_type fckher = FileName.rfind(".");
//			if(urmom == fckher)
//			{
//				fckyou = FileName;
//				fckthis = "";
//			}
//			else
//			{
//				fckyou = FileName.substr(0, fckher);
//				fckthis.assign( FileName, fckher+1, FileName.size()-fckher-1 ); // get that fck en extension, baby!!!
//			}
//
//			// new string concat
//			newfcker = sc::string::format("%s%i.%s", fckyou, i, fckthis);
//			if(!sc::file::isExist(newfcker))
//				break;
//		}
//	}
//	// sc::file::autoRename(FileName);
//	
//    
//    m_hFile = ::CreateFile(
//        newfcker.c_str(),
//        GENERIC_WRITE,
//        FILE_SHARE_READ,
//        NULL,
//        OPEN_ALWAYS,
//        FILE_ATTRIBUTE_NORMAL,
//        NULL);
//    if (m_hFile == INVALID_HANDLE_VALUE)
//    {
//        SAFE_CLOSE_HANDLE(m_hFile);
//    }
//    else
//    {
//        m_strFullFileName = newfcker;
//
//        // Bunch of unicode info at:
//        // http://www.unicode.org/faq/utf_bom.html    
//	    // 00 00 FE FF  UTF-32, big-endian
//	    // FF FE 00 00 	UTF-32, little-endian
//	    // FE FF 		UTF-16, big-endian
//	    // FF FE 		UTF-16, little-endian
//	    // EF BB BF 	UTF-8
//        DWORD dwBytesWritten=0;
//        if (Encoding == UTF8)
//        {	    
//	        unsigned char Mark[3];
//	        Mark[0] = 0xEF;
//	        Mark[1] = 0xBB;
//            Mark[2] = 0xBF;
//	        WriteFile(m_hFile, Mark, 3, &dwBytesWritten, NULL);
//        }
//        else if (Encoding == UTF16LE)
//        {
//            unsigned char Mark[2];
//            Mark[0] = 0xFF;
//            Mark[1] = 0xFE;
//            WriteFile(m_hFile, Mark, 2, &dwBytesWritten, NULL);
//        }
//        else if (Encoding == UTF16BE)
//        {
//            unsigned char Mark[2];
//            Mark[0] = 0xFE;
//            Mark[1] = 0xFF;
//            WriteFile(m_hFile, Mark, 2, &dwBytesWritten, NULL);
//        }
//        else
//        {
//        }
//    }
//}
//
//


TxtFile::TxtFile(const std::string& FileName, bool AutoRename, ENCODING Encoding)
    : m_hFile(INVALID_HANDLE_VALUE)
{
    sc::file::autoRename(FileName);
    
    m_hFile = ::CreateFile(
        FileName.c_str(),
        GENERIC_WRITE,
        FILE_SHARE_READ,
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    if (m_hFile == INVALID_HANDLE_VALUE)
    {
        SAFE_CLOSE_HANDLE(m_hFile);
    }
    else
    {
        m_strFullFileName = FileName;

        // Bunch of unicode info at:
        // http://www.unicode.org/faq/utf_bom.html    
	    // 00 00 FE FF  UTF-32, big-endian
	    // FF FE 00 00 	UTF-32, little-endian
	    // FE FF 		UTF-16, big-endian
	    // FF FE 		UTF-16, little-endian
	    // EF BB BF 	UTF-8
        DWORD dwBytesWritten=0;
        if (Encoding == UTF8)
        {	    
	        unsigned char Mark[3];
	        Mark[0] = 0xEF;
	        Mark[1] = 0xBB;
            Mark[2] = 0xBF;
	        WriteFile(m_hFile, Mark, 3, &dwBytesWritten, NULL);
        }
        else if (Encoding == UTF16LE)
        {
            unsigned char Mark[2];
            Mark[0] = 0xFF;
            Mark[1] = 0xFE;
            WriteFile(m_hFile, Mark, 2, &dwBytesWritten, NULL);
        }
        else if (Encoding == UTF16BE)
        {
            unsigned char Mark[2];
            Mark[0] = 0xFE;
            Mark[1] = 0xFF;
            WriteFile(m_hFile, Mark, 2, &dwBytesWritten, NULL);
        }
        else
        {
        }
    }
}

TxtFile::~TxtFile()
{
    if (m_hFile != INVALID_HANDLE_VALUE)
    {
        SAFE_CLOSE_HANDLE(m_hFile);
    }
}

void TxtFile::write(const std::string& Msg, bool NewLine)
{    
    if (m_hFile != INVALID_HANDLE_VALUE)
    {        
        DWORD dwWritten = 0;
        ::WriteFile(m_hFile, Msg.c_str(), (DWORD) Msg.size(), &dwWritten, NULL);
        if (NewLine)
        {
            std::string NewLine("\r\n");
            ::WriteFile(m_hFile, NewLine.c_str(), (DWORD) NewLine.size(), &dwWritten, NULL);
        }
    }
}

void TxtFile::write(const std::wstring& Msg, bool NewLine)
{
    if (m_hFile != INVALID_HANDLE_VALUE)
    {        
        DWORD dwWritten = 0;
        ::WriteFile(m_hFile, Msg.c_str(), (DWORD) sizeof(wchar_t)*Msg.size(), &dwWritten, NULL);
        if (NewLine)
        {
            std::wstring NewLine(L"\r\n");
            ::WriteFile(m_hFile, NewLine.c_str(), (DWORD) sizeof(wchar_t)*NewLine.size(), &dwWritten, NULL);
        }
    }
}

} // namespace sc