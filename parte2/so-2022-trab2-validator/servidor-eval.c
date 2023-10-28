#define _SERVIDOR 1
#include "common.h"

#include <sys/stat.h>
#include <fcntl.h>

/**
 * Undefine the replacement macros defined in eval.h
 **/
#include "eval-undef.h"

/**
 * @brief Sets default values for evaluation variables
 *
 * This sets the default behavior for all eval.c functions. It **must** call
 * bott eval_reset_vars() and eval_reset_stats()
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

question_t questions[] = {
    {"1.1",   "Valida se ficheiro nameDB existe (sucesso)", 0.0 },
    {"1.2",   "Valida se ficheiro nameDB existe (erro: não existe)", 0.0 },

    {"2.1",   "Cria FIFO nameFifo (sucesso)", 0.0 },
    {"2.2",   "Cria FIFO nameFifo (erro)", 0.0 },

    {"3",   "Arma sinais (sucesso)", 0.0 },

    {"4.1",   "Lê pedido do FIFO ", 0.0 },
    {"4.2",   "Lê pedido (erro)", 0.0 },

    {"5.1",   "Cria servidor dedicado (parent)", 0.0 },
    {"5.2",   "Cria servidor dedicado (child)", 0.0 },
    {"5.3",   "Cria servidor dedicado (erro)", 0.0 },

    {"6",   "Apaga Fifo e sai", 0.0 },

    {"7.a",   "Shutdown servidor (sucesso)", 0.0 },
    {"7.b",   "Shutdown servidor (erro)", 0.0 },

    {"8",   "Termina servidor dedicado", 0.0 },

    {"9",   "Arma sinais", 0.0 },

    {"10.1",   "Valida pedido (sucesso)", 0.0 },
    {"10.2",   "Valida pedido (erro)", 0.0 },

    {"11.a",   "Procura utilizador (ok)", 0.0 },
    {"11.b",   "Procura utilizador (bad password)", 0.0 },
    {"11.c",   "Procura utilizador (not found)", 0.0 },

    {"12.a",   "Reserva utilizador (missing)", 0.0 },
    {"12.b",   "Reserva utilizador (ok)", 0.0 },
    {"12.c",   "Reserva utilizador (DB error)", 0.0 },

    {"13.1",   "Cria FIFO (sucesso)", 0.0 },
    {"13.2",   "Cria FIFO (erro)", 0.0 },

    {"14.1",   "Envia Ack. Login (sucesso)", 0.0 },
    {"14.2",   "Envia Ack. Login (erro)", 0.0 },

    {"15.1",   "Lê fim sessão (sucesso)", 0.0 },
    {"15.2",   "Lê fim sessão (erro)", 0.0 },

    {"16.1",   "Termina a sessão com o cliente (sucesso)", 0.0 },
    {"16.2",   "Termina a sessão com o cliente (erro DB)", 0.0 },

    {"17",   "Remove FIFO e Termina servidor dedicado", 0.0 },

    {"18",   "Recebe sinal SIGUSR1", 0.0 },

    {"---", "_end_",0.0}
};

/* Variáveis globais */
Login clientRequest; // Variável que tem o pedido enviado do Cliente para o Servidor
int index_client;    // Índice do cliente que fez o pedido ao servidor/servidor dedicado na BD


void print_login( char *s, Login r ) {
    if (s) printf("%s\n", s);
    printf("nif                   : %d\n", r.nif);
    printf("senha                 : %s\n", r.senha);
    printf("nome                  : %s\n", r.nome);
    printf("saldo                 : %d\n", r.saldo);
    printf("pid_cliente           : %d\n", r.pid_cliente);
    printf("pid_servidor_dedicado : %d\n", r.pid_servidor_dedicado);
}

void createTestDB() {

    Login l = {
        .nif = -1,
        .saldo = -1,
        .pid_cliente = -1,
        .pid_servidor_dedicado = -1
    };

    FILE *f = fopen( FILE_DATABASE, "w" );

    l.nif = 1000001;
    strcpy( l.senha, "pass1");
    strcpy( l.nome, "Name1 Surname1");
    l.saldo = 101;
    l.pid_cliente = -1;
    l.pid_servidor_dedicado = -1;

    fwrite( &l, sizeof(Login), 1, f );

    l.nif = 1000002;
    strcpy( l.senha, "pass2");
    strcpy( l.nome, "Name2 Surname2");
    l.saldo = 102;
    l.pid_cliente = -1;
    l.pid_servidor_dedicado = -1;

    fwrite( &l, sizeof(Login), 1, f );

    l.nif = 1000003;
    strcpy( l.senha, "pass3");
    strcpy( l.nome, "Name3 Surname3");
    l.saldo = 103;
    l.pid_cliente = -1;
    l.pid_servidor_dedicado = 2004;

    fwrite( &l, sizeof(Login), 1, f );

    l.nif = 1000004;
    strcpy( l.senha, "pass4");
    strcpy( l.nome, "Name4 Surname4");
    l.saldo = 104;
    l.pid_cliente = -1;
    l.pid_servidor_dedicado = -1;

    fwrite( &l, sizeof(Login), 1, f );

    fclose( f );


}

void removeTestDB() {
    unlink( FILE_DATABASE );
}


struct {
    int success;
} _existsDB_S1_data;

int existsDB_S1(char *nameDB) {
    _existsDB_S1_data.success = -1234;
    _existsDB_S1_data.success = _student_existsDB_S1( nameDB );
    return _existsDB_S1_data.success;
}

/**
 * Evaluate S1
 **/
int eval_s1( ) {

    eval_info("Evaluating S1.1 - %s...", question_text(questions,"1.1"));
    eval_reset();

    char nameDB[] = "fake.db";
    FILE *f = fopen( nameDB, "w" );
    fclose(f);

    EVAL_CATCH( existsDB_S1( nameDB ) );

    if ( _NO_EXIT != _eval_env.stat ) {
        eval_error( "(S1) bad termination");
    }

    if ( _SUCCESS != _existsDB_S1_data.success ) {
        eval_error("(S1) existsDB_S1 returned error with existing file");
    }

    eval_check_successlog( "(S1)", "(S1) " );

    unlink( nameDB );

    eval_info("Evaluating S1.2 - %s...", question_text(questions,"1.2"));
    eval_reset_except_stats();

    EVAL_CATCH( existsDB_S1( nameDB ) );

    if ( _EXIT != _eval_env.stat && _ERROR != _existsDB_S1_data.success ) {
        eval_error("(S1) existsDB_S1 returned success with missing file");
    }

    eval_check_errorlog( "(S1)", "(S1) " );

    eval_close_logs( "(S1)" );
    return eval_complete("(S1)");
}

