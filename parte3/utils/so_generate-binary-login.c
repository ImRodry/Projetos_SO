/******************************************************************************
 ** ISCTE-IUL: Trabalho prático 2 de Sistemas Operativos
 **
 ** ESTE MÓDULO NÃO É SUPOSTO SER SUBMETIDO PARA AVALIAÇÃO
 ** Nome do Módulo: create_db.c v1
 ** Descrição/Explicação do Módulo:
 **     create_db <text_filename.txt> <text_separator> <binary_filename.dat>
 **     Reads the text file passed as argument, and for each line (record) creates
 **     a binary entity that is written in the binary file passed as argument.
 **
 ******************************************************************************/
#include "common.h"
// #define SO_HIDE_DEBUG   // Uncomment this line to hide all @DEBUG statements
#include "/home/so/reference/so_utils.h"

int main(int argc, char *argv[]) {    // Não é suposto que os alunos alterem nada na função main()
    printf("%sThis program generates a binary file with one or more elements of type Login.\n", FAINT_MAGENTA);
    printf("The elements to be produced shall be read from a text file passed as input (e.g., see example file utilizadores.txt)\n");
    printf("The user also specifies the field separator in the text file (e.g., in utilizadores.txt, the separator is character ':')%s\n", NO_COLOUR);
    if (argc < 3) {
        so_error("Valida Params", "SYNTAX: %s <input_text_filename.txt> <input_text_separator> <output_binary_filename.dat>", argv[0]);
        so_exit_on_error(-1, "Few arguments");
    }

    FILE *fSrcTxt = fopen(argv[1], "r");
    so_exit_on_null(fSrcTxt, "<text_filename.txt>");

    FILE *fDstDat = fopen(argv[3], "w");
    so_exit_on_null(fDstDat, "<binary_filename.dat>");

    char *ifs = argv[2];
    char strSourceRecord[1000];
    char *currentField;
    Login itemDB;
    while (so_fgets(strSourceRecord, sizeof(strSourceRecord), fSrcTxt) != NULL) { // Leitura OK?
        currentField = strtok(strSourceRecord, ifs);
        so_debug("field:%s", currentField);
        so_exit_on_null(currentField, "Error reading a field");
        itemDB.nif = atoi(currentField);

        currentField = strtok(NULL, ifs);
        so_debug("field:%s", currentField);
        so_exit_on_null(currentField, "Error reading a field");
        strcpy(itemDB.senha, currentField);

        currentField = strtok(NULL, ifs);
        so_debug("field:%s", currentField);
        so_exit_on_null(currentField, "Error reading a field");
        strcpy(itemDB.nome, currentField);

        currentField = strtok(NULL, ifs);
        so_debug("field:%s", currentField);
        so_exit_on_null(currentField, "Error reading a field");
        itemDB.saldo = atoi(currentField);

        currentField = strtok(NULL, ifs);
        so_debug("field:%s", currentField);
        so_exit_on_null(currentField, "Error reading a field");
        itemDB.pid_cliente = atoi(currentField);

        currentField = strtok(NULL, ifs);
        so_debug("field:%s", currentField);
        so_exit_on_null(currentField, "Error reading a field");
        itemDB.pid_servidor_dedicado = atoi(currentField);

        currentField = strtok(NULL, ifs);
        if (NULL != currentField) {
            so_exit_on_error(-1, "Record should not have more fields");
        }
        if (fwrite(&itemDB, sizeof(Login), 1, fDstDat) < 1) { // Escrita OK?
            so_exit_on_error(-1, "Write Binary File");
        }
   }
}