#ifndef VISTAS_VISTA_MENU_H
#define VISTAS_VISTA_MENU_H

#include <iostream>
#include <sstream>
#include <string>
#include "../core/core.h"
#include "vista_titulo.h"

// Muestra un texto en una linea. Con "" deja una linea en blanco.
void mostrarMensaje(const string &mensaje)
{
    cout << mensaje << endl;
}


// ---------- Menu principal y cobros ----------

void mostrarMenuPrincipal(const string &cajeroActual, int caja)
{
    ostringstream titulo;
    titulo << "Caja " << caja << " - Cajero: " << cajeroActual;
    mostrarTitulo(titulo.str());
    cout << "1. Cobros" << endl;
    cout << "2. Gestionar inventario" << endl;
    cout << "3. Gestionar clientes" << endl;
    cout << "4. Filas de clientes" << endl;
    cout << "5. Devoluciones de productos danados" << endl;
    cout << "6. Registrar nuevo cajero" << endl;
    cout << "7. Cerrar sesion" << endl;
    cout << "0. Salir" << endl;
}

#endif
