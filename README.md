# Gestión de Eventos

Proyecto universitario individual de Algoritmos, implementado en C++17. Su objetivo es administrar clientes, espacios y reservas de eventos, controlar horarios y aforo, registrar pagos y multas, gestionar el estado de los eventos y consultar reportes.

## Funcionalidades

- Registrar y listar clientes.
- Consultar los tres espacios predefinidos, sus horarios, capacidad y tarifa por hora.
- Registrar reservas con fecha válida, validación de horario, aforo y disponibilidad para evitar cruces en el mismo espacio.
- Seleccionar amenidades y calcular el total y el anticipo mínimo del 20%.
- Registrar el pago inicial en efectivo y pagos posteriores en efectivo o transferencia.
- Registrar manualmente multas de Q200 por cada día de atraso indicado.
- Impedir el inicio de un evento mientras el saldo, incluidas las multas, sea mayor que cero.
- Cancelar reservas confirmadas que aún estén pendientes de inicio; finalizar eventos iniciados.
- Buscar reservas por código y por nombre exacto de cliente.

## Requisitos técnicos

- Compilador de C++17; en macOS se puede usar `clang++` incluido con Xcode Command Line Tools.
- CMake 3.16 o superior, solo si se desea compilar mediante CMake.
- Ejecutar desde la carpeta raíz del proyecto para que el programa encuentre `datos/`.

## Estructuras y arreglos

El programa define las estructuras `Cliente`, `Espacio`, `Reserva` y `Pago`. Mantiene los registros en arreglos de tamaño fijo:

| Arreglo | Capacidad |
|---|---:|
| Clientes | 200 |
| Reservas | 500 |
| Pagos | 1500 |
| Espacios | 3 |
| Amenidades | 4 |

## Búsquedas y ordenamiento

Las búsquedas se realizan secuencialmente recorriendo los arreglos: cliente por código o nombre, reserva por código y espacio por código. Las reservas se ordenan ascendentemente mediante el método burbuja manual, comparando fechas `DD/MM/AAAA` por año, mes y día. Las fechas inválidas cargadas desde archivos quedan al final.

## Persistencia

Se usan `ifstream` y `ofstream` (de la biblioteca `<fstream>`) para cargar y guardar registros de texto delimitados por `|`:

- `datos/clientes.txt`
- `datos/reservas.txt`
- `datos/pagos.txt`
- `datos/ids.txt`

La carga valida campos y conversiones, informa errores de apertura o lectura y omite registros malformados. El guardado escribe primero archivos temporales y luego reemplaza los datos existentes con respaldo y reversión ante errores detectados.

## Reportes

1. **Cierre diario:** pagos recibidos por fecha de pago, y cantidad de reservas, saldos pendientes y amenidades por fecha del evento.
2. **Ingresos por espacio:** número de reservas no canceladas, total contratado y monto cobrado para cada espacio.
3. **Saldos pendientes:** reservas confirmadas con saldo mayor que cero.

## Compilar y ejecutar en macOS

Desde la carpeta `GestionEventos`:

```bash
clang++ -std=c++17 -Wall -Wextra -pedantic main.cpp -o GestionEventos
./GestionEventos
```

También se puede usar CMake:

```bash
cmake -S . -B build
cmake --build build
./build/GestionEventos
```

## Estructura del proyecto

```text
GestionEventos/
├── main.cpp
├── CMakeLists.txt
├── README.md
├── .gitignore
└── datos/
    ├── clientes.txt
    ├── reservas.txt
    ├── pagos.txt
    └── ids.txt
```

## Limitaciones conocidas

- Los arreglos tienen capacidad fija; no se amplían dinámicamente.
- Espacios, capacidades, horarios, tarifas y precios de amenidades son valores demostrativos definidos en `main.cpp`.
- La multa no se calcula automáticamente a partir de fechas: el usuario indica manualmente los días de atraso.
- La búsqueda por cliente requiere el nombre exacto.
- El programa no gestiona reembolsos al cancelar; estos deben resolverse externamente.
- La persistencia es local, en archivos de texto relativos a la carpeta desde la que se ejecuta el programa; no hay base de datos ni uso multiusuario.
- Los registros malformados se omiten durante la carga; al guardar los registros válidos, las líneas malformadas no se conservan.
