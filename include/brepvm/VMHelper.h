#pragma once

#include <vector>
#include <memory>

namespace evm { class VM; class Value; }
namespace pm3 { class Polytope; }

namespace brepvm
{

class VMHelper
{
public:
	static void StorePolys(evm::VM* vm, uint8_t reg,
		const std::vector<std::shared_ptr<pm3::Polytope>>& polytopes);

	static std::vector<std::shared_ptr<pm3::Polytope>> LoadPolys(evm::VM* vm, uint8_t reg);
	static std::vector<std::shared_ptr<pm3::Polytope>> LoadPolysFromCache(uint8_t idx);

	static std::shared_ptr<std::vector<evm::Value>> GetRegArray(evm::VM* vm, uint8_t reg);
	static std::shared_ptr<std::vector<evm::Value>> GetValArray(const evm::Value& val);

}; // VMHelper

}