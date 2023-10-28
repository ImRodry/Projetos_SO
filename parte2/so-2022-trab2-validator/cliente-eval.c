#define _CLIENTE 1
#include "common.h"

#include <fcntl.h>
#include <sys/stat.h>

/**
 * Undefine the replacement macros defined in eval.h
 **/
#include "eval-undef.h"


/**
 * @brief Sets default values for evaluation variables
 *
 * This sets the default behavior for all eval.c functions. It **must** call
 * both eval_reset_vars() and eval_reset_stats()
 */
void eval_reset_except_stats() {
    // Reset all values
    // Except for the exit() function the default behavior is to
    // capture function parameters and then call function
    eval_reset_vars();

    // Set timeout to 1.0 s
    _eval_env.timeout = 1.0;

    // Don't issue info message if exit is called
    _eval_exit_data.action = 0;

    // Abort test whenever pause() or execl() are called
    _eval_pause_data.action = 2;
    _eval_execl_data.action = 2;

    // Prevent signals to self
    _eval_raise_data.action = 1;
    _eval_kill_data.action = 1;
}

/**
 * @brief Sets default values for evaluation variables
 *
 * This sets the default behavior for all eval.c functions. It **must** call
 * bott eval_reset_vars() and eval_reset_stats()
 */
void eval_reset() {
    eval_reset_except_stats();

    // Reset stats counters
    eval_reset_stats();
}

// Global variables
Login clientRequest;     // Variável que tem o pedido enviado do Cliente para o Servidor


// Question list
question_t questions[] = {
    {"1.1",   "File exists and is FIFO", 0.0 },
    {"1.2",   "File exists but is not FIFO", 0.0 },
    {"1.3",   "File does not exist", 0.0 },

    {"2",     "Arma sinais", 0.0 },
    {"3",     "Pede dados ao utilizador", 0.0 },

    {"4.1",     "Cria FIFO", 0.0 },
    // {"4.2",     "Cria FIFO (erro)", 0.0 },

    {"5.1",     "Escreve pedido", 0.0 },
    {"5.2",     "Escreve pedido (erro)", 0.0 },

    {"6",     "Configura alarme", 0.0 },

    {"7.1",     "Lê login", 0.0 },
    {"7.2",     "Lê login (erro)", 0.0 },

    {"8.1",     "Login successful", 0.0 },
    {"8.2",     "Login failed", 0.0 },

    {"9",     "Processamento", 0.0 },

    {"10.1",     "Escreve fim de sessão", 0.0 },
    {"10.2",     "Escreve fim de sessão (erro)", 0.0 },

    {"11",     "Apaga FIFO e termina", 0.0 },

    {"12",     "Trata SIGUSR2", 0.0 },

    {"13.1",     "Cancela pedido", 0.0 },
    {"13.2",     "Cancela pedido (erro)", 0.0 },

    {"14",     "Trata SIGALRM (timeout)", 0.0 },

    {"---", "_end_",0.0}
};


/**
 * Wrapper for
 *
 */

struct {
    int action;
    int result;
} _existsFifoServidor_C1;

int existsFifoServidor_C1(char *nameFifo) {
    _existsFifoServidor_C1.result = -1;
    _existsFifoServidor_C1.result = _student_existsFifoServidor_C1(nameFifo);
    return _existsFifoServidor_C1.result;
}

/**
 * Evaluate C1
 **/
