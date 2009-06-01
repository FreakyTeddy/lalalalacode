/*
 * Bitmap.h
 *
 *  Created on: 28/05/2009
 *      Author: ale
 */

#ifndef BITMAP_H_
#define BITMAP_H_

/*TODO cambiar nombre de la clase.
 * Este objeto va a ser el encargado de llevar el control
 * de las partes descargadas y de las que falten descargar.
 * Tambien va a coordinar, el acceso al archivo que se
 * esta descargando.
 */

#include <list>
#include "BencodeParser.h"

class Bitmap {
public:
	Bitmap(BencodeParser* parser);
	virtual ~Bitmap();
	int getTamanio();
	/*Devuelve un arreglo con el bitmap, quien lo solicita debe liberarlo*/
	void getMap(char* map,int* length);
	/*Devuelve un arreglo con el bloque pedido y su longitud, quien lo solicita debe liberarlo*/
	bool getBlock(int index,int begin,char* data,int* longitud);

	//TODO agregar metodo para escribir un bloque en una pieza de un archivo

private:
	char* bitmap;
	int tamBitmapBytes;
	//TODO agregar una lista de mutex y bloquear escritura por archivo.
	std::list<int> piezasXarchivo; // TODO deberia ser una list de "Archivo"
	int tamanioPieza;
	int bloquesXPieza;
	int bytesTotales; //tamnio total de los archivos a descargar
};

#endif /* BITMAP_H_ */
