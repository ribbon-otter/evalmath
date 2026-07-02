//This file is licensed under MIT-0
//Copyright 2026 Ribbon-otter
//It is the implementation half of evalmath, a simple math string interpeter
#include <cassert>
#include <expected>
#include <string>
#include <string_view>
#include <optional>
#include <variant>
#include <vector>
#include <charconv>
#include <format>
#include <cmath>

#include "evalmath.h"

// uncomment to enable print statements and asserts helpful for development only
//#define EVALMATH_DEBUG 
#ifdef EVALMATH_DEBUG
    //only include <print> when debugging, since it is otherwise unneeded
    //and unsupported on some older versions of gcc which support std::expected
    #include <print>
    #define EVALMATH_PRINTLN(...) std::println( __VA_ARGS__)
    #define EVALMATH_PRINT(...) std::print(__VA_ARGS__)
    #define EVALMATH_ASSERT(...) assert( __VA_ARGS__)
    #define EVALMATH_PRINT_STACK(stack,tag) print_stack((stack), (tag))
#else
    #define EVALMATH_PRINTLN(...)
    #define EVALMATH_PRINT(...)
    #define EVALMATH_ASSERT(...)
    #define EVALMATH_PRINT_STACK(stack,tag)
#endif

using size_t = std::size_t;

namespace evalmath {
namespace details {
    bool static is_digit (char a) {
        return '0' <= a && a <= '9';
    }
    bool static is_whitespace (char a) {
        return ' ' == a;
    }
    bool static is_operator (char a) {
        return a == '+'
            || a == '*'
            || a == '-'
            || a == '/';
    }
    enum struct token_type {
        NUMBER,
        OPERATOR,
        RIGHT_PARENTHESES,
        LEFT_PARENTHESES,
        INVALID,
        END_OF_TEXT
    };
    enum struct token_error {
        NUM_SIZE,
        UNKNOWN
    };

#ifdef EVALMATH_DEBUG
    std::string_view static to_sv(const token_type& a) {
        switch (a) {
            case token_type::NUMBER: return "{num}";
            case token_type::OPERATOR: return "{op}";
            case token_type::RIGHT_PARENTHESES: return "{ ) }";
            case token_type::LEFT_PARENTHESES: return "{ ( }";
            case token_type::INVALID: return "{invalid}";
            case token_type::END_OF_TEXT: return "{EOT}";
            default: return "{unknown token type}";
        }
    }
#endif
    std::string_view static to_sv(const token_error& a) {
        switch (a) {
            case token_error::NUM_SIZE: return "a number is either too big or too small";
            case token_error::UNKNOWN:
            default: return "unknown syntax";
        }
    }
    
    struct token {
        token_type type;
        union {
            double value_num;
            char value_char; 
            token_error value_err;
        };
    };
   
    ///returns no result if the number doesn't fit in a double 
    std::optional<double> static parse_num(std::string_view num_str) {
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
#endif
        double value = 0;
        auto [ptr, ec] =
            std::from_chars(num_str.data(), num_str.data() + num_str.size(), value);
        EVALMATH_PRINTLN("\tparsing number '{}' ", num_str);
        //we shouldn't be passing invalid numbers ever 
        assert(ec != std::errc::invalid_argument); 
        assert(ptr == num_str.data() + num_str.size()); //and we use up the entire 
                                                        //token value
        //however we might parse a number that is too large/small for a double
        if (ec == std::errc::result_out_of_range) return {};
        
        assert(ec == std::errc()); //now there shouldn't be an error ever
        
        //silence unused value warnings on non-debug builds
        (void)ptr;
        (void)ec;
#ifdef __clang__
#pragma clang diagnostic pop
#endif
        return value;
    }
    
