#include <iostream>
#include <stdexcept>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>

using namespace std; // Use standard namespace for console I/O
using namespace sql; // Use MySQL connector namespace

// Function to execute and display results of a SQL query
void runQuery(Statement* stmt, const string& query) {
    try {
        ResultSet* res = stmt->executeQuery(query);     // Execute the SQL query and get result set
        
        ResultSetMetaData* meta = res->getMetaData();   // Get metadata about the result (column names/types)
        
        int columns = meta->getColumnCount();           // Get number of columns in the result
        
        // While there are results loop through each row
        while (res->next()) {
            // Loop through each column in current row
            for (int i = 1; i <= columns; ++i)
                cout << meta->getColumnLabel(i) << ": " << res->getString(i) << "\t";   // Print column name and value separated by tab
            cout << endl; // New line after each row
        }
        delete res; // delete the result when finished
    } catch (SQLException &e) {
        cerr << "Query failed: " << e.what() << endl;   // Handles SQL errors during query execution
    }
}

int main() {
    try {
        mysql::MySQL_Driver* driver = mysql::get_mysql_driver_instance();   // storing the sql driver instance
        
        // Connecting to the SQL server
        Connection* con = driver->connect("tcp://mysql.eecs.ku.edu", "348s25_a734b348", "thoz9Joo");
        
        con->setSchema("348s25_a734b348");  // Selecting which database to use

        // Creating a statement for executing the queries
        Statement* stmt = con->createStatement();

        // Query 1: Retrieve students with a GPA higher than 3.7. Display First Name, Last Name, City, and GPA.
        cout << "1. Students with GPA higher than 3.7:\n";
        runQuery(stmt, "SELECT StdFirstName, StdLastName, StdCity, StdGPA FROM Student WHERE StdGPA > 3.7");

        // Query 2: Display First Name, Last Name, and GPA of students in descending order of GPA.
        cout << "\n2. Students ordered by GPA descending:\n";
        runQuery(stmt, "SELECT StdFirstName, StdLastName, StdGPA FROM Student ORDER BY StdGPA DESC");

        // Query 3: Retrieve all information about senior-level IS courses (IS-4XX).
        cout << "\n3. Senior-level IS courses (IS-4XX):\n";
        runQuery(stmt, "SELECT * FROM Course WHERE CourseNo LIKE 'IS-4%'");

        // Query 4: List offerings without an instructor in the summer.
        cout << "\n4. Offerings without an instructor in summer:\n";
        runQuery(stmt, "SELECT OfferNo, CourseNo FROM Offering WHERE OffTerm = 'Summer' AND FacNo IS NULL");

        // Query 5: Display Offering number, course number, Faculty First and Last Name for courses offered in spring, faculty rank as assistant (ASST), and faculty number the same as offering faculty number.
        cout << "\n5. Spring courses offered by Assistant Professors:\n";
        runQuery(stmt, "SELECT Offering.OfferNo, Offering.CourseNo, Faculty.FacFirstName, Faculty.FacLastName "
                       "FROM Offering "
                       "JOIN Faculty ON Offering.FacNo = Faculty.FacNo "
                       "WHERE Offering.OffTerm = 'Spring' AND Faculty.FacRank = 'ASST' AND Offering.FacNo = Faculty.FacNo");

        // Query 6: Display distinct Faculty numbers in Offering Table.
        cout << "\n6. Distinct Faculty numbers in Offering Table:\n";
        runQuery(stmt, "SELECT DISTINCT FacNo FROM Offering");

        // Query 7: Display First Names of Students that live in Bothell and Faculty that live in Bellevue with UNION.
        cout << "\n7. Students in Bothell and Faculty in Bellevue:\n";
        runQuery(stmt, "SELECT StdFirstName FROM Student WHERE StdCity = 'Bothell' "
                       "UNION "
                       "SELECT FacFirstName FROM Faculty WHERE FacCity = 'Bellevue'");

        // Query 8: Insert Finance Freshman "John Doe" and show the result using SELECT * FROM Student.
        cout << "\n8. Insert John Doe into Student table:\n";
        stmt->execute("INSERT INTO Student (StdNo, StdFirstName, StdLastName, StdCity, StdState, StdZip, StdMajor, StdClass, StdGPA) "
                       "VALUES ('999-99-9999', 'John', 'Doe', 'Lawrence', 'KS', '66045', 'Finance', 'Freshman', 4.0)");
        runQuery(stmt, "SELECT * FROM Student WHERE StdNo = '999-99-9999'");

        // Query 9: Homer Wells changes his major to Accounting (Acct). Update and show the result using SELECT * FROM Student.
        cout << "\n9. Update Homer Wells major to Accounting:\n";
        stmt->execute("UPDATE Student SET StdMajor = 'Acct' WHERE StdFirstName = 'Homer' AND StdLastName = 'Wells'");
        runQuery(stmt, "SELECT * FROM Student WHERE StdFirstName = 'Homer' AND StdLastName = 'Wells'");

        // Query 10: List the students that are also on the faculty. Display all the information of such students.
        cout << "\n10. Students that are also faculty:\n";
        runQuery(stmt, "SELECT * FROM Student WHERE StdNo IN (SELECT FacNo FROM Faculty)");

        // Cleaning the objects created earlier in the program
        delete stmt; // Delete statement object
        delete con;  // Close database connection

    } catch (SQLException &e) {
        // Handle any database connection errors
        cerr << "MySQL error: " << e.what() << endl;
    }
    return 0; // Exit program
}