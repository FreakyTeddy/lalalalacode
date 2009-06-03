/*
 * Peer.h
 *
 *  Created on: 28/05/2009
 *      Author: ale
 */

#ifndef PEER_H_
#define PEER_H_


/*
 * Cada Peer se ejecuta en un hilo aparte y se va a encargar de
 * pedir y enviar partes a otros Peers remotos.
 */
#include "Socket.h"
#include "Thread.h"
#include "Torrent.h"

class Torrent;
class Peer: public Thread {
public:
	Peer(Socket* peerRemoto,Torrent* torrent);
	virtual ~Peer();
	void* run();

	/*
	 * Handshake, es el primer mensaje que se envia al Peer remoto.
	 * Envia info_hash y peer_id
	 */
	bool sendHandshake();

	/********* Metodos que envian diferentes mensajes al Peer remoto *********
	 *                                                                       *
	 * Todos los mensajes entre peers empiezan con un entero de 4 bytes que  *
	 * indica la cantidad de bytes a leer a continuacion.                    *
	 * El siguiente campo es un numero de 1 byte que representa el tipo de   *
	 * mensaje que se va a procesar, del cual va a depender la cantidad de   *
	 * parametros a leer. El resto de los campos es variable.                *
	 * 																		 *
	 * ***********************************************************************/

	/*
	 * El mensaje keep-alive, contiene todos 0, y sirve para
	 * indicarle al peer remoto que tiene que mantener la conexion.
	 * Este mensaje debe ser enviado periodicamente en un intervalo
	 * no mayor a 2 minutos.
	 */
	bool sendKeepAlive();

	/*
	 * Formato mensaje: <len=0001><Id=id>
	 * El campo len se envia en formato Big Endian.
	 * Posibles ids: 0 - choke: Indica a un cliente que sus peticiones serán ignoradas
	 *               1 - unchoke: Derogación de lo dispuesto por el mensaje choke
	 *               2 - interested: Declaración del interés por alguna parte ofrecida por el cliente remoto
	 *               3 - not interested: Derogación de lo dispuesto por el mensaje interested
	 */
	bool sendMsg(const char id);

	/*
	 *Formato mensaje have: <len=0005><Id=4><piece index>
	 *have: Información a un Peer de que una parte ha sido completada
	 */
	bool sendHave(int index);
	/*
	 * Formato mensaje Bitfiel: <len=0001 + X><Id=5><bitfield>  X = longitud en bytes de bitfield
	 * bitfield: Información de partes ya obtenidas. Cada bit del bitmap representa una pieza, si
	 * el bit esta en 0 indican piezas buscadas y si esta en 1 es una pieza completa.
	 */
	bool sendBitfield();
	/*
	 * Formato mensaje Request: <len=0013><Id=6><index><begin><length>
	 * request: Solicitud de un bloque
	 *   index: numero de pieza
	 *   begin: offset en bytes dentro de la pieza
	 *   length: longitud del bloque solicitado
	 */
	bool sendRequest(int index,int block,int length);
	/*
	 * Formato mensaje Piece: <len=0009 + X><Id=7><index><begin><block>  X = longitud en bytes de block
	 * piece: Transmisión de un bloque
	 *   index: numero de pieza
	 *   begin:offset en bytes dentro de la pieza
	 *   block: el bloque con los datos, que es una porcion de la pieza solicitada.
	 */
	bool sendPiece(int index,int begin,int length);
	/*
	 * Formato mensaje Piece: <len=0013><Id=8><index><begin><length>
	 * cancel: Petición de cancelación de transmisión de un bloque
	 *   index: numero de pieza
	 *   begin: offset en bytes dentro de la pieza
	 *   length: longitud del bloque solicitado y que quiero cancelar
	 */
	bool sendCancel(int index,int block,int length);


	/*
	 * Cuando llega un mensaje desde el peer remoto, se lo guarda en un
	 * buffer que luego se manda a procesar.
	 * */
	bool procesar(const char* buffer,int length);

private:
	Socket* peerRemoto;
	Torrent* torrent;
	//TODO agregar birfield del peer remoto  para llevar control blabla
};

#endif /* PEER_H_ */
