#ifndef VISTAS_VISTA_FACTURA_H
#define VISTAS_VISTA_FACTURA_H

#include <iomanip>
#include <iostream>
#include <string>
#include "../core/core.h"
#include "vista_titulo.h"

// ---------- Facturas ----------

void mostrarMenuFacturas()
{
    mostrarTitulo("Facturas");
    cout << "1. Ver las facturas mas recientes" << endl;
    cout << "2. Ver una factura por numero" << endl;
    cout << "0. Volver al menu principal" << endl;
}

// La tabla mide 76 columnas
void mostrarEncabezadoFacturas()
{
    cout << left << setw(9) << "Factura" << setw(12) << "Fecha" << setw(7) << "Hora"
         << setw(6) << "Caja" << setw(20) << "Metodo"
         << right << setw(12) << "Total (Q)" << "  " << left << "Estado" << endl;
    cout << string(76, '-') << endl;
}

void mostrarFilaFactura(int numero, const string &fecha, const string &hora, int caja,
                        const string &metodo, long long total, bool anulada)
{
    cout << left << setw(9) << numero << setw(12) << fecha << setw(7) << hora
         << setw(6) << caja << setw(20) << metodo
         << right << setw(12) << centavosATexto(total) << "  " << left
         << (anulada ? "ANULADA" : "VIGENTE") << endl;
}

// "mostradas" son las filas que se imprimieron y "total" las que hay
void mostrarResumenFacturas(int mostradas, int total)
{
    cout << "Total de facturas: " << total << endl;
    if (total > mostradas)
    {
        cout << "Se muestran las ultimas " << mostradas << "." << endl;
    }
}

void mostrarSinFacturas()
{
    cout << "Aun no se ha emitido ninguna factura." << endl;
}

void mostrarFacturaNoExiste(int numero)
{
    cout << "No existe la factura No. " << numero << "." << endl;
}

void mostrarFacturaSinArchivo(int numero)
{
    cout << "La factura No. " << numero << " esta registrada, pero no se pudo abrir su archivo." << endl;
}

void mostrarFacturaAnulada(int numero)
{
    cout << "Nota: la factura No. " << numero << " fue ANULADA." << endl;
}

// Se muestra al cargar las ventas si el archivo traia lineas danadas
void mostrarAdvertenciaVentas(int ignoradas)
{
    cout << "AVISO: se ignoraron " << ignoradas << " linea(s) invalida(s) de "
         << "ventas.txt." << endl;
    cout << "Si se emite una factura, esas lineas se perderan al guardar." << endl;
}

#endif
