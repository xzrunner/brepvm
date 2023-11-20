#include "brepvm/Profiler.h"

#include <queue>

#include <assert.h>

namespace brepvm
{

void Profiler::PushBlock(const std::string& name, size_t pos)
{
    auto block = std::make_shared<Block>();
    block->name = name;
    block->begin = pos;

    if (m_block_path.empty())
    {
        m_block_roots.push_back(block);
    }
    else
    {
        auto parent = m_block_path.top();
        parent->children.push_back(block);
    }

    m_block_path.push(block);
}

void Profiler::PopBlock(size_t pos, int reg)
{
    assert(!m_block_path.empty());
    auto block = m_block_path.top();

    block->end = pos;
    block->reg = reg;

    m_block_path.pop();
}

void Profiler::PrintBlockTree()
{
    for (auto root : m_block_roots) {
        PrintBlock(root, 0);
    }
}

std::vector<Profiler::CodeBlock>
Profiler::QueryCodeBlocks(const std::string& name) const
{
    std::vector<Profiler::CodeBlock> blocks;

    std::queue<std::shared_ptr<Block>> buf;
    for (auto block : m_block_roots) {
        buf.push(block);
    }

	while (!buf.empty())
	{
		auto b = buf.front(); buf.pop();

        if (b->name == name)
        {
            Profiler::CodeBlock dst;
            dst.begin = b->begin;
            dst.end   = b->end;
            dst.reg   = b->reg;

            blocks.push_back(dst);
        }

		for (auto& c : b->children) {
			if (c) {
				buf.push(c);
			}
		}
	}

    return blocks;
}

void Profiler::StatCall(const std::string& name)
{
    auto itr = m_stat_call.find(name);
    if (itr == m_stat_call.end()) {
        m_stat_call.insert({ name, 1 });
    } else {
        ++itr->second;
    }
}

void Profiler::PrintBlock(const std::shared_ptr<Block>& b, int level)
{
    if (!b || b->begin == b->end) {
        return;
    }

    //if (b->end - b->begin < 16) {
    //    return;
    //}

    if (b->children.size() == 1)
    {
        auto& c = b->children.front();
        if (b->begin == c->begin && b->end == c->end) 
        {
            PrintBlock(c, level);
            return;
        }
    }

    auto print_header = [&level]() 
    {
        for (int i = 0; i < level; ++i) {
            //printf("\t");
            printf(" ");
        }
    };

    print_header();
    printf("%s <<< %d\n", b->name.c_str(), static_cast<int>(b->begin));

    for (auto& child : b->children) {
        PrintBlock(child, level + 1);
    }

    print_header();
    printf("%s >>> %d\n", b->name.c_str(), static_cast<int>(b->end));
}

}