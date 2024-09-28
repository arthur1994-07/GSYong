#include "pch.h"

#include "./SquirrelMan.h"

#include "../../SigmaCore/gassert.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/Util/SystemInfo.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringUtil.h"


// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace
{
	#define ENCRYPT_KEY_LENGTH 16

	static unsigned char g_Key[] = {0x12, 0x34, 0x56, 0x78,
									0x9A, 0xBC, 0xDE, 0x0F,
									0xF0, 0xE1, 0xD2, 0xC3,
									0xB4, 0xA5, 0x96, 0x87};


	void Encrypt (unsigned char *pBuffer, unsigned int nLength)
	{
		unsigned int nCount = 0;

		for (nCount = 0; nCount < nLength; nCount++)
		{
			pBuffer[nCount] ^= g_Key[nCount % ENCRYPT_KEY_LENGTH];
		}
	}

}

using namespace Sqrat;


SquirrelMan::SquirrelMan()
{
	m_vm = sq_open(SQ_DEFAULT_STACK_SIZE); //creates a VM with initial stack size 1024

	sq_pushroottable(m_vm);

	
	sqstd_register_systemlib(m_vm);
	sqstd_register_mathlib(m_vm);

	
	DefaultVM::Set(m_vm);
	

	sq_setcompilererrorhandler(m_vm, &SqPrintCompileError);	
	sq_newclosure(m_vm, &SqPrintErrorMsg,0);	
	sq_seterrorhandler(m_vm);
	sq_setprintfunc(m_vm, SqPrintMsg, SqPrintMsg);
}


SquirrelMan::~SquirrelMan()
{
	sq_close(m_vm);	
}

// TAG_InitClientTriggerSystem_2
// TAG_InitClientTriggerSystem_7b_1
void SquirrelMan::LoadScript(const char *szFilePath)
{
	if (!szFilePath)
	{
		sc::writeLogError(std::string("SquirrelMan::LoadScript() - Script FileName is NULL."));
		return;
	}

	try
	{
        std::string strScriptPath( sc::getAppPath() );
        ::SetCurrentDirectory(strScriptPath.c_str());

		Script script1(m_vm);

		std::string path = szFilePath;
		GASSERT( path.size() >= 5 );

		std::string ext( path.end() - 3 , path.end());

		if( ext == "nut")
		{
			script1.CompileFile(szFilePath);
		}
		else if( ext == "nuc" )
		{
			FILE* pNucFile = fopen(szFilePath,"rb");
			GASSERT_MSG(pNucFile, path + " file failed open");

			fseek( pNucFile, 0, SEEK_END );
			size_t fileSize = ftell( pNucFile );
			fseek( pNucFile, 0, SEEK_SET );

			std::vector<unsigned char> buf;
			buf.resize(fileSize);
			size_t readSize = fread(&buf[0],1,buf.size(),pNucFile);
			GASSERT(readSize == buf.size());
			fclose(pNucFile);

			Encrypt(&buf[0],(unsigned int)buf.size());
			script1.CompileString( std::string(buf.begin(),buf.end()));
		}
		else
		{
			GASSERT(!"SquirrelMan::LoadScript() failed to decrypt .nuc.");
		}
			
		
		script1.Run();

        m_strFilePath = szFilePath;

		ToDo();

		m_bLoaded = TRUE;
	}
	catch(const Exception& e) 
	{	
		m_strError = e.Message();
	}
}


//
// Print Message Handlers
//
void SqPrintCompileError(HSQUIRRELVM,const SQChar * desc,const SQChar * source,SQInteger line,SQInteger column)
{
	printf("%s - file(%s) line(%d) column(%d) \n", desc, source, line, column);
}


void SqPrintMsg(HSQUIRRELVM v,const SQChar *s,...)
{
	static SQChar msgBuffer[1024];

	va_list vl;

	va_start(vl,s);

	scvsprintf(msgBuffer, s, vl);

	_putts(msgBuffer);
	sc::writeLogDebug(std::string(msgBuffer));

	va_end(vl);
}


SQInteger SqPrintErrorMsg(HSQUIRRELVM v) 
{
	using namespace Sqrat;

	SQPRINTFUNCTION pf = sq_geterrorfunc(v);

	if(pf)
	{
		const SQChar *sErr = 0;

		if( sq_gettop(v) >= 1 )
		{
			if( SQ_SUCCEEDED(sq_getstring(v,2,&sErr)) )
			{
				pf(v, _SC("\nSquirrel Error : [%s]\n"), sErr);
			}
			else
			{
				pf(v, _SC("\\nSquirrel Error : [%s]\n"));
			}
		}
	}

	sqstd_printcallstack(v);

	return 0;
}