int eval_c1( ) {
    eval_info("Evaluating C1.1 - %s...", question_text(questions,"1.1"));
    eval_reset();

    char nameFifo[] = "eval.fifo";
    mode_t mode = 0666;

    // C1.1 File exists and is FIFO
    mkfifo( nameFifo, mode );

    EVAL_CATCH( existsFifoServidor_C1(nameFifo) );
    if ( _NO_EXIT != _eval_env.stat ) {
        eval_error( "(C1) bad termination");
    }
    if ( _SUCCESS != _existsFifoServidor_C1.result ) {
        eval_error("(C1) existsFifoServidor_C1 returned error with existing FIFO");
    }

    eval_check_successlog( "(C1)", "(C1) " );

    unlink( nameFifo );

    eval_info("Evaluating C1.2 - %s...", question_text(questions,"1.2"));
    eval_reset_except_stats();

    // C1.2 File exists and is not FIFO
    FILE *f = fopen( nameFifo, "w" );
    fclose(f);

    EVAL_CATCH( existsFifoServidor_C1(nameFifo) );
    if ( _EXIT != _eval_env.stat && _ERROR != _existsFifoServidor_C1.result ) {
        eval_error("(C1) existsFifoServidor_C1 returned success with non FIFO file");
    }

    eval_check_errorlog( "(C1)", "(C1) " );

    unlink( nameFifo );

    eval_info("Evaluating C1.3 - %s...", question_text(questions,"1.3"));
    eval_reset_except_stats();

    // C1.3 File does not exist
    EVAL_CATCH( existsFifoServidor_C1(nameFifo) );
    if ( _EXIT != _eval_env.stat && _ERROR != _existsFifoServidor_C1.result ) {
        eval_error("(C1) existsFifoServidor_C1 returned success with missing file");
    }

    eval_check_errorlog( "(C1)", "(C1) " );
    eval_close_logs( "(C1)" );
    return eval_complete("(C1)");
}

/**
 * Wrapper for triggerSignals_C2()
 **/
struct {
    int action;
    int success;
} _triggerSignals_C2_data;

int triggerSignals_C2() {
    _triggerSignals_C2_data.success = -1024;
    _triggerSignals_C2_data.success = _student_triggerSignals_C2();
    return 0;
}

int eval_c2( ) {
    eval_info("Evaluating C2 - %s...", question_text(questions,"2"));
    eval_reset();

    initlog(&_data_log);
    _eval_signal_data.action = 2;

    EVAL_CATCH( triggerSignals_C2() );

    if ( _NO_EXIT != _eval_env.stat ) {
        eval_error( "(C2) bad termination");
    }

    if ( _SUCCESS != _triggerSignals_C2_data.success )
        eval_error("(C2) Bad return value");

    char buffer[100];

    snprintf( buffer, 100, "signal,%d,%p", SIGUSR2, trataSinalSIGUSR2_C12 );
    if ( _NOT_FOUND > findinlog( &_data_log, buffer )) {
        eval_error("(C2) SIGUSR2 not armed properly");
    }

    snprintf( buffer, 100, "signal,%d,%p", SIGINT, trataSinalSIGINT_C13 );
    if ( _NOT_FOUND > findinlog( &_data_log, buffer )) {
        eval_error("(C2) SIGINT not armed properly");
    }

    snprintf( buffer, 100, "signal,%d,%p", SIGALRM, trataSinalSIGALRM_C14 );
    if ( _NOT_FOUND > findinlog( &_data_log, buffer )) {
        eval_error("(C2) SIGALRM not armed properly");
    }

    eval_check_successlog( "(C2)", "(C2) " );

    eval_close_logs( "(C2)" );
    return eval_complete("(C2)");
}

/**
 * Wrapper for getDadosPedidoUtilizador_C3()
 **/
struct {
    int action;
    Login request;
} _getDadosPedidoUtilizador_C3_data;

Login getDadosPedidoUtilizador_C3() {
    _getDadosPedidoUtilizador_C3_data.request = (Login) {.nif = -1};
    _getDadosPedidoUtilizador_C3_data.request = _student_getDadosPedidoUtilizador_C3();
    return _getDadosPedidoUtilizador_C3_data.request;
}

int eval_c3( ) {
    eval_info("Evaluating C3 - %s...", question_text(questions,"3"));
    eval_reset();

    const int nif = 12345678;
    const char senha[] = "abc123";

    FILE* ft;
    ft = fopen(FILE_STDIN,"w");
    fprintf(ft,"%d\n", nif );
    fprintf(ft,"%s\n", senha );
    for(int i = 0; i < 5; i++ ) fprintf(ft,"\n" );
    fclose(ft);

    unlink( FILE_STDOUT );

    // Redirect standard I/O to files
    EVAL_CATCH_IO( getDadosPedidoUtilizador_C3(), FILE_STDIN, FILE_STDOUT );

    if ( _NO_EXIT != _eval_env.stat ) {
        eval_error( "(C3) bad termination");
    }

    // Remove I/O files
    unlink( FILE_STDIN );
    unlink( FILE_STDOUT );

    if ( nif != _getDadosPedidoUtilizador_C3_data.request.nif ) {
        eval_error( "(C3) invalid request.nif" );
    }

    if ( strncmp( senha, _getDadosPedidoUtilizador_C3_data.request.senha, strlen(senha) ) ) {
        eval_error( "(C3) invalid request.senha");
    }

    if ( getpid() != _getDadosPedidoUtilizador_C3_data.request.pid_cliente ) {
        eval_error( "(C3) invalid request.pid_cliente");
    }

    char buffer[1024];
    snprintf( buffer, 1024, "(C3) %d %s %d", nif, senha, getpid());

    eval_check_successlog( "(C3)", buffer );

    eval_close_logs( "(C3)" );
    return eval_complete("(C3)");
}

