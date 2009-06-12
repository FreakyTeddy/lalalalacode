/*
 * TestTracker.cpp
 *
 *  Created on: 11/06/2009
 *      Author: ale
 */

#include "../src/BencodeParser.h"
#include "../src/ParserMensaje.h"
#include "../src/Tracker.h"
#include "TestTracker.h"


TestTracker::TestTracker() {
	datos = NULL;
}

TestTracker::~TestTracker() {
	if (datos != NULL) {
		delete datos;
	}
}

void TestTracker::run() {
	BencodeParser parser("../Tests/AngelsAndDemons.torrent");
	if (parser.procesar()) {
		datos = parser.salidaParser();
		Tracker tracker;
		std::string url;
		char* datoTemp;
		int tam;
		datos->primero();
		datos->obtenerDatoPorNombre("announce",&datoTemp,tam);
		url = datoTemp;
		delete[] datoTemp;

		std::cout<<"http://open.tracker.thepiratebay.org"<<std::endl;
		assert(tracker.connect("open.tracker.thepiratebay.org",80),"Se conecto al tracker");

		ParserMensaje parser;
		unsigned int info_hash[5];
		int puerto = 9999; // Levantar un servidor en ese puerto antes de correr el test para ver si llega algo

		datos->obtenerDatoPorNombre("length",&datoTemp,tam);
		unsigned int left = atol(datoTemp);
		delete[] datoTemp;
		std::string peer_id = "ALEBLABLAAACacharii";

		if(datos->obtenerDatoPorNombre("info_hash",&datoTemp,tam)){
		memcpy(info_hash,datoTemp,LEN_SHA1);
		}
		delete[] datoTemp;
		std::string get = parser.crearGetConEvento("annonunce",info_hash,peer_id.c_str(),puerto,0,0,left,EVENT_STARTED);

		tracker.send(get.c_str(),get.length());

		assert(true,"Paso del send");
	} else {
		assert(false, "No se pudo parsear el archivo");
	}

}

