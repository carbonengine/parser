// Copyright © 2022 CCP ehf.

#include "../include/parserresult.h"
#include "../include/externals.h"
#include "parser.h"

namespace CcpParser
{

#define OP_EOF 0
#define OP_ERROR 10002

struct Token
{
	int32_t type;
	StringView string;
	float value;
};

struct Node
{
	enum NodeType
	{
		FLOAT_CONST,
		STRING_CONST,
		VARIABLE,
		UNARY_OP,
		BINARY_OP,
		CONDITIONAL,
		FUNCTION_CALL,
	};

	Node() = default;
	Node( NodeType type, const Token& token );
	~Node();

	NodeType nodeType;
	Token token;
	uint32_t offset = 0;
	union
	{
		const Variable* variable;
		const Function* function;
	};

	std::array<std::unique_ptr<Node>, MAX_FUNCTION_ARGUMENTS> children;
};

struct ParserState
{
	std::unique_ptr<Node> root;
	ParseResult result;

	const Externals* externals;

	Node* BinaryOp( Node* L, const Token& O, Node* R );
	Node* UnaryOp( const Token& O, Node* R );
	Node* Conditional( Node* C, const Token& O, Node* T, Node* F );
	Node* FunctionCall( const Token& name, Node* arguments, ParserState* parser );
	Node* Variable( const Token& name );
};
}