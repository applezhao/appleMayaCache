#pragma once
#include "vixo_cacheDataTypedef.h"
#include "vixo_matrixDecompose.h"

#include <maya/MPointArray.h>
#include <maya/MTimer.h>
#include <maya/MIntArray.h>

#include <maya\MFnPlugin.h>
#include <maya\MPxCommand.h>



class vixo_cacheExport:public MPxCommand
{
public:

	vixo_cacheExport(void);
	virtual ~vixo_cacheExport(void);

	virtual MStatus doIt ( const MArgList& args );
    static void* creator();
private:
	//void diff_CacheAnim(MString nameSpace,MStringArray objs,int start,int end,bool cacheForce,MStringArray & anim, MStringArray & cache,MStringArray & nothingToDo);
	//void cache_export(MString nameSpace,MStringArray caches,MStringArray vises,MStringArray cacheNeedDel,MStringArray nothingToDo,MString cacheFileName,MString visFileName,int start,int end);
	//void vis_export(MString nameSpace,MStringArray vises,MStringArray visNeedDel,MString fileName,int start,int end);
	void anim_export(MString nameSpace,MString animFileNamePre,char time[20],MStringArray anims,MStringArray animNeedDel,MStringArray nothingToDo,MString dirPath,int start,int end);
	void getAnimOldData(MString animFileName,MString animIndexFileName,MStringArray& animInFile,vector<structAnimInfo>& animInFileData);

	bool inSet(MString element,MStringArray& set);
	double asDouble(fstream & fin);
	void advance(fstream & fin);

	void orSet(MStringArray& seta,MStringArray& setb,MStringArray& res);
	void minusSet(MStringArray& seta,MStringArray& setb,MStringArray& res);
	void andSet(MStringArray& seta,MStringArray& setb,MStringArray& res);
	int inSetIndex(MString element,MStringArray& set);

	//void splitObjNames(MStringArray& nodes,MString& nameSpace,MStringArray & names);

	//void export();
	void getAllNameSpace(const MStringArray& allNodes,const MStringArray& objDel,MStringArray& allNameSpace);
	void getAllLigNameSpace(const MStringArray& allNameSpace,const MStringArray & nameSpaceMappingArr,MStringArray & ligNameSpace);
	void splitName(const MString& node,MString & nameSpace,MString & objName);
	void collectPrepareData(bool isCloth,MString dirPath,MString cacheDirName,MString animDirName,bool cacheForce,const MStringArray& allNodes,const MStringArray& objDel,vector<struct_prepareInfo>& prepareData,const MStringArray & nameSpaceMappingArr);

	void diff_cacheAnimNothing(bool cacheForce,struct_prepareInfo& prepareDataEle);
	void exportCacheVis(vector<struct_prepareInfo>& prepareData,int beginIdx,int endIdx,int startFrame,int endFrame);
	void diffCacheNewOld(struct_prepareInfo& prepareDataEle,struct_cacheExportInfo& cacheDataEle);
	void prepareCacheData(int start,int end,char time[20],struct_prepareInfo& prepareDataEle,struct_cacheExportInfo& cacheDataEle);
	void prepareVisData(int start,int end,char time[20],struct_prepareInfo& prepareDataEle,struct_visExportInfo& visDataEle);

	void exportPlayBlast(vector<struct_prepareInfo>& prepareData,int beginIdx,int endIdx,int startFrame,int endFrame,vector<struct_cacheExportInfo>& cacheData,vector<struct_visExportInfo>& visData);
	void postCacheExport(int start,int end,char time[20],struct_prepareInfo& prepareDataEle,struct_cacheExportInfo& cacheDataEle,MString nmsp);
	void postVisExport(int start,int end,char time[20],struct_prepareInfo& prepareDataEle,struct_visExportInfo& visDataEle);
	//void exportVisNewData(MString nameSpace,MStringArray vises,MStringArray visNeedDel,MStringArray& visExport,MStringArray& visNoInfo,vector<bool>& visExportData,int start,int end);
	//void getVisFileExistedDatas(MString fileName,MStringArray& visFileExist,vector<struct_tempTime>& visFileExistTime,vector<struct_visInfo>& visFileExistData);
};

MStatus initializePlugin( MObject obj )

{ 
	MStatus   status;

	MFnPlugin explugin( obj, "", "2011", "Any");
	status = explugin.registerCommand( "vixo_cacheExport", vixo_cacheExport::creator);
	if (!status) {
		status.perror("registerCommand");
		return status;
	}

	return status;
}

MStatus uninitializePlugin( MObject obj)

{
	MStatus   status;

	MFnPlugin explugin( obj );
	status = explugin.deregisterCommand( "vixo_cacheExport" );
	if (!status) {
		status.perror("deregisterCommand");
		return status;
	}
	
	return status;
}
