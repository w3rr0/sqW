#include <gtest/gtest.h>
#include "ast/SelectStmt.hpp"
#include "ast/WhereClause.hpp"
#include "storage/Database.hpp"
#include <vector>
#include <string>

extern Database db;
extern std::vector<std::string> gui_log;
extern std::vector<std::vector<std::string>> gui_results;
extern std::vector<std::string> gui_headers;
extern std::string gui_error;

// helper setup class that creates a table to test
class SelectStmtExecutionTest : public ::testing::Test {
protected:
    void SetUp() override {
        for (const auto& name : db.getTableNames()) {
            db.dropTable(name);
        }
        gui_log.clear();
        gui_results.clear();
        gui_headers.clear();

        db.createTable("employees");
        Table& t = db.getTable("employees");
        t.addColumn("id", Cell::INT);
        t.addColumn("name", Cell::TEXT);
        t.addColumn("salary", Cell::DOUBLE);

        // inserting new rows for SELECT tests
        t.insertRow({Cell(1), Cell(std::string("Adam")), Cell(5000.0)});
        t.insertRow({Cell(2), Cell(std::string("Ewa")),  Cell(6000.0)});
        t.insertRow({Cell(3), Cell(std::string("Adam")), Cell(4500.0)}); // name duplicate for tetsing DISTINCT
        t.insertRow({Cell(4), Cell(std::string("Jan")), Cell(500.0)});
        t.insertRow({Cell(5), Cell(std::string("Maja")), Cell(2000.0)});
    }
};

// CONSTRUCTOR tests
TEST_F(SelectStmtExecutionTest, ConstructorStoresDataCorrectly) {
    std::vector<std::string> cols = {"name", "salary"};
    SelectStmt stmt("employees", cols, true); // true = isDistinct

    EXPECT_EQ(stmt.getTableName(), "employees");
    EXPECT_EQ(stmt.getColumns().size(), 2);
    EXPECT_TRUE(stmt.getDistinct()); // select is DISTINCT
}


// EXECUTE tests
TEST_F(SelectStmtExecutionTest, ExecuteSelectStar) {
    SelectStmt stmt("employees", {"*"});
    stmt.execute();

    // diplaying everything so all rows and cols
    EXPECT_EQ(gui_headers.size(), 3); // id, name, salary headers
    EXPECT_EQ(gui_results.size(), 5); // 5 rows
}

// WHERE conditions
TEST_F(SelectStmtExecutionTest, ExecuteSelectWithWhere) {
    SelectStmt stmt("employees", {"name"});
    auto cond = std::make_unique<ComparisonCondition>("salary", ">", "5000");
    stmt.setWhere(std::move(cond));
    stmt.execute();

    ASSERT_EQ(gui_results.size(), 1); // only Ewa > 5000
    EXPECT_EQ(gui_results[0][0], "Ewa"); // only name to display so 'Ewa' name
}

// DISTINCT tests
TEST_F(SelectStmtExecutionTest, ExecuteSelectDistinct) {
    SelectStmt stmt("employees", {"name"}, true); // isDistinct = true
    stmt.execute();

    // two times 'Adam' in the table so without one will be 4 rows
    EXPECT_EQ(gui_results.size(), 4);
}

// ORDER BY
TEST_F(SelectStmtExecutionTest, ExecuteOrderBySalaryDesc) {
    // only name col to display
    SelectStmt stmt("employees", {"name"});
    stmt.setOrder("salary", false); // DESC -> false

    stmt.execute();

    ASSERT_EQ(gui_results.size(), 5);
    EXPECT_EQ(gui_results[0][0], "Ewa");  // 6000.0
    EXPECT_EQ(gui_results[1][0], "Adam"); // 5000.0
    EXPECT_EQ(gui_results[2][0], "Adam"); // 4500.0
    EXPECT_EQ(gui_results[3][0], "Maja"); // 2000.0
    EXPECT_EQ(gui_results[4][0], "Jan"); // 500.0
}

// AGGREAGTION: SUM, COUNT, MIN, MAX tests
TEST_F(SelectStmtExecutionTest, ExecuteAggregationSum) {
    // no other cols to display becuase of no 'group by' added yet
    SelectStmt stmt("employees", {});
    stmt.setAggregate(SelectStmt::Aggregate::SUM, "salary");

    stmt.execute();

    // RESULT display in aggregation function
    ASSERT_EQ(gui_headers[0], "RESULT");
    // sum of salaries is 18 000 so we are looking for it in gui results display
    EXPECT_NE(gui_results[0][0].find("18000"), std::string::npos);
}

TEST_F(SelectStmtExecutionTest, ExecuteAggregationCountStar) {
    SelectStmt stmt("employees", {});
    stmt.setAggregate(SelectStmt::Aggregate::COUNT, "*");

    stmt.execute();

    // std::string::npos -> not found so it should not equal
    EXPECT_NE(gui_results[0][0].find("5"), std::string::npos); // rows count becuase count(*)
}

