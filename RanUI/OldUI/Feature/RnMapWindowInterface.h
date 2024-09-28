
#pragma once

#include "../RanLogic/Party/GLPartyDefine.h"
class GLLevelFileClient;
struct NET_MSG_GENERIC; 

class RnMapWindowInterface
{
public:

	virtual ~RnMapWindowInterface() {}


	// 기존 맵의 인터페이스
	virtual void	VISIBLE_TARGET_POS( BOOL bVisible ) = 0;
	virtual void	SetMapLevelFile(const GLLevelFileClient& levFile, const CString& strMapName ) = 0;

    virtual void	MsgMapSearchNameList(const std::vector<std::pair<int, std::string> >& nameList) {}
	virtual void	MsgMapSearchNameDetailInfo(int level,const std::string& charName,const std::string& title,int school,int emClass,
		bool isParty,const std::string& clubName,DWORD dwGaeaID) {}
	virtual void	MsgMapSearchNamePosition(int level,const std::string& charName,DWORD mapID,const D3DXVECTOR3& pos) {}

	virtual void MsgMapPartyInfoListBoard(bool isExistMyBoard,DWORD mapID,
		const std::vector<std::string>& NameVec,const std::vector<DWORD>& MaxPlayerVec,
		const std::vector<DWORD>& CurrentPlayerVec,const std::vector<std::string>& TitleVec) {}

	virtual void MsgMapPartyInfoDetail(DWORD mapID,const std::vector<GLPARTY_LINK_MEM>& memberVec) {}
	
    virtual void MsgMapGuidanceAuthInfo(const NET_MSG_GENERIC* _pMessage) {}
	virtual void MsgMapCTIAuthInfo ( const NET_MSG_GENERIC* pMsg ) { }
};

