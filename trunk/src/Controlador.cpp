/*
 * Controlador.cpp
 *
 *  Created on: 29/05/2009
 *      Author: teddy
 */

#include "Controlador.h"

Controlador::Controlador(ClienteTorrent& c):cliente(c){}

Controlador::~Controlador(){}

void Controlador::setVentana(Ventana *ventana)
{
	this->ventana = ventana;
}

Torrent* Controlador::agregarTorrent(std::string ruta)
{
	return cliente.agregarTorrent(ruta);
}

void Controlador::borrarTorrent(Torrent *t)
{
	cliente.borrarTorrent(t);
}

void Controlador::continuarTorrent(Torrent *t)
{
	t->continuar();
}

void Controlador::detenerTorrent(Torrent *t)
{
	t->detener();
}

void Controlador::pausarTorrent(Torrent *t)
{
	std::cout<<"__ctrl pausar "<<std::endl;
	t->pausar();
}

void Controlador::actualizarEstado(Torrent *t)
{
	std::cout<<"__actualizar estado "<<std::endl;
	ventana->actualizarEstado(t);
}

void Controlador::agregarTorrentEnVista(Torrent *t)
{
	ventana->addTorrent(t);
}

void Controlador::cerrarCliente()
{
	cliente.finalizar();
}

void Controlador::refrescarPeers(Torrent *t)
{
	t->refrescarPeers();
}

void Controlador::notificarVista(std::string notificacion)
{
	ventana->mostrarNotificacion(notificacion);
}

