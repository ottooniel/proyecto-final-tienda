#ifndef VISTAS_VISTA_TITULO_H
#define VISTAS_VISTA_TITULO_H

#include <iostream>
#include <string>
#include "../core/core.h"

//   Procesar cobro
//   --------------
void mostrarTitulo(const string &texto)
{
    cout << endl;
    cout << texto << endl;
    cout << string(texto.length(), '-') << endl;
}

#endif
