#ifndef CONTROLADORES_HELPERS_H
#define CONTROLADORES_HELPERS_H

#include <sstream>
#include <string>
#include "../core/core.h"
#include "../vistas/vista_menu.h"

// Usuario y contrasena: no vacios, sin espacios y de largo limitado.
// Sin espacios porque el archivo se lee con >>, que separa por espacios.
bool campoValido(const string &texto)
{
    if (texto.empty())
    {
        mostrarMensaje("Error: no puede estar vacio.");
        return false;
    }
    if ((int)texto.length() > MAX_CAMPO)
    {
        ostringstream mensaje;
        mensaje << "Error: maximo " << MAX_CAMPO << " caracteres.";
        mostrarMensaje(mensaje.str());
        return false;
    }
    if (texto.find_first_of(" \t\r") != string::npos)
    {
        mostrarMensaje("Error: no puede contener espacios.");
        return false;
    }
    return true;
}

// Pide un texto (nombre o categoria de un producto, nombre de un cliente).
// Repite hasta que sea valido. Devuelve "" si se cierra la entrada.
string leerTextoCampo(const string &mensaje, int maximo)
{
    while (!entradaTerminada)
    {
        string texto = leerLinea(mensaje);
        if (entradaTerminada)
        {
            break;
        }
        if (texto.find_first_not_of(" \t\r") == string::npos)
        {
            mostrarMensaje("Error: no puede estar vacio.");
        }
        else if ((int)texto.length() > maximo)
        {
            ostringstream error;
            error << "Error: maximo " << maximo << " caracteres.";
            mostrarMensaje(error.str());
        }
        else if (texto.find(SEPARADOR_CAMPO) != string::npos)
        {
            mostrarMensaje("Error: no puede contener el caracter |.");
        }
        else
        {
            return texto;
        }
    }
    return "";
}

#endif
