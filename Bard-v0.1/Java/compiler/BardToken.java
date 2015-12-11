package compiler;

import alphavm.BardOp;

public class BardToken
{
  final static public int
    // general non-statement
    eof                              = 0,
    eol                              = 1,

    // structure
    symbol_class                     = 2,
    symbol_function                  = 3,
    symbol_else                      = 4,
    symbol_else_eol                  = 5,
    symbol_elseIf                    = 6,
    symbol_endClass                  = 7,
    symbol_endForEach                = 8,
    symbol_endFunction               = 9,
    symbol_endIf                     = 10,
    symbol_endLoop                   = 11,
    symbol_endObject                 = 12,
    symbol_endWhile                  = 13,
    symbol_in                        = 14,
    symbol_METHODS                   = 15,
    symbol_method                    = 16,
    symbol_object                    = 17,
    symbol_of                        = 18,
    symbol_PROPERTIES                = 19,
    symbol_close_brace               = 20,
    symbol_close_bracket             = 21,
    symbol_close_comment             = 22,
    symbol_close_paren               = 23,

    symbol_last_non_statement        = 24,  // End non-statement list

    // general
    comment                          = 25,
    identifier                       = 26,

    // control
    symbol_forEach                   = 27,
    symbol_if                        = 28,
    symbol_loop                      = 29,
    symbol_return                    = 30,
    symbol_throw                     = 31,
    symbol_trace                     = 32,
    symbol_tron                      = 33,
    symbol_troff                     = 34,
    symbol_while                     = 35,

    // expressions
    literal_Real                     = 36,
    literal_Real_pi                  = 37,
    literal_Integer                  = 38,
    literal_Character                     = 39,
    literal_Logical_true             = 40,
    literal_Logical_false            = 41,
    literal_String                   = 42,
    symbol_add_with_assign           = 43,
    symbol_ampersand                 = 44,
    symbol_and                       = 45,
    symbol_as                        = 46,
    symbol_at                        = 47,
    symbol_bitwise_and_with_assign   = 48,
    symbol_bitwise_not               = 49,
    symbol_bitwise_or                = 50,
    symbol_bitwise_or_with_assign    = 51,
    symbol_bitwise_xor               = 52,
    symbol_bitwise_xor_with_assign   = 53,
    symbol_colon                     = 54,
    symbol_comma                     = 55,
    symbol_compare                   = 56,
    symbol_decrement                 = 57,
    symbol_divide                    = 58,
    symbol_divide_with_assign        = 59,
    symbol_dollar                    = 60,
    symbol_downtogreaterthan         = 61,
    symbol_empty_braces              = 62,
    symbol_empty_brackets            = 63,
    symbol_eq                        = 64,
    symbol_equals                    = 65,
    symbol_ge                        = 66,
    symbol_global                    = 67,
    symbol_gt                        = 68,
    symbol_increment                 = 69,
    symbol_instanceOf                = 70,
    symbol_is                        = 71,
    symbol_le                        = 72,
    symbol_lt                        = 73,
    symbol_local                     = 74,
    symbol_member_with_assign        = 75,
    symbol_minus                     = 76,
    symbol_mod_with_assign           = 77,
    symbol_multiply_with_assign      = 78,
    symbol_ne                        = 79,
    symbol_noAction                  = 80,
    symbol_not                       = 81,
    symbol_null                      = 82,
    symbol_open_brace                = 83,
    symbol_open_bracket              = 84,
    symbol_open_paren                = 85,
    symbol_or                        = 86,
    symbol_percent                   = 87,
    symbol_period                    = 88,
    symbol_plus                      = 89,
    symbol_power                     = 90,
    symbol_power_with_assign         = 91,
    symbol_question_mark             = 92,
    symbol_semicolon                 = 93,
    symbol_shl                       = 94,
    symbol_shr                       = 95,
    symbol_shrx                      = 96,
    symbol_subtract_with_assign      = 97,
    symbol_this                      = 98,
    symbol_times                     = 99,
    symbol_upto                      = 100,
    symbol_uptolessthan              = 101,
    
    last_token                       = 200;  // 101, ,

