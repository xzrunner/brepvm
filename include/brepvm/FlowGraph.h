#pragma once

#include <vector>
#include <memory>

namespace brepvm
{

class Bytecodes;

class FlowGraph
{
public:
	FlowGraph(const std::shared_ptr<Bytecodes>& codes);

private:
	void Build();

private:
	struct BasicBlock
	{
		size_t begin, end;
	};

private:
	std::shared_ptr<Bytecodes> m_codes;

	std::vector<std::shared_ptr<BasicBlock>> m_blocks;

}; // FlowGraph

}