/**
 * buildNomeFifo is not checked
 */

int buildNomeFifo(char *buffer, int buffer_size, char *prefix, int pid, char *suffix) {
    snprintf(buffer, buffer_size, "%s%d%s", prefix, pid, suffix );
    return 0;
}

/**
 * Wrapper for createFifo_C4()
 **/
struct {
    int success;
} _createFifo_C4_data;

int createFifo_C4(char *nameFifo) {
    _createFifo_C4_data.success = -1234;
    _createFifo_C4_data.success = _student_createFifo_C4( nameFifo );
    return _createFifo_C4_data.success;
}

int eval_c4( ) {

    eval_info("Evaluating C4.1 - %s...", question_text(questions,"4.1"));
    eval_reset();

    // Check normal file creation

    char nameFifo[] = "eval.fifo";

    _eval_mkfifo_data.action = 1;
    _eval_mkfifo_data.status = 0;

    EVAL_CATCH( createFifo_C4( nameFifo ) );

    if ( _NO_EXIT != _eval_env.stat ) {
        eval_error( "(C4) bad termination");
    }

    if ( _SUCCESS != _createFifo_C4_data.success ) {
        eval_error("(C4) Bad return value");
    }

    if ( 1 == _eval_mkfifo_data.status ) {
        if ( strncmp( nameFifo, _eval_mkfifo_data.path, strlen(nameFifo) ) ) {
            eval_error( "(C4) FIFO created with invalid name");
        }
        if ( ! ( 0600 & _eval_mkfifo_data.mode ) ) {
            eval_error( "(C4) FIFO created with invalid mode, must be r/w by owner");
        }

        eval_check_successlog( "(C4)","(C4) " );

    } else {
        eval_error("(C4) mkfifo() should have been called exactly once");
    }

    unlink( nameFifo );

    // eval_info("Evaluating C4.2 - %s...", question_text(questions,"4.2"));
    // eval_reset_except_stats();

    // _eval_mkfifo_data.action = 0;
    // _eval_mkfifo_data.status = 0;

    // Check with existing file
    FILE *f = fopen( nameFifo, "w" );
    fclose(f);

    // EVAL_CATCH( createFifo_C4( nameFifo ) );

    // if ( _EXIT != _eval_env.stat && _ERROR != _createFifo_C4_data.success ) {
    //     eval_error("(C4) createFifo_C4 returned success wrongly: FIFO already existed");
    // }
    // eval_check_errorlog( "(C4)","C4" );

    unlink( nameFifo );

    eval_close_logs( "(C4)" );
    return eval_complete("(C4)");
}

/**
 * Wrapper for writeRequest_C5()
 **/
struct {
    int success;
} _writeRequest_C5_data;

int writeRequest_C5(Login request, char *nameFifo) {
    _writeRequest_C5_data.success = -1234;
    _writeRequest_C5_data.success = _student_writeRequest_C5( request, nameFifo );
    return _writeRequest_C5_data.success;
}

struct {
    int status;
    int action;
} _deleteFifoAndExit_C11_data;

