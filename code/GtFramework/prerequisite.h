/**
  @file prerequisite.h
  
  @brief ȫ��ǰ��

  @author Kaiming

  ������־ history
  ver:1.0
   
 */

#ifndef prerequisite_h__
#define prerequisite_h__

// PhysXͷ�ļ�
#include "NxPhysics.h"

// windows libraryͷ�ļ�
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// stlͷ�ļ�
#include <assert.h>

#include <string>
#include <vector>
#include <map>

// �ڲ����Ͷ���
typedef unsigned char uint8;
typedef unsigned int uint32;
typedef int int32;

// D3Dͷ�ļ�
#include <d3d9.h>
#include "dxsdk/Include/d3dx9.h"

#include "mathlib/math_def.h"
#include "util.h"

#include "input.h"

#endif