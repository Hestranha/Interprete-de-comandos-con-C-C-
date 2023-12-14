#include <iostream>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>

using namespace std;

int main()
{

    string nuevoPrompt = "\033[1;32mMiniShell> \033[0m"; // Prompt llamado MiniShell - verde
    char entrada[100];                                   // Tamaño de entrada para permitir rutas absolutas más largas

    while (true)
    {

        cout << nuevoPrompt;
        cin.getline(entrada, sizeof(entrada)); // Leemos la entrada completa del usuario

        entrada[strcspn(entrada, "\n")] = '\0'; // Eliminamos el carácter de nueva línea

        if (strcmp(entrada, "salir") == 0)
        {
            break;
        }
        else
        {

            char *args[50];
            int numPalabras = 0;

            // Variables para el manejo de señales
            bool existe_pipe = false, bloqueo = false;
            bool existe_salida = false, existe_entrada = false;
            bool ejecutar_en_segundo_plano = false; // Nuevo flag para ejecutar en segundo plano

            // Para los archivos de SALIDA y ENTRADA
            char *out_file = NULL;
            char *input_file = NULL;

            char *token = strtok(entrada, " ");

            while (token != NULL && bloqueo == false)
            {
                if (strcmp(token, ">") == 0)
                { // Si encontramos > , configuramos la redirección de salida
                    existe_salida = true;
                    token = strtok(NULL, " "); // Obtenemos el nombre del archivo de salida
                    out_file = token;
                }
                else if (strcmp(token, "<") == 0)
                { // Si encontramos > , configuramos la redirección de entrada
                    existe_entrada = true;
                    token = strtok(NULL, " "); // Obtenemos el nombre del archivo de entrada
                    input_file = token;
                }
                else if (strcmp(token, "|") == 0)
                { // Si encontramos | , configuramos el pipe
                    existe_pipe = true;
                    bloqueo = true;
                }
                else if (strcmp(token, "&") == 0)
                {
                    ejecutar_en_segundo_plano = true;
                }
                else
                {
                    args[numPalabras] = token;
                    numPalabras++;
                }
                token = strtok(NULL, " ");
            }

            args[numPalabras] = NULL; // Agregamos Null al final para el EXECVP

            if (numPalabras > 0)
            { // Verificamos que haya al menos una palabra

                int pid;
                int pipe1[2]; //[0] = READ , [1] = WRITE
                if (existe_pipe)
                {
                    pipe(pipe1);
                }

                pid = fork(); // Creamos un proceso hijo

                if (pid == 0)
                { // Código para el proceso hijo

                    if (existe_salida)
                    { // Si hay redirección de salida, abrimos el archivo para escritura
                        freopen(out_file, "w", stdout);
                    }
                    if (existe_entrada)
                    {
                        // Si hay redirección de entrada, abrimos el archivo para lectura
                        freopen(input_file, "r", stdin);
                    }
                    if (existe_pipe)
                    {
                        close(pipe1[0]); // Cerramos la lectura
                        close(1);        // Cerramos la salida estandar
                        dup(pipe1[1]);
                        close(pipe1[1]);
                    }
                    if (ejecutar_en_segundo_plano)
                    {
                        // Configurar el proceso para ejecutarse en segundo plano
                        setpgid(0, 0);
                    }

                    execvp(args[0], args);                  // Ejecutar el comando con argumentos
                    perror("Error al ejecutar el comando"); // En caso de error en execv
                    exit(EXIT_FAILURE);
                }
            }
        }
        return 0;
    }
}