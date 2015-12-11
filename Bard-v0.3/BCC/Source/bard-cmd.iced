Program    = require("./bard-program").Program
ParseError = require("./bard-tokenizer").ParseError

#==============================================================================
# Cmd
#==============================================================================
class Cmd
  cast_to: (to_type) ->
    this_type = @type()

    if (not this_type) then throw @error( "A value is required but a nil expression is given." )

    if (this_type == to_type)
      return this

    if (this_type.is_primitive() or to_type.is_primitive())
      return new ConvertToType( @t, this, to_type )

    throw @error( "Cannot convert from existing type " + this_type.name + " to required type " + to_type.name + "." )

  cmd_type: () ->
    return Cmd

  clone: (params,args) ->
    throw @t.error "[INTERNAL ERROR] clone() not implemented for " + @constructor.name + "."

  error: (message) ->
    return new ParseError( message )

  find_common_type: (lhs_type,rhs_type) ->
    if (lhs_type == rhs_type) then return lhs_type

    if (lhs_type.is_primitive and rhs_type.is_primitive)
      type = Program.type_String
      if (lhs_type == type or rhs_type == type) then return type

      type = Program.type_Real
      if (lhs_type == type or rhs_type == type) then return type

      type = Program.type_Integer
      if (lhs_type == type or rhs_type == type) then return type

      type = Program.type_Character
      if (lhs_type == type or rhs_type == type) then return type

    throw @error( "Cannot mix types " + lhs_type + " and " + rhs_type + "." )

  implicit_type: () ->
    return null

  is_literal: () ->
    return false

  is_library_name: () ->
    return false

  require_logical: () ->
    type = @type()
    if (type == Program.type_Logical) then return this
    throw @t.error( "A logical true/false value is required." )

  require_type: () ->
    type = @type()
    if (type) then return type
    throw @t.error( "A value is required but a nil expression is given." )

  require_value: () ->
    if (@type()) then return this
    throw @t.error( "A value is required but a nil expression is given." )

  requires_semicolon: () -> return true

  resolve: () ->
    throw @t.error( "[INTERNAL] resolve() undefined for " + @constructor.name + "." )

  resolve_access: (is_nested_operand) ->
    return @resolve()

  resolve_assignment: (new_value) ->
    throw @t.error "Illegal assignment."

  toString: () ->
    throw @t.error( "[INTERNAL] toString() undefined for " + @constructor.name + "." )

  print_c: (writer) ->
    throw @t.error( "[INTERNAL] print_c() undefined for " + @constructor.name + "." )

  print_c_header: (writer) ->
    # noAction

  type: () ->
    if (@cached_type) then return @cached_type
    return null


class Literal extends Cmd
  cmd_type: () ->
    return Literal

  implicit_type: () ->
    return @type()

  is_literal: () ->
    return true

  resolve: () ->
    return this

  toString: () ->
    return "" + @value + "->" + @type()

class LiteralNull extends Literal
  constructor: (@t) ->

  cmd_type: () ->
    return LiteralNull

  toString: () ->
    return "null"

class LiteralReal extends Literal
  constructor: (@t,@value) ->

  cmd_type: () ->
    return LiteralReal

  print_c: (writer) ->
    writer.print @value
    if (@value == (@value|0))
      # Write ".0" to disambiguate literal real from literal integer
      writer.print(".0")

  type: () ->
    return Program.type_Real

class LiteralInteger extends Literal
  constructor: (@t,@value) ->

  cast_to: (to_type) ->
    if (to_type == Program.type_Integer) then return this

    switch (to_type)
      when Program.type_String
        return new LiteralString( @t, @value + "" )

      when Program.type_Real
        return new LiteralReal( @t, @value )

    return super(to_type)

  cmd_type: () ->
    return LiteralInteger

  clone: (params,args) ->
    return new LiteralInteger( @t, @value )

  type: () ->
    return Program.type_Integer

  print_c_header: (writer) ->
    console.log "literal integer header"
    return this

  print_c: (writer) ->
    writer.print @value

