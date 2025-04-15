project('vex', 'c',
    version: '0.1.0',
    default_options: [
        'buildtype=debug',
        'warning_level=3',
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
  language: 'c'
)

flex = find_program('flex', required: true)
bison = find_program('bison', required: true)

parser_c = custom_target('parser.c',
  input: 'src/parser.y',
  output: ['parser.c', 'parser.h'],
  command: [bison, '-d', '-Wcounterexamples', '-o', '@OUTPUT0@', '@INPUT@'],
  depend_files: ['src/parser.y']
)

lexer_c = custom_target('lexer.c',
  input: 'src/lexer.l',
  output: 'lex.yy.c',
  command: [flex, '-o', '@OUTPUT@', '@INPUT@'],
  depend_files: ['src/lexer.l']
)

srcs = [
  'src/ast.c',
  'src/nim.c',
  parser_c,
  lexer_c
]

executable('vex',
  srcs,
  include_directories: include_directories('src/include'),
  install: true
)