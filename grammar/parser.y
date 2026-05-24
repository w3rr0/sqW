/**
 * @file parser.y
 * @brief Syntax grammar definition (BISON) for the SQL Interpreter.
 * * This parser constructs the AST by instantiating Statement classes
 * (SelectStmt, InsertStmt, etc.) based on recognized grammar rules
 */
%code requires { // this code will be added to the parser.hpp
    #include <string>
    #include <vector>
    #include <memory>

    // forward declarations
    #include "ast/Statement.hpp"
    #include "ast/WhereClause.hpp"
    #include "ast/ColumnDef.hpp"
    #include "ast/SelectStmt.hpp"
    #include "ast/InsertStmt.hpp"
    #include "ast/CreateStmt.hpp"
    #include "ast/DeleteStmt.hpp"
    #include "ast/UpdateStmt.hpp"
    #include "ast/DropTableStmt.hpp"
    #include "ast/AlterTableStmt.hpp"
}

%{
#include <iostream>
#include <cstdio>
#include <vector>

#include "ast/Statement.hpp"

int yylex();
void yyerror(const char *s);

/** @brief Pointer to the resulting Statement object after parsing */
std::vector<Statement*> root_statements;

/** @brief Log vector used to display execution status and errors */
extern std::vector<std::string> gui_log;

/** @brief Text string of the currently matched token passed from Flex */
extern char* yytext;

/** @brief Line counter managed automatically by Flex*/
extern int yylineno;
%}

/** @brief Union defining types for tokens and rules */
%union {
int num;
    char* str;
    bool boolean;
    Statement* stmt;
    WhereClause* where;
    ColumnDef* col_def;
    std::vector<ColumnDef>* col_defs;
    std::vector<std::string>* strings;
    std::vector<std::vector<std::string>>* multi_strings;
    // for instruction list
    std::vector<Statement*>* stmt_list;
    struct {
            SelectStmt::Aggregate type;
            char* col;
    } agg;
}

/* Tokens without values */
%token SELECT FROM WHERE CREATE TABLE INSERT INTO VALUES UPDATE DELETE SET DROP COLUMN ALTER ADD SHOW TABLES
%token TYPE_INT TYPE_STRING TYPE_BOOL TYPE_DOUBLE TYPE_FLOAT TYPE_NULL TYPE_NUMERIC
%token IN LIKE BETWEEN AND OR IS NOT
%token DISTINCT COUNT SUM MIN MAX
%token ORDER BY ASC DESC LIMIT OFFSET
%token LPAREN RPAREN COMMA SEMICOLON
%token STAR EQ GT GE LE LT NEQ

/* Tokens with values */
%token <num> NUMBER
%token <str> ID STR

/* Type definitions for grammar rules */
%type <stmt> instruction select_stmt create_stmt insert_stmt update_stmt delete_stmt drop_table_stmt alter_table_stmt
%type <where> where_clause condition
%type <strings> id_list value_list columns
%type <col_defs> column_defs
%type <col_def> column_def
%type <str> data_type value opt_order_by
%type <boolean> opt_distinct opt_asc_desc
%type <agg> aggregate_func
%type <multi_strings> values_lists
%type <str> opt_column
%type <num> optional_limit optional_offset
%type <stmt_list> program

/* rules for logical operators */
%left OR
%left AND

%%  /* -=-=-=- GRAMMAR RULES -=-=-=- */

/** @brief Main entry point for the grammar */
program:
    /* empty */
    | program instruction SEMICOLON {
            if ($2) {
                root_statements.push_back($2);
            }
    }
;

/** @brief Route to classic SQL command types*/
instruction:
    select_stmt         { $$ = $1; }
    | create_stmt       { $$ = $1; }
    | insert_stmt       { $$ = $1; }
    | update_stmt       { $$ = $1; }
    | delete_stmt       { $$ = $1; }
    | drop_table_stmt   { $$ = $1; }
    | alter_table_stmt  { $$ = $1; }
    | SHOW TABLES       { $$ = nullptr; }
    ;

/** @brief Rule for SELECT statements with support for filters, sorting, and aggregation */
select_stmt:
    SELECT opt_distinct columns FROM ID where_clause opt_order_by opt_asc_desc optional_limit optional_offset{
        // idxs: $1:SELECT, $2:opt_distinct, $3:columns, $4:FROM, $5:ID, $6:where_clause, $7:opt_order_by, $8:opt_asc_desc
        auto* sel = new SelectStmt($5, *$3, $2);

        if ($6) sel->setWhere(std::unique_ptr<WhereClause>($6));
        if ($7) {
            sel->setOrder($7, $8);
            free($7);
        }
        sel->limit = $9;
        sel->offset = $10;

        $$ = sel;
        free($5); delete $3;
    }
    | SELECT aggregate_func FROM ID where_clause opt_order_by opt_asc_desc optional_limit optional_offset {
        // $1:SELECT, $2:aggregate_func, $3:FROM, $4:ID, $5:where_clause,
        // $6:opt_order_by, $7:opt_asc_desc, $8:optional_limit, $9:optional_offset

        auto* stmt = new SelectStmt($4, {});

        // setting aggregation
        stmt->setAggregate($2.type, $2.col);

        // WHERE
        if ($5) stmt->setWhere(std::unique_ptr<WhereClause>($5));

        // ORDER BY
        if ($6) {
            stmt->setOrder($6, $7);
            free($6);
        }

        stmt->limit = $8;
        stmt->offset = $9;

        $$ = stmt;
        free($4);
        if ($2.col) free($2.col);
    }
    ;
