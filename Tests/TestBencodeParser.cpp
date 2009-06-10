/*
 * TestBencodeParser.cpp
 *
 *  Created on: 09/06/2009
 *      Author: ale
 */

#include "TestBencodeParser.h"
#include "../src/BencodeParser.h"
#include "../src/DatosParser.h"
#include <cstring>
#include <cstdio>

TestBencodeParser::TestBencodeParser() {

}

TestBencodeParser::~TestBencodeParser() {

}

void TestBencodeParser::run() {
		BencodeParser parser("../Tests/PSP.torrent");
		if(	parser.procesar()){
		DatosParser* datos = parser.salidaParser();
		datos->primero();
		while (!datos->final()) {
			if(memcmp("pieces",datos->obtenerDato(),datos->obtenerLongitudDato())==0){
				datos->siguiente();
				datos->siguiente();
			}
			std::cout << datos->obtenerDato() << std::endl;
			datos->siguiente();
		}
		delete datos;
	} else {
		assert(false, "ERROR AL PROCESAR EL ARCHIVO");
	}

}