struct {
    int success;
} _createFifo_S2_data;

int createFifo_S2(char *nameFifo) {
    _createFifo_S2_data.success = -1234;
    _createFifo_S2_data.success = _student_createFifo_S2( nameFifo );
    return _createFifo_S2_data.success;
}

/**
 * Evaluate S2
 **/
int eval_s2( ) {

    eval_info("Evaluating S2.1 - %s...", question_text(questions,"2.1"));
    eval_reset();

    char nameFifo[] = "eval.fifo";

    _eval_mkfifo_data.action = 1;
    _eval_mkfifo_data.status = 0;

    EVAL_CATCH( createFifo_S2( nameFifo ) );

    if ( _NO_EXIT != _eval_env.stat ) {
        eval_error( "(S2) bad termination");
    }

    if ( _SUCCESS != _createFifo_S2_data.success ) {
        eval_error("(S2) Bad return value");
    }

    if ( 1 == _eval_mkfifo_data.status ) {
        if ( strncmp( nameFifo, _eval_mkfifo_data.path, strlen(nameFifo) ) ) {
            eval_error( "(S2) FIFO created with invalid name");
        }
        if ( ! ( 0600 & _eval_mkfifo_data.mode ) ) {
            eval_error( "(S2) FIFO created with invalid mode, must be r/w by owner");
        }

        eval_check_successlog( "(S2)","(S2) " );

    } else {
        eval_error("(S2) mkfifo() should have been called exactly once");
    }

    // CC: Commented this test because some implementations may try to remove the FIFO before creating it (for security reasons)
    // eval_info("Evaluating S2.2 - %s...", question_text(questions,"2.2"));
    // eval_reset_except_stats();

    // _eval_mkfifo_data.action = 0;
    // _eval_mkfifo_data.status = 0;

    // Check with existing file
    // FILE *f = fopen( nameFifo, "w" );
    // fclose(f);

    // EVAL_CATCH( createFifo_S2( nameFifo ) );

    // if ( _EXIT != _eval_env.stat && _ERROR != _createFifo_S2_data.success ) {
    //     eval_error("(S1) createFifo_S2 returned success with missing file");
    // }

    // if ( _NO_EXIT != _eval_env.stat ) {
    //     eval_error( "(S2) bad termination");
    // }

    // if ( _ERROR != _createFifo_S2_data.success ) {
    //     eval_error("(S2) createFifo_S2 returned success with existing FIFO");
    // }

    // eval_check_errorlog( "(S2)","(S2) " );

    // unlink( nameFifo );

    eval_close_logs( "(S2)" );
    return eval_complete("(S2)");
}

struct {
    int success;
} _triggerSignals_S3_data;

int triggerSignals_S3() {
    _triggerSignals_S3_data.success = -1234;
    _triggerSignals_S3_data.success = _student_triggerSignals_S3();
    return _triggerSignals_S3_data.success;
}

/**
 * Evaluate S3
 **/
int eval_s3( ) {

    eval_info("Evaluating S3 - %s...", question_text(questions,"3"));
    eval_reset();

    initlog(&_data_log);
    _eval_signal_data.action = 2;

    EVAL_CATCH( triggerSignals_S3( ) );

    if ( _NO_EXIT != _eval_env.stat ) {
        eval_error( "(S3) bad termination");
    }

    if ( _SUCCESS != _triggerSignals_S3_data.success ) {
        eval_error("(S3) Bad return value");
    }

    char buffer[100];

    snprintf( buffer, 100, "signal,%d,%p", SIGINT, trataSinalSIGINT_S7 );
    if ( _NOT_FOUND > findinlog( &_data_log, buffer )) {
        eval_error("(S3) SIGINT not armed properly");
    }

    snprintf( buffer, 100, "signal,%d,%p", SIGCHLD, trataSinalSIGCHLD_S8 );
    if ( _NOT_FOUND > findinlog( &_data_log, buffer )) {
        eval_error("(S3) SIGCHLD not armed properly");
    }

    eval_check_successlog( "(S3)", "(S3) " );

    eval_close_logs( "(S3)" );
    return eval_complete("(S3)");
}

struct {
    Login r;
} _readRequest_S4_data;

Login readRequest_S4(char *nameFifo) {
    _readRequest_S4_data.r = (Login) {.nif = -1234};
    _readRequest_S4_data.r = _student_readRequest_S4( nameFifo );
    return _readRequest_S4_data.r;
}

/**
 * Evaluate S4
 **/
int eval_s4( ) {

    eval_info("Evaluating S4.1 - %s...", question_text(questions,"4.1"));
    eval_reset();

    Login r = {
        .nif = 12345678,
        .senha = "abc123xyz",
        .nome = "José Silva",
        .saldo = 1234,
        .pid_cliente = 16384,
        .pid_servidor_dedicado = 32768
    };

    char nameFifo[] = "eval.fifo";
    FILE *f = fopen( nameFifo, "w" );
    fwrite( &r, sizeof(Login), 1, f );
    fclose(f);

    EVAL_CATCH( readRequest_S4( nameFifo ) );

    if ( _NO_EXIT != _eval_env.stat ) {
        eval_error( "(S4) bad termination");
    }

    if ( _readRequest_S4_data.r.nif < 0 ) {
        eval_error("(S4) Bad return value");
    }

    if ( _readRequest_S4_data.r.nif != r.nif ||
        strcmp(_readRequest_S4_data.r.senha, r.senha) ||
        strcmp(_readRequest_S4_data.r.nome, r.nome) ||
        _readRequest_S4_data.r.saldo != r.saldo ||
        _readRequest_S4_data.r.pid_cliente != r.pid_cliente ) {
        eval_error("(S4) Data read from file does not match supplied data");
    }

    char buffer[1024];
    snprintf(buffer,1024,"(S4) %d %s %d", r.nif, r.senha, r.pid_cliente );
    eval_check_successlog( "(S4)", buffer );

    unlink( nameFifo );

    eval_info("Evaluating C4.2 - %s...", question_text(questions,"4.2"));
    eval_reset_except_stats();

    EVAL_CATCH( readRequest_S4(nameFifo) );

    if ( _NO_EXIT != _eval_env.stat ) {
        eval_error( "(S4) bad termination");
    }

    if ( -1 != _readRequest_S4_data.r.nif ) {
        eval_error( "(S4) bad return value");
    }

    eval_check_errorlog( "(S4)", "(S4) " );

    eval_close_logs( "(S4)" );
    return eval_complete("(S4)");
}

