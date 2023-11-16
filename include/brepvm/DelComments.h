#pragma once

#include <memory>

namespace brepvm
{

class Bytecodes;

class DelComments
{
public:
	static std::shared_ptr<Bytecodes> Do(
		const std::shared_ptr<Bytecodes>& codes
	);

}; // DelComments

}