#pragma	once

namespace ic
{
    class IDxSound
    {
    public:
        static VOID SetInstance( IDxSound* pInstance );
        static IDxSound* GetInstance();

    public:
	    virtual bool CreateSound(
            const TSTRING& strKeyword,
            const TSTRING& strFileName,
            int nVolType,
            bool bLoop = false ) = 0;

        virtual bool ReleaseSound( const TSTRING& strKeyword ) = 0;
        virtual bool ReleaseAllSound() = 0;

        virtual bool PlaySound( const TSTRING& strKeyword ) = 0;
	    virtual bool StopSound( const TSTRING& strKeyword ) = 0;

        virtual bool IsPlaying( const TSTRING& strKeyword ) = 0;
    };
}