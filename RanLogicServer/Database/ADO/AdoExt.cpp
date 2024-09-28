#include "pch.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../EngineLib/Common/ByteStream.h"
#include "AdoExt.h"

#include "../../SigmaCore/DebugInclude.h"

namespace sc
{
namespace db
{
// ----------------------------------------------------------------------------
    AdoExt::AdoExt(PF_PRINT pf)
        : sc::db::CjADO(pf)
    {
    }

    AdoExt::AdoExt(const std::string& csConn, PF_PRINT pf)
        : sc::db::CjADO(csConn, pf)
    {
    }

    AdoExt::AdoExt(LPCTSTR szConn, PF_PRINT pf)
        : sc::db::CjADO(szConn, pf)
    {
    }

    AdoExt::~AdoExt()
    {
    }

	BOOL AdoExt::CreateChunk( _variant_t& vartBLOB, DWORD dwSize )
	{
		try
		{
			if( dwSize == 0 )
				return FALSE;

			SAFEARRAYBOUND	rgsabound	= { dwSize, 0 };
			SAFEARRAY*		pSA			= SafeArrayCreate( VT_UI1, 1, &rgsabound );
			if( !pSA )
			{
				sc::writeLogError(
					sc::string::format(
						"db::MakeChunk SafeArrayCreate %1%",
						dwSize ) );

				return FALSE;
			}

			vartBLOB.vt		= (VT_ARRAY | VT_UI1);
			vartBLOB.parray = pSA;
		}
		catch (_com_error& e)
		{
			eHandler("AdoExt::MakeChunk", e);
		}
		catch (...)
		{
			PRINT(_T("TestBLOB(): Unhandled Exception!"));
		}

		return TRUE;
	}

    // Make a _variant_t variable inldues a Chunk data inside.
    BOOL AdoExt::MakeChunk(
        OUT _variant_t& vartBLOB,
        IN se::ByteStream& bytestream,
        OUT ULONG& nSizeOfData)
    {
        try
        {
            // Src
            LPBYTE pData = NULL;
            bytestream.GetBuffer(pData, nSizeOfData);	//Got dwSizeOfStream

            if (!pData || nSizeOfData <= 0)
            {
                PRINT(_T("MakeChunk(): Invalid ByteStream."));
                return FALSE;
            }

            BOOL bRet = TRUE;

            SAFEARRAY *pSA;
            SAFEARRAYBOUND rgsabound[1];
            rgsabound[0].lLbound = 0;	
            rgsabound[0].cElements = nSizeOfData;

            pSA = SafeArrayCreate(VT_UI1, 1, rgsabound);
            if (!pSA)
            {
                sc::writeLogError(
                    sc::string::format(
                        "db::MakeChunk SafeArrayCreate %1%",
                        nSizeOfData));
                return FALSE;
            }

            // Destination
            void HUGEP* pSafeBuffer = NULL;
            SafeArrayAccessData(pSA, (void **) &pSafeBuffer);
            if (!pSafeBuffer)
            {
                PRINT(_T("MakeChunk(): Invalid pSafeBuffer."));
                bRet = FALSE;
            }
            else
            {
                //
                //ByteStream::GetBuffer() MUST NOT be used for SafeArray.
                // Otherwise, SafeArray's reference count will be screwed and the buffer's destruction 
                // will cause some memory access-violation.
                //
                //bytestream.GetBuffer((LPBYTE &)pSafeBuffer, nSizeOfData);

                memcpy(pSafeBuffer, pData, nSizeOfData);
            }
            
            SafeArrayUnaccessData(pSA);

            vartBLOB.vt = (VT_ARRAY | VT_UI1);
            vartBLOB.parray = pSA;

            if (vartBLOB.parray->rgsabound[0].cElements != nSizeOfData)
            {
                sc::writeLogError(
                    sc::string::format(
                        "MakeChunk size wrong %1% != %2%",
                        vartBLOB.parray->rgsabound[0].cElements,
                        nSizeOfData));
            }

            return bRet;
        }
        catch (_com_error& e)
        {
            eHandler("AdoExt::MakeChunk", e);
        }
        catch (...)
        {
            PRINT(_T("TestBLOB(): Unhandled Exception!"));
        }

        return TRUE;
    }

