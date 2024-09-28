#pragma once

class DxLandMan;
class GLActiveMap
{
public:
    GLActiveMap() {}
    virtual ~GLActiveMap() {}

public:
	virtual DxLandMan* getactivemap() = 0;
};