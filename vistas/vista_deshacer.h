#ifndef VISTAS_VISTA_DESHACER_H
#define VISTAS_VISTA_DESHACER_H

#include <iostream>
#include <string>
#include "../core/core.h"
#include "vista_titulo.h"

void mostrarTituloDeshacer()
{
    mostrarTitulo("Deshacer el ultimo cobro");
}

void mostrarSinCobrosParaDeshacer(int caja)
{
    cout << "No hay cobros para deshacer en la caja " << caja << "." << endl;
    cout << "Solo se pueden deshacer los cobros hechos en esta caja desde que se abrio el programa." << endl;
}

// Cabecera del cobro que se va a deshacer; los productos se muestran aparte
void mostrarCobroADeshacer(int numeroFactura, const string &metodo, long long total,
                           int codigoCliente, const string &nombreCliente, int puntosCanjeados,
                           int puntosGanados)
{
    cout << "Ultimo cobro de esta caja: Factura No. " << numeroFactura << endl;
    cout << "Pago: " << metodo << " - Total: Q " << centavosATexto(total) << endl;
    if (codigoCliente == 0)
    {
        cout << "Cliente: sin puntos" << endl;
    }
    else
    {
        cout << "Cliente: " << nombreCliente << " (codigo " << codigoCliente << ")" << endl;
        cout << "Se le devolveran " << puntosCanjeados << " puntos canjeados y se le quitaran "
             << puntosGanados << " ganados." << endl;
    }
}

void mostrarDeshacerFacturaInvalida(int numero)
{
    cout << "Error: la factura No. " << numero << " no esta vigente en ventas.txt." << endl;
    cout << "No se puede deshacer ese cobro." << endl;
}

void mostrarNoSePuedeRevertirPuntos(int puntosActuales, int ganados)
{
    cout << "No se puede deshacer: el cliente ya no conserva los puntos que gano en este cobro" << endl;
    cout << "(gano " << ganados << " y ahora tiene " << puntosActuales
         << "). Probablemente los uso en otra compra." << endl;
}

void mostrarNoSePuedeReponer(const string &nombre)
{
    cout << "No se puede deshacer: el stock de " << nombre << " pasaria del maximo." << endl;
}

void mostrarProductoYaNoExiste(const string &nombre)
{
    cout << "Aviso: " << nombre << " ya no esta en el inventario; sus unidades no se repusieron." << endl;
}

void mostrarDeshacerCancelado()
{
    cout << "No se deshizo nada. El cobro sigue vigente." << endl;
}

void mostrarAvisoFacturaSinMarcar(int numero)
{
    cout << "Aviso: no se pudo marcar el archivo de la factura No. " << numero
         << " como anulada." << endl;
}

// "efectivo" dice si hay que devolver dinero en mano o reversar la tarjeta
void mostrarCobroDeshecho(int numeroFactura, long long total, bool efectivo, const string &metodo)
{
    cout << endl;
    cout << "Cobro deshecho. La factura No. " << numeroFactura << " quedo ANULADA." << endl;
    cout << "El stock y los puntos del cliente ya se corrigieron." << endl;
    if (efectivo)
    {
        cout << "Devuelva al cliente Q " << centavosATexto(total) << " en efectivo." << endl;
    }
    else
    {
        cout << "Revierta el pago de Q " << centavosATexto(total) << " hecho con " << metodo << "." << endl;
    }
}

#endif
