#pragma once

#include "CodeBlock.h"

#include <vector>

namespace brepvm
{

class Bytecodes
{
public:
	void Write(const char* data, size_t size);

	// rewrite
	void WriteNum(int pos, float num);

	auto& GetCode() const { return m_code; }
	void SetCode(const std::vector<uint8_t>& code) { m_code = code; }

	void SetCurrPos(int pos) { m_curr_pos = pos; }

private:
	std::vector<uint8_t> m_code;

	int m_curr_pos = -1;

}; // Bytecodes

}