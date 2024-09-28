

#ifndef _mrcString_h_
#define _mrcString_h_

#include "mrcConfig.h"
#include "mrcUtil/mrcFileSystem.h"


#if defined(_WIN32) || defined(_WIN64)
	#include <tchar.h>
#else
	#ifdef _UNICODE
		#define _tcslen wcslen
		#define _tcscmp wcscmp
		#define _stprintf swprintf

	#else
		#define _tcslen strlen		
		#define _tcscmp strcmp
		#define _stprintf sprintf

	#endif

#endif 


namespace mrc
{


#define StdStringT(T) std::basic_string<T, std::char_traits<T>, std::allocator<T> >	
#define CustomMemoryStringT(T) std::basic_string<T, std::char_traits<T>, boost::pool_allocator<T> >	

	template<typename T>
	bool operator <(const CustomMemoryStringT(T)& l,const StdStringT(T)& o)
	{
		return l.compare(0,l.length(),o.c_str(),o.length())<0;
	}

	template<typename T>
	bool operator <(const StdStringT(T)& l,const CustomMemoryStringT(T)& o)
	{
		return l.compare(0,l.length(),o.c_str(),o.length())<0;
	}

	template<typename T>
	bool operator <=(const CustomMemoryStringT(T)& l,const StdStringT(T)& o)
	{
		return l.compare(0,l.length(),o.c_str(),o.length())<=0;
	}

	template<typename T>
	bool operator <=(const StdStringT(T)& l,const CustomMemoryStringT(T)& o)
	{
		return l.compare(0,l.length(),o.c_str(),o.length())<=0;
	}

	template<typename T>
	bool operator >(const CustomMemoryStringT(T)& l,const StdStringT(T)& o)
	{
		return l.compare(0,l.length(),o.c_str(),o.length())>0;
	}

	template<typename T>
	bool operator >(const StdStringT(T)& l,const CustomMemoryStringT(T)& o)
	{
		return l.compare(0,l.length(),o.c_str(),o.length())>0;
	}

	template<typename T>
	bool operator >=(const CustomMemoryStringT(T)& l,const StdStringT(T)& o)
	{
		return l.compare(0,l.length(),o.c_str(),o.length())>=0;
	}

	template<typename T>
	bool operator >=(const StdStringT(T)& l,const CustomMemoryStringT(T)& o)
	{
		return l.compare(0,l.length(),o.c_str(),o.length())>=0;
	}

	template<typename T>
	bool operator ==(const CustomMemoryStringT(T)& l,const StdStringT(T)& o)
	{
		return l.compare(0,l.length(),o.c_str(),o.length())==0;
	}

	template<typename T>
	bool operator ==(const StdStringT(T)& l,const CustomMemoryStringT(T)& o)
	{
		return l.compare(0,l.length(),o.c_str(),o.length())==0;
	}
	template<typename T>

	bool operator !=(const CustomMemoryStringT(T)& l,const StdStringT(T)& o)
	{
		return l.compare(0,l.length(),o.c_str(),o.length())!=0;
	}

	template<typename T>
	bool operator !=(const StdStringT(T)& l,const CustomMemoryStringT(T)& o)
	{
		return l.compare(0,l.length(),o.c_str(),o.length())!=0;
	}

	template<typename T>
	CustomMemoryStringT(T) operator +=(const CustomMemoryStringT(T)& l,const StdStringT(T)& o)
	{
		return CustomMemoryStringT(T)(l)+=o.c_str();
	}

	template<typename T>
	CustomMemoryStringT(T) operator +=(const StdStringT(T)& l,const CustomMemoryStringT(T)& o)
	{
		return CustomMemoryStringT(T)(l.c_str())+=o.c_str();
	}

	template<typename T>
	CustomMemoryStringT(T) operator +(const CustomMemoryStringT(T)& l,const StdStringT(T)& o)
	{
		return CustomMemoryStringT(T)(l)+=o.c_str();
	}

	template<typename T>
	CustomMemoryStringT(T) operator +(const StdStringT(T)& l,const CustomMemoryStringT(T)& o)
	{
		return CustomMemoryStringT(T)(l.c_str())+=o.c_str();
	}

	template<typename T>
	CustomMemoryStringT(T) operator +(const T* l,const CustomMemoryStringT(T)& o)
	{
		return CustomMemoryStringT(T)(l)+=o;
	}


	#undef StdStringT
	#undef CustomMemoryStringT

// 	typedef _StringBase		  String;
// 	typedef _StringStreamBase StringStream;
	typedef StringStream	  stringstream;



//	WString toUtf16String(const NString& utf8string);
//	NString ToUtf8String(const WString& widestring);
	String  toString(real value);

	WString toWString( const NString& value );
	inline WString toWString( const WString& value ) { return value; }

	NString toNString( const WString& value );
	inline NString toNString(const NString& value) { return value; }

	WString toWString( const filesystem::Path& value );
	NString toNString( const filesystem::Path& value );
	String  toString( const filesystem::Path& value );


	String  toString( const char*		  value );
	String  toString( const wchar_t*      value );

	WString toWString( const wchar_t*   value );
	NString toNString( const char*      value );

	NString toNString( const wchar_t*   value );


	String  toString(const NString& value);
	String  toString(const WString& value);

	String  toString(int32 value);
	String  toString(int64 value);



	WString toWString(int32 value);
	NString toNString(int32 value);

// 	WString toWString(int64 value);
// 	NString toNString(int64 value);


	inline String  toString(const NString& value)
	{
#ifdef _UNICODE
		return toWString(value);
#else
		return value;
#endif
	}

	inline String  toString(const WString& value)
	{
#ifdef _UNICODE
		return value;
#else
		return toNString(value);
#endif
	}


}







#endif//////////////////////////////////////////////////////////////////////////