class LiteralCharacter extends Literal
  constructor: (@t,@value) ->

  cast_to: (to_type) ->
    if (to_type == Program.type_Character) then return this

    switch (to_type)
      when Program.type_Real
        return new LiteralReal( @t, @value.charCodeAt(0) )

      when Program.type_Integer
        return new LiteralInteger( @t, @value.charCodeAt(0) )

    return super(to_type)

  cmd_type: () ->
    return LiteralCharacter

  type: () ->
    return Program.type_Character

class LiteralLogical extends Literal
  constructor: (@t,@value) ->

  cmd_type: () ->
    return LiteralLogical

  print_c: (writer) ->
    if (@value) then writer.print(1)
    else             writer.print(0)

  type: () ->
    return Program.type_Logical

class LiteralString extends Literal
  constructor: (@t,@value) ->

  cast_to: (to_type) ->
    if (to_type.instance_of(Program.type_String)) then return this

    switch (to_type)
      when Program.type_Real
        return new LiteralReal( @t, Number(@value) )

      when Program.type_Integer
        return new LiteralInteger( @t, Number(@value) )

      when Program.type_Character
        if (@value == null or @value.length == 0)
          return new LiteralCharacter( @t, '\0' )
        return new LiteralCharacter( @t, @value[0] )

      #when Program.type_Byte
      #  if (@value == null or @value.length == 0)
      #    return new LiteralByte( @t, '\0' )
      #  return new LiteralByte( @t, @value[0] )

    return super(to_type)

  cmd_type: () ->
    return LiteralString

  print_c: (writer) ->
    writer.print JSON.stringify(@value)

  type: () ->
    return Program.type_LiteralString

class This
  constructor: (@t,@this_type) ->

  cmd_type: () ->
    return This

  toString: () ->
    return "this"

#==============================================================================
# Unary Commands
#==============================================================================
class Unary extends Cmd
  constructor: (@t,@operand) ->

  bard_prefix: () ->
    return @prefix()

  bard_suffix: () ->
    return @suffix()

  cmd_type: () ->
    return Unary

  implicit_type: () ->
    return @operand.implicit_type()

  prefix: () -> ""

  print_c: (writer) ->
    writer.print( @prefix() )
    @operand.print_c( writer )
    writer.print( @suffix() )

  resolve: () ->
    @operand = @operand.resolve()
    
    @cached_type = @operand.require_type()

    return @resolve_for_value_type( @cached_type )

  resolve_for_value_type: (value_type) ->
    return this

  suffix: () -> ""

  toString: () ->
    return "(" + @operand.toString() + @bard_suffix() + ")"

class UnaryWithModify extends Unary
  cmd_type: () ->
    return UnaryWithModify

  resolve: () ->
    @operand = @operand.resolve()
    
    # disable operand requirement for new
    #value_type = @require_type( @operand )
    # return @resolve_for_value_type( value_type )

    return this

class PostDecrement extends UnaryWithModify
  cmd_type: () ->
    return PostDecrement

  suffix: () ->
    return "--"

  resolve_for_value_type: (value_type) ->
    return this

class PostIncrement extends UnaryWithModify
  cmd_type: () ->
    return PostIncrement

  suffix: () ->
    return "++"

  resolve_for_value_type: (value_type) ->
    return this

class PreDecrement extends UnaryWithModify
  cmd_type: () ->
    return PreDecrement

  prefix: () ->
    return "--"

  resolve_for_value_type: (value_type) ->
    return this

class PreIncrement extends UnaryWithModify
  cmd_type: () ->
    return PreIncrement

  prefix: () ->
    return "++"

  resolve_for_value_type: (value_type) ->
    return this

