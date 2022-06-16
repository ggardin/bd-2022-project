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

vector<string> fetch_config(string, string, string, string, string);

void fotoCorpoCeleste(connection*);
void n_foto_per_pianeta(connection*);
void astronauti(connection*);
void n_basi_spaziali(connection*);
void stati_agSpaziale_baseSpaziale(connection*);
void stati_astronauti_noAccademie(connection*);
void treStati_differenze_media(connection*);

int main() {
	// Parametri di connessione a PostgreSQL
	string PG_HOST = "127.0.0.1";  // hostname
	string PG_PORT = "55000";      // numero porta
	string PG_DB   = "postgres";   // nome DB
	string PG_USER = "postgres";   // nome utente
	string PG_PASS = "postgrespw"; // password

	connection* conn;
	vector<row_t> rows;

	// Carica i parametri di connessione
	vector<string> config = fetch_config(PG_HOST, PG_PORT, PG_DB, PG_USER, PG_PASS);
	string conninfo = "host=" + config[0] + "port=" + config [1] + "dbname=" + config[2] + "user=" + config[3] + "password=" + config[4];

	cout << "Space Center Houston | DB Client" << endl
		 << "Le query con [*] sono interattive. "
		 << "Elenco delle operazioni disponibili:" << endl << endl;

	try {
		// Effettua (tentativo di) connessione
		conn = new connection(conninfo);
	}
	catch (std::runtime_error error) {
		cout << "Impossibile connettersi a PostgreSQL." << endl;
		cout << "Errore: " << error.what() << endl;
		return 1;
	}

	int qid;
	// Stampa elenco query
	cout << "1) Foto di un corpo celeste scattate tra due date [*]" << endl;
	cout << "2) Numero di foto scattate per ciascun corpo celeste (valore 0 incluso)" << endl;
	cout << "3) Astronauti che hanno ricevuto il brevetto in un certo Stato [*]" << endl;
	cout << "4) Numero delle basi spaziali gestite da ciascuna agenzia spaziale (valore 0 incluso)" << endl;
	cout << "5) Elenco degli Stati che hanno nel loro territorio almeno un'agenzia spaziale che a sua volta ha almeno una base spaziale" << endl;
	cout << "6) Elenco degli Stati che sono luoghi di nascita di astroanuti, ma privi di accademie" << endl;
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
					fotoCorpoCeleste(conn);
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
vector<string> fetch_config(string host, string port, string db, string user, string pass) {
	vector<string> tmp;
	ifstream file("./config.txt");

	// Se il file config.txt non è rilevato, prosegue con le impostazioni di default.
	if (!file) {
		cout << "File di configurazione non trovato.\nFallback alle impostazioni di default." << endl << endl;
		tmp.push_back(host + " ");
		tmp.push_back(port + " ");
		tmp.push_back(db   + " ");
		tmp.push_back(user + " ");
		tmp.push_back(pass + " ");
	}

	// Se il file config.txt è stato rilevato, ne scansiona il contenuto.
	while (file && !file.eof()) {
		string line;
		file >> line;
		tmp.push_back(line + " ");
	}

	file.close();

	return tmp;
}

// QUERY 1
void fotoCorpoCeleste(connection* conn) {
	string tmp = "";
	string inizio = "";
	string fine = "";
	cout << "Indicare il corpo celeste. Ad esempio: Terra, Giove." << endl;
	cout << "Corpo celeste: ";
	cin >> tmp;
	cout << endl;
	cout << "Indicare la data di inizio. Formato YYYY-MM-DD." << endl;
	cout << "Data di inizio: ";
	cin >> inizio;
	cout << endl;
	cout << "Indicare la data di inizio. Formato YYYY-MM-DD." << endl;
	cout << "Data di fine: ";
	cin >> fine;
	cout << endl;


	vector<row_t> rows = conn->exec("\
		SELECT codice, URL, larghezza, altezza\
		FROM Foto\
		WHERE soggetto='" + tmp + "' AND (data >= '" + inizio + " 00:00:00' AND data < '" + fine + " 00:00:00') AND tecnica = 'colore' AND larghezza >= 512\
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

	string tmp = "";
	cout << "Indicare lo Stato. Ad esempio: USA, Francia." << endl;
	cout << "Stato: ";
	cin >> tmp;
	cout << endl;

	vector<row_t> rows = conn->exec("\
		SELECT Astronauta.cf, Astronauta.nome, Astronauta.cognome, dataDiNascita\
		FROM Astronauta JOIN Accademia ON Accademia.nome=Astronauta.diploma\
		WHERE Accademia.stato='" + tmp + "'\
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