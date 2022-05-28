#include <iostream>
#include <iomanip>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "./dependencies/include/libpq-fe.h"
#include "./dependencies/include/pq.hpp"

using pq::connection;
using pq::row_t;

using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::ifstream;
using std::setw;
using std::string;
using std::vector;

void do_exit(PGconn);
vector<string> get_config();

int main() {
	vector<string> config = get_config();
	string conninfo = "host=" + config[0] + "dbname=" + config[1] + "user=" + config[2] + "password=" + config[3];

	PGconn *conn = PQconnectdb(conninfo);
	
	if (PQstatus(conn) == CONNECTION_BAD) {
		cerr << "Connection to database failed: " << PQerrorMessage(conn);
		do_exit(conn);
	}
	exit(0);
}

void do_exit(PGconn *conn) {
	PQfinish(conn);
	exit;
}

vector<string> fetch_config() {
	vector<string> tmp;
	ifstream file("./config.txt");

	if (!file) {
		cerr << "File di configurazione non trovato" << endl;
		exit(1);
	}

	while (!file.eof()) {
		string line;
		file >> line;
		tmp.push_back(line + " ");
	}

	file.close();

	return tmp;
}
