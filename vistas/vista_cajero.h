#ifndef VISTAS_VISTA_CAJERO_H
#define VISTAS_VISTA_CAJERO_H

#include <iostream>
#include <string>
#include "../core/core.h"
#include "vista_titulo.h"

// ---------- Inicio de sesion ----------

void mostrarTituloLogin()
{
    mostrarTitulo("Inicio de sesion");
}

void mostrarBienvenida(const string &usuario, int caja)
{
    cout << "Bienvenido, " << usuario << ". Caja asignada: " << caja << "." << endl;
}

// Para cajeros registrados antes de que existieran las cajas
void mostrarCajaAsignada(int caja)
{
    cout << "Se le asigno la caja " << caja << ". Esta caja queda fija para este cajero." << endl;
}

void mostrarSinCajaDisponible()
{
    cout << "Error: no se pudo asignar una caja a este cajero. Revise cajeros.txt." << endl;
}

void mostrarCredencialesIncorrectas(int intento, int maximo)
{
    // mensaje generico: no se dice cual de los dos datos fallo
    cout << "Usuario o contrasena incorrectos. Intento "
         << intento << " de " << maximo << "." << endl;
}

// ---------- Registro de cajeros ----------

void mostrarTituloRegistro()
{
    mostrarTitulo("Registrar cajero");
}

// "libres[i]" dice si la caja i+1 no tiene cajero
void mostrarCajasLibres(const bool libres[], int cantidad)
{
    cout << "Cajas libres:";
    for (int i = 0; i < cantidad; i++)
    {
        if (libres[i])
        {
            cout << " " << (i + 1);
        }
    }
    cout << endl;
}

void mostrarCajaOcupada(int caja)
{
    cout << "Error: la caja " << caja << " ya tiene cajero." << endl;
}

#endif
