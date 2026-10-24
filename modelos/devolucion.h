#ifndef MODELOS_DEVOLUCION_H
#define MODELOS_DEVOLUCION_H

#include <sstream>
#include <string>
#include "../core/archivos.h"
#include "../core/core.h"
#include "../core/fecha.h"
#include "../core/texto.h"
#include "producto.h"

struct Devolucion
{
    int numero;
    string fecha;           // AAAA-MM-DD
    string hora;            // HH:MM
    int caja;
    int factura;            // 0 = sin factura
    int codigoProducto;
    string nombreProducto;
    int cantidad;           // unidades danadas (y unidades que se entregan a cambio)
    int codigoReemplazo;    // producto que se entrega a cambio (0 = registro antiguo, sin reemplazo)
    string nombreReemplazo;
};

// Resultado de comprobar si un producto sirve de reemplazo
const int REEMPLAZO_OK = 0;
const int REEMPLAZO_SIN_STOCK = 1;
const int REEMPLAZO_MAS_CARO = 2;

// Comprueba si "reemplazo" puede entregarse a cambio de "unidades" del
// producto danado: debe haber stock suficiente y no costar mas que el danado
// (el cambio es sin costo para el cliente y sin reembolso de diferencias).
int validarReemplazo(const Producto &danado, const Producto &reemplazo, int unidades)
{
    if (reemplazo.cantidad < unidades)
    {
        return REEMPLAZO_SIN_STOCK;
    }
    if (aCentavos(reemplazo.precio) > aCentavos(danado.precio))
    {
        return REEMPLAZO_MAS_CARO;
    }
    return REEMPLAZO_OK;
}

bool devolucionValida(const Devolucion &d)
{
    if (d.numero < 1 || d.numero > MAX_NUMERO_DEVOLUCION)
    {
        return false;
    }
    if (!fechaValida(d.fecha) || !horaValida(d.hora))
    {
        return false;
    }
    if (d.caja < 1 || d.caja > NUM_CAJAS)
    {
        return false;
    }
    if (d.factura < 0 || d.factura > MAX_NUMERO_FACTURA)
    {
        return false;
    }
    if (d.codigoProducto < 1 || !textoCampoValido(d.nombreProducto, MAX_NOMBRE_PRODUCTO))
    {
        return false;
    }
    if (d.cantidad < 1 || d.cantidad > MAX_CANTIDAD)
    {
        return false;
    }
    if (d.codigoReemplazo == 0)
    {
        return d.nombreReemplazo.empty();
    }
    return d.codigoReemplazo >= 1 && textoCampoValido(d.nombreReemplazo, MAX_NOMBRE_PRODUCTO);
}

string devolucionATexto(const Devolucion &d)
{
    ostringstream flujo;
    flujo << d.numero << SEPARADOR_CAMPO
          << d.fecha << SEPARADOR_CAMPO
          << d.hora << SEPARADOR_CAMPO
          << d.caja << SEPARADOR_CAMPO
          << d.factura << SEPARADOR_CAMPO
          << d.codigoProducto << SEPARADOR_CAMPO
          << d.nombreProducto << SEPARADOR_CAMPO
          << d.cantidad << SEPARADOR_CAMPO
          << d.codigoReemplazo << SEPARADOR_CAMPO
          << d.nombreReemplazo;
    return flujo.str();
}

// Separa una linea por SEPARADOR_CAMPO. Devuelve cuantos campos tiene (los
// vacios cuentan, tambien el ultimo) o -1 si son mas de "maximo".
int separarCampos(const string &linea, string campos[], int maximo)
{
    int n = 0;
    string actual;
    for (int i = 0; i < (int)linea.length(); i++)
    {
        if (linea[i] == SEPARADOR_CAMPO)
        {
            if (n >= maximo)
            {
                return -1;
            }
            campos[n++] = actual;
            actual = "";
        }
        else
        {
            actual += linea[i];
        }
    }
    if (n >= maximo)
    {
        return -1;
    }
    campos[n++] = actual;
    return n;
}

