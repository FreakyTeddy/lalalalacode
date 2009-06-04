/*
 * Torrent.h
 *
 *  Created on: 28/05/2009
 *      Author: ale
 */

#ifndef TORRENT_H_
#define TORRENT_H_

#include <list>
#include <string>
#include "BencodeParser.h"
#include "ClienteTorrent.h"
#include "Controlador.h"
#include "FileManager.h"
#include "Mutex.h"
#include "Peer.h"
#include "Tracker.h"


/*******************************************************************
 * Torrent, contiene toda la informacion que viene en el archivo   *
 * .torrent, la cual es utilizada para comunicarse con el Tracker  *
 * y los Peers.                                                    *
 * *****************************************************************/

class Peer;
class Tracker;
class Controlador;
class ClienteTorrent;

class Torrent{
public:
	Torrent();
	virtual ~Torrent();

	/*
	 * Inicializa los atributos de Torent con la informacion que levanto el parser del
	 * archivo .torrent
	 */
	bool inicializarTorrent(BencodeParser* parser);

	bool conectarTracker(std::string url,int port);

	/* Se envia un GET con la siguiente informacion:
	 * info_hash
	 * peer_id
	 * port
	 * uploaded
	 * downloaded
	 * left
	 * Si numwant < 0 envio Evento
	 *  event={EVENT_STARTED,EVENT_COMPLETED,EVENT_STOPPED}
	 * sino
	 *  envio numwant
	 */
	bool enviarEventoEstado(const char* event,int numwant);

	/*Calcula cuantos bytes faltan descargar*/
	unsigned int  left();

	/*Cuando el Tracker recibe una lista de peers desde el Tracker remoto
	 * los agrega a la lista de peers del Torrent*/
	void agregarPeer(std::string ip,int puerto);

	void agregarPeer(Peer* peerNuevo);

	std::string getInfoHash();

	std::string getPeerId();

	/* Devuelve el estado por ej: "detenido", "pausado", "conectando", "completo", etc */
	std::string getEstado();

	/* devuelve el tamaño del archivo en bytes */
	unsigned int getTamanio();

	/* devuelve el tamanio descargado en bytes*/
	unsigned int getTamanioDescargado();

	int getVelocidadSubida();

	int getVelocidadBajada();


	/* devuelve el nombre del archivo .torrent */
	std::string getNombre();

	FileManager* getFileManager();

	/* metodos llamados por el controlador desde la vista
	 * para controlar el trafico del torrent */
	void detener();
	void continuar();
	void pausar();

	void setControlador(Controlador* ctrl);


private:

	ClienteTorrent* clienteTorrent;
	Tracker* tracker;
	Mutex llaveListaPeers;
	std::list<Peer*> peers;
	FileManager archivos;
	std::string info_hash;
	std::string nombre;
	int port; // puerto donde esta escuchando el Cliente.
	unsigned int uploaded;
	unsigned int downloaded;

	Controlador *controlador;
	/*torrent notifica a controlador por medio del metodo
	 * controlador->actualizarEstado(this) */

};

#endif /* TORRENT_H_ */
