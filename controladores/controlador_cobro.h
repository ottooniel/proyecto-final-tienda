#ifndef CONTROLADORES_CONTROLADOR_COBRO_H
#define CONTROLADORES_CONTROLADOR_COBRO_H

#include <sstream>
#include <string>
#include "../core/core.h"
#include "../core/fecha.h"
#include "../modelos/carrito.h"
#include "../modelos/cliente.h"
#include "../modelos/cola.h"
#include "../modelos/pila.h"
#include "../modelos/producto.h"
#include "../modelos/puntos.h"
#include "../modelos/registro_venta.h"
#include "../modelos/venta.h"
#include "../vistas/vista_cobro.h"
#include "../vistas/vista_fila.h"
#include "../vistas/vista_menu.h"
#include "controlador_arqueo.h"
#include "controlador_cliente.h"
#include "controlador_deshacer.h"
#include "controlador_factura.h"
#include "controlador_fila.h"
#include "controlador_producto.h"

// ---------- Cobro ----------
// Se cobra al cliente que esta al FRENTE de la fila de la caja del cajero.
// El cliente solo sale de la fila cuando el cobro termina bien; si se cancela,
// sigue en su lugar.

// Muestra los productos del cobro y el subtotal
void mostrarCarrito(const Carrito &carrito)
{
    if (carrito.total() == 0)
    {
        mostrarCarritoVacio();
        return;
    }
    mostrarEncabezadoCarrito();
    for (int i = 0; i < carrito.total(); i++)
    {
        const LineaVenta &linea = carrito.obtener(i);
        mostrarLineaCarrito(linea.codigo, linea.nombre, linea.cantidad, linea.precio);
    }
    mostrarSubtotalCarrito(carrito.subtotal());
}

// Pide un producto y una cantidad y los agrega al carrito. No deja pasar
// de las unidades que hay en el inventario (contando las que ya se agregaron).
void agregarProductoAlCarrito(Carrito &carrito, const Inventario &inventario)
{
    int codigo = leerEntero("Codigo del producto (0 para volver): ", 0, MAX_CODIGO);
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
    const Producto &p = inventario.obtener(posicion);
    int disponibles = p.cantidad - carrito.cantidadDe(codigo);
    if (disponibles < 1)
    {
        mostrarSinUnidades(p.nombre);
        return;
    }
    long long precio = aCentavos(p.precio);
    mostrarProductoParaVenta(p.codigo, p.nombre, precio, disponibles);

    ostringstream pregunta;
    pregunta << "Cantidad (1 a " << disponibles << ", 0 para volver): ";
    int cantidad = leerEntero(pregunta.str(), 0, disponibles);
    if (entradaTerminada || cantidad == 0)
    {
        return;
    }
    if (carrito.buscar(codigo) == -1 && carrito.total() >= MAX_LINEAS_CARRITO)
    {
        mostrarCarritoLleno(MAX_LINEAS_CARRITO);
        return;
    }
    if (!carrito.agregar(p.codigo, p.nombre, precio, cantidad))
    {
        mostrarErrorCobroInterno();
        return;
    }
    mostrarProductoAgregado(p.nombre, cantidad);
}

void quitarProductoDelCarrito(Carrito &carrito)
{
    if (carrito.total() == 0)
    {
        mostrarCarritoVacio();
        return;
    }
    mostrarCarrito(carrito);
    int codigo = leerEntero("Codigo del producto a quitar (0 para volver): ", 0, MAX_CODIGO);
    if (entradaTerminada || codigo == 0)
    {
        return;
    }
    int posicion = carrito.buscar(codigo);
    if (posicion == -1)
    {
        mostrarProductoNoEstaEnCarrito();
        return;
    }
    string nombre = carrito.obtener(posicion).nombre;
    carrito.quitar(codigo);
    mostrarProductoQuitado(nombre);
}

