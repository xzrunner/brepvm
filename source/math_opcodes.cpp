#include "brepvm/math_opcodes.h"
#include "brepvm/ValueType.h"

#include <SM_Vector.h>
#include <SM_Plane.h>
#include <SM_Matrix.h>
#include <SM_Cube.h>
#include <easyvm/VM.h>
#include <easyvm/VMHelper.h>
#include <easyvm/Value.h>

#include <stdexcept>

namespace brepvm
{

void MathOpCodeImpl::OpCodeInit(evm::VM* vm)
{
	vm->RegistOperator(OP_VEC2_CREATE_I, Vec2CreateI);

	vm->RegistOperator(OP_VEC3_CREATE_R, Vec3CreateR);
	vm->RegistOperator(OP_VEC3_CREATE_I, Vec3CreateI);
	vm->RegistOperator(OP_VEC3_PRINT, Vec3Print);
	vm->RegistOperator(OP_VEC3_ADD, Vec3Add);
	vm->RegistOperator(OP_VEC3_SUB, Vec3Sub);
	vm->RegistOperator(OP_VEC3_TRANSFORM, Vec3Transform);

	vm->RegistOperator(OP_VEC4_CREATE_I, Vec4CreateI);

	vm->RegistOperator(OP_MATRIX_CREATE, MatrixCreate);
	vm->RegistOperator(OP_MATRIX_ROTATE, MatrixRotate);
	vm->RegistOperator(OP_MATRIX_TRANSLATE, MatrixTranslate);

	vm->RegistOperator(OP_GET_X, GetX);
	vm->RegistOperator(OP_GET_Y, GetY);
	vm->RegistOperator(OP_GET_Z, GetZ);
	vm->RegistOperator(OP_GET_W, GetW);

	vm->RegistOperator(OP_CREATE_PLANE, CreatePlane);
	vm->RegistOperator(OP_CREATE_PLANE_2, CreatePlane2);
	vm->RegistOperator(OP_CREATE_CUBE, CreateCube);

	vm->RegistOperator(OP_ADD, Add);
	vm->RegistOperator(OP_SUB, Sub);
	vm->RegistOperator(OP_MUL, Mul);
	vm->RegistOperator(OP_DIV, Div);
	vm->RegistOperator(OP_NEGATE, Negate);
	vm->RegistOperator(OP_ABS, Abs);
}

void MathOpCodeImpl::Vec2CreateI(evm::VM* vm)
{
	uint8_t reg = vm->NextByte();

	auto v2 = std::make_shared<sm::vec2>();
	v2->x = evm::VMHelper::ReadData<float>(vm);
	v2->y = evm::VMHelper::ReadData<float>(vm);

	evm::Value v;
	v.type = V_VEC2;
	v.as.handle = new evm::Handle<sm::vec2>(v2);

	vm->SetRegister(reg, v);
}

void MathOpCodeImpl::Vec3CreateR(evm::VM* vm)
{
	uint8_t r_dst = vm->NextByte();

	uint8_t r_x = vm->NextByte();
	uint8_t r_y = vm->NextByte();
	uint8_t r_z = vm->NextByte();

	auto v3 = std::make_shared<sm::vec3>();
	if (r_x != 0xff) {
		v3->x = static_cast<float>(evm::VMHelper::GetRegNumber(vm, r_x));
	}
	if (r_y != 0xff) {
		v3->y = static_cast<float>(evm::VMHelper::GetRegNumber(vm, r_y));
	}
	if (r_z != 0xff) {
		v3->z = static_cast<float>(evm::VMHelper::GetRegNumber(vm, r_z));
	}

	evm::Value v;
	v.type = V_VEC3;
	v.as.handle = new evm::Handle<sm::vec3>(v3);

	vm->SetRegister(r_dst, v);
}

void MathOpCodeImpl::Vec3CreateI(evm::VM* vm)
{
	uint8_t reg = vm->NextByte();

	auto v3 = std::make_shared<sm::vec3>();
	v3->x = evm::VMHelper::ReadData<float>(vm);
	v3->y = evm::VMHelper::ReadData<float>(vm);
	v3->z = evm::VMHelper::ReadData<float>(vm);

	evm::Value v;
	v.type = V_VEC3;
	v.as.handle = new evm::Handle<sm::vec3>(v3);

	vm->SetRegister(reg, v);
}

void MathOpCodeImpl::Vec3Print(evm::VM* vm)
{
	uint8_t reg = vm->NextByte();
	auto vec = evm::VMHelper::GetRegHandler<sm::vec3>(vm, reg);
	if (vec) {
		printf("%f, %f, %f", vec->x, vec->y, vec->z);
	}
}

void MathOpCodeImpl::Vec3Add(evm::VM* vm)
{
	uint8_t r_dst = vm->NextByte();

	uint8_t r_src1 = vm->NextByte();
	uint8_t r_src2 = vm->NextByte();

	auto src1 = evm::VMHelper::GetRegHandler<sm::vec3>(vm, r_src1);
	auto src2 = evm::VMHelper::GetRegHandler<sm::vec3>(vm, r_src2);
	if (!src1 && !src2)
	{
		evm::Value v;
		vm->SetRegister(r_dst, v);
		return;
	}

	sm::vec3 ret;
	if (src1 && src2) {
		ret = *src1 + *src2;
	} else if (src1) {
		ret = *src1;
	} else {
		assert(src2);
		ret = *src2;
	}

	evm::Value v;
	v.type = V_VEC3;
	v.as.handle = new evm::Handle<sm::vec3>(std::make_shared<sm::vec3>(ret));

	vm->SetRegister(r_dst, v);
}

void MathOpCodeImpl::Vec3Sub(evm::VM* vm)
{
	uint8_t r_dst = vm->NextByte();

	uint8_t r_src1 = vm->NextByte();
	uint8_t r_src2 = vm->NextByte();

	auto src1 = evm::VMHelper::GetRegHandler<sm::vec3>(vm, r_src1);
	auto src2 = evm::VMHelper::GetRegHandler<sm::vec3>(vm, r_src2);
	if (!src1 && !src2)
	{
		evm::Value v;
		vm->SetRegister(r_dst, v);
		return;
	}

	sm::vec3 ret;
	if (src1 && src2) {
		ret = *src1 - *src2;
	} else if (src1) {
		ret = *src1;
	} else {
		assert(src2);
		ret = -*src2;
	}

	evm::Value v;
	v.type = V_VEC3;
	v.as.handle = new evm::Handle<sm::vec3>(std::make_shared<sm::vec3>(ret));

	vm->SetRegister(r_dst, v);
}

void MathOpCodeImpl::Vec3Transform(evm::VM* vm)
{
	uint8_t r_vec3 = vm->NextByte();
	uint8_t r_mat = vm->NextByte();

	auto vec3 = evm::VMHelper::GetRegHandler<sm::vec3>(vm, r_vec3);
	if (!vec3) {
		return;
	}

	auto mat = evm::VMHelper::GetRegHandler<sm::mat4>(vm, r_mat);
	if (!mat) {
		return;
	}

	sm::vec3 ret = (*mat) * (*vec3);

	evm::Value v;
	v.type = V_VEC3;
	v.as.handle = new evm::Handle<sm::vec3>(std::make_shared<sm::vec3>(ret));

	vm->SetRegister(r_vec3, v);
}

void MathOpCodeImpl::Vec4CreateI(evm::VM* vm)
{
	uint8_t reg = vm->NextByte();

	auto v4 = std::make_shared<sm::vec4>();
	v4->x = evm::VMHelper::ReadData<float>(vm);
	v4->y = evm::VMHelper::ReadData<float>(vm);
	v4->z = evm::VMHelper::ReadData<float>(vm);
	v4->w = evm::VMHelper::ReadData<float>(vm);

	evm::Value v;
	v.type = V_VEC4;
	v.as.handle = new evm::Handle<sm::vec4>(v4);

	vm->SetRegister(reg, v);
}

void MathOpCodeImpl::MatrixCreate(evm::VM* vm)
{
	uint8_t reg = vm->NextByte();

	auto mat = std::make_shared<sm::mat4>();

	evm::Value v;
	v.type = V_MAT4;
	v.as.handle = new evm::Handle<sm::mat4>(mat);

	vm->SetRegister(reg, v);
}

void MathOpCodeImpl::MatrixRotate(evm::VM* vm)
{
	uint8_t r_mat = vm->NextByte();
	uint8_t r_xyz = vm->NextByte();

	auto mat = evm::VMHelper::GetRegHandler<sm::mat4>(vm, r_mat);
	if (!mat) {
		return;
	}

	auto vec = evm::VMHelper::GetRegHandler<sm::vec3>(vm, r_xyz);
	if (!vec) {
		return;
	}

	mat->Rotate(vec->x, vec->y, vec->z);
}

void MathOpCodeImpl::MatrixTranslate(evm::VM* vm)
{
	uint8_t r_mat = vm->NextByte();
	uint8_t r_xyz = vm->NextByte();

	auto mat = evm::VMHelper::GetRegHandler<sm::mat4>(vm, r_mat);
	if (!mat) {
		return;
	}

	auto vec = evm::VMHelper::GetRegHandler<sm::vec3>(vm, r_xyz);
	if (!vec) {
		return;
	}

	mat->Translate(vec->x, vec->y, vec->z);
}

void MathOpCodeImpl::GetX(evm::VM* vm)
{
	uint8_t r_dst = vm->NextByte();
	uint8_t r_src = vm->NextByte();

	evm::Value val;

	auto& src = vm->GetRegister(r_src);
	switch (src.type)
	{
	case brepvm::V_VEC2:
	{
		auto vec2 = evm::VMHelper::GetHandleValue<sm::vec2>(src);
		if (vec2)
		{
			val.type = evm::ValueType::V_NUMBER;
			val.as.number = vec2->x;
		}
	}
		break;
	case brepvm::V_VEC3:
	{
		auto vec3 = evm::VMHelper::GetHandleValue<sm::vec3>(src);
		if (vec3) 
		{
			val.type = evm::ValueType::V_NUMBER;
			val.as.number = vec3->x;
		}
	}
		break;
	case brepvm::V_VEC4:
	{
		auto vec4 = evm::VMHelper::GetHandleValue<sm::vec4>(src);
		if (vec4) 
		{
			val.type = evm::ValueType::V_NUMBER;
			val.as.number = vec4->x;
		}
	}
		break;
	case brepvm::V_MAT4:
	{
		auto mat4 = evm::VMHelper::GetHandleValue<sm::mat4>(src);
		if (mat4)
		{
			auto v4 = std::make_shared<sm::vec4>(
				mat4->c[0][0], mat4->c[0][1], mat4->c[0][2], mat4->c[0][3]
				);

			val.type = V_VEC4;
			val.as.handle = new evm::Handle<sm::vec4>(v4);
		}
	}
		break;
	default:
		throw std::runtime_error("Unknown type!");
	}

	vm->SetRegister(r_dst, val);
}

void MathOpCodeImpl::GetY(evm::VM* vm)
{
	uint8_t r_dst = vm->NextByte();
	uint8_t r_src = vm->NextByte();

	evm::Value val;

	auto& src = vm->GetRegister(r_src);
	switch (src.type)
	{
	case brepvm::V_VEC2:
	{
		auto vec2 = evm::VMHelper::GetHandleValue<sm::vec2>(src);
		if (vec2)
		{
			val.type = evm::ValueType::V_NUMBER;
			val.as.number = vec2->y;
		}
	}
		break;
	case brepvm::V_VEC3:
	{
		auto vec3 = evm::VMHelper::GetHandleValue<sm::vec3>(src);
		if (vec3) 
		{
			val.type = evm::ValueType::V_NUMBER;
			val.as.number = vec3->y;
		}
	}
		break;
	case brepvm::V_VEC4:
	{
		auto vec4 = evm::VMHelper::GetHandleValue<sm::vec4>(src);
		if (vec4) 
		{
			val.type = evm::ValueType::V_NUMBER;
			val.as.number = vec4->y;
		}
	}
		break;
	case brepvm::V_MAT4:
	{
		auto mat4 = evm::VMHelper::GetHandleValue<sm::mat4>(src);
		if (mat4)
		{
			auto v4 = std::make_shared<sm::vec4>(
				mat4->c[1][0], mat4->c[1][1], mat4->c[1][2], mat4->c[1][3]
				);

			val.type = V_VEC4;
			val.as.handle = new evm::Handle<sm::vec4>(v4);
		}
	}
		break;
	default:
		throw std::runtime_error("Unknown type!");
	}

	vm->SetRegister(r_dst, val);
}

void MathOpCodeImpl::GetZ(evm::VM* vm)
{
	uint8_t r_dst = vm->NextByte();
	uint8_t r_src = vm->NextByte();

	evm::Value val;

	auto& src = vm->GetRegister(r_src);
	switch (src.type)
	{
	case brepvm::V_VEC2:
		val.type = evm::ValueType::V_NUMBER;
		val.as.number = 0;
		break;
	case brepvm::V_VEC3:
	{
		auto vec3 = evm::VMHelper::GetHandleValue<sm::vec3>(src);
		if (vec3) 
		{
			val.type = evm::ValueType::V_NUMBER;
			val.as.number = vec3->z;
		}
	}
		break;
	case brepvm::V_VEC4:
	{
		auto vec4 = evm::VMHelper::GetHandleValue<sm::vec4>(src);
		if (vec4) 
		{
			val.type = evm::ValueType::V_NUMBER;
			val.as.number = vec4->z;
		}
	}
		break;
	case brepvm::V_MAT4:
	{
		auto mat4 = evm::VMHelper::GetHandleValue<sm::mat4>(src);
		if (mat4)
		{
			auto v4 = std::make_shared<sm::vec4>(
				mat4->c[2][0], mat4->c[2][1], mat4->c[2][2], mat4->c[2][3]
				);

			val.type = V_VEC4;
			val.as.handle = new evm::Handle<sm::vec4>(v4);
		}
	}
		break;
	default:
		throw std::runtime_error("Unknown type!");
	}

	vm->SetRegister(r_dst, val);
}

void MathOpCodeImpl::GetW(evm::VM* vm)
{
	uint8_t r_dst = vm->NextByte();
	uint8_t r_src = vm->NextByte();

	evm::Value val;

	auto& src = vm->GetRegister(r_src);
	switch (src.type)
	{
	case brepvm::V_VEC2:
		val.type = evm::ValueType::V_NUMBER;
		val.as.number = 0;
		break;
	case brepvm::V_VEC3:
		val.type = evm::ValueType::V_NUMBER;
		val.as.number = 0;
		break;
	case brepvm::V_VEC4:
	{
		auto vec4 = evm::VMHelper::GetHandleValue<sm::vec4>(src);
		if (vec4) 
		{
			val.type = evm::ValueType::V_NUMBER;
			val.as.number = vec4->w;
		}
	}
		break;
	case brepvm::V_MAT4:
	{
		auto mat4 = evm::VMHelper::GetHandleValue<sm::mat4>(src);
		if (mat4)
		{
			auto v4 = std::make_shared<sm::vec4>(
				mat4->c[3][0], mat4->c[3][1], mat4->c[3][2], mat4->c[3][3]
				);

			val.type = V_VEC4;
			val.as.handle = new evm::Handle<sm::vec4>(v4);
		}
	}
		break;
	default:
		throw std::runtime_error("Unknown type!");
	}

	vm->SetRegister(r_dst, val);
}

void MathOpCodeImpl::CreatePlane(evm::VM* vm)
{
	uint8_t r_dst = vm->NextByte();

	uint8_t r_p0 = vm->NextByte();
	auto p0 = evm::VMHelper::GetRegHandler<sm::vec3>(vm, r_p0);

	uint8_t r_p1 = vm->NextByte();
	auto p1 = evm::VMHelper::GetRegHandler<sm::vec3>(vm, r_p1);

	uint8_t r_p2 = vm->NextByte();
	auto p2 = evm::VMHelper::GetRegHandler<sm::vec3>(vm, r_p2);

	if (!p0 || !p1 || !p2)
	{
		evm::Value v;
		vm->SetRegister(r_dst, v);
		return;
	}

	auto plane = std::make_shared<sm::Plane>();
	plane->Build(*p0, *p1, *p2);

	evm::Value v;
	v.type = V_PLANE;
	v.as.handle = new evm::Handle<sm::Plane>(plane);

	vm->SetRegister(r_dst, v);
}

void MathOpCodeImpl::CreatePlane2(evm::VM* vm)
{
	uint8_t r_dst = vm->NextByte();

	uint8_t r_ori = vm->NextByte();
	auto ori = evm::VMHelper::GetRegHandler<sm::vec3>(vm, r_ori);

	uint8_t r_dir = vm->NextByte();
	auto dir = evm::VMHelper::GetRegHandler<sm::vec3>(vm, r_dir);

	if (!ori || !dir)
	{
		evm::Value v;
		vm->SetRegister(r_dst, v);
		return;
	}

	auto plane = std::make_shared<sm::Plane>();
	plane->Build(*dir, *ori);

	evm::Value v;
	v.type = V_PLANE;
	v.as.handle = new evm::Handle<sm::Plane>(plane);

	vm->SetRegister(r_dst, v);
}

void MathOpCodeImpl::CreateCube(evm::VM* vm)
{
	uint8_t r_dst = vm->NextByte();

	uint8_t r_min = vm->NextByte();
	auto min = evm::VMHelper::GetRegHandler<sm::vec3>(vm, r_min);

	uint8_t r_max = vm->NextByte();
	auto max = evm::VMHelper::GetRegHandler<sm::vec3>(vm, r_max);

	if (!min || !max)
	{
		evm::Value v;
		vm->SetRegister(r_dst, v);
		return;
	}

	auto cube = std::make_shared<sm::cube>(*min, *max);

	evm::Value v;
	v.type = V_CUBE;
	v.as.handle = new evm::Handle<sm::cube>(cube);

	vm->SetRegister(r_dst, v);
}

void MathOpCodeImpl::Add(evm::VM* vm)
{
	uint8_t r_dst = vm->NextByte();
	uint8_t r_src1 = vm->NextByte();
	uint8_t r_src2 = vm->NextByte();

	bool b_src1 = r_src1 != 0xff;
	bool b_src2 = r_src2 != 0xff;
	if (!b_src1 && !b_src2)
	{
		evm::Value v;
		vm->SetRegister(r_dst, v);
		return;
	}
	else if (!b_src1 || !b_src2)
	{
		if (!b_src1) {
			vm->SetRegister(r_dst, vm->GetRegister(r_src2));
		} else {
			vm->SetRegister(r_dst, vm->GetRegister(r_src1));
		}
		return;
	}

	auto& src1 = vm->GetRegister(r_src1);
	auto& src2 = vm->GetRegister(r_src2);
	if (src1.type == evm::V_NIL && src2.type == evm::V_NIL)
	{
		evm::Value v;
		vm->SetRegister(r_dst, v);
		return;
	}
	else if (src1.type == evm::V_NIL || src2.type == evm::V_NIL)
	{
		if (src1.type == evm::V_NIL) {
			vm->SetRegister(r_dst, vm->GetRegister(r_src2));
		} else {
			vm->SetRegister(r_dst, vm->GetRegister(r_src1));
		}
		return;
	}

	if (src1.type == evm::ValueType::V_NUMBER &&
		src2.type == evm::ValueType::V_NUMBER)
	{
		evm::Value val;
		val.type = evm::ValueType::V_NUMBER;
		val.as.number = src1.as.number + src2.as.number;

		vm->SetRegister(r_dst, val);
	}
	else if (src1.type == V_VEC2 &&
		     src2.type == V_VEC2)
	{
		sm::vec2 src1_v2 = *evm::VMHelper::GetHandleValue<sm::vec2>(src1);
		sm::vec2 src2_v2 = *evm::VMHelper::GetHandleValue<sm::vec2>(src2);

		sm::vec2 ret = src1_v2 + src2_v2;

		evm::Value v;
		v.type = V_VEC2;
		v.as.handle = new evm::Handle<sm::vec2>(std::make_shared<sm::vec2>(ret));

		vm->SetRegister(r_dst, v);
	}
	else if (src1.type == V_VEC3 &&
		     src2.type == V_VEC3)
	{
		sm::vec3 src1_v3 = *evm::VMHelper::GetHandleValue<sm::vec3>(src1);
		sm::vec3 src2_v3 = *evm::VMHelper::GetHandleValue<sm::vec3>(src2);

		sm::vec3 ret = src1_v3 + src2_v3;

		evm::Value v;
		v.type = V_VEC3;
		v.as.handle = new evm::Handle<sm::vec3>(std::make_shared<sm::vec3>(ret));

		vm->SetRegister(r_dst, v);
	}
	else if (src1.type == V_VEC4 &&
		     src2.type == V_VEC4)
	{
		sm::vec4 src1_v4 = *evm::VMHelper::GetHandleValue<sm::vec4>(src1);
		sm::vec4 src2_v4 = *evm::VMHelper::GetHandleValue<sm::vec4>(src2);

		sm::vec4 ret = src1_v4 + src2_v4;

		evm::Value v;
		v.type = V_VEC4;
		v.as.handle = new evm::Handle<sm::vec4>(std::make_shared<sm::vec4>(ret));

		vm->SetRegister(r_dst, v);
	}
	else
	{
		throw std::runtime_error("Not Implemented!");
	}
}

void MathOpCodeImpl::Sub(evm::VM* vm)
{
	uint8_t r_dst = vm->NextByte();
	uint8_t r_src1 = vm->NextByte();
	uint8_t r_src2 = vm->NextByte();

	bool b_src1 = r_src1 != 0xff;
	bool b_src2 = r_src2 != 0xff;
	if (!b_src1 && !b_src2)
	{
		evm::Value v;
		vm->SetRegister(r_dst, v);
		return;
	}
	else if (!b_src1 || !b_src2)
	{
		if (!b_src1) 
		{
			auto& src2 = vm->GetRegister(r_src2);

			evm::Value val;

			switch (src2.type)
			{
			case evm::ValueType::V_NUMBER:
			{
				val.type = evm::ValueType::V_NUMBER;
				val.as.number = -src2.as.number;
			}
				break;
			case V_VEC2:
			{
				sm::vec2 src2_v2 = *evm::VMHelper::GetHandleValue<sm::vec2>(src2);

				val.type = V_VEC2;
				val.as.handle = new evm::Handle<sm::vec2>(std::make_shared<sm::vec2>(-src2_v2));
			}
				break;
			case V_VEC3:
			{
				sm::vec3 src2_v3 = *evm::VMHelper::GetHandleValue<sm::vec3>(src2);

				val.type = V_VEC3;
				val.as.handle = new evm::Handle<sm::vec3>(std::make_shared<sm::vec3>(-src2_v3));
			}
				break;
			case V_VEC4:
			{
				sm::vec4 src2_v4 = *evm::VMHelper::GetHandleValue<sm::vec4>(src2);

				src2_v4.x = -src2_v4.x;
				src2_v4.y = -src2_v4.y;
				src2_v4.z = -src2_v4.z;
				src2_v4.w = -src2_v4.w;

				val.type = V_VEC4;
				val.as.handle = new evm::Handle<sm::vec4>(std::make_shared<sm::vec4>(src2_v4));
			}
				break;
			}

			vm->SetRegister(r_dst, val);
		} 
		else 
		{
			vm->SetRegister(r_dst, vm->GetRegister(r_src1));
		}
		return;
	}

	auto& src1 = vm->GetRegister(r_src1);
	auto& src2 = vm->GetRegister(r_src2);
	if (src1.type == evm::ValueType::V_NUMBER &&
		src2.type == evm::ValueType::V_NUMBER)
	{
		evm::Value val;
		val.type = evm::ValueType::V_NUMBER;
		val.as.number = src1.as.number - src2.as.number;

		vm->SetRegister(r_dst, val);
	}
	else if (src1.type == V_VEC2 &&
		     src2.type == V_VEC2)
	{
		sm::vec2 src1_v2 = *evm::VMHelper::GetHandleValue<sm::vec2>(src1);
		sm::vec2 src2_v2 = *evm::VMHelper::GetHandleValue<sm::vec2>(src2);

		sm::vec2 ret = src1_v2 - src2_v2;

		evm::Value v;
		v.type = V_VEC2;
		v.as.handle = new evm::Handle<sm::vec2>(std::make_shared<sm::vec2>(ret));

		vm->SetRegister(r_dst, v);
	}
	else if (src1.type == V_VEC3 &&
		     src2.type == V_VEC3)
	{
		sm::vec3 src1_v3 = *evm::VMHelper::GetHandleValue<sm::vec3>(src1);
		sm::vec3 src2_v3 = *evm::VMHelper::GetHandleValue<sm::vec3>(src2);

		sm::vec3 ret = src1_v3 - src2_v3;

		evm::Value v;
		v.type = V_VEC3;
		v.as.handle = new evm::Handle<sm::vec3>(std::make_shared<sm::vec3>(ret));

		vm->SetRegister(r_dst, v);
	}
	else if (src1.type == V_VEC4 &&
		     src2.type == V_VEC4)
	{
		sm::vec4 src1_v4 = *evm::VMHelper::GetHandleValue<sm::vec4>(src1);
		sm::vec4 src2_v4 = *evm::VMHelper::GetHandleValue<sm::vec4>(src2);

		sm::vec4 ret = src1_v4 - src2_v4;

		evm::Value v;
		v.type = V_VEC4;
		v.as.handle = new evm::Handle<sm::vec4>(std::make_shared<sm::vec4>(ret));

		vm->SetRegister(r_dst, v);
	}
	else
	{
		throw std::runtime_error("Not Implemented!");
	}
}

void MathOpCodeImpl::Mul(evm::VM* vm)
{
	uint8_t r_dst = vm->NextByte();
	uint8_t r_src1 = vm->NextByte();
	uint8_t r_src2 = vm->NextByte();

	if (r_src1 == 0xff || r_src2 == 0xff) 
	{
		evm::Value v;
		vm->SetRegister(r_dst, v);
		return;
	}

	auto& src1 = vm->GetRegister(r_src1);
	auto& src2 = vm->GetRegister(r_src2);

	if (src1.type == evm::ValueType::V_NIL || 
		src2.type == evm::ValueType::V_NIL)
	{
		evm::Value v;
		vm->SetRegister(r_dst, v);
		return;
	}

	if (src1.type == evm::ValueType::V_NUMBER &&
		src2.type == evm::ValueType::V_NUMBER)
	{
		evm::Value val;
		val.type = evm::ValueType::V_NUMBER;
		val.as.number = src1.as.number * src2.as.number;

		vm->SetRegister(r_dst, val);
	}
	else if (src1.type == V_VEC2 &&
		     src2.type == V_VEC2)
	{
		sm::vec2 src1_v2 = *evm::VMHelper::GetHandleValue<sm::vec2>(src1);
		sm::vec2 src2_v2 = *evm::VMHelper::GetHandleValue<sm::vec2>(src2);

		sm::vec2 ret = src1_v2 * src2_v2;

		evm::Value v;
		v.type = V_VEC2;
		v.as.handle = new evm::Handle<sm::vec2>(std::make_shared<sm::vec2>(ret));

		vm->SetRegister(r_dst, v);
	}
	else if (src1.type == V_VEC3 &&
		     src2.type == V_VEC3)
	{
		sm::vec3 src1_v3 = *evm::VMHelper::GetHandleValue<sm::vec3>(src1);
		sm::vec3 src2_v3 = *evm::VMHelper::GetHandleValue<sm::vec3>(src2);

		sm::vec3 ret = src1_v3 * src2_v3;

		evm::Value v;
		v.type = V_VEC3;
		v.as.handle = new evm::Handle<sm::vec3>(std::make_shared<sm::vec3>(ret));

		vm->SetRegister(r_dst, v);
	}
	else if (src1.type == evm::ValueType::V_NUMBER &&
		     src2.type == V_VEC3)
	{
		sm::vec3 src2_v3 = *evm::VMHelper::GetHandleValue<sm::vec3>(src2);

		sm::vec3 ret = src2_v3 * static_cast<float>(src1.as.number);

		evm::Value v;
		v.type = V_VEC3;
		v.as.handle = new evm::Handle<sm::vec3>(std::make_shared<sm::vec3>(ret));

		vm->SetRegister(r_dst, v);
	}
	else if (src2.type == evm::ValueType::V_NUMBER &&
		     src1.type == V_VEC3)
	{
		sm::vec3 src1_v3 = *evm::VMHelper::GetHandleValue<sm::vec3>(src1);

		sm::vec3 ret = src1_v3 * static_cast<float>(src2.as.number);

		evm::Value v;
		v.type = V_VEC3;
		v.as.handle = new evm::Handle<sm::vec3>(std::make_shared<sm::vec3>(ret));

		vm->SetRegister(r_dst, v);
	}
	else
	{
		throw std::runtime_error("Not Implemented!");
	}
}

void MathOpCodeImpl::Div(evm::VM* vm)
{
	uint8_t r_dst = vm->NextByte();
	uint8_t r_src1 = vm->NextByte();
	uint8_t r_src2 = vm->NextByte();

	if (r_src1 == 0xff || r_src2 == 0xff)
	{
		evm::Value v;
		vm->SetRegister(r_dst, v);
		return;
	}

	auto& src1 = vm->GetRegister(r_src1);
	auto& src2 = vm->GetRegister(r_src2);
	if (src1.type == evm::ValueType::V_NUMBER &&
		src2.type == evm::ValueType::V_NUMBER)
	{
		if (src2.as.number == 0) {
			throw std::runtime_error("Divide zero!");
		}

		evm::Value val;
		val.type = evm::ValueType::V_NUMBER;
		val.as.number = src1.as.number / src2.as.number;

		vm->SetRegister(r_dst, val);
	}
	else if (src1.type == V_VEC2 &&
		     src2.type == V_VEC2)
	{
		sm::vec2 src1_v2 = *evm::VMHelper::GetHandleValue<sm::vec2>(src1);
		sm::vec2 src2_v2 = *evm::VMHelper::GetHandleValue<sm::vec2>(src2);

		if (src2_v2.x == 0 || src2_v2.y == 0) {
			throw std::runtime_error("Divide zero!");
		}

		sm::vec2 ret = src1_v2 / src2_v2;

		evm::Value v;
		v.type = V_VEC2;
		v.as.handle = new evm::Handle<sm::vec2>(std::make_shared<sm::vec2>(ret));

		vm->SetRegister(r_dst, v);
	}
	else if (src1.type == evm::ValueType::V_NUMBER &&
		     src2.type == V_VEC3)
	{
		if (src1.as.number == 0) {
			throw std::runtime_error("Divide zero!");
		}

		sm::vec3 src2_v3 = *evm::VMHelper::GetHandleValue<sm::vec3>(src2);

		sm::vec3 ret = src2_v3 / static_cast<float>(src1.as.number);

		evm::Value v;
		v.type = V_VEC3;
		v.as.handle = new evm::Handle<sm::vec3>(std::make_shared<sm::vec3>(ret));

		vm->SetRegister(r_dst, v);
	}
	else if (src2.type == evm::ValueType::V_NUMBER &&
		     src1.type == V_VEC3)
	{
		if (src2.as.number == 0) {
			throw std::runtime_error("Divide zero!");
		}

		sm::vec3 src1_v3 = *evm::VMHelper::GetHandleValue<sm::vec3>(src1);

		sm::vec3 ret = src1_v3 / static_cast<float>(src2.as.number);

		evm::Value v;
		v.type = V_VEC3;
		v.as.handle = new evm::Handle<sm::vec3>(std::make_shared<sm::vec3>(ret));

		vm->SetRegister(r_dst, v);
	}
	else
	{
		throw std::runtime_error("Not Implemented!");
	}
}

void MathOpCodeImpl::Negate(evm::VM* vm)
{
	uint8_t r_dst = vm->NextByte();
	uint8_t r_src = vm->NextByte();

	if (r_src == 0xff)
	{
		evm::Value v;
		vm->SetRegister(r_dst, v);
		return;
	}

	auto& src = vm->GetRegister(r_src);
	if (src.type == evm::ValueType::V_NIL)
	{
		evm::Value v;
		vm->SetRegister(r_dst, v);
		return;
	}

	if (src.type == evm::ValueType::V_NUMBER)
	{
		evm::Value val;
		val.type = evm::ValueType::V_NUMBER;
		val.as.number = -src.as.number;

		vm->SetRegister(r_dst, val);
	}
	else if (src.type == V_VEC2)
	{
		sm::vec2 src_v2 = *evm::VMHelper::GetHandleValue<sm::vec2>(src);

		evm::Value v;
		v.type = V_VEC2;
		v.as.handle = new evm::Handle<sm::vec2>(std::make_shared<sm::vec2>(-src_v2));

		vm->SetRegister(r_dst, v);
	}
	else if (src.type == V_VEC3)
	{
		sm::vec3 src_v3 = *evm::VMHelper::GetHandleValue<sm::vec3>(src);

		evm::Value v;
		v.type = V_VEC3;
		v.as.handle = new evm::Handle<sm::vec3>(std::make_shared<sm::vec3>(-src_v3));

		vm->SetRegister(r_dst, v);
	}
	else if (src.type == V_VEC4)
	{
		sm::vec4 src_v4 = *evm::VMHelper::GetHandleValue<sm::vec4>(src);

		sm::vec4 dst_v4 = src_v4;
		for (int i = 0; i < 4; ++i) {
			dst_v4.xyzw[i] = -dst_v4.xyzw[i];
		}

		evm::Value v;
		v.type = V_VEC4;
		v.as.handle = new evm::Handle<sm::vec4>(std::make_shared<sm::vec4>(dst_v4));

		vm->SetRegister(r_dst, v);
	}
	else
	{
		throw std::runtime_error("Not Implemented!");
	}
}

void MathOpCodeImpl::Abs(evm::VM* vm)
{
	uint8_t r_dst = vm->NextByte();
	uint8_t r_src = vm->NextByte();

	if (r_src == 0xff)
	{
		evm::Value v;
		vm->SetRegister(r_dst, v);
		return;
	}

	auto& src = vm->GetRegister(r_src);
	if (src.type == evm::ValueType::V_NUMBER)
	{
		evm::Value val;
		val.type = evm::ValueType::V_NUMBER;
		val.as.number = fabs(src.as.number);

		vm->SetRegister(r_dst, val);
	}
	else if (src.type == V_VEC2)
	{
		sm::vec2 src_v2 = *evm::VMHelper::GetHandleValue<sm::vec2>(src);
		sm::vec2 dst_v2 = sm::vec2(fabs(src_v2.x), fabs(src_v2.y));

		evm::Value v;
		v.type = V_VEC2;
		v.as.handle = new evm::Handle<sm::vec2>(std::make_shared<sm::vec2>(dst_v2));

		vm->SetRegister(r_dst, v);
	}
	else if (src.type == V_VEC3)
	{
		sm::vec3 src_v3 = *evm::VMHelper::GetHandleValue<sm::vec3>(src);
		sm::vec3 dst_v3 = sm::vec3(fabs(src_v3.x), fabs(src_v3.y), fabs(src_v3.z));

		evm::Value v;
		v.type = V_VEC3;
		v.as.handle = new evm::Handle<sm::vec3>(std::make_shared<sm::vec3>(dst_v3));

		vm->SetRegister(r_dst, v);
	}
	else if (src.type == V_VEC4)
	{
		sm::vec4 src_v4 = *evm::VMHelper::GetHandleValue<sm::vec4>(src);
		sm::vec4 dst_v4 = sm::vec4(fabs(src_v4.x), fabs(src_v4.y), fabs(src_v4.z), fabs(src_v4.w));

		evm::Value v;
		v.type = V_VEC4;
		v.as.handle = new evm::Handle<sm::vec4>(std::make_shared<sm::vec4>(dst_v4));

		vm->SetRegister(r_dst, v);
	}
	else
	{
		throw std::runtime_error("Not Implemented!");
	}
}

}