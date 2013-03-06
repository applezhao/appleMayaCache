#include "vixo_cacheExport.h"

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

#include <time.h>
#include <fstream>

#include <math.h>

vixo_cacheExport::vixo_cacheExport(void)
{
}


vixo_cacheExport::~vixo_cacheExport(void)
{
}

void * vixo_cacheExport::creator()
{
	return new vixo_cacheExport;
}

MStatus vixo_cacheExport::doIt(const MArgList& args)
{
	MStatus stat = MS::kSuccess;

	MString dirPath,cacheDirName,animDirName;
	MStringArray nodes,cacheVisDel,nameSpaceMappingArr;
	bool cacheForce;
	bool isCloth;
	int startFrame,endFrame,MAXNUM;

	unsigned index;

	index=args.flagIndex("dp","dirpath");
	if(MArgList::kInvalidArgIndex!=index)
	{
		args.get(index+1,dirPath);
	}

	index=args.flagIndex("cdn","cacheDirName");
	if(MArgList::kInvalidArgIndex!=index)
	{
		args.get(index+1,cacheDirName);
	}

	index=args.flagIndex("adn","animDirName");
	if(MArgList::kInvalidArgIndex!=index)
	{
		args.get(index+1,animDirName);
	}

	index=args.flagIndex("del","cacheVisDel");
	if(MArgList::kInvalidArgIndex!=index)
	{
		unsigned int i=index+1;
		cacheVisDel=args.asStringArray(i);
	}

	index=args.flagIndex("cf","cacheforce");
	if(MArgList::kInvalidArgIndex!=index)
	{
		args.get(index+1,cacheForce);
	}

	index=args.flagIndex("eff","iseff");
	if(MArgList::kInvalidArgIndex!=index)
	{
		args.get(index+1,isCloth);
	}
	//cout<<isCloth<<endl;

	index=args.flagIndex("st","starttime");
	if(MArgList::kInvalidArgIndex!=index)
	{
		args.get(index+1,startFrame);
	}

	index=args.flagIndex("mn","maxnum");
	if(MArgList::kInvalidArgIndex!=index)
	{
		args.get(index+1,MAXNUM);
	}

	index=args.flagIndex("et","endtime");
	if(MArgList::kInvalidArgIndex!=index)
	{
		args.get(index+1,endFrame);
	}

	index=args.flagIndex("n","nodes");
	if(MArgList::kInvalidArgIndex!=index)
	{
		unsigned int i=index+1;
		nodes=args.asStringArray(i);
	}

	index=args.flagIndex("nsm","namespaceMapping");
	if(MArgList::kInvalidArgIndex!=index)
	{
		unsigned int i=index+1;
		nameSpaceMappingArr=args.asStringArray(i);
	}

	//MStringArray allNameSpace;
	//getAllNameSpace(nodes,cacheVisDel,allNameSpace);
	cout<<"here0000"<<endl;
	vector<struct_prepareInfo> prepareData;
	collectPrepareData(isCloth,dirPath,cacheDirName,animDirName,cacheForce,nodes,cacheVisDel,prepareData,nameSpaceMappingArr);
	cout<<"1111"<<endl;
	/*
	
	MString nameSpace;
	MStringArray objsExport,objsDel;
	splitObjNames(cacheVisDel,nameSpace,objsDel);
	splitObjNames(nodes,nameSpace,objsExport);

	MString cacheFileName=dirPath+"/"+cacheDirName+"/"+nameSpace+"/"+"cache";
	MString visFileName=dirPath+"/"+cacheDirName+"/"+nameSpace+"/"+"vis";
	MString animDir=dirPath+"/"+animDirName+"/"+nameSpace+"/";

	//cout<<cacheFileName.asChar()<<endl;
	//cout<<visFileName.asChar()<<endl;
	//cout<<animDir.asChar()<<endl;


	MStringArray cache,anim,nothingToDo;
	diff_CacheAnim(nameSpace,objsExport,startFrame,endFrame,cacheForce,anim,cache,nothingToDo);

	//for(int i=0;i<anim.length();i++)
	//{
		
	//}

	MStringArray cacheNothingToDo,animNothingToDo;
	orSet(anim,nothingToDo,cacheNothingToDo);
	orSet(cache,nothingToDo,animNothingToDo);
	*/
	//cache_export(nameSpace,cache,objsExport,objsDel,cacheNothingToDo,cacheFileName,visFileName,startFrame,endFrame);

	//anim_export(nameSpace,anim,objsDel,animNothingToDo,animDir,startFrame);

	//vis_export(nameSpace,objsExport,objsDel,visFileName,startFrame,endFrame);

	//cout<<"here"<<endl;

	for(int i=0;i<prepareData.size();i=i+MAXNUM)
	{
		exportCacheVis(prepareData,i,min(i+MAXNUM,prepareData.size()),startFrame,endFrame);
	}
	cout<<"anim begin"<<endl;
	for(int i=0;i<prepareData.size();i++)
	{
		time_t t=time(0);
		char tmp[20];
		::strftime(tmp,sizeof(tmp),"%Y-%m-%d %X",localtime(&t));
		anim_export(prepareData[i].nameSpace,prepareData[i].animFileNamePre,tmp,prepareData[i].anims,prepareData[i].objDel,prepareData[i].animNothingToDo,prepareData[i].animDir,startFrame,endFrame);
	}

	return MS::kSuccess;
}

void vixo_cacheExport::exportCacheVis(vector<struct_prepareInfo>& prepareData,int beginIdx,int endIdx,int startFrame,int endFrame)
{
	vector<struct_visExportInfo> visExportData(endIdx-beginIdx);
	vector<struct_cacheExportInfo> cacheExportData(endIdx-beginIdx);

	time_t t=time(0);
	char tmp[20];
	::strftime(tmp,sizeof(tmp),"%Y-%m-%d %X",localtime(&t));
	cout<<"2222"<<startFrame<<" "<<endFrame<<" "<<beginIdx<<" "<<endIdx<<endl;;
	for(int i=beginIdx;i<endIdx;i++)
	{
		cout<<"3333"<<endl;
		diffCacheNewOld(prepareData[i],cacheExportData[i-beginIdx]);
		cout<<"3333"<<endl;
		prepareCacheData(startFrame,endFrame,tmp,prepareData[i],cacheExportData[i-beginIdx]);
		cout<<"3333"<<endl;
		prepareVisData(startFrame,endFrame,tmp,prepareData[i],visExportData[i-beginIdx]);
		cout<<"3333"<<endl;
	}
	MGlobal::displayInfo("=========begin export===========");
	for(int i=beginIdx;i<endIdx;i++)
	{
		MGlobal::displayInfo(prepareData[i].nameSpace);
	}


	exportPlayBlast(prepareData,beginIdx,endIdx,startFrame,endFrame,cacheExportData,visExportData);
	MGlobal::displayInfo("===========end export===========");
	for(int i=beginIdx;i<endIdx;i++)
	{
		cout<<"eeee0"<<endl;
		postCacheExport(startFrame,endFrame,tmp,prepareData[i],cacheExportData[i-beginIdx],prepareData[i].nameSpace);
		cout<<"eeee1"<<endl;
		postVisExport(startFrame,endFrame,tmp,prepareData[i],visExportData[i-beginIdx]);
		cout<<"eeee2"<<endl;
	}
}

