#include <vector>
#include <string>
#include "storage/Database.hpp"
#include "ast/Statement.hpp"

Database db("testDatabase");
std::vector<std::vector<std::string>> gui_results;
std::vector<std::string> gui_headers;
std::string gui_error = "";
std::vector<std::string> gui_log;

extern int yyparse();
typedef struct yy_buffer_state *YY_BUFFER_STATE;
extern YY_BUFFER_STATE yy_scan_string(const char * str);
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);
extern std::vector<Statement*> root_statements;
extern int yylineno;

void executeSQL(const char* input) {
    gui_error = "";
    gui_results.clear();
    gui_headers.clear();
    root_statements.clear();
    yylineno = 1;

    YY_BUFFER_STATE buffer = yy_scan_string(input);
    if (yyparse() == 0) {
        for (auto* stmt : root_statements) {
            if (stmt) {
                stmt->execute();
                delete stmt;
            }
        }
        root_statements.clear();
    }
    yy_delete_buffer(buffer);
}