int eval_c5( ) {
    eval_info("Evaluating C5.1 - %s...", question_text(questions,"5.1"));
    eval_reset();

    _deleteFifoAndExit_C11_data.status = 0;
    _deleteFifoAndExit_C11_data.action = 1;

    Login request = {
        .nif = 12345678,
        .senha = "abc123xyz",
        .nome = "José Silva",
        .saldo = 1234,
        .pid_cliente = 16384,
        .pid_servidor_dedicado = 32768
    };

    char nameFifo[] = "eval.fifo";
    FILE *f = fopen( nameFifo, "w" );
    fclose(f);

    EVAL_CATCH( writeRequest_C5( request, nameFifo ) );

    if ( _NO_EXIT != _eval_env.stat ) {
        eval_error( "(C5) bad termination");
    }
    if ( _SUCCESS != _writeRequest_C5_data.success ) {
        eval_error("(C5) writeRequest_C5 bad return value");
    }
    if ( _deleteFifoAndExit_C11_data.status > 0 ) {
        eval_error( "(C5) deleteFifoAndExit_C11() should NOT have been called");
    }

    f = fopen( nameFifo, "r" );
    if ( f == NULL ) {
        eval_error("(C5) Unable to open output file for testing");
    } else {
        Login l;
        if ( fread( &l, sizeof(Login), 1, f ) < 1 ) {
            eval_error("(C5) Unable to read output file for testing");
        } else {
            if ( request.nif != l.nif ||
                 strcmp(request.senha, l.senha) ||
                 strcmp(request.nome, l.nome) ||
                 request.saldo != l.saldo ||
                 request.pid_cliente != l.pid_cliente ) {
                eval_error("(C5) Data in file does not match supplied data");
            }
        }
        fclose(f);
    }

    // Remove I/O files
    unlink( nameFifo );

    eval_check_successlog( "(C5)","(C5) " );

    eval_info("Evaluating C5.2 - %s...", question_text(questions,"5.2"));
    eval_reset_except_stats();

    _deleteFifoAndExit_C11_data.status = 0;
    _deleteFifoAndExit_C11_data.action = 1;

    // Check with locked file
    f = fopen( nameFifo, "w" );
    fclose(f);
    chmod( nameFifo, 0000 );

    EVAL_CATCH( writeRequest_C5( request, nameFifo ) );

    if ( _EXIT != _eval_env.stat && _ERROR != _writeRequest_C5_data.success ) {
        eval_error("(C5) writeRequest_C5 returned success on write fail");
    }
    if ( 1 != _deleteFifoAndExit_C11_data.status ) {
        eval_error( "(C5) deleteFifoAndExit_C11() should have been called exactly once");
    }

    eval_check_errorlog( "(C5)","(C5) " );

    chmod( nameFifo, 0666 );
    unlink( nameFifo );

    eval_close_logs( "(C5)" );
    return eval_complete("(C5)");
}


void configuraTemporizador_C6(int tempoEspera) {
    _student_configuraTemporizador_C6( tempoEspera );
}

int eval_c6( ) {
    eval_info("Evaluating C6 - %s...", question_text(questions,"6"));
    eval_reset();

    _eval_alarm_data.status = 0;
    _eval_alarm_data.action = 1;

    int tempoEspera = 123;

    EVAL_CATCH( configuraTemporizador_C6(tempoEspera) );

    if ( _NO_EXIT != _eval_env.stat ) {
        eval_error( "(C6) bad termination");
    }

    if ( 1 != _eval_alarm_data.status ) {
        eval_error("(C6) alarm() should have been called exactly once");
    } else {
        if ( _eval_alarm_data.seconds != tempoEspera ) {
            eval_error("(C6) alarm() was not called with the correct parameters");
        }

        char buffer[1024];
        snprintf( buffer, 1024, "(C6) Espera resposta em %d segundos", tempoEspera );
        eval_check_successlog( "(C6)", buffer );
    }

    eval_close_logs( "(C6)" );
    return eval_complete("(C6)");
}

/**
 * Wrapper for readAckLogin_C7()
 **/
struct {
    Login l;
} _readAckLogin_C7_data;

Login readAckLogin_C7(char *nameFifo) {
    _readAckLogin_C7_data.l = (Login) {.pid_servidor_dedicado = -1234};
    _readAckLogin_C7_data.l = _student_readAckLogin_C7( nameFifo );
    return _readAckLogin_C7_data.l;
}