    ///returns a token if there are tokens left
    std::pair<std::optional<token>, size_t> static next_token(std::string_view text) {
        if (text.empty()) return {};
        auto all_start = text.begin(); //text including whitespace
        while (!text.empty() && is_whitespace(text.front())) text.remove_prefix(1);
        if (text.empty()) return {};
        
        auto token_start = text.begin(); //text after skipping whitespace
        char start_char = text.front();
        token_type type;
        text.remove_prefix(1);
        if (start_char == '.' && !text.empty() && is_digit(text.front())) {
            //we are a number starting with .
            type = token_type::NUMBER;
            while (!text.empty() && is_digit(text.front())) {
                text.remove_prefix(1);
            }
        } else if (is_digit(start_char) ) { 
            // negative numbers are treated as a positive number plus a uniary operator
            // parse digit
            type = token_type::NUMBER;
            while (!text.empty() && is_digit(text.front())) {
                text.remove_prefix(1);
            }
            if (!text.empty() && text.front() == '.') {
                text.remove_prefix(1);
            }
            while (!text.empty() && is_digit(text.front())) {
                text.remove_prefix(1);
            }
        } else if (is_operator(start_char)) { // parse operator
            type = token_type::OPERATOR;
        } else if ('(' == start_char) { // parse left parentheses
            type = token_type::LEFT_PARENTHESES;
        } else if (')' == start_char) { // parse right parentheses
            type = token_type::RIGHT_PARENTHESES;
        } else {
            type = token_type::INVALID;
            while (!text.empty() && !is_whitespace(text.front())) {
                text.remove_prefix(1);
            }
        }
        auto end = text.begin();//the end of where we stopped gobbling
        //how much the caller need to advance their head
        long len = end - all_start; 
        switch (type) {
            case token_type::NUMBER: {
                auto maybe_number = parse_num({token_start,end});
                if (maybe_number) {
                    return 
                        {token{.type = type, .value_num = *maybe_number}, len};
                    
                } else {
                    EVALMATH_PRINT("invalid number token found\n");
                    return {token{.type = token_type::INVALID, 
                           .value_err = token_error::NUM_SIZE}, len};
                }
            }
            case token_type::OPERATOR:
                return {token{.type = type, .value_char = start_char}, len};
            case token_type::LEFT_PARENTHESES:
                //we don't care out the value (we just suppress the warning)
                return {token{type, {0.0}}, len}; 
            case token_type::RIGHT_PARENTHESES:
                //we don't care about the value (we just suppress the warning)
                return {token{type, {0.0}}, len};
            case token_type::END_OF_TEXT:
            case token_type::INVALID: {
                return {token{.type = type, .value_err = token_error::UNKNOWN}, len};
            }
            default:
                assert(false); //should be unreachable
        }
        assert(false); //should be unreachable
    }
    
    int static op_precedence(char op) {
        if (op == '+' || op == '-') return 1;
        if (op == '*' || op == '/') return 2;
        assert(false);
        return 0;
    }

    int static op_precedence(token op) {
        assert(op.type == token_type::OPERATOR);
        return op_precedence(op.value_char);
    }

    double static eval_binary_op(double lhs, char op, double rhs) {
        if (op == '+') {
            return lhs + rhs;
        }
        if (op == '*') {
            return lhs * rhs;
        }
        if (op == '-') {
            return lhs - rhs;
        }
        if (op == '/') {
            return lhs / rhs;
        }
        assert(false); //we shouldn't produce tokens with ops
                       //that aren't handled here
        return 0;
    }
    
    bool static is_unary_op(char op) {
        //note that other parts of the code assumes all unary operators
        //are also binary operators
        return op == '+' || op == '-';
    }
    
    double static eval_unary_op(char op, double rhs) {
        if (op == '+') {
            return rhs;
        }
        if (op == '-') {
            return -rhs;
        }
        assert(false); //only '+' and '-' are uniry operators
        return 0;
    }

#ifdef EVALMATH_DEBUG
    void static print_stack(std::vector<token> &stack, std::string_view tag) {
        for (const auto& i : stack) {std::print("{},", to_sv(i.type));}
        std::println("<- {}", tag);
    }
#endif




