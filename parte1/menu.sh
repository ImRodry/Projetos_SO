#!/bin/bash
export SHOW_DEBUG=1    ## Comment this line to remove @DEBUG statements

###############################################################################
## ISCTE-IUL: Trabalho prático de Sistemas Operativos 2022/2023
##
## Aluno: Nº: 111008      Nome: Rodrigo Miguel Gomes Amaral Leitão
## Nome do Módulo: menu.sh
## Descrição/Explicação do Módulo: Código explicado nos comentários abaixo
##
##
###############################################################################

# We always want to go back to the menu, which is why this is an infinite loop
while true; do
	# Write the menu on the screen - 5.1.1
	echo "MENU:"
	echo "1: Regista/Atualiza saldo utilizador"
	echo "2: Compra produto"
	echo "3: Reposição de stock"
	echo "4: Estatísticas"
	echo -e "0: Sair\n"

	read -p "Opção: " option
	# Validate the option by checking if it is a number between 0 and 4 and nothing else
	if ! [[ "$option" =~ ^[0-4]$ ]]; then
		./error 5.2.1 $option
	else
		./success 5.2.1 $option
		case "$option" in
		0)
			# Exit the script cleanly, no success/error needed as it wasn't specified
			break
		;;
		1)
			echo "Regista utilizador / Atualiza saldo utilizador:"
			read -p "Indique o nome do utilizador: " user
			read -p "Indique a senha do utilizador: " password
			read -p "Para registar o utilizador, insira o NIF do utilizador: " taxID
			read -p "Indique o saldo a adicionar ao utilizador: " balance
			./regista_utilizador.sh "$user" "$password" "$balance" "$taxID"
			./success 5.2.2.1
		;;
		2)
			./compra.sh
			./success 5.2.2.2
		;;
		3)
			./refill.sh
			./success 5.2.2.3
		;;
		4)
			echo "Estatísticas:"
			echo "1: Listar utilizadores que já fizeram compras"
			echo "2: Listar os produtos mais vendidos"
			echo "3: Histograma de vendas"
			echo -e "0: Voltar ao menu principal\n"

			read -p "Sub-Opção: " subOption
			case "$subOption" in
			0)
				# Do nothing, we just want to go back to the main menu and don't want to fail
			;;
			1)
				./stats.sh listar
			;;
			2)
				echo "Listar os produtos mais vendidos:"
				read -p "Indique o número de produtos mais vendidos a listar: " numProducts
				./stats.sh popular "$numProducts"
			;;
			3)
				./stats.sh histograma
			;;
			*)
				./error 5.2.2.4
			;;
			esac
			# In order to prevent duplicating code, simply check if the option is valid (between 0 and 3) and send success
			if [[ $subOption =~ ^[0-3]$ ]]; then
				./success 5.2.2.4
			fi
		;;
		# A * case is not needed because we already validated the option
		esac
	fi
done
