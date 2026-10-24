#ifndef MODELOS_COLA_H
#define MODELOS_COLA_H

#include <string>
#include "../core/core.h"

// Lo que se guarda por cada cliente en la fila
struct Turno
{
    int numero;          // turno de esa caja (1, 2, 3...), no se repite
    int codigoCliente;   // codigo del cliente; 0 = compra sin puntos
};

struct NodoCola
{
    Turno dato;
    NodoCola *siguiente;
};

class Cola
{
private:
    NodoCola *frente;   
    NodoCola *fin;       
    int cantidad;      
    int proximoTurno;   

    // Sin copia: dos copias liberarian los mismos nodos
    Cola(const Cola &otra);
    Cola &operator=(const Cola &otra);

public:
    Cola()
    {
        frente = nullptr;
        fin = nullptr;
        cantidad = 0;
        proximoTurno = 1;
    }

    // Libera todos los nodos que queden en la fila
    ~Cola()
    {
        while (frente != nullptr)
        {
            NodoCola *viejo = frente;
            frente = frente->siguiente;
            delete viejo;
        }
    }

    bool estaVacia() const
    {
        return frente == nullptr;
    }

    bool estaLlena() const
    {
        return cantidad >= MAX_CLIENTES_FILA;
    }

    int total() const
    {
        return cantidad;
    }

    // Agrega un cliente al final de la fila y devuelve su numero de turno.
    // Devuelve 0 si la fila esta llena o el codigo no es valido
    // (de 0 a MAX_CODIGO_CLIENTE; 0 significa "sin puntos").
    int insertar(int codigoCliente)
    {
        if (estaLlena() || codigoCliente < 0 || codigoCliente > MAX_CODIGO_CLIENTE)
        {
            return 0;
        }
        NodoCola *nuevo = new NodoCola;
        nuevo->dato.numero = proximoTurno;
        nuevo->dato.codigoCliente = codigoCliente;
        nuevo->siguiente = nullptr;

        if (estaVacia())
        {
            frente = nuevo;
        }
        else
        {
            fin->siguiente = nuevo;
        }
        fin = nuevo;
        cantidad++;
        proximoTurno++;
        return nuevo->dato.numero;
    }

    // Saca al cliente del frente y lo deja en "turno". false si la fila esta vacia.
    bool eliminar(Turno &turno)
    {
        if (estaVacia())
        {
            return false;
        }
        NodoCola *viejo = frente;
        turno = viejo->dato;
        frente = frente->siguiente;
        if (frente == nullptr)
        {
            fin = nullptr;
        }
        delete viejo;
        cantidad--;
        return true;
    }

    // Ve al cliente del frente sin sacarlo. false si la fila esta vacia.
    bool verFrente(Turno &turno) const
    {
        if (estaVacia())
        {
            return false;
        }
        turno = frente->dato;
        return true;
    }

    // Ve al cliente que esta en esa posicion (0 = el frente) sin sacarlo.
    // false si la posicion no existe. Sirve para mostrar toda la fila.
    bool obtener(int posicion, Turno &turno) const
    {
        if (posicion < 0 || posicion >= cantidad)
        {
            return false;
        }
        NodoCola *actual = frente;
        for (int i = 0; i < posicion; i++)
        {
            actual = actual->siguiente;
        }
        turno = actual->dato;
        return true;
    }

    // true si el cliente (codigo mayor que 0) ya esta en esta fila
    bool contieneCliente(int codigoCliente) const
    {
        if (codigoCliente <= 0)
        {
            return false;
        }
        for (NodoCola *actual = frente; actual != nullptr; actual = actual->siguiente)
        {
            if (actual->dato.codigoCliente == codigoCliente)
            {
                return true;
            }
        }
        return false;
    }
};

#endif
