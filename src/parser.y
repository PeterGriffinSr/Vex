%{
#include <stdio.h>
#include <string.h>
#include "ast.h"
#include "memory.h"

extern FILE *yyin;
extern ASTNode *root;
extern Arena *global_arena;

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
    struct NodeList {
        struct ASTNode **elements;
        int count;
    } node_list;
}

%token <intval> IntLit
%token <floatval> FloatLit
%token <charval> CharLit
%token <strval> StringLit
%token <boolval> BoolLit
%token <strval> Ident

%left LogicalOr
%left LogicalAnd
%left Equal NotEqual
%left Less Greater LessEqual GreaterEqual
%left Plus Minus
%left Star Slash
%left PlusFloat MinusFloat
%left StarFloat SlashFloat
%right Not

%token LParen RParen LBracket RBracket LBrace RBrace Plus Minus Star Slash Assignment Comma Dot Underscore Pipe Less Greater Colon Semi
%token Equal NotEqual LessEqual GreaterEqual ThiccArrow SkinnyArrow Spread PlusFloat MinusFloat StarFloat SlashFloat LogicalAnd LogicalOr 
%token Let Type Match With If Else Rec None Some Ok Error Then Not
%token Int Float Char String Bool
%token Print Map Filter

%type <node> expr var_decl
%type <node_list> expr_list
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
  | expr PlusFloat expr { $$ = create_binary_node("+.", $1, $3); }
  | expr MinusFloat expr { $$ = create_binary_node("-.", $1, $3); }
  | expr StarFloat expr { $$ = create_binary_node("*.", $1, $3); }
  | expr SlashFloat expr { $$ = create_binary_node("/.", $1, $3); }
  | expr Less expr { $$ = create_binary_node("<", $1, $3); }
  | expr Greater expr { $$ = create_binary_node(">", $1, $3); }
  | expr Equal expr { $$ = create_binary_node("==", $1, $3); }
  | expr NotEqual expr { $$ = create_binary_node("!=", $1, $3); }
  | expr LessEqual expr { $$ = create_binary_node("<=", $1, $3); }
  | expr GreaterEqual expr { $$ = create_binary_node(">=", $1, $3); }
  | expr LogicalAnd expr { $$ = create_binary_node("&&", $1, $3); }
  | expr LogicalOr expr { $$ = create_binary_node("||", $1, $3); }
  | Minus expr { $$ = create_unary_node("-", $2); }
  | Not expr { $$ = create_unary_node("not", $2); }
  | IntLit { $$ = create_int_node($1); }
  | FloatLit { $$ = create_float_node($1); }
  | CharLit { $$ = create_char_node($1); }
  | StringLit { $$ = create_string_node($1); }
  | Ident { $$ = create_identifier_node($1); }
  | BoolLit { $$ = create_bool_node($1); }
  | LParen expr RParen { $$ = $2; }
  | LBracket expr_list RBracket { $$ = build_list($2.elements, $2.count); }

expr_list:
    expr { ASTNode **arr = arena_alloc(global_arena, sizeof(ASTNode *) * 1); arr[0] = $1; $$.elements = arr; $$.count = 1; }
    | expr_list Comma expr { size_t new_count = (size_t)$1.count + 1; ASTNode **arr = arena_alloc(global_arena, sizeof(ASTNode *) * new_count); memcpy(arr, $1.elements, sizeof(ASTNode *) * (size_t)$1.count); arr[$1.count] = $3; $$.elements = arr; $$.count = (int)new_count; }

var_decl:
    Let Ident Assignment expr { $$ = create_var_decl_node($2, NULL, $4); }
    | Let Ident Colon type Assignment expr { $$ = create_var_decl_node($2, $4, $6); }

%%
