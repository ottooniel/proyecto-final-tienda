#ifndef VISTAS_VISTA_CLIENTE_H
#define VISTAS_VISTA_CLIENTE_H

#include <iomanip>
#include <iostream>
#include <string>
#include "../core/core.h"
#include "vista_titulo.h"

// ---------- Clientes ----------

void mostrarMenuClientes()
{
    mostrarTitulo("Gestion de clientes");
    cout << "1. Registrar cliente nuevo" << endl;
    cout << "2. Consultar puntos de un cliente" << endl;
    cout << "3. Buscar cliente por nombre" << endl;
    cout << "0. Volver al menu principal" << endl;
}

// Menu que se usa al poner a un cliente en la fila
void mostrarMenuIdentificarCliente()
{
    mostrarTitulo("Identificar al cliente");
    cout << "1. Tengo mi codigo" << endl;
    cout << "2. Cliente nuevo" << endl;
    cout << "3. Buscar por nombre" << endl;
    cout << "0. Sin puntos" << endl;
}

// El codigo se muestra aqui para que el cajero se lo entregue al cliente
void mostrarClienteRegistrado(int codigo, const string &nombre)
{
    cout << "Cliente registrado correctamente: " << nombre << "." << endl;
    cout << "Su codigo es: " << codigo << endl;
    cout << "Entregue este codigo al cliente: lo necesita para acumular y usar sus puntos." << endl;
}

// Lo que ve el cajero cuando el cliente ya esta identificado
void mostrarClienteIdentificado(int codigo, const string &nombre, int puntos)
{
    cout << "Cliente: " << nombre << " (codigo " << codigo << ")"
         << " - Puntos acumulados: " << puntos << endl;
}

// La tabla mide 43 columnas
void mostrarEncabezadoClientes()
{
    cout << left << setw(8) << "Codigo" << setw(27) << "Nombre"
         << right << setw(8) << "Puntos" << endl;
    cout << string(43, '-') << endl;
}

void mostrarFilaCliente(int codigo, const string &nombre, int puntos)
{
    cout << left << setw(8) << codigo << setw(27) << nombre
         << right << setw(8) << puntos << endl;
}

// "mostradas" son las filas que se imprimieron y "total" las que hay
void mostrarResumenBusqueda(int mostradas, int total)
{
    if (total == 1)
    {
        cout << "Se encontro 1 cliente." << endl;
    }
    else
    {
        cout << "Se encontraron " << total << " clientes." << endl;
    }
    if (total > mostradas)
    {
        cout << "Se muestran los primeros " << mostradas
             << ". Escriba mas letras para acotar." << endl;
    }
}

// Se muestra al cargar los clientes si el archivo traia lineas danadas
void mostrarAdvertenciaClientes(int ignoradas)
{
    cout << "AVISO: se ignoraron " << ignoradas << " linea(s) invalida(s) de "
         << "clientes.txt." << endl;
    cout << "Si registra clientes o cambian sus puntos, esas lineas se perderan al guardar." << endl;
}

#endif
