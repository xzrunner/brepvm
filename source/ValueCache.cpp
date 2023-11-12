#include "brepvm/ValueCache.h"

#include <easyvm/Value.h>

namespace brepvm
{

bool ValueCache::Insert(size_t key, const evm::Value& val)
{
    auto state = m_cache.insert({ key, Item(val) });
    if (!state.second) {
        state.first->second.time = 0;
    }
    return state.second;
}

const evm::Value* ValueCache::Query(size_t key) const
{
    auto itr = m_cache.find(key);
    if (itr != m_cache.end()) {
        itr->second.time = 0;
        return &itr->second.val;
    } else {
        return nullptr;
    }
}

void ValueCache::Update()
{
    if (m_cache.empty()) {
        return;
    }

    for (auto& itr : m_cache) {
        ++itr.second.time;
    }

    float avg_time = 0;
    for (auto& itr : m_cache) {
        avg_time += itr.second.time;
    }
    avg_time /= m_cache.size();

    const float too_old = avg_time * (3 / (m_cache.size() / 100.0f));
    for (auto itr = m_cache.begin(); itr != m_cache.end(); )
    {
        if (itr->second.time > too_old) {
            itr = m_cache.erase(itr);
        } else {
            ++itr->second.time;
            ++itr;
        }
    }
}

}