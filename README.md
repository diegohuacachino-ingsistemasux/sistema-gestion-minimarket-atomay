# Sistema de Gestión Comercial e Inventario — Minimarket ATOMAY

Proyecto final desarrollado para la asignatura de **Estructura de Datos** (Programa Académico de Ingeniería de Sistemas e Informática — Universidad de Huánuco, 2026)[cite: 2, 59].

## Descripción del Proyecto
El sistema automatiza las operaciones diarias de mostrador y almacén del **Minimarket ATOMAY** (Tomayquichua, Huánuco), reemplazando el registro manual en cuadernos físicos por una solución estructurada en consola (**C++**)[cite: 5, 6]. Permite agilizar la atención durante las horas pico escolares, controlar existencias en tiempo real y sistematizar las cuentas por cobrar ("fiados")[cite: 5, 16].

## Estructuras de Datos Implementadas
* **Arreglos Estáticos (`struct Producto` y `struct Cliente`):** Almacenamiento contiguo en memoria RAM (`CAPACIDAD = 100`) para acceso directo por índice y búsqueda lineal de artículos y clientes frecuentes[cite: 20, 21, 35].
* **Cola Secuencial FIFO (`struct TurnoPedido`):** Implementación real con operaciones `Enqueue` (inserción por `finalCola`) y `Dequeue` (atención y boleteo estricto por `frente`), garantizando el orden de llegada en mostrador[cite: 21, 22].

## Funcionalidades Principales
1. **Catálogo e Inventario:** Registro con validación de no duplicidad y visualización tabular (precargado con 10 productos de prueba, incluyendo stock crítico $\le 5$).
2. **Gestión de Créditos ("Fiados"):** Alta de clientes frecuentes, consulta de deudas acumuladas, registro de abonos/amortizaciones y reactivación automática de líneas de crédito.
3. **Atención en Hora Pico (Cola FIFO):** Encolado de pedidos en fila, visualización de punteros `frente` y `finalCola`, despacho cronológico, cálculo exacto de vueltos y emisión de boleta en pantalla.
4. **Reportes y Alertas:** Detección preventiva de productos con stock bajo ($\le 5$ unidades) y balance de cierre de caja por modalidad de pago (`Efectivo`, `Yape` y `Fiado`).

## Compilación y Ejecución
1. Descargar el archivo fuente `Atomay.cpp`.
2. Abrir en **Dev-C++** (o cualquier compilador estándar de C++ como MinGW/GCC).
3. Compilar y ejecutar (`F11` en Dev-C++).

---
**Autor:** Diego Antonio Huacachino Escobal — IV Ciclo[cite: 2, 59]  
**Docente:** Ing. Yliana Vilca Ferrer[cite: 2, 59]