class Logicalize extends Unary
  cmd_type: () ->
    return Logicalize

  suffix: () ->
    return "?"

  resolve_for_value_type: (value_type) ->
    if (@operand instanceof Literal)
      return new LiteralLogical( @operand.t, !!@operand.operand )
    else
      return this

  type: () ->
    return Program.type_Logical


class Negate extends Unary
  cmd_type: () ->
    return Negate

  prefix: () ->
    return "-"

  resolve_for_value_type: (value_type) ->
    return this


class CastToType extends Unary
  constructor: (@t,@operand,@to_type) ->

  cmd_type: () ->
    return CastToType

  print_c: (writer) ->
    writer.print("((").print_type(@to_type).print(')')
    @operand.print_c( writer )
    writer.print(')')

  resolve_for_value_type: (value_type) ->
    if (value_type == @to_type) then return @operand
    # TODO
    return this

  toString: () ->
    return "((" + @to_type + ") " + @operand.toString() + ")"

  type: () ->
    return @to_type


class ReinterpretAsType extends Unary
  constructor: (@t,@operand,@as_type) ->

  cmd_type: () ->
    return ReinterpretAsType

  print_c: (writer) ->
    writer.print("((").print_type(@as_type).print(')')
    @operand.print_c( writer )
    writer.print(')')

  resolve_for_value_type: (value_type) ->
    # TODO: validate cast
    return this

  toString: () ->
    return "(" + @operand.toString() + ") as " + @as_type + ")"

  type: () ->
    return @as_type


class ConvertToType extends Unary
  constructor: (@t,@operand,@to_type) ->

  cmd_type: () ->
    return ConvertToType

  print_c: (writer) ->
    writer.print("((").print_type(@to_type).print(')')
    @operand.print_c( writer )
    writer.print(')')

  resolve_for_value_type: (value_type) ->
    # TODO: validate cast
    return this

  toString: () ->
    return "(" + @operand.toString() + "->" + @to_type + ")"

  type: () ->
    return @to_type


#==============================================================================
# Binary Commands
#==============================================================================
class Binary extends Cmd
  constructor: (@t,@lhs,@rhs) ->

  bard_name: () ->
    return @name()

  cast_args_to_common_type: (lhs_type,rhs_type) ->
    if (lhs_type == rhs_type) then return lhs_type

    if (lhs_type.is_primitive and rhs_type.is_primitive)
      type = Program.type_String
      if (lhs_type == type or rhs_type == type)
        @lhs = @lhs.cast_to( type )
        @rhs = @rhs.cast_to( type )
        return type

      type = Program.type_Real
      if (lhs_type == type or rhs_type == type)
        @lhs = @lhs.cast_to( type )
        @rhs = @rhs.cast_to( type )
        return type

      type = Program.type_Integer
      if (lhs_type == type or rhs_type == type)
        @lhs = @lhs.cast_to( type )
        @rhs = @rhs.cast_to( type )
        return type

      type = Program.type_Character
      if (lhs_type == type or rhs_type == type)
        @lhs = @lhs.cast_to( type )
        @rhs = @rhs.cast_to( type )
        return type

    throw @error( "Cannot mix types " + lhs_type + " and " + rhs_type + " in this operation." )

  clone: (params,args) ->
    cmd_type = @cmd_type()
    return new cmd_type( @t, @lhs.clone(params,args), @rhs.clone(params,args) )

  cmd_type: () ->
    return Binary

  combine_literal_operands: () ->
    cmd_type = @lhs.cmd_type()
    return new cmd_type( @t, @operate(@lhs.value, @rhs.value) )

  operate: (left,right) ->
    throw @t.error( "TODO: operate() for " + @constructor.name )

  print_c: (writer) ->
    writer.print( "(" )
    @lhs.print_c( writer )
    writer.print( " " )
    writer.print( @name() )
    writer.print( " " )
    @rhs.print_c( writer )
    writer.print( ")" )

  resolve: () ->
    @lhs = @lhs.resolve()
    @rhs = @rhs.resolve()
    
    lhs_type = @lhs.require_type()
    rhs_type = @rhs.require_type()

    return @resolve_args_of_type(lhs_type, rhs_type)

  resolve_args_of_type: (lhs_type,rhs_type) ->
    @common_type = @cast_args_to_common_type( lhs_type, rhs_type )
    @cached_type = @type()

    if (@lhs.is_literal() and @rhs.is_literal())
      return @combine_literal_operands()

    return this

  toString: () ->
    return @lhs.toString() + " " + @name() + " " + @rhs.toString()

  type: () ->
    if (@cached_type) then return @cached_type
    return @common_type


