#include "vixo_matrixDecompose.h"


MMatrix& vixo_matrixDecompose::build_rotate_matrix(MEulerRotation mer)
{
	double rotateX_array[4][4]={0},rotateY_array[4][4]={0},rotateZ_array[4][4]={0},rotateAxisX_array[4][4]={0},
			rotateAxisY_array[4][4]={0},rotateAxisZ_array[4][4]={0};
		double rx=mer.x;
		rotateX_array[1][1]=cos(rx);
		rotateX_array[1][2]=sin(rx);
		rotateX_array[2][1]=-sin(rx);
		rotateX_array[2][2]=cos(rx);

		rotateX_array[0][0]=1;
		rotateX_array[3][3]=1;

		double ry=mer.y;
		rotateY_array[0][0]=cos(ry);
		rotateY_array[0][2]=-sin(ry);
		rotateY_array[2][0]=sin(ry);
		rotateY_array[2][2]=cos(ry);

		rotateY_array[1][1]=1;
		rotateY_array[3][3]=1;

		double rz=mer.z;
		rotateZ_array[0][0]=cos(rz);
		rotateZ_array[0][1]=sin(rz);
		rotateZ_array[1][0]=-sin(rz);
		rotateZ_array[1][1]=cos(rz);

		rotateZ_array[2][2]=1;
		rotateZ_array[3][3]=1;

		MMatrix rotate_matrix(rotateX_array);
		MMatrix rotateY_matrix(rotateY_array);
		MMatrix rotateZ_matrix(rotateZ_array);
		rotate_matrix*=rotateY_matrix;
		rotate_matrix*=rotateZ_matrix;

		return rotate_matrix;
}

MMatrix& vixo_matrixDecompose::build_rotate_pivot_matrix(MPoint rotatepivot)
{
	double rotatePivot_array[4][4]={0};
	rotatePivot_array[3][0]=rotatepivot.x;
	rotatePivot_array[3][1]=rotatepivot.y;
	rotatePivot_array[3][2]=rotatepivot.z;

	rotatePivot_array[0][0]=1;
	rotatePivot_array[1][1]=1;
	rotatePivot_array[2][2]=1;
	rotatePivot_array[3][3]=1;

	MMatrix rotatePivot_matrix(rotatePivot_array);
	return rotatePivot_matrix;
}

MMatrix& vixo_matrixDecompose::build_scale_pivot_matrix(MPoint scalepivot)
{
	double scalePivot_array[4][4]={0};
	scalePivot_array[3][0]=scalepivot.x;
	scalePivot_array[3][1]=scalepivot.y;
	scalePivot_array[3][2]=scalepivot.z;

	scalePivot_array[0][0]=1;
	scalePivot_array[1][1]=1;
	scalePivot_array[2][2]=1;
	scalePivot_array[3][3]=1;
	MMatrix scalePivot_matrix(scalePivot_array);
	return scalePivot_matrix;
}

MMatrix& vixo_matrixDecompose::build_scale_matrix(double scale [])
{
	double scale_array[4][4]={0};
	scale_array[0][0]=scale[0];
	scale_array[1][1]=scale[1];
	scale_array[2][2]=scale[2];
	scale_array[3][3]=1;
	MMatrix scale_matrix(scale_array);
	return scale_matrix;
}

MMatrix& vixo_matrixDecompose::build_shear_matrix(double shear [])
{
	double shear_array[4][4]={0};
	shear_array[1][0]=shear[0];
	shear_array[2][0]=shear[1];
	shear_array[2][1]=shear[2];

	shear_array[0][0]=1;
	shear_array[1][1]=1;
	shear_array[2][2]=1;
	shear_array[3][3]=1;
	MMatrix shear_matrix(shear_array);
	return shear_matrix;
}

MMatrix& vixo_matrixDecompose::build_rotate_axis_matrix(MEulerRotation mero)
{
	double rotateAxisX_array[4][4]={0},rotateAxisY_array[4][4]={0},rotateAxisZ_array[4][4]={0};
	double rax=mero.x;
	rotateAxisX_array[1][1]=cos(rax);
	rotateAxisX_array[1][2]=sin(rax);
	rotateAxisX_array[2][1]=-sin(rax);
	rotateAxisX_array[2][2]=cos(rax);

	rotateAxisX_array[0][0]=1;
	rotateAxisX_array[3][3]=1;

	double ray=mero.y;
	rotateAxisY_array[0][0]=cos(ray);
	rotateAxisY_array[0][2]=-sin(ray);
	rotateAxisY_array[2][0]=sin(ray);
	rotateAxisY_array[2][2]=cos(ray);

	rotateAxisY_array[1][1]=1;
	rotateAxisY_array[3][3]=1;

	double raz=mero.z;
	rotateAxisZ_array[0][0]=cos(raz);
	rotateAxisZ_array[0][1]=sin(raz);
	rotateAxisZ_array[1][0]=-sin(raz);
	rotateAxisZ_array[1][1]=cos(raz);

	rotateAxisZ_array[2][2]=1;
	rotateAxisZ_array[3][3]=1;

	MMatrix rotateAxis_matrix(rotateAxisX_array);
	MMatrix rotateAxisY_matrix(rotateAxisY_array);
	MMatrix rotateAxisZ_matrix(rotateAxisZ_array);
	rotateAxis_matrix*=rotateAxisY_matrix;
	rotateAxis_matrix*=rotateAxisZ_matrix;

	return rotateAxis_matrix;
}

