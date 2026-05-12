// Copyright © 2022 CCP ehf.

#include "pch.h"
#include "parser.h"
#include "../include/ccpparser.h"
#include "codebuilder.h"
#include "parserstate.h"


namespace
{
template <typename T>
void AddData( std::vector<uint8_t>& buffer, const T& value )
{
	auto ptr = reinterpret_cast<const uint8_t*>( &value );
	buffer.insert( end( buffer ), ptr, ptr + sizeof( value ) );
}

CcpParser::OpCode GetBinaryOpCode( int tokenType )
{
	switch( tokenType )
	{
	case OP_EQ:
		return CcpParser::OpCode::FLOAT_EQ;
	case OP_NE:
		return CcpParser::OpCode::FLOAT_NE;
	case OP_GE:
		return CcpParser::OpCode::FLOAT_GE;
	case OP_LE:
		return CcpParser::OpCode::FLOAT_LE;
	case OP_GT:
		return CcpParser::OpCode::FLOAT_GT;
	case OP_LT:
		return CcpParser::OpCode::FLOAT_LT;
	case OP_PLUS:
		return CcpParser::OpCode::FLOAT_ADD;
	case OP_MINUS:
		return CcpParser::OpCode::FLOAT_SUB;
	case OP_TIMES:
		return CcpParser::OpCode::FLOAT_MUL;
	case OP_DIVIDE:
		return CcpParser::OpCode::FLOAT_DIV;
	case OP_MOD:
		return CcpParser::OpCode::FLOAT_MOD;
	case OP_EXP:
		return CcpParser::OpCode::FLOAT_EXP;
	case OP_AND:
		return CcpParser::OpCode::FLOAT_AND;
	case OP_OR:
		return CcpParser::OpCode::FLOAT_OR;
	default:
		assert( false );
		return CcpParser::OpCode::FLOAT_ADD;
	}
}

uint32_t BitScan( uint64_t mask )
{
#if _WIN32
	unsigned long idx = 0;
	_BitScanForward64( &idx, mask );
	return uint32_t( idx );
#elif __APPLE__
	return uint32_t( __builtin_ctzll( mask ) );
#endif
}

void PadToAlignment( std::vector<uint8_t>& data )
{
	while( data.size() % CcpParser::CONST_ALIGNMENT != 0 )
	{
		data.push_back( 0 );
	}
}
}

