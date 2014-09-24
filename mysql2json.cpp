
#include <mysql++/mysql++.h>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <json/json.h>


#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))

/**
 * @count is the number of rows to fetch, @count<0 means fetch all rows int @uqr
 */
bool mysql2json(const mysqlpp::UseQueryResult& uqr, Json::Value& list, int count)
{
	enum {
		T_INT = 1,
		T_FLOAT = 2,
		T_STRING = 3,
	};
	static const char* int_arr[] = {
		"TINYINT",
		"SMALLINT",
		"MEDIUMINT",
		"INT",
		"INTEGER",
		"BIGINT"
	};
	std::vector<std::string> ints(int_arr, int_arr+ARRAY_SIZE(int_arr));
	static const char* float_arr[] = {
		"FLOAT",
		"DOUBLE",
		"DECIMAL"
	};
	std::vector<std::string> floats(float_arr, float_arr+ARRAY_SIZE(float_arr));

	try {
		std::vector<std::string> field_names;
		std::vector<int> field_types;

		/* fetch fields' name and value type */
		for (int i = 0; i < uqr.num_fields(); ++i) {
			const mysqlpp::Field& f = uqr.fetch_field(i);
			std::string name = f.name();
			std::string type = f.type().sql_name();
			type = type.substr(0, type.find(" "));

			field_names.push_back(name);
			if (std::find(ints.begin(), ints.end(), type) != ints.end()) {
				field_types.push_back(T_INT);
			} else if (std::find(floats.begin(), floats.end(), type) != floats.end()) {
				field_types.push_back(T_FLOAT);
			} else {
				field_types.push_back(T_STRING);
			}
		}

		/* fetch rows and build a json list object */
		mysqlpp::Row row;
		while (row=uqr.fetch_row()) {
			if (0 == count) {
				break;
			}
			Json::Value val;
			for (int i = 0; i < row.size(); ++i) {
				std::string name = field_names[i];
				int type = field_types[i];
				if (type == T_INT) {
					val[name] = (int)row[i];
				} else if (type == T_FLOAT) {
					val[name] = (float)row[i];
				} else if (type == T_STRING) {
					val[name] = (std::string)row[i];
				}
			}
			list.append(val);
			--count;
		}
	} catch (mysqlpp::OptionalExceptions ex) {
		return false;
	}
	return true;
}
