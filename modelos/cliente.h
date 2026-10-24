#ifndef MODELOS_CLIENTE_H
#define MODELOS_CLIENTE_H

#include <sstream>
#include <string>
#include "../core/archivos.h"
#include "../core/core.h"
#include "../core/texto.h"

struct Cliente
{
    int codigo;     // lo asigna el sistema; el cliente lo conserva en su factura
    string nombre;
    int puntos;     // puntos de lealtad acumulados
};

// Reglas de un cliente:
//   codigo entre 1 y MAX_CODIGO_CLIENTE, nombre valido (puede repetirse
//   entre clientes), puntos entre 0 y MAX_PUNTOS.
bool clienteValido(const Cliente &c)
{
    if (c.codigo < 1 || c.codigo > MAX_CODIGO_CLIENTE)
    {
        return false;
    }
    if (!textoCampoValido(c.nombre, MAX_NOMBRE_CLIENTE))
    {
        return false;
    }
    if (c.puntos < 0 || c.puntos > MAX_PUNTOS)
    {
        return false;
    }
    return true;
}

// Cliente -> una linea de texto:  7|Ana Lopez|23
string clienteATexto(const Cliente &c)
{
    ostringstream flujo;
    flujo << c.codigo << SEPARADOR_CAMPO
          << c.nombre << SEPARADOR_CAMPO
          << c.puntos;
    return flujo.str();
}

// Una linea de texto -> Cliente. false si la linea esta mal formada
// (campos de menos o de mas, numeros invalidos o reglas incumplidas).
bool textoACliente(const string &linea, Cliente &c)
{
    // 3 campos = exactamente 2 separadores
    int separadores = 0;
    for (int i = 0; i < (int)linea.length(); i++)
    {
        if (linea[i] == SEPARADOR_CAMPO)
        {
            separadores++;
        }
    }
    if (separadores != 2)
    {
        return false;
    }

    string campos[3];
    int n = 0;
    istringstream flujo(linea);
    string campo;
    while (n < 3 && getline(flujo, campo, SEPARADOR_CAMPO))
    {
        campos[n] = campo;
        n++;
    }
    if (n != 3)
    {
        return false;
    }

    if (!textoAEntero(campos[0], c.codigo))
    {
        return false;
    }
    c.nombre = campos[1];
    if (!textoAEntero(campos[2], c.puntos))
    {
        return false;
    }
    return clienteValido(c);
}

class Clientes
{
private:
    Cliente *clientes;
    int numClientes;   // cuantos clientes hay
    int capacidad;     // cuantos caben antes de crecer

    // Sin copia: dos copias liberarian la misma memoria
    Clientes(const Clientes &otro);
    Clientes &operator=(const Clientes &otro);

    void crecer()
    {
        int nuevaCapacidad = (capacidad == 0) ? CAPACIDAD_INICIAL : capacidad * 2;
        Cliente *nuevo = new Cliente[nuevaCapacidad];
        for (int i = 0; i < numClientes; i++)
        {
            nuevo[i] = clientes[i];
        }
        delete[] clientes;
        clientes = nuevo;
        capacidad = nuevaCapacidad;
    }

    // El mayor codigo que existe + 1 (1 si no hay clientes)
    int siguienteCodigo() const
    {
        int mayor = 0;
        for (int i = 0; i < numClientes; i++)
        {
            if (clientes[i].codigo > mayor)
            {
                mayor = clientes[i].codigo;
            }
        }
        return mayor + 1;
    }

public:
    Clientes()
    {
        clientes = nullptr;
        numClientes = 0;
        capacidad = 0;
    }

    ~Clientes()
    {
        delete[] clientes;
    }

    // ----- Consultar -----

    int total() const
    {
        return numClientes;
    }

    // Cliente en la posicion dada (de 0 a total()-1). Quien llama
    // debe asegurar que la posicion existe.
    const Cliente &obtener(int posicion) const
    {
        return clientes[posicion];
    }

    // Posicion del cliente con ese codigo, o -1 si no existe
    int buscar(int codigo) const
    {
        for (int i = 0; i < numClientes; i++)
        {
            if (clientes[i].codigo == codigo)
            {
                return i;
            }
        }
        return -1;
    }

