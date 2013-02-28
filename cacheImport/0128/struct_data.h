#ifndef _STRUCT_DATA_CPP
#define _STRUCT_DATA_CPP

#include <maya/MIntArray.h>
#include <maya/MString.h>
#include <maya/MFloatArray.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include<fstream>
#include <vector>
using namespace std;
struct struct_position
{
	float x;
	float y;
	float z;
};

struct uvSetInfo
{
	MString name;
	MFloatArray uArr;
	MFloatArray vArr;
	MIntArray uvid;
	MIntArray uvCounts;
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
};

struct meshTopoInfo
{
	MString objName;
	int vertexNum;
	int faceNum;
	MIntArray polygonCount,polygonConn;
	vector<uvSetInfo> allUVs;
	MPointArray allPoints;

	meshTopoInfo()
	{
		objName="";
		vertexNum=-1;
		faceNum=-1;
		polygonCount.clear();
		polygonConn.clear();
		allPoints.clear();
		allUVs.clear();
	}

	meshTopoInfo(MString objName)
	{
		this->objName=objName;
		vertexNum=-1;
		faceNum=-1;
		polygonCount.clear();
		polygonConn.clear();
		allPoints.clear();
		allUVs.clear();
	}

	int indexInSurFile(fstream& fin,streampos& topoPos,streampos& cachePos)
	{
		int objNum,res=-1;
		fin.read((char*)&objNum,sizeof(int));
		for(int i=0;i<objNum;i++)
		{
			char name[255];
			fin.read((char*)&name[0],sizeof(char)*255);
			fin.read((char*)&topoPos,sizeof(streampos));
			fin.read((char*)&cachePos,sizeof(streampos));
			if(MString(name)==objName)
			{
				return i;
			}
		}
		return -1;
	}

	int indexInCacheFile(fstream& fin,streampos& topoPos,streampos& cachePos)
	{
		int objNum,res=-1;
		fin.read((char*)&objNum,sizeof(int));
		struct_basicObjInfo objIndex;
		for(int i=0;i<objNum;i++)
		{
			fin.read((char*)&objIndex,sizeof(struct_basicObjInfo));
			if(MString(objIndex.objName)==objName)
			{
				topoPos=objIndex.topoBegin;
				cachePos=objIndex.cacheBegin;
				res=i;
			}
		}
		
		return res;
	}

	int indexInCacheFile(fstream& fin,struct_basicObjInfo& objIndex)
	{
		int objNum,res=-1;
		fin.read((char*)&objNum,sizeof(int));
		struct_basicObjInfo objIndexTemp;
		for(int i=0;i<objNum;i++)
		{
			fin.read((char*)&objIndexTemp,sizeof(struct_basicObjInfo));
			if(MString(objIndexTemp.objName)==objName)
			{
				strcpy(objIndex.objName,objIndexTemp.objName);
				objIndex.vertexNum=objIndexTemp.vertexNum;
				objIndex.startFrame=objIndexTemp.startFrame;
				objIndex.endFrame=objIndexTemp.endFrame;
				objIndex.actualFrameSize=objIndexTemp.actualFrameSize;
				objIndex.cacheBegin=objIndexTemp.cacheBegin;
				objIndex.topoBegin=objIndexTemp.topoBegin;
				res=i;
			}
		}

		return res;
	}

	bool getInfoFromSur(MString surFileName)
	{
		fstream fin(surFileName.asChar(),ios_base::in|ios_base::binary);
		if(!fin)
			return false;
		streampos cachePos,topoPos;
		int index=indexInSurFile(fin,topoPos,cachePos);
		cout<<"suridx"<<index<<endl;
		if(index==-1)
		{
			fin.close();
			return false;
		}

		fin.seekp(topoPos);
		fin.read((char*)&vertexNum,sizeof(int));
		fin.read((char*)&faceNum,sizeof(int));
		cout<<vertexNum<<"   "<<faceNum<<endl;
		vector<int> polyCountVec(faceNum);
		polygonCount.setLength(faceNum);
		fin.read((char*)&polyCountVec[0],sizeof(int)*faceNum);
		for(int i=0;i<faceNum;i++)
			polygonCount[i]=polyCountVec[i];

		int polyConnNum;
		fin.read((char*)&polyConnNum,sizeof(int));
		cout<<"ttt"<<polyConnNum<<endl;
		vector<int> polyConnVec(polyConnNum);
		fin.read((char*)&polyConnVec[0],sizeof(int)*polyConnNum);
		polygonConn.setLength(polyConnNum);
		for(int i=0;i<polyConnNum;i++)
			polygonConn[i]=polyConnVec[i];


		int uvsetNum;
		fin.read((char*)&uvsetNum,sizeof(int));
		cout<<uvsetNum<<"aaa"<<endl;
		allUVs.resize(uvsetNum);

		for(int i=0;i<uvsetNum;i++)
		{
			char name[255];
			fin.read((char*)&name[0],sizeof(char)*255);
			allUVs[i].name=MString(name);
			cout<<name<<endl;

			int uvlength;
			fin.read((char*)&uvlength,sizeof(int));
			allUVs[i].uArr.setLength(uvlength);
			allUVs[i].vArr.setLength(uvlength);

			vector<float> uvs(uvlength);
			fin.read((char*)&uvs[0],sizeof(float)*uvlength);
			for(int j=0;j<uvlength;j++)
				allUVs[i].uArr[j]=uvs[j];
			fin.read((char*)&uvs[0],sizeof(float)*uvlength);
			for(int j=0;j<uvlength;j++)
				allUVs[i].vArr[j]=uvs[j];

			int countsLength;
			fin.read((char*)&countsLength,sizeof(int));
			allUVs[i].uvCounts.setLength(countsLength);
			vector<int> uvCountsVec(countsLength);
			fin.read((char*)&uvCountsVec[0],sizeof(int)*countsLength);
			for(int j=0;j<countsLength;j++)
				allUVs[i].uvCounts[j]=uvCountsVec[j];

			int uvidlength;
			fin.read((char*)&uvidlength,sizeof(int));
			allUVs[i].uvid.setLength(uvidlength);
			vector<int> uvidVec(uvidlength);
			fin.read((char*)&uvidVec[0],sizeof(int)*uvidlength);
			for(int j=0;j<uvidlength;j++)
				allUVs[i].uvid[j]=uvidVec[j];

			cout<<uvlength<<" "<<countsLength<<" "<<uvidlength<<" "<<polygonCount.length()<<"wwwwwwwwwwwwwwww"<<endl;
		}
		fin.close();
		return true;
	}

