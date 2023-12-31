#pragma once

#include "OpFieldType.h"

#include <array>
#include <vector>
#include <memory>

namespace brepvm
{

class Bytecodes;
class OpFieldMap;

class CodesBuilder
{
public:
	CodesBuilder(const std::shared_ptr<Bytecodes>& codes,
		const std::shared_ptr<OpFieldMap>& ops);
	~CodesBuilder();

	void WriteType(uint8_t op);
	void WriteReg(uint8_t reg);
	void WriteDouble(double d);
	void WriteFloat(float f);
	void WriteInt(int i);
	void WriteBool(bool b);
	void WriteString(const char* str);

private:
	std::shared_ptr<Bytecodes> m_codes;

	std::shared_ptr<OpFieldMap> m_ops;

	std::vector<OpFieldType> m_op_fields;
	int m_op_type;

}; // CodesBuilder

}