/*
 * primitivas.h
 *
 *  Created on: 26/4/2016
 *      Author: Franco Albornoz
 */
#include <parser/parser.h>
#ifndef PRIMITIVAS_H_
#define PRIMITIVAS_H_

t_puntero definirVariable(t_nombre_variable identificador_variable);
t_puntero obtenerPosicionVariable (t_nombre_variable identificador_variable);
t_valor_variable dereferenciar(t_puntero direccion_variable);
void asignar(t_puntero direccion_variable,t_valor_variable valor);
t_valor_variable obtenerValorCompartida(t_nombre_compartida variable);
t_valor_variable asignarValorCompartida(t_nombre_compartida variable,t_valor_variable valor);
t_puntero_instruccion irAlLabel(t_nombre_etiqueta etiqueta);
t_puntero_instruccion llamarFuncion(t_nombre_etiqueta etiqueta,t_puntero donde_retornar,t_puntero_instruccion linea_en_ejecucion);
t_puntero_instruccion retornar(t_valor_variable retorno);
int imprimir(t_valor_variable valor_mostrar);
int imprimirTexto(char*texto);
int entradaSalida(t_nombre_dispositivo dispositivo,int tiempo);
int wait(t_nombre_semaforo identificador_semaforo);
int signal(t_nombre_semaforo identificador_semaforo);

#endif /* PRIMITIVAS_H_ */