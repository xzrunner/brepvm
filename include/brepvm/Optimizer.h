#pragma once

#include "CodeBlock.h"

#include <map>
#include <vector>
#include <memory>

namespace brepvm
{

class Bytecodes;
class Profiler;

class Optimizer
{
public:
	Optimizer(const std::shared_ptr<Bytecodes>& old_codes);

	void Optimize(const std::shared_ptr<Profiler>& profiler);

	// rewrite
	void WriteNumber(int pos, float num);
	void FlushCache();

	auto GetNewCodes() const { return m_new_codes; }

private:
	std::vector<std::vector<CodeBlock>> 
		CalcCacheBlocks(const std::shared_ptr<Profiler>& profiler) const;
	std::vector<CodeBlock> 
		CalcRemoveBlocks(const std::vector<std::vector<CodeBlock>>& cache_blocks) const;

	void CacheBlocks(const std::shared_ptr<Profiler>& profiler) const;

	int Relocate(int pos) const;

private:
	std::shared_ptr<Bytecodes> m_old_codes;
	mutable std::vector<std::vector<CodeBlock>> m_cached_blocks;

	mutable std::shared_ptr<Bytecodes> m_new_codes = nullptr;

}; // Optimizer

}