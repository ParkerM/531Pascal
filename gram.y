/*A Bison parser for the programming language Pascal.
  Copyright (C) 1989-2002 Free Software Foundation, Inc.

  Authors: Jukka Virtanen <jtv@hut.fi>
           Helsinki University of Technology
           Computing Centre
           Finland

           Peter Gerwinski <peter@gerwinski.de>
           Essen, Germany

           Bill Cox <bill@cygnus.com> (error recovery rules)

           Frank Heckenbach <frank@pascal.gnu.de>

  This file is part of GNU Pascal.

  GNU Pascal is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published
  by the Free Software Foundation; either version 1, or (at your
  option) any later version.

  GNU Pascal is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with GNU Pascal; see the file COPYING. If not, write to the
  Free Software Foundation, 59 Temple Place - Suite 330, Boston, MA
  02111-1307, USA. */

/* Bison parser for ISO 7185 Pascal originally written on
 * 3 Feb 1987 by Jukka Virtanen <jtv@hut.fi>
 *
 * Modified for use at the University of South Carolina's CSCE 531
 * (Compiler Construction) course (Spring 2005) by Stephen Fenner
 * <fenner@cse.sc.edu>
 * Simplified (Spring 2015) by Stephen Fenner
 *
 * SHIFT/REDUCE CONFLICTS
 *
 * The dangling else will not cause a shift/reduce conflict - it's
 * solved by precedence rules.
 */

%{

/* Cause the `yydebug' variable to be defined.  */
#define YYDEBUG 1

#include "tree.h"
#include "encode.h"
#include "expr.h"
#include "functions.h"
#include "types.h"

void set_yydebug(int);
void yyerror(char *);

/* Like YYERROR but do call yyerror */
#define YYERROR1 { yyerror ("syntax error"); YYERROR; }

%}

/* Start symbol for the grammar */

%start pascal_program

/* The union representing a semantic stack entry */
%union {
    char *          y_string;
    int	           y_cint;
    long            y_int;
    double          y_real;
    
    stid_item_p     y_stid_item;
    ST_ID           y_stid;
    
    typedef_item_p  y_typedef_item;
    TYPE_LIST	     y_type_list;
    TYPE            y_type;

    EXPR		        y_expr;
    EXPR_LIST 	     y_expr_list;
    DIRECTIVETYPE   y_dir;
    DIR_LIST        y_dir_list;
    PARAM_LIST 	  y_param_list;
    
    BOOLEAN			  y_boolean;
    control_labels  y_control;
    FOR_DIRECTION   y_for_dir;
    
    num_const_p     y_num_const;
}

%token <y_string> LEX_ID

/* Reserved words. */

/* Reserved words in Standard Pascal */
%token LEX_ARRAY LEX_BEGIN LEX_CASE LEX_CONST LEX_DO LEX_DOWNTO LEX_END
%token LEX_FILE LEX_FOR LEX_FUNCTION LEX_GOTO LEX_IF LEX_LABEL LEX_NIL
%token LEX_OF LEX_PACKED LEX_PROCEDURE LEX_PROGRAM LEX_RECORD LEX_REPEAT
%token LEX_SET LEX_THEN LEX_TO LEX_TYPE LEX_UNTIL LEX_VAR LEX_WHILE LEX_WITH
%token LEX_FORWARD

/* The following ones are not tokens used in the parser.
 * However they are used in the same context as some tokens,
 * so assign unique numbers to them.
 */
%token pp_SIN pp_COS pp_EXP pp_LN pp_SQRT pp_ARCTAN rr_POW rr_EXPON
%token r_WRITE r_READ r_INITFDR r_LAZYTRYGET r_LAZYGET r_LAZYUNGET r_POW r_EXPON
%token z_ABS z_ARCTAN z_COS z_EXP z_LN z_SIN z_SQRT z_POW z_EXPON
%token set_card set_isempty set_equal set_le set_less set_in set_clear
%token set_include set_exclude set_include_range set_copy
%token set_intersection set_union set_diff set_symdiff
%token p_DONEFDR gpc_IOCHECK gpc_RUNTIME_ERROR

/* Redefinable identifiers. */

/* Redefinable identifiers in Standard Pascal */
%token p_INPUT p_OUTPUT p_REWRITE p_RESET p_PUT p_GET p_WRITE p_READ
%token p_WRITELN p_READLN p_PAGE p_NEW p_DISPOSE
%token p_ABS p_SQR p_SIN p_COS p_EXP p_LN p_SQRT p_ARCTAN
%token p_TRUNC p_ROUND p_PACK p_UNPACK p_ORD p_CHR p_SUCC p_PRED
%token p_ODD p_EOF p_EOLN p_MAXINT p_TRUE p_FALSE

/* Additional redefinable identifiers for Borland Pascal */
%token bp_RANDOM bp_RANDOMIZE BREAK CONTINUE

