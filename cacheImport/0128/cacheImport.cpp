#include "cacheImport.h"

#include <maya\MPlug.h>
#include <maya\MStringArray.h>


MTypeId cacheImport::id(0x25674);

MObject cacheImport::time;
MObject cacheImport::file;
MObject cacheImport::surFile;
MObject cacheImport::objName;
MObject cacheImport::outMesh;

cacheImport::cacheImport(void)
{
}

cacheImport::~cacheImport(void)
{
}

MStatus cacheImport::compute( const MPlug& plug, MDataBlock& data )
{
	MStatus stat=MS::kSuccess;	
	if(plug!=outMesh)
		return MS::kSuccess;
	if(plug.isConnected()==false)
		return MS::kSuccess;
	MPlugArray destPlug;
	plug.connectedTo(destPlug,false,true);
	if(destPlug.length()<=0)
		return MS::kSuccess;
	if(destPlug[0].node().hasFn(MFn::kMesh)==false)
		return MS::kSuccess;

	//get obj name
	MString objTransName=data.inputValue(objName).asString();
	if(objTransName=="")
	{
		MFnDagNode dagNodeFn(destPlug[0].node());
		if(dagNodeFn.parentCount()<=0)
			return MS::kSuccess;
		MFnDagNode transformFn(dagNodeFn.parent(0));
		MStringArray tempArray;
		transformFn.name().split(':',tempArray);
		objTransName=tempArray[tempArray.length()-1].asChar();
	}
	cout<<objTransName.asChar()<<endl;
	//~get obj name

	MDataHandle timeHandle=data.inputValue(time);
	int t=timeHandle.asTime().as(MTime::Unit::kFilm);

	MObject outMeshObj=data.outputValue(outMesh).asMesh();
	MFnMesh fnMesh(outMeshObj);
	cout<<fnMesh.numVertices()<<endl;
	if(fnMesh.numVertices()<=0)
	{
		//get sur info
		cout<<"surtopo000"<<endl;
		meshTopoInfo topoSur(objTransName);
		bool hasSurInfo=topoSur.getInfoFromSur(data.inputValue(surFile).asString());
		if(!hasSurInfo)
			return MS::kSuccess;
		cout<<"surtopo111"<<endl;

		///get topo info
		cout<<"surtopo222"<<endl;
		meshTopoInfo topoCache(objTransName);
		bool hasCacheInfo=topoCache.getInfoFromCache(data.inputValue(file).asString());
		cout<<"aaaa"<<hasCacheInfo<<endl
			<<topoCache.vertexNum<<" "<<topoSur.vertexNum<<endl
			<<topoCache.faceNum<<" "<<topoSur.faceNum<<endl;
		if(!hasCacheInfo||
			((topoSur.vertexNum!=topoCache.vertexNum)||
			(topoSur.faceNum!=topoCache.faceNum)||
			!CompareArray(topoSur.polygonCount,topoCache.polygonCount)||
			!CompareArray(topoSur.polygonConn,topoCache.polygonConn)))
		{
			cout<<"surtopo333"<<endl;
			topoSur.getPointsFromSur(data.inputValue(surFile).asString());
			MFnMeshData dataCreator;
			MObject newOutputData = dataCreator.create();
			buildMesh(topoSur,topoSur.allPoints,newOutputData);
			data.outputValue(plug).setMObject(newOutputData);
			data.setClean(plug);
			return MS::kSuccess;
		}
		cout<<"surtopo444"<<endl;
		
		bool hasCacheFrame=topoCache.getPointsFromCache(data.inputValue(file).asString(),t);
		cout<<"surtopo555"<<endl;
		MFnMeshData dataCreator;
		MObject newOutputData = dataCreator.create();
		if(!hasCacheFrame)
		{
			cout<<"surtopo666"<<endl;
			topoSur.getPointsFromSur(data.inputValue(surFile).asString());
			buildMesh(topoSur,topoSur.allPoints,newOutputData);
			cout<<"surtopo777"<<endl;
		}
		else
		{
			cout<<"surtopo888"<<endl;
			buildMesh(topoSur,topoCache.allPoints,newOutputData);
			cout<<"surtopo999"<<endl;
		}
		data.outputValue(plug).setMObject(newOutputData);
		data.setClean(plug);
		return MS::kSuccess;
		
	}
	else
	{
		meshTopoInfo topoCache(objTransName);
		bool hasCacheFrame=topoCache.getPointsFromCache(data.inputValue(file).asString(),t);
		if(!hasCacheFrame)
		{
			bool isOkay=topoCache.getPointsFromSur(data.inputValue(surFile).asString());
			if(!isOkay)
				return MS::kSuccess;
		}
		fnMesh.setPoints(topoCache.allPoints);
		data.outputValue(plug).setMObject(fnMesh.object());
		data.setClean(plug);
		return MS::kSuccess;
	}
}

