// ============================================================================
// PROYECTO FINAL: SISTEMA DE GESTION DE INVENTARIO, VENTAS Y CREDITOS
// ORGANIZACION: MINIMARKET ATOMAY (Tomayquichua, Huanuco)
// ============================================================================

#include <iostream>
#include <string>
#include <iomanip>
#include <cstdlib>

using namespace std;

const int CAPACIDAD = 100;

// ============================================================================
// 1. DEFINICION DE ESTRUCTURAS DE DATOS COMPUESTAS (STRUCTS)
// ============================================================================

struct Producto {
    string CodPro; // Codigo unico del producto (Ej. P001)
    string NomPro; // Nombre y presentacion
    float PreUni;  // Precio unitario de venta en soles
    int StoDis;    // Cantidad fisica disponible en inventario
    string FecVen; // Fecha de vencimiento (DD/MM/AAAA)
};

struct Cliente {
    string DniCli; // Documento Nacional de Identidad (8 digitos)
    string NomCli; // Nombres y apellidos del cliente frecuente
    float DeuCli;  // Saldo deudor acumulado por compras a credito
    bool EstCta;   // Estado de linea de credito (true = Activo, false = Suspendido)
};

// Estructura para cada turno/pedido en la Cola FIFO de atencion
struct TurnoPedido {
    int IdTran;       // Numero correlativo de turno/ticket
    string NombreCli; // Referencia rapida del cliente en fila
    string CodProd;   // Codigo del producto principal solicitado
    int Cantidad;     // Cantidad solicitada
    float MonTot;     // Monto estimado del pedido
};

// ============================================================================
// 2. INSTANCIACION EN MEMORIA PRINCIPAL (RAM) Y VARIABLES DE CONTROL
// ============================================================================

Producto inventario[CAPACIDAD];
int totalProd = 0;

Cliente clientes[CAPACIDAD];
int totalCli = 0;

// Cola Secuencial FIFO para atencion por orden de llegada (Enqueue y Dequeue reales)
TurnoPedido colaPedidos[CAPACIDAD];
int frente = -1;            // Apunta al primer pedido en entrar (proximo en salir)
int finalCola = -1;         // Apunta al ultimo pedido en llegar
int correlativoTurno = 100; // Generador correlativo de turnos/boletas

// Acumuladores de cierre de caja del turno
float ingresosEfectivo = 0.0f;
float ingresosYape = 0.0f;
float ventasAlFiado = 0.0f;
int totalVentasDespachadas = 0;

// ============================================================================
// 3. FUNCIONES DE APOYO Y PRECARGA DE DATOS INICIALES
// ============================================================================

void agregarProdInicial(string cod, string nom, float pre, int sto, string fec) {
    inventario[totalProd].CodPro = cod;
    inventario[totalProd].NomPro = nom;
    inventario[totalProd].PreUni = pre;
    inventario[totalProd].StoDis = sto;
    inventario[totalProd].FecVen = fec;
    totalProd++;
}

void agregarCliInicial(string dni, string nom, float deu, bool est) {
    clientes[totalCli].DniCli = dni;
    clientes[totalCli].NomCli = nom;
    clientes[totalCli].DeuCli = deu;
    clientes[totalCli].EstCta = est;
    totalCli++;
}

void cargarDatosIniciales() {
    // 10 Productos precargados (incluye articulos por unidad y stock critico <= 5)
    agregarProdInicial("P001", "Arroz Costeno 1kg",       4.50f, 28, "15/12/2026");
    agregarProdInicial("P002", "Leche Gloria Azul 400g",  4.20f, 35, "20/01/2027");
    agregarProdInicial("P003", "Gaseosa Inca Kola 500ml", 3.00f, 40, "10/03/2027");
    agregarProdInicial("P004", "Galleta Soda Field",      1.00f,  4, "18/11/2026"); // Stock bajo
    agregarProdInicial("P005", "Aceite Primor 1L",        8.50f, 15, "05/06/2027");
    agregarProdInicial("P006", "Azucar Rubia 1kg",        3.80f, 22, "30/04/2027");
    agregarProdInicial("P007", "Platano de Seda (Unid)",  0.40f, 18, "20/11/2026"); // Por unidad
    agregarProdInicial("P008", "Agua San Mateo 600ml",    1.50f,  3, "12/05/2027"); // Stock bajo
    agregarProdInicial("P009", "Atun Florida Trozos",     5.80f, 12, "14/08/2027");
    agregarProdInicial("P010", "Papas Lays Clasicas",     2.00f,  5, "02/12/2026"); // Stock bajo

    // 7 Clientes frecuentes precargados
    agregarCliInicial("72458912", "Juan Carlos Perez",    15.50f, true);
    agregarCliInicial("41902345", "Maria Elena Lopez",     0.00f, true);
    agregarCliInicial("45891230", "Rosa Angelica Torres", 28.00f, true);
    agregarCliInicial("70123456", "Luis Alberto Gomez",    8.40f, true);
    agregarCliInicial("43219876", "Carmen Rosa Diaz",     42.00f, true);
    agregarCliInicial("71894562", "Pedro Pablo Quispe",    0.00f, true);
    agregarCliInicial("40987654", "Jorge Luis Mendoza",   65.00f, false); // Suspendido
}