/* redefinable keyword extensions */
%token RETURN_ RESULT EXIT FAIL p_CLOSE CONJUGATE p_DEFINESIZE SIZEOF
%token BITSIZEOF ALIGNOF TYPEOF gpc_RETURNADDRESS gpc_FRAMEADDRESS
%token LEX_LABEL_ADDR

/* GPC internal tokens */
/* MODIFIED to introduce types. */
%token <y_int> LEX_INTCONST
%token <y_string> LEX_STRCONST
%token <y_real> LEX_REALCONST
%token LEX_RANGE LEX_ELLIPSIS

/* We don't declare precedences for operators etc. We don't need
   them since our rules define precedence implicitly, and too many
   precedences increase the chances of real conflicts going unnoticed. */
%token LEX_ASSIGN
%token '<' '=' '>' LEX_IN LEX_NE LEX_GE LEX_LE
%token '-' '+' LEX_OR LEX_OR_ELSE LEX_CEIL_PLUS LEX_CEIL_MINUS LEX_FLOOR_PLUS LEX_FLOOR_MINUS
%token '/' '*' LEX_DIV LEX_MOD LEX_AND LEX_AND_THEN LEX_SHL LEX_SHR LEX_XOR LEX_CEIL_MULT LEX_CEIL_DIV LEX_FLOOR_MULT LEX_FLOOR_DIV
%token LEX_POW LEX_POWER LEX_IS LEX_AS
%token LEX_NOT

/* Various extra tokens */
%token LEX_EXTERNAL p_MARK p_RELEASE p_UPDATE p_GETTIMESTAMP p_UNBIND
%token p_EXTEND bp_APPEND p_BIND p_SEEKREAD p_SEEKWRITE p_SEEKUPDATE LEX_SYMDIFF
%token p_ARG p_CARD p_EMPTY p_POSITION p_LASTPOSITION p_LENGTH p_TRIM p_BINDING
%token p_DATE p_TIME LEX_RENAME LEX_IMPORT LEX_USES LEX_QUALIFIED LEX_ONLY

%type <y_stid>      identifier new_identifier
%type <y_string>    new_identifier_1
%type <y_stid_item> id_list variable_declaration_list variable_declaration

%type <y_type> typename type_denoter new_ordinal_type subrange_type new_pointer_type pointer_domain_type
%type <y_type> new_structured_type array_type ordinal_index_type new_procedural_type functiontype

%type <y_typedef_item> type_definition function_heading
%type <y_type_list>    array_index_list

%type <y_param_list> procedural_type_formal_parameter_list formal_parameter_list procedural_type_formal_parameter formal_parameter 
%type <y_param_list> optional_procedural_type_formal_parameter_list optional_par_formal_parameter_list

%type <y_expr> actual_parameter assignment_or_call_statement variable_or_function_access_maybe_assignment
%type <y_expr> rest_of_statement /*standard_procedure_statement*/ index_expression_item
%type <y_expr> static_expression boolean_expression expression simple_expression

%type <y_expr> one_case_constant
%type <y_expr_list> case_constant_list

%type <y_expr> term signed_primary primary signed_factor factor variable_or_function_access
%type <y_expr> variable_or_function_access_no_standard_function variable_or_function_access_no_id
%type <y_expr> standard_functions optional_par_actual_parameter

%type <y_dir_list> directive_list
%type <y_dir> directive

%type <y_string> simple_if
%type <y_for_dir> for_direction

%type <y_boolean> optional_semicolon_or_else_branch
%type <y_string> case_element
%type <y_expr> constant number unsigned_number constant_literal string predefined_literal

%type <y_expr_list> index_expression_list actual_parameter_list optional_par_actual_parameter_list

%type <y_cint> sign rts_fun_onepar rts_fun_parlist multiplying_operator adding_operator relational_operator any_declaration_part variable_declaration_part any_decl simple_decl function_declaration

/* Precedence rules */

/* The following precedence declarations are just to avoid the dangling
   else shift-reduce conflict. We use prec_if rather than LEX_IF to
   avoid possible conflicts elsewhere involving LEX_IF going unnoticed. */
%nonassoc prec_if
%nonassoc LEX_ELSE

/* These tokens help avoid S/R conflicts from error recovery rules. */
%nonassoc lower_than_error
%nonassoc error

%%

/* Pascal parser starts here */

pascal_program:
    main_program_declaration '.'
  ;

main_program_declaration:
    program_heading semi any_global_declaration_part { start_main(); } statement_part { end_main(); }
  ;

program_heading:
    LEX_PROGRAM new_identifier optional_par_id_list
  ;

optional_par_id_list:
    /* empty */
  | '(' id_list ')'
  ;

/* $$ type should be ST_ID list. */
id_list:
    new_identifier              { $$ = new_stid_list($1); }
  | id_list ',' new_identifier  { $$ = append_stid_to_list($1, $3); }
  ;

