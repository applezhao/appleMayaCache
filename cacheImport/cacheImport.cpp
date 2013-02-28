#include "cacheImport.h"

#include <maya\MPlug.h>
#include <maya\MStringArray.h>


MTypeId cacheImport::id(0x25674);

MObject cacheImport::time;
MObject cacheImport::file;
MObject cacheImport::surFile;
MObject cacheImport::inMesh;
MObject cacheImport::outMesh;

cacheImport::cacheImport(void)
{
	objTransNameInFile=MString("");
	allObjTrans.clear();
}

cacheImport::~cacheImport(void)
{
}
//file -r -type "mayaAscii" -gl -loadReferenceDepth "all" -namespace "c001_maliang_c_skin" -options "v=0" "C:/Users/Administrator/Desktop/c001_maliang_c_skin.ma";

MStatus cacheImport::compute( const MPlug& plug, MDataBlock& data )
{
	//cout<<"cccccccccccc"<<endl;
	MStatus stat=MS::kSuccess;
	if(plug.array()!=outMesh)
		return MS::kSuccess;
// 	MPlugArray destPlug;
// 	plug.connectedTo(destPlug,false,true);
// 	if(destPlug.length()<=0)
// 		return MS::kSuccess;
	
	//cout<<plug.info().asChar()<<endl;
	int plugIdx=plug.logicalIndex();
	map<int,MString>::iterator mapIter=allObjTrans.find(plugIdx);
	if(mapIter==allObjTrans.end())
		return MS::kSuccess;
	MString objTrans=mapIter->second;
	//cout<<objTrans.asChar()<<endl;

	MDataHandle timeHandle=data.inputValue(time);
	int t=timeHandle.asTime().as(MTime::Unit::kFilm);

	MArrayDataHandle inMeshHandle=data.inputArrayValue(inMesh);
	//cout<<"is jump"<<endl;
	if(!inMeshHandle.jumpToElement(plugIdx))
		return MS::kSuccess;
	MPlug inMeshPlug(thisMObject(),inMesh);
	MPlug inMeshObjPlug=inMeshPlug.elementByLogicalIndex(plugIdx);
	MObject inMeshObj=inMeshObjPlug.asMObject();
	//MObject inMeshObj=inMeshHandle.inputValue().asMesh();
	//cout<<"is jumpdd"<<endl;

	MFnMeshData dataCreator;
	MObject newOutputData = dataCreator.create();

	MFnMesh fnMesh;
	fnMesh.copy(inMeshObj,newOutputData);

	MFnMesh fnoutMesh(newOutputData);
	
	MPointArray allPoints;
	int temp=getPointsFromCache(data.inputValue(file).asString(),t,objTrans,allPoints);
	//cout<<"ppp"<<temp<<endl;
	if(temp)
		fnoutMesh.setPoints(allPoints);
	//else 
	//	return MS::kSuccess;
// 	MArrayDataHandle outmeshhandle=data.outputArrayValue(outMesh);
// 	cout<<"ppp"<<temp<<endl;
// 	cout<<outmeshhandle.elementCount()<<endl;
// 	if(!outmeshhandle.jumpToElement(plugIdx))
// 		return MS::kSuccess;
// 	cout<<"ppp"<<temp<<endl;
// 	outmeshhandle.outputValue().set(newOutputData);
// 	cout<<"ppp"<<temp<<endl;
// 	outmeshhandle.outputValue().setClean();
	data.outputValue(plug).set(newOutputData);
	
	data.setClean(plug);
	//cout<<"ppp"<<temp<<endl;
	return MS::kSuccess;

}
bool cacheImport::getPointsFromCache(MString cacheFileName,int timeValue,MString objName,MPointArray& allPoints)
{
//que shao xin xi
	fstream fin(cacheFileName.asChar(),ios_base::in|ios_base::binary);
	if(fin.fail())
		return false;
	struct_basicObjInfo objIndex;
	int index=indexInCacheFile(fin,objIndex,objName);
	//cout<<"idx"<<index<<endl;
	if(index==-1)
	{
		fin.close();
		return false;
	}

	if(timeValue<objIndex.startFrame||timeValue>objIndex.endFrame)
	{
		fin.close();
	//	cout<<"here"<<endl;
		return false;
	}
	//cout<<objIndex.startFrame<<" "<<objIndex.endFrame<<" "<<objIndex.vertexNum<<" "<<timeValue<<endl;
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
		//cout<<i<<" "<<framePos[i].x<<" "<<framePos[i].y<<" "<<framePos[i].z<<endl;
	}
	fin.close();
	return true;
}


MStatus cacheImport::connectionMade( const MPlug& plug, const MPlug& otherPlug, bool asSrc )
{
	if(plug.array()==MPlug(thisMObject(),inMesh))
	{
		MObject node=thisMObject();
		if(!(plug.array()==MPlug(node,inMesh)&&otherPlug.node().hasFn(MFn::kMesh)))
			return MS::kSuccess;
		MFnDagNode dagNodeFn(otherPlug.node());
		if(dagNodeFn.parentCount()<=0)
			return MS::kSuccess;
		MFnDagNode transformFn(dagNodeFn.parent(0));
		MStringArray tempArray;
		transformFn.name().split(':',tempArray);
		MString transName=tempArray[tempArray.length()-1].asChar();
		allObjTrans.insert(make_pair(plug.logicalIndex(),transName));
		return MS::kSuccess;
	}
	return MPxNode::connectionMade(plug,otherPlug,asSrc);
}

MStatus cacheImport::connectionBroken( const MPlug& plug, const MPlug& otherPlug, bool asSrc )
{
	//MObject node=thisMObject();
	if(plug.array()==MPlug(thisMObject(),inMesh))
	{
		MObject node=thisMObject();
		if(plug.array()==MPlug(node,inMesh))
			allObjTrans.erase(plug.logicalIndex());
		return MS::kSuccess;
	}
	return MPxNode::connectionBroken(plug,otherPlug,asSrc);
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
	inMesh=tAttr.create("inMesh","im",MFnData::kMesh);
	tAttr.setArray(true);
	//tAttr.setStorable(false);
	//tAttr.setWritable(false);
	//tAttr.setUsesArrayDataBuilder(true);
	outMesh=tAttr.create("outMesh","om",MFnData::kMesh);
	tAttr.setArray(true);
	tAttr.setStorable(false);
	//tAttr.setCached(false);
	//tAttr.setWritable(false);
	//tAttr.setUsesArrayDataBuilder(true);
	
	addAttribute(time);
	addAttribute(file);
	addAttribute(surFile);
	addAttribute(inMesh);
	addAttribute(outMesh);

	attributeAffects(time,outMesh);
	attributeAffects(file,outMesh);
	attributeAffects(surFile,outMesh);
	attributeAffects(inMesh,outMesh);
	return MS::kSuccess;

}


