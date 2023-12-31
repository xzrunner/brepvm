#include "brepvm/Compiler.h"
#include "brepvm/Profiler.h"

#include <stdexcept>

#include <assert.h>

namespace brepvm
{

Compiler::Compiler()
{
    m_precomp_cond_branch = true;

    m_profiler = std::make_shared<Profiler>();
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

}