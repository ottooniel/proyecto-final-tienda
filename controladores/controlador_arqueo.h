#ifndef CONTROLADORES_CONTROLADOR_ARQUEO_H
#define CONTROLADORES_CONTROLADOR_ARQUEO_H

#include <string>
#include "../core/archivos.h"
#include "../core/core.h"
#include "../core/fecha.h"
#include "../modelos/arqueo.h"
#include "../modelos/registro_venta.h"
#include "../vistas/vista_arqueo.h"
#include "../vistas/vista_menu.h"

// ---------- Arqueo de caja ----------

// Pide la fecha del arqueo. Enter vacio = hoy. Devuelve "" si se cierra la entrada.
string pedirFechaArqueo()
{
    string hoy = fechaActual();
    while (!entradaTerminada)
    {
        string texto = leerLinea("Fecha del arqueo AAAA-MM-DD (Enter para hoy, " + hoy + "): ");
        if (entradaTerminada)
        {
            break;
        }
        if (texto.find_first_not_of(" \t\r") == string::npos)
        {
            return hoy;
        }
        // se quitan espacios de los extremos
        size_t inicio = texto.find_first_not_of(" \t\r");
        size_t fin = texto.find_last_not_of(" \t\r");
        texto = texto.substr(inicio, fin - inicio + 1);
        if (fechaValida(texto))
        {
            return texto;
        }
        mostrarFechaInvalida();
    }
    return "";
}

// Calcula el arqueo de una caja (o de todas si caja = 0), lo muestra y lo guarda
void hacerArqueo(const string &cajeroActual, int caja, const Ventas &ventas)
{
    string fecha = pedirFechaArqueo();
    if (entradaTerminada || fecha == "")
    {
        return;
    }
    ResumenArqueo resumen = calcularArqueo(ventas, fecha, caja);
    string *lineas = new string[LINEAS_ARQUEO];
    int n = arqueoATexto(resumen, fecha, caja, cajeroActual, lineas);
    mostrarMensaje("");
    for (int i = 0; i < n; i++)
    {
        mostrarMensaje(lineas[i]);
    }
    string ruta = nombreArchivoArqueo(fecha, caja);
    bool guardado = escribirLineasArchivo(ruta, lineas, n);
    delete[] lineas;

    if (guardado)
    {
        mostrarArqueoGuardado(ruta);
    }
    else
    {
        mostrarErrorArqueo(ruta);
    }
}

// Submenu de arqueo: de la caja del cajero o de toda la tienda
void gestionarArqueo(const string &cajeroActual, int caja, const Ventas &ventas)
{
    int opcion;
    do
    {
        mostrarMenuArqueo(caja);
        opcion = leerEntero("Seleccione una opcion: ", 0, 2);
        mostrarMensaje("");

        switch (opcion)
        {
        case 1:
            hacerArqueo(cajeroActual, caja, ventas);
            break;
        case 2:
            hacerArqueo(cajeroActual, 0, ventas);
            break;
        }
    } while (opcion != 0);
}

#endif
