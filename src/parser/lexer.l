%{
#include "parser.h"
#include "memory.h"
#include "error.h"
int yycolumn = 1;
const char *filename;
extern Arena *global_arena;
%}

%option noinput nounput

Digit           [0-9]
Letter          [a-zA-Z_]
Identifier      {Letter}({Letter}|{Digit})*
IntLiteral      {Digit}+
FloatLiteral    ({Digit}+"."{Digit}*|{Digit}*"."{Digit}+)
CharLiteral     \'(\\.|[^\\'])\'
StringLiteral   \"(\\.|[^\\"])*\"

%%

"#".*           { /* Ignore single-line comments */ }

"("             { yycolumn += yyleng; return LParen; }
")"             { yycolumn += yyleng; return RParen; }
"["             { yycolumn += yyleng; return LBracket; }
"]"             { yycolumn += yyleng; return RBracket; }
"{"             { yycolumn += yyleng; return LBrace; }
"}"             { yycolumn += yyleng; return RBrace; }
"+"             { yycolumn += yyleng; return Plus; }
"-"             { yycolumn += yyleng; return Minus; }
"*"             { yycolumn += yyleng; return Star; }
"/"             { yycolumn += yyleng; return Slash; }
"="             { yycolumn += yyleng; return Assignment; }
","             { yycolumn += yyleng; return Comma; }
"."             { yycolumn += yyleng; return Dot; }
"_"             { yycolumn += yyleng; return Underscore; }
"|"             { yycolumn += yyleng; return Pipe; }
"<"             { yycolumn += yyleng; return Less; }
">"             { yycolumn += yyleng; return Greater; }
":"             { yycolumn += yyleng; return Colon; }
";"             { yycolumn += yyleng; return Semi; }

"=="            { yycolumn += yyleng; return Equal; }
"!="            { yycolumn += yyleng; return NotEqual; }
"<="            { yycolumn += yyleng; return LessEqual; }
">="            { yycolumn += yyleng; return GreaterEqual; }
"||"            { yycolumn += yyleng; return LogicalOr; }
"&&"            { yycolumn += yyleng; return LogicalAnd; }
"=>"            { yycolumn += yyleng; return ThiccArrow; }
"->"            { yycolumn += yyleng; return SkinnyArrow; }
"+."            { yycolumn += yyleng; return PlusFloat; }
"-."            { yycolumn += yyleng; return MinusFloat; }
"*."            { yycolumn += yyleng; return StarFloat; }
"/."            { yycolumn += yyleng; return SlashFloat; }

"..."           { yycolumn += yyleng; return Spread; }

"val"           { yycolumn += yyleng; return Val; }
"type"          { yycolumn += yyleng; return Type; }
"match"         { yycolumn += yyleng; return Match; }
"with"          { yycolumn += yyleng; return With; }
"if"            { yycolumn += yyleng; return If; }
"else"          { yycolumn += yyleng; return Else; }
"then"          { yycolumn += yyleng; return Then; }
"fn"            { yycolumn += yyleng; return Fn; }
"None"          { yycolumn += yyleng; return None; }
"Some"          { yycolumn += yyleng; return Some; }
"Ok"            { yycolumn += yyleng; return Ok; }
"Error"         { yycolumn += yyleng; return Error; }
"not"           { yycolumn += yyleng; return Not; }
"int"           { yycolumn += yyleng; return Int; }
"char"          { yycolumn += yyleng; return Char; }
"bool"          { yycolumn += yyleng; return Bool; }
"float"         { yycolumn += yyleng; return Float; }
"string"        { yycolumn += yyleng; return String; }
"true"          { yycolumn += yyleng; yylval.boolval = 1; return BoolLit; }
"false"         { yycolumn += yyleng; yylval.boolval = 0; return BoolLit; }
"list"          { yycolumn += yyleng; return List; }

"print"         { yycolumn += yyleng; return Print; }
"map"           { yycolumn += yyleng; return Map; }
"filter"        { yycolumn += yyleng; return Filter; }

{CharLiteral}   { yycolumn += yyleng; yylval.charval = yytext[1]; return CharLit; }
{StringLiteral} { yycolumn += yyleng; int len = yyleng; char *stripped = arena_alloc(global_arena, (size_t)len - 1); memcpy(stripped, yytext + 1, (size_t)len - 2); stripped[len - 2] = '\0'; yylval.strval = stripped; return StringLit; }
{IntLiteral}    { yycolumn += yyleng; yylval.intval = atoi(yytext); return IntLit; }
{FloatLiteral}  { yycolumn += yyleng; yylval.floatval = strtof(yytext, NULL); return FloatLit; }
{Identifier}    { yycolumn += yyleng; char *copy = arena_alloc(global_arena, (size_t)yyleng + 1); memcpy(copy, yytext, (size_t)yyleng + 1); yylval.strval = copy; return Ident; }

[ \t\r]+        { yycolumn += yyleng; }
\n              { yycolumn = 1; yylineno++; }
\'[^']*$        { report_error("Unterminated char literal"); }
\"[^\"\n]*$     { report_error("Unterminated string literal"); }
.               { report_error("Unknown Character"); }

%%

int yywrap(void) { return 1; }