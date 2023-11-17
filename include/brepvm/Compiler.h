#pragma once

#include <easyvm/VM.h>

#include <array>
#include <stack>

namespace brepvm
{

class Compiler
{
public:
	Compiler();

	int NewRegister();
	void FreeRegister(int reg);

	void SetRegKeep(int reg, bool keep);

	void ExpectRegFree();

	bool IsPrecompCondBranch() const { 
		return m_precomp_cond_branch; 
	}

	void PushBlock(const std::string& name, size_t pos);
	void PopBlock(size_t pos);

	void PrintBlockTree();

private:
	struct Register
	{
		bool used = false;
		bool keep = false;
	};

	struct Block
	{
		std::string name;
		size_t begin, end;

		std::vector<std::shared_ptr<Block>> children;
	};

private:
	void PrintBlock(const std::shared_ptr<Block>& b, int level);

private:
	std::array<Register, REGISTER_COUNT> m_registers;

	std::vector<std::shared_ptr<Block>> m_block_roots;
	std::stack<std::shared_ptr<Block>> m_block_path;

	bool m_precomp_cond_branch = false;

}; // Compiler

}