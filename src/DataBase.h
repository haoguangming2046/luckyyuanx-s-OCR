#ifndef DATABASE_H_
#define DATABASE_H_
#include <QSqlDatabase>

class DataBase
{
public:
		DataBase();
		~DataBase();

public:
		static bool createConnection();
		static void createDatabase();
		static void createTable();

private:
		QSqlDatabase db;
};



#endif