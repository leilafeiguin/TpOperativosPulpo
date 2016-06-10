/*
 * CPU.c
 *
 *  Created on: 9/6/2016
 *      Author: utnso
 */

#include "funcionesCPU.h"
#include <commons/config.h>
#define ARCHIVOLOG "CPU.log"
#define CONFIG_CPU "config_cpu"
CONF_CPU config_cpu;

t_log* log;  //en COMUNES tendrian que estar las estructuras del log?


AnSISOP_funciones primitivas = {
		.AnSISOP_definirVariable		= definirVariable,
		.AnSISOP_obtenerPosicionVariable= obtenerPosicionVariable,
		.AnSISOP_dereferenciar			= dereferenciar,
		.AnSISOP_asignar				= asignar,
		.AnSISOP_obtenerValorCompartida = obtenerValorCompartida,
		.AnSISOP_asignarValorCompartida = asignarValorCompartida,
		.AnSISOP_irAlLabel				= irAlLabel,
		.AnSISOP_llamarConRetorno		= llamarFuncion,
		.AnSISOP_retornar				= retornar,
		.AnSISOP_imprimir				= imprimir,
		.AnSISOP_imprimirTexto			= imprimirTexto,
		.AnSISOP_entradaSalida			= entradaSalida,

};
AnSISOP_kernel primitivas_kernel = {
		.AnSISOP_wait					=wait,
		.AnSISOP_signal					=signal,
};


int main(int argc,char **argv){

	int sigusr1_desactivado =1; //el encanrgado de cambiar esto es el nucleo, tengo que tener forma de recibir la senal..
	log= log_create(ARCHIVOLOG, "CPU", 0, LOG_LEVEL_INFO);
	log_info(log,"Iniciando CPU\n");
	t_pcb * serializado;

	levantar_configuraciones();

	int umc = conectarConUmc();

	int nucleo = conectarConNucleo();
	t_paquete* datos_kernel=recibir(nucleo);  //una vez que nucleo se conecta con cpu debe mandar t_datos_kernel..

	while(sigusr1_desactivado){

		int quantum = ((t_datos_kernel*)(datos_kernel->data))->QUANTUM;
		int tamanioPag = ((t_datos_kernel*)(datos_kernel->data))->TAMPAG; /// liberar_paquete..
		int quantum_sleep = ((t_datos_kernel*)(datos_kernel->data))->QUANTUM_SLEEP;



		t_paquete* paquete_recibido = recibir(nucleo);
		t_pcb* pcb = desserializarPCB(paquete_recibido->data);
		liberar_paquete(paquete_recibido);

		int pid = pcb->pid;
		enviar(umc, 405, sizeof(int), pid); // codigo 405: cambio de proceso activo NUCLEO - UMC

		int programaBloqueado = 0;
		int programaFinalizado = 0;
		int programaAbortado = 0;

		while(quantum && !programaBloqueado && !programaFinalizado && !programaAbortado){

			t_direccion* datos_para_umc = crearEstructuraParaUMC (pcb, tamanioPag);
			enviar(umc, 404, datos_para_umc->size, datos_para_umc);
			t_paquete* instruccion=recibir(umc);
			char* sentencia= instruccion->data;
			analizadorLinea(depurarSentencia(strdup(sentencia)), &primitivas, &primitivas_kernel);
			liberar_paquete(instruccion);
			// y la alcutalizacion de los valores en la umc lo hacen la primitiva al analizarla linea

			pcb->pc++;
			quantum--;
			usleep(quantum_sleep);

			if (programaBloqueado){
				log_info(log, "El programa salió por bloqueo");
				enviar(nucleo, 340, sizeof(t_paquete*), serializado);
				destruirPCB(pcb);
					}

			if (programaAbortado){
				log_info(log, "El programa aborto");
				serializado = (t_pcb*)serializarPCB(pcb);
				enviar(nucleo, 333, sizeof(t_paquete*), serializado); //codigo de op 333, pcb abortado
				destruirPCB(pcb);
			}

			if (programaFinalizado){
				log_debug(log, "El programa finalizo");
				serializado = (t_pcb*)serializarPCB(pcb);
				enviar(nucleo, 320, sizeof(int), programaFinalizado); //codigo de op 408, pcb finalizo
				destruirPCB(pcb);
			}

			if(quantum &&!programaFinalizado&&!programaBloqueado&&!programaAbortado){
				serializado = (t_pcb*)serializarPCB(pcb);
				enviar(nucleo, 304, sizeof(t_paquete*), serializado); //codigo de op 407, pcb salio por quantum
				destruirPCB(pcb);
			}
		}

		liberar_paquete(datos_kernel);

		close(nucleo);
		close(umc);

		return 0;
	}

return 0;

}

//*******************************************FUNCIONES**********************************************************

int conectarConUmc(){
		int umc = conectar_a(config_cpu.IP_UMC, config_cpu.PUERTO_UMC);
		if(umc==-1){
			log_info(log,"CPU: No encontre memoria\n");
			exit (EXIT_FAILURE);
		}
		log_info(log,"CPU: Conecta bien memoria %d\n", umc);
		bool estado = realizar_handshake(umc);
		if (!estado) {
			log_info(log,"CPU:Handshake invalido con memoria\n");
			exit(EXIT_FAILURE);
		}
		else{
			log_info(log,"CPU:Handshake exitoso con memoria\n");
		}
return umc;
}


int conectarConNucleo(){
		int nucleo = conectar_a(config_cpu.IP_NUCLEO, config_cpu.PUERTO_NUCLEO);
		if(nucleo==-1){
			log_info(log,"CPU: No encontre nucleo\n");
			exit (EXIT_FAILURE);
		}
		log_info(log,"CPU: Conecta bien nucleo %d\n", nucleo);
		bool estado = realizar_handshake(nucleo);
		if (!estado) {
			log_info(log,"CPU:Handshake invalido con nucleo\n");
			exit(EXIT_FAILURE);
		}
		else{
			log_info(log,"CPU:Handshake exitoso con nucleo\n");
		}
return nucleo;
}


t_direccion*  crearEstructuraParaUMC (t_pcb* pcb, int tamPag){

	t_direccion* info;
	info->pagina=pcb->indiceDeCodigo [(pcb->pc)*2]/ tamPag;
	info->offset=pcb->indiceDeCodigo [((pcb->pc)*2)];
	info->size=pcb->indiceDeCodigo [((pcb->pc)*2)+1];
	return info;
}


void levantar_configuraciones() {

	t_config * archivo_configuracion = config_create("./CPU.confg");

	config_cpu.PUERTO_NUCLEO = config_get_string_value(archivo_configuracion, "PUERTO_NUCLEO");
	config_cpu.IP_NUCLEO = config_get_string_value(archivo_configuracion, "IP_NUCLEO");
	config_cpu.PUERTO_UMC = config_get_string_value(archivo_configuracion, "PUERTO_UMC");
	config_cpu.IP_UMC = config_get_int_value(archivo_configuracion, "IP_UMC");
	config_cpu.STACK_SIZE = config_get_int_value(archivo_configuracion, "STACK_SIZE");
	config_cpu.SIZE_PAGINA = config_get_int_value(archivo_configuracion,"SIZE_PAGINA");

}


char* depurarSentencia(char* sentencia){

		int i = strlen(sentencia);
		while (string_ends_with(sentencia, "\n")) {
			i--;
			sentencia = string_substring_until(sentencia, i);
		}
		return sentencia;

}
