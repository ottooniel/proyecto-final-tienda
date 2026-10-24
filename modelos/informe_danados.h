#ifndef MODELOS_INFORME_DANADOS_H
#define MODELOS_INFORME_DANADOS_H

#include <iomanip>
#include <sstream>
#include <string>
#include "../core/archivos.h"
#include "../core/core.h"
#include "../core/fecha.h"
#include "../core/texto.h"
#include "devolucion.h"
#include "producto.h"

const int ANCHO_INFORME = 76;
const int LINEAS_FIJAS_INFORME = 18;   // lineas del informe sin contar productos ni devoluciones

// Cuantas lineas de texto puede llegar a tener el informe
int lineasDeInformeDanados(const Inventario &inventario, const Devoluciones &devoluciones)
{
    return LINEAS_FIJAS_INFORME + inventario.total() + 2 * devoluciones.total();
}

// Valor (en centavos) de los productos danados del inventario, a precio de venta
long long valorDanados(const Inventario &inventario)
{
    long long suma = 0;
    for (int i = 0; i < inventario.total(); i++)
    {
        const Producto &p = inventario.obtener(i);
        suma += aCentavos(p.precio) * p.danados;
    }
    return suma;
}

// Unidades danadas que hay hoy en el inventario
long long unidadesDanadas(const Inventario &inventario)
{
    long long suma = 0;
    for (int i = 0; i < inventario.total(); i++)
    {
        suma += inventario.obtener(i).danados;
    }
    return suma;
}

// Arma el informe en "lineas" (que ya reservo quien llama con
// lineasDeInformeDanados espacios). Devuelve cuantas lineas escribio.
int informeDanadosATexto(const Inventario &inventario, const Devoluciones &devoluciones,
                         string lineas[])
{
    int n = 0;
    string doble(ANCHO_INFORME, '=');
    string simple(ANCHO_INFORME, '-');

    lineas[n++] = doble;
    lineas[n++] = "TODO AL ALCANCE - Informe de productos danados";
    lineas[n++] = "Generado: " + fechaActual() + " " + horaActual();
    lineas[n++] = doble;

    lineas[n++] = "RESUMEN POR PRODUCTO (danados que hay hoy en el inventario)";
    {
        ostringstream flujo;
        flujo << left << setw(8) << "Codigo" << setw(26) << "Producto" << setw(16) << "Categoria"
              << right << setw(6) << "Dan." << setw(16) << "Valor (Q)";
        lineas[n++] = flujo.str();
    }
    lineas[n++] = simple;
    int conDanados = 0;
    for (int i = 0; i < inventario.total(); i++)
    {
        const Producto &p = inventario.obtener(i);
        if (p.danados > 0)
        {
            ostringstream flujo;
            flujo << left << setw(8) << p.codigo << setw(26) << p.nombre << setw(16) << p.categoria
                  << right << setw(6) << p.danados
                  << setw(16) << centavosATexto(aCentavos(p.precio) * p.danados);
            lineas[n++] = flujo.str();
            conDanados++;
        }
    }
    if (conDanados == 0)
    {
        lineas[n++] = "No hay productos danados en el inventario.";
    }
    lineas[n++] = simple;
    {
        ostringstream flujo;
        flujo << "Total de unidades danadas: " << unidadesDanadas(inventario)
              << "   Valor: Q " << centavosATexto(valorDanados(inventario));
        lineas[n++] = flujo.str();
    }
    lineas[n++] = doble;

    lineas[n++] = "DETALLE DE DEVOLUCIONES REGISTRADAS";
    {
        ostringstream flujo;
        flujo << left << setw(5) << "No." << setw(11) << "Fecha" << setw(6) << "Hora"
              << setw(5) << "Caja" << setw(8) << "Factura" << setw(8) << "Codigo"
              << setw(26) << "Producto" << right << setw(6) << "Cant.";
        lineas[n++] = flujo.str();
    }
    lineas[n++] = simple;
    for (int i = 0; i < devoluciones.total(); i++)
    {
        const Devolucion &d = devoluciones.obtener(i);
        ostringstream flujo;
        flujo << left << setw(5) << d.numero << setw(11) << d.fecha << setw(6) << d.hora
              << setw(5) << d.caja << setw(8) << (d.factura == 0 ? string("-") : numeroConCeros(d.factura, 4))
              << setw(8) << d.codigoProducto << setw(26) << d.nombreProducto
              << right << setw(6) << d.cantidad;
        lineas[n++] = flujo.str();
        if (d.codigoReemplazo != 0)
        {
            ostringstream cambio;
            cambio << "     Cambiado por: " << d.nombreReemplazo << " (codigo " << d.codigoReemplazo << ")";
            lineas[n++] = cambio.str();
        }
    }
    if (devoluciones.total() == 0)
    {
        lineas[n++] = "No hay devoluciones registradas.";
    }
    lineas[n++] = simple;
    {
        ostringstream flujo;
        flujo << "Devoluciones: " << devoluciones.total()
              << "   Unidades cambiadas: " << devoluciones.unidadesDevueltas();
        lineas[n++] = flujo.str();
    }
    lineas[n++] = doble;
    return n;
}

// Guarda el informe en ARCHIVO_INFORME_DANADOS (lo reemplaza). false si no
// se pudo escribir.
bool guardarInformeDanados(const Inventario &inventario, const Devoluciones &devoluciones)
{
    string *lineas = new string[lineasDeInformeDanados(inventario, devoluciones)];
    int n = informeDanadosATexto(inventario, devoluciones, lineas);
    bool ok = escribirLineasArchivo(ARCHIVO_INFORME_DANADOS, lineas, n);
    delete[] lineas;
    return ok;
}

#endif
