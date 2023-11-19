#pragma once

#include <easyvm/VM.h>

namespace brepvm
{

class Profiler;

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

	auto GetProfiler() const { return m_profiler; }

private:
	struct Register
	{
		bool used = false;
		bool keep = false;
	};

private:
	std::array<Register, REGISTER_COUNT> m_registers;

	bool m_precomp_cond_branch = false;

	std::shared_ptr<Profiler> m_profiler;

}; // Compiler

}