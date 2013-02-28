#ifndef _VIXO_CACHE_DATA_TYPEDEF_H
#define _VIXO_CACHE_DATA_TYPEDEF_H

#include <maya\MArgList.h>
#include <maya\MStringArray.h>

#include <maya\MAnimUtil.h>
#include <maya\MPlugArray.h>
#include <maya\MObjectArray.h>
#include <maya\MFnMesh.h>
#include <maya\MDagPathArray.h>
#include <maya\MItMeshVertex.h>
#include <maya\MGlobal.h>
#include <maya\MSelectionList.h>
#include <maya\MItSelectionList.h>
#include <maya/MDagPath.h>
#include <maya/MFnMesh.h>

//#include <boost/array.hpp>

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

	//struct_visBasicInfo& operator=(const struct_visBasicInfo & src)
	//{
	//	struct_visBasicInfo dest;
	//	dest.startFrame=src.startFrame;
	//	dest.endFrame=src.endFrame;
	//	dest.visBegin=src.visBegin;
	//	::strcpy(dest.objName,src.objName);
	//	return dest;
	//}
};

struct meshTopoInfo
{
	int vertexNum;
	int faceNum;
	vector<int> polygonCount;
	int polygonConnNum;
	vector<int> polygonConn;

	meshTopoInfo(){}
	meshTopoInfo(int verNum,int polyNum,int faceConnNum)
	{
		vertexNum=verNum;
		faceNum=polyNum;
		polygonConnNum=faceConnNum;
		polygonCount.clear();
		polygonCount.resize(faceNum);
		polygonConn.clear();
		polygonConn.resize(polygonConnNum);
	}
	void init(int verNum,int polyNum,int faceConnNum)
	{
		vertexNum=verNum;
		faceNum=polyNum;
		polygonConnNum=faceConnNum;
		polygonCount.clear();
		polygonCount.resize(faceNum);
		polygonConn.clear();
		polygonConn.resize(polygonConnNum);
	}
};


struct struct_basicObjInfo
{
	char objName[250];
	int vertexNum;
	int startFrame;
	int endFrame;
	int actualFrameSize;
	streampos cacheBegin;
	streampos topoBegin;

	//struct_basicObjInfo& operator=(const struct_basicObjInfo & src)
	//{
	//	struct_basicObjInfo dest;
	//	dest.actualFrameSize=src.actualFrameSize;
	//	dest.vertexNum=src.vertexNum;
	//	dest.startFrame=src.startFrame;
	//	dest.endFrame=src.endFrame;
	//	dest.cacheBegin=src.cacheBegin;
	//	::strcpy(dest.objName,src.objName);
	//	return dest;
	//}
};

struct struct_position_test
{
	float test[4];
};

//typedef float[4] float4;

struct struct_cacheObjInfo
{
	vector<int> actualFrameIndex;// for internel usage
	vector<streampos> posIndex;//for cache file
	vector<vector<struct_position>> pointPositions;//for cache file
	//vector<float[4]> pointPositionsTest;//for cache file
};
//vector<test_struc> test;
//allPs.get(&test[0].test[0][0]);

struct struct_cacheExportInfo
{
	MString cacheFileNamePre;
	MStringArray cacheKeep,cacheAdd,cacheDel,cacheUpdate,cacheFileExist;
	vector<struct_indexInfo> cacheKeepTime,cacheAddTime,cacheUpdateTime;
	vector<struct_tempTime> cacheFileExistTime;
	vector<struct_cacheObjInfo> cacheKeepData,cacheAddData,cacheUpdateOldData,cacheUpdateNewData;
	vector<struct_basicObjInfo> cacheKeepBasic,cacheAddBasic,cacheUpdateOldBasic,cacheUpdateNewBasic,cacheFileExistBasic;
	vector<meshTopoInfo> cacheKeepMesh,cacheAddMesh,cacheUpdateMesh;
	vector<bool> cacheUpdateFlag;
	MDagPathArray cacheAddDags,cacheUpdateDags;
	streampos cacheBeginBasis;

