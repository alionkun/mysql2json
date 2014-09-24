#include <mysql++/mysql++.h>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <json/json.h>


bool mysql2json(const mysqlpp::UseQueryResult& uqr, Json::Value& val, int count)
{
	enum {
		T_INT = 1,
		T_FLOAT = 2,
		T_STRING = 3,
	};
	static const char* int_name[] = {
		"TINYINT",
		"SMALLINT",
		"MEDIUMINT",
		"INT",
		"INTEGER",
		"BIGINT"
	};
	std::vector<string> ints(int_type, int_type+sizeof(int_type)/sizeof(int_type[0]));
	static const char* float_type[] = {
		"FLOAT",
		"DOUBLE",
		"DECIMAL"
	};
	std::vector<string> floats(float_type, float_type+sizeof(float_type)/sizeof(float_type[0]));

	try {
		std::vector<string> name_vector;
		std::vector<int> type_vector;

		for (int i = 0; i < uqr.num_fields(); ++i) {
			const mysqlpp::Field& f = uqr.fetch_field(i);
			string name = f.name();
			string type = f.type().sql_name();
			type = type.substr(0, type.find(" "));

			name_vector.push_back(name);
			if (std::find(ints.begin(), ints.end(), type) != ints.end()) {
				type_vector.push_back(T_INT);
			} else if (std::find(floats.begin(), floats.end(), type) != floats.end()) {
				type_vector.push_back(T_FLOAT);
			} else {
				type_vector.push_back(T_STRING);
			}
		}

		mysqlpp::Row row;
		while (row=uqr.fetch_row()) {
			if (0 == count) {
				break;
			}
			Json::Value a;
			for (int i = 0; i < row.size(); ++i) {
				string name = name_vector[i];
				int type = type_vector[i];
				if (type == T_INT) {
					a[name] = (int)row[i];
				} else if (type == T_FLOAT) {
					a[name] = (float)row[i];
				} else if (type == T_STRING) {
					a[name] = (string)row[i];
				}
			}
			val.append(a);
			--count;
		}
	} catch (mysqlpp::OptionalExceptions ex) {
		return false;
	}
	return true;
}
