/* 
 * File:   main.cpp
 *
 * Created on 30 de mayo de 2009, 23:25
 */
#include <iomanip>
#include <ios>
#include <sstream>
#include <iostream>

#include <stdlib.h>
#include "BencodeParser.h"
#include "sha1.h"
/*
 * 
 */
using namespace std;

void obtenerInfoHash(DatosParser*datos, SHA1 sha,FILE *fp);
void ProcesarHash(char*hash, SHA1 sha);
void MostrarSalidaSha(unsigned *message_digest);
void mostrarInfoHash(unsigned *hash);
FILE* menuInicio();

int main(int argc, char** argv) {


    FILE *fp;
    DatosParser *datos;
    SHA1 sha;

    if ((fp = menuInicio()) == NULL) return 1;

    std::cout << " --- Comienzo del Parser --- " << std::endl;

    BencodeParser parser(fp);

    parser.procesar();

    //Se obtienen los resultados del parser
    datos = parser.salidaParser();
     
    //Imprime por pantalla los resultados del parser
    datos->primero();
    while (!datos->final()) {
        //Verifico antes de imprimir que el proximo dato no sea uno de los flags de inicio o fin de diccionario
        if (strcmp(datos->obtenerDato(), "Fin") && strcmp(datos->obtenerDato(), "Inicio"))
            std::cout << datos->obtenerDato() << std::endl;
      //  Verifico el campo pieces para obtener los hash de cada piece
        if (!strcmp(datos->obtenerDato(), "pieces")) {
            datos->siguiente();

      //      ProcesarHash(datos->obtenerDato(), sha);

        }
        datos->siguiente();
    }
    //Fin de la impresion de los datos obtenidos

    std::cout << " --- Fin del Parser --- " << std::endl;

    obtenerInfoHash(datos, sha,fp);

    delete datos;
    return (EXIT_SUCCESS);
}

void obtenerInfoHash(DatosParser*datos, SHA1 sha,FILE *fp) {


    //Se obtiene el info hash calculando el valor de sha1 de la cadena del torrent perteneciente al diccionario "info"
   
    int salida = 1, contador = 0;
    unsigned mensajeDigerido[5];

    
    int i=0;
    fseek(fp,0,SEEK_END);
    int fin=ftell(fp);
    
    
    fseek(fp,0,SEEK_SET);
    
    //Imprimo por completo el archivo .torrent
    do{
        
        char c=fgetc(fp);
        printf("%c",c>32?c:'.'); 
        i++;
    }while (i<fin);
    
    fseek(fp,datos->getOffsetInfoHash(),SEEK_SET);
    i=ftell(fp);
    char* buffer=new char[fin];
    
    int pos=0;
    do{
        
        buffer[pos]=fgetc(fp);        
        i++;pos++;
    }while (i<datos->getOffsetFin()-1);
   
    
    std::cout<< std::endl<<std::endl;
   // fread(buffer,1,fin-datos->getOffsetInfoHash(),fp);
    
  //  printf("%d\n",(fin-datos->getOffsetInfoHash()));
    for (int pos=0;pos<datos->getOffsetFin()-datos->getOffsetInfoHash(); pos++){
        char c=buffer[pos];
        printf("%c",c>32?c:'.');
    }
    
   // std::cout<< " fin archivo "<< fin<<" actual "<< i<<std::endl;
    //printf("chau");
    //std::cout<<buffer<<std::endl;
    //Inicializo el sha1
    sha.inicializacion();
    
    //Ingreso la cadena a calcularle el sha1
    sha.entrada(buffer,datos->getOffsetFin()-datos->getOffsetInfoHash()+2);
    //Obtengo la salida del sha1 en el mensajeDigerido 
    sha.salida(mensajeDigerido);

    std::cout<<std::endl << " --- Info Hash del archivo .torrent ---" << std::endl;
    //Muestro por pantalla el hash tal cual sale del algoritmo 
    mostrarInfoHash(mensajeDigerido);
    std::cout << std::endl;
    
    sha.entrada(sha.salidaAstring(mensajeDigerido),strlen(sha.salidaAstring(mensajeDigerido)));
    sha.salida(mensajeDigerido);
    
    mostrarInfoHash(mensajeDigerido);
    
    
}

FILE * menuInicio() {

    char bufferTorrent[512];
    int seleccion = 0;
    FILE*fp;

    std::cout << std::endl << " --- Se utilizara el archivo defaut ( PSP.torrent ) \n --- Si desea ingresar otro .torrent presionar 1---" << std::endl;
    std::cout << "     ";
    std::cin >> seleccion;

    if (seleccion == 1) {
        std::cout << " --- Ingresar nombre del archivo torrent (Ejemplo: Nombre.torrent ) --- " << std::endl;
        std::cout << "     ";
        std::cin >> bufferTorrent;

        fp = fopen(bufferTorrent, "rb");

    } else {
        fp = fopen("Naruto.torrent", "rb");
    }

    return fp;

}

void ProcesarHash(char * datos, SHA1 sha) {

    // Particiona el string con los hash de cada piece en cadenas de 20 bytes y los muestra por pantalla
    // Al ser datos binarios los que se envian del hash los caracteres que se muestran no son legibles 
    char hash[20];
    int pos;
    unsigned aux;

    for (aux = 0; aux < strlen(datos); aux++) {

        for (pos = 0; pos < 20; pos++) {
            hash[pos] = datos[aux];
            aux++;
        }
        aux--;
        std::cout << hash << std::endl;
    }

}

void mostrarInfoHash(unsigned *hash) {
    ios::fmtflags flags;

    flags = cout.setf(ios::hex , ios::basefield);
 
    for (int i = 0; i < 5; i++) {
        cout << hash[i] ;
    }

    cout << endl;

    cout.setf(flags);
}