/* $$ type should be TYPE. */
typename:
    LEX_ID { $$ = get_basic_type($1); }
  ;

/* $$ type should be ST_ID. */
identifier:
    LEX_ID { $$ = st_enter_id($1); }
  ;

/* $$ type should be ST_ID. */
new_identifier:
    new_identifier_1 { $$ = st_enter_id($1); }
  ;

/* $$ type should be y_string. */
new_identifier_1:
    LEX_ID   { $$ = $1; }
/* Standard Pascal constants */
  | p_MAXINT {}
  | p_FALSE  {}
  | p_TRUE   {}
/* Standard Pascal I/O */
  | p_INPUT   {}
  | p_OUTPUT  {}
  | p_REWRITE {}
  | p_RESET   {}
  | p_PUT     {}
  | p_GET     {}
  | p_WRITE   {}
  | p_READ    {}
  | p_WRITELN {}
  | p_READLN  {}
  | p_PAGE    {}
  | p_EOF     {}
  | p_EOLN    {}
/* Standard Pascal heap handling */
  | p_NEW     {}
  | p_DISPOSE {}
/* Standard Pascal arithmetic */
  | p_ABS    {}
  | p_SQR    {}
  | p_SIN    {}
  | p_COS    {}
  | p_EXP    {}
  | p_LN     {}
  | p_SQRT   {}
  | p_ARCTAN {}
  | p_TRUNC  {}
  | p_ROUND  {}
/* Standard Pascal transfer functions */
  | p_PACK   {}
  | p_UNPACK {}
/* Standard Pascal ordinal functions */
  | p_ORD  {}
  | p_CHR  {}
  | p_SUCC {}
  | p_PRED {}
  | p_ODD  {}
/* Other extensions */
  | BREAK     {}
  | CONTINUE  {}
  | RETURN_   {}
  | RESULT    {}
  | EXIT      {}
  | FAIL      {}
  | SIZEOF    {}
  | BITSIZEOF {}
  ;

any_global_declaration_part:
    /* empty */
  | any_global_declaration_part any_decl
  ;

any_declaration_part:
    /* empty */ { $$ = 0;}
  | any_declaration_part any_decl { $$ = $1+$2; }
  ;

any_decl:
    simple_decl
  | function_declaration
  ;

simple_decl:
    constant_definition_part { $$ = 0; }
  | type_definition_part { $$ = 0; }
  | variable_declaration_part
  ;

/* constant definition part */

constant_definition_part:
    LEX_CONST constant_definition_list
  ;

constant_definition_list:
    constant_definition
  | constant_definition_list constant_definition
  ;

constant_definition:
    new_identifier '=' static_expression semi
  ;

constant:
    identifier       {}
  | sign identifier  {}
  | number           { $$ = $1; }
  | constant_literal { $$ = $1; }
  ;

number:
    sign unsigned_number { EXPR sign = new_expr_intconst($1); $$ = new_expr_arith(sign, AR_MULT, $2); }
  | unsigned_number { $$ = $1; }
  ;

unsigned_number:
    LEX_INTCONST  { $$ = new_expr_intconst($1); }
  | LEX_REALCONST { $$ = new_expr_realconst($1); }
  ;

sign:
    '+' { $$ = 1; }
  | '-' { $$ = -1; }
  ;

constant_literal:
    string
  | predefined_literal
  ;

predefined_literal:
    LEX_NIL { }
  | p_FALSE { $$ = new_expr_boolconst(FALSE); }
  | p_TRUE  { $$ = new_expr_boolconst(TRUE); }
  ;

string:
    LEX_STRCONST { $$ = new_expr_strconst($1); }
  | string LEX_STRCONST { $$ = new_expr_strconst($2); }
  ;

/* $$ type should be NONE */
type_definition_part:
    LEX_TYPE type_definition_list semi         { process_unresolved_types(); }
  ;

/* $$ type should be NONE */
type_definition_list:
    type_definition                            { install_typedef($1); }
  | type_definition_list semi type_definition  { install_typedef($3); }
  ;

/* $$ type should be typedef_item_p (y_typedef_item) */
type_definition:
    new_identifier '=' type_denoter            { $$ = make_typedef_node($1, $3); }
  ;

/* $$ type should be TYPE (y_type) */
type_denoter:
    typename             { $$ = $1; }
  | new_ordinal_type     { $$ = $1; }
  | new_pointer_type     { $$ = $1; }
  | new_procedural_type  { $$ = $1; }
  | new_structured_type  { $$ = $1; }
  ;

/* $$ type should be TYPE. */
new_ordinal_type:
    enumerated_type  { /* enumerations not required by project 1. */ }
  | subrange_type    { $$ = $1; }
  ;

/* IGNORE for project 1. */
enumerated_type:
    '(' enum_list ')'
  ;

/* IGNORE for project 1. */
enum_list:
    enumerator
  | enum_list ',' enumerator
  ;

/* IGNORE for project 1. */
enumerator:
    new_identifier
  ;