void vixo_cacheExport::postCacheExport(int start,int end,char time[20],struct_prepareInfo& prepareDataEle,struct_cacheExportInfo& cacheDataEle,MString nmsp)
{
	//组织数据
	//cacheAdd
	//cacheAddTime cacheAddData cacheAddBasic cacheAddDags
	for(int i=0;i<cacheDataEle.cacheAdd.length();i++)
	{
		cacheDataEle.cacheAddData[i].posIndex[0]=sizeof(streampos)*(end-start+1);
		cacheDataEle.cacheAddData[i].actualFrameIndex.push_back(0);
		for(int j=1;j<end-start+1;j++)
		{
			int isequal=memcmp(&cacheDataEle.cacheAddData[i].pointPositions[j][0],&cacheDataEle.cacheAddData[i].pointPositions[j-1][0],sizeof(struct_position)*cacheDataEle.cacheAddBasic[i].vertexNum);
			if(isequal==0)//与上一帧数据相同
				cacheDataEle.cacheAddData[i].posIndex[j]=cacheDataEle.cacheAddData[i].posIndex[j-1];
			else
			{
				cacheDataEle.cacheAddBasic[i].actualFrameSize++;
				cacheDataEle.cacheAddData[i].actualFrameIndex.push_back(j);
				cacheDataEle.cacheAddData[i].posIndex[j]=cacheDataEle.cacheAddData[i].posIndex[j-1].operator+(sizeof(struct_position)*cacheDataEle.cacheAddBasic[i].vertexNum);
			}
		}
		cacheDataEle.cacheAddBasic[i].cacheBegin=cacheDataEle.cacheBeginBasis;
		cacheDataEle.cacheBeginBasis=cacheDataEle.cacheBeginBasis.operator+(	sizeof(streampos)*(end-start+1)+
			sizeof(struct_position)*(cacheDataEle.cacheAddBasic[i].actualFrameSize*cacheDataEle.cacheAddBasic[i].vertexNum));
	}

	//cacheUpdate
	//cacheUpdateTime cacheUpdateOldData cacheUpdateNewData cacheUpdateOldBasic cacheUpdateNewBasic cacheUpdateFlag cacheUpdateDags
	for(int i=0;i<cacheDataEle.cacheUpdate.length();i++)
	{
		cacheDataEle.cacheUpdateNewData[i].posIndex[0]=sizeof(streampos)*(end-start+1);
		cacheDataEle.cacheUpdateNewData[i].actualFrameIndex.push_back(0);
		for(int j=1;j<end-start+1;j++)
		{
			int isequal=memcmp(&cacheDataEle.cacheUpdateNewData[i].pointPositions[j][0],&cacheDataEle.cacheUpdateNewData[i].pointPositions[j-1][0],sizeof(struct_position)*cacheDataEle.cacheUpdateNewBasic[i].vertexNum);
			if(isequal==0)//与上一帧数据相同
				cacheDataEle.cacheUpdateNewData[i].posIndex[j]=cacheDataEle.cacheUpdateNewData[i].posIndex[j-1];
			else
			{
				cacheDataEle.cacheUpdateNewBasic[i].actualFrameSize++;
				cacheDataEle.cacheUpdateNewData[i].actualFrameIndex.push_back(j);
				cacheDataEle.cacheUpdateNewData[i].posIndex[j]=cacheDataEle.cacheUpdateNewData[i].posIndex[j-1].operator+(sizeof(struct_position)*cacheDataEle.cacheUpdateNewBasic[i].vertexNum);
			}
		}
		cacheDataEle.cacheUpdateNewBasic[i].cacheBegin=cacheDataEle.cacheBeginBasis;
		cacheDataEle.cacheBeginBasis=cacheDataEle.cacheBeginBasis.operator+(	sizeof(streampos)*(end-start+1)+
			sizeof(struct_position)*(cacheDataEle.cacheUpdateNewBasic[i].actualFrameSize*cacheDataEle.cacheUpdateNewBasic[i].vertexNum));
	}

	for(int i=0;i<cacheDataEle.cacheUpdate.length();i++)
	{
		if(cacheDataEle.cacheUpdateFlag[i]==true)
			continue;
		//cout<<"0"<<endl;
		//continue;
		if(cacheDataEle.cacheUpdateNewBasic[i].actualFrameSize!=cacheDataEle.cacheUpdateOldBasic[i].actualFrameSize)
		{
			cacheDataEle.cacheUpdateFlag[i]=true;
			continue;
		}

 		if(memcmp(&cacheDataEle.cacheUpdateNewData[i].posIndex[0],&cacheDataEle.cacheUpdateOldData[i].posIndex[0],sizeof(streampos)*(end-start+1))!=0)
 		{
 			cacheDataEle.cacheUpdateFlag[i]=true;
 			continue;
 		}

		int j=0;
		for(j=0;j<cacheDataEle.cacheUpdateNewBasic[i].actualFrameSize;j++)
		{
			if(memcmp(	&cacheDataEle.cacheUpdateNewData[i].pointPositions[cacheDataEle.cacheUpdateNewData[i].actualFrameIndex[j]][0],
				&cacheDataEle.cacheUpdateOldData[i].pointPositions[j][0],
				sizeof(struct_position)*cacheDataEle.cacheUpdateNewBasic[i].vertexNum)!=0)
			{
				break;
			}
		}
		if(j<cacheDataEle.cacheUpdateNewBasic[i].actualFrameSize)
		{
			cacheDataEle.cacheUpdateFlag[i]=true;
			continue;
		}
	}
	cacheDataEle.cacheUpdateOldData.clear();
	cacheDataEle.cacheUpdateOldBasic.clear();

	cout<<"cccc0"<<endl;
	cacheDataEle.getTopologyIngo(nmsp);
	cout<<"cccc1"<<endl;

	//写缓存文件
	MString cacheIndexFileName=prepareDataEle.cacheFileNamePre+".idat";
	MString cacheFileName=prepareDataEle.cacheFileNamePre+".ddat";
	cout<<cacheIndexFileName.asChar()<<endl;
	int data[3];
	data[0]=start;
	data[1]=end;
	data[2]=cacheDataEle.cacheAdd.length()+cacheDataEle.cacheKeep.length()+cacheDataEle.cacheUpdate.length();
	if(data[2]==0)
	{

 		fstream foutIndex(cacheIndexFileName.asChar(),ios_base::out|ios_base::trunc|ios_base::binary);
 		foutIndex.flush();
 		foutIndex.close();
 		fstream foutCache(cacheFileName.asChar(),ios_base::out|ios_base::trunc|ios_base::binary);
 		foutCache.flush();
 		foutCache.close();

	}
	else
	{
		fstream foutIndex(cacheIndexFileName.asChar(),ios_base::out|ios_base::trunc|ios_base::binary);

		foutIndex.write((char*)&data[0],sizeof(int)*3);
		foutIndex.write((char*)&cacheDataEle.cacheKeepTime[0],sizeof(struct_indexInfo)*cacheDataEle.cacheKeep.length());
		foutIndex.write((char*)&cacheDataEle.cacheAddTime[0],sizeof(struct_indexInfo)*cacheDataEle.cacheAdd.length());
		foutIndex.write((char*)&cacheDataEle.cacheUpdateTime[0],sizeof(struct_indexInfo)*cacheDataEle.cacheUpdate.length());
		foutIndex.flush();
		foutIndex.close();

		fstream foutCache(cacheFileName.asChar(),ios_base::out|ios_base::trunc|ios_base::binary);
		foutCache.write((char*)&data[2],sizeof(int));
		foutCache.write((char*)&cacheDataEle.cacheKeepBasic[0],sizeof(struct_basicObjInfo)*cacheDataEle.cacheKeep.length());
		//cout<<"test0:"<<cacheKeep.length()<<endl;
		foutCache.write((char*)&cacheDataEle.cacheAddBasic[0],sizeof(struct_basicObjInfo)*cacheDataEle.cacheAdd.length());
		//cout<<"test1:"<<cacheAdd.length()<<endl;
		foutCache.write((char*)&cacheDataEle.cacheUpdateNewBasic[0],sizeof(struct_basicObjInfo)*cacheDataEle.cacheUpdate.length());
		//cout<<"test1:"<<cacheUpdate.length()<<endl;
		//MString test("apple");
		//foutCache.write("apple",sizeof(char)*5);
		for(int i=0;i<cacheDataEle.cacheKeep.length();i++)
		{
			foutCache.write((char*)&cacheDataEle.cacheKeepData[i].posIndex[0],sizeof(streampos)*(end-start+1));
			for(int j=0;j<cacheDataEle.cacheKeepBasic[i].actualFrameSize;j++)
			{
				foutCache.write((char*)&cacheDataEle.cacheKeepData[i].pointPositions[j][0].x,sizeof(struct_position)*cacheDataEle.cacheKeepBasic[i].vertexNum);
			}
		}
		cacheDataEle.cacheKeepData.clear();
		for(int i=0;i<cacheDataEle.cacheAdd.length();i++)
		{
			foutCache.write((char*)&cacheDataEle.cacheAddData[i].posIndex[0],sizeof(streampos)*(end-start+1));
			for(int j=0;j<cacheDataEle.cacheAddBasic[i].actualFrameSize;j++)
			{
				foutCache.write((char*)&cacheDataEle.cacheAddData[i].pointPositions[cacheDataEle.cacheAddData[i].actualFrameIndex[j]][0].x,sizeof(struct_position)*cacheDataEle.cacheAddBasic[i].vertexNum);
			}
		}
		cacheDataEle.cacheAddData.clear();
		for(int i=0;i<cacheDataEle.cacheUpdate.length();i++)
		{
			foutCache.write((char*)&cacheDataEle.cacheUpdateNewData[i].posIndex[0],sizeof(streampos)*(end-start+1));
			for(int j=0;j<cacheDataEle.cacheUpdateNewBasic[i].actualFrameSize;j++)
			{
				foutCache.write((char*)&cacheDataEle.cacheUpdateNewData[i].pointPositions[cacheDataEle.cacheUpdateNewData[i].actualFrameIndex[j]][0].x,sizeof(struct_position)*cacheDataEle.cacheUpdateNewBasic[i].vertexNum);
			}
		}
		cacheDataEle.cacheUpdateNewData.clear();

		for(int i=0;i<cacheDataEle.cacheKeep.length();i++)
		{
			foutCache.write((char*)&cacheDataEle.cacheKeepMesh[i].vertexNum,sizeof(int));
			foutCache.write((char*)&cacheDataEle.cacheKeepMesh[i].faceNum,sizeof(int));
			foutCache.write((char*)&cacheDataEle.cacheKeepMesh[i].polygonConnNum,sizeof(int));
			foutCache.write((char*)&cacheDataEle.cacheKeepMesh[i].polygonCount[0],sizeof(int)*cacheDataEle.cacheKeepMesh[i].faceNum);
			foutCache.write((char*)&cacheDataEle.cacheKeepMesh[i].polygonConn[0],sizeof(int)*cacheDataEle.cacheKeepMesh[i].polygonConnNum);
		}

		for(int i=0;i<cacheDataEle.cacheAdd.length();i++)
		{
			foutCache.write((char*)&cacheDataEle.cacheAddMesh[i].vertexNum,sizeof(int));
			foutCache.write((char*)&cacheDataEle.cacheAddMesh[i].faceNum,sizeof(int));
			foutCache.write((char*)&cacheDataEle.cacheAddMesh[i].polygonConnNum,sizeof(int));
			foutCache.write((char*)&cacheDataEle.cacheAddMesh[i].polygonCount[0],sizeof(int)*cacheDataEle.cacheAddMesh[i].faceNum);
			foutCache.write((char*)&cacheDataEle.cacheAddMesh[i].polygonConn[0],sizeof(int)*cacheDataEle.cacheAddMesh[i].polygonConnNum);
		}

		for(int i=0;i<cacheDataEle.cacheUpdate.length();i++)
		{
			foutCache.write((char*)&cacheDataEle.cacheUpdateMesh[i].vertexNum,sizeof(int));
			foutCache.write((char*)&cacheDataEle.cacheUpdateMesh[i].faceNum,sizeof(int));
			foutCache.write((char*)&cacheDataEle.cacheUpdateMesh[i].polygonConnNum,sizeof(int));
			foutCache.write((char*)&cacheDataEle.cacheUpdateMesh[i].polygonCount[0],sizeof(int)*cacheDataEle.cacheUpdateMesh[i].faceNum);
			foutCache.write((char*)&cacheDataEle.cacheUpdateMesh[i].polygonConn[0],sizeof(int)*cacheDataEle.cacheUpdateMesh[i].polygonConnNum);
		}

		foutCache.flush();
		foutCache.close();
	}

	//导出更新日志
	MString cacheLogFileName=prepareDataEle.cacheFileNamePre+".log";
	fstream foutLog(cacheLogFileName.asChar(),ios_base::out|ios_base::trunc);
	for(int i=0;i<cacheDataEle.cacheAdd.length();i++)
	{
		foutLog<<"add:"<<cacheDataEle.cacheAdd[i].asChar()<<endl;
	}
	for(int i=0;i<cacheDataEle.cacheDel.length();i++)
	{
		foutLog<<"del:"<<cacheDataEle.cacheDel[i].asChar()<<endl;
	}
	for(int i=0;i<cacheDataEle.cacheUpdate.length();i++)
	{
		if(cacheDataEle.cacheUpdateFlag[i]==true)
		{
			foutLog<<"upd:"<<cacheDataEle.cacheUpdate[i].asChar()<<endl;
		}
	}
	foutLog.flush();
	foutLog.close();
	//cout<<"add:"<<cacheDataEle.cacheAdd[i].asChar()<<endl;
	//~导出更新日志
}

