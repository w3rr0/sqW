#include "gui/Utils.hpp"
#include <regex>
#include "imgui.h"
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <filesystem>
#include <sstream>
#include <regex>

void Utils::applyModernTheme() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // Smooth rounded corners and modern borders
    style.WindowRounding    = 8.0f;
    style.ChildRounding     = 6.0f;
    style.FrameRounding     = 5.0f;
    style.PopupRounding     = 6.0f;
    style.ScrollbarRounding = 9.0f;
    style.GrabRounding      = 5.0f;
    style.TabRounding       = 6.0f;

    style.WindowBorderSize  = 1.0f;
    style.ChildBorderSize   = 1.0f;
    style.PopupBorderSize   = 1.0f;
    style.FrameBorderSize   = 1.0f;
    style.TabBorderSize     = 0.0f;

    style.WindowPadding     = ImVec2(12.0f, 12.0f);
    style.FramePadding      = ImVec2(10.0f, 6.0f);
    style.ItemSpacing       = ImVec2(10.0f, 8.0f);
    style.ItemInnerSpacing  = ImVec2(8.0f, 6.0f);
    style.ScrollbarSize     = 14.0f;

    // Palette: Sleek Developer Dark (Tokyo Night / Catppuccin inspired)
    colors[ImGuiCol_Text]                  = ImVec4(0.92f, 0.93f, 0.96f, 1.00f);
    colors[ImGuiCol_TextDisabled]          = ImVec4(0.50f, 0.53f, 0.62f, 1.00f);
    colors[ImGuiCol_WindowBg]              = ImVec4(0.11f, 0.12f, 0.16f, 1.00f);
    colors[ImGuiCol_ChildBg]               = ImVec4(0.14f, 0.15f, 0.20f, 1.00f);
    colors[ImGuiCol_PopupBg]               = ImVec4(0.14f, 0.15f, 0.20f, 0.98f);
    colors[ImGuiCol_Border]                = ImVec4(0.24f, 0.26f, 0.35f, 0.80f);
    colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]               = ImVec4(0.18f, 0.20f, 0.28f, 1.00f);
    colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.24f, 0.27f, 0.38f, 1.00f);
    colors[ImGuiCol_FrameBgActive]         = ImVec4(0.28f, 0.31f, 0.44f, 1.00f);
    colors[ImGuiCol_TitleBg]               = ImVec4(0.11f, 0.12f, 0.16f, 1.00f);
    colors[ImGuiCol_TitleBgActive]         = ImVec4(0.16f, 0.18f, 0.25f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.11f, 0.12f, 0.16f, 0.75f);
    colors[ImGuiCol_MenuBarBg]             = ImVec4(0.14f, 0.15f, 0.20f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.11f, 0.12f, 0.16f, 0.50f);
    colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.26f, 0.29f, 0.40f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.34f, 0.38f, 0.52f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.40f, 0.45f, 0.62f, 1.00f);
    colors[ImGuiCol_CheckMark]             = ImVec4(0.45f, 0.65f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrab]            = ImVec4(0.45f, 0.65f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.55f, 0.75f, 1.00f, 1.00f);

    // Primary Buttons: Accent Indigo / Modern Blue
    colors[ImGuiCol_Button]                = ImVec4(0.30f, 0.42f, 0.80f, 1.00f);
    colors[ImGuiCol_ButtonHovered]         = ImVec4(0.38f, 0.50f, 0.90f, 1.00f);
    colors[ImGuiCol_ButtonActive]          = ImVec4(0.24f, 0.35f, 0.70f, 1.00f);

    // Headers (CollapsingHeader, TreeNodes)
    colors[ImGuiCol_Header]                = ImVec4(0.20f, 0.23f, 0.34f, 1.00f);
    colors[ImGuiCol_HeaderHovered]         = ImVec4(0.26f, 0.30f, 0.44f, 1.00f);
    colors[ImGuiCol_HeaderActive]          = ImVec4(0.32f, 0.37f, 0.54f, 1.00f);

    // Separators
    colors[ImGuiCol_Separator]             = ImVec4(0.24f, 0.26f, 0.35f, 0.80f);
    colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.38f, 0.42f, 0.58f, 1.00f);
    colors[ImGuiCol_SeparatorActive]       = ImVec4(0.45f, 0.50f, 0.70f, 1.00f);

    // Tabs
    colors[ImGuiCol_Tab]                   = ImVec4(0.16f, 0.18f, 0.25f, 1.00f);
    colors[ImGuiCol_TabHovered]            = ImVec4(0.28f, 0.33f, 0.48f, 1.00f);
    colors[ImGuiCol_TabActive]             = ImVec4(0.22f, 0.26f, 0.40f, 1.00f);
    colors[ImGuiCol_TabUnfocused]          = ImVec4(0.14f, 0.15f, 0.20f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.18f, 0.20f, 0.28f, 1.00f);

    // Tables
    colors[ImGuiCol_TableHeaderBg]         = ImVec4(0.18f, 0.20f, 0.28f, 1.00f);
    colors[ImGuiCol_TableBorderStrong]     = ImVec4(0.26f, 0.29f, 0.40f, 1.00f);
    colors[ImGuiCol_TableBorderLight]      = ImVec4(0.20f, 0.22f, 0.30f, 1.00f);
    colors[ImGuiCol_TableRowBg]            = ImVec4(0.14f, 0.15f, 0.20f, 1.00f);
    colors[ImGuiCol_TableRowBgAlt]         = ImVec4(0.16f, 0.17f, 0.23f, 1.00f);
}

