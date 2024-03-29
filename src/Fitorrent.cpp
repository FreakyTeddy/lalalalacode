/*
 * Fitorrent.cpp
 *
 *  Created on: 16/06/2009
 *      Author: Lucia
 */

#include "Controlador.h"
#include "ClienteTorrent.h"
#include "Ventana.h"
#include "Consola.h"
#include <iostream>

int main(int argc, char **argv) {
	Gtk::Main kit(0, 0);
	ClienteTorrent cliente;
	Controlador controlador(cliente);
	Vista* ventana = NULL;
	if (argc > 1) {
		std::string arg1 = argv[1];
		if (arg1 == "-c") {
			ventana = new Consola();
			if (ventana == NULL) {
				return ERROR;
			}
		}
	} else {

		ventana = new Ventana();
		if (ventana == NULL) {
			return ERROR;
		}
		char opcion = ' ';
		if (ventana->huboError()) {
			do {
				std::cout << "Desea correr la vista por consola? Y/N"
						<< std::endl;
				std::cin >> opcion;
			} while ((opcion != 'Y') && (opcion != 'N'));

			if (opcion == 'Y') {
				delete ventana;
				ventana = new Consola();
				if (ventana == NULL) {
					return ERROR;
				}
			}
			else{
				return ERROR;
			}
		}
	}
	cliente.setControlador(&controlador);
	ventana->setControlador(&controlador);
	controlador.setVentana(ventana);
	cliente.execute();
	controlador.correrVista();


	delete ventana;
	return OK;
}

