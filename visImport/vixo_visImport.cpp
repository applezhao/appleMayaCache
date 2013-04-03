#include "vixo_visImport.h"

#include <fstream>

#include <maya\MFnNumericAttribute.h>
#include <maya\MFnUnitAttribute.h>
#include <maya\MFnTypedAttribute.h>
#include <maya\MTime.h>

#include <maya\MFnDagNode.h>
#include <maya\MStringArray.h>
#include <maya\MPlug.h>
#include <maya\MPlugArray.h>

#include "struct_data.h"

MTypeId vixo_visImport::id( 0x45865 );

MObject vixo_visImport::time;
MObject vixo_visImport::file;
MObject vixo_visImport::vis;

vixo_visImport::vixo_visImport(void)
{

}


vixo_visImport::~vixo_visImport(void)
{

}

void* vixo_visImport::creator()
{
	return new vixo_visImport();
}

MStatus vixo_visImport::initialize()
{
	MFnNumericAttribute nAttr;
	vis=nAttr.create("visibility","vis",MFnNumericData::kBoolean,1);
	nAttr.setArray(true);

	MFnUnitAttribute uAttr;
	time=uAttr.create("time","t",MFnUnitAttribute::Type::kTime,1);

	MFnTypedAttribute tAttr;
	file=tAttr.create("file","f",MFnData::kString);
	tAttr.setStorable(true);
	
	addAttribute(time);
	addAttribute(file);
	addAttribute(vis);

	attributeAffects(time,vis);
	attributeAffects(file,vis);

	return MS::kSuccess;
}

MStatus vixo_visImport::compute( const MPlug& plug, MDataBlock& data )
{
	MStatus stat=MS::kSuccess;	
	if(plug.array()!=vis)
		return MS::kSuccess;

	//cout<<plug.info().asChar()<<endl;

	int objIdx=plug.logicalIndex();

	MDataHandle file_handle=data.inputValue(file);
	MString filename=file_handle.asString();

	MDataHandle timeHandle=data.inputValue(time);
	int t=timeHandle.asTime().as(MTime::Unit::kFilm);

	if(mapObjName.count(objIdx)<=0)
		return MS::kSuccess;

	//cout<<"test"<<endl;

	MString objNameValue(mapObjName.find(objIdx)->second.c_str());
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
	fin.seekg(objIndexes[fileObjIndex].visBegin.operator+(sizeof(char)*(t-objIndexes[fileObjIndex].startFrame)));
	char value;
	fin.read((char *)&value,sizeof(char));
	fin.close();

	//cout<<t<<" "<<(int)value<<endl;
	
	if(value==0)
		res=0;
	MDataHandle eleHandle=data.outputValue(plug);
	eleHandle.set(res);
	data.setClean(plug);

	return MS::kSuccess;
}

MStatus vixo_visImport::connectionMade(const MPlug& plug,const MPlug& otherPlug,bool asSrc)
{
	if(plug.array()==this->vis)
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
		mapObjName.insert(pair<int,string>(plug.logicalIndex(),tempArray[tempArray.length()-1].asChar()));
		return MS::kSuccess;
	}
	return MPxNode::connectionMade( plug, otherPlug, asSrc );

}

MStatus vixo_visImport::connectionBroken(const MPlug& plug,const MPlug& otherPlug,bool asSrc)
{
	if(plug.array()==this->vis)
	{
		mapObjName.erase(plug.logicalIndex());
		return MS::kSuccess;
	}
	return MPxNode::connectionMade( plug, otherPlug, asSrc );
}


MStatus vixo_visImport::setDependentsDirty( const MPlug &plugBeingDirtied,MPlugArray &affectedPlugs )
{
    MStatus status;
    MObject thisNode = thisMObject();

    if ( plugBeingDirtied.partialName() == "t" || plugBeingDirtied.partialName() == "f") 
	{
		MPlug arrayVis(thisNode,this->vis);
		//affectedPlugs.append(arrayVis);
		//cout<<"idr"<<arrayVis.numElements()<<endl;
		for(int i=0;i<arrayVis.numElements();i++)
		{
			affectedPlugs.append(arrayVis.elementByLogicalIndex(i));
		}
    }
    return( MS::kSuccess );
}