struct {
    int pid_filho;
} _createServidorDedicado_S5_data;

int createServidorDedicado_S5() {
    _createServidorDedicado_S5_data.pid_filho = -1234;
    _createServidorDedicado_S5_data.pid_filho = _student_createServidorDedicado_S5();
    return _createServidorDedicado_S5_data.pid_filho;
}

/**
 * Evaluate S5
 **/
int eval_s5( ) {

    eval_info("Evaluating 5.1 - %s...", question_text(questions,"5.1"));
    eval_reset();

    // Test parent (dummy fork() will return 1)
    _eval_fork_data.action = 2;
    EVAL_CATCH( createServidorDedicado_S5( ) );

    if ( _NO_EXIT != _eval_env.stat ) {
        eval_error( "(S5) bad termination");
    }

    if ( 1 != _createServidorDedicado_S5_data.pid_filho ) {
        eval_error("(S5) Bad return value");
    }

    char buffer[1024];
    snprintf(buffer,1024,"(S5) Servidor Dedicado: PID %d", 1 );
    eval_check_successlog( "(S5)", buffer );

    eval_info("Evaluating 5.2 - %s...", question_text(questions,"5.2"));
    eval_reset_except_stats();

    // Test child (dummy fork() will return 0)
    _eval_fork_data.action = 1;
    EVAL_CATCH( createServidorDedicado_S5( ) );

    if ( _NO_EXIT != _eval_env.stat ) {
        eval_error( "(S5) bad termination");
    }

    if ( 0 != _createServidorDedicado_S5_data.pid_filho ) {
        eval_error("(S5) Bad return value");
    }

    eval_info("Evaluating 5.3 - %s...", question_text(questions,"5.3"));
    eval_reset_except_stats();

    // Test error (dummy fork() will return -1)
    _eval_fork_data.action = 3;
    EVAL_CATCH( createServidorDedicado_S5( ) );

    if ( _NO_EXIT != _eval_env.stat ) {
        eval_error( "(S5) bad termination");
    }

    if ( -1 != _createServidorDedicado_S5_data.pid_filho ) {
        eval_error("(S5) Bad return value");
    }

    eval_check_errorlog( "(S5)", "(S5) " );

    eval_close_logs( "(S5)" );
    return eval_complete("(S5)");
}

struct {
    int status;
    int action;
} _deleteFifoAndExit_S6_data;

void deleteFifoAndExit_S6() {
    _deleteFifoAndExit_S6_data.status++;
    if ( _deleteFifoAndExit_S6_data.action == 0 )
        _student_deleteFifoAndExit_S6();
}

/**
 * Evaluate S6
 **/
int eval_s6( ) {

    eval_info("Evaluating S6 - %s...", question_text(questions,"6"));
    eval_reset();

    _eval_unlink_data.status = 0;
    _eval_unlink_data.action = 1;

    _eval_remove_data.status = 0;
    _eval_remove_data.action = 1;

    _deleteFifoAndExit_S6_data.action = 0;

    EVAL_CATCH( deleteFifoAndExit_S6() );

    if ( _EXIT != _eval_env.stat ) {
        eval_error( "(S6) bad termination, should have ended with exit call");
    } else {
        if ( 0 != _eval_exit_data.status ) {
            eval_error( "(S6) exit() called with wrong value");
        }
    }

    if ( 1 != _eval_unlink_data.status && 1 != _eval_remove_data.status  ) {
        eval_error("(S6) unlink()/remove() should have been called exactly once");
    } else {
        if (1 == _eval_unlink_data.status) {
            if (strncmp( _eval_unlink_data.path, FILE_REQUESTS, 1024 ) ) {
                eval_error("(S6) unlink() called with invalid path");
            }
        } else {
            if (strncmp( _eval_remove_data.path, FILE_REQUESTS, 1024 ) ) {
                eval_error("(S6) remove() called with invalid path");
            }
        }
    }

    eval_check_successlog( "(S6)", "(S6) " );

    eval_close_logs( "(S6)" );
    return eval_complete("(S6)");
}

void trataSinalSIGINT_S7(int sinalRecebido) {
    _student_trataSinalSIGINT_S7( sinalRecebido );
}

/**
 * Evaluate S7
 **/
int eval_s7( ) {

    eval_info("Evaluating S7.a - %s...", question_text(questions,"7.a"));
    eval_reset();

    _eval_unlink_data.status = 0;
    _eval_unlink_data.action = 1;

    _eval_kill_data.status = 0;
    _eval_kill_data.action = 2;

    _deleteFifoAndExit_S6_data.status = 0;
    _deleteFifoAndExit_S6_data.action = 1;

    // Only 1 request is active
    createTestDB();

    EVAL_CATCH( trataSinalSIGINT_S7( SIGINT ) );

    if ( _EXIT != _eval_env.stat ) {
        eval_error( "(S7) bad termination");
    }

    eval_check_successlog( "(S7)", "(S7) Shutdown Servidor" );
    eval_check_successlog( "(S7.1)", "(S7.1) " );
    eval_check_successlog( "(S7.4)", "(S7.4) " );

    if ( 1 != _deleteFifoAndExit_S6_data.status ) {
        eval_error("(S7) deleteFifoAndExit_S6() should have been called");
    }

    if ( 1 != _eval_kill_data.status ) {
        eval_error("(S7) kill() should have been called once for this test data");
    } else {
        if ( 2004 != _eval_kill_data.pid ) {
            eval_error("(S7) kill() called with wrong PID");
        }
        if ( SIGUSR1 != _eval_kill_data.sig ) {
            eval_error("(S7) kill() called with wrong signal number");
        }
    }

    removeTestDB();

    eval_info("Evaluating S7.b - %s...", question_text(questions,"7.b"));
    eval_reset_except_stats();

    _eval_unlink_data.status = 0;
    _eval_unlink_data.action = 1;

    _eval_kill_data.status = 0;
    _eval_kill_data.action = 2;

    _deleteFifoAndExit_S6_data.status = 0;
    _deleteFifoAndExit_S6_data.action = 1;

    EVAL_CATCH( trataSinalSIGINT_S7( SIGINT ) );

    if ( _EXIT != _eval_env.stat ) {
        eval_error( "(S7) bad termination");
    }

    eval_check_successlog( "(S7)", "(S7) Shutdown Servidor" );
    eval_check_errorlog( "(S7.1)", "(S7.1) " );

    if ( 1 != _deleteFifoAndExit_S6_data.status ) {
        eval_error("(S7) deleteFifoAndExit_S6() should have been called");
    }

    if ( 0 != _eval_kill_data.status ) {
        eval_error("(S7) kill() should have NOT been called once for this test data");
    }

    eval_close_logs( "(S7)" );
    return eval_complete("(S7)");
}

