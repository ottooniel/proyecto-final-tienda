#ifndef VISTAS_VISTA_FILA_H
#define VISTAS_VISTA_FILA_H

#include <iostream>
#include <sstream>
#include <string>
#include "../core/core.h"
#include "vista_titulo.h"

// ---------- Filas de clientes ----------

void mostrarMenuFilas(int cajaPropia)
{
    ostringstream titulo;
    titulo << "Filas de clientes (su caja: " << cajaPropia << ")";
    mostrarTitulo(titulo.str());
    cout << "1. Agregar cliente a una fila" << endl;
    cout << "2. Ver las filas" << endl;
    cout << "3. Retirar al cliente del frente de mi fila" << endl;
    cout << "0. Volver al menu principal" << endl;
}

// "totales[i]" son los clientes que hay en la fila de la caja i+1
void mostrarResumenFilas(const int totales[], int cantidad, int cajaPropia)
{
    cout << "Clientes en cada fila:" << endl;
    for (int i = 0; i < cantidad; i++)
    {
        cout << "  Caja " << (i + 1) << ": " << totales[i];
        if (i + 1 == cajaPropia)
        {
            cout << "  <- su caja";
        }
        cout << endl;
    }
}

void mostrarFilaLlena(int caja, int maximo)
{
    cout << "La fila de la caja " << caja << " esta llena (maximo " << maximo
         << " clientes)." << endl;
}

void mostrarClienteYaEnFila(int caja)
{
    cout << "Error: ese cliente ya esta en la fila de la caja " << caja << "." << endl;
}

// "delante" son los clientes que quedan antes que el
void mostrarClienteAgregado(int caja, int turno, int delante)
{
    cout << "Cliente agregado a la fila de la caja " << caja << " (turno " << turno << ")." << endl;
    if (delante == 0)
    {
        cout << "Es el siguiente en ser atendido." << endl;
    }
    else if (delante == 1)
    {
        cout << "Hay 1 cliente delante." << endl;
    }
    else
    {
        cout << "Hay " << delante << " clientes delante." << endl;
    }
}

void mostrarEncabezadoFila(int caja, int total, bool esPropia)
{
    cout << "Caja " << caja;
    if (esPropia)
    {
        cout << " (su caja)";
    }
    if (total == 0)
    {
        cout << ": vacia";
    }
    else if (total == 1)
    {
        cout << ": 1 cliente";
    }
    else
    {
        cout << ": " << total << " clientes";
    }
    cout << endl;
}

// Una linea de la fila. Con codigo 0 el cliente compra sin puntos.
void mostrarClienteEnFila(int posicion, int turno, int codigo, const string &nombre, int puntos)
{
    cout << "  " << posicion << ". Turno " << turno << " - ";
    if (codigo == 0)
    {
        cout << "Sin puntos" << endl;
    }
    else
    {
        cout << nombre << " (codigo " << codigo << ") - " << puntos << " pts" << endl;
    }
}

void mostrarFilaVacia(int caja)
{
    cout << "No hay clientes en la fila de la caja " << caja << "." << endl;
}

// Cliente que acaba de salir del frente de la fila para ser atendido
void mostrarAtendiendo(int caja, int turno, int codigo, const string &nombre, int puntos)
{
    cout << "Caja " << caja << " atiende el turno " << turno << ": ";
    if (codigo == 0)
    {
        cout << "cliente sin puntos." << endl;
    }
    else
    {
        cout << nombre << " (codigo " << codigo << ") - Puntos acumulados: " << puntos << endl;
    }
}

// Cliente que se retiro de la fila sin cobrarle
void mostrarClienteRetirado(int caja, int turno)
{
    cout << "Turno " << turno << " retirado de la fila de la caja " << caja << "." << endl;
}

void mostrarRetiroCancelado()
{
    cout << "No se retiro a nadie de la fila." << endl;
}

#endif