/** @brief Optional function for only unique values (DISTINCT) */
opt_distinct:
    /* blank */ { $$ = false; }
    | DISTINCT  { $$ = true; }
    ;

/** @brief WHERE clause with support for logical and comparison conditions */
where_clause:
    /* blank */ { $$ = nullptr; }
    | WHERE condition { $$ = $2; }
    ;

/** * @brief examples of conditions in WHERE */
condition:
    ID EQ value {
        $$ = new ComparisonCondition($1, "=", $3);
        free($1); free($3);
    }
    | ID NEQ value {
            $$ = new ComparisonCondition($1, "!=", $3);
            free($1); free($3);
    }
    | ID GT value {
        $$ = new ComparisonCondition($1, ">", $3);
        free($1); free($3);
    }
    | ID LT value {
        $$ = new ComparisonCondition($1, "<", $3);
        free($1); free($3);
    }
    | ID GE value {
        $$ = new ComparisonCondition($1, ">=", $3);
        free($1); free($3);
    }
    | ID LE value {
        $$ = new ComparisonCondition($1, "<=", $3);
        free($1); free($3);
    }
    | ID IS TYPE_NULL {
        $$ = new ComparisonCondition($1, "IS", "NULL");
        free($1);
    }
    | ID IS NOT TYPE_NULL {
            $$ = new ComparisonCondition($1, "IS NOT", "NULL");
            free($1);
        }
    | ID LIKE STR {
        $$ = new ComparisonCondition($1, "LIKE", $3);
        free($1); free($3);
    }
    | ID NOT LIKE STR {
            $$ = new ComparisonCondition($1, "NOT LIKE", $4);
            free($1); free($4);
        }
    | ID IN LPAREN value_list RPAREN {
        $$ = new InCondition($1, *$4);
        free($1); delete $4;
    }
    | ID BETWEEN value AND value {
        $$ = new BetweenCondition($1, $3, $5);
        free($1); free($3); free($5);
    }
    | condition AND condition {
        $$ = new LogicalCondition(std::unique_ptr<WhereClause>($1), "AND", std::unique_ptr<WhereClause>($3));
    }
    | condition OR condition  {
        $$ = new LogicalCondition(std::unique_ptr<WhereClause>($1), "OR", std::unique_ptr<WhereClause>($3));
    }
    | LPAREN condition RPAREN {
        $$ = $2;
    }
    ;

/** @brief Identifier string type definition */
id_list:
    ID { $$ = new std::vector<std::string>(); $$->push_back($1); free($1); }
    | id_list COMMA ID { $1->push_back($3); $$ = $1; free($3); }
    ;

/** @brief Schema definition (CREATE TABLE) */
create_stmt:
    CREATE TABLE ID LPAREN column_defs RPAREN {
        $$ = new CreateStmt($3, *$5);
        free($3); delete $5;
    }
    ;

// COLUMNS
column_defs:
    column_def { $$ = new std::vector<ColumnDef>(); $$->push_back(*$1); delete $1; }
    | column_defs COMMA column_def { $1->push_back(*$3); $$ = $1; delete $3; }
    ;

// $1, $3 -> pseudo-identification variables in Bison
column_def:
    ID data_type { $$ = new ColumnDef{$1, $2}; free($1); }
    ;

// optional column for ADD (COLUMN)
opt_column:
    /* blank */ { $$ = nullptr; }
    | COLUMN    { $$ = nullptr; }
    ;

/** @brief Possible data types in CREATE TABLE statement */
data_type:
    TYPE_INT      { $$ = strdup("INT"); }
    | TYPE_STRING { $$ = strdup("STRING"); }
    | TYPE_BOOL   { $$ = strdup("BOOL"); }
    | TYPE_DOUBLE { $$ = strdup("DOUBLE"); }
    | TYPE_FLOAT  { $$ = strdup("FLOAT"); }
    | TYPE_NUMERIC { $$ = strdup("NUMERIC"); }
    ;

/** @brief Data insertion (INSERT INTO) */
insert_stmt:
    INSERT INTO ID VALUES values_lists {
        $$ = new InsertStmt($3, *$5);
        delete $5;
        free($3);
    }
    ;

