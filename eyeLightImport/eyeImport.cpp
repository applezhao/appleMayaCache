#include "eyeImport.h"

#include <fstream>

#include <maya\MFnNumericAttribute.h>
#include <maya\MFnUnitAttribute.h>
#include <maya\MFnTypedAttribute.h>
#include <maya\MTime.h>

#include <maya\MFnDagNode.h>
#include <maya\MStringArray.h>
#include <maya\MPlug.h>
#include <maya\MPlugArray.h>
#include <maya/MVector.h>

#include "struct_data.h"

MTypeId eyeImport::id( 0x45730 );

MObject eyeImport::time;
MObject eyeImport::file;
MObject eyeImport::translate;

eyeImport::eyeImport(void)
	{

	}


eyeImport::~eyeImport(void)
	{

	}

void* eyeImport::creator()
	{
	return new eyeImport();
	}

MStatus eyeImport::initialize()
	{
	MFnNumericAttribute nAttr;
	translate=nAttr.create("translate","tr",MFnNumericData::k3Double);
	nAttr.setArray(true);

	MFnUnitAttribute uAttr;
	time=uAttr.create("time","t",MFnUnitAttribute::Type::kTime,1);

	MFnTypedAttribute tAttr;
	file=tAttr.create("file","f",MFnData::kString);
	tAttr.setStorable(true);

	addAttribute(time);
	addAttribute(file);
	addAttribute(translate);

	attributeAffects(time,translate);
	attributeAffects(file,translate);

	return MS::kSuccess;
	}

MStatus eyeImport::compute( const MPlug& plug, MDataBlock& data )
	{
	MStatus stat=MS::kSuccess;	
	if(plug.array()!=translate&&plug.parent().array()!=translate)
		return MS::kSuccess;

	//cout<<plug.info().asChar()<<endl;
	int objIdx=-1;
	if(plug.isChild())
		objIdx=plug.parent().logicalIndex();
	else if(plug.isElement())
		objIdx=plug.logicalIndex();

	MDataHandle file_handle=data.inputValue(file);
	MString filename=file_handle.asString();

	MDataHandle timeHandle=data.inputValue(time);
	int t=timeHandle.asTime().as(MTime::Unit::kFilm);

	if(mapObjName.count(objIdx)<=0)
		return MS::kSuccess;

	//cout<<"test"<<endl;

	MString objNameValue(mapObjName.find(objIdx)->second.c_str());
	//cout<<" "<<objIdx<<" "<<objNameValue.asChar()<<endl;
	bool res=true;
	ifstream fin(filename.asChar(),ios_base::in|ios_base::binary);
	if(fin.fail())
		{
		MDataHandle eleHandle=data.outputValue(plug);
		eleHandle.set(res);
		data.setClean(plug);
		return MS::kSuccess;
		}

	int objNum=0;
	fin.read((char*)&objNum,sizeof(int));
	vector<struct_visBasicInfo> objIndexes(objNum);
	fin.read((char*)&objIndexes[0],sizeof(struct_visBasicInfo)*objNum);

	int fileObjIndex=-1;
	for(int i=0;i<objNum;i++)
		{
		MStringArray tempArr;
		MString tempStr(objIndexes[i].objName);
		tempStr.split(':',tempArr);
		if(tempArr[tempArr.length()-1]==objNameValue)
			{
			fileObjIndex=i;
			break;
			}
		}

	if(fileObjIndex==-1)
		{
		MDataHandle eleHandle=data.outputValue(plug);
		eleHandle.set(res);
		data.setClean(plug);
		fin.close();
		return MS::kSuccess;
		}
	//cout<<"test1"<<endl;
	if(t<objIndexes[fileObjIndex].startFrame||t>objIndexes[fileObjIndex].endFrame)
		{
		fin.close();
		MDataHandle eleHandle=data.outputValue(plug);
		eleHandle.set(res);
		data.setClean(plug);
		return MS::kSuccess;
		}
	//cout<<"test2"<<endl;
	fin.seekg(objIndexes[fileObjIndex].visBegin.operator+(sizeof(double)*(t-objIndexes[fileObjIndex].startFrame)*3));
	double value[3];
	fin.read((char *)&value[0],sizeof(double)*3);
	fin.close();

	//cout<<t<<" "<<(int)value<<endl;
	cout<<value[0]<<" "<<value[1]<<" "<<value[2]<<endl;
	//if(value==0)
	//	res=0
	if(plug.isElement())
	{
		MDataHandle eleHandle=data.outputValue(plug);
		eleHandle.setMVector(MVector(value[0],value[1],value[2]));
		//eleHandle.setMFloatVector(value[0],value[1],value[2]);
		data.setClean(plug);
	}
	else if(plug.isChild())
	{
		MDataHandle eleHandle=data.outputValue(plug.parent());
		eleHandle.setMVector(MVector(value[0],value[1],value[2]));
		data.setClean(plug);
	}
	//MDataHandle eleHandle=data.outputValue(plug);
	//eleHandle.set(res);
	//data.setClean(plug);

	return MS::kSuccess;
	}

MStatus eyeImport::connectionMade(const MPlug& plug,const MPlug& otherPlug,bool asSrc)
	{
	if(plug.array()==this->translate||plug.parent().array()==this->translate)
		{
		//cout<<plug.info().asChar()<<" "<<plug.array().name().asChar()<<" "<<plug.logicalIndex()<<endl;
		MPlugArray arr;
		plug.connectedTo(arr,false,true);
		MFnDagNode dagNodeFn(arr[0].node());
		if(dagNodeFn.parentCount()<=0)
			return MS::kSuccess;
		//MFnDagNode transformFn(dagNodeFn.parent(0));
		MStringArray tempArray;
		dagNodeFn.name().split(':',tempArray);
		if(tempArray.length()<2)
			return MS::kSuccess;
		//cout<<tempArray[tempArray.length()-1].asChar()<<endl;
		int logic=-1;
		if(plug.array()==this->translate)
			logic=plug.logicalIndex();
		else
			logic=plug.parent().logicalIndex();
		mapObjName.insert(pair<int,string>(logic,tempArray[tempArray.length()-1].asChar()));
		return MS::kSuccess;
		}

	return MPxNode::connectionMade( plug, otherPlug, asSrc );

	}

MStatus eyeImport::connectionBroken(const MPlug& plug,const MPlug& otherPlug,bool asSrc)
	{
	if(plug.array()==this->translate||plug.parent().array()==this->translate)
		{
		int logic=-1;
		if(plug.array()==this->translate)
			logic=plug.logicalIndex();
		else
			logic=plug.parent().logicalIndex();
		mapObjName.erase(logic);
		return MS::kSuccess;
		}
	return MPxNode::connectionMade( plug, otherPlug, asSrc );
	}


MStatus eyeImport::setDependentsDirty( const MPlug &plugBeingDirtied,MPlugArray &affectedPlugs )
	{
	MStatus status;
	MObject thisNode = thisMObject();

	if ( plugBeingDirtied.partialName() == "t" || plugBeingDirtied.partialName() == "f") 
		{
		MPlug arrayVis(thisNode,this->translate);
		//affectedPlugs.append(arrayVis);
		//cout<<"idr"<<arrayVis.numElements()<<endl;
		for(int i=0;i<arrayVis.numElements();i++)
			{
			affectedPlugs.append(arrayVis.elementByLogicalIndex(i));
			}
		}
	return( MS::kSuccess );
	}
