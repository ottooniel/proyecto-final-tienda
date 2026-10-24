#ifndef CONTROLADORES_CONTROLADOR_FACTURA_H
#define CONTROLADORES_CONTROLADOR_FACTURA_H

#include <string>
#include "../core/core.h"
#include "../modelos/registro_venta.h"
#include "../modelos/venta.h"
#include "../vistas/vista_factura.h"
#include "../vistas/vista_menu.h"

// ---------- Facturas ----------

// Muestra las facturas mas recientes (hasta MAX_FACTURAS_LISTADAS)
void listarFacturas(const Ventas &ventas)
{
    if (ventas.total() == 0)
    {
        mostrarSinFacturas();
        return;
    }
    int mostradas = (ventas.total() < MAX_FACTURAS_LISTADAS) ? ventas.total() : MAX_FACTURAS_LISTADAS;
    mostrarEncabezadoFacturas();
    for (int i = ventas.total() - mostradas; i < ventas.total(); i++)
    {
        const RegistroVenta &r = ventas.obtener(i);
        mostrarFilaFactura(r.numero, r.fecha, r.hora, r.caja, nombreMetodoPago(r.metodo),
                           r.total, r.anulada);
    }
    mostrarResumenFacturas(mostradas, ventas.total());
}

// Pide un numero y muestra el contenido del archivo de esa factura
void verFactura(const Ventas &ventas)
{
    if (ventas.total() == 0)
    {
        mostrarSinFacturas();
        return;
    }
    int numero = leerEntero("Numero de factura (0 para volver): ", 0, MAX_NUMERO_FACTURA);
    if (entradaTerminada || numero == 0)
    {
        return;
    }
    int posicion = ventas.buscar(numero);
    if (posicion == -1)
    {
        mostrarFacturaNoExiste(numero);
        return;
    }

    int totalLineas = contarLineasFactura(numero);
    if (totalLineas == 0)
    {
        mostrarFacturaSinArchivo(numero);
        return;
    }
    string *lineas = new string[totalLineas];
    int leidas = leerLineasFactura(numero, lineas, totalLineas);
    mostrarMensaje("");
    for (int i = 0; i < leidas; i++)
    {
        mostrarMensaje(lineas[i]);
    }
    delete[] lineas;
    if (ventas.obtener(posicion).anulada)
    {
        mostrarFacturaAnulada(numero);
    }
}

// Submenu de facturas: solo se crean al cobrar y aqui solo se consultan
void gestionarFacturas(const Ventas &ventas)
{
    int opcion;
    do
    {
        mostrarMenuFacturas();
        opcion = leerEntero("Seleccione una opcion: ", 0, 2);
        mostrarMensaje("");

        switch (opcion)
        {
        case 1:
            listarFacturas(ventas);
            break;
        case 2:
            verFactura(ventas);
            break;
        }
    } while (opcion != 0);
}


// Guarda ventas.txt. Si falla, avisa y devuelve false.
bool guardarCambiosVentas(const Ventas &ventas)
{
    if (!ventas.guardar())
    {
        mostrarMensaje("Error: no se pudo guardar ventas.txt. El registro solo esta en memoria.");
        return false;
    }
    return true;
}

#endif
