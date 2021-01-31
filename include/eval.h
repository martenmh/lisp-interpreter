#ifndef LISP_EVAL_H
#define LISP_EVAL_H

#include "atom.h"
#include "debug.h"

bool argumentCountIs(size_t n, const Atom& args){
	Atom a = args;
    for(int i = 0; i < n-1; i++){
        if(a.isNil()) return false;
		a = a.cdr();
	}
	if(a.isNil() || !a.cdr().isNil()) return false;
	return true;
}

Atom eval(Atom expr, Environment& env);

Atom quote(Atom& args){
    if(!argumentCountIs(1, args)){
        throw EvalError(args, "Expected 1 argument for operator 'quote'");
	}
    return args.car(); // return unevaluated
}

Atom define(const Atom& args, Environment& env){

    if(!argumentCountIs(2, args)){
        throw EvalError(args, "Expected 2 arguments for operator 'define'");
    }
    Atom symbol; // lvalue
    Atom value;  // rvalue
    symbol = args.car();
    if(symbol.type != Type::Symbol){
        throw TypeError(symbol, format("Expected type {} mismatched with actual type {}", toString(Type::Symbol), toString(symbol.type)));
    }
    value = eval(args.cdr().car(), env);

    env.set(symbol, value);
    return symbol;
}

Atom lambda(Environment& env, const Atom& args, Atom& expr){
	if(!argumentCountIs(2, args)){
        throw EvalError(args, "Expected 1 argument for operator 'lambda'");
	}
	return Atom(env, args.car(), args.cdr());
}

Atom applyClosure(Atom& fn, Atom args){

    auto closure_env = Environment(fn.car());
	std::cout << "Before:" << closure_env.atom() << std::endl;
    Atom param_names = fn.cdr().car();
    Atom body = fn.cdr().cdr();

    // bind args to param_names
    while(!param_names.isNil() || !args.isNil()){
        closure_env.set(param_names.car(), args.car());
        param_names = param_names.cdr();
        args = args.cdr();
    }
    std::cout << "After:" << closure_env.atom() << std::endl;
    if(!param_names.isNil() || !args.isNil()){
        // arguments & parameters are not the same amount!
    }

    Atom result;
    // evaluate the body
    while(!body.isNil()){
        result = eval(body.car(), closure_env);
        body = body.cdr();
    }
    return result;
}

Atom apply(Atom& fn, const Atom& args){

	if(fn.type == Type::Builtin){
		return (*fn.builtin())(args);
	} else if(fn.type == Type::Closure){
		return applyClosure(fn, args);
	} else {
        throw TypeError(args, format("Expected function, got {}", toString(fn.type)));
	}
}

/**
 * The interpretation and execution of the program using the root s-expression and an environment
 * Note: LISP evaluates everything unless it's quoted
 * @param expr
 * @param env
 * @return
 */
Atom eval(Atom expr, Environment& env){
    Atom op, args;

    if(expr.type == Type::Symbol){
        return env.get(expr);
    } else if (expr.type != Type::Pair){
        return expr;    // if there is no list, just return the expression
    }

    if(!expr.isProperList()){
        throw EvalError(expr, "Expected a proper list");
    }

    op = expr.car();   // get first element as operator
    args = expr.cdr(); // use remaining elements as operands

    if(op.type == Type::Symbol){
        std::string symbol = *op.symbol();
        for(auto& c : symbol) c = toupper(c);
        // Operands functionality:
        if(symbol == "QUOTE"){
            return quote(args);
        } else if(symbol == "DEFINE"){
			return define(args, env);
        } else if(symbol == "LAMBDA"){
			return lambda(env, args, expr);
		}
    }

    op = eval(op, env); // evaluate operator
	Atom p = args;
    // evaluate all arguments
	while(!p.isNil()){
        p.car() = eval(p.car(), env);
		p = p.cdr();
	}
	return apply(op, args);
}

#endif //LISP_EVAL_H