void vixo_cacheExport::postVisExport(int start,int end,char time[20],struct_prepareInfo& prepareDataEle,struct_visExportInfo& visDataEle)
{
	//MStringArray visExport,visNoInfo;
	//vector<char> visNewExportData;
	visDataEle.visExport.clear();
	visDataEle.visNoInfo.clear();
	for(int i=0;i<prepareDataEle.objDel.length();i++)
	{
		visDataEle.visNoInfo.append(prepareDataEle.objDel[i]);
	}
	visDataEle.visNewExportData.clear();
	for(int i=0;i<prepareDataEle.objExport.length();i++)
	{
		if(visDataEle.flag_write[i]==true)
			visDataEle.visExport.append(prepareDataEle.objExport[i]);
		else
			visDataEle.visNoInfo.append(prepareDataEle.objExport[i]);
	}

	visDataEle.visNewExportData.resize(visDataEle.visExport.length()*(end-start+1));
	for(int i=0,j=0;i<prepareDataEle.objExport.length();i++)
	{
		if(visDataEle.flag_write[i]==true)
			for(int k=0;k<end-start+1;k++)
			{
				visDataEle.visNewExportData[j]=visDataEle.allVisData[i*(end-start+1)+k];
				j++;
			}
	}
	//~导出新数据

	//组织vis集合更新数据
	//MStringArray visKeep,visAdd,visDel,visUpdate;
	//vector<struct_indexInfo> visKeepTime,visNewExportTime;
	//vector<struct_visBasicInfo> visKeepBasic,visNewExportBasic;
	//vector<char> visKeepData;
	MStringArray tempRes;
	minusSet(visDataEle.visFileExist,prepareDataEle.objExport,tempRes);
	minusSet(tempRes,prepareDataEle.objDel,visDataEle.visKeep);//visKeep=fileExist-vis-visNeedDel
	minusSet(visDataEle.visExport,visDataEle.visFileExist,visDataEle.visAdd);//visAdd=visExport-fileExist
	andSet(visDataEle.visExport,visDataEle.visFileExist,visDataEle.visUpdate);//visUpdate=visExport && fileExist
	andSet(visDataEle.visFileExist,visDataEle.visNoInfo,visDataEle.visDel);//visDel=fileExist && noInfo

	//比较数据，得到最后的visUpdate
	//vector<bool> visUpdateFlag(visUpdate.length(),false);
	visDataEle.visUpdateFlag.resize(visDataEle.visUpdate.length(),false);
	for(int i=0;i<visDataEle.visUpdate.length();i++)
	{
		int idx=inSetIndex(visDataEle.visUpdate[i],visDataEle.visFileExist);
		int newIdx=inSetIndex(visDataEle.visUpdate[i],visDataEle.visExport);
		if(memcmp(&visDataEle.visFileExistData[idx*(end-start+1)],&visDataEle.visNewExportData[newIdx*(end-start+1)],sizeof(char)*(end-start+1))!=0)
		{
			visDataEle.visUpdateFlag[i]=true;
		}
	}

	//根据集合准备数据
	//只需要准备visKeep的struct_indexInfo，struct_visBasicInfo，struct_visInfo
	// visExport的struct_indexInfo,struct_visBasicInfo，struct_visInfo
	streampos visBeginBasis=sizeof(struct_visBasicInfo)*(visDataEle.visKeep.length()+visDataEle.visExport.length())+sizeof(int);

	visDataEle.visKeepTime.clear();
	visDataEle.visKeepTime.resize(visDataEle.visKeep.length());
	visDataEle.visKeepBasic.clear();
	visDataEle.visKeepBasic.resize(visDataEle.visKeep.length());
	visDataEle.visKeepData.clear();
	visDataEle.visKeepData.resize(visDataEle.visKeep.length()*(end-start+1));

	for(int i=0;i<visDataEle.visKeep.length();i++)
	{
		int idx=inSetIndex(visDataEle.visKeep[i],visDataEle.visFileExist);
		strcpy(visDataEle.visKeepTime[i].objName,visDataEle.visKeep[i].asChar());
		strcpy(visDataEle.visKeepTime[i].lastestUpdateTime,visDataEle.visFileExistTime[idx].lastestUpdateTime);
		strcpy(visDataEle.visKeepBasic[i].objName,visDataEle.visKeep[i].asChar());
		visDataEle.visKeepBasic[i].startFrame=start;
		visDataEle.visKeepBasic[i].endFrame=end;
		visDataEle.visKeepBasic[i].visBegin=visBeginBasis;
		visBeginBasis=visBeginBasis.operator+(sizeof(char)*(end-start+1));
		for(int j=0;j<end-start+1;j++)
		{
			visDataEle.visKeepData[i*(end-start+1)+j]=visDataEle.visFileExistData[idx*(end-start+1)+j];
		}
	}

	visDataEle.visNewExportTime.clear();
	visDataEle.visNewExportTime.resize(visDataEle.visExport.length());
	visDataEle.visNewExportBasic.clear();
	visDataEle.visNewExportBasic.resize(visDataEle.visExport.length());

	//time_t t=time(0);
	//char tmp[20];
	//::strftime(tmp,sizeof(tmp),"%Y-%m-%d %X",localtime(&t));

	for(int i=0;i<visDataEle.visExport.length();i++)
	{
		strcpy(visDataEle.visNewExportTime[i].objName,visDataEle.visExport[i].asChar());
		strcpy(visDataEle.visNewExportTime[i].lastestUpdateTime,time);
		visDataEle.visNewExportBasic[i].startFrame=start;
		visDataEle.visNewExportBasic[i].endFrame=end;
		strcpy(visDataEle.visNewExportBasic[i].objName,visDataEle.visExport[i].asChar());
		visDataEle.visNewExportBasic[i].visBegin=visBeginBasis;
		visBeginBasis=visBeginBasis.operator+(sizeof(char)*(end-start+1));
	}
	//~组织vis集合更新数据

	//写vis数据
	MString visIndexFileName=prepareDataEle.visFileNamePre+".ivis";
	MString visFileName=prepareDataEle.visFileNamePre+".dvis";
	if(visDataEle.visKeep.length()+visDataEle.visExport.length()<=0)
	{

		fstream foutindex(visIndexFileName.asChar(),ios_base::out|ios_base::trunc|ios_base::binary);
		foutindex.flush();
		foutindex.close();
		fstream foutvis(visFileName.asChar(),ios_base::out|ios_base::trunc|ios_base::binary);
		foutvis.flush();
		foutvis.close();

	}
	else
	{
		fstream foutIndex(visIndexFileName.asChar(),ios_base::out|ios_base::trunc|ios_base::binary);
		int data[3];
		data[0]=start;
		data[1]=end;
		data[2]=visDataEle.visKeep.length()+visDataEle.visExport.length();
		foutIndex.write((char*)&data[0],sizeof(int)*3);
		if(visDataEle.visKeep.length()>0)
			foutIndex.write((char*)&visDataEle.visKeepTime[0],sizeof(struct_indexInfo)*visDataEle.visKeep.length());
		if(visDataEle.visExport.length()>0)
			foutIndex.write((char*)&visDataEle.visNewExportTime[0],sizeof(struct_indexInfo)*visDataEle.visExport.length());
		foutIndex.flush();
		foutIndex.close();

		fstream foutVis(visFileName.asChar(),ios_base::out|ios_base::trunc|ios_base::binary);
		foutVis.write((char*)&data[2],sizeof(int));
		if(visDataEle.visKeep.length()>0)
			foutVis.write((char*)&visDataEle.visKeepBasic[0],sizeof(struct_visBasicInfo)*visDataEle.visKeep.length());
		if(visDataEle.visExport.length()>0)
			foutVis.write((char*)&visDataEle.visNewExportBasic[0],sizeof(struct_visBasicInfo)*visDataEle.visExport.length());
		foutVis.write((char*)&visDataEle.visKeepData[0],sizeof(char)*(end-start+1)*visDataEle.visKeep.length());
		foutVis.write((char*)&visDataEle.visNewExportData[0],sizeof(char)*(end-start+1)*visDataEle.visExport.length());
		foutVis.flush();
		foutVis.close();
	}
	//~写vis数据

	//导出更新日志
	MString visLogFileName=prepareDataEle.visFileNamePre+".log";
	fstream foutLog(visLogFileName.asChar(),ios_base::out|ios_base::trunc);
	for(int i=0;i<visDataEle.visAdd.length();i++)
		foutLog<<"add:"<<visDataEle.visAdd[i].asChar()<<endl;
	for(int i=0;i<visDataEle.visDel.length();i++)
		foutLog<<"del:"<<visDataEle.visDel[i].asChar()<<endl;
	for(int i=0;i<visDataEle.visUpdate.length();i++)
	{
		if(visDataEle.visUpdateFlag[i]==true)
			foutLog<<"upd:"<<visDataEle.visUpdate[i].asChar()<<endl;
	}
	foutLog.flush();
	foutLog.close();
	//~导出更新日志
}