    ///the reduce half of the shift reduce parser
    ///returns a value only if an error occurred
    std::optional<std::string> static
    reduce(std::vector<token> &stack) {
        EVALMATH_PRINTLN("----- Begin reduce operation");
        
        //iterations of the loop are written assuming that 
        //assuming other patterns don't length to the stack
        //and thus all patterns have an opportunity to match each 
        //additional token

        //thus all patterns much end with a "continue"
        // or a "return" (in the case of errors)
        
        while (!stack.empty()) {
            EVALMATH_PRINT_STACK(stack, "<- stack at start of matching");
            
            const auto& a = stack[stack.size() - 1];
            if (a.type == token_type::INVALID) {
                //If an error was found in tokenization, just print it now
                return std::string(to_sv(a.value_err));
            }
            if (stack.front().type == token_type::OPERATOR &&
                !is_unary_op(stack.front().value_char) //allow e.g. '+4'
                ) {
                return std::format("number missing before: '{}'", a.value_char);
            }
            if (stack.front().type == token_type::RIGHT_PARENTHESES) {
                return std::format("extra right parentheses at the beginning");
            }
            
            if (stack.size() < 2) break;
            const auto& b = stack[stack.size() - 2];
            if (a.type == token_type::NUMBER && b.type == token_type::NUMBER) {
                return std::format("missing operator between {} and {}",
                    b.value_num, a.value_num);
            }
            if (a.type == token_type::END_OF_TEXT && b.type == token_type::OPERATOR) {
                return std::format("formula ends in binary operator:", b.value_char);
            }
            if (a.type == token_type::OPERATOR
                && !is_unary_op(a.value_char) // 5 +-4 is not an error
                && b.type == token_type::OPERATOR) {
                    return std::format("missing operand between: {} and {}", b.value_char, a.value_char);
            }
            if (a.type == token_type::RIGHT_PARENTHESES
                && b.type == token_type::LEFT_PARENTHESES) {
                return std::format("parentheses pair missing contents");
            }

            if (a.type == token_type::LEFT_PARENTHESES
                && b.type == token_type::NUMBER) {
                return std::format(
                    "operator needed between {} and left parentheses", b.value_num);
            }
            //This is included for completeness
            //I am not sure it is possible for this error to be triggered.
            if (a.type == token_type::NUMBER
                && b.type == token_type::RIGHT_PARENTHESES) {
                return std::format(
                    "operator needed between right parentheses and {}", a.value_num);
            }
            // e.g. '<start>1)' is an error 
            if (stack.size() == 2
                && a.type == token_type::RIGHT_PARENTHESES
                && b.type != token_type::LEFT_PARENTHESES) {
                return std::format("missing left parentheses");
            }
            // reduce +2 to 2
            if (stack.size() == 2
                && a.type == token_type::NUMBER
                && b.type == token_type::OPERATOR
                && is_unary_op(b.value_char)
                ) {
                token result = {.type=token_type::NUMBER, 
                            .value_num=eval_unary_op(b.value_char, a.value_num) };
                stack.pop_back();
                stack.pop_back();
                stack.push_back(result);
                continue;
            }
            
            // oh look we are done :D
            if (stack.size() == 2
                && a.type == token_type::END_OF_TEXT 
                && b.type == token_type::NUMBER) {
                stack.pop_back(); //now the stack is just the number and we are done
                return {}; //no more reductions possible
            }
            if (a.type == token_type::END_OF_TEXT
                && b.type == token_type::LEFT_PARENTHESES) {
                return std::format("extra left parentheses on the end");
            }
            if (a.type == token_type::OPERATOR
                && !is_unary_op(a.value_char)
                && b.type == token_type::LEFT_PARENTHESES) {
                return std::format(
                    "need a value between left parentheses and {}", a.value_char);
            }
            if (a.type == token_type::RIGHT_PARENTHESES
                && b.type == token_type::OPERATOR) {
                return std::format(
                    "need a value between right parentheses and {}", b.value_char);
            }
            
            if (stack.size() < 3) break;
            const auto& c = stack[stack.size() - 3];
            // reduce (x) to x
            if (a.type == token_type::RIGHT_PARENTHESES 
                && b.type == token_type::NUMBER 
                && c.type == token_type::LEFT_PARENTHESES) {
                    token result = b;
                    stack.pop_back();
                    stack.pop_back();
                    stack.pop_back();
                    stack.push_back(result);
                continue;
            }
            if (a.type == token_type::END_OF_TEXT
                && c.type == token_type::LEFT_PARENTHESES) {
                return std::format("missing right parentheses");
            }
            //reduce e.g '<start>+6<end>' into '<start>6<end>'
            if (stack.size() == 3
                && a.type == token_type::END_OF_TEXT 
                && b.type == token_type::NUMBER 
                && c.type == token_type::OPERATOR
                && is_unary_op(c.value_char)) {
                token result = {.type=token_type::NUMBER, 
                            .value_num=eval_unary_op(c.value_char, b.value_num) };
                token end = a;
                stack.pop_back();
                stack.pop_back();
                stack.pop_back();
                stack.push_back(result);
                stack.push_back(end);
                continue;
            }
            //uniary operators are always higher precedence 
            //than binary operators, so we can evaluate them
            //as soon as we are sure they are unary operators
            // e.g. 1 * +3  => 1 * 3
            if (a.type == token_type::NUMBER
                && b.type == token_type::OPERATOR
                && is_unary_op(b.value_char)
                && (   c.type == token_type::OPERATOR 
                    || c.type == token_type::LEFT_PARENTHESES
                 )
                ) {
                token result = {.type=token_type::NUMBER, 
                        .value_num = eval_unary_op(b.value_char, a.value_num)
                    };
                stack.pop_back();
                stack.pop_back();
                stack.push_back(result);
                continue;
            }
            
            if (stack.size() < 4) break;
            const auto& d = stack[stack.size() - 4];
            //no risk that a higher priority operator can show up later
            //since we are at the end of the text
            //so we can start evaluating binary operations
            if (
                ((   a.type == token_type::END_OF_TEXT
                    || a.type == token_type::RIGHT_PARENTHESES
                    )
                && b.type == token_type::NUMBER
                && c.type == token_type::OPERATOR
                && d.type == token_type::NUMBER)
                
                ||

                (a.type == token_type::OPERATOR
                && b.type == token_type::NUMBER
                && c.type == token_type::OPERATOR
                && d.type == token_type::NUMBER
                && op_precedence(a) <= op_precedence(c))
                
                ) {
                token end = a; //remember the end so we can put it back
                double value_num = eval_binary_op(d.value_num,c.value_char,b.value_num); 
                
                EVALMATH_PRINTLN("evaluated {} {} {} as {}", 
                    d.value_num, c.value_char, b.value_num, value_num);
                stack.pop_back();
                stack.pop_back();
                stack.pop_back();
                stack.pop_back();
                stack.emplace_back(
                    token{token_type::NUMBER, {.value_num=value_num} });
                stack.emplace_back(end);
                continue; //reinitialize the a,b,c,d references 
                          //since they are officially invalid now
            } 
            EVALMATH_PRINT_STACK(stack, "stack at end of match");
            break;
        }

        EVALMATH_PRINTLN("----- reduce operation ended");
        EVALMATH_PRINT_STACK(stack, "final stack");
        return {};
    }
    