void trataSinalSIGCHLD_S8(int sinalRecebido) {
    _student_trataSinalSIGCHLD_S8( sinalRecebido );
}

/**
 * Evaluate S8
 **/
int eval_s8( ) {

    eval_info("Evaluating S8 - %s...", question_text(questions,"8"));
    eval_reset();

    int pid_servidor_dedicado = 16384;

    _eval_wait_data.status = 0;
    _eval_wait_data.action = 2;
    _eval_wait_data.ret = pid_servidor_dedicado;

    EVAL_CATCH( trataSinalSIGCHLD_S8( SIGCHLD ) );

    if ( _NO_EXIT != _eval_env.stat ) {
        eval_error( "(S8) bad termination");
    }

    if ( 1 != _eval_wait_data.status ) {
        eval_error("(S1) wait() should have been called exactly once");
    }

    char buffer[1024];
    snprintf(buffer,1024,"(S8) Terminou Servidor Dedicado %d", pid_servidor_dedicado );
    eval_check_successlog( "(S8)", buffer );

    eval_close_logs( "(S8)" );
    return eval_complete("(S8)");
}

struct {
    int success;
} _triggerSignals_SD9_data;

int triggerSignals_SD9() {
    _triggerSignals_SD9_data.success = -1234;
    _triggerSignals_SD9_data.success = _student_triggerSignals_SD9();
    return _triggerSignals_SD9_data.success;
}

/**
 * Evaluate SD9
 **/
int eval_sd9( ) {

    eval_info("Evaluating SD9 - %s...", question_text(questions,"9"));
    eval_reset();

    initlog(&_data_log);
    _eval_signal_data.action = 2;

    EVAL_CATCH( triggerSignals_SD9( ) );

    if ( _NO_EXIT != _eval_env.stat ) {
        eval_error( "(SD9) bad termination");
    }

    if ( _SUCCESS != _triggerSignals_SD9_data.success ) {
        eval_error("(SD9) Bad return value");
    }

    char buffer[100];

    snprintf( buffer, 100, "signal,%d,%p", SIGUSR1, trataSinalSIGUSR1_SD18 );
    if ( _NOT_FOUND > findinlog( &_data_log, buffer )) {
        eval_error("(SD9) SIGUSR1 not armed properly");
    }

    snprintf( buffer, 100, "signal,%d,%p", SIGINT, SIG_IGN );
    if ( _NOT_FOUND > findinlog( &_data_log, buffer )) {
        eval_error("(SD9) SIGINT not armed properly");
    }

    eval_check_successlog( "(SD9)", "(SD9) " );

    eval_close_logs( "(SD9)" );
    return eval_complete("(SD9)");
}

struct {
    int success;
} _validaPedido_SD10_data;

int validaPedido_SD10(Login request) {
    _validaPedido_SD10_data.success = -1234;
    _validaPedido_SD10_data.success = _student_validaPedido_SD10( request );
    return _validaPedido_SD10_data.success;
}

/**
 * Evaluate SD10
 **/
int eval_sd10( ) {

    eval_info("Evaluating SD10.1 - %s...", question_text(questions,"10.1"));
    eval_reset();

    Login request = {.pid_cliente = 1234};

    EVAL_CATCH( validaPedido_SD10( request ) );

    if ( _NO_EXIT != _eval_env.stat ) {
        eval_error( "(SD10) bad termination");
    }

    if ( _SUCCESS != _validaPedido_SD10_data.success ) {
        eval_error("(SD10) Bad return value");
    }

    eval_check_successlog( "(SD10)", "(SD10) " );

    eval_info("Evaluating SD10.1 - %s...", question_text(questions,"10.2"));
    eval_reset_except_stats();

    request.pid_cliente = -1;

    EVAL_CATCH( validaPedido_SD10( request ) );

    if ( _EXIT != _eval_env.stat && _ERROR != _validaPedido_SD10_data.success ) {
        eval_error("(SD10) Bad return value");
    }

    eval_check_errorlog( "(SD10)", "(SD10) " );

    eval_close_logs( "(SD10)" );
    return eval_complete("(SD10)");
}

/**
 * buildNomeFifo is not checked
 */

int buildNomeFifo(char *buffer, int buffer_size, char *prefix, int pid, char *suffix) {
    snprintf(buffer, buffer_size, "%s%d%s", prefix, pid, suffix );
    return 0;
}

struct {
    int index_client;
} _procuraUtilizadorBD_SD11_data;

int procuraUtilizadorBD_SD11(Login request, char *nameDB, Login *itemDB) {
    _procuraUtilizadorBD_SD11_data.index_client = -1234;
    _procuraUtilizadorBD_SD11_data.index_client = _student_procuraUtilizadorBD_SD11( request, nameDB, itemDB );
    return _procuraUtilizadorBD_SD11_data.index_client;
};

/**
 * Evaluate SD11
 **/
