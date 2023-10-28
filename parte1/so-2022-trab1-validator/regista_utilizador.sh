#!/bin/bash

# Utilização: ./regista_utilizador.sh <nome> <senha> <saldo> [<nif>]

if [[ $# -ne 3 && $# -ne 4 ]]; then
    echo "[*SO*] erro em regista_utilizador.sh: número de parametros errado: $#"
    i=0
    for v in "$@"; do
        i=$((i+1))
        echo "$i. '$v'"
    done
else
    if [ $# -eq 3 ]; then
        echo "[*SO*] regista_utilizador.sh '$1' '$2' '$3'"
    else
        echo "[*SO*] regista_utilizador.sh '$1' '$2' '$3' '$4'"
    fi
fi

