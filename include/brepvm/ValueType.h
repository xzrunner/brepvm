#pragma once

#include <easyvm/Value.h>

namespace brepvm
{

enum ValueType
{
	V_ARRAY = evm::ValueType::V_HANDLE + 1,

	V_VEC2,
	V_VEC3,
	V_VEC4,
	V_MAT4,

	V_PLANE,
	V_CUBE,

	V_POLY_POINT,
	V_POLY_FACE,
	V_POLY,
};

}