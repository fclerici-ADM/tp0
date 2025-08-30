#include <stdio.h>
#include "client.h"
#include <commons/log.h>
#include <commons/config.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include <readline/readline.h> // Necesitas la biblioteca readline para leer la entrada

t_config* config;
t_log* logger;


int main(void) {
    
	char* ip;
	char* puerto;

    /* ---------------- LOGGING ---------------- */

	logger = iniciar_logger();
	log_info(logger, "Hola! Soy un log");


	/* ---------------- ARCHIVOS DE CONFIGURACION ---------------- */

	config = iniciar_config();

	// Usando el config creado previamente, leemos los valores del config y los 
	// dejamos en las variables 'ip', 'puerto' y 'valor'
	ip = config_get_string_value(config, "IP");
	puerto = config_get_string_value(config, "PUERTO");
	
    // Loggeamos el valor de config
	log_info(logger, "Lei la IP %s y el PUERTO %s\n", ip, puerto);


	// 1. Crear la conexión con el servidor
    int socket_cliente = crear_conexion(ip, puerto);

    if (socket_cliente == -1) {
        log_info(logger,"Error: no se pudo conectar al servidor.\n");
        return EXIT_FAILURE;
    }

    log_info(logger,"Conectado al servidor. Escribe el mensaje a enviar y presiona Enter.\n");
    log_info(logger,"Escribe 'fin' para enviar el paquete y terminar la conexión.\n");

    // 2. Crear un paquete vacío
    t_paquete* mi_paquete = crear_paquete();
    
    char* linea_leida;

    // 3. Leer de la consola línea por línea y agregar al paquete
    linea_leida = readline("> ");
    while (strcmp(linea_leida, "fin") != 0) {
        // Agrega cada línea leída al paquete. El +1 es para el caracter nulo '\0'.
        agregar_a_paquete(mi_paquete, linea_leida, strlen(linea_leida) + 1);

        // Libera la memoria de la línea leída por readline()
        free(linea_leida);

        // Lee la siguiente línea
        linea_leida = readline("> ");
    }

    // 4. Enviar el paquete completo una vez que se escribió "fin"
    log_info(logger,"Enviando paquete... \n");
    enviar_paquete(mi_paquete, socket_cliente);

    // 5. Liberar la memoria del paquete y la línea leída
    free(linea_leida);
    eliminar_paquete(mi_paquete);
    
    // 6. Cerrar la conexión
    liberar_conexion(socket_cliente);

    log_info(logger,"Conexión cerrada. Cliente terminado.\n");

    return EXIT_SUCCESS;
}

t_log* iniciar_logger(void)
{
	// El primer parámetro es el path del archivo de log, el segundo el nombre para mostrar en consola.
	// El tercer parámetro es si querés que se muestre en consola, y el último el nivel mínimo de log.
	t_log* nuevo_logger = log_create("cliente.log", "CLIENTE_LOG", true, LOG_LEVEL_INFO);

	return nuevo_logger;
}

t_config* iniciar_config(void)
{
	// El único parámetro es el path del archivo de configuración.
	t_config* nuevo_config = config_create("cliente.config");
    if (nuevo_config == NULL) {
        // ¡Importante! Manejar el error si no se encuentra el archivo.
        log_info(logger,"¡Error! No se pudo encontrar el archivo de configuración.\n");
        exit(1);
    }
	return nuevo_config;
}
