#include "brepvm/Compiler.h"

#include <stdexcept>

#include <assert.h>

namespace brepvm
{

Compiler::Compiler()
{
    m_precomp_cond_branch = true;
}

int Compiler::NewRegister()
{
    for (int i = 0; i < REGISTER_COUNT; ++i)
    {
        if (!m_registers[i].used)
        {
            m_registers[i].used = true;
            return i;
        }
    }
    return -1;
}

void Compiler::FreeRegister(int reg)
{
    assert(reg >= 0 && reg < REGISTER_COUNT);

    if (m_registers[reg].keep) {
        return;
    }

    m_registers[reg].used = false;
}

void Compiler::SetRegKeep(int reg, bool keep)
{
    assert(reg >= 0 && reg < REGISTER_COUNT);
    m_registers[reg].keep = keep;
}

void Compiler::ExpectRegFree()
{
    for (auto r : m_registers)
    {
        if (r.used) {
            throw std::runtime_error("Register leakage!");
        }
    }
}

void Compiler::PushBlock(const std::string& name, size_t pos)
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

void Compiler::PopBlock(size_t pos)
{
    assert(!m_block_path.empty());
    auto block = m_block_path.top();
    block->end = pos;

    m_block_path.pop();
}

void Compiler::PrintBlockTree()
{
    for (auto root : m_block_roots) {
        PrintBlock(root, 0);
    }
}

void Compiler::PrintBlock(const std::shared_ptr<Block>& b, int level)
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
    printf("%s begin %d\n", b->name.c_str(), b->begin);

    for (auto& child : b->children) {
        PrintBlock(child, level + 1);
    }

    print_header();
    printf("%s end %d\n", b->name.c_str(), b->end);
}

}