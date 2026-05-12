// Copyright © 2022 CCP ehf.

#include "pch.h"
#include "../include/ccpparser.h"
#include "codebuilder.h"
#include "parserstate.h"


namespace CcpParser
{
// Implementation comes from Lemon
void* ParseAlloc( void* ( *mallocProc )( size_t ) );
void ParseFree( void* p, void ( *freeProc )( void* ) );
void Parse( void* yyp, int yymajor, CcpParser::Token token, CcpParser::ParserState* parser );
void ParseTrace( FILE* TraceFILE, char* zTracePrompt );

// Implementation is in scanner.re
Token ScanToken( const char* s );

ParseResult Parse(
	const char* expression,
	const Externals& externals,
	Program& program,
	Observer* observer )
{
	ParserState state;
	state.externals = &externals;

	auto parser = ParseAlloc( &malloc );
	auto cursor = expression;
	while( state.result )
	{
		auto token = ScanToken( cursor );
		if( token.type == OP_EOF )
		{
			break;
		}
		if( token.type == OP_ERROR )
		{
			state.result = { ParseResult::SCANNER_ERROR, token.string };
			break;
		}
		cursor = token.string.end;
		CcpParser::Parse( parser, token.type, token, &state );
	}
	if( state.result )
	{
		CcpParser::Parse( parser, OP_EOF, Token(), &state );
	}
	ParseFree( parser, &free );
	if( !state.result )
	{
		return state.result;
	}

	CodeBuilder::BuildCode( state, program, observer );
	return { ParseResult::OK };
}


}