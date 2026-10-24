#ifndef CORE_H
#define CORE_H

#include <iostream>
#include <sstream>
#include <string>
using namespace std;

// ---------- Configuracion ----------
// Cajeros (cajeros.txt: una linea por cajero)
//   usuario clave caja
// La tienda tiene 3 cajas y cada cajero tiene una caja fija (1, 2 o 3).
const string ARCHIVO_CAJEROS = "cajeros.txt";
const int MAX_CAMPO = 20;    
const int MAX_INTENTOS = 3;  
const int NUM_CAJAS = 3;     
const int MAX_CLIENTES_FILA = 50;

// Inventario (inventario.txt: una linea por producto)
//   codigo|nombre|categoria|precio|cantidad|danados
const string ARCHIVO_INVENTARIO = "inventario.txt";
const char SEPARADOR_CAMPO = '|';   
const int MAX_NOMBRE_PRODUCTO = 25;     
const int MAX_CATEGORIA_PRODUCTO = 15; 
const int CAPACIDAD_INICIAL = 5;        

// Limites que se piden al escribir un producto (evitan numeros absurdos
// y mantienen la tabla del inventario dentro de 80 columnas)
const int MAX_CODIGO = 999999;
const int MAX_CANTIDAD = 100000;
const double MAX_PRECIO = 100000;

// Clientes (clientes.txt: una linea por cliente)
//   codigo|nombre|puntos
// El codigo lo asigna el sistema (1, 2, 3...) y el cliente lo conserva
// en su factura. Los puntos son de lealtad.
const string ARCHIVO_CLIENTES = "clientes.txt";
const int MAX_NOMBRE_CLIENTE = 25;      
const int MAX_CODIGO_CLIENTE = 999999;   
const int MAX_PUNTOS = 100000000;        
const int MAX_RESULTADOS_BUSQUEDA = 10; 

// Cobro, puntos de lealtad y facturas.
// El dinero se maneja en CENTAVOS (numeros enteros) para no tener errores
// de redondeo con decimales: Q35.00 = 3500 centavos.
const int PAGO_EFECTIVO = 1;
const int PAGO_DEBITO = 2;
const int PAGO_CREDITO = 3;
const int MAX_LINEAS_CARRITO = 40;                
const long long CENTAVOS_POR_PUNTO = 1000;       
const int PUNTOS_POR_CANJE = 100;                
const long long DESCUENTO_POR_CANJE = 1000;       
const double MAX_EFECTIVO = 1000000;              
const int MAX_NUMERO_FACTURA = 999999;
const string PREFIJO_FACTURA = "factura_";      
const string EXTENSION_FACTURA = ".txt";

// Registro de ventas (ventas.txt: una linea por factura emitida)
//   numero|fecha|hora|caja|metodo|total|estado
// Sirve para listar las facturas y, mas adelante, para el arqueo de caja.
const string ARCHIVO_VENTAS = "ventas.txt";
const int MAX_FACTURAS_LISTADAS = 10;             

// Devoluciones de productos danados (devoluciones.txt: una linea por devolucion)
//   numero|fecha|hora|caja|factura|codigo|nombre|cantidad
// Las unidades devueltas quedan marcadas como "danadas" en el inventario (no
// se pueden vender) y el informe de danados se guarda en un archivo de texto.
const string ARCHIVO_DEVOLUCIONES = "devoluciones.txt";
const string ARCHIVO_INFORME_DANADOS = "informe_danados.txt";
const int MAX_NUMERO_DEVOLUCION = 999999;

// Arqueo de caja: el reporte se guarda en arqueo_AAAA-MM-DD_cajaN.txt
// (o arqueo_AAAA-MM-DD_todas.txt cuando incluye todas las cajas)
const string PREFIJO_ARQUEO = "arqueo_";

// Criterios para ordenar el inventario
const int ORDEN_POR_NOMBRE = 1;
const int ORDEN_POR_CATEGORIA = 2;

// ---------- Estado de la entrada ----------
// Se activa si la entrada se cierra (fin de archivo)
bool entradaTerminada = false;

// ---------- Entrada de datos ----------

// Lee una linea completa de texto
string leerLinea(const string &mensaje)
{
    string linea;
    cout << mensaje;
    if (!getline(cin, linea))
    {
        entradaTerminada = true;
        linea = "";
    }
    return linea;
}

// Lee un entero entre minimo y maximo. Repite hasta que el dato sea valido.
int leerEntero(const string &mensaje, int minimo, int maximo)
{
    while (!entradaTerminada)
    {
        string linea = leerLinea(mensaje);
        if (entradaTerminada)
        {
            break;
        }
        istringstream flujo(linea);
        int valor;
        char sobrante;
        // debe leerse un entero y no debe quedar nada mas escrito
        if ((flujo >> valor) && !(flujo >> sobrante))
        {
            if (valor >= minimo && valor <= maximo)
            {
                return valor;
            }
            cout << "Error: el valor debe estar entre " << minimo << " y " << maximo << "." << endl;
        }
        else
        {
            cout << "Error: ingrese un numero entero valido." << endl;
        }
    }
    return minimo;
}

// ---------- Conversion de texto a numeros ----------
// Devuelven false si el texto no es un numero o trae algo de mas
// ("12abc", "", "3.5.1"). No muestran nada en pantalla.

bool textoAEntero(const string &texto, int &valor)
{
    istringstream flujo(texto);
    char sobrante;
    return (flujo >> valor) && !(flujo >> sobrante);
}

bool textoADecimal(const string &texto, double &valor)
{
    istringstream flujo(texto);
    char sobrante;
    return (flujo >> valor) && !(flujo >> sobrante);
}

// ---------- Dinero en centavos ----------

// Quetzales con decimales -> centavos enteros (redondea al centavo mas cercano).
// Solo para montos que no son negativos.
long long aCentavos(double quetzales)
{
    return (long long)(quetzales * 100.0 + 0.5);
}

// Centavos -> texto con 2 decimales: 3500 -> "35.00". Solo para montos que
// no son negativos.
string centavosATexto(long long centavos)
{
    ostringstream flujo;
    flujo << (centavos / 100) << ".";
    long long resto = centavos % 100;
    if (resto < 10)
    {
        flujo << "0";
    }
    flujo << resto;
    return flujo.str();
}

// Lee un decimal entre minimo y maximo (por ejemplo un precio).
// Repite hasta que el dato sea valido.
double leerDecimal(const string &mensaje, double minimo, double maximo)
{
    while (!entradaTerminada)
    {
        string linea = leerLinea(mensaje);
        if (entradaTerminada)
        {
            break;
        }
        double valor;
        if (textoADecimal(linea, valor))
        {
            if (valor >= minimo && valor <= maximo)
            {
                return valor;
            }
            cout << "Error: el valor debe estar entre " << minimo << " y " << maximo << "." << endl;
        }
        else
        {
            cout << "Error: ingrese un numero valido (ej. 12.50)." << endl;
        }
    }
    return minimo;
}

#endif