int eval_sd11( ) {

    eval_info("Evaluating SD11.a - %s...", question_text(questions,"11.a"));
    eval_reset();

    createTestDB();

    Login request = {.nif = 1000004, .senha = "pass4"};
    char nameDB[] = FILE_DATABASE;
    Login itemDB;

    EVAL_CATCH( procuraUtilizadorBD_SD11( request, nameDB, &itemDB ) );

    if ( _NO_EXIT != _eval_env.stat ) {
        eval_error( "(SD11) bad termination");
    }

    // User is in position 3
    if ( 3 != _procuraUtilizadorBD_SD11_data.index_client ) {
        eval_error("(SD11) Bad return value");
    }

    eval_check_successlog( "(SD11)", "(SD11) " );
    eval_check_successlog( "(SD11.5)", "(SD11.5) 3" );

    eval_info("Evaluating SD11.b - %s...", question_text(questions,"11.b"));
    eval_reset_except_stats();

    // Test with bad password
    strcpy( request.senha, "wrong!" );

    EVAL_CATCH( procuraUtilizadorBD_SD11( request, nameDB, &itemDB ) );

    if ( _NO_EXIT != _eval_env.stat ) {
        eval_error( "(SD11) bad termination");
    }

    // Bad password returns missing user
    if ( -1 != _procuraUtilizadorBD_SD11_data.index_client ) {
        eval_error("(SD11) Bad return value");
    }

    eval_check_successlog( "(SD11)", "(SD11) " );
    eval_check_errorlog( "(SD11.5)", "(SD11.5) " );

    eval_info("Evaluating SD11.c - %s...", question_text(questions,"11.c"));
    eval_reset_except_stats();

    request.nif = 2000000;

    EVAL_CATCH( procuraUtilizadorBD_SD11( request, nameDB, &itemDB ) );

    if ( _NO_EXIT != _eval_env.stat ) {
        eval_error( "(SD11) bad termination");
    }

    // Missing user
    if ( -1 != _procuraUtilizadorBD_SD11_data.index_client ) {
        eval_error("(SD11) Bad return value");
    }

    eval_check_successlog( "(SD11)", "(SD11) " );
    eval_check_errorlog( "(SD11.2)", "(SD11.2) " );

    removeTestDB();

    eval_close_logs( "(S11)" );
    return eval_complete("(S11)");
}

struct {
    int success;
} _reservaUtilizadorBD_SD12_data;

int reservaUtilizadorBD_SD12(Login *request, char *nameDB, int index_client, Login itemDB) {
    _reservaUtilizadorBD_SD12_data.success = -1234;
    _reservaUtilizadorBD_SD12_data.success = _student_reservaUtilizadorBD_SD12( request, nameDB, index_client, itemDB );
    return  _reservaUtilizadorBD_SD12_data.success;
}

/**
 * Evaluate SD12
 **/
int eval_sd12( ) {


    eval_info("Evaluating SD12.a - %s...", question_text(questions,"12.a"));
    eval_reset();

    Login itemDB;

    itemDB.nif = 1000002;
    strcpy( itemDB.senha, "pass2");
    strcpy( itemDB.nome, "Name2 Surname2");
    itemDB.saldo = 102;
    itemDB.pid_cliente = -1;
    itemDB.pid_servidor_dedicado = -1;

    Login request = {
        .nif = 1000002,
        .senha = "pass2",
        .pid_cliente = -1,
        .pid_servidor_dedicado = -1234,
    };

    EVAL_CATCH( reservaUtilizadorBD_SD12( &request, FILE_DATABASE, -1, itemDB ) );

    if ( _NO_EXIT != _eval_env.stat ) {
        eval_error( "(SD12) bad termination");
    }

    if ( _SUCCESS != _reservaUtilizadorBD_SD12_data.success ) {
        eval_error("(SD12) Bad return value");
    }

    if ( -1 != request.pid_servidor_dedicado ) {
        eval_error("(SD12) Invalid return value for.pid_servidor_dedicado ");
    }

    eval_info("Evaluating SD12.b - %s...", question_text(questions,"12.b"));
    eval_reset_except_stats();

    createTestDB();

    int index_client = 1;

    EVAL_CATCH( reservaUtilizadorBD_SD12( &request, FILE_DATABASE, index_client, itemDB ) );

    if ( _NO_EXIT != _eval_env.stat ) {
        eval_error( "(SD12) bad termination");
    }

    if ( _SUCCESS != _reservaUtilizadorBD_SD12_data.success ) {
        eval_error("(SD12) Bad return value");
    }

    eval_check_successlog( "(SD12.2)", "(SD12.2) " );
    eval_check_successlog( "(SD12.3)", "(SD12.3) " );
    eval_check_successlog( "(SD12.4)", "(SD12.4) " );

    FILE *f = fopen( FILE_DATABASE, "r" );
    if ( f == NULL ) {
        eval_error("(SD12) Unable ot open file %s for testing", FILE_DATABASE);
    } else {
        Login r;

        fseek( f, index_client * sizeof(Login), SEEK_SET );
        fread( &r, sizeof(Login), 1, f );

        if ( r.nif != 1000002 )
            eval_error("(SD12) .nif is not correct in file record");

        if ( strcmp(r.senha, "pass2") )
            eval_error("(SD12) .nif is not correct in file record");
        if ( strcmp(r.nome, "Name2 Surname2") )
            eval_error("(SD12) .nome is not correct in file record");
        if ( r.saldo != 102 )
            eval_error("(SD12) .saldo is not correct in file record");
        if ( r.pid_cliente != -1 )
            eval_error("(SD12) .pid_cliente is not correct in file record");
        if ( r.pid_servidor_dedicado != getpid() )
            eval_error("(SD12) .pid_servidor_dedicado is not correct in file record");

        fclose(f);
    }

    removeTestDB();

    eval_info("Evaluating SD12.c - %s...", question_text(questions,"12.c"));
    eval_reset_except_stats();

    EVAL_CATCH( reservaUtilizadorBD_SD12( &request, FILE_DATABASE, index_client, itemDB ) );

    if ( _EXIT != _eval_env.stat && _ERROR != _reservaUtilizadorBD_SD12_data.success ) {
        eval_error("(SD12) Bad return value with missing DB");
    }

    eval_check_errorlog( "(SD12.2)", "(SD12.2) " );

    eval_close_logs( "(SD12)" );
    return eval_complete("(S12)");
}

