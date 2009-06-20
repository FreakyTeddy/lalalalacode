/*
 * PeerDown.cpp
 *
 *  Created on: 03/06/2009
 *      Author: ale
 */

#include "PeerDown.h"

PeerDown::PeerDown(Socket* peerRemoto, Torrent* torrent) :
	Peer(peerRemoto, torrent) {
	setTipo('D');
	std::cout << "new PeerDown "<<getIp() << std::endl;
}

PeerDown::~PeerDown() {
	join();
	std::cout << "~PeerDown" << std::endl;
	std::cout.flush();
}

void* PeerDown::run() {
	std::cout << "run PeerDown" << std::endl;
	if (sendHandshake() && sendBitfield()) {
		//unsigned int index = -1;
		int contadorCiclos = 0;
		bool error = !recvHandshake(); // error puede ser en la conexion, en lo recibido o al procesar

		while (getTorrent()->estaActivo() && !error && conexionEstaOK()) {
			int length;
			char* buffer;
			if (recvMsj(&buffer, length)) {
				error = (!procesar(buffer, length));
				delete[] buffer;
			} else {
				std::cout<<" Error al recibir "<<this->getIp()<<std::endl;
				error = true;
			}
			if (!error) {
				if (getPeer_choking() == true && getAm_interested() == false ) {
					if (actualizarImInterested()){
						sendMsg(ID_MSJ_INTERESTED); //Interested
						}
				}
				if (getPeer_choking() == false && getAm_interested() == true) {
					//sendRequest(index);
					setAm_interested(false);
				}else{
					actualizarImInterested();
				}


				contadorCiclos++;
				if (contadorCiclos == 30) {
					contadorCiclos = 0;
					sendKeepAlive();
				}
			}
			else{
				std::cout<<" Error al procesar "<<this->getIp()<<std::endl;
			}
		}
	}
	std::cout << "muere run PeerDown de "<<this->getIp()<< std::endl;
	std::cout.flush();
	cerrarConexion(); // si la conexion esta cerrada el peer puede ser eliminado de la lista
	return NULL;
}
