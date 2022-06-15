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

void fotoTerra(connection*);
void n_foto_per_pianeta(connection*);
void astronauti(connection*);
void n_basi_spaziali(connection*);
void stati_agSpaziale_baseSpaziale(connection*);
void stati_astronauti_noAccademie(connection*);
void treStati_differenze_media(connection*);

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
void fotoTerra(connection* conn) {
	vector<row_t> rows = conn->exec("SELECT codice, tecnica, larghezza, altezza FROM Foto WHERE PianetaScattato = 'Terra' AND(data >= '2020-05-16 00:00:00' and data < '2020-05-30 00:00:00')");

	cout << "Codice" << setw(20) << "Tecnica" << setw(55) << "Larghezza" << setw(55) << endl;

	for (row_t& row : rows) {
		string codice = row["codice"];
		string tecnica = row["tecnica"];
		string larghezza = row["larghezza"];
		string altezza = row["altezza"];

		cout << codice << setw(20) << tecnica << setw(55) << larghezza << setw(55) << altezza << setw(55) << endl;
	}
	cout << endl;
}

// QUERY 2
void n_foto_per_pianeta(connection* conn) {
	vector<row_t> rows = conn->exec("SELECT nome, COUNT(Foto.pianetaScattato) AS numeroFotoScattate FROM Corpo_celeste LEFT JOIN Foto ON Corpo_celeste.nome = Foto.pianetaScattato GROUP BY nome ORDER BY numeroFotoScattate DESC");

	cout << "Nome" << setw(20) << "Numero delle foto scattate" << endl;

	for (row_t& row : rows) {
		string nome = row["nome"];
		string numerofotoscattate = row["numerofotoscattate"];

		cout << nome << setw(20) << numerofotoscattate << endl;
	}
	cout << endl;
}

// QUERY 3
void astronauti(connection* conn) {
	vector<row_t> rows = conn->exec("SELECT Astronauta.codiceFiscale, nomeAstronauta, cognomeAstronauta, dataDiNascita FROM Astronauta JOIN Accademia ON Accademia.nome = Astronauta.scuolaFrequentata WHERE Accademia.stato = 'USA' GROUP BY codiceFiscale");

	cout << "Codice Fiscale" << setw(20) << "Nome" << setw(55) << "Cognome" << setw(55) << "Data di nascita" << endl;

	for (row_t& row : rows) {
		string codicefiscale = row["codiceFiscale"];
		string nomeAstronauta = row["nomeAstronauta"];
		string cognomeAstronauta = row["cognomeAstronauta"];
		string dataDiNascita = row["dataDiNascita"];

		cout << codicefiscale << setw(20) << nomeAstronauta << setw(55) << cognomeAstronauta << setw(55) << dataDiNascita << endl;
	}
	cout << endl;
}

// QUERY 4
void n_basi_spaziali(connection* conn) {
	vector<row_t> rows = conn->exec("SELECT acronimo, nomeCompleto, COUNT(Base_spaziale.enteDiGestione) AS numeroBasiSpaziali FROM Agenzia_spaziale LEFT JOIN Base_spaziale ON Base_spaziale.enteDiGestione = Agenzia_spaziale.acronimo GROUP BY acronimo ORDER BY numeroBasiSpaziali DESC");

	cout << "Acronimo" << setw(20) << "Nome completo agenzia spaziale" << setw(55) << "Numero basi spaziali in possesso" << endl;

	for (row_t& row : rows) {
		string acronimo = row["acronimo"];
		string nomecompleto = row["nomecompleto"];
		string numeroBasiSpaziali = row["numeroBasiSpaziali"];

		cout << acronimo << setw(20) << nomecompleto << setw(55) << numeroBasiSpaziali << endl;
	}
	cout << endl;
}

// QUERY 5
void stati_agSpaziale_baseSpaziale(connection* conn) {
	vector<row_t> rows = conn->exec("SELECT codiceStato, nomeCompletoStato FROM Stato JOIN Agenzia_spaziale ON Stato.nomeCompletoStato = Agenzia_spaziale.stato JOIN Base_spaziale ON Agenzia_spaziale.Stato = Base_spaziale.stato GROUP BY codiceStato ORDER BY codiceStato");

	cout << "Acronimo dello Stato" << setw(20) << "Nome completo" << endl;

	for (row_t& row : rows) {
		string codiceStato = row["codiceStato"];
		string nomeCompletoStato = row["nomeCompletoStato"];

		cout << codiceStato << setw(20) << nomeCompletoStato << endl;
	}
	cout << endl;
}

// QUERY 6
void stati_astronauti_noAccademie(connection* conn) {
	vector<row_t> rows = conn->exec("DROP VIEW IF EXISTS ZeroAccademie; DROP VIEW IF EXISTS NumeroAccademie; CREATE VIEW NumeroAccademie AS SELECT Stato.codiceStato, Stato.nomeCompletoStato, COUNT(Accademia.stato) AS numero_accademie FROM Stato LEFT JOIN Accademia ON Stato.nomeCompletoStato = Accademia.stato GROUP BY Stato.codiceStato; CREATE VIEW ZeroAccademie AS SELECT * FROM NumeroAccademie WHERE numero_accademie = 0; SELECT ZeroAccademie.nomeCompletoStato FROM ZeroAccademie JOIN Astronauta ON ZeroAccademie.nomeCompletoStato = Astronauta.stato GROUP BY ZeroAccademie.nomeCompletoStato");

	cout << "Nome completo dello Stato" << endl;

	for (row_t& row : rows) {
		string nomeCompletoStato = row["nomeCompletoStato"];

		cout << nomeCompletoStato << endl;
	}
	cout << endl;
}

// QUERY 7
void treStati_differenze_media(connection* conn) {
	vector<row_t> rows = conn->exec("DROP VIEW IF EXISTS Media; DROP VIEW IF EXISTS Differenza; DROP VIEW IF EXISTS SommaBudgetPerStato; CREATE VIEW SommaBudgetPerStato AS SELECT codiceStato, nomeCompletoStato, SUM(Programma_spaziale.budget) AS budget_tot, SUM(Programma_spaziale.costoTotale) AS costo_tot FROM Stato JOIN Agenzia_spaziale ON Stato.nomeCompletoStato = Agenzia_spaziale.stato JOIN Programma_spaziale ON Programma_spaziale.agenziaInteressata = Agenzia_spaziale.acronimo GROUP BY codiceStato; CREATE VIEW Differenza AS SELECT*, budget_tot - costo_tot AS delta FROM SommaBudgetPerStato; CREATE VIEW Media AS SELECT codiceStato, nomeCompletoStato FROM Differenza GROUP BY codiceStato, delta, nomeCompletoStato HAVING delta >= AVG(delta) ORDER BY delta DESC; SELECT * FROM Media LIMIT 3; ");

	cout << "Acronimo dello Stato" << setw(20) << "Nome completo" << endl;

	for (row_t& row : rows) {
		string codiceStato = row["codiceStato"];
		string nomeCompletoStato = row["nomeCompletoStato"];

		cout << codiceStato << setw(20) << nomeCompletoStato << endl;
	}
	cout << endl;
}