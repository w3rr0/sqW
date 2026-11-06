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
#define CLI_RED     "\033[31m"
#define CLI_PINK    "\033[35m"
#define CLI_YELLOW  "\033[33m"
#define CLI_GREEN   "\033[32m"
#define CLI_WHITE   "\033[37m"
#define CLI_RESET   "\033[0m"

// IMGUI colors in GUI for syntax highlighting (fallback defaults)
const ImVec4 GUI_RED    = ImVec4(1.0f, 0.3f, 0.3f, 1.0f);
const ImVec4 GUI_PINK   = ImVec4(1.0f, 0.4f, 0.7f, 1.0f);
const ImVec4 GUI_YELLOW = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
const ImVec4 GUI_GREEN  = ImVec4(0.3f, 1.0f, 0.3f, 1.0f);
const ImVec4 GUI_WHITE  = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

// GLOBAL DECLARATIONS
extern Database db;
extern std::vector<std::vector<std::string>> gui_results;
extern std::vector<std::string> gui_headers;
extern std::string gui_error;
extern std::vector<std::string> gui_log;

void executeSQL(const char* input);

class Utils {
public:
    enum class Theme {
        ModernDark = 0,     ///< Sleek Tokyo Night / Catppuccin Dark
        ClassicLight = 1,   ///< Minimalist Editorial Light (Teapot inspired)
        WarmAmber = 2,      ///< Fleet Health / Obsidian Warm Amber
        MidnightPurple = 3  ///< Cosmic Space Navy & Ethereal Indigo (Huly inspired)
    };

    struct ThemeColors {
        ImVec4 brand;          ///< Logo / Brand accent in header
        ImVec4 accent;         ///< Primary action button fill
        ImVec4 accentHovered;  ///< Primary action button hover state
        ImVec4 accentActive;   ///< Primary action button active state
        ImVec4 executeBtnText; ///< Text color on primary action button
        ImVec4 subtext;        ///< Secondary text, labels, hints
        ImVec4 statusOk;       ///< "Connected", success messages
        ImVec4 statusErr;      ///< Error alerts and badges
        ImVec4 kwColor;        ///< SQL keywords in syntax highlighting
        ImVec4 typeColor;      ///< SQL types in syntax highlighting
        ImVec4 numColor;       ///< Numbers in syntax highlighting
        ImVec4 strColor;       ///< Strings in syntax highlighting
    };

    static void applyModernTheme();
    static void applyTheme(Theme theme);
    static const ThemeColors& getCurrentThemeColors();
    static Theme getCurrentTheme();
    static void renderColoredSQL(const std::string& text, bool isGui);
    static void printTableCLI();
    static void runCLI();
};

#endif
