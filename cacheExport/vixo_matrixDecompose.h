#pragma once
#include <maya\MMatrix.h>
#include <maya\MEulerRotation.h>
#include <maya\MDoubleArray.h>
#include <maya\MStringArray.h>
#include <maya\MPoint.h>
#include <maya\MVector.h>
#include <maya\MAnimControl.h>
#include <maya\MTime.h>
#include <maya\MGlobal.h>
#include <maya\MSelectionList.h>
#include <maya\MDagPath.h>
#include <maya\MFnTransform.h>
#include <maya\MQuaternion.h>
#include <maya\MCommandResult.h>
class vixo_matrixDecompose
{
public:
	MMatrix& build_rotate_matrix(MEulerRotation mer);
	MMatrix& build_rotate_pivot_matrix(MPoint rotatepivot);
	MMatrix& build_scale_pivot_matrix(MPoint scalepivot);
	MMatrix& build_scale_matrix(double scale []);
	MMatrix& build_shear_matrix(double shear []);
	MMatrix& build_rotate_axis_matrix(MEulerRotation mero);
	MMatrix& build_rotate_pivot_translate_matrix(MVector rotatePivotTranslate);
	MMatrix& build_scale_pivot_translate_matrix(MVector scalePivotTranslate);

	MDoubleArray compute_matrix(int time, MStringArray nodes);
};

