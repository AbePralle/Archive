file = require("file")
require("./bard-string-augment")
TokenizerModule = require("./bard-tokenizer")
ProgramModule  = require("./bard-program")
ParseError = TokenizerModule.ParseError
Token      = TokenizerModule.Token
Tokenizer  = TokenizerModule.Tokenizer
TokenType  = TokenizerModule.TokenType
Attributes = ProgramModule.Attributes
Local      = ProgramModule.Local
Method     = ProgramModule.Method
Program    = ProgramModule.Program
Template   = ProgramModule.Template
Type       = ProgramModule.Type

Cmd = require( "./bard-cmd" )

#==============================================================================
# TokenReader
#==============================================================================
class TokenReader
  constructor: (@tokens) ->
    @position = 0
    @count = @tokens.length

  has_another: () ->
    return (@position < @count)

  peek: ( num_ahead=0 ) ->
    i = @position + num_ahead
    if (i < @count) then return @tokens[i]
    return null

  read: () ->
    return @tokens[ @position++ ]

  read_identifier: () ->
    t = @read()
    if (t.type != TokenType.IDENTIFIER)
      throw t.error( "Identifier expected." )
    return t.value


#==============================================================================
# Preprocessor
#==============================================================================
Preprocessor =
  process: (tokens) ->
    new_tokens = []

    previous_eol = null
    last_is_eol = false
    in_comment = false

    # Insert comment text as values of EOL tokens to support
    # auto-documentation.
    for t in tokens
      switch t.type
        when TokenType.EOL
          if (last_is_eol)
            # Two EOL's breaks comment appending.
            in_comment = false

          last_is_eol = true
          if (not in_comment)
            previous_eol = t
            new_tokens.push( t )

        when TokenType.COMMENT
          last_is_eol = false
          if (previous_eol)
            in_comment = true
            if (previous_eol.value)
              previous_eol.value += "\n" + t.value
            else
              previous_eol.value = t.value

          else if (t.line == 1)
            # If there is no previous EOL *and* the line number of the 
            # comment is 1, turn the comment into an EOL.  Otherwise we just
            # discard it.
            last_is_eol = true
            in_comment = true
            t.type = TokenType.EOL
            new_tokens.push( t )
            previous_eol = t

        else
          last_is_eol = false
          in_comment = false
          previous_eol = null
          new_tokens.push( t )


    # Merge literal strings that are either consecutive or separated by
    # exactly one EOL.
    tokens = new_tokens
    new_tokens = []

    for t in tokens
      if (t.type == TokenType.LITERAL_STRING)
        count = new_tokens.length
        if (count > 0)
          if (new_tokens[count-1].type == TokenType.LITERAL_STRING)
            new_tokens[count-1].value += t.value
            continue

          if (count > 1 and
              new_tokens[count-2].type == TokenType.LITERAL_STRING and
              new_tokens[count-1].type == TokenType.EOL)
            new_tokens[count-2].value += t.value
            continue

      new_tokens.push( t )

    #console.log new_tokens

    return new_tokens


