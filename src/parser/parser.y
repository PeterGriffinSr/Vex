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
    struct NodeList { struct ASTNode **elements; int count; } node_list;
    struct ParamList { struct Param *elements; int count; } param_list;
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
%nonassoc Else

%token LParen RParen LBracket RBracket LBrace RBrace Plus Minus Star Slash Assignment Comma Dot Underscore Pipe Less Greater Colon Semi
%token Equal NotEqual LessEqual GreaterEqual ThiccArrow SkinnyArrow Spread PlusFloat MinusFloat StarFloat SlashFloat LogicalAnd LogicalOr 
%token Let Type Match With If Else Rec None Some Ok Error Then Not
%token Int Float Char String Bool
%token Print Map Filter

%type <node> statement expr var_decl func_def primary_expr
%type <node_list> statement_list expr_list
%type <param_list> param_list
%type <strval> type

%%

program:
    statement_list { root = create_block_node($1.elements, $1.count); }

statement_list:
    statement { ASTNode **arr = arena_alloc(global_arena, sizeof(ASTNode *) * 1); arr[0] = $1; $$.elements = arr; $$.count = 1; }
    | statement_list statement { size_t new_count = (size_t)$1.count + 1; ASTNode **arr = arena_alloc(global_arena, sizeof(ASTNode *) * new_count); memcpy(arr, $1.elements, sizeof(ASTNode *) * (size_t)$1.count); arr[$1.count] = $2; $$.elements = arr; $$.count = (int)new_count; }

statement:
    expr Semi { $$ = $1; }
    | var_decl Semi { $$ = $1; }
    | func_def Semi { $$ = $1; }

type:
    Int { $$ = "int"; }
    | Float { $$ = "float"; }
    | Char { $$ = "char"; }
    | String { $$ = "string"; }
    | Bool { $$ = "bool"; }
    | LParen type RParen SkinnyArrow type { $$ = (const char *)create_higher_order_node($2, $5); }

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
  | primary_expr { $$ = $1; }

primary_expr:
    IntLit { $$ = create_int_node($1); }
  | FloatLit { $$ = create_float_node($1); }
  | CharLit { $$ = create_char_node($1); }
  | StringLit { $$ = create_string_node($1); }
  | Ident { $$ = create_identifier_node($1); }
  | BoolLit { $$ = create_bool_node($1); }
  | LParen expr RParen { $$ = $2; }
  | LBracket expr_list RBracket { $$ = build_list($2.elements, $2.count); }
  | If expr Then expr Else expr { $$ = create_if_stmt_node($2, $4, $6); }
  | Print Less type Greater expr { $$ = create_print_node($5, $3); }
  | primary_expr LParen expr_list RParen { $$ = create_call_node($1, $3.elements, $3.count); }
  | primary_expr LParen RParen { $$ = create_call_node($1, NULL, 0); }

expr_list:
    expr { ASTNode **arr = arena_alloc(global_arena, sizeof(ASTNode *) * 1); arr[0] = $1; $$.elements = arr; $$.count = 1; }
    | expr_list Comma expr { size_t new_count = (size_t)$1.count + 1; ASTNode **arr = arena_alloc(global_arena, sizeof(ASTNode *) * new_count); memcpy(arr, $1.elements, sizeof(ASTNode *) * (size_t)$1.count); arr[$1.count] = $3; $$.elements = arr; $$.count = (int)new_count; }

param_list:
    Ident { struct Param *arr = arena_alloc(global_arena, sizeof(struct Param)); arr[0].name = $1; arr[0].type = NULL; $$.elements = arr; $$.count = 1; }
  | Ident Colon type { struct Param *arr = arena_alloc(global_arena, sizeof(struct Param)); arr[0].name = $1; arr[0].type = $3; $$.elements = arr; $$.count = 1; }
  | param_list Comma Ident { size_t new_count = (size_t)$1.count + 1; struct Param *arr = arena_alloc(global_arena, sizeof(struct Param) * new_count); memcpy(arr, $1.elements, sizeof(struct Param) * (size_t)$1.count); arr[$1.count].name = $3; arr[$1.count].type = NULL; $$.elements = arr; $$.count = (int)new_count; }
  | param_list Comma Ident Colon type { size_t new_count = (size_t)$1.count + 1; struct Param *arr = arena_alloc(global_arena, sizeof(struct Param) * new_count); memcpy(arr, $1.elements, sizeof(struct Param) * (size_t)$1.count); arr[$1.count].name = $3; arr[$1.count].type = $5; $$.elements = arr; $$.count = (int)new_count; }

var_decl:
    Let Ident Assignment expr { $$ = create_var_decl_node($2, NULL, $4); }
    | Let Ident Colon type Assignment expr { $$ = create_var_decl_node($2, $4, $6); }

func_def:
    Let Ident LParen param_list RParen Assignment expr { $$ = create_function_node($2, $4.elements, $4.count, 0, $7); }
  | Let Ident LParen RParen Assignment expr { $$ = create_function_node($2, NULL, 0, 0, $6); }
  | Let Rec Ident LParen param_list RParen Assignment expr { $$ = create_function_node($3, $5.elements, $5.count, 1, $8); }
  | Let Rec Ident LParen RParen Assignment expr { $$ = create_function_node($3, NULL, 0, 1, $7); }

%%
