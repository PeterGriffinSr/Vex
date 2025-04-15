%{
#include <stdio.h>
#include "ast.h"

extern FILE *yyin;
extern ASTNode *root;

ASTNode *root = NULL;
int yylex(void);
void yyerror(const char *s);

void yyerror(const char *s) {
    fprintf(stderr, "Parse error: %s\n", s);
}
%}

%union {
    int intval;
    double floatval;
    const char* strval;
    char charval;
    int boolval;
    struct ASTNode* node;
}

%token <intval> IntLit
%token <floatval> FloatLit
%token <charval> CharLit
%token <strval> StringLit
%token <boolval> BoolLit
%token <strval> Ident

%left Less Greater LessEqual GreaterEqual
%left Equal NotEqual
%left Plus Minus 
%left Star Slash

%token LParen RParen LBracket RBracket LBrace RBrace Plus Minus Star Slash Assignment Comma Dot Underscore Pipe Less Greater Colon Semi
%token Equal NotEqual LessEqual GreaterEqual ThiccArrow SkinnyArrow Spread
%token Let Type Match With If Else Rec None Some Ok Error Then
%token Int Float Char String Bool
%token Print Map Filter

%type <node> expr var_decl
%type <strval> type

%%

program:
    expr Semi { root = $1; }
    | var_decl Semi { root = $1; }

type:
    Int { $$ = "int"; }
    | Float { $$ = "float"; }
    | Char { $$ = "char"; }
    | String { $$ = "string"; }
    | Bool { $$ = "bool"; }

expr:
    expr Plus expr { $$ = create_binary_node("+", $1, $3); }
  | expr Minus expr { $$ = create_binary_node("-", $1, $3); }
  | expr Star expr { $$ = create_binary_node("*", $1, $3); }
  | expr Slash expr { $$ = create_binary_node("/", $1, $3); }
  | expr Less expr { $$ = create_binary_node("<", $1, $3); }
  | expr Greater expr { $$ = create_binary_node(">", $1, $3); }
  | expr Equal expr { $$ = create_binary_node("==", $1, $3); }
  | expr NotEqual expr { $$ = create_binary_node("!=", $1, $3); }
  | expr LessEqual expr { $$ = create_binary_node("<=", $1, $3); }
  | expr GreaterEqual expr { $$ = create_binary_node(">=", $1, $3); }
  | IntLit { $$ = create_int_node($1); }
  | FloatLit { $$ = create_float_node($1); }
  | CharLit { $$ = create_char_node($1); }
  | StringLit { $$ = create_string_node($1); }
  | Ident { $$ = create_identifier_node($1); }
  | LParen expr RParen { $$ = $2; }

var_decl:
    Let Ident Assignment expr { $$ = create_var_decl_node($2, NULL, $4); }
    | Let Ident Colon type Assignment expr { $$ = create_var_decl_node($2, $4, $6); }

%%
