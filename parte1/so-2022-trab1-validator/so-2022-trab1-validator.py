#!/usr/bin/env python3

##########################################################################
# ISCTE-IUL: 1.º - Trabalho prático de Sistemas Operativos 2022/2023
#
# Avaliação automática de 1 trabalho
#
# Versão 0.3.2
##########################################################################

import subprocess
import os
import sys
import argparse
import datetime
import re
import glob
# import pandas as pd

stoponerror = False
##########################################################################
# Test data
#
##########################################################################
utilizadores = \
"1:Paulo Pereira:fsd423erddew:paulo.pereira@kiosk-iul.pt:235123532:10", \
"2:Catarina Cruz:12qwaszx:catarina.cruz@kiosk-iul.pt:234580880:50" , \
"3:Joao Baptista Goncalves:09polkmn:joao.goncalves@kiosk-iul.pt:215654377:20", \
"4:José Silva:abc123:jose.silva@kiosk-iul.pt:123456789:0"

produtos = \
"Croissant chocolate:Bolo:3:5:7", \
"Batatas fritas:Snack:2:2:10", \
"Cola:Bebida:3:10:10", \
"Sumo:Bebida:2:0:20", \
"Laranja:Fruta:1:0:10", \
"Banana:Fruta:1:0:5"

compras = \
"Croissant chocolate:Bolo:2:2023-02-25", \
"Sumo:Bebida:2:2023-03-01", \
"Sumo:Bebida:2:2023-03-02", \
"Batatas fritas:Snack:3:2023-02-21", \
"Batatas fritas:Snack:2:2023-03-03", \
"Sumo:Bebida:3:2023-02-23"

reposicao = \
"Croissant chocolate:Bolo:15", \
"Batatas fritas:Snack:2", \
"Cola:Bebida:5", \
"Sumo:Bebida:12", \
"Laranja:Fruta:6", \
"Banana:Fruta:8"

##########################################################################
# List of questions
#
###########################################################################
enunciado = {
    "1.1.1a":"Falha com 0 argumentos",
    "1.1.1b":"Falha com 1 argumentos",
    "1.1.1c":"Falha com 2 argumentos",
    "1.1.1d":"Falha com 5 argumentos",
    "1.1.2a":"Falha com utilizador inválido",
    "1.1.2b":"Falha com utilizador inválido",
    "1.1.3":"Falha com saldo inválido",
    "1.1.4":"Falha com NIF inválido",
    "1.2.1":"Detecta 'utilizadores.txt' em falta",
    "1.2.2":"Cria 'utilizadores.txt' se necessário ",
    "1.2.3":"Detecta novo utilizador",
    "1.2.4":"Falha com novo utilizador sem NIF",
    "1.2.5":"Gera novo ID",
    "1.2.6":"Gera email",
    "1.2.7":"Acrescenta novo utilizador",
    "1.2.7b":"Ficheiro correcto",
    "1.3.1":"Falha com senha errada",
    "1.3.2a":"Calcula novo saldo",
    "1.3.2b":"Actualiza 'utilizadores.txt' correctamente",
    "1.4.1":"Cria ficheiro 'saldos-ordenados.txt'",

    "2.1.1a":"Verifica se produtos.txt existe",
    "2.1.1b":"Verifica se utilizadores.txt existe",
    "2.1.2a":"Prepara menu e sai com opção 0",
    "2.1.2b":"Falha com produto inexistente",
    "2.1.3":"Falha com utilizador inválido",
    "2.1.4":"Falha com senha inválida",
    "2.2.1a":"Falha por falta de saldo",
    "2.2.1b":"Se Falhou por falta de saldo, valida se nº produtos não foi decrementado",
    "2.2.1c":"Utilizador e senha corretos, e o utilizador tem saldo",
    "2.2.2":"Actualiza 'utilizadores.txt' correctamente",
    "2.2.3":"Actualiza ficheiro 'produtos.txt' correctamente",
    "2.2.4":"Actualiza ficheiro 'relatorio_compras.txt' correctamente",
    "2.2.5":"Gera ficheiro 'lista-compras-utilizador.txt' correctamente",

    "3.1.1a":"Verifica se produtos.txt existe",
    "3.1.1b":"Verifica se reposicao.txt existe",
    "3.1.2a":"Falha com produtos inválidos",
    "3.1.2b":"Falha com nos. produtos inválidos",
    "3.1.2c":"Falha com nos. produtos <= 0",

    "3.2.1a":"Cria ficheiro 'produtos-em-falta.txt'",
    "3.2.1b":"Ficheiro 'produtos-em-falta.txt' correcto",

    "3.2.2a":"Actualiza o ficheiro 'produtos.txt'",
    "3.2.2b":"Ficheiro 'produtos.txt' correcto",
    "3.3.1":"Ficheiro 'cron.def' correcto",

    "4.1.1a":"Falha com 0 argumentos",
    "4.1.1b":"Falha com excesso de argumentos",
    "4.1.1c":"Falha com excesso de argumentos",
    "4.1.1d":"Falha com excesso de argumentos",
    "4.1.1e":"Falha com argumento inválido",
    "4.1.1f":"Falha com opção popular sem parâmetro adicional",
    "4.2.1a":"Opção listar correcta",
    "4.2.1b":"Opção listar: Ficheiro 'stats.txt' correcto",
    "4.2.2a":"Opção popular correcta",
    "4.2.2b":"Opção popular: Ficheiro 'stats.txt' correcto",
    "4.2.3a":"Opção histograma correcta",
    "4.2.3b":"Opção histograma: Ficheiro 'stats.txt' correcto",

    "5.1":"Sai do menu com 0",
    "5.2":"Chama regista_utilizador.sh correctamente",
    "5.3":"Chama compra.sh correctamente",
    "5.4":"Chama refill.sh correctamente",
    "5.5.1":"Chama stats.sh listar corretamente",
    "5.5.2":"Chama stats.sh popular corretamente",
    "5.5.3":"Chama stats.sh histograma corretamente",
}