// ============================================================================
// 4. BUSQUEDAS LINEALES Y OPERACIONES REALES DE COLA FIFO (ENQUEUE / DEQUEUE)
// ============================================================================

int buscarProducto(string criterio) {
    for (int i = 0; i < totalProd; i++) {
        if (inventario[i].CodPro == criterio || inventario[i].NomPro == criterio) {
            return i;
        }
    }
    return -1;
}

int buscarCliente(string dni) {
    for (int i = 0; i < totalCli; i++) {
        if (clientes[i].DniCli == dni) {
            return i;
        }
    }
    return -1;
}

// Verifica si la cola de pedidos pendientes esta vacia
bool colaVacia() {
    return (frente == -1);
}

// ENQUEUE O(1): Inserta un nuevo cliente/pedido al FINAL de la Cola FIFO
void encolarPedido() {
    if (finalCola >= CAPACIDAD - 1) {
        cout << "\nERROR: Capacidad maxima de la cola alcanzada.\n";
        return;
    }
    if (totalProd == 0) {
        cout << "\nALERTA: Catalogo vacio. Registre productos primero.\n";
        return;
    }

    string nomCli, codProd;
    int cant;

    cout << "\n--- ENCOLAR PEDIDO EN HORA PICO (ENQUEUE FIFO) ---\n";
    cin.ignore(1000, '\n');
    cout << "Nombre o referencia del cliente en fila: ";
    getline(cin, nomCli);

    cout << "Codigo del producto solicitado (Ej. P003): ";
    cin >> codProd;

    int pos = buscarProducto(codProd);
    if (pos == -1) {
        cout << "ERROR: Producto no encontrado.\n";
        return;
    }

    cout << "Producto: " << inventario[pos].NomPro
         << " | Precio: S/ " << fixed << setprecision(2) << inventario[pos].PreUni
         << " | Stock: " << inventario[pos].StoDis << "\n";
    cout << "Cantidad solicitada: ";
    cin >> cant;

    if (cant <= 0 || cant > inventario[pos].StoDis) {
        cout << "ERROR: Stock insuficiente o cantidad invalida.\n";
        return;
    }

    // Si la cola estaba vacia, frente y final apuntan al primer elemento (0)
    if (colaVacia()) {
        frente = 0;
        finalCola = 0;
    } else {
        finalCola++;
    }

    correlativoTurno++;
    colaPedidos[finalCola].IdTran = correlativoTurno;
    colaPedidos[finalCola].NombreCli = nomCli;
    colaPedidos[finalCola].CodProd = codProd;
    colaPedidos[finalCola].Cantidad = cant;
    colaPedidos[finalCola].MonTot = cant * inventario[pos].PreUni;

    cout << "TURNO ASIGNADO #" << correlativoTurno << " para " << nomCli
         << " (Posicionado al FINAL de la cola).\n";
}

