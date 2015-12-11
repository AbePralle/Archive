Writer = require( "./bard-writer" ).Writer

#==============================================================================
# Attributes
#==============================================================================
Attributes =
  flag_format_mask:  7
  flag_primitive:    0
  flag_native_data:  1
  flag_compound:     2
  flag_class:        3

  flag_defined:      8

#==============================================================================
# CodeWriter
# CWriter
#==============================================================================
class CodeWriter extends Writer
  constructor: (filepath,@name) ->
    super( filepath )

  print_type: (type) ->
    if (type)
      if (type.is_reference())
        return @print("struct ").print(type.c_name).print("*")
      else
        return @print type.c_name
    else
      return @print "void"

  visit: (cmd) ->
    if (not cmd) then return null

    if (cmd[@name])
      result = cmd[@name](this)
      return result
    else
      throw cmd.t.error "[INTERNAL] #{@name}() undefined."


class CWriter extends CodeWriter
  constructor: (filepath,name="print_c") ->
    super( filepath, name )


#==============================================================================
# Template
#==============================================================================
class Template
  # Note: @tokens is assigned after Template is created.

  constructor: (@t,@name,@attributes) ->

  instantiate: (type_info) ->
    Parser = require("./bard-parser").Parser
    parser = new Parser( @tokens )

    type_info.attributes = @attributes
    parser.parse_type_def( type_info )

    type_info.attributes |= Attributes.flag_defined

  is_class: () ->
    return !!(@attributes & Attributes.flag_class)


#==============================================================================
# Type
#==============================================================================
class Type
  constructor: (@t,@name,@attributes) ->
    @base_types = []
    @property_list = []
    @method_list = []
    @methods_by_name = {}  # table of lists
    @methods_by_signature = {} # one to one

  assign_c_name: () ->
    @c_name = @name

    for p in @property_list
      p.c_name = p.name

    for m in @method_list
      m.assign_c_name()

    return this

  compatible_with: (ancestor_type) ->
    if (@instance_of(ancestor_type)) then return true

    if (@is_native())
      if (this == Program.type_String)
        if (ancestor_type.name == "[Integer8]")
          return true

    return false

  instance_of: (ancestor_type) ->
    if (this == ancestor_type) then return true

    for base_type in @base_types
      if (base_type.instance_of(ancestor_type)) then return true

    return false

  is_class: () ->
    return (@attributes & Attributes.flag_format_mask) == Attributes.flag_class

  is_compound: () ->
    return (@attributes & Attributes.flag_format_mask) == Attributes.flag_compound

  is_native: () ->
    switch (@attributes & Attributes.flag_format_mask)
      when Attributes.flag_primitive
        return true
      when Attributes.flag_native_data
        return true
      else
        return false

  is_reference: () ->
    switch (@attributes & Attributes.flag_format_mask)
      when Attributes.flag_class
        return true
      when Attributes.flag_native_data
        return true
      else
        return false

  is_value_type: () ->
    return !@is_reference()

  organize: () ->
    # organize() recursively organizes all referenced types and prepares
    # method signatures.
    if (@organized) then return
    @organized = true

    if (not (@attributes & Attributes.flag_defined))
      base_name = @name.before_first( '<' )
  
      template = Program.templates[base_name]

      if (not template)
        throw @t.error "Reference to undefined type '#{@name}'."
  
      template.instantiate(this)

    for m in @method_list
      m.organize()

  print_c_header: (writer) ->
    if (@is_native()) then return
    writer.print( "//  " ).println( @name )
    writer.print( "struct " ).println( @name )
    writer.println( "{" )
    writer.indent( 2)
    for p in @property_list
      writer.print_type( p.type ).print( " " ).print( p.c_name ).println(";")
    writer.indent(-2)
    writer.println( "};" )
    writer.println()

  print_c: (writer) ->
    if (@is_native()) then return
    writer.print( "//  " ).println( @name )

    for m in @method_list
      m.print_c( writer )

  resolve: () ->
    # resolve() recursively resolves all referenced types and analyzes method
    # bodies.
    if (@resolved) then return
    @resolved = true

    Program.push_type_context( this )
    console.log "Resolving " + @name

    @organize()

    for m in @method_list
      m.resolve()

    Program.pop_context()

  toString: () ->
    return @name


#==============================================================================
# Variable
#==============================================================================
class Variable
  constructor: (@t,@name,@type) ->


#==============================================================================
# Local
#==============================================================================
class Local extends Variable


