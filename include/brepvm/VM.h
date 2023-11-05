#pragma once

#include "ValueCache.h"

#include <easyvm/Value.h>

#include <memory>
#include <vector>

namespace evm { class VM; }

namespace brepvm
{

class OpFieldMap;

class VM
{
public:
	std::shared_ptr<evm::VM> CreateVM(const std::vector<uint8_t>& codes) const;

	auto GetOpFields() const { return m_op_fields; }

	auto& GetCache() const { return m_cache; }

public:
	static VM* Instance();

private:
	VM();
	~VM();

private:
	static VM* m_instance;

private:
	std::shared_ptr<OpFieldMap> m_op_fields = nullptr;

	std::shared_ptr<ValueCache> m_cache = nullptr;

}; // VM

}