void vixo_cacheExport::exportPlayBlast(vector<struct_prepareInfo>& prepareData,int beginIdx,int endIdx,int start,int end,vector<struct_cacheExportInfo>& cacheData,vector<struct_visExportInfo>& visData)
{
	MPoint p;
	MSpace::Space space = MSpace::kWorld;
	//MTimer timer;
	for(int time=start;time<=end;time++)
	{
		MTime now((double)time);
		MAnimControl::setCurrentTime(now);

		//timer.beginTimer();
		for(int objIdx=0;objIdx<endIdx-beginIdx;objIdx++)
		{
			for(int i=0;i<cacheData[objIdx].cacheAdd.length();i++)
			{
				MFnMesh fnMesh(cacheData[objIdx].cacheAddDags[i]);
				MPointArray allPs;
				fnMesh.getPoints(allPs,space);
				for(int index=0;index<allPs.length();index++)
				{
					cacheData[objIdx].cacheAddData[i].pointPositions[time-start][index].x=(int(allPs[index].x*10000))/10000.0;
					cacheData[objIdx].cacheAddData[i].pointPositions[time-start][index].y=(int(allPs[index].y*10000))/10000.0;
					cacheData[objIdx].cacheAddData[i].pointPositions[time-start][index].z=(int(allPs[index].z*10000))/10000.0;
				}
			}

			for(int i=0;i<cacheData[objIdx].cacheUpdate.length();i++)
			{
				MFnMesh fnMesh(cacheData[objIdx].cacheUpdateDags[i]);
				MPointArray allPs;
				fnMesh.getPoints(allPs,space);
				for(int index=0;index<allPs.length();index++)
				{
					cacheData[objIdx].cacheUpdateNewData[i].pointPositions[time-start][index].x=(int(allPs[index].x*10000))/10000.0;
					cacheData[objIdx].cacheUpdateNewData[i].pointPositions[time-start][index].y=(int(allPs[index].y*10000))/10000.0;
					cacheData[objIdx].cacheUpdateNewData[i].pointPositions[time-start][index].z=(int(allPs[index].z*10000))/10000.0;
				}
			}

			for(int i=0;i<prepareData[objIdx+beginIdx].objExport.length();i++)
			{
				visData[objIdx].path[i]=visData[objIdx].path_backup[i];
				int path_length=visData[objIdx].path[i].length();
				bool boolf=true;
				for(int j=0;j<path_length;j++)
				{
					if(visData[objIdx].path[i].apiType()==MFn::kMesh)
					{
						MFnMesh mesh_node(visData[objIdx].path[i]);
						MPlug p=mesh_node.findPlug("ovv",true);
						boolf=boolf&&p.asBool();
						p=mesh_node.findPlug("v",true);
						boolf=boolf&&p.asBool();
					}
					else if(visData[objIdx].path[i].apiType()==MFn::kTransform)
					{
						MFnTransform trans_node(visData[objIdx].path[i]);
						MPlug p=trans_node.findPlug("ovv",true);
						boolf=boolf&&p.asBool();
						p=trans_node.findPlug("v",true);
						boolf=boolf&&p.asBool();
					}
					visData[objIdx].path[i].pop(1);
				}
				visData[objIdx].allVisData[i*(end-start+1)+time-start]=boolf;
				if(boolf==false)
					visData[objIdx].flag_write[i]=true;
			}
		}
		//timer.endTimer();
		//cout<<"exp:"<<timer.elapsedTime()<<endl;
	}	
}

