/*
 * Vista.h
 *
 *  Created on: 24/06/2009
 *      Author: ale
 */

/*Interfaz que debe implementar una vista*/

#ifndef VISTA_H_
#define VISTA_H_

#include "Torrent.h"
#include <string>
#include "Tipos.h"

class Controlador;
class Torrent;
class Vista {

protected:
	Controlador *controlador;

public:

	Vista();

	virtual ~Vista();

	virtual void setControlador(Controlador *c);

	/* actualiza el estado del torrent */
	virtual void actualizarEstado(Torrent*) = 0;

	/* agrega el torrent a la vista */
	virtual void addTorrent(Torrent*) = 0;

	/* muestra una notificacion para el usuario */
	virtual void mostrarNotificacion(std::string) = 0;

	/* abre la ventana principal */
	/* devuelve 0 en caso de exito y 1 en caso de error */
	virtual int correr() = 0;

	/* devuelve cero si no se especifico ningun puerto */
	virtual UINT getPuerto() = 0;

	virtual std::string getRutaDescargas() = 0;

	virtual void setPuerto(UINT puerto) = 0;

	virtual void setRutaDescargas(std::string ruta) = 0;

	virtual bool huboError() = 0;
};

#endif /* VISTA_H_ */
