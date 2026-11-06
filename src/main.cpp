#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <cfloat>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <filesystem>
#include <sstream>
#include <regex>
#include <chrono>
#include <iomanip>
#include "storage/Database.hpp"
#include "ast/Statement.hpp"
#include "gui/Utils.hpp"

// Flex / Bison declaration to connect the parser with cpp
extern int yyparse();                                       // fucntion taht starts the parsing process
extern void yyrestart(FILE*);                               // function that resets the lexer state
typedef struct yy_buffer_state *YY_BUFFER_STATE;            // Flex type for handling the buffers
extern YY_BUFFER_STATE yy_scan_string(const char * str);    // tells Flex to read from the string not a file
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);       // cleans up the memory used by string buffer
extern std::vector<Statement*> root_statements;             // global vector where Bison stores parsed SQL object (AST)
extern int yylineno;

// global data structures
std::string getDatabasePath() {
    const char* homeDir = std::getenv("HOME");
    if (!homeDir) return "database.bin";
    std::string sqwDir = std::string(homeDir) + "/.sqW";
    std::filesystem::create_directories(sqwDir);
    return sqwDir + "/database.bin";
}

const std::string DB_FILENAME = getDatabasePath();
Database db("test_database");
std::vector<std::vector<std::string>> gui_results;
std::vector<std::string> gui_headers;
std::string gui_error = "";
std::vector<std::string> gui_log;

// method that executes the ast statement
void executeSQL(const char* input) {
    gui_error = "";
    gui_results.clear();
    gui_headers.clear();
    gui_log.push_back("Executing: " + std::string(input));
    // clearing vector before next query
    root_statements.clear();

    yylineno = 1;

    // thsi creates a temporary buffer that allows Flex to read a string we wyped in the console in GUI
    YY_BUFFER_STATE buffer = yy_scan_string(input);
    if (yyparse() == 0) { // yyparse() function returs 0 when the SQL query is correct
        for (auto* stmt : root_statements) {
            if (stmt) {
                stmt->execute();
                delete stmt;
            }
        }
        root_statements.clear();
    } else {
        /* ignore */
    }
    yy_delete_buffer(buffer);
}