void vixo_cacheExport::diffCacheNewOld(struct_prepareInfo& prepareDataEle,struct_cacheExportInfo& cacheDataEle)
{
	MString cacheIndexFileName=prepareDataEle.cacheFileNamePre+".idat";
	ifstream fin(cacheIndexFileName.asChar(),ios_base::in|ios_base::binary);
	bool eofFlag=false;
	ifstream cacheFinTest(cacheIndexFileName.asChar(),ios_base::in|ios_base::binary);
	if(cacheFinTest)
	{
		char eofTest;
		cacheFinTest.read((char*)&eofTest,sizeof(char));
		eofFlag=cacheFinTest.eof();
		cacheFinTest.close();
	}
	MString cacheFileName=prepareDataEle.cacheFileNamePre+".ddat";
	if(!fin||eofFlag) //第一次缓存导出
	{
		cacheDataEle.cacheFileExist.clear();
		cacheDataEle.cacheFileExistTime.clear();
		cacheDataEle.cacheFileExistBasic.clear();
	}
	else
	{
		int st_et_objNum[3];
		fin.read((char*)st_et_objNum,sizeof(int)*3);

		vector<struct_indexInfo> indexInfo(st_et_objNum[2]);
		fin.read((char*)&indexInfo[0],sizeof(struct_indexInfo)*st_et_objNum[2]);
		fin.close();

		cacheDataEle.cacheFileExist.clear();
		cacheDataEle.cacheFileExist.setLength(st_et_objNum[2]);
		cacheDataEle.cacheFileExistTime.clear();
		cacheDataEle.cacheFileExistTime.resize(st_et_objNum[2]);
		cacheDataEle.cacheFileExistBasic.clear();
		cacheDataEle.cacheFileExistBasic.resize(st_et_objNum[2]);

		for(int i=0;i<st_et_objNum[2];i++)
		{
			cacheDataEle.cacheFileExist[i]=MString(indexInfo[i].objName);
			::strcpy(cacheDataEle.cacheFileExistTime[i].lastestUpdateTime,indexInfo[i].lastestUpdateTime);
		}


		fin.open(cacheFileName.asChar(),ios_base::in|ios_base::binary);
		int objNum=0;
		fin.read((char*)&objNum,sizeof(int));
		fin.read((char*)&cacheDataEle.cacheFileExistBasic[0],sizeof(struct_basicObjInfo)*st_et_objNum[2]);
		//cout<<"mmm"<<cacheDataEle.cacheFileExistBasic[0].cacheBegin<<endl;
		fin.close();
	}

	for(int i=0;i<prepareDataEle.cache.length();i++)
		{
		//cout<<"debug:"<<prepareDataEle.cache[i].asChar()<<endl;
		}

	minusSet(prepareDataEle.cache,cacheDataEle.cacheFileExist,cacheDataEle.cacheAdd);//cacheAdd=caches-cacheFileExist
	andSet(prepareDataEle.cache,cacheDataEle.cacheFileExist,cacheDataEle.cacheUpdate);//cacheUpdate=caches&&cacheFileExist
	MStringArray cacheUnExist;
	orSet(prepareDataEle.objDel,prepareDataEle.cacheNothingToDo,cacheUnExist);
	andSet(cacheDataEle.cacheFileExist,cacheUnExist,cacheDataEle.cacheDel);//cacheDel=cacheFileExist&&cacheUnExist
	MStringArray tempResCache;
	orSet(cacheUnExist,prepareDataEle.cache,tempResCache);
	minusSet(cacheDataEle.cacheFileExist,tempResCache,cacheDataEle.cacheKeep);//cacheKeep=cacheFileExist-(cacheUnExist+caches)

}

void vixo_cacheExport::prepareCacheData(int start,int end,char time[20],struct_prepareInfo& prepareDataEle,struct_cacheExportInfo& cacheDataEle)
{
	cacheDataEle.cacheBeginBasis=sizeof(struct_basicObjInfo)*(cacheDataEle.cacheAdd.length()+cacheDataEle.cacheKeep.length()+cacheDataEle.cacheUpdate.length())+sizeof(int);

	//cacheKeep //如果时间长，可以直接创建一维数组来存储数据
	//cacheKeepTime cacheKeepData cacheKeepBasic
	//init cacheBegin数据
	cacheDataEle.cacheKeepTime.resize(cacheDataEle.cacheKeep.length());
	cacheDataEle.cacheKeepBasic.resize(cacheDataEle.cacheKeep.length());
	cacheDataEle.cacheKeepData.resize(cacheDataEle.cacheKeep.length());
	MString cacheFileName=prepareDataEle.cacheFileNamePre+".ddat";
	fstream fin(cacheFileName.asChar(),ios_base::in|ios_base::binary);
	for(int i=0;i<cacheDataEle.cacheKeep.length();i++)
	{
		//cout<<"keep:"<<cacheDataEle.cacheKeep[i].asChar()<<endl;
		::strcpy(cacheDataEle.cacheKeepTime[i].objName,cacheDataEle.cacheKeep[i].asChar());
		int idx=inSetIndex(cacheDataEle.cacheKeep[i],cacheDataEle.cacheFileExist);
		//cout<<idx<<endl;
		::strcpy(cacheDataEle.cacheKeepTime[i].lastestUpdateTime,cacheDataEle.cacheFileExistTime[idx].lastestUpdateTime);
		//cacheKeepBasic[i]=cacheFileExistBasic[idx];
		cacheDataEle.cacheKeepBasic[i].actualFrameSize=cacheDataEle.cacheFileExistBasic[idx].actualFrameSize;
		cacheDataEle.cacheKeepBasic[i].startFrame=cacheDataEle.cacheFileExistBasic[idx].startFrame;
		cacheDataEle.cacheKeepBasic[i].endFrame=cacheDataEle.cacheFileExistBasic[idx].endFrame;
		cacheDataEle.cacheKeepBasic[i].vertexNum=cacheDataEle.cacheFileExistBasic[idx].vertexNum;
		::strcpy(cacheDataEle.cacheKeepBasic[i].objName,cacheDataEle.cacheFileExistBasic[idx].objName);
		//cacheKeepBasic[i].cacheBegin=0;
		cacheDataEle.cacheKeepData[i].posIndex.resize(cacheDataEle.cacheKeepBasic[i].endFrame-cacheDataEle.cacheKeepBasic[i].startFrame+1);
		cacheDataEle.cacheKeepData[i].pointPositions.resize(cacheDataEle.cacheKeepBasic[i].actualFrameSize);
		for(int j=0;j<cacheDataEle.cacheKeepBasic[i].actualFrameSize;j++)
			cacheDataEle.cacheKeepData[i].pointPositions[j].resize(cacheDataEle.cacheKeepBasic[i].vertexNum);
		fin.seekg(cacheDataEle.cacheFileExistBasic[idx].cacheBegin);
		//cout<<i<<":"<<cacheFileExistBasic[idx].cacheBegin<<endl;
		fin.read((char*)&cacheDataEle.cacheKeepData[i].posIndex[0],sizeof(streampos)*(cacheDataEle.cacheKeepBasic[i].endFrame-cacheDataEle.cacheKeepBasic[i].startFrame+1));
		//cout<<i<<":"<<cacheKeepData[i].posIndex[0]<<endl;
		for(int j=0;j<cacheDataEle.cacheKeepBasic[i].actualFrameSize;j++)
		{
			fin.read((char*)&cacheDataEle.cacheKeepData[i].pointPositions[j][0].x,sizeof(struct_position)*cacheDataEle.cacheKeepBasic[i].vertexNum);
		}
		//cout<<"keep:"<<cacheKeep[i].asChar()<<endl;
		//if(i==0)
		cacheDataEle.cacheKeepBasic[i].cacheBegin=cacheDataEle.cacheBeginBasis;
		cacheDataEle.cacheBeginBasis=cacheDataEle.cacheBeginBasis.operator+(	sizeof(streampos)*(end-start+1)+
			sizeof(struct_position)*(cacheDataEle.cacheKeepBasic[i].actualFrameSize*cacheDataEle.cacheKeepBasic[i].vertexNum));
	}
	fin.close();

	MSelectionList slist;

	//cacheAdd
	//cacheAddTime cacheAddData cacheAddBasic cacheAddDags
	cacheDataEle.cacheAddTime.resize(cacheDataEle.cacheAdd.length());
	cacheDataEle.cacheAddBasic.resize(cacheDataEle.cacheAdd.length());
	cacheDataEle.cacheAddData.resize(cacheDataEle.cacheAdd.length());	
	cacheDataEle.cacheAddDags.setLength(cacheDataEle.cacheAdd.length());
	for(int i=0;i<cacheDataEle.cacheAdd.length();i++)
	{
		//cout<<"add:"<<cacheDataEle.cacheAdd[i].asChar()<<endl;
		strcpy(cacheDataEle.cacheAddTime[i].objName,cacheDataEle.cacheAdd[i].asChar());
		strcpy(cacheDataEle.cacheAddTime[i].lastestUpdateTime,time);

		slist.clear();
		MGlobal::getSelectionListByName(prepareDataEle.nameSpace+":"+cacheDataEle.cacheAdd[i],slist);
		cout<<"select:"<<prepareDataEle.nameSpace.asChar()<<":"<<cacheDataEle.cacheAdd[i].asChar()<<endl;
		slist.getDagPath(0,cacheDataEle.cacheAddDags[i]);
		MItMeshVertex vtxIter(cacheDataEle.cacheAddDags[i]);
		cout<<vtxIter.count()<<endl;
		cacheDataEle.cacheAddBasic[i].vertexNum=vtxIter.count();
		cacheDataEle.cacheAddBasic[i].startFrame=start;
		cacheDataEle.cacheAddBasic[i].endFrame=end;
		cacheDataEle.cacheAddBasic[i].actualFrameSize=1;
		cacheDataEle.cacheAddBasic[i].cacheBegin=0;
		strcpy(cacheDataEle.cacheAddBasic[i].objName,cacheDataEle.cacheAdd[i].asChar());

		cacheDataEle.cacheAddData[i].actualFrameIndex.clear();
		cacheDataEle.cacheAddData[i].posIndex.resize(end-start+1);
		cacheDataEle.cacheAddData[i].pointPositions.resize(end-start+1);
		for(int j=0;j<end-start+1;j++)
			cacheDataEle.cacheAddData[i].pointPositions[j].resize(cacheDataEle.cacheAddBasic[i].vertexNum);
	}

	//cout<<"add over"<<endl;

	//cacheUpdate
	//cacheUpdateTime cacheUpdateOldData cacheUpdateNewData cacheUpdateOldBasic cacheUpdateNewBasic cacheUpdateFlag cacheUpdateDags
	cacheDataEle.cacheUpdateTime.resize(cacheDataEle.cacheUpdate.length());
	cacheDataEle.cacheUpdateOldData.resize(cacheDataEle.cacheUpdate.length());
	cacheDataEle.cacheUpdateNewData.resize(cacheDataEle.cacheUpdate.length());
	cacheDataEle.cacheUpdateOldBasic.resize(cacheDataEle.cacheUpdate.length());
	cacheDataEle.cacheUpdateNewBasic.resize(cacheDataEle.cacheUpdate.length());
	cacheDataEle.cacheUpdateFlag.resize(cacheDataEle.cacheUpdate.length(),false);
	cacheDataEle.cacheUpdateDags.setLength(cacheDataEle.cacheUpdate.length());
	fin.open(cacheFileName.asChar(),ios_base::in|ios_base::binary);
	for(int i=0;i<cacheDataEle.cacheUpdate.length();i++)
	{
		//cout<<"update:"<<cacheDataEle.cacheUpdate[i].asChar()<<endl;
		strcpy(cacheDataEle.cacheUpdateTime[i].objName,cacheDataEle.cacheUpdate[i].asChar());
		strcpy(cacheDataEle.cacheUpdateTime[i].lastestUpdateTime,time);

		int idx=inSetIndex(cacheDataEle.cacheUpdate[i],cacheDataEle.cacheFileExist);

		cacheDataEle.cacheUpdateOldBasic[i].startFrame=cacheDataEle.cacheFileExistBasic[idx].startFrame;
		cacheDataEle.cacheUpdateOldBasic[i].endFrame=cacheDataEle.cacheFileExistBasic[idx].endFrame;
		cacheDataEle.cacheUpdateOldBasic[i].actualFrameSize=cacheDataEle.cacheFileExistBasic[idx].actualFrameSize;
		cacheDataEle.cacheUpdateOldBasic[i].cacheBegin=cacheDataEle.cacheFileExistBasic[idx].cacheBegin;
		cacheDataEle.cacheUpdateOldBasic[i].vertexNum=cacheDataEle.cacheFileExistBasic[idx].vertexNum;
		::strcpy(cacheDataEle.cacheUpdateOldBasic[i].objName,cacheDataEle.cacheFileExistBasic[idx].objName);

		strcpy(cacheDataEle.cacheUpdateNewBasic[i].objName,cacheDataEle.cacheUpdate[i].asChar());

		slist.clear();
		MGlobal::getSelectionListByName(prepareDataEle.nameSpace+":"+cacheDataEle.cacheUpdate[i],slist);
		slist.getDagPath(0,cacheDataEle.cacheUpdateDags[i]);
		MItMeshVertex vtxIter(cacheDataEle.cacheUpdateDags[i]);

		cacheDataEle.cacheUpdateNewBasic[i].startFrame=start;
		cacheDataEle.cacheUpdateNewBasic[i].endFrame=end;
		cacheDataEle.cacheUpdateNewBasic[i].actualFrameSize=1;
		cacheDataEle.cacheUpdateNewBasic[i].vertexNum=vtxIter.count();
		cacheDataEle.cacheUpdateNewBasic[i].cacheBegin=0;

		cacheDataEle.cacheUpdateNewData[i].actualFrameIndex.clear();
		cacheDataEle.cacheUpdateNewData[i].posIndex.resize(end-start+1);
		cacheDataEle.cacheUpdateNewData[i].pointPositions.resize(end-start+1);
		for(int j=0;j<end-start+1;j++)
			cacheDataEle.cacheUpdateNewData[i].pointPositions[j].resize(cacheDataEle.cacheUpdateNewBasic[i].vertexNum);

		if(cacheDataEle.cacheUpdateOldBasic[i].startFrame!=start||cacheDataEle.cacheUpdateOldBasic[i].endFrame!=end||cacheDataEle.cacheUpdateOldBasic[i].vertexNum!=cacheDataEle.cacheUpdateNewBasic[i].vertexNum)
		{
			cacheDataEle.cacheUpdateFlag[i]=true;
			continue;
		}

		cacheDataEle.cacheUpdateOldData[i].posIndex.resize(end-start+1);
		cacheDataEle.cacheUpdateOldData[i].pointPositions.resize(cacheDataEle.cacheUpdateOldBasic[i].actualFrameSize);
		for(int j=0;j<cacheDataEle.cacheUpdateOldBasic[i].actualFrameSize;j++)
			cacheDataEle.cacheUpdateOldData[i].pointPositions[j].resize(cacheDataEle.cacheUpdateOldBasic[i].vertexNum);
		fin.seekg(cacheDataEle.cacheUpdateOldBasic[i].cacheBegin);
		fin.read((char*)&cacheDataEle.cacheUpdateOldData[i].posIndex[0],sizeof(streampos)*(end-start+1));
		for(int j=0;j<cacheDataEle.cacheUpdateOldBasic[i].actualFrameSize;j++)
		{
			fin.read((char*)&cacheDataEle.cacheUpdateOldData[i].pointPositions[j][0],sizeof(struct_position)*cacheDataEle.cacheUpdateOldBasic[i].vertexNum);
		}
	}
	fin.close();
}

