// Copyright © 2022 CCP ehf.

#include "pch.h"
#include "../include/ccpparser.h"
#include "helpers.h"

using namespace CcpParser;

namespace
{
struct WithLocale
{
	WithLocale( const char* locale )
	{
		setlocale( LC_ALL, locale );
	}
	~WithLocale()
	{
		//setlocale( LC_ALL, "C" );
	}
};
}

TEST(Basic, CannotParseEmptyInput)
{
	ASSERT_FALSE(CanParse(""));
	ASSERT_FALSE(CanParse(" "));
}

TEST(Basic, CanParseInt)
{
	ASSERT_TRUE(CanParse("123"));
	ASSERT_TRUE(CanParse("-123"));
	ASSERT_TRUE(CanParse("+123"));
}

TEST(Basic, CanParseFloats)
{
	ASSERT_TRUE(CanParse("123."));
	ASSERT_TRUE(CanParse("123.234"));
	ASSERT_TRUE(CanParse("123.234e-1"));
}

TEST( Basic, FloatParsingIsLocaleIndependent )
{
	{
		WithLocale l( "german" );
		ASSERT_EQ( 123.f, TestEval( "123." ) );
		ASSERT_EQ( 123.234f, TestEval( "123.234" ) );
		ASSERT_EQ( 123.234e-1f, TestEval( "123.234e-1" ) );
	}
	{
		WithLocale l( "" );
		ASSERT_EQ( 123.f, TestEval( "123." ) );
		ASSERT_EQ( 123.234f, TestEval( "123.234" ) );
		ASSERT_EQ( 123.234e-1f, TestEval( "123.234e-1" ) );
	}
}

TEST(Basic, CanParseSimpleOperations)
{
	ASSERT_TRUE(CanParse("1 + 2"));
}

TEST(Basic, HandlesSyntaxErrors)
{
	ASSERT_EQ(ParseResult::SCANNER_ERROR, CanParse("1..").type);
	ASSERT_EQ(ParseResult::SYNTAX_ERROR, CanParse("1 +").type);
}