grade = { }

userErrorLog = []

##########################################################################
# Utility functions
#
###########################################################################

if ( sys.stdout.encoding == "UTF-8" or sys.stdout.encoding == "utf-8" ):
  ok   = u"\u001b[32m✔\u001b[0m"
  fail = u"\u001b[31m✗\u001b[0m"
  warn = u"\u001b[33m⚠️\u001b[0m"
else:
  ok = "\u001b[32m[ ok ]\u001b[0m"
  fail = "\u001b[31m[fail]\u001b[0m"
  warn = "\u001b[33m[warn]\u001b[0m"

def create_file( filename, text ):
    """
    Creates a test file from a list of strings

    Parameters
    ----------
    filename : str
        Name of file to write, will be overriten
    text : list of str
        List of strings to write
    """
    f = open( filename, 'w' )
    for line in text:
        print( line, file = f )
    f.close()

def compare_files( fileA, fileB, skip_blanks = True, verbose = False, trim = True, respectOrder = True ):
    """
    Compares two text files, optionally skippping blank lines

    Parameters
    ----------
    fileA : str
        Filename of file A
    fileB : str
        Filename of file B
    skip_blanks : logical [optional]
        If set to True (default) blank lines will be skipped in the comparison
    trim : logical [optional]
        If set to True (default) lines will be trimmed (remove leading and
        trailing blanks) before comparison
    respectOrder : logical [optional]
        If set to True (default) lines will be compared in the same order on both files
        If set to false, the comparison is performed checking if all lines of fileA exist
        in fileB (in any order) and if all lines of fileB exist in fileA (in any order)
    verbose : logical [optional]
        If set to True prints out all different lines between the files. The
        default is not to print different lines.

    Returns
    -------
    compare_files : logical
        Returns true if the two files match, false otherwise
    """
    if ( not os.path.exists(fileA) ):
        print("(*error*) compare_files: file A '{}' not found".format(fileA) )
        return False

    if ( not os.path.exists(fileB) ):
        print("(*error*) compare_files: file B '{}' not found".format(fileB) )
        return False

    with open( fileA, 'r' ) as f:
        dataA = f.read().splitlines()

    with open( fileB, 'r' ) as f:
        dataB =  f.read().splitlines()

    if ( trim ):
        dataA = [ s.strip() for s in dataA ]
        dataB = [ s.strip() for s in dataB ]

    if ( skip_blanks ):
        dataA = [ s for s in dataA if s != '' ]
        dataB = [ s for s in dataB if s != '' ]

    equalContent = False
    equalNrLines = ( len(dataA) == len(dataB) )
    if equalNrLines:
        if ( respectOrder ):
            equalContent = ( dataA == dataB )
            if ( not equalContent and verbose ):
                i = 0
                while ( (i < len(dataA)) or (i < len(dataB) ) ):
                    if ( (i < len(dataA) ) and (i < len(dataB)) ):
                        if ( dataA[i] != dataB[i] ):
                            print('\n{:>30}:{} - {}'.format( fileA, i+1, dataA[i]) )
                            print('{:>30}:{} - {}'.format( fileB, i+1, dataB[i]) )
                    elif ( i < len(dataA) ):
                        print('\n{:>}:{} - {}'.format( fileA, i+1, dataA[i]) )
                        print('{:>30}:{} - '.format( fileB, i+1 ) )
                    else:
                        print('\n{:>30}:{} - '.format( fileA, i+1 ) )
                        print('{:>30}:{} - {}'.format( fileB, i+1, dataB[i]) )
                    i = i + 1
        else:
            i = 1
            for lineA in dataA:
                equalContent = False
                for lineB in dataB:
                    if ( lineA == lineB ):
                        equalContent = True
                        break
                if ( not equalContent ):
                    if ( verbose ):
                        print('{}:{} - {} not found in {}'.format( fileA, i, lineA, fileB) )
                    break
                i = i + 1
            if ( equalContent ):
                i = 1
                for lineB in dataB:
                    equalContent = False
                    for lineA in dataA:
                        if ( lineA == lineB ):
                            equalContent = True
                            break
                    if ( not equalContent ):
                        if ( verbose ):
                            print('{}:{} - {} not found in {}'.format( fileB, i, lineB, fileA) )
                        break
                    i = i + 1

    if ( not equalContent ):
        print("\n[{}] Files {} and {} don't match".format(fail,fileA, fileB))

        if ( verbose ):
            print('Got file {}:'.format( fileA ) )
            i = 1
            for line in dataA:
                print('   {:>3}: {}'.format( i, line ) )
                i = i + 1
            print('Expected file {}:'.format( fileB ) )
            i = 1
            for line in dataB:
                print('   {:>3}: {}'.format( i, line ) )
                i = i + 1
    return equalContent


