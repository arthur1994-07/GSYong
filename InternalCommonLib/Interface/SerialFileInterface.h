#pragma once

namespace ic
{
    enum FOPENTYPE
    {
        FOT_READ,
        FOT_WRITE
    };


    class SerialFileInterface
    {
    public:
        virtual BOOL OpenFile(const ic::FOPENTYPE _Type, const char* _szFileName, bool bencode=false) = 0;
        virtual void CloseFile() = 0;
        virtual BOOL SetOffSet(long _OffSet) = 0;
        virtual long GetfTell() = 0;
        virtual BOOL BeginBlock() = 0;
	    virtual BOOL EndBlock() = 0;

        // ------- 저장하기.
    public:
	    virtual BOOL operator << (short Value) = 0;
	    virtual BOOL operator << (int Value) = 0;
	    virtual BOOL operator << (WORD Value) = 0;
	    virtual BOOL operator << (DWORD Value) = 0;
	    virtual BOOL operator << (UINT Value) = 0;
	    virtual BOOL operator << (float Value) = 0;
	    virtual BOOL operator << (double Value) = 0;	
	    virtual BOOL operator << (BYTE Value) = 0;
	    virtual BOOL operator << (char Value) = 0;
	    virtual BOOL operator << (bool Value) = 0;
	    virtual BOOL operator << (const std::string& str) = 0;
	    virtual BOOL operator << (LONGLONG Value) = 0;
        virtual BOOL operator << (const D3DVECTOR& Value) = 0;
        virtual BOOL operator << (const D3DCOLORVALUE& Value) = 0;
        virtual BOOL operator << (const D3DBLEND& Value) = 0;

        // ------- 불러오기.
    public:
	    virtual BOOL operator >> (short& Value) = 0;
	    virtual BOOL operator >> (int& Value) = 0;
	    virtual BOOL operator >> (WORD& Value) = 0;
	    virtual BOOL operator >> (DWORD& Value) = 0;
	    virtual BOOL operator >> (UINT& Value) = 0;
	    virtual BOOL operator >> (float& Value) = 0;
	    virtual BOOL operator >> (double& Value) = 0;
	    virtual BOOL operator >> (BYTE& Value) = 0;
	    virtual BOOL operator >> (char& Value) = 0;
	    virtual BOOL operator >> (std::string& str) = 0;
	    virtual BOOL operator >> (bool& Value) = 0;
	    virtual BOOL operator >> (LONGLONG& Value) = 0;
        virtual BOOL operator >> (D3DVECTOR &Value) = 0;
        virtual BOOL operator >> (D3DCOLORVALUE &Value) = 0;
        virtual BOOL operator >> (D3DBLEND &Value) = 0;

    public:
        SerialFileInterface() {}
        virtual ~SerialFileInterface() {}
    };


    typedef std::auto_ptr<ic::SerialFileInterface> (*FUNC_SERIAL_FILE)();
    extern FUNC_SERIAL_FILE g_funcSerialFile;
}
