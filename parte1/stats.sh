#!/bin/bash
export SHOW_DEBUG=1    ## Comment this line to remove @DEBUG statements

###############################################################################
## ISCTE-IUL: Trabalho prático de Sistemas Operativos 2022/2023
##
## Aluno: Nº: 111008      Nome: Rodrigo Miguel Gomes Amaral Leitão
## Nome do Módulo: stats.sh
## Descrição/Explicação do Módulo: Código explicado nos comentários abaixo
##
##
###############################################################################

# Validations - 4.1
## First check if an argument was passed at all
if [ $# -eq 0 ]; then
	./error 4.1.1
	exit 1
fi
type=$1

# In the case we will validate the arguments and run the respective code
case "$type" in
	listar)
		# This type can only receive 1 argument: the type itself
		if [[ $# -ne 1 ]]; then
			./error 4.1.1
			exit 1
		fi
		if ! [[ -r utilizadores.txt && -r relatorio_compras.txt ]]; then 
			./error 4.2.1
			exit 1
		fi
		tempFile=$(mktemp) # Create a temporary file to hold the unsorted stats
		./debug "Temp file: $tempFile"
		cat utilizadores.txt | while IFS=':' read id name _; do
			# Count the amount of exact matches for the ID
			count=$(cut -d: -f3 relatorio_compras.txt | grep -xc "$id")
			./debug "ID: $id" "Name: $name" "Count: $count"
			if (( $count > 0 )); then
				line="$name: $count "
				if (( $count == 1 )); then
					line+="compra"
				else
					line+="compras"
				fi
				./debug "New line: $line"
				echo "$line" >> $tempFile # Append each line to the end of the file
			fi
		done
		# Sort the file by the second column (the count) in reverse numeric order using : as the field separator
		# Then direct the output to stats.txt and delete the temp file
		cat $tempFile | sort -k2 -n -r -t: > stats.txt && rm $tempFile
		./success 4.2.1
	;;
	popular)
		# This type must receive 2 arguments: the type itself and the amount of products to show
		if [[ $# -ne 2 ]]; then
			./error 4.1.1
			exit 1
		fi
		amount=$2
		if ! [[ "$amount" =~ ^[0-9]+$ ]] || ! [[ -r relatorio_compras.txt ]]; then
			./error 4.2.2
			exit 2
		fi
		# First get the names of all the products bought in the log, sort them alphabetically and get the unique entries while also counting them
		# Then sort them in reverse numeric order (the amount of times they appear)
		# Then get the first $amount lines (-$amount is equivalent to -n $amount)
		popularProducts=$(cut -d: -f1 relatorio_compras.txt | sort | uniq -c | sort -nr | head -$amount)
		./debug "Raw popular products: $popularProducts"
		echo "" > stats.txt # First clean the file
		echo "$popularProducts" | while read count name; do
			./debug "Name: $name" "Count: $count"
			line="$name: $count"
			if (( $count == 1 )); then
				line+=" compra"
			else
				line+=" compras"
			fi
			./debug "Line: $line"
			echo "$line" >> stats.txt # Append each line to the end of the file
		done
		./success 4.2.2
	;;
	histograma)
		# This type can only receive 1 argument: the type itself
		if [[ $# -ne 1 ]]; then
			./error 4.1.1
			exit 1
		fi
		# If it can't read the required file, exit and error
		if ! [[ -r relatorio_compras.txt ]]; then
			./error 4.2.3
			exit 3
		fi
		categoryCounts=$(cut -d: -f2 relatorio_compras.txt | sort | uniq -c)
		echo -n "" > stats.txt # First clean the file
		echo "$categoryCounts" | while read count category; do
			./debug "Category: $category" "Count: $count"
			# Append the name of the category, a colon and a tab and then $count amount of * characters (seq is a sequence from 1 to $count)
			echo -e "$category\t$(printf "*%.0s" $(seq 1 $count))" >> stats.txt
		done
		./success 4.2.3
	;;
	*)
		./error 4.1.1
		exit 1
	;;
esac