def compare_lists( listA, listB, skip_blanks = True, verbose = False, exact = True, trim = True ):
    """
    Compares two list of strings. The comparison may eliminate leading and
    trailing blanks and/or empty strings before comparison. The order of
    elements in the lists is not considered (i.e. lists are sorted before
    comparion). The comparison may consider either equal elements (A == B)
    or the presence of element A in string B (A in B).

    Parameters
    ----------
    listA : list of str
        List A
    listB : list of str
        List B
    exact : logical [optional]
        If set to True (default) the routine will check if the elements in list
        A match exactly the elements in list B. Otherwise the routine will check
        if the elements of list A are present (in) the elements of list B
    skip_blanks : logical [optional]
        If set to True (default) blank elements will be skipped in the comparison
    trim : logical [optional]
        If set to True (default) elements will be trimmed (remove leading and
        trailing blanks) before comparison
    verbose : logical [optional]
        If set to True prints and the lists are different the routine will print
        the two lists. The default is never to print the lists.

    Returns
    -------
    compare_lists : logical
        Returns true if the two lists match, false otherwise
    """

    dataA = listA.copy()
    dataB = listB.copy()

    if ( trim ):
        dataA = [ s.strip() for s in dataA ]
        dataB = [ s.strip() for s in dataB ]

    if ( skip_blanks ):
        dataA = [ s for s in dataA if s != '' ]
        dataB = [ s for s in dataB if s != '' ]

    if ( len(dataA) == len(dataB)):
        dataA.sort()
        dataB.sort()
        if ( exact ):
            equal = ( dataA == dataB )
        else:
            equal = True
            i = 0
            for b in dataB:
                if ( not dataA[i] in b):
                    equal = False
                    break
                i = i + 1
    else:
        equal = False

    if ( not equal ):
        print( "\n{} Lists don't match".format(fail))

        if ( verbose ):
            print( "Expected: ", listA )
            print( "Got:      ", listB )
            print()

    return equal

def cleanup( files ):
    """Deletes file in list

    Parameters
    ----------
    files : list of str | str
        File or List of files to be deleted
    """
    if ( isinstance( files, str ) ):
        if ( os.path.exists(files) ):
            os.remove( files )
    else:
        for f in files:
            if ( os.path.exists(f) ):
                os.remove( f )

def eval_msg( testPassed, key, test = None ):
    """Prints evaluation message

    Parameters
    ----------
    testPassed : logical
        Set to true if test is working properly
    key : str
        Question number
    test : str or list of str [optional]
        Description of test to print in case of failure
    """
    if ( testPassed ):
        print('[{}] {}\t{}'.format( ok, key, enunciado[key] ))
    else:
        if ( test ):
            if ( isinstance( test, str ) ):
                msg = test
            else:
                msg = ' '.join(test)
            print('[{}] Test was <{}>\t{}'.format( fail, key, msg ))
        print('[{}] {}\t{}'.format( fail, key, enunciado[key] ))
        if ( stoponerror ):
            print('Testing stopped because --stoponerror was specified' )
            sys.exit() # Temporário para parar a cada erro

def run_process( scr, input = None, timeout = 1, printerror = True ):
    """
    Runs a process and returns the associated CompletedProcess object

    The process has a maximum time of `timeout` seconds to execute, otherwise
    the command returns False.

    Parameters
    ----------
    scr : list
        Command to be run. Actual command invocation is the first list
        member, additional command line parameters are set in the
        remainder list elements
    input : list [optional]
        Input text to be sent to stdin, defaults to None
    timeout : int [optional]
        Timeout time, defaults to 1 (seconds)
    printerror : boolean [optional]
        The routine will store (extend) stderr output to variable userErrorLog
        If true, this will also print stderr output with a warning message

    Returns
    -------
    process : CompletedProcess | False
        Returns the associated CompletedProcess object on success, and
        False on timeout.
    """
    try:
        proc = subprocess.run( scr, input = input, stdout=subprocess.PIPE, stderr=subprocess.PIPE, timeout=timeout )
    except subprocess.TimeoutExpired:
        print( "{}\t{}\t(*error*) Timeout after {} second(s)".format(fail, scr[0],timeout) )
    except OSError as err:
        print( "{}\t{}\t (*error*) {}".format(fail, scr[0], err.strerror ) )
        if ( err.errno == 8 ):
            print( "{}\t{}\t (*error*) Possibly missing she-bang (e.g. #!/bin/bash)".format(fail, scr[0] ) )
    except Exception as inst:
        print( "{}\t{}\t (*error*) {}".format(fail, scr[0], type(inst) ) )
    else:
        if ( proc.stderr ):
            userErrorLog.clear()
            userErrorLog.extend( proc.stderr.decode("utf-8").split('\n') )
            if (printerror):
                error = proc.stderr.decode("utf-8").split('\n')
                if ( error[0] ):
                    print("\n{}  Output on user stderr from <{}>:".format(warn,' '.join(scr)))
                    for l in error:
                        if (l):
                            print( "{}\t{}".format(warn,l ))
                    print()
        return proc
    return False

