/*
 * cpu.c
 *
 *  Created on: 20 de abr. de 2016
 *      Author: Franco Albornoz
 */

#include "funcionesCPU.h"
#define ARCHIVOLOG "CPU.log"

#define CONFIG_CPU "config_cpu"
CONF_CPU *config_cpu;
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




int main(int argc,char **argv) {

	t_datos_kernel kernel_data;
	kernel_data.QUANTUM_SLEEP=3;

	int maxfd = 3;				// Indice de maximo FD

	struct timeval tv;			// Estructura para select()
	tv.tv_sec = 2;
	tv.tv_usec = 500000;

	fd_set readfds,masterfds;	// Estructuras para select()
	FD_ZERO(&readfds);
	FD_ZERO(&masterfds);

	log= log_create(ARCHIVOLOG, "CPU", 0, LOG_LEVEL_INFO);
	log_info(log,"Iniciando CPU\n");

	config_cpu = malloc(sizeof(CONF_CPU));
	get_config_cpu(config_cpu);//Crea y setea el config del cpu

	int nucleo = conectarConNucleo();
	FD_SET(nucleo,&masterfds);	// Se agrega socket a la lista de fds

	int umc = conectarConUmc();
	FD_SET(umc,&masterfds);		// Se agrega socket a la lista de fds

	t_pcb* pcb;					//Declaracion e inicializacion del PCB
	t_paquete* paquete_recibido;
	t_paquete* paquete_posicion_instruccion;

	while(1)
	{
		readfds = masterfds;	// Copio el struct con fds al auxiliar para read
		select(maxfd+1,&readfds,NULL,NULL,&tv);

		if (FD_ISSET(nucleo, &readfds))		// Si el nucleo envio algo
		{
			paquete_recibido = recibirPCB(nucleo);

			if(paquete_recibido->codigo_operacion==304){

				pcb = desserializarPCB(paquete_recibido->data);
				pcb->pc ++;
				log_info(log,"Recibi pcb %d... ejecturando\n",pcb->pid);

				//paquete_posicion_instruccion = enviarInstruccionAMemoria(umc, pcb);

				//FD_CLR(nucleo,&masterfds);
				//analizadorLinea("a = b + c",&primitivas,&primitivas_kernel);

				//void ejecutarInstruccion(....quantium);  hacer esta funcion
								/*al ejecutar: actuliza los valores del programa de umc
												acualiza el pc del pcb
												notifica al nucleo que concluyo el quantium
								*/

				// hacer la funcion que carga los datos del kernel.....(se sacan del archivo de configuracion?)
				ejecutarInstruccion(pcb, umc, kernel_data.QUANTUM_SLEEP);
				//falta avisar al nucleo que termino el quantum
				//liberar_pcb(pcb);   crear esta funcion
				printf("CPU: Cierra\n");
				return EXIT_SUCCESS;
		}


	close(nucleo);
	close(umc);
	}


	close(nucleo);
	close(umc);
	return EXIT_SUCCESS;

}
}

//***************************************FUNCIONES******************************************

void get_config_cpu (CONF_CPU *config_cpu){

	  t_config *fcpu = config_create(CONFIG_CPU);
	  config_cpu->PUERTO_NUCLEO = config_get_int_value(fcpu,"PUERTO_NUCLEO");
	  config_cpu->IP_NUCLEO = config_get_int_value(fcpu,"IP_NUCLEO");

	  config_cpu->PUERTO_UMC = config_get_int_value(fcpu,"PUERTO_UMC");
	  config_cpu->IP_UMC = config_get_int_value(fcpu,"IP_UMC");

	  config_cpu->STACK_SIZE = config_get_int_value(fcpu,"STACK_SIZE");

	  config_cpu->SIZE_PAGINA = config_get_int_value(fcpu, "SIZE_PAGINA");


	  config_destroy(fcpu);

}


int conectarConUmc(){

		int umc = conectar_a(config_cpu->IP_UMC, config_cpu->PUERTO_UMC);

		if(umc==0){
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

		int nucleo = conectar_a(config_cpu->IP_NUCLEO, config_cpu->PUERTO_NUCLEO);

		if(nucleo==0){
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


t_paquete* recibirPCB(int nucleo){

	t_paquete* pcb_recibido= recibir(nucleo);
	pcb_recibido=malloc(sizeof(t_pcb));

return pcb_recibido;
}


t_paquete* crearPaquete(void* data, int codigo, int size)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = codigo;
	paquete->tamanio = size;
	paquete->data = malloc(size);

	memcpy(paquete->data, data, size);
	return paquete;
}


t_paquete* enviarInstruccionAMemoria(int umc, int* indice, int32_t offset, uint32_t tamanio) {

	t_direccion *info = malloc(sizeof(*info)); // esta bien esta medida?

	info->pagina = indice;
	info->offset = offset;
	info->size = tamanio;

	t_paquete* paquete = crearPaquete(info, 403, sizeof(*info));  ///403 o 404

	enviar(umc, 404, paquete->tamanio, paquete->data);

	liberar_paquete(paquete);

	t_paquete* paquete_recibido = recibir(umc);

	if (paquete_recibido->codigo_operacion != 403) { ////403 o 404
		log_debug(log,"Hubo un error de lectura, cierro la ejecución del programa");
	}
	else return paquete_recibido;

	liberar_paquete(paquete_recibido);
	return NULL;
}


//creo que lo que mas dudoso esta es esta arte... revisar la forma de ejecusion..

void ejecutarInstruccion(t_pcb* pcb, int umc, int QUANTUM_SLEEP) {  //el retardo lo saco de nucleo

	uint32_t offset = pcb->pc * sizeof(t_instruccion); // asi se calcula el offset

	t_paquete* infoPaquete = enviarInstruccionAMemoria(umc, pcb->indiceDeCodigo, offset, sizeof(t_instruccion));

	t_instruccion* info= (t_instruccion*) (infoPaquete->data);///// analizar esta estructura
	//buscando instruccion

	t_paquete* instruccionPaquete = enviarInstruccionAMemoria(umc, pcb->indiceDeCodigo, info->start, info->offset);

	char* laInstruccion = malloc(instruccionPaquete->tamanio + 1);
	memcpy(laInstruccion,instruccionPaquete->data,instruccionPaquete->tamanio);
	laInstruccion[instruccionPaquete->tamanio] = '\0';
	// instruccion encontrada

	pcb->pc ++;

	analizadorLinea((char * const ) laInstruccion, &primitivas, &primitivas_kernel);
	free(laInstruccion);

	liberar_paquete(instruccionPaquete);
	liberar_paquete(infoPaquete);

	sleep(QUANTUM_SLEEP); //el programa espere durante un período de tiempo especificado en milisegundos
}

