#ifndef MODELOS_CAJERO_H
#define MODELOS_CAJERO_H

#include <string>
#include "../core/archivos.h"
#include "../core/core.h"

struct Cajero
{
    string usuario;
    string clave;
    int caja;   // caja fija del cajero, de 1 a NUM_CAJAS (0 = todavia sin asignar)
};

// true si hay al menos un cajero registrado
bool hayCajeros()
{
    return hayCajerosEnArchivo();
}

// Cuantos cajeros hay registrados. Como cada uno ocupa una caja, solo
// se pueden registrar NUM_CAJAS.
int cantidadCajeros()
{
    return contarCajerosEnArchivo();
}

// true si ya existe un cajero con ese usuario
bool existeCajero(const string &usuario)
{
    return existeUsuarioEnArchivo(usuario);
}

// true si usuario Y contrasena coinciden con un cajero registrado.
// En ese caso llena su caja (0 si es un cajero antiguo sin caja).
bool autenticarCajero(Cajero &cajero)
{
    int caja = 0;
    if (!buscarCredencialEnArchivo(cajero.usuario, cajero.clave, caja))
    {
        return false;
    }
    cajero.caja = caja;
    return true;
}

// true si algun cajero ya tiene esa caja
bool cajaOcupada(int caja)
{
    return cajaOcupadaEnArchivo(caja);
}

// Numero de la primera caja sin cajero, o 0 si estan todas ocupadas
int primeraCajaLibre()
{
    for (int caja = 1; caja <= NUM_CAJAS; caja++)
    {
        if (!cajaOcupada(caja))
        {
            return caja;
        }
    }
    return 0;
}

// Registra un cajero nuevo con su caja. false si no se pudo guardar.
bool guardarCajero(const Cajero &cajero)
{
    return agregarCajeroAlArchivo(cajero.usuario, cajero.clave, cajero.caja);
}

// Le da caja a un cajero que no tenia. false si no se pudo guardar.
bool asignarCaja(const string &usuario, int caja)
{
    return asignarCajaEnArchivo(usuario, caja);
}

#endif
