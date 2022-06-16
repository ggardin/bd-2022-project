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
using std::left;
using std::right;
using std::string;
using std::vector;

vector<string> fetch_config();

void fotoTerra(connection*);
void n_foto_per_pianeta(connection*);
void astronauti(connection*);
void n_basi_spaziali(connection*);
void stati_agSpaziale_baseSpaziale(connection*);
void stati_astronauti_noAccademie(connection*);
void treStati_differenze_media(connection*);

int main() {
	connection* conn;
	vector<row_t> rows;
	vector<string> config = fetch_config();
	string conninfo = "host=" + config[0] + "port=" + config [1] + "dbname=" + config[2] + "user=" + config[3] + "password=" + config[4];

	cout << "Space Center Houston | DB Client" << endl;

	try {
		conn = new connection(conninfo);
	}
	catch (std::runtime_error error) {
		cout << "Impossibile connettersi a PostgreSQL." << endl;
		cout << "Errore: " << error.what() << endl;
		return 1;
	}

	int qid;
	cout << "1) Elenco delle foto della Terra scattate tra il 16 maggio 2020 e il 29 maggio 2020" << endl;
	cout << "2) Numero delle foto scattate per ciascun pianeta (valore 0 incluso)" << endl;
	cout << "3) Elenco degli astronauti che hanno ricevuto il brevetto negli USA" << endl;
	cout << "4) Numero delle basi spaziali gestite da ciascuna agenzia spaziale (valore 0 incluso)" << endl;
	cout << "5) Elenco degli Stati che hanno nel loro territorio almeno un'agenzia spaziale che a sua volta ha almeno una base spaziale" << endl;
	cout << "6) Elenco degli Stati che hanno dato nascita ad astroanuti, ma che non possiedono accademie" << endl;
	cout << "7) Elenco dei primi 3 Stati che hanno la differenza minore tra il budget e costi totali dei programmi spaziali, ma questa differenza deve essere maggiore della media delle differenze di tutti gli Stati" << endl;
	cout << "0) Termina la connessione ed esci dal programma" << endl;

	do {
		cout << "\nInserire il numero della query da eseguire (1~7): ";
		cin >> qid;
		cout << endl;

		try {
			switch (qid) {
				case 0:
					cout << "Chiusura della connessione..." << endl;
					break;
				case 1:
					fotoTerra(conn);
					break;
				case 2:
					n_foto_per_pianeta(conn);
					break;
				case 3:
					astronauti(conn);
					break;
				case 4:
					n_basi_spaziali(conn);
					break;
				case 5:
					stati_agSpaziale_baseSpaziale(conn);
					break;
				case 6:
					stati_astronauti_noAccademie(conn);
					break;
				case 7:
					treStati_differenze_media(conn);
					break;
				default:
					break;
			}
		}
		catch (std::runtime_error error) {
			cout << "Errore: " << error.what() << endl;
			return EXIT_FAILURE;
		}
	}
	while(qid!=0);

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
void fotoTerra(connection* conn) {
	vector<row_t> rows = conn->exec("\
		SELECT codice, URL, larghezza, altezza\
		FROM Foto\
		WHERE soggetto='Terra' AND (data >= '2020-05-16 00:00:00' AND data < '2020-05-30 00:00:00') AND tecnica = 'colore' AND larghezza >= 512\
		");

	cout << setw(15) << left << "Codice"
		 << setw(60) << left << "URL"
		 << setw(15) << left << "Larghezza"
		 << setw(15) << left << "Altezza" << endl;

	for (row_t& row : rows) {
		string codice = row["codice"];
		string URL = row["url"];
		string larghezza = row["larghezza"];
		string altezza = row["altezza"];

		cout << setw(15) << left << codice
			 << setw(60) << left << URL
			 << setw(15) << left << larghezza
			 << setw(15) << left << altezza << endl;
	}
	cout << endl;
}

// QUERY 2
void n_foto_per_pianeta(connection* conn) {
	vector<row_t> rows = conn->exec("\
		SELECT nome, COUNT(Foto.soggetto) AS numeroFotoScattate\
		FROM Corpo_celeste LEFT JOIN Foto ON Corpo_celeste.nome=Foto.soggetto\
		GROUP BY nome\
		ORDER BY numeroFotoScattate DESC\
		");

	cout << setw(20) << left << "Nome"
		 << setw(20) << left << "Foto scattate" << endl;

	for (row_t& row : rows) {
		string nome = row["nome"];
		string numerofotoscattate = row["numerofotoscattate"];

		cout << setw(20) << left << nome
			 << setw(20) << left << numerofotoscattate << endl;
	}
	cout << endl;
}

// QUERY 3
void astronauti(connection* conn) {
	vector<row_t> rows = conn->exec("\
		SELECT Astronauta.cf, Astronauta.nome, Astronauta.cognome, dataDiNascita\
		FROM Astronauta JOIN Accademia ON Accademia.nome=Astronauta.diploma\
		WHERE Accademia.stato='USA'\
		GROUP BY cf\
		");

	cout << setw(25) << left << "Codice Fiscale"
		 << setw(30) << left << "Nome"
		 << setw(30) << left << "Cognome"
		 << setw(20) << left << "Data di nascita" << endl;

	for (row_t& row : rows) {
		string codicefiscale = row["cf"];
		string nomeAstronauta = row["nome"];
		string cognomeAstronauta = row["cognome"];
		string dataDiNascita = row["datadinascita"];

		cout << setw(25) << left << codicefiscale
			 << setw(30) << left << nomeAstronauta
			 << setw(30) << left << cognomeAstronauta
			 << setw(20) << left << dataDiNascita << endl;
	}
	cout << endl;
}

// QUERY 4
void n_basi_spaziali(connection* conn) {
	vector<row_t> rows = conn->exec("\
		SELECT acronimo, nomeCompleto, COUNT(Base_spaziale.gestore) AS numeroBasiSpaziali\
		FROM Agenzia_spaziale LEFT JOIN Base_spaziale ON Base_spaziale.gestore=Agenzia_spaziale.acronimo\
		GROUP BY acronimo\
		ORDER BY numeroBasiSpaziali DESC\
	");

	cout << setw(15) << left << "Acronimo"
		 << setw(60) << left << "Nome completo"
		 << setw(20) << left << "Num. basi spaziali" << endl;

	for (row_t& row : rows) {
		string acronimo = row["acronimo"];
		string nomecompleto = row["nomecompleto"];
		string numeroBasiSpaziali = row["numerobasispaziali"];

		cout << setw(15) << left << acronimo
			 << setw(60) << left << nomecompleto
			 << setw(20) << left << numeroBasiSpaziali << endl;
	}
	cout << endl;
}

// QUERY 5
void stati_agSpaziale_baseSpaziale(connection* conn) {
	vector<row_t> rows = conn->exec("\
		SELECT codice, nomeEsteso\
		FROM Stato JOIN Agenzia_spaziale ON Stato.nomeEsteso=Agenzia_spaziale.stato JOIN Base_spaziale ON Agenzia_spaziale.Stato=Base_spaziale.stato\
		GROUP BY codice\
		ORDER BY codice asc\
	");

	cout << setw(10) << left << "Codice"
		 << setw(30) << left << "Nome completo" << endl;

	for (row_t& row : rows) {
		string codiceStato = row["codice"];
		string nomeCompletoStato = row["nomeesteso"];

		cout << setw(10) << left << codiceStato
			 << setw(20) << left << nomeCompletoStato << endl;
	}
	cout << endl;
}

// QUERY 6
void stati_astronauti_noAccademie(connection* conn) {
	vector<row_t> rows = conn->exec("\
		SELECT ZeroAccademie.nomeEsteso\
		FROM ZeroAccademie JOIN Astronauta ON ZeroAccademie.nomeEsteso=Astronauta.stato\
		GROUP BY ZeroAccademie.nomeEsteso\
	");

	cout << "Nome dello Stato" << endl;

	for (row_t& row : rows) {
		string nomeCompletoStato = row["nomeesteso"];

		cout << nomeCompletoStato << endl;
	}
	cout << endl;
}

// QUERY 7
void treStati_differenze_media(connection* conn) {
	vector<row_t> rows = conn->exec("\
		SELECT *\
		FROM Media\
		LIMIT 3\
		");

	cout << setw(10) << left << "Codice"
		 << setw(30) << left << "Nome completo" << endl;

	for (row_t& row : rows) {
		string codiceStato = row["codice"];
		string nomeCompletoStato = row["nomeesteso"];

		cout << setw(10) << left << codiceStato
			 << setw(20) << left << nomeCompletoStato << endl;
	}
	cout << endl;
}