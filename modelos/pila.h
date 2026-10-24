#ifndef MODELOS_PILA_H
#define MODELOS_PILA_H

#include <string>
#include "../core/core.h"
#include "carrito.h"
#include "puntos.h"
#include "venta.h"

// Lo que hace falta para deshacer un cobro
struct CobroDeshacer
{
    int numeroFactura;
    int codigoCliente;      // 0 = compra sin puntos
    int puntosCanjeados;    // se le devuelven al cliente
    int puntosGanados;      // se le quitan al cliente
    int metodoPago;
    long long total;        // lo que pago, en centavos
    int numLineas;
    LineaVenta lineas[MAX_LINEAS_CARRITO];   // productos que salieron del stock
};

CobroDeshacer cobroParaDeshacer(const Venta &v, const Carrito &carrito)
{
    CobroDeshacer c;
    c.numeroFactura = v.numero;
    c.codigoCliente = v.codigoCliente;
    c.puntosCanjeados = v.puntosCanjeados;
    c.puntosGanados = v.puntosGanados;
    c.metodoPago = v.metodoPago;
    c.total = v.total;
    c.numLineas = carrito.total();
    for (int i = 0; i < carrito.total() && i < MAX_LINEAS_CARRITO; i++)
    {
        c.lineas[i] = carrito.obtener(i);
    }
    return c;
}

// Un nodo de la pila: el dato y un puntero al nodo de abajo
struct NodoPila
{
    CobroDeshacer dato;
    NodoPila *siguiente;
};

class Pila
{
private:
    NodoPila *cima;     // el ultimo cobro apilado
    int cantidad;

    // Sin copia: dos pilas apuntando a los mismos nodos liberarian dos veces
    Pila(const Pila &otra);
    Pila &operator=(const Pila &otra);

public:
    Pila()
    {
        cima = nullptr;
        cantidad = 0;
    }

    ~Pila()
    {
        while (cima != nullptr)
        {
            NodoPila *borrar = cima;
            cima = cima->siguiente;
            delete borrar;
        }
    }

    bool estaVacia() const
    {
        return cima == nullptr;
    }

    int total() const
    {
        return cantidad;
    }

    // Apila un cobro (queda en la cima)
    void push(const CobroDeshacer &cobro)
    {
        NodoPila *nuevo = new NodoPila;
        nuevo->dato = cobro;
        nuevo->siguiente = cima;
        cima = nuevo;
        cantidad++;
    }

    // Desapila el cobro de la cima y lo deja en "cobro". false si esta vacia.
    bool pop(CobroDeshacer &cobro)
    {
        if (cima == nullptr)
        {
            return false;
        }
        NodoPila *borrar = cima;
        cobro = borrar->dato;
        cima = cima->siguiente;
        delete borrar;
        cantidad--;
        return true;
    }

    // Copia el cobro de la cima SIN sacarlo. false si esta vacia.
    bool verCima(CobroDeshacer &cobro) const
    {
        if (cima == nullptr)
        {
            return false;
        }
        cobro = cima->dato;
        return true;
    }
};

// Se pueden devolver los puntos de un cobro si el cliente todavia tiene los
// que gano en el (no los gasto en otra compra) y al devolverle los canjeados
// no pasa de MAX_PUNTOS.
bool puedeRevertirPuntos(int puntosActuales, int ganados, int canjeados)
{
    if (puntosActuales < 0 || ganados < 0 || canjeados < 0 || puntosActuales < ganados)
    {
        return false;
    }
    return (long long)puntosActuales - ganados + canjeados <= MAX_PUNTOS;
}

#endif