struct {
    int success;
} _createFifo_SD13_data;

int createFifo_SD13(char *nameFifo) {
    _createFifo_SD13_data.success = -1234;
    _createFifo_SD13_data.success = _student_createFifo_SD13( nameFifo );
    return _createFifo_SD13_data.success;
}

/**
 * Evaluate SD13
 **/
int eval_sd13( ) {

    eval_info("Evaluating SD13.1 - %s...", question_text(questions,"13.1"));
    eval_reset();

    char nameFifo[1024];

    _eval_mkfifo_data.action = 1;
    _eval_mkfifo_data.status = 0;

    EVAL_CATCH( createFifo_SD13( nameFifo ) );

    if ( _NO_EXIT != _eval_env.stat ) {
        eval_error( "(SD13) bad termination");
    }

    if ( _SUCCESS != _createFifo_SD13_data.success ) {
        eval_error("(SD13) Bad return value");
    }

    if ( 1 == _eval_mkfifo_data.status ) {
        if ( strncmp( nameFifo, _eval_mkfifo_data.path, strlen(nameFifo) ) ) {
            eval_error( "(SD13) FIFO created with invalid name");
        }
        if ( ! ( 0600 & _eval_mkfifo_data.mode ) ) {
            eval_error( "(SD13) FIFO created with invalid mode, must be r/w by owner");
        }

        eval_check_successlog( "(SD13)","(SD13) " );

    } else {
        eval_error("(SD13) mkfifo() should have been called exactly once");
    }

    char buffer[1024];
    buildNomeFifo(buffer, 20, FILE_PREFIX_SRVDED, getpid(), FILE_SUFFIX_FIFO);
    if ( strncmp( nameFifo, buffer, 20) ) {
        eval_error("(SD13) wrong file name created");
    }

    eval_info("Evaluating SD13.2 - %s...", question_text(questions,"13.2"));
    eval_reset_except_stats();

    _eval_mkfifo_data.action = 0;
    _eval_mkfifo_data.status = 0;

    // Check with existing file
    FILE *f = fopen( nameFifo, "w" );
    fclose(f);

    EVAL_CATCH( createFifo_SD13( nameFifo ) );

    if ( _EXIT != _eval_env.stat && _ERROR != _createFifo_SD13_data.success ) {
        eval_error("(SD13) createFifo_C4 returned success with existing FIFO");
    }

    eval_check_errorlog( "(SD13)","(SD13) " );

    unlink( nameFifo );

    eval_close_logs( "(SD13)" );
    return eval_complete("(SD13)");
}

struct {
    int success;
} _sendAckLogin_SD14_data;

int sendAckLogin_SD14(Login ackLogin, char *nameFifo) {
    _sendAckLogin_SD14_data.success = -1234;
    _sendAckLogin_SD14_data.success = _student_sendAckLogin_SD14( ackLogin, nameFifo );
    return _sendAckLogin_SD14_data.success;
}

struct {
    int status;
    int action;
} _deleteFifoAndExit_SD17_data;

/**
 * Evaluate SD14
 **/
int eval_sd14( ) {

    eval_info("Evaluating SD14.1 - %s...", question_text(questions,"14.1"));
    eval_reset();

    _deleteFifoAndExit_SD17_data.status = 0;
    _deleteFifoAndExit_SD17_data.action = 1;

    Login ackLogin = {
        .nif = 12345678,
        .senha = "abc123xyz",
        .nome = "José Silva",
        .saldo = 1234,
        .pid_cliente = 16384,
        .pid_servidor_dedicado = 32768
    };
    char nameFifo[1024];

    EVAL_CATCH( sendAckLogin_SD14( ackLogin, nameFifo ) );

    if ( _NO_EXIT != _eval_env.stat ) {
        eval_error( "(SD14) bad termination");
    }

    if ( _SUCCESS != _sendAckLogin_SD14_data.success ) {
        eval_error("(SD14) Bad return value");
    }

    char buffer[1024];
    buildNomeFifo(buffer, 20, FILE_PREFIX_CLIENT, ackLogin.pid_cliente, FILE_SUFFIX_FIFO);
    if ( strncmp( nameFifo, buffer, 20) ) {
        eval_error("(SD14) wrong file name created");
    }

    FILE * f = fopen( nameFifo, "r" );
    if ( f == NULL ) {
        eval_error("(SD14) Unable to open output file for testing");
    } else {
        Login l;
        if ( fread( &l, sizeof(Login), 1, f ) < 1 ) {
            eval_error("(SD14) Unable to read output file for testing");
        } else {
            if ( ackLogin.nif != l.nif ||
                 strcmp(ackLogin.senha, l.senha) ||
                 strcmp(ackLogin.nome, l.nome) ||
                 ackLogin.saldo != l.saldo ||
                 ackLogin.pid_cliente != l.pid_cliente ||
                 ackLogin.pid_servidor_dedicado != l.pid_servidor_dedicado) {
                eval_error("(SD14) Data in file does not match supplied data");
            }
        }
        fclose(f);
    }

    // Remove I/O files
    unlink( nameFifo );

    if ( _deleteFifoAndExit_SD17_data.status > 0 ) {
        eval_error( "(SD14) deleteFifoAndExit_SD17() should NOT have been called");
    }

    eval_check_successlog( "(SD14)", "(SD14) " );

    eval_info("Evaluating SD14.2 - %s...", question_text(questions,"14.2"));
    eval_reset_except_stats();

    _deleteFifoAndExit_SD17_data.status = 0;
    _deleteFifoAndExit_SD17_data.action = 1;

    // Check with locked file
    f = fopen( nameFifo, "w" );
    fclose(f);
    chmod( nameFifo, 0000 );

    EVAL_CATCH( sendAckLogin_SD14( ackLogin, nameFifo ) );

    if ( _EXIT != _eval_env.stat && _ERROR != _sendAckLogin_SD14_data.success ) {
        eval_error("(SD14) sendAckLogin_SD14 returned success on write fail");
    }

    if ( 1 != _deleteFifoAndExit_SD17_data.status ) {
        eval_error( "(SD14) deleteFifoAndExit_SD17() should have been called exactly once");
    }

    eval_check_errorlog( "(SD14)","(SD14) " );

    chmod( nameFifo, 0666 );
    unlink( nameFifo );

    eval_close_logs( "(SD14)" );
    return eval_complete("(SD14)");
}