    bool AdoExt::AppendChunk(const std::string& Column, IN se::ByteStream& ByteStream)
    {        
        try
        {
            const long CHUNK_SIZE = 1024;
            _ParameterPtr pParam;
            LPBYTE pBuffer = NULL;
            DWORD dwSize = 0;
            ByteStream.GetBuffer(pBuffer, dwSize);
            if (!pBuffer)
            {
                _variant_t varBlob;
                varBlob = VT_NULL;
                pParam = m_pCommand->CreateParameter((_bstr_t) Column.c_str(), adLongVarBinary, adParamInput, sizeof(_variant_t));
                pParam->Value = varBlob;
                pParam->Type = adLongVarBinary;
                CHECK_HRESULT(m_pCommand->Parameters->Append(pParam));
                return true;
            }

            pParam = m_pCommand->CreateParameter((_bstr_t) Column.c_str(), adLongVarBinary, adParamInput, dwSize);

            long LeftSize = dwSize;
            while (LeftSize > 0)
            {
                long WriteSize = 0;                
                if (LeftSize > CHUNK_SIZE)
                    WriteSize = CHUNK_SIZE;
                else
                    WriteSize = LeftSize;
            
                VARIANT buff_val;
                BuffToVariant(buff_val, pBuffer+dwSize-LeftSize, WriteSize, Column);
                pParam->AppendChunk(buff_val);
                LeftSize = LeftSize - WriteSize;
            }

            CHECK_HRESULT(m_pCommand->Parameters->Append(pParam));

            return true;
        }
        catch (_com_error& e)
        {
            sc::writeLogError(
                sc::string::format(
                    "AdoExt::AppendChunk %1%", Column));
            eHandler(Column, e);
            return false;
        }
        catch (...)
        {
            PRINT("AdoExt::AppendChunk Unhandled Exception!");
            return false;
        }        
    }

    bool AdoExt::BuffToVariant(VARIANT& var, void* buff, long size, const std::string& Name)
    {        
        try
        {
            // Data
            BYTE* pByte;
            SAFEARRAY FAR* psa;
            SAFEARRAYBOUND rgsabound[1];

            // Init
            rgsabound[0].lLbound = 0; 
            rgsabound[0].cElements = size;

            psa = SafeArrayCreate(VT_I1, 1, rgsabound);
            if(SafeArrayAccessData(psa,(void **)&pByte) == NOERROR)
            {
                memcpy((LPVOID) pByte,(LPVOID) buff, size);
            }
            SafeArrayUnaccessData(psa);

            // return
            var.vt = VT_ARRAY | VT_UI1;
            var.parray = psa;

            return true;
        }
        catch (_com_error& e)
        {
            sc::writeLogError(
                sc::string::format(
                    "AdoExt::BuffToVariant %1%", Name));
            eHandler(Name, e);
            return false;
        }
        catch (...)
        {
            sc::writeLogError(
                sc::string::format(
                    "AdoExt::BuffToVariant %1% Unhandled Exception!",
                    Name));
            return false;
        }
    }

	BOOL		AccessVariantChunk( void** ppBuff, const _variant_t& var )
	{
		if( !var.parray )
			return FALSE;

		void HUGEP*		pSafeBuffer = NULL;					//we support 32bit only so we don't need hugep keyword
		SafeArrayAccessData( var.parray, &pSafeBuffer );
		(*ppBuff) = pSafeBuffer;
		if( !pSafeBuffer )
		{
			sc::writeLogError( "AccessVariantChunk - can not access a data" );
			return FALSE;
		} //if

		return TRUE;
	}

	void		UnaccessVariantChunk( const _variant_t& var )
	{
		if( !var.parray )
			return;

		SafeArrayUnaccessData( var.parray );
	}

// ----------------------------------------------------------------------------
} // namespace db
} // namespace sc