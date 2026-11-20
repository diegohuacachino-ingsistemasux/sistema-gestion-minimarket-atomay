# Sistema de Gestión Comercial e Inventario — Minimarket ATOMAY

## Datos Académicos del Proyecto
* **Universidad:** Universidad de Huánuco (UDH)[cite: 65]
* **Facultad / Programa:** Facultad de Ingeniería — Ingeniería de Sistemas e Informática[cite: 65]
* **Asignatura:** Estructura de Datos (IV Ciclo)[cite: 65]
* **Docente:** Ing. Yliana Vilca Ferrer[cite: 60, 65]
* **Autor:** Diego Antonio Huacachino Escobal[cite: 65]
* **Año:** 2026[cite: 65]

---

## Descripción del Proyecto
El presente sistema de consola desarrollado en **C++** automatiza las operaciones diarias de mostrador y almacén del **Minimarket ATOMAY** (distrito de Tomayquichua, Huánuco)[cite: 49]. Su objetivo es erradicar el cuello de botella operativo durante las horas pico escolares[cite: 49], reemplazando el registro manual en cuadernos físicos por una solución estructurada que agiliza el cobro[cite: 49], controla el stock en tiempo real[cite: 54] y sistematiza las cuentas por cobrar ("fiados")[cite: 54].

## Estructuras de Datos Implementadas
* **Arreglos Estáticos (`struct Producto` y `struct Cliente`):** Almacenamiento contiguo en memoria principal (`CAPACIDAD = 100`) para acceso directo por índice y búsqueda lineal rápida de artículos y clientes frecuentes[cite: 54, 67].
* **Cola Secuencial FIFO (`struct TurnoPedido`):** Implementación real con operaciones `Enqueue` (inserción al final mediante `finalCola`) y `Dequeue` (atención y boleteo estricto por `frente`), garantizando el orden cronológico de llegada en caja[cite: 54, 61].

## Funcionalidades Principales
1. **Catálogo e Inventario:** Registro con validación de no duplicidad y visualización tabular (incluye precarga inicial de 10 productos variados con stock crítico $\le 5$).
2. **Gestión de Créditos ("Fiados"):** Alta de clientes frecuentes, consulta de deudas acumuladas, registro de abonos/amortizaciones parciales o totales y reactivación automática de líneas de crédito[cite: 50, 54].
3. **Atención en Hora Pico (Cola FIFO):** Encolado de pedidos en fila (`Enqueue`), visualización de punteros `frente` y `finalCola`, despacho cronológico (`Dequeue`), cálculo exacto de vueltos y emisión de boleta en pantalla[cite: 54, 61].
4. **Reportes y Alertas Operativas:** Detección preventiva de productos próximos a agotarse ($\le 5$ unidades) y balance de cierre de turno por modalidad de pago (`Efectivo`, `Yape` y `Fiado`)[cite: 54].

## Compilación y Ejecución
1. Descargar el archivo fuente `Atomay.cpp` desde este repositorio.
2. Abrirlo en **Dev-C++** (o cualquier compilador estándar de C++ como MinGW/GCC).
3. Compilar y ejecutar el programa (presionar **`F11`** en Dev-C++).
