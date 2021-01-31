#include "atom.h"
#include "debug.h"
#include "tokenizer.h"

#include <gtest/gtest.h>
#include <queue>

std::queue<Token> createQueue(const std::vector<Token>& tokens) {
	std::queue<Token> queue;
	for(auto& elem: tokens) {
		queue.push(elem);
	}
	return queue;
}

bool operator!=(const Token& lhs, const Token& rhs) {
	return (lhs.type != rhs.type ||
			lhs.value != rhs.value ||
			lhs.line != rhs.line ||
			lhs.column != rhs.column);
}

bool operator==(std::queue<Token> lhs, std::queue<Token> rhs) {
	while(!lhs.empty() && !rhs.empty()) {
		if(lhs.front() != rhs.front())
			return false;
		lhs.pop();
		rhs.pop();
	}
	if(!lhs.empty() || !rhs.empty())
		return false;
	return true;
}

// Test programs
const std::vector<std::string> programs = {
	/** Useless programs **/
	{"  \t 42"},
	{"$"}, // will cause an exception
	{"nil "},
	{" ( abc\n(dev . NIL))"},
	{"(  )"},
	{"\t42.00041"},
	{"(abc .   (def \n. NIL))"},
	{"(s \n\t  (t . u) v . (w . nil))"},
	{"(FoO BAr)"},
	/** Somewhat interesting programs **/
	{"(define foo 4)"}, // define a variable
	{"foo"},            // should evaluate to 4
	{"(define bar foo)"},
	{"bar"}, // should evaluate to 4
	{"(define bar (quote foo))"},
    {"bar"}, // should evaluate to foo
};

TEST(Atom, Construction) {
	try {
		Atom a;
		ASSERT_TRUE(a.isNil()); // Atoms should be nil when given an empty constructor
		a = Atom(long(42));
		ASSERT_EQ(*a.integer(), 42);
		a = Atom(42.0f);
		ASSERT_DOUBLE_EQ(*a.rational(), 42.0f);
		a = Atom("forty-two");
		ASSERT_EQ(*a.symbol(), "forty-two");
		a = Atom(Atom(long(42)),
				 Atom(Atom(42.0f), nil));

		ASSERT_EQ(*a.car().integer(), 42);
		ASSERT_DOUBLE_EQ(*a.cdr().car().rational(), 42.0f);
		ASSERT_TRUE(a.cdr().cdr().isNil());
	} catch(std::bad_variant_access& err) {
		FAIL(); // all access should be valid
	}
}

TEST(Atom, Changes) {
}
TEST(TokenizerTest, Program0) {
	std::queue<Token> tokens0;

	tokens0 = tokenizer(programs[0]);
	ASSERT_EQ(tokens0, createQueue({Token{.type   = TokenType::NUMBER,
										  .value  = "42",
										  .line   = 0,
										  .column = 6}}));
}
TEST(TokenizerTest, Program1) {
	try {
		auto tokens1 = tokenizer(programs[1]);
		FAIL(); // tokenizer should throw..
	} catch(LexError& err) {
		// ASSERT_EQ("", err.what());
	}
}

TEST(TokenizerTest, Program2) {
	auto tokens2 = tokenizer(programs[2]);
	ASSERT_EQ(tokens2, createQueue({{TokenType::IDENTIFIER, "nil", 0, 0}}));
}

TEST(TokenizerTest, Program3) {
	auto tokens3 = tokenizer(programs[3]);
	ASSERT_EQ(tokens3, createQueue({{TokenType::LEFT_PAREN, "(", 0, 0},
									{TokenType::IDENTIFIER, "abc", 0, 3},
									{TokenType::LEFT_PAREN, "(", 1, 0},
									{TokenType::IDENTIFIER, "dev", 1, 1},
									{TokenType::DOT, ".", 1, 5},
									{TokenType::IDENTIFIER, "NIL", 1, 7},
									{TokenType::RIGHT_PAREN, ")", 1, 10},
									{TokenType::RIGHT_PAREN, ")", 1, 11}}));
}
TEST(TokenizerTest, Program4) {
	auto tokens4 = tokenizer(programs[4]);
	ASSERT_EQ(tokens4, createQueue({{TokenType::LEFT_PAREN, "(", 0, 0},
									{TokenType::RIGHT_PAREN, ")", 0, 3}}));
}

TEST(TokenizerTest, Program5) {
	auto tokens5 = tokenizer(programs[5]);
	auto to      = createQueue({{TokenType::NUMBER, "42.00041", 0, 0}});

	ASSERT_EQ(tokens5, to);
}

TEST(Parser, AtomComparison) {
	//auto tokens = tokenizer(programs);
}

TEST(Environment, Test) {
}

TEST(Evaluation, SideEffects) {
}
