#include <iostream>
#include <string>
#include "core/core.h"
#include "modelos/cajero.h"
#include "modelos/producto.h"
#include "modelos/cliente.h"
#include "modelos/cola.h"
#include "modelos/pila.h"
#include "modelos/devolucion.h"
#include "modelos/registro_venta.h"
#include "vistas/vista_menu.h"
#include "controladores/controlador_cajero.h"
#include "controladores/controlador_producto.h"
#include "controladores/controlador_cliente.h"
#include "controladores/controlador_fila.h"
#include "controladores/controlador_cobro.h"
#include "controladores/controlador_devolucion.h"
using namespace std;

int main()
{
    string cajeroActual = "";
    int cajaActual = 0;   // caja fija del cajero que inicio sesion
    int opcion;

    // una fila por caja: filas[0] es la caja 1. Viven mientras corre el
    // programa, aunque cambie el cajero que tiene la sesion abierta.
    Cola filas[NUM_CAJAS];

    // una pila por caja con sus cobros, para poder deshacer el ultimo
    Pila pilas[NUM_CAJAS];

    // primera ejecucion: sin cajeros no hay quien inicie sesion
    if (!hayCajeros())
    {
        mostrarMensaje("No hay cajeros registrados. Registre el primero.");
        while (!hayCajeros() && !entradaTerminada)
        {
            registrarCajero();
        }
    }

    cajeroActual = iniciarSesion(cajaActual);
    if (cajeroActual == "")
    {
        mostrarMensaje("No se pudo iniciar sesion. Saliendo del programa...");
        return 0;
    }

    // el inventario y los clientes se cargan una vez, con la sesion ya iniciada
    Inventario inventario;
    int ignoradas = inventario.cargar();
    if (ignoradas > 0)
    {
        mostrarAdvertenciaInventario(ignoradas);
    }
    Clientes clientes;
    int clientesIgnorados = clientes.cargar();
    if (clientesIgnorados > 0)
    {
        mostrarAdvertenciaClientes(clientesIgnorados);
    }
    Devoluciones devoluciones;
    int devolucionesIgnoradas = devoluciones.cargar();
    if (devolucionesIgnoradas > 0)
    {
        mostrarAdvertenciaDevoluciones(devolucionesIgnoradas);
    }
    Ventas ventas;
    int ventasIgnoradas = ventas.cargar();
    if (ventasIgnoradas > 0)
    {
        mostrarAdvertenciaVentas(ventasIgnoradas);
    }

    do
    {
        mostrarMenuPrincipal(cajeroActual, cajaActual);
        opcion = leerEntero("Seleccione una opcion: ", 0, 7);
        mostrarMensaje("");

        switch (opcion)
        {
        case 1:
            gestionarCobros(cajeroActual, cajaActual, filas, pilas, inventario, clientes, ventas);
            break;
        case 2:
            gestionarInventario(inventario);
            break;
        case 3:
            gestionarClientes(clientes);
            break;
        case 4:
            gestionarFilas(filas, clientes, cajaActual);
            break;
        case 5:
            gestionarDevoluciones(inventario, devoluciones, ventas, cajaActual);
            break;
        case 6:
            registrarCajero();
            break;
        case 7:
            cajeroActual = "";
            cajaActual = 0;
            mostrarMensaje("Sesion cerrada.");
            cajeroActual = iniciarSesion(cajaActual);
            if (cajeroActual == "")
            {
                mostrarMensaje("No se pudo iniciar sesion.");
                opcion = 0;
            }
            break;
        case 0:
            mostrarMensaje("Saliendo del programa...");
            break;
        }

    } while (opcion != 0);

    cin.get();
    return 0;
}
