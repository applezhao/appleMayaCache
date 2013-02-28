#pragma once

#include <maya/MSimple.h>
#include <maya\MStringArray.h>
#include <maya\MTransformationMatrix.h>
#include <maya\MFnTransform.h>
#include <maya\MGlobal.h>
#include <maya\MSelectionList.h>
#include <maya\MDagPath.h>
#include <maya\MItSelectionList.h>
#include <maya\MFnMesh.h>
#include <maya\MMatrix.h>
#include <maya\MCommandResult.h>
#include <maya\MQuaternion.h>
#include <maya\MEulerRotation.h>
#include <math.h>
#include <maya\MTime.h>
#include <maya\MAnimControl.h>
#include <maya\MFloatArray.h>
#include <maya\MQuaternion.h>
#include <maya/MSimple.h>
#include <maya/MGlobal.h>
#include <maya/MString.h>
#include <maya\MStringArray.h>
#include <maya/MPoint.h>
#include <maya/MVector.h>
#include <maya/MTime.h>
#include <maya/MArgList.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MDagPath.h>
#include <maya/MItDag.h>
#include <maya/MSelectionList.h>
#include <maya\MItSelectionList.h>
#include <maya/MAnimControl.h>
#include <maya/MPxCommand.h>
#include <maya/MComputation.h>
#include <maya\MItMeshVertex.h>
#include <maya\MDistance.h>
#include <maya/MFnPlugin.h>
#include <maya\MPxNode.h>
#include <maya\MStatus.h>
#include <maya\MPlug.h>
#include <maya\MDataBlock.h>
#include <maya\MObject.h>
#include <maya\MTypeId.h>
#include <maya\MFnNumericAttribute.h>
#include <maya\MFnUnitAttribute.h>
#include <maya\MFnTypedAttribute.h>
#include <maya\MFnVectorArrayData.h>
#include <maya\MFloatVectorArray.h>
#include <maya\MVectorArray.h>
#include <maya\MStringArray.h>
#include <maya\MFloatVector.h>
#include <maya\MFnGenericAttribute.h>
#include <maya\MFnCompoundAttribute.h>
#include <maya\MFnStringData.h>
#include <maya\MDataHandle.h>
#include <maya\MFnAttribute.h>
#include <maya\MArrayDataBuilder.h>
#include <maya\MPointArray.h>
#include <maya\MPoint.h>
#include <maya\MFloatArray.h>
#include <maya\MMatrix.h>
#include <maya\MFnMeshData.h>
#include <maya\MFnMesh.h>
#include <maya\MDagPathArray.h>
#include <maya\MPlug.h>
#include <maya\MPlugArray.h>

#include <map>

template<class T> static bool
	CompareArray(const T &a, const T &b)
{
	if(a.length() != b.length()) return false;
	for(unsigned int i = 0; i < a.length(); ++i){
		if(a[i] != b[i]) return false;
	}
	return true;
}


#include "struct_data.h"

class cacheImport : public MPxNode
{
public:
						cacheImport();
	virtual				~cacheImport(); 

	virtual MStatus		compute( const MPlug& plug, MDataBlock& data );
	virtual MStatus	connectionBroken( const MPlug& plug, const MPlug& otherPlug, bool asSrc );
	virtual MStatus connectionMade( const MPlug& plug, const MPlug& otherPlug, bool asSrc );
// 	virtual MStatus setDependentsDirty(const MPlug& plug, MPlugArray& plugArray)
// 	{
// 		if ( plug.partialName() == MString("t")) 
// 		{
// 			MObject thisNode = thisMObject();
// 			MPlug pB( thisNode, outMesh );
// 			plugArray.append( pB );
// 			for(int i=0;i<pB.numElements();i++)
// 				plugArray.append(pB.elementByLogicalIndex(i));
// 		}
// 		return( MS::kSuccess );
// 	}
	static  void*		creator();
	static  MStatus		initialize();

public:
	static MObject time;
	static MObject file;
	static MObject surFile;
	//static MObject objName;
	static MObject inMesh;
	static MObject outMesh;

	static MTypeId id;
private:
	int getData(MString fileName,MString objName,int frame,MPointArray& postionDatas);
	bool inset(MString ele,MStringArray& seta)
	{
		for(int i=0;i<seta.length();i++)
		{
			if(ele==seta[i])
				return true;
		}
		return false;
	}
	bool getPointsFromCache(MString cacheFileName,int timeValue,MString objName,MPointArray& allPoints);
	int indexInCacheFile(fstream& fin,struct_basicObjInfo& objIndex,MString objName)
	{
		int objNum,res=-1;
		fin.read((char*)&objNum,sizeof(int));
		struct_basicObjInfo objIndexTemp;
		for(int i=0;i<objNum;i++)
		{
			//cout<<"fff"<<i<<endl;
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
	MString objTransNameInFile;
	map<int,MString> allObjTrans;
};

MStatus initializePlugin( MObject obj )

{ 
	MStatus   status;

	MFnPlugin implugin( obj, "", "2011", "Any");
	status = implugin.registerNode( "vixo_cacheImport", cacheImport::id, cacheImport::creator,
								  cacheImport::initialize );
	if (!status) {
		status.perror("registerNode");
		return status;
	}

	return status;
}

MStatus uninitializePlugin( MObject obj)

{
	MStatus   status;

	MFnPlugin implugin( obj );
	status = implugin.deregisterNode( cacheImport::id );
	if (!status) {
		status.perror("deregisterNode");
		return status;
	}
	
	return status;
}

