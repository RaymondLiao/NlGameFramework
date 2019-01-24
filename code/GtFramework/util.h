#include "prerequisite.h"

/**
@brief 判断一个键当前是否按下
@return 
@param uint32 key
@remark 
*/
inline bool isKeyDown( uint32 key)
{
	return ((GetKeyState( key ) & 0x80) == 0x80);
}

/**
@brief 渲染一个网格平面，width为边长，count为边上的格数
@return 
@param IDirect3DDevice9 * device
@param float width
@param int count
@remark 
*/
inline void draw_grid_plane( IDirect3DDevice9* device, float width, int count )
{
	std::vector<float3> pointArray;

	float halfWidth = width * 0.5f;
	float segmentLength = width / (float)count;

	for (int i=0; i < count + 1; ++i)
	{
		pointArray.push_back( float3( -halfWidth + i * segmentLength, 0, -halfWidth)  );
		pointArray.push_back( float3( -halfWidth + i * segmentLength, 0, halfWidth)  );
	}

	for (int i=0; i < count + 1; ++i)
	{
		pointArray.push_back( float3( -halfWidth, 0, -halfWidth + i * segmentLength)  );
		pointArray.push_back( float3( halfWidth , 0, -halfWidth + i * segmentLength)  );
	}

	device->DrawPrimitiveUP( D3DPT_LINELIST, 2 * (count + 1), pointArray.data(), sizeof(float3) );
}

inline float getRandom( float min, float max )
{
	float amount = (rand() % RAND_MAX / (float)RAND_MAX);
	return min + (max - min) * amount;
}

inline std::string& getMediaPath()
{
	static bool first = true;
	static std::string path;
	if (first)
	{
		first = false;

		char buffer[MAX_PATH];
		char* strLastSlash = NULL;
		GetModuleFileName( NULL, buffer, MAX_PATH );
		buffer[MAX_PATH - 1] = 0;

		strLastSlash = strrchr( buffer, '\\' );
		if (strLastSlash)
		{
			*(strLastSlash + 1) = '\0';
		}

		path = buffer;
	}

	return path;
}