void vixo_cacheExport::prepareVisData(int start,int end,char time[20],struct_prepareInfo& prepareDataEle,struct_visExportInfo& visDataEle)
{
	//读取旧数据
	MString visIndexFileName=prepareDataEle.visFileNamePre+".ivis";
	fstream fin(visIndexFileName.asChar(),ios_base::in|ios_base::binary);
	bool eofFlag=false;
	ifstream visFinTest(visIndexFileName.asChar(),ios_base::in|ios_base::binary);
	if(visFinTest)
	{
		char eofTest;
		visFinTest.read((char*)&eofTest,sizeof(char));
		eofFlag=visFinTest.eof();
		visFinTest.close();
	}
	MString visFileName=prepareDataEle.visFileNamePre+".dvis";
	//cout<<"testapple"<<endl;
	if(!fin||eofFlag) //第一次vis导出
	{
		//cout<<"apple"<<endl;
		visDataEle.visFileExist.clear();
		visDataEle.visFileExistTime.clear();
		visDataEle.visFileExistData.clear();
	}
	else
	{

		int st_et_objNum[3];
		fin.read((char*)&st_et_objNum[0],sizeof(int)*3);

		vector<struct_indexInfo> indexInfo(st_et_objNum[2]);
		fin.read((char*)&indexInfo[0],sizeof(struct_indexInfo)*st_et_objNum[2]);
		fin.close();

		visDataEle.visFileExist.clear();
		visDataEle.visFileExist.setLength(st_et_objNum[2]);
		visDataEle.visFileExistTime.clear();
		visDataEle.visFileExistTime.resize(st_et_objNum[2]);
		visDataEle.visFileExistData.clear();
		visDataEle.visFileExistData.resize(st_et_objNum[2]*(end-start+1));

		for(int i=0;i<st_et_objNum[2];i++)
		{
			visDataEle.visFileExist[i]=MString(indexInfo[i].objName);
			strcpy(visDataEle.visFileExistTime[i].lastestUpdateTime,indexInfo[i].lastestUpdateTime);
		}

		fin.open(visFileName.asChar(),ios_base::in|ios_base::binary);
		int objNum=0;
		fin.read((char*)&objNum,sizeof(int));
		//vector<struct_visBasicInfo> visFileExistBasic(objNum);
		visDataEle.visFileExistBasic.resize(objNum);
		fin.read((char*)&visDataEle.visFileExistBasic[0],sizeof(struct_visBasicInfo)*st_et_objNum[2]);
		fin.read((char*)&visDataEle.visFileExistData[0],sizeof(char)*st_et_objNum[2]*(end-start+1));
		fin.close();
	}
	//~读取旧数据

	visDataEle.path.setLength(prepareDataEle.objExport.length());
	visDataEle.path_backup.setLength(prepareDataEle.objExport.length());
	visDataEle.path_trans.setLength(prepareDataEle.objExport.length());
	MSelectionList slist;
	for(int i=0;i<prepareDataEle.objExport.length();i++)
	{
		slist.clear();
		MGlobal::getSelectionListByName(prepareDataEle.nameSpace+":"+prepareDataEle.objExport[i],slist);

		MItSelectionList iter(slist,MFn::kMesh);
		iter.getDagPath(visDataEle.path[i]);

		MItSelectionList iter_trans(slist,MFn::kTransform);
		iter_trans.getDagPath(visDataEle.path_trans[i]);

		if(visDataEle.path[i].length()<=0)
			visDataEle.path_backup[i]=visDataEle.path_trans[i];
		else
			visDataEle.path_backup[i]=visDataEle.path[i];
	}

	visDataEle.allVisData.resize(prepareDataEle.objExport.length()*(end-start+1));
	visDataEle.flag_write.resize(prepareDataEle.objExport.length(),false);
}

