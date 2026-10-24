#ifndef CORE_TEXTO_H
#define CORE_TEXTO_H

#include <cctype>
#include <iomanip>
#include <sstream>
#include <string>
#include "core.h"

// Nombre o categoria (de un producto o de un cliente): con contenido, sin
// pasar el largo maximo y sin el separador del archivo ni saltos de linea
// (romperian la linea al guardar).
bool textoCampoValido(const string &texto, int maximo)
{
    if (texto.find_first_not_of(" \t") == string::npos)
    {
        return false;
    }
    if ((int)texto.length() > maximo)
    {
        return false;
    }
    if (texto.find(SEPARADOR_CAMPO) != string::npos)
    {
        return false;
    }
    if (texto.find_first_of("\r\n") != string::npos)
    {
        return false;
    }
    return true;
}

// Copia del texto en minusculas, para comparar sin distinguir mayusculas
string aMinusculas(const string &texto)
{
    string resultado = texto;
    for (int i = 0; i < (int)resultado.length(); i++)
    {
        resultado[i] = tolower((unsigned char)resultado[i]);
    }
    return resultado;
}

// Numero con ceros a la izquierda: (7, 4) -> "0007"
string numeroConCeros(int numero, int digitos)
{
    ostringstream flujo;
    flujo << setfill('0') << setw(digitos) << numero;
    return flujo.str();
}

#endif
