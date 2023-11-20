#include "brepvm/DelComments.h"
#include "brepvm/Bytecodes.h"
#include "brepvm/Decompiler.h"
#include "brepvm/VM.h"

#include <easyvm/OpCodes.h>

#include <vector>
#include <iterator>

namespace brepvm
{

std::shared_ptr<Bytecodes> DelComments::
Do(const std::shared_ptr<Bytecodes>& old_codes)
{
    std::vector<uint8_t> dst;

	Decompiler dc(old_codes, VM::Instance()->GetOpFields());

	auto src = old_codes->GetCode();

	int ip = 0;
	while (ip < src.size())
	{
		size_t sz = dc.CalcOpSize(ip);
		if (src[ip] != evm::OP_COMMENTS) {
			std::copy(src.begin() + ip, src.begin() + ip + sz, std::back_inserter(dst));
		}
		ip += sz;
	}

	// todo: relocate jump and Optimizer's block

    auto new_codes = std::make_shared<Bytecodes>();
    new_codes->SetCode(dst);
    return new_codes;
}

}