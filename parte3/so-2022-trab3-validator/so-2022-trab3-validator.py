#!/usr/bin/env python3

##########################################################################
# ISCTE-IUL: Trabalho prático de Sistemas Operativos (2ª parte / 3ª parte)
#
# Avaliação automática de 1 trabalho
#
# Version 1
##########################################################################

import subprocess
import os
import sys
import argparse

##########################################################################
# Utility functions
#
###########################################################################

if ( sys.stdout.encoding == "UTF-8" or sys.stdout.encoding == "utf-8" ):
  ok   = u"\u001b[1;32m[✔]\u001b[0m"
  fail = u"\u001b[1;31m[✗]\u001b[0m"
  warn = u"\u001b[1;33m[⚠️]\u001b[0m"
  info = u"\u001b[1;34m[ℹ︎]\u001b[0m"
else:
  ok   = "\u001b[1;32m[ ok ]\u001b[0m"
  fail = "\u001b[1;31m[fail]\u001b[0m"
  warn = "\u001b[1;33m[warn]\u001b[0m"
  info = "\u001b[1;34m[info]\u001b[0m"

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

##########################################################################
# Test program
#
##########################################################################
def eval( t, prog, debug = False, grade = False, stoponerror = False, nocleanup = False ) :
    """Testa o programa especificado

    Parameters
    ----------
    t : str
        Source directory
    prog : str
        Programa a testar (sem extensão)
    debug : logical (optional)
        If set to true program will be compiled with preprocessor macro DEBUG=1,
        adding additional debug messages, defaults to False
    grade : logical (optional)
        If set to true a grade report will be printed at the end, defaults to
        False.
    """
    print( u"\n\u001b[1m[{}]\u001b[0m".format(prog))

    if (not os.path.exists( t + "/{}.c".format(prog) )):
        print( "{} {} not found, skipping test".format(fail, t + "/{}.c".format(prog)) )
        return

    # Cleanup files from previous tests
    cleanup( ["{}.c".format(prog), "{}-eval".format(prog) ] )

    # Compile the code
    make = ['make', '-f', 'Makefile.{}'.format(prog), 'SOURCE='+t]

    if (debug):
        make.append( 'DEBUG=1')

    if (stoponerror):
        make.append( 'STOPONERROR=1')

    eval = ['./{}-eval'.format(prog)]
    if (grade):
        eval.append( '-l' )

    try:
        print("{} Compiling {}/{}.c ...".format(info, t,prog))
        proc = subprocess.run( make, stdout = subprocess.PIPE )
        if ( proc.returncode == 0 ):
            print("{} Code compiled ok\n".format(ok));
            print("{} Evaluating {}/{}.c ...\n".format(info, t, prog))
            proc = subprocess.run( eval )
        else:
            print( "{} Unable to compile {}/{}.c, skipping test".format(fail,t,prog) )
    except:
        print( "{} Unable to test {}/{}.c".format(fail,t,prog) )

    if ( proc.returncode and stoponerror ):
        print('Testing stopped because --stoponerror was specified' )
        exit(1) # Temporário para parar a cada erro

    # Cleanup test files
    if ( not nocleanup ):
        cleanup( ["{}-eval".format(prog) ] )

##########################################################################
# Main
#
##########################################################################

parser = argparse.ArgumentParser( description="Evaluate test problem",
    usage = '%(prog)s [-h] [-d] [-g] [-e] [-s] [-c] [-x] <source>' )

parser.add_argument( "-d","--debug", dest="debug", action='store_true', \
                    help = "Print additional debug information")

parser.add_argument( "-g","--grade", dest="grade", action='store_true', \
                    help = "Generate grades")

parser.add_argument( "-e","--stoponerror", dest="stoponerror", action='store_true', \
                    help = "Stop validation on any error")

parser.add_argument( "-s","--server", dest="server", action='store_true', \
                    help = "Validate only the Server (i.e., do not validate the Client)")

parser.add_argument( "-c","--client", dest="client", action='store_true', \
                    help = "Validate only the Client (i.e., do not validate the Server)")

parser.add_argument( "-x","--nocleanup", dest="nocleanup", action='store_true', \
                    help = "Do not erase executables after validating")

parser.add_argument( 'Source',
    metavar='source', nargs = '?', default = None,
    type =str,
    help='Directory with source files to evaluate' )

# Parse command line arguments
args = parser.parse_args()

if ( args.debug ):
    print("User requested (option -d) that extended DEBUG information is shown")

# If requested stoponerror, stop if any error is found (to avoid the parsing of the whole script on initial errors)
if ( args.stoponerror ):
    print("User requested (option -e) that evaluation STOPS IMMEDIATELY if any error is found")

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

# Run tests

print("Evaluating directory '{}'".format(testdir))

if ( args.server ):
    print("User requested (option -s) that CLIENT is NOT validated")
else:
    eval( testdir, "cliente", debug = args.debug, grade = args.grade, stoponerror = args.stoponerror, nocleanup = args.nocleanup )

if ( args.client ):
    print("User requested (option -c) that SERVER is NOT validated")
else:
    eval( testdir, "servidor", debug = args.debug, grade = args.grade, stoponerror = args.stoponerror, nocleanup = args.nocleanup )

print("\ndone.")