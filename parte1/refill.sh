#!/bin/bash
export SHOW_DEBUG=1    ## Comment this line to remove @DEBUG statements

###############################################################################
## ISCTE-IUL: Trabalho prático de Sistemas Operativos 2022/2023
##
## Aluno: Nº: 111008      Nome: Rodrigo Miguel Gomes Amaral Leitão
## Nome do Módulo: refill.sh
## Descrição/Explicação do Módulo: Código explicado nos comentários abaixo
##
##
###############################################################################

# Validations - 3.1
## Check if produtos.txt and reposicao.txt both exist - 3.1.1
if [[ -f produtos.txt && -f reposicao.txt ]]; then
    ./success 3.1.1
else
    ./error 3.1.1
    exit 1
fi

## Ensure all products in reposicao.txt are valid and have a valid reposition value - 3.1.2
# Iterate through all the lines in the file, check if any has less than 3 fields and/or the third field is not a number and print the product name if so
erroredProduct=$(awk -F: '{ if (NF < 3 || !($3 ~ /^[0-9]+$/)) print $1 }' reposicao.txt)
if [[ -n $erroredProduct ]]; then # Error if the length is non-zero
	./error 3.1.2 $erroredProduct
	exit 1
else
	./success 3.1.2
fi

# Preparations - 3.2
## Create a file with the missing products - 3.2.1
# Here we're using awk to iterate through every line
# and check if the max stock - the current stock is positive and if so, write a new line
# The new line is made up of the product name, the difference between the max stock and the current stock and the word "unidades" or "unidade" depending on the difference
missingProducts=$(awk -F: '{ if ($5 - $4 > 0) printf "%s: %d %s\n", $1, $5 - $4, ($5 - $4 == 1 ? "unidade" : "unidades") }' produtos.txt)
if [[ -n $missingProducts ]]; then
	echo -e "**** Produtos em falta em $(date +%F) ****\n$missingProducts" > produtos-em-falta.txt
	./success 3.2.1
else 
	./error 3.2.1
	exit 1
fi

## Go through each line, grab the name of the product and amount of units to add and add them to the stock - 3.2.2
cat reposicao.txt | while IFS=':' read name _ amount; do
	# Pass both name and amount as variables and update the line in which the product name matches, while making sure the stock doesn't go above the maximum stock
	newProductEntry=$(awk -F: -v OFS=':' '{ if ($1 == "'"$name"'") { $4 = ($4 + "'"$amount"'" >= $5) ? $5 : $4 + "'"$amount"'"; print }  }' produtos.txt)
	./debug "New product entry: $newProductEntry" "Name: $name" "Amount: $amount"
	sed -Ei "/^$name:/c\\$newProductEntry" produtos.txt && ./success 3.2.2 || { ./error 3.2.2; exit 2; }
done