#==============================================================================
# Method
#==============================================================================
class Method
  constructor: (@t,@this_type,@name) ->
    @statements = new Cmd.StatementList(@t)
    @parameter_list = []
    @local_list = []

  assign_c_name: () ->
    @c_name = @this_type.c_name + "__" + @name
    return this

  create_call: (t,context,args) ->
    if (not context) then context = Program.this_type

    if (args)
      arg  # TODO: fill in missing default args
      for arg,i in args
        arg  # TODO: cast each arg to parameter type
    else
      args = []

    if (@is_alias)
      if (@output_alias)
        return new Cmd.AliasCall( t, context, this, args )
      else
        if (@return_type)
          result = @statements.data[0].clone( @parameter_list, args ).resolve()
          return result
        else
          throw t.error "TODO: alias without return value"

    throw t.error "TODO: create_call"

  create_signature: () ->
    if (@signature) then return

    sig = @name
    sig += "("

    first = true
    for p in @parameter_list
      if (first) then first = false
      else            sig += ","

      if (not p.type) then throw p.t.error "TODO: typeless parameter support in Method::create_signature"

      sig += p.type.name

    sig += ")"

    if (@return_type)
      sig += "->"
      sig += @return_type.name

    @signature = sig

  organize: () ->
    if (@organized) then return this
    @organized = true

    Program.push_method_context( this )

    @validate_parameters()

    @create_signature()

    @this_type.methods_by_name[@name] = @this_type.methods_by_name[@name] or []
    existing = @this_type.methods_by_signature[@signature]
    if (existing)
      throw t.error( "A method with signature '" + @signature + "' already exists in type " + @this_type.name + "." )
    @this_type.methods_by_name[@name].push( this )
    @this_type.methods_by_signature[@signature] = this

    Program.pop_context()

    return this

  print_c_header: (writer) ->
    if (@is_alias) then return

  print_c: (writer) ->
    if (@is_alias) then return
    writer.print_type( @return_type ).print( " " ).print( @c_name ).print( "( " )
    writer.print_type( @this_type ).print( " THIS" )
    writer.println( " )" )
    writer.println( "{" )
    writer.indent(2)
    @statements.print_c( writer )
    writer.indent(-2)
    writer.println( "}" )
    writer.println()

  resolve: () ->
    if (@is_alias) then return
    Program.push_method_context( this )
    @statements = @statements.resolve()
    Program.pop_context()
    return this

  toString: () ->
    return @signature or @name

  validate_parameters: () ->
    @min_arguments = @parameter_list.length
    @max_arguments = @parameter_list.length