int eval_c7( ) {
    eval_info("Evaluating C7.1 - %s...", question_text(questions,"7.1"));
    eval_reset();

    _deleteFifoAndExit_C11_data.status = 0;
    _deleteFifoAndExit_C11_data.action = 1;

    Login l = {
        .nif = 12345678,
        .senha = "abc123xyz",
        .nome = "José Silva",
        .saldo = 1234,
        .pid_cliente = 16384,
        .pid_servidor_dedicado = 32768
    };

    char nameFifo[] = "eval.fifo";
    FILE *f = fopen( nameFifo, "w" );
    fwrite( &l, sizeof(Login), 1, f );
    fclose(f);

    EVAL_CATCH( readAckLogin_C7(nameFifo) );

    if ( _NO_EXIT != _eval_env.stat ) {
        eval_error( "(C7) bad termination");
    }
    if ( _readAckLogin_C7_data.l.pid_servidor_dedicado < 0 ) {
        eval_error( "(C7) bad return value");
    }
    if ( _readAckLogin_C7_data.l.nif != l.nif ||
        strcmp(_readAckLogin_C7_data.l.senha, l.senha) ||
        strcmp(_readAckLogin_C7_data.l.nome, l.nome) ||
        _readAckLogin_C7_data.l.saldo != l.saldo ||
        _readAckLogin_C7_data.l.pid_cliente != l.pid_cliente ) {
        eval_error("(C7) Data read from file does not match supplied data");
    }
    if ( _deleteFifoAndExit_C11_data.status > 0 ) {
        eval_error( "(C7) deleteFifoAndExit_C11() should NOT have been called");
    }

    char buffer[1024];
    snprintf(buffer,1024,"(C7) %s %d %d", l.nome, l.saldo, l.pid_servidor_dedicado );
    eval_check_successlog( "(C7)", buffer );

    unlink( nameFifo );

    eval_info("Evaluating C7.2 - %s...", question_text(questions,"7.2"));
    eval_reset_except_stats();

    _deleteFifoAndExit_C11_data.status = 0;
    _deleteFifoAndExit_C11_data.action = 1;

    EVAL_CATCH( readAckLogin_C7(nameFifo) );

    if ( _NO_EXIT != _eval_env.stat ) {
        eval_error( "(C7) bad termination");
    }
    if ( 1 != _deleteFifoAndExit_C11_data.status ) {
        eval_error( "(C7) deleteFifoAndExit_C11() should have been called exactly once");
    }

    eval_check_errorlog( "(C7)", "(C7) " );

    eval_close_logs( "(C7)" );
    return eval_complete("(C7)");

}

struct {
    int success;
} _validateAutenticacaoServidor_C8_data;

int validateAutenticacaoServidor_C8(Login ackLogin) {
    _validateAutenticacaoServidor_C8_data.success = -1234;
    _validateAutenticacaoServidor_C8_data.success = _student_validateAutenticacaoServidor_C8( ackLogin );
    return _writeRequest_C5_data.success;
}

int eval_c8( ) {
    eval_info("Evaluating C8.1 - %s...", question_text(questions,"8.1"));
    eval_reset();

    _eval_alarm_data.status = 0;
    _eval_alarm_data.action = 1;

    _deleteFifoAndExit_C11_data.status = 0;
    _deleteFifoAndExit_C11_data.action = 1;

    Login l = {
        .nif = 12345678,
        .senha = "abc123xyz",
        .nome = "José Silva",
        .saldo = 1234,
        .pid_cliente = 16384,
        .pid_servidor_dedicado = 32768
    };

    EVAL_CATCH( validateAutenticacaoServidor_C8(l) );

    if ( _NO_EXIT != _eval_env.stat ) {
        eval_error( "(C8) bad termination");
    }

    if ( 1 != _eval_alarm_data.status ) {
        eval_error( "(C8) alarm() should have been called exactly once");
    } else {
        if ( 0 != _eval_alarm_data.ret ) {
            eval_error( "(C8) alarm() called with invalid parameters");
        }
    }
    if ( _SUCCESS != _validateAutenticacaoServidor_C8_data.success ) {
        eval_error( "(C8) bad return value on successful login");
    }
    if ( _deleteFifoAndExit_C11_data.status > 0 ) {
        eval_error( "(C8) deleteFifoAndExit_C11() should NOT have been called");
    }

    eval_check_successlog( "(C8)", "(C8) " );

    eval_info("Evaluating C8.2 - %s...", question_text(questions,"8.2"));
    eval_reset_except_stats();

    _deleteFifoAndExit_C11_data.status = 0;
    _deleteFifoAndExit_C11_data.action = 1;

    l.pid_servidor_dedicado = -1;

    EVAL_CATCH( validateAutenticacaoServidor_C8(l) );

    if ( _EXIT != _eval_env.stat && _ERROR != _validateAutenticacaoServidor_C8_data.success ) {
        eval_error( "(C7) validateAutenticacaoServidor_C8 returned success on failed login");
    }
    if ( 1 != _deleteFifoAndExit_C11_data.status ) {
        eval_error( "(C8) deleteFifoAndExit_C11() should have been called exactly once");
    }

    eval_check_errorlog( "(C8)", "(C8) " );

    eval_close_logs( "(C8)" );
    return eval_complete("(C8)");
}

