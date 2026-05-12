// Copyright © 2022 CCP ehf.

#include "pch.h"
#include "../include/ccpparser.h"
#include "helpers.h"

using namespace CcpParser;

namespace
{
float test0()
{
	return 123.f;
}

bool test1Called = false;
float test1( float x )
{
	test1Called = true;
	return x + 1;
}
float test2( float x, float y )
{
	return x - y;
}
float test3( float x, float y, float z )
{
	return x + y + z;
}
float ctest1( void* ctx, float x )
{
	return x + *static_cast<float*>( ctx );
}

bool stest1Called = false;
std::string stest1Arg;
float stest1( const char* str, float x )
{
	stest1Called = true;
	stest1Arg = str;
	if( strcmp( str, "test0" ) == 0 )
	{
		return x + 1;
	}
	else
	{
		return x - 1;
	}
}
}

TEST( Functions, CanCallFunctionWithNoArgs )
{
	ASSERT_EQ( 123.f, TestEval( "t()", {}, { Function( "t", &test0 ) } ) );
}

TEST( Functions, CanCallFunction )
{
	ASSERT_EQ( 2.f, TestEval( "t(1)", {}, { Function( "t", &test1 ) } ) );
}

TEST( Functions, ErrorsOnUndefinedFunction )
{
	Program program;
	ASSERT_FALSE( Parse( "t(1)", {}, program ) );
}

TEST( Functions, ErrorsOnArityMismatch )
{
	Program program;
	Function t( "t", &test1 );
	View<Function> funcs = { &t, 1 };
	Externals externals;
	externals.functions.count = 1;
	externals.functions.data = &funcs;

	ASSERT_FALSE( Parse( "t()", externals, program ) );
	ASSERT_FALSE( Parse( "t(1, 2)", externals, program ) );
}

TEST( Functions, CanCallFunctionWithTwoArgs )
{
	ASSERT_EQ( -1.f, TestEval( "t(1, 2)", {}, { Function( "t", &test2 ) } ) );
}

TEST( Functions, CanCallMultipleFunctionsFunctions )
{
	ASSERT_EQ( 0.f, TestEval( "t1(t2(1, 2))", {}, { Function( "t1", &test1 ), Function( "t2", &test2 ) } ) );
}

TEST( Functions, CanHaveFunctionOverloads )
{
	ASSERT_EQ( 124.f, TestEval( "t(t())", {}, { Function( "t", &test0 ), Function( "t", &test1 ) } ) );
}

TEST( Functions, CanUseFunctionWithContext )
{
	std::vector<Function> functions = { Function( "t", &ctest1, 0, 0 ) };
	View<Function> funcs = functions;

	Externals externals;
	externals.functions.count = 1;
	externals.functions.data = &funcs;

	Program program;
	ASSERT_TRUE( Parse( "t(123)", externals, program ) );

	float ctx = 7;
	float* buffer = &ctx;
	void* a[] = { &buffer };

	std::unique_ptr<uint8_t> arena( new uint8_t[program.GetTempArenaSize()] );
	ASSERT_EQ( 130.f, program.Eval( a, arena.get() ) );
}

TEST( Functions, CanSpecifyBufferForFunctionWithContext )
{
	std::vector<Function> functions = { Function( "t", &ctest1, 1, 0 ) };
	View<Function> funcs = functions;

	Externals externals;
	externals.functions.count = 1;
	externals.functions.data = &funcs;

	Program program;
	ASSERT_TRUE( Parse( "t(123)", externals, program ) );

	float ctx = 7;
	float* buffer = &ctx;
	void* a[] = { nullptr, &buffer };

	std::unique_ptr<uint8_t> arena( new uint8_t[program.GetTempArenaSize()] );
	ASSERT_EQ( 130.f, program.Eval( a, arena.get() ) );
}

TEST( Functions, EvaluatesPureFunctionsOnParse )
{
	std::vector<Function> functions = { Function( "t", &test1, FunctionFlags::PURE_FUNC ) };
	View<Function> funcs = functions;

	Externals externals;
	externals.functions.count = 1;
	externals.functions.data = &funcs;

	test1Called = false;
	Program program;
	ASSERT_TRUE( Parse( "t(123)", externals, program ) );
	ASSERT_TRUE( test1Called );

	test1Called = false;
	std::unique_ptr<uint8_t> arena( new uint8_t[program.GetTempArenaSize()] );
	ASSERT_EQ( 124.f, program.Eval( nullptr, arena.get() ) );
	ASSERT_FALSE( test1Called );
}

TEST( Functions, CanUseFunctionsWithStringParameter )
{
	ASSERT_EQ( 122.f, TestEval( "t(\"a\", 123)", {}, { Function( "t", &stest1 ) } ) );
	ASSERT_EQ( 124.f, TestEval( "t(\"test0\", 123)", {}, { Function( "t", &stest1 ) } ) );
}

TEST( Functions, CanUseEscapeSequencesInStrings )
{
	ASSERT_EQ( 122.f, TestEval( "t(\"tes\\\"t0\", 123)", {}, { Function( "t", &stest1 ) } ) );
	ASSERT_EQ( "tes\"t0", stest1Arg );
}

TEST( Functions, EvaluatesPureFunctionsWithStringArgOnParse )
{
	std::vector<Function> functions = { Function( "t", &stest1, FunctionFlags::PURE_FUNC ) };
	View<Function> funcs = functions;

	Externals externals;
	externals.functions.count = 1;
	externals.functions.data = &funcs;

	stest1Called = false;
	Program program;
	ASSERT_TRUE( Parse( "t(\"abc\", 123)", externals, program ) );
	ASSERT_TRUE( stest1Called );

	stest1Called = false;
	std::unique_ptr<uint8_t> arena( new uint8_t[program.GetTempArenaSize()] );
	ASSERT_EQ( 122.f, program.Eval( nullptr, arena.get() ) );
	ASSERT_FALSE( stest1Called );
}

TEST( Functions, CanUseStandardFunctions )
{
	ASSERT_EQ( 2, TestEval( "sqrt(4)" ) );
	ASSERT_EQ( 1, TestEval( "cos(0)" ) );
	ASSERT_EQ( 3, TestEval( "max(1, 2, 3)" ) );
}