    // Busca los clientes cuyo nombre CONTIENE el texto, sin distinguir
    // mayusculas ni espacios al inicio o al final. Guarda en "posiciones"
    // las primeras "maximo" coincidencias y devuelve cuantas hay EN TOTAL
    // (puede ser mas que "maximo"). Un texto en blanco no encuentra nada.
    int buscarPorNombre(const string &texto, int posiciones[], int maximo) const
    {
        size_t inicio = texto.find_first_not_of(" \t\r");
        if (inicio == string::npos)
        {
            return 0;
        }
        size_t fin = texto.find_last_not_of(" \t\r");
        string buscado = aMinusculas(texto.substr(inicio, fin - inicio + 1));

        int encontrados = 0;
        for (int i = 0; i < numClientes; i++)
        {
            if (aMinusculas(clientes[i].nombre).find(buscado) != string::npos)
            {
                if (encontrados < maximo)
                {
                    posiciones[encontrados] = i;
                }
                encontrados++;
            }
        }
        return encontrados;
    }

    // ----- Crear y modificar puntos -----

    // Agrega un cliente que ya trae su codigo (asi se cargan los del archivo).
    // false si no cumple las reglas o el codigo ya existe.
    bool agregar(const Cliente &c)
    {
        if (!clienteValido(c) || buscar(c.codigo) != -1)
        {
            return false;
        }
        if (numClientes == capacidad)
        {
            crecer();
        }
        clientes[numClientes] = c;
        numClientes++;
        return true;
    }

    // Registra un cliente nuevo con 0 puntos; el sistema le asigna el codigo.
    // Devuelve ese codigo, o 0 si el nombre no es valido o ya no quedan codigos.
    int registrar(const string &nombre)
    {
        if (!textoCampoValido(nombre, MAX_NOMBRE_CLIENTE))
        {
            return 0;
        }
        Cliente nuevo;
        nuevo.codigo = siguienteCodigo();
        nuevo.nombre = nombre;
        nuevo.puntos = 0;
        if (!agregar(nuevo))
        {
            return 0;
        }
        return nuevo.codigo;
    }

    // Suma puntos al cliente. false si no existe, si los puntos son negativos
    // o si se pasaria de MAX_PUNTOS (en ese caso no cambia nada).
    bool sumarPuntos(int codigo, int puntos)
    {
        int pos = buscar(codigo);
        if (pos == -1 || puntos < 0 || puntos > MAX_PUNTOS - clientes[pos].puntos)
        {
            return false;
        }
        clientes[pos].puntos += puntos;
        return true;
    }

    // Resta puntos al cliente (canje o deshacer un cobro). false si no
    // existe, si los puntos son negativos o si tiene menos de los que se
    // piden (en ese caso no cambia nada).
    bool restarPuntos(int codigo, int puntos)
    {
        int pos = buscar(codigo);
        if (pos == -1 || puntos < 0 || puntos > clientes[pos].puntos)
        {
            return false;
        }
        clientes[pos].puntos -= puntos;
        return true;
    }

    // ----- Memoria -----

    // Libera el arreglo y deja la coleccion como recien creada
    void vaciar()
    {
        delete[] clientes;
        clientes = nullptr;
        numClientes = 0;
        capacidad = 0;
    }

    // ----- Archivo -----

    // Reemplaza el contenido por lo que hay en clientes.txt.
    // Si el archivo no existe, queda vacia.
    // Devuelve cuantas lineas se ignoraron por estar mal formadas o repetidas.
    int cargar()
    {
        vaciar();
        int ignoradas = 0;
        int totalLineas = contarLineasArchivo(ARCHIVO_CLIENTES);
        if (totalLineas == 0)
        {
            return 0;
        }
        string *lineas = new string[totalLineas];
        int leidas = leerLineasArchivo(ARCHIVO_CLIENTES, lineas, totalLineas);
        for (int i = 0; i < leidas; i++)
        {
            Cliente c;
            if (!textoACliente(lineas[i], c) || !agregar(c))
            {
                ignoradas++;
            }
        }
        delete[] lineas;
        return ignoradas;
    }

    // Escribe todos los clientes en clientes.txt. false si no se pudo.
    bool guardar() const
    {
        string *lineas = new string[numClientes];
        for (int i = 0; i < numClientes; i++)
        {
            lineas[i] = clienteATexto(clientes[i]);
        }
        bool ok = escribirLineasArchivo(ARCHIVO_CLIENTES, lineas, numClientes);
        delete[] lineas;
        return ok;
    }
};

#endif