/* $$ type should be TYPE (y_TYPE). */
subrange_type:
    constant LEX_RANGE constant  { $$ = create_subrange($1, $3); }
  ;

/* $$ type should be TYPE (y_TYPE). */
new_pointer_type:
    pointer_char pointer_domain_type  { $$ = ty_build_ptr($2); } 
  ;

pointer_char:
    '^'
  | '@'
  ;

/* $$ type should be TYPE (y_TYPE). */
pointer_domain_type:
    new_identifier      { $$ = get_basic_type(st_get_id_str($1)); }
  | new_procedural_type { $$ = $1; }
  ;

/* $$ type should be TYPE (y_TYPE). */
new_procedural_type:
    LEX_PROCEDURE optional_procedural_type_formal_parameter_list             { 
    /* create new procedure type with param list. */ 
    $$ = ty_build_func(ty_build_basic(TYVOID), $2, TRUE);
    }
  | LEX_FUNCTION optional_procedural_type_formal_parameter_list functiontype { /* create new functiontype-returning function type with param list. */ 
  $$ = ty_build_func($3, $2, TRUE);
  }
  ;

/* $$ type should be PARAM_LIST. */
optional_procedural_type_formal_parameter_list:
    /* empty */  { $$ = NULL; }
  | '(' procedural_type_formal_parameter_list ')' { $$ = $2; }
  ;

/* $$ type should be PARAM_LIST. */
procedural_type_formal_parameter_list:
    procedural_type_formal_parameter { $$ = $1; }
  | procedural_type_formal_parameter_list semi procedural_type_formal_parameter  { $$ = merge_param_lists($1, $3); } 
  ;

/* $$ type should be PARAM_LIST. */
procedural_type_formal_parameter:
    id_list                      { $$ = id_list_to_param_list($1, ty_build_basic(TYVOID), FALSE); }
  | id_list ':' typename         { $$ = id_list_to_param_list($1, $3, FALSE); }
  | LEX_VAR id_list ':' typename {  $$ = id_list_to_param_list($2, $4, TRUE); }
  | LEX_VAR id_list              {  $$ = id_list_to_param_list($2, ty_build_basic(TYVOID), TRUE); }
  ;

/* $$ type should be TYPE (y_TYPE). */
new_structured_type:
    array_type  { $$ = $1; }
  | set_type    { /* IGNORE for project 1. */ }
  | record_type { /* IGNORE for project 1. */ }
  ;

/* Array */

/* $$ type should be TYPE (y_TYPE). */
array_type:
    LEX_ARRAY '[' array_index_list ']' LEX_OF type_denoter { $$ = ty_build_array($6, $3); if(!isDataType($6)) { error("Array type must be data type."); }  }
  ;

/* $$ type should be TYPE_LIST. */
array_index_list:
    ordinal_index_type                       { $$ = make_new_type_list($1); }
  | array_index_list ',' ordinal_index_type  { $$ = append_to_type_list($1, $3); }
  ;

/* $$ type should be TYPE (y_type). */
ordinal_index_type:
    new_ordinal_type  { $$ = $1; }
  | typename          { $$ = $1; }
  ;


/* sets */
set_type:
    LEX_SET LEX_OF type_denoter
  ;

record_type:
    LEX_RECORD record_field_list LEX_END
  ;

record_field_list:
    /* empty */
  | fixed_part optional_semicolon
  | fixed_part semi variant_part
  | variant_part
  ;

fixed_part:
    record_section
  | fixed_part semi record_section
  ;

record_section:
    id_list ':' type_denoter
  ;

variant_part:
    LEX_CASE variant_selector LEX_OF variant_list rest_of_variant
  ;

rest_of_variant:
    optional_semicolon
  | case_default '(' record_field_list ')' optional_semicolon
  ;

variant_selector:
    new_identifier ':' variant_type
  | variant_type
  ;

variant_type:
    typename
  | new_ordinal_type
  ;

variant_list:
    variant
  | variant_list semi variant
  ;

variant:
    case_constant_list ':' '(' record_field_list ')'
  ;

case_constant_list:
    one_case_constant { $$ = new_expr_list($1); }
  | case_constant_list ',' one_case_constant { $$ = append_to_expr_list($1, $3); }
  ;

one_case_constant:
    static_expression
  | static_expression LEX_RANGE static_expression { $$ = new_expr_subrange($1, $3); }
  ;

/* variable declaration part */
/* We keep track of total size needed in case of local variables by
   using a simple inherited attribute of type int */

variable_declaration_part:
    LEX_VAR variable_declaration_list { 
    $$ = size_of_vars($2); 
    }
  ;

variable_declaration_list:
    variable_declaration
  | variable_declaration_list variable_declaration { $$ = merge_stid_list($1, $2); }
  ;

variable_declaration:
    id_list ':' type_denoter semi 	{ 
    vardec($1, $3);
    $$ = $1;
    }
  ;

