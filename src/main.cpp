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

    // setting up the window
    GLFWwindow* window = glfwCreateWindow(1280, 720, "sqW", NULL, NULL);

    // all future new looks will be added to thsi window
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // loading the UI library and  connect it to the window (GLFW) and the drawing engine (OpenGL3).
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::GetIO().FontGlobalScale = 1.4f;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 150");

    char sql_input[10024] = "";

    // MAIN CONSOLE layout
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents(); // checks for mouse clicks
        ImGui_ImplOpenGL3_NewFrame(); ImGui_ImplGlfw_NewFrame(); ImGui::NewFrame();

        // starting new UI frame
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGui::Begin("MainLayout", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

        // dividing UI into two columns: Workspace and Database schema
        ImGui::Columns(2, "MainColumns", true);
        ImGui::SetColumnWidth(0, ImGui::GetIO().DisplaySize.x * 0.75f);

        ImGui::BeginChild("WorkspaceChild");

        ImGui::Text("sqW Terminal (Press CTRL+ENTER to execute) >");

        ImGuiIO& io = ImGui::GetIO();

        // CTRL + ENTER for fast execution of the query
        bool ctrl_pressed = io.KeyCtrl || io.KeySuper;
        bool enter_pressed = ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter);
        bool shortcut_pressed = ctrl_pressed && enter_pressed;

        ImGui::SetWindowFontScale(1.2f);
        float input_height = ImGui::GetIO().DisplaySize.y * 0.30f;

        // input id, height and width, max input size, tab allowed in writing queries
        ImGui::InputTextMultiline("##query", sql_input, IM_ARRAYSIZE(sql_input), ImVec2(-FLT_MIN, input_height), ImGuiInputTextFlags_AllowTabInput);

        // shortcut will be executed only if our cursor is in input box (so we are currently writing something)
        bool is_input_focused = ImGui::IsItemFocused();

        // if triggered
        if (ImGui::Button("Execute Query", ImVec2(-FLT_MIN, 45)) || (is_input_focused && shortcut_pressed)) {
            if (strlen(sql_input) > 0) {
                executeSQL(sql_input);
                // memset(sql_input, 0, sizeof(sql_input)); // after exxecution we clear our terminal
                ImGui::SetKeyboardFocusHere(-1); // going back to the input box
            }
        }

        // HISTORY LOG layout
        ImGui::Separator();
        ImGui::Text("History Log:");
        ImGui::BeginChild("LogRegion", ImVec2(0, ImGui::GetIO().DisplaySize.y * 0.25f), true);
        for (const auto& log_line : gui_log) {
            if (log_line.find("Executing:") != std::string::npos) {
                ImGui::TextColored(GUI_PINK, "Executing: ");
                ImGui::SameLine();
                Utils::renderColoredSQL(log_line.substr(11), true);
            } else if (log_line.find("Error") != std::string::npos) {
                ImGui::TextColored(GUI_RED, "%s", log_line.c_str());
            } else {
                ImGui::TextUnformatted(log_line.c_str());
            }
        }
        ImGui::EndChild();

        // TABLE RESULTS
        ImGui::Separator();
        ImGui::Text("Table Results:");
        if (!gui_headers.empty() && ImGui::BeginTable("ResultsTable", (int)gui_headers.size(), ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable)) {
            for (const auto& h : gui_headers) ImGui::TableSetupColumn(h.c_str());
            ImGui::TableHeadersRow();
            for (const auto& row : gui_results) {
                ImGui::TableNextRow();
                for (const auto& cell : row) {
                    ImGui::TableNextColumn();
                    ImGui::Text("%s", cell.c_str());
                }
            }
            ImGui::EndTable();
        }
        ImGui::EndChild();

        // DATABASE SCHEMA layout
        ImGui::NextColumn();
        ImGui::BeginChild("SchemaBrowser");
        ImGui::TextColored(ImVec4(0.2f, 0.3f, 0.55f, 1.0f), "Database schema");
        ImGui::Separator();

        auto tableNames = db.getTableNames();
        for (const auto& tName : tableNames) {
            if (ImGui::CollapsingHeader(tName.c_str())) {
                try {
                    Table& t = db.getTable(tName);
                    for (const auto& col : t.getColumns()) {
                        ImGui::BulletText("%s (%s)", col.getName().c_str(), col.getTypeString().c_str());
                    }
                } catch (...) {}
            }
        }

        // EXIT BUTTON
        ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 60);
        ImGui::Separator();
        if (ImGui::Button("Exit Program", ImVec2(-FLT_MIN, 40))) {
            glfwSetWindowShouldClose(window, true);
        }

        ImGui::EndChild();
        ImGui::End();

        // prepares the UI data for drawing
        ImGui::Render();
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f); glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        // show the finshed frame
        glfwSwapBuffers(window);
    }

    // cleanup the whole window before exiting
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    db.saveToFile(DB_FILENAME);
    return 0;
}