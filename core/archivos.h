#ifndef ARCHIVOS_H
#define ARCHIVOS_H

#include <fstream>
#include <sstream>
#include <string>
#include "core.h"

// ---------- Archivos de texto genericos (una linea = un registro) ----------

// Quita el \r del final (archivos editados en Windows) y dice si la
// linea tiene contenido. Las lineas en blanco se ignoran.
bool lineaUtil(string &linea)
{
    if (!linea.empty() && linea[linea.length() - 1] == '\r')
    {
        linea.erase(linea.length() - 1);
    }
    return linea.find_first_not_of(" \t") != string::npos;
}

// Cuenta las lineas con contenido. Devuelve 0 si el archivo no existe.
int contarLineasArchivo(const string &ruta)
{
    ifstream archivo(ruta.c_str());
    if (!archivo.is_open())
    {
        return 0;
    }
    int total = 0;
    string linea;
    while (getline(archivo, linea))
    {
        if (lineaUtil(linea))
        {
            total++;
        }
    }
    archivo.close();
    return total;
}

// Llena el arreglo "lineas" (que ya reservo quien llama) con las lineas
// con contenido, hasta "maximo". Devuelve cuantas leyo.
int leerLineasArchivo(const string &ruta, string lineas[], int maximo)
{
    ifstream archivo(ruta.c_str());
    if (!archivo.is_open())
    {
        return 0;
    }
    int leidas = 0;
    string linea;
    while (leidas < maximo && getline(archivo, linea))
    {
        if (lineaUtil(linea))
        {
            lineas[leidas] = linea;
            leidas++;
        }
    }
    archivo.close();
    return leidas;
}

// Reemplaza TODO el contenido del archivo por las lineas dadas
// (sin ios::app, el archivo se vuelve a crear desde cero).
// Devuelve false si no se pudo abrir o escribir.
bool escribirLineasArchivo(const string &ruta, const string lineas[], int cantidad)
{
    ofstream archivo(ruta.c_str());
    if (!archivo.is_open())
    {
        return false;
    }
    for (int i = 0; i < cantidad; i++)
    {
        archivo << lineas[i] << endl;
    }
    archivo.close();
    return !archivo.fail();
}

// true si el archivo existe y se puede abrir
bool existeArchivo(const string &ruta)
{
    ifstream archivo(ruta.c_str());
    bool existe = archivo.is_open();
    archivo.close();
    return existe;
}

// ---------- Cajeros ----------

// Separa una linea de cajeros.txt en usuario, clave y caja.
// false si la linea no trae al menos usuario y clave.
// La caja queda en 0 (sin asignar) si falta o no es un numero de 1 a NUM_CAJAS.
bool separarLineaCajero(const string &linea, string &usuario, string &clave, int &caja)
{
    istringstream flujo(linea);
    if (!(flujo >> usuario >> clave))
    {
        return false;
    }
    caja = 0;
    string tercero;
    int valor;
    if ((flujo >> tercero) && textoAEntero(tercero, valor) && valor >= 1 && valor <= NUM_CAJAS)
    {
        caja = valor;
    }
    return true;
}

// true si el archivo existe y tiene al menos un cajero
bool hayCajerosEnArchivo()
{
    ifstream archivo(ARCHIVO_CAJEROS.c_str());
    if (!archivo.is_open())
    {
        return false;
    }
    string linea, u, c;
    int caja;
    while (getline(archivo, linea))
    {
        if (lineaUtil(linea) && separarLineaCajero(linea, u, c, caja))
        {
            archivo.close();
            return true;
        }
    }
    archivo.close();
    return false;
}

// Cuantos cajeros hay registrados
int contarCajerosEnArchivo()
{
    ifstream archivo(ARCHIVO_CAJEROS.c_str());
    if (!archivo.is_open())
    {
        return 0;
    }
    int total = 0;
    string linea, u, c;
    int caja;
    while (getline(archivo, linea))
    {
        if (lineaUtil(linea) && separarLineaCajero(linea, u, c, caja))
        {
            total++;
        }
    }
    archivo.close();
    return total;
}

