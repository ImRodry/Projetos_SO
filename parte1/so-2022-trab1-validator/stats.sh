#!/bin/bash

# Utilização: ./stats.sh listar | popular <nr> | histograma"

erro=0
case "$1" in
    listar)
        if [ $# -ne 1 ]; then
            echo "[*SO*] erro em stats.sh listar: número de parametros errado"
            erro=1
        else 
            echo "[*SO*] ok stats.sh listar"
        fi

    ;;
    popular)
        if [ $# -ne 2 ]; then
            echo "[*SO*] erro em stats.sh popular: número de parametros errado"
            erro=1
        else 
            echo "[*SO*] ok stats.sh popular '$2'"
        fi
    ;;
    histograma)
        if [ $# -ne 1 ]; then
            echo "[*SO*] erro em stats.sh histograma: número de parametros errado"
            erro=1
        else 
            echo "[*SO*] ok stats.sh histograma"
        fi

    ;;
    *)
        echo "[*SO*] erro em stats.sh: parâmetros inválidos"
        erro=1
    ;;
esac

if [ "$erro" != "0" ]; then
    i=0
    for v in $@; do
        i=$((i+1))
        echo "[*SO*] $i. '$v'"
    done
fi
