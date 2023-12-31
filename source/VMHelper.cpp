#include "brepvm/VMHelper.h"
#include "brepvm/ValueType.h"
#include "brepvm/VM.h"

#include <easyvm/VM.h>
#include <easyvm/VMHelper.h>

#include <stdexcept>

namespace
{

void load_polys(std::vector<std::shared_ptr<pm3::Polytope>>& dst, const evm::Value& src)
{
	switch (src.type)
	{
	case evm::ValueType::V_NIL:
		break;
	case brepvm::V_POLY:
	{
		auto poly = evm::VMHelper::GetHandleValue<pm3::Polytope>(src);
		dst.push_back(poly);
	}
		break;
	case brepvm::V_ARRAY:
	{
		auto items = brepvm::VMHelper::GetValArray(src);
		for (auto& item : *items) {
			load_polys(dst, item);
		}
	}
		break;
	default:
		throw std::runtime_error("Unknown type!");
	}
}

}

namespace brepvm
{

void VMHelper::StorePolys(evm::VM* vm, uint8_t reg,
	                      const std::vector<std::shared_ptr<pm3::Polytope>>& polytopes)
{
	if (reg == 0xff) {
		return;
	}

	if (polytopes.empty()) 
	{
		evm::Value v;
		vm->SetRegister(reg, v);
	}
	else if (polytopes.size() == 1)
	{
		evm::Value v;
		v.type = V_POLY;
		v.as.handle = new evm::Handle<pm3::Polytope>(polytopes[0]);

		vm->SetRegister(reg, v);
	}
	else
	{
		auto list = std::make_shared<std::vector<evm::Value>>();

		for (auto src : polytopes)
		{
			evm::Value v;
			v.type = V_POLY;
			v.as.handle = new evm::Handle<pm3::Polytope>(src);

			list->push_back(v);
		}

		evm::Value v;
		v.type = V_ARRAY;
		v.as.handle = new evm::Handle<std::vector<evm::Value>>(list);

		vm->SetRegister(reg, v);
	}
}

std::vector<std::shared_ptr<pm3::Polytope>> 
VMHelper::LoadPolys(evm::VM* vm, uint8_t reg)
{
	if (reg == 0xff) {
		return {};
	}

    std::vector<std::shared_ptr<pm3::Polytope>> dst;
	load_polys(dst, vm->GetRegister(reg));
    return dst;
}

std::vector<std::shared_ptr<pm3::Polytope>> 
VMHelper::LoadPolysFromCache(uint8_t idx)
{
	std::vector<std::shared_ptr<pm3::Polytope>> dst;

	auto cache = VM::Instance()->GetCache();
	auto val = cache->Fetch(idx);
	if (val) {
		load_polys(dst, *val);
	}

	return dst;
}

std::shared_ptr<std::vector<evm::Value>> 
VMHelper::GetRegArray(evm::VM* vm, uint8_t reg)
{
	if (reg == 0xff) {
		return nullptr;
	}

	auto& val = vm->GetRegister(reg);
	if (val.type != V_ARRAY) {
		vm->Error("The register doesn't contain a array.");
		return nullptr;
	}

	return GetValArray(val);
}

std::shared_ptr<std::vector<evm::Value>> 
VMHelper::GetValArray(const evm::Value& val)
{
	if (val.type == V_ARRAY) {
		return evm::VMHelper::GetHandleValue<std::vector<evm::Value>>(val);
	} else {
		return nullptr;
	}
}

}