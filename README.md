# Sistema de Cobros - Todo al Alcance

Proyecto final de Programación II (Universidad Mariano Gálvez de Guatemala).
Sistema de cobros en consola para la tienda de conveniencia "Todo al Alcance", hecho en C++ con arquitectura MVC.

## Funcionalidades

- Inicio de sesión de cajeros (3 intentos), con una caja fija por cajero.
- Gestión de inventario: crear, ver, buscar, modificar, eliminar y ordenar (burbuja) por nombre o categoría.
- Clientes con puntos de lealtad y canje (100 puntos = Q10).
- Filas de clientes por caja (colas) y deshacer el último cobro (pilas).
- Cobro con efectivo, tarjeta de débito o tarjeta de crédito, con factura en archivo de texto.
- Devolución de productos dañados e informe de dañados.
- Arqueo de caja por caja o de todas las cajas.

## Estructura

```
main.cpp
core/            configuración, lectura de datos, archivos, texto y fechas
modelos/         entidades y reglas del negocio
vistas/          lo que se muestra en pantalla
controladores/   flujo de cada funcionalidad
```

## Compilar y ejecutar

```
g++ -std=c++11 main.cpp -o sistema_cobros
./sistema_cobros
```

El programa crea sus archivos de datos (cajeros, inventario, clientes, ventas, facturas y arqueos) en la carpeta desde donde se ejecuta. Esos archivos no se suben al repositorio.
