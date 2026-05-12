// Copyright © 2022 CCP ehf.

#pragma once

#include "stringview.h"

namespace CcpParser
{

struct ParseResult
{
	enum Type
	{
		OK,
		SCANNER_ERROR,
		SYNTAX_ERROR,
		UNDEFINED_VARIABLE,
		UNDEFINED_FUNCTION,

		INTERNAL_ERROR,
	};

	Type type = OK;
	StringView name = { nullptr, nullptr };

	operator bool() const;
};

std::string ToString( const ParseResult& result, const char* expression = nullptr );

}