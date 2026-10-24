#ifndef VISTAS_VISTA_DEVOLUCION_H
#define VISTAS_VISTA_DEVOLUCION_H

#include <iostream>
#include <string>
#include "../core/core.h"
#include "vista_titulo.h"

// ---------- Devoluciones de productos danados ----------

void mostrarMenuDevoluciones()
{
    mostrarTitulo("Devoluciones de productos danados");
    cout << "1. Cambiar un producto danado por otro" << endl;
    cout << "2. Ver informe de productos danados" << endl;
    cout << "0. Volver al menu principal" << endl;
}

void mostrarSinEspacioDanados(const string &nombre)
{
    cout << "Error: " << nombre << " ya no admite mas unidades danadas." << endl;
}

// Para una factura que el cajero escribio y no sirve como origen de la devolucion
void mostrarFacturaNoValidaParaDevolucion(int numero, bool existe)
{
    if (!existe)
    {
        cout << "No existe la factura No. " << numero << "." << endl;
    }
    else
    {
        cout << "La factura No. " << numero << " esta ANULADA: no sirve como origen." << endl;
    }
}

void mostrarReemplazoSinStock(const string &nombre, int disponibles)
{
    cout << "No hay suficientes unidades de " << nombre << " (disponibles: "
         << disponibles << ")." << endl;
}

// Precios en centavos
void mostrarReemplazoMasCaro(const string &nombre, long long precioReemplazo, long long precioDanado)
{
    cout << nombre << " cuesta Q " << centavosATexto(precioReemplazo)
         << " y el producto danado Q " << centavosATexto(precioDanado) << "." << endl;
    cout << "El cambio es sin costo: el reemplazo no puede costar mas que el danado." << endl;
}

void mostrarResumenDevolucion(const string &danado, const string &reemplazo, int cantidad, int factura)
{
    cout << endl;
    cout << "Resumen del cambio:" << endl;
    cout << "Producto danado que se recibe: " << cantidad << " x " << danado << endl;
    cout << "Producto que se entrega al cliente: " << cantidad << " x " << reemplazo << endl;
    if (factura == 0)
    {
        cout << "Factura de origen: ninguna" << endl;
    }
    else
    {
        cout << "Factura de origen: No. " << factura << endl;
    }
}

void mostrarCancelarDevolucion()
{
    cout << "Cambio cancelado. No se modifico el inventario." << endl;
}

void mostrarDevolucionRegistrada(int numero, const string &danado, int totalDanados,
                                 const string &reemplazo, int cantidad)
{
    cout << endl;
    cout << "Cambio No. " << numero << " registrado." << endl;
    cout << "Entregue al cliente: " << cantidad << " x " << reemplazo << " (sin costo)." << endl;
    cout << danado << " queda con " << totalDanados << " unidad(es) marcada(s) como danada(s)." << endl;
    cout << "Las unidades danadas no se pueden vender y no vuelven al stock." << endl;
}

void mostrarErrorDevolucion()
{
    cout << "Error: no se pudo registrar la devolucion. No se cambio nada." << endl;
}

void mostrarSinNumerosDeDevolucion()
{
    cout << "Error: ya no quedan numeros de devolucion disponibles." << endl;
}

void mostrarInformeGuardado(const string &ruta)
{
    cout << endl;
    cout << "Informe guardado en " << ruta << "." << endl;
}

void mostrarErrorInforme(const string &ruta)
{
    cout << endl;
    cout << "Error: no se pudo guardar " << ruta << ". El informe solo se mostro en pantalla." << endl;
}

// Se muestra al cargar las devoluciones si el archivo traia lineas danadas
void mostrarAdvertenciaDevoluciones(int ignoradas)
{
    cout << "AVISO: se ignoraron " << ignoradas << " linea(s) invalida(s) de "
         << "devoluciones.txt." << endl;
    cout << "Si registra una devolucion, esas lineas se perderan al guardar." << endl;
}

#endif
