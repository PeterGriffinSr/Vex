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
    struct { const char **elements; int count; } type_list;
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
%token Val Type Match With If Else None Some Ok Error Then Not Fn List
%token Int Float Char String Bool
%token Print Map Filter

%type <node> statement expr var_decl primary_expr func_def
%type <node_list> statement_list expr_list
%type <param_list> param_list
%type <type_list> type_list
%type <strval> type list_type

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
  | LBrace statement_list RBrace { $$ = create_block_node($2.elements, $2.count); }
  | primary_expr { $$ = $1; }

primary_expr:
    IntLit { $$ = create_int_node($1); }
  | FloatLit { $$ = create_float_node($1); }
  | CharLit { $$ = create_char_node($1); }
  | StringLit { $$ = create_string_node($1); }
  | Ident { $$ = create_identifier_node($1); }
  | BoolLit { $$ = create_bool_node($1); }
  | Print Less type Greater expr { $$ = create_print_node($5, $3); }
  | LParen expr RParen { $$ = $2; }
  | LBracket expr_list RBracket { $$ = build_list($2.elements, $2.count); }
  | Ident LParen expr_list RParen { $$ = create_call_node(create_identifier_node($1), $3.elements, $3.count); }
  | Ident LParen RParen { $$ = create_call_node(create_identifier_node($1), NULL, 0); }
  | LParen expr RParen LParen expr_list RParen { $$ = create_call_node($2, $5.elements, $5.count); }
  | LParen expr RParen LParen RParen { $$ = create_call_node($2, NULL, 0); }

expr_list:
    expr { ASTNode **arr = arena_alloc(global_arena, sizeof(ASTNode *) * 1); arr[0] = $1; $$.elements = arr; $$.count = 1; }
    | expr_list Comma expr { size_t new_count = (size_t)$1.count + 1; ASTNode **arr = arena_alloc(global_arena, sizeof(ASTNode *) * new_count); memcpy(arr, $1.elements, sizeof(ASTNode *) * (size_t)$1.count); arr[$1.count] = $3; $$.elements = arr; $$.count = (int)new_count; }

param_list:
    Ident { struct Param *arr = arena_alloc(global_arena, sizeof(struct Param)); arr[0].name = $1; arr[0].type = NULL; $$.elements = arr; $$.count = 1; }
  | param_list Comma Ident { size_t new_count = (size_t)$1.count + 1; struct Param *arr = arena_alloc(global_arena, sizeof(struct Param) * new_count); memcpy(arr, $1.elements, sizeof(struct Param) * (size_t)$1.count); arr[$1.count].name = $3; arr[$1.count].type = NULL; $$.elements = arr; $$.count = (int)new_count; }

type_list:
    type { const char **arr = arena_alloc(global_arena, sizeof(char*)); arr[0] = $1; $$.elements = arr; $$.count = 1; }
  | type_list Comma type { size_t new_count = (size_t)$1.count + 1; const char **arr = arena_alloc(global_arena, sizeof(char*) * new_count); memcpy(arr, $1.elements, sizeof(char*) * (size_t)$1.count); arr[$1.count] = $3; $$.elements = arr; $$.count = (int)new_count; }

list_type:
    List Less type Greater { char *buf = arena_alloc(global_arena, 32); snprintf(buf, 32, "<%s>", $3); $$ = buf; }

var_decl:
    Val type Colon Ident Assignment expr { $$ = create_var_decl_node($4, $2, $6); }
    | Val list_type Colon Ident Assignment expr { $$ = create_var_decl_node($4, $2, $6); }

func_def:
    Val LParen type_list RParen SkinnyArrow type Colon Ident Fn LParen param_list RParen ThiccArrow expr { for (int i = 0; i < $11.count; i++) { $11.elements[i].type = $3.elements[i]; } $$ = create_function_node($8, $11.elements, $11.count, $3.elements, $6, $14); }
    | Val LParen RParen SkinnyArrow type Colon Ident Fn LParen RParen ThiccArrow expr { $$ = create_function_node($7, NULL, 0, NULL, $5, $12); }

%%
