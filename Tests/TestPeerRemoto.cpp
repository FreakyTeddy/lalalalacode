/*
 * TestPeerRemoto.cpp
 *
 *  Created on: 25/06/2009
 *      Author: ale
 */

#include "TestPeerRemoto.h"
#include "../src/ClienteTorrent.h"
#include "../src/Controlador.h"
#include "../src/Torrent.h"
TestPeerRemoto::TestPeerRemoto() {}

TestPeerRemoto::~TestPeerRemoto() {}

void TestPeerRemoto::run(){
	ClienteTorrent cliente;
	Controlador control(cliente);

	Torrent* torrent = cliente.agregarTorrent("./Tests/blues.torrent");
	torrent->agregarPeer("201.213.28.226",9999);
	torrent->continuar();
	//cliente.run();
	sleep(100);
	torrent->detener();
		sleep(10);
		delete torrent;
}
