#ifndef _VIXO_CACHE_DATA_TYPEDEF_H
#define _VIXO_CACHE_DATA_TYPEDEF_H

#include <vector>
using namespace std;
struct struct_position
{
	float x;
	float y;
	float z;
};

struct struct_indexInfo
{
	char objName[250];
	char lastestUpdateTime[20];
};

struct struct_tempTime
{
	char lastestUpdateTime[20];
};

struct struct_visBasicInfo
{
	char objName[250];
	int startFrame;
	int endFrame;
	streampos visBegin;
};


struct struct_basicObjInfo
{
	char objName[250];
	int vertexNum;
	int startFrame;
	int endFrame;
	int actualFrameSize;
	streampos cacheBegin;
};

struct struct_cacheObjInfo
{
	vector<int> actualFrameIndex;// for internel usage
	vector<streampos> posIndex;//for cache file
	vector<vector<struct_position>> pointPositions;//for cache file
};
#endif