	bool getInfoFromCache(MString cacheFileName)
	{
		fstream fin(cacheFileName.asChar(),ios_base::in|ios_base::binary);
		if(fin.fail())
			return -1;
		streampos cachePos,topoPos;
		int index=indexInCacheFile(fin,topoPos,cachePos);
		if(index==-1)
		{
			fin.close();
			return false;
		}
		cout<<index<<"cache"<<endl;
		fin.seekp(topoPos);
		fin.read((char*)&vertexNum,sizeof(int));
		fin.read((char*)&faceNum,sizeof(int));
		int polyConnNum;
		fin.read((char*)&polyConnNum,sizeof(int));
		polygonCount.setLength(faceNum);
		polygonConn.setLength(polyConnNum);
		vector<int> temp(faceNum);
		fin.read((char*)&temp[0],sizeof(int)*faceNum);
		for(int i=0;i<faceNum;i++)
			polygonCount[i]=temp[i];

		temp.resize(polyConnNum);
		fin.read((char*)&temp[0],sizeof(int)*polyConnNum);
		for(int i=0;i<polyConnNum;i++)
			polygonConn[i]=temp[i];
		fin.close();
		return true;
	}

	bool getPointsFromSur(MString surFileName)
	{
		fstream fin(surFileName.asChar(),ios_base::in|ios_base::binary);
		if(!fin)
			return false;
		streampos cachePos,topoPos;
		int index=indexInSurFile(fin,topoPos,cachePos);
		if(index==-1)
		{
			fin.close();
			return false;
		}
		fin.seekp(topoPos);
		fin.read((char*)&vertexNum,sizeof(int));
		allPoints.setLength(vertexNum);
		fin.seekp(cachePos);
		vector<float> allPVec(vertexNum*3);
		fin.read((char*)&allPVec[0],sizeof(float)*vertexNum*3);
		for(int i=0;i<vertexNum;i++)
			allPoints[i]=MPoint(allPVec[3*i+0],allPVec[3*i+1],allPVec[3*i+2]);
		fin.close();

		return true;
	}

	bool getPointsFromCache(MString cacheFileName,int timeValue)
	{

		//que shao xin xi
		fstream fin(cacheFileName.asChar(),ios_base::in|ios_base::binary);
		if(fin.fail())
			return -1;
		struct_basicObjInfo objIndex;
		int index=indexInCacheFile(fin,objIndex);
		if(index==-1)
		{
			fin.close();
			return false;
		}

		if(timeValue<objIndex.startFrame||timeValue>objIndex.endFrame)
		{
			fin.close();
			return false;
		}

		fin.seekp(objIndex.cacheBegin);
		vector<streampos> allPosIndex(objIndex.endFrame-objIndex.startFrame+1);
		fin.read((char*)&allPosIndex[0],sizeof(streampos)*(objIndex.endFrame-objIndex.startFrame+1));
		fin.seekp(objIndex.cacheBegin.operator+(allPosIndex[timeValue-objIndex.startFrame]));
		vector<struct_position> framePos(objIndex.vertexNum);
		fin.read((char*)&framePos[0],sizeof(struct_position)*objIndex.vertexNum);
		allPoints.setLength(objIndex.vertexNum);
		for(int i=0;i<objIndex.vertexNum;i++)
		{
			allPoints[i]=MPoint(framePos[i].x,framePos[i].y,framePos[i].z);
		}
		fin.close();
		return true;
	}
};

struct struct_cacheObjInfo
{
	vector<int> actualFrameIndex;// for internal usage
	vector<streampos> posIndex;//for cache file
	vector<vector<struct_position>> pointPositions;//for cache file
};
#endif