def check_proc_out( scr, output, input = None, timeout = 1, printerror = True, verbose = True ):
    """
    Runs a process and checks if the stdout output matches the supplied values

    The process has a maximum time of `timeout` seconds to execute, otherwise
    the command returns False.

    Parameters
    ----------
    scr : list
        Command to be run. Actual command invocation is the first list
        member, additional command line parameters are set in the
        remainder list elements
    output : str
        Expected output
    input : list [optional]
        Input text to be sent to stdin, defaults to None
    timeout : int [optional]
        Timeout time, defaults to 1 (seconds)
    printerror : boolean [optional]
        The routine will store (extend) stderr output to variable userErrorLog
        If true, this will also print stderr output with a warning message

    Returns
    -------
    check : logical
        Returns true if the required `output` is in the process stdout
    """

    try:
        proc = subprocess.run( scr, input = input, stdout=subprocess.PIPE, stderr=subprocess.PIPE, timeout=timeout )

        if ( proc.stderr ):
            userErrorLog.clear()
            userErrorLog.extend( proc.stderr.decode("utf-8").split('\n') )
            if (printerror):
                error = proc.stderr.decode("utf-8").split('\n')
                if ( error[0] ):
                    print("\n{}  Output on user stderr from <{}>:".format(warn,' '.join(scr)))
                    for l in error:
                        if (l):
                            print( "{}\t{}".format(warn,l ))
                    print()

        if ( output in proc.stdout ):
            # print( "[{}]\t{}\t(*success*) got {} expected {}".format(ok,scr[0], proc.stdout, output) )
            result = True
        else:
            print( "[{}]\t{}\t(*error*) got {} expected {}".format(fail,scr[0], proc.stdout, output) )
            result = False

    except subprocess.TimeoutExpired:
        print( "[{}]\t{}\t(*error*) Timeout after {} second(s)".format(fail, scr[0],timeout) )
        result = False

    except OSError as err:
        print( "{}\t{}\t (*error*) {}".format(fail, scr[0], err.strerror ) )
        if ( err.errno == 8 ):
            print( "{}\t{}\t (*error*) Possibly missing she-bang (e.g. #!/bin/bash)".format(fail, scr[0] ) )
        result = False

    return result



##########################################################################
# Crete Input files from test data
#
###########################################################################

def create_test_files():
    """Creates test files
    """
    create_file( 'utilizadores.txt', utilizadores )
    create_file( 'produtos.txt', produtos )
    create_file( 'reposicao.txt', reposicao )

def cleanup_test_files():
    """Removes test files
    """
    cleanup ( ['utilizadores.txt','produtos.txt','reposicao.txt'])

##########################################################################
# Parte 1 - regista_utilizador.sh
#
##########################################################################

def test_regista_utilizador( t ) :
    """Testa o script 'regista_utilizador.sh'

    Alíneas 1.*

    Parameters
    ----------
    t : str
        Source directory
    """
    scr = t + "/regista_utilizador.sh"

    print( "\n1 - regista_utilizador.sh...")

    if (not os.path.exists( scr )):
        print( "1. (*error*) {} not found {}".format(scr,fail) )
        return

    cleanup( "utilizadores.txt" )

    eval_msg( check_proc_out( [ scr ],
         b'@ERROR {1.1.1}' ),
        '1.1.1a', test = scr )

    test = [ scr, 'A' ]
    eval_msg( check_proc_out( test,
         b'@ERROR {1.1.1}' ),
        '1.1.1b', test = test )

    test = [ scr, 'A', 'B' ]
    eval_msg( check_proc_out( test,
         b'@ERROR {1.1.1}' ),
        '1.1.1c', test = test )

    test = [ scr, 'A', 'B', 'C', 'D', 'E' ]
    eval_msg( check_proc_out( test,
         b'@ERROR {1.1.1}' ),
        '1.1.1d', test = test )

    test = [ scr, 'Catarina Cruz Missing', '12qwaszx', '10' ]
    eval_msg( check_proc_out( test,
         b'@ERROR {1.1.2}' ),
        '1.1.2a', test = test )

    test = [ scr, 'Catarina', '12qwaszx', '10' ]
    eval_msg( check_proc_out( test,
         b'@ERROR {1.1.2}' ),
        '1.1.2b', test = test )

    test = [ scr, 'Catarina Cruz', '12qwaszx', 'a' ]
    eval_msg( check_proc_out( test,
         b'@ERROR {1.1.3}' ),
        '1.1.3', test = test )

    test = [ scr, 'Catarina Cruz', '12qwaszx', '10', 'absd' ]
    eval_msg( check_proc_out( test,
         b'@ERROR {1.1.4}' ),
        '1.1.4', test = test )

    # 1.2

    cleanup( "utilizadores.txt" )

    test = [ scr, 'Catarina Cruz', '12qwaszx', '10', '234580880' ]
    eval_msg( check_proc_out( test,
         b'@ERROR {1.2.1}' ),
        '1.2.1', test = test )

    eval_msg( os.path.exists( "utilizadores.txt" ), '1.2.2', test = test )

    test = [ scr, 'Ricardo Fonseca', 'senha', '10', '123456789' ]
    eval_msg( check_proc_out( test,
         b'@ERROR {1.2.3}' ),
        '1.2.3', test = test )

    cleanup( "utilizadores.txt" )

    test = [ scr, 'Ricardo Fonseca', 'senha', '10' ]
    eval_msg( check_proc_out( test,
         b'@ERROR {1.2.4}' ),
        '1.2.4', test = test )

    cleanup( "utilizadores.txt" )

    test = [ scr, 'Ricardo Fonseca', 'senha', '10', '123456789' ]
    eval_msg( check_proc_out( test,
         b'@ERROR {1.2.5}' ),
        '1.2.5', test = test )

    cleanup( "utilizadores.txt" )

    test = [ scr, 'Joao Baptista Goncalves', 'senha', '10', '123456789' ]
    eval_msg( check_proc_out( test,
         b'@SUCCESS {1.2.6} [joao.goncalves@kiosk-iul.pt]' ),
        '1.2.6', test = test )

    cleanup( "utilizadores.txt" )

    # 1.2.7
    create_file( 'utilizadores.txt', utilizadores )

    newuser= "{}:Ricardo Fonseca:senha:ricardo.fonseca@kiosk-iul.pt:123456789:10".format( len(utilizadores)+1 )

    test = [ scr, 'Ricardo Fonseca', 'senha', '10', '123456789' ]
    eval_msg( check_proc_out( test,
         b'@SUCCESS {1.2.7} [' ),
        '1.2.7', test = test )

    create_file( 'utilizadores.eval',
                utilizadores + (newuser,) )

    eval_msg( compare_files( "utilizadores.txt", "utilizadores.eval", verbose = True ),
             '1.2.7b', test = test )

    cleanup( ["utilizadores.eval","utilizadores.txt"] )

    # 1.3
    create_file( 'utilizadores.txt', utilizadores )

    test = [ scr, 'Catarina Cruz', 'bad-pass', '10' ]
    eval_msg( check_proc_out( test,
         b'@ERROR {1.3.1}' ),
        '1.3.1', test = test )

    cleanup( "utilizadores.txt" )

    # 1.3.2
    create_file( 'utilizadores.txt', utilizadores )

    test = [ scr, 'Catarina Cruz', '12qwaszx', '10' ]
    eval_msg( check_proc_out( test,
         b'@SUCCESS {1.3.2} [60]' ),
        '1.3.2a', test = test )

    update=list(utilizadores)
    update[1] = "2:Catarina Cruz:12qwaszx:catarina.cruz@kiosk-iul.pt:234580880:60"
    create_file( 'utilizadores.eval', update )

    eval_msg( compare_files( "utilizadores.txt", "utilizadores.eval", verbose = True ),
             '1.3.2b', test = test )

    cleanup( ["utilizadores.eval","utilizadores.txt"] )

    # 1.4
    # this should be done automatically
    sorted = [ update[1], update[2], update[0], update[3] ]
    create_file( 'saldos-ordenados.eval', sorted )

    eval_msg( compare_files( "saldos-ordenados.txt", "saldos-ordenados.eval", verbose = True ),
             '1.4.1', test = test )

    cleanup( ["saldos-ordenados.eval","saldos-ordenados.txt"] )

