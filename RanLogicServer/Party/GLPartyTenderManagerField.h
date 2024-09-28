#ifndef _GLPARTYTENDERMANAGER_H_
#define _GLPARTYTENDERMANAGER_H_

#include <map>
#include "../../RanLogic/Party/GLPartyDefine.h"
#include "../../RanLogic/Item/GLItemCustom.h"

class CItemDrop;
struct GLTenderItemCustomField : public GLTenderItemCustom
{
	typedef std::vector<DWORD> MemberGaeaIDsVector;
	GLTenderItemCustomField(const MapID& mapID, CItemDrop* pItemDrop, const float fInsertTime, const D3DXVECTOR3& vGeneratePosition);

	void Reset(void);

	CItemDrop* m_pItemDrop;
	MemberGaeaIDsVector m_TenderMemberGaeaIDs;
	MemberGaeaIDsVector m_AbandonMemberGaeaIDs;
};

class GLGaeaServer;
class GLPartyTenderManagerField
{
	typedef std::map<DWORD, GLTenderItemCustomField>	ItemCustomMap;
	typedef ItemCustomMap::iterator				ItemCustomMapIter;
	typedef ItemCustomMap::const_iterator		ItemCustomMapCIter;
	typedef ItemCustomMap::value_type			ItemCustomMapValue;

	typedef std::map<GLPartyID, ItemCustomMap> PartyItemCustomMap;
	typedef PartyItemCustomMap::iterator				PartyItemCustomMapIter;
	typedef PartyItemCustomMap::const_iterator	PartyItemCustomMapCIter;
	typedef PartyItemCustomMap::value_type       	PartyItemCustomMapValue;

public:
	const bool isLootable(const GLPartyID& PartyID, const DWORD dwGlobalID) const;
	const bool AddTenderItem(const GLPartyID& PartyID, const MapID& mapID, const D3DXVECTOR3& vGeneratePosition, CItemDrop* pItemDrop);
	const bool AddDistributionItem(const GLPartyID& PartyID, const MapID& mapID, const D3DXVECTOR3& vGeneratePosition, CItemDrop* pItemDrop);
	// 입찰에 참여한 멤버가 있나?
	const bool IsTenderMemberExist( const GLPartyID& PartyId, const CItemDrop* pItem );
	// 소유권 재설정을 해야 하는지 체크
	const bool IsTenderOwnReset( DWORD dwPartyId, const CItemDrop* pItem);
	
	const bool MsgProcess(NET_MSG_GENERIC* pMessage);

	void FrameMove(const float fTime, const float fElapsedTime);

private:
	const bool MsgProcess_TenderItem(NET_MSG_GENERIC* pMessage);	
	const bool MsgProcess_JunkItem(NET_MSG_GENERIC* pMessage);
	const bool MsgProcess_TransferItem(NET_MSG_GENERIC* pMessage);

	void _addItem(const GLPartyID& PartyID, const MapID& mapID, const D3DXVECTOR3& vGeneratePosition, CItemDrop* pItemDrop);

public:
	GLPartyTenderManagerField(GLGaeaServer* const pGaeaServer);
	~GLPartyTenderManagerField(void);

private:	
	GLGaeaServer* const m_pGaeaServer;

	PartyItemCustomMap m_PartyItemCustomMap;	
	float m_fRefreshTime;
	float m_fCurrentTime;
};

#endif