struct {
    int tempo;
} _sleepRandomTime_C9_data;

int sleepRandomTime_C9() {
    _sleepRandomTime_C9_data.tempo = -1234;
    _sleepRandomTime_C9_data.tempo = _student_sleepRandomTime_C9( );
    return _sleepRandomTime_C9_data.tempo;
}

int eval_c9( ) {
    eval_info("Evaluating C9 - %s...", question_text(questions,"9"));
    eval_reset();

    _eval_sleep_data.status = 0;
    _eval_sleep_data.action = 1;

    EVAL_CATCH( sleepRandomTime_C9() );

    if ( _NO_EXIT != _eval_env.stat ) {
        eval_error( "(C8) bad termination");
    }

    // The so_rand() macro was modified to always return 1340000000
    if ( 4 != _sleepRandomTime_C9_data.tempo ) {
        eval_error( "(C9) bad return value");
    }

    if ( 1 != _eval_sleep_data.status ) {
        eval_error( "(C9) sleep() should have been called exactly once");
    } else {
        if ( _sleepRandomTime_C9_data.tempo != _eval_sleep_data.seconds ) {
            eval_error( "(C9) sleep() called with wrong value");
        }
    }

    char buffer[1024];
    snprintf(buffer, 1024, "(C9) Processamento durante %d segundos", 4 );
    eval_check_successlog( "(C9)", buffer );

    eval_close_logs( "(C9)" );
    return eval_complete("(C9)");
}


struct {
    int success;
} _writeFimSessao_C10_data;

int writeFimSessao_C10(char *nameFifo, int tempoDeProcessamento, Login ackLogin) {
    _writeFimSessao_C10_data.success = -1234;
    _writeFimSessao_C10_data.success = _student_writeFimSessao_C10( nameFifo, tempoDeProcessamento, ackLogin );
    return _writeFimSessao_C10_data.success;
}

int eval_c10( ) {
    eval_info("Evaluating C10.1 - %s...", question_text(questions,"10.1"));
    eval_reset();

    Login l = { .pid_servidor_dedicado = 16384 };

    char nameFifo[] = "sd-16384.fifo";

    _deleteFifoAndExit_C11_data.status = 0;
    _deleteFifoAndExit_C11_data.action = 1;

    char name[1024];
    int tempo = 1;

    EVAL_CATCH( writeFimSessao_C10( name, tempo, l ) );

    if ( _NO_EXIT != _eval_env.stat && 1 != _deleteFifoAndExit_C11_data.status ) {
        eval_error( "(C10) bad termination");
    }
    if ( _SUCCESS != _writeFimSessao_C10_data.success && 1 != _deleteFifoAndExit_C11_data.status ) {
        eval_error( "(C10) bad return value");
    }
    if ( strncmp( nameFifo, name, strlen(nameFifo)) ) {
        eval_error("(C10) Bad nameFifo created");
    } else {
        FILE *f = fopen( nameFifo, "r" );
        if ( f == NULL ) {
            eval_error("(C10) Unable to open file %s for testing", nameFifo );
        } else {
            char buffer[1024];
            fgets( buffer, 1024, f );
            if ( strncmp( buffer, "Sessão Login ativa durante 1 segundos", 38 ) ) {
                eval_error("(C10) Bad message written to FIFO" );
            }
            fclose(f);
        }
    }
    unlink( nameFifo );

    if ( _deleteFifoAndExit_C11_data.status > 1) {
        eval_error( "(C10) deleteFifoAndExit_C11() should have been called exactly once");
    }

    eval_check_successlog( "(C10)", "(C10) " );

    eval_info("Evaluating C10.2 - %s...", question_text(questions,"10.2"));
    eval_reset_except_stats();

    _deleteFifoAndExit_C11_data.status = 0;
    _deleteFifoAndExit_C11_data.action = 1;

    FILE * f = fopen( nameFifo, "w" );
    fclose(f);
    chmod( nameFifo, 0000 );

    EVAL_CATCH( writeFimSessao_C10( name, tempo, l ) );

    if ( _EXIT != _eval_env.stat && _ERROR != _writeFimSessao_C10_data.success && 1 != _deleteFifoAndExit_C11_data.status ) {
        eval_error("(C10) deleteFifoAndExit_C11 returned success on missing database");
    }
    if ( 1 != _deleteFifoAndExit_C11_data.status ) {
        eval_error( "(C10) deleteFifoAndExit_C11 should have been called exactly once");
    }

    chmod( nameFifo, 0666 );
    unlink( nameFifo );

    eval_check_errorlog( "(C10)", "(C10) " );

    eval_close_logs( "(C10)" );
    return eval_complete("(C10)");
}

