#ifndef MODELOS_PRODUCTO_H
#define MODELOS_PRODUCTO_H

#include <iomanip>
#include <sstream>
#include <string>
#include "../core/archivos.h"
#include "../core/core.h"
#include "../core/texto.h"

struct Producto
{
    int codigo;
    string nombre;
    string categoria;
    double precio;
    int cantidad;   // unidades disponibles para vender
    int danados;    // unidades danadas devueltas (no se pueden vender)
};

// Resultado de comprobar si se puede reponer stock (ver Inventario::puedeReponer)
const int REPOSICION_OK = 1;
const int REPOSICION_NO_EXISTE = 0;
const int REPOSICION_DESBORDE = -1;

// Reglas de un producto:
//   codigo >= 1, nombre y categoria validos, precio >= 0.01 (el archivo
//   guarda 2 decimales), cantidad y danados >= 0 (un producto puede agotarse).
bool productoValido(const Producto &p)
{
    if (p.codigo < 1)
    {
        return false;
    }
    if (!textoCampoValido(p.nombre, MAX_NOMBRE_PRODUCTO) ||
        !textoCampoValido(p.categoria, MAX_CATEGORIA_PRODUCTO))
    {
        return false;
    }
    if (p.precio < 0.01)
    {
        return false;
    }
    if (p.cantidad < 0 || p.danados < 0)
    {
        return false;
    }
    return true;
}

// Producto -> una linea de texto:  12|Cuaderno|Utiles|15.50|40|0
string productoATexto(const Producto &p)
{
    ostringstream flujo;
    flujo << p.codigo << SEPARADOR_CAMPO
          << p.nombre << SEPARADOR_CAMPO
          << p.categoria << SEPARADOR_CAMPO
          << fixed << setprecision(2) << p.precio << SEPARADOR_CAMPO
          << p.cantidad << SEPARADOR_CAMPO
          << p.danados;
    return flujo.str();
}

// Una linea de texto -> Producto. false si la linea esta mal formada
// (campos de menos o de mas, numeros invalidos o reglas incumplidas).
bool textoAProducto(const string &linea, Producto &p)
{
    // 6 campos = exactamente 5 separadores
    int separadores = 0;
    for (int i = 0; i < (int)linea.length(); i++)
    {
        if (linea[i] == SEPARADOR_CAMPO)
        {
            separadores++;
        }
    }
    if (separadores != 5)
    {
        return false;
    }

    string campos[6];
    int n = 0;
    istringstream flujo(linea);
    string campo;
    while (n < 6 && getline(flujo, campo, SEPARADOR_CAMPO))
    {
        campos[n] = campo;
        n++;
    }
    if (n != 6)
    {
        return false;
    }

    if (!textoAEntero(campos[0], p.codigo))
    {
        return false;
    }
    p.nombre = campos[1];
    p.categoria = campos[2];
    if (!textoADecimal(campos[3], p.precio))
    {
        return false;
    }
    if (!textoAEntero(campos[4], p.cantidad))
    {
        return false;
    }
    if (!textoAEntero(campos[5], p.danados))
    {
        return false;
    }
    return productoValido(p);
}

// ---------- Comparacion de productos (para ordenar) ----------

// true si el producto "a" debe ir DESPUES del producto "b".
//   ORDEN_POR_NOMBRE:    por nombre; si empatan, por codigo.
//   ORDEN_POR_CATEGORIA: por categoria; si empatan, por nombre y luego codigo.
// El desempate hace que el orden sea siempre el mismo.
bool debeIrDespues(const Producto &a, const Producto &b, int criterio)
{
    if (criterio == ORDEN_POR_CATEGORIA)
    {
        string categoriaA = aMinusculas(a.categoria);
        string categoriaB = aMinusculas(b.categoria);
        if (categoriaA != categoriaB)
        {
            return categoriaA > categoriaB;
        }
    }
    string nombreA = aMinusculas(a.nombre);
    string nombreB = aMinusculas(b.nombre);
    if (nombreA != nombreB)
    {
        return nombreA > nombreB;
    }
    return a.codigo > b.codigo;
}

