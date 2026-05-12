// Copyright © 2022 CCP ehf.

#pragma once

#include "../include/externals.h"

namespace CcpParser
{

constexpr uint32_t CONST_ALIGNMENT = 4;

enum class OpCode : uint8_t
{
	MOVE, // dest, src0

	FLOAT_ADD, // dest, src0, src1
	FLOAT_SUB, // dest, src0, src1
	FLOAT_MUL, // dest, src0, src1
	FLOAT_DIV, // dest, src0, src1
	FLOAT_MOD, // dest, src0, src1
	FLOAT_EXP, // dest, src0, src1
	FLOAT_NEG, // dest, src0
	FLOAT_NOT, // dest, src0

	FLOAT_EQ, // dest, src0, src1
	FLOAT_NE, // dest, src0, src1
	FLOAT_GE, // dest, src0, src1
	FLOAT_LE, // dest, src0, src1
	FLOAT_GT, // dest, src0, src1
	FLOAT_LT, // dest, src0, src1

	FLOAT_OR, // dest, src0, src1
	FLOAT_AND, // dest, src0, src1

	CALL, // func, dest, src0, src1, ...
	RET, // src0
	JUMP_Z, // src0, addr
	JUMP_NZ, // src0, addr
	JUMP, // addr
};

struct PackedFunction
{
	uint64_t fp;
	FunctionFlags flags;
	uint16_t arity;
	uint32_t ctxBuffer : 8;
	uint32_t ctxOffset : 24;

	template <typename... Args>
	float Call( void** ctx, const char* str, Args... args ) const
	{
		if( HasFlag( flags, FunctionFlags::USES_CONTEXT ) )
		{
			if( HasFlag( flags, FunctionFlags::USES_STRING_ARG ) )
			{
				return reinterpret_cast<float ( * )( void*, const char*, Args... )>( fp )( *ctx, str, args... );
			}
			else
			{
				return reinterpret_cast<float ( * )( void*, Args... )>( fp )( *ctx, args... );
			}
		}
		else
		{
			if( HasFlag( flags, FunctionFlags::USES_STRING_ARG ) )
			{
				return reinterpret_cast<float ( * )( const char*, Args... )>( fp )( str, args... );
			}
			else
			{
				return reinterpret_cast<float ( * )( Args... )>( fp )( args... );
			}
		}
	}
};

static_assert( sizeof( PackedFunction ) == 16 );

struct Register
{
	enum Type
	{
		CONST = 0,
		TEMP = 1,
		VARIABLE = 2,
	};

	uint32_t offset : 22;
	uint32_t type : 10;
};

inline bool operator==( const Register& r0, const Register& r1 )
{
	return r0.type == r1.type && r0.offset == r1.offset;
}

inline bool operator!=( const Register& r0, const Register& r1 )
{
	return r0.type != r1.type || r0.offset != r1.offset;
}

static_assert( sizeof( Register ) == 4 );
}