#include <iostream>
#include <iomanip>
#include <fstream>
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

vector<string> fetch_config();

int main() {
	vector<string> config = fetch_config();
	connection* conn;
	string conninfo = "host=" + config[0] + "dbname=" + config[1] + "user=" + config[2] + "password=" + config[3];

	try {
		conn = new connection(conninfo);
	}
	catch (std::runtime_error error) {
		cout << "\033[1;31mImpossibile connettersi a PostegreSQL.\033[0m" << endl;
		cout << "\033[1;31mErrore: " << error.what() << "\033[0m" << endl;
		return 1;
	}

	int qid;
	cout << "1)\t" << endl;
	cout << "2)\t" << endl;
	cout << "3)\t" << endl;
	cout << "4)\t" << endl;
	cout << "5)\t" << endl;
	cout << "6)\t" << endl;
	cout << "Inserire la query da eseguire (1~6): ";
	cin >> qid;

	try {
		switch (qid) {
		case 1:
			break;
		case 2:
			break;
		case 3:
			break;
		case 4:
			break;
		case 5:
			break;
		case 6:
		default:
			break;
		}
	}
	catch (std::runtime_error error) {
		cout << "\033[1;31mErrore: " << error.what() << "\033[0m" << endl;
		return 1;
	}

	delete conn;
	return 0;
}

vector<string> fetch_config() {
	vector<string> tmp;
	ifstream file("./config.txt");

	if (!file) {
		cerr << "File di configurazione non trovato." << endl;
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