class Add extends Binary
  name: () -> "+"

  cmd_type: () ->
    return Add

  operate: (left,right) ->
    return left + right


class Subtract extends Binary
  name: () -> "-"

  cmd_type: () ->
    return Subtract

  operate: (left,right) ->
    return left - right

class Multiply extends Binary
  name: () -> "*"

  cmd_type: () ->
    return Multiply

  operate: (left,right) ->
    return left - right

class Divide extends Binary
  name: () -> "/"

  cast_args_to_common_type: (lhs_type,rhs_type) ->
    type = super(lhs_type,rhs_type)

    if (type == Program.type_Integer)
      @lhs = @lhs.cast_to( Program.type_Real )
      @rhs = @rhs.cast_to( Program.type_Real )
      return Program.type_Real

    return type

  cmd_type: () ->
    return Divide

  operate: (left,right) ->
    return left / right


class IntegerDivide extends Binary
  bard_name: () -> ":/:"

  cmd_type: () ->
    return IntegerDivide

  name: () -> "/"

  operate: (left,right) ->
    return (left / right) | 0

  resolve: () ->
    result = super().cast_to(Program.type_Integer)
    return result


class Compare extends Binary
  cmd_type: () ->
    return Compare

  combine_literal_operands: () ->
    return new LiteralLogical( @t, @operate(@lhs.value, @rhs.value) )

  type: () ->
    return Program.type_Logical


class CompareEQ extends Compare
  name: () -> "=="

  cmd_type: () ->
    return CompareEQ

  operate: (left,right) ->
    return left == right

class CompareNE extends Compare
  name: () -> "!="

  cmd_type: () ->
    return CompareNE

  operate: (left,right) ->
    return left != right

class CompareLT extends Compare
  name: () -> "<"

  cmd_type: () ->
    return CompareLT

  operate: (left,right) ->
    return left < right

class CompareLE extends Compare
  name: () -> "<="

  cmd_type: () ->
    return CompareLE

  operate: (left,right) ->
    return left <= right

class CompareGT extends Compare
  name: () -> ">"

  cmd_type: () ->
    return CompareGT

  operate: (left,right) ->
    return left > right

class CompareGE extends Compare
  name: () -> ">="

  cmd_type: () ->
    return CompareGE

  operate: (left,right) ->
    return left >= right


#==============================================================================
# Access and Call
#==============================================================================
class Access extends Cmd
  constructor: (@t,@name) ->

  clone: (params,args) ->
    if (not @args)
      # May need to substitute an alias arg
      for p,i in params
        if (p.name == @name)
          return args[i].clone()

    # General case
    result = new Access(@t,@name)
    if (@args)
      result.args = []
      for arg in @args
        result.args.push( arg.clone(params,args) )

    return result

  cmd_type: () ->
    return Access

  is_library_name: () ->
    return ((not @args) and (Program.partial_library_prefixes[@name]))

  resolve: () ->
    return @resolve_access( false )

  resolve_access: (is_nested_operand) ->
    if (@operand)
      @operand = @operand.resolve_access( true )

      if (@operand.is_library_name())
        # Change "PartialPackageName"."ElementName" to "PartialPackageName.ElementName"
        result = new Access( @t, @operand.name + "." + @name )
        result.args = @args
        return result.resolve_access( is_nested_operand )

    if (is_nested_operand and @is_library_name()) then return this

    if (not @args)
      # Could be a local or property
      v = Program.find_local( @name )
      if (v)
        return new LocalRead( @t, v )

    return Program.resolve_call( @t, null, @operand, @name, @args )

  resolve_assignment: (new_value) ->
    if (not @args)
      v = Program.find_local( @name )
      if (v)
        return new LocalWrite( @t, v, new_value )

    throw @t.error "Illegal assignment."

  toString: () ->
    return @name

  type: () ->
    return null

