#include "brepvm/Bytecodes.h"

#include <assert.h>

namespace brepvm
{

void Bytecodes::Write(const char* data, size_t size)
{
	if (m_curr_pos < 0) 
	{
		std::copy(data, data + size, std::back_inserter(m_code));
	} 
	else if (m_curr_pos + size <= m_code.size()) 
	{
		for (int i = 0; i < size; ++i) {
			m_code[m_curr_pos + i] = data[i];
		}
		m_curr_pos += static_cast<int>(size);
	}
}

void Bytecodes::WriteNum(int pos, float num)
{
	SetCurrPos(pos);
	Write(reinterpret_cast<const char*>(&num), sizeof(float));
	SetCurrPos(-1);
}

void Bytecodes::StatCall(const std::string& name)
{
	auto itr = m_stat_call.find(name);
	if (itr == m_stat_call.end()) {
		m_stat_call.insert({ name, 1 });
	} else {
		++itr->second;
	}
}

}