void vixo_cacheExport::collectPrepareData(bool isCloth,MString dirPath,MString cacheDirName,MString animDirName,bool cacheForce,const MStringArray& allNodes,const MStringArray& objDel,vector<struct_prepareInfo>& prepareData,const MStringArray & nameSpaceMappingArr)
{
	MStringArray allNameSpace;
	getAllNameSpace(allNodes,objDel,allNameSpace);
	MStringArray allLigNameSpace;
	getAllLigNameSpace(allNameSpace,nameSpaceMappingArr,allLigNameSpace);

	prepareData.resize(allNameSpace.length());

	MString nameSpace,objName;
	for(int i=0;i<allNodes.length();i++)
	{
		//cout<<allNodes[i].asChar()<<endl;
		splitName(allNodes[i],nameSpace,objName);
		//cout<<nameSpace.asChar()<<" "<<objName.asChar()<<endl;
		int idx=inSetIndex(nameSpace,allNameSpace);
		prepareData[idx].objExport.append(objName);
	}
	for(int i=0;i<objDel.length();i++)
	{
		splitName(objDel[i],nameSpace,objName);
		int idx=inSetIndex(nameSpace,allNameSpace);
		prepareData[idx].objDel.append(objName);
	}
	for(int i=0;i<allNameSpace.length();i++)
	{
		prepareData[i].nameSpace=allNameSpace[i];
		prepareData[i].ligNameSpace=allLigNameSpace[i];
		if(isCloth)
		{
			prepareData[i].cacheFileNamePre=dirPath+"/"+prepareData[i].ligNameSpace+"/"+"cloth";
			prepareData[i].visFileNamePre="";
			prepareData[i].animFileNamePre="";
		}
		else
		{
			prepareData[i].cacheFileNamePre=dirPath+"/"+prepareData[i].ligNameSpace+"/"+"cache";
			prepareData[i].visFileNamePre=dirPath+"/"+prepareData[i].ligNameSpace+"/"+"vis";
			prepareData[i].animFileNamePre=dirPath+"/"+prepareData[i].ligNameSpace+"/"+"anim";
		}
		cout<<prepareData[i].cacheFileNamePre.asChar()<<endl;
		
		prepareData[i].animDir=dirPath+"/"+animDirName+"/"+prepareData[i].ligNameSpace+"/";
		diff_cacheAnimNothing(cacheForce,prepareData[i]);
	}
	
}

void vixo_cacheExport::getAllLigNameSpace(const MStringArray& allNameSpace,const MStringArray & nameSpaceMappingArr,MStringArray & ligNameSpace)
{
	MStringArray animNMS,ligNMS;
	animNMS.setLength(nameSpaceMappingArr.length()/2);
	ligNMS.setLength(nameSpaceMappingArr.length()/2);
	for(int i=0;i<nameSpaceMappingArr.length()/2;i++)
	{
		animNMS[i]=nameSpaceMappingArr[2*i];
		ligNMS[i]=nameSpaceMappingArr[2*i+1];
	}
	ligNameSpace.clear();
	ligNameSpace.setLength(allNameSpace.length());
	for(int i=0;i<allNameSpace.length();i++)
	{
		int idx=inSetIndex(allNameSpace[i],animNMS);
		if(idx==-1)
		{
			MGlobal::displayWarning("there is no nameSpace mapping information about "+allNameSpace[i]);
			ligNameSpace[i]=allNameSpace[i];
			continue;
		}
		ligNameSpace[i]=ligNMS[idx];
	}
}

void vixo_cacheExport::diff_cacheAnimNothing(bool cacheForce,struct_prepareInfo& prepareDataEle)
{
	prepareDataEle.cache.clear();
	prepareDataEle.anims.clear();
	prepareDataEle.nothingToDo.clear();
	prepareDataEle.cacheNothingToDo.clear();
	prepareDataEle.animNothingToDo.clear();

	if(cacheForce==true)
	{
		prepareDataEle.cache=prepareDataEle.objExport;
		prepareDataEle.animNothingToDo=prepareDataEle.objExport;
		return;
	}

	MSelectionList slist;
	MDagPath shapePath;
	bool isAnim=false;
	bool isCache=false;
	for(int i=0;i<prepareDataEle.objExport.length();i++)
	{
		slist.clear();
		MGlobal::getSelectionListByName(prepareDataEle.nameSpace+":"+prepareDataEle.objExport[i],slist);
		slist.getDagPath(0,shapePath);
		shapePath.extendToShape();

		MFnDagNode forTweak(shapePath);

		if(MAnimUtil::isAnimated(shapePath.node(),false)==true||forTweak.findPlug("tweakLocation").isConnected()) // key点
		{
			isAnim=false;
			isCache=true;
		}
		else
		{
			MPlugArray animedPlug;
			if(MAnimUtil::findAnimatedPlugs(shapePath.node(),animedPlug,true))//key过任何父级别的transform
			{
				int it=0;
				for(it=0;it<animedPlug.length();it++)
				{
					MObjectArray curveArr;
					if(MAnimUtil::findAnimation(animedPlug[it],curveArr))
					{
						MObject animCurveNode = curveArr[0];
						MFnAnimCurve animCurve (animCurveNode);
						int numKeys = animCurve.numKeys();
						if (numKeys>1)//不是仅仅key了一下 nocache 0 animed 1
						{
							isCache=true;
							isAnim=false;
							break;
						}
					}
				}
				if(it==animedPlug.length())//仅仅key了一下
				{
					isCache=false;
					isAnim=true;
				}

			}
			else //没有key过，但是可能动过
			{
				MCommandResult result;

				MGlobal::executeCommand("getAttr "+prepareDataEle.nameSpace+":"+prepareDataEle.objExport[i]+".worldMatrix[0]",result);
				MDoubleArray matrix_result;
				result.getResult(matrix_result);
				
				double matrix_array[4][4];
				double *p=&matrix_array[0][0];
				for(int it=0;it<matrix_result.length();it++)
				{
					*p=matrix_result[it];
					p++;
				}
				MMatrix matrix(matrix_array);
				if(matrix.isEquivalent(matrix.identity))//没有动过，不导出
				{
					isAnim=false;
					isCache=false;
				}
				else //仅仅动了一下，但是没有key帧，导出动画
				{
					isAnim=true;
					isCache=false;
				}
			}
		}

		if(isAnim==true)
			prepareDataEle.anims.append(prepareDataEle.objExport[i]);
		else if(isCache==true)
			prepareDataEle.cache.append(prepareDataEle.objExport[i]);
		else
			prepareDataEle.nothingToDo.append(prepareDataEle.objExport[i]);
	}

	orSet(prepareDataEle.anims,prepareDataEle.nothingToDo,prepareDataEle.cacheNothingToDo);
	orSet(prepareDataEle.cache,prepareDataEle.nothingToDo,prepareDataEle.animNothingToDo);
}

void vixo_cacheExport::getAllNameSpace(const MStringArray& allNodes,const MStringArray& objDel,MStringArray& allNameSpace)
{
	allNameSpace.clear();
	MString nameSpace,objName;
	for(int i=0;i<allNodes.length();i++)
	{
		splitName(allNodes[i],nameSpace,objName);
		if(!inSet(nameSpace,allNameSpace))
			allNameSpace.append(nameSpace);
	}
	for(int i=0;i<objDel.length();i++)
	{
		splitName(objDel[i],nameSpace,objName);
		if(!inSet(nameSpace,allNameSpace))
			allNameSpace.append(nameSpace);
	}
}

void vixo_cacheExport::splitName(const MString& node,MString & nameSpace,MString & objName)
{
	MStringArray tempSplit,temp4prefix;
	node.split('|',tempSplit);
	tempSplit[tempSplit.length()-1].split(':',temp4prefix);
	nameSpace=temp4prefix[0];
	objName=MString("");
	for(int i=1;i<temp4prefix.length()-1;i++)
	{
		objName=objName+temp4prefix[i]+MString(":");
	}
	objName=objName+temp4prefix[temp4prefix.length()-1];
	cout<<"mmmm:"<<tempSplit[tempSplit.length()-1].asChar()<<endl;
	//cout<<"aaaa:"<<temp4prefix.length()<<objName.asChar()<<endl;
// 	temp4prefix[1]+":"+temp4prefix[2]
// 
// 	objName=tempSplit[tempSplit.length()-1];
// 
// 	node.split('|',tempSplit);
// 	tempSplit[tempSplit.length()-1].split(':',temp4prefix);
// 	nameSpace=temp4prefix[temp4prefix.length()-2];
}

