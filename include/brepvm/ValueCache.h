#pragma once

#include <easyvm/Value.h>

#include <map>

namespace brepvm
{

class ValueCache
{
public:
	bool Insert(size_t key, const evm::Value& val);
	const evm::Value* Query(size_t key) const;

	void Update();

private:
	struct Item
	{
		Item(const evm::Value& v) : val(v) {}

		mutable int time = 0;
		evm::Value val;
	};

private:
	std::map<size_t, Item> m_cache;

}; // ValueCache

}