// Muestra los pedidos actualmente esperando en la Cola FIFO (desde frente hasta finalCola)
void mostrarColaPendientes() {
    cout << "\n===============================================================================\n";
    cout << "             COLA DE ATENCION EN MOSTRADOR (ORDEN FIFO ESTRICTO)               \n";
    cout << "===============================================================================\n";

    if (colaVacia()) {
        cout << "La cola esta vacia (frente = -1, final = -1). No hay clientes esperando.\n";
        cout << "===============================================================================\n";
        return;
    }

    cout << "Puntero FRENTE (proximo en salir): Turno #" << colaPedidos[frente].IdTran
         << " | Puntero FINAL: Turno #" << colaPedidos[finalCola].IdTran << "\n";
    cout << "-------------------------------------------------------------------------------\n";
    cout << left << setw(10) << "TURNO"
         << setw(24) << "CLIENTE"
         << setw(12) << "COD.PROD"
         << setw(10) << "CANT."
         << setw(14) << "ESTIMADO" << "\n";
    cout << "-------------------------------------------------------------------------------\n";

    for (int i = frente; i <= finalCola; i++) {
        cout << left << "#" << setw(9) << colaPedidos[i].IdTran
             << setw(24) << colaPedidos[i].NombreCli
             << setw(12) << colaPedidos[i].CodProd
             << setw(10) << colaPedidos[i].Cantidad
             << "S/ " << fixed << setprecision(2) << colaPedidos[i].MonTot << "\n";
    }
    cout << "===============================================================================\n";
}

// DEQUEUE O(1): Retira al cliente del FRENTE de la cola, cobra, descuenta stock y boletea
void despacharSiguienteCola() {
    if (colaVacia()) {
        cout << "\nALERTA: No hay pedidos pendientes en la cola para despachar.\n";
        return;
    }

    // 1. Toma el elemento ubicado exactamente en el FRENTE
    TurnoPedido actual = colaPedidos[frente];

    cout << "\n--- DESPACHANDO TURNO DEL FRENTE (DEQUEUE FIFO) ---\n";
    cout << "Turno Nro : #" << actual.IdTran << " | Cliente: " << actual.NombreCli << "\n";

    int posProd = buscarProducto(actual.CodProd);
    float monTot = 0.0f;

    if (posProd != -1 && actual.Cantidad <= inventario[posProd].StoDis) {
        monTot = actual.Cantidad * inventario[posProd].PreUni;
        inventario[posProd].StoDis -= actual.Cantidad;
        cout << "Articulo  : " << inventario[posProd].NomPro
             << " (" << actual.Cantidad << " unid.) -> S/ "
             << fixed << setprecision(2) << monTot << "\n";
    } else {
        cout << "AVISO: El stock cambio mientras esperaba. Se cobrara solo articulos adicionales.\n";
    }

    // Permite agregar productos adicionales antes de cerrar la boleta
    char mas;
    cout << "¿El cliente desea agregar otro producto adicional en caja? (S/N): ";
    cin >> mas;
    cin.ignore(1000, '\n');

    while (mas == 'S' || mas == 's') {
        string crit;
        int cExtra;
        cout << "Ingrese codigo o nombre del producto adicional: ";
        getline(cin, crit);
        int pExtra = buscarProducto(crit);

        if (pExtra != -1) {
            cout << "Disponible: " << inventario[pExtra].StoDis
                 << " | Precio: S/ " << inventario[pExtra].PreUni << "\n";
            cout << "Cantidad: ";
            cin >> cExtra;
            if (cExtra > 0 && cExtra <= inventario[pExtra].StoDis) {
                float sub = cExtra * inventario[pExtra].PreUni;
                monTot += sub;
                inventario[pExtra].StoDis -= cExtra;
                cout << "Agregado -> Subtotal: S/ " << fixed << setprecision(2) << sub << "\n";
            } else {
                cout << "ERROR: Stock insuficiente.\n";
            }
        } else {
            cout << "ERROR: Producto no encontrado.\n";
        }
        cout << "¿Agregar otro articulo? (S/N): ";
        cin >> mas;
        cin.ignore(1000, '\n');
    }

    // 2. Actualiza punteros de la cola (Caso critico: sale el unico elemento)
    if (frente == finalCola) {
        frente = -1;
        finalCola = -1; // La cola quedo completamente vacia
    } else {
        frente++;       // Avanza el frente al siguiente cliente en espera
    }

    if (monTot <= 0.0f) {
        cout << "Turno #" << actual.IdTran << " retirado de la cola sin cobro.\n";
        return;
    }

    // 3. Procesamiento de pago y emision de boleta
    int opPago;
    float monRec = 0.0f, monVue = 0.0f;
    string metPag;

    cout << "\nTOTAL A COBRAR: S/ " << fixed << setprecision(2) << monTot << "\n";
    cout << "Modalidad de pago [1. Efectivo | 2. Yape | 3. Fiado]: ";
    cin >> opPago;

    switch (opPago) {
        case 1:
            metPag = "Efectivo";
            do {
                cout << "Efectivo recibido (S/): ";
                cin >> monRec;
                if (monRec < monTot) cout << "ERROR: Monto menor al total.\n";
            } while (monRec < monTot);
            monVue = monRec - monTot;
            ingresosEfectivo += monTot;
            break;

        case 2:
            metPag = "Yape";
            monRec = monTot;
            ingresosYape += monTot;
            break;

        case 3: {
            metPag = "Fiado";
            string dni;
            cout << "Ingrese DNI del cliente para cargar a cuenta: ";
            cin >> dni;
            int posCli = buscarCliente(dni);
            if (posCli == -1 || !clientes[posCli].EstCta) {
                cout << "ERROR: Cliente no registrado o con credito SUSPENDIDO. Cobrar al contado.\n";
                metPag = "Efectivo";
                monRec = monTot;
                ingresosEfectivo += monTot;
            } else {
                clientes[posCli].DeuCli += monTot;
                ventasAlFiado += monTot;
                cout << "Cargado a " << clientes[posCli].NomCli
                     << " | Nueva deuda: S/ " << clientes[posCli].DeuCli << "\n";
            }
            break;
        }

        default:
            metPag = "Efectivo";
            monRec = monTot;
            ingresosEfectivo += monTot;
            break;
    }

    totalVentasDespachadas++;

    cout << "\n============================================\n";
    cout << "       BOLETA DE VENTA - MINIMARKET ATOMAY  \n";
    cout << "============================================\n";
    cout << " Turno Desencolado : #" << actual.IdTran << "\n";
    cout << " Cliente           : " << actual.NombreCli << "\n";
    cout << " Metodo de Pago    : " << metPag << "\n";
    cout << " Total Cobrado     : S/ " << fixed << setprecision(2) << monTot << "\n";
    if (metPag == "Efectivo") {
        cout << " Efectivo Recibido : S/ " << monRec << "\n";
        cout << " Vuelto Exacto     : S/ " << monVue << "\n";
    }
    cout << "============================================\n";
}