function_declaration:
    function_heading semi directive_list semi {
      //Generate function declaration with directives
      ST_DR rec = apply_directives($1, $3);
      st_install($1->new_def, rec);
    }
  | function_heading semi { 
      //Generate function declaration with local definition
      install_function_decl($1);
      enter_function_block($1);
  } any_declaration_part {
	   int block;
      b_func_prologue(st_lookup($1->new_def, &block)->u.decl.v.global_func_name);
      encode_function_def($1);
      b_alloc_local_vars($4);
  } statement_part semi {
      exit_function_block($1);
  }
  ;

function_heading:
    LEX_PROCEDURE new_identifier optional_par_formal_parameter_list {
      $$ = make_typedef_node($2, ty_build_func(ty_build_basic(TYVOID), $3, TRUE));
    }
  | LEX_FUNCTION new_identifier optional_par_formal_parameter_list functiontype {
      $$ = make_typedef_node($2, ty_build_func($4, $3, TRUE));
    }
  ;

directive_list:
    directive { $$ = create_dir_list($1); }
  | directive_list semi directive { $$ = append_to_dir_list($1, $3); }
  ;

directive:
    LEX_FORWARD { $$ = DIRECTIVE_FORWARD; }
  | LEX_EXTERNAL { $$ = DIRECTIVE_EXTERNAL; }
  ;

functiontype:
    /* empty */ { $$ = ty_build_basic(TYERROR); error("A function should not be typeless."); }
  | ':' typename { $$ = $2; if(!isSimpleType($2)) { error("Return type must be simple type."); } }
  ;

/* parameter specification section */

optional_par_formal_parameter_list:
    /* empty */ { $$ = NULL; }
  | '(' formal_parameter_list ')' { $$ = $2;}
  ;

formal_parameter_list:
    formal_parameter { $$ = $1; }
  | formal_parameter_list semi formal_parameter { $$ = merge_param_lists($1, $3); }
  ;

formal_parameter:
    id_list ':' typename { $$ =  id_list_to_param_list($1, $3, FALSE); }
  | LEX_VAR id_list ':' typename { $$ = id_list_to_param_list($2, $4, TRUE); }
  ;


/* Pascal statements */

statement_part:
    compound_statement
  ;

compound_statement:
    LEX_BEGIN statement_sequence LEX_END
  ;

statement_sequence:
    statement
  | statement_sequence semi statement
  ;

statement:
    structured_statement
  | simple_statement
  ;

structured_statement:
    compound_statement
  | with_statement {}
  | conditional_statement
  | repetitive_statement
  ;

with_statement:
    LEX_WITH structured_variable_list LEX_DO statement {}
  ;

structured_variable_list:
    structured_variable
  | structured_variable_list ',' structured_variable
  ;

structured_variable:
    variable_or_function_access
  ;

conditional_statement:
    if_statement
  | case_statement
  ;

simple_if:
    LEX_IF boolean_expression LEX_THEN
    {
        char *after_if_label = new_symbol();
        encode_expression($2);
        b_cond_jump(TYSIGNEDCHAR, B_ZERO, after_if_label);
        
        $<y_string>$ = after_if_label;
    }
    statement
    {
        $$ = $<y_string>4;
    }
  ;

if_statement:
    simple_if LEX_ELSE
    {
        char *end_label = new_symbol();
        b_jump(end_label);
        b_label($1); 
        $<y_string>$ = end_label;
    }
    statement
    {
        b_label($<y_string>3);
    }
  | simple_if %prec prec_if { b_label($1); }
  ;

case_statement:
    LEX_CASE expression LEX_OF {
      char *end_label = new_symbol();
      if (!isOrdinalType($2->expr_typetag))
      {
        error("Case expression is not of ordinal type");
      }
      
      enter_case_block();
      EXPR expr = parse_expr_for_case($2);
      
      encode_expression(expr);
      $<y_string>$ = end_label;      
    } case_element_list optional_semicolon_or_else_branch LEX_END {
    	if($6 == FALSE)
    	{
      	b_pop();
      }
      exit_case_block();
      b_label($<y_string>4);
    }
  ;

optional_semicolon_or_else_branch:
    optional_semicolon { $$ = FALSE; } //Pass false, no else statement
  | case_default { b_pop(); } statement_sequence { $$ = TRUE;} //Pass true, else statement exists
  ;

case_element_list:
    case_element { b_jump($<y_string>0); b_label($1); }
  | case_element_list semi case_element { b_jump($<y_string>0); b_label($3); }
  ;