class Inventario
{
private:
    Producto *productos;
    int numProductos;   // cuantos productos hay
    int capacidad;      // cuantos caben antes de crecer

    // No se permite copiar un Inventario (dos copias liberarian la
    // misma memoria). Por eso estas dos lineas estan declaradas y sin codigo.
    Inventario(const Inventario &otro);
    Inventario &operator=(const Inventario &otro);

    void crecer()
    {
        int nuevaCapacidad = (capacidad == 0) ? CAPACIDAD_INICIAL : capacidad * 2;
        Producto *nuevo = new Producto[nuevaCapacidad];
        for (int i = 0; i < numProductos; i++)
        {
            nuevo[i] = productos[i];
        }
        delete[] productos;
        productos = nuevo;
        capacidad = nuevaCapacidad;
    }

public:
    Inventario()
    {
        productos = nullptr;
        numProductos = 0;
        capacidad = 0;
    }

    ~Inventario()
    {
        delete[] productos;
    }

    // ----- Consultar -----

    int total() const
    {
        return numProductos;
    }

    // Producto en la posicion dada (de 0 a total()-1). Quien llama
    // debe asegurar que la posicion existe.
    const Producto &obtener(int posicion) const
    {
        return productos[posicion];
    }

    // Posicion del producto con ese codigo, o -1 si no existe
    int buscar(int codigo) const
    {
        for (int i = 0; i < numProductos; i++)
        {
            if (productos[i].codigo == codigo)
            {
                return i;
            }
        }
        return -1;
    }

    // ----- Crear, modificar, eliminar -----

    // false si el producto no cumple las reglas o el codigo ya existe
    bool agregar(const Producto &p)
    {
        if (!productoValido(p) || buscar(p.codigo) != -1)
        {
            return false;
        }
        if (numProductos == capacidad)
        {
            crecer();
        }
        productos[numProductos] = p;
        numProductos++;
        return true;
    }

    // Cambia nombre, categoria, precio y cantidad. El codigo y los danados
    // no cambian. false si no existe o los datos nuevos no son validos
    // (en ese caso el producto queda como estaba).
    bool modificar(int codigo, const string &nombre, const string &categoria,
                   double precio, int cantidadNueva)
    {
        int pos = buscar(codigo);
        if (pos == -1)
        {
            return false;
        }
        Producto cambio = productos[pos];
        cambio.nombre = nombre;
        cambio.categoria = categoria;
        cambio.precio = precio;
        cambio.cantidad = cantidadNueva;
        if (!productoValido(cambio))
        {
            return false;
        }
        productos[pos] = cambio;
        return true;
    }

    // Elimina el producto y corre los siguientes una posicion a la izquierda.
    // false si no existe.
    bool eliminar(int codigo)
    {
        int pos = buscar(codigo);
        if (pos == -1)
        {
            return false;
        }
        for (int i = pos; i < numProductos - 1; i++)
        {
            productos[i] = productos[i + 1];
        }
        numProductos--;
        return true;
    }

    // Marca unidades como danadas (producto devuelto que ya no se puede
    // vender). No toca la cantidad disponible: esas unidades ya salieron del
    // stock cuando se vendieron. false si el producto no existe, si las
    // unidades no son al menos 1 o si los danados pasarian de MAX_CANTIDAD
    // (en ese caso no cambia nada).
    bool marcarDanados(int codigo, int unidades)
    {
        int pos = buscar(codigo);
        if (pos == -1 || unidades < 1 || unidades > MAX_CANTIDAD - productos[pos].danados)
        {
            return false;
        }
        productos[pos].danados += unidades;
        return true;
    }

    // Estado de reponer al stock "unidades" de ese producto (al deshacer un
    // cobro): REPOSICION_OK, REPOSICION_NO_EXISTE (el producto ya se elimino
    // del inventario) o REPOSICION_DESBORDE (la cantidad pasaria de MAX_CANTIDAD).
    int puedeReponer(int codigo, int unidades) const
    {
        int pos = buscar(codigo);
        if (pos == -1)
        {
            return REPOSICION_NO_EXISTE;
        }
        if (unidades < 1 || unidades > MAX_CANTIDAD - productos[pos].cantidad)
        {
            return REPOSICION_DESBORDE;
        }
        return REPOSICION_OK;
    }