void cacheImport::buildMesh(meshTopoInfo & meshTopo,MPointArray& points,MObject& obj)
{
	int map1Idx=-1;
	for(int i=0;i<meshTopo.allUVs.size();i++)
	{
		cout<<"tttt:"<<meshTopo.allUVs[i].name.asChar()<<endl;;
		if(meshTopo.allUVs[i].name==MString("map1"))
		{
			map1Idx=i;
		}
	}
	cout<<"map11111111111~~"<<map1Idx<<endl;
	MStatus stat;
	MFnMesh fnMesh;
	fnMesh.create(meshTopo.vertexNum,meshTopo.faceNum,
		points,
		meshTopo.polygonCount,meshTopo.polygonConn,
		meshTopo.allUVs[map1Idx].uArr,
		meshTopo.allUVs[map1Idx].vArr,
		obj,&stat);
	cout<<"stat00:"<<stat.errorString().asChar()<<endl;
	cout<<fnMesh.numVertices()<<" "<<fnMesh.numFaceVertices()<<endl;
	stat=fnMesh.assignUVs(meshTopo.polygonCount,meshTopo.allUVs[map1Idx].uvid);
	cout<<"stat11:"<<stat.errorString().asChar()<<endl;

	MFnMesh fnTemp(obj);

	MStringArray existUVSets;
	fnTemp.getUVSetNames(existUVSets);

	for(int i=0;i<meshTopo.allUVs.size();i++)
	{
		if(meshTopo.allUVs[i].name!=MString("map1"))
		{
			cout<<"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"<<endl;
			fnTemp.createUVSetDataMeshWithName(meshTopo.allUVs[i].name);
			fnTemp.clearUVs(&meshTopo.allUVs[i].name);
			fnTemp.setUVs(meshTopo.allUVs[i].uArr,meshTopo.allUVs[i].vArr,&meshTopo.allUVs[i].name);
			//fnMesh.assignUVs(meshTopo.allUVs[i].uvCounts,meshTopo.allUVs[i].uvid,&meshTopo.allUVs[i].name);
			fnTemp.assignUVs(meshTopo.polygonCount,meshTopo.allUVs[i].uvid,&meshTopo.allUVs[i].name);
		}
		
	}
	fnTemp.setPoints(points);
}

int cacheImport::getData(MString fileName,MString objName,int frame,MPointArray& postionDatas)
{
	int objNum=0;
	vector<struct_basicObjInfo> objIndexes;
	int fileObjIndex=-1;

	fstream fin(fileName.asChar(),ios_base::in|ios_base::binary);
	if(fin.fail())
		return -1;
	fin.read((char*)&objNum,sizeof(int));
	objIndexes.resize(objNum);
	fin.read((char*)&objIndexes[0],sizeof(struct_basicObjInfo)*objNum);

	for(int i=0;i<objNum;i++)
	{
		if(MString(objIndexes[i].objName)==objName)
		{
			fileObjIndex=i;
			break;
		}
	}

	if(fileObjIndex==-1)
	{
		fin.close();
		return -1;
	}

	if(frame<objIndexes[fileObjIndex].startFrame||frame>objIndexes[fileObjIndex].endFrame)
	{
		fin.close();
		return -1;
	}

	fin.seekp(objIndexes[fileObjIndex].cacheBegin.operator+(sizeof(streampos)*(frame-objIndexes[fileObjIndex].startFrame)));
	streampos posIndex;
	fin.read((char*)&posIndex,sizeof(streampos));
	fin.seekp(objIndexes[fileObjIndex].cacheBegin.operator+(posIndex));
	vector<struct_position> pointPositions(objIndexes[fileObjIndex].vertexNum);
	fin.read((char*)&pointPositions[0],sizeof(struct_position)*objIndexes[fileObjIndex].vertexNum);
	fin.close();

	postionDatas.setLength(objIndexes[fileObjIndex].vertexNum);
	for(int i=0;i<objIndexes[fileObjIndex].vertexNum;i++)
	{
		postionDatas[i]=MPoint(pointPositions[i].x,pointPositions[i].y,pointPositions[i].z);
	}
	return 0;
}

void* cacheImport::creator()
{
	return new cacheImport();
}

MStatus cacheImport::initialize()
{
	MFnUnitAttribute uAttr;
	time=uAttr.create("time","t",MFnUnitAttribute::Type::kTime,1);

	MFnTypedAttribute tAttr;
	file=tAttr.create("file","f",MFnStringData::kString);
	surFile=tAttr.create("surFile","sf",MFnStringData::kString);
	objName=tAttr.create("objName","on",MFnStringData::kString);
	outMesh=tAttr.create("outMesh","om",MFnData::kMesh);
	//tAttr.setArray(true);
	tAttr.setStorable(false);
	tAttr.setWritable(false);
	
	addAttribute(time);
	addAttribute(file);
	addAttribute(surFile);
	addAttribute(objName);
	addAttribute(outMesh);

	attributeAffects(time,outMesh);
	attributeAffects(file,outMesh);
	attributeAffects(surFile,outMesh);
	attributeAffects(objName,outMesh);
	return MS::kSuccess;

}