#==============================================================================
# Program
#==============================================================================
Program =
  reset: () ->
    @templates = {}
    @template_list = []

    @types = {}
    @type_list = []

    @main_class_info = null

    @this_type = null
    @this_method = null
    @type_context_stack = []
    @method_context_stack = []

    @local_stack = []


    Token = require("./bard-tokenizer").Token
    t = new Token( 0 )
    @type_Real          = @create_built_in_type( t, "Real", Attributes.flag_primitive )
    @type_Integer       = @create_built_in_type( t, "Integer", Attributes.flag_primitive )
    @type_Character     = @create_built_in_type( t, "Character", Attributes.flag_primitive )
    @type_Logical       = @create_built_in_type( t, "Logical", Attributes.flag_primitive )
    @type_Variant       = @create_built_in_type( t, "Variant", Attributes.flag_native_data )
    @type_String        = @create_built_in_type( t, "String", Attributes.flag_native_data )
    @type_LiteralString = @create_built_in_type( t, "LiteralString", Attributes.flag_native_data )
    @type_LiteralString.base_types.push( @type_String )

    @next_unique_id = 1

  create_built_in_type: (t,name,attributes) ->
    type = @get_type_reference( t, name )
    type.attributes = attributes | Attributes.flag_defined
    return type

  find_method: (t,type_context,context,name,args,force_error) ->
    if (context)
      context = context.resolve()

    if (not type_context)
      if (context)          then type_context = type_context.type()
      if (not type_context) then type_context = @this_type

    candidates = []
    methods_by_name = type_context.methods_by_name[name] or []

    nargs = 0
    arg_types = []
    if (args)
      nargs = args.length
      for arg,i in args
        arg = arg.resolve()
        args[i] = arg
        arg_types.push( arg.require_type() )

    for m in methods_by_name
      if (nargs >= m.min_arguments and nargs <= m.max_arguments)
        candidates.push( m )

    if (candidates.length == 1) then return candidates[0]

    m = @find_method_using_arg_filter( candidates, arg_types, @exact_arg_type_filter )
    if (m) then return m

    m = @find_method_using_arg_filter( candidates, arg_types, @instance_of_arg_type_filter )
    if (m) then return m

    m = @find_method_using_arg_filter( candidates, arg_types, @compatible_arg_type_filter )
    if (m) then return m

    if (not force_error) then return null

    if (candidates.length == 0)
      if (args and args.length)
        if (type_context)
          throw t.error "No such method " + name + "(" + args.length + ") in type " + type_context.name + "."
        else
          throw t.error "No such method " + name + "(" + args.length + ")."
      else
        if (type_context)
          throw t.error "No such property method '" + name + "' in type " + type_context.name + "."
        else
          throw t.error "No such property or method '" + name + "'."

    else
      candidate_list = ""
      for c in candidates
        candidate_list += c.signature + "\n"

      buffer = "Ambiguous call " + name + "("

      first = true
      for arg_type in arg_types
        if (first) then first = false
        else            buffer += ','
        buffer += arg_type.name

      buffer += ") may refer to one of the following methods:\n\n" + candidate_list.indent(2) + "\n"
      buffer += "Cast or convert arguments to match the parameter types of one of the above candidate methods."

      throw t.error buffer

  find_method_using_filter: (candidates,type_context,arg_types,filter_fn) ->
    matches = candidates.length

    i = matches
    while (--i >= 0)
      if (not filter_fn(candidates[i],type_context,arg_types))
        --matches
        temp = candidates[i]
        candidates[i] = candidates[matches]
        candidates[matches] = temp

    switch (matches)
      when 0
        # No culling performed when there are no matches at all.
        return null

      when 1
        return candidates[0]

      else
        # Two or more matches; cull the others that have been shifted
        # to the far end.
        while (candidates.length > matches)
          candidates.pop()

        return null  # Haven't narrowed it down to a single method yet

  exact_arg_type_filter: (arg_type,param_type) ->
    return (arg_type == param_type)

  find_local: (name) ->
    i = @local_stack.length
    while (--i >= 0)
      v = @local_stack[i]
      if (v.name == name)
        return v

    return null

  instance_of_arg_type_filter: (arg_type,param_type) ->
    return (arg_type.instance_of(param_type))

  compatible_arg_type_filter: (arg_type,param_type) ->
    return (arg_type.compatible_with(param_type))

  find_method_using_arg_filter: (candidates,arg_types,filter_fn) ->
    matches = candidates.length

    i = matches
    while (--i >= 0)
      m = candidates[i]
      for arg_type,a in arg_types
        param = m.parameter_list[a]
        if (not filter_fn(arg_type,param.type))
          --matches
          temp = candidates[i]
          candidates[i] = candidates[matches]
          candidates[matches] = temp
          break

    switch (matches)
      when 0
        # No culling performed when there are no matches at all.
        return null

      when 1
        return candidates[0]

      else
        # Two or more matches; cull the others that have been shifted
        # to the far end.
        while (candidates.length > matches)
          candidates.pop()

        return null  # Haven't narrowed it down to a single method yet


  push_method_context: (this_method) ->
    @type_context_stack.push( @this_type )
    @method_context_stack.push( @this_method )
    @this_type = this_method.this_type or null
    @this_method = this_method
    return this

  push_type_context: (this_type) ->
    @type_context_stack.push( @this_type )
    @method_context_stack.push( @this_method )
    @this_type = this_type
    @this_method = null
    return this

  pop_context: () ->
    @this_method = @method_context_stack.pop()
    @this_type = @type_context_stack.pop()
    return this

  resolve_call: (t,type_context,context,name,args) ->
    m = @find_method(t,type_context,context,name,args)

    if (not m)
      @find_method(t,type_context,context,name,args,true)  # Force an error

    return m.create_call( t, context, args )

  get_type_reference: (t,name) ->
    type = @types[name]
    if (type) then return type

    # Create a placeholder type for now - we'll create the full
    # type from a template after we've parsed everything.
    type = new Type( t, name, 0 )
    @types[name] = type
    @type_list.push( type )

    return type


  resolve: () ->
    if (Program.main_class_info)
      Program.main_class_info.type = Program.get_type_reference( Program.main_class_info.t, Program.main_class_info.name )

    i = 0
    while (i < @type_list.length)
      @type_list[i].resolve()
      ++i

  print_c: (filepath) ->
    for type in @type_list
      type.assign_c_name()

    @type_Real.c_name = "double"
    @type_Integer.c_name = "int"
    @type_Character.c_name = "char"
    @type_Logical.c_name = "int"
    @type_String.c_name = "string"

    writer = new CWriter( filepath, "print_c_header" )
    writer.println( "#include <stdio.h>" )

    for type in @type_list
      type.print_c_header( writer )

    writer.name = "print_c"

    for type in @type_list
      type.print_c( writer )

    writer.println( "int main()" )
    writer.println( "{" )
    writer.println( "  struct Test test;" )
    writer.println( "  Test__init( &test );" )
    writer.println( "  return 0;" )
    writer.println( "}" )

    writer.close()


#==============================================================================
# EXPORTS
#==============================================================================
module.exports.Attributes = Attributes
module.exports.Local      = Local
module.exports.Method     = Method
module.exports.Program    = Program
module.exports.Type       = Type
module.exports.Template   = Template
module.exports.Variable   = Variable

Cmd = require( "./bard-cmd" )