##########################################################################
# Parte 2 - compra.sh
#
##########################################################################

def test_compra( t ):
    """Testa o script 'compra.sh'

    Alíneas 2.*

    Parameters
    ----------
    t : str
        Source directory
    """
    scr = t + "/compra.sh"

    userErrorLog = []

    print( "\n2 - compra.sh...")

    if (not os.path.exists( scr )):
        print( "2. (*error*) {} not found {}".format(scr,fail) )
        return


    # Alínea 2.1

    cleanup_test_files()

    eval_msg( check_proc_out( [ scr ],
         b'@ERROR {2.1.1}',
        input = b"0\n" ),
        '2.1.1a', test = scr )

    create_file( "produtos.txt", produtos )

    eval_msg( check_proc_out( [ scr ],
         b'@ERROR {2.1.1}',
        input = b"0\n" ),
        '2.1.1b', test = scr )

    create_file( "utilizadores.txt", utilizadores )

    eval_msg( check_proc_out( [ scr ],
         b'1: Croissant chocolate: 3 EUR\n2: Batatas fritas: 2 EUR\n3: Cola: 3 EUR\n0: Sair\n',
        input = b"0\n" ),
        '2.1.2a', test = scr )

    eval_msg( check_proc_out( [ scr ],
         b'@ERROR {2.1.2}',
        input = b"10\n" ),
        '2.1.2b', test = scr )

    eval_msg( check_proc_out( [ scr ],
         b'@ERROR {2.1.3}',
        input = b"1\n100\n12qwaszx\n" ),
        '2.1.3', test = scr )

    eval_msg( check_proc_out( [ scr ],
         b'@ERROR {2.1.4}',
        input = b"1\n2\nbadpass\n" ),
        '2.1.4', test = scr )

    eval_msg( check_proc_out( [ scr ],
         b'@ERROR {2.2.1} [3] [0]',
        input = b"1\n4\nabc123\n" ),
        '2.2.1a', test = scr )

    create_file( 'produtos.eval', produtos )
    eval_msg( compare_files( "produtos.txt", "produtos.eval", verbose = True ),
             '2.2.1b', test = scr )

    create_file( 'produtos.txt', produtos )
    create_file( 'utilizadores.txt', utilizadores )
    create_file( 'relatorio_compras.txt', compras )
    today=datetime.date.today().strftime("%Y-%m-%d")

    eval_msg( check_proc_out( [ scr ],
         b'@SUCCESS {2.2.1} [3] [50]',
        input = b"1\n2\n12qwaszx\n" ),
        '2.2.1c', test = scr )

    update=list(utilizadores)
    update[1]="2:Catarina Cruz:12qwaszx:catarina.cruz@kiosk-iul.pt:234580880:47"

    create_file( 'utilizadores.eval', update )
    eval_msg( compare_files( "utilizadores.txt", "utilizadores.eval", verbose = True ),
             '2.2.2', test = scr )

    # 2.2.3
    update=list(produtos)
    update[0]="Croissant chocolate:Bolo:3:4:7"
    create_file( 'produtos.eval', update )
    eval_msg( compare_files( "produtos.txt", "produtos.eval", verbose = True ),
             '2.2.3', test = scr )

    # 2.2.4
    update=list(compras)
    update.append( "Croissant chocolate:Bolo:2:" + today )

    create_file( 'relatorio_compras.eval', update )
    eval_msg( compare_files( "relatorio_compras.txt", "relatorio_compras.eval", verbose = True ),
             '2.2.4', test = scr )

    # 2.2.5
    f = open( 'lista-compras-utilizador.eval', 'w' )
    print("**** {}: Compras de Catarina Cruz ****".format(today), file=f)
    for line in update:
        values=line.split(':')
        if ( values[2] == "2" ):
            print( "{}, {}".format(values[0], values[3] ), file = f )
    f.close()

    eval_msg( compare_files( "lista-compras-utilizador.txt", "lista-compras-utilizador.eval", verbose = True, respectOrder = False ),
             '2.2.5', test = scr )

    cleanup( ["utilizadores.eval", "relatorio_compras.eval", "produtos.eval", "lista-compras-utilizador.eval" ] )
    cleanup( ["utilizadores.txt", "relatorio_compras.txt", "produtos.txt", "lista-compras-utilizador.txt" ] )