	void getTopologyIngo(MString nmsp)
	{
		cacheKeepMesh.resize(cacheKeep.length());
		cacheAddMesh.resize(cacheAdd.length());
		cacheUpdateMesh.resize(cacheUpdate.length());
		for(int i=0;i<cacheKeep.length();i++)
		{
			MSelectionList slist;
			MGlobal::getSelectionListByName(nmsp+":"+cacheKeep[i],slist);
			MDagPath dagPath;
			slist.getDagPath(0,dagPath);
			dagPath.extendToShape();

			MFnMesh fnMesh(dagPath);
			cacheKeepMesh[i].vertexNum=fnMesh.numVertices();
			cacheKeepMesh[i].faceNum=fnMesh.numPolygons();
			cout<<dagPath.partialPathName().asChar()<<"          "<<cacheKeepMesh[i].vertexNum<<endl;
			cacheKeepMesh[i].polygonCount.clear();
			cacheKeepMesh[i].polygonConn.clear();
			for(int j=0;j<fnMesh.numPolygons();j++)
			{
				cacheKeepMesh[i].polygonCount.push_back( fnMesh.polygonVertexCount(j) );
				MIntArray vlist;
				fnMesh.getPolygonVertices(j,vlist);
				for(int k=0;k<vlist.length();k++)
					cacheKeepMesh[i].polygonConn.push_back(vlist[k]);
			}
			cacheKeepMesh[i].polygonConnNum=cacheKeepMesh[i].polygonConn.size();
		}

		for(int i=0;i<cacheAdd.length();i++)
		{
			MSelectionList slist;
			MGlobal::getSelectionListByName(nmsp+":"+cacheAdd[i],slist);
			MDagPath dagPath;
			slist.getDagPath(0,dagPath);
			dagPath.extendToShape();

			MFnMesh fnMesh(dagPath);
			cacheAddMesh[i].vertexNum=fnMesh.numVertices();
			cacheAddMesh[i].faceNum=fnMesh.numPolygons();
			cacheAddMesh[i].polygonCount.clear();
			cacheAddMesh[i].polygonConn.clear();
			cout<<dagPath.partialPathName().asChar()<<"          "<<cacheAddMesh[i].vertexNum<<endl;
			for(int j=0;j<fnMesh.numPolygons();j++)
			{
				cacheAddMesh[i].polygonCount.push_back( fnMesh.polygonVertexCount(j) );
				MIntArray vlist;
				fnMesh.getPolygonVertices(j,vlist);
				for(int k=0;k<vlist.length();k++)
					cacheAddMesh[i].polygonConn.push_back(vlist[k]);
			}
			cacheAddMesh[i].polygonConnNum=cacheAddMesh[i].polygonConn.size();
		}

		for(int i=0;i<cacheUpdate.length();i++)
		{
			MSelectionList slist;
			MGlobal::getSelectionListByName(nmsp+":"+cacheUpdate[i],slist);
			MDagPath dagPath;
			slist.getDagPath(0,dagPath);
			dagPath.extendToShape();

			MFnMesh fnMesh(dagPath);
			cacheUpdateMesh[i].vertexNum=fnMesh.numVertices();
			cacheUpdateMesh[i].faceNum=fnMesh.numPolygons();
			cacheUpdateMesh[i].polygonCount.clear();
			cacheUpdateMesh[i].polygonConn.clear();
			cout<<dagPath.partialPathName().asChar()<<"          "<<cacheUpdateMesh[i].vertexNum<<endl;
			for(int j=0;j<fnMesh.numPolygons();j++)
			{
				cacheUpdateMesh[i].polygonCount.push_back( fnMesh.polygonVertexCount(j) );
				MIntArray vlist;
				fnMesh.getPolygonVertices(j,vlist);
				for(int k=0;k<vlist.length();k++)
					cacheUpdateMesh[i].polygonConn.push_back(vlist[k]);
			}
			cacheUpdateMesh[i].polygonConnNum=cacheUpdateMesh[i].polygonConn.size();
		}
		updateTopoFileStreamPos();
	}

	void updateTopoFileStreamPos()
	{
		for(int i=0;i<cacheKeep.length();i++)
		{
			cacheKeepBasic[i].topoBegin=cacheBeginBasis;
			cacheBeginBasis=cacheBeginBasis.operator+(sizeof(int)*(3+cacheKeepMesh[i].faceNum+cacheKeepMesh[i].polygonConnNum));
		}

		for(int i=0;i<cacheAdd.length();i++)
		{
			cacheAddBasic[i].topoBegin=cacheBeginBasis;
			cacheBeginBasis=cacheBeginBasis.operator+(sizeof(int)*(3+cacheAddMesh[i].faceNum+cacheAddMesh[i].polygonConnNum));
		}

		for(int i=0;i<cacheUpdate.length();i++)
		{
			cacheUpdateNewBasic[i].topoBegin=cacheBeginBasis;
			cacheBeginBasis=cacheBeginBasis.operator+(sizeof(int)*(3+cacheUpdateMesh[i].faceNum+cacheUpdateMesh[i].polygonConnNum));
		}
	}
};

struct struct_visExportInfo
{
	MString visFileNamePre;
	MStringArray visFileExist;
	vector<struct_tempTime> visFileExistTime;
	vector<char> visFileExistData;
	MDagPathArray path,path_backup,path_trans;
	vector<char> allVisData;
	vector<bool> flag_write;
	MStringArray visExport,visNoInfo;
	vector<char> visNewExportData;
	MStringArray visKeep,visAdd,visDel,visUpdate;
	vector<struct_indexInfo> visKeepTime,visNewExportTime;
	vector<struct_visBasicInfo> visKeepBasic,visNewExportBasic;
	vector<char> visKeepData;
	vector<bool> visUpdateFlag;
	vector<struct_visBasicInfo> visFileExistBasic;
};

struct struct_prepareInfo
{
	MString nameSpace;
	MString ligNameSpace;
	MStringArray objExport;
	MStringArray cache;
	MStringArray anims;
	MStringArray nothingToDo;
	MStringArray cacheNothingToDo;
	MStringArray animNothingToDo;
	MStringArray objDel;
	MString cacheFileNamePre;
	MString visFileNamePre;
	MString animDir;
	MString animFileNamePre;
};

struct structAnimInfo
{
	char objName[255];
	char lastestUpdateTime[20];
	double data[12];
	structAnimInfo(){}
	structAnimInfo(MString objname,char time[20],MDoubleArray& result,int idx)
	{
		for(int i=0;i<255;i++)
			objName[i]='\0';
		for(int i=0;i<20;i++)
			lastestUpdateTime[i]='\0';
		strcpy(objName,objname.asChar());
		strcpy(lastestUpdateTime,time);
		for(int i=0;i<12;i++)
			data[i]=result[12*idx+i];
	}
};

//const int MAXNUM=3;
#endif