case_element:
    case_constant_list {
    	char *end_label = new_symbol();
    	char *statement_label = new_symbol();
    	
    	EXPR_LIST list = $1;
    	while(list != NULL)
    	{
    		EXPR expr = list->base;
    		if (!isCaseableType(expr->expr_typetag))
    		{
    		  // Do nothing -- error reporting done by isCaseableType().
    		}
    		else if (expr->expr_tag == E_SUBRANGE)
    		{
    			int lo = (int)get_expr_constant(expr->left);
    			int hi = (int)get_expr_constant(expr->right);
    			
    			if (check_subrange(lo, hi))
    			{
    				add_subrange(lo, hi);
    				
		  		  char* next_dispatch = new_symbol();
				    b_dispatch(B_LT, TYSIGNEDLONGINT, lo, next_dispatch, FALSE);
				    b_dispatch(B_LE, TYSIGNEDLONGINT, hi, statement_label, TRUE);
				    b_label(next_dispatch);
				  }
				  else
				  {
				  	error("Overlapping constants in case statement");
				  }
    		}
    		else
    		{
    			int i = (int)get_expr_constant(expr);
    			
    			if (check_constant(i))
    			{
    				add_constant(i);
    		  	b_dispatch(B_EQ, TYSIGNEDLONGINT, i, statement_label, TRUE);
    		  }
    		  else
    		  {
    		  	error("Overlapping constants in case statement");
    		  }
    		}
    		list = list->next;
    	}
    	
    	b_jump(end_label);
    	b_label(statement_label);
    	$<y_string>$ = end_label;
    } ':' statement {
    	$$ = $<y_string>2;
    }
  ;

case_default:
    LEX_ELSE
  | semi LEX_ELSE
  ;

repetitive_statement:
    repeat_statement
  | while_statement
  | for_statement
  ;

repeat_statement:
    LEX_REPEAT statement_sequence LEX_UNTIL boolean_expression
  ;

while_statement:
    LEX_WHILE boolean_expression LEX_DO
    {
        char *while_after_label = new_symbol();
        char *while_cond_label = new_symbol();
        store_label(while_after_label);
        
        b_label(while_cond_label);
        encode_expression($2);
        b_cond_jump(TYSIGNEDCHAR, B_ZERO, while_after_label);
        
        control_labels lbls = {while_cond_label, while_after_label};
        $<y_control>$ = lbls;
    }
    statement
    {
        control_labels lbls = $<y_control>4;
        b_jump(lbls.conditional_label);
        b_label(lbls.after_label);
    }
  ;

for_statement:
    LEX_FOR variable_or_function_access LEX_ASSIGN expression for_direction expression LEX_DO
    {
        char *for_exit_label = new_symbol();
        char *for_cond_label = new_symbol();
        store_label(for_exit_label);
        
        if (!isOrdinalType($2->expr_typetag))
        {
          error("For-loop control variable not of ordinal type");
        }
        else if ($2->expr_typetag != $4->expr_typetag || $2->expr_typetag != $6->expr_typetag || $4->expr_typetag != $6->expr_typetag)
        {
          error("Type mismatch in for-loop control");
        }
        else if (!isOrdinalType($4->expr_typetag) || !isOrdinalType($6->expr_typetag))
        {
          error("For-loop control variable not of ordinal type");
        }
        
        encode_expression($6);
        if ($6->expr_tag == E_VAR || $6->expr_tag == E_ARRAY) { b_deref($6->expr_typetag); }
        
        if ($6->expr_typetag == TYUNSIGNEDCHAR || $6->expr_typetag == TYSIGNEDCHAR)
        {
          b_convert($6->expr_typetag, TYSIGNEDLONGINT);
        }
        
        b_duplicate(TYSIGNEDLONGINT);
        
        encode_expression($2);
        encode_expression($4);
        b_assign($2->expr_typetag);
        
        b_label(for_cond_label);
        
        if ($2->expr_typetag == TYUNSIGNEDCHAR || $2->expr_typetag == TYSIGNEDCHAR)
        {
          b_convert($6->expr_typetag, TYSIGNEDLONGINT);
        }
        
        if ($5 == FOR_TO)
        {
            b_arith_rel_op(B_LT, TYSIGNEDLONGINT);
        }
        else //if ($5 == FOR_DOWNTO)
        {
            b_arith_rel_op(B_GT, TYSIGNEDLONGINT);
        }
        b_cond_jump(TYSIGNEDLONGINT, B_NONZERO, for_exit_label);
        
        control_labels lbls = { for_cond_label, for_exit_label };
        
        $<y_control>$ = lbls;
        
    }
    statement
    {
        control_labels lbls = $<y_control>8;
        
        b_duplicate(TYSIGNEDLONGINT);
        
        encode_expression($2);
        
        B_INC_DEC_OP idop = ($5 == FOR_TO) ? B_PRE_INC : B_PRE_DEC;
        
        b_inc_dec($2->expr_typetag, idop, 0);
        
        b_jump(lbls.conditional_label);
        b_label(lbls.after_label);
        b_pop();
    }
  ;

for_direction:
    LEX_TO      { $$ = FOR_TO; }
  | LEX_DOWNTO  { $$ = FOR_DOWNTO; }
  ;

