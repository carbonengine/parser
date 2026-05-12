// Copyright © 2022 CCP ehf.

#pragma once

#include "../include/externals.h"
#include "code.h"

namespace CcpParser
{

class Program;
struct ParserState;
struct Node;
struct Observer;


class CodeBuilder
{
public:
	static void BuildCode( const ParserState& state, Program& program, Observer* observer );

private:
	Register BuildCode( const Node* node, const ParserState& state );

	Register AddConst( float constant );
	Register AddConst( const StringView& constant );
	Register AddFunction( const Function& function );
	size_t AddJump( OpCode code );
	size_t AddJump( OpCode code, Register arg0 );
	void ResolveAddr( size_t offset );
	Register AllocateTemp();
	void DeallocateTemp( Register );
	void AddRegister( Register arg0 );
	void AddCode( OpCode code );

	void GetProgram( Program& program ) const;

private:
	uint64_t m_availableTemps = 0xffffffffffffffff;

	uint32_t m_tempSize = 0;
	std::vector<uint8_t> m_constData;
	std::vector<uint8_t> m_code;
	Observer* m_observer = nullptr;
};

}