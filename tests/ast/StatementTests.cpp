#include <gtest/gtest.h>
#include <vector>
#include <string>
#include "storage/Database.hpp"

extern Database db;
extern std::vector<std::vector<std::string>> gui_results;
extern std::vector<std::string> gui_headers;
extern std::string gui_error;
extern std::vector<std::string> gui_log;
extern void executeSQL(const char* input);

class EndToEndSqlTest : public ::testing::Test {
protected:
    void SetUp() override {
        for (const auto& name : db.getTableNames()) {
            db.dropTable(name);
        }
        gui_log.clear();
        gui_results.clear();
        gui_headers.clear();
        gui_error.clear();
    }
};

TEST_F(EndToEndSqlTest, TestInsertWithColumnsAndAvgAndMultiOrderBy) {
    executeSQL("CREATE TABLE users (id INT, name STRING, points DOUBLE);");
    ASSERT_TRUE(db.hasTable("users"));

    // Insert with specified columns, omitting 'name' -> should be NULL
    executeSQL("INSERT INTO users (id, points) VALUES (1, 50.0), (2, 100.0), (3, NULL);");

    // Also insert a complete row
    executeSQL("INSERT INTO users VALUES (4, 'Anna', 100.0);");

    // 1. Check COUNT(*) vs COUNT(points) vs COUNT(name)
    executeSQL("SELECT COUNT(*) FROM users;");
    ASSERT_EQ(gui_results.size(), 1);
    EXPECT_NE(gui_results[0][0].find("4"), std::string::npos);

    executeSQL("SELECT COUNT(points) FROM users;");
    ASSERT_EQ(gui_results.size(), 1);
    EXPECT_NE(gui_results[0][0].find("3"), std::string::npos); // row 3 points is NULL

    executeSQL("SELECT COUNT(name) FROM users;");
    ASSERT_EQ(gui_results.size(), 1);
    EXPECT_NE(gui_results[0][0].find("1"), std::string::npos); // only Anna has a non-NULL name

    // 2. Check AVG(points) -> (50 + 100 + 100) / 3 = 83.3333
    executeSQL("SELECT AVG(points) FROM users;");
    ASSERT_EQ(gui_results.size(), 1);
    EXPECT_NE(gui_results[0][0].find("83.33"), std::string::npos);

    // 3. Check Multi-column ORDER BY: points DESC, id ASC
    executeSQL("SELECT id, points FROM users ORDER BY points DESC, id ASC;");
    ASSERT_EQ(gui_results.size(), 4);
    // points 100.0 (id=2), then points 100.0 (id=4), then points 50.0 (id=1), then points NULL (id=3)
    EXPECT_EQ(gui_results[0][0], "2");
    EXPECT_EQ(gui_results[1][0], "4");
    EXPECT_EQ(gui_results[2][0], "1");
    EXPECT_EQ(gui_results[3][0], "3");
}