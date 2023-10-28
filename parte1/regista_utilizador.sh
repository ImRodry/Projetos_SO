#!/bin/bash
export SHOW_DEBUG=1    ## Comment this line to remove @DEBUG statements

###############################################################################
## ISCTE-IUL: Trabalho prático de Sistemas Operativos 2022/2023
##
## Aluno: Nº: 111008      Nome: Rodrigo Miguel Gomes Amaral Leitão
## Nome do Módulo: regista_utilizador.sh
## Descrição/Explicação do Módulo: Código explicado nos comentários abaixo
##
##
###############################################################################

name=$1
password=$2
balance=$3
taxID=$4 # Optional

# Validation - 1.1
## Validate the amount of arguments passed (either 3 or 4) - 1.1.1
if [[ "$#" == 3 || "$#" == 4 ]]; then
    ./success 1.1.1
    ./debug "$# parameters"
else
    ./error 1.1.1
    exit 1
fi

## Validate that the name is a valid student - 1.1.2
# First get all the names and remove the commas
# Then use grep with the -x flag to match the entire line against the name
if cut -d":" -f5 /etc/passwd | cut -d, -f1 | grep -xq "$name"; then
    ./success 1.1.2
    ./debug "Name: $name"
else
    ./error 1.1.2
    exit 2
fi

## Validate that balance is a positive integer or 0 - 1.1.3
if [[ $balance =~ ^[0-9]+$ ]]; then
    ./success 1.1.3
    ./debug "Balance: $balance"
else
    ./error 1.1.3
    exit 3
fi

## Validates if the taxID is 9 digits long if present - 1.1.4
if [[ -n "$4" ]]; then
    if [[ $taxID =~ ^[0-9]{9}$ ]]; then
        ./success 1.1.4
        ./debug "TaxID: $taxID"
    else
        ./error 1.1.4
        exit 4
    fi
else
    ./debug "Tax ID not present, skipped check"
fi

# Adding to the database - 1.2
## Checks if utilizadores.txt exists - 1.2.1
if [[ -f utilizadores.txt ]]; then
    ./success 1.2.1
    ./debug "utilizadores.txt exists"
else
    ./error 1.2.1
    ## Tries to create the file and exits if it fails - 1.2.2
    if touch utilizadores.txt; then
        ./success 1.2.2
        ./debug "utilizadores.txt created"
    else
        ./error 1.2.2
        exit 2
    fi
fi

## Checks if the user is already registered - 1.2.3
if cut -d":" -f2 utilizadores.txt | grep -q "$name"; then
    ./success 1.2.3
    
    # Finds the correct DB entry and also validates the password at the same time - 1.3.1
    dbEntry=$(grep -E "^[0-9]+\:$name:$password:" utilizadores.txt)
    if [[ -n $dbEntry ]]; then # If the length is non-zero, the password was correct
        ./success 1.3.1
        ./debug "DB Entry: $dbEntry"
    else
        ./error 1.3.1
        exit 1
    fi
    
    currentBalance=$(echo $dbEntry | cut -d":" -f6)
    newBalance=$(( $currentBalance + $balance ))
    ./debug "New Balance: $newBalance"
    
    # Updates the DB entry with the new balance - 1.3.2
    newDbEntry=$(echo $dbEntry | awk -v newBalance=$newBalance 'BEGIN {FS=OFS=":"} {$NF=newBalance}1')
    if sed -i "s/$dbEntry/$newDbEntry/" utilizadores.txt; then
        ./success 1.3.2 $newBalance
    else
        ./error 1.3.2
        exit 2
    fi
else
    ./error 1.2.3
    
    ## Since we need to register the user, check if taxID was passed - 1.2.4
    if [[ -n "$taxID" ]]; then # Checks if the length of $taxID is non-zero 
        ./success 1.2.4
    else
        ./error 1.2.4
        exit 4
    fi
    
    ## Create the user ID by adding 1 to the last one - 1.2.5
    if [[ -s utilizadores.txt ]]; then # Checks if the size is more than 0
        # Cuts out all of the IDs in the file, then gets the last one and increments it by 1
        userID=$(( $(cut -d: -f1 utilizadores.txt | tail -1) + 1 ))
        ./success 1.2.5 $userID
    else
        ./error 1.2.5
        userID=1
    fi
    
    ## Create the email from the first and last name of the user - 1.2.6
    if (( $(echo "$name" | wc -w) < 2 )); then
        ./error 1.2.6
        exit 6
    fi
    email=$(echo "$name" | awk '{print tolower($1) "." tolower($NF) "@kiosk-iul.pt"}') # joining arguments together with space instead of a comma joins them without spaces - ironic
    ./debug "Email: $email"
    ./success 1.2.6 $email
    
    ## Append the new user data to the DB file - 1.2.7
    newUserEntry="$userID:$name:$password:$email:$taxID:$balance"
    if echo "$newUserEntry" >> ./utilizadores.txt; then
        ./success 1.2.7 $newUserEntry
    else
        ./error 1.2.7
        exit 7
    fi
fi

# List all registered users - 1.4

## Sorts the DB through the 6th key (k6) separated by colons (t:) numerically (n) and in reverse (r)
if sort -t: -k6 -nr utilizadores.txt > saldos-ordenados.txt; then
    ./success 1.4.1
else
    ./error 1.4.1
    exit 4
fi