MMatrix& vixo_matrixDecompose::build_rotate_pivot_translate_matrix(MVector rotatePivotTranslate)
{
	double rotatePivotTranslate_array[4][4]={0};
	rotatePivotTranslate_array[3][0]=rotatePivotTranslate.x;
	rotatePivotTranslate_array[3][1]=rotatePivotTranslate.y;
	rotatePivotTranslate_array[3][2]=rotatePivotTranslate.z;

	rotatePivotTranslate_array[0][0]=1;
	rotatePivotTranslate_array[1][1]=1;
	rotatePivotTranslate_array[2][2]=1;
	rotatePivotTranslate_array[3][3]=1;

	MMatrix rotatePivotTranslate_matrix(rotatePivotTranslate_array);
	return rotatePivotTranslate_matrix;
}

MMatrix& vixo_matrixDecompose::build_scale_pivot_translate_matrix(MVector scalePivotTranslate)
{
	double scalePivotTranslate_array[4][4]={0};
	scalePivotTranslate_array[3][0]=scalePivotTranslate.x;
	scalePivotTranslate_array[3][1]=scalePivotTranslate.y;
	scalePivotTranslate_array[3][2]=scalePivotTranslate.z;

	scalePivotTranslate_array[0][0]=1;
	scalePivotTranslate_array[1][1]=1;
	scalePivotTranslate_array[2][2]=1;
	scalePivotTranslate_array[3][3]=1;

	MMatrix scalePivotTranslate_matrix(scalePivotTranslate_array);
	return scalePivotTranslate_matrix;
}

MDoubleArray vixo_matrixDecompose::compute_matrix(int time, MStringArray nodes)
{
	MStatus stat = MS::kSuccess;

	int size=nodes.length();
	MDoubleArray ret(12*size);
	MSpace::Space space=MSpace::Space::kWorld;
	for(int iter=0;iter<size;iter++)
	{
		MTime now((double)time);
		MAnimControl::setCurrentTime(now);
		//MGlobal::clearSelectionList();
		//MGlobal::selectByName(nodes[iter],MGlobal::ListAdjustment::kAddToList);
		MSelectionList list;
		list.clear();
		//MGlobal::getActiveSelectionList(list);
		MGlobal::getSelectionListByName(nodes[iter],list);
		MDagPath path;
		MObject obj;
		list.getDagPath(0,path);
		MFnTransform transformnode(path);

		MQuaternion mq;
		transformnode.getRotation(mq,space);
		MEulerRotation mer=mq.asEulerRotation();
		MQuaternion mo=transformnode.rotateOrientation(space);
		MEulerRotation mero=mo.asEulerRotation();
		ret[12*iter+3]=(mer.x+mero.x)*180/M_PI;
		ret[12*iter+4]=(mer.y+mero.y)*180/M_PI;
		ret[12*iter+5]=(mer.z+mero.z)*180/M_PI;	

		double matrix_array[4][4];
		MCommandResult result;
		MGlobal::executeCommand("getAttr "+nodes[iter]+".worldMatrix[0]",result);
		MDoubleArray matrix_result;
		result.getResult(matrix_result);			
		double *p=&matrix_array[0][0];
		for(int i=0;i<matrix_result.length();i++)
		{
			*p=matrix_result[i];
			p++;
		}
		//delete p;
		MMatrix matrix(matrix_array);

		MTransformationMatrix fnOldMat(matrix);

		double scale[3];
		fnOldMat.getScale(scale,MSpace::kWorld);
		ret[12*iter+6]=scale[0];
		ret[12*iter+7]=scale[1];
		ret[12*iter+8]=scale[2];

		double shear[3];
		//transformnode.getShear(shear);
		fnOldMat.getShear(shear,MSpace::kWorld);
		ret[12*iter+9]=shear[0];
		ret[12*iter+10]=shear[1];
		ret[12*iter+11]=shear[2];

		//
		MVector rotatePivotTranslate=transformnode.rotatePivotTranslation(MSpace::kTransform);
		MMatrix rotatePivotTranslate_matrix=build_rotate_pivot_translate_matrix(rotatePivotTranslate);

		MVector scalePivotTranslate=transformnode.scalePivotTranslation(MSpace::kTransform);
		MMatrix scalePivotTranslate_matrix=build_scale_pivot_translate_matrix(scalePivotTranslate);
		//

		MPoint rotatepivot=transformnode.rotatePivot(MSpace::kTransform);
		MMatrix rotatePivot_matrix=build_rotate_pivot_matrix(rotatepivot);

		MPoint scalepivot=transformnode.scalePivot(MSpace::kTransform);
		MMatrix scalePivot_matrix=build_scale_pivot_matrix(scalepivot);

		MMatrix rotate_matrix=build_rotate_matrix(mer);
		MMatrix scale_matrix=build_scale_matrix(scale);
		MMatrix shear_matrix=build_shear_matrix(shear);
		MMatrix rotateAxis_matrix=build_rotate_axis_matrix(mero);
		
		matrix=rotatePivotTranslate_matrix.inverse()*rotatePivot_matrix.inverse()*rotate_matrix.inverse()*rotateAxis_matrix.inverse()*rotatePivot_matrix*scalePivotTranslate_matrix.inverse()*scalePivot_matrix.inverse()*shear_matrix.inverse()*scale_matrix.inverse()*scalePivot_matrix*matrix;
		//matrix=rotatePivot_matrix.inverse()*rotate_matrix.inverse()*rotateAxis_matrix.inverse()*rotatePivot_matrix*scalePivot_matrix.inverse()*shear_matrix.inverse()*scale_matrix.inverse()*scalePivot_matrix*matrix;
		//MTransformationMatrix fnmatrix(matrix);
		//MVector translate=fnmatrix.getTranslation(space);
		ret[12*iter]=matrix(3,0);
		//translate.x;
		ret[12*iter+1]=matrix(3,1);
		ret[12*iter+2]=matrix(3,2);
	}

	for(int i=0;i<12*size;i++)
	{
		ret[i]=(int(ret[i]*10000))/10000.0;
	}

	return ret;
}

