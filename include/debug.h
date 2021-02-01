#ifndef LISP_DEBUG_H
#define LISP_DEBUG_H

#include "atom.h"
#include "format.h"
#include "token.h"

#include <utility>

inline static std::string toString(Type type) {
	switch(type) {
	case Type::Nil: return "Nil";
	case Type::Pair: return "Pair";
	case Type::Symbol: return "Symbol";
	case Type::Integer: return "Integer";
	case Type::Rational: return "Rational";
	case Type::Builtin: return "Builtin";
	case Type::Closure: return "Closure";
	default: return "Unknown Type";
	}
}

inline std::string toString(TokenType type) {
	switch(type) {
	case TokenType::LEFT_PAREN: return "(";
	case TokenType::RIGHT_PAREN: return ")";
	case TokenType::LEFT_BRACKET: return "[";
	case TokenType::RIGHT_BRACKET: return "]";
	case TokenType::LEFT_BRACE: return "{";
	case TokenType::RIGHT_BRACE: return "}";
	case TokenType::IDK: return "~@";
	case TokenType::CARET: return "^";
	case TokenType::AT: return "@";
	case TokenType::GRAVE: return "`";
	case TokenType::TILDE: return "~";
	case TokenType::QUOTE: return "'";
	case TokenType::DOT: return ".";
	case TokenType::STRING: return "string";
	case TokenType::IDENTIFIER: return "identifier";
	case TokenType::NUMBER: return "number";
	}
}

/**
 * A program error occurs during the lexical analysis (Tokenizer) or during the parsing
 */
class ProgramError: std::exception {
	std::string m_msg;
    Token m_token;
  public:
	explicit ProgramError(const Token& token, std::string msg):
		m_msg(std::move(msg)), m_token(token) {
	}
	Token& token(){ return m_token; }
	std::string what() { return m_msg; }
};

/**
 * A lex error occurs during lexical analysis (Tokenizer), eg. unknown character
 */
class LexError: public ProgramError {
  public:
	explicit LexError(const Token& token, const std::string& msg):
		ProgramError(token, msg) {
	}
};

/**
 *
 */
class SyntaxError: public ProgramError {
  public:
	explicit SyntaxError(const Token& token, const std::string& msg):
		ProgramError(token, msg) {
	}
};

/**
 * A runtime error occurs during the evaluation of the Atom and the usage of the Environment
 */
class RunTimeError: std::exception {
	Atom        m_atom;
	std::string m_msg;

  public:
	RunTimeError(const Atom& atom, std::string msg):
		m_atom(atom), m_msg(std::move(msg)) {
	}
	[[nodiscard]] Atom atom() const { return m_atom; }
	virtual std::string        what() { return m_msg; }
};

/**
 * A type error occurs when an expected type is not the same as the actual type
 */
class TypeError: public RunTimeError {
  public:
    TypeError(const Atom& atom, const std::string& msg):
        RunTimeError(atom, msg) {
    }
    std::string what() override {
		return "Type Error:" + RunTimeError::what();
	}
};

class Environment;

/**
 * An environment error occurs when a symbol that has not been defined is attempted to be evaluated
 */
class EnvError: public RunTimeError {
	Environment& m_env;

  public:
	EnvError(Environment& env, const Atom& atom, const std::string& msg):
		m_env(env), RunTimeError(atom, msg) {
	}
	[[nodiscard]] Environment& environment() { return m_env; }
};

/**
 * An evaluation error occurs when types are mismatched or an argument list is shorter/longer than expected
 */
class EvalError: public RunTimeError {
  public:
	EvalError(const Atom& errExpr, const std::string& msg):
		RunTimeError(errExpr, msg) {}
};

/**
 * Report am error
 * @param err
 */
inline void staticError(ProgramError& err) {
	// format:
	/*
       filename, line:col error: msg
       line | $$$$
            | ^~~~
    */
	// where ^~~~~ is the length of the token
	std::stringstream str;
	Token& t = err.token();
	str << "REPL, " << t.line << ":" << t.column << err.what() << "\n";
	str << t.line << " |" << t.value << "\n"; // < show entire line & highlight token
	str << " |" << "^~~~~~";

}

/**
 *
 * @param err
 */
inline void dynamicError(EvalError& err) {
	std::cerr << err.what() << "\n";
}

#define CHECK(x) std::cout << #x << ": " << x << std::endl

#endif //LISP_DEBUG_H