    ///replaces infinities and nan with errors
    std::expected<double, std::string> static filter_non_normal(double x) {
        switch (std::fpclassify(x))
        {
            case FP_INFINITE:
                if (x > 0) return std::unexpected("result is too large");
                else return std::unexpected("result is too large of a negative number");
            case FP_NAN:
                return std::unexpected("undefined calculation");
            case FP_NORMAL:
            case FP_SUBNORMAL:
            case FP_ZERO:
                return x;
            default:
                //this should never happen
                return std::unexpected("result is too strange in an unknown way");
        }
    }
}
using namespace details;
std::expected<double, std::string> eval(std::string_view formula) {
    //stack reduce parser
    std::vector<token> stack;
    while (true) {
        auto [maybe_token, len] = next_token(formula);
        if (!maybe_token) break;
        token this_token = *maybe_token;
        //advance the parse
        formula.remove_prefix(len);
        stack.push_back(this_token);
        if (auto maybe_error = reduce(stack)) return std::unexpected(*maybe_error);
    }
    //END_OF_TEXT tokens don't use value, but we supply a 0.0 suppress warnings
    stack.push_back(token{token_type::END_OF_TEXT, {.value_num=0.0} }); 
    if (auto maybe_error = reduce(stack)) return std::unexpected(*maybe_error);
    
    ///I should add a particular match to reduce to catch the invalid formula
    ///before these hit, but we shouldn't crash if it somehow happens in production
    EVALMATH_ASSERT(stack.size() == 1);
    EVALMATH_ASSERT(stack.front().type == token_type::NUMBER);
    if (stack.size() != 1 && stack.front().type == token_type::NUMBER) { 
        //one of our better error detecting rules didn't find
        //the specific error inside reduce
        return std::unexpected(std::format("invalid formula '{}'", formula));
    }
    EVALMATH_PRINTLN("eval returns -- {} evaluated as {}", 
        formula, stack.front().value_num);
    
    double result = stack.front().value_num;
    return filter_non_normal(result);
}

} //namespace

// vim: ts=4 sw=4 expandtab