  static public String get_name( int token )
  {
    switch (token)
    {
      case eof:
        return "end of file";
      case eol:
        return "eol";

      case symbol_class:
        return "class";
      case symbol_function:
        return "function";
      case symbol_else:
        return "else";
      case symbol_else_eol:
        return "else";
      case symbol_elseIf:
        return "elseIf";
      case symbol_endClass:
        return "endClass";
      case symbol_endForEach:
        return "endForEach";
      case symbol_endFunction:
        return "endFunction";
      case symbol_endIf:
        return "endIf";
      case symbol_endLoop:
        return "endLoop";
      case symbol_endObject:
        return "endObject";
      case symbol_endWhile:
        return "endWhile";
      case symbol_in:
        return "in";
      case symbol_METHODS:
        return "METHODS";
      case symbol_method:
        return "method";
      case symbol_object:
        return "object";
      case symbol_of:
        return "of";
      case symbol_PROPERTIES:
        return "PROPERTIES";
      case symbol_close_brace:
        return "}";
      case symbol_close_bracket:
        return "]";
      case symbol_close_comment:
        return "}#";
      case symbol_close_paren:
        return ")";

      case comment:
        return "comment";
      case identifier:
        return "identifier";

      case symbol_forEach:
        return "forEach";
      case symbol_if:
        return "if";
      case symbol_loop:
        return "loop";
      case symbol_return:
        return "return";
      case symbol_throw:
        return "throw";
      case symbol_trace:
        return "trace";
      case symbol_tron:
        return "tron";
      case symbol_troff:
        return "troff";
      case symbol_while:
        return "while";

      case literal_Real:
        return "Real";
      case literal_Real_pi:
        return "pi";
      case literal_Integer:
        return "Integer";
      case literal_Character:
        return "Character";
      case literal_Logical_true:
        return "true";
      case literal_Logical_false:
        return "false";
      case literal_String:
        return "String";
      case symbol_add_with_assign:
        return "+=";
      case symbol_ampersand:
        return "&";
      case symbol_and:
        return "and";
      case symbol_as:
        return "as";
      case symbol_at:
        return "@";
      case symbol_bitwise_and_with_assign:
        return "&=";
      case symbol_bitwise_not:
        return "!";
      case symbol_bitwise_or:
        return "|";
      case symbol_bitwise_or_with_assign:
        return "|=";
      case symbol_bitwise_xor:
        return "~";
      case symbol_bitwise_xor_with_assign:
        return "~=";
      case symbol_colon:
        return ":";
      case symbol_comma:
        return ",";
      case symbol_compare:
        return "<>";
      case symbol_decrement:
        return "--";
      case symbol_divide:
        return "/";
      case symbol_divide_with_assign:
        return "/=";
      case symbol_dollar:
        return "$";
      case symbol_downtogreaterthan:
        return "..>";
      case symbol_empty_braces:
        return "{}";
      case symbol_empty_brackets:
        return "[]";
      case symbol_eq:
        return "==";
      case symbol_equals:
        return "=";
      case symbol_ge:
        return ">=";
      case symbol_global:
        return "global";
      case symbol_gt:
        return ">";
      case symbol_increment:
        return "++";
      case symbol_instanceOf:
        return "instanceOf";
      case symbol_is:
        return "is";
      case symbol_le:
        return "<=";
      case symbol_local:
        return "local";
      case symbol_lt:
        return "<";
      case symbol_member_with_assign:
        return ".=";
      case symbol_minus:
        return "-";
      case symbol_mod_with_assign:
        return "%=";
      case symbol_multiply_with_assign:
        return "*=";
      case symbol_ne:
        return "!=";
      case symbol_noAction:
        return "noAction";
      case symbol_not:
        return "not";
      case symbol_null:
        return "null";
      case symbol_open_brace:
        return "{";
      case symbol_open_bracket:
        return "[";
      case symbol_open_paren:
        return "(";
      case symbol_or:
        return "or";
      case symbol_percent:
        return "%";
      case symbol_period:
        return ".";
      case symbol_plus:
        return "+";
      case symbol_power:
        return "^";
      case symbol_power_with_assign:
        return "^=";
      case symbol_question_mark:
        return "?";
      case symbol_semicolon:
        return ";";
      case symbol_shl:
        return ":<<:";
      case symbol_shr:
        return ":>>:";
      case symbol_shrx:
        return ":>>>:";
      case symbol_subtract_with_assign:
        return "-=";
      case symbol_this:
        return "this";
      case symbol_times:
        return "*";
      case symbol_upto:
        return "..";
      case symbol_uptolessthan:
        return "..<";
      default:
        return "(Unknown token)";
    }
  }
};

