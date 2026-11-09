#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <sstream>
#include <ctime>
#include <limits>
using namespace std;

const int MAX_CLIENTES=200, MAX_RESERVAS=500, MAX_PAGOS=1500, MAX_ESPACIOS=3, MAX_AMENIDADES=4;
const double ANTICIPO_PORC=20.0, MULTA_DIARIA=200.0;
struct Cliente { int id; string nombre, telefono; };
struct Espacio { string codigo, nombre; int capacidad, inicio, fin; double tarifa; };
struct Reserva { int id, clienteId; string espacio, fecha, estado, estadoEvento; int inicio, fin, asistentes; double total, pagado, multa; string amenidades; };
struct Pago { int reservaId; string fecha, medio; double monto; };
Cliente clientes[MAX_CLIENTES]; Reserva reservas[MAX_RESERVAS]; Pago pagos[MAX_PAGOS];
Espacio espacios[MAX_ESPACIOS]={{"E01","Salon Aurora",500,8,18,2000},{"E02","Salon Eclipse",800,18,23,2500},{"E03","Estadio Central",5000,8,23,5000}};
string amenidadesN[MAX_AMENIDADES]={"Sonido","Pantallas","Iluminacion","Seguridad"};
double amenidadesP[MAX_AMENIDADES]={3000,3000,2000,1500};
int nC=0,nR=0,nP=0,nextC=1,nextR=1001;

