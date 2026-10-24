#ifndef MODELOS_REGISTRO_VENTA_H
#define MODELOS_REGISTRO_VENTA_H

#include <sstream>
#include <string>
#include "../core/archivos.h"
#include "../core/core.h"
#include "../core/fecha.h"
#include "venta.h"

struct RegistroVenta
{
    int numero;
    string fecha;       // AAAA-MM-DD
    string hora;        // HH:MM
    int caja;
    int metodo;
    long long total;    // centavos
    bool anulada;       // false = VIGENTE
};

bool registroVentaValido(const RegistroVenta &r)
{
    if (r.numero < 1 || r.numero > MAX_NUMERO_FACTURA)
    {
        return false;
    }
    if (!fechaValida(r.fecha) || !horaValida(r.hora))
    {
        return false;
    }
    if (r.caja < 1 || r.caja > NUM_CAJAS)
    {
        return false;
    }
    if (r.metodo != PAGO_EFECTIVO && r.metodo != PAGO_DEBITO && r.metodo != PAGO_CREDITO)
    {
        return false;
    }
    return r.total >= 1;
}

// Los datos de una venta -> la linea que se guarda en ventas.txt
RegistroVenta registroDeVenta(const Venta &v)
{
    RegistroVenta r;
    r.numero = v.numero;
    r.fecha = v.fecha;
    r.hora = v.hora;
    r.caja = v.caja;
    r.metodo = v.metodoPago;
    r.total = v.total;
    r.anulada = false;
    return r;
}

string registroVentaATexto(const RegistroVenta &r)
{
    ostringstream flujo;
    flujo << r.numero << SEPARADOR_CAMPO
          << r.fecha << SEPARADOR_CAMPO
          << r.hora << SEPARADOR_CAMPO
          << r.caja << SEPARADOR_CAMPO
          << r.metodo << SEPARADOR_CAMPO
          << centavosATexto(r.total) << SEPARADOR_CAMPO
          << (r.anulada ? "ANULADA" : "VIGENTE");
    return flujo.str();
}

// Una linea de ventas.txt -> RegistroVenta. false si esta mal formada.
bool textoARegistroVenta(const string &linea, RegistroVenta &r)
{
    // 7 campos = exactamente 6 separadores
    int separadores = 0;
    for (int i = 0; i < (int)linea.length(); i++)
    {
        if (linea[i] == SEPARADOR_CAMPO)
        {
            separadores++;
        }
    }
    if (separadores != 6)
    {
        return false;
    }

    string campos[7];
    int n = 0;
    istringstream flujo(linea);
    string campo;
    while (n < 7 && getline(flujo, campo, SEPARADOR_CAMPO))
    {
        campos[n] = campo;
        n++;
    }
    if (n != 7)
    {
        return false;
    }

    double totalEnQuetzales;
    if (!textoAEntero(campos[0], r.numero) || !textoAEntero(campos[3], r.caja) ||
        !textoAEntero(campos[4], r.metodo) || !textoADecimal(campos[5], totalEnQuetzales))
    {
        return false;
    }
    if (totalEnQuetzales < 0)
    {
        return false;
    }
    r.fecha = campos[1];
    r.hora = campos[2];
    r.total = aCentavos(totalEnQuetzales);
    if (campos[6] == "VIGENTE")
    {
        r.anulada = false;
    }
    else if (campos[6] == "ANULADA")
    {
        r.anulada = true;
    }
    else
    {
        return false;
    }
    return registroVentaValido(r);
}

// Arreglo dinamico con el registro de todas las facturas emitidas
class Ventas
{
private:
    RegistroVenta *registros;
    int numRegistros;
    int capacidad;

    Ventas(const Ventas &otro);
    Ventas &operator=(const Ventas &otro);

    void crecer()
    {
        int nuevaCapacidad = (capacidad == 0) ? CAPACIDAD_INICIAL : capacidad * 2;
        RegistroVenta *nuevo = new RegistroVenta[nuevaCapacidad];
        for (int i = 0; i < numRegistros; i++)
        {
            nuevo[i] = registros[i];
        }
        delete[] registros;
        registros = nuevo;
        capacidad = nuevaCapacidad;
    }

public:
    Ventas()
    {
        registros = nullptr;
        numRegistros = 0;
        capacidad = 0;
    }

    ~Ventas()
    {
        delete[] registros;
    }

    int total() const
    {
        return numRegistros;
    }

    const RegistroVenta &obtener(int posicion) const
    {
        return registros[posicion];
    }

    // Posicion de la factura con ese numero, o -1 si no esta registrada
    int buscar(int numero) const
    {
        for (int i = 0; i < numRegistros; i++)
        {
            if (registros[i].numero == numero)
            {
                return i;
            }
        }
        return -1;
    }

    // Numero de la proxima factura: el mayor + 1 (1 si no hay ninguna).
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
        return (mayor >= MAX_NUMERO_FACTURA) ? 0 : mayor + 1;
    }

    // false si el registro no es valido o ese numero ya esta registrado
    bool agregar(const RegistroVenta &r)
    {
        if (!registroVentaValido(r) || buscar(r.numero) != -1)
        {
            return false;
        }
        if (numRegistros == capacidad)
        {
            crecer();
        }
        registros[numRegistros] = r;
        numRegistros++;
        return true;
    }

    // Marca la factura como ANULADA (al deshacer su cobro). false si no esta
    // registrada o ya estaba anulada.
    bool anular(int numero)
    {
        int pos = buscar(numero);
        if (pos == -1 || registros[pos].anulada)
        {
            return false;
        }
        registros[pos].anulada = true;
        return true;
    }

    void vaciar()
    {
        delete[] registros;
        registros = nullptr;
        numRegistros = 0;
        capacidad = 0;
    }

    // Reemplaza el contenido por lo que hay en ventas.txt. Devuelve cuantas
    // lineas se ignoraron por estar mal formadas o repetidas.
    int cargar()
    {
        vaciar();
        int ignoradas = 0;
        int totalLineas = contarLineasArchivo(ARCHIVO_VENTAS);
        if (totalLineas == 0)
        {
            return 0;
        }
        string *lineas = new string[totalLineas];
        int leidas = leerLineasArchivo(ARCHIVO_VENTAS, lineas, totalLineas);
        for (int i = 0; i < leidas; i++)
        {
            RegistroVenta r;
            if (!textoARegistroVenta(lineas[i], r) || !agregar(r))
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
            lineas[i] = registroVentaATexto(registros[i]);
        }
        bool ok = escribirLineasArchivo(ARCHIVO_VENTAS, lineas, numRegistros);
        delete[] lineas;
        return ok;
    }
};

// Numero para la proxima factura: el siguiente del registro que no tenga ya
// un archivo (por si ventas.txt se borro pero las facturas siguen ahi).
// Devuelve 0 si no queda ninguno.
int numeroParaFactura(const Ventas &ventas)
{
    int numero = ventas.siguienteNumero();
    while (numero != 0 && numero <= MAX_NUMERO_FACTURA && existeArchivo(nombreArchivoFactura(numero)))
    {
        numero++;
    }
    return (numero > MAX_NUMERO_FACTURA) ? 0 : numero;
}

#endif
