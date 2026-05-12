// Copyright © 2022 CCP ehf.

#include "pch.h"
#include "parserstate.h"
#include "stdfunctions.h"

namespace CcpParser
{
#include "parser.c"

bool UnescapeString( const StringView& str, std::vector<uint8_t>& buffer );
}

namespace
{
bool EvaluateBinaryExpression( float& result, CcpParser::Node* left, int32_t tokenType, CcpParser::Node* right )
{
	switch( tokenType )
	{
	case OP_AND:
		result = left->token.value && right->token.value;
		return true;
	case OP_OR:
		result = left->token.value || right->token.value;
		return true;

	case OP_EQ:
		result = left->token.value == right->token.value ? 1.f : 0.f;
		return true;
	case OP_NE:
		result = left->token.value != right->token.value ? 1.f : 0.f;
		return true;
	case OP_GE:
		result = left->token.value >= right->token.value ? 1.f : 0.f;
		return true;
	case OP_LE:
		result = left->token.value <= right->token.value ? 1.f : 0.f;
		return true;
	case OP_GT:
		result = left->token.value > right->token.value ? 1.f : 0.f;
		return true;
	case OP_LT:
		result = left->token.value < right->token.value ? 1.f : 0.f;
		return true;

	case OP_PLUS:
		result = left->token.value + right->token.value;
		return true;
	case OP_MINUS:
		result = left->token.value - right->token.value;
		return true;
	case OP_TIMES:
		result = left->token.value * right->token.value;
		return true;
	case OP_DIVIDE:
		result = left->token.value / right->token.value;
		return true;
	case OP_MOD:
		result = float( fmod( left->token.value, right->token.value ) );
		return true;

	case OP_EXP:
		result = float( pow( left->token.value, right->token.value ) );
		return true;
	default:
		return false;
	}
}

bool EvaluateUnaryExpression( float& result, int32_t tokenType, CcpParser::Node* right )
{
	switch( tokenType )
	{
	case OP_MINUS:
		result = -right->token.value;
		return true;
	case OP_NOT:
		result = right->token.value == 0 ? 1.f : 0.f;
		return true;
	default:
		return false;
	}
}

bool EvaluateFunctionCall( float& result, const CcpParser::Function* func, const CcpParser::Node* arguments )
{
	auto args = arguments ? arguments->offset : 0;
	uint32_t argOffset = 0;
	std::vector<uint8_t> strBuffer;
	const char* str = nullptr;
	if( HasFlag( func->flags, CcpParser::FunctionFlags::USES_STRING_ARG ) )
	{
		argOffset = 1;
		UnescapeString( arguments->children[0]->token.string, strBuffer );
		strBuffer.push_back( 0 );
		str = reinterpret_cast<const char*>( strBuffer.data() );
	}

	result = 0;
	switch( args - argOffset )
	{
	case 0:
		result = func->Call( nullptr, str );
		break;
	case 1:
		result = func->Call( nullptr, str, arguments->children[argOffset]->token.value );
		break;
	case 2:
		result = func->Call( nullptr, str, arguments->children[argOffset]->token.value, arguments->children[argOffset + 1]->token.value );
		break;
	case 3:
		result = func->Call(
			nullptr,
			str,
			arguments->children[argOffset]->token.value,
			arguments->children[argOffset + 1]->token.value,
			arguments->children[argOffset + 2]->token.value );
		break;
	case 4:
		result = func->Call(
			nullptr,
			str,
			arguments->children[argOffset]->token.value,
			arguments->children[argOffset + 1]->token.value,
			arguments->children[argOffset + 2]->token.value,
			arguments->children[argOffset + 3]->token.value );
		break;
	case 5:
		result = func->Call(
			nullptr,
			str,
			arguments->children[argOffset]->token.value,
			arguments->children[argOffset + 1]->token.value,
			arguments->children[argOffset + 2]->token.value,
			arguments->children[argOffset + 3]->token.value,
			arguments->children[argOffset + 4]->token.value );
		break;
	case 6:
		result = func->Call(
			nullptr,
			str,
			arguments->children[argOffset]->token.value,
			arguments->children[argOffset + 1]->token.value,
			arguments->children[argOffset + 2]->token.value,
			arguments->children[argOffset + 3]->token.value,
			arguments->children[argOffset + 4]->token.value,
			arguments->children[argOffset + 5]->token.value );
		break;
	case 7:
		result = func->Call(
			nullptr,
			str,
			arguments->children[argOffset]->token.value,
			arguments->children[argOffset + 1]->token.value,
			arguments->children[argOffset + 2]->token.value,
			arguments->children[argOffset + 3]->token.value,
			arguments->children[argOffset + 4]->token.value,
			arguments->children[argOffset + 5]->token.value,
			arguments->children[argOffset + 6]->token.value );
		break;
	case 8:
		result = func->Call(
			nullptr,
			str,
			arguments->children[argOffset]->token.value,
			arguments->children[argOffset + 1]->token.value,
			arguments->children[argOffset + 2]->token.value,
			arguments->children[argOffset + 3]->token.value,
			arguments->children[argOffset + 4]->token.value,
			arguments->children[argOffset + 5]->token.value,
			arguments->children[argOffset + 6]->token.value,
			arguments->children[argOffset + 7]->token.value );
		break;
	default:
		static_assert( 8 == CcpParser::MAX_FUNCTION_ARGUMENTS, "need to handle extra function argument count" );
		return false;
	}
	return true;
}

const CcpParser::Function* FindFunction( const CcpParser::View<CcpParser::Function>& functions, const CcpParser::StringView& name, const CcpParser::Node* arguments )
{
	auto args = arguments ? arguments->offset : 0;
	for( auto& func : functions )
	{
		if( HasFlag( func.flags, CcpParser::FunctionFlags::USES_STRING_ARG ) )
		{
			if( func.arity + 1 == args && func.name == name && arguments && arguments->children[0]->nodeType == CcpParser::Node::STRING_CONST )
			{
				return &func;
			}
		}
		else if( func.arity == args && func.name == name )
		{
			return &func;
		}
	}
	return nullptr;
}

bool AreArgumentsConstants( const CcpParser::Node* arguments )
{
	if( !arguments )
	{
		return true;
	}
	for( auto& arg : arguments->children )
	{
		if( !arg )
		{
			return true;
		}
		if( arg->nodeType != CcpParser::Node::FLOAT_CONST && arg->nodeType != CcpParser::Node::STRING_CONST )
		{
			return false;
		}
	}
	return true;
}
}

