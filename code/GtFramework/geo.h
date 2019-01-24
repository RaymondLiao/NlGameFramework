//////////////////////////////////////////////////////////////////////////
//
// yikaiming (C) 2013
// gkENGINE Source File 
//
// Name:   	geo.h
// Desc:	
// 	
// 
// Author:  YiKaiming
// Date:	2013/11/18
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _geo_h_
#define _geo_h_

struct plane
{
	float3 normal;
	float dist;

	plane( const float3 &n, const float &d ) {  normal=n; dist=d; }

	void set_plane( const float3 &n, const float3 &point ) { 
		normal=n; 
		dist=-(float3::dot(point,normal)); 
	}

	float3 reflect_dir(const float3& source) const
	{
		return normal*(2* float3::dot(normal,source))-source;
	}
	float3 reflect_point(const float3& source) const
	{
		return source - normal*(2* (float3::dot(normal,source)+dist));
	}
};

#endif