struct {
    int success;
} _readFimSessao_SD15_data;

int readFimSessao_SD15(char * nameFifo) {
    _readFimSessao_SD15_data.success = -1234;
    _readFimSessao_SD15_data.success = _student_readFimSessao_SD15( nameFifo );
    return _readFimSessao_SD15_data.success;
}

/**
 * Evaluate SD15
 **/
int eval_sd15( ) {

    eval_info("Evaluating SD15.1 - %s...", question_text(questions,"15.1"));
    eval_reset();

    _deleteFifoAndExit_SD17_data.status = 0;
    _deleteFifoAndExit_SD17_data.action = 1;

    char nameFifo[] = "name.fifo";
    FILE *f = fopen( nameFifo, "w");
    fprintf(f,"abc123\n");
    fclose(f);

    EVAL_CATCH( readFimSessao_SD15( nameFifo ) );

    if ( _NO_EXIT != _eval_env.stat ) {
        eval_error( "(SD15) bad termination");
    }

    if ( _SUCCESS != _readFimSessao_SD15_data.success ) {
        eval_error("(SD15) Bad return value");
    }

    if ( _deleteFifoAndExit_SD17_data.status > 0 ) {
        eval_error( "(SD15) deleteFifoAndExit_SD17() should NOT have been called");
    }

    eval_check_successlog( "(SD15)","(SD15) abc123" );

    unlink( nameFifo );

    eval_info("Evaluating SD15.2 - %s...", question_text(questions,"15.2"));
    eval_reset_except_stats();

    _deleteFifoAndExit_SD17_data.status = 0;
    _deleteFifoAndExit_SD17_data.action = 1;

    EVAL_CATCH( readFimSessao_SD15( nameFifo ) );

    if ( _EXIT != _eval_env.stat && _ERROR != _readFimSessao_SD15_data.success ) {
        eval_error("(SD15) Bad return value (success on missing file)");
    }

    if ( 1 != _deleteFifoAndExit_SD17_data.status ) {
        eval_error( "(SD15) deleteFifoAndExit_SD17() should have been called exactly once");
    }

    eval_check_errorlog( "(SD15)","(SD15) " );

    eval_close_logs( "(SD15)" );
    return eval_complete("(SD15)");
}

struct {
    int success;
    int action;
    int status;

    Login *clientRequest;
    char *nameDB;
    int index_client;

} _terminaSrvDedicado_SD16_data;

int terminaSrvDedicado_SD16(Login *clientRequest, char *nameDB, int index_client) {
    _terminaSrvDedicado_SD16_data.status++;
    if ( _terminaSrvDedicado_SD16_data.action == 0 ) {
        _terminaSrvDedicado_SD16_data.success = -1234;
        _terminaSrvDedicado_SD16_data.success = _student_terminaSrvDedicado_SD16( clientRequest, nameDB, index_client );
    } else {
        _terminaSrvDedicado_SD16_data.clientRequest = clientRequest;
        _terminaSrvDedicado_SD16_data.nameDB = nameDB;
        _terminaSrvDedicado_SD16_data.index_client = index_client;

        _terminaSrvDedicado_SD16_data.success = 0;
    }
    return _terminaSrvDedicado_SD16_data.success;
}

/**
 * Evaluate S1D16
 **/
int eval_sd16( ) {

    eval_info("Evaluating SD16.1 - %s...", question_text(questions,"16.1"));
    eval_reset();

    _terminaSrvDedicado_SD16_data.action = 0;
    _eval_unlink_data.action = 1;

    createTestDB();

    char nameDB[] = FILE_DATABASE;

    Login l;

    l.nif = 1000003;
    strcpy( l.senha, "pass3");
    strcpy( l.nome, "Name3 Surname3");
    l.saldo = 103;
    l.pid_cliente = 16384;
    l.pid_servidor_dedicado = 2004;

    int index_client = 3;

    _deleteFifoAndExit_SD17_data.status = 0;
    _deleteFifoAndExit_SD17_data.action = 1;

    EVAL_CATCH( terminaSrvDedicado_SD16( &l, nameDB, index_client ) );

    if ( _NO_EXIT != _eval_env.stat && 1 != _deleteFifoAndExit_SD17_data.status ) {
        eval_error( "(SD16) bad termination");
    }

    if ( _SUCCESS != _terminaSrvDedicado_SD16_data.success && 1 != _deleteFifoAndExit_SD17_data.status ) {
        eval_error("(SD16) Bad return value");
    }

    FILE * f = fopen( nameDB, "r" );
    if ( f == NULL ) {
        eval_error("(SD16) Unable to open %s file for testing", nameDB );
    } else {
        Login r;

        fseek( f, index_client * sizeof(Login), SEEK_SET );
        fread( &r, sizeof(Login), 1, f );

        // CC: nif, senha and saldo are not required to be updated, just the PIDs
        // if ( r.nif != l.nif )
        //     eval_error("(SD16) .nif is not correct in file record");
        // if ( strcmp(r.senha, l.senha ) )
        //     eval_error("(SD16) .nif is not correct in file record");
        // if ( strcmp(r.nome, l.nome ) )
        //     eval_error("(SD16) .nome is not correct in file record");
        // if ( r.saldo != l.saldo )
        //     eval_error("(SD16) .saldo is not correct in file record");
        if ( r.pid_cliente != -1 )
            eval_error("(SD16) .pid_cliente is not correct in file record");
        if ( r.pid_servidor_dedicado != -1 )
            eval_error("(SD16) .pid_servidor_dedicado is not correct in file record");

        fclose(f);
    }
    if ( _deleteFifoAndExit_SD17_data.status > 1) {
        eval_error( "(SD17) _deleteFifoAndExit_SD17_data() should have been called exactly once");
    }

    eval_check_successlog( "(SD16.2)", "(SD16.2) " );
    eval_check_successlog( "(SD16.3)", "(SD16.3) " );
    eval_check_successlog( "(SD16.4)", "(SD16.4) " );

    removeTestDB();

    // Test with missing DB
    eval_info("Evaluating SD16.2 - %s...", question_text(questions,"16.2"));
    eval_reset_except_stats();

    _deleteFifoAndExit_SD17_data.status = 0;
    _deleteFifoAndExit_SD17_data.action = 1;

    EVAL_CATCH( terminaSrvDedicado_SD16( &l, nameDB, index_client ) );

    if ( _EXIT != _eval_env.stat && _ERROR != _terminaSrvDedicado_SD16_data.success && 1 != _deleteFifoAndExit_SD17_data.status ) {
        eval_error("(SD16) terminaSrvDedicado_SD16 returned success on missing database");
    }

    if ( 1 != _deleteFifoAndExit_SD17_data.status ) {
        eval_error( "(SD16) deleteFifoAndExit_SD17() should have been called exactly once");
    }

    eval_check_errorlog( "(SD16.2)", "(SD16.2) " );

    eval_close_logs( "(SD16)" );
    return eval_complete("(SD16)");
}

