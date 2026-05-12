// Copyright © 2022 CCP ehf.

#include "pch.h"
#include "helpers.h"

using namespace CcpParser;


TEST(Operators, CanAdd)
{
	ASSERT_EQ(7, TestEval("3 + 4"));
	ASSERT_EQ(133.f, TestEval("x+10", { { "x", 123 } }));
}

TEST(Operators, CanSubtract)
{
	ASSERT_EQ(-1, TestEval("3 - 4"));
	ASSERT_EQ(2, TestEval("x-10", { { "x", 12 } }));
}

TEST(Operators, CanMultiply)
{
	ASSERT_EQ(7, TestEval("3.5 * 2"));
	ASSERT_EQ(11, TestEval("10*x", { { "x", 1.1f } }));
}

TEST(Operators, CanDivide)
{
	ASSERT_EQ(-100, TestEval("10/-0.1"));
	ASSERT_EQ(12.3f, TestEval("x/10", { { "x", 123.f } }));
}

TEST(Operators, CanModulo)
{
	ASSERT_EQ(3, TestEval("7%4"));
	ASSERT_EQ(5, TestEval("x%10", { { "x", 125.f } }));
}

TEST(Operators, CanExponent)
{
	ASSERT_EQ(49, TestEval("7^2"));
	ASSERT_EQ(256, TestEval("2^x", { { "x", 8.f } }));
}

TEST(Operators, CanUseEq)
{
	ASSERT_EQ(0, TestEval("7==2"));
	ASSERT_EQ(1, TestEval("2==x", { { "x", 2.f } }));
}

TEST(Operators, CanUseNotEq)
{
	ASSERT_EQ(1, TestEval("7!=2"));
	ASSERT_EQ(0, TestEval("2!=x", { { "x", 2.f } }));
}

TEST(Operators, CanUseLess)
{
	ASSERT_EQ(1, TestEval("5<7"));
	ASSERT_EQ(0, TestEval("x<10", { { "x", 20.f } }));
}

TEST(Operators, CanUseLessEq)
{
	ASSERT_EQ(1, TestEval("5<=7"));
	ASSERT_EQ(1, TestEval("x<=10", { { "x", 10.f } }));
}

TEST(Operators, CanUseGreater)
{
	ASSERT_EQ(0, TestEval("5>7"));
	ASSERT_EQ(1, TestEval("x>10", { { "x", 20.f } }));
}

TEST(Operators, CanUseGreaterEq)
{
	ASSERT_EQ(1, TestEval("7>=7"));
	ASSERT_EQ(0, TestEval("x>=10", { { "x", 2.f } }));
}

TEST(Operators, CanUseNegate)
{
	ASSERT_EQ(-7, TestEval("-7"));
	ASSERT_EQ(-0.5f, TestEval("-x", { { "x", 0.5f } }));
}

TEST(Operators, CanUseNot)
{
	ASSERT_EQ(0, TestEval("!7"));
	ASSERT_EQ(1, TestEval("!x", { { "x", 0.f } }));
}

TEST(Operators, CanUseAnd)
{
	ASSERT_EQ(1, TestEval("7 && 8"));
	ASSERT_EQ(0, TestEval("7 && 0"));
	ASSERT_EQ(0, TestEval("x && 3", { { "x", 0.f } }));
	ASSERT_EQ(1, TestEval("x && 3", { { "x", 1.f } }));
}

TEST(Operators, CanUseOr)
{
	ASSERT_EQ(1, TestEval("7 || 8"));
	ASSERT_EQ(1, TestEval("0 || 2"));
	ASSERT_EQ(1, TestEval("x || 3", { { "x", 0.f } }));
	ASSERT_EQ(1, TestEval("x || 3", { { "x", 1.f } }));
}

TEST(Operators, Precedence)
{
	ASSERT_EQ(7.f, TestEval("1 + 2 * 3"));
	ASSERT_EQ(5.f, TestEval("1 * 2 + 3"));
	ASSERT_EQ(9.f, TestEval("(1 + 2) * 3"));
	ASSERT_EQ(3.f, TestEval("1 ^ 2 * 3"));
	ASSERT_EQ(1.f, TestEval("1 ^ 2 ^ 3"));
	ASSERT_EQ(3.f, TestEval("1 - -2"));

	ASSERT_EQ(-4, TestEval("1-2-3"));
	ASSERT_EQ(2, TestEval("1-(2-3)"));
	ASSERT_EQ(-5, TestEval("1-2*3"));
	ASSERT_EQ(-1, TestEval("1*2-3"));

	ASSERT_EQ(1, TestEval("1+1==2"));
	ASSERT_EQ(1, TestEval("-1+2"));
	ASSERT_EQ(3, TestEval("1--2"));

	ASSERT_EQ(1, TestEval("0||2==2"));
}

namespace
{
	float test1(float x)
	{
		return x + 1;
	}
}

TEST(Operators, CanUseConditionals)
{
	ASSERT_EQ(4.f, TestEval("t(x) ? 1+x+2 : 4+x", { {"x", 1} }, { Function("t", &test1) }));
	ASSERT_EQ(5.f, TestEval("t(x)-2 ? 1+x+2 : 4+x", { {"x", 1} }, { Function("t", &test1) }));
}