class LocalRead extends Cmd
  constructor: (@t,@local_info) ->

  clone: (params,args) ->
    return new LocalRead( @t, @local_info )

  cmd_type: () ->
    return LocalRead

  print_c: (writer) ->
    writer.print( @local_info.name )

  toString: () ->
    return @local_info.name

  type: () ->
    return @local_info.type

class LocalWrite extends Cmd
  constructor: (@t,@local_info,@new_value) ->

  clone: (params,args) ->
    return new LocalWrite( @t, @local_info, @new_value )

  cmd_type: () ->
    return LocalWrite

  print_c: (writer) ->
    writer.print( @local_info.name ).print(" = ")
    @new_value.print_c( writer )

  toString: () ->
    return @local_info.name + " = " + @new_value

  type: () ->
    return @local_info.type

class AliasCall extends Cmd
  constructor: (@t,@context,@routine_info,@args) ->

  clone: (params,args) ->
    result = new AliasCall( @t, null, @routine_info, [] )
    if (@context) then result.context = context.clone(params,args)
    for arg in @args
      result.args.push( arg.clone(params,args) )
    return result

  cmd_type: () ->
    return AliasCall

  print_output_alias: (writer) ->
    alias = @routine_info.output_alias
    while (alias.length)
      token_start = alias.indexOf('$')
      if (token_start == -1)
        writer.print alias
        break

      writer.print( alias.substr(0,token_start) )
      alias = alias.substr(token_start+1)

      found_arg = false
      for param,i in @routine_info.parameter_list
        if (alias.lastIndexOf(param.name,0) == 0)
          writer.visit( @args[i] )  # CWriter calls 'print_c' on given Cmd, etc.
          alias = alias.substr(param.name.length)
          found_arg = true
          break

      if (not found_arg)
        if (alias.lastIndexOf("this",0) == 0)
          if (@context)
            writer.visit( @context )
          else
            throw "Illegal access to 'this' object context from routine - only methods can access 'this'."

        else
          writer.print "$"

  print_c: (writer) ->
    @print_output_alias(writer)

  type: () ->
    return @routine_info.return_type


#==============================================================================
# Assignment
#=============================================================================
class Assign extends Cmd
  constructor: (@t,@target,@value) ->

  cmd_type: () ->
    return Assign

  print_c: (writer) ->
    writer.print "TODO"

  resolve: () ->
    return @target.resolve_assignment( @value.resolve() )

  toString: () ->
    return @target + " = " + @value


#==============================================================================
# Statements and Control Structures
#==============================================================================
class GlobalDeclaration extends Cmd
  constructor: (@t,@name,@type,@initial_value) ->

  cmd_type: () ->
    return GlobalDeclaration

  toString: () ->
    result = "global " + @name
    if (@initial_value) then result += "=" + @initial_value
    if (@type) then result += " : " + @type
    return result


class LocalDeclaration extends Cmd
  constructor: (@t,@name,@type,@initial_value) ->

  cmd_type: () ->
    return LocalDeclaration

  print_c_declarations: (writer) ->
    writer.print_type( @type ).print( " " ).print( @name ).println(";")
    return this

  print_c: (writer) ->
    if (@initial_value)
      writer.print( @name ).print(" = ")
      @initial_value.print_c( writer )

  resolve: () ->
    @index = Program.local_stack.length
    Program.local_stack.push( this )

    if (@initial_value)
      @initial_value = @initial_value.resolve().require_value()
      if (not @type)
        @type = @initial_value.implicit_type()
        if (not @type) then throw @t.error( "Unable to infer local variable type from initial value." )

    else
      if (not @type) then throw @t.error( "Missing type declaration or initial value." )
      
    @type.resolve()

    return this

  toString: () ->
    result = "local " + @name
    if (@initial_value) then result += "=" + @initial_value
    if (@type) then result += " : " + @type
    return result