void deleteFifoAndExit_C11() {
    _deleteFifoAndExit_C11_data.status++;
    if ( _deleteFifoAndExit_C11_data.action == 0 )
        _student_deleteFifoAndExit_C11();
}

int eval_c11( ) {
    eval_info("Evaluating C11 - %s...", question_text(questions,"11"));
    eval_reset();

    _eval_unlink_data.status = 0;
    _eval_unlink_data.action = 1;

    _eval_remove_data.status = 0;
    _eval_remove_data.action = 1;

    _deleteFifoAndExit_C11_data.action = 0;

    char nomeFifoCliente[1024];
    buildNomeFifo(nomeFifoCliente, 20, FILE_PREFIX_CLIENT, getpid(), FILE_SUFFIX_FIFO);

    EVAL_CATCH( deleteFifoAndExit_C11( ) );

    if ( _EXIT != _eval_env.stat ) {
        eval_error( "(C11) bad termination, should have ended with exit call");
    } else {
        if ( 0 != _eval_exit_data.status ) {
            eval_error( "(C11) exit() called with wrong value");
        }
    }

    if ( 1 != _eval_unlink_data.status && 1 != _eval_remove_data.status  ) {
        eval_error("(C11) unlink()/remove() should have been called exactly once");
    } else {
        if (1 == _eval_unlink_data.status) {
            if (strncmp( _eval_unlink_data.path, nomeFifoCliente, 1024 ) ) {
                eval_error("(C11) unlink() called with invalid path");
            }
        } else {
            if (strncmp( _eval_remove_data.path, nomeFifoCliente, 1024 ) ) {
                eval_error("(C11) remove() called with invalid path");
            }
        }
    }

    eval_check_successlog( "(C11)", "(C11) " );

    eval_close_logs( "(C11)" );
    return eval_complete("(C11)");
}

void trataSinalSIGUSR2_C12(int sinalRecebido) {
    _student_trataSinalSIGUSR2_C12(sinalRecebido);
}

int eval_c12( ) {
    eval_info("Evaluating C12 - %s...", question_text(questions,"12"));
    eval_reset();

    _deleteFifoAndExit_C11_data.status = 0;
    _deleteFifoAndExit_C11_data.action = 1;

    EVAL_CATCH( trataSinalSIGUSR2_C12( SIGUSR2 ) );

    if ( _NO_EXIT != _eval_env.stat ) {
        eval_error( "(C12) bad termination");
    }
    if ( 1 != _deleteFifoAndExit_C11_data.status ) {
        eval_error( "(C12) deleteFifoAndExit_C11() should have been called exactly once");
    }

    eval_check_successlog( "(C12)", "(C12) " );

    eval_close_logs( "(C12)" );
    return eval_complete("(C12)");
}


void trataSinalSIGINT_C13(int sinalRecebido) {
    _student_trataSinalSIGINT_C13( sinalRecebido );
}

