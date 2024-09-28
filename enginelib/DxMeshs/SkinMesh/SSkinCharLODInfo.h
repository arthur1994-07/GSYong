#pragma once

#include <string>
#include <vector>

typedef std::vector<float>          FloatContainer;
typedef FloatContainer::iterator    FloatContainerIterator;
typedef std::vector<TSTRING>        StringContainer;
typedef StringContainer::iterator   StringContainerIterator;

const int NO_LOD = -1;

/*
    LOD ������ -1 �� ��� �ʱ� �����̸�               
                0 �̻��� ��� LOD �� �� ���� �̴�.

    LOD ������ ��ġ�� ���������� �����ϵ��� ��������.

*/
struct SSkinCharLODInfo
{
    FloatContainer      kDistance;  //@ LOD ���� ���� �Ÿ�
    StringContainer     kMeshName;  //@ LOD �޽���   
    int                 nLevel;     //@ LOD ���� ����    

    void resize(int i)  { kDistance.resize(i); kMeshName.resize(i); }
    bool empty()        { return kDistance.empty(); }
    size_t size()       { return kDistance.size(); }
    void clear()        { kDistance.clear(); kMeshName.clear(); nLevel = NO_LOD; }

    SSkinCharLODInfo() : nLevel(NO_LOD) {}
};