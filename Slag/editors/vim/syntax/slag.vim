" Vim syntax file
" Language:     Slag
" Maintainer:   Abe Pralle <Abe.Pralle@plasmaworks.com>
" URL:    
" Last Change:  2010.06.03

" Windows: place in c:\Program Files\vim\vimfiles\syntax\
" MacVim:  place in ~/.vim/syntax/

" Quit when a syntax file was already loaded
if !exists("main_syntax")
  if exists("b:current_syntax")
    finish
  endif
  " we define it here so that included files can test for it
  let main_syntax='slag'
endif

" keyword definitions
syn keyword slagClassDecl   class endClass aspect endAspect compound endCompound enum endEnum
syn keyword slagClassDecl   with endWith
syn keyword slagClassDecl   augment endAugment framework endFramework
syn keyword slagMember      CLASS_PROPERTIES CLASS_VARS CLASS_METHODS PROPERTIES VARS METHODS
syn keyword slagMember      BASE_TYPES CATEGORIES ENUMERATE SINGLETONS
syn match   slagError       "\<for\(\s\|(\)"
syn keyword slagConditional  if elseIf else endIf
syn keyword slagConditional  which whichIs case caseNext others endWhich endWhichIs
syn keyword slagConditional  contingent endContingent satisfied unsatisfied
syn keyword slagConditional  try catch endTry
syn keyword slagConditional  block endBlock
syn keyword slagLoop         while endWhile forEach endForEach 
syn keyword slagLoop         loop endLoop
syn keyword slagBranch       escapeForEach escapeWhile escapeLoop
syn keyword slagBranch       escapeWhich escapeWhichIs escapeIf escapeContingent escapeBlock
syn keyword slagBranch       nextIteration
syn keyword slagBoolean      true false
syn keyword slagConstant     null void gt lt eq infinity NaN
syn keyword slagTypedef      this prior
syn keyword slagStatement    return breakpoint necessary sufficient removeCurrent noAction
syn keyword slagSpecial      breakpoint insertUnderlying
syn keyword slagType         Int64 Int32 Byte Char Real64 Real32 Logical null var
syn keyword slagStorageClass overlaying underlying
syn keyword slagScopeDecl    readOnly writeOnly public private limited const local delegate
syn keyword slagScopeDecl    deferred singleton managed
syn keyword slagStorageClass native requisite abstract final propagated runtime
syn keyword slagExceptions   throw try catch finally
syn keyword slagPreProc      useFramework
"syn match  slaggPreProc      "^\[.*]"
syn region  slagPreProc      start="^\[" end="[\n\]]"
syn match   slagNumber       "\<\d\+\(\.\d\+\)\=\>"
syn match   slagNumber       "\<0b[01]\+\(\.[01]\+\)\=\>"
syn match   slagNumber       "\<0x\x\+\(\.\x\+\)\=\>"

"syn match   slagIdentifier  "\h\w*"
"syn match   slagUserLabelRef  ".\k\+"

syn keyword slagStatement    require ensure assert
syn keyword slagOperator     and or xor not
syn keyword slagOperator     instanceOf is isNot notInstanceOf duplicate
"syn match   slagOperator     "\.\.\<"
"syn match   slagOperator     "\.\.\>"
"syn match   slagOperator     "\.\."
"syn keyword slagOperator     == != <= >= 
"syn keyword slagOperator     + - * / = ^ %
"syn match   slagOperator    "+"
"syn match   slagOperator    "-"
"syn match   slagOperator    "\*"
"syn match   slagOperator    "/"
"syn match   slagOperator    "="
"syn match   slagOperator    "^"
"syn match   slagOperator    "%"
"syn match   slagType    ":"
"syn match   slagType    ";"
"syn match   slagOperator     "<[^<]"me=e-1
"syn match   slagOperator     ">[^>]"me=e-1

syn keyword slagFuncDef  method multimethod

syn match   slagLineComment  "#.*" 
syn region  slagLineComment  start="#{" end="}#"
syn region  slagString       start=+"+ skip=+\\\\\|\\"+ end=+"+
syn region  slagString       start=+\/\/+ skip=+\\\/+ end=+\/\/+
syn match   slagString       "'.'"
syn match   slagString       "'\\.'"

hi def link slagIdentifier  Identifier
hi def link slagFuncDef        Function
hi def link slagMember      Special
hi def link slagVarArg         Function
hi def link slagBraces      Function
hi def link slagBranch      Conditional
hi def link slagUserLabelRef    slagUserLabel
hi def link slagLabel      Label
hi def link slagUserLabel    Label
hi def link slagConditional    Conditional
hi def link slagLoop      Repeat
hi def link slagExceptions    Exception
hi def link slagStorageClass    StorageClass
hi def link slagMethodDecl    slagStorageClass
hi def link slagClassDecl    slagStorageClass
hi def link slagScopeDecl    slagStorageClass
hi def link slagBoolean    Boolean
hi def link slagSpecial    Special
hi def link slagSpecialError    Error
hi def link slagSpecialCharError  Error
hi def link slagString      String
hi def link slagCharacter    Character
hi def link slagSpecialChar    SpecialChar
hi def link slagNumber      Number
hi def link slagError      Error
hi def link slagStringError    Error
hi def link slagStatement    Statement
"hi def link slagOperator    Operator
hi def link slagComment    Comment
hi def link slagDocComment    Comment
hi def link slagLineComment    Comment
hi def link slagConstant    Constant
hi def link slagTypedef    Typedef
hi def link slagTodo      Todo
hi def link slagPreProc                PreProc

hi def link slagCommentTitle    SpecialComment
hi def link slagDocTags    Special
hi def link slagDocParam    Function
hi def link slagDocSeeTagParam    Function
hi def link slagCommentStar    slagComment

hi def link slagType      Type
hi def link slagExternal    Include

hi def link htmlComment    Special
hi def link htmlCommentPart    Special
hi def link slagSpaceError    Error

let b:current_syntax = "slag"

if main_syntax == 'slag'
  unlet main_syntax
endif

let b:spell_options="contained"

syn sync fromstart

