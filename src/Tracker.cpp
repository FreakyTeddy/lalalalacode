/*
 * Tracker.cpp
 *
 *  Created on: 28/05/2009
 *      Author: ale
 */

#include "Constantes.h"
#include "Tracker.h"
#include <cstring>
#include <arpa/inet.h>
#include <sstream>
Tracker::Tracker() {
	puerto = PUERTO_DEFAULT_TRACKER;
	minInterval = 0;
}

Tracker::~Tracker() {

}

void* Tracker::run() {
	int cantidad;
	int caracteresProcesados=0;
	int longitud,posUltimoProcesado;
	bool seCerro = false;
	std::string buffer;
	char bufferTemp[1000];
	memset(bufferTemp, 0, 1000);

	while (trackerRemoto.is_valid() && !seCerro) {
		if ((cantidad = this->trackerRemoto.receive(bufferTemp, 999)) > 0) {
			bufferTemp[cantidad] = '\0';
			buffer.insert(caracteresProcesados,bufferTemp,cantidad);
			longitud=caracteresProcesados+cantidad;
			if (procesarResponse(buffer,longitud,posUltimoProcesado)) {
				 buffer.erase(0,posUltimoProcesado);
			}
			else {
				caracteresProcesados+=cantidad;
				buffer.resize(caracteresProcesados+cantidad);
			}

		} else {
			seCerro = true;
		}
	}

	return NULL;
}

std::string Tracker::getPath() {
	return path;
}

std::string Tracker::getUrl() {
	return url;
}

bool Tracker::connect() {

	if (this->url.length() > 0) {
		return (trackerRemoto.connect(this->url, puerto) == OK);
	} else {
		return false;
	}
}

bool Tracker::send(const char* stream, unsigned int size) {
	return ((unsigned int) trackerRemoto.send(stream, size) == size);
}

void Tracker::cerrarConexion() {
	trackerRemoto.close();
}

void Tracker::inicilizar(std::string url) {
	int ini = url.find('/', 0);
	int fin = url.find_last_of('/', url.length());
	unsigned int puerto = url.find(':', ini);
	path.assign(url, fin + 1, url.length() - fin - 1);
	if (puerto != std::string::npos) {
		std::string temp;
		temp.assign(url, puerto + 1, fin - puerto - 1);
		this->puerto = atoi(temp.c_str());
		this->url.assign(url, ini + 2, puerto - (ini + 2));
	} else {
		this->url.assign(url, ini + 2, fin - (ini + 2));
	}
}

unsigned int Tracker::getMinInterval() {
	return minInterval;
}

void Tracker::setTorrent(Torrent* unTorrent) {
	this->torrent = unTorrent;
}

bool Tracker::procesarResponse(std::string& buffer,int& longitud,int &posUltimoProcesado) {
    std::string salida;
	if (extraerBencode(buffer, longitud,salida,posUltimoProcesado)) {
		std::cout<<buffer<<std::endl;
		BencodeParser parser(salida.c_str(), longitud);
		if (parser.procesar()) {
			DatosParser* datos;
			char *aux;
			int tam;
			datos = parser.salidaParser();
			datos->primero();
			if (datos->obtenerDatoPorNombre("peers", &aux, tam)) {

				decodificarPeers(datos->obtenerDato(),
						datos->obtenerLongitudDato());
				delete[] aux;
			}
			delete datos;
		}
		return true;
	} else {
		return false;
	}
}

bool Tracker::extraerBencode(std::string &buffer, int &longitud,std::string &salida,int &posUltimoProcesado) {

	unsigned i = 0, marca,longitudBencode;
	longitudBencode=obtenerLongitudBencode (buffer,marca);
	if(longitudBencode>0){
		char *aux = new char[longitudBencode+1];
		int longTemp = (longitudBencode+marca-1);
		if (longTemp == longitud -1) {
			if (buffer[longTemp] == 'e') {
				i = 0;
				for (unsigned pos = marca; pos
				< (longitudBencode + marca); pos++) {
					aux[i] = buffer[pos];
					i++;
				}
				aux[i] = '\0';
				salida.insert(0, aux, longitudBencode);
				longitud = longitudBencode;

				posUltimoProcesado=longitudBencode+marca;
			} else {
				return false;
			}
		}
		else {
		return false;
		}
		delete[] aux;
	} else
		return false;
	return true;
}

int Tracker::obtenerLongitudBencode (std::string &buffer,unsigned int &marca){

	unsigned inicio = 0, i=0,marcaPeers,marcaFinLong,salida;

	marca       = buffer.find("d8:complete", 0);
	marcaPeers  = buffer.find("peers",0);
	marcaFinLong= buffer.find(":",marcaPeers);

	if (marca != string::npos && marcaPeers != string::npos && marcaFinLong != string::npos) {
		//Obtengo la longitud total de los peers
		std::string longitudPeers;
		for (inicio = marcaPeers + 5; inicio < marcaFinLong ; inicio++) {
			longitudPeers[i] = buffer[inicio];
			i++;
		}
       std::cout<< longitudPeers.c_str()<< std::endl;
		salida= atoi(longitudPeers.c_str())+(marcaFinLong-marca)+2;
		return salida;
	}
	else return ERROR;

}


void Tracker::decodificarPeers(char * cadena, unsigned int longitudCadena) {

	int cantMax =  torrent->getCantidadMaximaPeers();
	int cantPeers = torrent->getCantPeers();
	int cantIps = (int) (longitudCadena / 6);
	unsigned short int puerto;
	int i = 0;
	while ((cantMax > cantPeers) && (i < cantIps) && torrent->estaActivo()) {
		std::stringstream ip;
		int index = (i * 6);
		unsigned char temp = (unsigned char) cadena[index];
		ip << (int) temp << ".";
		temp = (unsigned char) cadena[index + 1];
		ip << (int) temp << ".";
		temp = (unsigned char) cadena[index + 2];
		ip << (int) temp << ".";
		temp = (unsigned char) cadena[index + 3];
		ip << (int) temp;
		memcpy(&puerto, cadena + index + 4, sizeof(unsigned short int));
		puerto = ntohs(puerto);
		std::string ip_string = ip.str();
		std::cout<<"IP: "<<ip_string<<" Puerto: "<< puerto<<std::endl;
		torrent->agregarPeer(ip_string, puerto);
		i++;
		cantPeers = torrent->getCantPeers();
		while( torrent->estaActivo() && (cantPeers == cantMax)){
			torrent->removerPeersInactivos();
			cantPeers = torrent->getCantPeers();
			std::cout<<"*** sleep 5: tracker remover peers inactivos ***"<<std::endl;
			sleep(5);
		}

	}
}


