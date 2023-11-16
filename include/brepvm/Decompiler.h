#pragma once

#include "CodeBlock.h"
#include "OpFieldType.h"

#include <memory>
#include <vector>
#include <string>

namespace brepvm
{

class Bytecodes;
class OpFieldMap;

class Decompiler
{
public:
	Decompiler(const std::shared_ptr<Bytecodes>& codes,
		const std::shared_ptr<OpFieldMap>& ops);

	void Print(int begin, int end);

	uint32_t Hash(int begin, int end);

	void JumpLabelEncode();
	void JumpLabelDecode();

	void JumpLabelRelocate(const std::vector<CodeBlock>& rm_blocks);

	void ReplaceHash(uint32_t old_hash, uint32_t new_hash);

private:
	void AdvancePtr(const OpFieldType& type, int& ip) const;

private:
	std::shared_ptr<Bytecodes> m_codes;
	std::shared_ptr<OpFieldMap> m_ops;

	static std::vector<std::string> m_op_names;

}; // Decompiler

}