#ifndef VISTAS_VISTA_ARQUEO_H
#define VISTAS_VISTA_ARQUEO_H

#include <iostream>
#include <string>
#include "../core/core.h"
#include "vista_titulo.h"

// ---------- Arqueo de caja ----------

void mostrarMenuArqueo(int caja)
{
    mostrarTitulo("Arqueo de caja");
    cout << "1. Arqueo de mi caja (caja " << caja << ")" << endl;
    cout << "2. Arqueo de todas las cajas" << endl;
    cout << "0. Volver" << endl;
}

void mostrarFechaInvalida()
{
    cout << "Error: escriba la fecha como AAAA-MM-DD (ejemplo: 2026-10-18)." << endl;
}

void mostrarArqueoGuardado(const string &ruta)
{
    cout << endl;
    cout << "Arqueo guardado en " << ruta << "." << endl;
}

void mostrarErrorArqueo(const string &ruta)
{
    cout << endl;
    cout << "Error: no se pudo guardar " << ruta << ". El arqueo solo se mostro en pantalla." << endl;
}

#endif
