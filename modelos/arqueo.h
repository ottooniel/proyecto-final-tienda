#ifndef MODELOS_ARQUEO_H
#define MODELOS_ARQUEO_H

#include <iomanip>
#include <sstream>
#include <string>
#include "../core/archivos.h"
#include "../core/core.h"
#include "../core/fecha.h"
#include "registro_venta.h"
#include "venta.h"

struct ResumenArqueo
{
    int facturas;                       // facturas vigentes
    long long total;                    // suma de las vigentes, en centavos
    int cantidadMetodo[4];              // [PAGO_EFECTIVO..PAGO_CREDITO] (el 0 no se usa)
    long long totalMetodo[4];
    int cantidadCaja[NUM_CAJAS + 1];    // [1..NUM_CAJAS] (el 0 no se usa)
    long long totalCaja[NUM_CAJAS + 1];
    int anuladas;
    long long totalAnuladas;
};

// Calcula el arqueo de la fecha dada. caja = 0 incluye todas las cajas.
ResumenArqueo calcularArqueo(const Ventas &ventas, const string &fecha, int caja)
{
    ResumenArqueo r;
    r.facturas = 0;
    r.total = 0;
    r.anuladas = 0;
    r.totalAnuladas = 0;
    for (int i = 0; i < 4; i++)
    {
        r.cantidadMetodo[i] = 0;
        r.totalMetodo[i] = 0;
    }
    for (int i = 0; i <= NUM_CAJAS; i++)
    {
        r.cantidadCaja[i] = 0;
        r.totalCaja[i] = 0;
    }

    for (int i = 0; i < ventas.total(); i++)
    {
        const RegistroVenta &v = ventas.obtener(i);
        if (v.fecha != fecha || (caja != 0 && v.caja != caja))
        {
            continue;
        }
        if (v.anulada)
        {
            r.anuladas++;
            r.totalAnuladas += v.total;
            continue;
        }
        r.facturas++;
        r.total += v.total;
        r.cantidadMetodo[v.metodo]++;
        r.totalMetodo[v.metodo] += v.total;
        r.cantidadCaja[v.caja]++;
        r.totalCaja[v.caja] += v.total;
    }
    return r;
}

// Nombre del archivo del arqueo: arqueo_2026-10-18_caja1.txt / ..._todas.txt
string nombreArchivoArqueo(const string &fecha, int caja)
{
    ostringstream flujo;
    flujo << PREFIJO_ARQUEO << fecha << "_";
    if (caja == 0)
    {
        flujo << "todas";
    }
    else
    {
        flujo << "caja" << caja;
    }
    flujo << ".txt";
    return flujo.str();
}

const int ANCHO_ARQUEO = 60;
const int LINEAS_ARQUEO = 40;   // lineas que se reservan para el texto del arqueo

// Una fila de la tabla: etiqueta a la izquierda, cantidad y total a la derecha
string filaArqueo(const string &etiqueta, int cantidad, long long total)
{
    ostringstream flujo;
    flujo << left << setw(26) << etiqueta << right << setw(10) << cantidad
          << setw(22) << centavosATexto(total);
    return flujo.str();
}

// Arma el texto del arqueo en "lineas" (con espacio para LINEAS_ARQUEO).
// Devuelve cuantas lineas escribio.
int arqueoATexto(const ResumenArqueo &r, const string &fecha, int caja, const string &cajero,
                 string lineas[])
{
    int n = 0;
    string doble(ANCHO_ARQUEO, '=');
    string simple(ANCHO_ARQUEO, '-');

    lineas[n++] = doble;
    lineas[n++] = "          TODO AL ALCANCE - Arqueo de caja";
    lineas[n++] = doble;
    lineas[n++] = "Fecha del arqueo: " + fecha;
    if (caja == 0)
    {
        lineas[n++] = "Alcance: todas las cajas";
    }
    else
    {
        ostringstream flujo;
        flujo << "Alcance: caja " << caja;
        lineas[n++] = flujo.str();
    }
    lineas[n++] = "Generado por: " + cajero + " (" + fechaActual() + " " + horaActual() + ")";
    lineas[n++] = simple;

    lineas[n++] = "VENTAS VIGENTES";
    {
        ostringstream flujo;
        flujo << left << setw(26) << "Metodo de pago" << right << setw(10) << "Facturas"
              << setw(22) << "Total (Q)";
        lineas[n++] = flujo.str();
    }
    for (int metodo = PAGO_EFECTIVO; metodo <= PAGO_CREDITO; metodo++)
    {
        lineas[n++] = filaArqueo(nombreMetodoPago(metodo), r.cantidadMetodo[metodo], r.totalMetodo[metodo]);
    }
    lineas[n++] = simple;
    lineas[n++] = filaArqueo("TOTAL DE VENTAS", r.facturas, r.total);
    if (r.facturas == 0)
    {
        lineas[n++] = "No hay ventas vigentes en esta fecha.";
    }
    lineas[n++] = doble;

    if (caja == 0)
    {
        lineas[n++] = "VENTAS POR CAJA";
        for (int c = 1; c <= NUM_CAJAS; c++)
        {
            ostringstream etiqueta;
            etiqueta << "Caja " << c;
            lineas[n++] = filaArqueo(etiqueta.str(), r.cantidadCaja[c], r.totalCaja[c]);
        }
        lineas[n++] = doble;
    }

    lineas[n++] = "FACTURAS ANULADAS (no incluidas en los totales)";
    lineas[n++] = filaArqueo("Anuladas", r.anuladas, r.totalAnuladas);
    lineas[n++] = doble;
    return n;
}

// Guarda el arqueo en su archivo (lo reemplaza si ya existia). false si no
// se pudo escribir.
bool guardarArqueo(const ResumenArqueo &r, const string &fecha, int caja, const string &cajero)
{
    string *lineas = new string[LINEAS_ARQUEO];
    int n = arqueoATexto(r, fecha, caja, cajero, lineas);
    bool ok = escribirLineasArchivo(nombreArchivoArqueo(fecha, caja), lineas, n);
    delete[] lineas;
    return ok;
}

#endif
