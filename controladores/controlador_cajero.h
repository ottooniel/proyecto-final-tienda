#ifndef CONTROLADORES_CONTROLADOR_CAJERO_H
#define CONTROLADORES_CONTROLADOR_CAJERO_H

#include <sstream>
#include <string>
#include "../core/core.h"
#include "../modelos/cajero.h"
#include "../vistas/vista_cajero.h"
#include "../vistas/vista_menu.h"
#include "helpers.h"

// ---------- Gestion de cajeros ----------

// Muestra las cajas libres y pide una. Repite si la caja ya tiene cajero.
// Devuelve 0 si se cierra la entrada.
int pedirCajaLibre()
{
    bool libres[NUM_CAJAS];
    for (int i = 0; i < NUM_CAJAS; i++)
    {
        libres[i] = !cajaOcupada(i + 1);
    }
    mostrarCajasLibres(libres, NUM_CAJAS);

    ostringstream mensaje;
    mensaje << "Caja del cajero (1 a " << NUM_CAJAS << "): ";
    while (!entradaTerminada)
    {
        int caja = leerEntero(mensaje.str(), 1, NUM_CAJAS);
        if (entradaTerminada)
        {
            break;
        }
        if (!cajaOcupada(caja))
        {
            return caja;
        }
        mostrarCajaOcupada(caja);
    }
    return 0;
}

void registrarCajero()
{
    mostrarTituloRegistro();

    // cada cajero ocupa una caja, asi que no caben mas que cajas
    if (cantidadCajeros() >= NUM_CAJAS)
    {
        ostringstream mensaje;
        mensaje << "Error: ya estan registrados los " << NUM_CAJAS
                << " cajeros (una caja por cajero).";
        mostrarMensaje(mensaje.str());
        return;
    }

    Cajero cajero;
    cajero.caja = 0;
    cajero.usuario = leerLinea("Usuario: ");
    if (entradaTerminada || !campoValido(cajero.usuario))
    {
        return;
    }
    if (existeCajero(cajero.usuario))
    {
        mostrarMensaje("Error: el usuario ya existe.");
        return;
    }
    cajero.clave = leerLinea("Contrasena: ");
    if (entradaTerminada || !campoValido(cajero.clave))
    {
        return;
    }
    cajero.caja = pedirCajaLibre();
    if (entradaTerminada || cajero.caja == 0)
    {
        return;
    }

    if (guardarCajero(cajero))
    {
        mostrarMensaje("Cajero registrado correctamente.");
    }
    else
    {
        mostrarMensaje("Error: no se pudo abrir el archivo para escribir.");
    }
}

// Devuelve el usuario que inicio sesion y deja su caja en "caja".
// Devuelve "" si fallo todos los intentos o no se pudo asignar caja.
string iniciarSesion(int &caja)
{
    mostrarTituloLogin();
    for (int intento = 1; intento <= MAX_INTENTOS && !entradaTerminada; intento++)
    {
        Cajero cajero;
        cajero.caja = 0;
        cajero.usuario = leerLinea("Usuario: ");
        cajero.clave = leerLinea("Contrasena: ");
        if (entradaTerminada)
        {
            break;
        }
        if (autenticarCajero(cajero))
        {
            if (cajero.caja == 0)
            {
                // cajero registrado antes de que existieran las cajas:
                // se le da la primera caja libre y queda fija
                int libre = primeraCajaLibre();
                if (libre == 0 || !asignarCaja(cajero.usuario, libre))
                {
                    mostrarSinCajaDisponible();
                    return "";
                }
                cajero.caja = libre;
                mostrarCajaAsignada(libre);
            }
            caja = cajero.caja;
            mostrarBienvenida(cajero.usuario, caja);
            return cajero.usuario;
        }
        mostrarCredencialesIncorrectas(intento, MAX_INTENTOS);
    }
    return "";
}

#endif
