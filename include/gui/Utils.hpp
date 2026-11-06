/**
* @file Utils.hpp
 * @brief Utility functions and global state declarations for the SQL Interpreter.
 * * This file provides color definitions for both CLI and GUI environments,
 * helper methods for rendering SQL with syntax highlighting
 * and managing the command-line interface.
 */
#ifndef UTILS_HPP
#define UTILS_HPP

#include "imgui.h"
#include <vector>
#include <string>
#include "storage/Database.hpp"

// ANSI colors in CLI for syntax highlighting
/** @brief ANSI escape code for red text in terminal */
#define CLI_RED     "\033[31m"
/** @brief ANSI escape code for pink text in terminal */
#define CLI_PINK    "\033[35m"
/** @brief ANSI escape code for yellow text in terminal */
#define CLI_YELLOW  "\033[33m"
/** @brief ANSI escape code for green text in terminal */
#define CLI_GREEN   "\033[32m"
/** @brief ANSI escape code for white text in terminal */
#define CLI_WHITE   "\033[37m"
/** @brief ANSI escape code to reset terminal formatting */
#define CLI_RESET   "\033[0m"

// IMGUI colors in GUI for syntax highlighting
/** @brief ImGui color vector for red (Errors / Keywords) */
const ImVec4 GUI_RED    = ImVec4(1.0f, 0.3f, 0.3f, 1.0f);
/** @brief ImGui color vector for pink (Metadata) */
const ImVec4 GUI_PINK   = ImVec4(1.0f, 0.4f, 0.7f, 1.0f);
/** @brief ImGui color vector for yellow (Numbers) */
const ImVec4 GUI_YELLOW = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
/** @brief ImGui color vector for green (Strings / Success) */
const ImVec4 GUI_GREEN  = ImVec4(0.3f, 1.0f, 0.3f, 1.0f);
/** @brief ImGui color vector for white (Regular text) */
const ImVec4 GUI_WHITE  = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

// GLOBAL DECLARATIONS
/** @brief Global database instance used by the interpreter */
extern Database db;
/** @brief Global container for query result rows */
extern std::vector<std::vector<std::string>> gui_results;
/** @brief Global container for query result column headers */
extern std::vector<std::string> gui_headers;
/** @brief Global string for storing the last execution error message */
extern std::string gui_error;
/** @brief Global log containing the history of executed queries and results */
extern std::vector<std::string> gui_log;

/**
 * @brief External declaration of the SQL execution engine.
 * @param input The raw SQL query string to be parsed and executed.
 */
void executeSQL(const char* input);

/**
 * @class Utils
 * @brief Utility class providing UI and CLI helper functions.
 */
class Utils {
public:
    /**
     * @brief Configures modern dark theme colors and rounding for Dear ImGui.
     */
    static void applyModernTheme();

    /**
     * @brief Renders SQL text with syntax highlighting using regular expressions.
     * @param text The SQL query or snippet to render.
     * @param isGui If true, uses ImGui::TextColored; if false, uses terminal ANSI codes.
     */
    static void renderColoredSQL(const std::string& text, bool isGui);
    /**
     * @brief Formats and prints the current query results (gui_results) to the terminal.
     * * Creates an ASCII table with dynamically adjusted column widths.
     */
    static void printTableCLI();

    /**
     * @brief Starts the Command Line Interface loop.
     * * Handles user input from stdin and coordinates with the execution engine.
     */
    static void runCLI();
};

#endif