##########################################################################
# Parte 3 - refill.sh
#
##########################################################################

def test_refill( t ):
    """Testa o script 'refill.sh'

    Alíneas 3.*

    Parameters
    ----------
    t : str
        Source directory
    """

    scr = t + "/refill.sh"

    userErrorLog = []

    print( "\n3 - refill.sh...")


    if (not os.path.exists( scr )):
        print( "3. (*error*) {} not found {}".format(scr,fail) )
        return

    # Alínea 3.1
    cleanup_test_files()

    eval_msg( check_proc_out( [ scr ],
         b'@ERROR {3.1.1}') ,
        '3.1.1a', test = scr )

    create_file( "produtos.txt", produtos )

    eval_msg( check_proc_out( [ scr ],
         b'@ERROR {3.1.1}' ),
        '3.1.1b', test = scr )

    badrep = list(reposicao)
    badrep[2] = "Gasolina:Bebida:5",

    create_file( "reposicao.txt", badrep )
    eval_msg( check_proc_out( [ scr ],
         b'@ERROR {3.1.2}' ),
        '3.1.2a', test = scr )

    badrep[2] = "Cola:Bebida:abs",
    create_file( "reposicao.txt", badrep )
    eval_msg( check_proc_out( [ scr ],
         b'@ERROR {3.1.2}' ),
        '3.1.2b', test = scr )

    badrep[2] = "Cola:Bebida:0",
    create_file( "reposicao.txt", badrep )
    eval_msg( check_proc_out( [ scr ],
         b'@ERROR {3.1.2}' ),
        '3.1.2c', test = scr )

    create_file( "reposicao.txt", reposicao )
    eval_msg( check_proc_out( [ scr ],
         b'@SUCCESS {3.2.1}' ),
        '3.2.1a', test = scr )

    f = open( 'produtos-em-falta.eval', 'w' )
    today=datetime.date.today().strftime("%Y-%m-%d")
    print("**** Produtos em falta em {} ****".format(today), file=f)
    for line in produtos:
        values=line.split(':')
        if ( int(values[3]) < int(values[4]) ):
            print( "{}: {} unidades".format(values[0], int(values[4]) - int(values[3]) ),
                  file=f )
    f.close()

    eval_msg( compare_files( "produtos-em-falta.txt", "produtos-em-falta.eval", verbose = True ),
             '3.2.1b', test = scr )

    cleanup( ["produtos-em-falta.eval", "produtos-em-falta.txt"] )

    # 3.2.2
    create_file( "produtos.txt", produtos )
    create_file( "reposicao.txt", reposicao )

    eval_msg( check_proc_out( [ scr ],
         b'@SUCCESS {3.2.2}' ),
        '3.2.2a', test = scr )

    # create solution from test data
    update = list(produtos)
    for rep in reposicao:
        refill = rep.split(':')
        nome = refill[0]
        nitens = int(refill[2])
        for i in range(0,len(update)):
            info = update[i].split(':')
            if ( info[0] == nome ):
                stock = int(info[3])
                max_stock = int( info[4])
                stock += nitens
                if ( stock > max_stock ):
                    stock = max_stock
                info[3] = str(stock)
                update[i] = ':'.join(info)
                break

    create_file( "produtos.eval", update )
    eval_msg( compare_files( "produtos.txt", "produtos.eval", verbose = True ),
             '3.2.2b', test = scr )

    cleanup( ["reposicao.txt","produtos.txt","produtos.eval","produtos-em-falta.txt"])

    # 3.3.1
    success = True
    try:
        f = open( t + '/cron.def', 'r' )
    except IOError:
        success = False
        print( "(*error*) File 'cron.def' not found" )

    if ( success ):
        success = False
        output = f.read().splitlines()
        for line in output:
            if ( re.search('^(\s*)17(\s+)9(\s+)(\*)(\s+)(\*)(\s+)4(\s+)cd', line) ):
                success = True
                break

    eval_msg( success, '3.3.1' )