/** @brief Many lists of values to insert into table */
values_lists:
    values_lists COMMA LPAREN value_list RPAREN {
        $1->push_back(*$4); // $1 -> vector<vector<string>>*
        delete $4;          // $4 -> vector<string>*
        $$ = $1;
    }
    | LPAREN value_list RPAREN {
        $$ = new std::vector<std::vector<std::string>>();
        $$->push_back(*$2);
        delete $2;
    }
    ;

/** @brief List of values to insert into table */
value_list:
    value_list COMMA value {
        $1->push_back($3);
        $$ = $1;
    }
    | value {
        $$ = new std::vector<std::string>();
        $$->push_back($1);
    }
    ;

/** @brief Delete row from table (DELETE FROM) function definitions */
delete_stmt:
    DELETE FROM ID where_clause {
        auto* del = new DeleteStmt($3);
        if ($4) del->setWhere(std::unique_ptr<WhereClause>($4));
        $$ = del;
        free($3);
    }
    ;

/** @brief Data update (UPDATE) */
update_stmt:
    UPDATE ID SET ID EQ value where_clause {
        auto* upd = new UpdateStmt($2, $4, $6);
        if ($7) {
            // if WHERE condition
            upd->setWhere(std::unique_ptr<WhereClause>($7));
        }
        $$ = upd;
        free($2); free($4); free($6);
    }
    ;

/** @brief Table removal (DROP TABLE) function definitions */
drop_table_stmt:
    DROP TABLE ID { $$ = new DropTableStmt($3); free($3); }
    ;

/** @brief Table modification (ALTER TABLE) */
alter_table_stmt:
    ALTER TABLE ID DROP COLUMN ID {
        $$ = new AlterTableStmt($3, $6, AlterTableStmt::DROP);
        free($3); free($6);
    }
    | ALTER TABLE ID ADD opt_column ID data_type {
        Cell::Type t = Cell::TEXT;
        std::string st($7); // $7 to teraz data_type
        if (st == "INT") t = Cell::INT;
        else if (st == "DOUBLE") t = Cell::DOUBLE;
        else if (st == "BOOL") t = Cell::BOOL;

        $$ = new AlterTableStmt($3, $6, t, AlterTableStmt::ADD);

        free($3); free($6); free($7);
    }
    ;

/** @brief sorting (ORDER BY) function definitions */
opt_order_by:
    /* blank */ { $$ = nullptr; }
    | ORDER BY ID { $$ = $3; } //column name
    ;

/** @brief optional option ASC / DESC used in ORDER BY function */
opt_asc_desc:
    /* blank */ { $$ = true; } // default
    | ASC       { $$ = true; }
    | DESC      { $$ = false; }
    ;

/** @brief limit */
optional_limit:
    LIMIT NUMBER { $$ = $2; }
    | /* empty */ { $$ = -1; }

/** @brief offset  */
optional_offset:
    OFFSET NUMBER { $$ = $2; }
    | /* empty */ { $$ = 0; }

/** * @brief Possible column types to display in table */
columns:
    STAR      { $$ = new std::vector<std::string>{"*"}; }
    | id_list { $$ = $1; }
    | aggregate_func {
        $$ = new std::vector<std::string>{"AGGREGATE_PLACEHOLDER"};
    }
    ;

/** @brief Aggregate function definitions */
aggregate_func:
      COUNT LPAREN STAR RPAREN   { $$.type = SelectStmt::Aggregate::COUNT; $$.col = strdup("*"); }
    | COUNT LPAREN ID RPAREN     { $$.type = SelectStmt::Aggregate::COUNT; $$.col = $3; }
    | SUM LPAREN ID RPAREN       { $$.type = SelectStmt::Aggregate::SUM;   $$.col = $3; }
    | MIN LPAREN ID RPAREN       { $$.type = SelectStmt::Aggregate::MIN;   $$.col = $3; }
    | MAX LPAREN ID RPAREN       { $$.type = SelectStmt::Aggregate::MAX;   $$.col = $3; }
    ;

/** * @brief Possible types used in conditions in WHERE clause */
value:
    NUMBER { $$ = strdup(std::to_string($1).c_str()); }
    | STR { $$ = $1; }
    | ID { $$ = $1; }
    | TYPE_NULL { $$ = strdup("NULL"); }
    ;

%%


/** * @brief Syntax error handler.
 * Automatically called by Bison when it encounters an invalid token sequence.
 */
void yyerror(const char *s) {
    std::string token;
    if (yytext) {
        token = yytext;
    } else {
        token = "EOF";
    }

    std::string errMsg = "Syntax Error at line " + std::to_string(yylineno) +
                         " near token '" + token + "'";

    gui_log.push_back("Error: " + errMsg);
}