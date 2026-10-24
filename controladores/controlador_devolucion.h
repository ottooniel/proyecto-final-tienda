#ifndef CONTROLADORES_CONTROLADOR_DEVOLUCION_H
#define CONTROLADORES_CONTROLADOR_DEVOLUCION_H

#include <sstream>
#include <string>
#include "../core/archivos.h"
#include "../core/core.h"
#include "../core/fecha.h"
#include "../modelos/devolucion.h"
#include "../modelos/informe_danados.h"
#include "../modelos/producto.h"
#include "../modelos/registro_venta.h"
#include "../vistas/vista_cobro.h"
#include "../vistas/vista_devolucion.h"
#include "../vistas/vista_menu.h"
#include "../vistas/vista_producto.h"
#include "controlador_producto.h"

// ---------- Devoluciones de productos danados ----------
// La devolucion de un producto danado es un CAMBIO: el cliente recibe otro
// producto (el mismo o uno que no cueste mas) sin pagar. El danado ya salio
// del stock cuando se vendio, asi que no se vuelve a sumar a "cantidad": se
// marca como danado (columna Dan.) para que quede registrado y no se pueda
// vender ni tirar sin control. El reemplazo si sale del stock.

// Guarda devoluciones.txt. Si falla, avisa y devuelve false.
bool guardarCambiosDevoluciones(const Devoluciones &devoluciones)
{
    if (!devoluciones.guardar())
    {
        mostrarMensaje("Error: no se pudo guardar devoluciones.txt. El registro solo esta en memoria.");
        return false;
    }
    return true;
}

// Pide la factura de la compra (0 si el cliente no la tiene). Solo acepta
// facturas registradas y vigentes. Devuelve -1 si se cierra la entrada.
int pedirFacturaDeOrigen(const Ventas &ventas)
{
    while (!entradaTerminada)
    {
        int numero = leerEntero("Factura de la compra (0 si no la tiene): ", 0, MAX_NUMERO_FACTURA);
        if (entradaTerminada)
        {
            break;
        }
        if (numero == 0)
        {
            return 0;
        }
        int posicion = ventas.buscar(numero);
        if (posicion != -1 && !ventas.obtener(posicion).anulada)
        {
            return numero;
        }
        mostrarFacturaNoValidaParaDevolucion(numero, posicion != -1);
    }
    return -1;
}

// Pide el producto que se le entrega al cliente a cambio. Puede ser el mismo
// producto danado o otro que no cueste mas y tenga stock. Devuelve su codigo,
// o 0 si el cajero cancela o se cierra la entrada.
int pedirReemplazo(const Inventario &inventario, const Producto &danado, int cantidad)
{
    while (!entradaTerminada)
    {
        mostrarMensaje("Producto a entregar al cliente (el mismo codigo para reponer el mismo).");
        int codigo = leerEntero("Codigo del producto de reemplazo (0 para volver): ", 0, MAX_CODIGO);
        if (entradaTerminada || codigo == 0)
        {
            return 0;
        }
        int posicion = inventario.buscar(codigo);
        if (posicion == -1)
        {
            mostrarProductoNoExiste();
            continue;
        }
        const Producto &reemplazo = inventario.obtener(posicion);
        int estado = validarReemplazo(danado, reemplazo, cantidad);
        if (estado == REEMPLAZO_SIN_STOCK)
        {
            mostrarReemplazoSinStock(reemplazo.nombre, reemplazo.cantidad);
        }
        else if (estado == REEMPLAZO_MAS_CARO)
        {
            mostrarReemplazoMasCaro(reemplazo.nombre, aCentavos(reemplazo.precio), aCentavos(danado.precio));
        }
        else
        {
            return codigo;
        }
    }
    return 0;
}

