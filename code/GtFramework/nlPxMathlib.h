/*
  @file   nlPxMathlib.h
  
  @author Liaosheng

  @brief 

  更改日志 history
  ver:1.0
 */

#ifndef _NlPxMathLib_h_
#define _NlPxMathLib_h_

#include "prerequisite.h"

const float INFINITY_F = 3.4e+038f;

//---------------------------------------------------------------------------------
inline float3 CovToFloat3( NxVec3 vec )
{
	return float3( vec.x, vec.y, vec.z );
}
//---------------------------------------------------------------------------------
inline NxVec3 CovToPXVec( float3 vec )
{
	return NxVec3( vec.x, vec.y, vec.z );
}

//---------------------------------------------------------------------------------
inline void SetupDXMat44( const NxVec3& pos, const NxMat33& rot, float44& res )
{
	float dxMat[16];
	rot.getColumnMajorStride4( &dxMat[0] );
	pos.get( &(dxMat[12]) );

	dxMat[3] = dxMat[7] = dxMat[11] = 0.0f;
	dxMat[15] = 1.0f;

	res = dxMat;
}
//---------------------------------------------------------------------------------
inline void SetupDXMat44( const NxMat34& mat, float44& res )
{
	float dxMat[16];
	mat.M.getColumnMajorStride4( &dxMat[0] );
	mat.t.get( &(dxMat[12]) );

	dxMat[3] = dxMat[7] = dxMat[11] = 0.0f;
	dxMat[15] = 1.0f;

	res = dxMat;
}
//---------------------------------------------------------------------------------
inline void SetupPXMat34( const float44& mat, NxMat34& res )
{
	res.setColumnMajor44( &mat.m[0] );
}

#endif