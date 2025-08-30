#include "server.h"

int main(void) {
	logger = log_create("log.log", "Servidor", 1, LOG_LEVEL_DEBUG);

	int server_fd = iniciar_servidor();
	if (server_fd == -1) {
        log_error(logger, "Error al iniciar el servidor.");
        return EXIT_FAILURE;
    }
	
	log_info(logger, "Servidor listo para recibir al cliente");

	
	while (1) {
		int cliente_fd = esperar_cliente(server_fd);
		
		if (cliente_fd == -1) {
            log_error(logger, "Error al aceptar una nueva conexion. El servidor continuara escuchando.");
            continue; // Vuelve a esperar una nueva conexion
        }
		
		t_list* lista;
		int cod_op;
		do {
            cod_op = recibir_operacion(cliente_fd);
            switch (cod_op) {
                case MENSAJE:
                    recibir_mensaje(cliente_fd);
                    break;
                case PAQUETE:
                    lista = recibir_paquete(cliente_fd);
                    log_info(logger, "Me llegaron los siguientes valores:\n");
                    // Itera y loguea cada valor del paquete
                    list_iterate(lista, (void*) iterator);
                    // Libera la memoria de la lista y sus elementos
                    list_destroy_and_destroy_elements(lista, (void*) free);
                    break;
                case -1:
                    log_warning(logger, "El cliente se desconecto. El servidor seguira escuchando.");
                    close(cliente_fd); // Cierra el socket del cliente
                    break;
                default:
                    log_warning(logger, "Operacion desconocida. No quieras meter la pata");
                    break;
            }
        } while (cod_op != -1); // Sale del bucle cuando el cliente se desconecta
    }

    // El servidor nunca llegara a este punto en este bucle infinito
    return EXIT_SUCCESS;
}

void iterator(char* value) {
	log_info(logger,"%s", value);
}
