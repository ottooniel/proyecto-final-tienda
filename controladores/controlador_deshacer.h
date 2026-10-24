#ifndef CONTROLADORES_CONTROLADOR_DESHACER_H
#define CONTROLADORES_CONTROLADOR_DESHACER_H

#include <string>
#include "../core/core.h"
#include "../core/fecha.h"
#include "../modelos/cliente.h"
#include "../modelos/pila.h"
#include "../modelos/producto.h"
#include "../modelos/puntos.h"
#include "../modelos/registro_venta.h"
#include "../modelos/venta.h"
#include "../vistas/vista_cobro.h"
#include "../vistas/vista_deshacer.h"
#include "controlador_cliente.h"
#include "controlador_factura.h"
#include "controlador_fila.h"
#include "controlador_producto.h"

// ---------- Deshacer el ultimo cobro ----------

// Deshace el ultimo cobro de la caja (cima de su pila): repone el stock,
// devuelve y quita puntos, anula la factura y la venta. El cobro solo sale de
// la pila cuando todo se pudo revertir; antes se comprueba todo para no dejar
// nada a medias.
void deshacerUltimoCobro(int caja, Pila pilas[], Inventario &inventario, Clientes &clientes,
                         Ventas &ventas)
{
    mostrarTituloDeshacer();
    Pila &pila = pilas[caja - 1];
    CobroDeshacer cobro;
    if (!pila.verCima(cobro))
    {
        mostrarSinCobrosParaDeshacer(caja);
        return;
    }
    int posVenta = ventas.buscar(cobro.numeroFactura);
    if (posVenta == -1 || ventas.obtener(posVenta).anulada)
    {
        mostrarDeshacerFacturaInvalida(cobro.numeroFactura);
        return;
    }

    // se muestra lo que se va a deshacer
    string nombreCliente;
    int puntosCliente;
    datosDeCliente(clientes, cobro.codigoCliente, nombreCliente, puntosCliente);
    mostrarCobroADeshacer(cobro.numeroFactura, nombreMetodoPago(cobro.metodoPago), cobro.total,
                          cobro.codigoCliente, nombreCliente, cobro.puntosCanjeados,
                          cobro.puntosGanados);
    mostrarEncabezadoCarrito();
    for (int i = 0; i < cobro.numLineas; i++)
    {
        mostrarLineaCarrito(cobro.lineas[i].codigo, cobro.lineas[i].nombre, cobro.lineas[i].cantidad,
                            cobro.lineas[i].precio);
    }
    int confirmar = leerEntero("Deshacer este cobro? 1. Si  2. No: ", 1, 2);
    if (entradaTerminada || confirmar != 1)
    {
        mostrarDeshacerCancelado();
        return;
    }

    // se comprueba TODO antes de cambiar nada
    if (cobro.codigoCliente != 0)
    {
        if (clientes.buscar(cobro.codigoCliente) == -1 ||
            !puedeRevertirPuntos(puntosCliente, cobro.puntosGanados, cobro.puntosCanjeados))
        {
            mostrarNoSePuedeRevertirPuntos(puntosCliente, cobro.puntosGanados);
            return;
        }
    }
    for (int i = 0; i < cobro.numLineas; i++)
    {
        if (inventario.puedeReponer(cobro.lineas[i].codigo, cobro.lineas[i].cantidad) == REPOSICION_DESBORDE)
        {
            mostrarNoSePuedeReponer(cobro.lineas[i].nombre);
            return;
        }
    }

    // se aplica la reversa
    for (int i = 0; i < cobro.numLineas; i++)
    {
        if (!inventario.reponer(cobro.lineas[i].codigo, cobro.lineas[i].cantidad))
        {
            // el producto ya no esta en el inventario (se elimino despues del cobro)
            mostrarProductoYaNoExiste(cobro.lineas[i].nombre);
        }
    }
    if (cobro.codigoCliente != 0)
    {
        clientes.restarPuntos(cobro.codigoCliente, cobro.puntosGanados);
        clientes.sumarPuntos(cobro.codigoCliente, cobro.puntosCanjeados);
    }
    ventas.anular(cobro.numeroFactura);
    if (!anularFacturaEnArchivo(cobro.numeroFactura, fechaActual(), horaActual()))
    {
        mostrarAvisoFacturaSinMarcar(cobro.numeroFactura);
    }
    pila.pop(cobro);

    guardarCambios(inventario);
    if (cobro.codigoCliente != 0)
    {
        guardarCambiosClientes(clientes);
    }
    guardarCambiosVentas(ventas);

    mostrarCobroDeshecho(cobro.numeroFactura, cobro.total, cobro.metodoPago == PAGO_EFECTIVO,
                         nombreMetodoPago(cobro.metodoPago));
}

#endif
