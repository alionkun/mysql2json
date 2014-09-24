
#include <mysql++/mysql++.h>
#include <json/json.h>
using namespace std;

bool mysql2json(const mysqlpp::UseQueryResult& uqr, Json::Value& list, int count);

int main(int argc, char** argv)
{
	try {
		mysqlpp::Connection conn("test", "127.0.0.1", "root", "a", 3306);
		mysqlpp::Query query = conn.query();
		query << "select * from t_xstudent";
		mysqlpp::UseQueryResult uqr = query.use();
		if (!uqr) {
			cerr << "uqr is NULL" << endl;
			return 1;
		}
		Json::Value list;
		if (!mysql2json(uqr, list, 2)) {
			cerr << "mysql2json() failed" << endl;
			return 1;
		}
		cout << "the json string is: " << list.toStyledString();
	} catch (mysqlpp::Exception ex) {
		cerr << "mysql err: " << ex.what() << endl;	
		return 1;
	}
	return 0;
}