simple_statement:
    empty_statement { /* do nothing */ }
  | assignment_or_call_statement { encode_expression($1); }
  | standard_procedure_statement { printf("Standard Procedure"); }
  | statement_extensions { /* ignore */ }
  ;

empty_statement:
    /* empty */ %prec lower_than_error
  ;

/* function calls */

optional_par_actual_parameter_list:
    /* empty */ { }
  | '(' actual_parameter_list ')'	{ $$ = $2; }
  ;

actual_parameter_list:
    actual_parameter { $$ = new_expr_list($1); }						
  | actual_parameter_list ',' actual_parameter { $$ = append_to_expr_list($1, $3); }
  ;

actual_parameter:
    expression
  ;

/* ASSIGNMENT and procedure calls */

assignment_or_call_statement:

    variable_or_function_access_maybe_assignment rest_of_statement { if ($2 != NULL){ $$ = new_expr_assign($1, $2); } 
    																                                           else { $$ = $1; } }
  ;

variable_or_function_access_maybe_assignment:
    identifier { $$ = new_expr_identifier($1); }
  | address_operator variable_or_function_access { /* ignore */ }
  | variable_or_function_access_no_id
  ;

rest_of_statement:
    /* Empty */			  { $$ = NULL; }
  | LEX_ASSIGN expression { $$ = $2; }
  ;

standard_procedure_statement:
    rts_proc_onepar '(' actual_parameter ')'
  | rts_proc_parlist '(' actual_parameter_list ')'
  | p_WRITE optional_par_write_parameter_list
  | p_WRITELN optional_par_write_parameter_list
  | p_READ optional_par_actual_parameter_list
  | p_READLN optional_par_actual_parameter_list
  | p_PAGE optional_par_actual_parameter_list
  | p_DISPOSE '(' actual_parameter ')'
  | p_DISPOSE '(' actual_parameter ',' actual_parameter_list ')'
  ;

optional_par_write_parameter_list:
    /* empty */
  | '(' write_actual_parameter_list ')'
  ;

write_actual_parameter_list:
    write_actual_parameter
  | write_actual_parameter_list ',' write_actual_parameter
  ;

write_actual_parameter:
    actual_parameter
  | actual_parameter ':' expression
  | actual_parameter ':' expression ':' expression
  ;

/* run time system calls with one parameter */
rts_proc_onepar:
    p_PUT
  | p_GET
  | p_MARK
  | p_RELEASE
  | p_CLOSE
  | p_UPDATE
  | p_GETTIMESTAMP
  | p_UNBIND
  ;

rts_proc_parlist:
    p_REWRITE     /* Up to three args */
  | p_RESET       /* Up to three args */
  | p_EXTEND      /* Up to three args */
  | bp_APPEND     /* Up to three args */
  | p_PACK        /* Three args */
  | p_UNPACK      /* Three args */
  | p_BIND        /* Two args */
  | p_SEEKREAD
  | p_SEEKWRITE
  | p_SEEKUPDATE
  | p_DEFINESIZE  /* Two args */
  | LEX_AND           /* Two args */
  | LEX_OR            /* Two args */
  | LEX_NOT           /* One arg */
  | LEX_XOR        /* Two args */
  | LEX_SHL           /* Two args */
  | LEX_SHR           /* Two args */
  ;

statement_extensions:
    return_statement
  | continue_statement
  | break_statement
  ;

return_statement:
    RETURN_
  | RETURN_ expression
  | EXIT
  | FAIL
  ;

break_statement:
    BREAK   { char *last_loop_label = get_last_label(); b_jump(last_loop_label); }
  ;

continue_statement:
    CONTINUE
  ;

variable_access_or_typename:
    variable_or_function_access_no_id
  | LEX_ID
  ;

index_expression_list:
      index_expression_item { $$ = new_expr_list($1); }
    | index_expression_list ',' index_expression_item { $$ = append_to_expr_list($1, $3); }
    ;

index_expression_item:
      expression
    | expression LEX_RANGE expression
  ;

/* expressions */

static_expression:
    expression
  ;

boolean_expression:
    expression { if ($1->expr_typetag != TYSIGNEDCHAR) { error("Non-Boolean expression"); } $$ = $1; }
  ;

expression:
    expression relational_operator simple_expression { $$ = new_expr_compr($1, $2, $3); }
  | expression LEX_IN simple_expression
  | simple_expression
  ;

simple_expression:
    term
  | simple_expression adding_operator term 	{ $$ = new_expr_arith($1, $2, $3); }
  | simple_expression LEX_SYMDIFF term
  | simple_expression LEX_OR term
  | simple_expression LEX_XOR term
  ;

term:
    signed_primary
  | term multiplying_operator signed_primary { $$ = new_expr_arith($1, $2, $3); }
  | term LEX_AND signed_primary
  ;

signed_primary:
    primary 
  | sign signed_primary { $$ = new_expr_sign($1, $2); }
  ;

