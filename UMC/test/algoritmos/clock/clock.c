/*
 * clock.c
 *
 *  Created on: 8/6/2016
 *      Author: utnso
 */

#include "clock.h"

context(algoritmo_clock) {

	describe("Algoritmo CLOCK") {

		before {
			log = log_create(ARCHIVOLOG, "UMC", 0, LOG_LEVEL_INFO);
			tabla_de_paginas = list_create();
			algoritmo = "CLOCK";

		}end

		after {
			list_destroy(tabla_de_paginas);
			log_destroy(log);
		}end

		void nueva_entrada(int pid, int pagina) {

			t_entrada_tabla_de_paginas * entrada = crear_nueva_entrada(pid,pagina);
			list_add(tabla_de_paginas,entrada);
		}

		it("Reemplaza acorde a lo esperado") {

			nueva_entrada(2,0);

			t_entrada_tabla_de_paginas * entrada_0 = crear_nueva_entrada(1,0);
			entrada_0->marco = 0;
			entrada_0->presencia = true;
			entrada_0->puntero = false;

			t_entrada_tabla_de_paginas * entrada_1 = crear_nueva_entrada(1,1);
			entrada_1->marco = 1;
			entrada_1->presencia = true;
			entrada_1->puntero = true;

			t_entrada_tabla_de_paginas * entrada_2 = crear_nueva_entrada(1,2);
			entrada_2->uso = false;

			list_add(tabla_de_paginas,entrada_0);
			list_add(tabla_de_paginas,entrada_1);
			list_add(tabla_de_paginas,entrada_2);

			algoritmo_remplazo(entrada_2,1);

			should_int(entrada_0->marco) be equal to(0);
			should_bool(entrada_0->uso) be equal to(false);
			should_bool(entrada_0->presencia) be equal to(true);
			should_bool(entrada_0->puntero) be equal to(true);

			should_int(entrada_1->marco) be equal to(1);
			should_bool(entrada_1->uso) be equal to(false);
			should_bool(entrada_1->presencia) be equal to(false);
			should_bool(entrada_1->puntero) be equal to(false);

			should_int(entrada_2->marco) be equal to(1);
			should_bool(entrada_2->uso) be equal to(true);
			should_bool(entrada_2->presencia) be equal to(true);
			should_bool(entrada_2->puntero) be equal to(false);

			algoritmo_remplazo(entrada_1,1);

			should_int(entrada_0->marco) be equal to(0);
			should_bool(entrada_0->uso) be equal to(false);
			should_bool(entrada_0->presencia) be equal to(false);
			should_bool(entrada_0->puntero) be equal to(false);

			should_int(entrada_1->marco) be equal to(0);
			should_bool(entrada_1->uso) be equal to(true);
			should_bool(entrada_1->presencia) be equal to(true);
			should_bool(entrada_1->puntero) be equal to(false);

			should_int(entrada_2->marco) be equal to(1);
			should_bool(entrada_2->uso) be equal to(true);
			should_bool(entrada_2->presencia) be equal to(true);
			should_bool(entrada_2->puntero) be equal to(true);

			free(entrada_0);
			free(entrada_1);
			free(entrada_2);

		}end

		it("Caso especial") {

			t_entrada_tabla_de_paginas * entrada_0 = crear_nueva_entrada(1,0);
			entrada_0->marco = 0;
			entrada_0->presencia = true;
			entrada_0->puntero = true;

			t_entrada_tabla_de_paginas * entrada_1 = crear_nueva_entrada(1,1);
			entrada_1->marco = 1;
			entrada_1->presencia = true;
			entrada_1->puntero = false;
			entrada_1->uso = false;

			t_entrada_tabla_de_paginas * entrada_2 = crear_nueva_entrada(1,2);
			entrada_2->marco = 2;
			entrada_2->presencia = true;
			entrada_2->puntero = false;

			t_entrada_tabla_de_paginas * entrada_3 = crear_nueva_entrada(1,3);
			entrada_3->presencia = false;
			entrada_3->puntero = false;

			list_add(tabla_de_paginas,entrada_0);
			list_add(tabla_de_paginas,entrada_1);
			list_add(tabla_de_paginas,entrada_2);
			list_add(tabla_de_paginas,entrada_3);

			algoritmo_remplazo(entrada_3,1);

			should_int(entrada_0->marco) be equal to(0);
			should_bool(entrada_0->uso) be equal to(false);
			should_bool(entrada_0->presencia) be equal to(true);
			should_bool(entrada_0->puntero) be equal to(false);

			should_int(entrada_1->marco) be equal to(1);
			should_bool(entrada_1->uso) be equal to(false);
			should_bool(entrada_1->presencia) be equal to(false);
			should_bool(entrada_1->puntero) be equal to(false);

			should_int(entrada_2->marco) be equal to(2);
			should_bool(entrada_2->uso) be equal to(true);
			should_bool(entrada_2->presencia) be equal to(true);
			should_bool(entrada_2->puntero) be equal to(true);

			should_int(entrada_3->marco) be equal to(1);
			should_bool(entrada_3->uso) be equal to(true);
			should_bool(entrada_3->presencia) be equal to(true);
			should_bool(entrada_3->puntero) be equal to(false);

			free(entrada_0);
			free(entrada_1);
			free(entrada_2);
			free(entrada_3);
		}end

	}end

}