void deleteFifoAndExit_SD17() {
    _deleteFifoAndExit_SD17_data.status++;
    if ( _deleteFifoAndExit_SD17_data.action == 0 )
        _student_deleteFifoAndExit_SD17();
}

/**
 * Evaluate SD17
 **/
int eval_sd17( ) {

    eval_info("Evaluating SD17 - %s...", question_text(questions,"17"));
    eval_reset();

    _eval_unlink_data.status = 0;
    _eval_unlink_data.action = 1;

    _eval_remove_data.status = 0;
    _eval_remove_data.action = 1;

    _deleteFifoAndExit_SD17_data.action = 0;

    char nameFifoServidorDedicado[20];
    buildNomeFifo(nameFifoServidorDedicado, 20, FILE_PREFIX_SRVDED, getpid(), FILE_SUFFIX_FIFO);

    EVAL_CATCH( deleteFifoAndExit_SD17( ) );

    if ( _EXIT != _eval_env.stat ) {
        eval_error( "(SD17) bad termination");
    } else {
        if ( 0 != _eval_exit_data.status ) {
            eval_error( "(SD17) exit() called with wrong value");
        }
    }

    if ( 1 != _eval_unlink_data.status && 1 != _eval_remove_data.status  ) {
        eval_error("(SD17) unlink()/remove() should have been called exactly once");
    } else {
        if (1 == _eval_unlink_data.status) {
            if (strncmp( _eval_unlink_data.path, nameFifoServidorDedicado, 1024 ) ) {
                eval_error("(SD17) unlink() called with invalid path");
            }
        } else {
            if (strncmp( _eval_remove_data.path, nameFifoServidorDedicado, 1024 ) ) {
                eval_error("(SD17) remove() called with invalid path");
            }
        }
    }

    eval_check_successlog( "(SD17)", "(SD17) " );

    eval_close_logs( "(SD17)" );
    return eval_complete("(SD17)");
}

void trataSinalSIGUSR1_SD18(int sinalRecebido) {
    _student_trataSinalSIGUSR1_SD18( sinalRecebido );
}

/**
 * Evaluate SD18
 **/
int eval_sd18( ) {

    eval_info("Evaluating SD18 - %s...", question_text(questions,"18"));
    eval_reset();

    clientRequest.pid_cliente = 16384;
    index_client = 16;

    _terminaSrvDedicado_SD16_data.status = 0;
    _terminaSrvDedicado_SD16_data.action = 1;

    _eval_kill_data.action = 2;
    _eval_kill_data.status = 0;

    EVAL_CATCH( trataSinalSIGUSR1_SD18( SIGUSR1 ) );

    if ( _NO_EXIT != _eval_env.stat ) {
        eval_error( "(SD18) bad termination");
    }

    if ( 1 != _eval_kill_data.status ) {
        eval_error( "SD18 kill() should have been called exactly once");
    } else {
        if ( _eval_kill_data.pid != clientRequest.pid_cliente ) {
            eval_error( "SD18 kill() called with wrong PID");
        }

        if ( _eval_kill_data.sig != SIGUSR2 ) {
            eval_error( "SD18 kill() called with signal");
        }
    }

    if ( 1 != _terminaSrvDedicado_SD16_data.status ) {
        eval_error( "SD18 terminaSrvDedicado_SD16() should have been called exactly once");
    } else {
        if (_terminaSrvDedicado_SD16_data.clientRequest != &clientRequest ) {
            eval_error( "SD18 terminaSrvDedicado_SD16() called with wrong clientRequest");
        }

        if (strncmp(_terminaSrvDedicado_SD16_data.nameDB, FILE_DATABASE, strlen(FILE_DATABASE) ) ) {
            eval_error( "SD18 terminaSrvDedicado_SD16() called with wrong nameDB");
        }

        if (_terminaSrvDedicado_SD16_data.index_client != index_client ) {
            eval_error( "SD18 terminaSrvDedicado_SD16() called with wrong index_client");
        }
    }

    eval_check_successlog( "(SD18)", "(SD18) " );

    eval_close_logs( "(SD18)" );
    return eval_complete("(SD18)");
}

int main(int argc, char *argv[]) {

    initlog( &_success_log );
    initlog( &_error_log );

    eval_info(" %s/servidor.c\n", TOSTRING( _EVAL ) );

    int nerr = 0;

    nerr += eval_s1();
    nerr += eval_s2();
    nerr += eval_s3();
    nerr += eval_s4();
    nerr += eval_s5();
    nerr += eval_s6();
    nerr += eval_s7();
    nerr += eval_s8();
    nerr += eval_sd9();
    nerr += eval_sd10();
    nerr += eval_sd11();
    nerr += eval_sd12();
    nerr += eval_sd13();
    nerr += eval_sd14();
    nerr += eval_sd15();
    nerr += eval_sd16();
    nerr += eval_sd17();
    nerr += eval_sd18();


    eval_info("Finished." );

    // Printout error summary report
    _eval_stats.error = nerr;
    eval_complete("servidor");

    /* Print out full evaluation report */
    if ( argc >= 2 ) {
        if ( ! strncmp( argv[1], "-x", 2 ) ) question_export(questions, "servidor" );
        if ( ! strncmp( argv[1], "-l", 2 ) ) question_list(questions, "servidor" );
    }

}