// Una linea de devoluciones.txt -> Devolucion. false si esta mal formada.
// Acepta 10 campos (con reemplazo) y 8 campos (registro antiguo, sin reemplazo).
bool textoADevolucion(const string &linea, Devolucion &d)
{
    string campos[10];
    int n = separarCampos(linea, campos, 10);
    if (n != 10 && n != 8)
    {
        return false;
    }

    if (!textoAEntero(campos[0], d.numero) || !textoAEntero(campos[3], d.caja) ||
        !textoAEntero(campos[4], d.factura) || !textoAEntero(campos[5], d.codigoProducto) ||
        !textoAEntero(campos[7], d.cantidad))
    {
        return false;
    }
    d.fecha = campos[1];
    d.hora = campos[2];
    d.nombreProducto = campos[6];
    d.codigoReemplazo = 0;
    d.nombreReemplazo = "";
    if (n == 10)
    {
        if (!textoAEntero(campos[8], d.codigoReemplazo))
        {
            return false;
        }
        d.nombreReemplazo = campos[9];
    }
    return devolucionValida(d);
}

// Arreglo dinamico con el historial de devoluciones
class Devoluciones
{
private:
    Devolucion *registros;
    int numRegistros;
    int capacidad;

    Devoluciones(const Devoluciones &otra);
    Devoluciones &operator=(const Devoluciones &otra);

    void crecer()
    {
        int nuevaCapacidad = (capacidad == 0) ? CAPACIDAD_INICIAL : capacidad * 2;
        Devolucion *nuevo = new Devolucion[nuevaCapacidad];
        for (int i = 0; i < numRegistros; i++)
        {
            nuevo[i] = registros[i];
        }
        delete[] registros;
        registros = nuevo;
        capacidad = nuevaCapacidad;
    }

public:
    Devoluciones()
    {
        registros = nullptr;
        numRegistros = 0;
        capacidad = 0;
    }

    ~Devoluciones()
    {
        delete[] registros;
    }

    int total() const
    {
        return numRegistros;
    }

    const Devolucion &obtener(int posicion) const
    {
        return registros[posicion];
    }

    // Numero de la proxima devolucion: el mayor + 1 (1 si no hay ninguna).
    // Devuelve 0 si ya se usaron todos los numeros.
    int siguienteNumero() const
    {
        int mayor = 0;
        for (int i = 0; i < numRegistros; i++)
        {
            if (registros[i].numero > mayor)
            {
                mayor = registros[i].numero;
            }
        }
        return (mayor >= MAX_NUMERO_DEVOLUCION) ? 0 : mayor + 1;
    }

    // Suma de las unidades de todas las devoluciones
    long long unidadesDevueltas() const
    {
        long long suma = 0;
        for (int i = 0; i < numRegistros; i++)
        {
            suma += registros[i].cantidad;
        }
        return suma;
    }

    // false si la devolucion no es valida o ese numero ya esta registrado
    bool agregar(const Devolucion &d)
    {
        if (!devolucionValida(d))
        {
            return false;
        }
        for (int i = 0; i < numRegistros; i++)
        {
            if (registros[i].numero == d.numero)
            {
                return false;
            }
        }
        if (numRegistros == capacidad)
        {
            crecer();
        }
        registros[numRegistros] = d;
        numRegistros++;
        return true;
    }

    void vaciar()
    {
        delete[] registros;
        registros = nullptr;
        numRegistros = 0;
        capacidad = 0;
    }

    // Reemplaza el contenido por lo que hay en devoluciones.txt. Devuelve
    // cuantas lineas se ignoraron por estar mal formadas o repetidas.
    int cargar()
    {
        vaciar();
        int ignoradas = 0;
        int totalLineas = contarLineasArchivo(ARCHIVO_DEVOLUCIONES);
        if (totalLineas == 0)
        {
            return 0;
        }
        string *lineas = new string[totalLineas];
        int leidas = leerLineasArchivo(ARCHIVO_DEVOLUCIONES, lineas, totalLineas);
        for (int i = 0; i < leidas; i++)
        {
            Devolucion d;
            if (!textoADevolucion(lineas[i], d) || !agregar(d))
            {
                ignoradas++;
            }
        }
        delete[] lineas;
        return ignoradas;
    }

    bool guardar() const
    {
        string *lineas = new string[numRegistros];
        for (int i = 0; i < numRegistros; i++)
        {
            lineas[i] = devolucionATexto(registros[i]);
        }
        bool ok = escribirLineasArchivo(ARCHIVO_DEVOLUCIONES, lineas, numRegistros);
        delete[] lineas;
        return ok;
    }
};

#endif
