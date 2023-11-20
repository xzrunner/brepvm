#pragma once

#include <vector>
#include <memory>
#include <string>
#include <stack>
#include <map>

namespace brepvm
{

class Profiler
{
public:
	Profiler() {}

	void PushBlock(const std::string& name, size_t pos);
	void PopBlock(size_t pos, int reg);

	void PrintBlockTree();

	struct CodeBlock
	{
		size_t begin;
		size_t end;

		int reg;
	};
	std::vector<CodeBlock> QueryCodeBlocks(const std::string& name) const;

	// statistics
	void StatCall(const std::string& name);
	void AddCost(int cost) { m_cost += cost; }
	int GetCost() const { return m_cost; }

private:
	struct Block
	{
		std::string name;

		size_t begin;
		size_t end;

		int reg;

		std::vector<std::shared_ptr<Block>> children;
	};

private:
	void PrintBlock(const std::shared_ptr<Block>& b, int level);

private:
	std::vector<std::shared_ptr<Block>> m_block_roots;
	std::stack<std::shared_ptr<Block>> m_block_path;

	// statistics
	std::map<std::string, int> m_stat_call;
	int m_cost = 0;

}; // Profiler

}