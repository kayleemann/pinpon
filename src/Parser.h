#include <utility>

#ifndef PARSER_H
#define PARSER_H
#include "Tokenizer.h"

class SyntaxNode;

class Parser {
	Tokenizer *lexer;
	vector<Token> allTokens;
	int currentTokenIndex{};
public:
	explicit Parser(Tokenizer *t) : lexer(t) {}

	SyntaxNode *run();
	SyntaxNode *run_text();
	SyntaxNode *run_statement();
	SyntaxNode *run_return();
	SyntaxNode *run_import();
	SyntaxNode *run_if();
	SyntaxNode *run_assign();
	SyntaxNode *run_function();
	SyntaxNode *run_expression();
	SyntaxNode *run_args();
	SyntaxNode *run_expression_tail();

	bool accept(TokenType type, Token *out=nullptr);
	bool accept(const vector<TokenType>& types, const vector<Token *>& outs);
	bool accept(const vector<TokenType>& types, const vector<Token *>& outs,
				const vector<TokenType>& reject);
	bool expect(TokenType type);
	Token currentToken();
};

enum class NodeType {
	CALL, TERMINAL, ARGS, CALL_TAIL, TEXT, FUNC, PARAMS,
    RETURN, IF, ASSIGN, GET, SET, IMPORT, VARKWPARAM, KWARG,
    VARPARAM,
};
const string NodeTypeStrings[] = {
        "CALL", "TERMINAL", "ARGS", "CALL_TAIL", "TEXT", "FUNC", "PARAMS",
        "RETURN", "IF", "ASSIGN", "GET", "SET", "IMPORT", "VARKWPARAM", "KWARG",
        "VARPARAM",
};

class SyntaxNode {
public:
	NodeType type;
	Token content;
	vector<SyntaxNode*> children;

	explicit SyntaxNode(NodeType _type) : type(_type) {}
	explicit SyntaxNode(Token _content) : type(NodeType::TERMINAL), content(std::move(_content)) {}

	~SyntaxNode();

	string toString(int indent = 0);
};

#endif