TEST_F(SelectStmtExecutionTest, ExecuteAggregationMin) {
    SelectStmt stmt("employees", {});
    stmt.setAggregate(SelectStmt::Aggregate::MIN, "salary");

    stmt.execute();

    // MIN salary is 500 so we are looking for it in gui results
    EXPECT_NE(gui_results[0][0].find("500"), std::string::npos); // MIN salary
}

TEST_F(SelectStmtExecutionTest, ExecuteAggregationMax) {
    SelectStmt stmt("employees", {});
    stmt.setAggregate(SelectStmt::Aggregate::MAX, "salary");

    stmt.execute();

    // MAX salary is 6000 so we are looking for it in gui results
    EXPECT_NE(gui_results[0][0].find("6000"), std::string::npos); // MAX salary
}

// LIMIT and OFFSET tests
TEST_F(SelectStmtExecutionTest, ExecuteLimitOffset) {
    SelectStmt stmt("employees", {"id", "salary"});
    stmt.limit = 1; // only one to display
    stmt.offset = 3; // skipping first three rows so fourth one should be displayed

    stmt.execute();

    ASSERT_EQ(gui_results.size(), 1);
    EXPECT_EQ(gui_results[0][0], "4"); // id = 4 for fourth row
    EXPECT_EQ(gui_results[0][1], "500"); // salary = 500.0 for fourth row
}

// no columns error tests
TEST_F(SelectStmtExecutionTest, ExecuteLogsErrorForMissingColumn) {
    SelectStmt stmt("employees", {"non_existent_column"});

    stmt.execute();

    // there is a gui error message so false -> empty
    EXPECT_FALSE(gui_error.empty());
    ASSERT_FALSE(gui_log.empty());
    EXPECT_NE(gui_log.back().find("SELECT failed"), std::string::npos);
}

TEST_F(SelectStmtExecutionTest, ExecuteAggregationAvg) {
    SelectStmt stmt("employees", {});
    stmt.setAggregate(SelectStmt::Aggregate::AVG, "salary");

    stmt.execute();

    ASSERT_EQ(gui_headers[0], "RESULT");
    // sum is 18000 across 5 employees, avg is 3600
    EXPECT_NE(gui_results[0][0].find("3600"), std::string::npos);
}

TEST_F(SelectStmtExecutionTest, ExecuteMultiColumnOrderBy) {
    SelectStmt stmt("employees", {"name", "salary"});
    stmt.setOrderList({{"name", true}, {"salary", false}}); // name ASC, salary DESC

    stmt.execute();

    ASSERT_EQ(gui_results.size(), 5);
    // Adam (5000) should come before Adam (4500)
    EXPECT_EQ(gui_results[0][0], "Adam");
    EXPECT_EQ(gui_results[0][1], "5000");

    EXPECT_EQ(gui_results[1][0], "Adam");
    EXPECT_EQ(gui_results[1][1], "4500");

    EXPECT_EQ(gui_results[2][0], "Ewa");
    EXPECT_EQ(gui_results[3][0], "Jan");
    EXPECT_EQ(gui_results[4][0], "Maja");
}

TEST_F(SelectStmtExecutionTest, ExecuteAggregatesIgnoreNulls) {
    db.createTable("nullable_table");
    Table& t = db.getTable("nullable_table");
    t.addColumn("val", Cell::DOUBLE);

    // Row 0 is NULL (crucial test for MIN/MAX initialization)
    t.insertRow({Cell()});
    t.insertRow({Cell(100.0)});
    t.insertRow({Cell()});
    t.insertRow({Cell(300.0)});

    // COUNT(*) should be 4
    SelectStmt countStar("nullable_table", {});
    countStar.setAggregate(SelectStmt::Aggregate::COUNT, "*");
    countStar.execute();
    EXPECT_NE(gui_results[0][0].find("4"), std::string::npos);

    // COUNT(val) should be 2 (ignoring the 2 NULLs)
    SelectStmt countCol("nullable_table", {});
    countCol.setAggregate(SelectStmt::Aggregate::COUNT, "val");
    countCol.execute();
    EXPECT_NE(gui_results[0][0].find("2"), std::string::npos);

    // SUM(val) should be 400
    SelectStmt sumStmt("nullable_table", {});
    sumStmt.setAggregate(SelectStmt::Aggregate::SUM, "val");
    sumStmt.execute();
    EXPECT_NE(gui_results[0][0].find("400"), std::string::npos);

    // AVG(val) should be 200 (400 / 2)
    SelectStmt avgStmt("nullable_table", {});
    avgStmt.setAggregate(SelectStmt::Aggregate::AVG, "val");
    avgStmt.execute();
    EXPECT_NE(gui_results[0][0].find("200"), std::string::npos);

    // MIN(val) should be 100, not NULL
    SelectStmt minStmt("nullable_table", {});
    minStmt.setAggregate(SelectStmt::Aggregate::MIN, "val");
    minStmt.execute();
    EXPECT_NE(gui_results[0][0].find("100"), std::string::npos);

    // MAX(val) should be 300
    SelectStmt maxStmt("nullable_table", {});
    maxStmt.setAggregate(SelectStmt::Aggregate::MAX, "val");
    maxStmt.execute();
    EXPECT_NE(gui_results[0][0].find("300"), std::string::npos);
}