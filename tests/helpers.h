// Copyright © 2022 CCP ehf.

#pragma once
#include "../include/ccpparser.h"

struct TestVariable
{
	const char* name;
	float value;
};

struct TestEvalResult
{
	CcpParser::ParseResult status;
	float value;

	operator bool() const
	{
		return status;
	}
};

bool operator==(float expected, const TestEvalResult& result);
bool operator==(int expected, const TestEvalResult& result);

std::ostream& operator<<(std::ostream& os, const TestEvalResult& evalResult);

CcpParser::ParseResult CanParse(const char* expr);
TestEvalResult TestEval(const char* expr, const std::vector<TestVariable>& variables, const std::vector<CcpParser::Function>& functions = {});
TestEvalResult TestEval(const char* expr);
