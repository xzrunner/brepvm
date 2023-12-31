#include "brepvm/geo_opcodes.h"
#include "brepvm/ValueType.h"
#include "brepvm/VM.h"
#include "brepvm/VMHelper.h"

#include <SM_Matrix.h>
#include <SM_Calc.h>
#include <polymesh3/Polytope.h>
#include <polymesh3/PolytopeAlgos.h>
#include <easyvm/VM.h>
#include <easyvm/VMHelper.h>
#include <easyvm/Value.h>

#include <stdexcept>

namespace
{

void transform(const evm::Value& val, const sm::mat4& mat)
{
	switch (val.type)
	{
	case evm::V_NIL:
		break;
	case brepvm::V_ARRAY:
	{
		auto list = brepvm::VMHelper::GetValArray(val);
		for (auto& item : *list) {
			transform(item, mat);
		}
	}
		break;
	case brepvm::V_VEC3:
	{
		auto vec3 = evm::VMHelper::GetHandleValue<sm::vec3>(val);
		if (vec3) {
			*vec3 = mat * (*vec3);
		}
	}
		break;
	case brepvm::V_POLY:
	{
		auto poly = evm::VMHelper::GetHandleValue<pm3::Polytope>(val);
		if (poly)
		{
			auto& pts = poly->Points();
			for (auto& p : pts) {
				p->pos = mat * p->pos;
			}

			poly->SetTopoDirty();
		}
	}
		break;
	case brepvm::V_PLANE:
	{
		auto plane = evm::VMHelper::GetHandleValue<sm::Plane>(val);
		if (plane) {
			sm::transform_plane(*plane, mat);
		}
	}
		break;
	case brepvm::V_POLY_POINT:
	{
		auto point = evm::VMHelper::GetHandleValue<pm3::Polytope::Point>(val);
		if (point) {
			point->pos = mat * point->pos;
		}
	}
		break;
	default:
		throw std::runtime_error("Not Implemented!");
	}
}

evm::Value value_clone(const evm::Value& src)
{
	evm::Value dst;

	switch (src.type)
	{
	case evm::ValueType::V_NIL:
		break;
	case brepvm::V_ARRAY:
	{
		auto src_items = brepvm::VMHelper::GetValArray(src);
		auto dst_items = std::make_shared<std::vector<evm::Value>>();
		for (auto& src_item : *src_items) {
			dst_items->push_back(value_clone(src_item));
		}

		dst.type = brepvm::V_ARRAY;
		dst.as.handle = new evm::Handle<std::vector<evm::Value>>(dst_items);
	}
		break;
	case brepvm::V_POLY:
	{
		auto src_poly = evm::VMHelper::GetHandleValue<pm3::Polytope>(src);
		auto dst_poly = std::make_shared<pm3::Polytope>(*src_poly);

		dst.type = brepvm::V_POLY;
		dst.as.handle = new evm::Handle<pm3::Polytope>(dst_poly);
	}
		break;
	default:
		throw std::runtime_error("Not Implemented!");
	}

	return dst;
}

}

