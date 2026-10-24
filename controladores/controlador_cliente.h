#ifndef CONTROLADORES_CONTROLADOR_CLIENTE_H
#define CONTROLADORES_CONTROLADOR_CLIENTE_H

#include <string>
#include "../core/core.h"
#include "../modelos/cliente.h"
#include "../vistas/vista_cliente.h"
#include "../vistas/vista_menu.h"
#include "helpers.h"

// ---------- Clientes ----------

// Guarda los clientes en el archivo. Si falla, avisa y devuelve false.
bool guardarCambiosClientes(const Clientes &clientes)
{
    if (!clientes.guardar())
    {
        mostrarMensaje("Error: no se pudo guardar clientes.txt. El cambio solo esta en memoria.");
        return false;
    }
    return true;
}

// Le pasa a la Vista los datos de un cliente
void mostrarFilaDeCliente(const Cliente &c)
{
    mostrarFilaCliente(c.codigo, c.nombre, c.puntos);
}

// Pide el nombre, registra al cliente (el Modelo le asigna el codigo) y lo
// guarda. Devuelve el codigo nuevo, o 0 si no se pudo registrar.
int registrarCliente(Clientes &clientes)
{
    mostrarMensaje("--- Registrar cliente ---");

    string nombre = leerTextoCampo("Nombre del cliente: ", MAX_NOMBRE_CLIENTE);
    if (entradaTerminada)
    {
        return 0;
    }
    int codigo = clientes.registrar(nombre);
    if (codigo == 0)
    {
        mostrarMensaje("Error: no se pudo registrar al cliente (ya no quedan codigos).");
        return 0;
    }
    mostrarClienteRegistrado(codigo, nombre);
    guardarCambiosClientes(clientes);
    return codigo;
}

// Pide un codigo y muestra el cliente. Devuelve ese codigo, o 0 si no
// existe o se cierra la entrada.
int pedirCodigoCliente(const Clientes &clientes)
{
    int codigo = leerEntero("Codigo del cliente: ", 1, MAX_CODIGO_CLIENTE);
    if (entradaTerminada)
    {
        return 0;
    }
    if (clientes.buscar(codigo) == -1)
    {
        mostrarMensaje("No existe un cliente con ese codigo.");
        return 0;
    }
    return codigo;
}

// Muestra el nombre y los puntos de un cliente que ya se sabe que existe
void mostrarClientePorCodigo(const Clientes &clientes, int codigo)
{
    const Cliente &c = clientes.obtener(clientes.buscar(codigo));
    mostrarClienteIdentificado(c.codigo, c.nombre, c.puntos);
}

void consultarPuntosCliente(const Clientes &clientes)
{
    mostrarMensaje("--- Consultar puntos ---");
    if (clientes.total() == 0)
    {
        mostrarMensaje("No hay clientes registrados.");
        return;
    }
    int codigo = pedirCodigoCliente(clientes);
    if (codigo != 0)
    {
        mostrarClientePorCodigo(clientes, codigo);
    }
}

// Pide parte del nombre y muestra las coincidencias (hasta
// MAX_RESULTADOS_BUSQUEDA). Devuelve cuantas hay en total, 0 si ninguna.
int buscarClientesPorNombre(const Clientes &clientes)
{
    mostrarMensaje("--- Buscar cliente por nombre ---");
    if (clientes.total() == 0)
    {
        mostrarMensaje("No hay clientes registrados.");
        return 0;
    }
    string texto = leerLinea("Nombre (o parte del nombre): ");
    if (entradaTerminada)
    {
        return 0;
    }
    if (texto.find_first_not_of(" \t\r") == string::npos)
    {
        mostrarMensaje("Error: escriba al menos una letra.");
        return 0;
    }

    int posiciones[MAX_RESULTADOS_BUSQUEDA];
    int total = clientes.buscarPorNombre(texto, posiciones, MAX_RESULTADOS_BUSQUEDA);
    if (total == 0)
    {
        mostrarMensaje("No se encontraron clientes con ese nombre.");
        return 0;
    }

    int mostradas = (total < MAX_RESULTADOS_BUSQUEDA) ? total : MAX_RESULTADOS_BUSQUEDA;
    mostrarEncabezadoClientes();
    for (int i = 0; i < mostradas; i++)
    {
        mostrarFilaDeCliente(clientes.obtener(posiciones[i]));
    }
    mostrarResumenBusqueda(mostradas, total);
    return total;
}

// Busca por nombre y deja que el cajero escriba el codigo del cliente que
// corresponde. Devuelve ese codigo, o 0 si no encontro o el cajero cancela.
int elegirClientePorNombre(const Clientes &clientes)
{
    if (buscarClientesPorNombre(clientes) == 0)
    {
        return 0;
    }
    int codigo = leerEntero("Codigo del cliente (0 para volver): ", 0, MAX_CODIGO_CLIENTE);
    if (entradaTerminada || codigo == 0)
    {
        return 0;
    }
    if (clientes.buscar(codigo) == -1)
    {
        mostrarMensaje("No existe un cliente con ese codigo.");
        return 0;
    }
    return codigo;
}

// Pregunta quien es el cliente (al ponerlo en la fila). Se repite el menu
// hasta que se identifica a alguien o se elige "Sin puntos".
// Devuelve el codigo del cliente, o 0 si compra sin puntos o se cierra la entrada.
int identificarCliente(Clientes &clientes)
{
    while (!entradaTerminada)
    {
        mostrarMenuIdentificarCliente();
        int opcion = leerEntero("Seleccione una opcion: ", 0, 3);
        if (entradaTerminada || opcion == 0)
        {
            return 0;
        }
        mostrarMensaje("");

        int codigo = 0;
        if (opcion == 1)
        {
            codigo = pedirCodigoCliente(clientes);
        }
        else if (opcion == 2)
        {
            codigo = registrarCliente(clientes);
        }
        else
        {
            codigo = elegirClientePorNombre(clientes);
        }

        if (codigo > 0)
        {
            if (opcion != 2)
            {
                // el cliente nuevo ya se mostro al registrarlo
                mostrarClientePorCodigo(clientes, codigo);
            }
            return codigo;
        }
    }
    return 0;
}

// Submenu de clientes: se queda aqui hasta que el cajero elige volver
void gestionarClientes(Clientes &clientes)
{
    int opcion;
    do
    {
        mostrarMenuClientes();
        opcion = leerEntero("Seleccione una opcion: ", 0, 3);
        mostrarMensaje("");

        switch (opcion)
        {
        case 1:
            registrarCliente(clientes);
            break;
        case 2:
            consultarPuntosCliente(clientes);
            break;
        case 3:
            buscarClientesPorNombre(clientes);
            break;
        }
    } while (opcion != 0);
}

#endif