primary:
    factor
  | primary LEX_POW factor
  | primary LEX_POWER factor
  | primary LEX_IS typename
  ;

signed_factor:
    factor
  | sign signed_factor 	{ $$ = new_expr_sign($1, $2); }
  ;

factor:
    variable_or_function_access
  | constant_literal
  | unsigned_number
  | set_constructor         { /* ignore */ }
  | LEX_NOT signed_factor   { /* ignore? */ }
  | address_operator factor { /* ignore */ }
  ;

address_operator:
    '@'
  ;

variable_or_function_access:
    variable_or_function_access_no_standard_function
  | standard_functions
  ;

variable_or_function_access_no_standard_function:
    identifier { $$ = new_expr_identifier($1); }
  | variable_or_function_access_no_id
  ;

variable_or_function_access_no_id:
    p_OUTPUT  { /* ignore */ }
  | p_INPUT   { /* ignore */ }
  | variable_or_function_access '.' new_identifier { /* ignore */ }
  | '(' expression ')' { $$ = $2; }
  | variable_or_function_access pointer_char
  | variable_or_function_access '[' index_expression_list ']' { $$ = new_expr_array($1, $3); }
  | variable_or_function_access_no_standard_function '(' actual_parameter_list ')' { $$ = new_expr_var_funccall($1, $3); }
  | p_NEW '(' variable_access_or_typename ')' { /* TODO Pointer Variable */ }
  ;

set_constructor:
    '[' ']'
  | '[' set_constructor_element_list ']'
  ;

set_constructor_element_list:
    member_designator
  | set_constructor_element_list ',' member_designator
  ;

member_designator:
    expression
  | expression LEX_RANGE expression
  ;

standard_functions:
    rts_fun_onepar '(' actual_parameter ')' { EXPR_LIST tempList = new_expr_list($3); $$ = new_expr_unfunc($1, tempList); }
  | rts_fun_optpar optional_par_actual_parameter { /* ignore EOF */ }
  | rts_fun_parlist '(' actual_parameter_list ')' { $$ = new_expr_unfunc($1, $3); }
  ;

optional_par_actual_parameter:

    /* empty */ { }
  | '(' actual_parameter ')' { $$ = $2; }
  ;

rts_fun_optpar:
    p_EOF   { /* ignore */ }
  | p_EOLN  { /* ignore */ }
  ;

rts_fun_onepar:
    p_ABS          { /* ignore */ }
  | p_SQR          { /* ignore */ }
  | p_SIN          { /* ignore */ }
  | p_COS          { /* ignore */ }
  | p_EXP          { /* ignore */ }
  | p_LN           { /* ignore */ }
  | p_SQRT         { /* ignore */ }
  | p_ARCTAN       { /* ignore */ }
  | p_ARG          { /* ignore */ }
  | p_TRUNC        { /* ignore */ }
  | p_ROUND        { /* ignore */ }
  | p_CARD         { /* ignore */ }
  | p_ORD          { $$ = UF_ORD; }
  | p_CHR          { $$ = UF_CHR; }
  | p_ODD          { /* ignore */ }
  | p_EMPTY        { /* ignore */ }
  | p_POSITION     { /* ignore */ }
  | p_LASTPOSITION { /* ignore */ }
  | p_LENGTH       { /* ignore */ }
  | p_TRIM         { /* ignore */ }
  | p_BINDING      { /* ignore */ }
  | p_DATE         { /* ignore */ }
  | p_TIME         { /* ignore */ }
  ;

rts_fun_parlist:
    p_SUCC    { $$ = UF_SUCC; }
  | p_PRED    { $$ = UF_PRED; }
  ;

relational_operator:
    LEX_NE 	{ $$ = CM_NEQUAL; }
  | LEX_LE  { $$ = CM_LSEQL; }
  | LEX_GE 	{ $$ = CM_GTEQL; }
  | '='		{ $$ = CM_EQUAL; }
  | '<'		{ $$ = CM_LESS; }
  | '>'		{ $$ = CM_GREAT; }
  ;

multiplying_operator:
    LEX_DIV { $$ = AR_IDIV; }
  | LEX_MOD	{ $$ = AR_MOD; }
  | '/'		{ $$ = AR_RDIV; }
  | '*'		{ $$ = AR_MULT; }
  ;

adding_operator:
    '-'	{ $$ = AR_SUB; }
  | '+'	{ $$ = AR_ADD; }
  ;

semi:
    ';'
  ;

optional_semicolon:
    /* empty */
  | ';'
  ;

%%

void yyerror(char *msg)
{
    error(msg);
}

/* Sets the value of the 'yydebug' variable to VALUE.
   This is a function so we don't have to have YYDEBUG defined
   in order to build the compiler.  */
void
set_yydebug (int value)
{
#if YYDEBUG != 0
  yydebug = value;
#else
  warning ("YYDEBUG not defined.");
#endif
}
