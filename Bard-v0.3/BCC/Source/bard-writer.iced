#==============================================================================
# bard-writer.iced
#
# Created 2014.08.23 by Abe Pralle
#==============================================================================
fs = require("fs")

# writeFileSync filename, data, [options]
class Writer
  indentation: 0
  at_line_start: true

  constructor: (@filepath) ->
    @buffer = ""

  indent: (spaces) ->
    @indentation += spaces
    return this

  close: () ->
    if (@filepath)
      fs.writeFileSync( @filepath, @buffer, "utf8" )

  print: (value) ->
    if (value == "\n")
      @write(10)
      return this

    if (@at_line_start and @indentation)
      i = @indentation
      while (--i >= 0)
        @write(32)
      @at_line_start = false

    if (value.indexOf)
      # Printing a string; separate by EOL to allow indentation mechanism to work
      ending_i = 0
      i = value.indexOf('\n')
      while (i != -1)
        @buffer += value.substr(0,i)
        @write(10)
        ending_i = i + 1
        i = value.indexOf('\n',i+1)

      if (ending_i < value.length)
        @buffer += value.substr(ending_i)

      return this

    else
      @buffer += value
      return this

  println: (value) ->
    if (value?) then @print( value )
    return @write(10)

  set_indent: (@indentation) ->
    return this

  write: (byte) ->
    byte &= 255
    @buffer += String.fromCharCode( byte )
    @at_line_start = (byte == 10)
    return this

  toString: () ->
    return @buffer

#==============================================================================
# EXPORTS
#==============================================================================
module.exports.Writer = Writer
