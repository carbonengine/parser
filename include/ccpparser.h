// Copyright © 2022 CCP ehf.

#pragma once

#include "parserresult.h"
#include "externals.h"

namespace CcpParser
{

class Program;

struct Observer
{
	virtual void OnVariable( const Variable* ) = 0;
	virtual void OnFunction( const Function* ) = 0;
};

ParseResult Parse(
	const char* expression,
	const Externals& externals,
	Program& program,
	Observer* observer = nullptr );

class Program
{
public:
	size_t GetTempArenaSize() const;
	float Eval( void** externData, void* tempArena ) const;

	std::string Disassemble() const;
	bool IsValid() const;
	operator bool() const;

private:
	std::vector<uint8_t> m_code;
	uint32_t m_codeStart = 0;
	uint32_t m_arenaSize = 0;

	friend class CodeBuilder;
};

}