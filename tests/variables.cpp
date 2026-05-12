// Copyright © 2022 CCP ehf.

#include "pch.h"
#include "helpers.h"

using namespace CcpParser;


TEST(Variables, CanEvalVariable)
{
	ASSERT_EQ(123.f, TestEval("x", { { "x", 123 } }));
}

TEST(Variables, CanEvalVariableExpression)
{
	ASSERT_EQ(133.f, TestEval("x + 10", { { "x", 123 } }));
}

TEST(Variables, CanHaveMultipleVariables)
{
	ASSERT_EQ(120.f, TestEval("x - y", { { "x", 123 }, { "y", 3 } }));
}

TEST(Variables, CanHaveMultipleBuffers)
{
	std::vector<Variable> vars;
	vars.push_back({ "x", 0, 0 });
	vars.push_back({ "y", 1, 0 });

	VariableView varViews[] = { vars };

	Externals externals;
	externals.variables = varViews;
	Program program;
	ASSERT_TRUE( Parse("x - y", externals, program) );

	float x = 3;
	float y = 4;

	std::unique_ptr<uint8_t> arena(new uint8_t[program.GetTempArenaSize()]);
	void* ext[] = { &x, &y };

	ASSERT_EQ(-1, program.Eval(ext, arena.get()));
}

TEST(Variables, ErrorsOnUndefinedVariable)
{
	ASSERT_FALSE(TestEval("1 + x"));
}

TEST(Variables, CanHaveLongVariableNames)
{
	ASSERT_EQ( 6, TestEval( "__aVery_Long12_34_VariableName + __aVery_Long12_34_VariableName", { { "__aVery_Long12_34_VariableName", 3 } } ) );
}

namespace
{
const Variable s_xvar = { "x", 0, 0 };
const Variable s_yvar = { "y", 1, 0 };

const Variable* TestVariableFactory( void* ctx, const StringView& name )
{
	if( name == "x" )
	{
		return &s_xvar;
	}
	if( name == "y" )
	{
		return &s_yvar;
	}
	return nullptr;
}

}

TEST(Variables, CanUseVariableFactory)
{
	Variable var = { "x", 0, 0 };
	float x = 123;
	float y = 10;
	float* buffers[] = { &x, &y };

	Externals externals;
	externals.variableFactory = { TestVariableFactory, nullptr };
	Program program;

	ASSERT_TRUE( Parse( "x + y", externals, program ) );
	std::unique_ptr<uint8_t> arena( new uint8_t[program.GetTempArenaSize()] );
	ASSERT_EQ( 133, program.Eval( (void**)buffers, arena.get() ) );

	ASSERT_FALSE( Parse( "x + y + z", externals, program ) );
}

TEST(Constants, CanUseConstants)
{
	Constant c{ "pi", 3.14f };
	ConstantView constViews[] = { { &c, 1 } };

	Externals externals;
	externals.constants = constViews;

	Program program;
	ASSERT_TRUE(Parse("pi", externals, program));

	std::unique_ptr<uint8_t> arena(new uint8_t[program.GetTempArenaSize()]);
	ASSERT_EQ(c.value, program.Eval(nullptr, arena.get()));
}

TEST( Constants, CanUseStdConstants )
{
	ASSERT_EQ( 3, TestEval( "rint(_pi)" ) );
}

namespace
{
struct TestObserver: public Observer
{
	void OnVariable( const Variable* variable ) override
	{
		variables.insert( variable );
	}
	
	void OnFunction( const Function* function ) override
	{
		functions.insert( function );
	}


	std::set<const Variable*> variables;
	std::set<const Function*> functions;


};

float test1( float x )
{
	return x + 1;
}

}

TEST( Observers, CanObserveVariableUsage )
{
	Variable vars[] = {
		{ "x", 0, 0 },
	};
	VariableView variableViews[] = { { vars, 1 } };

	Externals externals;
	externals.variables = variableViews;

	TestObserver observer;
	Program program;
	ASSERT_TRUE( Parse( "x + 1", externals, program, &observer ) );
	ASSERT_TRUE( observer.functions.empty() );
	ASSERT_EQ( std::set<const Variable*>{ &vars[0] }, observer.variables );
}

TEST( Observers, VariablesObservedAfterOptimizations )
{
	Variable vars[] = {
		{ "x", 0, 0 },
	};
	VariableView variableViews[] = { { vars, 1 } };

	Externals externals;
	externals.variables = variableViews;

	TestObserver observer;
	Program program;
	ASSERT_TRUE( Parse( "1 ? 2 : x", externals, program, &observer ) );
	ASSERT_TRUE( observer.functions.empty() );
	ASSERT_TRUE( observer.variables.empty() );
}

TEST( Observers, CanObserveFunctionUsage )
{
	Function t( "t", &test1 );
	View<Function> funcs = { &t, 1 };

	Externals externals;
	externals.functions.count = 1;
	externals.functions.data = &funcs;

	TestObserver observer;
	Program program;
	ASSERT_TRUE( Parse( "t(1)", externals, program, &observer ) );
	ASSERT_TRUE( observer.variables.empty() );
	ASSERT_EQ( std::set<const Function*>{ &t }, observer.functions );
}

TEST( Observers, FunctionsObservedAfterOptimizations )
{
	Function t( "t", &test1, FunctionFlags::PURE_FUNC );
	View<Function> funcs = { &t, 1 };

	Externals externals;
	externals.functions.count = 1;
	externals.functions.data = &funcs;

	TestObserver observer;
	Program program;
	ASSERT_TRUE( Parse( "t(1)", externals, program, &observer ) );
	ASSERT_TRUE( observer.variables.empty() );
	ASSERT_TRUE( observer.functions.empty() );
}
