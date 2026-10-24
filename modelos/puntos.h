#ifndef MODELOS_PUNTOS_H
#define MODELOS_PUNTOS_H

#include <string>
#include "../core/core.h"

// Puntos que gana el cliente por lo que pago (en centavos)
int puntosGanados(long long totalPagado)
{
    if (totalPagado < 0)
    {
        return 0;
    }
    long long puntos = totalPagado / CENTAVOS_POR_PUNTO;
    return (puntos > MAX_PUNTOS) ? MAX_PUNTOS : (int)puntos;
}

// Puntos que se le pueden acreditar de verdad al cliente: los que gano, pero
// sin pasar de MAX_PUNTOS en total. "puntosCliente" son los que tiene antes
// de sumar (ya descontado el canje).
int puntosAcreditables(int puntosCliente, int ganados)
{
    if (puntosCliente < 0 || ganados < 0 || puntosCliente > MAX_PUNTOS)
    {
        return 0;
    }
    int espacio = MAX_PUNTOS - puntosCliente;
    return (ganados > espacio) ? espacio : ganados;
}

// Cuantos canjes puede usar el cliente en este cobro: los que le alcanzan
// en puntos y los que caben en el subtotal (en centavos).
int canjesMaximos(int puntosCliente, long long subtotal)
{
    if (puntosCliente < 0 || subtotal < 2)
    {
        return 0;
    }
    int porPuntos = puntosCliente / PUNTOS_POR_CANJE;
    long long porMonto = (subtotal - 1) / DESCUENTO_POR_CANJE;
    return (porMonto < porPuntos) ? (int)porMonto : porPuntos;
}

// Descuento (en centavos) que dan esos canjes
long long descuentoPorCanjes(int canjes)
{
    return (long long)canjes * DESCUENTO_POR_CANJE;
}

#endif