int eval_c13( ) {
    eval_info("Evaluating C13.1 - %s...", question_text(questions,"13.1"));
    eval_reset();

    _deleteFifoAndExit_C11_data.status = 0;
    _deleteFifoAndExit_C11_data.action = 1;

    clientRequest.pid_servidor_dedicado = 32768;

    EVAL_CATCH( trataSinalSIGINT_C13( SIGINT ) );

    if ( _NO_EXIT != _eval_env.stat ) {
        eval_error( "(C13) bad termination");
    }

    char fname[20];
    buildNomeFifo(fname, 20, FILE_PREFIX_SRVDED, clientRequest.pid_servidor_dedicado, FILE_SUFFIX_FIFO);

    FILE * f = fopen( fname, "r" );
    if ( f == NULL ) {
        eval_error("(C13) Unable to open file %s for testing", fname );
    } else {
        char buffer[1024];
        fgets( buffer, 1024, f );
        if ( strncmp( buffer, "Sessão cancelada pelo utilizador", 32 ) ) {
            eval_error("(C13) Bad message written to FIFO" );
        }
        fclose(f);
    }

    unlink(fname);

    if ( 1 != _deleteFifoAndExit_C11_data.status ) {
        eval_error( "(C13) deleteFifoAndExit_C11() should have been called exactly once");
    }

    eval_check_successlog( "(C13)", "(C13) " );

    eval_info("Evaluating C13.2 - %s...", question_text(questions,"13.2"));
    eval_reset_except_stats();

    // Check with write error
    f = fopen( fname, "w" );
    fclose(f);
    chmod(fname,0000);

    _deleteFifoAndExit_C11_data.status = 0;
    _deleteFifoAndExit_C11_data.action = 1;

    EVAL_CATCH( trataSinalSIGINT_C13( SIGINT ) );

    if ( _NO_EXIT != _eval_env.stat ) {
        eval_error( "(C13) bad termination");
    }

    if ( 1 != _deleteFifoAndExit_C11_data.status ) {
        eval_error( "(C13) deleteFifoAndExit_C11() should have been called exactly once");
    }

    eval_check_errorlog( "(C13)", "(C13) " );

    chmod(fname,0666);
    unlink(fname);

    eval_close_logs( "(C13)" );
    return eval_complete("(C13)");
}


void trataSinalSIGALRM_C14(int sinalRecebido) {
    _student_trataSinalSIGALRM_C14( sinalRecebido );
}

int eval_c14( ) {
    eval_info("Evaluating C14 - %s...", question_text(questions,"14"));
    eval_reset();

    _deleteFifoAndExit_C11_data.status = 0;
    _deleteFifoAndExit_C11_data.action = 1;

    EVAL_CATCH( trataSinalSIGALRM_C14( SIGALRM ) );

    if ( _NO_EXIT != _eval_env.stat ) {
        eval_error( "(C14) bad termination");
    }

    if ( 1 != _deleteFifoAndExit_C11_data.status ) {
        eval_error( "(C14) deleteFifoAndExit_C11() should have been called exactly once");
    }

    eval_check_errorlog( "(C14)", "(C14) " );

    eval_close_logs( "(C14)" );
    return eval_complete("(C14)");
}



int main(int argc, char *argv[]) {

    initlog( &_success_log );
    initlog( &_error_log );

    eval_info(" %s/cliente.c\n", TOSTRING( _EVAL ) );

    int nerr = 0;

    nerr += eval_c1();
    nerr += eval_c2();
    nerr += eval_c3();
    nerr += eval_c4();
    nerr += eval_c5();
    nerr += eval_c6();
    nerr += eval_c7();
    nerr += eval_c8();
    nerr += eval_c9();
    nerr += eval_c10();
    nerr += eval_c11();
    nerr += eval_c12();
    nerr += eval_c13();
    nerr += eval_c14();

    eval_info("Finished." );

    // Printout error summary report
    _eval_stats.error = nerr;
    eval_complete("cliente");

    /* Print out full evaluation report */
    if ( argc >= 2 ) {
        if ( ! strncmp( argv[1], "-x", 2 ) ) question_export(questions, "cliente" );
        if ( ! strncmp( argv[1], "-l", 2 ) ) question_list(questions, "cliente" );
    }

}