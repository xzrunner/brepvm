#include "brepvm/ValueCache.h"

#include <easyvm/Value.h>

namespace brepvm
{

bool ValueCache::Insert(size_t key, const evm::Value& val)
{
    auto state = m_cache.insert({ key, val });
    return state.second;
}

const evm::Value* ValueCache::Query(size_t key) const
{
    auto itr = m_cache.find(key);
    if (itr != m_cache.end()) {
        return &itr->second;
    } else {
        return nullptr;
    }
}

}