// Arma el carrito. Devuelve true si el cajero sigue al pago con al menos un
// producto, false si cancela o se cierra la entrada.
bool armarCarrito(Carrito &carrito, const Inventario &inventario)
{
    while (!entradaTerminada)
    {
        mostrarMenuCarrito();
        int opcion = leerEntero("Seleccione una opcion: ", 0, 4);
        if (entradaTerminada)
        {
            break;
        }
        mostrarMensaje("");

        if (opcion == 1)
        {
            agregarProductoAlCarrito(carrito, inventario);
        }
        else if (opcion == 2)
        {
            quitarProductoDelCarrito(carrito);
        }
        else if (opcion == 3)
        {
            mostrarCarrito(carrito);
        }
        else if (opcion == 4)
        {
            if (carrito.total() == 0)
            {
                mostrarCarritoVacio();
            }
            else
            {
                mostrarCarrito(carrito);
                return true;
            }
        }
        else
        {
            if (carrito.total() == 0)
            {
                return false;
            }
            int confirmar = leerEntero("Cancelar el cobro? 1. Si  2. No: ", 1, 2);
            if (entradaTerminada)
            {
                break;
            }
            if (confirmar == 1)
            {
                return false;
            }
        }
    }
    return false;
}

// Pregunta cuantos canjes de puntos usa el cliente. Devuelve 0 si no puede
// o no quiere canjear. Si se cierra la entrada tambien devuelve 0.
int pedirCanjes(int puntosCliente, long long subtotal)
{
    int maximo = canjesMaximos(puntosCliente, subtotal);
    if (maximo == 0)
    {
        return 0;
    }
    mostrarOfertaCanje(puntosCliente, maximo);
    ostringstream pregunta;
    pregunta << "Canjes a usar (0 a " << maximo << "): ";
    int canjes = leerEntero(pregunta.str(), 0, maximo);
    return entradaTerminada ? 0 : canjes;
}

// Pide el metodo de pago y, si es efectivo, el dinero recibido.
// Devuelve true si el pago queda listo; false si se cancela. "recibido" y
// "cambio" quedan en 0 cuando se paga con tarjeta.
bool pedirPago(long long total, int &metodo, long long &recibido, long long &cambio)
{
    recibido = 0;
    cambio = 0;
    while (!entradaTerminada)
    {
        mostrarMenuPago();
        metodo = leerEntero("Seleccione una opcion: ", 0, 3);
        if (entradaTerminada || metodo == 0)
        {
            return false;
        }

        if (metodo != PAGO_EFECTIVO)
        {
            mostrarPagoTarjeta(nombreMetodoPago(metodo));
            return true;
        }

        if (total > aCentavos(MAX_EFECTIVO))
        {
            mostrarEfectivoNoPermitido(MAX_EFECTIVO);
            continue;
        }
        while (!entradaTerminada)
        {
            double dinero = leerDecimal("Efectivo recibido en Q (0 para volver): ", 0, MAX_EFECTIVO);
            if (entradaTerminada || dinero == 0)
            {
                break;
            }
            long long entregado = aCentavos(dinero);
            if (entregado < total)
            {
                mostrarEfectivoInsuficiente(total - entregado);
                continue;
            }
            recibido = entregado;
            cambio = entregado - total;
            mostrarCambio(cambio);
            return true;
        }
    }
    return false;
}

// Imprime en pantalla la factura que se acaba de guardar
void mostrarFacturaEmitida(const Venta &venta, const Carrito &carrito)
{
    string *lineas = new string[lineasDeFactura(carrito)];
    int n = facturaATexto(venta, carrito, lineas);
    for (int i = 0; i < n; i++)
    {
        mostrarMensaje(lineas[i]);
    }
    delete[] lineas;
}

