require("./bard-string-augment")
ParserModule = require("./bard-parser")

try
  parser = new ParserModule.Parser()
  program = parser.parse( "Test.bard" )
  program.resolve()
  program.print_c( "output.c" )

catch err
  console.log err.toString()
  process.exit(1)

