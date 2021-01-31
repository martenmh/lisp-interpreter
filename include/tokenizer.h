#ifndef LISP_TOKENIZER_H
#define LISP_TOKENIZER_H

#include "debug.h"
#include "format.h"
#include "token.h"

#include <iostream>
#include <queue>
#include <set>
#include <utility>

std::queue<Token> tokenizer(const std::string& input) {
	std::queue<Token> tokens;
	auto              current = input.begin();
	size_t            line = 0, column = 0;

	/** Helper functions **/
	auto is_end = [&]() { return *current == '\0' || *current == EOF; };
	// peek n places forward
	auto peek = [&](size_t n = 1) { return *(current + n); };

	auto newline = [&]() { line++; column = 0; };
	// advance a character & column || line
	auto advance = [&]() {
		if(*current == '\n')
			newline();
		else
			column++;
		current++;
	};

	auto add_token = [&](TokenType type, const std::string& str) {
		tokens.emplace(type, str, line, column);
	};

	auto number = [&]() -> Token {
		auto start = current;
		while(isdigit(peek()))
			advance();
		if(peek() == '.' && isdigit(peek(2))) {
			advance(); // consume '.'
			while(isdigit(peek()))
				advance();
		}
		// todo: check if necessary
		//if(start == current)
		advance();
		return Token{TokenType::NUMBER, std::string(start, current + 1), line, column};
	};

	// Whitelist all accepted identifiers
	std::set<char> acceptedIdentifiers = {'?', '+', '-', '*', '/'};
	auto           is_identifier       = [&](char c) -> bool {
        return (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z') ||
               acceptedIdentifiers.find(c) != acceptedIdentifiers.end();
	};

	auto identifier = [&]() -> Token {
		auto start = current;
		while(is_identifier(peek()) || isdigit(peek()))
			advance();
		return Token{TokenType::IDENTIFIER, std::string(start, current + 1), line, column};
	};

	while(!is_end()) {
		/** Tokenize multi character tokens **/
		if(isdigit(*current))
			tokens.push(number());
		if(is_identifier(*current))
			tokens.push(identifier());

		// Tokenize all single character tokens
		switch(*current) {
		// !is_end()
		case '\0': [[fallthrough]];
		case EOF: continue; // go to while start
		case ' ': [[fallthrough]];
		case '\t':
			break;
		case '\n':
			newline();
			break;
		case '(':
			add_token(TokenType::LEFT_PAREN, "(");
			break;
		case ')':
			add_token(TokenType::RIGHT_PAREN, ")");
			break;
		case '.':
			add_token(TokenType::DOT, ".");
			break;
		default:
			// just create identifiers of all remaining characters
			//add_token(TokenType::IDENTIFIER, std::string(*current, 1));
			break;
			//throw LexError(format("Unknown token {}", *current));
		}
		advance();
	}
	return tokens;
}

#endif //LISP_TOKENIZER_H
