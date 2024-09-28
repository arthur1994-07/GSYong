#pragma once

#include "../../SigmaCore/Database/Ado/AdoClass.h"

namespace se
{
    class ByteStream;
}

namespace sc
{

namespace db
{
    class AdoExt : public sc::db::CjADO
    {
    public:
        AdoExt(PF_PRINT pf=NULL);
        AdoExt(const std::string& csConn, PF_PRINT pf=NULL);
        AdoExt(LPCTSTR szConn, PF_PRINT pf=NULL);
        virtual ~AdoExt();

        // Make a _variant_t variable inldues a Chunk data inside.
		BOOL	CreateChunk( _variant_t& vartBLOB, DWORD dwSize );
        BOOL	MakeChunk(OUT _variant_t &vartBLOB, IN se::ByteStream& bytestream, OUT ULONG &nSizeOfData);

        bool AppendChunk(const std::string& Column, IN se::ByteStream& ByteStream);

    protected:
        bool BuffToVariant(VARIANT& var, void* buff, long size, const std::string& Name);
    };

	BOOL		AccessVariantChunk( void** ppBuff, const _variant_t& var );
	void		UnaccessVariantChunk( const _variant_t& var );
} // namespace db

} // namespace sc