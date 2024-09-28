template<class T>
void CServerTriggerInstance::SendMsgMySelf(const DWORD dwCharDbNum, const EMNET_MSG dwMsgType, const T& NetMsg)
{
    GLLandMan* pLandMan = GetLandMan();

    if (pLandMan)
    {
        msgpack::sbuffer buf;
        msgpack::pack(buf, NetMsg);
        pLandMan->SendMsgMySelf(dwCharDbNum, dwMsgType, buf);
    }
}

template<class T>
void CServerTriggerInstance::SendMsgClient(const DWORD dwCharDbNum, const EMNET_MSG dwMsgType, const T& NetMsg)
{
    GLLandMan* pLandMan = GetLandMan();

    if (pLandMan)
    {
        msgpack::sbuffer buf;
        msgpack::pack(buf, NetMsg);

        if (dwCharDbNum == 0)
        {
            pLandMan->SendMsgPC(dwMsgType, buf);
        }
        else
        {
            pLandMan->SendMsgPC(dwCharDbNum, dwMsgType, buf);
        }
    }
}

template<class T>
void CServerTriggerInstance::SendMsgAgent(const EMNET_MSG dwMsgType, const T& NetMsg)
{
    GLLandMan* pLandMan = GetLandMan();

    if (pLandMan)
    {
        msgpack::sbuffer buf;
        msgpack::pack(buf, NetMsg);
        pLandMan->SendMsgAgent(dwMsgType, buf);
    }
}