// ============================================================================
// 5. MODULOS DE INVENTARIO, CREDITOS (ALTA Y ABONOS) Y REPORTES
// ============================================================================

void registrarProducto() {
    if (totalProd >= CAPACIDAD) {
        cout << "\nERROR: Capacidad maxima de inventario alcanzada (100).\n";
        return;
    }
    string cod, nom, fec;
    float pre;
    int sto;

    cout << "\n--- REGISTRO DE NUEVO PRODUCTO ---\n";
    cout << "Codigo unico (Ej. P011): ";
    cin >> cod;

    if (buscarProducto(cod) != -1) {
        cout << "ERROR: El codigo '" << cod << "' ya existe en el catalogo.\n";
        return;
    }

    cin.ignore(1000, '\n');
    cout << "Nombre y presentacion (NomPro): ";
    getline(cin, nom);
    cout << "Precio unitario S/ (PreUni): ";
    cin >> pre;
    cout << "Stock inicial (StoDis): ";
    cin >> sto;
    cout << "Fecha vencimiento (DD/MM/AAAA): ";
    cin >> fec;

    if (pre <= 0 || sto < 0 || nom == "") {
        cout << "ERROR: Datos invalidos.\n";
        return;
    }

    agregarProdInicial(cod, nom, pre, sto, fec);
    cout << "EXITO: Producto registrado en la posicion [" << totalProd - 1 << "].\n";
}

void listarProductos() {
    cout << "\n===============================================================================\n";
    cout << "                   CATALOGO E INVENTARIO ACTUAL - ATOMAY                       \n";
    cout << "===============================================================================\n";
    cout << left << setw(8)  << "CODIGO"
         << setw(28) << "PRODUCTO"
         << setw(12) << "PRECIO"
         << setw(10) << "STOCK"
         << setw(15) << "VENCIMIENTO" << "\n";
    cout << "-------------------------------------------------------------------------------\n";

    for (int i = 0; i < totalProd; i++) {
        cout << left << setw(8)  << inventario[i].CodPro
             << setw(28) << inventario[i].NomPro
             << "S/ " << setw(9) << fixed << setprecision(2) << inventario[i].PreUni
             << setw(10) << inventario[i].StoDis
             << setw(15) << inventario[i].FecVen << "\n";
    }
    cout << "===============================================================================\n";
    cout << "Total de articulos registrados: " << totalProd << " / " << CAPACIDAD << "\n";
}