class Return extends Cmd
  constructor: (@t,value) ->
    if (value) then @value = value

  cmd_type: () ->
    return Return

  print_c: (writer) ->
    writer.print "return"

    if (@value)
      writer.print(" ")
      @value.print_c( writer )

  resolve: () ->
    if (@value) then @value = @value.resolve()
    return this

  toString: () ->
    if (@value) then return "return " + @value
    return "return"


statements_to_string = (statements) ->
  result = ""
  for statement in statements
    result += statement.toString() + "\n"
  return result

class ControlStructure extends Cmd
  cmd_type: () ->
    return ControlStructure

  requires_semicolon: () -> return false


class StatementList extends ControlStructure
  constructor: (@t) ->
    @data = []

  add: (statement) ->
    @data.push( statement )
    return this

  cmd_type: () ->
    return StatementList

  insert: (statement) ->
    @data.unshift( statement )
    return this

  print_c: (writer) ->
    for statement,index in @data
      if (statement.print_c_declarations)
        statement.print_c_declarations( writer )

    for statement,index in @data
      statement.print_c( writer )
      if (statement.requires_semicolon()) then writer.println ";"

  resolve: () ->
    local_count = Program.local_stack.length

    for statement,index in @data
      @data[index] = statement.resolve()

    while (Program.local_stack.length > local_count)
      Program.local_stack.pop()

    return this

  toString: () ->
    result = "statements\n"
    result += statements_to_string( @data ).indent(2)
    result += "endStatements"
    return result


class If extends ControlStructure
  constructor: (@t,@condition) ->
    @statements = new StatementList(@t)

  cmd_type: () ->
    return If

  print_c: (writer) ->
    writer.print( "if (" )
    @condition.print_c( writer )
    writer.println( ")" )
    writer.println( "{" ).indent(2)
    @statements.print_c( writer )
    writer.indent(-2).println( "}" )

    if (@elseIf_conditions)
      for c,i in @elseIf_conditions
        writer.print( "else if (" )
        c.print_c( writer )
        writer.println( ")" )
        writer.println( "{" ).indent(2)
        @elseIf_statements[i].print_c( writer )
        writer.indent(-2).println( "}" )

    if (@else_statements)
      writer.println( "else" )
      writer.println( "{" ).indent(2)
      @else_statements.print_c( writer )
      writer.indent(-2).println( "}" )

  resolve: () ->
    @condition = @condition.resolve()
    @statements.resolve()

    if (@elseIf_conditions)
      for c,i in @elseIf_conditions
        c = c.resolve()
        c = c.require_logical()
        @elseIf_conditions[i] = c
        @elseIf_statements[i].resolve()
    
    if (@else_statements)
      @else_statements.resolve()

    return this

  toString: () ->
    result = "if (" + @condition + ")\n"
    result += statements_to_string( @statements ).indent(2)

    if (@elseIf_conditions)
      for i of @elseIf_conditions
        result += "elseIf (" + @elseIf_conditions[i] + ")\n"
        result += statements_to_string( @elseIf_statements[i] ).indent(2)

    if (@else_statements)
      result += "else\n"
      result += statements_to_string( @else_statements ).indent(2)
    result += "endIf"
    return result

