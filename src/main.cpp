#include "atom.h"
#include "builtin.h"
#include "environment.h"
#include "eval.h"
#include "parser.h"
#include "ringbuffer.h"
#include "tokenizer.h"

#include <algorithm>
#include <iostream>

void interpretFile(const std::string& fileName, Environment& env) {
	std::ifstream file(fileName, std::ios::binary);
	std::string   input;
	if(!file.is_open()) {
		std::cerr << format("Failed to open file {}.", fileName) << "\n";
	}
	try {
		file >> input;
		auto tokens = tokenizer(input);

		//            totalExpectedRightParen += balanced(tokens);
		// see if we need to add a new line
		//			if(totalExpectedRightParen) {
		//				unbalanced = true;
		//				unbalancedTokens.insert(unbalancedTokens.end(), tokens.begin(), tokens.end());
		//				continue;
		//			} else {
		//                unbalanced = false;
		//				unbalancedTokens.clear();
		//			}

		Atom root = expression(tokens);

		//std::cout << root << std::endl;
		Atom s = eval(root, env);
		std::cout << s << std::endl;
	} catch(EnvError& err) {
		std::cerr << err.what() << "\n";
	} catch(ProgramError& err) {
		std::cerr << err.what() << "\n";
		staticError(err);
	} catch(EvalError& err) {
		std::cerr << err.what() << "\n";
		dynamicError(err);
	} catch(TypeError& err) {
		std::cerr << err.what() << "\n";
		//dynamicError(err);
	}
}

void repl(const std::string& prompt, const std::string& continuePrompt, Environment& env) {
	std::string                 input;
	RingBuffer<std::string, 50> history;
	std::vector<Token>          unbalancedTokens;
	bool                        unbalanced              = false;
	size_t                      totalExpectedRightParen = 0;
	// see if the left parentheses are equal to right parentheses
	auto balanced = [](std::deque<Token>& tokens) {
		size_t expectedRightParen = 0;
		for(const auto& token: tokens) {
			if(token.type == TokenType::LEFT_PAREN)
				expectedRightParen++;
			else if(token.type == TokenType::RIGHT_PAREN)
				expectedRightParen--;
		}
		return expectedRightParen;
	};

	for(;;) {
		if(unbalanced)
			std::cout << continuePrompt;
		else
			std::cout << prompt;

		std::getline(std::cin, input); // read
		history.push_back(input);
		if(input == ":history") {
			const size_t size = history.size();
			for(int i = 0; i < size; i++) {
				std::cout << history[i] << "\n";
			}
		}

		if(input == ":q")
			break;
		// import user file
		if(input == ":load" || input == ":l") {
			//std::string input = readFile(fileName);
			//interpret(input, env);
		}
        if(input.size() == 0) continue;
		try {
			auto tokens = tokenizer(input);

			//            totalExpectedRightParen += balanced(tokens);
			// see if we need to add a new line
			//			if(totalExpectedRightParen) {
			//				unbalanced = true;
			//				unbalancedTokens.insert(unbalancedTokens.end(), tokens.begin(), tokens.end());
			//				continue;
			//			} else {
			//                unbalanced = false;
			//				unbalancedTokens.clear();
			//			}

			Atom root = expression(tokens);

			//std::cout << root << std::endl;
			Atom s = eval(root, env);
			std::cout << s << std::endl;
		} catch(EnvError& err) {
			std::cerr << err.what() << "\n";
		} catch(ProgramError& err) {
			std::cerr << err.what() << "\n";
			staticError(err);
		} catch(EvalError& err) {
			std::cerr << err.what() << "\n";
			dynamicError(err);
		} catch(TypeError& err) {
			std::cerr << err.what() << "\n";
			//dynamicError(err);
		}
	}
}

int main(int argv, char** argc) {
	Environment env(nil, {{"car", builtin::car},
						  {"cdr", builtin::cdr},
						  {"cons", builtin::cons},
						  {"+", builtin::add},
						  {"-", builtin::sub},
						  {"*", builtin::mul},
						  {"/", builtin::div},
						  {"=", builtin::eq},
						  {"<", builtin::less},
						  {"getchar", builtin::getchar},
						  {"putchar", builtin::putchar}});
	env.set(Atom("t"), Atom("t"));
	if(argv == 2) {
		interpretFile(argc[1], env);
	} else {
		repl(">> ", ".. ", env);
	}
}