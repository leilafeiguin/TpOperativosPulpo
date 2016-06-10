/*
 * marcos.c
 *
 *  Created on: 5/6/2016
 *      Author: utnso
 */

#include "marcos.h"

void marco_nuevo(t_entrada_tabla_de_paginas * entrada_que_necesita_marco) {

	if (tiene_cantidad_maxima_marcos_asignados(
			entrada_que_necesita_marco->pid)) {

		algoritmo_remplazo(entrada_que_necesita_marco,
				entrada_que_necesita_marco->pid);

		entrada_que_necesita_marco->presencia = true;

	} else {

		bool marco_disponible(void * entrada) {

			t_control_marco * marco = (t_control_marco *) entrada;

			return marco->disponible;
		}

		t_control_marco * marco_libre = list_find(control_de_marcos,
				marco_disponible);

		marco_libre->disponible = false;

		entrada_que_necesita_marco->marco = marco_libre->numero;

		t_list * lista_clock = lista_circular_clock(tabla_de_paginas,
				entrada_que_necesita_marco->pid);

		if (list_is_empty(lista_clock)) {
			entrada_que_necesita_marco->puntero = true;
		}

		entrada_que_necesita_marco->presencia = true;

	}
}

bool tiene_cantidad_maxima_marcos_asignados(int pid) {

	bool coincide_pid_y_esta_presente(void * elemento) {

		t_entrada_tabla_de_paginas * entrada =
				(t_entrada_tabla_de_paginas *) elemento;

		return entrada->pid == pid && entrada->presencia;

	}

	int marcos_utilizados = list_count_satisfying(tabla_de_paginas,
			coincide_pid_y_esta_presente);

	return marcos_utilizados == cantidad_maxima_marcos;
}

void inicializar_marcos() {

	control_de_marcos = list_create();

	int var;
	for (var = 0; var < cantidad_marcos; ++var) {

		t_control_marco * entrada = malloc(sizeof(t_control_marco));

		entrada->numero = var;
		entrada->disponible = true;

		list_add(control_de_marcos, entrada);

	}
}

void algoritmo_remplazo(t_entrada_tabla_de_paginas * entrada_sin_marco, int pid) {

	t_list * lista_clock = lista_circular_clock(tabla_de_paginas, pid);

	bool buscar_victima_y_modificar_uso(void * elemento) {

		t_entrada_tabla_de_paginas * entrada =
				(t_entrada_tabla_de_paginas *) elemento;

		if (entrada->uso) {

			entrada->uso = false;
			return false;

		} else {

			return true;
		}

	}

	t_entrada_tabla_de_paginas * victima = list_find(lista_clock,
			buscar_victima_y_modificar_uso);

	if (victima == NULL) {

		victima = list_find(lista_clock, buscar_victima_y_modificar_uso);

	}

	if (victima->modificado) {

		swap_escribir(victima);
		victima->modificado = false;
	}

	entrada_sin_marco->marco = victima->marco;

	entrada_sin_marco->uso = true;

	entrada_sin_marco->presencia = true;
	victima->presencia = false;

	avanzar_victima(lista_clock, entrada_sin_marco);

}

t_list * lista_circular_clock(t_list * lista, int pid) {

	bool coincide_pid_y_presencia(void * elemento) {

		t_entrada_tabla_de_paginas * entrada =
				(t_entrada_tabla_de_paginas *) elemento;

		return entrada->pid == pid && entrada->presencia;
	}

	t_list * entradas_presentes_proceso = list_filter(lista,
			coincide_pid_y_presencia);

	bool orden_menor_marco_primero(void * elem1, void * elem2) {

		t_entrada_tabla_de_paginas * entrada1 =
				(t_entrada_tabla_de_paginas *) elem1;

		t_entrada_tabla_de_paginas * entrada2 =
				(t_entrada_tabla_de_paginas *) elem2;

		return entrada1->marco < entrada2->marco;

	}

	if (list_is_empty(entradas_presentes_proceso)) {

		return entradas_presentes_proceso;

	} else {

		list_sort(entradas_presentes_proceso, orden_menor_marco_primero);

		bool es_puntero(void * elemento) {

			t_entrada_tabla_de_paginas * entrada =
					(t_entrada_tabla_de_paginas *) elemento;

			return entrada->puntero;
		}

		t_entrada_tabla_de_paginas * puntero = list_find(
				entradas_presentes_proceso, es_puntero);

		return circular_list_starting_with(entradas_presentes_proceso, puntero);
	}
}

void avanzar_victima(t_list * lista_clock,
		t_entrada_tabla_de_paginas * entrada_con_marco_nuevo) {

	t_entrada_tabla_de_paginas * puntero_viejo = head(lista_clock);

	if (de_una_entrada(lista_clock)) {

		entrada_con_marco_nuevo->puntero = true;

	} else {

		t_entrada_tabla_de_paginas * nuevo_puntero = list_get(lista_clock, 1);

		nuevo_puntero->puntero = true;

	}

	puntero_viejo->puntero = false;

}

bool de_una_entrada(t_list * lista) {

	return list_size(lista) == 1;

}

