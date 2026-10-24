#ifndef MODELOS_CARRITO_H
#define MODELOS_CARRITO_H

#include <string>
#include "../core/core.h"
#include "producto.h"

struct LineaVenta
{
    int codigo;
    string nombre;
    long long precio;   // precio de UNA unidad, en centavos
    int cantidad;
};

class Carrito
{
private:
    LineaVenta *lineas;
    int numLineas;
    int capacidad;

    // Sin copia, igual que las otras colecciones
    Carrito(const Carrito &otro);
    Carrito &operator=(const Carrito &otro);

    void crecer()
    {
        int nuevaCapacidad = (capacidad == 0) ? CAPACIDAD_INICIAL : capacidad * 2;
        LineaVenta *nuevo = new LineaVenta[nuevaCapacidad];
        for (int i = 0; i < numLineas; i++)
        {
            nuevo[i] = lineas[i];
        }
        delete[] lineas;
        lineas = nuevo;
        capacidad = nuevaCapacidad;
    }

public:
    Carrito()
    {
        lineas = nullptr;
        numLineas = 0;
        capacidad = 0;
    }

    ~Carrito()
    {
        delete[] lineas;
    }

    // Cuantas lineas (productos distintos) hay
    int total() const
    {
        return numLineas;
    }

    const LineaVenta &obtener(int posicion) const
    {
        return lineas[posicion];
    }

    // Posicion de la linea de ese producto, o -1 si no esta en el carrito
    int buscar(int codigo) const
    {
        for (int i = 0; i < numLineas; i++)
        {
            if (lineas[i].codigo == codigo)
            {
                return i;
            }
        }
        return -1;
    }

    // Unidades de ese producto que ya estan en el carrito (0 si no esta)
    int cantidadDe(int codigo) const
    {
        int pos = buscar(codigo);
        return (pos == -1) ? 0 : lineas[pos].cantidad;
    }

    // Agrega unidades de un producto (si ya estaba, las suma a su linea).
    // false si algun dato no es valido, si la linea pasaria de MAX_CANTIDAD
    // unidades o si ya hay MAX_LINEAS_CARRITO productos distintos.
    bool agregar(int codigo, const string &nombre, long long precio, int cantidad)
    {
        if (codigo < 1 || precio < 1 || cantidad < 1 || cantidad > MAX_CANTIDAD)
        {
            return false;
        }
        int pos = buscar(codigo);
        if (pos != -1)
        {
            if (lineas[pos].cantidad > MAX_CANTIDAD - cantidad)
            {
                return false;
            }
            lineas[pos].cantidad += cantidad;
            return true;
        }
        if (numLineas >= MAX_LINEAS_CARRITO)
        {
            return false;
        }
        if (numLineas == capacidad)
        {
            crecer();
        }
        lineas[numLineas].codigo = codigo;
        lineas[numLineas].nombre = nombre;
        lineas[numLineas].precio = precio;
        lineas[numLineas].cantidad = cantidad;
        numLineas++;
        return true;
    }

    // Quita la linea completa de ese producto. false si no estaba.
    bool quitar(int codigo)
    {
        int pos = buscar(codigo);
        if (pos == -1)
        {
            return false;
        }
        for (int i = pos; i < numLineas - 1; i++)
        {
            lineas[i] = lineas[i + 1];
        }
        numLineas--;
        return true;
    }

    // Suma de precio * cantidad de todas las lineas, en centavos
    long long subtotal() const
    {
        long long suma = 0;
        for (int i = 0; i < numLineas; i++)
        {
            suma += lineas[i].precio * lineas[i].cantidad;
        }
        return suma;
    }
};

// true si el inventario tiene suficientes unidades de todo lo que hay en el carrito
bool carritoDisponible(const Carrito &carrito, const Inventario &inventario)
{
    for (int i = 0; i < carrito.total(); i++)
    {
        int pos = inventario.buscar(carrito.obtener(i).codigo);
        if (pos == -1 || inventario.obtener(pos).cantidad < carrito.obtener(i).cantidad)
        {
            return false;
        }
    }
    return true;
}

// Resta del inventario todo lo que hay en el carrito. false (y no cambia
// nada) si no alcanza alguno de los productos.
bool descontarCarrito(Inventario &inventario, const Carrito &carrito)
{
    if (!carritoDisponible(carrito, inventario))
    {
        return false;
    }
    for (int i = 0; i < carrito.total(); i++)
    {
        inventario.descontar(carrito.obtener(i).codigo, carrito.obtener(i).cantidad);
    }
    return true;
}

#endif
