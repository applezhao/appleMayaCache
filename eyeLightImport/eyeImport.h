
#pragma once
#include <maya/MFnPlugin.h>
#include <maya\MPxNode.h>
#include <map>
class eyeImport:public MPxNode
	{
	public:
		eyeImport(void);
		virtual ~eyeImport(void);

		virtual MStatus		compute( const MPlug& plug, MDataBlock& data );
		static  void*		creator();
		static  MStatus		initialize();

		virtual MStatus connectionMade(const MPlug& plug,const MPlug& otherPlug,bool asSrc);
		virtual MStatus connectionBroken(const MPlug& plug,const MPlug& otherPlug,bool asSrc);

		virtual MStatus setDependentsDirty( const MPlug& plugBeingDirtied,MPlugArray &affectedPlugs );


	public:
		static MObject time;
		static MObject file;
		static MObject translate;

		static MTypeId id;

	private:
		map<int,string> mapObjName;
	};

MStatus initializePlugin( MObject obj )

	{ 
	MStatus   status;
	MFnPlugin implugin( obj, "", "2011", "Any");
	status = implugin.registerNode( "eyeImport", eyeImport::id, eyeImport::creator,
		eyeImport::initialize );
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

	status = implugin.deregisterNode( eyeImport::id );
	if (!status) {
		status.perror("deregisterNode");
		return status;
		}

	return status;
	}