namespace brepvm
{

void GeoOpCodeImpl::OpCodeInit(evm::VM* vm)
{
	vm->RegistOperator(OP_CREATE_POLYFACE, CreatePolyFace);
	vm->RegistOperator(OP_CREATE_POLYTOPE, CreatePolytope);
	vm->RegistOperator(OP_CREATE_POLYFACE_2, CreatePolyFace2);
	vm->RegistOperator(OP_CREATE_POLYTOPE_2, CreatePolytope2);

	vm->RegistOperator(OP_POLYTOPE_SUBTRACT, PolytopeSubtract);
	vm->RegistOperator(OP_POLYTOPE_EXTRUDE, PolytopeExtrude);
	vm->RegistOperator(OP_POLYTOPE_CLIP, PolytopeClip);
	vm->RegistOperator(OP_POLYTOPE_SET_DIRTY, PolytopeSetDirty);

	vm->RegistOperator(OP_POLYPOINT_SELECT, PolyPointSelect);
	vm->RegistOperator(OP_POLYFACE_SELECT, PolyFaceSelect);

	vm->RegistOperator(OP_POLY_COPY_FROM_MEM, PolyCopyFromMem);

	vm->RegistOperator(OP_TRANSFORM, Transform);	
}

void GeoOpCodeImpl::CreatePolyFace(evm::VM* vm)
{
	uint8_t r_dst = vm->NextByte();
	uint8_t r_plane = vm->NextByte();

	auto plane = evm::VMHelper::GetRegHandler<sm::Plane>(vm, r_plane);
	if (!plane)
	{
		evm::Value v;
		vm->SetRegister(r_dst, v);
		return;
	}

	auto face = std::make_shared<pm3::Polytope::Face>();
	face->plane = *plane;

	evm::Value v;
	v.type = V_POLY_FACE;
	v.as.handle = new evm::Handle<pm3::Polytope::Face>(face);

	vm->SetRegister(r_dst, v);
}

void GeoOpCodeImpl::CreatePolytope(evm::VM* vm)
{
	uint8_t r_dst = vm->NextByte();
	uint8_t r_src = vm->NextByte();

	auto faces = brepvm::VMHelper::GetRegArray(vm, r_src);
	if (!faces)
	{
		evm::Value v;
		vm->SetRegister(r_dst, v);
		return;
	}

	std::vector<std::shared_ptr<pm3::Polytope::Face>> faces_vec;
	for (auto& f : *faces) 
	{
		auto face = evm::VMHelper::GetHandleValue<pm3::Polytope::Face>(f);
		faces_vec.push_back(face);
	}

	auto poly = std::make_shared<pm3::Polytope>(faces_vec);

	evm::Value v;
	v.type = V_POLY;
	v.as.handle = new evm::Handle<pm3::Polytope>(poly);

	vm->SetRegister(r_dst, v);
}

void GeoOpCodeImpl::CreatePolyFace2(evm::VM* vm)
{
	uint8_t r_dst = vm->NextByte();
	uint8_t r_border = vm->NextByte();
	// todo: holes
	uint8_t r_holes = vm->NextByte();

	auto v_border = brepvm::VMHelper::GetRegArray(vm, r_border);
	if (!v_border)
	{
		evm::Value v;
		vm->SetRegister(r_dst, v);
		return;
	}

	std::vector<size_t> border;
	border.reserve(v_border->size());
	for (auto v : *v_border) {
		border.push_back(static_cast<size_t>(v.as.number));
	}

	auto face = std::make_shared<pm3::Polytope::Face>();
	face->border = border;

	evm::Value v;
	v.type = V_POLY_FACE;
	v.as.handle = new evm::Handle<pm3::Polytope::Face>(face);

	vm->SetRegister(r_dst, v);
}

void GeoOpCodeImpl::CreatePolytope2(evm::VM* vm)
{
	uint8_t r_dst = vm->NextByte();

	uint8_t r_points = vm->NextByte();
	auto v_points = brepvm::VMHelper::GetRegArray(vm, r_points);

	uint8_t r_faces = vm->NextByte();
	auto v_faces = brepvm::VMHelper::GetRegArray(vm, r_faces);

	if (!v_points || !v_faces)
	{
		evm::Value v;
		vm->SetRegister(r_dst, v);
		return;
	}

	std::vector<pm3::Polytope::PointPtr> points;
	for (auto& v_p : *v_points)
	{
		auto pos = evm::VMHelper::GetHandleValue<sm::vec3>(v_p);
		points.push_back(std::make_shared<pm3::Polytope::Point>(*pos));
	}

	std::vector<std::shared_ptr<pm3::Polytope::Face>> faces;
	for (auto& v_f : *v_faces) 
	{
		auto face = evm::VMHelper::GetHandleValue<pm3::Polytope::Face>(v_f);
		faces.push_back(face);
	}

	auto poly = std::make_shared<pm3::Polytope>(points, faces);

	evm::Value v;
	v.type = V_POLY;
	v.as.handle = new evm::Handle<pm3::Polytope>(poly);

	vm->SetRegister(r_dst, v);
}

void GeoOpCodeImpl::PolytopeSubtract(evm::VM* vm)
{
	uint8_t r_dst  = vm->NextByte();
	uint8_t r_base = vm->NextByte();
	uint8_t r_tool = vm->NextByte();

	auto base = brepvm::VMHelper::LoadPolys(vm, r_base);
	if (base.empty()) {
		evm::Value v;
		vm->SetRegister(r_dst, v);
		return;
	}

	auto tool = brepvm::VMHelper::LoadPolys(vm, r_tool);

	std::vector<std::pair<pm3::PolytopePtr, pm3::PolytopePtr>> hist;
	auto polys = pm3::PolytopeAlgos::Subtract(base, tool, hist);
	VMHelper::StorePolys(vm, r_dst, polys);
}

void GeoOpCodeImpl::PolytopeExtrude(evm::VM* vm)
{
	uint8_t r_poly = vm->NextByte();
	uint8_t r_dist = vm->NextByte();

	auto poly = evm::VMHelper::GetRegHandler<pm3::Polytope>(vm, r_poly);
	if (!poly) {
		return;
	}

	auto dist = evm::VMHelper::GetRegNumber(vm, r_dist);

	pm3::PolytopeAlgos::Extrude(poly, static_cast<float>(dist));
}

void GeoOpCodeImpl::PolytopeClip(evm::VM* vm)
{
	uint8_t r_poly = vm->NextByte();
	auto src = brepvm::VMHelper::LoadPolys(vm, r_poly);

	uint8_t r_plane = vm->NextByte();
	auto plane = evm::VMHelper::GetRegHandler<sm::Plane>(vm, r_plane);

	auto keep = static_cast<he::Polyhedron::KeepType>(vm->NextByte());
	auto seam = static_cast<bool>(vm->NextByte());

	if (src.empty() || !plane) {
		return;
	}

	std::vector<std::shared_ptr<pm3::Polytope>> dst;

	for (auto p : src)
	{
		if (p->GetTopoPoly()->Clip(*plane, keep, seam)) 
		{
			p->BuildFromTopo();
			dst.push_back(p);
		}
	}

	if (dst.size() != src.size())
	{
		auto list = std::make_shared<std::vector<evm::Value>>();

		for (auto p : dst)
		{
			evm::Value v;
			v.type = V_POLY;
			v.as.handle = new evm::Handle<pm3::Polytope>(p);

			list->push_back(v);
		}

		evm::Value val;
		val.type = V_ARRAY;
		val.as.handle = new evm::Handle<std::vector<evm::Value>>(list);

		vm->SetRegister(r_poly, val);
	}
}

void GeoOpCodeImpl::PolytopeSetDirty(evm::VM* vm)
{
	uint8_t r_poly = vm->NextByte();
	auto polys = brepvm::VMHelper::LoadPolys(vm, r_poly);
	for (auto poly : polys) {
		poly->SetTopoDirty();
	}
}

void GeoOpCodeImpl::PolyPointSelect(evm::VM* vm)
{
	uint8_t r_dst = vm->NextByte();

	uint8_t r_poly = vm->NextByte();
	auto polys = brepvm::VMHelper::LoadPolys(vm, r_poly);

	uint8_t r_cube = vm->NextByte();
	auto cube = evm::VMHelper::GetRegHandler<sm::cube>(vm, r_cube);

	if (polys.empty() || !cube) {
		return;
	}

	auto vector = std::make_shared<std::vector<evm::Value>>();

	for (auto poly : polys)
	{
		for (auto pt : poly->Points())
		{
			if (sm::is_point_in_cube(*cube, pt->pos)) 
			{
				evm::Value v;
				v.type = V_POLY_POINT;
				v.as.handle = new evm::Handle<pm3::Polytope::Point>(pt);

				vector->push_back(v);
			}
		}
	}

	evm::Value val;
	val.type = V_ARRAY;
	val.as.handle = new evm::Handle<std::vector<evm::Value>>(vector);

	vm->SetRegister(r_dst, val);
}

void GeoOpCodeImpl::PolyFaceSelect(evm::VM* vm)
{
	uint8_t r_dst = vm->NextByte();

	uint8_t r_poly = vm->NextByte();
	auto polys = brepvm::VMHelper::LoadPolys(vm, r_poly);

	uint8_t r_normal = vm->NextByte();
	auto normal = evm::VMHelper::GetRegHandler<sm::vec3>(vm, r_normal);

	uint8_t r_region = vm->NextByte();
	double region = evm::VMHelper::GetRegNumber(vm, r_region);

	if (polys.empty() || !normal) {
		return;
	}

	auto vector = std::make_shared<std::vector<evm::Value>>();

	for (auto poly : polys)
	{
		for (auto face : poly->Faces())
		{
			float angle = sm::get_angle(sm::vec3(0, 0, 0), face->plane.normal, *normal);
			if (fabs(angle) < region) 
			{
				evm::Value v;
				v.type = V_POLY_FACE;
				v.as.handle = new evm::Handle<pm3::Polytope::Face>(face);

				vector->push_back(v);
			}
		}
	}

	evm::Value val;
	val.type = V_ARRAY;
	val.as.handle = new evm::Handle<std::vector<evm::Value>>(vector);

	vm->SetRegister(r_dst, val);
}

void GeoOpCodeImpl::Transform(evm::VM* vm)
{
	uint8_t r_obj = vm->NextByte();

	uint8_t r_mat = vm->NextByte();
	auto mat = evm::VMHelper::GetRegHandler<sm::mat4>(vm, r_mat);
	if (!mat) {
		return;
	}

	if (r_obj != 0xff) {
		auto& v_obj = vm->GetRegister(r_obj);
		transform(v_obj, *mat);
	}
}

void GeoOpCodeImpl::PolyCopyFromMem(evm::VM* vm)
{
	uint8_t r_dst = vm->NextByte();
	uint32_t key = evm::VMHelper::ReadData<uint32_t>(vm);

	if (r_dst == 0xff) {
		return;
	}

	auto cache = VM::Instance()->GetCache();
	auto val = cache->Query(key);
	assert(val);

	auto poly = std::make_shared<pm3::Polytope>();
	evm::Value dst = value_clone(*val);
	vm->SetRegister(r_dst, dst);
}

}