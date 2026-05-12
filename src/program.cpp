// Copyright © 2022 CCP ehf.

#include "pch.h"
#include "../include/ccpparser.h"
#include "code.h"


namespace
{
template <typename T>
const T& Read( const uint8_t*& code )
{
	auto& t = *reinterpret_cast<const T*>( code );
	code += sizeof( T );
	return t;
}

template <typename T>
T& FetchRegister( const uint8_t*& code, const void* constants, const void* temps, void** externals )
{
	auto& reg = Read<CcpParser::Register>( code );
	switch( reg.type )
	{
	case CcpParser::Register::CONST:
		return *(T*)( static_cast<const uint8_t*>( constants ) + reg.offset );
	case CcpParser::Register::TEMP:
		return *(T*)( static_cast<const uint8_t*>( temps ) + reg.offset );
	default:
		return *(T*)( static_cast<const uint8_t*>( externals[reg.type - 2] ) + reg.offset );
	}
}
}

namespace CcpParser
{

size_t Program::GetTempArenaSize() const
{
	return m_arenaSize > 32 ? m_arenaSize : 0;
}

float Program::Eval( void** externData, void* tempArena ) const
{
	auto code = m_code.data() + m_codeStart;
	uint8_t staticArena[32];
	if( m_arenaSize <= 32 )
	{
		tempArena = staticArena;
	}
	for( ;; )
	{
		switch( OpCode( *code++ ) )
		{
		case OpCode::FLOAT_EQ: {
			auto& dest = FetchRegister<float>( code, m_code.data(), tempArena, externData );
			auto& src0 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
			auto& src1 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
			dest = src0 == src1 ? 1.f : 0.f;
			break;
		}
		case OpCode::FLOAT_NE: {
			auto& dest = FetchRegister<float>( code, m_code.data(), tempArena, externData );
			auto& src0 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
			auto& src1 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
			dest = src0 != src1 ? 1.f : 0.f;
			break;
		}
		case OpCode::FLOAT_GE: {
			auto& dest = FetchRegister<float>( code, m_code.data(), tempArena, externData );
			auto& src0 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
			auto& src1 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
			dest = src0 >= src1 ? 1.f : 0.f;
			break;
		}
		case OpCode::FLOAT_LE: {
			auto& dest = FetchRegister<float>( code, m_code.data(), tempArena, externData );
			auto& src0 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
			auto& src1 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
			dest = src0 <= src1 ? 1.f : 0.f;
			break;
		}
		case OpCode::FLOAT_GT: {
			auto& dest = FetchRegister<float>( code, m_code.data(), tempArena, externData );
			auto& src0 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
			auto& src1 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
			dest = src0 > src1 ? 1.f : 0.f;
			break;
		}
		case OpCode::FLOAT_LT: {
			auto& dest = FetchRegister<float>( code, m_code.data(), tempArena, externData );
			auto& src0 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
			auto& src1 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
			dest = src0 < src1 ? 1.f : 0.f;
			break;
		}
		case OpCode::FLOAT_ADD: {
			auto& dest = FetchRegister<float>( code, m_code.data(), tempArena, externData );
			auto& src0 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
			auto& src1 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
			dest = src0 + src1;
			break;
		}
		case OpCode::FLOAT_SUB: {
			auto& dest = FetchRegister<float>( code, m_code.data(), tempArena, externData );
			auto& src0 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
			auto& src1 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
			dest = src0 - src1;
			break;
		}
		case OpCode::FLOAT_MUL: {
			auto& dest = FetchRegister<float>( code, m_code.data(), tempArena, externData );
			auto& src0 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
			auto& src1 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
			dest = src0 * src1;
			break;
		}
		case OpCode::FLOAT_DIV: {
			auto& dest = FetchRegister<float>( code, m_code.data(), tempArena, externData );
			auto& src0 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
			auto& src1 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
			dest = src0 / src1;
			break;
		}
		case OpCode::FLOAT_MOD: {
			auto& dest = FetchRegister<float>( code, m_code.data(), tempArena, externData );
			auto& src0 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
			auto& src1 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
			dest = float( fmod( src0, src1 ) );
			break;
		}
		case OpCode::FLOAT_EXP: {
			auto& dest = FetchRegister<float>( code, m_code.data(), tempArena, externData );
			auto& src0 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
			auto& src1 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
			dest = float( pow( src0, src1 ) );
			break;
		}
		case OpCode::FLOAT_AND: {
			auto& dest = FetchRegister<float>( code, m_code.data(), tempArena, externData );
			auto& src0 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
			auto& src1 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
			dest = src0 && src1;
			break;
		}
		case OpCode::FLOAT_OR: {
			auto& dest = FetchRegister<float>( code, m_code.data(), tempArena, externData );
			auto& src0 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
			auto& src1 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
			dest = src0 || src1;
			break;
		}
		case OpCode::FLOAT_NEG: {
			auto& dest = FetchRegister<float>( code, m_code.data(), tempArena, externData );
			auto& src0 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
			dest = -src0;
			break;
		}
		case OpCode::FLOAT_NOT: {
			auto& dest = FetchRegister<float>( code, m_code.data(), tempArena, externData );
			auto& src0 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
			dest = src0 == 0 ? 1.f : 0.f;
			break;
		}
		case OpCode::CALL: {
			const auto& f = FetchRegister<PackedFunction>( code, m_code.data(), tempArena, externData );
			auto& dest = FetchRegister<float>( code, m_code.data(), tempArena, externData );
			void** ctx = reinterpret_cast<void**>( static_cast<uint8_t*>( externData[f.ctxBuffer] ) + f.ctxOffset );
			const char* str = nullptr;
			if( HasFlag( f.flags, FunctionFlags::USES_STRING_ARG ) )
			{
				str = &FetchRegister<char>( code, m_code.data(), tempArena, externData );
			}
			switch( f.arity )
			{
			case 0:
				dest = f.Call( ctx, str );
				break;
			case 1: {
				auto& a0 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
				dest = f.Call( ctx, str, a0 );
				break;
			}
			case 2: {
				auto& a0 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
				auto& a1 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
				dest = f.Call( ctx, str, a0, a1 );
				break;
			}
			case 3: {
				auto& a0 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
				auto& a1 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
				auto& a2 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
				dest = f.Call( ctx, str, a0, a1, a2 );
				break;
			}
			case 4: {
				auto& a0 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
				auto& a1 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
				auto& a2 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
				auto& a3 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
				dest = f.Call( ctx, str, a0, a1, a2, a3 );
				break;
			}
			case 5: {
				auto& a0 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
				auto& a1 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
				auto& a2 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
				auto& a3 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
				auto& a4 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
				dest = f.Call( ctx, str, a0, a1, a2, a3, a4 );
				break;
			}
			case 6: {
				auto& a0 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
				auto& a1 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
				auto& a2 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
				auto& a3 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
				auto& a4 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
				auto& a5 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
				dest = f.Call( ctx, str, a0, a1, a2, a3, a4, a5 );
				break;
			}
			case 7: {
				auto& a0 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
				auto& a1 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
				auto& a2 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
				auto& a3 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
				auto& a4 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
				auto& a5 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
				auto& a6 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
				dest = f.Call( ctx, str, a0, a1, a2, a3, a4, a5, a6 );
				break;
			}
			case 8: {
				auto& a0 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
				auto& a1 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
				auto& a2 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
				auto& a3 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
				auto& a4 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
				auto& a5 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
				auto& a6 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
				auto& a7 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
				dest = f.Call( ctx, str, a0, a1, a2, a3, a4, a5, a6, a7 );
				break;
			}
			default:
				static_assert( 8 == MAX_FUNCTION_ARGUMENTS, "need to handle extra function argument count" );
				assert( false );
				return 0;
			}
			break;
		}
		case OpCode::RET:
			return FetchRegister<float>( code, m_code.data(), tempArena, externData );
		case OpCode::JUMP: {
			auto addr = *reinterpret_cast<const uint32_t*>( code );
			code = m_code.data() + m_codeStart + addr;
			break;
		}
		case OpCode::JUMP_Z: {
			auto& src0 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
			if( src0 == 0 )
			{
				auto addr = *reinterpret_cast<const uint32_t*>( code );
				code = m_code.data() + m_codeStart + addr;
			}
			else
			{
				code += 4;
			}
			break;
		}
		case OpCode::JUMP_NZ: {
			auto& src0 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
			if( src0 != 0 )
			{
				auto addr = *reinterpret_cast<const uint32_t*>( code );
				code = m_code.data() + m_codeStart + addr;
			}
			else
			{
				code += 4;
			}
			break;
		}
		case OpCode::MOVE: {
			auto& dest = FetchRegister<float>( code, m_code.data(), tempArena, externData );
			auto& src0 = FetchRegister<float>( code, m_code.data(), tempArena, externData );
			dest = src0;
			break;
		}
		default:
			assert( false );
			return 0;
		}
	}
}

namespace
{
std::ostream& operator<<( std::ostream& os, const Register& r )
{
	auto offset = r.offset;
	switch( r.type )
	{
	case Register::CONST:
		return os << 'c' << offset;
	case Register::TEMP:
		return os << 't' << offset;
	default:
		return os << "e[" << ( r.type - 2 ) << "][" << offset << ']';
	}
}

}
std::string Program::Disassemble() const
{
	std::stringstream os;

	std::vector<uint32_t> floatConsts;
	std::vector<uint32_t> stringConsts;
	std::vector<uint32_t> funcConsts;

	auto RegisterRefF = [&]( const uint8_t*& code ) -> Register {
		auto r = Read<Register>( code );
		if( r.type == Register::CONST )
		{
			floatConsts.push_back( r.offset );
		}
		return r;
	};

	auto RegisterRefS = [&]( const uint8_t*& code ) -> Register {
		auto r = Read<Register>( code );
		if( r.type == Register::CONST )
		{
			stringConsts.push_back( r.offset );
		}
		return r;
	};

	auto RegisterRefFunc = [&]( const uint8_t*& code ) -> Register {
		auto r = Read<Register>( code );
		if( r.type == Register::CONST )
		{
			funcConsts.push_back( r.offset );
		}
		return r;
	};
	std::vector<uint32_t> labels;
	auto codeStart = m_code.data() + m_codeStart;
	auto code = codeStart;
	os << "temps: " << m_arenaSize << std::endl;
	os << "code:" << std::endl;
	while( code < m_code.data() + m_code.size() )
	{
		auto offset = uint32_t( code - codeStart );
		if( std::find( begin( labels ), end( labels ), offset ) != end( labels ) )
		{
			os << offset << ':' << std::endl;
		}
		switch( Read<OpCode>( code ) )
		{
		case OpCode::FLOAT_OR:
			os << "  or_f " << RegisterRefF( code ) << ", " << RegisterRefF( code ) << ", " << RegisterRefF( code ) << std::endl;
			break;
		case OpCode::FLOAT_AND:
			os << "  and_f " << RegisterRefF( code ) << ", " << RegisterRefF( code ) << ", " << RegisterRefF( code ) << std::endl;
			break;

		case OpCode::FLOAT_EQ:
			os << "  eq_f " << RegisterRefF( code ) << ", " << RegisterRefF( code ) << ", " << RegisterRefF( code ) << std::endl;
			break;
		case OpCode::FLOAT_NE:
			os << "  ne_f " << RegisterRefF( code ) << ", " << RegisterRefF( code ) << ", " << RegisterRefF( code ) << std::endl;
			break;
		case OpCode::FLOAT_GE:
			os << "  ge_f " << RegisterRefF( code ) << ", " << RegisterRefF( code ) << ", " << RegisterRefF( code ) << std::endl;
			break;
		case OpCode::FLOAT_LE:
			os << "  le_f " << RegisterRefF( code ) << ", " << RegisterRefF( code ) << ", " << RegisterRefF( code ) << std::endl;
			break;
		case OpCode::FLOAT_GT:
			os << "  gt_f " << RegisterRefF( code ) << ", " << RegisterRefF( code ) << ", " << RegisterRefF( code ) << std::endl;
			break;
		case OpCode::FLOAT_LT:
			os << "  lt_f " << RegisterRefF( code ) << ", " << RegisterRefF( code ) << ", " << RegisterRefF( code ) << std::endl;
			break;


		case OpCode::FLOAT_ADD:
			os << "  add_f " << RegisterRefF( code ) << ", " << RegisterRefF( code ) << ", " << RegisterRefF( code ) << std::endl;
			break;
		case OpCode::FLOAT_SUB:
			os << "  sub_f " << RegisterRefF( code ) << ", " << RegisterRefF( code ) << ", " << RegisterRefF( code ) << std::endl;
			break;
		case OpCode::FLOAT_MUL:
			os << "  mul_f " << RegisterRefF( code ) << ", " << RegisterRefF( code ) << ", " << RegisterRefF( code ) << std::endl;
			break;
		case OpCode::FLOAT_DIV:
			os << "  div_f " << RegisterRefF( code ) << ", " << RegisterRefF( code ) << ", " << RegisterRefF( code ) << std::endl;
			break;
		case OpCode::FLOAT_MOD:
			os << "  mod_f " << RegisterRefF( code ) << ", " << RegisterRefF( code ) << ", " << RegisterRefF( code ) << std::endl;
			break;
		case OpCode::FLOAT_EXP:
			os << "  pow_f " << RegisterRefF( code ) << ", " << RegisterRefF( code ) << ", " << RegisterRefF( code ) << std::endl;
			break;
		case OpCode::FLOAT_NEG:
			os << "  neg_f " << RegisterRefF( code ) << ", " << RegisterRefF( code ) << std::endl;
			break;
		case OpCode::CALL: {
			auto funcRef = RegisterRefFunc( code );
			os << "  call " << funcRef << ", " << RegisterRefF( code );

			auto f = reinterpret_cast<const PackedFunction*>( m_code.data() + funcRef.offset );
			if( HasFlag( f->flags, FunctionFlags::USES_STRING_ARG ) )
			{
				os << ", " << RegisterRefS( code );
			}
			for( uint16_t i = 0; i < f->arity; ++i )
			{
				os << ", " << RegisterRefF( code );
			}
			os << std::endl;
			break;
		}
		case OpCode::RET:
			os << "  ret " << RegisterRefF( code ) << std::endl;
			break;
		case OpCode::JUMP: {
			os << "  jump_z ";
			auto addr = Read<uint32_t>( code );
			labels.push_back( addr );
			os << addr << std::endl;
			break;
		}
		case OpCode::JUMP_Z: {
			os << "  jump_z " << RegisterRefF( code ) << ", ";
			auto addr = Read<uint32_t>( code );
			labels.push_back( addr );
			os << addr << std::endl;
			break;
		}
		case OpCode::JUMP_NZ: {
			os << "  jump_nz " << RegisterRefF( code ) << ", ";
			auto addr = Read<uint32_t>( code );
			labels.push_back( addr );
			os << addr << std::endl;
			break;
		}
		case OpCode::MOVE:
			os << "  move " << RegisterRefF( code ) << ", " << RegisterRefF( code ) << std::endl;
			break;
		default:
			os << "  ERROR!" << std::endl;
			code = m_code.data() + m_code.size();
			break;
		}
	}

	if( m_codeStart > 0 )
	{
		auto PadSize = []( uint32_t size ) {
			return ( size + CONST_ALIGNMENT - 1 ) / CONST_ALIGNMENT * CONST_ALIGNMENT;
		};
		std::stringstream constOs;
		constOs << "const:" << std::endl;
		for( uint32_t i = 0; i < m_codeStart; )
		{
			constOs << "  c" << i;
			auto found = std::find( begin( floatConsts ), end( floatConsts ), i );
			if( found != end( floatConsts ) )
			{
				constOs << "  float " << *reinterpret_cast<const float*>( m_code.data() + i ) << std::endl;
				i += PadSize( sizeof( float ) );
				continue;
			}
			found = std::find( begin( stringConsts ), end( stringConsts ), i );
			if( found != end( stringConsts ) )
			{
				auto str = reinterpret_cast<const char*>( m_code.data() + i );
				constOs << "  string \"" << str << "\"" << std::endl;
				i += PadSize( uint32_t( strlen( str ) + 1 ) );
				continue;
			}
			found = std::find( begin( funcConsts ), end( funcConsts ), i );
			if( found != end( funcConsts ) )
			{

				auto func = reinterpret_cast<const PackedFunction*>( m_code.data() + i );
				constOs << "  func 0x" << std::hex << func->fp << std::dec << '(';
				bool first = true;
				if( HasFlag( func->flags, FunctionFlags::USES_CONTEXT ) )
				{
					constOs << "void*";
					first = false;
				}
				if( HasFlag( func->flags, FunctionFlags::USES_STRING_ARG ) )
				{
					if( !first )
					{
						constOs << ", ";
					}
					constOs << "const char*";
					first = false;
				}
				for( uint16_t j = 0; j < func->arity; ++j )
				{
					if( j || !first )
					{
						constOs << ", ";
					}
					constOs << "float";
				}
				constOs << ')' << std::endl;
				i += PadSize( sizeof( PackedFunction ) );
				continue;
			}
			constOs << "  ? " << m_code[i] << std::endl;
			++i;
		}
		return constOs.str() + os.str();
	}
	else
	{
		return os.str();
	}
}

bool Program::IsValid() const
{
	return !m_code.empty();
}

Program::operator bool() const
{
	return IsValid();
}

}