#!/bin/bash
export SHOW_DEBUG=1    ## Comment this line to remove @DEBUG statements

###############################################################################
## ISCTE-IUL: Trabalho prático de Sistemas Operativos 2022/2023
##
## Aluno: Nº: 111008      Nome: Rodrigo Miguel Gomes Amaral Leitão
## Nome do Módulo: compra.sh
## Descrição/Explicação do Módulo: Código explicado nos comentários abaixo
## 
##
###############################################################################

# Check if produtos.txt and utilizadores.txt both exist - 2.1.1
if [[ -f produtos.txt && -f utilizadores.txt ]]; then
    ./success 2.1.1
else
    ./error 2.1.1
    exit 1
fi

# Reads the file, numbers each line and iterates through them
cat produtos.txt | nl | sed "s/\t/:/g" | sed -E "s/^ +//" | while IFS=':' read -r number name _ price stock _; do
    if (( $stock <= 0 )); then
        ./debug "Skipping product with no stock: $name"
        continue
    fi
    echo "$number: $name: $price EUR"
done

echo -e "0: Sair\n"
# -p is for prompt
read -p "Insira a sua opção: " choice

if [[ $choice == 0 ]]; then
    ./success 2.1.2
    exit 0
    # Use sed to get line $choice in produtos.txt and check if the stock of that line is positive
    # Also we use cat together with wc to get a number output without the file name
elif ! [[ $choice =~ ^[0-9]+$ ]]; then
    ./error 2.1.2
    ./debug "Choice is not a number: $choice"
    exit 2
elif ! (( $(cat produtos.txt | wc -l) >= $choice )); then
    ./error 2.1.2
    ./debug "Invalid line choice: $choice"
    exit 2
elif ! (( $(sed -n ${choice}p produtos.txt | cut -d: -f4) > 0 )); then
    ./error 2.1.2
    ./debug "No stock for chosen option: $choice"
    exit 2
else
    ./success 2.1.2
fi
# This is only declared here to ensure $choice is valid
productEntry=$(sed -n ${choice}p produtos.txt)
./debug "Product entry" "$productEntry"

read -p "Insira o ID do seu utilizador: " id

userEntry=$(grep -E "^$id:" utilizadores.txt)
if [[ -z $userEntry ]]; then
    ./error 2.1.3
    exit 3
else
    ./success 2.1.3 "$(echo $userEntry | cut -d: -f2)"
    ./debug "User entry" "$userEntry"
fi

read -p "Insira a sua senha: " password

if [[ $(echo "$userEntry" | cut -d: -f3) == $password ]]; then
    ./success 2.1.4
else
    ./error 2.1.4
    exit 4
fi

# 2.2
## Compare the balance and the price - 2.2.1
price=$(echo "$productEntry" | cut -d: -f3)
balance=$(echo "$userEntry" | cut -d: -f6)
if (( $balance >= $price )); then
    ./success 2.2.1 "$price" "$balance"
else
    ./error 2.2.1 "$price" "$balance"
    exit 5
fi

## Substract the price from the balance - 2.2.2
newBalance=$(( $balance - $price ))
newUserEntry=$(echo "$userEntry" | sed -E "s/:$balance$/:$newBalance/")
./debug "New user entry: $newUserEntry"
sed -Ei "s/^$userEntry$/$newUserEntry/" utilizadores.txt && ./success 2.2.2 || { ./error 2.2.2; exit 2; }

## Remove 1 unit from stock - 2.2.3
newStock=$(( $(echo "$productEntry" | cut -d: -f4) - 1 ))
newProductEntry=$(echo "$productEntry" | awk -F: -v OFS=: '{ $4=$4-1; print }' )
./debug "New line: $newProductEntry"

sed -Ei "s/^$productEntry$/$newProductEntry/" produtos.txt && ./success 2.2.3 || { ./error 2.2.3; exit 3; }

## Log the transaction - 2.2.4
transactionEntry=$(echo "$newProductEntry" | cut -d: -f1,2):$id:$(date +%F)
./debug "Transaction entry: $transactionEntry"
if echo "$transactionEntry" >> relatorio_compras.txt; then
    ./success 2.2.4
else
    ./error 2.2.4
    exit 4
fi

## Create shopping report for the current user - 2.2.5
echo "**** $( date +%F ): Compras de $( echo $newUserEntry | cut -d: -f2 ) ****" > lista-compras-utilizador.txt || { ./error 2.2.5; exit 1; }
cat relatorio_compras.txt | while IFS=':' read name _ entryId date; do
    ./debug "Name: $name" "Entry ID: $entryId" "Date: $date"
    if [[ $entryId == $id ]]; then
        echo "$name, $date" >> lista-compras-utilizador.txt
    fi
done
