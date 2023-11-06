#pragma once

#include <stdint.h>

namespace brepvm
{

struct CodeBlock
{
	uint32_t hash;

	int begin;
	int end;

	int reg;

	int times = 0;

	int group;

	bool dirty = false;
};

}