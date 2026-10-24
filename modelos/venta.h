#ifndef MODELOS_VENTA_H
#define MODELOS_VENTA_H

#include <iomanip>
#include <sstream>
#include <string>
#include "../core/archivos.h"
#include "../core/core.h"
#include "../core/texto.h"
#include "carrito.h"
#include "puntos.h"

// Nombre del metodo de pago (PAGO_EFECTIVO, PAGO_DEBITO o PAGO_CREDITO)
string nombreMetodoPago(int metodo)
{
    if (metodo == PAGO_EFECTIVO)
    {
        return "Efectivo";
    }
    if (metodo == PAGO_DEBITO)
    {
        return "Tarjeta de debito";
    }
    if (metodo == PAGO_CREDITO)
    {
        return "Tarjeta de credito";
    }
    return "Desconocido";
}

// Todos los datos de un cobro terminado (los montos van en centavos)
struct Venta
{
    int numero;             // numero de factura
    string fecha;           // AAAA-MM-DD
    string hora;            // HH:MM
    int caja;
    string cajero;
    int codigoCliente;      // 0 = compra sin puntos
    string nombreCliente;
    int metodoPago;
    long long subtotal;
    long long descuento;
    long long total;        // lo que paga el cliente
    long long recibido;     // efectivo recibido (0 si pago con tarjeta)
    long long cambio;
    int puntosCanjeados;
    int puntosGanados;
    int puntosSaldo;        // puntos que le quedan al cliente
};

// Cada factura se guarda en su propio archivo: factura_0007.txt
string nombreArchivoFactura(int numero)
{
    return PREFIJO_FACTURA + numeroConCeros(numero, 4) + EXTENSION_FACTURA;
}

const int ANCHO_FACTURA = 60;
const int LINEAS_FIJAS_FACTURA = 26;   // lineas de la factura sin contar los productos

// Cuantas lineas de texto puede llegar a tener la factura de ese carrito
int lineasDeFactura(const Carrito &carrito)
{
    return LINEAS_FIJAS_FACTURA + carrito.total();
}

// "Q 35.00"
string montoEnQuetzales(long long centavos)
{
    return "Q " + centavosATexto(centavos);
}

// Una linea con el texto a la izquierda y el monto pegado a la derecha
string lineaConMonto(const string &etiqueta, const string &monto)
{
    int espacios = ANCHO_FACTURA - (int)etiqueta.length() - (int)monto.length();
    if (espacios < 1)
    {
        espacios = 1;
    }
    return etiqueta + string(espacios, ' ') + monto;
}