// Cobra al cliente que esta al frente de la fila de la caja del cajero:
// productos -> canje de puntos -> metodo de pago -> confirmacion -> factura.
void procesarCobro(const string &cajeroActual, int caja, Cola filas[], Pila pilas[],
                   Inventario &inventario, Clientes &clientes, Ventas &ventas)
{
    mostrarTituloCobro();
    if (cajeroActual == "" || caja < 1 || caja > NUM_CAJAS)
    {
        mostrarMensaje("Debe iniciar sesion para procesar cobros.");
        return;
    }
    Cola &fila = filas[caja - 1];
    Turno turno;
    if (!fila.verFrente(turno))
    {
        mostrarCobroSinFila(caja);
        return;
    }
    if (inventario.total() == 0)
    {
        mostrarMensaje("El inventario esta vacio: no hay productos para cobrar.");
        return;
    }
    if (numeroParaFactura(ventas) == 0)
    {
        mostrarSinNumerosDeFactura();
        return;
    }

    string nombreCliente;
    int puntosCliente;
    datosDeCliente(clientes, turno.codigoCliente, nombreCliente, puntosCliente);
    mostrarAtendiendo(caja, turno.numero, turno.codigoCliente, nombreCliente, puntosCliente);

    // 1. productos
    Carrito carrito;
    if (!armarCarrito(carrito, inventario))
    {
        mostrarCancelarCobro();
        return;
    }

    // 2. canje de puntos (solo clientes con codigo)
    long long subtotal = carrito.subtotal();
    int canjes = 0;
    if (turno.codigoCliente != 0)
    {
        canjes = pedirCanjes(puntosCliente, subtotal);
    }
    long long descuento = descuentoPorCanjes(canjes);
    long long total = subtotal - descuento;
    mostrarResumenCobro(subtotal, descuento, total);

    // 3. pago
    int metodo = 0;
    long long recibido = 0;
    long long cambio = 0;
    if (entradaTerminada || !pedirPago(total, metodo, recibido, cambio))
    {
        mostrarCancelarCobro();
        return;
    }

    // 4. confirmacion
    int confirmar = leerEntero("Confirmar el cobro? 1. Si  2. No: ", 1, 2);
    if (entradaTerminada || confirmar != 1)
    {
        mostrarCancelarCobro();
        return;
    }

    // 5. se arma la venta
    Venta venta;
    venta.numero = numeroParaFactura(ventas);
    venta.fecha = fechaActual();
    venta.hora = horaActual();
    venta.caja = caja;
    venta.cajero = cajeroActual;
    venta.codigoCliente = turno.codigoCliente;
    venta.nombreCliente = nombreCliente;
    venta.metodoPago = metodo;
    venta.subtotal = subtotal;
    venta.descuento = descuento;
    venta.total = total;
    venta.recibido = recibido;
    venta.cambio = cambio;
    venta.puntosCanjeados = canjes * PUNTOS_POR_CANJE;
    venta.puntosGanados = 0;
    venta.puntosSaldo = 0;
    if (turno.codigoCliente != 0)
    {
        int trasCanje = puntosCliente - venta.puntosCanjeados;
        venta.puntosGanados = puntosAcreditables(trasCanje, puntosGanados(total));
        venta.puntosSaldo = trasCanje + venta.puntosGanados;
    }

    // Se comprueba todo ANTES de cambiar nada
    if (venta.numero == 0 || !registroVentaValido(registroDeVenta(venta)) ||
        !carritoDisponible(carrito, inventario))
    {
        mostrarErrorCobroInterno();
        return;
    }
    if (!guardarFactura(venta, carrito))
    {
        mostrarErrorFactura();
        return;
    }

    // 6. ya con la factura guardada, se aplican los cambios
    descontarCarrito(inventario, carrito);
    if (turno.codigoCliente != 0)
    {
        clientes.restarPuntos(turno.codigoCliente, venta.puntosCanjeados);
        clientes.sumarPuntos(turno.codigoCliente, venta.puntosGanados);
    }
    ventas.agregar(registroDeVenta(venta));
    fila.eliminar(turno);
    pilas[caja - 1].push(cobroParaDeshacer(venta, carrito));   // para poder deshacerlo

    guardarCambios(inventario);
    if (turno.codigoCliente != 0)
    {
        guardarCambiosClientes(clientes);
    }
    guardarCambiosVentas(ventas);

    mostrarCobroExitoso(venta.numero);
    mostrarFacturaEmitida(venta, carrito);
    if (turno.codigoCliente != 0)
    {
        mostrarPuntosGanados(venta.puntosGanados, venta.puntosSaldo);
    }
}


// Submenu de cobros: se queda aqui hasta que el cajero elige volver
void gestionarCobros(const string &cajeroActual, int caja, Cola filas[], Pila pilas[],
                     Inventario &inventario, Clientes &clientes, Ventas &ventas)
{
    int opcion;
    do
    {
        mostrarMenuCobros(caja);
        opcion = leerEntero("Seleccione una opcion: ", 0, 4);
        mostrarMensaje("");

        switch (opcion)
        {
        case 1:
            procesarCobro(cajeroActual, caja, filas, pilas, inventario, clientes, ventas);
            break;
        case 2:
            deshacerUltimoCobro(caja, pilas, inventario, clientes, ventas);
            break;
        case 3:
            gestionarFacturas(ventas);
            break;
        case 4:
            gestionarArqueo(cajeroActual, caja, ventas);
            break;
        }
    } while (opcion != 0);
}

#endif