##########################################################################
# Parte 4 - stats.sh
#
##########################################################################

def test_stats( t ):
    """Testa o script 'stats.sh'

    Alíneas 4.*

    Parameters
    ----------
    t : str
        Source directory
    """

    scr = t + "/stats.sh"

    userErrorLog = []

    print( "\n4 - stats.sh...")

    if (not os.path.exists( scr )):
        print( "4. (*error*) {} not found {}".format(scr,fail) )
        return

    test = [ scr ]
    eval_msg( check_proc_out( test,
         b'@ERROR {4.1.1}' ),
        '4.1.1a', test = test )

    test = [ scr, 'listar', '1' ]
    eval_msg( check_proc_out( test,
         b'@ERROR {4.1.1}' ),
        '4.1.1b', test = test )

    test = [ scr, 'popular', '1', '2' ]
    eval_msg( check_proc_out( test,
         b'@ERROR {4.1.1}' ),
        '4.1.1c', test = test )

    test = [ scr, 'histograma', '1' ]
    eval_msg( check_proc_out( test,
         b'@ERROR {4.1.1}' ),
        '4.1.1d', test = test )

    test = [ scr, 'A' ]
    eval_msg( check_proc_out( test,
         b'@ERROR {4.1.1}' ),
        '4.1.1e', test = test )

    test = [ scr, 'popular' ]
    eval_msg( check_proc_out( test,
         b'@ERROR {4.1.1}' ),
        '4.1.1f', test = test )

    # 4.2.1 - listar

    create_file( 'utilizadores.txt', utilizadores )
    create_file( 'relatorio_compras.txt', compras )

    # create solution from test data
    solution = {}
    for user in utilizadores:
        user_info = user.split(':')
        user_id   = user_info[0]
        user_name = user_info[1]

        ncompras = 0
        for compra in compras:
            compra_info = compra.split(':')
            compra_id = compra_info[2]
            if ( user_id == compra_id ):
                ncompras += 1

        if ( ncompras > 0 ):
            solution[user_name] = ncompras

    f = open( 'stats.eval', 'w' )
    for i in sorted(solution.items(), key=lambda x: x[1], reverse=True):
        if ( i[1] == 1 ):
            print( "{}: {} compra".format(i[0], i[1]), file=f )
        else:
            print( "{}: {} compras".format(i[0], i[1]), file=f )
    f.close()

    test = [ scr, 'listar' ]
    eval_msg( check_proc_out( test,
          b'@SUCCESS {4.2.1}' ),
         '4.2.1a', test = test )

    eval_msg( compare_files( "stats.txt", "stats.eval", verbose = True ),
             '4.2.1b', test = scr )

    cleanup( ["stats.eval", "stats.txt"] )

    # 4.2.2 - popular

    # create solution from test data
    solution = {}
    for compra in compras:
        info = compra.split(':')
        if ( info[0] in solution ):
            solution[ info[0] ] += 1
        else:
            solution[ info[0] ] = 1

    solution = sorted(solution.items(), key=lambda x: x[1], reverse=True)

    i = 0
    f = open( 'stats.eval', 'w' )
    for item in solution:
        if ( item[1] == 1 ):
            print( "{}: {} compra".format(item[0], item[1]), file=f )
        else:
            print( "{}: {} compras".format(item[0], item[1]), file=f )

        i += 1
        if ( i >= 3 ):
            break
    f.close()

    create_file( 'relatorio_compras.txt', compras )
    create_file( 'produtos.txt', produtos )
    test = [ scr, 'popular', '3' ]

    eval_msg( check_proc_out( test,
          b'@SUCCESS {4.2.2}' ),
         '4.2.2a', test = test )

    eval_msg( compare_files( "stats.txt", "stats.eval", verbose = True ),
             '4.2.2b', test = scr )

    cleanup( ["stats.eval", "stats.txt"] )

    # 4.2.3
    hist = {}
    for compra in compras:
        info = compra.split(':')
        if ( info[1] in hist ):
            hist[ info[1] ] += 1
        else:
            hist[ info[1] ] = 1

    test = [ scr, 'histograma' ]
    eval_msg( check_proc_out( test,
          b'@SUCCESS {4.2.3}' ),
         '4.2.3a', test = test )

    success = True
    try:
        f = open( 'stats.txt', 'r' )
    except IOError:
        success = False
        print( "(*error*) File 'stats.txt' not found" )

    if ( success ):
        success = False
        output = f.read().splitlines()
        if ( len(hist) <= len(output) ):
            success = True
            for line in output:
                found = re.search('^(.+?)\s+(\*+)$', line)
                if found:
                    if ( not found.group(1) in hist ):
                        print( "(*error*) 'stats.txt': Unexpected category: {}".format(line) )
                        success = False
                        break

                    if ( len(found.group(2)) != hist[found.group(1)] ):
                        print( "(*error*) 'stats.txt': Unexpected quantity for category {}: got {}, expected {}".format(found.group(1), len(found.group(2)), hist[found.group(1)] ) )
                        success = False
                        break
                else:
                    print( "(*error*) 'stats.txt': Unexpected line: {}".format(line) )
                    success = False

    eval_msg( success, '4.2.3b', test = test )

    cleanup( [ "stats.txt", "produtos.txt", "relatorio_compras.txt", "utilizadores.txt" ])


