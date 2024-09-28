#pragma once

#include "../../=MsgPack/include/msgpack.hpp"
#include "../String/StringFormat.h"
#include "../Log/LogMan.h"

// 2012-07-12 jgkim
namespace msgpack
{
    //! 2012-07-12 jgkim
    template <typename T>
    bool BufferToObject(const char* Buffer, size_t BufferSize, T& Obj, bool ForNetworkPacket=true)
    {
        try
        {
            if (ForNetworkPacket && BufferSize >= NET_DATA_BUFSIZE)
            {
                sc::writeLogError(
                    sc::string::format(
                        "msgpack BufferToObject packet length over Type %1% %2% >= %3%",
                        typeid(Obj).name(),
                        BufferSize,
                        NET_DATA_BUFSIZE));
                return false;
            }

            msgpack::unpacked UnPack;
            msgpack::unpack(&UnPack, Buffer, BufferSize);
            msgpack::object obj = UnPack.get();
            obj.convert(&Obj);
            return true;
        }
        catch (msgpack::unpack_error& Error)
        {       
            sc::writeLogError(
                sc::string::format(
                    "msgpack BufferToObject unpack_error Type %1% %2%",
                    typeid(Obj).name(),
                    Error.what()));
            return false;
        }
        catch (msgpack::type_error& Error)
        {
            sc::writeLogError(
                sc::string::format(
                    "msgpack BufferToObject type_error Type %1% %2%",
                    typeid(Obj).name(),
                    Error.what()));
            return false;
        }
    }
} // namespace msgpack