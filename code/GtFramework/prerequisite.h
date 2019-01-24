/**
  @file prerequisite.h
  
  @brief 全局前提

  @author Kaiming

  更改日志 history
  ver:1.0
   
 */

#ifndef prerequisite_h__
#define prerequisite_h__

// PhysX头文件
#include "NxPhysics.h"

// windows library头文件
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// stl头文件
#include <assert.h>

#include <string>
#include <vector>
#include <map>

// 内部类型定义
typedef unsigned char uint8;
typedef unsigned int uint32;
typedef int int32;

// D3D头文件
#include <d3d9.h>
#include "dxsdk/Include/d3dx9.h"

#include "mathlib/math_def.h"
#include "util.h"

#include "input.h"

#endif