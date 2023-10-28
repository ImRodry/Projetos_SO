#!/bin/bash
. /home/so/reference/so_utils.sh
echo -e "${FAINT_MAGENTA}This program displays on STDOUT a binary file with one or more elements of type Produto.${NO_COLOUR}"
(($# < 1)) && so_error "Valida Params" "SYNTAX: $0 <input_binary_filename.dat>" && exit

# This script displays binary files.
# These files all need to have elements of the same type. In this case, the type is:
# typedef struct {
#     int idProduto;                  // Identificador único do produto
#     char nomeProduto[40];           // Nome do Produto
#     char categoria[40];             // Categoria do Produto
#     int preco;                      // Preço do Produto
#     int stock;                      // Stock do Produto
# } Produto;

# This tool then shows in the STDOUT the information:
#            | idP| nomePro | categor | preco | stock |
hexdump -e '"| %d | %-20.40s | %-7.40s | %d | %2d |\n"' $1