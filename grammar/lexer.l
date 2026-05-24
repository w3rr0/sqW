/**
 * @file lexer.l
 * @brief Lexical scanner definition (FLEX) for the SQL Interpreter.
 * * The scanner identifies keywords, operators, data types, and literals,
 * passing them to the BISON parser along with their values (yylval)
 */
%{
#include <iostream>
#include <string.h>

#include "ast/Statement.hpp"
#include "ast/SelectStmt.hpp"
#include "ast/WhereClause.hpp"
#include "ast/ColumnDef.hpp"

#include "parser.hpp"

/** @brief Function called by Bison to fetch the next token from input*/
int yylex();
%}

/* Flex Options */
/* Stop after reading a single file */
%option noyywrap
/* Make keywords case-insensitive */
%option caseless
/* automatic line counting by Flex */
%option yylineno

%% /* -=-=-=- TOKENS -=-=-=- */
    /* === DQL (Data Query Language) KEYWORDS (Queries) === */
"SELECT"    { return SELECT; }
"DISTINCT"  { return DISTINCT; }
"FROM"      { return FROM; }
"WHERE"     { return WHERE; }

    /* === DML (Data Manipulation Language) / DDL (Data Definition Language) KEYWORDS (Data & Schema Management) === */
"CREATE"    { return CREATE; }
"TABLE"     { return TABLE; }
"INSERT"    { return INSERT; }
"INTO"      { return INTO; }
"VALUES"    { return VALUES; }
"UPDATE"    { return UPDATE; }
"DELETE"    { return DELETE; }
"SET"       { return SET; }
"DROP"      { return DROP; }
"COLUMN"    { return COLUMN; }
"ALTER"     { return ALTER; }
"ADD"       { return ADD; }

    /* === LOGICAL OPERATORS & FILTERS === */
"IN"        { return IN; }
"LIKE"      { return LIKE; }
"BETWEEN"   { return BETWEEN; }
"IS"        { return IS; }
"NOT"       { return NOT; }

    /* === SORTING AND LIMITS === */
"ORDER"     { return ORDER; }
"BY"        { return BY; }
"ASC"       { return ASC; }
"DESC"      { return DESC; }
"LIMIT"     { return LIMIT; }
"OFFSET"    { return OFFSET; }

    /* === AGGREGATE FUNCTIONS === */
"COUNT"      { return COUNT; }
"SUM"        { return SUM; }
"MIN"        { return MIN; }
"MAX"        { return MAX; }

    /* === DATA TYPES ===*/
"INT"        { return TYPE_INT; }
"INTEGER"    { return TYPE_INT; }
"STRING"     { return TYPE_STRING; }
"TEXT"       { return TYPE_STRING; }
"VARCHAR"    { return TYPE_STRING; }
"BOOLEAN"    { return TYPE_BOOL; }
"BOOL"       { return TYPE_BOOL; }
"DOUBLE"     { return TYPE_DOUBLE; }
"FLOAT"      { return TYPE_FLOAT; }
"NULL"       { return TYPE_NULL; }
"NUMERIC"    { return TYPE_NUMERIC; }

    /* === MATHEMATICAL OPERATORS & COMPARISONS === */
"*"         { return STAR; }
"="         { return EQ; }
">"         { return GT; }
">="        { return GE; }
"<="        { return LE; }
"<"         { return LT; }
"!="        { return NEQ; }
"AND"       { return AND; }
"OR"        { return OR; }

    /* === PUNCTUATION === */
"("         { return LPAREN; }
")"         { return RPAREN; }
","         { return COMMA; }
";"         { return SEMICOLON; }

    /* === ADDITIONAL FUNCTIONS === */
"SHOW"      { return SHOW; }
"TABLES"    { return TABLE; }


    /* === LITERALS & IDENTIFIERS === */

    /** @brief Float numbers, stored as strings to preserve precision */
[0-9]+\.[0-9]+ {
    yylval.str = strdup(yytext);
    return STR;
}

    /** @brief Integers, converted to int and passed through yylval.num */
[0-9]+ {
    yylval.num = atoi(yytext);
    return NUMBER;
}

    /** * @brief Identifiers (Table and Column names).
     * Converted to lowercase for database consistency
     */
[a-zA-Z_][a-zA-Z0-9_]* {
    std::string s(yytext);
    for(auto & c : s) c = (unsigned char)std::tolower(c);
    yylval.str = strdup(s.c_str());
    return ID;
}

    /** * @brief String literals in single '' or double "" quotes */
(\'[^\']*\'|\"[^\"]*\") {
    yylval.str = strdup(yytext);
    return STR;
}

    /* === ERROR HANDLING & WHITESPACE === */
[ \t\n\r]+  { /* ignore */ }
.           { std::cerr << "Unknown char " << yytext << std::endl; }
%%