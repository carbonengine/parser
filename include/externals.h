// Copyright © 2022 CCP ehf.

#pragma once

#include <cstdint>
#include "stringview.h"

namespace CcpParser
{

using OffsetType = uint32_t;
using ValueType = float;

struct Variable
{
	// Variable name
	const char* name;
	// Argument buffer index
	uint32_t buffer;
	// Offset into the argument buffer where the variable value is stored
	OffsetType offset;
};

struct Constant
{
	// Constant name
	const char* name;
	// Constant value
	ValueType value;
};

enum class FunctionFlags : uint16_t
{
	NONE = 0,
	// Function is pure: it's result value only depends on values of arguments and has no side effects.
	// Such functions may get optimized-out (and called) during parsing phase.
	PURE_FUNC = 1 << 0,
	// Function has void* context parameter.
	USES_CONTEXT = 1 << 1,
	// Function has a string parameter (the first parameter).
	USES_STRING_ARG = 1 << 2,
};

inline FunctionFlags operator|( FunctionFlags x, FunctionFlags y )
{
	return FunctionFlags( uint16_t( x ) | uint16_t( y ) );
}

inline bool HasFlag( FunctionFlags value, FunctionFlags flag )
{
	return ( uint16_t( value ) & uint16_t( flag ) ) != 0;
}


// Max number of arguments to a function (including an optional string parameter)
constexpr size_t MAX_FUNCTION_ARGUMENTS = 8;

struct Function
{
	Function() = default;

	template <typename... Args>
	Function( const char* name_, float ( *func )( Args... ), FunctionFlags flags_ = FunctionFlags::NONE ) :
		name( name_ ),
		function( reinterpret_cast<void*>( func ) ),
		arity( sizeof...( Args ) ),
		ctxBuffer( 0 ),
		ctxOffset( 0 ),
		flags( flags_ )
	{
		static_assert( sizeof...( Args ) <= MAX_FUNCTION_ARGUMENTS, "function has too many arguments" );
		static_assert( std::conjunction<std::is_same<Args, float>...>::value, "parser expects functions with float arguments only" );
	}

	template <typename Ctx, typename... Args>
	Function( const char* name_, float ( *func )( Ctx* ctx, Args... ), uint32_t ctxBuffer_, OffsetType ctxOffset_, FunctionFlags flags_ = FunctionFlags::NONE ) :
		name( name_ ),
		function( reinterpret_cast<void*>( func ) ),
		arity( sizeof...( Args ) ),
		ctxBuffer( ctxBuffer_ ),
		ctxOffset( ctxOffset_ ),
		flags( flags_ | FunctionFlags::USES_CONTEXT )
	{
		static_assert( sizeof...( Args ) <= MAX_FUNCTION_ARGUMENTS, "function has too many arguments" );
		static_assert( std::conjunction<std::is_same<Args, float>...>::value, "parser expects functions with float arguments only" );
	}

	template <typename... Args>
	Function( const char* name_, float ( *func )( const char*, Args... ), FunctionFlags flags_ = FunctionFlags::NONE ) :
		name( name_ ),
		function( reinterpret_cast<void*>( func ) ),
		arity( sizeof...( Args ) ),
		ctxBuffer( 0 ),
		ctxOffset( 0 ),
		flags( flags_ | FunctionFlags::USES_STRING_ARG )
	{
		static_assert( sizeof...( Args ) <= MAX_FUNCTION_ARGUMENTS, "function has too many arguments" );
		static_assert( std::conjunction<std::is_same<Args, float>...>::value, "parser expects functions with float arguments only" );
	}

	template <typename Ctx, typename... Args>
	Function( const char* name_, float ( *func )( Ctx* ctx, const char*, Args... ), uint32_t ctxBuffer_, OffsetType ctxOffset_, FunctionFlags flags_ = FunctionFlags::NONE ) :
		name( name_ ),
		function( reinterpret_cast<void*>( func ) ),
		arity( sizeof...( Args ) ),
		ctxBuffer( ctxBuffer_ ),
		ctxOffset( ctxOffset_ ),
		flags( flags_ | FunctionFlags::USES_CONTEXT | FunctionFlags::USES_STRING_ARG )
	{
		static_assert( sizeof...( Args ) + 1 <= MAX_FUNCTION_ARGUMENTS, "function has too many arguments" );
		static_assert( std::conjunction<std::is_same<Args, float>...>::value, "parser expects functions with float arguments only" );
	}

	template <typename... Args>
	float Call( void* ctx, const char* str, Args... args ) const
	{
		if( HasFlag( flags, FunctionFlags::USES_CONTEXT ) )
		{
			if( HasFlag( flags, FunctionFlags::USES_STRING_ARG ) )
			{
				return reinterpret_cast<float ( * )( void*, const char*, Args... )>( function )( ctx, str, args... );
			}
			else
			{
				return reinterpret_cast<float ( * )( void*, Args... )>( function )( ctx, args... );
			}
		}
		else
		{
			if( HasFlag( flags, FunctionFlags::USES_STRING_ARG ) )
			{
				return reinterpret_cast<float ( * )( const char*, Args... )>( function )( str, args... );
			}
			else
			{
				return reinterpret_cast<float ( * )( Args... )>( function )( args... );
			}
		}
	}

	// Function name
	const char* name = nullptr;
	// Typeless function pointer
	void* function = nullptr;
	// Number of parameters (not counting context or string parameters)
	uint16_t arity = 0;
	// If the function has void* context parameter, this is the index of the arguments buffer where the context parameter
	// is stored.
	uint32_t ctxBuffer = 0;
	// If the function has void* context parameter, this is the offset into the arguments buffer where the context parameter
	// is stored.
	OffsetType ctxOffset = 0;
	// Function flags
	FunctionFlags flags = FunctionFlags::NONE;
};

template <typename T>
struct View
{
	View() = default;
	View( const T* data_, size_t count_ ) :
		data( data_ ),
		count( count_ )
	{
	}
	View( const std::vector<T>& vector ) :
		data( vector.data() ),
		count( vector.size() )
	{
	}
	template <size_t Length>
	View( const T (&array)[Length] ) :
		data( array ),
		count( Length )
	{
	}

	View& operator=( const std::vector<T>& vector )
	{
		data = vector.data();
		count = vector.size();
		return *this;
	}
	template <size_t Length>
	View& operator=( const T (&array)[Length] )
	{
		data = array;
		count = Length;
		return *this;
	}


	const T* data = nullptr;
	size_t count = 0;
};

template <typename T>
const T* begin( const View<T>& view )
{
	return view.data;
}

template <typename T>
const T* end( const View<T>& view )
{
	return view.data + view.count;
}

using VariableView = View<Variable>;
using FunctionView = View<Function>;
using ConstantView = View<Constant>;


struct VariableFactory
{
	const Variable* ( *findVariable )( void* context, const StringView& name ) = nullptr;
	void* context = nullptr;
};

struct Externals
{
	View<VariableView> variables;
	View<FunctionView> functions;
	View<ConstantView> constants;

	VariableFactory variableFactory;
};


}