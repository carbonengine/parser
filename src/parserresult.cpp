// Copyright © 2022 CCP ehf.

#include "pch.h"
#include "../include/parserresult.h"

namespace CcpParser
{

ParseResult::operator bool() const
{
	return type == OK;
}

std::string ToString( const ParseResult& result, const char* expression )
{
	std::stringstream os;
	auto PutLocation = [&]() {
		if( expression && result.name.begin )
		{
			if( result.name.begin >= expression && result.name.begin < expression + strlen( expression ) )
			{
				os << " at location " << ( result.name.begin - expression );
			}
		}
	};
	switch( result.type )
	{
	case ParseResult::OK:
		return "OK";
	case ParseResult::SCANNER_ERROR:
		os << "Unexpected character \'" << ToString( result.name ) << "\'";
		PutLocation();
		break;
	case ParseResult::SYNTAX_ERROR:
		os << "Syntax error";
		PutLocation();
		break;
	case ParseResult::UNDEFINED_VARIABLE:
		os << "Undefined variable/constant \'" << ToString( result.name ) << "\'";
		PutLocation();
		break;
	case ParseResult::UNDEFINED_FUNCTION:
		os << "Undefined function \'" << ToString( result.name ) << "\'";
		PutLocation();
		break;
	default:
		os << "Internal error";
		PutLocation();
		break;
	}
	return os.str();
}

}