void registrarCliente() {
    if (totalCli >= CAPACIDAD) {
        cout << "\nERROR: Padron de clientes lleno.\n";
        return;
    }
    string dni, nom;
    cout << "\n--- REGISTRO DE CLIENTE PARA LINEA DE CREDITO ---\n";
    cout << "DNI del cliente (8 digitos): ";
    cin >> dni;

    if (buscarCliente(dni) != -1) {
        cout << "ERROR: El DNI ya se encuentra registrado.\n";
        return;
    }

    cin.ignore(1000, '\n');
    cout << "Nombres y apellidos completos: ";
    getline(cin, nom);

    agregarCliInicial(dni, nom, 0.00f, true);
    cout << "EXITO: Cliente '" << nom << "' habilitado con linea ACTIVA.\n";
}

void listarClientesCredito() {
    cout << "\n===============================================================================\n";
    cout << "            PADRON DE CLIENTES CON LINEA DE CREDITO Y FIADOS                   \n";
    cout << "===============================================================================\n";
    cout << left << setw(12) << "DNI"
         << setw(28) << "CLIENTE"
         << setw(16) << "DEUDA ACTUAL"
         << setw(18) << "ESTADO CREDITO" << "\n";
    cout << "-------------------------------------------------------------------------------\n";

    float deudaTotal = 0.0f;
    for (int i = 0; i < totalCli; i++) {
        string est = clientes[i].EstCta ? "ACTIVO" : "SUSPENDIDO";
        cout << left << setw(12) << clientes[i].DniCli
             << setw(28) << clientes[i].NomCli
             << "S/ " << setw(13) << fixed << setprecision(2) << clientes[i].DeuCli
             << setw(18) << est << "\n";
        deudaTotal += clientes[i].DeuCli;
    }
    cout << "===============================================================================\n";
    cout << "Total cuentas por cobrar (Fiados): S/ " << fixed << setprecision(2) << deudaTotal << "\n";
}

// Funcion para registrar pagos/amortizaciones y dar de alta lineas suspendidas
void gestionarAbonoYCredito() {
    string dni;
    cout << "\n--- GESTION DE ABONOS DE DEUDA Y ALTA DE CREDITO ---\n";
    cout << "Ingrese DNI del cliente: ";
    cin >> dni;

    int pos = buscarCliente(dni);
    if (pos == -1) {
        cout << "ERROR: Cliente no encontrado.\n";
        return;
    }

    cout << "Cliente : " << clientes[pos].NomCli << "\n";
    cout << "Deuda   : S/ " << fixed << setprecision(2) << clientes[pos].DeuCli << "\n";
    cout << "Estado  : " << (clientes[pos].EstCta ? "ACTIVO" : "SUSPENDIDO") << "\n";
    cout << "----------------------------------------------------\n";
    cout << "1. Registrar pago / abono parcial o total\n";
    cout << "2. Dar de ALTA (Activar linea de credito)\n";
    cout << "3. SUSPENDER linea de credito\n";
    cout << "Seleccione accion: ";

    int acc;
    cin >> acc;

    if (acc == 1) {
        if (clientes[pos].DeuCli <= 0.0f) {
            cout << "El cliente no tiene deuda pendiente.\n";
            return;
        }
        float abono;
        cout << "Ingrese monto a abonar (S/): ";
        cin >> abono;
        if (abono > 0.0f && abono <= clientes[pos].DeuCli) {
            clientes[pos].DeuCli -= abono;
            cout << "EXITO: Abono aplicado. Nueva deuda: S/ "
                 << fixed << setprecision(2) << clientes[pos].DeuCli << "\n";
            if (clientes[pos].DeuCli == 0.0f && !clientes[pos].EstCta) {
                clientes[pos].EstCta = true;
                cout << "AVISO: Al saldar su deuda, la linea de credito fue REACTIVADA.\n";
            }
        } else {
            cout << "ERROR: Monto invalido o superior a la deuda.\n";
        }
    } else if (acc == 2) {
        clientes[pos].EstCta = true;
        cout << "EXITO: Linea de credito DADA DE ALTA para " << clientes[pos].NomCli << ".\n";
    } else if (acc == 3) {
        clientes[pos].EstCta = false;
        cout << "AVISO: Linea de credito SUSPENDIDA.\n";
    }
}