// Cambia un producto danado por otro: el danado se marca como danado y el
// reemplazo sale del stock, sin cobrarle ni devolverle dinero al cliente.
void registrarDevolucion(Inventario &inventario, Devoluciones &devoluciones, const Ventas &ventas,
                         int caja)
{
    mostrarMensaje("--- Cambiar un producto danado por otro ---");
    if (inventario.total() == 0)
    {
        mostrarMensaje("El inventario esta vacio.");
        return;
    }
    if (devoluciones.siguienteNumero() == 0)
    {
        mostrarSinNumerosDeDevolucion();
        return;
    }

    int codigo = leerEntero("Codigo del producto danado (0 para volver): ", 0, MAX_CODIGO);
    if (entradaTerminada || codigo == 0)
    {
        return;
    }
    int posicion = inventario.buscar(codigo);
    if (posicion == -1)
    {
        mostrarProductoNoExiste();
        return;
    }
    const Producto danado = inventario.obtener(posicion);
    mostrarEncabezadoInventario();
    mostrarFilaProducto(danado);

    int espacio = MAX_CANTIDAD - danado.danados;
    if (espacio < 1)
    {
        mostrarSinEspacioDanados(danado.nombre);
        return;
    }
    ostringstream pregunta;
    pregunta << "Unidades danadas (1 a " << espacio << ", 0 para volver): ";
    int cantidad = leerEntero(pregunta.str(), 0, espacio);
    if (entradaTerminada || cantidad == 0)
    {
        return;
    }
    int factura = pedirFacturaDeOrigen(ventas);
    if (factura < 0)
    {
        return;
    }
    int codigoReemplazo = pedirReemplazo(inventario, danado, cantidad);
    if (entradaTerminada || codigoReemplazo == 0)
    {
        mostrarCancelarDevolucion();
        return;
    }
    const Producto reemplazo = inventario.obtener(inventario.buscar(codigoReemplazo));

    mostrarResumenDevolucion(danado.nombre, reemplazo.nombre, cantidad, factura);
    int confirmar = leerEntero("Registrar el cambio? 1. Si  2. No: ", 1, 2);
    if (entradaTerminada || confirmar != 1)
    {
        mostrarCancelarDevolucion();
        return;
    }

    Devolucion d;
    d.numero = devoluciones.siguienteNumero();
    d.fecha = fechaActual();
    d.hora = horaActual();
    d.caja = caja;
    d.factura = factura;
    d.codigoProducto = danado.codigo;
    d.nombreProducto = danado.nombre;
    d.cantidad = cantidad;
    d.codigoReemplazo = reemplazo.codigo;
    d.nombreReemplazo = reemplazo.nombre;

    // Se comprueba todo ANTES de cambiar nada
    if (!devolucionValida(d) || !inventario.marcarDanados(codigo, cantidad))
    {
        mostrarErrorDevolucion();
        return;
    }
    if (!inventario.descontar(codigoReemplazo, cantidad))
    {
        // no deberia pasar: el reemplazo ya se valido. Se deshace lo anterior.
        inventario.quitarDanados(codigo, cantidad);
        mostrarErrorDevolucion();
        return;
    }
    devoluciones.agregar(d);

    guardarCambios(inventario);
    guardarCambiosDevoluciones(devoluciones);
    mostrarDevolucionRegistrada(d.numero, danado.nombre,
                                inventario.obtener(inventario.buscar(codigo)).danados,
                                reemplazo.nombre, cantidad);
}

// Muestra el informe de danados en pantalla y lo guarda en un archivo
void verInformeDanados(const Inventario &inventario, const Devoluciones &devoluciones)
{
    string *lineas = new string[lineasDeInformeDanados(inventario, devoluciones)];
    int n = informeDanadosATexto(inventario, devoluciones, lineas);
    for (int i = 0; i < n; i++)
    {
        mostrarMensaje(lineas[i]);
    }
    bool guardado = escribirLineasArchivo(ARCHIVO_INFORME_DANADOS, lineas, n);
    delete[] lineas;

    if (guardado)
    {
        mostrarInformeGuardado(ARCHIVO_INFORME_DANADOS);
    }
    else
    {
        mostrarErrorInforme(ARCHIVO_INFORME_DANADOS);
    }
}

// Submenu de devoluciones: se queda aqui hasta que el cajero elige volver
void gestionarDevoluciones(Inventario &inventario, Devoluciones &devoluciones, const Ventas &ventas,
                           int caja)
{
    int opcion;
    do
    {
        mostrarMenuDevoluciones();
        opcion = leerEntero("Seleccione una opcion: ", 0, 2);
        mostrarMensaje("");

        switch (opcion)
        {
        case 1:
            registrarDevolucion(inventario, devoluciones, ventas, caja);
            break;
        case 2:
            verInformeDanados(inventario, devoluciones);
            break;
        }
    } while (opcion != 0);
}

#endif
