#pragma once

namespace Faction
{
  //  
  //  __forceinline const unsigned int Manager::getNFaction(void) const
  //  {
  //      const unsigned int _nLeftFactionID(factionIDQueue.size());
  //      return MAX_FACTION - _nLeftFactionID;
  //  }
  //  __forceinline const int Manager::getNMember(const FactionID& _factionID) const
  //  {
  //      FactionIDMapCIter _iter = factionIDMap.find(_factionID);
  //      if ( _iter == factionIDMap.end() )
  //          return -1;

  //      const FactionGroup& _factionGroup = factionGroupArray[_iter->second];

  //      return _factionGroup.factionMemberVector.size();
  //  }
  //  __forceinline const FactionID* Manager::getFaction(const GLActorID& _actorID) const
  //  {
  //      ActorMapCIter _iter = actorMap.find(_actorID);
  //      if ( _iter == actorMap.end() )
  //      {
  //          return NULL;
  //      }

  //      const FactionGroup& _factionGroup = factionGroupArray[_iter->second];

		//return &(_factionGroup.factionID);
  //  }
  //  __forceinline const FactionMemberVector* Manager::getFactionMembers(const FactionID& _factionID) const
  //  {
  //      FactionIDMapCIter _iter = factionIDMap.find(_factionID);
  //      if ( _iter == factionIDMap.end() )
  //      {
  //          return NULL;
  //      }

  //      const FactionGroup& _factionGroup = factionGroupArray[_iter->second];

  //      return &(_factionGroup.factionMemberVector);
  //  }
  //  __forceinline const EMRELATION_TYPE Manager::getRelation(const FactionID& _myFactionID, const FactionID& _targetFactionID) const
  //  {
  //      const int _myArrayNum = GetFactionArrayNum(_myFactionID);
  //      const int _targetArrayNum = GetFactionArrayNum(_targetFactionID);

  //      if ( _myArrayNum == -1 || _targetArrayNum == -1 )
  //          return EMRELATION_ERROR;

  //      return factionRelationTable[_myArrayNum][_targetArrayNum];
  //  }
  //  __forceinline const EMRELATION_TYPE Manager::getRelation(const GLActorID& _myActorID, const GLActorID& _targetActorID) const
  //  {
  //      const int _myArrayNum = GetFactionArrayNum(_myActorID);
  //      const int _targetArrayNum = GetFactionArrayNum(_targetActorID);
  //      
  //      if ( _myArrayNum == -1 || _targetArrayNum == -1 )
  //          return EMRELATION_ERROR;

  //      return factionRelationTable[_myArrayNum][_targetArrayNum];
  //  }
  //  
  //  __forceinline const bool Manager::setRelation(const FactionID& _factionID, const FactionID& _targetFactionID, const EMRELATION_TYPE _emRelation)
  //  {
  //      if ( _emRelation > EMRELATION_NSIZE )
  //          return false;

  //      const int _myArrayNum = GetFactionArrayNum(_factionID);
  //      const int _targetArrayNum = GetFactionArrayNum(_targetFactionID);

  //      if ( _myArrayNum == -1 || _targetArrayNum == -1 )
  //          return false;

  //      if ( factionRelationTable[_myArrayNum][_targetArrayNum] == _emRelation )
  //          return false;

  //      factionRelationTable[_myArrayNum][_targetArrayNum] = _emRelation;

  //      return true;
  //  }

  //  __forceinline const EMRELATION_FLAG_TYPE Manager::getHPVisible() const
  //  {
  //      return emHpVisibleRelation;
  //  }

  //  __forceinline const bool Manager::setHPVisible(const EMRELATION_FLAG_TYPE _emHpVisible)
  //  {
  //      const bool bChanged( emHpVisibleRelation != _emHpVisible );
  //      emHpVisibleRelation = _emHpVisible;
  //      return bChanged;
  //  }

  //  __forceinline void Manager::removeFaction(FactionID& _factionID)
  //  {
  //      const int _factionArrayNum = GetFactionArrayNum(_factionID);
  //      if ( _factionArrayNum == -1 )
  //          return;
  //      FactionIDMapIter _factionIter = this->factionIDMap.find(_factionID);
  //      if ( _factionIter != this->factionIDMap.end() )
  //      {
  //          this->factionIDMap.erase(_factionIter);
  //      }
  //      _factionID.reset();
  //      factionIDQueue.push(_factionArrayNum);
  //  }

  //  __forceinline void Manager::removeActor(const GLActorID& _actorID)
  //  {
  //      ActorMapCIter _actorIter = this->actorMap.find(_actorID);
  //      if ( _actorIter == this->actorMap.end() )
  //          return;

  //      FactionGroup& _factionGroup = this->factionGroupArray[_actorIter->second];
  //      FactionMemberVector& _factionMemberVector = _factionGroup.factionMemberVector;

  //      FactionMemberVectorIter _iteratorFactionMember(std::find(_factionMemberVector.begin(), _factionMemberVector.end(), _actorID));
  //      if ( _iteratorFactionMember != _factionMemberVector.end() )
  //      {
  //          _factionMemberVector.erase(_iteratorFactionMember);
  //      }
  //      this->actorMap.erase(_actorIter);

  //      // 만약 이전 진영의 멤버가 하나도 남지 않게 되면 이전 진영을 ( !파.괴.한.다.! );

  //      if ( _factionMemberVector.size() == 0 )
  //      {
  //          removeFaction( _factionGroup.factionID );
  //      }
  //  }

  //  __forceinline void Manager::reset(void)
  //  {
  //      actorMap.clear();
  //      factionIDMap.clear();
  //      factionIDQueue = FactionIDQueue();

  //      for ( unsigned int i = 0; i < MAX_FACTION; ++i )
  //      {
  //          factionGroupArray[i].factionID.reset();
  //          factionGroupArray[i].factionMemberVector.clear();

  //          factionIDQueue.push(i);
  //          for ( int j = 0; j < MAX_FACTION; ++j)
  //          {
  //              factionRelationTable[i][j] = EMRELATION_ERROR;
  //          }
  //          factionRelationTable[i][i] = EMRELATION_ALLY; //기본적으로 미설정시 자기진영같은 같은 편으로 인식;
  //      }
  //  }

  //  __forceinline const int Manager::GetFactionArrayNum(const GLActorID& _actorID) const
  //  {
  //      ActorMapCIter _iter = actorMap.find(_actorID);
  //      if ( _iter == actorMap.end() )
  //          return -1;

  //      return _iter->second;
  //  }
  //  __forceinline const int Manager::GetFactionArrayNum(const FactionID& _factionID) const
  //  {
  //      FactionIDMapCIter _iter = factionIDMap.find(_factionID);
  //      if ( _iter == factionIDMap.end() )
  //          return -1;

  //      return _iter->second;
  //  }
  //  __forceinline const int Manager::GenerateFaction(const FactionID& _factionID)
  //  {
  //      FactionIDMapCIter _iter = factionIDMap.find(_factionID);
  //      if ( _iter != factionIDMap.end() )
  //          return _iter->second;

  //      if ( factionIDQueue.size() == 0 )
  //      {
  //          return -1;
  //      }

  //      const DWORD _factionArrayNum(factionIDQueue.front());
  //      factionGroupArray[_factionArrayNum].factionID = _factionID;

  //      factionIDQueue.pop();	

  //      return (int)_factionArrayNum;
  //  }
}