void vixo_cacheExport::anim_export(MString nameSpace,MString animFileNamePre,char time[20],MStringArray anims,MStringArray animNeedDel,MStringArray nothingToDo,MString dirPath,int start,int end)
{
	MStringArray animAdd,animKeep,animUpdate,animDel,animInFile;
	vector<structAnimInfo> animAddData,animKeepData,animUpdateData,animInFileData;

	MString animFileName=animFileNamePre+".danim";
	MString animIndexFileName=animFileNamePre+".ianim";
	MString animLogFileName=animFileNamePre+".log";

	//读取旧数据
	getAnimOldData(animFileName,animIndexFileName,animInFile,animInFileData);
	//~读取旧数据

	//for(int i=0;i<animInFile.length();i++)
	//	cout<<animInFile[i].asChar()<<endl;

	MStringArray allNeedDel;
	orSet(animNeedDel,nothingToDo,allNeedDel);
	andSet(animInFile,allNeedDel,animDel);

	//get new anim Datas
	MStringArray objNames(anims.length(),MString(""));
	for(int i=0;i<anims.length();i++)
	{
		objNames[i]=nameSpace+":"+anims[i];
	}
	vixo_matrixDecompose matrixDecompose;
	MDoubleArray matrixResult=matrixDecompose.compute_matrix(start,objNames);
	//~get new anim Datas

	for(int i=0;i<anims.length();i++)
	{
		int index=inSetIndex(anims[i],animInFile);
		if(index==-1)
		{
			//cout<<anims[i]<<endl;
			animAdd.append(anims[i]);
			structAnimInfo ele(anims[i],time,matrixResult,i);
			animAddData.push_back(ele);
		}
		else
		{
			bool flag=false;
			for(int  j=0;j<12;j++)
				if(abs(matrixResult[12*i+j]-animInFileData[index].data[j])>=0.001)
					flag=true;

			if(flag)//update
			{
				animUpdate.append(anims[i]);
				structAnimInfo ele(anims[i],time,matrixResult,i);
				animUpdateData.push_back(ele);
			}
			else
			{
				animKeep.append(anims[i]);
				structAnimInfo ele(anims[i],time,matrixResult,i);
				animKeepData.push_back(ele);
			}
		}
	}

	for(int i=0;i<animInFile.length();i++)
	{
		if((!inSet(animInFile[i],anims))&&(!inSet(animInFile[i],animDel)))
		{
			//stop
			animKeep.append(animInFile[i]);
			animKeepData.push_back(animInFileData[i]);
		}
	}

	if(animKeep.length()+animAdd.length()+animUpdate.length()<=0)
	{
		fstream foutIndex(animIndexFileName.asChar(),ios_base::out|ios_base::trunc|ios_base::binary);
		foutIndex.flush();
		foutIndex.close();
		fstream foutAnim(animFileName.asChar(),ios_base::out|ios_base::trunc|ios_base::binary);
		foutAnim.flush();
		foutAnim.close();
	}
	else
	{
		fstream foutIndex(animIndexFileName.asChar(),ios_base::out|ios_base::trunc|ios_base::binary);
		int data[3];
		data[0]=start;
		data[1]=end;
		data[2]=animKeep.length()+animAdd.length()+animUpdate.length();
		foutIndex.write((char*)&data[0],sizeof(int)*3);
		for(int i=0;i<animKeep.length();i++)
		{
			foutIndex.write((char*)&animKeepData[i].objName[0],sizeof(char)*255);
			foutIndex.write((char*)&animKeepData[i].lastestUpdateTime[0],sizeof(char)*20);
		}
		for(int i=0;i<animUpdate.length();i++)
		{
			foutIndex.write((char*)&animUpdateData[i].objName[0],sizeof(char)*255);
			foutIndex.write((char*)&animUpdateData[i].lastestUpdateTime[0],sizeof(char)*20);
		}
		for(int i=0;i<animAdd.length();i++)
		{
			foutIndex.write((char*)&animAddData[i].objName[0],sizeof(char)*255);
			foutIndex.write((char*)&animAddData[i].lastestUpdateTime[0],sizeof(char)*20);
		}
		foutIndex.flush();
		foutIndex.close();

		fstream foutAnim(animFileName.asChar(),ios_base::out|ios_base::trunc|ios_base::binary);
		for(int i=0;i<animKeep.length();i++)
		{
			foutAnim.write((char*)&animKeepData[i].data[0],sizeof(double)*12);
		}
		for(int i=0;i<animUpdate.length();i++)
		{
			foutAnim.write((char*)&animUpdateData[i].data[0],sizeof(double)*12);
		}
		for(int i=0;i<animAdd.length();i++)
		{
			foutAnim.write((char*)&animAddData[i].data[0],sizeof(double)*12);
		}
		foutAnim.flush();
		foutAnim.close();
	}


	//anim log
	fstream foutLog(animLogFileName.asChar(),ios_base::out|ios_base::trunc);
	for(int i=0;i<animAdd.length();i++)
		foutLog<<"add:"<<animAdd[i].asChar()<<endl;
	for(int i=0;i<animUpdate.length();i++)
		foutLog<<"upd:"<<animUpdate[i].asChar()<<endl;
	for(int i=0;i<animDel.length();i++)
		foutLog<<"del:"<<animDel[i].asChar()<<endl;
	foutLog.flush();
	foutLog.close();
}

void vixo_cacheExport::getAnimOldData(MString animFileName,MString animIndexFileName,MStringArray& animInFile,vector<structAnimInfo>& animInFileData)
{
	fstream fin(animIndexFileName.asChar(),ios_base::in|ios_base::binary);
	bool eofFlag=false;
	ifstream animFinTest(animIndexFileName.asChar(),ios_base::in|ios_base::binary);
	if(animFinTest)
	{
		char eofTest;
		animFinTest.read((char*)&eofTest,sizeof(char));
		eofFlag=animFinTest.eof();
		animFinTest.close();
	}
	if(!fin||eofFlag)
	{
		animInFile.clear();
		animInFileData.clear();
		fin.close();
	}
	else
	{
		int objNum[3];
		fin.read((char*)&objNum[0],sizeof(int)*3);
		animInFile.setLength(objNum[2]);
		animInFileData.resize(objNum[2]);
		for(int i=0;i<objNum[2];i++)
		{
			fin.read((char*)&animInFileData[i].objName[0],sizeof(char)*255);
			fin.read((char*)&animInFileData[i].lastestUpdateTime[0],sizeof(char)*20);
			animInFile[i]=MString(animInFileData[i].objName);
		}
		fin.close();

		fstream finData(animFileName.asChar(),ios_base::in|ios_base::binary);
		for(int i=0;i<objNum[2];i++)
		{
			finData.read((char*)&animInFileData[i].data[0],sizeof(double)*12);
		}
		finData.close();
	}
}

bool vixo_cacheExport::inSet(MString element,MStringArray& set)
{
	for(int i=0;i<set.length();i++)
	{
		if(element==set[i])
			return true;
	}
	return false;
}

void vixo_cacheExport::orSet(MStringArray& seta,MStringArray& setb,MStringArray& res)
{
	res.clear();
	res=seta;
	for(int i=0;i<setb.length();i++)
	{
		if(!inSet(setb[i],seta))
			res.append(setb[i]);
	}
}

void vixo_cacheExport::minusSet(MStringArray& seta,MStringArray& setb,MStringArray& res)
{
	res.clear();
	for(int i=0;i<seta.length();i++)
	{
		if(!inSet(seta[i],setb))
		{
			res.append(seta[i]);
			//cout<<"minus"<<seta[i].asChar()<<endl;
		}
			
	}
}

void vixo_cacheExport::andSet(MStringArray& seta,MStringArray& setb,MStringArray& res)
{
	res.clear();
	for(int i=0;i<setb.length();i++)
	{
		if(inSet(setb[i],seta))
		{
			//cout<<setb[i].asChar()<<endl;
			res.append(setb[i]);
			//cout<<setb[i].asChar()<<endl;
		}
	}
}

int vixo_cacheExport::inSetIndex(MString element,MStringArray& set)
{
	for(int i=0;i<set.length();i++)
	{
		if(element==set[i])
			return i;
	}
	return -1;
}

double vixo_cacheExport::asDouble(fstream & fin)
{
	advance(fin);
	double value;
	fin>>value;
	return value;
}

void vixo_cacheExport::advance(fstream & fin)
{
	while(fin)
	{
		fin>>ws;
		break;
	}
}