// true si alguna linea tiene ese usuario
bool existeUsuarioEnArchivo(const string &usuario)
{
    ifstream archivo(ARCHIVO_CAJEROS.c_str());
    if (!archivo.is_open())
    {
        return false;
    }
    string linea, u, c;
    int caja;
    while (getline(archivo, linea))
    {
        if (lineaUtil(linea) && separarLineaCajero(linea, u, c, caja) && u == usuario)
        {
            archivo.close();
            return true;
        }
    }
    archivo.close();
    return false;
}

// true si alguna linea tiene ese usuario Y esa contrasena. En ese caso
// deja en "caja" la caja del cajero (0 si es un cajero antiguo sin caja).
bool buscarCredencialEnArchivo(const string &usuario, const string &clave, int &caja)
{
    ifstream archivo(ARCHIVO_CAJEROS.c_str());
    if (!archivo.is_open())
    {
        return false;
    }
    string linea, u, c;
    int cajaLeida;
    while (getline(archivo, linea))
    {
        if (lineaUtil(linea) && separarLineaCajero(linea, u, c, cajaLeida) &&
            u == usuario && c == clave)
        {
            caja = cajaLeida;
            archivo.close();
            return true;
        }
    }
    archivo.close();
    return false;
}

// true si algun cajero ya tiene esa caja
bool cajaOcupadaEnArchivo(int caja)
{
    ifstream archivo(ARCHIVO_CAJEROS.c_str());
    if (!archivo.is_open())
    {
        return false;
    }
    string linea, u, c;
    int cajaLeida;
    while (getline(archivo, linea))
    {
        if (lineaUtil(linea) && separarLineaCajero(linea, u, c, cajaLeida) && cajaLeida == caja)
        {
            archivo.close();
            return true;
        }
    }
    archivo.close();
    return false;
}

// Agrega una linea al final del archivo (ios::app no borra lo anterior).
// Si la ultima linea no termina en salto (por ejemplo, archivo editado en
// el Bloc de notas), primero lo agrega para no pegar dos cajeros.
// Devuelve false si no se pudo abrir el archivo para escribir.
bool agregarCajeroAlArchivo(const string &usuario, const string &clave, int caja)
{
    bool faltaSalto = false;
    {
        ifstream lectura(ARCHIVO_CAJEROS.c_str(), ios::binary);
        if (lectura.is_open())
        {
            lectura.seekg(0, ios::end);
            if (lectura.tellg() > 0)
            {
                lectura.seekg(-1, ios::end);
                char ultimo;
                if (lectura.get(ultimo) && ultimo != '\n')
                {
                    faltaSalto = true;
                }
            }
            lectura.close();
        }
    }

    ofstream archivo(ARCHIVO_CAJEROS.c_str(), ios::app);
    if (!archivo.is_open())
    {
        return false;
    }
    if (faltaSalto)
    {
        archivo << endl;
    }
    archivo << usuario << " " << clave << " " << caja << endl;
    archivo.close();
    return !archivo.fail();
}

// Le pone caja al cajero con ese usuario (para los registrados antes de
// que existieran las cajas). Reescribe el archivo. false si no existe el
// usuario o no se pudo escribir.
bool asignarCajaEnArchivo(const string &usuario, int caja)
{
    int total = contarLineasArchivo(ARCHIVO_CAJEROS);
    if (total == 0)
    {
        return false;
    }
    string *lineas = new string[total];
    int leidas = leerLineasArchivo(ARCHIVO_CAJEROS, lineas, total);

    bool encontrado = false;
    for (int i = 0; i < leidas && !encontrado; i++)
    {
        string u, c;
        int cajaActual;
        if (separarLineaCajero(lineas[i], u, c, cajaActual) && u == usuario)
        {
            ostringstream nueva;
            nueva << u << " " << c << " " << caja;
            lineas[i] = nueva.str();
            encontrado = true;
        }
    }

    bool ok = encontrado && escribirLineasArchivo(ARCHIVO_CAJEROS, lineas, leidas);
    delete[] lineas;
    return ok;
}

#endif
