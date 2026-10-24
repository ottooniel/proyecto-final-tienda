#ifndef VISTAS_VISTA_PRODUCTO_H
#define VISTAS_VISTA_PRODUCTO_H

#include <iomanip>
#include <iostream>
#include <string>
#include "../core/core.h"
#include "vista_titulo.h"

// ---------- Inventario ----------

void mostrarMenuInventario()
{
    mostrarTitulo("Gestion de inventario");
    cout << "1. Mostrar inventario" << endl;
    cout << "2. Agregar producto" << endl;
    cout << "3. Buscar producto" << endl;
    cout << "4. Modificar producto" << endl;
    cout << "5. Eliminar producto" << endl;
    cout << "6. Ordenar inventario" << endl;
    cout << "0. Volver al menu principal" << endl;
}

void mostrarMenuOrden()
{
    mostrarTitulo("Ordenar inventario");
    cout << "1. Por nombre" << endl;
    cout << "2. Por categoria" << endl;
    cout << "0. Cancelar" << endl;
}

// La tabla mide 76 columnas, cabe en una consola de 80
void mostrarEncabezadoInventario()
{
    cout << left << setw(8) << "Codigo" << setw(27) << "Nombre" << setw(17) << "Categoria"
         << right << setw(10) << "Precio (Q)" << setw(7) << "Cant." << setw(7) << "Dan." << endl;
    cout << string(76, '-') << endl;
}

void mostrarFilaInventario(int codigo, const string &nombre, const string &categoria,
                           double precio, int cantidad, int danados)
{
    cout << left << setw(8) << codigo << setw(27) << nombre << setw(17) << categoria
         << right << fixed << setprecision(2) << setw(10) << precio
         << setw(7) << cantidad << setw(7) << danados << endl;
}

void mostrarTotalProductos(int total)
{
    cout << "Total de productos: " << total << endl;
}

// Se muestra al cargar el inventario si el archivo traia lineas danadas
void mostrarAdvertenciaInventario(int ignoradas)
{
    cout << "AVISO: se ignoraron " << ignoradas << " linea(s) invalida(s) de "
         << "inventario.txt." << endl;
    cout << "Si modifica el inventario, esas lineas se perderan al guardar." << endl;
}

#endif
