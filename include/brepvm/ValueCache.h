#pragma once

#include <map>

namespace evm { class Value; }

namespace brepvm
{

class ValueCache
{
public:
	bool Insert(size_t key, const evm::Value& val);
	const evm::Value* Query(size_t key) const;

private:
	std::map<size_t, evm::Value> m_cache;

}; // ValueCache

}