class While extends ControlStructure
  constructor: (@t,@condition) ->
    @statements = new StatementList(@t)

  cmd_type: () ->
    return While

  print_c: (writer) ->
    writer.print( "while (" )
    @condition.print_c( writer )
    writer.println( ")" )
    writer.println( "{" ).indent(2)
    @statements.print_c( writer )
    writer.indent(-2).println( "}" )

  resolve: () ->
    @condition = @condition.resolve()
    type = @condition.require_type()
    if (type != Program.type_Logical)
      throw @t.error "A Logical true/false value is required."

    @statements.resolve()
    return this


  toString: () ->
    result = "while (" + @condition + ")\n"
    #result += @statements.toString().indent(2)

    result += "endWhile"
    return result

class ForEachInRange extends ControlStructure
  constructor: (@t,@var_t,@var_name,@first,@last) ->
    @statements = new StatementList(@t)

  cmd_type: () ->
    return While

  print_c: (writer) ->
    writer.println( "{" ).indent(2)

    @control_var.print_c_declarations( writer )

    writer.print( "for (" )
    @control_var.print_c( writer )
    writer.print( "; " ).print( @var_name )
    writer.print( " <= " )
    @last.print_c( writer )
    writer.print("; ++").print(@var_name).println( ")" ).println("{").indent(2)
    @statements.print_c( writer )
    writer.indent(-2).println( "}" )

    writer.indent(-2).println( "}" )


  resolve: () ->
    if (@control_var) then return this # already resolved

    @first = @first.resolve()
    @last  = @last.resolve()
    @element_type = @find_common_type( @first.require_type(), @last.require_type() )
    @first = @first.cast_to( @element_type )
    @last  = @last.cast_to( @element_type )

    @control_var = new LocalDeclaration( @var_t, @var_name, @element_type, @first ).resolve()
    # leaves an entry on the local stack

    @statements.resolve()

    Program.local_stack.pop() # remove control_var

    return this


  toString: () ->
    result = "forEach (" + @var_name + " in " + @first + ".." + @last + ")\n"
    result += @statements.toString().indent(2)
    result += "endForEach"
    return result


#==============================================================================
# EXPORTS
#=============================================================================
module.exports.statements_to_string = statements_to_string
module.exports.Access            = Access
module.exports.Add               = Add
module.exports.AliasCall         = AliasCall
module.exports.Assign            = Assign
module.exports.Binary            = Binary
module.exports.Cmd               = Cmd
module.exports.Compare           = Compare
module.exports.CompareEQ         = CompareEQ
module.exports.CompareGE         = CompareGE
module.exports.CompareGT         = CompareGT
module.exports.CompareLE         = CompareLE
module.exports.CompareLT         = CompareLT
module.exports.CompareNE         = CompareNE
module.exports.ControlStructure  = ControlStructure
module.exports.ConvertToType     = ConvertToType
module.exports.Divide            = Divide
module.exports.ForEachInRange    = ForEachInRange
module.exports.GlobalDeclaration = GlobalDeclaration
module.exports.If                = If
module.exports.IntegerDivide     = IntegerDivide
module.exports.Literal           = Literal
module.exports.LiteralCharacter  = LiteralCharacter
module.exports.LiteralInteger    = LiteralInteger
module.exports.LiteralLogical    = LiteralLogical
module.exports.LiteralString     = LiteralString
module.exports.LiteralNull       = LiteralNull
module.exports.LiteralReal       = LiteralReal
module.exports.LocalDeclaration  = LocalDeclaration
module.exports.LocalRead         = LocalRead
module.exports.LocalWrite        = LocalWrite
module.exports.Logicalize        = Logicalize
module.exports.Multiply          = Multiply
module.exports.Negate            = Negate
module.exports.PostDecrement     = PostDecrement
module.exports.PostIncrement     = PostIncrement
module.exports.PreDecrement      = PreDecrement
module.exports.PreIncrement      = PreIncrement
module.exports.Return            = Return
module.exports.ReinterpretAsType = ReinterpretAsType
module.exports.StatementList     = StatementList
module.exports.Subtract          = Subtract
module.exports.This              = This
module.exports.Unary             = Unary
module.exports.UnaryWithModify   = UnaryWithModify
module.exports.While             = While

