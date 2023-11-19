#include "brepvm/FlowGraph.h"
#include "brepvm/Decompiler.h"
#include "brepvm/VM.h"
#include "brepvm/Bytecodes.h"

#include <set>

namespace brepvm
{

FlowGraph::FlowGraph(const std::shared_ptr<Bytecodes>& codes)
	: m_codes(codes)
{
	Build();
}

void FlowGraph::Build()
{
	auto& code = m_codes->GetCode();
	if (code.empty()) {
		return;
	}

	std::set<size_t> leaders;
	leaders.insert(0);

	Decompiler dc(m_codes, VM::Instance()->GetOpFields());
	for (size_t ip = 0, n = code.size(); ip < n; )
	{
		size_t sz = dc.CalcOpSize(ip);

		int opcode = code[ip];
		if (Decompiler::IsJumpOp(opcode))
		{
			int dst = 0;
			memcpy(&dst, &code[ip + 1], sizeof(int));
			leaders.insert(dst);

			leaders.insert(ip + sz);
		}

		ip += sz;
	}
}

}