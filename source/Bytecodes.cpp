#include "brepvm/Bytecodes.h"

#include <iterator>

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

}