##########################################################################
# Parte 5 - menu.sh
#
##########################################################################

def test_menu( t ):
    """Testa o script 'menu.sh'

    Alíneas 5.*

    Parameters
    ----------
    t : str
        Source directory
    """

    scr = t + "/menu.sh"

    userErrorLog = []

    print( "\n5 - menu.sh...")

    if (not os.path.exists( scr )):
        print( "5. (*error*) {} not found {}".format(scr,fail) )
        return

    # Alínea 5.1
    opts = bytes('0\n','ascii')
    proc = run_process( [scr], input = opts )
    if ( proc ):
        eval_msg( True, '5.1')
    else:
        eval_msg( False, '5.1')
        print('{} {}'.format( "Menu fails to exit, manual correction needed",fail))
        return

    # Alínea 5.2
    ## A ordem que está no enunciado em 5.2.2.1 para pedir os argumentos é:
    ##  #1: Opção:
    ##  #2: Indique o nome do utilizador:
    ##  #3: Indique a senha do utilizador:
    ##  #4: Para registar o utilizador, insira o NIF do utilizador:
    ##  #5: Indique o saldo a adicionar ao utilizador: 10
    opts = bytes('1\nRicardo Fonseca\npass\n123456789\n2\n\n0\n','ascii')
    eval_msg( check_proc_out( [ scr ],
        b"[*SO*] regista_utilizador.sh 'Ricardo Fonseca' 'pass' '2' '123456789'\n", input = opts ),
        '5.2' )

    # Alínea 5.3
    opts = bytes('2\n\n0\n','ascii')
    eval_msg( check_proc_out( [ scr ],
        b"[*SO*] compra.sh ok\n", input = opts ),
        '5.3' )

    # Alínea 5.4
    opts = bytes('3\n\n0\n','ascii')
    eval_msg( check_proc_out( [ scr ],
        b"[*SO*] refill.sh ok\n", input = opts ),
        '5.4' )


    opts = bytes('4\n1\n\n0\n','ascii')
    eval_msg( check_proc_out( [ scr ],
        b"[*SO*] ok stats.sh listar\n", input = opts ),
        '5.5.1' )

    opts = bytes('4\n2\n3\n\n0\n','ascii')
    eval_msg( check_proc_out( [ scr ],
        b"[*SO*] ok stats.sh popular '3'\n", input = opts ),
        '5.5.2' )

    opts = bytes('4\n3\n\n0\n','ascii')
    eval_msg( check_proc_out( [ scr ],
        b"[*SO*] ok stats.sh histograma\n", input = opts ),
        '5.5.3' )


##########################################################################
# Main
#
##########################################################################

parser = argparse.ArgumentParser( description="Evaluate test problem",
    usage = '%(prog)s [-h] [-g] [-l] [-d] [-e] <source>' )

parser.add_argument( "-g","--generate", dest="generate", action='store_true', \
                    help = "Generate test files")

parser.add_argument( "-l","--cleanup", dest="cleanup", action='store_true', \
                    help = "Cleanup test files")

parser.add_argument( "-d","--debug", dest="debug", action='store_true', \
                    help = "Shows DEBUG information")

parser.add_argument( "-e","--stoponerror", dest="stoponerror", action='store_true', \
                    help = "Stop validation on any error")

parser.add_argument( 'Source',
    metavar='source', nargs = '?', default = '..',
    type =str,
    help='Directory with source files to evaluate' )

# Parse command line arguments
args = parser.parse_args()

# If requested generate test files and exit
if ( args.generate ):
    print("Generating test files...")
    create_test_files()
    print("done.")
    exit(0)

# If requested cleanup test files and exit
if ( args.cleanup ):
    print("Cleaning up test files...")
    cleanup_test_files()
    print("done.")
    exit(0)

# If requested stoponerror, set variable to stop if any error is found (to avoid the parsing of the whole script on initial errors)
if ( args.stoponerror ):
    print("Testing will stop immediately if any error is found")
    stoponerror = True

if ( args.Source is None ):
    print("Source directory not specified")
    parser.print_usage()
    exit(1)

# Get test directory
testdir = args.Source

# Check if directory exists
if (not os.path.exists( testdir )):
    print('(*error*) Directory "{}" does not exist'.format(testdir))
    exit(1)

# Check if it is current directory
if ( os.path.samefile(".", testdir)):
    print('(*error*) Source must not be local (.) directory')
    exit(2)

# Check for additional scripts
scripts = ["error", "success", "debug",
    "regista_utilizador.sh",
    "compra.sh",
    "refill.sh",
    "stats.sh"]

for s in scripts:
    if ( not os.path.exists(s)):
        print('(*error*) "{}" script not found'.format(s))
        exit(3)

questions = [*enunciado]
pauta = []

for i in questions:
    grade[i] = 0.0

# Run tests
print("Evaluating directory '{}'...".format(testdir))

test_regista_utilizador( testdir )
test_compra( testdir )
test_refill( testdir )
test_stats( testdir )
test_menu( testdir )

print("\ndone.")