namespace CcpParser
{

Node::Node( NodeType type, const Token& t ) :
	nodeType( type ),
	token( t )
{
	function = nullptr;
}

Node::~Node()
{
}



Node* ParserState::BinaryOp( Node* L, const Token& O, Node* R )
{
	if( L->nodeType == Node::FLOAT_CONST && R->nodeType == Node::FLOAT_CONST && EvaluateBinaryExpression( L->token.value, L, O.type, R ) )
	{
		delete R;
		return L;
	}
	else
	{
		auto A = new Node( Node::BINARY_OP, O );
		A->children[0].reset( L );
		A->children[1].reset( R );
		return A;
	}
}

Node* ParserState::UnaryOp( const Token& O, Node* R )
{
	if( R->nodeType == Node::FLOAT_CONST && EvaluateUnaryExpression( R->token.value, O.type, R ) )
	{
		return R;
	}
	else
	{
		auto A = new Node( Node::UNARY_OP, O );
		A->children[0].reset( R );
		return A;
	}
}

Node* ParserState::Conditional( Node* C, const Token& O, Node* T, Node* F )
{
	if( C->nodeType == Node::FLOAT_CONST )
	{
		if( C->token.value != 0 )
		{
			delete C;
			delete F;
			return T;
		}
		else
		{
			delete C;
			delete T;
			return F;
		}
	}
	auto A = new Node( Node::CONDITIONAL, O );
	A->children[0].reset( C );
	A->children[1].reset( T );
	A->children[2].reset( F );
	return A;
}

Node* ParserState::FunctionCall( const Token& name, Node* arguments, ParserState* parser )
{

	const Function* func = nullptr;
	for( auto& each : parser->externals->functions )
	{
		func = FindFunction( each, name.string, arguments );
		if( func )
		{
			break;
		}
	}
	if( !func )
	{
		func = FindFunction( g_stdFunctions, name.string, arguments );
	}
	Node* node;
	if( func && HasFlag( func->flags, FunctionFlags::PURE_FUNC ) && !HasFlag( func->flags, FunctionFlags::USES_CONTEXT ) && AreArgumentsConstants( arguments ) )
	{
		float result = 0;
		if( !EvaluateFunctionCall( result, func, arguments ) )
		{
			assert( false );
			parser->result = { ParseResult::INTERNAL_ERROR, name.string };
		}
		if( arguments )
		{
			node = arguments;
			for( auto& arg : arguments->children )
			{
				arg.reset();
			}
			node->nodeType = Node::FLOAT_CONST;
			node->token.value = result;
		}
		else
		{
			node = new Node( Node::FLOAT_CONST, Token() );
			node->token.value = result;
		}
	}
	else
	{
		node = arguments ? arguments : new Node( Node::FUNCTION_CALL, Token() );
		node->token = name;
		node->function = func;
		if( !func )
		{
			parser->result = { ParseResult::UNDEFINED_FUNCTION, name.string };
		}
	}
	return node;
}

Node* ParserState::Variable( const Token& name )
{
	for( auto& vars : externals->variables )
	{
		for( auto& var : vars )
		{
			if( var.name == name.string )
			{
				auto A = new Node( Node::VARIABLE, name );
				A->variable = &var;
				return A;
			}
		}
	}
	auto A = new Node( Node::FLOAT_CONST, name );
	for( auto& vars : externals->constants )
	{
		for( auto& var : vars )
		{
			if( var.name == name.string )
			{
				A->token.value = var.value;
				return A;
			}
		}
	}
	for( auto& var : g_stdConstants )
	{
		if( var.name == name.string )
		{
			A->token.value = var.value;
			return A;
		}
	}

	if( externals->variableFactory.findVariable )
	{
		if ( auto var = ( *externals->variableFactory.findVariable )( externals->variableFactory.context, name.string ) )
		{
			A->nodeType = Node::VARIABLE;
			A->variable = var;
			return A;
		}
	}
	result = { ParseResult::UNDEFINED_VARIABLE, name.string };
	return A;
}
}