// method taht renders SQL text with syntax highlighting using Regex
void Utils::renderColoredSQL(const std::string& text, bool isGui) {
    // sql keywords
    static std::regex regKeywords("\\b(SELECT|FROM|WHERE|CREATE|TABLE|INSERT|INTO|VALUES|UPDATE|DELETE|SET|DROP|"
                                  "COLUMN|ALTER|ADD|LIMIT|OFFSET""|DISTINCT|IS|NOT|AND|OR|ORDER|BY|ASC|DESC|BETWEEN|"
                                  "LIKE|IN|COUNT|MIN|MAX|SUM|SHOW|TABLES)\\b", std::regex_constants::icase);
    // sql data types
    static std::regex regTypes("\\b(INT|INTEGER|STRING|TEXT|VARCHAR|BOOL|BOOLEAN|DOUBLE|FLOAT|NULL)\\b",
                                    std::regex_constants::icase);
    // numbers
    static std::regex regNumbers("\\b\\d+(\\.\\d+)?\\b");
    // strings
    static std::regex regStrings("('\\s*[^']*\\s*'|\\\"\\s*[^\\\"]*\\s*\\\")");
    // connectting words into a sentence
    static std::regex wordRegex("([\\w*]+|'[^']*'|\\\"[^\\\"]*\\\"|[,;\\(\\)=<>!\\.]+|[ ]+)");

    auto words_begin = std::sregex_iterator(text.begin(), text.end(), wordRegex);
    auto words_end = std::sregex_iterator();

    for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
        std::string part = i->str();
        ImVec4 color = GUI_WHITE;
        std::string cliColor = CLI_WHITE;

        // applying correct colors based on the token type
        if (std::regex_match(part, regKeywords)) { color = GUI_RED; cliColor = CLI_RED; }
        else if (std::regex_match(part, regTypes)) { color = GUI_PINK; cliColor = CLI_PINK; }
        else if (std::regex_match(part, regNumbers)) { color = GUI_YELLOW; cliColor = CLI_YELLOW; }
        else if (std::regex_match(part, regStrings)) { color = GUI_GREEN; cliColor = CLI_GREEN; }

        if (isGui) {
            if (part[0] == ' ') {
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::CalcTextSize(part.c_str()).x);
            } else {
                ImGui::TextColored(color, "%s", part.c_str());
                ImGui::SameLine(0, 0); // keeps symbols like '(' or '*' with no space
            }
        } else {
            std::cout << cliColor << part << CLI_RESET;
        }
    }
    if (isGui) ImGui::NewLine(); else std::cout << std::flush;
}

