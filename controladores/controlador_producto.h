#ifndef CONTROLADORES_CONTROLADOR_PRODUCTO_H
#define CONTROLADORES_CONTROLADOR_PRODUCTO_H

#include <string>
#include "../core/core.h"
#include "../modelos/producto.h"
#include "../vistas/vista_menu.h"
#include "../vistas/vista_titulo.h"
#include "../vistas/vista_producto.h"
#include "helpers.h"

// ---------- Inventario ----------

// Le pasa a la Vista los datos de un producto (la Vista no conoce al Modelo)
void mostrarFilaProducto(const Producto &p)
{
    mostrarFilaInventario(p.codigo, p.nombre, p.categoria, p.precio, p.cantidad, p.danados);
}

// Guarda el inventario en el archivo. Si falla, avisa y devuelve false.
bool guardarCambios(const Inventario &inventario)
{
    if (!inventario.guardar())
    {
        mostrarMensaje("Error: no se pudo guardar inventario.txt. El cambio solo esta en memoria.");
        return false;
    }
    return true;
}

void listarInventario(const Inventario &inventario)
{
    if (inventario.total() == 0)
    {
        mostrarMensaje("El inventario esta vacio.");
        return;
    }
    mostrarTitulo("Inventario");
    mostrarEncabezadoInventario();
    for (int i = 0; i < inventario.total(); i++)
    {
        mostrarFilaProducto(inventario.obtener(i));
    }
    mostrarTotalProductos(inventario.total());
}

void agregarProducto(Inventario &inventario)
{
    mostrarTitulo("Agregar producto");

    Producto p;
    p.codigo = leerEntero("Codigo: ", 1, MAX_CODIGO);
    if (entradaTerminada)
    {
        return;
    }
    if (inventario.buscar(p.codigo) != -1)
    {
        mostrarMensaje("Error: ya existe un producto con ese codigo.");
        return;
    }
    p.nombre = leerTextoCampo("Nombre: ", MAX_NOMBRE_PRODUCTO);
    p.categoria = leerTextoCampo("Categoria: ", MAX_CATEGORIA_PRODUCTO);
    p.precio = leerDecimal("Precio en Q (mayor que 0): ", 0.01, MAX_PRECIO);
    p.cantidad = leerEntero("Cantidad disponible (0 o mas): ", 0, MAX_CANTIDAD);
    p.danados = 0;
    if (entradaTerminada)
    {
        return;
    }

    if (!inventario.agregar(p))
    {
        mostrarMensaje("Error: no se pudo agregar el producto.");
        return;
    }
    if (guardarCambios(inventario))
    {
        mostrarMensaje("Producto agregado correctamente.");
    }
}

void buscarProducto(const Inventario &inventario)
{
    mostrarTitulo("Buscar producto");
    if (inventario.total() == 0)
    {
        mostrarMensaje("El inventario esta vacio.");
        return;
    }
    int codigo = leerEntero("Codigo a buscar: ", 1, MAX_CODIGO);
    if (entradaTerminada)
    {
        return;
    }
    int posicion = inventario.buscar(codigo);
    if (posicion == -1)
    {
        mostrarMensaje("No existe un producto con ese codigo.");
        return;
    }
    mostrarEncabezadoInventario();
    mostrarFilaProducto(inventario.obtener(posicion));
}

void modificarProducto(Inventario &inventario)
{
    mostrarTitulo("Modificar producto");
    if (inventario.total() == 0)
    {
        mostrarMensaje("El inventario esta vacio.");
        return;
    }
    int codigo = leerEntero("Codigo del producto: ", 1, MAX_CODIGO);
    if (entradaTerminada)
    {
        return;
    }
    int posicion = inventario.buscar(codigo);
    if (posicion == -1)
    {
        mostrarMensaje("No existe un producto con ese codigo.");
        return;
    }

    mostrarMensaje("Datos actuales:");
    mostrarEncabezadoInventario();
    mostrarFilaProducto(inventario.obtener(posicion));
    mostrarMensaje("Escriba los datos nuevos (el codigo no cambia).");

    string nombre = leerTextoCampo("Nombre: ", MAX_NOMBRE_PRODUCTO);
    string categoria = leerTextoCampo("Categoria: ", MAX_CATEGORIA_PRODUCTO);
    double precio = leerDecimal("Precio en Q (mayor que 0): ", 0.01, MAX_PRECIO);
    int cantidad = leerEntero("Cantidad disponible (0 o mas): ", 0, MAX_CANTIDAD);
    if (entradaTerminada)
    {
        return;
    }

    if (!inventario.modificar(codigo, nombre, categoria, precio, cantidad))
    {
        mostrarMensaje("Error: no se pudo modificar el producto.");
        return;
    }
    if (guardarCambios(inventario))
    {
        mostrarMensaje("Producto modificado correctamente.");
    }
}

void eliminarProducto(Inventario &inventario)
{
    mostrarTitulo("Eliminar producto");
    if (inventario.total() == 0)
    {
        mostrarMensaje("El inventario esta vacio.");
        return;
    }
    int codigo = leerEntero("Codigo del producto: ", 1, MAX_CODIGO);
    if (entradaTerminada)
    {
        return;
    }
    int posicion = inventario.buscar(codigo);
    if (posicion == -1)
    {
        mostrarMensaje("No existe un producto con ese codigo.");
        return;
    }

    mostrarEncabezadoInventario();
    mostrarFilaProducto(inventario.obtener(posicion));
    int confirmar = leerEntero("Eliminar este producto? 1. Si  2. No: ", 1, 2);
    if (entradaTerminada || confirmar != 1)
    {
        mostrarMensaje("Eliminacion cancelada.");
        return;
    }

    inventario.eliminar(codigo);
    if (guardarCambios(inventario))
    {
        mostrarMensaje("Producto eliminado correctamente.");
    }
}

void ordenarInventario(Inventario &inventario)
{
    if (inventario.total() < 2)
    {
        mostrarMensaje("Se necesitan al menos 2 productos para ordenar.");
        return;
    }
    mostrarMenuOrden();
    int opcion = leerEntero("Seleccione una opcion: ", 0, 2);
    if (entradaTerminada || opcion == 0)
    {
        return;
    }

    int criterio = (opcion == 1) ? ORDEN_POR_NOMBRE : ORDEN_POR_CATEGORIA;
    inventario.ordenar(criterio);
    if (guardarCambios(inventario))
    {
        mostrarMensaje(criterio == ORDEN_POR_NOMBRE ? "Inventario ordenado por nombre."
                                                    : "Inventario ordenado por categoria.");
        listarInventario(inventario);
    }
}

// Submenu de inventario: se queda aqui hasta que el cajero elige volver
void gestionarInventario(Inventario &inventario)
{
    int opcion;
    do
    {
        mostrarMenuInventario();
        opcion = leerEntero("Seleccione una opcion: ", 0, 6);
        mostrarMensaje("");

        switch (opcion)
        {
        case 1:
            listarInventario(inventario);
            break;
        case 2:
            agregarProducto(inventario);
            break;
        case 3:
            buscarProducto(inventario);
            break;
        case 4:
            modificarProducto(inventario);
            break;
        case 5:
            eliminarProducto(inventario);
            break;
        case 6:
            ordenarInventario(inventario);
            break;
        }
    } while (opcion != 0);
}

#endif
