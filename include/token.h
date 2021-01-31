#ifndef LISP_TOKEN_H
#define LISP_TOKEN_H

enum class TokenType {
	LEFT_PAREN,
	RIGHT_PAREN, // ()
	LEFT_BRACKET,
	RIGHT_BRACKET, // []
	LEFT_BRACE,
	RIGHT_BRACE, // {}
	IDK,         // ~@
	CARET,       // ^
	AT,          // @
	GRAVE,       //`
	TILDE,       // ~
	QUOTE,       // '
	DOT,         // .
	STRING,
	IDENTIFIER,
	NUMBER
};

struct Token {
	TokenType   type;
	std::string value;
	size_t      line = 0, column = 0;
};

#endif //LISP_TOKEN_H
