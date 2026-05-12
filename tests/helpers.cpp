// Copyright © 2022 CCP ehf.

#include "pch.h"
#include "helpers.h"

using namespace CcpParser;

bool operator==(float expected, const TestEvalResult& result)
{
	if (!result.status)
	{
		return false;
	}
	return expected == result.value;
}

bool operator==(int expected, const TestEvalResult& result)
{
	return float(expected) == result;
}

std::ostream& operator<<(std::ostream& os, const TestEvalResult& evalResult)
{
	if (evalResult)
	{
		os << evalResult.value;
	}
	else
	{
		os << ToString(evalResult.status);
	}
	return os;
}


ParseResult CanParse(const char* expr)
{
	Program program;
	return Parse(expr, {}, program);
}

TestEvalResult TestEval(const char* expr, const std::vector<TestVariable>& variables, const std::vector<Function>& functions)
{
	std::vector<Variable> vars;
	std::vector<float> values;
	uint32_t offset = 0;
	for (auto v : variables)
	{
		vars.push_back({ v.name, 0, offset });
		values.push_back(v.value);
		offset += sizeof(float);
	}
	Externals externals;
	VariableView varViews[] = { vars };
	externals.variables = varViews;
	FunctionView funcs[] = { functions };
	externals.functions = funcs;

	Program program;
	auto parsed = Parse(expr, externals, program);
	if (!parsed)
	{
		return { parsed, 0 };
	}

	auto code = program.Disassemble();

	std::unique_ptr<uint8_t> arena(new uint8_t[program.GetTempArenaSize()]);
	void* ext[] = { values.data() };
	return { parsed, program.Eval(ext, arena.get()) };
}

TestEvalResult TestEval(const char* expr)
{
	Program program;
	auto parsed = Parse(expr, {}, program);
	if (!parsed)
	{
		return { parsed, 0.f };
	}
	std::unique_ptr<uint8_t> arena(new uint8_t[program.GetTempArenaSize()]);
	return { parsed, program.Eval(nullptr, arena.get()) };
}
