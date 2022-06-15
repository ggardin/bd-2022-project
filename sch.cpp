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

void astronauti(connection*);

int main() {
	connection* conn;

	vector<string> config = fetch_config();
	string conninfo = "host=" + config[0] + "port=" + config [1] + "dbname=" + config[2] + "user=" + config[3] + "password=" + config[4];

	try {
		conn = new connection(conninfo);
	}
	catch (std::runtime_error error) {
		cout << "Impossibile connettersi a PostegreSQL." << endl;
		cout << "Errore: " << error.what() << endl;
		return 1;
	}

	int qid;
	cout << "1)\tAstronauti che hanno ricevuto il brevetto negli USA" << endl;
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
			astronauti(conn);
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

// Funzione helper per il parsing del file di configurazione
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

// QUERY 1
void astronauti(connection* conn) {
	vector<row_t> rows = conn->exec("SELECT * FROM astronauta");

	cout << "\033[1;4;94mID" << setw(20) << "Cognome" << setw(55) << "Indirizzo"
		 << "\033[0m" << endl;

	for (row_t& row : rows) {
		string id = row["id"];
		string name = row["cognome"];
		string add = row["indirizzo"];

		cout << id << setw(20) << name << setw(55) << add << endl;
	}
	cout << endl;
}