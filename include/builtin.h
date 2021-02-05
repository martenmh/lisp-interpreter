#ifndef LISP_BUILTIN_H
#define LISP_BUILTIN_H

#include "debug.h"
#include "atom.h"
#include "eval.h"

namespace builtin {
    Atom car(Atom args) {
        if(!argumentCountIs(1, args)) {
            throw EvalError(args, "Expected 1 argument for function 'car'");
        }
        if(args.car().isNil())
            return nil;
        else if(args.car().type != Type::Pair) {
            throw TypeError(args, format("invalid argument type '{}' mismatched with expected type '{}' to built-in function 'car'", toString(Type::Pair), toString(args.car().type)));
        } else {
            return args.car().car();
        }
    }
    Atom cdr(Atom args) {
        if(!argumentCountIs(1, args)) {
            throw EvalError(args, "Expected 1 argument for function 'cdr'");
        }
        if(args.car().isNil())
            return nil;
        else if(args.car().type != Type::Pair) {
            throw TypeError(args, format("invalid argument type '{}' mismatched with expected type '{}' to built-in function 'cdr'", toString(Type::Pair), toString(args.car().type)));
        } else {
            return args.car().cdr();
        }
    }

    Atom cons(Atom args) {
        if(!argumentCountIs(2, args)) {
            throw EvalError(args, "Expected 2 arguments for function 'cons'");
        }
        return Atom(args.car(), args.cdr());
    }

/** Arithmetic **/
#define BINARY_ARITHMETIC(lhs, rhs, op)                              \
	if(lhs.type == Type::Rational && rhs.type == Type::Rational)     \
		return Atom(double(*lhs.rational() op * rhs.rational()));    \
	else if(lhs.type == Type::Integer && rhs.type == Type::Rational) \
		return Atom(double(*lhs.integer() op * rhs.rational()));     \
	else if(lhs.type == Type::Rational && rhs.type == Type::Integer) \
		return Atom(double(*lhs.rational() op * rhs.integer()));     \
	else                                                             \
		return Atom(long(*lhs.integer() op * rhs.integer()));

#define BINARY_OP(lhs, rhs, op)                              \
	if(lhs.type == Type::Rational && rhs.type == Type::Rational)     \
		return (*lhs.rational() op * rhs.rational());    \
	else if(lhs.type == Type::Integer && rhs.type == Type::Rational) \
		return (*lhs.integer() op * rhs.rational());     \
	else if(lhs.type == Type::Rational && rhs.type == Type::Integer) \
		return (*lhs.rational() op * rhs.integer());     \
	else                                                             \
		return (*lhs.integer() op * rhs.integer());

    bool isArithmetic(Atom& atom) {
        return atom.type == Type::Rational ||
               atom.type == Type::Integer;
    }

    Atom eq(Atom args) {
        if(!argumentCountIs(2, args)){
            throw EvalError(args, "Expected 2 arguments for binary '='");
        }

        Atom lhs = args.car();
        Atom rhs = args.cdr().car();
        if(!isArithmetic(lhs) || !isArithmetic(rhs))
            throw TypeError(args, format("invalid operands '{}' and '{}' to binary '='", toString(lhs.type), toString(rhs.type)));

		// Immediately invoked function
        return [&]() -> bool {
			BINARY_OP(lhs, rhs, ==)
		}() ? Atom("t") : nil;
    }

    Atom less(Atom args) {
        if(!argumentCountIs(2, args)){
            throw EvalError(args, "Expected 2 arguments for binary '='");
        }

        Atom lhs = args.car();
        Atom rhs = args.cdr().car();
        if(!isArithmetic(lhs) || !isArithmetic(rhs))
            throw TypeError(args, format("invalid operands '{}' and '{}' to binary '='", toString(lhs.type), toString(rhs.type)));

        // Immediately invoked function
        return [&]() -> bool {
          BINARY_OP(lhs, rhs, <)
        }() ? Atom("t") : nil;
    }

    Atom add(Atom args) {
        if(!argumentCountIs(2, args)) {
            throw EvalError(args, "Expected 2 arguments for binary '+'");
        }
        Atom lhs = args.car();
        Atom rhs = args.cdr().car();
        if(!isArithmetic(lhs) || !isArithmetic(rhs))
            throw TypeError(args, format("invalid operands '{}' and '{}' to binary '+'", toString(lhs.type), toString(rhs.type)));
        BINARY_ARITHMETIC(lhs, rhs, +)
    }
    Atom sub(Atom args) {
        if(!argumentCountIs(2, args)) {
            throw EvalError(args, "Expected 2 arguments for binary '-'");
        }
        Atom lhs = args.car();
        Atom rhs = args.cdr().car();
        if(!isArithmetic(lhs) || !isArithmetic(rhs))
            throw TypeError(args, format("invalid operands '{}' and '{}' to binary '-'", toString(lhs.type), toString(rhs.type)));
        BINARY_ARITHMETIC(lhs, rhs, -)
    }

    Atom mul(Atom args) {
        if(!argumentCountIs(2, args)) {
            throw EvalError(args, "Expected 2 arguments for binary '*'");
        }
        Atom lhs = args.car();
        Atom rhs = args.cdr().car();
        if(!isArithmetic(lhs) || !isArithmetic(rhs))
            throw TypeError(args, format("invalid operands '{}' and '{}' to binary '*'", toString(lhs.type), toString(rhs.type)));
        BINARY_ARITHMETIC(lhs, rhs, *)
    }
    Atom div(Atom args) {
        if(!argumentCountIs(2, args)) {
            throw EvalError(args, "Expected 2 arguments for binary '/'");
        }
        Atom lhs = args.car();
        Atom rhs = args.cdr().car();
        if(!isArithmetic(lhs) || !isArithmetic(rhs))
            throw TypeError(args, format("invalid operands '{}' and '{}' to binary '/'", toString(lhs.type), toString(rhs.type)));
        BINARY_ARITHMETIC(lhs, rhs, /)
    }
#undef BINARY_ARITHMETIC

	/** I/O **/
    Atom putchar(Atom args) {
        if(!argumentCountIs(1, args)) {
            throw EvalError(args, "Expected 1 argument for function 'putchar'");
        }
        putchar(args.car());
        //std::cout << args.car();
		return nil;
    }

    Atom getchar(Atom args) {
        char c = ::getchar();
		return Atom(std::string(1, c));
    }

//	Atom puts(Atom args) {
//        if(!argumentCountIs(1, args)) {
//            throw EvalError(args, "Expected 1 argument for function 'car'");
//        }
//        if(args.car().isNil())
//            return nil;
//        else if(args.car().type != Type::Pair) {
//            throw TypeError(args, format("invalid argument type '{}' mismatched with expected type '{}' to built-in function 'car'", toString(Type::Pair), toString(args.car().type)));
//        } else {
//            return args.car().car();
//        }
//    }
//
//    Atom gets(Atom args) {
//        if(!argumentCountIs(1, args)) {
//            throw EvalError(args, "Expected 1 argument for function 'car'");
//        }
//        if(args.car().isNil())
//            return nil;
//        else if(args.car().type != Type::Pair) {
//            throw TypeError(args, format("invalid argument type '{}' mismatched with expected type '{}' to built-in function 'car'", toString(Type::Pair), toString(args.car().type)));
//        } else {
//            return args.car().car();
//        }
//    }
} // namespace builtin

#endif //LISP_BUILTIN_H