int main(int argc, char** argv) {
    try {
        if (std::filesystem::exists(DB_FILENAME)) {
            db.loadFromFile(DB_FILENAME);
        }
    } catch (...) {}

    bool use_gui = false;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--gui") use_gui = true;
    }
    // CLI version
    if (!use_gui){
        Utils::runCLI();
        db.saveToFile(DB_FILENAME);
        return 0;
    }

    // Window initializion GLFW (common features such as version, core profile)
    if (!glfwInit()) return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // required for macOS

    // setting up the window with modern title
    GLFWwindow* window = glfwCreateWindow(1360, 780, "sqW - SQL Database Studio", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Apply modern developer theme
    Utils::applyModernTheme();

    ImGuiIO& io = ImGui::GetIO();
    // Load crisp TrueType monospace font across macOS, Windows, and Linux
    const char* fontCandidates[] = {
        // macOS
        "/System/Library/Fonts/SFNSMono.ttf",
        "/System/Library/Fonts/Menlo.ttc",
        "/System/Library/Fonts/Monaco.ttf",
        // Windows
        "C:\\Windows\\Fonts\\consola.ttf",
        "C:\\Windows\\Fonts\\segoeui.ttf",
        // Linux
        "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf",
        "/usr/share/fonts/TTF/DejaVuSansMono.ttf",
        "/usr/share/fonts/truetype/ubuntu/UbuntuMono-R.ttf"
    };
    bool fontLoaded = false;
    for (const char* path : fontCandidates) {
        if (std::filesystem::exists(path)) {
            ImFont* f = io.Fonts->AddFontFromFileTTF(path, 16.0f);
            if (f) {
                fontLoaded = true;
                break;
            }
        }
    }
    if (!fontLoaded) {
        io.FontGlobalScale = 1.25f;
    }

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 150");

    char sql_input[10024] = "";
    char table_filter[128] = "";
    double lastExecTimeMs = 0.0;
    int lastRowCount = 0;
    bool hasExecuted = false;
    std::string saveStatusMsg = "";

    static int currentThemeIdx = 0;
    const char* themeNames[] = {
        "Modern Dark",
        "Classic Light",
        "Warm Amber",
        "Midnight Purple"
    };

    // MAIN CONSOLE loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Main fullscreen window
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(io.DisplaySize);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12.0f, 12.0f));
        ImGui::Begin("MainLayout", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                                            ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        const Utils::ThemeColors& tc = Utils::getCurrentThemeColors();

        // TOP HEADER BAR
        ImGui::AlignTextToFramePadding();
        ImGui::TextColored(tc.brand, "sqW");
        ImGui::SameLine();
        ImGui::AlignTextToFramePadding();
        ImGui::TextColored(tc.subtext, "Database: %s", db.getName().c_str());
        ImGui::SameLine();
        ImGui::AlignTextToFramePadding();
        ImGui::TextColored(tc.statusOk, "Connected");
        ImGui::SameLine();
        ImGui::AlignTextToFramePadding();
        ImGui::TextColored(tc.subtext, "| Theme:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(170.0f);
        if (ImGui::Combo("##ThemeSelector", &currentThemeIdx, themeNames, IM_ARRAYSIZE(themeNames))) {
            Utils::applyTheme(static_cast<Utils::Theme>(currentThemeIdx));
        }

        // Top-right template shortcuts (dynamically aligned to fit within the right boundary)
        const ImGuiStyle& style = ImGui::GetStyle();
        const ImVec2 szSelect = ImGui::CalcTextSize("+ SELECT", nullptr, true);
        const ImVec2 szInsert = ImGui::CalcTextSize("+ INSERT", nullptr, true);
        const ImVec2 szCreate = ImGui::CalcTextSize("+ CREATE", nullptr, true);
        const ImVec2 szClear  = ImGui::CalcTextSize("Clear Editor", nullptr, true);

        const float btnPadding = style.FramePadding.x * 2.0f;
        const float wSelect = szSelect.x + btnPadding;
        const float wInsert = szInsert.x + btnPadding;
        const float wCreate = szCreate.x + btnPadding;
        const float wClear  = szClear.x + btnPadding;
        const float totalButtonsWidth = wSelect + wInsert + wCreate + wClear + (style.ItemSpacing.x * 3.0f);

        float rightStartX = ImGui::GetWindowWidth() - style.WindowPadding.x - totalButtonsWidth;
        if (rightStartX > ImGui::GetCursorPosX() + style.ItemSpacing.x) {
            ImGui::SameLine(rightStartX);
        } else {
            ImGui::SameLine();
        }

        if (ImGui::Button("+ SELECT")) {
            auto names = db.getTableNames();
            std::string sampleTable = names.empty() ? "table_name" : names[0];
            std::string q = "SELECT * FROM " + sampleTable + " LIMIT 10;";
            strncpy(sql_input, q.c_str(), sizeof(sql_input) - 1);
        }
        ImGui::SameLine();
        if (ImGui::Button("+ INSERT")) {
            auto names = db.getTableNames();
            std::string sampleTable = names.empty() ? "table_name" : names[0];
            std::string q = "INSERT INTO " + sampleTable + " VALUES (...);";
            strncpy(sql_input, q.c_str(), sizeof(sql_input) - 1);
        }
        ImGui::SameLine();
        if (ImGui::Button("+ CREATE")) {
            std::string q = "CREATE TABLE new_table (id INT, name STRING, score DOUBLE);";
            strncpy(sql_input, q.c_str(), sizeof(sql_input) - 1);
        }
        ImGui::SameLine();
        if (ImGui::Button("Clear Editor")) {
            memset(sql_input, 0, sizeof(sql_input));
        }

        ImGui::Separator();
        ImGui::Spacing();

        // SPLIT INTO TWO MAIN PANELS
        ImGui::Columns(2, "MainColumns", true);
        ImGui::SetColumnWidth(0, io.DisplaySize.x * 0.73f);

        // LEFT PANEL: WORKSPACE (QUERY EDITOR & RESULTS & LOGS)
        ImGui::BeginChild("WorkspaceChild", ImVec2(0, 0), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        if (ImGui::BeginTabBar("MainTabs")) {
            // TAB 1: QUERY & RESULTS
            if (ImGui::BeginTabItem("Query & Results")) {
                ImGui::Spacing();
                ImGui::TextColored(tc.subtext, "SQL Query Input (Cmd+Enter to execute):");

                // Execute shortcut checking
                bool ctrl_pressed = io.KeyCtrl || io.KeySuper;
                bool enter_pressed = ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter);
                bool shortcut_pressed = ctrl_pressed && enter_pressed;

                float input_height = std::min(150.0f, io.DisplaySize.y * 0.22f);
                ImGui::InputTextMultiline("##query", sql_input, IM_ARRAYSIZE(sql_input),
                                          ImVec2(-FLT_MIN, input_height),
                                          ImGuiInputTextFlags_AllowTabInput);
                bool is_input_focused = ImGui::IsItemFocused();

                // Run Query Button (Accent Color)
                ImGui::Spacing();
                ImGui::PushStyleColor(ImGuiCol_Button, tc.accent);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, tc.accentHovered);
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, tc.accentActive);
                ImGui::PushStyleColor(ImGuiCol_Text, tc.executeBtnText);

                bool executeTriggered = ImGui::Button("Execute Query (Cmd+Enter)", ImVec2(-FLT_MIN, 32)) || (is_input_focused && shortcut_pressed);
                ImGui::PopStyleColor(4);

                if (executeTriggered && strlen(sql_input) > 0) {
                    auto tStart = std::chrono::high_resolution_clock::now();
                    executeSQL(sql_input);
                    auto tEnd = std::chrono::high_resolution_clock::now();
                    lastExecTimeMs = std::chrono::duration<double, std::milli>(tEnd - tStart).count();
                    lastRowCount = (int)gui_results.size();
                    hasExecuted = true;
                    saveStatusMsg = "";
                    ImGui::SetKeyboardFocusHere(-1);
                }

                // STATUS BAR
                ImGui::Spacing();
                ImGui::Separator();
                if (hasExecuted) {
                    if (!gui_error.empty()) {
                        ImGui::TextColored(tc.statusErr, "Error: %s", gui_error.c_str());
                    } else {
                        ImGui::TextColored(tc.statusOk, "Success: %d rows returned", lastRowCount);
                        ImGui::SameLine();
                        ImGui::TextDisabled("(%.2f ms)", lastExecTimeMs);
                    }
                } else {
                    ImGui::TextDisabled("Ready for SQL queries.");
                }

                // CSV Export button on the right
                if (!gui_results.empty() && !gui_headers.empty()) {
                    ImGui::SameLine(ImGui::GetWindowWidth() - 170);
                    if (ImGui::Button("Copy as CSV")) {
                        std::string csv;
                        for (size_t i = 0; i < gui_headers.size(); ++i) {
                            csv += gui_headers[i] + (i + 1 < gui_headers.size() ? "," : "\n");
                        }
                        for (const auto& row : gui_results) {
                            for (size_t i = 0; i < row.size(); ++i) {
                                csv += row[i] + (i + 1 < row.size() ? "," : "\n");
                            }
                        }
                        ImGui::SetClipboardText(csv.c_str());
                    }
                }

                // TABLE RESULTS VIEW
                ImGui::Spacing();
                if (!gui_headers.empty()) {
                    if (ImGui::BeginTable("ResultsTable", (int)gui_headers.size(),
                                          ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY,
                                          ImVec2(0, -8.0f))) {
                        for (const auto& h : gui_headers) {
                            ImGui::TableSetupColumn(h.c_str(), ImGuiTableColumnFlags_WidthStretch);
                        }
                        ImGui::TableHeadersRow();

                        for (const auto& row : gui_results) {
                            ImGui::TableNextRow();
                            for (const auto& cell : row) {
                                ImGui::TableNextColumn();
                                if (cell == "NULL" || cell == "null") {
                                    ImGui::TextColored(tc.subtext, "[NULL]");
                                } else {
                                    ImGui::TextUnformatted(cell.c_str());
                                }
                            }
                        }
                        ImGui::EndTable();
                    }
                } else {
                    ImGui::TextDisabled("No table results to display yet.");
                }

                ImGui::EndTabItem();
            }

            // TAB 2: EXECUTION HISTORY LOG
            if (ImGui::BeginTabItem("Execution History")) {
                ImGui::Spacing();
                if (ImGui::Button("Clear History")) {
                    gui_log.clear();
                }
                ImGui::SameLine();
                ImGui::TextDisabled("(Click any previous query to load it into the editor)");
                ImGui::Separator();

                ImGui::BeginChild("LogRegion", ImVec2(0, -FLT_MIN), true);
                for (int i = (int)gui_log.size() - 1; i >= 0; --i) {
                    const auto& log_line = gui_log[i];
                    if (log_line.find("Executing:") != std::string::npos) {
                        std::string queryText = log_line.substr(11);
                        std::string label = "> " + queryText + "##" + std::to_string(i);

                        if (ImGui::Selectable(label.c_str(), false)) {
                            strncpy(sql_input, queryText.c_str(), sizeof(sql_input) - 1);
                        }
                        if (ImGui::IsItemHovered()) {
                            ImGui::SetTooltip("Click to load: %s", queryText.c_str());
                        }
                    } else if (log_line.find("Error") != std::string::npos) {
                        ImGui::TextColored(tc.statusErr, "[Error] %s", log_line.c_str());
                    } else {
                        ImGui::TextColored(tc.statusOk, "[OK] %s", log_line.c_str());
                    }
                    ImGui::Separator();
                }
                ImGui::EndChild();

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::EndChild(); // WorkspaceChild

        // RIGHT PANEL: DATABASE SCHEMA BROWSER
        ImGui::NextColumn();
        ImGui::BeginChild("SchemaBrowser", ImVec2(0, 0), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        ImGui::TextColored(tc.accent, "DATABASE SCHEMA");
        auto tableNames = db.getTableNames();
        ImGui::TextDisabled("Tables in database: %d", (int)tableNames.size());
        ImGui::Spacing();

        ImGui::InputTextWithHint("##filter", "Search tables...", table_filter, sizeof(table_filter));
        ImGui::Separator();

        // Calculate bottom buttons area height dynamically with comfortable padding
        float bottomAreaHeight = 110.0f;
        if (!saveStatusMsg.empty()) bottomAreaHeight += 24.0f;

        ImGui::BeginChild("TablesList", ImVec2(0, -bottomAreaHeight), false);
        for (const auto& tName : tableNames) {
            // Apply search filter if set
            if (strlen(table_filter) > 0) {
                std::string lowerTName = tName;
                std::string lowerFilter = table_filter;
                for (auto& c : lowerTName) c = (char)std::tolower(c);
                for (auto& c : lowerFilter) c = (char)std::tolower(c);
                if (lowerTName.find(lowerFilter) == std::string::npos) continue;
            }

            size_t rowCount = 0;
            size_t colCount = 0;
            try {
                const Table& t = db.getTable(tName);
                rowCount = t.getRowCount();
                colCount = t.getColumns().size();
            } catch (...) {}

            std::string headerLabel = tName + " (" + std::to_string(rowCount) + " rows)";

            if (ImGui::CollapsingHeader(headerLabel.c_str())) {
                try {
                    Table& t = db.getTable(tName);

                    // Quick table actions
                    std::string quickSelectId = "View Top 50##" + tName;
                    if (ImGui::Button(quickSelectId.c_str(), ImVec2(-FLT_MIN, 26))) {
                        std::string q = "SELECT * FROM " + tName + " LIMIT 50;";
                        strncpy(sql_input, q.c_str(), sizeof(sql_input) - 1);
                        executeSQL(sql_input);
                        hasExecuted = true;
                        lastRowCount = (int)gui_results.size();
                    }

                    ImGui::Spacing();
                    for (const auto& col : t.getColumns()) {
                        ImGui::Bullet();
                        ImGui::Text("%s", col.getName().c_str());
                        ImGui::SameLine();

                        // Colored type badge
                        std::string typeStr = col.getTypeString();
                        ImVec4 typeColor = ImVec4(0.5f, 0.7f, 1.0f, 1.0f); // default blue INT
                        if (typeStr == "DOUBLE" || typeStr == "FLOAT") typeColor = ImVec4(0.3f, 0.9f, 0.8f, 1.0f);
                        else if (typeStr == "TEXT" || typeStr == "STRING") typeColor = ImVec4(0.85f, 0.5f, 0.95f, 1.0f);
                        else if (typeStr == "BOOL") typeColor = ImVec4(1.0f, 0.65f, 0.35f, 1.0f);

                        ImGui::TextColored(typeColor, "[%s]", typeStr.c_str());
                    }
                    ImGui::Spacing();
                } catch (...) {}
            }

            // Context menu on right click of table header
            if (ImGui::BeginPopupContextItem(tName.c_str())) {
                if (ImGui::MenuItem("SELECT * (Limit 50)")) {
                    std::string q = "SELECT * FROM " + tName + " LIMIT 50;";
                    strncpy(sql_input, q.c_str(), sizeof(sql_input) - 1);
                    executeSQL(sql_input);
                    hasExecuted = true;
                    lastRowCount = (int)gui_results.size();
                }
                if (ImGui::MenuItem("SELECT COUNT(*)")) {
                    std::string q = "SELECT COUNT(*) FROM " + tName + ";";
                    strncpy(sql_input, q.c_str(), sizeof(sql_input) - 1);
                    executeSQL(sql_input);
                    hasExecuted = true;
                    lastRowCount = (int)gui_results.size();
                }
                if (ImGui::MenuItem("DROP TABLE")) {
                    std::string q = "DROP TABLE " + tName + ";";
                    strncpy(sql_input, q.c_str(), sizeof(sql_input) - 1);
                }
                ImGui::EndPopup();
            }
        }
        ImGui::EndChild(); // TablesList

        // BOTTOM ACTION BUTTONS
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (!saveStatusMsg.empty()) {
            ImGui::TextColored(tc.statusOk, "%s", saveStatusMsg.c_str());
            ImGui::Spacing();
        }
        if (ImGui::Button("Save Database", ImVec2(-FLT_MIN, 28))) {
            try {
                db.saveToFile(DB_FILENAME);
                saveStatusMsg = "Saved successfully!";
            } catch (const std::exception& e) {
                saveStatusMsg = std::string("Save failed: ") + e.what();
            }
        }

        ImGui::Spacing();
        if (ImGui::Button("Exit Studio", ImVec2(-FLT_MIN, 28))) {
            glfwSetWindowShouldClose(window, true);
        }
        ImGui::Dummy(ImVec2(0, 8.0f));

        ImGui::EndChild(); // SchemaBrowser

        ImGui::End(); // MainLayout
        ImGui::PopStyleVar();

        // Render frame
        ImGui::Render();
        const ImVec4& bg = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
        glClearColor(bg.x, bg.y, bg.z, bg.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // Cleanup before exiting
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    db.saveToFile(DB_FILENAME);
    return 0;
}