void generarReportesYAlertas() {
    const int STOCK_MINIMO = 5;
    int alertas = 0;

    cout << "\n===============================================================================\n";
    cout << "               REPORTE DE ALERTAS DE REPOSICION (STOCK <= 5 UNID.)             \n";
    cout << "===============================================================================\n";

    for (int i = 0; i < totalProd; i++) {
        if (inventario[i].StoDis <= STOCK_MINIMO) {
            cout << "[ALERTA] Cod: " << left << setw(6) << inventario[i].CodPro
                 << " | " << setw(26) << inventario[i].NomPro
                 << " | Stock actual: " << inventario[i].StoDis << " unid.\n";
            alertas++;
        }
    }
    if (alertas == 0) cout << "Todo el inventario tiene stock optimo (> 5 unid.).\n";

    cout << "\n===============================================================================\n";
    cout << "                    BALANCE DE CIERRE DE TURNO - CAJA ATOMAY                   \n";
    cout << "===============================================================================\n";
    cout << "Ventas despachadas (Dequeue) : " << totalVentasDespachadas << "\n";
    cout << "Pedidos aun en fila (Cola)   : " << (colaVacia() ? 0 : (finalCola - frente + 1)) << "\n";
    cout << "Ingresos en Efectivo         : S/ " << fixed << setprecision(2) << ingresosEfectivo << "\n";
    cout << "Ingresos por Yape            : S/ " << fixed << setprecision(2) << ingresosYape << "\n";
    cout << "Ventas al Fiado              : S/ " << fixed << setprecision(2) << ventasAlFiado << "\n";
    cout << "TOTAL MOVIDO EN EL TURNO     : S/ " << fixed << setprecision(2)
         << (ingresosEfectivo + ingresosYape + ventasAlFiado) << "\n";
    cout << "===============================================================================\n";
}

// ============================================================================
// 6. FUNCION PRINCIPAL (CONTROLADOR DEL MENU INTERACTIVO)
// ============================================================================
int main() {
    cargarDatosIniciales();

    int opcion = 0;
    do {
        cout << "\n==========================================================\n";
        cout << "         MINIMARKET ATOMAY - SISTEMA DE GESTION           \n";
        cout << "==========================================================\n";
        cout << "1. Registrar Nuevo Producto en Inventario\n";
        cout << "2. Ver Catalogo Completo de Productos (10 precargados)\n";
        cout << "3. Registrar Cliente Frecuente (Linea de Credito)\n";
        cout << "4. Ver Clientes con Linea de Credito y Fiados\n";
        cout << "5. Registrar Abono de Deuda o Dar de Alta Credito\n";
        cout << "6. Encolar Pedido de Cliente en Hora Pico (Enqueue FIFO)\n";
        cout << "7. Ver Cola de Clientes Esperando en Mostrador\n";
        cout << "8. Atender Siguiente Turno y Emitir Boleta (Dequeue FIFO)\n";
        cout << "9. Generar Reportes de Caja y Alertas de Stock\n";
        cout << "10. Salir del Sistema\n";
        cout << "----------------------------------------------------------\n";
        cout << "Seleccione una opcion (1-10): ";
        cin >> opcion;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            opcion = 0;
        }

        switch (opcion) {
            case 1: registrarProducto(); break;
            case 2: listarProductos(); break;
            case 3: registrarCliente(); break;
            case 4: listarClientesCredito(); break;
            case 5: gestionarAbonoYCredito(); break;
            case 6: encolarPedido(); break;
            case 7: mostrarColaPendientes(); break;
            case 8: despacharSiguienteCola(); break;
            case 9: generarReportesYAlertas(); break;
            case 10: cout << "\nCerrando sistema del Minimarket ATOMAY...\n"; break;
            default: cout << "\nERROR: Opcion invalida (1-10).\n";
        }
    } while (opcion != 10);

    system("pause");
    return 0;
}