string hoy(){ time_t t=time(nullptr); tm *p=localtime(&t); char b[11]; strftime(b,sizeof(b),"%d/%m/%Y",p); return b; }
string limpiar(const string&s){ string r=s; for(char &c:r) if(c=='|') c='/'; return r; }
bool fechaValida(const string& f){
 if(f.size()!=10 || f[2]!='/' || f[5]!='/') return false;
 for(int i=0;i<10;i++) if(i!=2 && i!=5 && (f[i]<'0' || f[i]>'9')) return false;
 int dia=stoi(f.substr(0,2)), mes=stoi(f.substr(3,2)), anio=stoi(f.substr(6,4));
 if(anio<1 || mes<1 || mes>12) return false;
 int diasMes[]={31,28,31,30,31,30,31,31,30,31,30,31};
 bool bisiesto=(anio%400==0 || (anio%4==0 && anio%100!=0));
 if(bisiesto) diasMes[1]=29;
 return dia>=1 && dia<=diasMes[mes-1];
}
int buscarClienteId(int id){ for(int i=0;i<nC;i++) if(clientes[i].id==id) return i; return -1; }
int buscarClienteNombre(string nombre){ for(int i=0;i<nC;i++) if(clientes[i].nombre==nombre) return i; return -1; }
int buscarReservaId(int id){ for(int i=0;i<nR;i++) if(reservas[i].id==id) return i; return -1; }
int buscarEspacio(string c){ for(int i=0;i<MAX_ESPACIOS;i++) if(espacios[i].codigo==c) return i; return -1; }
void pausar(){ cout<<"\nPresione ENTER para continuar..."; cin.ignore(numeric_limits<streamsize>::max(),'\n'); }
int leerEntero(string msg,int minV,int maxV){ int x; while(true){ cout<<msg; if(cin>>x && x>=minV && x<=maxV){cin.ignore(numeric_limits<streamsize>::max(),'\n'); return x;} cout<<"Dato invalido. Intente nuevamente.\n"; cin.clear(); cin.ignore(numeric_limits<streamsize>::max(),'\n'); } }
double leerMonto(string msg){ double x; while(true){cout<<msg; if(cin>>x && x>=0){cin.ignore(numeric_limits<streamsize>::max(),'\n'); return x;} cout<<"Ingrese un monto valido (mayor o igual a cero).\n";cin.clear();cin.ignore(numeric_limits<streamsize>::max(),'\n');} }
string leerTexto(string msg){ string s; do{cout<<msg; getline(cin,s); if(s.empty()) cout<<"El dato no puede quedar vacio.\n";}while(s.empty()); return limpiar(s); }
bool disponible(string esp,string fecha,int ini,int fin){ for(int i=0;i<nR;i++) if(reservas[i].espacio==esp && reservas[i].fecha==fecha && reservas[i].estado=="CONFIRMADA") if(ini<reservas[i].fin && fin>reservas[i].inicio) return false; return true; }
double saldo(int i){ return reservas[i].total+reservas[i].multa-reservas[i].pagado; }
void registrarCliente(){ if(nC>=MAX_CLIENTES){cout<<"Limite de clientes alcanzado.\n";return;} Cliente c; c.id=nextC++; c.nombre=leerTexto("Nombre completo: "); c.telefono=leerTexto("Telefono: "); clientes[nC++]=c; cout<<"Cliente registrado. Codigo: "<<c.id<<"\n"; }
void listarClientes(){ if(!nC){cout<<"No hay clientes.\n";return;} for(int i=0;i<nC;i++) cout<<clientes[i].id<<" | "<<clientes[i].nombre<<" | "<<clientes[i].telefono<<"\n"; }
void listarEspacios(){ cout<<"\nESPACIOS\n"; for(int i=0;i<MAX_ESPACIOS;i++) cout<<espacios[i].codigo<<" - "<<espacios[i].nombre<<" | Aforo "<<espacios[i].capacidad<<" | "<<espacios[i].inicio<<":00-"<<espacios[i].fin<<":00 | Q"<<espacios[i].tarifa<<"/hora\n"; }
void registrarReserva(){
 if(nR>=MAX_RESERVAS){cout<<"Limite de reservas alcanzado.\n";return;} if(nP>=MAX_PAGOS){cout<<"No hay espacio para registrar el pago inicial.\n";return;} if(nC==0){cout<<"Primero registre un cliente.\n";return;}
 listarClientes(); int cid=leerEntero("Codigo del cliente: ",1,1000000); if(buscarClienteId(cid)<0){cout<<"Cliente inexistente.\n";return;}
 listarEspacios(); string cod=leerTexto("Codigo del espacio: "); int e=buscarEspacio(cod); if(e<0){cout<<"Espacio inexistente.\n";return;}
 string fecha=leerTexto("Fecha del evento (DD/MM/AAAA): "); if(!fechaValida(fecha)){cout<<"Fecha invalida. Use DD/MM/AAAA y una fecha real.\n";return;} int ini=leerEntero("Hora inicio (0-23): ",0,23), fin=leerEntero("Hora fin (1-24): ",1,24);
 if(ini<espacios[e].inicio || fin>espacios[e].fin || ini>=fin){cout<<"Horario fuera del rango permitido o incorrecto.\n";return;}
 if(!disponible(cod,fecha,ini,fin)){cout<<"Conflicto: el espacio ya esta reservado en ese horario.\n";return;}
 int asistentes=leerEntero("Cantidad estimada de asistentes: ",1,100000);
 if(asistentes>espacios[e].capacidad){cout<<"Aforo excedido. Capacidad maxima: "<<espacios[e].capacidad<<"\n";return;}
 string extras="Ninguna"; double extra=0; cout<<"Amenidades disponibles:\n"; for(int a=0;a<MAX_AMENIDADES;a++) cout<<a+1<<". "<<amenidadesN[a]<<" Q"<<amenidadesP[a]<<"\n";
 int op; do{op=leerEntero("Agregar amenidad (1-4) o 0 para terminar: ",0,4); if(op>0){extra+=amenidadesP[op-1]; if(extras=="Ninguna") extras=amenidadesN[op-1]; else extras+=" + "+amenidadesN[op-1];}}while(op!=0);
 double total=(fin-ini)*espacios[e].tarifa+extra, anticipo=total*ANTICIPO_PORC/100.0;
 cout<<fixed<<setprecision(2)<<"Total: Q"<<total<<" | Anticipo minimo (20%): Q"<<anticipo<<"\n";
 double pago=leerMonto("Pago inicial: Q"); if(pago<anticipo || pago>total){cout<<"Pago no valido: debe cubrir el anticipo y no superar el total. Reserva no confirmada.\n";return;}
 Reserva r; r.id=nextR++; r.clienteId=cid;r.espacio=cod;r.fecha=fecha;r.inicio=ini;r.fin=fin;r.asistentes=asistentes;r.total=total;r.pagado=pago;r.multa=0;r.amenidades=extras;r.estado="CONFIRMADA";r.estadoEvento="PENDIENTE";reservas[nR++]=r;
 if(nP<MAX_PAGOS){pagos[nP++]={r.id,hoy(),"EFECTIVO",pago};}
 cout<<"Reserva confirmada. Codigo: "<<r.id<<". Saldo: Q"<<saldo(nR-1)<<"\n";
}
void consultarReserva(){ int id=leerEntero("Codigo de reserva: ",1,10000000); int i=buscarReservaId(id); if(i<0){cout<<"Reserva no encontrada.\n";return;} int c=buscarClienteId(reservas[i].clienteId); cout<<fixed<<setprecision(2)<<"Reserva "<<id<<" | Cliente: "<<(c>=0?clientes[c].nombre:"-")<<" | Espacio: "<<reservas[i].espacio<<" | Fecha: "<<reservas[i].fecha<<" | "<<reservas[i].inicio<<"-"<<reservas[i].fin<<" | Asistentes: "<<reservas[i].asistentes<<" | Total Q"<<reservas[i].total<<" | Pagado Q"<<reservas[i].pagado<<" | Multa Q"<<reservas[i].multa<<" | Saldo Q"<<saldo(i)<<" | Estado "<<reservas[i].estado<<" | Evento "<<reservas[i].estadoEvento<<"\nAmenidades: "<<reservas[i].amenidades<<"\n"; }
void buscarPorNombre(){ string nom=leerTexto("Nombre exacto del cliente: "); int c=buscarClienteNombre(nom); if(c<0){cout<<"Cliente no encontrado.\n";return;} bool hallo=false; for(int i=0;i<nR;i++) if(reservas[i].clienteId==clientes[c].id){cout<<"Reserva "<<reservas[i].id<<" | "<<reservas[i].fecha<<" | "<<reservas[i].espacio<<" | Saldo Q"<<saldo(i)<<"\n";hallo=true;} if(!hallo)cout<<"El cliente no tiene reservas.\n"; }
void registrarPago(){ int id=leerEntero("Codigo de reserva: ",1,10000000),i=buscarReservaId(id); if(i<0){cout<<"Reserva inexistente.\n";return;} if(reservas[i].estado!="CONFIRMADA"){cout<<"No se aceptan pagos para reservas no confirmadas.\n";return;} double s=saldo(i); if(s<=0){cout<<"La reserva ya esta pagada.\n";return;} cout<<"Saldo actual: Q"<<s<<"\n"; double m=leerMonto("Monto a pagar: Q"); if(m<=0 || m>s){cout<<"Monto invalido o superior al saldo.\n";return;} int medio=leerEntero("Medio: 1 Efectivo / 2 Transferencia: ",1,2); if(nP>=MAX_PAGOS){cout<<"No hay espacio para mas movimientos.\n";return;} reservas[i].pagado+=m; pagos[nP++]={id,hoy(),medio==1?"EFECTIVO":"TRANSFERENCIA",m}; cout<<"Pago registrado. Saldo restante: Q"<<saldo(i)<<"\n"; }
void cancelarReserva(){ int id=leerEntero("Codigo de reserva: ",1,10000000),i=buscarReservaId(id); if(i<0){cout<<"Reserva inexistente.\n";return;} if(reservas[i].estado!="CONFIRMADA" || reservas[i].estadoEvento!="PENDIENTE"){cout<<"Solo se pueden cancelar reservas confirmadas y pendientes de inicio.\n";return;} reservas[i].estado="CANCELADA"; cout<<"Reserva cancelada. Pagos historicos conservados: Q"<<reservas[i].pagado<<". El reembolso, si corresponde, debe gestionarse externamente y documentarse.\n"; }
void registrarMulta(){ int id=leerEntero("Codigo de reserva: ",1,10000000),i=buscarReservaId(id); if(i<0){cout<<"Reserva inexistente.\n";return;} if(reservas[i].estado!="CONFIRMADA"){cout<<"Reserva no activa.\n";return;} int dias=leerEntero("Dias de atraso: ",1,3650); reservas[i].multa+=dias*MULTA_DIARIA; cout<<"Multa agregada: Q"<<dias*MULTA_DIARIA<<". Total multas: Q"<<reservas[i].multa<<"\n"; }
void iniciarEvento(){ int id=leerEntero("Codigo de reserva: ",1,10000000),i=buscarReservaId(id); if(i<0){cout<<"Reserva inexistente.\n";return;} if(reservas[i].estado!="CONFIRMADA" || reservas[i].estadoEvento!="PENDIENTE"){cout<<"La reserva no esta en estado valido para iniciar.\n";return;} if(saldo(i)>0){cout<<"Inicio bloqueado. Saldo y multas pendientes: Q"<<saldo(i)<<"\n";return;} reservas[i].estadoEvento="INICIADO"; cout<<"Evento iniciado correctamente.\n"; }
void finalizarEvento(){ int id=leerEntero("Codigo de reserva: ",1,10000000),i=buscarReservaId(id); if(i<0){cout<<"Reserva inexistente.\n";return;} if(reservas[i].estadoEvento!="INICIADO"){cout<<"El evento no esta iniciado.\n";return;} reservas[i].estadoEvento="FINALIZADO"; cout<<"Evento finalizado.\n"; }
void ordenarReservasPorFecha(){ for(int i=0;i<nR-1;i++) for(int j=0;j<nR-i-1;j++) if(reservas[j].fecha>reservas[j+1].fecha){Reserva t=reservas[j];reservas[j]=reservas[j+1];reservas[j+1]=t;} }
void reporteCierre(){ string f=leerTexto("Fecha a reportar (DD/MM/AAAA): "); double ef=0,tr=0;int cant=0,pend=0,con=0,sin=0; cout<<"\nCIERRE DIARIO "<<f<<"\n"; for(int i=0;i<nP;i++)if(pagos[i].fecha==f){if(pagos[i].medio=="EFECTIVO")ef+=pagos[i].monto;else tr+=pagos[i].monto;} for(int i=0;i<nR;i++)if(reservas[i].fecha==f){cant++;if(saldo(i)>0)pend++;if(reservas[i].amenidades=="Ninguna")sin++;else con++;} cout<<fixed<<setprecision(2)<<"Reservas del dia: "<<cant<<"\nEfectivo recibido: Q"<<ef<<"\nTransferencias: Q"<<tr<<"\nReservas con saldo: "<<pend<<"\nCon amenidades: "<<con<<" | Sin amenidades: "<<sin<<"\n"; }
void reportePorEspacio(){ cout<<"\nINGRESOS POR ESPACIO\n"; for(int e=0;e<MAX_ESPACIOS;e++){double t=0,p=0;int c=0;for(int i=0;i<nR;i++)if(reservas[i].espacio==espacios[e].codigo && reservas[i].estado!="CANCELADA"){t+=reservas[i].total;p+=reservas[i].pagado;c++;}cout<<espacios[e].nombre<<" | Reservas: "<<c<<" | Total contratado: Q"<<t<<" | Cobrado: Q"<<p<<"\n";} }
void reporteSaldos(){ cout<<"\nSALDOS PENDIENTES\n"; int c=0;for(int i=0;i<nR;i++)if(reservas[i].estado=="CONFIRMADA" && saldo(i)>0){cout<<"Reserva "<<reservas[i].id<<" | "<<reservas[i].fecha<<" | Saldo Q"<<saldo(i)<<"\n";c++;}if(!c)cout<<"No hay saldos pendientes.\n"; }
void guardar(){ ofstream fc("datos/clientes.txt"),fr("datos/reservas.txt"),fp("datos/pagos.txt"),fi("datos/ids.txt"); if(!fc||!fr||!fp||!fi){cout<<"Error al abrir archivos para guardar. Ejecute desde la carpeta del proyecto.\n";return;} for(int i=0;i<nC;i++)fc<<clientes[i].id<<'|'<<clientes[i].nombre<<'|'<<clientes[i].telefono<<'\n'; for(int i=0;i<nR;i++){Reserva&r=reservas[i];fr<<r.id<<'|'<<r.clienteId<<'|'<<r.espacio<<'|'<<r.fecha<<'|'<<r.inicio<<'|'<<r.fin<<'|'<<r.asistentes<<'|'<<r.total<<'|'<<r.pagado<<'|'<<r.multa<<'|'<<r.estado<<'|'<<r.estadoEvento<<'|'<<r.amenidades<<'\n';}for(int i=0;i<nP;i++)fp<<pagos[i].reservaId<<'|'<<pagos[i].fecha<<'|'<<pagos[i].medio<<'|'<<pagos[i].monto<<'\n';fi<<nextC<<'|'<<nextR<<'\n';cout<<"Datos guardados correctamente.\n"; }
void cargar(){ string l; ifstream fc("datos/clientes.txt");while(getline(fc,l)&&nC<MAX_CLIENTES){stringstream s(l);string a,b,c;if(getline(s,a,'|')&&getline(s,b,'|')&&getline(s,c,'|')){clientes[nC++]={stoi(a),b,c};}} ifstream fr("datos/reservas.txt");while(getline(fr,l)&&nR<MAX_RESERVAS){stringstream s(l);string v[13];bool ok=true;for(int k=0;k<13;k++)if(!getline(s,v[k],'|')){ok=false;break;}if(ok){Reserva r;r.id=stoi(v[0]);r.clienteId=stoi(v[1]);r.espacio=v[2];r.fecha=v[3];r.inicio=stoi(v[4]);r.fin=stoi(v[5]);r.asistentes=stoi(v[6]);r.total=stod(v[7]);r.pagado=stod(v[8]);r.multa=stod(v[9]);r.estado=v[10];r.estadoEvento=v[11];r.amenidades=v[12];reservas[nR++]=r;}} ifstream fp("datos/pagos.txt");while(getline(fp,l)&&nP<MAX_PAGOS){stringstream s(l);string a,b,c,d;if(getline(s,a,'|')&&getline(s,b,'|')&&getline(s,c,'|')&&getline(s,d,'|'))pagos[nP++]={stoi(a),b,c,stod(d)};} ifstream fi("datos/ids.txt");int a,b;if(fi>>a>>b){nextC=a;nextR=b;}else{for(int i=0;i<nC;i++)if(clientes[i].id>=nextC)nextC=clientes[i].id+1;for(int i=0;i<nR;i++)if(reservas[i].id>=nextR)nextR=reservas[i].id+1;} }
void menuBusquedas(){int op;do{cout<<"\nBUSQUEDAS\n1. Por codigo de reserva\n2. Por nombre exacto de cliente\n0. Volver\n";op=leerEntero("Opcion: ",0,2);if(op==1)consultarReserva();if(op==2)buscarPorNombre();}while(op!=0);}
void menuReportes(){int op;do{cout<<"\nREPORTES\n1. Cierre diario\n2. Ingresos por espacio\n3. Saldos pendientes\n0. Volver\n";op=leerEntero("Opcion: ",0,3);if(op==1)reporteCierre();if(op==2)reportePorEspacio();if(op==3)reporteSaldos();}while(op!=0);}
int main(){cargar();int op;do{cout<<"\n========================================\n SISTEMA DE GESTION Y RESERVA DE EVENTOS\n========================================\n1. Registrar cliente\n2. Listar clientes\n3. Consultar espacios\n4. Registrar reserva\n5. Registrar pago\n6. Registrar multa por atraso\n7. Iniciar evento\n8. Finalizar evento\n9. Cancelar reserva\n10. Buscar reservas\n11. Reportes\n12. Ordenar reservas por fecha\n13. Guardar datos\n0. Guardar y salir\n";op=leerEntero("Seleccione: ",0,13);switch(op){case 1:registrarCliente();break;case 2:listarClientes();break;case 3:listarEspacios();break;case 4:registrarReserva();break;case 5:registrarPago();break;case 6:registrarMulta();break;case 7:iniciarEvento();break;case 8:finalizarEvento();break;case 9:cancelarReserva();break;case 10:menuBusquedas();break;case 11:menuReportes();break;case 12:ordenarReservasPorFecha();cout<<"Reservas ordenadas por fecha (burbuja).\n";break;case 13:guardar();break;case 0:guardar();cout<<"Hasta pronto.\n";break;}}while(op!=0);return 0;}