    // Suma al stock unidades que habian salido por una venta. false (sin
    // cambios) si puedeReponer no devuelve REPOSICION_OK.
    bool reponer(int codigo, int unidades)
    {
        if (puedeReponer(codigo, unidades) != REPOSICION_OK)
        {
            return false;
        }
        productos[buscar(codigo)].cantidad += unidades;
        return true;
    }

    // Quita unidades de la cuenta de danados (para deshacer un marcado).
    // false si el producto no existe o hay menos danados de los que se piden.
    bool quitarDanados(int codigo, int unidades)
    {
        int pos = buscar(codigo);
        if (pos == -1 || unidades < 1 || unidades > productos[pos].danados)
        {
            return false;
        }
        productos[pos].danados -= unidades;
        return true;
    }

    // Resta del stock las unidades vendidas. false si el producto no existe,
    // si las unidades no son al menos 1 o si hay menos de las que se piden
    // (en ese caso no cambia nada).
    bool descontar(int codigo, int unidades)
    {
        int pos = buscar(codigo);
        if (pos == -1 || unidades < 1 || unidades > productos[pos].cantidad)
        {
            return false;
        }
        productos[pos].cantidad -= unidades;
        return true;
    }

    // ----- Ordenar (algoritmo de Burbuja) -----

    // Ordena el inventario segun el criterio (ORDEN_POR_NOMBRE o
    // ORDEN_POR_CATEGORIA). En cada pasada se comparan los vecinos y se
    // intercambian si estan al reves, asi el mayor "sube" hasta el final
    // como una burbuja. Si una pasada no intercambia nada, ya esta ordenado.
    void ordenar(int criterio)
    {
        for (int pasada = 0; pasada < numProductos - 1; pasada++)
        {
            bool huboCambio = false;
            for (int i = 0; i < numProductos - 1 - pasada; i++)
            {
                if (debeIrDespues(productos[i], productos[i + 1], criterio))
                {
                    Producto temporal = productos[i];
                    productos[i] = productos[i + 1];
                    productos[i + 1] = temporal;
                    huboCambio = true;
                }
            }
            if (!huboCambio)
            {
                break;
            }
        }
    }

    // ----- Memoria -----

    // Libera el arreglo y deja el inventario como recien creado
    void vaciar()
    {
        delete[] productos;
        productos = nullptr;
        numProductos = 0;
        capacidad = 0;
    }

    // ----- Archivo -----

    // Reemplaza el contenido del inventario por lo que hay en inventario.txt.
    // Si el archivo no existe, el inventario queda vacio.
    // Devuelve cuantas lineas se ignoraron por estar mal formadas o repetidas.
    int cargar()
    {
        vaciar();
        int ignoradas = 0;
        int totalLineas = contarLineasArchivo(ARCHIVO_INVENTARIO);
        if (totalLineas == 0)
        {
            return 0;
        }
        string *lineas = new string[totalLineas];
        int leidas = leerLineasArchivo(ARCHIVO_INVENTARIO, lineas, totalLineas);
        for (int i = 0; i < leidas; i++)
        {
            Producto p;
            if (!textoAProducto(lineas[i], p) || !agregar(p))
            {
                ignoradas++;
            }
        }
        delete[] lineas;
        return ignoradas;
    }

    // Escribe todo el inventario en inventario.txt. false si no se pudo.
    bool guardar() const
    {
        string *lineas = new string[numProductos];
        for (int i = 0; i < numProductos; i++)
        {
            lineas[i] = productoATexto(productos[i]);
        }
        bool ok = escribirLineasArchivo(ARCHIVO_INVENTARIO, lineas, numProductos);
        delete[] lineas;
        return ok;
    }
};

#endif
