
#ifndef DATOSPARSER_H_
#define DATOSPARSER_H_

#include <list>
#include <iostream>
#include <string>


using namespace std;

class DatosParser{

private:
   list<int> len;
   list <char*> datos;// Lista con las cadenas de datos del .torrent
   list <char*>::iterator it;//iterador para recorrer la lista
   list<int>::iterator itLongitudes;
public:

   //Constructor
   DatosParser();

   //Destructor
   ~DatosParser();

   //Agrega un dato al fin de la lista
   void agregarDato(const char * cadena,int longitud);

   //Retorna el valor al que se encuentre apuntando el iterador
   char* obtenerDato();

   //Retorna la longitud del dato al cual apunta el iterador
   int obtenerLongitudDato();

   //Posiciona al iterador en la primera posicion de la lista
   void primero();

   //Mueve el iterador a la cadena siguiente en la lista
   void siguiente();

   //Mueve el iterador a la cadena previa a la actual, si es que la actual no es la primera de la lista
   void anterior();

   //Verifica si se llego al final de la lista, devuelve FALSE si aun no llego y TRUE si esta en el final
   bool final();

   // Dado un nombre, devuelve en "dato" una copia del dato. Hay que liberar la memoria de "dato"
   bool obtenerDatoPorNombre(std::string nombre, char** dato,int& longitud);

};

#endif
