import alphavm.*;

public class BCProgram extends BardProgram
{
  static public void main( String[] args )
  {
    new BCProgram().launch();
  }

  public BCProgram()
  {
    init();
  }

  public void define_id_table()
  {
    vm.id_table.configure(
      new String[]
      {
        "Object",
        "init_object",
        "Variant",
        "Real",
        "Integer",
        "Character",
        "Logical",
        "null",
        "RealObject",
        "IntegerObject",
        "CharacterObject",
        "LogicalObject",
        "String",
        "Value",
        "Global",
        "Collection",
        "List",
        "Table",
        "class",
        "Main",
        "PROPERTIES",
        "ch",
        "METHODS",
        "method",
        "init",
        "Parser",
        "println",
        "parse",
        "TypeDefs",
        "resolve",
        "local",
        "writer",
        "JavaVMWriter",
        "compile",
        "save",
        "endClass",
        "built_in",
        "native",
        "return",
        "false",
        "get",
        "v",
        "throw",
        "set",
        "key",
        "value",
        "index",
        "keys",
        "System",
        "Real_to_Integer_high_bits",
        "n",
        "Real_to_Integer_low_bits",
        "sleep",
        "seconds",
        "this",
        "count",
        "clear",
        "add",
        "add_all",
        "other",
        "forEach",
        "in",
        "if",
        "not",
        "instanceOf",
        "remove",
        "remove_at",
        "insert",
        "obj",
        "before_index",
        "swap",
        "i1",
        "i2",
        "temp",
        "reverse",
        "while",
        "endWhile",
        "first",
        "last",
        "remove_first",
        "remove_last",
        "sort",
        "pass",
        "sorted",
        "true",
        "i",
        "a",
        "b",
        "endIf",
        "buffer",
        "print",
        "element",
        "else",
        "endForEach",
        "at",
        "contains",
        "Time",
        "current",
        "object",
        "Random",
        "seed_value",
        "seed",
        "bits",
        "low",
        "high",
        "limit",
        "endObject",
        "Writer",
        "write",
        "close",
        "Byte",
        "Byte[]",
        "duplicate",
        "substring",
        "index_of",
        "look_for",
        "before",
        "after",
        "compare_to",
        "Reader",
        "StringReader",
        "Character[]",
        "ensure_capacity",
        "result",
        "LineReader",
        "TextWriter",
        "Integer[]",
        "list",
        "Pair",
        "_a",
        "_b",
        "File",
        "filepath",
        "_filepath",
        "FileReader",
        "FileWriter",
        "available",
        "has_another",
        "read",
        "peek",
        "NativeFileWriter",
        "ready",
        "dest",
        "is",
        "src",
        "prepped",
        "_src",
        "or",
        "data",
        "position",
        "_data",
        "offset",
        "zero_char",
        "consume",
        "consume_whitespace",
        "noAction",
        "st",
        "of",
        "_dest",
        "Math",
        "floor",
        "ParseReader",
        "line",
        "column",
        "filename",
        "reader",
        "as",
        "and",
        "elseIf",
        "TokenType",
        "eof",
        "eol",
        "symbol_class",
        "symbol_function",
        "symbol_else",
        "symbol_elseIf",
        "symbol_endClass",
        "symbol_endForEach",
        "symbol_endFunction",
        "symbol_endIf",
        "symbol_endLoop",
        "symbol_endObject",
        "symbol_endWhile",
        "symbol_in",
        "symbol_METHODS",
        "symbol_method",
        "symbol_object",
        "symbol_of",
        "symbol_PROPERTIES",
        "symbol_close_brace",
        "symbol_close_bracket",
        "symbol_close_comment",
        "symbol_close_paren",
        "symbol_close_special",
        "symbol_last_non_statement",
        "comment",
        "identifier",
        "symbol_forEach",
        "symbol_if",
        "symbol_loop",
        "symbol_return",
        "symbol_throw",
        "symbol_trace",
        "symbol_tron",
        "symbol_troff",
        "symbol_while",
        "literal_Real",
        "literal_Real_pi",
        "literal_Integer",
        "literal_Character",
        "literal_Logical_true",
        "literal_Logical_false",
        "literal_String",
        "symbol_add_with_assign",
        "symbol_ampersand",
        "symbol_and",
        "symbol_as",
        "symbol_at",
        "symbol_bitwise_and_with_assign",
        "symbol_bitwise_not",
        "symbol_bitwise_or",
        "symbol_bitwise_or_with_assign",
        "symbol_bitwise_xor",
        "symbol_bitwise_xor_with_assign",
        "symbol_colon",
        "symbol_comma",
        "symbol_compare",
        "symbol_decrement",
        "symbol_divide",
        "symbol_divide_with_assign",
        "symbol_dollar",
        "symbol_downtogreaterthan",
        "symbol_empty_braces",
        "symbol_empty_brackets",
        "symbol_eq",
        "symbol_equals",
        "symbol_ge",
        "symbol_global",
        "symbol_gt",
        "symbol_increment",
        "symbol_instanceOf",
        "symbol_is",
        "symbol_le",
        "symbol_lt",
        "symbol_local",
        "symbol_member_with_assign",
        "symbol_minus",
        "symbol_mod_with_assign",
        "symbol_multiply_with_assign",
        "symbol_ne",
        "symbol_noAction",
        "symbol_not",
        "symbol_null",
        "symbol_open_brace",
        "symbol_open_bracket",
        "symbol_open_paren",
        "symbol_open_special",
        "symbol_or",
        "symbol_percent",
        "symbol_period",
        "symbol_plus",
        "symbol_power",
        "symbol_power_with_assign",
        "symbol_question_mark",
        "symbol_semicolon",
        "symbol_shl",
        "symbol_shr",
        "symbol_shrx",
        "symbol_subtract_with_assign",
        "symbol_this",
        "symbol_times",
        "symbol_upto",
        "symbol_uptolessthan",
        "symbol_xor",
        "last_token",
        "name_lookup",
        "keywords",
        "set_up_keywords",
        "token_list",
        "type",
        "Token",
        "content",
        "scanner",
        "Scanner",
        "_type",
        "_content",
        "_value",
        "_line",
        "_column",
        "t",
        "digit",
        "intval",
        "error",
        "mesg",
        "tokens",
        "tokenize",
        "scan_next",
        "loop",
        "endLoop",
        "consume_eols",
        "scan_comment",
        "scan_identifier",
        "scan_String",
        "scan_number",
        "scan_symbol",
        "keep_reading",
        "terminator",
        "add_String_or_Char",
        "scan_Character",
        "scan_alternate_String",
        "scan_hex_value",
        "ch2",
        "ch3",
        "digits",
        "next_is_hex_digit",
        "is_negative",
        "is_Real",
        "scan_Integer",
        "start_pos",
        "fraction",
        "negative_exponent",
        "power",
        "code",
        "TokenReader",
        "_tokens",
        "rewind",
        "num_ahead",
        "this_type",
        "TypeDef",
        "this_method",
        "Method",
        "property_list",
        "local_declarations",
        "parse_types",
        "Template",
        "type_has_specialization",
        "name",
        "mappings",
        "placeholder_names",
        "must_consume",
        "pos",
        "next_is",
        "replacement_name",
        "replacement_tokens",
        "rt",
        "parse_type_definition",
        "text",
        "next_is_end_command",
        "discard_end_commands",
        "must_consume_end_command",
        "read_identifier",
        "parse_template",
        "end_symbol",
        "template",
        "Templates",
        "base_types",
        "parse_type",
        "def",
        "parse_property_list",
        "parse_method",
        "property",
        "Property",
        "add_property",
        "initial_value",
        "parse_expression",
        "parse_attributes",
        "Attributes",
        "parse_specialization_string",
        "Type",
        "unchecked_type",
        "checked_type",
        "param_name",
        "param_type",
        "add_parameter",
        "Local",
        "return_type",
        "attributes",
        "qualifiers",
        "Qualifiers",
        "is_native",
        "add_method",
        "parse_multi_line_statements",
        "body",
        "statements",
        "CmdStatementList",
        "next_is_statement",
        "parse_statement",
        "parse_single_line_statements",
        "allow_control_structures",
        "parse_if",
        "err",
        "parse_local_declaration",
        "CmdReturnNil",
        "CmdReturnValue",
        "CmdTron",
        "CmdTroff",
        "CmdPrintln",
        "expression",
        "CmdAssign",
        "CmdIf",
        "cmd_if",
        "else_body",
        "add_local",
        "vv",
        "implicit_type",
        "e",
        "CmdLocalDeclaration",
        "CmdWriteLocal",
        "CmdWriteLocalDefault",
        "Cmd",
        "parse_logical_xor",
        "parse_logical_or",
        "lhs",
        "CmdLogicalXor",
        "parse_logical_and",
        "CmdLogicalOr",
        "parse_comparison",
        "CmdLogicalAnd",
        "parse_bitwise_xor",
        "CmdCompareEQ",
        "CmdCompareNE",
        "CmdCompareLT",
        "CmdCompareGT",
        "CmdCompareLE",
        "CmdCompareGE",
        "CmdInstanceOf",
        "parse_bitwise_or",
        "CmdBitwiseXor",
        "parse_bitwise_and",
        "CmdBitwiseOr",
        "parse_bitwise_shift",
        "CmdBitwiseAnd",
        "parse_add_subtract",
        "CmdBitwiseSHL",
        "CmdBitwiseSHR",
        "CmdBitwiseSHRX",
        "parse_multiply_divide_mod",
        "CmdAdd",
        "CmdSubtract",
        "parse_power",
        "CmdMultiply",
        "CmdDivide",
        "CmdMod",
        "parse_pre_unary",
        "CmdPower",
        "CmdLogicalNot",
        "CmdNegate",
        "CmdBitwiseNot",
        "parse_post_unary",
        "parse_access",
        "operand",
        "CmdLogicalize",
        "parse_term",
        "access",
        "parse_access_command",
        "CmdAccess",
        "args",
        "CmdArgs",
        "CmdLiteralString",
        "CmdLiteralNull",
        "CmdLiteralReal",
        "CmdLiteralInteger",
        "CmdLiteralCharacter",
        "CmdLiteralLogical",
        "_t",
        "_name",
        "lookup",
        "find",
        "instantiate",
        "Types",
        "unchecked",
        "must_find",
        "type_table",
        "type_Object",
        "type_String",
        "type_Real",
        "type_Integer",
        "type_Character",
        "type_Logical",
        "type_RealObject",
        "type_IntegerObject",
        "type_CharacterObject",
        "type_LogicalObject",
        "type_Variant",
        "type_NullObject",
        "type_IntegerList",
        "type_CharacterList",
        "type_ByteList",
        "type_Value",
        "type_Global",
        "type_Collection",
        "type_List",
        "type_Table",
        "define",
        "is_builtIn",
        "is_primitive",
        "m_to_String",
        "Op",
        "push_Object_this",
        "return_Object",
        "defined",
        "collect_ids",
        "type_def",
        "IDTable",
        "properties",
        "id",
        "get_id",
        "is_singleton",
        "possibly_null",
        "_def",
        "_possibly_null",
        "is_reference",
        "is_Integer",
        "is_Character",
        "is_Logical",
        "is_Object",
        "is_String",
        "instance_of",
        "ancestor",
        "organize",
        "methods",
        "organized",
        "resolved",
        "_qualifiers",
        "base_type",
        "m",
        "find_property",
        "p",
        "find_method",
        "signature",
        "mv",
        "Analyzer",
        "original_properties",
        "property_work_list",
        "collect_properties",
        "create_signature",
        "original_methods",
        "candidate_methods",
        "collect_methods",
        "type_context",
        "collect_method",
        "command_work_list",
        "pv",
        "CmdWriteProperty",
        "m_init_object",
        "from_list",
        "collect_property",
        "existing_p",
        "existing_m",
        "_type_context",
        "_index",
        "method_id",
        "ip",
        "literal_String_values",
        "literal_Real_values",
        "parameters",
        "locals",
        "full_signature",
        "as_type",
        "is_init",
        "add_return_guard",
        "push_method_context",
        "visitor",
        "ImplicitReturnTypeVisitor",
        "dispatch",
        "found_return",
        "pop_method_context",
        "CmdReturnThis",
        "CmdThrowMissingReturn",
        "on_method_started",
        "on_method_finished",
        "local_scope_stack",
        "compatible_methods",
        "empty_args",
        "context_stack",
        "push_local_scope",
        "pop_local_scope",
        "x",
        "y",
        "target_count",
        "find_local",
        "apply_implicit_return_type",
        "resolve_call",
        "context",
        "return_type_hint",
        "error_on_fail",
        "cast_to",
        "CmdDynamicCall",
        "original_args",
        "all_match",
        "m_returning_Object",
        "sig",
        "arg",
        "Visitor",
        "visit",
        "cmd",
        "determine_implicit_return_type",
        "variable_name",
        "resolve_with_type_hint",
        "require_Logical",
        "require_value",
        "allow_unchecked",
        "cast_t",
        "to_type",
        "CmdCastToType",
        "common_type",
        "rhs",
        "lhs_type",
        "rhs_type",
        "resolve_assignment",
        "new_value",
        "message",
        "statement",
        "result_type",
        "pop_discard",
        "cmd1",
        "cmd2",
        "CmdLiteral",
        "push_literal_Real",
        "push_literal_Integer",
        "push_literal_String",
        "of_type",
        "push_literal_Object_null",
        "CmdUnary",
        "opcode",
        "_operand",
        "operand_type",
        "resolve_for_literal_Real_operand",
        "resolve_for_literal_Integer_operand",
        "resolve_for_literal_Logical_operand",
        "resolve_for_literal_operand",
        "resolve_for_Real_operand",
        "resolve_for_Integer_operand",
        "resolve_for_Logical_operand",
        "resolve_for_miscellaneous_operand",
        "operand_value",
        "logical_not",
        "negate_Real",
        "negate_Integer",
        "bitwise_not_Integer",
        "logicalize_Real",
        "logicalize_Integer",
        "logicalize_Object",
        "_args",
        "no_hint",
        "type_hint",
        "CmdReadProperty",
        "CmdReadLocal",
        "CmdNewObject",
        "CmdReadSingleton",
        "_of_type",
        "read_singleton",
        "_context",
        "_property",
        "property_type",
        "read_property_Real",
        "read_property_Integer",
        "read_property_Character",
        "read_property_Logical",
        "read_property_Object",
        "_new_value",
        "write_property_Real",
        "write_property_Integer",
        "write_property_Character",
        "write_property_Logical",
        "write_property_Object",
        "init_method",
        "create_object",
        "dynamic_call",
        "method_info",
        "context_type",
        "CmdBox",
        "CmdStatement",
        "CmdControlStructure",
        "condition",
        "_condition",
        "condition_type",
        "CmdIfNonNull",
        "control_id",
        "next_control_id",
        "next_check",
        "end_label",
        "has_else",
        "else_count",
        "jump_if_false",
        "write_address",
        "jump",
        "define_label",
        "CmdElseIf",
        "checked_local",
        "_body",
        "_else_body",
        "local_var_name",
        "label",
        "jump_if_null",
        "write_local_Object",
        "CmdStatementWithOperand",
        "print_String",
        "print_Real",
        "print_Integer",
        "print_Character",
        "print_Logical",
        "print_Object",
        "println_nil",
        "throw_missing_return",
        "trace_on",
        "trace_off",
        "CmdReturn",
        "return_nil",
        "CmdReturnNull",
        "_return_type",
        "return_Real",
        "return_Integer",
        "CmdCast",
        "_to_type",
        "from_type",
        "CmdGeneralizeReference",
        "CmdSpecializeReference",
        "CmdCastIntegerToReal",
        "CmdCastRealToInteger",
        "type_check",
        "cast_Integer_to_Real",
        "cast_Real_to_Integer",
        "CmdTypeManipulation",
        "target_type",
        "_target_type",
        "Fixed_instanceOf",
        "CmdBinary",
        "_lhs",
        "_rhs",
        "resolve_for_common_type",
        "resolve_operands_and_cast_to_common_type",
        "combine_literal_Real_operands",
        "combine_literal_Integer_operands",
        "combine_literal_Logical_operands",
        "combine_literal_operands",
        "resolve_for_Real_operands",
        "resolve_for_Integer_operands",
        "resolve_for_Logical_operands",
        "resolve_for_miscellaneous_operands",
        "resolve_operands",
        "cast_to_common_type",
        "lhs_value",
        "rhs_value",
        "add_Real",
        "add_Integer",
        "bitwise_or_Integer",
        "subtract_Real",
        "subtract_Integer",
        "multiply_Real",
        "multiply_Integer",
        "bitwise_and_Integer",
        "divide_Real",
        "mod_Real",
        "mod_Integer",
        "power_Real",
        "power_Integer",
        "bitwise_xor_Integer",
        "shl_Integer",
        "shr_Integer",
        "shrx_Integer",
        "if_false_push_false_and_jump",
        "CmdEitherOr",
        "if_true_push_true_and_jump",
        "_common_type",
        "jump_if_not_null",
        "CmdComparison",
        "eq_Real",
        "eq_Integer",
        "eq_Object",
        "ne_Real",
        "ne_Integer",
        "ne_Object",
        "lt_Real",
        "lt_Integer",
        "gt_Real",
        "gt_Integer",
        "le_Real",
        "le_Integer",
        "ge_Real",
        "ge_Integer",
        "local_info",
        "_v",
        "new_value_type",
        "write_local_Real",
        "write_local_Integer",
        "push_literal_Real_0",
        "push_literal_Integer_0",
        "read_local_Real",
        "read_local_Integer",
        "read_local_Object",
        "halt",
        "jump_if_true",
        "return_Variant",
        "return_Native",
        "throw_Exception",
        "println_Variant",
        "finite_loop",
        "unbox_Real",
        "unbox_Integer",
        "unbox_Character",
        "unbox_Logical",
        "box_Real",
        "box_Integer",
        "box_Character",
        "box_Logical",
        "Variant_instanceOf",
        "cast_Object_to_Variant",
        "cast_Real_to_Variant",
        "cast_Integer_to_Variant",
        "cast_Character_to_Variant",
        "cast_Logical_to_Variant",
        "cast_Variant_to_Object",
        "cast_Variant_to_Real",
        "cast_Variant_to_Integer",
        "cast_Variant_to_Character",
        "cast_Character_to_Integer",
        "push_literal_Native_null",
        "push_literal_Real_negative_1",
        "push_literal_Real_1",
        "push_literal_Integer_negative_1",
        "push_literal_Integer_1",
        "negate_Logical",
        "is_Variant",
        "add_Object",
        "read_local_Variant",
        "read_local_Native",
        "write_local_Variant",
        "write_local_Native",
        "increment_local_Object",
        "increment_local_Real",
        "increment_local_Integer",
        "decrement_local_Object",
        "decrement_local_Real",
        "decrement_local_Integer",
        "read_property_Native",
        "write_property_Native",
        "read_this_property_Object",
        "read_this_property_Real",
        "read_this_property_Integer",
        "write_this_property_Object",
        "write_this_property_Real",
        "write_this_property_Integer",
        "List_create",
        "List_count",
        "List_get",
        "List_set",
        "List_add",
        "List_insert",
        "List_remove_at",
        "List_clear",
        "Table_create",
        "Table_count",
        "Table_set",
        "Table_get_key",
        "last_op",
        "BackpatchInfo",
        "_label",
        "_position",
        "code_string_char_lookup",
        "current_control_id",
        "backpatch_locations",
        "label_definitions",
        "backpatch",
        "info",
        "program_name",
        "to_octal_String",
        "code_buffer",
        "m_v",
        "base_type_v",
        "property_v",
        "st_v",
        "print_hex",
        "code_index",
        "print_byte_string",
        "min_digits",
        "print_octal_encoding",
        "init_object()",
        "init()",
        "String()",
        "Character()",
        "Real()",
        "Integer()",
        "Logical()",
        "get(Variant)",
        "set(Variant,Variant)",
        "Collection()",
        "List()",
        "Table()",
        "key(Integer)",
        "keys()",
        "duplicate()",
        "substring(Integer,Integer)",
        "substring(Integer)",
        "index_of(Character)",
        "contains(Character)",
        "before(Integer)",
        "after(Integer)",
        "count()",
        "get(Integer)",
        "compare_to(String)",
        "Reader()",
        "clear()",
        "add(Variant)",
        "add_all(List)",
        "remove(Variant)",
        "remove_at(Integer)",
        "insert(Variant)",
        "insert(Variant,Integer)",
        "swap(Integer,Integer)",
        "reverse()",
        "first()",
        "last()",
        "remove_first()",
        "remove_last()",
        "sort()",
        "at(Integer)",
        "contains(Variant)",
        "Real_to_Integer_high_bits(Real)",
        "Real_to_Integer_low_bits(Real)",
        "sleep(Real)",
        "current()",
        "init(Integer)",
        "seed(Integer)",
        "bits(Integer)",
        "Real(Real,Real)",
        "Integer(Integer)",
        "Integer(Integer,Integer)",
        "write(Variant)",
        "print(Variant)",
        "println()",
        "println(Variant)",
        "close()",
        "add(Integer)",
        "Writer()",
        "ensure_capacity(Integer)",
        "LineReader()",
        "set(Integer,Integer)",
        "add(Integer[])",
        "init(Variant,Variant)",
        "init(String)",
        "Character[]()",
        "available()",
        "has_another()",
        "read()",
        "peek()",
        "ready()",
        "write(Character)",
        "init(Reader)",
        "peek(Integer)",
        "consume(Character)",
        "consume_whitespace()",
        "consume(String)",
        "init(Writer)",
        "floor(Real)",
        "init(String,String)",
        "set_up_keywords()",
        "init(Scanner,Integer)",
        "init(Scanner,Integer,Character[])",
        "init(Scanner,Integer,Real)",
        "init(String,Integer,Integer)",
        "error(String)",
        "tokenize(String)",
        "tokenize(String,String)",
        "consume_eols()",
        "scan_next()",
        "scan_identifier()",
        "scan_String(Character)",
        "scan_alternate_String()",
        "scan_Character()",
        "scan_hex_value(Integer)",
        "next_is_hex_digit()",
        "scan_number()",
        "scan_Integer()",
        "scan_comment()",
        "scan_symbol()",
        "add_String_or_Char()",
        "init(List)",
        "rewind()",
        "parse(String)",
        "parse(Template,TypeDef,Token)",
        "next_is(Integer)",
        "consume(Integer)",
        "must_consume(Integer)",
        "next_is_end_command()",
        "discard_end_commands()",
        "must_consume_end_command()",
        "read_identifier()",
        "parse_types()",
        "parse_template(Integer)",
        "parse_type_definition()",
        "parse_property_list()",
        "parse_attributes(Attributes)",
        "parse_specialization_string()",
        "parse_type()",
        "parse_method()",
        "parse_multi_line_statements(CmdStatementList)",
        "parse_single_line_statements(CmdStatementList)",
        "next_is_statement()",
        "parse_statement(CmdStatementList,Logical)",
        "parse_if()",
        "parse_local_declaration(CmdStatementList)",
        "parse_expression()",
        "parse_logical_xor()",
        "parse_logical_xor(Cmd)",
        "parse_logical_or()",
        "parse_logical_or(Cmd)",
        "parse_logical_and()",
        "parse_logical_and(Cmd)",
        "parse_comparison()",
        "parse_comparison(Cmd)",
        "parse_bitwise_xor()",
        "parse_bitwise_xor(Cmd)",
        "parse_bitwise_or()",
        "parse_bitwise_or(Cmd)",
        "parse_bitwise_and()",
        "parse_bitwise_and(Cmd)",
        "parse_bitwise_shift()",
        "parse_bitwise_shift(Cmd)",
        "parse_add_subtract()",
        "parse_add_subtract(Cmd)",
        "parse_multiply_divide_mod()",
        "parse_multiply_divide_mod(Cmd)",
        "parse_power()",
        "parse_power(Cmd)",
        "parse_pre_unary()",
        "parse_post_unary()",
        "parse_post_unary(Cmd)",
        "parse_access()",
        "parse_access(Cmd)",
        "parse_access_command(Token)",
        "parse_term()",
        "init(Token,String,Integer)",
        "is_primitive()",
        "is_reference()",
        "instance_of(TypeDef)",
        "add_property(Property)",
        "add_method(Method)",
        "find_property(String)",
        "find_method(String)",
        "organize()",
        "organize(Token)",
        "collect_properties(List)",
        "collect_property(Property)",
        "collect_methods(List)",
        "collect_method(Method)",
        "resolve()",
        "resolve(Token)",
        "compile(JavaVMWriter)",
        "init(Token,TypeDef,String,Integer)",
        "create_signature()",
        "full_signature()",
        "add_parameter(Local)",
        "add_local(Local)",
        "init(Token,String)",
        "add(String)",
        "add(String,String)",
        "contains(String)",
        "get(String)",
        "init(TypeDef,Logical)",
        "name()",
        "is_Real()",
        "is_Integer()",
        "is_Character()",
        "is_Logical()",
        "is_Object()",
        "is_String()",
        "instance_of(Type)",
        "init(Token)",
        "implicit_type()",
        "dispatch(Visitor)",
        "determine_implicit_return_type()",
        "type()",
        "variable_name()",
        "resolve_with_type_hint(Type)",
        "require_Logical()",
        "require_value()",
        "require_value(Logical)",
        "cast_to(Token,Type)",
        "common_type(Cmd,Cmd)",
        "resolve_assignment(Cmd)",
        "add(Cmd)",
        "insert(Cmd)",
        "init(Token,Cmd)",
        "init(Token,String,CmdArgs)",
        "init(Token,Cmd,String,CmdArgs)",
        "add(Template)",
        "find(String)",
        "instantiate(TypeDef,Token)",
        "find(String,Logical)",
        "must_find(Token,String,Logical)",
        "define(Token,String,Integer)",
        "get(Token,String)",
        "must_find(Token,String)",
        "collect_ids()",
        "on_method_started(Method)",
        "next_control_id()",
        "write_address(String)",
        "define_label(String)",
        "on_method_finished()",
        "backpatch(BackpatchInfo)",
        "write(Integer)",
        "save(String)",
        "print_hex(Integer,Integer)",
        "to_octal_String(Integer)",
        "print_byte_string(Integer)",
        "print_octal_encoding(Integer)",
        "get_id(String)",
        "init(Token,TypeDef,String)",
        "duplicate(TypeDef,Integer)",
        "init(Token,String,Type)",
        "offset()",
        "push_method_context(Method)",
        "pop_method_context()",
        "push_local_scope()",
        "pop_local_scope()",
        "find_local(String)",
        "apply_implicit_return_type(Type)",
        "resolve_call(Token,Type,Cmd,String,CmdArgs,Type,Logical)",
        "find_method(Token,Type,String,CmdArgs,Type,Logical)",
        "init(Cmd)",
        "init(Cmd,Cmd)",
        "cast_to(List)",
        "visit(Cmd)",
        "init(Token,Real)",
        "init(Token,Integer)",
        "init(Token,Character)",
        "init(Token,Logical)",
        "resolve_for_literal_Real_operand(Real)",
        "resolve_for_literal_Integer_operand(Integer)",
        "resolve_for_literal_Logical_operand(Logical)",
        "resolve_for_literal_operand()",
        "resolve_for_Real_operand()",
        "resolve_for_Integer_operand()",
        "resolve_for_Logical_operand()",
        "resolve_for_miscellaneous_operand()",
        "init(Token,Type)",
        "init(Token,Cmd,Property)",
        "init(Token,Cmd,Property,Cmd)",
        "init(Token,Type,CmdArgs)",
        "init(Token,Cmd,Method,CmdArgs)",
        "init(Token,Cmd,CmdStatementList,CmdStatementList)",
        "init(Token,TypeDef)",
        "init(Token,Cmd,Type)",
        "init(Token,Cmd,Cmd)",
        "resolve_for_common_type(Type)",
        "resolve_operands()",
        "cast_to_common_type(Type)",
        "resolve_operands_and_cast_to_common_type()",
        "combine_literal_Real_operands(Real,Real)",
        "combine_literal_Integer_operands(Integer,Integer)",
        "combine_literal_Logical_operands(Logical,Logical)",
        "combine_literal_operands(Type)",
        "resolve_for_Real_operands()",
        "resolve_for_Integer_operands()",
        "resolve_for_Logical_operands()",
        "resolve_for_miscellaneous_operands(Type)",
        "init(Token,Cmd,Cmd,Type)",
        "init(Token,Local)",
        "init(Token,Local,Cmd)",
        "init(String,Integer)"
      }
    );
  }

  public void define_types()
  {
    // Type definitions
    define_type( "Object", 2, 0);
    define_type( "Variant", 2, 0);
    define_type( "Real", 7, 0);
    define_type( "Integer", 7, 0);
    define_type( "Character", 7, 0);
    define_type( "Logical", 7, 0);
    define_type( "null", 7, 0);
    define_type( "RealObject", 4, 0);
    define_type( "IntegerObject", 4, 0);
    define_type( "CharacterObject", 4, 0);
    define_type( "LogicalObject", 4, 0);
    define_type( "String", 4, 0);
    define_type( "Value", 0, 0);
    define_type( "Global", 0, 0);
    define_type( "Collection", 0, 0);
    define_type( "List", 4, 0);
    define_type( "Table", 4, 0);
    define_type( "Main", 0, 1);
    define_type( "System", 0, 0);
    define_type( "Time", 0, 0);
    define_type( "Random", 12, 0);
    define_type( "Writer", 0, 0);
    define_type( "Byte[]", 4, 0);
    define_type( "Character[]", 4, 0);
    define_type( "Integer[]", 4, 0);
    define_type( "Pair", 0, 2);
    define_type( "File", 0, 1);
    define_type( "Reader", 0, 0);
    define_type( "FileReader", 4, 0);
    define_type( "NativeFileWriter", 4, 0);
    define_type( "FileWriter", 0, 2);
    define_type( "LineReader", 0, 3);
    define_type( "StringReader", 0, 3);
    define_type( "TextWriter", 0, 2);
    define_type( "Math", 0, 0);
    define_type( "ParseReader", 0, 5);
    define_type( "TokenType", 8, 107);
    define_type( "Token", 0, 6);
    define_type( "Scanner", 8, 7);
    define_type( "TokenReader", 0, 3);
    define_type( "Parser", 0, 6);
    define_type( "TypeDef", 0, 13);
    define_type( "Method", 0, 17);
    define_type( "Template", 0, 4);
    define_type( "Attributes", 0, 1);
    define_type( "Type", 0, 2);
    define_type( "CmdStatementList", 0, 2);
    define_type( "CmdIf", 0, 4);
    define_type( "Cmd", 0, 1);
    define_type( "CmdAccess", 0, 4);
    define_type( "Templates", 0, 1);
    define_type( "Types", 0, 0);
    define_type( "TypeDefs", 8, 21);
    define_type( "JavaVMWriter", 0, 7);
    define_type( "IDTable", 8, 2);
    define_type( "Qualifiers", 0, 4);
    define_type( "Property", 0, 7);
    define_type( "Local", 0, 5);
    define_type( "Analyzer", 0, 10);
    define_type( "CmdArgs", 0, 1);
    define_type( "Visitor", 0, 0);
    define_type( "ImplicitReturnTypeVisitor", 0, 1);
    define_type( "CmdReturnValue", 0, 2);
    define_type( "CmdLiteral", 0, 1);
    define_type( "CmdLiteralReal", 0, 2);
    define_type( "CmdLiteralInteger", 0, 2);
    define_type( "CmdLiteralCharacter", 0, 2);
    define_type( "CmdLiteralLogical", 0, 2);
    define_type( "CmdLiteralString", 0, 2);
    define_type( "CmdLiteralNull", 0, 2);
    define_type( "CmdUnary", 0, 3);
    define_type( "CmdLogicalNot", 0, 3);
    define_type( "CmdNegate", 0, 3);
    define_type( "CmdBitwiseNot", 0, 3);
    define_type( "CmdLogicalize", 0, 3);
    define_type( "CmdReadSingleton", 0, 2);
    define_type( "CmdReadProperty", 0, 3);
    define_type( "CmdWriteProperty", 0, 4);
    define_type( "CmdNewObject", 0, 4);
    define_type( "CmdDynamicCall", 0, 4);
    define_type( "CmdBox", 0, 3);
    define_type( "CmdStatement", 0, 1);
    define_type( "CmdControlStructure", 0, 1);
    define_type( "CmdElseIf", 0, 1);
    define_type( "CmdIfNonNull", 0, 5);
    define_type( "CmdStatementWithOperand", 0, 2);
    define_type( "CmdPrintln", 0, 2);
    define_type( "CmdThrowMissingReturn", 0, 1);
    define_type( "CmdTron", 0, 1);
    define_type( "CmdTroff", 0, 1);
    define_type( "CmdReturn", 0, 1);
    define_type( "CmdReturnNil", 0, 1);
    define_type( "CmdReturnNull", 0, 1);
    define_type( "CmdReturnThis", 0, 2);
    define_type( "CmdCast", 0, 3);
    define_type( "CmdCastToType", 0, 4);
    define_type( "CmdGeneralizeReference", 0, 4);
    define_type( "CmdSpecializeReference", 0, 4);
    define_type( "CmdCastIntegerToReal", 0, 3);
    define_type( "CmdCastRealToInteger", 0, 3);
    define_type( "CmdTypeManipulation", 0, 3);
    define_type( "CmdInstanceOf", 0, 3);
    define_type( "CmdBinary", 0, 4);
    define_type( "CmdAdd", 0, 4);
    define_type( "CmdSubtract", 0, 4);
    define_type( "CmdMultiply", 0, 4);
    define_type( "CmdDivide", 0, 4);
    define_type( "CmdMod", 0, 4);
    define_type( "CmdPower", 0, 4);
    define_type( "CmdBitwiseAnd", 0, 4);
    define_type( "CmdBitwiseOr", 0, 4);
    define_type( "CmdBitwiseXor", 0, 4);
    define_type( "CmdBitwiseSHL", 0, 4);
    define_type( "CmdBitwiseSHR", 0, 4);
    define_type( "CmdBitwiseSHRX", 0, 4);
    define_type( "CmdLogicalAnd", 0, 4);
    define_type( "CmdLogicalOr", 0, 4);
    define_type( "CmdEitherOr", 0, 5);
    define_type( "CmdLogicalXor", 0, 4);
    define_type( "CmdComparison", 0, 4);
    define_type( "CmdCompareEQ", 0, 4);
    define_type( "CmdCompareNE", 0, 4);
    define_type( "CmdCompareLT", 0, 4);
    define_type( "CmdCompareGT", 0, 4);
    define_type( "CmdCompareLE", 0, 4);
    define_type( "CmdCompareGE", 0, 4);
    define_type( "CmdLocalDeclaration", 0, 2);
    define_type( "CmdAssign", 0, 3);
    define_type( "CmdWriteLocal", 0, 3);
    define_type( "CmdWriteLocalDefault", 0, 2);
    define_type( "CmdReadLocal", 0, 2);
    define_type( "Op", 0, 155);
    define_type( "BackpatchInfo", 0, 2);

    // Base type and property declarations.
    BardType type;
    type = vm.must_find_type( "RealObject" );
    add_base_type( type, "Object" );
    type = vm.must_find_type( "IntegerObject" );
    add_base_type( type, "Object" );
    type = vm.must_find_type( "CharacterObject" );
    add_base_type( type, "Object" );
    type = vm.must_find_type( "LogicalObject" );
    add_base_type( type, "Object" );
    type = vm.must_find_type( "String" );
    add_base_type( type, "Object" );
    type = vm.must_find_type( "Value" );
    add_base_type( type, "Object" );
    type = vm.must_find_type( "Global" );
    add_base_type( type, "Object" );
    type = vm.must_find_type( "Collection" );
    add_base_type( type, "Object" );
    type = vm.must_find_type( "List" );
    add_base_type( type, "Collection" );
    type = vm.must_find_type( "Table" );
    add_base_type( type, "Collection" );
    type = vm.must_find_type( "Main" );
    add_base_type( type, "Object" );
    add_property( type, 0, "ch", "Character" );
    type = vm.must_find_type( "System" );
    add_base_type( type, "Object" );
    type = vm.must_find_type( "Time" );
    add_base_type( type, "Object" );
    type = vm.must_find_type( "Random" );
    add_base_type( type, "Object" );
    type = vm.must_find_type( "Writer" );
    add_base_type( type, "Object" );
    type = vm.must_find_type( "Byte[]" );
    add_base_type( type, "Writer" );
    type = vm.must_find_type( "Character[]" );
    add_base_type( type, "String" );
    add_base_type( type, "Writer" );
    type = vm.must_find_type( "Integer[]" );
    add_base_type( type, "Object" );
    type = vm.must_find_type( "Pair" );
    add_base_type( type, "Object" );
    add_property( type, 0, "a", "Variant" );
    add_property( type, 1, "b", "Variant" );
    type = vm.must_find_type( "File" );
    add_base_type( type, "Object" );
    add_property( type, 0, "filepath", "String" );
    type = vm.must_find_type( "Reader" );
    add_base_type( type, "Object" );
    type = vm.must_find_type( "FileReader" );
    add_base_type( type, "Reader" );
    type = vm.must_find_type( "NativeFileWriter" );
    add_base_type( type, "Object" );
    type = vm.must_find_type( "FileWriter" );
    add_base_type( type, "Writer" );
    add_property( type, 0, "dest", "NativeFileWriter" );
    add_property( type, 1, "buffer", "Character[]" );
    type = vm.must_find_type( "LineReader" );
    add_base_type( type, "Reader" );
    add_property( type, 0, "src", "Reader" );
    add_property( type, 1, "buffer", "Character[]" );
    add_property( type, 2, "prepped", "Logical" );
    type = vm.must_find_type( "StringReader" );
    add_base_type( type, "Reader" );
    add_property( type, 0, "data", "String" );
    add_property( type, 1, "count", "Integer" );
    add_property( type, 2, "position", "Integer" );
    type = vm.must_find_type( "TextWriter" );
    add_base_type( type, "Writer" );
    add_property( type, 0, "dest", "Writer" );
    add_property( type, 1, "buffer", "Character[]" );
    type = vm.must_find_type( "Math" );
    add_base_type( type, "Object" );
    type = vm.must_find_type( "ParseReader" );
    add_base_type( type, "Object" );
    add_property( type, 0, "data", "String" );
    add_property( type, 1, "position", "Integer" );
    add_property( type, 2, "count", "Integer" );
    add_property( type, 3, "line", "Integer" );
    add_property( type, 4, "column", "Integer" );
    type = vm.must_find_type( "TokenType" );
    add_base_type( type, "Object" );
    add_property( type, 0, "eof", "Integer" );
    add_property( type, 1, "eol", "Integer" );
    add_property( type, 2, "symbol_class", "Integer" );
    add_property( type, 3, "symbol_function", "Integer" );
    add_property( type, 4, "symbol_else", "Integer" );
    add_property( type, 5, "symbol_elseIf", "Integer" );
    add_property( type, 6, "symbol_endClass", "Integer" );
    add_property( type, 7, "symbol_endForEach", "Integer" );
    add_property( type, 8, "symbol_endFunction", "Integer" );
    add_property( type, 9, "symbol_endIf", "Integer" );
    add_property( type, 10, "symbol_endLoop", "Integer" );
    add_property( type, 11, "symbol_endObject", "Integer" );
    add_property( type, 12, "symbol_endWhile", "Integer" );
    add_property( type, 13, "symbol_in", "Integer" );
    add_property( type, 14, "symbol_METHODS", "Integer" );
    add_property( type, 15, "symbol_method", "Integer" );
    add_property( type, 16, "symbol_object", "Integer" );
    add_property( type, 17, "symbol_of", "Integer" );
    add_property( type, 18, "symbol_PROPERTIES", "Integer" );
    add_property( type, 19, "symbol_close_brace", "Integer" );
    add_property( type, 20, "symbol_close_bracket", "Integer" );
    add_property( type, 21, "symbol_close_comment", "Integer" );
    add_property( type, 22, "symbol_close_paren", "Integer" );
    add_property( type, 23, "symbol_close_special", "Integer" );
    add_property( type, 24, "symbol_last_non_statement", "Integer" );
    add_property( type, 25, "comment", "Integer" );
    add_property( type, 26, "identifier", "Integer" );
    add_property( type, 27, "symbol_forEach", "Integer" );
    add_property( type, 28, "symbol_if", "Integer" );
    add_property( type, 29, "symbol_loop", "Integer" );
    add_property( type, 30, "symbol_return", "Integer" );
    add_property( type, 31, "symbol_throw", "Integer" );
    add_property( type, 32, "symbol_trace", "Integer" );
    add_property( type, 33, "symbol_tron", "Integer" );
    add_property( type, 34, "symbol_troff", "Integer" );
    add_property( type, 35, "symbol_while", "Integer" );
    add_property( type, 36, "literal_Real", "Integer" );
    add_property( type, 37, "literal_Real_pi", "Integer" );
    add_property( type, 38, "literal_Integer", "Integer" );
    add_property( type, 39, "literal_Character", "Integer" );
    add_property( type, 40, "literal_Logical_true", "Integer" );
    add_property( type, 41, "literal_Logical_false", "Integer" );
    add_property( type, 42, "literal_String", "Integer" );
    add_property( type, 43, "symbol_add_with_assign", "Integer" );
    add_property( type, 44, "symbol_ampersand", "Integer" );
    add_property( type, 45, "symbol_and", "Integer" );
    add_property( type, 46, "symbol_as", "Integer" );
    add_property( type, 47, "symbol_at", "Integer" );
    add_property( type, 48, "symbol_bitwise_and_with_assign", "Integer" );
    add_property( type, 49, "symbol_bitwise_not", "Integer" );
    add_property( type, 50, "symbol_bitwise_or", "Integer" );
    add_property( type, 51, "symbol_bitwise_or_with_assign", "Integer" );
    add_property( type, 52, "symbol_bitwise_xor", "Integer" );
    add_property( type, 53, "symbol_bitwise_xor_with_assign", "Integer" );
    add_property( type, 54, "symbol_colon", "Integer" );
    add_property( type, 55, "symbol_comma", "Integer" );
    add_property( type, 56, "symbol_compare", "Integer" );
    add_property( type, 57, "symbol_decrement", "Integer" );
    add_property( type, 58, "symbol_divide", "Integer" );
    add_property( type, 59, "symbol_divide_with_assign", "Integer" );
    add_property( type, 60, "symbol_dollar", "Integer" );
    add_property( type, 61, "symbol_downtogreaterthan", "Integer" );
    add_property( type, 62, "symbol_empty_braces", "Integer" );
    add_property( type, 63, "symbol_empty_brackets", "Integer" );
    add_property( type, 64, "symbol_eq", "Integer" );
    add_property( type, 65, "symbol_equals", "Integer" );
    add_property( type, 66, "symbol_ge", "Integer" );
    add_property( type, 67, "symbol_global", "Integer" );
    add_property( type, 68, "symbol_gt", "Integer" );
    add_property( type, 69, "symbol_increment", "Integer" );
    add_property( type, 70, "symbol_instanceOf", "Integer" );
    add_property( type, 71, "symbol_is", "Integer" );
    add_property( type, 72, "symbol_le", "Integer" );
    add_property( type, 73, "symbol_lt", "Integer" );
    add_property( type, 74, "symbol_local", "Integer" );
    add_property( type, 75, "symbol_member_with_assign", "Integer" );
    add_property( type, 76, "symbol_minus", "Integer" );
    add_property( type, 77, "symbol_mod_with_assign", "Integer" );
    add_property( type, 78, "symbol_multiply_with_assign", "Integer" );
    add_property( type, 79, "symbol_ne", "Integer" );
    add_property( type, 80, "symbol_noAction", "Integer" );
    add_property( type, 81, "symbol_not", "Integer" );
    add_property( type, 82, "symbol_null", "Integer" );
    add_property( type, 83, "symbol_open_brace", "Integer" );
    add_property( type, 84, "symbol_open_bracket", "Integer" );
    add_property( type, 85, "symbol_open_paren", "Integer" );
    add_property( type, 86, "symbol_open_special", "Integer" );
    add_property( type, 87, "symbol_or", "Integer" );
    add_property( type, 88, "symbol_percent", "Integer" );
    add_property( type, 89, "symbol_period", "Integer" );
    add_property( type, 90, "symbol_plus", "Integer" );
    add_property( type, 91, "symbol_power", "Integer" );
    add_property( type, 92, "symbol_power_with_assign", "Integer" );
    add_property( type, 93, "symbol_question_mark", "Integer" );
    add_property( type, 94, "symbol_semicolon", "Integer" );
    add_property( type, 95, "symbol_shl", "Integer" );
    add_property( type, 96, "symbol_shr", "Integer" );
    add_property( type, 97, "symbol_shrx", "Integer" );
    add_property( type, 98, "symbol_subtract_with_assign", "Integer" );
    add_property( type, 99, "symbol_this", "Integer" );
    add_property( type, 100, "symbol_times", "Integer" );
    add_property( type, 101, "symbol_upto", "Integer" );
    add_property( type, 102, "symbol_uptolessthan", "Integer" );
    add_property( type, 103, "symbol_xor", "Integer" );
    add_property( type, 104, "last_token", "Integer" );
    add_property( type, 105, "name_lookup", "Table" );
    add_property( type, 106, "keywords", "Table" );
    type = vm.must_find_type( "Token" );
    add_base_type( type, "Object" );
    add_property( type, 0, "type", "Integer" );
    add_property( type, 1, "line", "Integer" );
    add_property( type, 2, "column", "Integer" );
    add_property( type, 3, "value", "Real" );
    add_property( type, 4, "filepath", "String" );
    add_property( type, 5, "content", "String" );
    type = vm.must_find_type( "Scanner" );
    add_base_type( type, "Object" );
    add_property( type, 0, "filepath", "String" );
    add_property( type, 1, "reader", "ParseReader" );
    add_property( type, 2, "tokens", "List" );
    add_property( type, 3, "line", "Integer" );
    add_property( type, 4, "column", "Integer" );
    add_property( type, 5, "content", "Character[]" );
    add_property( type, 6, "comment", "Character[]" );
    type = vm.must_find_type( "TokenReader" );
    add_base_type( type, "Object" );
    add_property( type, 0, "tokens", "List" );
    add_property( type, 1, "position", "Integer" );
    add_property( type, 2, "count", "Integer" );
    type = vm.must_find_type( "Parser" );
    add_base_type( type, "Object" );
    add_property( type, 0, "filepath", "String" );
    add_property( type, 1, "reader", "TokenReader" );
    add_property( type, 2, "this_type", "TypeDef" );
    add_property( type, 3, "this_method", "Method" );
    add_property( type, 4, "property_list", "List" );
    add_property( type, 5, "local_declarations", "List" );
    type = vm.must_find_type( "TypeDef" );
    add_base_type( type, "Object" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "name", "String" );
    add_property( type, 2, "qualifiers", "Integer" );
    add_property( type, 3, "index", "Integer" );
    add_property( type, 4, "attributes", "Attributes" );
    add_property( type, 5, "base_types", "List" );
    add_property( type, 6, "methods", "List" );
    add_property( type, 7, "properties", "List" );
    add_property( type, 8, "defined", "Logical" );
    add_property( type, 9, "organized", "Logical" );
    add_property( type, 10, "resolved", "Logical" );
    add_property( type, 11, "unchecked_type", "Type" );
    add_property( type, 12, "checked_type", "Type" );
    type = vm.must_find_type( "Method" );
    add_base_type( type, "Object" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "type_context", "TypeDef" );
    add_property( type, 2, "name", "String" );
    add_property( type, 3, "signature", "String" );
    add_property( type, 4, "qualifiers", "Integer" );
    add_property( type, 5, "return_type", "Type" );
    add_property( type, 6, "method_id", "Integer" );
    add_property( type, 7, "ip", "Integer" );
    add_property( type, 8, "attributes", "Attributes" );
    add_property( type, 9, "body", "CmdStatementList" );
    add_property( type, 10, "code", "Integer[]" );
    add_property( type, 11, "literal_String_values", "List" );
    add_property( type, 12, "literal_Real_values", "List" );
    add_property( type, 13, "parameters", "List" );
    add_property( type, 14, "locals", "List" );
    add_property( type, 15, "organized", "Logical" );
    add_property( type, 16, "resolved", "Logical" );
    type = vm.must_find_type( "Template" );
    add_base_type( type, "Object" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "name", "String" );
    add_property( type, 2, "placeholder_names", "List" );
    add_property( type, 3, "tokens", "List" );
    type = vm.must_find_type( "Attributes" );
    add_base_type( type, "Object" );
    add_property( type, 0, "data", "Table" );
    type = vm.must_find_type( "Type" );
    add_base_type( type, "Object" );
    add_property( type, 0, "def", "TypeDef" );
    add_property( type, 1, "possibly_null", "Logical" );
    type = vm.must_find_type( "CmdStatementList" );
    add_base_type( type, "Cmd" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "statements", "List" );
    type = vm.must_find_type( "CmdIf" );
    add_base_type( type, "CmdControlStructure" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "condition", "Cmd" );
    add_property( type, 2, "body", "CmdStatementList" );
    add_property( type, 3, "else_body", "CmdStatementList" );
    type = vm.must_find_type( "Cmd" );
    add_base_type( type, "Object" );
    add_property( type, 0, "t", "Token" );
    type = vm.must_find_type( "CmdAccess" );
    add_base_type( type, "Cmd" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "operand", "Cmd" );
    add_property( type, 2, "name", "String" );
    add_property( type, 3, "args", "CmdArgs" );
    type = vm.must_find_type( "Templates" );
    add_base_type( type, "Object" );
    add_property( type, 0, "lookup", "Table" );
    type = vm.must_find_type( "Types" );
    add_base_type( type, "Object" );
    type = vm.must_find_type( "TypeDefs" );
    add_base_type( type, "Object" );
    add_property( type, 0, "type_table", "Table" );
    add_property( type, 1, "type_Object", "TypeDef" );
    add_property( type, 2, "type_String", "TypeDef" );
    add_property( type, 3, "type_Real", "TypeDef" );
    add_property( type, 4, "type_Integer", "TypeDef" );
    add_property( type, 5, "type_Character", "TypeDef" );
    add_property( type, 6, "type_Logical", "TypeDef" );
    add_property( type, 7, "type_RealObject", "TypeDef" );
    add_property( type, 8, "type_IntegerObject", "TypeDef" );
    add_property( type, 9, "type_CharacterObject", "TypeDef" );
    add_property( type, 10, "type_LogicalObject", "TypeDef" );
    add_property( type, 11, "type_Variant", "TypeDef" );
    add_property( type, 12, "type_NullObject", "TypeDef" );
    add_property( type, 13, "type_IntegerList", "TypeDef" );
    add_property( type, 14, "type_CharacterList", "TypeDef" );
    add_property( type, 15, "type_ByteList", "TypeDef" );
    add_property( type, 16, "type_Value", "TypeDef" );
    add_property( type, 17, "type_Global", "TypeDef" );
    add_property( type, 18, "type_Collection", "TypeDef" );
    add_property( type, 19, "type_List", "TypeDef" );
    add_property( type, 20, "type_Table", "TypeDef" );
    type = vm.must_find_type( "JavaVMWriter" );
    add_base_type( type, "Object" );
    add_property( type, 0, "writer", "TextWriter" );
    add_property( type, 1, "code_string_char_lookup", "List" );
    add_property( type, 2, "this_type", "Type" );
    add_property( type, 3, "this_method", "Method" );
    add_property( type, 4, "current_control_id", "Integer" );
    add_property( type, 5, "backpatch_locations", "List" );
    add_property( type, 6, "label_definitions", "List" );
    type = vm.must_find_type( "IDTable" );
    add_base_type( type, "Object" );
    add_property( type, 0, "data", "Table" );
    add_property( type, 1, "list", "List" );
    type = vm.must_find_type( "Qualifiers" );
    add_base_type( type, "Object" );
    add_property( type, 0, "is_primitive", "Integer" );
    add_property( type, 1, "is_builtIn", "Integer" );
    add_property( type, 2, "is_native", "Integer" );
    add_property( type, 3, "is_singleton", "Integer" );
    type = vm.must_find_type( "Property" );
    add_base_type( type, "Object" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "type_context", "TypeDef" );
    add_property( type, 2, "type", "Type" );
    add_property( type, 3, "name", "String" );
    add_property( type, 4, "qualifiers", "Integer" );
    add_property( type, 5, "index", "Integer" );
    add_property( type, 6, "initial_value", "Cmd" );
    type = vm.must_find_type( "Local" );
    add_base_type( type, "Object" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "name", "String" );
    add_property( type, 2, "type", "Type" );
    add_property( type, 3, "initial_value", "Cmd" );
    add_property( type, 4, "index", "Integer" );
    type = vm.must_find_type( "Analyzer" );
    add_base_type( type, "Object" );
    add_property( type, 0, "this_type", "TypeDef" );
    add_property( type, 1, "this_method", "Method" );
    add_property( type, 2, "locals", "List" );
    add_property( type, 3, "local_scope_stack", "List" );
    add_property( type, 4, "property_work_list", "List" );
    add_property( type, 5, "candidate_methods", "List" );
    add_property( type, 6, "compatible_methods", "List" );
    add_property( type, 7, "command_work_list", "List" );
    add_property( type, 8, "empty_args", "CmdArgs" );
    add_property( type, 9, "context_stack", "List" );
    type = vm.must_find_type( "CmdArgs" );
    add_base_type( type, "Object" );
    add_property( type, 0, "data", "List" );
    type = vm.must_find_type( "Visitor" );
    add_base_type( type, "Object" );
    type = vm.must_find_type( "ImplicitReturnTypeVisitor" );
    add_base_type( type, "Visitor" );
    add_property( type, 0, "found_return", "Logical" );
    type = vm.must_find_type( "CmdReturnValue" );
    add_base_type( type, "CmdReturn" );
    add_base_type( type, "CmdStatementWithOperand" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "operand", "Cmd" );
    type = vm.must_find_type( "CmdLiteral" );
    add_base_type( type, "Cmd" );
    add_property( type, 0, "t", "Token" );
    type = vm.must_find_type( "CmdLiteralReal" );
    add_base_type( type, "CmdLiteral" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "value", "Real" );
    type = vm.must_find_type( "CmdLiteralInteger" );
    add_base_type( type, "CmdLiteral" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "value", "Integer" );
    type = vm.must_find_type( "CmdLiteralCharacter" );
    add_base_type( type, "CmdLiteral" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "value", "Character" );
    type = vm.must_find_type( "CmdLiteralLogical" );
    add_base_type( type, "CmdLiteral" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "value", "Logical" );
    type = vm.must_find_type( "CmdLiteralString" );
    add_base_type( type, "CmdLiteral" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "value", "String" );
    type = vm.must_find_type( "CmdLiteralNull" );
    add_base_type( type, "CmdLiteral" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "of_type", "Type" );
    type = vm.must_find_type( "CmdUnary" );
    add_base_type( type, "Cmd" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "operand", "Cmd" );
    add_property( type, 2, "opcode", "Integer" );
    type = vm.must_find_type( "CmdLogicalNot" );
    add_base_type( type, "CmdUnary" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "operand", "Cmd" );
    add_property( type, 2, "opcode", "Integer" );
    type = vm.must_find_type( "CmdNegate" );
    add_base_type( type, "CmdUnary" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "operand", "Cmd" );
    add_property( type, 2, "opcode", "Integer" );
    type = vm.must_find_type( "CmdBitwiseNot" );
    add_base_type( type, "CmdUnary" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "operand", "Cmd" );
    add_property( type, 2, "opcode", "Integer" );
    type = vm.must_find_type( "CmdLogicalize" );
    add_base_type( type, "CmdUnary" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "operand", "Cmd" );
    add_property( type, 2, "opcode", "Integer" );
    type = vm.must_find_type( "CmdReadSingleton" );
    add_base_type( type, "Cmd" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "of_type", "Type" );
    type = vm.must_find_type( "CmdReadProperty" );
    add_base_type( type, "Cmd" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "context", "Cmd" );
    add_property( type, 2, "property", "Property" );
    type = vm.must_find_type( "CmdWriteProperty" );
    add_base_type( type, "Cmd" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "context", "Cmd" );
    add_property( type, 2, "property", "Property" );
    add_property( type, 3, "new_value", "Cmd" );
    type = vm.must_find_type( "CmdNewObject" );
    add_base_type( type, "Cmd" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "of_type", "Type" );
    add_property( type, 2, "init_method", "Method" );
    add_property( type, 3, "args", "CmdArgs" );
    type = vm.must_find_type( "CmdDynamicCall" );
    add_base_type( type, "Cmd" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "context", "Cmd" );
    add_property( type, 2, "method_info", "Method" );
    add_property( type, 3, "args", "CmdArgs" );
    type = vm.must_find_type( "CmdBox" );
    add_base_type( type, "CmdUnary" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "operand", "Cmd" );
    add_property( type, 2, "opcode", "Integer" );
    type = vm.must_find_type( "CmdStatement" );
    add_base_type( type, "Cmd" );
    add_property( type, 0, "t", "Token" );
    type = vm.must_find_type( "CmdControlStructure" );
    add_base_type( type, "CmdStatement" );
    add_property( type, 0, "t", "Token" );
    type = vm.must_find_type( "CmdElseIf" );
    add_base_type( type, "Cmd" );
    add_property( type, 0, "t", "Token" );
    type = vm.must_find_type( "CmdIfNonNull" );
    add_base_type( type, "CmdControlStructure" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "condition", "Cmd" );
    add_property( type, 2, "body", "CmdStatementList" );
    add_property( type, 3, "else_body", "CmdStatementList" );
    add_property( type, 4, "checked_local", "Local" );
    type = vm.must_find_type( "CmdStatementWithOperand" );
    add_base_type( type, "CmdStatement" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "operand", "Cmd" );
    type = vm.must_find_type( "CmdPrintln" );
    add_base_type( type, "CmdStatementWithOperand" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "operand", "Cmd" );
    type = vm.must_find_type( "CmdThrowMissingReturn" );
    add_base_type( type, "CmdStatement" );
    add_property( type, 0, "t", "Token" );
    type = vm.must_find_type( "CmdTron" );
    add_base_type( type, "CmdStatement" );
    add_property( type, 0, "t", "Token" );
    type = vm.must_find_type( "CmdTroff" );
    add_base_type( type, "CmdStatement" );
    add_property( type, 0, "t", "Token" );
    type = vm.must_find_type( "CmdReturn" );
    add_base_type( type, "CmdStatement" );
    add_property( type, 0, "t", "Token" );
    type = vm.must_find_type( "CmdReturnNil" );
    add_base_type( type, "CmdReturn" );
    add_property( type, 0, "t", "Token" );
    type = vm.must_find_type( "CmdReturnNull" );
    add_base_type( type, "CmdReturn" );
    add_property( type, 0, "t", "Token" );
    type = vm.must_find_type( "CmdReturnThis" );
    add_base_type( type, "CmdReturn" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "return_type", "Type" );
    type = vm.must_find_type( "CmdCast" );
    add_base_type( type, "CmdUnary" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "operand", "Cmd" );
    add_property( type, 2, "opcode", "Integer" );
    type = vm.must_find_type( "CmdCastToType" );
    add_base_type( type, "CmdCast" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "operand", "Cmd" );
    add_property( type, 2, "opcode", "Integer" );
    add_property( type, 3, "to_type", "Type" );
    type = vm.must_find_type( "CmdGeneralizeReference" );
    add_base_type( type, "CmdCastToType" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "operand", "Cmd" );
    add_property( type, 2, "opcode", "Integer" );
    add_property( type, 3, "to_type", "Type" );
    type = vm.must_find_type( "CmdSpecializeReference" );
    add_base_type( type, "CmdCastToType" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "operand", "Cmd" );
    add_property( type, 2, "opcode", "Integer" );
    add_property( type, 3, "to_type", "Type" );
    type = vm.must_find_type( "CmdCastIntegerToReal" );
    add_base_type( type, "CmdCast" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "operand", "Cmd" );
    add_property( type, 2, "opcode", "Integer" );
    type = vm.must_find_type( "CmdCastRealToInteger" );
    add_base_type( type, "CmdCast" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "operand", "Cmd" );
    add_property( type, 2, "opcode", "Integer" );
    type = vm.must_find_type( "CmdTypeManipulation" );
    add_base_type( type, "Cmd" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "operand", "Cmd" );
    add_property( type, 2, "target_type", "Type" );
    type = vm.must_find_type( "CmdInstanceOf" );
    add_base_type( type, "CmdTypeManipulation" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "operand", "Cmd" );
    add_property( type, 2, "target_type", "Type" );
    type = vm.must_find_type( "CmdBinary" );
    add_base_type( type, "Cmd" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "lhs", "Cmd" );
    add_property( type, 2, "rhs", "Cmd" );
    add_property( type, 3, "opcode", "Integer" );
    type = vm.must_find_type( "CmdAdd" );
    add_base_type( type, "CmdBinary" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "lhs", "Cmd" );
    add_property( type, 2, "rhs", "Cmd" );
    add_property( type, 3, "opcode", "Integer" );
    type = vm.must_find_type( "CmdSubtract" );
    add_base_type( type, "CmdBinary" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "lhs", "Cmd" );
    add_property( type, 2, "rhs", "Cmd" );
    add_property( type, 3, "opcode", "Integer" );
    type = vm.must_find_type( "CmdMultiply" );
    add_base_type( type, "CmdBinary" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "lhs", "Cmd" );
    add_property( type, 2, "rhs", "Cmd" );
    add_property( type, 3, "opcode", "Integer" );
    type = vm.must_find_type( "CmdDivide" );
    add_base_type( type, "CmdBinary" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "lhs", "Cmd" );
    add_property( type, 2, "rhs", "Cmd" );
    add_property( type, 3, "opcode", "Integer" );
    type = vm.must_find_type( "CmdMod" );
    add_base_type( type, "CmdBinary" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "lhs", "Cmd" );
    add_property( type, 2, "rhs", "Cmd" );
    add_property( type, 3, "opcode", "Integer" );
    type = vm.must_find_type( "CmdPower" );
    add_base_type( type, "CmdBinary" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "lhs", "Cmd" );
    add_property( type, 2, "rhs", "Cmd" );
    add_property( type, 3, "opcode", "Integer" );
    type = vm.must_find_type( "CmdBitwiseAnd" );
    add_base_type( type, "CmdBinary" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "lhs", "Cmd" );
    add_property( type, 2, "rhs", "Cmd" );
    add_property( type, 3, "opcode", "Integer" );
    type = vm.must_find_type( "CmdBitwiseOr" );
    add_base_type( type, "CmdBinary" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "lhs", "Cmd" );
    add_property( type, 2, "rhs", "Cmd" );
    add_property( type, 3, "opcode", "Integer" );
    type = vm.must_find_type( "CmdBitwiseXor" );
    add_base_type( type, "CmdBinary" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "lhs", "Cmd" );
    add_property( type, 2, "rhs", "Cmd" );
    add_property( type, 3, "opcode", "Integer" );
    type = vm.must_find_type( "CmdBitwiseSHL" );
    add_base_type( type, "CmdBinary" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "lhs", "Cmd" );
    add_property( type, 2, "rhs", "Cmd" );
    add_property( type, 3, "opcode", "Integer" );
    type = vm.must_find_type( "CmdBitwiseSHR" );
    add_base_type( type, "CmdBinary" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "lhs", "Cmd" );
    add_property( type, 2, "rhs", "Cmd" );
    add_property( type, 3, "opcode", "Integer" );
    type = vm.must_find_type( "CmdBitwiseSHRX" );
    add_base_type( type, "CmdBinary" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "lhs", "Cmd" );
    add_property( type, 2, "rhs", "Cmd" );
    add_property( type, 3, "opcode", "Integer" );
    type = vm.must_find_type( "CmdLogicalAnd" );
    add_base_type( type, "CmdBinary" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "lhs", "Cmd" );
    add_property( type, 2, "rhs", "Cmd" );
    add_property( type, 3, "opcode", "Integer" );
    type = vm.must_find_type( "CmdLogicalOr" );
    add_base_type( type, "CmdBinary" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "lhs", "Cmd" );
    add_property( type, 2, "rhs", "Cmd" );
    add_property( type, 3, "opcode", "Integer" );
    type = vm.must_find_type( "CmdEitherOr" );
    add_base_type( type, "CmdBinary" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "lhs", "Cmd" );
    add_property( type, 2, "rhs", "Cmd" );
    add_property( type, 3, "opcode", "Integer" );
    add_property( type, 4, "common_type", "Type" );
    type = vm.must_find_type( "CmdLogicalXor" );
    add_base_type( type, "CmdBinary" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "lhs", "Cmd" );
    add_property( type, 2, "rhs", "Cmd" );
    add_property( type, 3, "opcode", "Integer" );
    type = vm.must_find_type( "CmdComparison" );
    add_base_type( type, "CmdBinary" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "lhs", "Cmd" );
    add_property( type, 2, "rhs", "Cmd" );
    add_property( type, 3, "opcode", "Integer" );
    type = vm.must_find_type( "CmdCompareEQ" );
    add_base_type( type, "CmdComparison" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "lhs", "Cmd" );
    add_property( type, 2, "rhs", "Cmd" );
    add_property( type, 3, "opcode", "Integer" );
    type = vm.must_find_type( "CmdCompareNE" );
    add_base_type( type, "CmdComparison" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "lhs", "Cmd" );
    add_property( type, 2, "rhs", "Cmd" );
    add_property( type, 3, "opcode", "Integer" );
    type = vm.must_find_type( "CmdCompareLT" );
    add_base_type( type, "CmdComparison" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "lhs", "Cmd" );
    add_property( type, 2, "rhs", "Cmd" );
    add_property( type, 3, "opcode", "Integer" );
    type = vm.must_find_type( "CmdCompareGT" );
    add_base_type( type, "CmdComparison" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "lhs", "Cmd" );
    add_property( type, 2, "rhs", "Cmd" );
    add_property( type, 3, "opcode", "Integer" );
    type = vm.must_find_type( "CmdCompareLE" );
    add_base_type( type, "CmdComparison" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "lhs", "Cmd" );
    add_property( type, 2, "rhs", "Cmd" );
    add_property( type, 3, "opcode", "Integer" );
    type = vm.must_find_type( "CmdCompareGE" );
    add_base_type( type, "CmdComparison" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "lhs", "Cmd" );
    add_property( type, 2, "rhs", "Cmd" );
    add_property( type, 3, "opcode", "Integer" );
    type = vm.must_find_type( "CmdLocalDeclaration" );
    add_base_type( type, "CmdStatement" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "local_info", "Local" );
    type = vm.must_find_type( "CmdAssign" );
    add_base_type( type, "CmdStatementWithOperand" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "operand", "Cmd" );
    add_property( type, 2, "new_value", "Cmd" );
    type = vm.must_find_type( "CmdWriteLocal" );
    add_base_type( type, "CmdStatement" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "v", "Local" );
    add_property( type, 2, "new_value", "Cmd" );
    type = vm.must_find_type( "CmdWriteLocalDefault" );
    add_base_type( type, "CmdStatement" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "v", "Local" );
    type = vm.must_find_type( "CmdReadLocal" );
    add_base_type( type, "Cmd" );
    add_property( type, 0, "t", "Token" );
    add_property( type, 1, "v", "Local" );
    type = vm.must_find_type( "Op" );
    add_base_type( type, "Object" );
    add_property( type, 0, "halt", "Integer" );
    add_property( type, 1, "jump", "Integer" );
    add_property( type, 2, "jump_if_false", "Integer" );
    add_property( type, 3, "jump_if_true", "Integer" );
    add_property( type, 4, "if_false_push_false_and_jump", "Integer" );
    add_property( type, 5, "if_true_push_true_and_jump", "Integer" );
    add_property( type, 6, "jump_if_null", "Integer" );
    add_property( type, 7, "jump_if_not_null", "Integer" );
    add_property( type, 8, "pop_discard", "Integer" );
    add_property( type, 9, "return_nil", "Integer" );
    add_property( type, 10, "return_Variant", "Integer" );
    add_property( type, 11, "return_Object", "Integer" );
    add_property( type, 12, "return_Real", "Integer" );
    add_property( type, 13, "return_Integer", "Integer" );
    add_property( type, 14, "return_Native", "Integer" );
    add_property( type, 15, "throw_Exception", "Integer" );
    add_property( type, 16, "println_nil", "Integer" );
    add_property( type, 17, "println_Variant", "Integer" );
    add_property( type, 18, "finite_loop", "Integer" );
    add_property( type, 19, "unbox_Real", "Integer" );
    add_property( type, 20, "unbox_Integer", "Integer" );
    add_property( type, 21, "unbox_Character", "Integer" );
    add_property( type, 22, "unbox_Logical", "Integer" );
    add_property( type, 23, "box_Real", "Integer" );
    add_property( type, 24, "box_Integer", "Integer" );
    add_property( type, 25, "box_Character", "Integer" );
    add_property( type, 26, "box_Logical", "Integer" );
    add_property( type, 27, "type_check", "Integer" );
    add_property( type, 28, "Variant_instanceOf", "Integer" );
    add_property( type, 29, "Fixed_instanceOf", "Integer" );
    add_property( type, 30, "cast_Object_to_Variant", "Integer" );
    add_property( type, 31, "cast_Real_to_Variant", "Integer" );
    add_property( type, 32, "cast_Integer_to_Variant", "Integer" );
    add_property( type, 33, "cast_Character_to_Variant", "Integer" );
    add_property( type, 34, "cast_Logical_to_Variant", "Integer" );
    add_property( type, 35, "cast_Variant_to_Object", "Integer" );
    add_property( type, 36, "cast_Variant_to_Real", "Integer" );
    add_property( type, 37, "cast_Variant_to_Integer", "Integer" );
    add_property( type, 38, "cast_Variant_to_Character", "Integer" );
    add_property( type, 39, "cast_Integer_to_Real", "Integer" );
    add_property( type, 40, "cast_Real_to_Integer", "Integer" );
    add_property( type, 41, "cast_Character_to_Integer", "Integer" );
    add_property( type, 42, "push_Object_this", "Integer" );
    add_property( type, 43, "push_literal_Object_null", "Integer" );
    add_property( type, 44, "push_literal_String", "Integer" );
    add_property( type, 45, "push_literal_Native_null", "Integer" );
    add_property( type, 46, "push_literal_Real", "Integer" );
    add_property( type, 47, "push_literal_Real_negative_1", "Integer" );
    add_property( type, 48, "push_literal_Real_0", "Integer" );
    add_property( type, 49, "push_literal_Real_1", "Integer" );
    add_property( type, 50, "push_literal_Integer", "Integer" );
    add_property( type, 51, "push_literal_Integer_negative_1", "Integer" );
    add_property( type, 52, "push_literal_Integer_0", "Integer" );
    add_property( type, 53, "push_literal_Integer_1", "Integer" );
    add_property( type, 54, "negate_Real", "Integer" );
    add_property( type, 55, "negate_Integer", "Integer" );
    add_property( type, 56, "negate_Logical", "Integer" );
    add_property( type, 57, "logicalize_Object", "Integer" );
    add_property( type, 58, "logicalize_Real", "Integer" );
    add_property( type, 59, "logicalize_Integer", "Integer" );
    add_property( type, 60, "eq_Object", "Integer" );
    add_property( type, 61, "eq_Real", "Integer" );
    add_property( type, 62, "eq_Integer", "Integer" );
    add_property( type, 63, "ne_Object", "Integer" );
    add_property( type, 64, "ne_Real", "Integer" );
    add_property( type, 65, "ne_Integer", "Integer" );
    add_property( type, 66, "le_Real", "Integer" );
    add_property( type, 67, "le_Integer", "Integer" );
    add_property( type, 68, "ge_Real", "Integer" );
    add_property( type, 69, "ge_Integer", "Integer" );
    add_property( type, 70, "lt_Real", "Integer" );
    add_property( type, 71, "lt_Integer", "Integer" );
    add_property( type, 72, "gt_Real", "Integer" );
    add_property( type, 73, "gt_Integer", "Integer" );
    add_property( type, 74, "is_Variant", "Integer" );
    add_property( type, 75, "is_Object", "Integer" );
    add_property( type, 76, "add_Object", "Integer" );
    add_property( type, 77, "add_Real", "Integer" );
    add_property( type, 78, "add_Integer", "Integer" );
    add_property( type, 79, "subtract_Real", "Integer" );
    add_property( type, 80, "subtract_Integer", "Integer" );
    add_property( type, 81, "multiply_Real", "Integer" );
    add_property( type, 82, "multiply_Integer", "Integer" );
    add_property( type, 83, "divide_Real", "Integer" );
    add_property( type, 84, "mod_Real", "Integer" );
    add_property( type, 85, "mod_Integer", "Integer" );
    add_property( type, 86, "power_Real", "Integer" );
    add_property( type, 87, "power_Integer", "Integer" );
    add_property( type, 88, "bitwise_and_Integer", "Integer" );
    add_property( type, 89, "bitwise_or_Integer", "Integer" );
    add_property( type, 90, "bitwise_xor_Integer", "Integer" );
    add_property( type, 91, "shl_Integer", "Integer" );
    add_property( type, 92, "shr_Integer", "Integer" );
    add_property( type, 93, "shrx_Integer", "Integer" );
    add_property( type, 94, "logical_not", "Integer" );
    add_property( type, 95, "read_local_Variant", "Integer" );
    add_property( type, 96, "read_local_Object", "Integer" );
    add_property( type, 97, "read_local_Real", "Integer" );
    add_property( type, 98, "read_local_Integer", "Integer" );
    add_property( type, 99, "read_local_Native", "Integer" );
    add_property( type, 100, "write_local_Variant", "Integer" );
    add_property( type, 101, "write_local_Object", "Integer" );
    add_property( type, 102, "write_local_Real", "Integer" );
    add_property( type, 103, "write_local_Integer", "Integer" );
    add_property( type, 104, "write_local_Native", "Integer" );
    add_property( type, 105, "increment_local_Object", "Integer" );
    add_property( type, 106, "increment_local_Real", "Integer" );
    add_property( type, 107, "increment_local_Integer", "Integer" );
    add_property( type, 108, "decrement_local_Object", "Integer" );
    add_property( type, 109, "decrement_local_Real", "Integer" );
    add_property( type, 110, "decrement_local_Integer", "Integer" );
    add_property( type, 111, "read_property_Object", "Integer" );
    add_property( type, 112, "read_property_Real", "Integer" );
    add_property( type, 113, "read_property_Integer", "Integer" );
    add_property( type, 114, "read_property_Character", "Integer" );
    add_property( type, 115, "read_property_Logical", "Integer" );
    add_property( type, 116, "read_property_Native", "Integer" );
    add_property( type, 117, "write_property_Object", "Integer" );
    add_property( type, 118, "write_property_Real", "Integer" );
    add_property( type, 119, "write_property_Integer", "Integer" );
    add_property( type, 120, "write_property_Character", "Integer" );
    add_property( type, 121, "write_property_Logical", "Integer" );
    add_property( type, 122, "write_property_Native", "Integer" );
    add_property( type, 123, "read_this_property_Object", "Integer" );
    add_property( type, 124, "read_this_property_Real", "Integer" );
    add_property( type, 125, "read_this_property_Integer", "Integer" );
    add_property( type, 126, "write_this_property_Object", "Integer" );
    add_property( type, 127, "write_this_property_Real", "Integer" );
    add_property( type, 128, "write_this_property_Integer", "Integer" );
    add_property( type, 129, "create_object", "Integer" );
    add_property( type, 130, "read_singleton", "Integer" );
    add_property( type, 131, "dynamic_call", "Integer" );
    add_property( type, 132, "List_create", "Integer" );
    add_property( type, 133, "List_count", "Integer" );
    add_property( type, 134, "List_get", "Integer" );
    add_property( type, 135, "List_set", "Integer" );
    add_property( type, 136, "List_add", "Integer" );
    add_property( type, 137, "List_insert", "Integer" );
    add_property( type, 138, "List_remove_at", "Integer" );
    add_property( type, 139, "List_clear", "Integer" );
    add_property( type, 140, "Table_create", "Integer" );
    add_property( type, 141, "Table_count", "Integer" );
    add_property( type, 142, "Table_set", "Integer" );
    add_property( type, 143, "Table_get_key", "Integer" );
    add_property( type, 144, "trace_on", "Integer" );
    add_property( type, 145, "trace_off", "Integer" );
    add_property( type, 146, "print_Object", "Integer" );
    add_property( type, 147, "print_String", "Integer" );
    add_property( type, 148, "print_Real", "Integer" );
    add_property( type, 149, "print_Integer", "Integer" );
    add_property( type, 150, "print_Character", "Integer" );
    add_property( type, 151, "print_Logical", "Integer" );
    add_property( type, 152, "throw_missing_return", "Integer" );
    add_property( type, 153, "bitwise_not_Integer", "Integer" );
    add_property( type, 154, "last_op", "Integer" );
    type = vm.must_find_type( "BackpatchInfo" );
    add_base_type( type, "Object" );
    add_property( type, 0, "position", "Integer" );
    add_property( type, 1, "label", "String" );
  }

  public void define_methods()
  {
    BardType type_String = vm.must_find_type("String");
    BardMethod m;
    m = add_method( "Object", "init_object", 0, 0, 0,"Object", code, -1 );
    m = add_method( "Object", "init", 0, 0, 0,"Object", code, 3 );
    m = add_method( "Object", "String", 4, 0, 0,"String", code, 7 );
    m = add_method( "Object", "Character", 0, 0, 0,"Character", code, 9 );
    m = add_method( "Object", "Real", 0, 0, 0,"Real", code, 14 );
    m.Real_literals = to_double_array( new long[]{ 0x0L } );
    m = add_method( "Object", "Integer", 0, 0, 0,"Integer", code, 19 );
    m = add_method( "Object", "Logical", 0, 0, 0,"Logical", code, 24 );
    m = add_method( "Object", "get", 0, 1, 1,"Variant", code, 28 );
    add_parameter( m, 0, "v", "Variant" );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "ERROR - get() not defined for this object (method likely called on Variant instead of object type in a forEach loop)." );
    m = add_method( "Object", "set", 0, 2, 2,"Object", code, 33 );
    add_parameter( m, 0, "key", "Variant" );
    add_parameter( m, 1, "value", "Variant" );
    m.String_literals = new BardString[3];    m.String_literals[0] = new BardString( type_String, "setting " );
    m.String_literals[1] = new BardString( type_String, " to " );
    m.String_literals[2] = new BardString( type_String, "ERROR - set() not defined for this object." );
    m = add_method( "Object", "Collection", 0, 0, 0,"Collection", code, 91 );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "ERROR - object is not a Collection" );
    m = add_method( "Object", "List", 0, 0, 0,"List", code, 96 );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "ERROR - object is not a list" );
    m = add_method( "Object", "Table", 0, 0, 0,"Table", code, 101 );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "ERROR - object is not a table" );
    m = add_method( "Object", "key", 0, 1, 1,"Variant", code, 106 );
    add_parameter( m, 0, "index", "Integer" );
    m = add_method( "Object", "keys", 0, 0, 0,"List", code, 111 );
    m = add_method( "Variant", "init_object", 0, 0, 0,"Variant", code, 116 );
    m = add_method( "Real", "init_object", 0, 0, 0,"Real", code, 120 );
    m = add_method( "Integer", "init_object", 0, 0, 0,"Integer", code, 124 );
    m = add_method( "Character", "init_object", 0, 0, 0,"Character", code, 128 );
    m = add_method( "Logical", "init_object", 0, 0, 0,"Logical", code, 132 );
    m = add_method( "null", "init_object", 0, 0, 0,"null", code, 136 );
    m = add_method( "RealObject", "init_object", 0, 0, 0,"RealObject", code, 140 );
    m = add_method( "IntegerObject", "init_object", 0, 0, 0,"IntegerObject", code, 144 );
    m = add_method( "CharacterObject", "init_object", 0, 0, 0,"CharacterObject", code, 148 );
    m = add_method( "LogicalObject", "init_object", 0, 0, 0,"LogicalObject", code, 152 );
    m = add_method( "String", "init_object", 0, 0, 0,"String", code, 156 );
    m = add_method( "String", "String", 0, 0, 0,"String", code, 160 );
    m = add_method( "String", "duplicate", 0, 0, 0,"String", code, 164 );
    m = add_method( "String", "substring", 4, 2, 2,"String", code, 168 );
    add_parameter( m, 0, "i1", "Integer" );
    add_parameter( m, 1, "i2", "Integer" );
    m = add_method( "String", "substring", 0, 1, 1,"String", code, 170 );
    add_parameter( m, 0, "i1", "Integer" );
    m = add_method( "String", "index_of", 0, 1, 2,"Integer", code, 186 );
    add_parameter( m, 0, "look_for", "Character" );
    add_local( m, 1, "i", "Integer" );
    m = add_method( "String", "contains", 0, 1, 1,"Logical", code, 225 );
    add_parameter( m, 0, "ch", "Character" );
    m = add_method( "String", "before", 0, 1, 1,"String", code, 238 );
    add_parameter( m, 0, "index", "Integer" );
    m = add_method( "String", "after", 0, 1, 1,"String", code, 252 );
    add_parameter( m, 0, "index", "Integer" );
    m = add_method( "String", "count", 4, 0, 0,"Integer", code, 271 );
    m = add_method( "String", "get", 4, 1, 1,"Character", code, 273 );
    add_parameter( m, 0, "index", "Integer" );
    m = add_method( "String", "compare_to", 4, 1, 1,"Integer", code, 275 );
    add_parameter( m, 0, "other", "String" );
    m = add_method( "String", "Reader", 0, 0, 0,"Reader", code, 277 );
    m = add_method( "Value", "init_object", 0, 0, 0,"Value", code, 286 );
    m = add_method( "Global", "init_object", 0, 0, 0,"Global", code, 290 );
    m = add_method( "Collection", "init_object", 0, 0, 0,"Collection", code, 294 );
    m = add_method( "Collection", "Collection", 0, 0, 0,"Collection", code, 298 );
    m = add_method( "Collection", "count", 0, 0, 0,"Integer", code, 302 );
    m = add_method( "List", "init_object", 0, 0, 0,"List", code, 304 );
    m = add_method( "List", "String", 0, 0, 5,"String", code, 308 );
    add_local( m, 0, "buffer", "Character[]" );
    add_local( m, 1, "first", "Logical" );
    add_local( m, 2, "collection:2", "List" );
    add_local( m, 3, "counter:2", "Integer" );
    add_local( m, 4, "element", "Variant" );
    m = add_method( "List", "get", 0, 1, 1,"Variant", code, 398 );
    add_parameter( m, 0, "v", "Variant" );
    m = add_method( "List", "count", 2, 0, 0,"Integer", code, 418 );
    m = add_method( "List", "clear", 2, 0, 0,null, code, 423 );
    m = add_method( "List", "add", 2, 1, 1,"List", code, 428 );
    add_parameter( m, 0, "value", "Variant" );
    m = add_method( "List", "add_all", 0, 1, 4,"List", code, 430 );
    add_parameter( m, 0, "other", "List" );
    add_local( m, 1, "collection:1", "List" );
    add_local( m, 2, "counter:1", "Integer" );
    add_local( m, 3, "value", "Variant" );
    m = add_method( "List", "get", 2, 1, 1,"Variant", code, 466 );
    add_parameter( m, 0, "index", "Integer" );
    m = add_method( "List", "remove", 4, 1, 1,"Logical", code, 473 );
    add_parameter( m, 0, "value", "Variant" );
    m = add_method( "List", "remove_at", 2, 1, 1,"Variant", code, 475 );
    add_parameter( m, 0, "index", "Integer" );
    m = add_method( "List", "insert", 2, 1, 1,"List", code, 477 );
    add_parameter( m, 0, "obj", "Variant" );
    m = add_method( "List", "insert", 2, 2, 2,"List", code, 479 );
    add_parameter( m, 0, "obj", "Variant" );
    add_parameter( m, 1, "before_index", "Integer" );
    m = add_method( "List", "swap", 0, 2, 3,"List", code, 481 );
    add_parameter( m, 0, "i1", "Integer" );
    add_parameter( m, 1, "i2", "Integer" );
    add_local( m, 2, "temp", "Variant" );
    m = add_method( "List", "reverse", 0, 0, 3,"List", code, 505 );
    add_local( m, 0, "i1", "Integer" );
    add_local( m, 1, "i2", "Integer" );
    add_local( m, 2, "temp", "Variant" );
    m = add_method( "List", "first", 0, 0, 0,"Variant", code, 553 );
    m = add_method( "List", "last", 0, 0, 0,"Variant", code, 560 );
    m = add_method( "List", "remove_first", 0, 0, 0,"Variant", code, 570 );
    m = add_method( "List", "remove_last", 0, 0, 0,"Variant", code, 577 );
    m = add_method( "List", "sort", 0, 0, 5,"List", code, 587 );
    add_local( m, 0, "pass", "Integer" );
    add_local( m, 1, "sorted", "Logical" );
    add_local( m, 2, "i", "Integer" );
    add_local( m, 3, "a", "Real" );
    add_local( m, 4, "b", "Real" );
    m = add_method( "Table", "init_object", 0, 0, 0,"Table", code, 678 );
    m = add_method( "Table", "get", 2, 1, 1,"Variant", code, 682 );
    add_parameter( m, 0, "key", "Variant" );
    m = add_method( "Table", "set", 2, 2, 2,"Table", code, 689 );
    add_parameter( m, 0, "key", "Variant" );
    add_parameter( m, 1, "value", "Variant" );
    m = add_method( "Table", "key", 4, 1, 1,"Variant", code, 698 );
    add_parameter( m, 0, "index", "Integer" );
    m = add_method( "Table", "keys", 4, 0, 0,"List", code, 700 );
    m = add_method( "Table", "count", 2, 0, 0,"Integer", code, 702 );
    m = add_method( "Table", "at", 4, 1, 1,"Variant", code, 707 );
    add_parameter( m, 0, "index", "Integer" );
    m = add_method( "Table", "contains", 4, 1, 1,"Logical", code, 709 );
    add_parameter( m, 0, "key", "Variant" );
    m = add_method( "Main", "init_object", 0, 0, 0,"Main", code, 711 );
    m = add_method( "Main", "init", 0, 0, 1,"Main", code, 715 );
    add_local( m, 0, "writer", "JavaVMWriter" );
    m.String_literals = new BardString[2];    m.String_literals[0] = new BardString( type_String, "Test.bard" );
    m.String_literals[1] = new BardString( type_String, "TestProgram" );
    m = add_method( "System", "init_object", 0, 0, 0,"System", code, 752 );
    m = add_method( "System", "Real_to_Integer_high_bits", 4, 1, 1,"Integer", code, 756 );
    add_parameter( m, 0, "n", "Real" );
    m = add_method( "System", "Real_to_Integer_low_bits", 4, 1, 1,"Integer", code, 761 );
    add_parameter( m, 0, "n", "Real" );
    m = add_method( "System", "sleep", 4, 1, 1,null, code, 766 );
    add_parameter( m, 0, "seconds", "Real" );
    m = add_method( "Time", "init_object", 0, 0, 0,"Time", code, 768 );
    m = add_method( "Time", "current", 4, 0, 0,"Real", code, 772 );
    m = add_method( "Random", "init_object", 0, 0, 0,"Random", code, 774 );
    m = add_method( "Random", "init", 0, 0, 0,"Random", code, 778 );
    m = add_method( "Random", "String", 0, 0, 0,"String", code, 797 );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "Random Number Generator" );
    m = add_method( "Random", "Real", 0, 0, 0,"Real", code, 802 );
    m.Real_literals = to_double_array( new long[]{ 0x4000000000000000L,0x4037000000000000L,0x4000000000000000L } );
    m = add_method( "Random", "Integer", 0, 0, 0,"Integer", code, 841 );
    m = add_method( "Random", "Logical", 0, 0, 0,"Logical", code, 850 );
    m = add_method( "Random", "init", 0, 1, 1,"Random", code, 862 );
    add_parameter( m, 0, "seed_value", "Integer" );
    m = add_method( "Random", "seed", 4, 1, 1,null, code, 872 );
    add_parameter( m, 0, "value", "Integer" );
    m = add_method( "Random", "bits", 4, 1, 1,"Integer", code, 874 );
    add_parameter( m, 0, "n", "Integer" );
    m = add_method( "Random", "Real", 0, 2, 2,"Real", code, 876 );
    add_parameter( m, 0, "low", "Real" );
    add_parameter( m, 1, "high", "Real" );
    m = add_method( "Random", "Integer", 0, 1, 1,"Integer", code, 892 );
    add_parameter( m, 0, "limit", "Integer" );
    m = add_method( "Random", "Integer", 0, 2, 2,"Integer", code, 904 );
    add_parameter( m, 0, "low", "Integer" );
    add_parameter( m, 1, "high", "Integer" );
    m = add_method( "Writer", "init_object", 0, 0, 0,"Writer", code, 925 );
    m = add_method( "Writer", "write", 0, 1, 1,null, code, 929 );
    add_parameter( m, 0, "value", "Variant" );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "Call to undefined write()" );
    m = add_method( "Writer", "print", 0, 1, 1,"Writer", code, 935 );
    add_parameter( m, 0, "value", "Variant" );
    m = add_method( "Writer", "println", 0, 0, 0,"Writer", code, 946 );
    m = add_method( "Writer", "println", 0, 1, 1,"Writer", code, 958 );
    add_parameter( m, 0, "value", "Variant" );
    m = add_method( "Writer", "close", 0, 0, 0,null, code, 977 );
    m = add_method( "Byte[]", "init_object", 0, 0, 0,"Byte[]", code, 979 );
    m = add_method( "Byte[]", "write", 0, 1, 1,"Byte[]", code, 983 );
    add_parameter( m, 0, "value", "Variant" );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "Byte" );
    m = add_method( "Byte[]", "clear", 4, 0, 0,"Byte[]", code, 1002 );
    m = add_method( "Byte[]", "count", 4, 0, 0,"Integer", code, 1004 );
    m = add_method( "Byte[]", "add", 4, 1, 1,"Byte[]", code, 1006 );
    add_parameter( m, 0, "n", "Integer" );
    m = add_method( "Byte[]", "get", 4, 1, 1,"Integer", code, 1008 );
    add_parameter( m, 0, "index", "Integer" );
    m = add_method( "Byte[]", "Writer", 0, 0, 0,"Writer", code, 1010 );
    m = add_method( "Character[]", "init_object", 0, 0, 0,"Character[]", code, 1014 );
    m = add_method( "Character[]", "String", 0, 0, 0,"String", code, 1018 );
    m = add_method( "Character[]", "duplicate", 0, 0, 1,"Character[]", code, 1022 );
    add_local( m, 0, "result", "Character[]" );
    m = add_method( "Character[]", "write", 0, 1, 1,"Character[]", code, 1053 );
    add_parameter( m, 0, "value", "Variant" );
    m = add_method( "Character[]", "print", 0, 1, 1,"Character[]", code, 1062 );
    add_parameter( m, 0, "value", "Variant" );
    m = add_method( "Character[]", "ensure_capacity", 4, 1, 1,null, code, 1071 );
    add_parameter( m, 0, "n", "Integer" );
    m = add_method( "Character[]", "clear", 4, 0, 0,"Character[]", code, 1073 );
    m = add_method( "Character[]", "add", 4, 1, 1,"Character[]", code, 1075 );
    add_parameter( m, 0, "value", "Variant" );
    m = add_method( "Character[]", "LineReader", 0, 0, 0,"LineReader", code, 1077 );
    m = add_method( "Character[]", "Writer", 0, 0, 0,"Writer", code, 1089 );
    m = add_method( "Integer[]", "init_object", 0, 0, 0,"Integer[]", code, 1098 );
    m = add_method( "Integer[]", "String", 0, 0, 5,"String", code, 1102 );
    add_local( m, 0, "buffer", "Character[]" );
    add_local( m, 1, "first", "Logical" );
    add_local( m, 2, "collection:2", "Integer[]" );
    add_local( m, 3, "counter:2", "Integer" );
    add_local( m, 4, "value", "Integer" );
    m = add_method( "Integer[]", "count", 4, 0, 0,"Integer", code, 1194 );
    m = add_method( "Integer[]", "get", 4, 1, 1,"Integer", code, 1196 );
    add_parameter( m, 0, "index", "Integer" );
    m = add_method( "Integer[]", "set", 4, 2, 2,"Integer[]", code, 1198 );
    add_parameter( m, 0, "index", "Integer" );
    add_parameter( m, 1, "value", "Integer" );
    m = add_method( "Integer[]", "ensure_capacity", 4, 1, 1,"Integer[]", code, 1200 );
    add_parameter( m, 0, "n", "Integer" );
    m = add_method( "Integer[]", "clear", 4, 0, 0,"Integer[]", code, 1202 );
    m = add_method( "Integer[]", "add", 4, 1, 1,"Integer[]", code, 1204 );
    add_parameter( m, 0, "value", "Integer" );
    m = add_method( "Integer[]", "add", 0, 1, 4,"Integer[]", code, 1206 );
    add_parameter( m, 0, "list", "Integer[]" );
    add_local( m, 1, "collection:1", "Integer[]" );
    add_local( m, 2, "counter:1", "Integer" );
    add_local( m, 3, "value", "Integer" );
    m = add_method( "Integer[]", "duplicate", 0, 0, 1,"Integer[]", code, 1246 );
    add_local( m, 0, "result", "Integer[]" );
    m = add_method( "Integer[]", "first", 0, 0, 0,"Integer", code, 1277 );
    m = add_method( "Integer[]", "last", 0, 0, 0,"Integer", code, 1286 );
    m = add_method( "Pair", "init_object", 0, 0, 0,"Pair", code, 1300 );
    m = add_method( "Pair", "String", 0, 0, 0,"String", code, 1304 );
    m = add_method( "Pair", "init", 0, 2, 2,"Pair", code, 1332 );
    add_parameter( m, 0, "_a", "Variant" );
    add_parameter( m, 1, "_b", "Variant" );
    m = add_method( "File", "init_object", 0, 0, 0,"File", code, 1346 );
    m = add_method( "File", "init", 0, 1, 1,"File", code, 1350 );
    add_parameter( m, 0, "_filepath", "String" );
    m = add_method( "File", "LineReader", 0, 0, 0,"LineReader", code, 1359 );
    m = add_method( "File", "Reader", 0, 0, 0,"Reader", code, 1371 );
    m = add_method( "File", "Writer", 0, 0, 0,"Writer", code, 1382 );
    m = add_method( "File", "Character[]", 4, 0, 0,"Character[]", code, 1398 );
    m = add_method( "Reader", "init_object", 0, 0, 0,"Reader", code, 1400 );
    m = add_method( "Reader", "available", 0, 0, 0,"Integer", code, 1404 );
    m = add_method( "Reader", "has_another", 0, 0, 0,"Logical", code, 1406 );
    m = add_method( "Reader", "read", 0, 0, 0,"Variant", code, 1408 );
    m = add_method( "Reader", "peek", 0, 0, 0,"Variant", code, 1410 );
    m = add_method( "Reader", "close", 0, 0, 0,null, code, 1412 );
    m = add_method( "FileReader", "init_object", 0, 0, 0,"FileReader", code, 1414 );
    m = add_method( "FileReader", "available", 4, 0, 0,"Integer", code, 1418 );
    m = add_method( "FileReader", "has_another", 4, 0, 0,"Logical", code, 1420 );
    m = add_method( "FileReader", "read", 4, 0, 0,"Variant", code, 1422 );
    m = add_method( "FileReader", "peek", 4, 0, 0,"Variant", code, 1424 );
    m = add_method( "FileReader", "close", 4, 0, 0,null, code, 1426 );
    m = add_method( "FileReader", "init", 4, 1, 1,"FileReader", code, 1428 );
    add_parameter( m, 0, "filepath", "String" );
    m = add_method( "NativeFileWriter", "init_object", 0, 0, 0,"NativeFileWriter", code, 1432 );
    m = add_method( "NativeFileWriter", "init", 4, 1, 1,"NativeFileWriter", code, 1436 );
    add_parameter( m, 0, "filepath", "String" );
    m = add_method( "NativeFileWriter", "ready", 4, 0, 0,"Logical", code, 1440 );
    m = add_method( "NativeFileWriter", "write", 4, 1, 1,null, code, 1442 );
    add_parameter( m, 0, "ch", "Character" );
    m = add_method( "NativeFileWriter", "close", 4, 0, 0,null, code, 1444 );
    m = add_method( "FileWriter", "init_object", 0, 0, 0,"FileWriter", code, 1446 );
    m = add_method( "FileWriter", "write", 0, 1, 4,null, code, 1450 );
    add_parameter( m, 0, "value", "Variant" );
    add_local( m, 1, "collection:1", "Character[]" );
    add_local( m, 2, "counter:1", "Integer" );
    add_local( m, 3, "ch", "Character" );
    m = add_method( "FileWriter", "close", 0, 0, 0,null, code, 1540 );
    m = add_method( "FileWriter", "init", 0, 1, 1,"FileWriter", code, 1548 );
    add_parameter( m, 0, "filepath", "String" );
    m = add_method( "FileWriter", "ready", 0, 0, 0,"Logical", code, 1562 );
    m = add_method( "LineReader", "init_object", 0, 0, 0,"LineReader", code, 1571 );
    m = add_method( "LineReader", "available", 0, 0, 0,"Integer", code, 1575 );
    m = add_method( "LineReader", "has_another", 0, 0, 0,"Logical", code, 1591 );
    m = add_method( "LineReader", "read", 0, 0, 1,"Variant", code, 1605 );
    add_local( m, 0, "ch", "Character" );
    m = add_method( "LineReader", "peek", 0, 0, 0,"Variant", code, 1678 );
    m = add_method( "LineReader", "init", 0, 1, 1,"LineReader", code, 1702 );
    add_parameter( m, 0, "_src", "Reader" );
    m = add_method( "StringReader", "init_object", 0, 0, 0,"StringReader", code, 1725 );
    m = add_method( "StringReader", "available", 0, 0, 0,"Integer", code, 1729 );
    m = add_method( "StringReader", "has_another", 0, 0, 0,"Logical", code, 1739 );
    m = add_method( "StringReader", "read", 0, 0, 0,"Variant", code, 1749 );
    m = add_method( "StringReader", "peek", 0, 0, 0,"Variant", code, 1774 );
    m = add_method( "StringReader", "init", 0, 1, 1,"StringReader", code, 1802 );
    add_parameter( m, 0, "_data", "String" );
    m = add_method( "StringReader", "peek", 0, 1, 2,"Character", code, 1825 );
    add_parameter( m, 0, "offset", "Integer" );
    add_local( m, 1, "zero_char", "Character" );
    m = add_method( "StringReader", "consume", 0, 1, 1,"Logical", code, 1861 );
    add_parameter( m, 0, "ch", "Character" );
    m = add_method( "StringReader", "consume_whitespace", 0, 0, 0,"Logical", code, 1884 );
    m = add_method( "StringReader", "consume", 0, 1, 3,"Logical", code, 1926 );
    add_parameter( m, 0, "st", "String" );
    add_local( m, 1, "collection:1", "String" );
    add_local( m, 2, "i", "Integer" );
    m = add_method( "TextWriter", "init_object", 0, 0, 0,"TextWriter", code, 1984 );
    m = add_method( "TextWriter", "init", 0, 0, 0,"TextWriter", code, 1988 );
    m = add_method( "TextWriter", "String", 0, 0, 0,"String", code, 2012 );
    m = add_method( "TextWriter", "write", 0, 1, 1,null, code, 2021 );
    add_parameter( m, 0, "value", "Variant" );
    m = add_method( "TextWriter", "print", 0, 1, 4,"TextWriter", code, 2030 );
    add_parameter( m, 0, "value", "Variant" );
    add_local( m, 1, "collection:1", "Character[]" );
    add_local( m, 2, "counter:1", "Integer" );
    add_local( m, 3, "ch", "Character" );
    m = add_method( "TextWriter", "println", 0, 1, 1,"TextWriter", code, 2087 );
    add_parameter( m, 0, "value", "Variant" );
    m = add_method( "TextWriter", "init", 0, 1, 1,"TextWriter", code, 2106 );
    add_parameter( m, 0, "_dest", "Writer" );
    m = add_method( "Math", "init_object", 0, 0, 0,"Math", code, 2125 );
    m = add_method( "Math", "floor", 4, 1, 1,"Real", code, 2129 );
    add_parameter( m, 0, "n", "Real" );
    m = add_method( "ParseReader", "init_object", 0, 0, 0,"ParseReader", code, 2131 );
    m = add_method( "ParseReader", "init", 0, 1, 4,"ParseReader", code, 2135 );
    add_parameter( m, 0, "filename", "String" );
    add_local( m, 1, "reader", "Reader" );
    add_local( m, 2, "buffer", "Character[]" );
    add_local( m, 3, "ch", "Integer" );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "  " );
    m = add_method( "ParseReader", "init", 0, 2, 7,"ParseReader", code, 2266 );
    add_parameter( m, 0, "filename", "String" );
    add_parameter( m, 1, "_data", "String" );
    add_local( m, 2, "buffer", "Character[]" );
    add_local( m, 3, "collection:3", "String" );
    add_local( m, 4, "counter:3", "Integer" );
    add_local( m, 5, "ch", "Character" );
    add_local( m, 6, "value", "Integer" );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "  " );
    m = add_method( "ParseReader", "has_another", 0, 0, 0,"Logical", code, 2403 );
    m = add_method( "ParseReader", "peek", 0, 0, 0,"Character", code, 2413 );
    m = add_method( "ParseReader", "peek", 0, 1, 1,"Character", code, 2439 );
    add_parameter( m, 0, "offset", "Integer" );
    m = add_method( "ParseReader", "read", 0, 0, 1,"Character", code, 2471 );
    add_local( m, 0, "result", "Character" );
    m = add_method( "TokenType", "init_object", 0, 0, 0,"TokenType", code, 2528 );
    m.String_literals = new BardString[78];    m.String_literals[0] = new BardString( type_String, "[End Of File]" );
    m.String_literals[1] = new BardString( type_String, "[End Of Line]" );
    m.String_literals[2] = new BardString( type_String, "class" );
    m.String_literals[3] = new BardString( type_String, "function" );
    m.String_literals[4] = new BardString( type_String, "else" );
    m.String_literals[5] = new BardString( type_String, "elseIf" );
    m.String_literals[6] = new BardString( type_String, "endClass" );
    m.String_literals[7] = new BardString( type_String, "endForEach" );
    m.String_literals[8] = new BardString( type_String, "endFunction" );
    m.String_literals[9] = new BardString( type_String, "endIf" );
    m.String_literals[10] = new BardString( type_String, "endLoop" );
    m.String_literals[11] = new BardString( type_String, "endObject" );
    m.String_literals[12] = new BardString( type_String, "endWhile" );
    m.String_literals[13] = new BardString( type_String, "in" );
    m.String_literals[14] = new BardString( type_String, "METHODS" );
    m.String_literals[15] = new BardString( type_String, "method" );
    m.String_literals[16] = new BardString( type_String, "object" );
    m.String_literals[17] = new BardString( type_String, "of" );
    m.String_literals[18] = new BardString( type_String, "PROPERTIES" );
    m.String_literals[19] = new BardString( type_String, "}#" );
    m.String_literals[20] = new BardString( type_String, ">>" );
    m.String_literals[21] = new BardString( type_String, "[Comment]" );
    m.String_literals[22] = new BardString( type_String, "[Identifier]" );
    m.String_literals[23] = new BardString( type_String, "forEach" );
    m.String_literals[24] = new BardString( type_String, "if" );
    m.String_literals[25] = new BardString( type_String, "loop" );
    m.String_literals[26] = new BardString( type_String, "return" );
    m.String_literals[27] = new BardString( type_String, "throw" );
    m.String_literals[28] = new BardString( type_String, "trace" );
    m.String_literals[29] = new BardString( type_String, "tron" );
    m.String_literals[30] = new BardString( type_String, "troff" );
    m.String_literals[31] = new BardString( type_String, "while" );
    m.String_literals[32] = new BardString( type_String, "[Real]" );
    m.String_literals[33] = new BardString( type_String, "[pi]" );
    m.String_literals[34] = new BardString( type_String, "[Integer]" );
    m.String_literals[35] = new BardString( type_String, "[Character]" );
    m.String_literals[36] = new BardString( type_String, "true" );
    m.String_literals[37] = new BardString( type_String, "false" );
    m.String_literals[38] = new BardString( type_String, "[String]" );
    m.String_literals[39] = new BardString( type_String, "+=" );
    m.String_literals[40] = new BardString( type_String, "and" );
    m.String_literals[41] = new BardString( type_String, "as" );
    m.String_literals[42] = new BardString( type_String, "at" );
    m.String_literals[43] = new BardString( type_String, "&=" );
    m.String_literals[44] = new BardString( type_String, "|=" );
    m.String_literals[45] = new BardString( type_String, "~=" );
    m.String_literals[46] = new BardString( type_String, "<>" );
    m.String_literals[47] = new BardString( type_String, "--" );
    m.String_literals[48] = new BardString( type_String, "/=" );
    m.String_literals[49] = new BardString( type_String, "..>" );
    m.String_literals[50] = new BardString( type_String, "{}" );
    m.String_literals[51] = new BardString( type_String, "[]" );
    m.String_literals[52] = new BardString( type_String, "==" );
    m.String_literals[53] = new BardString( type_String, ">=" );
    m.String_literals[54] = new BardString( type_String, "global" );
    m.String_literals[55] = new BardString( type_String, "++" );
    m.String_literals[56] = new BardString( type_String, "instanceOf" );
    m.String_literals[57] = new BardString( type_String, "is" );
    m.String_literals[58] = new BardString( type_String, "<=" );
    m.String_literals[59] = new BardString( type_String, "local" );
    m.String_literals[60] = new BardString( type_String, ".=" );
    m.String_literals[61] = new BardString( type_String, "%=" );
    m.String_literals[62] = new BardString( type_String, "*=" );
    m.String_literals[63] = new BardString( type_String, "!=" );
    m.String_literals[64] = new BardString( type_String, "noAction" );
    m.String_literals[65] = new BardString( type_String, "not" );
    m.String_literals[66] = new BardString( type_String, "null" );
    m.String_literals[67] = new BardString( type_String, "<<" );
    m.String_literals[68] = new BardString( type_String, "or" );
    m.String_literals[69] = new BardString( type_String, "^=" );
    m.String_literals[70] = new BardString( type_String, ":<<:" );
    m.String_literals[71] = new BardString( type_String, ":>>:" );
    m.String_literals[72] = new BardString( type_String, ":>>>:" );
    m.String_literals[73] = new BardString( type_String, "-=" );
    m.String_literals[74] = new BardString( type_String, "this" );
    m.String_literals[75] = new BardString( type_String, ".." );
    m.String_literals[76] = new BardString( type_String, "..<" );
    m.String_literals[77] = new BardString( type_String, "xor" );
    m = add_method( "TokenType", "init", 0, 0, 0,"TokenType", code, 3889 );
    m = add_method( "TokenType", "set_up_keywords", 0, 0, 4,null, code, 3897 );
    add_local( m, 0, "token_list", "List" );
    add_local( m, 1, "collection:1", "List" );
    add_local( m, 2, "counter:1", "Integer" );
    add_local( m, 3, "type", "Variant" );
    m = add_method( "Token", "init_object", 0, 0, 0,"Token", code, 4148 );
    m = add_method( "Token", "String", 0, 0, 5,"String", code, 4152 );
    add_local( m, 0, "t", "Integer" );
    add_local( m, 1, "digit", "Integer" );
    add_local( m, 2, "intval", "Integer" );
    add_local( m, 3, "ch", "Character" );
    add_local( m, 4, "result", "Variant" );
    m.String_literals = new BardString[2];    m.String_literals[0] = new BardString( type_String, "[End Of Line:" );
    m.String_literals[1] = new BardString( type_String, "???" );
    m = add_method( "Token", "init", 0, 2, 2,"Token", code, 4357 );
    add_parameter( m, 0, "scanner", "Scanner" );
    add_parameter( m, 1, "_type", "Integer" );
    m = add_method( "Token", "init", 0, 3, 3,"Token", code, 4387 );
    add_parameter( m, 0, "scanner", "Scanner" );
    add_parameter( m, 1, "_type", "Integer" );
    add_parameter( m, 2, "_content", "Character[]" );
    m = add_method( "Token", "init", 0, 3, 3,"Token", code, 4408 );
    add_parameter( m, 0, "scanner", "Scanner" );
    add_parameter( m, 1, "_type", "Integer" );
    add_parameter( m, 2, "_value", "Real" );
    m = add_method( "Token", "init", 0, 3, 3,"Token", code, 4426 );
    add_parameter( m, 0, "_filepath", "String" );
    add_parameter( m, 1, "_line", "Integer" );
    add_parameter( m, 2, "_column", "Integer" );
    m = add_method( "Token", "error", 0, 1, 2,"String", code, 4445 );
    add_parameter( m, 0, "mesg", "String" );
    add_local( m, 1, "buffer", "Character[]" );
    m.String_literals = new BardString[4];    m.String_literals[0] = new BardString( type_String, "ERROR in \"" );
    m.String_literals[1] = new BardString( type_String, "\", line " );
    m.String_literals[2] = new BardString( type_String, ", column " );
    m.String_literals[3] = new BardString( type_String, ":  " );
    m = add_method( "Scanner", "init_object", 0, 0, 0,"Scanner", code, 4519 );
    m = add_method( "Scanner", "init", 0, 0, 0,"Scanner", code, 4523 );
    m = add_method( "Scanner", "tokenize", 0, 1, 1,"List", code, 4547 );
    add_parameter( m, 0, "_filepath", "String" );
    m = add_method( "Scanner", "tokenize", 0, 2, 2,"List", code, 4582 );
    add_parameter( m, 0, "_filepath", "String" );
    add_parameter( m, 1, "data", "String" );
    m = add_method( "Scanner", "consume", 0, 1, 1,"Logical", code, 4619 );
    add_parameter( m, 0, "ch", "Character" );
    m = add_method( "Scanner", "consume", 0, 1, 2,"Logical", code, 4658 );
    add_parameter( m, 0, "st", "String" );
    add_local( m, 1, "i", "Integer" );
    m = add_method( "Scanner", "consume_eols", 0, 0, 0,"Logical", code, 4718 );
    m = add_method( "Scanner", "error", 0, 1, 2,"String", code, 4745 );
    add_parameter( m, 0, "mesg", "String" );
    add_local( m, 1, "buffer", "Character[]" );
    m.String_literals = new BardString[4];    m.String_literals[0] = new BardString( type_String, "Syntax error in \"" );
    m.String_literals[1] = new BardString( type_String, "\" line " );
    m.String_literals[2] = new BardString( type_String, ", column " );
    m.String_literals[3] = new BardString( type_String, ":  " );
    m = add_method( "Scanner", "scan_next", 0, 0, 2,"Logical", code, 4837 );
    add_local( m, 0, "ch", "Character" );
    add_local( m, 1, "type", "Integer" );
    m = add_method( "Scanner", "scan_identifier", 0, 0, 2,null, code, 5253 );
    add_local( m, 0, "keep_reading", "Logical" );
    add_local( m, 1, "ch", "Character" );
    m = add_method( "Scanner", "scan_String", 0, 1, 2,null, code, 5374 );
    add_parameter( m, 0, "terminator", "Character" );
    add_local( m, 1, "ch", "Character" );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "End Of File reached while looking for end of string." );
    m = add_method( "Scanner", "scan_alternate_String", 0, 0, 1,null, code, 5448 );
    add_local( m, 0, "ch", "Character" );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "End Of File reached while looking for end of string." );
    m = add_method( "Scanner", "scan_Character", 0, 0, 4,"Character", code, 5559 );
    add_local( m, 0, "ch", "Character" );
    add_local( m, 1, "value", "Integer" );
    add_local( m, 2, "ch2", "Integer" );
    add_local( m, 3, "ch3", "Integer" );
    m.String_literals = new BardString[6];    m.String_literals[0] = new BardString( type_String, "Character expected." );
    m.String_literals[1] = new BardString( type_String, "Character expected; found End Of Line." );
    m.String_literals[2] = new BardString( type_String, "Escaped character expected; found End Of File." );
    m.String_literals[3] = new BardString( type_String, "Invalid escape sequence.  Supported: \\n \\r \\t \\0 \\/ \\' \\\\ \\\" \\" );
    m.String_literals[4] = new BardString( type_String, "uXXXX \\" );
    m.String_literals[5] = new BardString( type_String, "xXX." );
    m = add_method( "Scanner", "scan_hex_value", 0, 1, 5,"Character", code, 5963 );
    add_parameter( m, 0, "digits", "Integer" );
    add_local( m, 1, "value", "Integer" );
    add_local( m, 2, "ch", "Character" );
    add_local( m, 3, "buffer", "Character[]" );
    add_local( m, 4, "intval", "Integer" );
    m.String_literals = new BardString[2];    m.String_literals[0] = new BardString( type_String, "-digit hex value expected; found end of file." );
    m.String_literals[1] = new BardString( type_String, "Invalid hex digit " );
    m = add_method( "Scanner", "next_is_hex_digit", 0, 0, 1,"Logical", code, 6151 );
    add_local( m, 0, "ch", "Character" );
    m = add_method( "Scanner", "scan_number", 0, 0, 9,null, code, 6202 );
    add_local( m, 0, "is_negative", "Logical" );
    add_local( m, 1, "is_Real", "Logical" );
    add_local( m, 2, "n", "Real" );
    add_local( m, 3, "ch", "Character" );
    add_local( m, 4, "intval", "Integer" );
    add_local( m, 5, "start_pos", "Integer" );
    add_local( m, 6, "fraction", "Real" );
    add_local( m, 7, "negative_exponent", "Logical" );
    add_local( m, 8, "power", "Real" );
    m.Real_literals = to_double_array( new long[]{ 0x4024000000000000L } );
    m = add_method( "Scanner", "scan_Integer", 0, 0, 3,"Real", code, 6507 );
    add_local( m, 0, "n", "Real" );
    add_local( m, 1, "ch", "Character" );
    add_local( m, 2, "intval", "Integer" );
    m.Real_literals = to_double_array( new long[]{ 0x0L,0x4024000000000000L } );
    m = add_method( "Scanner", "scan_comment", 0, 0, 1,null, code, 6568 );
    add_local( m, 0, "keep_reading", "Logical" );
    m = add_method( "Scanner", "scan_symbol", 0, 0, 1,"Integer", code, 6698 );
    add_local( m, 0, "ch", "Character" );
    m.String_literals = new BardString[7];    m.String_literals[0] = new BardString( type_String, "Use 'and' instead of '&&'." );
    m.String_literals[1] = new BardString( type_String, "End of line expected after '...'." );
    m.String_literals[2] = new BardString( type_String, "<<:" );
    m.String_literals[3] = new BardString( type_String, ">>:" );
    m.String_literals[4] = new BardString( type_String, ">>>:" );
    m.String_literals[5] = new BardString( type_String, "Unexpected input '" );
    m.String_literals[6] = new BardString( type_String, "'." );
    m = add_method( "Scanner", "add_String_or_Char", 0, 0, 1,null, code, 7631 );
    add_local( m, 0, "code", "Real" );
    m = add_method( "TokenReader", "init_object", 0, 0, 0,"TokenReader", code, 7694 );
    m = add_method( "TokenReader", "init", 0, 1, 1,"TokenReader", code, 7698 );
    add_parameter( m, 0, "_tokens", "List" );
    m = add_method( "TokenReader", "init", 0, 1, 1,"TokenReader", code, 7719 );
    add_parameter( m, 0, "filepath", "String" );
    m = add_method( "TokenReader", "init", 0, 2, 2,"TokenReader", code, 7745 );
    add_parameter( m, 0, "filepath", "String" );
    add_parameter( m, 1, "data", "String" );
    m = add_method( "TokenReader", "rewind", 0, 0, 0,null, code, 7773 );
    m = add_method( "TokenReader", "has_another", 0, 0, 0,"Logical", code, 7780 );
    m = add_method( "TokenReader", "peek", 0, 0, 0,"Token", code, 7790 );
    m = add_method( "TokenReader", "peek", 0, 1, 1,"Token", code, 7803 );
    add_parameter( m, 0, "num_ahead", "Integer" );
    m = add_method( "TokenReader", "read", 0, 0, 1,"Token", code, 7819 );
    add_local( m, 0, "result", "Token" );
    m = add_method( "Parser", "init_object", 0, 0, 0,"Parser", code, 7845 );
    m = add_method( "Parser", "parse", 0, 1, 1,null, code, 7859 );
    add_parameter( m, 0, "_filepath", "String" );
    m = add_method( "Parser", "parse", 0, 3, 15,null, code, 7881 );
    add_parameter( m, 0, "temp", "Template" );
    add_parameter( m, 1, "type", "TypeDef" );
    add_parameter( m, 2, "t", "Token" );
    add_local( m, 3, "type_has_specialization", "Logical" );
    add_local( m, 4, "mappings", "Table" );
    add_local( m, 5, "tokens", "List" );
    add_local( m, 6, "pos", "Integer" );
    add_local( m, 7, "count", "Integer" );
    add_local( m, 8, "i", "Integer" );
    add_local( m, 9, "t", "Token" );
    add_local( m, 10, "replacement_name", "String" );
    add_local( m, 11, "replacement_tokens", "List" );
    add_local( m, 12, "collection:12", "List" );
    add_local( m, 13, "counter:12", "Integer" );
    add_local( m, 14, "rt", "Variant" );
    m.String_literals = new BardString[6];    m.String_literals[0] = new BardString( type_String, "Type specialization expected for type " );
    m.String_literals[1] = new BardString( type_String, "Type specializations expected for type " );
    m.String_literals[2] = new BardString( type_String, "Not enough type specializers." );
    m.String_literals[3] = new BardString( type_String, "Identifier expected after '$'." );
    m.String_literals[4] = new BardString( type_String, "No such placeholder $" );
    m.String_literals[5] = new BardString( type_String, "Illegal specialization for type " );
    m = add_method( "Parser", "error", 0, 1, 2,"String", code, 8497 );
    add_parameter( m, 0, "mesg", "String" );
    add_local( m, 1, "t", "Token" );
    m = add_method( "Parser", "peek", 0, 0, 0,"Token", code, 8573 );
    m = add_method( "Parser", "peek", 0, 1, 1,"Token", code, 8582 );
    add_parameter( m, 0, "num_ahead", "Integer" );
    m = add_method( "Parser", "next_is", 0, 1, 1,"Logical", code, 8593 );
    add_parameter( m, 0, "type", "Integer" );
    m = add_method( "Parser", "read", 0, 0, 0,"Token", code, 8615 );
    m = add_method( "Parser", "consume", 0, 1, 1,"Logical", code, 8624 );
    add_parameter( m, 0, "type", "Integer" );
    m = add_method( "Parser", "must_consume", 0, 1, 2,null, code, 8663 );
    add_parameter( m, 0, "type", "Integer" );
    add_local( m, 1, "t", "Token" );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "' expected." );
    m = add_method( "Parser", "consume", 0, 1, 1,"Logical", code, 8742 );
    add_parameter( m, 0, "text", "String" );
    m = add_method( "Parser", "consume_eols", 0, 0, 0,"Logical", code, 8804 );
    m = add_method( "Parser", "next_is_end_command", 0, 0, 0,"Logical", code, 8835 );
    m = add_method( "Parser", "discard_end_commands", 0, 0, 0,null, code, 8856 );
    m = add_method( "Parser", "must_consume_end_command", 0, 0, 0,null, code, 8880 );
    m.String_literals = new BardString[2];    m.String_literals[0] = new BardString( type_String, "Syntax error: unexpected '" );
    m.String_literals[1] = new BardString( type_String, "'; [End Of Line] or ';' expected." );
    m = add_method( "Parser", "read_identifier", 0, 0, 0,"String", code, 8951 );
    m.String_literals = new BardString[2];    m.String_literals[0] = new BardString( type_String, "Identifier expected instead of '" );
    m.String_literals[1] = new BardString( type_String, "'." );
    m = add_method( "Parser", "parse_types", 0, 0, 0,null, code, 9030 );
    m.String_literals = new BardString[2];    m.String_literals[0] = new BardString( type_String, "Syntax error: unexpected '" );
    m.String_literals[1] = new BardString( type_String, "'." );
    m = add_method( "Parser", "parse_template", 0, 1, 5,null, code, 9118 );
    add_parameter( m, 0, "end_symbol", "Integer" );
    add_local( m, 1, "t", "Token" );
    add_local( m, 2, "name", "String" );
    add_local( m, 3, "template", "Template" );
    add_local( m, 4, "first", "Logical" );
    m = add_method( "Parser", "parse_type_definition", 0, 0, 1,null, code, 9237 );
    add_local( m, 0, "first", "Logical" );
    m.String_literals = new BardString[2];    m.String_literals[0] = new BardString( type_String, "Syntax error: unexpected '" );
    m.String_literals[1] = new BardString( type_String, "'." );
    m = add_method( "Parser", "parse_property_list", 0, 0, 7,"Logical", code, 9406 );
    add_local( m, 0, "t", "Token" );
    add_local( m, 1, "name", "String" );
    add_local( m, 2, "property", "Property" );
    add_local( m, 3, "type", "Type" );
    add_local( m, 4, "collection:4", "List" );
    add_local( m, 5, "counter:4", "Integer" );
    add_local( m, 6, "property", "Variant" );
    m = add_method( "Parser", "parse_attributes", 0, 1, 2,null, code, 9557 );
    add_parameter( m, 0, "list", "Attributes" );
    add_local( m, 1, "name", "String" );
    m = add_method( "Parser", "parse_specialization_string", 0, 0, 1,"String", code, 9616 );
    add_local( m, 0, "buffer", "Character[]" );
    m.String_literals = new BardString[2];    m.String_literals[0] = new BardString( type_String, "<<" );
    m.String_literals[1] = new BardString( type_String, ">>" );
    m = add_method( "Parser", "parse_type", 0, 0, 3,"Type", code, 9684 );
    add_local( m, 0, "t", "Token" );
    add_local( m, 1, "name", "String" );
    add_local( m, 2, "type", "TypeDef" );
    m = add_method( "Parser", "parse_method", 0, 0, 5,"Logical", code, 9766 );
    add_local( m, 0, "t", "Token" );
    add_local( m, 1, "name", "String" );
    add_local( m, 2, "first", "Logical" );
    add_local( m, 3, "param_name", "String" );
    add_local( m, 4, "param_type", "Type" );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "native" );
    m = add_method( "Parser", "parse_multi_line_statements", 0, 1, 1,null, code, 9997 );
    add_parameter( m, 0, "statements", "CmdStatementList" );
    m = add_method( "Parser", "parse_single_line_statements", 0, 1, 1,null, code, 10044 );
    add_parameter( m, 0, "statements", "CmdStatementList" );
    m = add_method( "Parser", "next_is_statement", 0, 0, 0,"Logical", code, 10095 );
    m = add_method( "Parser", "parse_statement", 0, 2, 5,null, code, 10122 );
    add_parameter( m, 0, "statements", "CmdStatementList" );
    add_parameter( m, 1, "allow_control_structures", "Logical" );
    add_local( m, 2, "t", "Token" );
    add_local( m, 3, "err", "Logical" );
    add_local( m, 4, "expression", "Cmd" );
    m.String_literals = new BardString[2];    m.String_literals[0] = new BardString( type_String, "Control structures must begin on a separate line." );
    m.String_literals[1] = new BardString( type_String, "println" );
    m = add_method( "Parser", "parse_if", 0, 0, 2,"CmdIf", code, 10403 );
    add_local( m, 0, "t", "Token" );
    add_local( m, 1, "cmd_if", "CmdIf" );
    m = add_method( "Parser", "parse_local_declaration", 0, 1, 17,null, code, 10564 );
    add_parameter( m, 0, "statements", "CmdStatementList" );
    add_local( m, 1, "t", "Token" );
    add_local( m, 2, "first", "Logical" );
    add_local( m, 3, "name", "String" );
    add_local( m, 4, "v", "Local" );
    add_local( m, 5, "type", "Type" );
    add_local( m, 6, "collection:6", "List" );
    add_local( m, 7, "counter:6", "Integer" );
    add_local( m, 8, "v", "Variant" );
    add_local( m, 9, "collection:9", "List" );
    add_local( m, 10, "counter:9", "Integer" );
    add_local( m, 11, "vv", "Variant" );
    add_local( m, 12, "v", "Local" );
    add_local( m, 13, "collection:13", "List" );
    add_local( m, 14, "counter:13", "Integer" );
    add_local( m, 15, "e", "Variant" );
    add_local( m, 16, "v", "Local" );
    m = add_method( "Parser", "parse_expression", 0, 0, 0,"Cmd", code, 10875 );
    m = add_method( "Parser", "parse_logical_xor", 0, 0, 0,"Cmd", code, 10882 );
    m = add_method( "Parser", "parse_logical_xor", 0, 1, 2,"Cmd", code, 10893 );
    add_parameter( m, 0, "lhs", "Cmd" );
    add_local( m, 1, "t", "Token" );
    m = add_method( "Parser", "parse_logical_or", 0, 0, 0,"Cmd", code, 10936 );
    m = add_method( "Parser", "parse_logical_or", 0, 1, 2,"Cmd", code, 10947 );
    add_parameter( m, 0, "lhs", "Cmd" );
    add_local( m, 1, "t", "Token" );
    m = add_method( "Parser", "parse_logical_and", 0, 0, 0,"Cmd", code, 10990 );
    m = add_method( "Parser", "parse_logical_and", 0, 1, 2,"Cmd", code, 11001 );
    add_parameter( m, 0, "lhs", "Cmd" );
    add_local( m, 1, "t", "Token" );
    m = add_method( "Parser", "parse_comparison", 0, 0, 0,"Cmd", code, 11044 );
    m = add_method( "Parser", "parse_comparison", 0, 1, 2,"Cmd", code, 11055 );
    add_parameter( m, 0, "lhs", "Cmd" );
    add_local( m, 1, "t", "Token" );
    m = add_method( "Parser", "parse_bitwise_xor", 0, 0, 0,"Cmd", code, 11286 );
    m = add_method( "Parser", "parse_bitwise_xor", 0, 1, 2,"Cmd", code, 11297 );
    add_parameter( m, 0, "lhs", "Cmd" );
    add_local( m, 1, "t", "Token" );
    m = add_method( "Parser", "parse_bitwise_or", 0, 0, 0,"Cmd", code, 11340 );
    m = add_method( "Parser", "parse_bitwise_or", 0, 1, 2,"Cmd", code, 11351 );
    add_parameter( m, 0, "lhs", "Cmd" );
    add_local( m, 1, "t", "Token" );
    m = add_method( "Parser", "parse_bitwise_and", 0, 0, 0,"Cmd", code, 11394 );
    m = add_method( "Parser", "parse_bitwise_and", 0, 1, 2,"Cmd", code, 11405 );
    add_parameter( m, 0, "lhs", "Cmd" );
    add_local( m, 1, "t", "Token" );
    m = add_method( "Parser", "parse_bitwise_shift", 0, 0, 0,"Cmd", code, 11448 );
    m = add_method( "Parser", "parse_bitwise_shift", 0, 1, 2,"Cmd", code, 11459 );
    add_parameter( m, 0, "lhs", "Cmd" );
    add_local( m, 1, "t", "Token" );
    m = add_method( "Parser", "parse_add_subtract", 0, 0, 0,"Cmd", code, 11566 );
    m = add_method( "Parser", "parse_add_subtract", 0, 1, 2,"Cmd", code, 11577 );
    add_parameter( m, 0, "lhs", "Cmd" );
    add_local( m, 1, "t", "Token" );
    m = add_method( "Parser", "parse_multiply_divide_mod", 0, 0, 0,"Cmd", code, 11652 );
    m = add_method( "Parser", "parse_multiply_divide_mod", 0, 1, 2,"Cmd", code, 11663 );
    add_parameter( m, 0, "lhs", "Cmd" );
    add_local( m, 1, "t", "Token" );
    m = add_method( "Parser", "parse_power", 0, 0, 0,"Cmd", code, 11770 );
    m = add_method( "Parser", "parse_power", 0, 1, 2,"Cmd", code, 11781 );
    add_parameter( m, 0, "lhs", "Cmd" );
    add_local( m, 1, "t", "Token" );
    m = add_method( "Parser", "parse_pre_unary", 0, 0, 1,"Cmd", code, 11824 );
    add_local( m, 0, "t", "Token" );
    m = add_method( "Parser", "parse_post_unary", 0, 0, 0,"Cmd", code, 11915 );
    m = add_method( "Parser", "parse_post_unary", 0, 1, 2,"Cmd", code, 11926 );
    add_parameter( m, 0, "operand", "Cmd" );
    add_local( m, 1, "t", "Token" );
    m = add_method( "Parser", "parse_access", 0, 0, 0,"Cmd", code, 11965 );
    m = add_method( "Parser", "parse_access", 0, 1, 3,"Cmd", code, 11976 );
    add_parameter( m, 0, "operand", "Cmd" );
    add_local( m, 1, "t", "Token" );
    add_local( m, 2, "access", "CmdAccess" );
    m = add_method( "Parser", "parse_access_command", 0, 1, 5,"CmdAccess", code, 12020 );
    add_parameter( m, 0, "t", "Token" );
    add_local( m, 1, "name", "String" );
    add_local( m, 2, "access", "CmdAccess" );
    add_local( m, 3, "args", "CmdArgs" );
    add_local( m, 4, "first", "Logical" );
    m = add_method( "Parser", "parse_term", 0, 0, 4,"Cmd", code, 12160 );
    add_local( m, 0, "t", "Token" );
    add_local( m, 1, "result", "Cmd" );
    add_local( m, 2, "value", "Integer" );
    add_local( m, 3, "value", "Integer" );
    m.String_literals = new BardString[2];    m.String_literals[0] = new BardString( type_String, "Syntax error: unexpected '" );
    m.String_literals[1] = new BardString( type_String, "'." );
    m = add_method( "TypeDef", "init_object", 0, 0, 0,"TypeDef", code, 12430 );
    m = add_method( "TypeDef", "String", 0, 0, 0,"String", code, 12456 );
    m = add_method( "TypeDef", "init", 0, 3, 3,"TypeDef", code, 12462 );
    add_parameter( m, 0, "_t", "Token" );
    add_parameter( m, 1, "_name", "String" );
    add_parameter( m, 2, "_qualifiers", "Integer" );
    m = add_method( "TypeDef", "is_primitive", 0, 0, 0,"Logical", code, 12516 );
    m = add_method( "TypeDef", "is_reference", 0, 0, 0,"Logical", code, 12530 );
    m = add_method( "TypeDef", "instance_of", 0, 1, 4,"Logical", code, 12538 );
    add_parameter( m, 0, "ancestor", "TypeDef" );
    add_local( m, 1, "collection:1", "List" );
    add_local( m, 2, "counter:1", "Integer" );
    add_local( m, 3, "base_type", "Variant" );
    m = add_method( "TypeDef", "add_property", 0, 1, 1,null, code, 12613 );
    add_parameter( m, 0, "property", "Property" );
    m = add_method( "TypeDef", "add_method", 0, 1, 1,null, code, 12631 );
    add_parameter( m, 0, "m", "Method" );
    m = add_method( "TypeDef", "find_property", 0, 1, 4,"Property", code, 12641 );
    add_parameter( m, 0, "name", "String" );
    add_local( m, 1, "collection:1", "List" );
    add_local( m, 2, "counter:1", "Integer" );
    add_local( m, 3, "p", "Variant" );
    m = add_method( "TypeDef", "find_method", 0, 1, 5,"Method", code, 12698 );
    add_parameter( m, 0, "signature", "String" );
    add_local( m, 1, "collection:1", "List" );
    add_local( m, 2, "counter:1", "Integer" );
    add_local( m, 3, "mv", "Variant" );
    add_local( m, 4, "m", "Method" );
    m = add_method( "TypeDef", "organize", 0, 0, 0,"TypeDef", code, 12756 );
    m = add_method( "TypeDef", "organize", 0, 1, 25,"TypeDef", code, 12766 );
    add_parameter( m, 0, "_t", "Token" );
    add_local( m, 1, "collection:1", "List" );
    add_local( m, 2, "counter:1", "Integer" );
    add_local( m, 3, "base_type", "Variant" );
    add_local( m, 4, "collection:4", "List" );
    add_local( m, 5, "counter:4", "Integer" );
    add_local( m, 6, "m", "Variant" );
    add_local( m, 7, "original_properties", "List" );
    add_local( m, 8, "collection:8", "List" );
    add_local( m, 9, "counter:8", "Integer" );
    add_local( m, 10, "base_type", "Variant" );
    add_local( m, 11, "collection:11", "List" );
    add_local( m, 12, "counter:11", "Integer" );
    add_local( m, 13, "m", "Variant" );
    add_local( m, 14, "original_methods", "List" );
    add_local( m, 15, "collection:15", "List" );
    add_local( m, 16, "counter:15", "Integer" );
    add_local( m, 17, "base_type", "Variant" );
    add_local( m, 18, "m", "Method" );
    add_local( m, 19, "list", "List" );
    add_local( m, 20, "collection:20", "List" );
    add_local( m, 21, "counter:20", "Integer" );
    add_local( m, 22, "pv", "Variant" );
    add_local( m, 23, "p", "Property" );
    add_local( m, 24, "m_init_object", "Method" );
    m.String_literals = new BardString[3];    m.String_literals[0] = new BardString( type_String, "init_object()" );
    m.String_literals[1] = new BardString( type_String, "init_object" );
    m.String_literals[2] = new BardString( type_String, "init_object()" );
    m = add_method( "TypeDef", "collect_properties", 0, 1, 4,null, code, 13296 );
    add_parameter( m, 0, "from_list", "List" );
    add_local( m, 1, "collection:1", "List" );
    add_local( m, 2, "counter:1", "Integer" );
    add_local( m, 3, "p", "Variant" );
    m = add_method( "TypeDef", "collect_property", 0, 1, 3,null, code, 13334 );
    add_parameter( m, 0, "p", "Property" );
    add_local( m, 1, "i", "Integer" );
    add_local( m, 2, "existing_p", "Property" );
    m = add_method( "TypeDef", "collect_methods", 0, 1, 4,null, code, 13417 );
    add_parameter( m, 0, "from_list", "List" );
    add_local( m, 1, "collection:1", "List" );
    add_local( m, 2, "counter:1", "Integer" );
    add_local( m, 3, "m", "Variant" );
    m = add_method( "TypeDef", "collect_method", 0, 1, 3,null, code, 13455 );
    add_parameter( m, 0, "m", "Method" );
    add_local( m, 1, "i", "Integer" );
    add_local( m, 2, "existing_m", "Method" );
    m = add_method( "TypeDef", "resolve", 0, 0, 0,null, code, 13544 );
    m = add_method( "TypeDef", "resolve", 0, 1, 4,null, code, 13553 );
    add_parameter( m, 0, "_t", "Token" );
    add_local( m, 1, "collection:1", "List" );
    add_local( m, 2, "counter:1", "Integer" );
    add_local( m, 3, "m", "Variant" );
    m = add_method( "TypeDef", "compile", 0, 1, 5,null, code, 13615 );
    add_parameter( m, 0, "writer", "JavaVMWriter" );
    add_local( m, 1, "collection:1", "List" );
    add_local( m, 2, "counter:1", "Integer" );
    add_local( m, 3, "mv", "Variant" );
    add_local( m, 4, "m", "Method" );
    m = add_method( "Method", "init_object", 0, 0, 0,"Method", code, 13676 );
    m = add_method( "Method", "init", 0, 4, 4,"Method", code, 13728 );
    add_parameter( m, 0, "_t", "Token" );
    add_parameter( m, 1, "_type_context", "TypeDef" );
    add_parameter( m, 2, "_name", "String" );
    add_parameter( m, 3, "_qualifiers", "Integer" );
    m = add_method( "Method", "create_signature", 0, 0, 5,null, code, 13765 );
    add_local( m, 0, "buffer", "Character[]" );
    add_local( m, 1, "first", "Logical" );
    add_local( m, 2, "collection:2", "List" );
    add_local( m, 3, "counter:2", "Integer" );
    add_local( m, 4, "p", "Variant" );
    m = add_method( "Method", "full_signature", 0, 0, 0,"String", code, 13875 );
    m = add_method( "Method", "add_parameter", 0, 1, 1,null, code, 13932 );
    add_parameter( m, 0, "v", "Local" );
    m = add_method( "Method", "add_local", 0, 1, 1,null, code, 13948 );
    add_parameter( m, 0, "v", "Local" );
    m = add_method( "Method", "organize", 0, 0, 8,null, code, 13966 );
    add_local( m, 0, "collection:0", "List" );
    add_local( m, 1, "counter:0", "Integer" );
    add_local( m, 2, "v", "Variant" );
    add_local( m, 3, "as_type", "Type" );
    add_local( m, 4, "is_init", "Logical" );
    add_local( m, 5, "add_return_guard", "Logical" );
    add_local( m, 6, "visitor", "ImplicitReturnTypeVisitor" );
    add_local( m, 7, "buffer", "Character[]" );
    m.String_literals = new BardString[2];    m.String_literals[0] = new BardString( type_String, "init" );
    m.String_literals[1] = new BardString( type_String, "init_object" );
    m = add_method( "Method", "resolve", 0, 0, 3,null, code, 14315 );
    add_local( m, 0, "collection:0", "List" );
    add_local( m, 1, "counter:0", "Integer" );
    add_local( m, 2, "v", "Variant" );
    m = add_method( "Method", "compile", 0, 1, 1,null, code, 14400 );
    add_parameter( m, 0, "writer", "JavaVMWriter" );
    m = add_method( "Template", "init_object", 0, 0, 0,"Template", code, 14421 );
    m = add_method( "Template", "init", 0, 2, 2,"Template", code, 14425 );
    add_parameter( m, 0, "_t", "Token" );
    add_parameter( m, 1, "_name", "String" );
    m = add_method( "Attributes", "init_object", 0, 0, 0,"Attributes", code, 14449 );
    m = add_method( "Attributes", "add", 0, 1, 1,null, code, 14457 );
    add_parameter( m, 0, "key", "String" );
    m = add_method( "Attributes", "add", 0, 2, 2,null, code, 14467 );
    add_parameter( m, 0, "key", "String" );
    add_parameter( m, 1, "value", "String" );
    m = add_method( "Attributes", "contains", 0, 1, 1,"Logical", code, 14480 );
    add_parameter( m, 0, "key", "String" );
    m = add_method( "Attributes", "count", 0, 0, 0,"Integer", code, 14492 );
    m = add_method( "Attributes", "get", 0, 1, 1,"String", code, 14499 );
    add_parameter( m, 0, "index", "Integer" );
    m = add_method( "Attributes", "get", 0, 1, 1,"String", code, 14513 );
    add_parameter( m, 0, "key", "String" );
    m = add_method( "Type", "init_object", 0, 0, 0,"Type", code, 14526 );
    m = add_method( "Type", "init", 0, 2, 2,"Type", code, 14530 );
    add_parameter( m, 0, "_def", "TypeDef" );
    add_parameter( m, 1, "_possibly_null", "Logical" );
    m = add_method( "Type", "name", 0, 0, 0,"String", code, 14544 );
    m = add_method( "Type", "is_primitive", 0, 0, 0,"Logical", code, 14552 );
    m = add_method( "Type", "is_reference", 0, 0, 0,"Logical", code, 14568 );
    m = add_method( "Type", "is_Real", 0, 0, 0,"Logical", code, 14576 );
    m = add_method( "Type", "is_Integer", 0, 0, 0,"Logical", code, 14587 );
    m = add_method( "Type", "is_Character", 0, 0, 0,"Logical", code, 14598 );
    m = add_method( "Type", "is_Logical", 0, 0, 0,"Logical", code, 14609 );
    m = add_method( "Type", "is_Object", 0, 0, 0,"Logical", code, 14620 );
    m = add_method( "Type", "is_String", 0, 0, 0,"Logical", code, 14631 );
    m = add_method( "Type", "instance_of", 0, 1, 1,"Logical", code, 14642 );
    add_parameter( m, 0, "ancestor", "Type" );
    m = add_method( "Type", "organize", 0, 0, 0,null, code, 14655 );
    m = add_method( "Type", "resolve", 0, 1, 1,null, code, 14664 );
    add_parameter( m, 0, "_t", "Token" );
    m = add_method( "CmdStatementList", "init_object", 0, 0, 0,"CmdStatementList", code, 14674 );
    m = add_method( "CmdStatementList", "dispatch", 0, 1, 4,null, code, 14683 );
    add_parameter( m, 0, "v", "Visitor" );
    add_local( m, 1, "collection:1", "CmdStatementList" );
    add_local( m, 2, "counter:1", "Integer" );
    add_local( m, 3, "statement", "Cmd" );
    m = add_method( "CmdStatementList", "resolve", 0, 0, 1,"Cmd", code, 14738 );
    add_local( m, 0, "i", "Integer" );
    m = add_method( "CmdStatementList", "compile", 0, 1, 5,null, code, 14783 );
    add_parameter( m, 0, "writer", "JavaVMWriter" );
    add_local( m, 1, "collection:1", "CmdStatementList" );
    add_local( m, 2, "counter:1", "Integer" );
    add_local( m, 3, "cmd", "Cmd" );
    add_local( m, 4, "result_type", "Type" );
    m = add_method( "CmdStatementList", "count", 0, 0, 0,"Integer", code, 14847 );
    m = add_method( "CmdStatementList", "get", 0, 1, 1,"Cmd", code, 14854 );
    add_parameter( m, 0, "index", "Integer" );
    m = add_method( "CmdStatementList", "add", 0, 1, 1,"CmdStatementList", code, 14866 );
    add_parameter( m, 0, "cmd", "Cmd" );
    m = add_method( "CmdStatementList", "insert", 0, 1, 1,"CmdStatementList", code, 14878 );
    add_parameter( m, 0, "cmd", "Cmd" );
    m = add_method( "CmdIf", "init_object", 0, 0, 0,"CmdIf", code, 14892 );
    m = add_method( "CmdIf", "resolve", 0, 0, 1,"Cmd", code, 14896 );
    add_local( m, 0, "condition_type", "Type" );
    m = add_method( "CmdIf", "compile", 0, 1, 6,null, code, 14979 );
    add_parameter( m, 0, "writer", "JavaVMWriter" );
    add_local( m, 1, "control_id", "Integer" );
    add_local( m, 2, "next_check", "Integer" );
    add_local( m, 3, "end_label", "Character[]" );
    add_local( m, 4, "has_else", "Logical" );
    add_local( m, 5, "else_count", "Integer" );
    m.String_literals = new BardString[3];    m.String_literals[0] = new BardString( type_String, "end_" );
    m.String_literals[1] = new BardString( type_String, "check_" );
    m.String_literals[2] = new BardString( type_String, "check_" );
    m = add_method( "CmdIf", "init", 0, 2, 2,"CmdIf", code, 15294 );
    add_parameter( m, 0, "_t", "Token" );
    add_parameter( m, 1, "_condition", "Cmd" );
    m = add_method( "Cmd", "init_object", 0, 0, 0,"Cmd", code, 15321 );
    m = add_method( "Cmd", "init", 0, 1, 1,"Cmd", code, 15325 );
    add_parameter( m, 0, "_t", "Token" );
    m = add_method( "Cmd", "implicit_type", 0, 0, 0,"Type", code, 15334 );
    m = add_method( "Cmd", "dispatch", 0, 1, 1,null, code, 15338 );
    add_parameter( m, 0, "v", "Visitor" );
    m = add_method( "Cmd", "determine_implicit_return_type", 0, 0, 0,null, code, 15346 );
    m = add_method( "Cmd", "type", 0, 0, 0,"Type", code, 15348 );
    m = add_method( "Cmd", "variable_name", 0, 0, 0,"String", code, 15352 );
    m = add_method( "Cmd", "resolve", 0, 0, 0,"Cmd", code, 15356 );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "resolve() not defined for extended Cmd." );
    m = add_method( "Cmd", "resolve_with_type_hint", 0, 1, 1,"Cmd", code, 15365 );
    add_parameter( m, 0, "as_type", "Type" );
    m = add_method( "Cmd", "require_Logical", 0, 0, 0,"Cmd", code, 15372 );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "Logical true or false value expected." );
    m = add_method( "Cmd", "require_value", 0, 0, 0,"Cmd", code, 15401 );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "Value expected." );
    m = add_method( "Cmd", "require_value", 0, 1, 2,"Cmd", code, 15422 );
    add_parameter( m, 0, "allow_unchecked", "Logical" );
    add_local( m, 1, "_type", "Type" );
    m.String_literals = new BardString[2];    m.String_literals[0] = new BardString( type_String, "Value expected." );
    m.String_literals[1] = new BardString( type_String, "Null-checked value required." );
    m = add_method( "Cmd", "cast_to", 0, 2, 2,"Cmd", code, 15473 );
    add_parameter( m, 0, "cast_t", "Token" );
    add_parameter( m, 1, "to_type", "Type" );
    m = add_method( "Cmd", "common_type", 0, 2, 4,"Type", code, 15499 );
    add_parameter( m, 0, "lhs", "Cmd" );
    add_parameter( m, 1, "rhs", "Cmd" );
    add_local( m, 2, "lhs_type", "Type" );
    add_local( m, 3, "rhs_type", "Type" );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "Invalid mix of Logical and non-Logical types." );
    m = add_method( "Cmd", "resolve_assignment", 0, 1, 1,"Cmd", code, 15674 );
    add_parameter( m, 0, "new_value", "Cmd" );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "resolve_assignment() is not defined for an extended Cmd." );
    m = add_method( "Cmd", "compile", 0, 1, 1,null, code, 15683 );
    add_parameter( m, 0, "writer", "JavaVMWriter" );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "compile() not defined for extended Cmd." );
    m = add_method( "Cmd", "error", 0, 1, 1,"String", code, 15692 );
    add_parameter( m, 0, "message", "String" );
    m = add_method( "CmdAccess", "init_object", 0, 0, 0,"CmdAccess", code, 15703 );
    m = add_method( "CmdAccess", "implicit_type", 0, 0, 2,"Type", code, 15707 );
    add_local( m, 0, "as_type", "Type" );
    add_local( m, 1, "v", "Local" );
    m = add_method( "CmdAccess", "dispatch", 0, 1, 1,null, code, 15770 );
    add_parameter( m, 0, "v", "Visitor" );
    m = add_method( "CmdAccess", "resolve", 0, 0, 1,"Cmd", code, 15810 );
    add_local( m, 0, "no_hint", "Type" );
    m = add_method( "CmdAccess", "resolve_with_type_hint", 0, 1, 9,"Cmd", code, 15851 );
    add_parameter( m, 0, "type_hint", "Type" );
    add_local( m, 1, "as_type", "Type" );
    add_local( m, 2, "operand_type", "Type" );
    add_local( m, 3, "result", "Cmd" );
    add_local( m, 4, "p", "Property" );
    add_local( m, 5, "result", "Cmd" );
    add_local( m, 6, "v", "Local" );
    add_local( m, 7, "result", "Cmd" );
    add_local( m, 8, "p", "Property" );
    m.String_literals = new BardString[3];    m.String_literals[0] = new BardString( type_String, "TODO: CmdAccess::resolve with primitive context" );
    m.String_literals[1] = new BardString( type_String, "TODO: check for Object::[TypeName]." );
    m.String_literals[2] = new BardString( type_String, "TODO: CmdAccess::resolve()" );
    m = add_method( "CmdAccess", "resolve_assignment", 0, 1, 5,"Cmd", code, 16225 );
    add_parameter( m, 0, "new_value", "Cmd" );
    add_local( m, 1, "operand_type", "Type" );
    add_local( m, 2, "p", "Property" );
    add_local( m, 3, "v", "Local" );
    add_local( m, 4, "p", "Property" );
    m.String_literals = new BardString[2];    m.String_literals[0] = new BardString( type_String, "Cannot find property in CmdAccess::resolve_assignment().  TODO: add check for implicit write method." );
    m.String_literals[1] = new BardString( type_String, "TODO: CmdAccess" );
    m = add_method( "CmdAccess", "init", 0, 2, 2,"CmdAccess", code, 16385 );
    add_parameter( m, 0, "_t", "Token" );
    add_parameter( m, 1, "_name", "String" );
    m = add_method( "CmdAccess", "init", 0, 3, 3,"CmdAccess", code, 16399 );
    add_parameter( m, 0, "_t", "Token" );
    add_parameter( m, 1, "_name", "String" );
    add_parameter( m, 2, "_args", "CmdArgs" );
    m = add_method( "CmdAccess", "init", 0, 4, 4,"CmdAccess", code, 16418 );
    add_parameter( m, 0, "_t", "Token" );
    add_parameter( m, 1, "_operand", "Cmd" );
    add_parameter( m, 2, "_name", "String" );
    add_parameter( m, 3, "_args", "CmdArgs" );
    m = add_method( "Templates", "init_object", 0, 0, 0,"Templates", code, 16442 );
    m = add_method( "Templates", "add", 0, 1, 1,null, code, 16450 );
    add_parameter( m, 0, "t", "Template" );
    m = add_method( "Templates", "find", 0, 1, 2,"Template", code, 16465 );
    add_parameter( m, 0, "name", "String" );
    add_local( m, 1, "i", "Integer" );
    m = add_method( "Templates", "instantiate", 0, 2, 3,null, code, 16517 );
    add_parameter( m, 0, "type", "TypeDef" );
    add_parameter( m, 1, "t", "Token" );
    add_local( m, 2, "temp", "Template" );
    m.String_literals = new BardString[2];    m.String_literals[0] = new BardString( type_String, "Reference to undefined type '" );
    m.String_literals[1] = new BardString( type_String, "'." );
    m = add_method( "Types", "init_object", 0, 0, 0,"Types", code, 16603 );
    m = add_method( "Types", "find", 0, 2, 3,"Type", code, 16607 );
    add_parameter( m, 0, "name", "String" );
    add_parameter( m, 1, "unchecked", "Logical" );
    add_local( m, 2, "def", "TypeDef" );
    m = add_method( "Types", "must_find", 0, 3, 4,"Type", code, 16643 );
    add_parameter( m, 0, "t", "Token" );
    add_parameter( m, 1, "name", "String" );
    add_parameter( m, 2, "unchecked", "Logical" );
    add_local( m, 3, "def", "TypeDef" );
    m = add_method( "TypeDefs", "init_object", 0, 0, 0,"TypeDefs", code, 16672 );
    m = add_method( "TypeDefs", "init", 0, 0, 2,"TypeDefs", code, 16680 );
    add_local( m, 0, "t", "Token" );
    add_local( m, 1, "m_to_String", "Method" );
    m.String_literals = new BardString[27];    m.String_literals[0] = new BardString( type_String, "[Internal]" );
    m.String_literals[1] = new BardString( type_String, "Object" );
    m.String_literals[2] = new BardString( type_String, "String" );
    m.String_literals[3] = new BardString( type_String, "Real" );
    m.String_literals[4] = new BardString( type_String, "Integer" );
    m.String_literals[5] = new BardString( type_String, "Character" );
    m.String_literals[6] = new BardString( type_String, "Logical" );
    m.String_literals[7] = new BardString( type_String, "RealProperty" );
    m.String_literals[8] = new BardString( type_String, "IntegerProperty" );
    m.String_literals[9] = new BardString( type_String, "CharacterProperty" );
    m.String_literals[10] = new BardString( type_String, "LogicalProperty" );
    m.String_literals[11] = new BardString( type_String, "Variant" );
    m.String_literals[12] = new BardString( type_String, "null" );
    m.String_literals[13] = new BardString( type_String, "RealObject" );
    m.String_literals[14] = new BardString( type_String, "IntegerObject" );
    m.String_literals[15] = new BardString( type_String, "CharacterObject" );
    m.String_literals[16] = new BardString( type_String, "LogicalObject" );
    m.String_literals[17] = new BardString( type_String, "String" );
    m.String_literals[18] = new BardString( type_String, "Integer[]" );
    m.String_literals[19] = new BardString( type_String, "Character[]" );
    m.String_literals[20] = new BardString( type_String, "Byte[]" );
    m.String_literals[21] = new BardString( type_String, "Value" );
    m.String_literals[22] = new BardString( type_String, "Global" );
    m.String_literals[23] = new BardString( type_String, "Collection" );
    m.String_literals[24] = new BardString( type_String, "List" );
    m.String_literals[25] = new BardString( type_String, "Table" );
    m.String_literals[26] = new BardString( type_String, "String" );
    m = add_method( "TypeDefs", "define", 0, 3, 4,"TypeDef", code, 17118 );
    add_parameter( m, 0, "t", "Token" );
    add_parameter( m, 1, "name", "String" );
    add_parameter( m, 2, "qualifiers", "Integer" );
    add_local( m, 3, "type", "TypeDef" );
    m = add_method( "TypeDefs", "resolve", 0, 0, 4,null, code, 17144 );
    add_local( m, 0, "index", "Integer" );
    add_local( m, 1, "collection:1", "TypeDefs" );
    add_local( m, 2, "counter:1", "Integer" );
    add_local( m, 3, "type", "TypeDef" );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "Main" );
    m = add_method( "TypeDefs", "compile", 0, 1, 4,null, code, 17222 );
    add_parameter( m, 0, "writer", "JavaVMWriter" );
    add_local( m, 1, "collection:1", "TypeDefs" );
    add_local( m, 2, "counter:1", "Integer" );
    add_local( m, 3, "type", "TypeDef" );
    m = add_method( "TypeDefs", "count", 0, 0, 0,"Integer", code, 17265 );
    m = add_method( "TypeDefs", "get", 0, 1, 1,"TypeDef", code, 17272 );
    add_parameter( m, 0, "index", "Integer" );
    m = add_method( "TypeDefs", "get", 0, 2, 4,"TypeDef", code, 17286 );
    add_parameter( m, 0, "t", "Token" );
    add_parameter( m, 1, "name", "String" );
    add_local( m, 2, "type", "TypeDef" );
    add_local( m, 3, "type_def", "TypeDef" );
    m = add_method( "TypeDefs", "find", 0, 1, 4,"TypeDef", code, 17333 );
    add_parameter( m, 0, "name", "String" );
    add_local( m, 1, "result", "TypeDef" );
    add_local( m, 2, "temp", "Template" );
    add_local( m, 3, "type_def", "TypeDef" );
    m = add_method( "TypeDefs", "must_find", 0, 2, 3,"TypeDef", code, 17404 );
    add_parameter( m, 0, "t", "Token" );
    add_parameter( m, 1, "name", "String" );
    add_local( m, 2, "type", "TypeDef" );
    m.String_literals = new BardString[2];    m.String_literals[0] = new BardString( type_String, "No such type: " );
    m.String_literals[1] = new BardString( type_String, "Can not find main type " );
    m = add_method( "TypeDefs", "collect_ids", 0, 0, 5,null, code, 17505 );
    add_local( m, 0, "i", "Integer" );
    add_local( m, 1, "type", "TypeDef" );
    add_local( m, 2, "collection:2", "List" );
    add_local( m, 3, "counter:2", "Integer" );
    add_local( m, 4, "v", "Variant" );
    m = add_method( "JavaVMWriter", "init_object", 0, 0, 0,"JavaVMWriter", code, 17589 );
    m = add_method( "JavaVMWriter", "init", 0, 0, 0,"JavaVMWriter", code, 17608 );
    m = add_method( "JavaVMWriter", "on_method_started", 0, 1, 1,null, code, 17612 );
    add_parameter( m, 0, "m", "Method" );
    m = add_method( "JavaVMWriter", "next_control_id", 0, 0, 0,"Integer", code, 17624 );
    m = add_method( "JavaVMWriter", "write_address", 0, 1, 1,null, code, 17639 );
    add_parameter( m, 0, "label", "String" );
    m = add_method( "JavaVMWriter", "define_label", 0, 1, 1,null, code, 17668 );
    add_parameter( m, 0, "label", "String" );
    m = add_method( "JavaVMWriter", "on_method_finished", 0, 0, 0,null, code, 17691 );
    m = add_method( "JavaVMWriter", "backpatch", 0, 1, 5,null, code, 17720 );
    add_parameter( m, 0, "info", "BackpatchInfo" );
    add_local( m, 1, "label", "String" );
    add_local( m, 2, "collection:2", "List" );
    add_local( m, 3, "counter:2", "Integer" );
    add_local( m, 4, "def", "Variant" );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "Unresolved backpatch: " );
    m = add_method( "JavaVMWriter", "write", 0, 1, 1,null, code, 17824 );
    add_parameter( m, 0, "n", "Integer" );
    m = add_method( "JavaVMWriter", "save", 0, 1, 52,null, code, 17837 );
    add_parameter( m, 0, "program_name", "String" );
    add_local( m, 1, "i", "Integer" );
    add_local( m, 2, "code_buffer", "Integer[]" );
    add_local( m, 3, "collection:3", "TypeDefs" );
    add_local( m, 4, "counter:3", "Integer" );
    add_local( m, 5, "type", "TypeDef" );
    add_local( m, 6, "collection:6", "List" );
    add_local( m, 7, "counter:6", "Integer" );
    add_local( m, 8, "m_v", "Variant" );
    add_local( m, 9, "m", "Method" );
    add_local( m, 10, "first", "Logical" );
    add_local( m, 11, "collection:11", "List" );
    add_local( m, 12, "counter:11", "Integer" );
    add_local( m, 13, "name", "Variant" );
    add_local( m, 14, "collection:14", "TypeDefs" );
    add_local( m, 15, "counter:14", "Integer" );
    add_local( m, 16, "type", "TypeDef" );
    add_local( m, 17, "collection:17", "TypeDefs" );
    add_local( m, 18, "counter:17", "Integer" );
    add_local( m, 19, "type", "TypeDef" );
    add_local( m, 20, "collection:20", "List" );
    add_local( m, 21, "counter:20", "Integer" );
    add_local( m, 22, "base_type_v", "Variant" );
    add_local( m, 23, "base_type", "TypeDef" );
    add_local( m, 24, "collection:24", "List" );
    add_local( m, 25, "counter:24", "Integer" );
    add_local( m, 26, "property_v", "Variant" );
    add_local( m, 27, "property", "Property" );
    add_local( m, 28, "collection:28", "TypeDefs" );
    add_local( m, 29, "counter:28", "Integer" );
    add_local( m, 30, "type", "TypeDef" );
    add_local( m, 31, "collection:31", "List" );
    add_local( m, 32, "counter:31", "Integer" );
    add_local( m, 33, "m_v", "Variant" );
    add_local( m, 34, "m", "Method" );
    add_local( m, 35, "i", "Integer" );
    add_local( m, 36, "collection:36", "List" );
    add_local( m, 37, "counter:36", "Integer" );
    add_local( m, 38, "e", "Variant" );
    add_local( m, 39, "v", "Local" );
    add_local( m, 40, "collection:40", "List" );
    add_local( m, 41, "counter:40", "Integer" );
    add_local( m, 42, "st_v", "Variant" );
    add_local( m, 43, "first", "Logical" );
    add_local( m, 44, "collection:44", "List" );
    add_local( m, 45, "counter:44", "Integer" );
    add_local( m, 46, "v", "Variant" );
    add_local( m, 47, "high", "Integer" );
    add_local( m, 48, "low", "Integer" );
    add_local( m, 49, "code_index", "Integer" );
    add_local( m, 50, "first", "Logical" );
    add_local( m, 51, "i", "Integer" );
    m.String_literals = new BardString[77];    m.String_literals[0] = new BardString( type_String, ".java" );
    m.String_literals[1] = new BardString( type_String, "import vm.*;" );
    m.String_literals[2] = new BardString( type_String, "public class " );
    m.String_literals[3] = new BardString( type_String, " extends BardProgram" );
    m.String_literals[4] = new BardString( type_String, "  static public void main( String[] args )\012  {" );
    m.String_literals[5] = new BardString( type_String, "    new " );
    m.String_literals[6] = new BardString( type_String, "().launch();" );
    m.String_literals[7] = new BardString( type_String, "  }\012" );
    m.String_literals[8] = new BardString( type_String, "  public " );
    m.String_literals[9] = new BardString( type_String, "()" );
    m.String_literals[10] = new BardString( type_String, "  {\012    init();\012  }\012  \012  public void define_id_table()\012  {\012    vm.id_table.configure(\012      new String[]\012      {" );
    m.String_literals[11] = new BardString( type_String, "        \"" );
    m.String_literals[12] = new BardString( type_String, ",\012        \"" );
    m.String_literals[13] = new BardString( type_String, "      }" );
    m.String_literals[14] = new BardString( type_String, "    );" );
    m.String_literals[15] = new BardString( type_String, "  }" );
    m.String_literals[16] = new BardString( type_String, "  public void define_types()" );
    m.String_literals[17] = new BardString( type_String, "  {" );
    m.String_literals[18] = new BardString( type_String, "    define_type( \"" );
    m.String_literals[19] = new BardString( type_String, "\", " );
    m.String_literals[20] = new BardString( type_String, ", " );
    m.String_literals[21] = new BardString( type_String, " );" );
    m.String_literals[22] = new BardString( type_String, "    add_base_type( \"" );
    m.String_literals[23] = new BardString( type_String, "\", \"" );
    m.String_literals[24] = new BardString( type_String, "\" );" );
    m.String_literals[25] = new BardString( type_String, "    add_property( \"" );
    m.String_literals[26] = new BardString( type_String, "\", " );
    m.String_literals[27] = new BardString( type_String, ", \"" );
    m.String_literals[28] = new BardString( type_String, "\", \"" );
    m.String_literals[29] = new BardString( type_String, "\" );" );
    m.String_literals[30] = new BardString( type_String, "  }" );
    m.String_literals[31] = new BardString( type_String, "  public void define_methods()" );
    m.String_literals[32] = new BardString( type_String, "  {" );
    m.String_literals[33] = new BardString( type_String, "    vm.type_String = vm.must_find_type( \"String\" );" );
    m.String_literals[34] = new BardString( type_String, "    add_method( \"" );
    m.String_literals[35] = new BardString( type_String, "\", \"" );
    m.String_literals[36] = new BardString( type_String, "\", " );
    m.String_literals[37] = new BardString( type_String, ", " );
    m.String_literals[38] = new BardString( type_String, ", " );
    m.String_literals[39] = new BardString( type_String, ", " );
    m.String_literals[40] = new BardString( type_String, "\"," );
    m.String_literals[41] = new BardString( type_String, "null," );
    m.String_literals[42] = new BardString( type_String, " code, " );
    m.String_literals[43] = new BardString( type_String, ", " );
    m.String_literals[44] = new BardString( type_String, " );" );
    m.String_literals[45] = new BardString( type_String, "    add_parameter( " );
    m.String_literals[46] = new BardString( type_String, ", \"" );
    m.String_literals[47] = new BardString( type_String, "\", \"" );
    m.String_literals[48] = new BardString( type_String, "\" );" );
    m.String_literals[49] = new BardString( type_String, "    add_local( " );
    m.String_literals[50] = new BardString( type_String, ", \"" );
    m.String_literals[51] = new BardString( type_String, "\", \"" );
    m.String_literals[52] = new BardString( type_String, "\" );" );
    m.String_literals[53] = new BardString( type_String, "    add_String_literal( " );
    m.String_literals[54] = new BardString( type_String, ", vm.create_String(\"" );
    m.String_literals[55] = new BardString( type_String, "\") );" );
    m.String_literals[56] = new BardString( type_String, "    working_method.Real_literals = to_double_array( new long[]{ " );
    m.String_literals[57] = new BardString( type_String, ", " );
    m.String_literals[58] = new BardString( type_String, "0x" );
    m.String_literals[59] = new BardString( type_String, " } );" );
    m.String_literals[60] = new BardString( type_String, "  }" );
    m.String_literals[61] = new BardString( type_String, "  public " );
    m.String_literals[62] = new BardString( type_String, " init()" );
    m.String_literals[63] = new BardString( type_String, "  {" );
    m.String_literals[64] = new BardString( type_String, "    BardMethod.use_return_types_in_method_signatures = true;" );
    m.String_literals[65] = new BardString( type_String, "    vm = create_vm();" );
    m.String_literals[66] = new BardString( type_String, "    set_up_code( code_strings, " );
    m.String_literals[67] = new BardString( type_String, " );" );
    m.String_literals[68] = new BardString( type_String, "    define_id_table();" );
    m.String_literals[69] = new BardString( type_String, "    define_types();" );
    m.String_literals[70] = new BardString( type_String, "    define_methods();" );
    m.String_literals[71] = new BardString( type_String, "    return this;" );
    m.String_literals[72] = new BardString( type_String, "  }" );
    m.String_literals[73] = new BardString( type_String, "  public String[] code_strings = new String[]" );
    m.String_literals[74] = new BardString( type_String, "  {" );
    m.String_literals[75] = new BardString( type_String, "    \"" );
    m.String_literals[76] = new BardString( type_String, "  };" );
    m = add_method( "JavaVMWriter", "print_hex", 0, 2, 2,null, code, 20212 );
    add_parameter( m, 0, "n", "Integer" );
    add_parameter( m, 1, "min_digits", "Integer" );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "0123456789abcdef" );
    m = add_method( "JavaVMWriter", "to_octal_String", 0, 1, 1,"String", code, 20263 );
    add_parameter( m, 0, "n", "Integer" );
    m.String_literals = new BardString[8];    m.String_literals[0] = new BardString( type_String, "\\t" );
    m.String_literals[1] = new BardString( type_String, "\\n" );
    m.String_literals[2] = new BardString( type_String, "\\r" );
    m.String_literals[3] = new BardString( type_String, "\\\\" );
    m.String_literals[4] = new BardString( type_String, "\\\"" );
    m.String_literals[5] = new BardString( type_String, "" );
    m.String_literals[6] = new BardString( type_String, "" );
    m.String_literals[7] = new BardString( type_String, "" );
    m = add_method( "JavaVMWriter", "print_byte_string", 0, 1, 1,null, code, 20461 );
    add_parameter( m, 0, "n", "Integer" );
    m = add_method( "JavaVMWriter", "print_octal_encoding", 0, 1, 1,null, code, 20654 );
    add_parameter( m, 0, "n", "Integer" );
    m = add_method( "IDTable", "init_object", 0, 0, 0,"IDTable", code, 20677 );
    m = add_method( "IDTable", "List", 0, 0, 0,"List", code, 20690 );
    m = add_method( "IDTable", "add", 0, 1, 2,"Integer", code, 20696 );
    add_parameter( m, 0, "name", "String" );
    add_local( m, 1, "id", "Integer" );
    m = add_method( "IDTable", "get_id", 0, 1, 1,"Integer", code, 20748 );
    add_parameter( m, 0, "name", "String" );
    m = add_method( "Qualifiers", "init_object", 0, 0, 0,"Qualifiers", code, 20757 );
    m = add_method( "Property", "init_object", 0, 0, 0,"Property", code, 20781 );
    m = add_method( "Property", "init", 0, 3, 3,"Property", code, 20785 );
    add_parameter( m, 0, "_t", "Token" );
    add_parameter( m, 1, "_type_context", "TypeDef" );
    add_parameter( m, 2, "_name", "String" );
    m = add_method( "Property", "organize", 0, 0, 0,null, code, 20804 );
    m = add_method( "Property", "resolve", 0, 0, 0,null, code, 20819 );
    m = add_method( "Property", "duplicate", 0, 2, 3,"Property", code, 20836 );
    add_parameter( m, 0, "_type_context", "TypeDef" );
    add_parameter( m, 1, "_index", "Integer" );
    add_local( m, 2, "result", "Property" );
    m = add_method( "Local", "init_object", 0, 0, 0,"Local", code, 20883 );
    m = add_method( "Local", "String", 0, 0, 0,"String", code, 20887 );
    m = add_method( "Local", "init", 0, 2, 2,"Local", code, 20930 );
    add_parameter( m, 0, "_t", "Token" );
    add_parameter( m, 1, "_name", "String" );
    m = add_method( "Local", "init", 0, 3, 3,"Local", code, 20944 );
    add_parameter( m, 0, "_t", "Token" );
    add_parameter( m, 1, "_name", "String" );
    add_parameter( m, 2, "_type", "Type" );
    m = add_method( "Local", "organize", 0, 0, 0,null, code, 20963 );
    m = add_method( "Local", "resolve", 0, 0, 0,null, code, 20971 );
    m = add_method( "Local", "offset", 0, 0, 0,"Integer", code, 20982 );
    m = add_method( "Analyzer", "init_object", 0, 0, 0,"Analyzer", code, 20992 );
    m = add_method( "Analyzer", "push_method_context", 0, 1, 4,null, code, 21039 );
    add_parameter( m, 0, "m", "Method" );
    add_local( m, 1, "collection:1", "List" );
    add_local( m, 2, "counter:1", "Integer" );
    add_local( m, 3, "v", "Variant" );
    m = add_method( "Analyzer", "pop_method_context", 0, 0, 0,null, code, 21120 );
    m = add_method( "Analyzer", "push_local_scope", 0, 0, 0,null, code, 21158 );
    m = add_method( "Analyzer", "pop_local_scope", 0, 0, 2,null, code, 21170 );
    add_local( m, 0, "x", "Object" );
    add_local( m, 1, "target_count", "Integer" );
    m.String_literals = new BardString[2];    m.String_literals[0] = new BardString( type_String, "y" );
    m.String_literals[1] = new BardString( type_String, "[Internal Error] No local scope to pop in Analyzer." );
    m = add_method( "Analyzer", "find_local", 0, 1, 3,"Local", code, 21225 );
    add_parameter( m, 0, "name", "String" );
    add_local( m, 1, "i", "Integer" );
    add_local( m, 2, "v", "Local" );
    m = add_method( "Analyzer", "apply_implicit_return_type", 0, 1, 1,null, code, 21279 );
    add_parameter( m, 0, "implicit_type", "Type" );
    m = add_method( "Analyzer", "resolve_call", 0, 7, 8,"Cmd", code, 21355 );
    add_parameter( m, 0, "t", "Token" );
    add_parameter( m, 1, "type_context", "Type" );
    add_parameter( m, 2, "context", "Cmd" );
    add_parameter( m, 3, "name", "String" );
    add_parameter( m, 4, "args", "CmdArgs" );
    add_parameter( m, 5, "return_type_hint", "Type" );
    add_parameter( m, 6, "error_on_fail", "Logical" );
    add_local( m, 7, "m", "Method" );
    m = add_method( "Analyzer", "find_method", 0, 6, 27,"Method", code, 21418 );
    add_parameter( m, 0, "t", "Token" );
    add_parameter( m, 1, "type_context", "Type" );
    add_parameter( m, 2, "name", "String" );
    add_parameter( m, 3, "args", "CmdArgs" );
    add_parameter( m, 4, "return_type_hint", "Type" );
    add_parameter( m, 5, "error_on_fail", "Logical" );
    add_local( m, 6, "original_args", "CmdArgs" );
    add_local( m, 7, "collection:7", "List" );
    add_local( m, 8, "counter:7", "Integer" );
    add_local( m, 9, "mv", "Variant" );
    add_local( m, 10, "m", "Method" );
    add_local( m, 11, "collection:11", "List" );
    add_local( m, 12, "counter:11", "Integer" );
    add_local( m, 13, "mv", "Variant" );
    add_local( m, 14, "m", "Method" );
    add_local( m, 15, "all_match", "Logical" );
    add_local( m, 16, "m_returning_Object", "Method" );
    add_local( m, 17, "sig", "String" );
    add_local( m, 18, "collection:18", "List" );
    add_local( m, 19, "counter:18", "Integer" );
    add_local( m, 20, "mv", "Variant" );
    add_local( m, 21, "m", "Method" );
    add_local( m, 22, "buffer", "Character[]" );
    add_local( m, 23, "first", "Logical" );
    add_local( m, 24, "collection:24", "CmdArgs" );
    add_local( m, 25, "counter:24", "Integer" );
    add_local( m, 26, "arg", "Cmd" );
    m.String_literals = new BardString[5];    m.String_literals[0] = new BardString( type_String, "Multiple matches for call to " );
    m.String_literals[1] = new BardString( type_String, "No such " );
    m.String_literals[2] = new BardString( type_String, "method " );
    m.String_literals[3] = new BardString( type_String, "property or method " );
    m.String_literals[4] = new BardString( type_String, " in type " );
    m = add_method( "CmdArgs", "init_object", 0, 0, 0,"CmdArgs", code, 22007 );
    m = add_method( "CmdArgs", "init", 0, 0, 0,"CmdArgs", code, 22016 );
    m = add_method( "CmdArgs", "init", 0, 1, 1,"CmdArgs", code, 22020 );
    add_parameter( m, 0, "cmd", "Cmd" );
    m = add_method( "CmdArgs", "init", 0, 2, 2,"CmdArgs", code, 22032 );
    add_parameter( m, 0, "cmd1", "Cmd" );
    add_parameter( m, 1, "cmd2", "Cmd" );
    m = add_method( "CmdArgs", "dispatch", 0, 1, 4,null, code, 22052 );
    add_parameter( m, 0, "v", "Visitor" );
    add_local( m, 1, "collection:1", "CmdArgs" );
    add_local( m, 2, "counter:1", "Integer" );
    add_local( m, 3, "arg", "Cmd" );
    m = add_method( "CmdArgs", "count", 0, 0, 0,"Integer", code, 22099 );
    m = add_method( "CmdArgs", "get", 0, 1, 1,"Cmd", code, 22106 );
    add_parameter( m, 0, "index", "Integer" );
    m = add_method( "CmdArgs", "add", 0, 1, 1,"CmdArgs", code, 22118 );
    add_parameter( m, 0, "cmd", "Cmd" );
    m = add_method( "CmdArgs", "resolve", 0, 0, 1,"Cmd", code, 22130 );
    add_local( m, 0, "i", "Integer" );
    m = add_method( "CmdArgs", "cast_to", 0, 1, 5,null, code, 22168 );
    add_parameter( m, 0, "parameters", "List" );
    add_local( m, 1, "i", "Integer" );
    add_local( m, 2, "collection:2", "CmdArgs" );
    add_local( m, 3, "counter:2", "Integer" );
    add_local( m, 4, "arg", "Cmd" );
    m = add_method( "CmdArgs", "compile", 0, 1, 4,null, code, 22232 );
    add_parameter( m, 0, "writer", "JavaVMWriter" );
    add_local( m, 1, "collection:1", "CmdArgs" );
    add_local( m, 2, "counter:1", "Integer" );
    add_local( m, 3, "cmd", "Cmd" );
    m = add_method( "Visitor", "init_object", 0, 0, 0,"Visitor", code, 22271 );
    m = add_method( "Visitor", "visit", 0, 1, 1,null, code, 22275 );
    add_parameter( m, 0, "cmd", "Cmd" );
    m = add_method( "ImplicitReturnTypeVisitor", "init_object", 0, 0, 0,"ImplicitReturnTypeVisitor", code, 22277 );
    m = add_method( "ImplicitReturnTypeVisitor", "visit", 0, 1, 1,null, code, 22285 );
    add_parameter( m, 0, "cmd", "Cmd" );
    m = add_method( "CmdReturnValue", "init_object", 0, 0, 0,"CmdReturnValue", code, 22304 );
    m = add_method( "CmdReturnValue", "String", 0, 0, 0,"String", code, 22308 );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "return " );
    m = add_method( "CmdReturnValue", "determine_implicit_return_type", 0, 0, 0,null, code, 22334 );
    m = add_method( "CmdReturnValue", "resolve", 0, 0, 1,"Cmd", code, 22347 );
    add_local( m, 0, "return_type", "Type" );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "Method is not declared as returning a value." );
    m = add_method( "CmdReturnValue", "compile", 0, 1, 2,null, code, 22418 );
    add_parameter( m, 0, "writer", "JavaVMWriter" );
    add_local( m, 1, "return_type", "Type" );
    m = add_method( "CmdLiteral", "init_object", 0, 0, 0,"CmdLiteral", code, 22495 );
    m = add_method( "CmdLiteral", "implicit_type", 0, 0, 0,"Type", code, 22499 );
    m = add_method( "CmdLiteral", "resolve", 0, 0, 0,"Cmd", code, 22506 );
    m = add_method( "CmdLiteralReal", "init_object", 0, 0, 0,"CmdLiteralReal", code, 22510 );
    m = add_method( "CmdLiteralReal", "String", 0, 0, 0,"String", code, 22514 );
    m = add_method( "CmdLiteralReal", "type", 0, 0, 0,"Type", code, 22531 );
    m = add_method( "CmdLiteralReal", "cast_to", 0, 2, 3,"Cmd", code, 22540 );
    add_parameter( m, 0, "cast_t", "Token" );
    add_parameter( m, 1, "to_type", "Type" );
    add_local( m, 2, "new_value", "Integer" );
    m.String_literals = new BardString[4];    m.String_literals[0] = new BardString( type_String, "Invalid conversion from Real to Character." );
    m.String_literals[1] = new BardString( type_String, "Invalid conversion from Real to Logical.  Use \"value?\" instead." );
    m.String_literals[2] = new BardString( type_String, "" );
    m.String_literals[3] = new BardString( type_String, "TODO: CmdLiteralReal::cast_to()." );
    m = add_method( "CmdLiteralReal", "compile", 0, 1, 1,null, code, 22667 );
    add_parameter( m, 0, "writer", "JavaVMWriter" );
    m = add_method( "CmdLiteralReal", "init", 0, 2, 2,"CmdLiteralReal", code, 22702 );
    add_parameter( m, 0, "_t", "Token" );
    add_parameter( m, 1, "_value", "Real" );
    m = add_method( "CmdLiteralInteger", "init_object", 0, 0, 0,"CmdLiteralInteger", code, 22716 );
    m = add_method( "CmdLiteralInteger", "String", 0, 0, 0,"String", code, 22720 );
    m = add_method( "CmdLiteralInteger", "type", 0, 0, 0,"Type", code, 22737 );
    m = add_method( "CmdLiteralInteger", "cast_to", 0, 2, 2,"Cmd", code, 22746 );
    add_parameter( m, 0, "cast_t", "Token" );
    add_parameter( m, 1, "to_type", "Type" );
    m.String_literals = new BardString[4];    m.String_literals[0] = new BardString( type_String, "Invalid conversion from Integer to Logical.  Use \"value?\" instead." );
    m.String_literals[1] = new BardString( type_String, "" );
    m.String_literals[2] = new BardString( type_String, "IntegerObject" );
    m.String_literals[3] = new BardString( type_String, "Invalid cast from Integer to " );
    m = add_method( "CmdLiteralInteger", "compile", 0, 1, 1,null, code, 22962 );
    add_parameter( m, 0, "writer", "JavaVMWriter" );
    m = add_method( "CmdLiteralInteger", "init", 0, 2, 2,"CmdLiteralInteger", code, 22981 );
    add_parameter( m, 0, "_t", "Token" );
    add_parameter( m, 1, "_value", "Integer" );
    m = add_method( "CmdLiteralCharacter", "init_object", 0, 0, 0,"CmdLiteralCharacter", code, 22995 );
    m = add_method( "CmdLiteralCharacter", "String", 0, 0, 0,"String", code, 22999 );
    m = add_method( "CmdLiteralCharacter", "type", 0, 0, 0,"Type", code, 23016 );
    m = add_method( "CmdLiteralCharacter", "cast_to", 0, 2, 2,"Cmd", code, 23025 );
    add_parameter( m, 0, "cast_t", "Token" );
    add_parameter( m, 1, "to_type", "Type" );
    m.String_literals = new BardString[4];    m.String_literals[0] = new BardString( type_String, "Invalid conversion from Character to Real." );
    m.String_literals[1] = new BardString( type_String, "Invalid conversion from Character to Logical.  Use \"ch?\" instead." );
    m.String_literals[2] = new BardString( type_String, "" );
    m.String_literals[3] = new BardString( type_String, "TODO: CmdLiteralCharacter::cast_to()." );
    m = add_method( "CmdLiteralCharacter", "compile", 0, 1, 1,null, code, 23147 );
    add_parameter( m, 0, "writer", "JavaVMWriter" );
    m = add_method( "CmdLiteralCharacter", "init", 0, 2, 2,"CmdLiteralCharacter", code, 23166 );
    add_parameter( m, 0, "_t", "Token" );
    add_parameter( m, 1, "_value", "Character" );
    m = add_method( "CmdLiteralLogical", "init_object", 0, 0, 0,"CmdLiteralLogical", code, 23180 );
    m = add_method( "CmdLiteralLogical", "String", 0, 0, 0,"String", code, 23184 );
    m.String_literals = new BardString[2];    m.String_literals[0] = new BardString( type_String, "true" );
    m.String_literals[1] = new BardString( type_String, "false" );
    m = add_method( "CmdLiteralLogical", "type", 0, 0, 0,"Type", code, 23199 );
    m = add_method( "CmdLiteralLogical", "cast_to", 0, 2, 2,"Cmd", code, 23208 );
    add_parameter( m, 0, "cast_t", "Token" );
    add_parameter( m, 1, "to_type", "Type" );
    m.String_literals = new BardString[6];    m.String_literals[0] = new BardString( type_String, "Invalid conversion from Logical to Real." );
    m.String_literals[1] = new BardString( type_String, "Invalid conversion from Logical to Integer." );
    m.String_literals[2] = new BardString( type_String, "Invalid conversion from Logical to Character." );
    m.String_literals[3] = new BardString( type_String, "true" );
    m.String_literals[4] = new BardString( type_String, "false" );
    m.String_literals[5] = new BardString( type_String, "TODO: CmdLiteralLogical::cast_to()." );
    m = add_method( "CmdLiteralLogical", "compile", 0, 1, 1,null, code, 23314 );
    add_parameter( m, 0, "writer", "JavaVMWriter" );
    m = add_method( "CmdLiteralLogical", "init", 0, 2, 2,"CmdLiteralLogical", code, 23346 );
    add_parameter( m, 0, "_t", "Token" );
    add_parameter( m, 1, "_value", "Logical" );
    m = add_method( "CmdLiteralString", "init_object", 0, 0, 0,"CmdLiteralString", code, 23360 );
    m = add_method( "CmdLiteralString", "String", 0, 0, 0,"String", code, 23364 );
    m = add_method( "CmdLiteralString", "type", 0, 0, 0,"Type", code, 23393 );
    m = add_method( "CmdLiteralString", "compile", 0, 1, 1,null, code, 23402 );
    add_parameter( m, 0, "writer", "JavaVMWriter" );
    m = add_method( "CmdLiteralString", "init", 0, 2, 2,"CmdLiteralString", code, 23437 );
    add_parameter( m, 0, "_t", "Token" );
    add_parameter( m, 1, "_value", "String" );
    m = add_method( "CmdLiteralNull", "init_object", 0, 0, 0,"CmdLiteralNull", code, 23451 );
    m = add_method( "CmdLiteralNull", "String", 0, 0, 0,"String", code, 23455 );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "null" );
    m = add_method( "CmdLiteralNull", "init", 0, 1, 1,"CmdLiteralNull", code, 23460 );
    add_parameter( m, 0, "_t", "Token" );
    m = add_method( "CmdLiteralNull", "type", 0, 0, 0,"Type", code, 23478 );
    m = add_method( "CmdLiteralNull", "resolve", 0, 0, 0,"Cmd", code, 23484 );
    m = add_method( "CmdLiteralNull", "require_value", 0, 1, 2,"Cmd", code, 23497 );
    add_parameter( m, 0, "allow_unchecked", "Logical" );
    add_local( m, 1, "_type", "Type" );
    m.String_literals = new BardString[2];    m.String_literals[0] = new BardString( type_String, "Value expected." );
    m.String_literals[1] = new BardString( type_String, "Non-null value required for method with a null-checked return type." );
    m = add_method( "CmdLiteralNull", "compile", 0, 1, 1,null, code, 23540 );
    add_parameter( m, 0, "writer", "JavaVMWriter" );
    m = add_method( "CmdUnary", "init_object", 0, 0, 0,"CmdUnary", code, 23551 );
    m = add_method( "CmdUnary", "dispatch", 0, 1, 1,null, code, 23561 );
    add_parameter( m, 0, "v", "Visitor" );
    m = add_method( "CmdUnary", "type", 0, 0, 0,"Type", code, 23577 );
    m = add_method( "CmdUnary", "resolve", 0, 0, 9,"Cmd", code, 23586 );
    add_local( m, 0, "operand_type", "Type" );
    add_local( m, 1, "result", "Cmd" );
    add_local( m, 2, "result", "Cmd" );
    add_local( m, 3, "result", "Cmd" );
    add_local( m, 4, "result", "Cmd" );
    add_local( m, 5, "result", "Cmd" );
    add_local( m, 6, "result", "Cmd" );
    add_local( m, 7, "result", "Cmd" );
    add_local( m, 8, "result", "Cmd" );
    m.String_literals = new BardString[3];    m.String_literals[0] = new BardString( type_String, "Unsupported operand type " );
    m.String_literals[1] = new BardString( type_String, " for '" );
    m.String_literals[2] = new BardString( type_String, "' operation." );
    m = add_method( "CmdUnary", "compile", 0, 1, 1,null, code, 23911 );
    add_parameter( m, 0, "writer", "JavaVMWriter" );
    m = add_method( "CmdUnary", "init", 0, 2, 2,"CmdUnary", code, 23929 );
    add_parameter( m, 0, "_t", "Token" );
    add_parameter( m, 1, "_operand", "Cmd" );
    m = add_method( "CmdUnary", "name", 0, 0, 0,"String", code, 23943 );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "[unnamed unary op]" );
    m = add_method( "CmdUnary", "resolve_for_literal_Real_operand", 0, 1, 1,"Cmd", code, 23948 );
    add_parameter( m, 0, "operand_value", "Real" );
    m = add_method( "CmdUnary", "resolve_for_literal_Integer_operand", 0, 1, 1,"Cmd", code, 23952 );
    add_parameter( m, 0, "operand_value", "Integer" );
    m = add_method( "CmdUnary", "resolve_for_literal_Logical_operand", 0, 1, 1,"Cmd", code, 23956 );
    add_parameter( m, 0, "operand_value", "Logical" );
    m = add_method( "CmdUnary", "resolve_for_literal_operand", 0, 0, 0,"Cmd", code, 23960 );
    m = add_method( "CmdUnary", "resolve_for_Real_operand", 0, 0, 0,"Cmd", code, 23964 );
    m = add_method( "CmdUnary", "resolve_for_Integer_operand", 0, 0, 0,"Cmd", code, 23968 );
    m = add_method( "CmdUnary", "resolve_for_Logical_operand", 0, 0, 0,"Cmd", code, 23972 );
    m = add_method( "CmdUnary", "resolve_for_miscellaneous_operand", 0, 0, 0,"Cmd", code, 23976 );
    m = add_method( "CmdLogicalNot", "init_object", 0, 0, 0,"CmdLogicalNot", code, 23980 );
    m = add_method( "CmdLogicalNot", "implicit_type", 0, 0, 0,"Type", code, 23990 );
    m = add_method( "CmdLogicalNot", "name", 0, 0, 0,"String", code, 23999 );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "not" );
    m = add_method( "CmdLogicalNot", "resolve_for_literal_Real_operand", 0, 1, 1,"Cmd", code, 24004 );
    add_parameter( m, 0, "operand_value", "Real" );
    m = add_method( "CmdLogicalNot", "resolve_for_literal_Integer_operand", 0, 1, 1,"Cmd", code, 24008 );
    add_parameter( m, 0, "operand_value", "Integer" );
    m = add_method( "CmdLogicalNot", "resolve_for_literal_Logical_operand", 0, 1, 1,"Cmd", code, 24012 );
    add_parameter( m, 0, "operand_value", "Logical" );
    m = add_method( "CmdLogicalNot", "resolve_for_Real_operand", 0, 0, 0,"Cmd", code, 24026 );
    m = add_method( "CmdLogicalNot", "resolve_for_Integer_operand", 0, 0, 0,"Cmd", code, 24030 );
    m = add_method( "CmdLogicalNot", "resolve_for_Logical_operand", 0, 0, 0,"Cmd", code, 24034 );
    m = add_method( "CmdNegate", "init_object", 0, 0, 0,"CmdNegate", code, 24045 );
    m = add_method( "CmdNegate", "implicit_type", 0, 0, 0,"Type", code, 24055 );
    m = add_method( "CmdNegate", "name", 0, 0, 0,"String", code, 24064 );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "" );
    m = add_method( "CmdNegate", "resolve_for_literal_Real_operand", 0, 1, 1,"Cmd", code, 24086 );
    add_parameter( m, 0, "operand_value", "Real" );
    m = add_method( "CmdNegate", "resolve_for_literal_Integer_operand", 0, 1, 1,"Cmd", code, 24100 );
    add_parameter( m, 0, "operand_value", "Integer" );
    m = add_method( "CmdNegate", "resolve_for_literal_Logical_operand", 0, 1, 1,"Cmd", code, 24114 );
    add_parameter( m, 0, "operand_value", "Logical" );
    m = add_method( "CmdNegate", "resolve_for_Real_operand", 0, 0, 0,"Cmd", code, 24128 );
    m = add_method( "CmdNegate", "resolve_for_Integer_operand", 0, 0, 0,"Cmd", code, 24139 );
    m = add_method( "CmdNegate", "resolve_for_Logical_operand", 0, 0, 0,"Cmd", code, 24150 );
    m = add_method( "CmdBitwiseNot", "init_object", 0, 0, 0,"CmdBitwiseNot", code, 24161 );
    m = add_method( "CmdBitwiseNot", "implicit_type", 0, 0, 0,"Type", code, 24171 );
    m = add_method( "CmdBitwiseNot", "name", 0, 0, 0,"String", code, 24180 );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "" );
    m = add_method( "CmdBitwiseNot", "resolve_for_literal_Integer_operand", 0, 1, 1,"Cmd", code, 24202 );
    add_parameter( m, 0, "operand_value", "Integer" );
    m = add_method( "CmdBitwiseNot", "resolve_for_literal_Logical_operand", 0, 1, 1,"Cmd", code, 24206 );
    add_parameter( m, 0, "operand_value", "Logical" );
    m = add_method( "CmdBitwiseNot", "resolve_for_Integer_operand", 0, 0, 0,"Cmd", code, 24220 );
    m = add_method( "CmdBitwiseNot", "resolve_for_Logical_operand", 0, 0, 0,"Cmd", code, 24231 );
    m = add_method( "CmdLogicalize", "init_object", 0, 0, 0,"CmdLogicalize", code, 24242 );
    m = add_method( "CmdLogicalize", "implicit_type", 0, 0, 0,"Type", code, 24252 );
    m = add_method( "CmdLogicalize", "type", 0, 0, 0,"Type", code, 24261 );
    m = add_method( "CmdLogicalize", "name", 0, 0, 0,"String", code, 24270 );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "" );
    m = add_method( "CmdLogicalize", "resolve_for_literal_Real_operand", 0, 1, 1,"Cmd", code, 24292 );
    add_parameter( m, 0, "operand_value", "Real" );
    m = add_method( "CmdLogicalize", "resolve_for_literal_Integer_operand", 0, 1, 1,"Cmd", code, 24306 );
    add_parameter( m, 0, "operand_value", "Integer" );
    m = add_method( "CmdLogicalize", "resolve_for_Real_operand", 0, 0, 0,"Cmd", code, 24320 );
    m = add_method( "CmdLogicalize", "resolve_for_Integer_operand", 0, 0, 0,"Cmd", code, 24331 );
    m = add_method( "CmdLogicalize", "resolve_for_Logical_operand", 0, 0, 0,"Cmd", code, 24342 );
    m = add_method( "CmdLogicalize", "resolve_for_miscellaneous_operand", 0, 0, 1,"Cmd", code, 24348 );
    add_local( m, 0, "operand_type", "Type" );
    m = add_method( "CmdReadSingleton", "init_object", 0, 0, 0,"CmdReadSingleton", code, 24397 );
    m = add_method( "CmdReadSingleton", "implicit_type", 0, 0, 0,"Type", code, 24401 );
    m = add_method( "CmdReadSingleton", "type", 0, 0, 0,"Type", code, 24407 );
    m = add_method( "CmdReadSingleton", "resolve", 0, 0, 0,"Cmd", code, 24413 );
    m = add_method( "CmdReadSingleton", "compile", 0, 1, 1,null, code, 24426 );
    add_parameter( m, 0, "writer", "JavaVMWriter" );
    m = add_method( "CmdReadSingleton", "init", 0, 2, 2,"CmdReadSingleton", code, 24449 );
    add_parameter( m, 0, "_t", "Token" );
    add_parameter( m, 1, "_of_type", "Type" );
    m = add_method( "CmdReadProperty", "init_object", 0, 0, 0,"CmdReadProperty", code, 24463 );
    m = add_method( "CmdReadProperty", "dispatch", 0, 1, 1,null, code, 24467 );
    add_parameter( m, 0, "v", "Visitor" );
    m = add_method( "CmdReadProperty", "type", 0, 0, 0,"Type", code, 24491 );
    m = add_method( "CmdReadProperty", "variable_name", 0, 0, 0,"String", code, 24499 );
    m = add_method( "CmdReadProperty", "resolve", 0, 0, 0,"Cmd", code, 24507 );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "Unchecked reference access." );
    m = add_method( "CmdReadProperty", "compile", 0, 1, 3,null, code, 24549 );
    add_parameter( m, 0, "writer", "JavaVMWriter" );
    add_local( m, 1, "property_type", "Type" );
    add_local( m, 2, "opcode", "Integer" );
    m = add_method( "CmdReadProperty", "init", 0, 3, 3,"CmdReadProperty", code, 24670 );
    add_parameter( m, 0, "_t", "Token" );
    add_parameter( m, 1, "_context", "Cmd" );
    add_parameter( m, 2, "_property", "Property" );
    m = add_method( "CmdWriteProperty", "init_object", 0, 0, 0,"CmdWriteProperty", code, 24689 );
    m = add_method( "CmdWriteProperty", "dispatch", 0, 1, 1,null, code, 24693 );
    add_parameter( m, 0, "v", "Visitor" );
    m = add_method( "CmdWriteProperty", "type", 0, 0, 0,"Type", code, 24733 );
    m = add_method( "CmdWriteProperty", "resolve", 0, 0, 0,"Cmd", code, 24737 );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "' can only store null-checked references." );
    m = add_method( "CmdWriteProperty", "compile", 0, 1, 3,null, code, 24872 );
    add_parameter( m, 0, "writer", "JavaVMWriter" );
    add_local( m, 1, "property_type", "Type" );
    add_local( m, 2, "opcode", "Integer" );
    m = add_method( "CmdWriteProperty", "init", 0, 4, 4,"CmdWriteProperty", code, 25001 );
    add_parameter( m, 0, "_t", "Token" );
    add_parameter( m, 1, "_context", "Cmd" );
    add_parameter( m, 2, "_property", "Property" );
    add_parameter( m, 3, "_new_value", "Cmd" );
    m = add_method( "CmdNewObject", "init_object", 0, 0, 0,"CmdNewObject", code, 25025 );
    m = add_method( "CmdNewObject", "dispatch", 0, 1, 1,null, code, 25029 );
    add_parameter( m, 0, "v", "Visitor" );
    m = add_method( "CmdNewObject", "type", 0, 0, 0,"Type", code, 25053 );
    m = add_method( "CmdNewObject", "resolve", 0, 0, 0,"Cmd", code, 25059 );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "init" );
    m = add_method( "CmdNewObject", "compile", 0, 1, 4,null, code, 25133 );
    add_parameter( m, 0, "writer", "JavaVMWriter" );
    add_local( m, 1, "collection:1", "CmdArgs" );
    add_local( m, 2, "counter:1", "Integer" );
    add_local( m, 3, "arg", "Cmd" );
    m = add_method( "CmdNewObject", "init", 0, 3, 3,"CmdNewObject", code, 25256 );
    add_parameter( m, 0, "_t", "Token" );
    add_parameter( m, 1, "_of_type", "Type" );
    add_parameter( m, 2, "_args", "CmdArgs" );
    m = add_method( "CmdDynamicCall", "init_object", 0, 0, 0,"CmdDynamicCall", code, 25275 );
    m = add_method( "CmdDynamicCall", "dispatch", 0, 1, 1,null, code, 25279 );
    add_parameter( m, 0, "v", "Visitor" );
    m = add_method( "CmdDynamicCall", "type", 0, 0, 0,"Type", code, 25319 );
    m = add_method( "CmdDynamicCall", "variable_name", 0, 0, 0,"String", code, 25327 );
    m = add_method( "CmdDynamicCall", "resolve", 0, 0, 1,"Cmd", code, 25335 );
    add_local( m, 0, "context_type", "Type" );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "Unchecked reference access." );
    m = add_method( "CmdDynamicCall", "compile", 0, 1, 4,null, code, 25391 );
    add_parameter( m, 0, "writer", "JavaVMWriter" );
    add_local( m, 1, "collection:1", "CmdArgs" );
    add_local( m, 2, "counter:1", "Integer" );
    add_local( m, 3, "arg", "Cmd" );
    m = add_method( "CmdDynamicCall", "init", 0, 4, 4,"CmdDynamicCall", code, 25487 );
    add_parameter( m, 0, "_t", "Token" );
    add_parameter( m, 1, "_content", "Cmd" );
    add_parameter( m, 2, "m", "Method" );
    add_parameter( m, 3, "_args", "CmdArgs" );
    m = add_method( "CmdBox", "init_object", 0, 0, 0,"CmdBox", code, 25527 );
    m = add_method( "CmdStatement", "init_object", 0, 0, 0,"CmdStatement", code, 25537 );
    m = add_method( "CmdStatement", "type", 0, 0, 0,"Type", code, 25541 );
    m = add_method( "CmdStatement", "resolve", 0, 0, 0,"Cmd", code, 25545 );
    m = add_method( "CmdControlStructure", "init_object", 0, 0, 0,"CmdControlStructure", code, 25549 );
    m = add_method( "CmdElseIf", "init_object", 0, 0, 0,"CmdElseIf", code, 25553 );
    m = add_method( "CmdIfNonNull", "init_object", 0, 0, 0,"CmdIfNonNull", code, 25557 );
    m = add_method( "CmdIfNonNull", "resolve", 0, 0, 0,"Cmd", code, 25561 );
    m = add_method( "CmdIfNonNull", "compile", 0, 1, 2,null, code, 25607 );
    add_parameter( m, 0, "writer", "JavaVMWriter" );
    add_local( m, 1, "label", "Character[]" );
    m.String_literals = new BardString[6];    m.String_literals[0] = new BardString( type_String, "" );
    m.String_literals[1] = new BardString( type_String, "else_" );
    m.String_literals[2] = new BardString( type_String, "end_" );
    m.String_literals[3] = new BardString( type_String, "end_" );
    m.String_literals[4] = new BardString( type_String, "else_" );
    m.String_literals[5] = new BardString( type_String, "end_" );
    m = add_method( "CmdIfNonNull", "init", 0, 4, 5,"CmdIfNonNull", code, 25886 );
    add_parameter( m, 0, "_t", "Token" );
    add_parameter( m, 1, "_condition", "Cmd" );
    add_parameter( m, 2, "_body", "CmdStatementList" );
    add_parameter( m, 3, "_else_body", "CmdStatementList" );
    add_local( m, 4, "local_var_name", "String" );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "A single variable or method name is required for a checked if." );
    m = add_method( "CmdStatementWithOperand", "init_object", 0, 0, 0,"CmdStatementWithOperand", code, 25958 );
    m = add_method( "CmdStatementWithOperand", "dispatch", 0, 1, 1,null, code, 25962 );
    add_parameter( m, 0, "v", "Visitor" );
    m = add_method( "CmdStatementWithOperand", "type", 0, 0, 0,"Type", code, 25978 );
    m = add_method( "CmdStatementWithOperand", "resolve", 0, 0, 0,"Cmd", code, 25982 );
    m = add_method( "CmdStatementWithOperand", "init", 0, 2, 2,"CmdStatementWithOperand", code, 25998 );
    add_parameter( m, 0, "_t", "Token" );
    add_parameter( m, 1, "_operand", "Cmd" );
    m = add_method( "CmdPrintln", "init_object", 0, 0, 0,"CmdPrintln", code, 26012 );
    m = add_method( "CmdPrintln", "String", 0, 0, 0,"String", code, 26016 );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "println " );
    m = add_method( "CmdPrintln", "resolve", 0, 0, 0,"Cmd", code, 26042 );
    m = add_method( "CmdPrintln", "compile", 0, 1, 3,null, code, 26058 );
    add_parameter( m, 0, "writer", "JavaVMWriter" );
    add_local( m, 1, "operand_type", "Type" );
    add_local( m, 2, "opcode", "Integer" );
    m = add_method( "CmdThrowMissingReturn", "init_object", 0, 0, 0,"CmdThrowMissingReturn", code, 26183 );
    m = add_method( "CmdThrowMissingReturn", "compile", 0, 1, 1,null, code, 26187 );
    add_parameter( m, 0, "writer", "JavaVMWriter" );
    m = add_method( "CmdTron", "init_object", 0, 0, 0,"CmdTron", code, 26198 );
    m = add_method( "CmdTron", "compile", 0, 1, 1,null, code, 26202 );
    add_parameter( m, 0, "writer", "JavaVMWriter" );
    m = add_method( "CmdTroff", "init_object", 0, 0, 0,"CmdTroff", code, 26213 );
    m = add_method( "CmdTroff", "compile", 0, 1, 1,null, code, 26217 );
    add_parameter( m, 0, "writer", "JavaVMWriter" );
    m = add_method( "CmdReturn", "init_object", 0, 0, 0,"CmdReturn", code, 26228 );
    m = add_method( "CmdReturnNil", "init_object", 0, 0, 0,"CmdReturnNil", code, 26232 );
    m = add_method( "CmdReturnNil", "String", 0, 0, 0,"String", code, 26236 );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "return" );
    m = add_method( "CmdReturnNil", "resolve", 0, 0, 0,"Cmd", code, 26241 );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "init" );
    m = add_method( "CmdReturnNil", "compile", 0, 1, 1,null, code, 26281 );
    add_parameter( m, 0, "writer", "JavaVMWriter" );
    m = add_method( "CmdReturnNull", "init_object", 0, 0, 0,"CmdReturnNull", code, 26292 );
    m = add_method( "CmdReturnNull", "String", 0, 0, 0,"String", code, 26296 );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "return null" );
    m = add_method( "CmdReturnNull", "resolve", 0, 0, 1,"Cmd", code, 26301 );
    add_local( m, 0, "return_type", "Type" );
    m.String_literals = new BardString[2];    m.String_literals[0] = new BardString( type_String, "Method is not declared as returning a value." );
    m.String_literals[1] = new BardString( type_String, "Non-null return value required." );
    m = add_method( "CmdReturnNull", "compile", 0, 1, 1,null, code, 26346 );
    add_parameter( m, 0, "writer", "JavaVMWriter" );
    m = add_method( "CmdReturnThis", "init_object", 0, 0, 0,"CmdReturnThis", code, 26366 );
    m = add_method( "CmdReturnThis", "String", 0, 0, 0,"String", code, 26370 );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "return this" );
    m = add_method( "CmdReturnThis", "resolve", 0, 0, 0,"Cmd", code, 26375 );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "Method is not declared as returning a value." );
    m = add_method( "CmdReturnThis", "compile", 0, 1, 1,null, code, 26407 );
    add_parameter( m, 0, "writer", "JavaVMWriter" );
    m = add_method( "CmdReturnThis", "init", 0, 2, 2,"CmdReturnThis", code, 26427 );
    add_parameter( m, 0, "_t", "Token" );
    add_parameter( m, 1, "_return_type", "TypeDef" );
    m = add_method( "CmdCast", "init_object", 0, 0, 0,"CmdCast", code, 26452 );
    m = add_method( "CmdCastToType", "init_object", 0, 0, 0,"CmdCastToType", code, 26462 );
    m = add_method( "CmdCastToType", "implicit_type", 0, 0, 0,"Type", code, 26472 );
    m = add_method( "CmdCastToType", "type", 0, 0, 0,"Type", code, 26478 );
    m = add_method( "CmdCastToType", "resolve", 0, 0, 1,"Cmd", code, 26484 );
    add_local( m, 0, "from_type", "Type" );
    m.String_literals = new BardString[5];    m.String_literals[0] = new BardString( type_String, "Cannot cast a possibly null reference to a non-null reference type." );
    m.String_literals[1] = new BardString( type_String, "IntegerObject" );
    m.String_literals[2] = new BardString( type_String, "Integer" );
    m.String_literals[3] = new BardString( type_String, "Unhandled cast: " );
    m.String_literals[4] = new BardString( type_String, "->" );
    m = add_method( "CmdCastToType", "init", 0, 3, 3,"CmdCastToType", code, 26893 );
    add_parameter( m, 0, "_t", "Token" );
    add_parameter( m, 1, "_operand", "Cmd" );
    add_parameter( m, 2, "_to_type", "Type" );
    m = add_method( "CmdGeneralizeReference", "init_object", 0, 0, 0,"CmdGeneralizeReference", code, 26912 );
    m = add_method( "CmdGeneralizeReference", "resolve", 0, 0, 0,"Cmd", code, 26922 );
    m = add_method( "CmdGeneralizeReference", "compile", 0, 1, 1,null, code, 26938 );
    add_parameter( m, 0, "writer", "JavaVMWriter" );
    m = add_method( "CmdSpecializeReference", "init_object", 0, 0, 0,"CmdSpecializeReference", code, 26948 );
    m = add_method( "CmdSpecializeReference", "resolve", 0, 0, 0,"Cmd", code, 26958 );
    m = add_method( "CmdSpecializeReference", "compile", 0, 1, 1,null, code, 26974 );
    add_parameter( m, 0, "writer", "JavaVMWriter" );
    m = add_method( "CmdCastIntegerToReal", "init_object", 0, 0, 0,"CmdCastIntegerToReal", code, 27005 );
    m = add_method( "CmdCastIntegerToReal", "type", 0, 0, 0,"Type", code, 27015 );
    m = add_method( "CmdCastIntegerToReal", "resolve", 0, 0, 0,"Cmd", code, 27024 );
    m = add_method( "CmdCastIntegerToReal", "compile", 0, 1, 1,null, code, 27040 );
    add_parameter( m, 0, "writer", "JavaVMWriter" );
    m = add_method( "CmdCastRealToInteger", "init_object", 0, 0, 0,"CmdCastRealToInteger", code, 27059 );
    m = add_method( "CmdCastRealToInteger", "type", 0, 0, 0,"Type", code, 27069 );
    m = add_method( "CmdCastRealToInteger", "resolve", 0, 0, 0,"Cmd", code, 27078 );
    m = add_method( "CmdCastRealToInteger", "compile", 0, 1, 1,null, code, 27094 );
    add_parameter( m, 0, "writer", "JavaVMWriter" );
    m = add_method( "CmdTypeManipulation", "init_object", 0, 0, 0,"CmdTypeManipulation", code, 27113 );
    m = add_method( "CmdTypeManipulation", "dispatch", 0, 1, 1,null, code, 27117 );
    add_parameter( m, 0, "v", "Visitor" );
    m = add_method( "CmdTypeManipulation", "resolve", 0, 0, 0,"Cmd", code, 27133 );
    m = add_method( "CmdTypeManipulation", "init", 0, 3, 3,"CmdTypeManipulation", code, 27146 );
    add_parameter( m, 0, "_t", "Token" );
    add_parameter( m, 1, "_operand", "Cmd" );
    add_parameter( m, 2, "_target_type", "Type" );
    m = add_method( "CmdInstanceOf", "init_object", 0, 0, 0,"CmdInstanceOf", code, 27165 );
    m = add_method( "CmdInstanceOf", "implicit_type", 0, 0, 0,"Type", code, 27169 );
    m = add_method( "CmdInstanceOf", "type", 0, 0, 0,"Type", code, 27178 );
    m = add_method( "CmdInstanceOf", "resolve", 0, 0, 1,"Cmd", code, 27187 );
    add_local( m, 0, "operand_type", "Type" );
    m = add_method( "CmdInstanceOf", "compile", 0, 1, 1,null, code, 27272 );
    add_parameter( m, 0, "writer", "JavaVMWriter" );
    m = add_method( "CmdBinary", "init_object", 0, 0, 0,"CmdBinary", code, 27303 );
    m = add_method( "CmdBinary", "implicit_type", 0, 0, 2,"Type", code, 27313 );
    add_local( m, 0, "lhs_type", "Type" );
    add_local( m, 1, "rhs_type", "Type" );
    m = add_method( "CmdBinary", "dispatch", 0, 1, 1,null, code, 27358 );
    add_parameter( m, 0, "v", "Visitor" );
    m = add_method( "CmdBinary", "type", 0, 0, 0,"Type", code, 27382 );
    m = add_method( "CmdBinary", "resolve", 0, 0, 0,"Cmd", code, 27391 );
    m = add_method( "CmdBinary", "compile", 0, 1, 1,null, code, 27402 );
    add_parameter( m, 0, "writer", "JavaVMWriter" );
    m = add_method( "CmdBinary", "init", 0, 3, 3,"CmdBinary", code, 27428 );
    add_parameter( m, 0, "_t", "Token" );
    add_parameter( m, 1, "_lhs", "Cmd" );
    add_parameter( m, 2, "_rhs", "Cmd" );
    m = add_method( "CmdBinary", "name", 0, 0, 0,"String", code, 27447 );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "[unnamed binary op]" );
    m = add_method( "CmdBinary", "resolve_for_common_type", 0, 1, 10,"Cmd", code, 27452 );
    add_parameter( m, 0, "common_type", "Type" );
    add_local( m, 1, "common_type", "Type" );
    add_local( m, 2, "result", "Cmd" );
    add_local( m, 3, "result", "Cmd" );
    add_local( m, 4, "result", "Cmd" );
    add_local( m, 5, "result", "Cmd" );
    add_local( m, 6, "result", "Cmd" );
    add_local( m, 7, "result", "Cmd" );
    add_local( m, 8, "result", "Cmd" );
    add_local( m, 9, "result", "Cmd" );
    m.String_literals = new BardString[3];    m.String_literals[0] = new BardString( type_String, "Unsupported operand type " );
    m.String_literals[1] = new BardString( type_String, " for '" );
    m.String_literals[2] = new BardString( type_String, "' operation." );
    m = add_method( "CmdBinary", "resolve_operands", 0, 0, 2,"Type", code, 27795 );
    add_local( m, 0, "lhs_type", "Type" );
    add_local( m, 1, "rhs_type", "Type" );
    m = add_method( "CmdBinary", "cast_to_common_type", 0, 1, 1,null, code, 27848 );
    add_parameter( m, 0, "common_type", "Type" );
    m = add_method( "CmdBinary", "resolve_operands_and_cast_to_common_type", 0, 0, 1,"Type", code, 27888 );
    add_local( m, 0, "common_type", "Type" );
    m = add_method( "CmdBinary", "combine_literal_Real_operands", 0, 2, 2,"Cmd", code, 27905 );
    add_parameter( m, 0, "lhs_value", "Real" );
    add_parameter( m, 1, "rhs_value", "Real" );
    m = add_method( "CmdBinary", "combine_literal_Integer_operands", 0, 2, 2,"Cmd", code, 27909 );
    add_parameter( m, 0, "lhs_value", "Integer" );
    add_parameter( m, 1, "rhs_value", "Integer" );
    m = add_method( "CmdBinary", "combine_literal_Logical_operands", 0, 2, 2,"Cmd", code, 27913 );
    add_parameter( m, 0, "lhs_value", "Logical" );
    add_parameter( m, 1, "rhs_value", "Logical" );
    m = add_method( "CmdBinary", "combine_literal_operands", 0, 1, 1,"Cmd", code, 27917 );
    add_parameter( m, 0, "common_type", "Type" );
    m = add_method( "CmdBinary", "resolve_for_Real_operands", 0, 0, 0,"Cmd", code, 27921 );
    m = add_method( "CmdBinary", "resolve_for_Integer_operands", 0, 0, 0,"Cmd", code, 27925 );
    m = add_method( "CmdBinary", "resolve_for_Logical_operands", 0, 0, 0,"Cmd", code, 27929 );
    m = add_method( "CmdBinary", "resolve_for_miscellaneous_operands", 0, 1, 1,"Cmd", code, 27933 );
    add_parameter( m, 0, "common_type", "Type" );
    m = add_method( "CmdAdd", "init_object", 0, 0, 0,"CmdAdd", code, 27937 );
    m = add_method( "CmdAdd", "name", 0, 0, 0,"String", code, 27947 );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "" );
    m = add_method( "CmdAdd", "combine_literal_Real_operands", 0, 2, 2,"Cmd", code, 27969 );
    add_parameter( m, 0, "lhs_value", "Real" );
    add_parameter( m, 1, "rhs_value", "Real" );
    m = add_method( "CmdAdd", "combine_literal_Integer_operands", 0, 2, 2,"Cmd", code, 27985 );
    add_parameter( m, 0, "lhs_value", "Integer" );
    add_parameter( m, 1, "rhs_value", "Integer" );
    m = add_method( "CmdAdd", "combine_literal_Logical_operands", 0, 2, 2,"Cmd", code, 28001 );
    add_parameter( m, 0, "lhs_value", "Logical" );
    add_parameter( m, 1, "rhs_value", "Logical" );
    m = add_method( "CmdAdd", "combine_literal_operands", 0, 1, 1,"Cmd", code, 28018 );
    add_parameter( m, 0, "common_type", "Type" );
    m = add_method( "CmdAdd", "resolve_for_Real_operands", 0, 0, 0,"Cmd", code, 28138 );
    m = add_method( "CmdAdd", "resolve_for_Integer_operands", 0, 0, 0,"Cmd", code, 28149 );
    m = add_method( "CmdAdd", "resolve_for_Logical_operands", 0, 0, 0,"Cmd", code, 28160 );
    m = add_method( "CmdSubtract", "init_object", 0, 0, 0,"CmdSubtract", code, 28171 );
    m = add_method( "CmdSubtract", "name", 0, 0, 0,"String", code, 28181 );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "" );
    m = add_method( "CmdSubtract", "combine_literal_Real_operands", 0, 2, 2,"Cmd", code, 28203 );
    add_parameter( m, 0, "lhs_value", "Real" );
    add_parameter( m, 1, "rhs_value", "Real" );
    m = add_method( "CmdSubtract", "combine_literal_Integer_operands", 0, 2, 2,"Cmd", code, 28219 );
    add_parameter( m, 0, "lhs_value", "Integer" );
    add_parameter( m, 1, "rhs_value", "Integer" );
    m = add_method( "CmdSubtract", "resolve_for_Real_operands", 0, 0, 0,"Cmd", code, 28235 );
    m = add_method( "CmdSubtract", "resolve_for_Integer_operands", 0, 0, 0,"Cmd", code, 28246 );
    m = add_method( "CmdMultiply", "init_object", 0, 0, 0,"CmdMultiply", code, 28257 );
    m = add_method( "CmdMultiply", "name", 0, 0, 0,"String", code, 28267 );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "" );
    m = add_method( "CmdMultiply", "combine_literal_Real_operands", 0, 2, 2,"Cmd", code, 28289 );
    add_parameter( m, 0, "lhs_value", "Real" );
    add_parameter( m, 1, "rhs_value", "Real" );
    m = add_method( "CmdMultiply", "combine_literal_Integer_operands", 0, 2, 2,"Cmd", code, 28305 );
    add_parameter( m, 0, "lhs_value", "Integer" );
    add_parameter( m, 1, "rhs_value", "Integer" );
    m = add_method( "CmdMultiply", "combine_literal_Logical_operands", 0, 2, 2,"Cmd", code, 28321 );
    add_parameter( m, 0, "lhs_value", "Logical" );
    add_parameter( m, 1, "rhs_value", "Logical" );
    m = add_method( "CmdMultiply", "resolve_for_Real_operands", 0, 0, 0,"Cmd", code, 28338 );
    m = add_method( "CmdMultiply", "resolve_for_Integer_operands", 0, 0, 0,"Cmd", code, 28349 );
    m = add_method( "CmdMultiply", "resolve_for_Logical_operands", 0, 0, 0,"Cmd", code, 28360 );
    m = add_method( "CmdDivide", "init_object", 0, 0, 0,"CmdDivide", code, 28371 );
    m = add_method( "CmdDivide", "implicit_type", 0, 0, 3,"Type", code, 28381 );
    add_local( m, 0, "lhs_type", "Type" );
    add_local( m, 1, "rhs_type", "Type" );
    add_local( m, 2, "result", "Type" );
    m = add_method( "CmdDivide", "name", 0, 0, 0,"String", code, 28446 );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "" );
    m = add_method( "CmdDivide", "combine_literal_Real_operands", 0, 2, 2,"Cmd", code, 28468 );
    add_parameter( m, 0, "lhs_value", "Real" );
    add_parameter( m, 1, "rhs_value", "Real" );
    m = add_method( "CmdDivide", "combine_literal_Integer_operands", 0, 2, 2,"Cmd", code, 28484 );
    add_parameter( m, 0, "lhs_value", "Integer" );
    add_parameter( m, 1, "rhs_value", "Integer" );
    m = add_method( "CmdDivide", "resolve_for_Real_operands", 0, 0, 0,"Cmd", code, 28502 );
    m = add_method( "CmdDivide", "resolve_for_Integer_operands", 0, 0, 0,"Cmd", code, 28513 );
    m = add_method( "CmdMod", "init_object", 0, 0, 0,"CmdMod", code, 28556 );
    m = add_method( "CmdMod", "name", 0, 0, 0,"String", code, 28566 );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "" );
    m = add_method( "CmdMod", "combine_literal_Real_operands", 0, 2, 2,"Cmd", code, 28588 );
    add_parameter( m, 0, "lhs_value", "Real" );
    add_parameter( m, 1, "rhs_value", "Real" );
    m = add_method( "CmdMod", "combine_literal_Integer_operands", 0, 2, 2,"Cmd", code, 28604 );
    add_parameter( m, 0, "lhs_value", "Integer" );
    add_parameter( m, 1, "rhs_value", "Integer" );
    m = add_method( "CmdMod", "resolve_for_Real_operands", 0, 0, 0,"Cmd", code, 28620 );
    m = add_method( "CmdMod", "resolve_for_Integer_operands", 0, 0, 0,"Cmd", code, 28631 );
    m = add_method( "CmdPower", "init_object", 0, 0, 0,"CmdPower", code, 28642 );
    m = add_method( "CmdPower", "name", 0, 0, 0,"String", code, 28652 );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "" );
    m = add_method( "CmdPower", "combine_literal_Real_operands", 0, 2, 2,"Cmd", code, 28674 );
    add_parameter( m, 0, "lhs_value", "Real" );
    add_parameter( m, 1, "rhs_value", "Real" );
    m = add_method( "CmdPower", "combine_literal_Integer_operands", 0, 2, 2,"Cmd", code, 28690 );
    add_parameter( m, 0, "lhs_value", "Integer" );
    add_parameter( m, 1, "rhs_value", "Integer" );
    m = add_method( "CmdPower", "resolve_for_Real_operands", 0, 0, 0,"Cmd", code, 28706 );
    m = add_method( "CmdPower", "resolve_for_Integer_operands", 0, 0, 0,"Cmd", code, 28717 );
    m = add_method( "CmdBitwiseAnd", "init_object", 0, 0, 0,"CmdBitwiseAnd", code, 28728 );
    m = add_method( "CmdBitwiseAnd", "name", 0, 0, 0,"String", code, 28738 );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "" );
    m = add_method( "CmdBitwiseAnd", "combine_literal_Integer_operands", 0, 2, 2,"Cmd", code, 28760 );
    add_parameter( m, 0, "lhs_value", "Integer" );
    add_parameter( m, 1, "rhs_value", "Integer" );
    m = add_method( "CmdBitwiseAnd", "combine_literal_Logical_operands", 0, 2, 2,"Cmd", code, 28776 );
    add_parameter( m, 0, "lhs_value", "Logical" );
    add_parameter( m, 1, "rhs_value", "Logical" );
    m = add_method( "CmdBitwiseAnd", "resolve_for_Integer_operands", 0, 0, 0,"Cmd", code, 28793 );
    m = add_method( "CmdBitwiseAnd", "resolve_for_Logical_operands", 0, 0, 0,"Cmd", code, 28804 );
    m = add_method( "CmdBitwiseOr", "init_object", 0, 0, 0,"CmdBitwiseOr", code, 28815 );
    m = add_method( "CmdBitwiseOr", "name", 0, 0, 0,"String", code, 28825 );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "" );
    m = add_method( "CmdBitwiseOr", "combine_literal_Integer_operands", 0, 2, 2,"Cmd", code, 28847 );
    add_parameter( m, 0, "lhs_value", "Integer" );
    add_parameter( m, 1, "rhs_value", "Integer" );
    m = add_method( "CmdBitwiseOr", "combine_literal_Logical_operands", 0, 2, 2,"Cmd", code, 28863 );
    add_parameter( m, 0, "lhs_value", "Logical" );
    add_parameter( m, 1, "rhs_value", "Logical" );
    m = add_method( "CmdBitwiseOr", "resolve_for_Integer_operands", 0, 0, 0,"Cmd", code, 28880 );
    m = add_method( "CmdBitwiseOr", "resolve_for_Logical_operands", 0, 0, 0,"Cmd", code, 28891 );
    m = add_method( "CmdBitwiseXor", "init_object", 0, 0, 0,"CmdBitwiseXor", code, 28902 );
    m = add_method( "CmdBitwiseXor", "name", 0, 0, 0,"String", code, 28912 );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "" );
    m = add_method( "CmdBitwiseXor", "combine_literal_Integer_operands", 0, 2, 2,"Cmd", code, 28934 );
    add_parameter( m, 0, "lhs_value", "Integer" );
    add_parameter( m, 1, "rhs_value", "Integer" );
    m = add_method( "CmdBitwiseXor", "combine_literal_Logical_operands", 0, 2, 2,"Cmd", code, 28950 );
    add_parameter( m, 0, "lhs_value", "Logical" );
    add_parameter( m, 1, "rhs_value", "Logical" );
    m = add_method( "CmdBitwiseXor", "resolve_for_Integer_operands", 0, 0, 0,"Cmd", code, 28977 );
    m = add_method( "CmdBitwiseXor", "resolve_for_Logical_operands", 0, 0, 0,"Cmd", code, 28988 );
    m = add_method( "CmdBitwiseSHL", "init_object", 0, 0, 0,"CmdBitwiseSHL", code, 28999 );
    m = add_method( "CmdBitwiseSHL", "name", 0, 0, 0,"String", code, 29009 );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, ":<<:" );
    m = add_method( "CmdBitwiseSHL", "combine_literal_Integer_operands", 0, 2, 2,"Cmd", code, 29014 );
    add_parameter( m, 0, "lhs_value", "Integer" );
    add_parameter( m, 1, "rhs_value", "Integer" );
    m = add_method( "CmdBitwiseSHL", "resolve_for_Integer_operands", 0, 0, 0,"Cmd", code, 29030 );
    m = add_method( "CmdBitwiseSHR", "init_object", 0, 0, 0,"CmdBitwiseSHR", code, 29041 );
    m = add_method( "CmdBitwiseSHR", "name", 0, 0, 0,"String", code, 29051 );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, ":>>:" );
    m = add_method( "CmdBitwiseSHR", "combine_literal_Integer_operands", 0, 2, 2,"Cmd", code, 29056 );
    add_parameter( m, 0, "lhs_value", "Integer" );
    add_parameter( m, 1, "rhs_value", "Integer" );
    m = add_method( "CmdBitwiseSHR", "resolve_for_Integer_operands", 0, 0, 0,"Cmd", code, 29072 );
    m = add_method( "CmdBitwiseSHRX", "init_object", 0, 0, 0,"CmdBitwiseSHRX", code, 29083 );
    m = add_method( "CmdBitwiseSHRX", "name", 0, 0, 0,"String", code, 29093 );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, ":>>>:" );
    m = add_method( "CmdBitwiseSHRX", "combine_literal_Integer_operands", 0, 2, 2,"Cmd", code, 29098 );
    add_parameter( m, 0, "lhs_value", "Integer" );
    add_parameter( m, 1, "rhs_value", "Integer" );
    m = add_method( "CmdBitwiseSHRX", "resolve_for_Integer_operands", 0, 0, 0,"Cmd", code, 29114 );
    m = add_method( "CmdLogicalAnd", "init_object", 0, 0, 0,"CmdLogicalAnd", code, 29125 );
    m = add_method( "CmdLogicalAnd", "compile", 0, 1, 2,null, code, 29135 );
    add_parameter( m, 0, "writer", "JavaVMWriter" );
    add_local( m, 1, "label", "Character[]" );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "" );
    m = add_method( "CmdLogicalAnd", "name", 0, 0, 0,"String", code, 29222 );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "and" );
    m = add_method( "CmdLogicalAnd", "combine_literal_Logical_operands", 0, 2, 2,"Cmd", code, 29227 );
    add_parameter( m, 0, "lhs_value", "Logical" );
    add_parameter( m, 1, "rhs_value", "Logical" );
    m = add_method( "CmdLogicalAnd", "resolve_for_Logical_operands", 0, 0, 0,"Cmd", code, 29244 );
    m = add_method( "CmdLogicalOr", "init_object", 0, 0, 0,"CmdLogicalOr", code, 29248 );
    m = add_method( "CmdLogicalOr", "resolve", 0, 0, 3,"Cmd", code, 29258 );
    add_local( m, 0, "common_type", "Type" );
    add_local( m, 1, "lhs_type", "Type" );
    add_local( m, 2, "rhs_type", "Type" );
    m = add_method( "CmdLogicalOr", "compile", 0, 1, 2,null, code, 29372 );
    add_parameter( m, 0, "writer", "JavaVMWriter" );
    add_local( m, 1, "label", "Character[]" );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "" );
    m = add_method( "CmdLogicalOr", "name", 0, 0, 0,"String", code, 29459 );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "or" );
    m = add_method( "CmdLogicalOr", "combine_literal_Logical_operands", 0, 2, 2,"Cmd", code, 29464 );
    add_parameter( m, 0, "lhs_value", "Logical" );
    add_parameter( m, 1, "rhs_value", "Logical" );
    m = add_method( "CmdLogicalOr", "resolve_for_Logical_operands", 0, 0, 0,"Cmd", code, 29481 );
    m = add_method( "CmdEitherOr", "init_object", 0, 0, 0,"CmdEitherOr", code, 29485 );
    m = add_method( "CmdEitherOr", "type", 0, 0, 0,"Type", code, 29495 );
    m = add_method( "CmdEitherOr", "resolve", 0, 0, 0,"Cmd", code, 29501 );
    m = add_method( "CmdEitherOr", "compile", 0, 1, 2,null, code, 29531 );
    add_parameter( m, 0, "writer", "JavaVMWriter" );
    add_local( m, 1, "label", "Character[]" );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "" );
    m = add_method( "CmdEitherOr", "name", 0, 0, 0,"String", code, 29618 );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "or" );
    m = add_method( "CmdEitherOr", "init", 0, 4, 4,"CmdEitherOr", code, 29623 );
    add_parameter( m, 0, "_t", "Token" );
    add_parameter( m, 1, "_lhs", "Cmd" );
    add_parameter( m, 2, "_rhs", "Cmd" );
    add_parameter( m, 3, "_common_type", "Type" );
    m = add_method( "CmdLogicalXor", "init_object", 0, 0, 0,"CmdLogicalXor", code, 29647 );
    m = add_method( "CmdLogicalXor", "name", 0, 0, 0,"String", code, 29657 );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "xor" );
    m = add_method( "CmdLogicalXor", "combine_literal_Logical_operands", 0, 2, 2,"Cmd", code, 29662 );
    add_parameter( m, 0, "lhs_value", "Logical" );
    add_parameter( m, 1, "rhs_value", "Logical" );
    m = add_method( "CmdLogicalXor", "resolve_for_Logical_operands", 0, 0, 0,"Cmd", code, 29689 );
    m = add_method( "CmdComparison", "init_object", 0, 0, 0,"CmdComparison", code, 29700 );
    m = add_method( "CmdComparison", "implicit_type", 0, 0, 0,"Type", code, 29710 );
    m = add_method( "CmdComparison", "type", 0, 0, 0,"Type", code, 29719 );
    m = add_method( "CmdCompareEQ", "init_object", 0, 0, 0,"CmdCompareEQ", code, 29728 );
    m = add_method( "CmdCompareEQ", "name", 0, 0, 0,"String", code, 29738 );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "==" );
    m = add_method( "CmdCompareEQ", "combine_literal_Real_operands", 0, 2, 2,"Cmd", code, 29743 );
    add_parameter( m, 0, "lhs_value", "Real" );
    add_parameter( m, 1, "rhs_value", "Real" );
    m = add_method( "CmdCompareEQ", "combine_literal_Integer_operands", 0, 2, 2,"Cmd", code, 29759 );
    add_parameter( m, 0, "lhs_value", "Integer" );
    add_parameter( m, 1, "rhs_value", "Integer" );
    m = add_method( "CmdCompareEQ", "combine_literal_Logical_operands", 0, 2, 2,"Cmd", code, 29775 );
    add_parameter( m, 0, "lhs_value", "Logical" );
    add_parameter( m, 1, "rhs_value", "Logical" );
    m = add_method( "CmdCompareEQ", "combine_literal_operands", 0, 1, 1,"Cmd", code, 29791 );
    add_parameter( m, 0, "common_type", "Type" );
    m = add_method( "CmdCompareEQ", "resolve_for_Real_operands", 0, 0, 0,"Cmd", code, 29866 );
    m = add_method( "CmdCompareEQ", "resolve_for_Integer_operands", 0, 0, 0,"Cmd", code, 29877 );
    m = add_method( "CmdCompareEQ", "resolve_for_Logical_operands", 0, 0, 0,"Cmd", code, 29888 );
    m = add_method( "CmdCompareEQ", "resolve_for_miscellaneous_operands", 0, 1, 1,"Cmd", code, 29899 );
    add_parameter( m, 0, "common_type", "Type" );
    m = add_method( "CmdCompareNE", "init_object", 0, 0, 0,"CmdCompareNE", code, 29926 );
    m = add_method( "CmdCompareNE", "name", 0, 0, 0,"String", code, 29936 );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "!=" );
    m = add_method( "CmdCompareNE", "combine_literal_Real_operands", 0, 2, 2,"Cmd", code, 29941 );
    add_parameter( m, 0, "lhs_value", "Real" );
    add_parameter( m, 1, "rhs_value", "Real" );
    m = add_method( "CmdCompareNE", "combine_literal_Integer_operands", 0, 2, 2,"Cmd", code, 29957 );
    add_parameter( m, 0, "lhs_value", "Integer" );
    add_parameter( m, 1, "rhs_value", "Integer" );
    m = add_method( "CmdCompareNE", "combine_literal_Logical_operands", 0, 2, 2,"Cmd", code, 29973 );
    add_parameter( m, 0, "lhs_value", "Logical" );
    add_parameter( m, 1, "rhs_value", "Logical" );
    m = add_method( "CmdCompareNE", "combine_literal_operands", 0, 1, 1,"Cmd", code, 29989 );
    add_parameter( m, 0, "common_type", "Type" );
    m = add_method( "CmdCompareNE", "resolve_for_Real_operands", 0, 0, 0,"Cmd", code, 30064 );
    m = add_method( "CmdCompareNE", "resolve_for_Integer_operands", 0, 0, 0,"Cmd", code, 30075 );
    m = add_method( "CmdCompareNE", "resolve_for_Logical_operands", 0, 0, 0,"Cmd", code, 30086 );
    m = add_method( "CmdCompareNE", "resolve_for_miscellaneous_operands", 0, 1, 1,"Cmd", code, 30097 );
    add_parameter( m, 0, "common_type", "Type" );
    m = add_method( "CmdCompareLT", "init_object", 0, 0, 0,"CmdCompareLT", code, 30124 );
    m = add_method( "CmdCompareLT", "name", 0, 0, 0,"String", code, 30134 );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "" );
    m = add_method( "CmdCompareLT", "combine_literal_Real_operands", 0, 2, 2,"Cmd", code, 30156 );
    add_parameter( m, 0, "lhs_value", "Real" );
    add_parameter( m, 1, "rhs_value", "Real" );
    m = add_method( "CmdCompareLT", "combine_literal_Integer_operands", 0, 2, 2,"Cmd", code, 30172 );
    add_parameter( m, 0, "lhs_value", "Integer" );
    add_parameter( m, 1, "rhs_value", "Integer" );
    m = add_method( "CmdCompareLT", "combine_literal_operands", 0, 1, 1,"Cmd", code, 30188 );
    add_parameter( m, 0, "common_type", "Type" );
    m = add_method( "CmdCompareLT", "resolve_for_Real_operands", 0, 0, 0,"Cmd", code, 30225 );
    m = add_method( "CmdCompareLT", "resolve_for_Integer_operands", 0, 0, 0,"Cmd", code, 30236 );
    m = add_method( "CmdCompareLT", "resolve_for_miscellaneous_operands", 0, 1, 1,"Cmd", code, 30247 );
    add_parameter( m, 0, "common_type", "Type" );
    m = add_method( "CmdCompareGT", "init_object", 0, 0, 0,"CmdCompareGT", code, 30269 );
    m = add_method( "CmdCompareGT", "name", 0, 0, 0,"String", code, 30279 );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "" );
    m = add_method( "CmdCompareGT", "combine_literal_Real_operands", 0, 2, 2,"Cmd", code, 30301 );
    add_parameter( m, 0, "lhs_value", "Real" );
    add_parameter( m, 1, "rhs_value", "Real" );
    m = add_method( "CmdCompareGT", "combine_literal_Integer_operands", 0, 2, 2,"Cmd", code, 30317 );
    add_parameter( m, 0, "lhs_value", "Integer" );
    add_parameter( m, 1, "rhs_value", "Integer" );
    m = add_method( "CmdCompareGT", "combine_literal_operands", 0, 1, 1,"Cmd", code, 30333 );
    add_parameter( m, 0, "common_type", "Type" );
    m = add_method( "CmdCompareGT", "resolve_for_Real_operands", 0, 0, 0,"Cmd", code, 30370 );
    m = add_method( "CmdCompareGT", "resolve_for_Integer_operands", 0, 0, 0,"Cmd", code, 30381 );
    m = add_method( "CmdCompareGT", "resolve_for_miscellaneous_operands", 0, 1, 1,"Cmd", code, 30392 );
    add_parameter( m, 0, "common_type", "Type" );
    m = add_method( "CmdCompareLE", "init_object", 0, 0, 0,"CmdCompareLE", code, 30414 );
    m = add_method( "CmdCompareLE", "name", 0, 0, 0,"String", code, 30424 );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "<=" );
    m = add_method( "CmdCompareLE", "combine_literal_Real_operands", 0, 2, 2,"Cmd", code, 30429 );
    add_parameter( m, 0, "lhs_value", "Real" );
    add_parameter( m, 1, "rhs_value", "Real" );
    m = add_method( "CmdCompareLE", "combine_literal_Integer_operands", 0, 2, 2,"Cmd", code, 30445 );
    add_parameter( m, 0, "lhs_value", "Integer" );
    add_parameter( m, 1, "rhs_value", "Integer" );
    m = add_method( "CmdCompareLE", "combine_literal_operands", 0, 1, 1,"Cmd", code, 30461 );
    add_parameter( m, 0, "common_type", "Type" );
    m = add_method( "CmdCompareLE", "resolve_for_Real_operands", 0, 0, 0,"Cmd", code, 30498 );
    m = add_method( "CmdCompareLE", "resolve_for_Integer_operands", 0, 0, 0,"Cmd", code, 30509 );
    m = add_method( "CmdCompareLE", "resolve_for_miscellaneous_operands", 0, 1, 1,"Cmd", code, 30520 );
    add_parameter( m, 0, "common_type", "Type" );
    m = add_method( "CmdCompareGE", "init_object", 0, 0, 0,"CmdCompareGE", code, 30542 );
    m = add_method( "CmdCompareGE", "name", 0, 0, 0,"String", code, 30552 );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, ">=" );
    m = add_method( "CmdCompareGE", "combine_literal_Real_operands", 0, 2, 2,"Cmd", code, 30557 );
    add_parameter( m, 0, "lhs_value", "Real" );
    add_parameter( m, 1, "rhs_value", "Real" );
    m = add_method( "CmdCompareGE", "combine_literal_Integer_operands", 0, 2, 2,"Cmd", code, 30573 );
    add_parameter( m, 0, "lhs_value", "Integer" );
    add_parameter( m, 1, "rhs_value", "Integer" );
    m = add_method( "CmdCompareGE", "combine_literal_operands", 0, 1, 1,"Cmd", code, 30589 );
    add_parameter( m, 0, "common_type", "Type" );
    m = add_method( "CmdCompareGE", "resolve_for_Real_operands", 0, 0, 0,"Cmd", code, 30626 );
    m = add_method( "CmdCompareGE", "resolve_for_Integer_operands", 0, 0, 0,"Cmd", code, 30637 );
    m = add_method( "CmdCompareGE", "resolve_for_miscellaneous_operands", 0, 1, 1,"Cmd", code, 30648 );
    add_parameter( m, 0, "common_type", "Type" );
    m = add_method( "CmdLocalDeclaration", "init_object", 0, 0, 0,"CmdLocalDeclaration", code, 30670 );
    m = add_method( "CmdLocalDeclaration", "dispatch", 0, 1, 1,null, code, 30674 );
    add_parameter( m, 0, "v", "Visitor" );
    m = add_method( "CmdLocalDeclaration", "resolve", 0, 0, 0,"Cmd", code, 30692 );
    m = add_method( "CmdLocalDeclaration", "compile", 0, 1, 1,null, code, 30706 );
    add_parameter( m, 0, "writer", "JavaVMWriter" );
    m = add_method( "CmdLocalDeclaration", "init", 0, 2, 2,"CmdLocalDeclaration", code, 30708 );
    add_parameter( m, 0, "_t", "Token" );
    add_parameter( m, 1, "_v", "Local" );
    m = add_method( "CmdAssign", "init_object", 0, 0, 0,"CmdAssign", code, 30722 );
    m = add_method( "CmdAssign", "dispatch", 0, 1, 1,null, code, 30726 );
    add_parameter( m, 0, "v", "Visitor" );
    m = add_method( "CmdAssign", "resolve", 0, 0, 0,"Cmd", code, 30750 );
    m = add_method( "CmdAssign", "init", 0, 3, 3,"CmdAssign", code, 30762 );
    add_parameter( m, 0, "_t", "Token" );
    add_parameter( m, 1, "_operand", "Cmd" );
    add_parameter( m, 2, "_new_value", "Cmd" );
    m = add_method( "CmdWriteLocal", "init_object", 0, 0, 0,"CmdWriteLocal", code, 30781 );
    m = add_method( "CmdWriteLocal", "dispatch", 0, 1, 1,null, code, 30785 );
    add_parameter( m, 0, "v", "Visitor" );
    m = add_method( "CmdWriteLocal", "resolve", 0, 0, 1,"Cmd", code, 30801 );
    add_local( m, 0, "new_value_type", "Type" );
    m.String_literals = new BardString[1];    m.String_literals[0] = new BardString( type_String, "' can only store null-checked references." );
    m = add_method( "CmdWriteLocal", "compile", 0, 1, 2,null, code, 30938 );
    add_parameter( m, 0, "writer", "JavaVMWriter" );
    add_local( m, 1, "type", "Type" );
    m = add_method( "CmdWriteLocal", "init", 0, 3, 3,"CmdWriteLocal", code, 31011 );
    add_parameter( m, 0, "_t", "Token" );
    add_parameter( m, 1, "_v", "Local" );
    add_parameter( m, 2, "_new_value", "Cmd" );
    m = add_method( "CmdWriteLocalDefault", "init_object", 0, 0, 0,"CmdWriteLocalDefault", code, 31030 );
    m = add_method( "CmdWriteLocalDefault", "resolve", 0, 0, 0,"Cmd", code, 31034 );
    m = add_method( "CmdWriteLocalDefault", "compile", 0, 1, 1,null, code, 31038 );
    add_parameter( m, 0, "writer", "JavaVMWriter" );
    m = add_method( "CmdWriteLocalDefault", "init", 0, 2, 2,"CmdWriteLocalDefault", code, 31129 );
    add_parameter( m, 0, "_t", "Token" );
    add_parameter( m, 1, "_v", "Local" );
    m = add_method( "CmdReadLocal", "init_object", 0, 0, 0,"CmdReadLocal", code, 31165 );
    m = add_method( "CmdReadLocal", "type", 0, 0, 0,"Type", code, 31169 );
    m = add_method( "CmdReadLocal", "variable_name", 0, 0, 0,"String", code, 31177 );
    m = add_method( "CmdReadLocal", "resolve", 0, 0, 0,"Cmd", code, 31185 );
    m = add_method( "CmdReadLocal", "compile", 0, 1, 2,null, code, 31189 );
    add_parameter( m, 0, "writer", "JavaVMWriter" );
    add_local( m, 1, "type", "Type" );
    m = add_method( "CmdReadLocal", "init", 0, 2, 2,"CmdReadLocal", code, 31254 );
    add_parameter( m, 0, "_t", "Token" );
    add_parameter( m, 1, "_v", "Local" );
    m = add_method( "Op", "init_object", 0, 0, 0,"Op", code, 31268 );
    m = add_method( "BackpatchInfo", "init_object", 0, 0, 0,"BackpatchInfo", code, 32047 );
    m = add_method( "BackpatchInfo", "init", 0, 2, 2,"BackpatchInfo", code, 32051 );
    add_parameter( m, 0, "_label", "String" );
    add_parameter( m, 1, "_position", "Integer" );
  }

  public BCProgram init()
  {
    vm = create_vm();
    set_up_code( code_strings, 32066 );
    define_id_table();
    define_types();
    define_methods();
    return this;
  }

  public String[] code_strings = new String[]
  {
    "*\011+\011*\011+\011+\0112\000\0144\014.\000\0130\0132\000\0144\0144\0144\014,\000\016+\011\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027,\000\035\200\205\001\203\241_\377\200\205\001\203\241\035\200\205\001\203\241,\001\035\200\205\001\203\241\035\200\205\001\203\241_\376\200\205\001\203\241\035\020,\002\016+\011,\000\016+\011,\000\016+\011,\000\016+\011+\035\010+\011\200\206\000\011+\011*\010+\011*\0130\013*\0144\014*\0144\014*\0144\014*\011+\011*\011+\011*\011+\011*\011+\011*\011+\011*\011+\011*\011+\011*\011+\011+\011*b\377*\200\205\000\203\2022\001P\200\205\002\203|\011+\0112\000g\376\001\022*b\376\200\205\001\203\203b\377>\002\005b\376\014\001\000k\376b\376*\200\205\000\203\202G\003\3452\0017\0144\014*b\377\200\205\001\203~2\0017A\0144\014*2\000b\3772\001P\200\205\002\203|\011+\011*b\3772\001N*\200\205\000\203\2022\001P\200\205\002\203|\011+\0114\0144\0144\014\200\203 *\200\205\001\203\254\011+\011*\011+\011*\011+\011*\011+\011*\011+\0114\014*\011+\011\200\203\027\200\205\000\203n\032\027e\377`\3772[ \200\205\001\203\241\0065g\376*e\3752\000g\374\001#`\375b\374\200\210d\373k\374b\376\002\0054g\376\001\011`\3772, \200\205\001\203\241\006`\377_\373\200\205\001\203\241\006b\374`\375\200\207G\003\325`\3772] \200\205\001\203\241\006`\377\200\205\000\203o\011+\011_\377\033\003^\002\005+\035\010\001\000*_\377$\200\210\010+\011*\200\207\0144\014*\200\215\006\007\007+\011`\377e\3762\000g\375\001\016`\376b\375\200\210d\374k\375*_\374\200\212\006b\375`\376\200\207G\003\352*\011+\011*b\377\200\210\010+\0114\014+\011+\011+\011*b\377\200\210d\375*b\377*b\376\200\210\200\211*b\376_\375\200\211*\011+\0112\000g\377*\200\2072\001Pg\376\001\030*b\377\200\210d\375*b\377*b\376\200\210\200\211*b\376_\375\200\211k\377n\376b\377b\376G\003\341*\011+\011*2\000\200\210\010+\011**\200\2072\001P\200\210\010+\011*2\000\200\214\006+\011**\200\2072\001P\200\214\010+\0112\001g\377\001J5g\3762\000g\375\001+*b\375\200\210#f\374*b\3752\001N\200\210#f\373a\374a\373F\002\021*b\375b\3752\001N\200\205\002\203\215\0064g\376\001\000k\375b\375*\200\207b\377PG\003\313b\376\002\006*\200\207g\377\001\000k\377b\377*\200\207G\003\260\377\377\377\257*\011+\011*\011+\011*_\377\200\221\010+\011*_\377_\376\200\220\011+\011+\011+\011*\200\217\0144\014+\0114\014*\011+\011\200\204(,\000\200\205\001\203\323\200\2044\200\205\000\204\026\200\2035\200\205\000\203ne\377\200\2044`\377\200\205\001\204\030`\377,\001\200\205\001\204N*\011+\011*\011+\0112\000\0144\0142\000\0144\014\007\007*\011+\0110\013*\011+\011*\200\204\023\200\205\000\203\2312\203\350'Q(\200\205\001\203\232\006*\011+\011,\000\011+\011*2\036\200\205\001\203\234'.\000.\001VQ*2\027\200\205\001\203\234'M2\001'M.\00225'V2\001'MS\0130\013*2 \200\205\001\203\234\0144\014*2\001\200\205\001\203\2342\001>\0144\014*b\377\200\205\001\203\233*\011+\011\007\0074\014*\200\205\000\203qa\376a\377OQa\377M\0130\013*2 \200\205\001\203\234b\377U\0144\014*2 \200\205\001\203\234b\376b\377P2\001NUb\377N\0144\014*\011+\011,\000\035\020\007\007*2x \200\205\001\203\240*\011+\011*2\012 \200\205\001\203\241\006*\011+\011*_\377\200\205\001\203\241\006*2\012 \200\205\001\203\241\006*\011+\011\007\007*\011+\011*_\377\",\000\035\200\205\001\203t$\200\205\001\203\245\006*\011+\011+\0114\014+\0114\014*\011+\011*\011+\011*\011+\011\200\203",
    "\027\200\205\000\203n\032\027e\377`\377*\200\205\000\203\202\200\205\001\203\247`\377*\035\200\205\001\203\241\006`\377\011+\011*_\377\200\205\001\203\207\011+\011*_\377\200\205\001\203\207\011+\011\007\007+\011+\011\200\203\037*\200\205\000\203\205\200\205\001\203\264\011+\011\200\203!*\200\205\001\203\271\011+\011*\011+\011\200\203\027\200\205\000\203n\032\027e\377`\3772[ \200\205\001\203\241\0065g\376*e\3752\000g\374\001&`\375b\374\200\205\001\203\203g\373k\374b\376\002\0054g\376\001\011`\3772, \200\205\001\203\241\006`\377b\373\037\200\205\001\203\241\006b\374`\375\200\205\000\203\202G\003\320`\3772] \200\205\001\203\241\006`\377\011+\0114\0144\014+\011+\011+\011+\011`\377e\3762\000g\375\001\022`\376b\375\200\205\001\203\203g\374k\375*b\374\200\205\001\203\245\006b\375`\376\200\205\000\203\202G\003\344+\011\200\203\030\200\205\000\203n\032\030e\377`\377*\200\205\000\203\202\200\205\001\203\247\006`\377*\200\205\001\203\252\006`\377\011+\011*2\000\200\205\001\203\203\0144\014**\200\205\000\203\2022\001P\200\205\001\203\203\0144\014*\011+\011\200\203\027\200\205\000\203n\032\027*o\000\200\205\001\203\2412  \200\205\001\203\241*o\001\200\205\001\203\241\011+\011*_\377v\000*_\376v\001*\011+\011*\011+\011*`\377w\000*\011+\011\200\203\037*\200\205\000\203\205\200\205\001\203\264\011+\011\200\203\034*p\000\200\205\001\203\254\011+\011\200\203!\200\203\036*p\000\200\205\001\203\254\200\205\001\203\271\011+\011+\011*\011+\0114\0144\014+\011+\011\007\007*\011+\0114\0144\014+\011+\011\007\007*\011+\011*\011+\011*\011+\0114\014\007\007\007\007*\011+\011_\377\033\004\002\013*p\000_\377%\200\205\001\203\263\001G*p\001+K\002\014*\200\203\027\200\205\000\203n\032\027w\001\001\000*p\001\200\205\000\203\206_\377\200\205\001\203\240\006*p\001e\3762\000g\375\001\023`\376b\375\200\205\001\203\203g\374k\375*p\000b\374\200\205\001\203\263b\375`\376\200\205\000\203\202G\003\343\007\007*p\000\200\205\000\203\244\007\007*\200\203\035`\377\200\205\001\203\254w\000*\011+\011*p\000\200\205\000\203\262\0144\014*\011+\011*\200\205\000\203\257\002\0052\001\014\001\0032\000\0144\014*t\002\005\006*p\000\200\205\000\203\257\0144\014*t\002\002\013*4{\002*p\001\035\010\001\000*p\001\200\205\000\203\206\006\001!*p\000\200\205\000\203\260%g\377b\3772\012>\002\007*p\001\035\010\001\000*p\001b\377 \200\205\001\203\241\006*p\000\200\205\000\203\257\003\327*p\001\035\010+\011*t\002^\002\013*\200\205\000\203\260\006*5{\002\001\000*p\001\035\010+\011*`\377w\000*4{\002*\200\203\027\200\205\000\203n\032\027w\001*\011+\011*\011+\011*r\001*r\002P\0144\014*r\002*r\001G\0144\014**r\0022\001Ny\002*p\000*r\0022\001P\200\205\001\203\203 \010+\011*r\002*r\001>\002\0062\000\037\010\001\000*p\000*r\002\200\205\001\203\203 \010+\011*`\377w\000**p\000\200\205\000\203\202y\001*2\000y\002*\011+\011*r\002b\377N*r\001E\002\0112\000g\376b\376\014\001\000*p\000*r\002b\377N\200\205\001\203\203\0144\014*\200\205\000\203\261%b\377A\002\0044\014\001\000*\200\205\000\203\260\0065\0144\014*2 \200\205\001\203\266\005\006*2\011\200\205\001\203\266\002\026\001\000*2 \200\205\001\203\266\005\006*2\011\200\205\001\203\266\003\3605\014\001\0004\0144\014`\377e\3762\000g\375\001\026*b\375\200\205\001\203\265`\377b\375\200\205\001\203\203A\002\0044\014\001\000k\375b\375`\376\200\205\000\203\202G\003\340**r\002`\377\200\205\000\203\202Ny\0025\0144\014*\011+\011*\200\203\027\200\205\000\203n\032\027w\000*\200\203\027\200\205\000\203n\032\027w\001*\011+\011*p\000\200\205\000\203o\011+\011*_\377\200\205\001\203\241\006\007\007*p\001\200\205\000\203\206_\377\200\205\001\203\241\006*p\001e\376",
    "2\000g\375\001\024`\376b\375\200\205\001\203\203g\374k\375*p\000b\374 \200\205\001\203\240b\375`\376\200\205\000\203\202G\003\342*\011+\011*_\377\200\205\001\203\241\006*2\012 \200\205\001\203\241\006*\011+\011*`\377w\000*\200\203\027\200\205\000\203n\032\027w\001*\011+\011*\011+\0110\013*\011+\011\200\203\032`\377\200\205\001\203\254\200\205\000\203\205e\376\200\203\027\200\205\000\203n\032\027e\375`\375`\376\200\205\000\203\256\200\205\001\203\247\001:`\376\200\205\000\203\260%g\374b\3742\012>\005\014b\3742 E\004\005b\3742~C\002\013`\375b\374 \200\205\001\203\207\006\001\022b\3742\010>\002\013`\375,\000\035\200\205\001\203\241\006\001\000`\376\200\205\000\203\257\003\260\377\377\377\277*`\375w\000**p\000\200\205\000\203\202y\002*2\000y\001*2\001y\003*2\001y\004*\011+\011\200\203\027\200\205\000\203n\032\027e\375`\375`\376\200\205\000\203\202\200\205\001\203\247`\376e\3742\000g\373\001A`\374b\373\200\205\001\203\203g\372k\373b\372g\371b\3712\012>\005\014b\3712 E\004\005b\3712~C\002\013`\375b\372 \200\205\001\203\207\006\001\022b\3712\010>\002\013`\375,\000\035\200\205\001\203\241\006\001\000b\373`\374\200\205\000\203\202G\003\260\377\377\377\265*`\375w\000**p\000\200\205\000\203\202y\002*2\000y\001*2\001y\003*2\001y\004*\011+\011*r\001*r\002G\0144\014*r\001*r\002>\002\0052\000\014\001\000*p\000*r\001\200\205\001\203\203\0144\014*r\001b\377N*r\002E\002\0052\000\014\001\000*p\000*r\001b\377N\200\205\001\203\203\0144\014*p\000*r\001\200\205\001\203\203g\377**r\0012\001Ny\001b\3772\012>\002\020**r\0032\001Ny\003*2\001y\004\001\011**r\0042\001Ny\004b\377\0144\014*2\000y\000*2\001y\001*2\002y\002*2\003y\003*2\004y\004*2\005y\005*2\006y\006*2\007y\007*2\010y\010*2\011y\011*2\012y\012*2\013y\013*2\014y\014*2\015y\015*2\016y\016*2\017y\017*2\020y\020*2\021y\021*2\022y\022*2\023y\023*2\024y\024*2\025y\025*2\026y\026*2\027y\027*2\030y\030*2\031y\031*2\032y\032*2\033y\033*2\034y\034*2\035y\035*2\036y\036*2\037y\037*2 y *2!y!*2\"y\"*2#y#*2$y$*2%y%*2&y&*2'y'*2(y(*2)y)*2*y**2+y+*2,y,*2-y-*2.y.*2/y/*20y0*21y1*22y2*23y3*24y4*25y5*26y6*27y7*28y8*29y9*2:y:*2;y;*2<y<*2=y=*2>y>*2?y?*2@y@*2AyA*2ByB*2CyC*2DyD*2EyE*2FyF*2GyG*2HyH*2IyI*2JyJ*2KyK*2LyL*2MyM*2NyN*2OyO*2PyP*2QyQ*2RyR*2SyS*2TyT*2UyU*2VyV*2WyW*2XyX*2YyY*2ZyZ*2[y[*2\\y\\*2]y]*2^y^*2_y_*2`y`*2aya*2byb*2cyc*2dyd*2eye*2fyf*2gyg*2\200\310yh*\200\216*r\000\037,\000\035\200\220*r\001\037,\001\035\200\220",
    "*r\002\037,\002\035\200\220*r\003\037,\003\035\200\220*r\004\037,\004\035\200\220*r\005\037,\005\035\200\220*r\006\037,\006\035\200\220*r\007\037,\007\035\200\220*r\010\037,\010\035\200\220*r\011\037,\011\035\200\220*r\012\037,\012\035\200\220*r\013\037,\013\035\200\220*r\014\037,\014\035\200\220*r\015\037,\015\035\200\220*r\016\037,\016\035\200\220*r\017\037,\017\035\200\220*r\020\037,\020\035\200\220*r\021\037,\021\035\200\220*r\022\037,\022\035\200\220*r\023\0372} \200\220*r\024\0372] \200\220*r\025\037,\023\035\200\220*r\026\0372) \200\220*r\027\037,\024\035\200\220*r\031\037,\025\035\200\220*r\032\037,\026\035\200\220*r\033\037,\027\035\200\220*r\034\037,\030\035\200\220*r\035\037,\031\035\200\220*r\036\037,\032\035\200\220*r\037\037,\033\035\200\220*r \037,\034\035\200\220*r!\037,\035\035\200\220*r\"\037,\036\035\200\220*r#\037,\037\035\200\220*r$\037, \035\200\220*r%\037,!\035\200\220*r&\037,\"\035\200\220*r'\037,#\035\200\220*r(\037,$\035\200\220*r)\037,%\035\200\220*r*\037,&\035\200\220*r+\037,'\035\200\220*r,\0372& \200\220*r-\037,(\035\200\220*r.\037,)\035\200\220*r/\037,*\035\200\220*r0\037,+\035\200\220*r1\0372! \200\220*r2\0372| \200\220*r3\037,,\035\200\220*r4\0372~ \200\220*r5\037,-\035\200\220*r6\0372: \200\220*r7\0372, \200\220*r8\037,.\035\200\220*r9\037,/\035\200\220*r:\0372/ \200\220*r;\037,0\035\200\220*r<\0372$ \200\220*r=\037,1\035\200\220*r>\037,2\035\200\220*r?\037,3\035\200\220*r@\037,4\035\200\220*rA\0372= \200\220*rB\037,5\035\200\220*rC\037,6\035\200\220*rD\0372> \200\220*rE\037,7\035\200\220*rF\037,8\035\200\220*rG\037,9\035\200\220*rH\037,:\035\200\220*rI\0372< \200\220*rJ\037,;\035\200\220*rK\037,<\035\200\220*rL\0372- \200\220*rM\037,=\035\200\220*rN\037,>\035\200\220*rO\037,?\035\200\220*rP\037,@\035\200\220*rQ\037,A\035\200\220*rR\037,B\035\200\220*rS\0372{ \200\220*rT\0372[ \200\220*rU\0372( \200\220*rV\037,C\035\200\220*rW\037,D\035\200\220*rX\0372% \200\220*rY\0372. \200\220*rZ\0372+ \200\220*r[\0372^ \200\220*r\\\037,E\035\200\220*r]\0372? \200\220*r^\0372; \200\220*r_\037,F\035\200\220*r`\037,G\035\200\220*ra\037,H\035\200\220*rb\037,I\035\200\220*rc\037,J\035\200\220*rd\0372* \200\220*re\037,K\035\200\220*rf\037,L\035\200\220*rg\037,M\035\200\220wi*\200\216wj*\011+\011*\200\205\000\203\274*\011+\011\200\206)*r\002\037\200\212*r\003\037\200\212*r\004\037\200\212*r\005\037\200\212*r\006\037\200\212*r\007\037\200\212*r\010\037\200\212*r\011\037\200\212*r\012\037\200\212*r\013\037\200\212*r\014\037\200\212*r\015\037\200\212*r\016\037\200\212*r\017\037\200\212*r\020\037\200\212*r\021\037\200\212*r\022\037\200\212*r\033\037\200\212*r\034\037\200\212*r\035\037\200\212*r\036\037\200\212*r\037\037\200\212*r \037\200\212*r!\037\200\212*r\"\037\200\212*r#\037\200\212*r(\037\200\212*r)\037\200\212*r-\037\200\212*r.\037\200\212*r/\037\200\212*rC\037\200\212*rF\037\200\212*rG\037\200\212*rJ\037\200\212*rP\037\200\212*rQ\037\200\212*rR\037\200\212*rW\037\200\212*",
    "rc\037\200\212*rg\037\200\212e\377`\377e\3762\000g\375\001\026`\376b\375\200\210d\374k\375*pj*pi_\374\200\221_\374\200\220\006b\375`\376\200\207G\003\342\007\007*\011+\011*r\000g\377b\377\200\204$r\032>\002\006*p\005\011\001\200\267b\377\200\204$r\001>\004\005*p\005+?\002 \200\203\027\200\205\000\203n\032\027,\000\035\200\205\001\203\241*p\005\035\200\205\001\203\2412] \200\205\001\203\241\200\205\000\203o\011\001\200\207b\377\200\204$r*>\002\006*p\005\011\001xb\377\200\204$r$>\002\021\200\203\027\200\205\000\203n\032\027*q\003\036\200\205\001\203\241\011\001^b\377\200\204$r&>\002\026*q\003(g\376\200\203\027\200\205\000\203n\032\027b\376\037\200\205\001\203\241\011\001?b\377\200\204$r'>\002\032*q\003(g\375b\375g\374\200\203\027\200\205\000\203n\032\027b\374 \200\205\001\203\241\011\001\034\200\204$pib\377\037\200\221d\373_\373\"+?\002\010_\373\"\032\013\011\001\000,\001\011+\011*b\376y\000*`\377r\003y\001*`\377r\004y\002*`\377p\000w\004*\011+\011*`\377b\376\200\205\002\203\275\006*`\375\200\205\000\203{w\005*\011+\011*`\377b\376\200\205\002\203\275\006*a\375x\003*\011+\011*`\377w\004*b\376y\001*b\375y\002*\011+\011\200\203\027\200\205\000\203n\032\027e\376`\376,\000\035\200\205\001\203\241*p\004\035\200\205\001\203\241,\001\035\200\205\001\203\241*r\001\037\200\205\001\203\241,\002\035\200\205\001\203\241*r\002\037\200\205\001\203\241\006`\376,\003\035\200\205\001\203\241`\377\035\200\205\001\203\241\006`\376\200\205\000\203o\011+\011*\011+\011*\200\203\027\200\205\000\203n\032\027w\005*\200\203\027\200\205\000\203n\032\027w\006*\011+\011*`\377w\000*\200\203#*p\000\200\205\001\203\254w\001*\200\206\000w\002\001\000*\200\205\000\203\305\003\372*p\002\011+\011*`\377w\000*\200\203#*p\000`\376\200\205\002\203\273w\001*\200\206\000w\002\001\000*\200\205\000\203\305\003\372*p\002\011+\011*p\001\200\205\000\203\257^\002\0044\014\001\000*p\001\200\205\000\203\261b\377A\002\0044\014\001\000*p\001\200\205\000\203\260\0065\0144\0142\000g\376\001\030*p\001b\376\200\205\001\203\265`\377b\376\200\205\001\203\203A\002\0044\014\001\000k\376b\376`\377\200\205\000\203\202G\003\336`\377\200\205\000\203\202\001\007*p\001\200\205\000\203\260\006\021\3675\0144\014*2\012\200\205\001\203\266^\002\0044\014\001\000\001\000*2\012\200\205\001\203\266\003\3705\0144\014\200\203\027\200\205\000\203n\032\027e\376`\376,\000\035\200\205\001\203\241\006`\376*p\000\035\200\205\001\203\241\006`\376,\001\035\200\205\001\203\241\006`\376*r\003\037\200\205\001\203\241\006`\376,\002\035\200\205\001\203\241\006`\376*r\004\037\200\205\001\203\241\006`\376,\003\035\200\205\001\203\241\006`\376`\377\035\200\205\001\203\241\006`\376\200\205\000\203o\011+\011\001\000*2 \200\205\001\203\266\003\370*p\001\200\205\000\203\257^\002\0044\014\001\000**p\001r\003y\003**p\001r\004y\004*p\001\200\205\000\203\261g\377b\3772\012>\002\200\227*p\001\200\205\000\203\260\006*p\006\200\205\000\203\202;\002s*p\002\200\207;\004\020*p\002\200\205\000\203\220\"\032%r\000\200\204$r\001>\004\015*p\002\200\205\000\203\220\"\032%p\005+?\002/*p\002\200\205\000\203\220\"\032%\200\203\027\200\205\000\203n\032\027*p\002\200\205\000\203\220\"\032%p\005\035\200\205\001\203\241\200\205\000\203\242*p\006\035\200\205\001\203\241\032\013w\005\001\023*p\002\200\203%*\200\204$r\001*p\006\200\205\003\203\276\035\200\212\006*p\006\200\205\000\203\206\006\001\020*p\002\200\203%*\200\204$r\001\200\205\002\203\275\035\200\212\0065\014\001\017b\3772#>\002\010*\200\205\000\203\3165\014\001\000*p\006\200\205\000\203\206\006b\3772aE\004\005b\3772zC\005\014b\3772AE\004\005b\3772ZC\005\005b\3772_>\002@*\200\205\000\203\306\200\204$pj*p\005\035\200\205\001\203\225\002\030*p\002",
    "\200\203%*\200\204$pj*p\005\035\200\221$\200\205\002\203\275\035\200\212\006\001\023*p\002\200\203%*\200\204$r\032*p\005\200\205\003\203\276\035\200\212\0065\014\001Xb\3772\">\002\012*2\"\200\205\001\203\3075\014\001Gb\3772'>\002\012*2'\200\205\001\203\3075\014\0016b\37720E\004\005b\37729C\002\010*\200\205\000\203\3145\014\001 *\200\205\000\203\317g\376b\3762\0017A\002\020*p\002\200\203%*b\376\200\205\002\203\275\035\200\212\006\001\0005\0144\014*p\005\200\205\000\203\206\006*p\005*p\001\200\205\000\203\260 \200\205\001\203\241\0065g\377\001Y*p\001\200\205\000\203\257\002N*p\001\200\205\000\203\261g\376b\3762aE\004\005b\3762zC\005\014b\3762AE\004\005b\3762ZC\005\005b\3762_>\005\014b\37620E\004\005b\37629C\002\020*p\005*p\001\200\205\000\203\260 \200\205\001\203\241\006\001\0034g\377\001\0034g\377b\377\003\260\377\377\377\243\007\007*p\001\200\205\000\203\260\006*p\005\200\205\000\203\206\006\001)*p\001\200\205\000\203\261g\376b\376b\377>\002\016*p\001\200\205\000\203\260\006*\200\205\000\203\320\007\001\014*p\005*\200\205\000\203\311 \200\205\001\203\241\006*p\001\200\205\000\203\257\003\317*,\000\200\205\001\203\301\016\007\007*p\005\200\205\000\203\206\006\001U*p\001\200\205\000\203\257\002M*p\001\200\205\000\203\261g\377b\3772/>\0020*p\001\200\205\000\203\260\006*p\001\200\205\000\203\261g\377b\3772/>\002\016*p\001\200\205\000\203\260\006*\200\205\000\203\320\007\001\012*p\0052/ \200\205\001\203\241\006\001\014*p\005*\200\205\000\203\311 \200\205\001\203\241\006\001\000*p\001\200\205\000\203\257\003\260\377\377\377\243*,\000\200\205\001\203\301\016\007\007*p\001\200\205\000\203\257^\002\011*,\000\200\205\001\203\301\016\001\000*p\001\200\205\000\203\261g\377b\3772\012>\002\011*,\001\200\205\001\203\301\016\001\000b\3772\\>\002\200\363*p\001\200\205\000\203\260\006*p\001\200\205\000\203\257^\002\011*,\002\200\205\001\203\301\016\001\000*2b\200\205\001\203\266\002\0052\010\014\001\000*2f\200\205\001\203\266\002\0052\014\014\001\000*2n\200\205\001\203\266\002\0052\012\014\001\000*2r\200\205\001\203\266\002\0052\015\014\001\000*2t\200\205\001\203\266\002\0052\011\014\001\000*20\200\205\001\203\266\002\0052\000\014\001\000*2/\200\205\001\203\266\002\0052/\014\001\000*2'\200\205\001\203\266\002\0052'\014\001\000*2\\\200\205\001\203\266\002\0052\\\014\001\000*2\"\200\205\001\203\266\002\0052\"\014\001\000*2x\200\205\001\203\266\002\011*2\002\200\205\001\203\312\014\001\000*2u\200\205\001\203\266\002\011*2\004\200\205\001\203\312\014\001\000*\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027,\003\035\200\205\001\203\241,\004\035\200\205\001\203\241\035\200\205\001\203\241,\005\035\200\205\001\203\241\035\200\205\001\203\241\200\205\001\203\301\016\001\000*p\001\200\205\000\203\260g\376b\3762\200\200X2\000A\002Y*p\001\200\205\000\203\260g\375b\3762 X2\000>\002\031b\3762\037Xg\376b\3762?Xg\375b\3762\006[b\375Y\014\001,*p\001\200\205\000\203\260g\374b\3762\017Xg\376b\3752?Xg\375b\3742?Xg\374b\3762\014[b\3752\006[Yb\374Y\014\001\000b\376\0144\0142\000g\376b\377\001\200\255*p\001\200\205\000\203\257^\002%*\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027b\377\037\200\205\001\203\241,\000\035\200\205\001\203\241\035\200\205\001\203\241\200\205\001\203\301\016\001\000*\200\205\000\203\313^\002e*p\001\200\205\000\203\261g\375\200\203\027\200\205\000\203n\032\027e\374`\374,\001\035\200\205\001\203\241\006b\3752 G\005\005b\3752\177>\002\013`\374b\375\037\200\205\001\203\241\006\001\025`\3742' \200\205\001\203\241b\375 \200\205\001\203\2412' \200\205\001\203\241\006`\3742. \200\205\001\203\241\006*\200\203\027\200\205\000\203n\032\027`\374\035\200\205\001\203\241\200\205\001\203\301\016\001\000*p\001\200\205\000\203\260)g\373b\3762\004[b\373Ng",
    "\376\021\260\377\377\377Qb\376\0144\014*p\001\200\205\000\203\261g\377b\37720E\004\005b\37729C\005\014b\3772aE\004\005b\3772fC\005\014b\3772AE\004\005b\3772FC\0144\014*2-\200\205\001\203\266g\3774g\376*\200\205\000\203\315f\375*p\001\200\205\000\203\261g\374\001\034*p\001\200\205\000\203\260)g\373a\375.\000Qb\373'Mf\375*p\001\200\205\000\203\261g\374b\37420E\004\005b\37429C\003\326*p\001\200\205\000\203\2612.>\002e*p\0012\001\200\205\001\203\265g\374b\37420E\004\005b\37429C\002-*p\001\200\205\000\203\260\0065g\376*p\001r\001g\372*\200\205\000\203\315f\371a\375a\3712\012*p\001r\001b\372PW'SMf\375\001\036b\377\002\007a\3756f\375\001\000*p\002\200\203%*\200\204$r&a\375\200\205\003\203\277\035\200\212\006\007\001\000*2E\200\205\001\203\266\005\006*2e\200\205\001\203\266\00255g\376*2-\200\205\001\203\266g\370b\370^\002\011*2+\200\205\001\203\266\006\001\000*\200\205\000\203\315f\367b\370\002\011a\375a\367Sf\375\001\007a\375a\367Qf\375\001\000b\377\002\007a\3756f\375\001\000b\376\002\024*p\002\200\203%*\200\204$r$a\375\200\205\003\203\277\035\200\212\006\001\022*p\002\200\203%*\200\204$r&a\375\200\205\003\203\277\035\200\212\006\007\007.\000f\377*p\001\200\205\000\203\261g\376\001\034*p\001\200\205\000\203\260)g\375a\377.\001Qb\375'Mf\377*p\001\200\205\000\203\261g\376b\37620E\004\005b\37629C\003\326a\377\0130\013*p\006\200\205\000\203\202;\002\011*p\006\200\205\000\203\242\006\001\000*p\001\200\205\000\203\260\006*2{\200\205\001\203\266\002<5g\377\001-*2}\200\205\001\203\266\002\027*2#\200\205\001\203\266\002\003\007\001\012*p\0062} \200\205\001\203\241\006\001\016*p\006*p\001\200\205\000\203\260 \200\205\001\203\241\006*p\001\200\205\000\203\257\003\313\001#\001\016*p\006*p\001\200\205\000\203\260 \200\205\001\203\241\006*p\001\200\205\000\203\257\004\011*p\001\200\205\000\203\2612\012A\003\337\007\007*p\001\200\205\000\203\260g\377b\3772\012>\002\007\200\204$r\001\014\001\203\215b\3772!>\002\026*2=\200\205\001\203\266\002\007\200\204$rO\014\001\005\200\204$r1\014\001\203pb\3772$>\002\007\200\204$r<\014\001\203bb\3772%>\002\026*2=\200\205\001\203\266\002\007\200\204$rM\014\001\005\200\204$rX\014\001\203Eb\3772&>\002'*2&\200\205\001\203\266\002\011*,\000\200\205\001\203\301\016\001\024*2=\200\205\001\203\266\002\007\200\204$r0\014\001\005\200\204$r,\014\001\203\027b\3772(>\002\007\200\204$rU\014\001\203\011b\3772)>\002\007\200\204$r\026\014\001\202\373b\3772*>\002\026*2=\200\205\001\203\266\002\007\200\204$rN\014\001\005\200\204$rd\014\001\202\336b\3772+>\002%*2=\200\205\001\203\266\002\007\200\204$r+\014\001\024*2+\200\205\001\203\266\002\007\200\204$rE\014\001\005\200\204$rZ\014\001\202\262b\3772,>\002\007\200\204$r7\014\001\202\244b\3772->\002%*2=\200\205\001\203\266\002\007\200\204$rb\014\001\024*2-\200\205\001\203\266\002\007\200\204$r9\014\001\005\200\204$rL\014\001\202xb\3772.>\002m*2.\200\205\001\203\266\002O*2.\200\205\001\203\266\002\"\001\000*2 \200\205\001\203\266\003\370*2\012\200\205\001\203\266^\002\011*,\001\200\205\001\203\301\016\001\0002\0017\014\001#*2<\200\205\001\203\266\002\007\200\204$rf\014\001\024*2>\200\205\001\203\266\002\007\200\204$r=\014\001\005\200\204$re\014\001\024*2=\200\205\001\203\266\002\007\200\204$rK\014\001\005\200\204$rY\014\001\202\004b\3772/>\002(*2/\200\205\001\203\266\002\012*\200\205\000\203\3102\0017\014\001\024*2=\200\205\001\203\266\002\007\200\204$r;\014\001\005\200\204$r:\014\001\201\325b\3772:>\0024",
    "*,\002\200\205\001\203\270\002\007\200\204$r_\014\001\036*,\003\200\205\001\203\270\002\007\200\204$r`\014\001\017*,\004\200\205\001\203\270\002\007\200\204$ra\014\001\000\200\204$r6\014\001\201\232b\3772;>\002\007\200\204$r^\014\001\201\214b\3772<>\0024*2<\200\205\001\203\266\002\007\200\204$rV\014\001#*2=\200\205\001\203\266\002\007\200\204$rH\014\001\024*2>\200\205\001\203\266\002\007\200\204$r8\014\001\005\200\204$rI\014\001\201Qb\3772=>\002\026*2=\200\205\001\203\266\002\007\200\204$r@\014\001\005\200\204$rA\014\001\2014b\3772>>\002%*2=\200\205\001\203\266\002\007\200\204$rB\014\001\024*2>\200\205\001\203\266\002\007\200\204$r\027\014\001\005\200\204$rD\014\001\201\010b\3772?>\002\007\200\204$r]\014\001\200\372b\3772@>\002\007\200\204$r/\014\001\200\354b\3772[>\002\026*2]\200\205\001\203\266\002\007\200\204$r?\014\001\000\200\204$rT\014\001\200\317b\3772]>\002\007\200\204$r\024\014\001\200\301b\3772^>\002\026*2=\200\205\001\203\266\002\007\200\204$r\\\014\001\005\200\204$r[\014\001\200\244b\3772{>\002\026*2}\200\205\001\203\266\002\007\200\204$r>\014\001\000\200\204$rS\014\001\200\207b\3772|>\002%*2=\200\205\001\203\266\002\007\200\204$r3\014\001\024*2|\200\205\001\203\266\002\007\200\204$r\001\014\001\005\200\204$r2\014\001[b\3772}>\002\026*2#\200\205\001\203\266\002\007\200\204$r\025\014\001\005\200\204$r\023\014\001>b\3772~>\002\026*2=\200\205\001\203\266\002\007\200\204$r5\014\001\005\200\204$r4\014\001!*\200\203\027\200\205\000\203n\032\027,\005\035\200\205\001\203\241b\377 \200\205\001\203\241,\006\035\200\205\001\203\241\200\205\000\203o\200\205\001\203\301\0164\014*p\005\200\205\000\203\2022\001>\002\037*p\0052\000\200\205\001\203\203'f\377*p\002\200\203%*\200\204$r'a\377\200\205\003\203\277\035\200\212\006\001\023*p\002\200\203%*\200\204$r**p\005\200\205\003\203\276\035\200\212\006\007\007*\011+\011*`\377w\000**p\000\200\207y\002*2\000y\001*\011+\011*\200\204&`\377\200\205\001\203\302w\000**p\000\200\207y\002*2\000y\001*\011+\011*\200\204&`\377`\376\200\205\002\203\303w\000**p\000\200\207y\002*2\000y\001*\011+\011*2\000y\001\007\007*r\001*r\002G\0144\014*p\000*r\001\200\210\"\032%\011+\011*p\000*r\001b\377N\200\210\"\032%\011+\011*p\000*r\001\200\210\"\032%e\377**r\0012\001Ny\001`\377\011+\011*\200\206\000w\004*\200\206\000w\005*\011+\011*`\377w\000*\200\203'*p\000\200\205\001\203\254w\001*\200\205\000\203\334\007\007`\376p\0012<\200\205\001\203\177g\374\200\216e\373`\377p\003e\372`\377p\002\200\207;\002\201\363b\374^\002|`\377p\002\200\2072\001>\0029`\375\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027,\000\035\200\205\001\203\241`\376p\001\035\200\205\001\203\241\035\200\205\001\203\2412. \200\205\001\203\241\035\200\205\001\203\241\200\205\001\203\301\016\0017`\375\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027,\001\035\200\205\001\203\241`\376p\001\035\200\205\001\203\241\035\200\205\001\203\2412. \200\205\001\203\241\035\200\205\001\203\241\200\205\001\203\301\016\001\000*\200\203'`\376p\001`\376p\001`\376p\0012<\200\205\001\203~\200\205\001\203}\200\205\002\203\273w\001*\200\204$rV\200\205\001\203\3272\000g\371\001Rb\3712\000I\002\027*\200\204$r7\200\205\001\203\326^\002\012`\375,\002\200\205\001\203\301\016\001\000\001\000\200\206\000e\372\001\011`\372*\200\205\000\203\260\035\200\212\006*\200\204$r7\200\205\001\203\325\005\010*\200\204$r\027\200\205\001\203\325^\003\342`\373`\377p\002b\371\200\210`\372\035\200\220\006k\371b\371`\377p\002\200\207G\003\260\377\377\377\244*\200\204$r\027\200\205\001\203\327\200\206\000e\372`\377p\003\200\207g\370",
    "2\000g\367\001\200\312`\377p\003b\367\200\210\"\032%e\366`\366r\000\200\204$r<>\002\200\252k\367b\367b\370>\005\021`\377p\003b\367\200\210\"\032%r\000\200\204$r\032A\002\022`\377p\003b\367\200\210\"\032%,\003\200\205\001\203\301\016\001\000`\377p\003b\367\200\210\"\032%p\005e\365`\373`\365\035\200\221\"\032\017e\364`\3649\002'`\364e\3632\000g\362\001\023`\363b\362\200\210d\361k\362`\372_\361\"\032%\035\200\212\006b\362`\363\200\207G\003\345\0015`\366\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027,\004\035\200\205\001\203\241`\365\035\200\205\001\203\241\035\200\205\001\203\2412. \200\205\001\203\241\035\200\205\001\203\241\200\205\001\203\301\016\001\007`\372`\366\035\200\212\006k\367b\367b\370G\003\260\377\377\377/\001D`\376p\0012<\200\205\001\203\177\0029`\366\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027,\005\035\200\205\001\203\241`\376p\001\035\200\205\001\203\241\035\200\205\001\203\2412. \200\205\001\203\241\035\200\205\001\203\241\200\205\001\203\301\016\001\000*\200\203'`\372\200\205\001\203\321w\001*`\376w\002*\200\205\000\203\336\007\007+e\376*p\001\200\205\000\203\257\005\006*p\001r\002;\002!*p\001\200\205\000\203\257\002\012*p\001\200\205\000\203\261e\376\001\015*p\001p\000\200\205\000\203\220\"\032%e\376\001\016\200\203%*p\0002\0012\001\200\205\003\203\300e\376`\376`\377\200\205\001\203\301\011+\011*p\001\200\205\000\203\261\011+\011*p\001b\377\200\205\001\203\265\011+\011*p\001\200\205\000\203\257\004\013*p\001\200\205\000\203\261r\000b\377>\0144\014*p\001\200\205\000\203\260\011+\011*p\001\200\205\000\203\257^\002\0044\014\001\000*\200\205\000\203\261r\000b\377A\002\0044\014\001\000*p\001\200\205\000\203\260\0065\0144\014*b\377\200\205\001\203\326\002\003\007\001\000*p\001\200\205\000\203\261e\376`\376\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\0272' \200\205\001\203\241\200\204$pib\377\037\200\221\200\205\001\203\241\035\200\205\001\203\241,\000\035\200\205\001\203\241\035\200\205\001\203\241\200\205\001\203\301\016\007\007*p\001\200\205\000\203\257^\002\0044\014\001\000*\200\205\000\203\261r\000\200\204$r\032A\002\0044\014\001\000*\200\205\000\203\261p\005`\377\200\205\001\203\2042\000>^\002\0044\014\001\000*p\001\200\205\000\203\260\0065\0144\014*\200\204$r\001\200\205\001\203\326^\002\0044\014\001\000\001\000*\200\204$r\001\200\205\001\203\326\003\3665\0144\014*\200\204$r\001\200\205\001\203\325\005\010*\200\204$r^\200\205\001\203\325\0144\014\001\000*\200\204$r\001\200\205\001\203\326\005\010*\200\204$r^\200\205\001\203\326\003\354\007\007*\200\204$r\001\200\205\001\203\326\005\010*\200\204$r^\200\205\001\203\326^\0020*\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027,\000\035\200\205\001\203\241*\200\205\000\203\261\200\205\000\203o\035\200\205\001\203\241,\001\035\200\205\001\203\241\035\200\205\001\203\241\200\205\001\203\301\016\001\000\007\007*\200\204$r\032\200\205\001\203\325^\002;*\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027,\000\035\200\205\001\203\241*\200\205\000\203\261\200\205\000\203o\035\200\205\001\203\241\035\200\205\001\203\241,\001\035\200\205\001\203\241\035\200\205\001\203\241\200\205\001\203\301\016\001\000*\200\205\000\203\260p\005\011+\011*\200\205\000\203\304\006\001G*\200\204$r\002\200\205\001\203\325\002\012*\200\204$r\006\200\205\001\203\335\001.*\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027,\000\035\200\205\001\203\241*\200\205\000\203\261\200\205\000\203o\035\200\205\001\203\241,\001\035\200\205\001\203\241\035\200\205\001\203\241\200\205\001\203\301\016*\200\205\000\203\304\006*p\001\200\205\000\203\257\003\260\377\377\377\261\007\007*\200\205\000\203\260e\376*\200\205\000\203\333e\375\200\203+`\376`\375\200\205\002\204\036e\374*\200\204$rV\200\205\001\203\326\00235g\373\001\0264g\373*\200\204$r<\200\205\001\203\327`\374p\002*\200\205\000\203\333\035\200\212\006b\373\005\010*\200\204$r7\200\205\001\203\326\003\334*\200\204$r\027\200\205\001\203\327\001\000\200\2042`\374\200\205\001\204>\001\017*\200\205\000\203\260",
    "e\376`\374p\003`\376\035\200\212\006`\376r\000b\377A\003\350\007\007*\200\204$r6\200\205\001\203\326\002&5g\377\001\0214g\377*p\002p\005*\200\205\000\203\342p\000\035\200\212\006b\377\005\010*\200\204$r7\200\205\001\203\326\003\341\001\000*\200\205\000\203\332*\200\205\000\203\331\001b*\200\204$r\022\200\205\001\203\326\002\016*\200\205\000\203\332\001\000*\200\205\000\203\337\003\372\001F*\200\204$r\016\200\205\001\203\326\002\016*\200\205\000\203\332\001\000*\200\205\000\203\343\003\372\001.*\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027,\000\035\200\205\001\203\241*\200\205\000\203\261\200\205\000\203o\035\200\205\001\203\241,\001\035\200\205\001\203\241\035\200\205\001\203\241\200\205\001\203\301\016*\200\205\000\203\331*\200\204$r\006\200\205\001\203\326^\003\260\377\377\377\223\007\007*\200\204$r\032\200\205\001\203\325^\002\0044\014\001\000*p\004\200\215\006\001>*\200\205\000\203\261e\377*\200\205\000\203\333e\376\200\2038`\377*p\002`\376\200\205\003\204Te\375*p\002`\375\200\205\001\204\014*p\004`\375\035\200\212\006*\200\204$rA\200\205\001\203\326\002\012`\375*\200\205\000\203\352w\006\001\000*\200\204$r\032\200\205\001\203\325\003\260\377\377\377\270*\200\204$r6\200\205\001\203\326\002-*\200\205\000\203\342e\374*p\004e\3732\000g\372\001\022`\373b\372\200\210d\371k\372_\371\"\0328`\374w\002b\372`\373\200\207G\003\346\001\0004\014*\200\204$rT\200\205\001\203\326^\002\003\007\001\000\001\015*\200\205\000\203\333e\376`\377`\376\200\205\001\204\037*\200\204$r\032\200\205\001\203\325\005\010*\200\204$r7\200\205\001\203\326\003\337*\200\204$r\024\200\205\001\203\327\007\007\200\203\027\200\205\000\203n\032\027e\377*\200\204$rV\200\205\001\203\326\006`\377,\000\035\200\205\001\203\241\006\001\016`\377*\200\205\000\203\260\200\205\000\203o\035\200\205\001\203\241\006*\200\204$r\027\200\205\001\203\326^\003\347`\377,\001\035\200\205\001\203\241\006`\377\011+\011*\200\205\000\203\261e\377*\200\205\000\203\333e\376*\200\204$rV\200\205\001\203\325\002\031\200\203\027\200\205\000\203n\032\027`\376\035\200\205\001\203\241*\200\205\000\203\341\035\200\205\001\203\241e\376\001\000\200\2044`\377`\376\200\205\002\204De\375*\200\204$r]\200\205\001\203\326\002\007`\375p\013\011\001\000`\375p\014\011+\011*\200\205\000\203\261e\377*\200\204$r\017\200\205\001\203\326^\002\0044\014\001\000*\200\205\000\203\333e\376*\200\203*`\377*p\002`\3762\000\200\205\004\204\031w\003*\200\204$rU\200\205\001\203\326\002W*\200\204$r\032\200\205\001\203\325\002C5g\375\001.4g\375*\200\205\000\203\261e\377*\200\205\000\203\333e\374*\200\204$r6\200\205\001\203\327*\200\205\000\203\342e\373*p\003\200\2039`\377`\374`\373\200\205\003\204V\200\205\001\204\034b\375\005\010*\200\204$r7\200\205\001\203\326\003\304\001\000*\200\204$r\026\200\205\001\203\327\001\000*\200\204$rY\200\205\001\203\326\002\013*p\003*\200\205\000\203\342w\005\001\000**p\003p\010\200\205\001\203\340*p\003p\010,\000\200\205\001\204!\002\021*p\003*p\003r\004\200\2047r\002Yy\004\001\000*\200\204$r6\200\205\001\203\326\006*p\002*p\003\200\205\001\204\015**p\003p\011\200\205\001\203\3445\0144\014*\200\205\000\203\331\001!*`\3775\200\205\002\203\347\001\000*\200\204$r\001\200\205\001\203\326\005\010*\200\204$r^\200\205\001\203\326\003\354*\200\205\000\203\331*\200\205\000\203\346\003\331\007\007\001!*`\3774\200\205\002\203\347*\200\204$r^\200\205\001\203\326^\002\003\007\001\000\001\000*\200\204$r^\200\205\001\203\326\003\366*\200\205\000\203\346\003\331*\200\204$r\001\200\205\001\203\327\007\007*p\001\200\205\000\203\257^\002\0044\014\001\000*\200\205\000\203\261r\000\200\204$r\030I\0144\014*\200\205\000\203\261e\375b\376\002\031*\200\204$r\034\200\205\001\203\325\002\015`\377*\200\205\000\203\350\200\205\001\2049\006\007\001\000\001 4g\374*\200\204$r\034\200\205\001\203\325\002\0055g\374\001\000b\374\002\012`\375,\000\200\205\001\203\301\016\001\000*\200\204$rJ\200\205\001\203\325\002\011*`\377\200\205\001\203\351\007\001o*\200\204$r\036\200\205\001\203\326\002-*\200\205\000\203\330\002\021`\377\200\203[`\375\200\205\001\204,\032[\200\205\001\2049\006",
    "\001\023`\377\200\203>`\375*\200\205\000\203\352\200\205\002\204;\032>\200\205\001\2049\006\007\0018*\200\204$r!\200\205\001\203\326\002\022`\377\200\203X`\375\200\205\001\204,\032X\200\205\001\2049\006\007\001\034*\200\204$r\"\200\205\001\203\326\002\022`\377\200\203Y`\375\200\205\001\204,\032Y\200\205\001\2049\006\007\001\000*,\001\200\205\001\203\270\002\026`\377\200\203V`\375*\200\205\000\203\352\200\205\002\204;\032V\200\205\001\2049\006\007\001\000*\200\205\000\203\352e\373*\200\205\000\203\261e\375*\200\204$rA\200\205\001\203\326\002\026`\377\200\203\177`\375`\373*\200\205\000\203\352\200\205\003\204x\200\205\001\2049\006\007\001\000`\377`\373\200\205\001\2049\006\007\007*\200\205\000\203\260e\377\200\203/`\377*\200\205\000\203\352\200\205\002\204;e\376*\200\205\000\203\304\002D*`\376p\002\200\205\001\203\344*\200\204$r\004\200\205\001\203\325\004\015*2\001\200\205\001\203\265r\000\200\204$r\001>\002\031`\376\200\203.*\200\205\000\203\260\200\205\001\204,\032.w\003*`\376p\003\200\205\001\203\344\001\000*\200\204$r\011\200\205\001\203\327\001?*`\376p\002\200\205\001\203\345*\200\205\000\203\304\006*\200\204$r\004\200\205\001\203\325\004\015*2\001\200\205\001\203\265r\000\200\204$r\001A\002\031`\376\200\203.*\200\205\000\203\260\200\205\001\204,\032.w\003*`\376p\003\200\205\001\203\345\001\000`\376\011+\011*p\005\200\215\006*\200\205\000\203\260e\3765g\375\001>4g\375*\200\205\000\203\261e\376*\200\205\000\203\333e\374\200\2039`\376`\374\200\205\002\204\036e\373*\200\204$rA\200\205\001\203\326\002\012`\373*\200\205\000\203\352w\003\001\000*p\005`\373\035\200\212\006*p\003`\373\200\205\001\204\035b\375\005\010*\200\204$r7\200\205\001\203\326\003\260\377\377\377\264*\200\204$r6\200\205\001\203\326\002-*\200\205\000\203\342e\372*p\005e\3712\000g\370\001\022`\371b\370\200\210d\367k\370_\367\"\0329`\372w\002b\370`\371\200\207G\003\346\001?*p\005e\3662\000g\365\001,`\366b\365\200\210d\364k\365_\364\"\0329e\363`\363p\002+K\004\005`\363p\0039\002\015`\363`\363p\003\200\205\000\204-w\002\001\000b\365`\366\200\207G\003\314*p\005e\3622\000g\361\001P`\362b\361\200\210d\360k\361_\360\"\0329e\357`\377\200\203~`\357p\000`\357\200\205\002\204\206\200\205\001\2049\006`\357p\0039\002\027`\377\200\203\200\200`\357p\000`\357`\357p\003\200\205\003\204\207\200\205\001\2049\006\001\021`\377\200\203\200\201`\357p\000`\357\200\205\002\204\206\200\205\001\2049\006b\361`\362\200\207G\003\260\377\377\377\250\007\007*\200\205\000\203\353\011+\011**\200\205\000\203\355\200\205\001\203\354\011+\011*\200\205\000\203\261e\376*\200\204$rg\200\205\001\203\326\002\026*\200\203v`\376`\377*\200\205\000\203\355\200\205\003\204x\032v\200\205\001\203\354\011\001\000`\377\011+\011**\200\205\000\203\357\200\205\001\203\356\011+\011*\200\205\000\203\261e\376*\200\204$rW\200\205\001\203\326\002\026*\200\203t`\376`\377*\200\205\000\203\357\200\205\003\204x\032t\200\205\001\203\356\011\001\000`\377\011+\011**\200\205\000\203\361\200\205\001\203\360\011+\011*\200\205\000\203\261e\376*\200\204$r-\200\205\001\203\326\002\026*\200\203s`\376`\377*\200\205\000\203\361\200\205\003\204x\032s\200\205\001\203\360\011\001\000`\377\011+\011**\200\205\000\203\363\200\205\001\203\362\011+\011*\200\205\000\203\261e\376*\200\204$r@\200\205\001\203\326\002\026*\200\203x`\376`\377*\200\205\000\203\363\200\205\003\204x\032x\200\205\001\203\362\011\001\200\274*\200\204$rO\200\205\001\203\326\002\026*\200\203y`\376`\377*\200\205\000\203\363\200\205\003\204x\032y\200\205\001\203\362\011\001\200\234*\200\204$rI\200\205\001\203\326\002\026*\200\203z`\376`\377*\200\205\000\203\363\200\205\003\204x\032z\200\205\001\203\362\011\001|*\200\204$rD\200\205\001\203\326\002\026*\200\203{`\376`\377*\200\205\000\203\363\200\205\003\204x\032{\200\205\001\203\362\011\001\\*\200\204$rH\200\205\001\203\326\002\026*\200\203|`\376`\377*\200\205\000\203\363\200\205\003\204x\032|\200\205\001\203\362\011\001<*\200\204$rB\200\205\001\203\326\002\026*\200\203}`\376`\377*\200\205\000\203\363\200\205\003\204x\032}\200\205\001\203\362\011\001\034*\200\204$rF\200\205\001\203\326\002\022",
    "\200\203e`\376`\377*\200\205\000\203\342\200\205\003\204w\032e\011\001\000`\377\011+\011**\200\205\000\203\365\200\205\001\203\364\011+\011*\200\205\000\203\261e\376*\200\204$r4\200\205\001\203\326\002\026*\200\203o`\376`\377*\200\205\000\203\365\200\205\003\204x\032o\200\205\001\203\364\011\001\000`\377\011+\011**\200\205\000\203\367\200\205\001\203\366\011+\011*\200\205\000\203\261e\376*\200\204$r2\200\205\001\203\326\002\026*\200\203n`\376`\377*\200\205\000\203\367\200\205\003\204x\032n\200\205\001\203\366\011\001\000`\377\011+\011**\200\205\000\203\371\200\205\001\203\370\011+\011*\200\205\000\203\261e\376*\200\204$r,\200\205\001\203\326\002\026*\200\203m`\376`\377*\200\205\000\203\371\200\205\003\204x\032m\200\205\001\203\370\011\001\000`\377\011+\011**\200\205\000\203\373\200\205\001\203\372\011+\011*\200\205\000\203\261e\376*\200\204$r_\200\205\001\203\326\002\026*\200\203p`\376`\377*\200\205\000\203\373\200\205\003\204x\032p\200\205\001\203\374\011\001@*\200\204$r`\200\205\001\203\326\002\026*\200\203q`\376`\377*\200\205\000\203\373\200\205\003\204x\032q\200\205\001\203\374\011\001 *\200\204$ra\200\205\001\203\326\002\026*\200\203r`\376`\377*\200\205\000\203\373\200\205\003\204x\032r\200\205\001\203\374\011\001\000`\377\011+\011**\200\205\000\203\375\200\205\001\203\374\011+\011*\200\205\000\203\261e\376*\200\204$rZ\200\205\001\203\326\002\026*\200\203g`\376`\377*\200\205\000\203\375\200\205\003\204x\032g\200\205\001\203\374\011\001 *\200\204$rL\200\205\001\203\326\002\026*\200\203h`\376`\377*\200\205\000\203\375\200\205\003\204x\032h\200\205\001\203\374\011\001\000`\377\011+\011**\200\205\000\203\377\200\205\001\203\376\011+\011*\200\205\000\203\261e\376*\200\204$rd\200\205\001\203\326\002\026*\200\203i`\376`\377*\200\205\000\203\377\200\205\003\204x\032i\200\205\001\203\376\011\001@*\200\204$r:\200\205\001\203\326\002\026*\200\203j`\376`\377*\200\205\000\203\377\200\205\003\204x\032j\200\205\001\203\376\011\001 *\200\204$rX\200\205\001\203\326\002\026*\200\203k`\376`\377*\200\205\000\203\377\200\205\003\204x\032k\200\205\001\203\376\011\001\000`\377\011+\011**\200\205\000\204\001\200\205\001\204\000\011+\011*\200\205\000\203\261e\376*\200\204$r[\200\205\001\203\326\002\026*\200\203l`\376`\377*\200\205\000\204\001\200\205\003\204x\032l\200\205\001\204\000\011\001\000`\377\011+\011*\200\205\000\203\261e\377*\200\204$rQ\200\205\001\203\326\002\020\200\203G`\377*\200\205\000\204\001\200\205\002\204;\032G\011\0014*\200\204$rL\200\205\001\203\326\002\020\200\203H`\377*\200\205\000\204\001\200\205\002\204;\032H\011\001\032*\200\204$r1\200\205\001\203\326\002\020\200\203I`\377*\200\205\000\204\001\200\205\002\204;\032I\011\001\000*\200\205\000\204\002\011+\011**\200\205\000\204\004\200\205\001\204\003\011+\011*\200\205\000\203\261e\376*\200\204$r]\200\205\001\203\326\002\022*\200\203J`\376`\377\200\205\002\204;\032J\200\205\001\204\003\011\001\000`\377\011+\011**\200\205\000\204\007\200\205\001\204\005\011+\011*\200\205\000\203\261e\376*\200\204$rY\200\205\001\203\326\002\027*`\376\200\205\001\204\006e\375`\375`\377w\001*`\375\200\205\001\204\005\011\001\000`\377\011+\011*\200\205\000\203\304\006*\200\205\000\203\333e\376*\200\204$rV\200\205\001\203\325\002\031\200\203\027\200\205\000\203n\032\027`\376\035\200\205\001\203\241*\200\205\000\203\341\035\200\205\001\203\241e\376\001\000\200\2031`\377`\376\200\205\002\204\036e\375*\200\204$rU\200\205\001\203\326\002D\200\203;\200\205\000\203ne\374`\375`\374w\003*\200\204$r\026\200\205\001\203\326^\002*5g\373\001\0154g\373`\374*\200\205\000\203\352\200\205\001\2049\006b\373\005\010*\200\204$r7\200\205\001\203\326\003\345*\200\204$r\026\200\205\001\203\327\001\000\001\000`\375\011+\011*\200\205\000\203\261e\377*\200\204$rU\200\205\001\203\326\002\023*\200\205\000\203\352e\376*\200\204$r\026\200\205\001\203\327`\376\011\001\200\351*\200\204$r\032\200\205\001\203\325\002\011*`\377\200\205\001\204\006\011\001\200\326*\200\204$r*\200\205\001\203\326\002\016\200\203D`\377`\377p\005\200\205\002\204\036\011\001\200\276*\200\204$rR\200\205\001\203\326\002\012\200\203E`\377\200\205\001\204,\011\001\200\252*\200\204$r$\200\205\001\203\326\002\016\200\203@`\377`\377q\003\200\205\002\204d\011\001\200\222*\200\204$r&",
    "\200\205\001\203\326\002\023`\377q\003(g\375\200\203A`\377b\375\200\205\002\204e\011\001u*\200\204$r'\200\205\001\203\326\002\023`\377q\003(g\374\200\203B`\377b\374\200\205\002\204f\011\001X*\200\204$r(\200\205\001\203\326\002\013\200\203C`\3775\200\205\002\204g\011\001C*\200\204$r)\200\205\001\203\326\002\013\200\203C`\3774\200\205\002\204g\011\001.*\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027,\000\035\200\205\001\203\241*\200\205\000\203\261\200\205\000\203o\035\200\205\001\203\241,\001\035\200\205\001\203\241\035\200\205\001\203\241\200\205\001\203\301\016+\011*\200\203,\200\205\000\203n\032,w\004*4{\010*4{\011*4{\012*\011+\011*p\001\011+\011*`\377w\000*`\376w\001*b\375y\002*\200\206\000w\005*\200\206\000w\006*\200\206\000w\007*\200\203-*5\200\205\002\204#w\013*\200\203-*4\200\205\002\204#w\014*\011+\011*r\002\200\2047r\000X2\000A\0144\014*\200\205\000\204\011^\0144\014*`\377K\002\0045\014\001\000*\200\205\000\204\011\005\005`\377\200\205\000\204\011\002\0044\014\001\000*p\005e\3762\000g\375\001\031`\376b\375\200\210d\374k\375_\374\"\032)`\377\200\205\001\204\013\002\0045\014\001\000b\375`\376\200\207G\003\3374\0144\014`\377*p\007\200\207y\005*p\007`\377\035\200\212\006\007\007*p\006`\377\035\200\212\006\007\007*p\007e\3762\000g\375\001\"`\376b\375\200\210d\374k\375_\374\"\0328p\003`\377\200\205\001\203\2042\000>\002\010_\374\"\0328\011\001\000b\375`\376\200\207G\003\326+\011+\011*p\006e\3762\000g\375\001#`\376b\375\200\210d\374k\375_\374\"\032*e\373`\373p\003`\377\200\205\001\203\2042\000>\002\005`\373\011\001\000b\375`\376\200\207G\003\325+\011+\011**p\000\200\205\001\204\021\011+\011*t\011\002\004*\011\001\000*5{\011\200\204:*w\000*t\010^\002\012\200\2042*`\377\200\205\002\204@\001\000*\200\205\000\204\012\002 *p\005\200\2072\000>\004\007*\200\2044p\001K^\002\014*p\005\200\2044p\001\035\200\212\006\001\000\001\000*p\005e\3762\000g\375\001\022`\376b\375\200\210d\374k\375_\374\"\032)\200\205\000\204\020\006b\375`\376\200\207G\003\346*p\006e\3732\000g\372\001\021`\373b\372\200\210d\371k\372_\371\"\032*\200\205\000\204\020b\372`\373\200\207G\003\347\200\204:p\004e\370`\370\200\215\006`\370*p\007\200\205\001\203\210\006*p\007\200\215\006*p\005e\3672\000g\366\001\024`\367b\366\200\210d\365k\366*_\365\"\032)p\007\200\205\001\204\022b\366`\367\200\207G\003\344*`\370\200\205\001\204\022*p\006e\3642\000g\363\001\021`\364b\363\200\210d\362k\363_\362\"\032*\200\205\000\204\032b\363`\364\200\207G\003\347\200\204:p\005e\361`\361\200\215\006`\361*p\006\200\205\001\203\210\006*p\006\200\215\006*p\005e\3602\000g\357\001\024`\360b\357\200\210d\356k\357*_\356\"\032)p\006\200\205\001\204\024b\357`\360\200\207G\003\344*`\361\200\205\001\204\024*,\000\200\205\001\204\017e\355`\355+K\005\006`\355p\001*?\002\034\200\203**p\000*,\0012\000\200\205\004\204\031e\355`\355\200\205\000\204\020*`\355\200\205\001\204\025\001\000\200\204:p\007e\354`\354\200\215\006*p\007e\3532\000g\352\001``\353b\352\200\210d\351k\352_\351\"\0328e\350`\350p\002+K\002*`\350p\0069\002\015`\350`\350p\006\200\205\000\204-w\002\001\000`\350p\002+K\002\014`\350\200\2044p\001\032-w\002\001\000\001\000`\350p\0069\002\027`\354\200\203M`\350p\000+`\350`\350p\006\200\205\004\204r\035\200\212\006\001\000b\352`\353\200\207G\003\260\377\377\377\230*,\002\200\205\001\204\017e\347\001\020`\347p\011`\354\200\205\000\203\222\"\0320\200\205\001\204:\006`\354\200\207;\003\352*\011+\011`\377e\3762\000g\375\001\022`\376b\375\200\210",
    "d\374k\375*_\374\"\0328\200\205\001\204\023b\375`\376\200\207G\003\346\007\0072\000g\376\001/*p\007b\376\200\210\"\0328e\375`\375p\003`\377p\003\200\205\001\203\2042\000>\002\022*p\007b\376`\377\035\200\211`\377b\376y\005\007\001\000k\376b\376*p\007\200\207G\003\310*p\007`\377\200\204:p\000*p\007\200\207\200\205\002\204U\035\200\212\006\007\007`\377e\3762\000g\375\001\022`\376b\375\200\210d\374k\375*_\374\"\032*\200\205\001\204\025b\375`\376\200\207G\003\346\007\0072\000g\376\001@*p\006b\376\200\210\"\032*e\375`\375p\003`\377p\003\200\205\001\203\2042\000>\004\025`\375p\005`\377p\005K\005\012`\375p\001`\377p\001K^\002\014*p\006b\376`\377\035\200\211\007\001\000k\376b\376*p\006\200\207G\003\260\377\377\377\267*p\006`\377\035\200\212\006\007\007**p\000\200\205\001\204\027\007\007*t\012\002\003\007\001\000*5{\012*`\377\200\205\001\204\021\006\200\204:*w\000*p\006e\3762\000g\375\001\021`\376b\375\200\210d\374k\375_\374\"\032*\200\205\000\204\026b\375`\376\200\207G\003\347\007\007`\377*\032-w\002*p\006e\3762\000g\375\001!`\376b\375\200\210d\374k\375_\374\"\032*e\373`\373p\001*K\002\011`\373`\377\200\205\001\204\030\001\000b\375`\376\200\207G\003\327\007\007*\200\203,\200\205\000\203n\032,w\010*\200\203\030\200\205\000\203n\032\030w\012*\200\206\000w\013*\200\206\000w\014*\200\206\000w\015*\200\206\000w\016*4{\017*4{\020*\011+\011*`\377w\000*`\376w\001*`\375w\002*b\374y\004*\200\203.*p\000\200\205\001\204,\032.w\011*\011+\011\200\203\027\200\205\000\203n\032\027e\377`\377*p\002\035\200\205\001\203\241\006`\3772( \200\205\001\203\241\0065g\376*p\015e\3752\000g\374\001,`\375b\374\200\210d\373k\374b\376\002\0054g\376\001\011`\3772, \200\205\001\203\241\006`\377_\373\"\0329p\002\200\205\000\204$\035\200\205\001\203\241\006b\374`\375\200\207G\003\314`\3772) \200\205\001\203\241\006*`\377w\003\007\007*p\005+K\002\006*p\003\011\001\000\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027*p\003\035\200\205\001\203\2412. \200\205\001\203\241\035\200\205\001\203\241*p\005\200\205\000\204$\035\200\205\001\203\241\011+\011*`\377\200\205\001\204\035*p\015`\377\035\200\212\006\007\007`\377*p\016\200\207y\004*p\016`\377\035\200\212\006\007\007*t\017\002\003\007\001\000*5{\017*p\016e\3772\000g\376\001\021`\377b\376\200\210d\375k\376_\375\"\0329\200\205\000\204\020b\376`\377\200\207G\003\347*p\005+K\002\031\200\2043*p\0024\200\205\002\204Ae\374`\3749\002\007*`\374w\005\001\000\001\000*\200\205\000\204\032*p\002,\000\200\205\001\203\2042\000>\005\013*p\002,\001\200\205\001\203\2042\000>g\3735g\372b\373\002\012**p\001p\014w\005\001:*p\005+K\0023\200\204:*\200\205\001\204X\200\203=\200\205\000\203n\032=e\371*p\011`\371\200\205\001\204.*p\005+K\002\016`\371t\000^\002\0054g\372\001\000\001\000\200\204:\200\205\000\204Y\001\000b\372\002D*p\0059\002+b\373\002\024*p\011\200\203]*p\000*p\001\200\205\002\204v\200\205\001\2049\006\001\021*p\011\200\203W*p\000\200\205\001\204,\032W\200\205\001\2049\006\001\021*p\011\200\203[*p\000\200\205\001\204,\032[\200\205\001\2049\006\001\000*p\0059\002I*p\005*p\000\200\205\001\204\027\200\203\027\200\205\000\203n\032\027e\370`\370*p\003\035\200\205\001\203\241\006`\3702. \200\205\001\203\241\006`\370*p\005\200\205\000\204$\035\200\205\001\203\241\006*\200\2046\200\203\027\200\205\000\203n\032\027`\370\035\200\205\001\203\241\200\205\001\204Sy\006\001\013*\200\2046*p\003\200\205\001\204Sy\006\007\007*t\020\002\003\007\001\000*5{\020*p\016e\3772\000g",
    "\376\001\021`\377b\376\200\210d\375k\376_\375\"\0329\200\205\000\204\026b\376`\377\200\207G\003\347*p\0059\002\013*p\005*p\000\200\205\001\204\027\001\000\200\204:*\200\205\001\204X*p\011\200\205\000\204\026\006\200\204:\200\205\000\204Y\007\007`\377*\200\205\001\204G*p\011`\377\200\205\001\204\030`\377\200\205\000\204K\007\007*\011+\011*`\377w\000*`\376w\001*\200\206\000w\002*\200\206\000w\003*\011+\011*\200\216w\000*\011+\011*`\377`\377\200\205\002\204 \007\007*p\000`\377\035`\376\035\200\220\006\007\007*p\000`\377\035\200\205\001\203\225\0144\014*p\000\200\217\0144\014*p\000b\377\200\205\001\203y\"\032\013\011+\011*p\000`\377\035\200\221\"\032\013\011+\011*\011+\011*`\377w\000*b\376{\001*\011+\011*p\000p\001\011+\011*p\000r\002\200\2047r\000X2\000A\0144\014*\200\205\000\204\011^\0144\014*p\000\200\2044p\003K\0144\014*p\000\200\2044p\004K\0144\014*p\000\200\2044p\005K\0144\014*p\000\200\2044p\006K\0144\014*p\000\200\2044p\001K\0144\014*p\000\200\2044p\002K\0144\014*p\000`\377p\000\200\205\001\204\013\0144\014*p\000\200\205\000\204\020\006\007\007*p\000`\377\200\205\001\204\027\007\007*\200\206\000w\001*\011+\011`\377*\200\205\001\204c\200\204:\200\205\000\204Z*e\3762\000g\375\001\022`\376b\375\200\205\001\203\203e\374k\375`\374`\377\200\205\001\204.b\375`\376\200\205\000\203\202G\003\344\200\204:\200\205\000\204[\007\007\200\204:\200\205\000\204Z2\000g\377*p\001\200\207\001\025*p\001b\377*p\001b\377\200\210\"\0320\200\205\000\204\026\035\200\211k\377\021\351\200\204:\200\205\000\204[+\011*e\3762\000g\375\001+`\376b\375\200\205\001\203\203e\374k\375`\374`\377\200\205\001\204\030`\374\200\205\000\2040e\373`\373+K^\002\013`\377\200\204\200\203r\010\200\205\001\204M\001\000b\375`\376\200\205\000\203\202G\003\313\007\007*p\001\200\207\0144\014*p\001b\377\200\210\"\0320\011+\011*p\001`\377\035\200\212\006*\011+\011*p\001`\377\0352\000\200\213\006*\011+\011*\011+\011**p\001\200\205\000\204\026\200\205\000\2044w\001*p\001\200\205\000\2040e\377`\377\200\205\000\204\012\002\027\200\203T*p\000*p\001*p\002*p\003\200\205\004\204u\200\205\000\204\026\011\001\000*p\001\200\205\000\2043\006*p\002\200\205\000\204\026\006*p\0039\002\011*p\003\200\205\000\204\026\006\001\000*\011+\011`\377\200\205\000\204Hg\3762\001g\375\200\203\027\200\205\000\203n\032\027,\000\035\200\205\001\203\241b\376\037\200\205\001\203\241e\374*p\0039\004\007*p\003\200\205\000\203\202;g\3732\000g\372b\373\002\004k\372\001\000*p\001`\377\200\205\001\204\030`\377\200\204\200\203r\002\200\205\001\204Mb\372;\002G`\377\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027,\001\035\200\205\001\203\241b\376\037\200\205\001\203\241\035\200\205\001\203\2412_ \200\205\001\203\241\035\200\205\001\203\241b\375\037\200\205\001\203\241\035\200\205\001\203\241\200\205\001\204I\001\022`\377\200\203\027\200\205\000\203n\032\027`\374\035\200\205\001\203\241\200\205\001\204I*p\002`\377\200\205\001\204\030b\372;\002\035`\377\200\204\200\203r\001\200\205\001\204M`\377\200\203\027\200\205\000\203n\032\027`\374\035\200\205\001\203\241\200\205\001\204I\001\000b\373\002O`\377\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027,\002\035\200\205\001\203\241b\376\037\200\205\001\203\241\035\200\205\001\203\2412_ \200\205\001\203\241\035\200\205\001\203\241b\375\037\200\205\001\203\241\035\200\205\001\203\241\200\205\001\204J*p\003`\377\200\205\001\204\030\001\000`\377\200\203\027\200\205\000\203n\032\027`\374\035\200\205\001\203\241\200\205\001\204J\007\007*`\377w\000*`\376w\001*\200\203.*p\000\200\205\001\204,\032.w\002*\011+\011*\011+\011*`\377w\000*\011+\011+\011+\011`\377*\200\205\001\204c\007\007\007\007+\011+\011+\011+\011*,\000",
    "\200\205\001\203\301\016+\011*\200\205\000\204\026\011+\011*\200\205\000\20409\004\007*\200\205\000\2040\200\205\000\204(\002\004*\011\001\000*,\000\200\205\001\203\301\016+\011*\200\205\000\2040+K\002\011*,\000\200\205\001\203\301\016\001\000*\011+\011*\200\205\000\2040e\376*\200\205\000\2040+K\002\011*,\000\200\205\001\203\301\016\001\000*\200\205\000\2040t\001\004\003b\377^\002\013*p\000,\001\200\205\001\203\301\016\001\000*\011+\011*\200\205\000\2040`\376K\002\004*\011\001\000\200\203_`\377*`\376\200\205\003\204w\011+\011`\377\200\205\000\2040e\375`\376\200\205\000\2040e\374`\375`\374K\002\005`\375\011\001\000`\375\200\205\000\204\011\004\005`\374\200\205\000\204\011\002N`\375\200\205\000\204'\005\005`\374\200\205\000\204'\002\011\200\2044p\002p\014\011\001\000`\375\200\205\000\204(\005\005`\374\200\205\000\204(\002\011*,\000\200\205\001\203\301\016\001\000`\375\200\205\000\204%\005\005`\374\200\205\000\204%\002\011\200\2044p\003p\014\011\001\000\200\2044p\004p\014\011\0017`\375p\000`\374p\000K\002\011`\375p\000p\013\011\001\000`\375`\374\200\205\001\204+\002\005`\374\011\001\000`\374`\375\200\205\001\204+\002\005`\375\011\001\000\200\2044p\001p\013\011+\011*,\000\200\205\001\203\301\016+\011*,\000\200\205\001\203\301\016\007\007*p\000`\377\200\205\001\203\301\011+\011*\011+\011\200\2043*p\0024\200\205\002\204Ae\377`\3779\002\005`\377\011\001&*p\001+K\004\005*p\003+K\002\030\200\204:*p\002\200\205\001\204\\e\376`\3769\002\007`\376p\002\011\001\000\001\000+\011+\011`\377*\200\205\001\204c*p\0019\002\012*p\001`\377\200\205\001\204.\001\000*p\0039\002\012*p\003`\377\200\205\001\204.\001\000\007\007*p\0019\002\027**p\001\200\2043*p\0024\200\205\002\204A\200\205\001\2042\200\205\000\2044w\001\001\000+e\377*`\377\200\205\001\2042\011+\011\200\2043*p\0024\200\205\002\204Ae\376*p\0019\002\200\266**p\001`\376\200\205\001\2042\200\205\000\2044w\001*p\001\200\205\000\2040e\375`\375\200\205\000\204\011\002&`\3769\004\005*p\003+K\002\021*p\001*p\000`\376\200\205\002\2046\200\205\000\204\026\011\001\000*,\000\200\205\001\203\301\016\001q`\375`\377K\002\006*p\001\011\001\000\200\204:*p\000`\375*p\001*p\002*p\003`\3774\200\205\007\204^e\374`\3749\002\005`\374\011\001\000`\375p\000*p\002\200\205\001\204\016e\373`\3739\002\023\200\203L*p\000*p\001`\373\200\205\003\204q\200\205\000\204\026\011\001\000,\001\035\020\200\204:*p\000`\375*p\001*p\002*p\003`\3775\200\205\007\204^e\372+\011\001\200\246\200\204:*p\002\200\205\001\204\\e\371`\3719\002\015\200\203\200\202*p\000`\371\200\205\002\204\206\011\001\000`\3769\002&*p\0039\002\023\200\203N*p\000`\376*p\003\200\205\003\204s\200\205\000\204\026\011\001\013\200\203K*p\000`\376\200\205\002\204p\011\001\000\200\204:*p\000\200\204:p\000p\014+*p\002*p\003`\3774\200\205\007\204^e\370`\3709\002\005`\370\011\001\000\200\204:p\000*p\002\200\205\001\204\016e\367`\3679\002\021\200\203L*p\000+`\367\200\205\003\204q\200\205\000\204\026\011\001\000\200\204:*p\000\200\204:p\000p\014+*p\002*p\003`\3775\200\205\007\204^\006*,\002\200\205\001\203\301\016+\011*p\0019\002L**p\001\200\205\000\204\026\200\205\000\2044w\001*p\001\200\205\000\2040e\376*p\003+K\002(`\376p\000*p\002\200\205\001\204\016e\375`\3759\002\025\200\203M*p\000*p\001`\375`\377\200\205\004\204r\200\205\000\204\026\011\001\000\001\000*,\000\200\205\001\203\301\016\001\000\200\204:*p\002\200\205\001\204\\e\374`\3749\002\022\200\203\200\200*p\000`\374`\377\200\205\003\204\207\200\205\000\204\026\011\001\000\200\204:p\000*p\002\200\205\001\204\016e\373`\3739\002\023\200\203M*p\000+`\373`\377\200\205\004\204r\200\205\000\204\026\011\001\000*,\001\200\205\001\203\301\016",
    "+\011*`\377w\000*`\376w\002*\011+\011*`\377w\000*`\376w\002*`\375w\003*\011+\011*`\377w\000*`\375w\002*`\376w\001*`\374w\003*\011+\011*\200\216w\000*\011+\011*p\000`\377p\001\035`\377\035\200\220\006\007\007`\3772<\200\205\001\203~g\376b\3762\000E\002\027*p\000`\3772\000b\3762\001P\200\205\002\203|\035\200\221\"\032+\011\001\013*p\000`\377\035\200\221\"\032+\011+\011*`\377p\001\200\205\001\204?e\375`\375+K\0029`\376\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027,\000\035\200\205\001\203\241`\377p\001\035\200\205\001\203\241\035\200\205\001\203\241,\001\035\200\205\001\203\241\035\200\205\001\203\241\200\205\001\203\301\016\001\000\200\204(`\375`\377`\376\200\205\003\203\324\007\007*\011+\011\200\2044`\377\200\205\001\204?e\375`\3759\002\022b\376\002\007`\375p\013\011\001\005`\375p\014\011\001\002+\011+\011\200\2044`\377`\376\200\205\002\204Ee\374b\375\002\007`\374p\013\011\001\005`\374p\014\011+\011*\200\216w\000*\011+\011\200\203%,\0002\0012\001\200\205\003\203\300e\377**`\377,\001\200\2047r\001\200\205\003\204Cw\001**`\377,\002\200\2047r\001\200\205\003\204Cw\002**`\377,\003\200\2047r\000\200\2047r\001Y\200\205\003\204Cw\003**`\377,\004\200\2047r\000\200\2047r\001Y\200\205\003\204Cw\004**`\377,\005\200\2047r\000\200\2047r\001Y\200\205\003\204Cw\005**`\377,\006\200\2047r\000\200\2047r\001Y\200\205\003\204Cw\006*`\377,\0072\000\200\205\003\204C\006*`\377,\0102\000\200\205\003\204C\006*`\377,\0112\000\200\205\003\204C\006*`\377,\0122\000\200\205\003\204C\006**`\377,\0132\000\200\205\003\204Cw\013**`\377,\0142\000\200\205\003\204Cw\014**`\377,\0152\000\200\205\003\204Cw\007**`\377,\0162\000\200\205\003\204Cw\010**`\377,\0172\000\200\205\003\204Cw\011**`\377,\0202\000\200\205\003\204Cw\012**`\377,\0212\000\200\205\003\204Cw\002**`\377,\0222\000\200\205\003\204Cw\015**`\377,\0232\000\200\205\003\204Cw\016**`\377,\0242\000\200\205\003\204Cw\017**`\377,\0252\000\200\205\003\204Cw\020**`\377,\0262\000\200\205\003\204Cw\021**`\377,\0272\000\200\205\003\204Cw\022**`\377,\0302\000\200\205\003\204Cw\023**`\377,\0312\000\200\205\003\204Cw\024**p\003w\007**p\004w\010**p\005w\011**p\006w\012\200\203*`\377*p\002,\0322\000\200\205\004\204\031e\376`\376p\012\200\204\200\203r*\200\205\001\203\245\006`\376p\012\200\204\200\203r\013\200\205\001\203\245\006*p\002`\376\200\205\001\204\015*\011+\011*`\377`\376\200\205\002\204De\374`\374b\375y\002`\3745{\010`\374\011+\011*+,\000\200\205\002\204E\0062\000g\377\001\013*b\377\200\205\001\203\203\200\205\000\204\026k\377b\377*p\000\200\217G\003\3542\000g\377*e\3762\000g\375\001\023`\376b\375\200\205\001\203\203e\374k\375`\374b\377y\003k\377b\375`\376\200\205\000\203\202G\003\343\007\007*\200\205\000\204F*e\3762\000g\375\001\022`\376b\375\200\205\001\203\203e\374k\375`\374`\377\200\205\001\204\030b\375`\376\200\205\000\203\202G\003\344\007\007*p\000\200\217\0144\014*p\000b\377\200\205\001\203\224\"\032)\011+\011*`\376\200\205\001\204?e\375`\3759\002\005`\375\011\001\000\200\203)`\377`\3762\000\200\205\003\204\010e\374*p\000`\376\035`\374\035\200\220\006`\374\011+\011*p\000`\377\035\200\221\"\032)e\376`\3769\002\005`\376\011\001\000\200\2042`\377\200\205\001\204?e\375`\3759\002\037\200\203)`\375p\000`\3772\000\200\205\003\204\010e\374*p\000`\377\035`\374\035\200\220\006`\374\011\001\000+\011+\011*`\376",
    "\200\205\001\204?e\375`\375+?\002\005`\375\011\001\000`\3779\002&`\377\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027,\000\035\200\205\001\203\241`\376\035\200\205\001\203\241\035\200\205\001\203\241\200\205\001\203\301\016\001%\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027,\001\035\200\205\001\203\241`\376\035\200\205\001\203\241\035\200\205\001\203\2412. \200\205\001\203\241\016+\0112\000g\377\001C*p\000b\377\200\205\001\203\224\"\032)e\376\200\2046`\376p\001\200\205\001\204\037\006`\376p\007e\3752\000g\374\001\026`\375b\374\200\210d\373k\374\200\2046_\373\"\0328p\003\200\205\001\204\037\006b\374`\375\200\207G\003\342k\377b\377*p\000\200\217G\003\260\377\377\377\264\007\007*\200\206\000w\001*\200\206\000w\005*\200\206\000w\006*\011+\011*\011+\011*`\377w\003*2\000y\004\007\007**r\0042\001Ny\004*r\004\0144\014*p\005\200\203\200\204`\377*p\003p\012\200\205\000\203\202\200\205\002\204\210\035\200\212\006*2\000\200\205\001\204M\007\007*p\006\200\203\200\204`\377*p\003p\012\200\205\000\203\202\200\205\002\204\210\035\200\212\006\007\007\001\015**p\005\200\205\000\203\222\"\032\200\204\200\205\001\204L*p\005\200\207;\003\354*p\006\200\215\006\007\007`\377p\001e\376*p\006e\3752\000g\374\0019`\375b\374\200\210d\373k\374_\373\"\032\200\204p\001`\376\200\205\001\203\2042\000>\002\037*p\003p\012`\377r\000_\373\"\032\200\204r\000`\377r\000P2\001P\200\205\002\203\251\006\007\001\000b\374`\375\200\207G\003\260\377\377\377\277\200\203\027\200\205\000\203n\032\027,\000\035\200\205\001\203\241`\376\035\200\205\001\203\241\016\007\007*p\003p\012b\377\200\205\001\203\245\006\007\0072\000g\376\001\016*p\001*b\376\200\205\001\204P\035\200\212\006k\376b\3762\201\000G\003\353\200\203\030\200\205\000\203n\032\030e\375\200\2044e\3742\000g\373\001B`\374b\373\200\205\001\203\203e\372k\373`\372p\006e\3712\000g\370\001#`\371b\370\200\210d\367k\370_\367\"\032*e\366`\366`\375\200\205\000\203\202y\007`\375`\366p\012\200\205\001\203\252\006b\370`\371\200\207G\003\325b\373`\374\200\205\000\203\202G\003\260\377\377\377\264*\200\203\032\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027`\377\035\200\205\001\203\241,\000\035\200\205\001\203\241\035\200\205\001\203\241\035\200\205\001\203\241\035\200\205\001\203\241\200\205\001\203\254\200\205\000\203\246\032!w\000*p\000,\001\035\200\205\001\203\243\006*p\000\200\205\000\203\242\006*p\000\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027,\002\035\200\205\001\203\241`\377\035\200\205\001\203\241\035\200\205\001\203\241,\003\035\200\205\001\203\241\035\200\205\001\203\243\006*p\0002{ \200\205\001\203\243\006*p\000,\004\035\200\205\001\203\243\006*p\000\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027,\005\035\200\205\001\203\241`\377\035\200\205\001\203\241\035\200\205\001\203\241,\006\035\200\205\001\203\241\035\200\205\001\203\243\006*p\000,\007\035\200\205\001\203\243\006*p\000\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027,\010\035\200\205\001\203\241`\377\035\200\205\001\203\241\035\200\205\001\203\241,\011\035\200\205\001\203\241\035\200\205\001\203\243\006*p\000,\012\035\200\205\001\203\243\0065g\365\200\2046p\001e\3642\000g\363\001@`\364b\363\200\210d\362k\363b\365\002\0174g\365*p\000,\013\035\200\205\001\203\241\006\001\012*p\000,\014\035\200\205\001\203\241\006*p\000\200\203\027\200\205\000\203n\032\027_\362\200\205\001\203\2412\" \200\205\001\203\241\035\200\205\001\203\241\006b\363`\364\200\207G\003\260\377\377\377\270*p\000\200\205\000\203\242\006*p\000,\015\035\200\205\001\203\243\006*p\000,\016\035\200\205\001\203\243\006*p\000,\017\035\200\205\001\203\243\006*p\000\200\205\000\203\242\006*p\000,\020\035\200\205\001\203\243\006*p\000,\021\035\200\205\001\203\243\006\200\2044e\3612\000g\360\001\200\202`\361b\360\200\205\001\203\203e\357k\360*p\000\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027,",
    "\022\035\200\205\001\203\241`\357p\001\035\200\205\001\203\241\035\200\205\001\203\241,\023\035\200\205\001\203\241\035\200\205\001\203\241`\357r\002\037\200\205\001\203\241\035\200\205\001\203\241,\024\035\200\205\001\203\241\035\200\205\001\203\241`\357p\007\200\207\037\200\205\001\203\241\035\200\205\001\203\241,\025\035\200\205\001\203\241\035\200\205\001\203\243\006b\360`\361\200\205\000\203\202G\003\260\377\377\377t*p\000\200\205\000\203\242\006\200\2044e\3562\000g\355\001\201B`\356b\355\200\205\001\203\203e\354k\355`\354p\005e\3532\000g\352\001b`\353b\352\200\210d\351k\352_\351\"\032)e\350*p\000\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027,\026\035\200\205\001\203\241`\354p\001\035\200\205\001\203\241\035\200\205\001\203\241,\027\035\200\205\001\203\241\035\200\205\001\203\241`\350p\001\035\200\205\001\203\241\035\200\205\001\203\241,\030\035\200\205\001\203\241\035\200\205\001\203\243\006b\352`\353\200\207G\003\260\377\377\377\226`\354p\007e\3472\000g\346\001\200\255`\347b\346\200\210d\345k\346_\345\"\0328e\344*p\000\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027,\031\035\200\205\001\203\241`\354p\001\035\200\205\001\203\241\035\200\205\001\203\241,\032\035\200\205\001\203\241\035\200\205\001\203\241`\344r\005\037\200\205\001\203\241\035\200\205\001\203\241,\033\035\200\205\001\203\241\035\200\205\001\203\241`\344p\003\035\200\205\001\203\241\035\200\205\001\203\241,\034\035\200\205\001\203\241\035\200\205\001\203\241`\344p\002\200\205\000\204$\035\200\205\001\203\241\035\200\205\001\203\241,\035\035\200\205\001\203\241\035\200\205\001\203\243\006b\346`\347\200\207G\003\260\377\377\377Kb\355`\356\200\205\000\203\202G\003\260\377\377\376\264*p\000,\036\035\200\205\001\203\243\006*p\000\200\205\000\203\242\006*p\000,\037\035\200\205\001\203\243\006*p\000, \035\200\205\001\203\243\006*p\000,!\035\200\205\001\203\243\006\200\2044e\3432\000g\342\001\203\250`\343b\342\200\205\001\203\203e\341k\342`\341p\006e\3402\000g\337\001\203\211`\340b\337\200\210d\336k\337_\336\"\032*e\335`\335p\001`\341K\002\203p*p\000\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027,\"\035\200\205\001\203\241`\335p\001p\001\035\200\205\001\203\241\035\200\205\001\203\241,#\035\200\205\001\203\241\035\200\205\001\203\241`\335p\002\035\200\205\001\203\241\035\200\205\001\203\241,$\035\200\205\001\203\241\035\200\205\001\203\241`\335r\004\037\200\205\001\203\241\035\200\205\001\203\241,%\035\200\205\001\203\241\035\200\205\001\203\241`\335p\015\200\207\037\200\205\001\203\241\035\200\205\001\203\241,&\035\200\205\001\203\241\035\200\205\001\203\241`\335p\016\200\207\037\200\205\001\203\241\035\200\205\001\203\241,'\035\200\205\001\203\241\035\200\205\001\203\241\006`\335p\0059\0023*p\000\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\0272\" \200\205\001\203\241`\335p\005\200\205\000\204$\035\200\205\001\203\241\035\200\205\001\203\241,(\035\200\205\001\203\241\035\200\205\001\203\241\006\001\012*p\000,)\035\200\205\001\203\241\006*p\000\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027,*\035\200\205\001\203\241`\335r\007\037\200\205\001\203\241\035\200\205\001\203\241,+\035\200\205\001\203\241\035\200\205\001\203\241\006*p\000`\335p\013\200\207\037\200\205\001\203\241\006*p\000,,\035\200\205\001\203\243\0062\000g\334`\335p\016e\3332\000g\332\001\201\014`\333b\332\200\210d\331k\332_\331\"\0329e\330b\334`\335p\015\200\207G\002y*p\000\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027,-\035\200\205\001\203\241b\334\037\200\205\001\203\241\035\200\205\001\203\241,.\035\200\205\001\203\241\035\200\205\001\203\241`\330p\001\035\200\205\001\203\241\035\200\205\001\203\241,/\035\200\205\001\203\241\035\200\205\001\203\241`\330p\002\200\205\000\204$\035\200\205\001\203\241\035\200\205\001\203\241,0\035\200\205\001\203\241",
    "\035\200\205\001\203\243\006\001w*p\000\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027,1\035\200\205\001\203\241b\334\037\200\205\001\203\241\035\200\205\001\203\241,2\035\200\205\001\203\241\035\200\205\001\203\241`\330p\001\035\200\205\001\203\241\035\200\205\001\203\241,3\035\200\205\001\203\241\035\200\205\001\203\241`\330p\002\200\205\000\204$\035\200\205\001\203\241\035\200\205\001\203\241,4\035\200\205\001\203\241\035\200\205\001\203\243\006k\334b\332`\333\200\207G\003\260\377\377\376\3542\000g\334`\335p\013e\3272\000g\326\001O`\327b\326\200\210d\325k\326*p\000\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027,5\035\200\205\001\203\241b\334\037\200\205\001\203\241\035\200\205\001\203\241,6\035\200\205\001\203\241\035\200\205\001\203\241\006*p\000_\325\"\200\205\000\203o\035\200\205\001\203\241\006*p\000,7\035\200\205\001\203\243\006k\334b\326`\327\200\207G\003\260\377\377\377\251`\335p\014\200\207;\002\200\220*p\000,8\035\200\205\001\203\241\0065g\324`\335p\014e\3232\000g\322\001c`\323b\322\200\210d\321k\322b\324\002\0054g\324\001\012*p\000,9\035\200\205\001\203\241\006*p\000,:\035\200\205\001\203\241\006\200\204\022_\321#\200\205\001\203\226g\320\200\204\022_\321#\200\205\001\203\227g\317b\320;\002\022*b\3202\001\200\205\002\204O*b\3172\010\200\205\002\204O\001\010*b\3172\001\200\205\002\204O*p\0002L \200\205\001\203\241\006b\322`\323\200\207G\003\260\377\377\377\225*p\000,;\035\200\205\001\203\243\006\001\000\001\000b\337`\340\200\207G\003\260\377\377\374ob\342`\343\200\205\000\203\202G\003\260\377\377\374N*p\000,<\035\200\205\001\203\243\006*p\000\200\205\000\203\242\006*p\000\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027,=\035\200\205\001\203\241`\377\035\200\205\001\203\241\035\200\205\001\203\241,>\035\200\205\001\203\241\035\200\205\001\203\243\006*p\000,?\035\200\205\001\203\243\006*p\000,@\035\200\205\001\203\243\006*p\000,A\035\200\205\001\203\243\006*p\000\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027,B\035\200\205\001\203\241`\375\200\205\000\203\202\037\200\205\001\203\241\035\200\205\001\203\241,C\035\200\205\001\203\241\035\200\205\001\203\243\006*p\000,D\035\200\205\001\203\243\006*p\000,E\035\200\205\001\203\243\006*p\000,F\035\200\205\001\203\243\006*p\000,G\035\200\205\001\203\243\006*p\000,H\035\200\205\001\203\243\006*p\000\200\205\000\203\242\006*p\000,I\035\200\205\001\203\243\006*p\000,J\035\200\205\001\203\243\0062\000g\3165g\315\001Mb\315\002\0054g\315\001\012*p\0002, \200\205\001\203\243\006*p\000,K\035\200\205\001\203\241\0062\000g\314\001\017*`\375b\316\200\205\001\203\203\200\205\001\204Qk\316k\314b\3142\204\000G\004\010b\316`\375\200\205\000\203\202G\003\340*p\0002\" \200\205\001\203\241\006b\316`\375\200\205\000\203\202G\003\260\377\377\377\251*p\000\200\205\000\203\242\006*p\000,L\035\200\205\001\203\243\006*p\0002} \200\205\001\203\243\006*p\000\200\205\000\203\244\007\007b\3772\000>\004\005b\3762\000C\002\003\007\001\000*b\3772\004\\b\3762\001P\200\205\002\204O*p\000,\000b\3772\017X\200\205\001\203\203 \200\205\001\203\241\006\007\007b\3772\011>\002\005,\000\011\001\000b\3772\012>\002\005,\001\011\001\000b\3772\015>\002\005,\002\011\001\000b\3772\\>\002\005,\003\011\001\000b\3772\">\002\005,\004\011\001\000b\3772 E\004\005b\3772~C\002\026\200\203\027\200\205\000\203n\032\027,\005\035\200\205\001\203\241b\377 \200\205\001\203\241\011\001\000\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\0272\\ \200\205\001\203\241b\3772\006\\\037\200\205\001\203\241\035\200\205\001\203\241,\006\035\200\205\001\203\241\035\200\205\001\203\241b\3772\003\\2\007X\037\200\205\001\203\241\035\200\205\001\203\241,\007\035\200\205\001\203\241\035\200\205\001\203\241b\3772\007X\037\200\205\001\203\241\011+\011b\3772@7E\004\005b\3772\177C\002\010*b\377",
    "\200\205\001\204R\001\200\250b\3772\200\200E\004\005b\3772\217\377C\002\024*2\200\200b\3772\010\\Y\200\205\001\204R*b\377\200\205\001\204R\001\200\206b\3772\220\020\000E\004\005b\3772\237\377\377C\002\035*2\200\220b\3772\020\\Y\200\205\001\204R*b\3772\010\\\200\205\001\204R*b\377\200\205\001\204R\001[b\3772\240\020\000\000E\004\005b\3772\257\377\377\377C\002&*2\200\240b\3772\030\\Y\200\205\001\204R*b\3772\020\\\200\205\001\204R*b\3772\010\\\200\205\001\204R*b\377\200\205\001\204R\001'*2\200\260\200\205\001\204R*b\3772\030\\\200\205\001\204R*b\3772\020\\\200\205\001\204R*b\3772\010\\\200\205\001\204R*b\377\200\205\001\204R\007\007*p\000*p\001b\3772\200\377X\200\210\"\200\205\000\203o\035\200\205\001\203\241\006\007\007*\200\216w\000*\200\206\000w\001*\011+\011*p\001\011+\011*p\000`\377\035\200\205\001\203\225\002\013*p\000`\377\035\200\221$\014\001\000*p\000\200\217g\376*p\000`\377\035b\376\037\200\220\006*p\001`\377\035\200\212\006b\376\0144\014*`\377\200\205\001\204\037\0144\014*2\001y\000*2\002y\001*2\004y\002*2\010y\003*\011+\011*\011+\011*`\377w\000*`\376w\001*`\375w\003*\011+\011*p\001\200\205\000\204\020\006*p\002\200\205\000\204\020\007\007*p\001\200\205\000\204\026*p\002*p\000\200\205\001\204\027\007\007\200\2038*p\000`\377*p\003\200\205\003\204Te\375`\375*p\002w\002`\375*r\004y\004`\375b\376y\005`\375*p\006w\006`\375\011+\011*\011+\011\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027*p\001\035\200\205\001\203\2412: \200\205\001\203\241\035\200\205\001\203\241\200\204-\200\205\000\204$\035\200\205\001\203\241\011+\011*`\377w\000*`\376w\001*\011+\011*`\377w\000*`\376w\001*`\375w\002*\011+\011*p\002\200\205\000\204\020\007\007*p\002*p\000\200\205\001\204\027\007\007*r\0042\001N7\0144\014*\200\206\000w\002*\200\206\000w\003*\200\206\000w\004*\200\206\000w\005*\200\206\000w\006*\200\206\000w\007*\200\203;\200\205\000\203nw\010*\200\206\000w\011*\011+\011*p\011*p\001\035\200\212\006*p\011*p\002\035\200\212\006*p\011*p\003\035\200\212\006*`\377w\001*\200\206\000w\002*\200\206\000w\003*p\001p\015e\3762\000g\375\001\020`\376b\375\200\210d\374k\375*p\002_\374\200\212\006b\375`\376\200\207G\003\350\007\007**p\011\200\205\000\203\222\"\032\017w\003**p\011\200\205\000\203\222\"\032\017w\002**p\011\200\205\000\203\222\"\032*w\001\007\007*p\003*p\002\200\207\037\200\212\006\007\007*p\003\200\2072\000>\002\021+e\377`\377,\000\035\200\205\001\203t\006,\001\016\001\000*p\003\200\205\000\203\222$g\376\001\007*p\002\200\205\000\203\222\006*p\002\200\207b\376I\003\360\007\007*p\002\200\2072\001Pg\376\001 *p\002b\376\200\210\"\0329e\375`\375p\001`\377\200\205\001\203\2042\000>\002\005`\375\011\001\000n\376b\3762\000E\003\331+\011+\011`\377+K\002\003\007\001\000*p\001p\005+K\002\011*p\001`\377w\005\001/`\377*p\001p\005\200\205\001\204+^\002\"*p\001p\005`\377\200\205\001\204+\002\011*p\001`\377w\005\001\013*p\001\200\2044p\001p\013w\005\001\000\007\007*`\377`\376`\374`\373`\372b\371\200\205\006\204_e\370`\370+K\002\004+\011\001\000`\3739\002\013`\373`\370p\015\200\205\001\204b\001\000\200\203O`\377`\375`\370`\373\200\205\004\204t\200\205\000\204\026\011+\011`\374e\371`\374+K\002\007*p\010e\374\001\000*p\005\200\215\006*p\006\200\215\006`\376p\000p\006e\3702\000g\367\0017`\370b\367\200\210d\366k\367_\366\"\032*e\365`\365p\002`\375\200\205\001\203\2042\000>\002\031`\365p\015\200\207`\374\200\205\000\203\202>\002\012*",
    "p\005`\365\035\200\212\006\001\000\001\000b\367`\370\200\207G\003\301`\374\200\205\000\204\026\006`\3739\002g*p\005e\3642\000g\363\001:`\364b\363\200\210d\362k\363_\362\"\032*e\361`\361p\0059\004\011`\373`\361p\005\200\205\001\204+\002\030`\373`\361p\005K\002\005`\361\011\001\000*p\006`\361\035\200\212\006\001\000b\363`\364\200\207G\003\260\377\377\377\276*p\006\200\207;\002\021*p\005\200\215\006*p\005*p\006\200\205\001\203\210\006\001\000\001\000*p\005\200\2072\001I\002s5g\360+e\357*p\005\200\205\000\203\217\"\032*p\003e\356*p\005e\3552\000g\354\001=`\355b\354\200\210d\353k\354_\353\"\032*e\352`\352p\003`\356\200\205\001\203\2042\000>\002\034`\352p\0059\004\013`\352p\005p\000\200\2044p\001<\002\006`\352e\357\001\000\001\0034g\360b\354`\355\200\207G\003\260\377\377\377\273b\360\004\003`\3579\002\005`\357\011\001\000\001\000*p\005\200\2072\001>\002\014*p\005\200\205\000\203\217\"\032*\011\001\000b\372^\002\004+\011\001\000\200\203\027\200\205\000\203n\032\027e\351*p\005\200\207;\002\013`\351,\000\035\200\205\001\203\241\006\001&`\351,\001\035\200\205\001\203\241\006`\3719\002\013`\351,\002\035\200\205\001\203\241\006\001\011`\351,\003\035\200\205\001\203\241\006`\371e\374`\351`\375\035\200\205\001\203\241\006`\3749\002W`\3512( \200\205\001\203\241\0065g\350`\374e\3472\000g\346\001,`\347b\346\200\205\001\203\203e\345k\346b\350\002\0054g\350\001\011`\3512, \200\205\001\203\241\006`\351`\345\200\205\000\2040\200\205\000\204$\035\200\205\001\203\241\006b\346`\347\200\205\000\203\202G\003\312`\3512) \200\205\001\203\241\006\001\000`\351,\004\035\200\205\001\203\241\006`\351`\376\200\205\000\204$\035\200\205\001\203\241\006`\3512. \200\205\001\203\241\006`\377\200\203\027\200\205\000\203n\032\027`\351\035\200\205\001\203\241\200\205\001\203\301\016+\011*\200\206\000w\000*\011+\011*\011+\011*p\000`\377\035\200\212\006*\011+\011*p\000`\377\035\200\212\006*p\000`\376\035\200\212\006*\011+\011`\377*\0320\200\205\001\204c*e\3762\000g\375\001\022`\376b\375\200\205\001\203\203e\374k\375`\374`\377\200\205\001\204.b\375`\376\200\205\000\203\202G\003\344\007\007*p\000\200\207\0144\014*p\000b\377\200\210\"\0320\011+\011*p\000`\377\035\200\212\006*\011+\0112\000g\377*p\000\200\207\001\030*p\000b\377*p\000b\377\200\210\"\0320\200\205\000\204\026\200\205\000\2044\035\200\211k\377\021\346+\0112\000g\376*e\3752\000g\374\001'`\375b\374\200\205\001\203\203e\373k\374*p\000b\376`\373`\373p\000`\377b\376\200\210\"\0329p\002\200\205\002\2046\035\200\211k\376b\374`\375\200\205\000\203\202G\003\317\007\007*e\3762\000g\375\001\022`\376b\375\200\205\001\203\203e\374k\375`\374`\377\200\205\001\204\030b\375`\376\200\205\000\203\202G\003\344\007\007*\011+\011\007\007*4{\000*\011+\011`\377\034>\002\013*5{\000`\377\200\205\000\204/\001\000\007\007*\011+\011\200\203\027\200\205\000\203n\032\027,\000\035\200\205\001\203\241*p\001\200\205\000\203o\035\200\205\001\203\241\011+\011\200\204:*p\001\200\205\000\204-\200\205\001\204]\007\007**p\001\200\205\000\204\026\200\205\000\2044w\001\200\204:p\001p\005e\377`\377+K\002\011*,\000\200\205\001\203\301\016\001\000*p\001`\377t\001\200\205\001\2045\006**p\001*p\000\200\204:p\001p\005\200\205\002\2046\200\205\000\204\026w\001*\011+\011*p\001`\377\200\205\001\204\030*p\001\200\205\000\2040e\376`\376\200\205\000\204%\002\013`\377\200\204\200\203r\014\200\205\001\204M\001)`\376\200\205\000\204&\005\005`\376\200\205\000\204'\005\005`\376\200\205\000\204(\002\013`\377\200\204\200\203r\015\200\205\001\204M\001\011`\377\200\204\200\203r\013\200\205\001\204M\007\007*\011+\011*\200\205\000\2040\011+\011*\011+\011*\011+\011\200\203\027\200\205\000\203n\032\027*q\001\036\200\205\001",
    "\203\241\011+\011\200\2044p\003p\014\011+\011`\376\200\205\000\204%\002\004*\011\001\000`\376\200\205\000\204&\002\023*q\001(g\375\200\203A*p\000b\375\200\205\002\204e\011\001X`\376\200\205\000\204'\002\011*,\000\200\205\001\203\301\016\001H`\376\200\205\000\204(\002\011*,\001\200\205\001\203\301\016\0018`\376\200\205\000\204*\002*\200\203D*p\000\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027,\002\035\200\205\001\203\241*q\001\036\200\205\001\203\241\035\200\205\001\203\241\200\205\002\204\036\011\001\007*,\003\200\205\001\203\301\016+\011`\377\200\204\200\203r.\200\205\001\204M`\377`\377p\003p\014\200\207\200\205\001\204M`\377p\003p\014*q\001\036\200\212\006\007\007*`\377w\000*a\376x\001*\011+\011*\011+\011\200\203\027\200\205\000\203n\032\027*r\001\037\200\205\001\203\241\011+\011\200\2044p\004p\014\011+\011`\376\200\205\000\204&\002\004*\011\001\000`\376\200\205\000\204%\002\017\200\203@*p\000*r\001'\200\205\002\204d\011\001\200\265`\376\200\205\000\204'\002\016\200\203B*p\000*r\001\200\205\002\204f\011\001\200\240`\376\200\205\000\204(\002\011*,\000\200\205\001\203\301\016\001\200\220`\376\200\205\000\204*\002*\200\203D*p\000\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027,\001\035\200\205\001\203\241*r\001\037\200\205\001\203\241\035\200\205\001\203\241\200\205\002\204\036\011\001_`\376p\000\200\2044p\010K\005\005`\376\200\205\000\204)\002\026\200\2031*p\000,\002\200\203;*\200\205\001\204`\200\205\003\204<\200\205\000\204\026\011\0017*\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027,\003\035\200\205\001\203\241`\376\200\205\000\204$\035\200\205\001\203\241\035\200\205\001\203\2412. \200\205\001\203\241\035\200\205\001\203\241\200\205\001\203\301\016+\011`\377\200\204\200\203r2\200\205\001\204M`\377*r\001\200\205\001\204M\007\007*`\377w\000*b\376y\001*\011+\011*\011+\011\200\203\027\200\205\000\203n\032\027*s\001 \200\205\001\203\241\011+\011\200\2044p\005p\014\011+\011`\376\200\205\000\204'\002\004*\011\001\000`\376\200\205\000\204%\002\011*,\000\200\205\001\203\301\016\001]`\376\200\205\000\204&\002\016\200\203A*p\000*s\001\200\205\002\204e\011\001H`\376\200\205\000\204(\002\011*,\001\200\205\001\203\301\016\0018`\376\200\205\000\204*\002*\200\203D*p\000\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027,\002\035\200\205\001\203\241*s\001 \200\205\001\203\241\035\200\205\001\203\241\200\205\002\204\036\011\001\007*,\003\200\205\001\203\301\016+\011`\377\200\204\200\203r2\200\205\001\204M`\377*s\001\200\205\001\204M\007\007*`\377w\000*b\376z\001*\011+\011*\011+\011*t\001\002\005,\000\011\001\003,\001\011+\011\200\2044p\006p\014\011+\011`\376\200\205\000\204(\002\004*\011\001\000`\376\200\205\000\204%\002\011*,\000\200\205\001\203\301\016\001M`\376\200\205\000\204&\002\011*,\001\200\205\001\203\301\016\001=`\376\200\205\000\204'\002\011*,\002\200\205\001\203\301\016\001-`\376\200\205\000\204*\002\037*t\001\002\015\200\203D*p\000,\003\200\205\002\204\036\011\001\013\200\203D*p\000,\004\200\205\002\204\036\011\001\007*,\005\200\205\001\203\301\016+\011`\377\200\204\200\203r2\200\205\001\204M*t\001\002\011`\3772\001\200\205\001\204M\001\007`\3772\000\200\205\001\204M\007\007*`\377w\000*b\376{\001*\011+\011*\011+\011\200\203\027\200\205\000\203n\032\0272\" \200\205\001\203\241*p\001\035\200\205\001\203\2412\" \200\205\001\203\241\011+\011\200\2044p\002p\014\011+\011`\377\200\204\200\203r,\200\205\001\204M`\377`\377p\003p\013\200\207\200\205\001\204M`\377p\003p\013*p\001\035\200\212\006\007\007*`\377w\000*`\376w\001*\011+\011*\011+\011,\000\011+\011*`\377w\000*\200\2044p\001p\013w\001*\011+\011*p\001\011+\011*p\001*p\000\200\205\001\204\027*\011+\011*\200\205\000\2040e\376*\200\205\000\2040+K\002\011*,\000\200\205\001\203\301\016\001\000b\377^\002\013*p\000,\001\200\205\001\203\301\016\001\000*\011+\011`\377\200\204\200\203r+\200\205\001\204M\007\007",
    "*2\0017y\002*\011+\011`\377*\200\205\001\204c*p\001`\377\200\205\001\204.\007\007*p\001\200\205\000\2040\011+\011**p\001\200\205\000\204\026\200\205\000\2044w\001*p\001\200\205\000\2040e\377*p\001\034?\002r`\377\200\205\000\204%\002\031**p\001\032@q\001\200\205\001\204he\376`\3769\002\005`\376\011\001\000\001P`\377\200\205\000\204&\002\031**p\001\032Ar\001\200\205\001\204ie\375`\3759\002\005`\375\011\001\000\0010`\377\200\205\000\204(\002\031**p\001\032Ct\001\200\205\001\204je\374`\3749\002\005`\374\011\001\000\001\020*\200\205\000\204ke\373`\3739\002\005`\373\011\001\000\001\000`\377\200\205\000\204%\002\022*\200\205\000\204le\372`\3729\002\005`\372\011\001\000\001B`\377\200\205\000\204&\002\022*\200\205\000\204me\371`\3719\002\005`\371\011\001\000\001)`\377\200\205\000\204(\002\022*\200\205\000\204ne\370`\3709\002\005`\370\011\001\000\001\020*\200\205\000\204oe\367`\3679\002\005`\367\011\001\000*\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027,\000\035\200\205\001\203\241`\377\200\205\000\204$\035\200\205\001\203\241\035\200\205\001\203\241,\001\035\200\205\001\203\241\035\200\205\001\203\241*\200\205\000\204$\035\200\205\001\203\241\035\200\205\001\203\241,\002\035\200\205\001\203\241\035\200\205\001\203\241\200\205\001\203\301\016+\011*p\001`\377\200\205\001\204\030`\377*r\002\200\205\001\204M\007\007*`\377w\000*`\376w\001*\011+\011,\000\011+\011+\011+\011+\011+\011+\011+\011+\011+\011+\011+\011+\011+\011+\011+\011+\011+\011*2\0017y\002*\011+\011\200\2044p\006p\014\011+\011,\000\011+\011+\011+\011+\011+\011\200\203C*p\000b\377^\200\205\002\204g\011+\011+\011+\011+\011+\011*\200\204\200\203r^y\002*\011+\011*2\0017y\002*\011+\011*p\001\200\205\000\204-\011+\011\200\203\027\200\205\000\203n\032\027,\000\035\200\205\001\203\2412- \200\205\001\203\241\011+\011\200\203@*p\000a\3776\200\205\002\204d\011+\011\200\203A*p\000b\3777\200\205\002\204e\011+\011\200\203C*p\000b\377^\200\205\002\204g\011+\011*\200\204\200\203r6y\002*\011+\011*\200\204\200\203r7y\002*\011+\011*\200\204\200\203r^y\002*\011+\011*2\0017y\002*\011+\011*p\001\200\205\000\204-\011+\011\200\203\027\200\205\000\203n\032\027,\000\035\200\205\001\203\2412! \200\205\001\203\241\011+\011+\011+\011\200\203C*p\000b\377^\200\205\002\204g\011+\011*\200\204\200\203r\200\231y\002*\011+\011*\200\204\200\203r^y\002*\011+\011*2\0017y\002*\011+\011\200\2044p\006p\014\011+\011\200\2044p\006p\014\011+\011\200\203\027\200\205\000\203n\032\027,\000\035\200\205\001\203\2412? \200\205\001\203\241\011+\011\200\203C*p\000a\377:\200\205\002\204g\011+\011\200\203C*p\000b\377;\200\205\002\204g\011+\011*\200\204\200\203r:y\002*\011+\011*\200\204\200\203r;y\002*\011+\011*p\001\011+\011*p\001\200\205\000\2040e\377`\377\200\205\000\204\012\002\036`\377t\001^\002\014\200\203C*p\0005\200\205\002\204g\011\001\000*\200\204\200\203r9y\002*\011\001\002+\011+\011*\011+\011*p\001\011+\011*p\001\011+\011*p\001*p\000\200\205\001\204\027*\011+\011`\377\200\204\200\203r\200\202\200\205\001\204M`\377*p\001p\000r\003\200\205\001\204M\007\007*`\377w\000*`\376w\001*\011+\011*\011+\011`\377*\200\205\001\204c*p\0019\002\012*p\001`\377\200\205\001\204.\001\000\007\007*p\002p\002\011+\011*p\002p\003\011+\011*p\0019\002 **p\001\200\205\000\204\026w\001*p\001\200\205\000\2040t\001\002\013*p\000,\000\200\205\001\203\301\016\001\000\001\000*\011+\011*p\0019\002\012*p\001`\377\200\205\001\204\030\001\011`\377\200\204\200\203r*\200\205\001\204M*",
    "p\002p\002e\3762\000g\375`\376\200\205\000\204%\002\010\200\204\200\203rpg\375\0013`\376\200\205\000\204&\002\010\200\204\200\203rqg\375\001$`\376\200\205\000\204'\002\010\200\204\200\203rrg\375\001\025`\376\200\205\000\204(\002\010\200\204\200\203rsg\375\001\006\200\204\200\203rog\375`\377b\375\200\205\001\204M`\377*p\002r\005\200\205\001\204M\007\007*`\377w\000*`\376w\001*`\375w\002*\011+\011*\011+\011`\377*\200\205\001\204c*p\0019\002\012*p\001`\377\200\205\001\204.\001\000*p\0039\002\012*p\003`\377\200\205\001\204.\001\000\007\007+\011+\011**p\003\200\205\000\204\026\200\205\000\2044w\003*p\003\200\205\000\2040t\001\004\010*p\002p\002t\001^\002;*p\000\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\0272' \200\205\001\203\241*p\002p\003\035\200\205\001\203\241\035\200\205\001\203\241,\000\035\200\205\001\203\241\035\200\205\001\203\241\200\205\001\203\301\016\001\000*p\0019\002\013**p\001\200\205\000\204\026w\001\001\000**p\003\200\205\000\204\026*p\000*p\002p\002\200\205\002\2046\200\205\000\204\026w\003*\011+\011*p\0019\002\012*p\001`\377\200\205\001\204\030\001\011`\377\200\204\200\203r*\200\205\001\204M*p\003`\377\200\205\001\204\030*p\002p\002e\3762\000g\375`\376\200\205\000\204%\002\010\200\204\200\203rvg\375\0013`\376\200\205\000\204&\002\010\200\204\200\203rwg\375\001$`\376\200\205\000\204'\002\010\200\204\200\203rxg\375\001\025`\376\200\205\000\204(\002\010\200\204\200\203ryg\375\001\006\200\204\200\203rug\375`\377b\375\200\205\001\204M`\377*p\002r\005\200\205\001\204M\007\007*`\377w\000*`\376w\001*`\375w\002*`\374w\003*\011+\011*\011+\011`\377*\200\205\001\204c*p\0039\002\012*p\003`\377\200\205\001\204.\001\000\007\007*p\001\011+\011*p\001*p\000\200\205\001\204\027*p\0039\002\011*p\003\200\205\000\204\026\006\001\000*\200\204:*p\000*p\001,\000*p\003+4\200\205\006\204_w\002*p\0029\004\004*p\0039\002\015*p\003*p\002p\015\200\205\001\204b\001\000*\011+\011`\377\200\204\200\203r\200\201\200\205\001\204M`\377*p\001p\000r\003\200\205\001\204M*p\0039\002)*p\003e\3762\000g\375\001\022`\376b\375\200\205\001\203\203e\374k\375`\374`\377\200\205\001\204\030b\375`\376\200\205\000\203\202G\003\344\001\000*p\0029\002/`\377\200\204\200\203r\200\203\200\205\001\204M*p\0039\002\015`\377*p\003\200\205\000\203\202\200\205\001\204M\001\007`\3772\000\200\205\001\204M`\377*p\002r\006\200\205\001\204M\001\000\007\007*`\377w\000*`\376w\001*`\375w\003*\011+\011*\011+\011`\377*\200\205\001\204c*p\0019\002\012*p\001`\377\200\205\001\204.\001\000*p\0039\002\012*p\003`\377\200\205\001\204.\001\000\007\007*p\002p\005\011+\011*p\002p\002\011+\011*p\0019\002'**p\001\200\205\000\204\026\200\205\000\2044w\001*p\001\200\205\000\2040e\377`\377t\001\002\013*p\000,\000\200\205\001\203\301\016\001\000\001\000*p\003\200\205\000\204\026\006*\011+\011*p\0019\002\012*p\001`\377\200\205\001\204\030\001\011`\377\200\204\200\203r*\200\205\001\204M*p\003e\3762\000g\375\001\022`\376b\375\200\205\001\203\203e\374k\375`\374`\377\200\205\001\204\030b\375`\376\200\205\000\203\202G\003\344`\377\200\204\200\203r\200\203\200\205\001\204M`\377*p\003\200\205\000\203\202\200\205\001\204M`\377*p\002r\006\200\205\001\204M\007\007*`\377w\000*`\376w\001*`\375w\002*`\374w\003*p\003+K\002\011*\200\204:p\010w\003\001\000*\011+\011*2\0017y\002*\011+\011*\011+\011+\011+\011*\011+\011*\011+\011*\011+\011*\011+\011\200\204:\200\205\000\204Z\200\204:p\002*p\004\035\200\212\006*p\002\200\205\000\204\026\006\200\204:\200\205\000\204[*p\0039\002\011*p\003\200\205\000",
    "\204\026\006\001\000*\011+\011\200\203\027\200\205\000\203n\032\027`\377\200\205\000\204H\037\200\205\001\203\241,\000\035\200\205\001\203\241e\376*p\001`\377\200\205\001\204\030`\377\200\204\200\203r\006\200\205\001\204M*p\0039\002%`\377\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027,\001\035\200\205\001\203\241`\376\035\200\205\001\203\241\035\200\205\001\203\241\200\205\001\204I\001#`\377\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027,\002\035\200\205\001\203\241`\376\035\200\205\001\203\241\035\200\205\001\203\241\200\205\001\204I`\377\200\204\200\203re\200\205\001\204M`\377*p\004\200\205\000\204W\200\205\001\204M*p\002`\377\200\205\001\204\030*p\0039\002Y`\377\200\204\200\203r\001\200\205\001\204M`\377\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027,\003\035\200\205\001\203\241`\376\035\200\205\001\203\241\035\200\205\001\203\241\200\205\001\204I`\377\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027,\004\035\200\205\001\203\241`\376\035\200\205\001\203\241\035\200\205\001\203\241\200\205\001\204J*p\003`\377\200\205\001\204\030\001\000`\377\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027,\005\035\200\205\001\203\241`\376\035\200\205\001\203\241\035\200\205\001\203\241\200\205\001\204J\007\007*`\377w\000*`\376w\001*`\375w\002*`\374w\003*p\001\200\205\000\2041e\373`\373+K\002\013*p\000,\000\200\205\001\203\301\016\001\000*\200\2039*p\000`\373*p\001\200\205\000\2040p\000p\014\200\205\003\204Vw\004*\011+\011*\011+\011`\377*\200\205\001\204c*p\001`\377\200\205\001\204.\007\007+\011+\011**p\001\200\205\000\204\026\200\205\000\2044w\001*\011+\011*`\377w\000*`\376w\001*\011+\011*\011+\011\200\203\027\200\205\000\203n\032\027,\000\035\200\205\001\203\241*p\001\200\205\000\203o\035\200\205\001\203\241\011+\011**p\001\200\205\000\204\026\200\205\000\2044w\001*\011+\011*p\001`\377\200\205\001\204\030*p\001\200\205\000\2040e\3762\000g\375`\376p\000\200\2044p\002\200\205\001\204\013\002\010\200\204\200\203r\200\223g\375\001B`\376\200\205\000\204%\002\010\200\204\200\203r\200\224g\375\0013`\376\200\205\000\204&\002\010\200\204\200\203r\200\225g\375\001$`\376\200\205\000\204'\002\010\200\204\200\203r\200\226g\375\001\025`\376\200\205\000\204(\002\010\200\204\200\203r\200\227g\375\001\006\200\204\200\203r\200\222g\375`\377b\375\200\205\001\204M`\377\200\204\200\203r\020\200\205\001\204M\007\007*\011+\011`\377\200\204\200\203r\200\230\200\205\001\204M\007\007*\011+\011`\377\200\204\200\203r\200\220\200\205\001\204M\007\007*\011+\011`\377\200\204\200\203r\200\221\200\205\001\204M\007\007*\011+\011*\011+\011,\000\011+\011\200\204:p\001p\002,\000\200\205\001\203\2042\000>\002\024\200\203]*p\000\200\204:p\001p\001\200\205\002\204v\200\205\000\204\026\011\001\000*\011+\011`\377\200\204\200\203r\011\200\205\001\204M\007\007*\011+\011,\000\011+\011\200\204:p\001p\005e\377`\377+K\002\011*,\000\200\205\001\203\301\016\001\000`\377t\001^\002\013*p\000,\001\200\205\001\203\301\016\001\000*\011+\011`\377\200\204\200\203r+\200\205\001\204M`\377\200\204\200\203r\013\200\205\001\204M\007\007*\011+\011,\000\011+\011*p\001*p\000\200\205\001\204\027\200\204:p\001p\005+K\002\011*,\000\200\205\001\203\301\016\001\000*\011+\011`\377\200\204\200\203r*\200\205\001\204M`\377\200\204\200\203r\013\200\205\001\204M\007\007*`\377w\000*`\376p\014w\001*p\001*p\000\200\205\001\204\027*\011+\011*2\0017y\002*\011+\011*2\0017y\002*\011+\011*p\003\011+\011*p\003\011+\011*p\003*p\000\200\205\001\204\027**p\001\200\205\000\204\026\200\205\000\2044w\001*p\001\200\205\000\2040e\377`\377*p\003K\002\006*p\001\011\001\000`\377p\000*p\003p\000K\002#*p\003t\001\002\023\200\203`*p\000*p\001*p\003\200\205\003\204w\032`\011\001\000*,\000\200\205\001\203\301\016\001\000`\377\200\205\000\204\012\004\006*p\003\200\205\000\204\012\0020`\377*p\003\200\205\001\204+\002\023\200\203`*p\000*p\001*p\003\200\205\003\204w\032`\011\001\021\200\203a*p\000",
    "*p\001*p\003\200\205\003\204w\032a\011\001\000`\377\200\205\000\204\011\004\006*p\003\200\205\000\204\012\002.*p\003p\000\200\2044p\010K\005\006*p\003\200\205\000\204)\002\030\200\2031*p\000,\001\200\203;*p\001\200\205\001\204`\200\205\003\204<\200\205\000\204\026\011\001\000\001\000`\377\200\205\000\204\012\004\006*p\003\200\205\000\204\011\002\036*p\003\200\205\000\204&\002\024\200\2031*p\000*p\001,\002+\200\205\004\204=\200\205\000\204\026\011\001\000\001\000*p\003\200\205\000\204%\002\034`\377\200\205\000\204&\002\023\200\203b*p\000*p\001\200\205\002\204;\032b\200\205\000\204\026\011\001\000\001$*p\003\200\205\000\204&\002\034`\377\200\205\000\204%\002\023\200\203c*p\000*p\001\200\205\002\204;\032c\200\205\000\204\026\011\001\000\001\000*\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027,\003\035\200\205\001\203\241`\377\200\205\000\204$\035\200\205\001\203\241\035\200\205\001\203\241,\004\035\200\205\001\203\241\035\200\205\001\203\241*p\003\200\205\000\204$\035\200\205\001\203\241\035\200\205\001\203\241\200\205\001\203\301\016+\011*`\377w\000*`\376w\001*`\375w\003*\011+\011*2\0017y\002*\011+\011**p\001\200\205\000\204\026\200\205\000\2044w\001*\011+\011*p\001`\377\200\205\001\204\030\007\007*2\0017y\002*\011+\011**p\001\200\205\000\204\026\200\205\000\2044w\001*\011+\011*p\001`\377\200\205\001\204\030`\377\200\204\200\203r\033\200\205\001\204M`\377*p\003p\000r\003\200\205\001\204M\007\007*2\0017y\002*\011+\011\200\2044p\003p\014\011+\011**p\001\200\205\000\204\026\200\205\000\2044w\001*\011+\011*p\001`\377\200\205\001\204\030`\377\200\204\200\203r'\200\205\001\204M\007\007*2\0017y\002*\011+\011\200\2044p\004p\014\011+\011**p\001\200\205\000\204\026\200\205\000\2044w\001*\011+\011*p\001`\377\200\205\001\204\030`\377\200\204\200\203r(\200\205\001\204M\007\007*\011+\011`\377*\200\205\001\204c*p\001`\377\200\205\001\204.\007\007*p\002*p\000\200\205\001\204\027*\011+\011*`\377w\000*`\376w\001*`\375w\002*\011+\011*\011+\011\200\2044p\006p\014\011+\011\200\2044p\006p\014\011+\011**p\001\200\205\000\204\026\200\205\000\2044w\001*p\001\200\205\000\2040e\377`\377*p\002\200\205\001\204+\002\014\200\203C*p\0005\200\205\002\204g\011\001\000`\377\200\205\000\204\011\002\014\200\203C*p\0004\200\205\002\204g\011\001\000*p\002\200\205\000\204\011\002\014\200\203C*p\0004\200\205\002\204g\011\001\000*\011+\011*p\001`\377\200\205\001\204\030`\377\200\204\200\203r\035\200\205\001\204M`\377*p\002p\000r\003\200\205\001\204M\007\007*2\0017y\003*\011+\011*p\001\200\205\000\204-e\377*p\002\200\205\000\204-e\376`\377+K\005\004`\376+K\002\004+\011\001\000**p\001*p\002\200\205\002\2047\011+\011`\377*\200\205\001\204c*p\001`\377\200\205\001\204.*p\002`\377\200\205\001\204.\007\007*p\001\200\205\000\2040\011+\011**\200\205\000\204|\200\205\001\204y\011+\011*p\001`\377\200\205\001\204\030*p\002`\377\200\205\001\204\030`\377*r\003\200\205\001\204M\007\007*`\377w\000*`\376w\001*`\375w\002*\011+\011,\000\011+\011*\200\205\000\204|e\376*p\001\034?\004\005*p\002\034?\002\200\211`\376\200\205\000\204%\002 **p\001\032@q\001*p\002\032@q\001\200\205\002\204}e\375`\3759\002\005`\375\011\001\000\001``\376\200\205\000\204&\002 **p\001\032Ar\001*p\002\032Ar\001\200\205\002\204~e\374`\3749\002\005`\374\011\001\000\0019`\376\200\205\000\204(\002 **p\001\032Ct\001*p\002\032Ct\001\200\205\002\204\177e\373`\3739\002\005`\373\011\001\000\001\022*`\376\200\205\001\204\200e\372`\3729\002\005`\372\011\001\000\001\000`\376\200\205\000\204%\002\022*\200\205\000\204\201e\371`\3719\002\005`\371\011\001\000\001D`\376\200\205\000\204&\002\022*\200\205\000\204\202e\370",
    "`\3709\002\005`\370\011\001\000\001+`\376\200\205\000\204(\002\022*\200\205\000\204\203e\367`\3679\002\005`\367\011\001\000\001\022*`\376\200\205\001\204\204e\366`\3669\002\005`\366\011\001\000*\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027,\000\035\200\205\001\203\241`\376\200\205\000\204$\035\200\205\001\203\241\035\200\205\001\203\241,\001\035\200\205\001\203\241\035\200\205\001\203\241*\200\205\000\204$\035\200\205\001\203\241\035\200\205\001\203\241,\002\035\200\205\001\203\241\035\200\205\001\203\241\200\205\001\203\301\016+\011**p\001\200\205\000\204\026\200\205\000\2044w\001**p\002\200\205\000\204\026\200\205\000\2044w\002*p\001\200\205\000\2040e\377*p\002\200\205\000\2040e\376**p\001*p\002\200\205\002\2047\011+\011**p\001*p\001p\000`\377\200\205\002\2046\200\205\000\204\026w\001**p\002*p\002p\000`\377\200\205\002\2046\200\205\000\204\026w\002\007\007*\200\205\000\204ze\377*`\377\200\205\001\204{`\377\011+\011+\011+\011+\011+\011+\011+\011+\011+\011+\011+\011+\011+\011+\011+\011+\011+\011*2\0017y\003*\011+\011\200\203\027\200\205\000\203n\032\027,\000\035\200\205\001\203\2412+ \200\205\001\203\241\011+\011\200\203@*p\000a\377a\376M\200\205\002\204d\011+\011\200\203A*p\000b\377b\376N\200\205\002\204e\011+\011\200\203C*p\000b\377\005\002b\376\200\205\002\204g\011+\011`\377\200\205\000\204'\0023\200\203D*p\000\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027*p\001\032Bs\001 \200\205\001\203\241*p\002\032Bs\001 \200\205\001\203\241\035\200\205\001\203\241\200\205\002\204\036\011\001<`\377\200\205\000\204*\0023\200\203D*p\000\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027*p\001\032Dp\001\035\200\205\001\203\241*p\002\032Dp\001\035\200\205\001\203\241\035\200\205\001\203\241\200\205\002\204\036\011\001\002+\011+\011*\200\204\200\203rMy\003*\011+\011*\200\204\200\203rNy\003*\011+\011*\200\204\200\203rYy\003*\011+\011*2\0017y\003*\011+\011\200\203\027\200\205\000\203n\032\027,\000\035\200\205\001\203\2412- \200\205\001\203\241\011+\011\200\203@*p\000a\377a\376O\200\205\002\204d\011+\011\200\203A*p\000b\377b\376P\200\205\002\204e\011+\011*\200\204\200\203rOy\003*\011+\011*\200\204\200\203rPy\003*\011+\011*2\0017y\003*\011+\011\200\203\027\200\205\000\203n\032\027,\000\035\200\205\001\203\2412* \200\205\001\203\241\011+\011\200\203@*p\000a\377a\376Q\200\205\002\204d\011+\011\200\203A*p\000b\377b\376R\200\205\002\204e\011+\011\200\203C*p\000b\377\004\002b\376\200\205\002\204g\011+\011*\200\204\200\203rQy\003*\011+\011*\200\204\200\203rRy\003*\011+\011*\200\204\200\203rXy\003*\011+\011*2\0017y\003*\011+\011*p\001\200\205\000\204-e\377*p\002\200\205\000\204-e\376`\377+K\005\004`\376+K\002\004+\011\001\000**p\001*p\002\200\205\002\2047e\375`\375\200\205\000\204&\002\011\200\2044p\003p\014\011\001\000`\375\011+\011\200\203\027\200\205\000\203n\032\027,\000\035\200\205\001\203\2412/ \200\205\001\203\241\011+\011\200\203@*p\000a\377a\376S\200\205\002\204d\011+\011\200\203@*p\000b\377'b\376'S\200\205\002\204d\011+\011*\200\204\200\203rSy\003*\011+\011**p\001*p\000\200\2044p\003p\014\200\205\002\2046w\001**p\002*p\000\200\2044p\003p\014\200\205\002\2046w\002*\200\205\000\204\026\011+\011*2\0017y\003*\011+\011\200\203\027\200\205\000\203n\032\027,\000\035\200\205\001\203\2412% \200\205\001\203\241\011+\011\200\203@*p\000a\377a\376T\200\205\002\204d\011+\011\200\203A*p\000b\377b\376U\200\205\002\204e\011+\011*\200\204\200\203rTy\003*\011+\011*\200\204\200\203rUy\003*\011+\011*2\0017y\003*\011+\011\200\203\027\200\205\000\203n\032\027,\000\035\200\205\001\203\2412^ \200\205\001\203\241",
    "\011+\011\200\203@*p\000a\377a\376V\200\205\002\204d\011+\011\200\203A*p\000b\377b\376W\200\205\002\204e\011+\011*\200\204\200\203rVy\003*\011+\011*\200\204\200\203rWy\003*\011+\011*2\0017y\003*\011+\011\200\203\027\200\205\000\203n\032\027,\000\035\200\205\001\203\2412& \200\205\001\203\241\011+\011\200\203A*p\000b\377b\376X\200\205\002\204e\011+\011\200\203C*p\000b\377\004\002b\376\200\205\002\204g\011+\011*\200\204\200\203rXy\003*\011+\011*\200\204\200\203rXy\003*\011+\011*2\0017y\003*\011+\011\200\203\027\200\205\000\203n\032\027,\000\035\200\205\001\203\2412| \200\205\001\203\241\011+\011\200\203A*p\000b\377b\376Y\200\205\002\204e\011+\011\200\203C*p\000b\377\005\002b\376\200\205\002\204g\011+\011*\200\204\200\203rYy\003*\011+\011*\200\204\200\203rYy\003*\011+\011*2\0017y\003*\011+\011\200\203\027\200\205\000\203n\032\027,\000\035\200\205\001\203\2412~ \200\205\001\203\241\011+\011\200\203A*p\000b\377b\376Z\200\205\002\204e\011+\011\200\203C*p\000b\377\004\003b\376^\005\007b\377^\004\002b\376\200\205\002\204g\011+\011*\200\204\200\203rZy\003*\011+\011*\200\204\200\203rZy\003*\011+\011*2\0017y\003*\011+\011,\000\011+\011\200\203A*p\000b\377b\376[\200\205\002\204e\011+\011*\200\204\200\203r[y\003*\011+\011*2\0017y\003*\011+\011,\000\011+\011\200\203A*p\000b\377b\376\\\200\205\002\204e\011+\011*\200\204\200\203r\\y\003*\011+\011*2\0017y\003*\011+\011,\000\011+\011\200\203A*p\000b\377b\376]\200\205\002\204e\011+\011*\200\204\200\203r]y\003*\011+\011*2\0017y\003*\011+\011\200\203\027\200\205\000\203n\032\027`\377\200\205\000\204H\037\200\205\001\203\241,\000\035\200\205\001\203\241e\376*p\001`\377\200\205\001\204\030`\377\200\204\200\203r\004\200\205\001\204M`\377\200\203\027\200\205\000\203n\032\027`\376\035\200\205\001\203\241\200\205\001\204I*p\002`\377\200\205\001\204\030`\377\200\203\027\200\205\000\203n\032\027`\376\035\200\205\001\203\241\200\205\001\204J\007\007,\000\011+\011\200\203C*p\000b\377\004\002b\376\200\205\002\204g\011+\011*\011+\011*2\0017y\003*\011+\011*\200\205\000\204ze\377`\377\200\205\000\204\012\002Z*p\001\200\205\000\2040e\376*p\002\200\205\000\2040e\375`\376\200\205\000\204\012\004\005`\375\200\205\000\204\012\002:`\376t\001^\002\006*p\001\011\001\000`\375t\001^\004\004`\377t\001\002\012`\377p\000p\014e\377\001\000\200\203u*p\000*p\001*p\002`\377\200\205\004\204\205\200\205\000\204\026\011\001\000\001\000**\200\205\000\204|\200\205\001\204y\011+\011\200\203\027\200\205\000\203n\032\027`\377\200\205\000\204H\037\200\205\001\203\241,\000\035\200\205\001\203\241e\376*p\001`\377\200\205\001\204\030`\377\200\204\200\203r\005\200\205\001\204M`\377\200\203\027\200\205\000\203n\032\027`\376\035\200\205\001\203\241\200\205\001\204I*p\002`\377\200\205\001\204\030`\377\200\203\027\200\205\000\203n\032\027`\376\035\200\205\001\203\241\200\205\001\204J\007\007,\000\011+\011\200\203C*p\000b\377\005\002b\376\200\205\002\204g\011+\011*\011+\011*2\0017y\003*\011+\011*p\004\011+\011*p\001\034E\002\006*p\002\011\001\000*p\002\034E\002\006*p\001\011\001\000*\011+\011\200\203\027\200\205\000\203n\032\027`\377\200\205\000\204H\037\200\205\001\203\241,\000\035\200\205\001\203\241e\376*p\001`\377\200\205\001\204\030`\377\200\204\200\203r\007\200\205\001\204M`\377\200\203\027\200\205\000\203n\032\027`\376\035\200\205\001\203\241\200\205\001\204I*p\002`\377\200\205\001\204\030`\377\200\203\027\200\205\000\203n\032\027`\376\035\200\205\001\203\241\200\205\001\204J\007\007,\000\011+\011*`\377w\000*`\376w\001*`\375w\002*`\374w\004*\011+\011*2\0017y\003*\011+\011,\000\011+\011\200\203C*p\000b\377\004\003b\376^\005\007b\377^\004\002b\376\200\205\002\204g\011+\011*\200\204\200\203rZy",
    "\003*\011+\011*2\0017y\003*\011+\011\200\2044p\006p\014\011+\011\200\2044p\006p\014\011+\011*2\0017y\003*\011+\011,\000\011+\011\200\203C*p\000a\377a\376=\200\205\002\204g\011+\011\200\203C*p\000b\377b\376>\200\205\002\204g\011+\011\200\203C*p\000b\377b\376>\200\205\002\204g\011+\011`\377\200\205\000\204'\002\032\200\203C*p\000*p\001\032Bs\001*p\002\032Bs\001>\200\205\002\204g\011\001(`\377\200\205\000\204*\002\037\200\203C*p\000*p\001\032Dp\001*p\002\032Dp\001\200\205\001\203\2042\000>\200\205\002\204g\011\001\002+\011+\011*\200\204\200\203r=y\003*\011+\011*\200\204\200\203r>y\003*\011+\011*\200\204\200\203r>y\003*\011+\011`\377\200\205\000\204'\002\011*\200\204\200\203r>y\003\001\007*\200\204\200\203r<y\003*\011+\011*2\0017y\003*\011+\011,\000\011+\011\200\203C*p\000a\377a\376@\200\205\002\204g\011+\011\200\203C*p\000b\377b\376A\200\205\002\204g\011+\011\200\203C*p\000b\377b\376A\200\205\002\204g\011+\011`\377\200\205\000\204'\002\032\200\203C*p\000*p\001\032Bs\001*p\002\032Bs\001A\200\205\002\204g\011\001(`\377\200\205\000\204*\002\037\200\203C*p\000*p\001\032Dp\001*p\002\032Dp\001\200\205\001\203\2042\000A\200\205\002\204g\011\001\002+\011+\011*\200\204\200\203r@y\003*\011+\011*\200\204\200\203rAy\003*\011+\011*\200\204\200\203rAy\003*\011+\011`\377\200\205\000\204'\002\011*\200\204\200\203rAy\003\001\007*\200\204\200\203r?y\003*\011+\011*2\0017y\003*\011+\011\200\203\027\200\205\000\203n\032\027,\000\035\200\205\001\203\2412< \200\205\001\203\241\011+\011\200\203C*p\000a\377a\376F\200\205\002\204g\011+\011\200\203C*p\000b\377b\376G\200\205\002\204g\011+\011`\377\200\205\000\204'\002\032\200\203C*p\000*p\001\032Bs\001*p\002\032Bs\001G\200\205\002\204g\011\001\000+\011+\011*\200\204\200\203rFy\003*\011+\011*\200\204\200\203rGy\003*\011+\011`\377\200\205\000\204'\002\013*\200\204\200\203rGy\003*\011\001\000+\011+\011*2\0017y\003*\011+\011\200\203\027\200\205\000\203n\032\027,\000\035\200\205\001\203\2412> \200\205\001\203\241\011+\011\200\203C*p\000a\377a\376H\200\205\002\204g\011+\011\200\203C*p\000b\377b\376I\200\205\002\204g\011+\011`\377\200\205\000\204'\002\032\200\203C*p\000*p\001\032Bs\001*p\002\032Bs\001I\200\205\002\204g\011\001\000+\011+\011*\200\204\200\203rHy\003*\011+\011*\200\204\200\203rIy\003*\011+\011`\377\200\205\000\204'\002\013*\200\204\200\203rIy\003*\011\001\000+\011+\011*2\0017y\003*\011+\011,\000\011+\011\200\203C*p\000a\377a\376B\200\205\002\204g\011+\011\200\203C*p\000b\377b\376C\200\205\002\204g\011+\011`\377\200\205\000\204'\002\032\200\203C*p\000*p\001\032Bs\001*p\002\032Bs\001C\200\205\002\204g\011\001\000+\011+\011*\200\204\200\203rBy\003*\011+\011*\200\204\200\203rCy\003*\011+\011`\377\200\205\000\204'\002\013*\200\204\200\203rCy\003*\011\001\000+\011+\011*2\0017y\003*\011+\011,\000\011+\011\200\203C*p\000a\377a\376D\200\205\002\204g\011+\011\200\203C*p\000b\377b\376E\200\205\002\204g\011+\011`\377\200\205\000\204'\002\032\200\203C*p\000*p\001\032Bs\001*p\002\032Bs\001E\200\205\002\204g\011\001\000+\011+\011*\200\204\200\203rDy\003*\011+\011*\200\204\200\203rEy\003*\011+\011`\377\200\205\000\204'\002\013*\200\204\200\203rEy\003*\011\001\000+\011+\011*\011+\011\200\204:p\002*p\001\035\200\212\006`\377*\200\205\001\204c\007\007\200\204:p\002*p\001\035\200\212\006*\011+\011\007\007*`\377w\000*`\376w\001*",
    "\011+\011*\011+\011`\377*\200\205\001\204c*p\001`\377\200\205\001\204.*p\002`\377\200\205\001\204.\007\007*p\001*p\002\200\205\001\2048\011+\011*`\377w\000*`\376w\001*`\375w\002*\011+\011*\011+\011`\377*\200\205\001\204c*p\002`\377\200\205\001\204.\007\007**p\002\200\205\000\204\026\200\205\000\2044w\002*p\002\200\205\000\2040e\377*p\001p\002+K\002\011*p\001`\377w\002\001\000`\377t\001\004\010*p\001p\002t\001^\002;*p\000\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\027\200\203\027\200\205\000\203n\032\0272' \200\205\001\203\241*p\001p\001\035\200\205\001\203\241\035\200\205\001\203\241,\000\035\200\205\001\203\241\035\200\205\001\203\241\200\205\001\203\301\016\001\000**p\002*p\000*p\001p\002\200\205\002\2046\200\205\000\204\026w\002*\011+\011*p\002`\377\200\205\001\204\030*p\001p\002e\376`\376\200\205\000\204%\002\013`\377\200\204\200\203rf\200\205\001\204M\001\033`\376\200\205\000\204\011\002\013`\377\200\204\200\203rg\200\205\001\204M\001\011`\377\200\204\200\203re\200\205\001\204M`\377*p\001\200\205\000\204W\200\205\001\204M\007\007*`\377w\000*`\376w\001*`\375w\002*\011+\011*\011+\011*\011+\011*p\001p\002\200\205\000\204%\002\024`\377\200\204\200\203r0\200\205\001\204M`\377\200\204\200\203rf\200\205\001\204M\0010*p\001p\002\200\205\000\204\011\002\024`\377\200\204\200\203r4\200\205\001\204M`\377\200\204\200\203rg\200\205\001\204M\001\022`\377\200\204\200\203r+\200\205\001\204M`\377\200\204\200\203re\200\205\001\204M`\377*p\001\200\205\000\204W\200\205\001\204M\007\007*`\377w\000*`\376w\001*p\001p\002+K\002\015*p\001\200\2044p\001p\013w\002\001\000*\011+\011*\011+\011*p\001p\002\011+\011*p\001p\001\011+\011*\011+\011*p\001p\002e\376`\376\200\205\000\204%\002\013`\377\200\204\200\203ra\200\205\001\204M\001\033`\376\200\205\000\204\011\002\013`\377\200\204\200\203rb\200\205\001\204M\001\011`\377\200\204\200\203r`\200\205\001\204M`\377*p\001\200\205\000\204W\200\205\001\204M\007\007*`\377w\000*`\376w\001*\011+\011*2\000y\000*2\001y\001*2\002y\002*2\003y\003*2\004y\004*2\005y\005*2\006y\006*2\007y\007*2\010y\010*2\011y\011*2\012y\012*2\013y\013*2\014y\014*2\015y\015*2\016y\016*2\017y\017*2\020y\020*2\021y\021*2\022y\022*2\023y\023*2\024y\024*2\025y\025*2\026y\026*2\027y\027*2\030y\030*2\031y\031*2\032y\032*2\033y\033*2\034y\034*2\035y\035*2\036y\036*2\037y\037*2 y *2!y!*2\"y\"*2#y#*2$y$*2%y%*2&y&*2(y'*2)y(*2*y)*2+y**2,y+*2-y,*2.y-*2/y.*20y/*21y0*22y1*23y2*24y3*25y4*26y5*27y6*28y7*29y8*2:y9*2;y:*2<y;*2=y<*2>y=*2?y>*2@y?*2Ay@*2ByA*2CyB*2DyC*2EyD*2FyE*2GyF*2HyG*2IyH*2JyI*2KyJ*2LyK*2MyL*2NyM*2OyN*2PyO*2QyP*2RyQ*2SyR*2TyS*2UyT*2VyU*2WyV*2XyW*2YyX*2ZyY*2[yZ*2\\y[*2]y\\*2^y]*2_y^",
    "*2`y_*2ay`*2bya*2cyb*2dyc*2eyd*2fye*2gyf*2hyg*2iyh*2jyi*2kyj*2lyk*2myl*2nym*2oyn*2pyo*2qyp*2ryq*2syr*2tys*2uyt*2vyu*2wyv*2xyw*2yyx*2zyy*2{yz*2|y{*2}y|*2~y}*2\177y~*2\200\200y\177*2\200\201y\200\200*2\200\202y\200\201*2\200\203y\200\202*2\200\204y\200\203*2\200\205y\200\204*2\200\206y\200\205*2\200\207y\200\206*2\200\210y\200\207*2\200\211y\200\210*2\200\212y\200\211*2\200\213y\200\212*2\200\214y\200\213*2\200\215y\200\214*2\200\216y\200\215*2\200\217y\200\216*2\200\220y\200\217*2\200\221y\200\220*2\200\222y\200\221*2\200\223y\200\222*2\200\224y\200\223*2\200\225y\200\224*2\200\226y\200\225*2\200\227y\200\226*2\200\230y\200\227*2\200\231y\200\230*2\200\232y\200\231*2\201^y\200\232*\011+\011*\011+\011*`\377w\001*b\376y\000*\011+\011"
  };
}
