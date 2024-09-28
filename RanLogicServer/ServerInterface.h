class GLServerInterFace
{
	virtual void SENDTOCLIENT(NET_MSG_GENERIC* nmg, DWORD dwChaNum) = 0;	//mjeon.AF
	virtual void SENDTOCLIENT(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, DWORD dwChaNum, bool UseCrc=false) = 0;

	virtual void SENDTOCLIENT(DWORD dwClientID, NET_MSG_GENERIC* nmg) = 0;
	virtual void SENDTOCLIENT(DWORD dwClientID, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) = 0;

	virtual void SENDTOFIELD(DWORD dwClientID, NET_MSG_GENERIC* nmg) = 0;
	virtual void SENDTOFIELD(DWORD dwClientID, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) = 0;

	virtual void SENDTOFIELD(DWORD dwChannel, DWORD dwFieldSvrNum, NET_MSG_GENERIC* nmg) = 0;	//mjeon.SendField를 사용하도록 구현 : 메시지 압축을 하지 않는다.
	virtual void SENDTOFIELD(DWORD dwChannel, DWORD dwFieldSvrNum, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) = 0;

	virtual void SENDTOFIELDSVR(int nChannel, int nField, NET_MSG_GENERIC* nmg) = 0; //SendClient를 사용: 메시지에 대해 압축을 수행한다.
	virtual void SENDTOFIELDSVR(int nChannel, int nField, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) = 0;

	virtual void SENDTOCHANNEL(NET_MSG_GENERIC* nmg, int nChannel) = 0;
	virtual void SENDTOCHANNEL(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, int nChannel, bool UseCrc=false) = 0;

	virtual int SENDTOALLCHANNEL(NET_MSG_GENERIC* nmg) = 0;
	virtual int SENDTOALLCHANNEL(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) = 0;

	virtual void SENDTOALLCLIENT(NET_MSG_GENERIC* nmg) = 0;
	virtual void SENDTOALLCLIENT(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) = 0;

	virtual void SENDTOALLCLIENT(NET_MSG_GENERIC* nmg, int nChannel) = 0;
	virtual void SENDTOALLCLIENT(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, int nChannel, bool UseCrc=false) = 0;

	virtual void SENDTOALLIANCECLIENT(DWORD dwClubID, NET_MSG_GENERIC* nmg) = 0;
	virtual void SENDTOALLIANCECLIENT(DWORD dwClubID, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) = 0;

	virtual void SENDTOCACHE(NET_MSG_GENERIC* pMsg) = 0;
	virtual void SENDTOCACHE(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) = 0;

	virtual void SENDCLUBLOG(DWORD ClubDbNum, club::EM_LOG_TYPE LogType, const std::string& LogMsg) = 0;

	virtual void SENDTOMATCH(NET_MSG_GENERIC *msg) = 0;
	virtual void SENDTOMATCH(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) = 0;
	virtual void SENDTOMYSELF(NET_MSG_GENERIC *msg) = 0;
	virtual void SENDTOMYSELF(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) = 0;

}