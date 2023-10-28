#!/bin/bash
. /home/so/reference/so_utils.sh
echo -e "${FAINT_MAGENTA}This program displays on STDOUT a binary file with one or more elements of type Login.${NO_COLOUR}"
(($# < 1)) && so_error "Valida Params" "SYNTAX: $0 <input_binary_filename.dat>" && exit

# This script displays binary files.
# These files all need to have elements of the same type. In this case, the type is:
# typedef struct {
#     int nif;                      // Número de contribuinte do utilizador
#     char senha[20];               // Senha do utilizador
#     char nome[52];                // Nome do utilizador
#     int saldo;                    // Saldo do utilizador
#     int pid_cliente;              // PID do processo Cliente
#     int pid_servidor_dedicado;    // PID do processo Servidor Dedicado
# } Login;

# This tool then shows in the STDOUT the information:
#            | nif|  senha  |   nome  | saldo | pid_cliente | pid_servidor_dedicado |
hexdump -e '"| %d | %-9.20s | %-24.52s | %3d | %2d | %2d |\n"' $1