// method for formatting and printing table results in console
void Utils::printTableCLI() {
    if (gui_headers.empty()) return;
    std::vector<size_t> colWidths(gui_headers.size());
    for (size_t i = 0; i < gui_headers.size(); ++i) {
        colWidths[i] = gui_headers[i].length();
        for (const auto& row : gui_results) {
            if (i < row.size()) {
                colWidths[i] = std::max(colWidths[i], row[i].length());
            }
        }
    }
    auto sep = [&]() {
        std::cout << "+";
        for (size_t w : colWidths) std::cout << std::string(w + 2, '-') << "+";
        std::cout << std::endl;
    };
    sep();
    std::cout << "|";
    for (size_t i = 0; i < gui_headers.size(); ++i) {
        std::cout << " " << std::left << std::setw(colWidths[i]) << gui_headers[i] << " |";
    }
    std::cout << std::endl;
    sep();
    for (const auto& row : gui_results) {
        std::cout << "|";
        for (size_t i = 0; i < row.size(); ++i) {
            std::cout << " " << std::left << std::setw(colWidths[i]) << row[i] << " |";
        }
        std::cout << std::endl;
    }
    sep();
}

// method with CLI loop
void Utils::runCLI() {
    std::cout << CLI_PINK << "SQL CLI Mode " << CLI_RESET;
    std::cout << CLI_WHITE << "(type 'exit' to quit)" << CLI_RESET << std::endl;

    char line[10024];
    // buffer for queries continued after 'ENTER'
    std::string queryEnter= "";

    std::cout << CLI_PINK << "Available tables: " << CLI_RESET;
    std::cout << CLI_WHITE << "(type 'show tables' to see)\n" << CLI_RESET;


    while (true) {
        // if is empty start of te line will be 'sqW' otherwise if we will be finishing the query in new line '->'
        if (queryEnter.empty()) {
            std::cout << CLI_WHITE << "sqW> " << CLI_RESET;
        } else {
            std::cout << CLI_WHITE << "  -> " << CLI_RESET;
        }

        if (!std::cin.getline(line, sizeof(line))) break;
        if (std::string(line) == "exit") break;
        if (strlen(line) == 0) continue;

        // adding line with space char to the buffer
        queryEnter += std::string(line) + " ";

        std::string lowerInput = line;
        for (auto & c : lowerInput) {
            c = (unsigned char)std::tolower(c);
        }

        if (lowerInput == "show tables") {
            auto names = db.getTableNames();
            if(names.empty()) {
                std::cout << "[No tables]";
            } else {
                for (const auto& n : names) {
                    std::cout << "[" << n << "] ";
                }
            }
            std::cout << "\n";
            // clearing buffer after finished query
            queryEnter = "";
            continue;
        }

        // if we find ';' in our buffer than the query should be ended after it and then optional errors may appear
        if (queryEnter.find(';') != std::string::npos) {
            std::cout << CLI_PINK << "Executing: " << CLI_RESET;
            renderColoredSQL(queryEnter, false);
            std::cout << std::endl;

            executeSQL(queryEnter.c_str());

            // log and table display
            if (!gui_log.empty()) {
                std::string last = gui_log.back();
                if (last.find("Error") != std::string::npos) {
                    std::cout << CLI_RED << last << CLI_RESET << std::endl;
                } else {
                    std::cout << CLI_GREEN << last << CLI_RESET << std::endl;
                }
            }
            if (!gui_results.empty()) {
                printTableCLI();
            }

            std::cout << std::endl;
            queryEnter = ""; // reseting buffer for new query
            gui_log.clear();
        }
    }
}