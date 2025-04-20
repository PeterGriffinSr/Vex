project('vex', ['c', 'cpp'],
    version: '0.1.0',
    default_options: [
        'buildtype=debug',
        'warning_level=3',
        'c_std=c17',
    ],
    license: 'MIT'
)

cc = meson.get_compiler('c')

add_project_arguments(
  '-Wshadow',
  '-Wundef',
  '-Wwrite-strings',
  '-Wredundant-decls',
  '-Wmissing-declarations',
  '-Wconversion',
  '-Wfatal-errors',
  '-pedantic',
  '-Wvla',
  '-Wstrict-prototypes',
  '-D_XOPEN_SOURCE=700',
  language: 'c'
)

flex = find_program('flex', required: true)
bison = find_program('bison', required: true)

parser_c = custom_target('parser.c',
  input: 'src/parser/parser.y',
  output: ['parser.c', 'parser.h'],
  command: [bison, '-d', '-o', '@OUTPUT0@', '@INPUT@'],
  depend_files: ['src/parser/parser.y']
)

lexer_c = custom_target('lexer.c',
  input: 'src/parser/lexer.l',
  output: 'lex.yy.c',
  command: [flex, '-o', '@OUTPUT@', '@INPUT@'],
  depend_files: ['src/parser/lexer.l']
)

llvm = dependency('llvm',
  version: '>= 19.0.0',
  method: 'config-tool',
  native: true,
  required: true,
  modules: ['core', 'executionengine', 'orcjit', 'native']
)

srcs = [
  lexer_c,
  parser_c,
  'src/ast/ast.c',
  'src/typechecker/tc.c',
  'src/repl/repl.c',
  'src/repl/eval.c',
  'src/llvm/llvm.c',
  'src/core/memory.c',
  'src/core/error.c',
  'src/core/common.c',
  'src/main.c',
]

executable('vex',
  srcs,
  include_directories: include_directories('src/include'),
  dependencies: [llvm],
  install: true
)