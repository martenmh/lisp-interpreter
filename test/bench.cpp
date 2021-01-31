#include "tokenizer.h"

#include <benchmark/benchmark.h>
std::string input = "(+ 3 (3 5 2 (* 5 (- 6 2))))";

class Tokenizer {
	size_t                      line = 0, column = 0;
	std::string::const_iterator current;
	auto                        is_end() { return *current == '\0' || *current == EOF; };
	auto                        peek(size_t n = 1) { return (*current + 1); };
	auto                        advance() {
        current++;
        column++;
	};

	auto number() -> Token {
		auto   start     = current;
		size_t start_col = column;
		if(peek() == '.' && isdigit(peek(2))) {
			advance(); // consume '.'
			while(isdigit(peek()))
				advance();
		}
		return Token{TokenType::NUMBER, std::string(start, current), line, column};
	};
	auto identifier() -> Token {
		auto start = current;
		while(isalpha(peek()) || isdigit(peek()))
			advance();
		return Token{TokenType::IDENTIFIER, std::string(start, current), line, column};
	};

  public:
	std::queue<Token> tokenizer(const std::string& input) {
		std::queue<Token> tokens;
		current = input.begin();

		while(!is_end()) {
			if(isdigit(*current))
				tokens.push(number());
			if(isalpha(*current))
				tokens.push(identifier());

			switch(*current) {
			case ' ':
				break;
			case '\n':
				line++;
				column = 0;
				break;
			case '(':
				tokens.push(Token{TokenType::LEFT_PAREN, "(", line, column});
				break;
			case ')':
				tokens.push(Token{TokenType::RIGHT_PAREN, ")", line, column});
				break;
			default:
				break;
			}
			advance();
		}
		return tokens;
	}
};

static void BM_class_tokenizer(benchmark::State& state) {
	Tokenizer tokenizer;
	for(auto _: state) {
		tokenizer.tokenizer(input);
	}
}

static void BM_lambda_tokenizer(benchmark::State& state) {
	for(auto _: state) {
		tokenizer(input);
	}
}

BENCHMARK(BM_lambda_tokenizer);
BENCHMARK(BM_class_tokenizer);

BENCHMARK_MAIN();
