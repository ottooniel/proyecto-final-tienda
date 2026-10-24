#ifndef VISTAS_VISTA_COBRO_H
#define VISTAS_VISTA_COBRO_H

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include "../core/core.h"
#include "vista_titulo.h"

// ---------- Cobro ----------

void mostrarTituloCobro()
{
    mostrarTitulo("Procesar cobro");
}

void mostrarCobroSinFila(int caja)
{
    cout << "No hay clientes en la fila de la caja " << caja << "." << endl;
    cout << "Agregue al cliente desde 'Filas de clientes' antes de cobrar." << endl;
}

void mostrarMenuCarrito()
{
    mostrarTitulo("Productos del cobro");
    cout << "1. Agregar producto" << endl;
    cout << "2. Quitar producto" << endl;
    cout << "3. Ver productos agregados" << endl;
    cout << "4. Continuar al pago" << endl;
    cout << "0. Cancelar el cobro" << endl;
}

// El precio va en centavos
void mostrarProductoParaVenta(int codigo, const string &nombre, long long precio, int disponibles)
{
    cout << codigo << " - " << nombre << " - Q " << centavosATexto(precio)
         << " - Disponibles: " << disponibles << endl;
}

void mostrarProductoNoExiste()
{
    cout << "No existe un producto con ese codigo." << endl;
}

void mostrarSinUnidades(const string &nombre)
{
    cout << "No quedan unidades disponibles de " << nombre << "." << endl;
}

void mostrarProductoAgregado(const string &nombre, int cantidad)
{
    cout << "Agregado: " << cantidad << " x " << nombre << "." << endl;
}

void mostrarProductoQuitado(const string &nombre)
{
    cout << "Producto quitado: " << nombre << "." << endl;
}

void mostrarProductoNoEstaEnCarrito()
{
    cout << "Ese producto no esta en el cobro." << endl;
}

void mostrarCarritoLleno(int maximo)
{
    cout << "Error: el cobro ya tiene " << maximo << " productos distintos (maximo)." << endl;
}

void mostrarCarritoVacio()
{
    cout << "Aun no hay productos en el cobro." << endl;
}

// La tabla mide 63 columnas
void mostrarEncabezadoCarrito()
{
    cout << left << setw(8) << "Codigo" << setw(27) << "Producto"
         << right << setw(6) << "Cant." << setw(10) << "Precio" << setw(12) << "Subtotal" << endl;
    cout << string(63, '-') << endl;
}

// Los montos van en centavos
void mostrarLineaCarrito(int codigo, const string &nombre, int cantidad, long long precio)
{
    cout << left << setw(8) << codigo << setw(27) << nombre
         << right << setw(6) << cantidad << setw(10) << centavosATexto(precio)
         << setw(12) << centavosATexto(precio * cantidad) << endl;
}

void mostrarSubtotalCarrito(long long subtotal)
{
    cout << string(63, '-') << endl;
    cout << "Subtotal: Q " << centavosATexto(subtotal) << endl;
}

void mostrarCancelarCobro()
{
    cout << "Cobro cancelado. No se cambio el inventario ni los puntos." << endl;
    cout << "El cliente sigue en la fila." << endl;
}

// Oferta de canje: "maxCanjes" son los que alcanzan y caben en el subtotal
void mostrarOfertaCanje(int puntos, int maxCanjes)
{
    cout << endl;
    cout << "El cliente tiene " << puntos << " puntos." << endl;
    cout << "Cada canje cuesta " << PUNTOS_POR_CANJE << " puntos y descuenta Q "
         << centavosATexto(DESCUENTO_POR_CANJE) << "." << endl;
    cout << "Puede usar hasta " << maxCanjes << " canje(s)." << endl;
}

void mostrarResumenCobro(long long subtotal, long long descuento, long long total)
{
    cout << endl;
    cout << "Resumen del cobro:" << endl;
    cout << "Subtotal: Q " << centavosATexto(subtotal) << endl;
    if (descuento > 0)
    {
        cout << "Descuento por puntos: -Q " << centavosATexto(descuento) << endl;
    }
    cout << "TOTAL A PAGAR: Q " << centavosATexto(total) << endl;
}

void mostrarMenuPago()
{
    mostrarTitulo("Metodo de pago");
    cout << "1. Efectivo" << endl;
    cout << "2. Tarjeta de debito" << endl;
    cout << "3. Tarjeta de credito" << endl;
    cout << "0. Cancelar el cobro" << endl;
}

void mostrarEfectivoNoPermitido(double maximo)
{
    cout << "El total pasa de Q " << fixed << setprecision(2) << maximo
         << ": solo se puede pagar con tarjeta." << endl;
}

void mostrarEfectivoInsuficiente(long long faltante)
{
    cout << "El efectivo no alcanza: faltan Q " << centavosATexto(faltante) << "." << endl;
}

void mostrarCambio(long long cambio)
{
    cout << "Cambio a entregar: Q " << centavosATexto(cambio) << endl;
}

// El pago con tarjeta esta simulado: no hay conexion con ningun banco
void mostrarPagoTarjeta(const string &metodo)
{
    cout << "Procesando pago con " << metodo << " (simulado)... Pago aprobado." << endl;
}

void mostrarCobroExitoso(int numeroFactura)
{
    cout << endl;
    cout << "Cobro realizado. Factura No. " << numeroFactura << " guardada." << endl;
}

void mostrarPuntosGanados(int ganados, int saldo)
{
    cout << "Puntos ganados: " << ganados << " - Puntos acumulados: " << saldo << endl;
}

void mostrarErrorFactura()
{
    cout << "Error: no se pudo guardar el archivo de la factura. No se cobro nada." << endl;
}

void mostrarSinNumerosDeFactura()
{
    cout << "Error: ya no quedan numeros de factura disponibles." << endl;
}

void mostrarErrorCobroInterno()
{
    cout << "Error: no se pudo completar el cobro. No se cambio nada." << endl;
}


// ---------- Menu de cobros y deshacer ----------

void mostrarMenuCobros(int caja)
{
    ostringstream titulo;
    titulo << "Cobros (caja " << caja << ")";
    mostrarTitulo(titulo.str());
    cout << "1. Procesar cobro" << endl;
    cout << "2. Deshacer el ultimo cobro" << endl;
    cout << "3. Ver facturas" << endl;
    cout << "4. Arqueo de caja" << endl;
    cout << "0. Volver al menu principal" << endl;
}

#endif