#==============================================================================
# Parser
#==============================================================================
class Parser
  constructor: (filepath,data) ->
    if (not Program.types) then Program.reset()

    @included_library_names = []

    @reader_stack = []
    @processed_filepaths = {}

    @create_reader(filepath,data)

  create_reader: (filepath,data) ->
    reader = null
    if (filepath)
      if (!filepath.length or filepath[0] instanceof Token)
        # 'filepath' is actually a token list
        tokens = filepath
        reader = new TokenReader(tokens)

      else
        # Create the reader if we haven't processed this filepath already
        filepath = file.path.abspath( filepath )

        if (@processed_filepaths[filepath]) then return false # already processed
        @processed_filepaths[filepath] = filepath

        tokens = Tokenizer.tokenize( filepath, data )
        tokens = Preprocessor.process( tokens )
        reader = new TokenReader(tokens)

    if (not reader) then return false  # file already processed
    @push_reader( reader )
    return true

  parse: (filepath,data) ->
    if (not @create_reader(filepath,data) ) then return null

    while (@parse_element()) then
    
    @pop_reader()
    return Program

  push_reader: (reader) ->
    if (@reader) then @reader_stack.push( @reader )
    @reader = reader
    return reader

  pop_reader: () ->
    if (@reader_stack.length) then @reader = @reader_stack.pop()
    else                           @reader = null

  consume: (token_type) ->
    if (typeof(token_type) == "undefined")
      throw new ParseError( "[INTERNAL ERROR] Reference to undefined token type." )

    t = @peek()
    if (not t or t.type != token_type) then return false
    @read()
    return true

  consume_end_commands: () ->
    found = false
    while (@consume(TokenType.EOL) or @consume(TokenType.SYMBOL_SEMICOLON))
      found = true
    return found

  consume_eols: () ->
    found_any = false
    while (@consume(TokenType.EOL))
      found_any = true
    return found_any

  must_consume: (token_type,message) ->
    t = @peek()
    if (@consume(token_type)) then return t

    if (not message)
      message = "'" + TokenType.name_lookup[ token_type ] + "' expected but found \"#{t}\"."

    if (not @peek())
      throw new ParseError( "End of file; " + message )

    throw @peek().error( "Syntax error: " + message )

  must_consume_eol: () ->
    @must_consume( TokenType.EOL )

  next_is: (type) ->
    if (not @reader.has_another()) then return false
    return (@peek().type == type)

  next_is_end_command: () ->
    if (not @reader.has_another()) then return false
    type = @peek().type
    return (type == TokenType.EOL or type == TokenType.SYMBOL_SEMICOLON)

  next_is_statement: () ->
    if (not @reader.has_another()) then return false
    return (not TokenType.is_structural[@peek().type])

  parse_element: () ->
    @consume_end_commands()
    t = @peek()

    if (@parse_library_command()) then return true

    if (@parse_template_def()) then return true

    if (not @reader.has_another()) then return false

    if (@next_is_statement())
      @parse_multi_line_statements( Program.statements )
    else
      @parse_expression()  # Will generate an error

    return true

  parse_library_command: () ->
    if (not @consume(TokenType.KEYWORD_library)) then return false

    if (@consume(TokenType.SYMBOL_PERIOD))
      # Library setting
      setting_name = @read_identifier()
      t = @read()
      if (not t.value)
        throw t.error "Value expected (string, number, etc.)."

      settings = Program.this_library.settings[setting_name] or []
      settings.push( t.value )
      Program.this_library.settings[setting_name] = settings
      #console.log Program.this_library.settings

      @must_consume_eol()
      return true

    library_id = ""
    first = true
    while (first or @consume(TokenType.SYMBOL_PERIOD))
      if (first) then first = false
      else            library_id += "."
      library_id += @read_identifier()

    @must_consume_eol()

    Program.set_library_context( library_id )
    if (@included_library_names.indexOf(library_id) == -1)
      @included_library_names.push( library_id )


    return true

  parse_template_def: () ->
    t = @peek()

    attributes = 0
    closing_type = 0
    if (@consume(TokenType.KEYWORD_class))
      attributes = Attributes.flag_class
      closing_type = TokenType.KEYWORD_endClass

    else
      return false

    template = new Template( t, @read_identifier(), attributes )

    if (@consume(TokenType.SYMBOL_OPEN_SPECIALIZE))
      template.placeholder_info = []
      first = true
      while (first or @consume(TokenType.SYMBOL_COMMA))
        first = false
        t = @must_consume( TokenType.SYMBOL_DOLLAR, "'$Placeholder' expected." )
        name = @read_identifier()
        template.placeholder_info.push( {t:t,name:name} )

      @must_consume( TokenType.SYMBOL_CLOSE_SPECIALIZE, "'$Placeholder' or '>>' expected." )
      #console.log template.placeholder_info

    else if (not Program.main_class_info and template.is_class())
      Program.main_class_info = {t:t,name:template.name}

    tokens = []
    template.tokens = tokens
    Program.template_list.push( template )
    Program.templates[template.name] = template

    if (@consume(TokenType.SYMBOL_SEMICOLON)) then return true
    @must_consume_eol()

    while (@reader.has_another())
      t = @reader.read()
      if (t.type == closing_type)
        return true

      tokens.push( t )

    throw t.error( "Type definition is missing closing '" + TokenType.name_lookup[closing_type] + "'." )

  parse_type_def: (@this_type) ->
    while (@parse_category()) then

    @this_type = null

    if (@reader.has_another())
      t = @peek()
      throw t.error "Syntax error: unexpected \"#{t}\"."

  parse_category: () ->
    @consume_end_commands()

    if (@consume(TokenType.KEYWORD_PROPERTIES))
      while (@parse_property_list()) then
      return true

    if (@consume(TokenType.KEYWORD_METHODS))
      while (@parse_method()) then
      return true

    return false

  parse_property_list: () ->
    @consume_end_commands()
    if (not @next_is(TokenType.IDENTIFIER)) then return false

    cur_list = []
    first = true
    while (first or @consume(TokenType.SYMBOL_COMMA))
      first = false

      t = @peek()
      name = @read_identifier()

      initial_value = undefined
      if (@consume(TokenType.SYMBOL_EQUALS))
        initial_value = @parse_expression()

      p = {t:t, name:name, initial_value:initial_value}
      @this_type.property_list.push( p )
      cur_list.push( p )

    if (@consume(TokenType.SYMBOL_COLON))
      type = @parse_type()
      for p in cur_list
        p.type = type

    return true

  parse_method: () ->
    @consume_end_commands()
    t = @peek()

    if (not @consume(TokenType.KEYWORD_method)) then return false

    name = @read_identifier()

    @this_method = new Method( t, @this_type, name )
    @this_type.method_list.push( @this_method )
    @this_method.this_library = Program.this_library

    @finish_parsing_method()

    return true

  finish_parsing_method: () ->
    if (@consume(TokenType.SYMBOL_OPEN_PAREN))
      first = true
      while (first or @consume(TokenType.SYMBOL_COMMA))
        first = false
        t = @peek()
        name = @read_identifier()
        v = new Local( t, name )
        if (@consume(TokenType.SYMBOL_COLON))
          v.type = @parse_type()

        @this_method.parameter_list.push( v )
        @this_method.local_list.push( v )

      @must_consume( TokenType.SYMBOL_CLOSE_PAREN )

    if (@consume(TokenType.SYMBOL_ARROW))
      @this_method.return_type = @parse_type()

    @consume_end_commands()

    t = @peek()
    if (@consume(TokenType.SYMBOL_FAT_ARROW))
      if (not @parse_output_alias(t)) then @parse_code_alias(t)
    else
      @parse_multi_line_statements( @this_method.statements )

    @this_method = null

  parse_output_alias: (t) ->
    @consume_end_commands()
    if (not @consume(TokenType.SYMBOL_DOLLAR)) then return false

    t = @must_read( TokenType.LITERAL_STRING, 'Alias string expected, e.g. $"return $a"' )
    alias = t.value

    @consume_end_commands()
    while (@consume(TokenType.SYMBOL_FAT_ARROW) | @consume(TokenType.SYMBOL_DOLLAR) | @next_is(TokenType.LITERAL_STRING))
      t = @must_read( TokenType.LITERAL_STRING, 'Alias string expected, e.g. $"return $a"' )
      alias += t.value
      @consume_end_commands()

    #console.log "ALIAS:" + alias

    @this_method.is_alias = true
    @this_method.output_alias = alias

    return true

  parse_code_alias: (t) ->
    @consume_eols()

    if (@this_method.return_type)
      @this_method.statements.data.push( @parse_expression() )
    else
      @parse_multi_line_statements( @this_method.statements )

    @this_method.is_alias = true
    return true

  #@consume_end_commands()
  #if (@next_is(TokenType.SYMBOL_DOLLAR))

  parse_type: () ->
    t = @peek()
    name = @parse_type_name()
    return Program.get_type_reference( t, name )

  parse_type_name: () ->
    name = ""

    indirection_level = 0
    while (@consume(TokenType.SYMBOL_OPEN_BRACKET))
      name += "["
      ++indirection_level

    name += @read_identifier()

    c = indirection_level
    while (c > 0)
      @must_consume( TokenType.SYMBOL_CLOSE_BRACKET )
      name += "]"
      --c

    while (@consume(TokenType.SYMBOL_EMPTY_BRACKETS))
      name += "[]"

    return name

  parse_multi_line_statements: (statements) ->
    @consume_end_commands()
    while (@next_is_statement())
      @parse_statement( statements, true )
      while (@consume(TokenType.EOL) or @consume(TokenType.SYMBOL_SEMICOLON)) then # no action
      @consume_end_commands()

  parse_single_line_statements: (statements) ->
    while (@next_is_statement())
      @parse_statement( statements, false )
      if (not @consume(TokenType.SYMBOL_SEMICOLON)) then return
      while (@consume(TokenType.SYMBOL_SEMICOLON)) then # noAction

    if (not @consume(TokenType.EOL))
      if (not TokenType.is_structural[@peek().type]) then @must_consume_eol()  # force an error

  parse_statement: (statements,allow_control_structures) ->
    t = @peek()

    #if (next_is(TokenType.keyword_delegate))
    #  throw t.error( "Unused delegate definition." )
    #endIf

    if (allow_control_structures)
      if (@next_is(TokenType.KEYWORD_forEach))
        statements.add( @parse_forEach() )
        return

      if (@next_is(TokenType.KEYWORD_if))
        statements.add( @parse_if() )
        return

      if (@next_is(TokenType.KEYWORD_while))
        statements.add( @parse_while() )
        return

    #  elseIf (next_is(TokenType.keyword_which))
    #    statements.add( parse_which )
    #    return

    #  elseIf (next_is(TokenType.keyword_whichIs))
    #    statements.add( parse_which( true ) )
    #    return

    #  elseIf (next_is(TokenType.keyword_contingent))
    #    statements.add( parse_contingent )
    #    return

    #  elseIf (next_is(TokenType.keyword_while))
    #    statements.add( parse_while )
    #    return

    #  elseIf (next_is(TokenType.keyword_loop))
    #    statements.add( parse_loop )
    #    return

    #  elseIf (next_is(TokenType.keyword_try))
    #    statements.add( parse_try )
    #    return

    #  endIf
    else
      err = false
      if (@next_is(TokenType.KEYWORD_if)) then err = true

      if (err) then throw t.error( "Control structures must begin on a separate line." )

    if (@consume(TokenType.KEYWORD_return))
      if (not @this_method)
        throw t.error( "Illegal 'return' outside of a method." )

      if (@next_is_end_command())
        statements.add( new Cmd.Return(t) )
      else
        statements.add( new Cmd.Return(t,@parse_expression()) )

      return

    else if (@next_is(TokenType.KEYWORD_local))
      if (not @this_method)
        throw @peek().error "Cannot declare local variables outside of a method."

      @parse_variable_declaration( statements, Cmd.LocalDeclaration )
      return

    else if (@next_is(TokenType.KEYWORD_global))
      if (@this_method)
        throw @peek().error "Cannot declare global variables inside a method."
      @parse_variable_declaration( statements, Cmd.GlobalDeclaration )
      return

    #elseIf (consume(TokenType.keyword_yield))

    #  if (next_is_end_command)
    #    statements.add( CmdYieldNil(t) )
    #  elseIf (this_routine.is_task and this_routine.return_type is null)
    #    throw t.error( "This task does not declare a return type." )
    #  else
    #    statements.add( CmdYieldValue(t,parse_expression) )
    #  endIf
    #  return

    #elseIf (consume(TokenType.keyword_yieldAndWait))
    #  statements.add( CmdYieldAndWait(t,parse_expression) )
    #  return

    #elseIf (consume(TokenType.keyword_yieldWhile))
    #  local cmd_while = CmdWhile( t, parse_expression )

    #  if (consume(TokenType.keyword_withTimeout))
    #    local t2 = peek
    #    local timeout_expr = parse_expression
    #    local timeout_name = "_timeout_" + Program.unique_id
    #    cmd_while.condition = CmdLogicalAnd( t2,
    #      CmdCompareLT( t2, CmdAccess(t2, "Time", "current"), CmdAccess(t2,timeout_name) ),
    #      cmd_while.condition )

    #    local v = Local( t2, timeout_name )
    #    this_routine.add_local( v )
    #    statements.add( CmdLocalDeclaration( t2, v ) )
    #    statements.add( 
    #      CmdWriteLocal( t2, v, 
    #        CmdAdd( t2,
    #          CmdAccess(t2,"Time","current"),
    #          timeout_expr
    #        )
    #      )
    #    )
    #  endIf

    #  cmd_while.body.add( CmdYieldNil(t) )
    #  statements.add( cmd_while )

    #  return

    #elseIf (consume(TokenType.keyword_throw))
    #  statements.add( CmdThrow(t,parse_expression) )
    #  return

    #elseIf (consume(TokenType.keyword_noAction))
    #  noAction
    #  return

    #elseIf (consume(TokenType.keyword_trace))
    #  local cmd_trace = CmdTrace(t,this_routine)
    #  while (reader.has_another)
    #    if (next_is(TokenType.symbol_semicolon) or next_is(TokenType.eol) or next_is(TokenType.symbol_close_brace))
    #      escapeWhile
    #    endIf

    #    local pos1 = reader.position
    #    t = peek
    #    if (consume(TokenType.symbol_comma))
    #      cmd_trace.labels.add(", ")
    #      cmd_trace.expressions.add( CmdLiteralString(t,", ") )
    #    else
    #      cmd_trace.expressions.add( parse_expression )
    #      cmd_trace.labels.add( reader.source_string(pos1,reader.position-1) )
    #    endIf

    #  endWhile
    #  statements.add( cmd_trace )
    #  return

    #elseIf (consume(TokenType.keyword_tron))
    #  statements.add( CmdTron(t) )
    #  return

    #elseIf (consume(TokenType.keyword_troff))
    #  statements.add( CmdTroff(t) )
    #  return
    #endIf

    ##{
    #if (consume("println"))
    #  if (next_is_end_command) statements.add( CmdPrintln(t) )
    #  else                     statements.add( CmdPrintln(t, parse_expression) )
    #  return
    #endIf

    #if (consume("print"))
    #  statements.add( CmdPrintln(t, parse_expression).without_newline )
    #  return
    #endIf
    #}#

    #if (consume(TokenType.keyword_escapeContingent))
    #  statements.add( CmdEscapeContingent(t) )
    #  return
    #endIf

    #if (consume(TokenType.keyword_escapeForEach))
    #  statements.add( CmdEscapeForEach(t) )
    #  return
    #endIf

    #if (consume(TokenType.keyword_escapeIf))
    #  statements.add( CmdEscapeIf(t) )
    #  return
    #endIf

    #if (consume(TokenType.keyword_escapeLoop))
    #  statements.add( CmdEscapeLoop(t) )
    #  return
    #endIf

    #if (consume(TokenType.keyword_escapeTry))
    #  statements.add( CmdEscapeTry(t) )
    #  return
    #endIf

    #if (consume(TokenType.keyword_escapeWhich))
    #  statements.add( CmdEscapeWhich(t) )
    #  return
    #endIf

    #if (consume(TokenType.keyword_escapeWhile))
    #  statements.add( CmdEscapeWhile(t) )
    #  return
    #endIf

    #if (consume(TokenType.keyword_nextIteration))
    #  statements.add( CmdNextIteration(t) )
    #  return
    #endIf

    #if (consume(TokenType.keyword_necessary))
    #  statements.add( CmdNecessary(t, parse_expression))
    #  return
    #endIf

    #if (consume(TokenType.keyword_sufficient))
    #  statements.add( CmdSufficient(t, parse_expression))
    #  return
    #endIf

    #if (consume(TokenType.symbol_increment))
    #  statements.add( CmdIncrement(t, parse_expression) )
    #  return
    #endIf

    #if (consume(TokenType.symbol_decrement))
    #  statements.add( CmdDecrement(t, parse_expression) )
    #  return
    #endIf

    expression = @parse_expression()

    t = @peek()
    if (@consume(TokenType.SYMBOL_EQUALS))
      statements.add( new Cmd.Assign(t,expression,@parse_expression()) )
      return

    ##{
    #if (consume(TokenType.symbol_access_assign))
    #  local rhs = parse_expression
    #  local access = rhs as CmdAccess
    #  if (access is null) throw rhs.error( "Property access or method call expected." )
    #  while (access.operand?)
    #    local operand = access.operand as CmdAccess
    #    if (operand is null) throw access.operand.t.error( "Property access or method call expected." )
    #    access = operand
    #  endWhile
    #  access.operand = expression.clone
    #  statements.add( CmdAssign(t,expression,access) )
    #endIf
    #}#

    #if (consume(TokenType.symbol_increment))
    #  statements.add( CmdIncrement(t, expression) )
    #  return
    #endIf

    #if (consume(TokenType.symbol_decrement))
    #  statements.add( CmdDecrement(t, expression) )
    #  return
    #endIf

    #local t_type = t.type
    #if (t_type >= TokenType.first_shorthand_operator and t_type <= TokenType.last_shorthand_operator)
    #  read
    #  local lhs = expression.clone
    #  local rhs = parse_expression
    #  local new_value : Cmd

    #  which (t_type)
    #    case TokenType.symbol_add_assign:          new_value = CmdAdd( t, lhs, rhs )
    #    case TokenType.symbol_subtract_assign:     new_value = CmdSubtract( t, lhs, rhs )
    #    case TokenType.symbol_multiply_assign:     new_value = CmdMultiply( t, lhs, rhs )
    #    case TokenType.symbol_divide_assign:       new_value = CmdDivide( t, lhs, rhs )
    #    case TokenType.symbol_mod_assign:          new_value = CmdMod( t, lhs, rhs )
    #    case TokenType.symbol_power_assign:        new_value = CmdPower( t, lhs, rhs )
    #    case TokenType.symbol_bitwise_and_assign:  new_value = CmdBitwiseAnd( t, lhs, rhs )
    #    case TokenType.symbol_bitwise_or_assign:   new_value = CmdBitwiseOr( t, lhs, rhs )
    #    case TokenType.symbol_bitwise_xor_assign:  new_value = CmdBitwiseXor( t, lhs, rhs )
    #    case TokenType.symbol_access_assign
    #      local access = rhs as CmdAccess
    #      if (access is null or access.operand?) throw t.error( "Syntax error." )
    #      access.operand = lhs
    #      new_value = access
    #  endWhich
    #  statements.add( CmdAssign(t, expression, new_value) )
    #  return
    #endIf

    ## No-parens args can follow an initial expression
    #if (not next_is_end_command)
    #  local access = expression as CmdAccess
    #  if (access? and access.args is null)
    #    local args = CmdArgs()
    #    while (not next_is_end_command and peek.type >= TokenType.last_non_statement)
    #      args.add( parse_expression )
    #    endWhile
    #    access.args = args
    #  endIf
    #endIf

    statements.add( expression )

  parse_variable_declaration: ( statements, cmd_type, allow_global ) ->
    @reader.read()  # 'local' / 'global'

    vars = []
    first = true
    while (first or @consume(TokenType.SYMBOL_COMMA))
      first = false
      t = @peek()
      name = @read_identifier()

      initial_value = null
      if (@consume(TokenType.SYMBOL_EQUALS))
        initial_value = @parse_expression()

      vars.push( {t:t,name:name,initial_value:initial_value} )

    if (@consume(TokenType.SYMBOL_COLON))
      type = @parse_type()
      for v in vars
        v.type = type

    for v in vars
      statements.add( new cmd_type(v.t, v.name, v.type, v.initial_value) )

  parse_forEach: () ->
    t = @reader.read()
    has_open_paren = @consume( TokenType.SYMBOL_OPEN_PAREN )

    control_var_t    = @peek()
    control_var_name = @read_identifier()

    @must_consume( TokenType.KEYWORD_in )

    expr = @parse_expression()

    @must_consume( TokenType.SYMBOL_UP_TO )

    last = @parse_expression()

    if (has_open_paren) then @must_consume( TokenType.SYMBOL_CLOSE_PAREN )

    cmd = new Cmd.ForEachInRange( t, control_var_t, control_var_name, expr, last )

    if (@consume_eols())
      # multi-line forEach
      @parse_multi_line_statements( cmd.statements )

      @must_consume( TokenType.KEYWORD_endForEach )

    else
      # single-line forEach
      @parse_single_line_statements( cmd.statements )
      @consume_eols()

    return cmd


  parse_if: () ->
    t = @reader.read()
    cmd_if = new Cmd.If( t, @parse_expression() )

    if (@consume_eols())
      # multi-line if
      @parse_multi_line_statements( cmd_if.statements )

      while (@next_is(TokenType.KEYWORD_elseIf))
        if (not cmd_if.elseIf_conditions)
          cmd_if.elseIf_conditions = []
          cmd_if.elseIf_statements = []

        # Need a slight kludge to avoid dangling elseIf problems.
        starting_position = @reader.position
        t = @reader.read()
        elseIf_condition = @parse_expression()
        if (@consume(TokenType.EOL))
          elseIf_statements = new Cmd.StatementList(t)
          @parse_multi_line_statements( elseIf_statements )

          cmd_if.elseIf_conditions.push( elseIf_condition )
          cmd_if.elseIf_statements.push( elseIf_statements )
        else
          @reader.position = starting_position
          break

      if (@next_is(TokenType.KEYWORD_else) and @peek(1).type == TokenType.EOL)
        t = @reader.read()
        cmd_if.else_statements = new Cmd.StatementList(t)
        @parse_multi_line_statements( cmd_if.else_statements )

      @must_consume( TokenType.KEYWORD_endIf )

    else
      # single-line if
      @parse_single_line_statements( cmd_if.statements )
      @consume_eols()

      while (@next_is(TokenType.KEYWORD_elseIf))
        if (not cmd_if.elseIf_conditions)
          cmd_if.elseIf_conditions = []
          cmd_if.elseIf_statements = []

        # Need logic to avoid dangling elseIf problems.
        starting_position = @reader.position
        t = @reader.read()
        elseIf_condition = @parse_expression()
        if (@next_is(TokenType.EOL))
          @reader.position = starting_position
          break
        else
          elseIf_statements = new Cmd.StatementList(t)
          @parse_single_line_statements( elseIf_statements )
          @must_consume_eol()

          cmd_if.elseIf_conditions.push( elseIf_condition )
          cmd_if.elseIf_statements.push( elseIf_statements )

      if (@next_is(TokenType.KEYWORD_else) and @peek(1).type != TokenType.EOL)
        t = @reader.read()
        cmd_if.else_statements = new Cmd.StatementList(t)
        @parse_single_line_statements( cmd_if.else_statements )

    return cmd_if


  parse_while: () ->
    t = @reader.read()
    cmd_while = new Cmd.While( t, @parse_expression() )

    if (@consume_eols())
      # multi-line while
      @parse_multi_line_statements( cmd_while.statements )

      @must_consume( TokenType.KEYWORD_endWhile )

    else
      # single-line while
      @parse_single_line_statements( cmd_while.statements )
      @consume_eols()

    return cmd_while


  #----------------------------------------------------------------------------
  parse_expression: () ->
    return @parse_range()

  parse_range: () ->
    return @parse_logical_xor()

  parse_logical_xor: () ->
    return @parse_logical_or()

  parse_logical_or: () ->
    return @parse_logical_and()

  parse_logical_and: () ->
    return @parse_comparison()

  parse_comparison: (lhs) ->
    if (lhs)
      t = @peek()

      if (@consume(TokenType.SYMBOL_EQ))
        @consume_eols()
        return @parse_comparison( new Cmd.CompareEQ(t,lhs,@parse_bitwise_xor()) )

      else if (@consume(TokenType.SYMBOL_NE))
        @consume_eols()
        return @parse_comparison( new Cmd.CompareNE(t,lhs,@parse_bitwise_xor()) )

      else if (@consume(TokenType.SYMBOL_LT))
        @consume_eols()
        return @parse_comparison( new Cmd.CompareLT(t,lhs,@parse_bitwise_xor()) )

      else if (@consume(TokenType.SYMBOL_LE))
        @consume_eols()
        return @parse_comparison( new Cmd.CompareLE(t,lhs,@parse_bitwise_xor()) )

      else if (@consume(TokenType.SYMBOL_GT))
        @consume_eols()
        return @parse_comparison( new Cmd.CompareGT(t,lhs,@parse_bitwise_xor()) )

      else if (@consume(TokenType.SYMBOL_GE))
        @consume_eols()
        return @parse_comparison( new Cmd.CompareGE(t,lhs,@parse_bitwise_xor()) )

      return lhs

    else
      return @parse_comparison( @parse_bitwise_xor() )

  parse_bitwise_xor: () ->
    return @parse_bitwise_or()

  parse_bitwise_or: () ->
    return @parse_bitwise_and()

  parse_bitwise_and: () ->
    return @parse_bitwise_shift()

  parse_bitwise_shift: () ->
    return @parse_add_subtract()

  parse_add_subtract: (lhs) ->
    if (lhs)
      t = @peek()
      if (@consume(TokenType.SYMBOL_PLUS))
        @consume_eols()
        return @parse_add_subtract( new Cmd.Add(t,lhs,@parse_multiply_divide_mod()) )
      else if (@consume(TokenType.SYMBOL_MINUS))
        @consume_eols()
        return @parse_add_subtract( new Cmd.Subtract(t,lhs,@parse_multiply_divide_mod()) )
      return lhs

    else
      return @parse_add_subtract( @parse_multiply_divide_mod() )

  parse_multiply_divide_mod: (lhs) ->
    if (lhs)
      t = @peek()
      if (@consume(TokenType.SYMBOL_TIMES))
        @consume_eols()
        return @parse_multiply_divide_mod( new Cmd.Multiply(t,lhs,@parse_pre_unary()) )
      else if (@consume(TokenType.SYMBOL_DIVIDE))
        @consume_eols()
        return @parse_multiply_divide_mod( new Cmd.Divide(t,lhs,@parse_pre_unary()) )
      else if (@consume(TokenType.SYMBOL_INTEGER_DIVIDE))
        @consume_eols()
        return @parse_multiply_divide_mod( new Cmd.IntegerDivide(t,lhs,@parse_pre_unary()) )
      return lhs

    else
      return @parse_multiply_divide_mod( @parse_pre_unary() )

  parse_pre_unary: () ->
    t = @peek()
    if (@consume(TokenType.SYMBOL_INCREMENT))
      return new Cmd.PreIncrement( t, @parse_pre_unary() )
    else if (@consume(TokenType.SYMBOL_DECREMENT))
      return new Cmd.PreDecrement( t, @parse_pre_unary() )
    else if (@consume(TokenType.SYMBOL_MINUS))
      return new Cmd.Negate( t, @parse_pre_unary() )

    #else if (@consume(TokenType.KEYWORD_not))
    #  @consume_eols()
    #  return new Cmd.LogicalNot( t, parse_pre_unary )
    #else if (@consume(TokenType.SYMBOL_MINUS))
    #  @consume_eols()
    #  return new Cmd.Negate( t, parse_pre_unary )
    #else if (@consume(TokenType.SYMBOL_BITWISE_NOT))
    #  @consume_eols()
    #  return new Cmd.BitwiseNot( t, parse_pre_unary )

    return @parse_post_unary()

  parse_post_unary: (operand) ->
    if (operand)
      t = @peek()
      if (@consume(TokenType.SYMBOL_QUESTION_MARK))
        return @parse_post_unary( new Cmd.Logicalize(t, operand) )
      else if (@consume(TokenType.SYMBOL_INCREMENT))
        return @parse_post_unary( new Cmd.PostIncrement( t, operand ) )
      else if (@consume(TokenType.SYMBOL_DECREMENT))
        return @parse_post_unary( new Cmd.PostDecrement( t, operand ) )
      else
        return operand
    else
      return @parse_post_unary( @parse_access() )

  parse_access: (operand) ->
    if (operand)
      t = @peek()
      if (@consume(TokenType.SYMBOL_PERIOD))
        access = @parse_access_command()
        access.operand = operand
        return @parse_access( access )
      else
        return operand

    else
      return @parse_access( @parse_term() )


  parse_term: () ->
    t = @peek()
    if (@consume(TokenType.SYMBOL_OPEN_PAREN))
      @consume_eols()
      result = @parse_expression()
      @consume_eols()
      @must_consume( TokenType.SYMBOL_CLOSE_PAREN )
      return result

    else if (@next_is(TokenType.IDENTIFIER) or @next_is(TokenType.SYMBOL_AT))
      return @parse_access_command()

    else if (@consume(TokenType.LITERAL_STRING))
    #  if (@consume(TokenType.SYMBOL_OPEN_PAREN))
    #    local first = true
    #    local args = CmdArgs()
    #    while (first or @consume(TokenType.symbol_comma))
    #      first = false
    #      args.add( @parse_expression() )
    #    endWhile
    #    @must_consume( TokenType.symbol_close_paren )
    #    return CmdFormattedString( t, t->String, args )
    #  endIf
      return new Cmd.LiteralString( t, t.value )

    else if (@consume(TokenType.KEYWORD_null))
      return new Cmd.LiteralNull(t)

    else if (@consume(TokenType.LITERAL_REAL))
      return new Cmd.LiteralReal(t, t.value)

    else if (@consume(TokenType.LITERAL_INTEGER))
      return new Cmd.LiteralInteger(t, t.value)

    else if (@consume(TokenType.LITERAL_CHARACTER))
      return new Cmd.LiteralCharacter(t, t.value)

    else if (@consume(TokenType.KEYWORD_true))
      return new Cmd.LiteralLogical( t, true )

    else if (@consume(TokenType.KEYWORD_false))
      return new Cmd.LiteralLogical( t, false )

    else if (@consume(TokenType.KEYWORD_this))
      return new Cmd.This( t, @this_type )

    #else if (@consume(TokenType.keyword_function))
    #  return parse_function( t )

    else if (@consume(TokenType.KEYWORD_pi))
      return new Cmd.LiteralReal( t, Math.PI )

    #else if (@consume(TokenType.symbol_open_bracket))
    #  # [ literal, list ]
    #  if (parsing_properties?)
    #    local cmd = CmdNewObject( t, Types.reference(t,"PropertyList") ) : Cmd

    #    local first = true
    #    while (first or @consume(TokenType.symbol_comma))
    #      first = false
    #      local value = @parse_expression()
    #      cmd = CmdAccess( value.t, cmd, "add", CmdArgs(value) )
    #      @consume_eols()
    #    endWhile
    #    @must_consume( TokenType.symbol_close_bracket )

    #    return cmd

    #  else
    #    local list = CmdLiteralList(t)
    #    @consume_eols()
    #    if (not @consume(TokenType.symbol_close_bracket))
    #      local first = true
    #      while (first or @consume(TokenType.symbol_comma))
    #        first = false
    #        list.args.add( @parse_expression() )
    #        @consume_eols()
    #      endWhile
    #      @must_consume( TokenType.symbol_close_bracket )
    #    endIf
    #    return list
    #  endIf

    #else if (@consume(TokenType.symbol_open_brace))
    #  # { key:value, key:value, ... }
    #  local table = CmdLiteralTable(t)
    #  @consume_eols()
    #  if (not @consume(TokenType.symbol_close_brace))
    #    local first = true
    #    while (first or @consume(TokenType.symbol_comma))
    #      first = false
    #      if (@peek().type == TokenType.identifier)
    #        local kt = read
    #        table.keys.add( CmdLiteralTableKey(kt, kt->String) )
    #      else
    #        ++parsing_properties
    #        table.keys.add( @parse_expression() )
    #        --parsing_properties
    #      endIf
    #      @must_consume( TokenType.symbol_colon )
    #      ++parsing_properties
    #      table.values.add( @parse_expression() )
    #      --parsing_properties
    #      @consume_eols()
    #    endWhile
    #    @must_consume( TokenType.symbol_close_brace )
    #  endIf
    #  return table

    #else if (@consume(TokenType.symbol_empty_braces))
    #  return CmdNewObject( t, Types.reference( t, "PropertyTable" ) )

    #else if (@consume(TokenType.symbol_empty_brackets))
    #  return CmdNewObject( t, Types.reference( t, "PropertyList" ) )

    #else if (@consume(TokenType.keyword_prior))
    #  @consume_eols()
    #  @must_consume( TokenType.symbol_period )
    #  @consume_eols()
    #  local name = read_identifier
    #  local args = parse_args
    #  return CmdPriorCall( t, name, args )

    #else if (next_is(TokenType.keyword_delegate))
    #  return parse_delegate

    #else if (@consume(TokenType.alias_arg_index))
    #  return alias_args[ t->Integer ].clone

    else
      throw @peek().error( "Syntax error: unexpected \"" + @peek() + "\"." )

  parse_access_command: () ->
    @consume_eols()
    t = @peek()
    name = @parse_type_name()
    result = new Cmd.Access( t, name )

    if (@consume(TokenType.SYMBOL_OPEN_PAREN))
      args = []
      while (args.length == 0 or @consume(TokenType.SYMBOL_COMMA))
        args.push( @parse_expression() )

      @must_consume( TokenType.SYMBOL_CLOSE_PAREN )

    result.args = args

    return result

  peek: (num_ahead) ->
    return @reader.peek(num_ahead)

  read: () ->
    return @reader.read()

  must_read: (type,message) ->
    if (not @next_is(type)) then @must_consume(type,message)
    return @reader.read()

  read_identifier: () ->
    return @reader.read_identifier()



#==============================================================================
# EXPORTS
#==============================================================================
module.exports.Parser       = Parser
module.exports.Preprocessor = Preprocessor
module.exports.Program      = Program
module.exports.TokenType    = Tokenizer.TokenType

