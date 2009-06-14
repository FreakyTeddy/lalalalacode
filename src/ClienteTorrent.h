/*
 * ClienteTorrent.h
 *
 *  Created on: 28/05/2009
 *      Author: ale
 */

#ifndef CLIENTETORRENT_H_
#define CLIENTETORRENT_H_

#include <list>
#include <iostream>
#include "Controlador.h"
#include "Socket.h"
#include "Thread.h"
#include "Configuracion.h"

/* ********************************************************************
 * TODO Completar Comentario                                          *
 * El cliente torrent es el encargado de agregar y sacar torrents.    *
 * El usuario accede a él a traves de la vista.                       *
 **********************************************************************/
class Torrent;
class Controlador;
class FileManager;

class ClienteTorrent : public Thread {

private:
	/*Hay un Torrent por cada archivo .torrent que ingresa el usuario. */
	std::list<Torrent*> torrents;

	/*
	 * peer_id (string de 20 bytes) es un codigo que genera la aplicacion
	 * al iniciar y que debe identificar al ClienteTorrent univocamente
	 * por lo menos en la maquina local, es decir, no puede haber 2 ClienteTorrent
	 * corriendo en una pc con los mismos peers ids.
	 * TODO definir que algoritmo usar para generar el codigo.
	 * Opciones: usar Pocess ID, time stamp.
	 */
	char peer_id[20];
	Socket peerListener; //socket para escuchar conexiones de peers
	unsigned int puerto; //puerto en el que escucha conexiones
	bool activo; // TODO sirve para dejar de escuchar conexiones
	Controlador *controlador;
	Configuracion config;

	std::string rutaDescargas;//ruta donde se guardan los archivos

public:

	ClienteTorrent();

	~ClienteTorrent();

	/* recibe la ruta del archivo .torrent
	 * devuelve un puntero al objeto torrent o NULL en caso de error */
	Torrent* agregarTorrent(std::string ruta);

	/* borra el torrent indicado en el parametro
	 * ver si devuelve algo en caso de error */
	void borrarTorrent(Torrent*);

	/*
	 * El ClienteTorrent recibe conexiones de Peers remotos y los
	 * linkea con el Torrent correspondiente.
	 */
	void* run();

	/* Realiza todo lo necesario para cerrar el programa
	 * por ej: cerrar todas las conexiones y guardar info sobre los torrents
	 * se llama al cerrar la ventana*/
	void finalizar();

	/*
	 *  Devuelve el Torrent asociado al hash pasado por parametro o NULL si no lo
	 * encuentra
	 */
	Torrent* buscarTorrent(std::string  hashTorrent);

	bool estaActivo();

	std::string getPeerId();

	unsigned int getPuerto();

	void setControlador(Controlador *ctrl);

	Configuracion* getConfiguracion();

	void inicializarDirectorios();
};

#endif /* CLIENTETORRENT_H_ */
