#ifndef LISP_PARSER_H
#define LISP_PARSER_H

#include "debug.h"
#include "format.h"

#include <queue>

Atom expression(std::queue<Token>& tokens);

/**
 * Parse a list
 * @param tokens incoming tokens from expression()
 * @return an atom containing a list
 */
Atom list(std::queue<Token>& tokens) {
	Atom result;
	Atom p;
	// expectation assertion
	auto expect = [&](TokenType type) {
		if(tokens.front().type != type)
			throw SyntaxError(tokens.front(), format("Expected {}", toString(type)));
	};

	expect(TokenType::LEFT_PAREN);
	tokens.pop();

	if(tokens.back().type != TokenType::RIGHT_PAREN) {
		expect(TokenType::RIGHT_PAREN);
	}
	while(!tokens.empty()) {
		Atom item;
		if(tokens.front().type == TokenType::RIGHT_PAREN) {
			tokens.pop();
			break;
		}
		if(tokens.front().type == TokenType::DOT) {
			tokens.pop();
			// improper list
			if(p.isNil())
				throw SyntaxError(tokens.front(), "Improper list");

			item    = expression(tokens);
			p.cdr() = item;
			expect(TokenType::RIGHT_PAREN);
			tokens.pop();
			break;
		}
		item = expression(tokens);
		if(p.isNil()) {
			result = Atom(item, nil);
			p      = result;
		} else {
			p.cdr() = Atom(item, nil);
			p       = p.cdr();
		}
	}
	return result;
}

/**
 * Parse simple data (numbers & identifiers)
 * @param tokens incoming tokens from expression()
 * @return an atom containing simple data
 */
Atom simple(std::queue<Token>& tokens) {
	Atom   atom;
	Token& token = tokens.front();
	switch(token.type) {
	case TokenType::NUMBER: {
		size_t read;

		long long value = std::stoll(token.value, &read);
		if(read != token.value.size()) {
			atom.value = std::stod(token.value);

			atom.type = Type::Rational;
		} else if(read == token.value.size()) {
			atom.value = value;
			atom.type  = Type::Integer;
		} else {
			// throw
		}
		break;
	}
	case TokenType::IDENTIFIER:
		if(token.value == "nil") {
			atom = nil;
		} else {
			atom.value = token.value;
			atom.type  = Type::Symbol;
		}
		break;
	case TokenType::STRING:
		break;
	}
	tokens.pop(); // ?
	return atom;
}

/**
 * Parse an expression
 * The parser creates a binary tree using the Atom class which ends with a NIL
 * @param tokens incoming tokens from the tokenizer
 * @return an atom containing an expression
 */
Atom expression(std::queue<Token>& tokens) {

	Token token = tokens.front();
	if(token.type == TokenType::LEFT_PAREN) {
		return list(tokens);
	} else if(token.type == TokenType::RIGHT_PAREN) {
		throw SyntaxError(token, "Expected List or Expression.");
	} else
		return simple(tokens);
}

#endif //LISP_PARSER_H