// Arma el texto de la factura en "lineas" (que ya reservo quien llama con
// lineasDeFactura espacios). Devuelve cuantas lineas escribio. Ninguna
// linea queda en blanco.
int facturaATexto(const Venta &v, const Carrito &carrito, string lineas[])
{
    int n = 0;
    string doble(ANCHO_FACTURA, '=');
    string simple(ANCHO_FACTURA, '-');

    lineas[n++] = doble;
    lineas[n++] = "          TODO AL ALCANCE - Tienda de Conveniencia";
    lineas[n++] = doble;
    lineas[n++] = "Factura No.: " + numeroConCeros(v.numero, 4);
    lineas[n++] = "Fecha: " + v.fecha + "   Hora: " + v.hora;
    {
        ostringstream flujo;
        flujo << "Caja: " << v.caja << "   Cajero: " << v.cajero;
        lineas[n++] = flujo.str();
    }
    if (v.codigoCliente == 0)
    {
        lineas[n++] = "Cliente: Sin puntos (consumidor final)";
    }
    else
    {
        ostringstream flujo;
        flujo << "Cliente: " << v.nombreCliente << " (codigo " << v.codigoCliente << ")";
        lineas[n++] = flujo.str();
    }
    lineas[n++] = simple;

    {
        ostringstream flujo;
        flujo << left << setw(5) << "Cant" << " " << setw(25) << "Descripcion" << " "
              << right << setw(12) << "Precio (Q)" << " " << setw(14) << "Subtotal (Q)";
        lineas[n++] = flujo.str();
    }
    for (int i = 0; i < carrito.total(); i++)
    {
        const LineaVenta &linea = carrito.obtener(i);
        ostringstream flujo;
        flujo << left << setw(5) << linea.cantidad << " " << setw(25) << linea.nombre << " "
              << right << setw(12) << centavosATexto(linea.precio) << " "
              << setw(14) << centavosATexto(linea.precio * linea.cantidad);
        lineas[n++] = flujo.str();
    }
    lineas[n++] = simple;

    lineas[n++] = lineaConMonto("Subtotal:", montoEnQuetzales(v.subtotal));
    if (v.puntosCanjeados > 0)
    {
        ostringstream flujo;
        flujo << "Descuento por puntos (" << (v.puntosCanjeados / PUNTOS_POR_CANJE) << " canje(s)):";
        lineas[n++] = lineaConMonto(flujo.str(), "-" + montoEnQuetzales(v.descuento));
    }
    lineas[n++] = lineaConMonto("TOTAL:", montoEnQuetzales(v.total));
    lineas[n++] = simple;

    lineas[n++] = "Metodo de pago: " + nombreMetodoPago(v.metodoPago);
    if (v.metodoPago == PAGO_EFECTIVO)
    {
        lineas[n++] = lineaConMonto("Efectivo recibido:", montoEnQuetzales(v.recibido));
        lineas[n++] = lineaConMonto("Cambio:", montoEnQuetzales(v.cambio));
    }
    lineas[n++] = simple;

    if (v.codigoCliente == 0)
    {
        lineas[n++] = "Compra sin puntos de lealtad";
    }
    else
    {
        ostringstream canjeados, ganados, saldo;
        canjeados << "Puntos canjeados: " << v.puntosCanjeados;
        ganados << "Puntos ganados: " << v.puntosGanados;
        saldo << "Puntos acumulados: " << v.puntosSaldo;
        lineas[n++] = canjeados.str();
        lineas[n++] = ganados.str();
        lineas[n++] = saldo.str();
    }
    lineas[n++] = doble;
    lineas[n++] = "              Gracias por su compra!";
    lineas[n++] = doble;
    return n;
}

// Crea el archivo de la factura. false si el numero no es valido o no se
// pudo escribir el archivo.
bool guardarFactura(const Venta &v, const Carrito &carrito)
{
    if (v.numero < 1 || v.numero > MAX_NUMERO_FACTURA)
    {
        return false;
    }
    string *lineas = new string[lineasDeFactura(carrito)];
    int n = facturaATexto(v, carrito, lineas);
    bool ok = escribirLineasArchivo(nombreArchivoFactura(v.numero), lineas, n);
    delete[] lineas;
    return ok;
}

// Cuantas lineas tiene el archivo de esa factura (0 si no existe)
int contarLineasFactura(int numero)
{
    return contarLineasArchivo(nombreArchivoFactura(numero));
}

// Lee las lineas de la factura en "lineas" (hasta "maximo"). Devuelve cuantas leyo.
int leerLineasFactura(int numero, string lineas[], int maximo)
{
    return leerLineasArchivo(nombreArchivoFactura(numero), lineas, maximo);
}

const string MARCA_ANULADA = "*** FACTURA ANULADA";

// Marca el archivo de la factura como anulada poniendo una linea al inicio.
// false si el archivo no existe o no se pudo escribir. Si ya estaba marcada
// no hace nada y devuelve true.
bool anularFacturaEnArchivo(int numero, const string &fecha, const string &hora)
{
    int total = contarLineasFactura(numero);
    if (total == 0)
    {
        return false;
    }
    string *lineas = new string[total + 1];
    int leidas = leerLineasFactura(numero, lineas + 1, total);
    bool ok = leidas > 0;
    if (ok && lineas[1].compare(0, MARCA_ANULADA.length(), MARCA_ANULADA) == 0)
    {
        delete[] lineas;
        return true;
    }
    if (ok)
    {
        lineas[0] = MARCA_ANULADA + " el " + fecha + " a las " + hora + " ***";
        ok = escribirLineasArchivo(nombreArchivoFactura(numero), lineas, leidas + 1);
    }
    delete[] lineas;
    return ok;
}

#endif
