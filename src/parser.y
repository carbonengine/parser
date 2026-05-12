// Copyright © 2022 CCP ehf.

% include{
    #include "src/parserstate.h"
}

%token_type{ Token }
%default_type {Node*}
%extra_argument {ParserState* parser}
%default_destructor { delete $$; }

%nonassoc OP_QUESTION.
%left OP_OR.
%left OP_AND.
%nonassoc OP_EQ OP_NE OP_GT OP_GE OP_LT OP_LE.
%left OP_PLUS OP_MINUS.
%left OP_TIMES OP_DIVIDE OP_MOD.
%left OP_EXP.
%left OP_NOT.


%syntax_error
{
    parser->result = { ParseResult::SYNTAX_ERROR };
}

program ::= expr(A).   
{ 
    parser->root.reset(A);
}

expr(A) ::= OP_FLOAT_CONST(B). 
{
    A = new Node(Node::FLOAT_CONST, B);
}

expr(A) ::= OP_ID(B). 
{
    A = parser->Variable(B);
}

expr(A) ::= OP_LEFT_PAR expr(B) OP_RIGHT_PAR.
{
    A = B;
}

expr(A) ::= OP_MINUS(O) expr(B).
{
    A = parser->UnaryOp(O, B);
}

expr(A) ::= OP_NOT(O) expr(B).
{
    A = parser->UnaryOp(O, B);
}

expr(A) ::= OP_PLUS expr(B).
{
    A = B;
}

expr(A) ::= expr(L) OP_AND(O) expr(R).
{
    A = parser->BinaryOp(L, O, R);
}

expr(A) ::= expr(L) OP_OR(O) expr(R).
{
    A = parser->BinaryOp(L, O, R);
}

expr(A) ::= expr(L) OP_EQ(O) expr(R).
{
    A = parser->BinaryOp(L, O, R);
}

expr(A) ::= expr(L) OP_NE(O) expr(R).
{
    A = parser->BinaryOp(L, O, R);
}

expr(A) ::= expr(L) OP_GT(O) expr(R).
{
    A = parser->BinaryOp(L, O, R);
}

expr(A) ::= expr(L) OP_GE(O) expr(R).
{
    A = parser->BinaryOp(L, O, R);
}

expr(A) ::= expr(L) OP_LT(O) expr(R).
{
    A = parser->BinaryOp(L, O, R);
}

expr(A) ::= expr(L) OP_LE(O) expr(R).
{
    A = parser->BinaryOp(L, O, R);
}




expr(A) ::= expr(L) OP_PLUS(O) expr(R).
{
    A = parser->BinaryOp(L, O, R);
}

expr(A) ::= expr(L) OP_MINUS(O) expr(R).
{
    A = parser->BinaryOp(L, O, R);
}

expr(A) ::= expr(L) OP_TIMES(O) expr(R).
{
    A = parser->BinaryOp(L, O, R);
}

expr(A) ::= expr(L) OP_DIVIDE(O) expr(R).
{
    A = parser->BinaryOp(L, O, R);
}

expr(A) ::= expr(L) OP_MOD(O) expr(R).
{
    A = parser->BinaryOp(L, O, R);
}

expr(A) ::= expr(L) OP_EXP(O) expr(R).
{
    A = parser->BinaryOp(L, O, R);
}

expr(A) ::= expr(C) OP_QUESTION(O) expr(T) OP_COLON expr(F).
{
    A = parser->Conditional(C, O, T, F);
}

arguments(A) ::= expr(B).
{
    A = new Node(Node::FUNCTION_CALL, Token());
    A->children[0].reset(B);
    A->offset = 1;
}

arguments(A) ::= arguments(C) OP_COMA expr(B).
{
    if (C->offset < MAX_FUNCTION_ARGUMENTS)
    {
        C->children[C->offset].reset(B);
    }
    else
    {
        delete B;
    }
    A = C;
    ++A->offset;
}


expr(A) ::= OP_ID(N) OP_LEFT_PAR arguments(B) OP_RIGHT_PAR.
{
    A = parser->FunctionCall(N, B, parser);
}

expr(A) ::= OP_ID(N) OP_LEFT_PAR OP_RIGHT_PAR.
{
    A = parser->FunctionCall(N, nullptr, parser);
}

expr(A) ::= OP_ID(N) OP_LEFT_PAR OP_STRING_CONST(B) OP_COMA arguments(C) OP_RIGHT_PAR.
{
    if (C->offset + 1 < MAX_FUNCTION_ARGUMENTS)
    {
        for (size_t i = C->offset; i > 0; --i)
        {
            std::swap(C->children[i], C->children[i - 1]);
        }
        C->children[0].reset(new Node(Node::STRING_CONST, B));
    }
    ++C->offset;

    A = parser->FunctionCall(N, C, parser);
}

expr(A) ::= OP_ID(N) OP_LEFT_PAR OP_STRING_CONST(S) OP_RIGHT_PAR.
{
    auto str = new Node(Node::STRING_CONST, S);

    A = new Node(Node::FUNCTION_CALL, Token());
    A->children[0].reset(str);
    A->offset = 1;

    A = parser->FunctionCall(N, A, parser);
}
