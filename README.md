# Sistema de Gestión y Reserva de Espacios para Eventos

Proyecto individual de Algoritmos, C++17. Administra clientes, espacios, reservas, aforo, amenidades, pagos, multas, cancelaciones, búsquedas, ordenamiento manual, reportes y persistencia en archivos de texto.

## Requisitos
- Compilador compatible con C++17.
- CMake 3.16 o superior (opcional si compila directamente con g++).

## Compilar con CMake
Desde esta carpeta:
```bash
cmake -S . -B build
cmake --build build
```
Ejecutar el binario generado dentro de `build` (en Windows suele ser `build\Debug\GestionEventos.exe`; en Linux/macOS `./build/GestionEventos`).

También se puede compilar directamente:
```bash
g++ -std=c++17 main.cpp -o GestionEventos
```

**Importante:** ejecutar el programa desde la carpeta raíz `GestionEventos`, porque los archivos persistentes se guardan en `datos/`.

## Funciones principales
1. Registro y consulta de clientes.
2. Reserva por fecha y espacio, validando horario, conflictos y aforo.
3. Cálculo de tarifa por hora y amenidades.
4. Anticipo mínimo del 20%, pagos parciales y multas configuradas en Q200 por día (se registran manualmente los días de atraso).
5. Inicio bloqueado mientras exista saldo pendiente; cancelación y finalización.
6. Búsqueda secuencial por código de reserva y por nombre exacto del cliente.
7. Ordenamiento burbuja manual por fecha.
8. Reportes: cierre diario, ingresos por espacio y saldos pendientes.
9. Persistencia en `datos/clientes.txt`, `datos/reservas.txt`, `datos/pagos.txt` y `datos/ids.txt` usando `fstream`.

## Datos de ejemplo
Los espacios y tarifas son valores demostrativos configurados en `main.cpp`. El audio del ingeniero especifica la lógica de reserva, cobro por horas, amenidades, pagos y cierre diario, pero no fija estos precios ni el porcentaje de anticipo o multa.

## Archivos
- `main.cpp`: código fuente.
- `CMakeLists.txt`: configuración CMake.
- `datos/`: archivos persistentes (se crean/actualizan al guardar).
- `reportes/`: carpeta reservada para evidencias o exportaciones futuras.
