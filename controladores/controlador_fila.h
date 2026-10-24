#ifndef CONTROLADORES_CONTROLADOR_FILA_H
#define CONTROLADORES_CONTROLADOR_FILA_H

#include <sstream>
#include <string>
#include "../core/core.h"
#include "../modelos/cliente.h"
#include "../modelos/cola.h"
#include "../vistas/vista_fila.h"
#include "../vistas/vista_menu.h"
#include "controlador_cliente.h"

// ---------- Filas de clientes ----------
// Hay una fila (Cola) por caja: filas[0] es la caja 1, filas[1] la caja 2...

// Numero de la caja en cuya fila esta el cliente (codigo mayor que 0),
// o 0 si no esta en ninguna.
int cajaDondeEsta(const Cola filas[], int codigoCliente)
{
    for (int i = 0; i < NUM_CAJAS; i++)
    {
        if (filas[i].contieneCliente(codigoCliente))
        {
            return i + 1;
        }
    }
    return 0;
}

// Busca el nombre y los puntos de un cliente de la fila (con codigo 0 no hay datos)
void datosDeCliente(const Clientes &clientes, int codigo, string &nombre, int &puntos)
{
    nombre = "";
    puntos = 0;
    if (codigo == 0)
    {
        return;
    }
    int posicion = clientes.buscar(codigo);
    if (posicion == -1)
    {
        // no deberia pasar: los clientes no se eliminan
        nombre = "(cliente desconocido)";
        return;
    }
    nombre = clientes.obtener(posicion).nombre;
    puntos = clientes.obtener(posicion).puntos;
}

void mostrarTurnoEnFila(int posicion, const Turno &turno, const Clientes &clientes)
{
    string nombre;
    int puntos;
    datosDeCliente(clientes, turno.codigoCliente, nombre, puntos);
    mostrarClienteEnFila(posicion, turno.numero, turno.codigoCliente, nombre, puntos);
}

void verFilas(const Cola filas[], const Clientes &clientes, int cajaPropia)
{
    mostrarMensaje("--- Filas de clientes ---");
    for (int i = 0; i < NUM_CAJAS; i++)
    {
        mostrarEncabezadoFila(i + 1, filas[i].total(), i + 1 == cajaPropia);
        for (int p = 0; p < filas[i].total(); p++)
        {
            Turno turno;
            if (filas[i].obtener(p, turno))
            {
                mostrarTurnoEnFila(p + 1, turno, clientes);
            }
        }
    }
}

// Pregunta a que caja va el cliente, lo identifica (codigo o sin puntos) y
// lo pone al final de esa fila.
void agregarClienteAFila(Cola filas[], Clientes &clientes, int cajaPropia)
{
    mostrarMensaje("--- Agregar cliente a una fila ---");

    int totales[NUM_CAJAS];
    for (int i = 0; i < NUM_CAJAS; i++)
    {
        totales[i] = filas[i].total();
    }
    mostrarResumenFilas(totales, NUM_CAJAS, cajaPropia);

    ostringstream pregunta;
    pregunta << "Caja de la fila (1 a " << NUM_CAJAS << ", 0 para cancelar): ";
    int caja = leerEntero(pregunta.str(), 0, NUM_CAJAS);
    if (entradaTerminada || caja == 0)
    {
        return;
    }
    Cola &fila = filas[caja - 1];
    if (fila.estaLlena())
    {
        mostrarFilaLlena(caja, MAX_CLIENTES_FILA);
        return;
    }

    int codigo = identificarCliente(clientes);
    if (entradaTerminada)
    {
        return;
    }
    // un cliente no puede estar en dos filas a la vez
    int cajaActual = cajaDondeEsta(filas, codigo);
    if (cajaActual != 0)
    {
        mostrarClienteYaEnFila(cajaActual);
        return;
    }

    int turno = fila.insertar(codigo);
    if (turno == 0)
    {
        mostrarMensaje("Error: no se pudo agregar el cliente a la fila.");
        return;
    }
    mostrarClienteAgregado(caja, turno, fila.total() - 1);
}

// Saca al cliente del frente de la fila de su caja SIN cobrarle (se fue, se
// equivoco de fila...). Para cobrarle se usa "Procesar cobro".
void retirarDelFrente(Cola filas[], const Clientes &clientes, int caja)
{
    mostrarMensaje("--- Retirar al cliente del frente ---");
    Turno turno;
    if (!filas[caja - 1].verFrente(turno))
    {
        mostrarFilaVacia(caja);
        return;
    }
    string nombre;
    int puntos;
    datosDeCliente(clientes, turno.codigoCliente, nombre, puntos);
    mostrarClienteEnFila(1, turno.numero, turno.codigoCliente, nombre, puntos);

    int confirmar = leerEntero("Retirar a este cliente de la fila? 1. Si  2. No: ", 1, 2);
    if (entradaTerminada || confirmar != 1)
    {
        mostrarRetiroCancelado();
        return;
    }
    filas[caja - 1].eliminar(turno);
    mostrarClienteRetirado(caja, turno.numero);
}

// Submenu de filas: se queda aqui hasta que el cajero elige volver
void gestionarFilas(Cola filas[], Clientes &clientes, int cajaPropia)
{
    int opcion;
    do
    {
        mostrarMenuFilas(cajaPropia);
        opcion = leerEntero("Seleccione una opcion: ", 0, 3);
        mostrarMensaje("");

        switch (opcion)
        {
        case 1:
            agregarClienteAFila(filas, clientes, cajaPropia);
            break;
        case 2:
            verFilas(filas, clientes, cajaPropia);
            break;
        case 3:
            retirarDelFrente(filas, clientes, cajaPropia);
            break;
        }
    } while (opcion != 0);
}

#endif