namespace CcpParser
{
bool UnescapeString( const StringView& str, std::vector<uint8_t>& buffer );
bool CompareUnescapedString( const StringView& str, const char* escapedString, const char* escapedStringEnd );


Register CodeBuilder::AddConst( float constant )
{
	for( uint32_t offset = 0; offset + CONST_ALIGNMENT <= uint32_t( m_constData.size() ); offset += CONST_ALIGNMENT )
	{
		if( *reinterpret_cast<float*>( m_constData.data() + offset ) == constant )
		{
			return Register{ offset, Register::CONST };
		}
	}

	auto offset = uint32_t( m_constData.size() );
	AddData( m_constData, constant );
	PadToAlignment( m_constData );
	return Register{ offset, Register::CONST };
}

Register CodeBuilder::AddConst( const StringView& constant )
{
	for( uint32_t offset = 0; offset + CONST_ALIGNMENT <= uint32_t( m_constData.size() ); offset += CONST_ALIGNMENT )
	{
		if( CompareUnescapedString( constant, reinterpret_cast<const char*>( m_constData.data() + offset ), reinterpret_cast<const char*>( m_constData.data() + m_constData.size() ) ) )
		{
			return Register{ offset, Register::CONST };
		}
	}

	auto offset = uint32_t( m_constData.size() );
	UnescapeString( constant, m_constData );
	m_constData.push_back( 0 );
	PadToAlignment( m_constData );
	return Register{ offset, Register::CONST };
}

Register CodeBuilder::AddFunction( const Function& function )
{
	PackedFunction pf = { reinterpret_cast<uint64_t>( function.function ), function.flags, function.arity, function.ctxBuffer, function.ctxOffset };
	for( uint32_t offset = 0; offset + std::max( CONST_ALIGNMENT, uint32_t( sizeof( pf ) ) ) <= uint32_t( m_constData.size() ); offset += CONST_ALIGNMENT )
	{
		if( memcmp( m_constData.data() + offset, &pf, sizeof( pf ) ) == 0 )
		{
			return Register{ offset, Register::CONST };
		}
	}

	auto offset = uint32_t( m_constData.size() );
	AddData( m_constData, pf );
	PadToAlignment( m_constData );
	return Register{ offset, Register::CONST };
}

size_t CodeBuilder::AddJump( OpCode code )
{
	AddCode( code );
	auto offset = m_code.size();
	AddData( m_code, uint32_t( 0 ) );
	return offset;
}

size_t CodeBuilder::AddJump( OpCode code, Register arg0 )
{
	AddCode( code );
	AddRegister( arg0 );
	auto offset = m_code.size();
	AddData( m_code, uint32_t( 0 ) );
	return offset;
}

void CodeBuilder::ResolveAddr( size_t offset )
{
	uint32_t addr = uint32_t( m_code.size() );
	*reinterpret_cast<uint32_t*>( m_code.data() + offset ) = addr;
}

Register CodeBuilder::AllocateTemp()
{
	auto idx = BitScan( m_availableTemps );
	m_availableTemps &= ~( uint64_t( 1 ) << idx );

	Register r = { uint32_t( idx * sizeof( float ) ), Register::TEMP };
	m_tempSize = std::max( m_tempSize, uint32_t( r.offset + sizeof( float ) ) );
	return r;
}

void CodeBuilder::DeallocateTemp( Register reg )
{
	if( reg.type == Register::TEMP )
	{
		m_availableTemps |= uint64_t( 1 ) << ( reg.offset / sizeof( float ) );
	}
}

void CodeBuilder::AddRegister( Register arg0 )
{
	AddData( m_code, arg0 );
}

void CodeBuilder::AddCode( OpCode code )
{
	AddData( m_code, code );
}

void CodeBuilder::GetProgram( Program& program ) const
{
	program.m_code.clear();
	program.m_code.reserve( m_code.size() + m_constData.size() );
	program.m_code.insert( end( program.m_code ), begin( m_constData ), end( m_constData ) );
	program.m_codeStart = uint32_t( program.m_code.size() );
	program.m_code.insert( end( program.m_code ), begin( m_code ), end( m_code ) );
	program.m_arenaSize = m_tempSize;
}

Register CodeBuilder::BuildCode( const Node* node, const ParserState& state )
{
	switch( node->nodeType )
	{
	case Node::FLOAT_CONST:
		return AddConst( node->token.value );
	case Node::STRING_CONST:
		return AddConst( node->token.string );
	case Node::VARIABLE:
		if( m_observer )
		{
			m_observer->OnVariable( node->variable );
		}
		return Register{ node->variable->offset, Register::VARIABLE + node->variable->buffer };
	case Node::BINARY_OP: {
		auto l = BuildCode( node->children[0].get(), state );
		auto r = BuildCode( node->children[1].get(), state );
		Register d;
		if( l.type == Register::TEMP )
		{
			d = l;
		}
		else if( r.type == Register::TEMP )
		{
			d = r;
		}
		else
		{
			d = AllocateTemp();
		}
		AddCode( GetBinaryOpCode( node->token.type ) );
		AddRegister( d );
		AddRegister( l );
		AddRegister( r );
		if( d != l)
		{
			DeallocateTemp( l );
		}
		if( d != r)
		{
			DeallocateTemp( r );
		}
		return d;
	}
	case Node::UNARY_OP: {
		auto l = BuildCode( node->children[0].get(), state );
		Register d;
		if( l.type == Register::TEMP )
		{
			d = l;
		}
		else
		{
			d = AllocateTemp();
		}
		switch( node->token.type )
		{
		case OP_MINUS:
			AddCode( OpCode::FLOAT_NEG );
			break;
		case OP_NOT:
			AddCode( OpCode::FLOAT_NOT );
			break;
		default:
			assert( false );
		}
		AddRegister( d );
		AddRegister( l );
		if( d != l )
		{
			DeallocateTemp( l );
		}
		return d;
	}
	case Node::FUNCTION_CALL: {
		if( m_observer )
		{
			m_observer->OnFunction( node->function );
		}

		std::array<Register, MAX_FUNCTION_ARGUMENTS> args;
		auto& func = *node->function;
		auto arity = func.arity;

		if( HasFlag( func.flags, FunctionFlags::USES_STRING_ARG ) )
		{
			++arity;
		}

		for( uint32_t i = 0; i < arity; ++i )
		{
			args[i] = BuildCode( node->children[i].get(), state );
		}
		auto d = AllocateTemp();
		AddData( m_code, OpCode::CALL );
		AddRegister( AddFunction( func ) );
		AddRegister( d );
		for( uint32_t i = 0; i < arity; ++i )
		{
			AddRegister( args[i] );
			DeallocateTemp( args[i] );
		}
		return d;
	}
	case Node::CONDITIONAL: {
		auto c = BuildCode( node->children[0].get(), state );
		auto r = c.type == Register::TEMP ? c : AllocateTemp();
		auto addrF = AddJump( OpCode::JUMP_Z, c );
		auto t = BuildCode( node->children[1].get(), state );
		AddCode( OpCode::MOVE );
		AddRegister( r );
		AddRegister( t );
		DeallocateTemp( t );
		auto addrE = AddJump( OpCode::JUMP );
		ResolveAddr( addrF );
		auto f = BuildCode( node->children[2].get(), state );
		AddCode( OpCode::MOVE );
		AddRegister( r );
		AddRegister( f );
		DeallocateTemp( f );
		ResolveAddr( addrE );
		return r;
	}
	default:
		assert( false );
	}
	return {};
}

void CodeBuilder::BuildCode( const ParserState& state, Program& program, Observer* observer )
{
	CodeBuilder builder;
	builder.m_code.reserve( 64 );
	builder.m_constData.reserve( 64 );

	builder.m_observer = observer;
	auto ret = builder.BuildCode( state.root.get(), state );
	builder.AddCode( OpCode::RET );
	builder.AddRegister( ret );
	builder.GetProgram( program );
}
}