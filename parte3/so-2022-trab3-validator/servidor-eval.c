#define _SERVIDOR 1
#include "common.h"
#include "so_utils.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define GRADE_SUCCESS_ERROR 1.0
#define GRADE_EVAL 1.0

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

question_t questions[] = {
    {"1.1",   "Connect to existing shm", 0.0 },
    {"1.1.1",   "Validate log so_success or so_error", 0.0 },
    {"1.2",   "Create shm, missing files", 0.0 },
    {"1.2.1",   "Validate log so_success or so_error", 0.0 },
    {"1.3",   "Create shm, missing files present", 0.0 },
    {"1.3.1",   "Validate log so_success or so_error", 0.0 },

    {"2.1",   "Create message queue (existing queue)", 0.0 },
    {"2.1.1",   "Validate log so_success or so_error", 0.0 },
    {"2.2",   "Create message queue (new queue)", 0.0 },
    {"2.2.1",   "Validate log so_success or so_error", 0.0 },

    {"3.1",   "Create semaphore array (existing array)", 0.0 },
    {"3.1.1",   "Validate log so_success or so_error", 0.0 },
    {"3.2",   "Create semaphore array (new array)", 0.0 },
    {"3.2.1",   "Validate log so_success or so_error", 0.0 },

    {"4",     "Arm signal handlers", 0.0 },
    {"4.1",   "Validate log so_success or so_error", 0.0 },

    {"5.1",   "Receive login (ok)", 0.0 },
    {"5.1.1",   "Validate log so_success or so_error", 0.0 },
    {"5.2",   "Receive login (error)", 0.0 },
    {"5.2.1",   "Validate log so_success or so_error", 0.0 },

    {"6.1",   "Dedicated server (child)", 0.0 },
    {"6.2",   "Dedicated server (parent)", 0.0 },
    {"6.2.1",   "Validate log so_success or so_error", 0.0 },
    {"6.3",   "Dedicated server (error)", 0.0 },
    {"6.3.1",   "Validate log so_success or so_error", 0.0 },

    {"7.1",   "Shutdown (no SHM)", 0.0 },
    {"7.2",   "Shutdown (active servers)", 0.0 },
    {"7.2.1",   "Validate log so_success or so_error", 0.0 },
    {"7.3",   "Shutdown (no active servers)", 0.0 },
    {"7.3.1",   "Validate log so_success or so_error", 0.0 },

    {"8",     "Handle SIGINT", 0.0 },
    {"8.1",   "Validate log so_success or so_error", 0.0 },

    {"9",     "Handle SIGCHLD", 0.0 },
    {"9.1",   "Validate log so_success or so_error", 0.0 },

    {"10",     "Arm dedicated server signal handlers", 0.0 },
    {"10.1",   "Validate log so_success or so_error", 0.0 },

    {"11.1",   "Validate request (ok)", 0.0 },
    {"11.1.1",   "Validate log so_success or so_error", 0.0 },
    {"11.2",   "Validate request (fail)", 0.0 },
    {"11.2.1",   "Validate log so_success or so_error", 0.0 },

    {"12.1",   "Search User DB (ok)", 0.0 },
    {"12.1.1",   "Validate log so_success or so_error", 0.0 },
    {"12.2",   "Search User DB (fail, bad nif)", 0.0 },
    {"12.2.1",   "Validate log so_success or so_error", 0.0 },
    {"12.3",   "Search User DB (fail, bad pass)", 0.0 },
    {"12.3.1",   "Validate log so_success or so_error", 0.0 },

    {"13",     "Reserve User DB", 0.0 },
    {"13.1",   "Validate log so_success or so_error", 0.0 },

    {"14",     "Send product list", 0.0 },
    {"14.1",   "Validate log so_success or so_error", 0.0 },

    {"15.1",     "Receive client order (ok)", 0.0 },
    {"15.1.1",   "Validate log so_success or so_error", 0.0 },
    {"15.2",     "Receive client order (fail)", 0.0 },
    {"15.2.1",   "Validate log so_success or so_error", 0.0 },

    {"16.1",     "Send puchase ack (found)", 0.0 },
    {"16.1.1",   "Validate log so_success or so_error", 0.0 },
    {"16.2",     "Send puchase ack (out of stock)", 0.0 },
    {"16.2.1",   "Validate log so_success or so_error", 0.0 },
    {"16.3",     "Send puchase ack (not found)", 0.0 },
    {"16.3.1",   "Validate log so_success or so_error", 0.0 },
    {"16.4",     "Send puchase ack (cancelled)", 0.0 },
    {"16.4.1",   "Validate log so_success or so_error", 0.0 },

    {"17",     "Dedicated server shutdown", 0.0 },
    {"17.1",   "Validate log so_success or so_error", 0.0 },

    {"18",     "Handle SIGUSR1 on dedicated server", 0.0 },
    {"18.1",   "Validate log so_success or so_error", 0.0 },

    {"---", "_end_",0.0}
};

/* Variáveis globais */
int shmId = RET_ERROR;                  // Variável que tem o ID da Shared Memory
int msgId = RET_ERROR;                  // Variável que tem o ID da Message Queue
int semId = RET_ERROR;                  // Variável que tem o ID do Grupo de Semáforos
MsgContent msg;                         // Variável que serve para todas as mensagens trocadas entre Cliente e Servidor
DadosServidor *db;                      // Variável que vai ficar com a memória partilhada
int indexClient;                        // Índice do cliente que fez o pedido ao servidor/servidor dedicado na BD


DadosServidor localDadosServidor;

/**
 * @brief Prints Login variable data
 *
 * @param s     Message to print first (may be set to NULL to avoid printing message)
 * @param r     Login variable to print
 */
void print_login( char *s, Login r ) {
    if (s) printf("%s\n", s);
    printf("nif                 : %d\n", r.nif);
    printf("senha               : %s\n", r.senha);
    printf("nome                : %s\n", r.nome);
    printf("saldo               : %d\n", r.saldo);
    printf("pidCliente          : %d\n", r.pidCliente);
    printf("pidServidorDedicado : %d\n", r.pidServidorDedicado);
}


/**
 * @brief Compares 2 Login values
 *
 * @param l1    Login value 1
 * @param l2    Login value 2
 * @return int  0 if equal, -1 if different
 */
int compare_user( Login l1, Login l2 ) {
    so_debug("");
    if ( l1.nif != l2.nif ) return -1;
    so_debug("");
    if ( l1.saldo != l2.saldo ) return -1;
    so_debug("");
    if ( strncmp( l1.senha, l2.senha, 20 ) != 0 ) return -1;
    so_debug("");
    if ( strncmp( l1.nome, l2.nome, 52 ) != 0 ) return -1;
    so_debug("");
    if ( l1.pidCliente != l2.pidCliente ) return -1;
    so_debug("");
    if ( l1.pidServidorDedicado != l2.pidServidorDedicado ) return -1;
    so_debug("");
    return 0;
}

/**
 * @brief Generates test user
 *
 * @param i         User index
 * @return Login    test user
 */
Login test_user( int i ) {
    Login l;
    l.nif = 1000001 + i;
    sprintf(l.senha,"pass%d", i+1 );
    sprintf(l.nome,"Name%d Surname%d", i+1, i+1 );
    l.saldo = 101 + i;
    l.pidCliente = -1;
    l.pidServidorDedicado = -1;

    return l;
}

/**
 * @brief Create a test FILE_DATABASE_USERS
 *
 * The file has MAX_USERS/2, each user is set to test_user(i)
 */
void createTestDBUsers( ) {

    FILE *f = fopen( FILE_DATABASE_USERS, "w" );

    if ( f ) {
        for( int i = 0; i < MAX_USERS/2; i ++ ) {
            Login l = test_user(i);
            if ( fwrite( &l, sizeof(Login), 1, f ) < 1 ) {
                eval_error("(*critical*) Error writing to file %s, aborting...", FILE_DATABASE_USERS );
                exit(1);
            };
        }
        fclose( f );
    } else {
        eval_error("(*critical*) Unable to open file %s for writing, aborting...", FILE_DATABASE_USERS );
        exit(1);
    }
}

/**
 * @brief Removes FILE_DATABASE_USERS file
 *
 */
void removeTestDBUsers() {
    unlink( FILE_DATABASE_USERS );
}

/**
 * @brief Validates localDadosServidor.listUsers
 *
 * Assumes .listUsers was created from file generated by `createTestDBUsers()`
 *
 * @return int  0 if correct, -1 if any differences found
 */
int validateTestDBUsers() {

    for( int i = 0; i < MAX_USERS/2; i ++ ) {
        if ( compare_user( test_user(i), localDadosServidor.listUsers[i] ) ) return -1;
    }

    for( int i = MAX_USERS/2 + 1; i < MAX_USERS; i++ ) {
        if ( localDadosServidor.listUsers[i].nif != USER_NOT_FOUND ) return -1;
    }

    return 0;
}

/**
 * @brief Prints Produto variable data
 *
 * @param s     Message to print first (may be set to NULL to avoid printing message)
 * @param p     Produto variable to print
 */

void print_produto( char *s, Produto p ) {
    if (s) printf("%s\n", s);
    printf("idProduto     : %d\n", p.idProduto);
    printf("nomeProduto   : %s\n", p.nomeProduto);
    printf("categoria     : %s\n", p.categoria);
    printf("preco         : %d\n", p.preco);
    printf("stock         : %d\n", p.stock);
}

/**
 * @brief Compares 2 Produto values
 *
 * @param p1    Produto value 1
 * @param p2    Produto value 2
 * @return int  0 if equal, -1 if different
 */
int compare_product( Produto p1, Produto p2 ) {
    if ( p1.idProduto != p2.idProduto ) return -1;
    if ( strncmp( p1.nomeProduto, p2.nomeProduto, 40 ) != 0 ) return -1;
    if ( strncmp( p1.categoria, p2.categoria, 40 ) != 0 ) return -1;
    if ( p1.preco != p2.preco ) return -1;
    if ( p1.stock != p2.stock ) return -1;
    return 0;
}

/**
 * @brief Generates test product
 *
 * @param i         Product index
 * @return Produto  test product
 */
Produto test_product( int i ) {
    Produto p;
    p.idProduto = i + 1;
    sprintf(p.nomeProduto,"Produto #%d", i+1 );
    sprintf(p.categoria,"Categoria #%d", i % 3 );
    p.preco = 10 + 2 * i;
    p.stock = i % 3;

    return p;
}

/**
 * @brief Create a test FILE_DATABASE_PRODUCTS
 *
 * The file has MAX_PRODUCTS/2, each product is set to test_product(i)
 */
void createTestDBProducts( ) {

    FILE *f = fopen( FILE_DATABASE_PRODUCTS, "w" );

    if ( f ) {
        for( int i = 0; i < MAX_PRODUCTS/2; i ++ ) {
            Produto p = test_product(i);
            if ( fwrite( &p, sizeof(Produto), 1, f ) < 1 ) {
                eval_error("(*critical*) Error writing to file %s, aborting...", FILE_DATABASE_PRODUCTS );
                exit(1);
            }
        }
        fclose( f );
    } else {
        eval_error("(*critical*) Unable to open file %s for writing, aborting...", FILE_DATABASE_PRODUCTS );
        exit(1);
    }
}

/**
 * @brief Validates localDadosServidor.listProducts
 *
 * Assumes .listProducts was created from file generated by `createTestDBProducts()`
 *
 * @return int  0 if correct, -1 if any differences found
 */
int validateTestDBProducts() {

    for( int i = 0; i < MAX_PRODUCTS/2; i ++ ) {
        if ( compare_product( test_product(i), localDadosServidor.listProducts[i] ) ) return -1;
    }

    for( int i = MAX_PRODUCTS/2 + 1; i < MAX_PRODUCTS; i++ ) {
        if ( localDadosServidor.listProducts[i].idProduto != PRODUCT_NOT_FOUND ) return -1;
    }

    return 0;
}

/**
 * @brief Removes FILE_DATABASE_PRODUCTS file
 *
 */
void removeTestDBProducts() {
    unlink( FILE_DATABASE_PRODUCTS );
}

/**
 * @brief Resets localDadosServidor variable
 *
 * All values are set to 0 and:
 * 1. listUsers[*].nif are set to USER_NOT_FOUND
 * 2. listUsers[*].pidServidorDedicado are set to -1
 * 3. listProducts[*].idProduto are set to PRODUCT_NOT_FOUND
 */
void reset_dadosServidor() {
    memset( &localDadosServidor, 0, sizeof(localDadosServidor) );

    for( int i = 0; i < MAX_USERS; i++ ) {
        localDadosServidor.listUsers[i].nif = USER_NOT_FOUND;
        localDadosServidor.listUsers[i].pidServidorDedicado = -1;
    }

    for( int i = 0; i < MAX_PRODUCTS; i++ ) {
        localDadosServidor.listProducts[i].idProduto = PRODUCT_NOT_FOUND;
    }
}

/**
 * Evaluate S1
 **/

struct {
    int action;
    int ret;
} _initShm_S1_data;

int initShm_S1() {
    _initShm_S1_data.ret = 0;
    _initShm_S1_data.ret = _student_initShm_S1();
    return _initShm_S1_data.ret;
}

int eval_s1( ) {
    eval_info("Evaluating S1.1 - %s...", question_text(questions,"1.1"));
    eval_reset();

    _eval_shmget_data.status = 0;
    _eval_shmget_data.action = 1; // inject

    _eval_shmget_data.key = 0;
    _eval_shmget_data.size = 0;
    _eval_shmget_data.shmflg = 0xFFFF;

    _eval_shmget_data.shmid = 0xD8140;

    _eval_shmat_data.status = 0;
    _eval_shmat_data.action = 1; // inject
    _eval_shmat_data.shmid = -1;
    _eval_shmat_data.shmaddr = &localDadosServidor;
    _eval_shmat_data.shmflg = 0xFFFF;

    EVAL_CATCH( initShm_S1() );
    int _result_grade = 0;

    if ( _EXIT == _eval_env.stat ) {
        eval_error( "(S1) bad termination:");
    } else {
        ++_result_grade;
    }

    if ( 1 != _eval_shmget_data.status ) {
        eval_error("(S1) shmget() should have been called exactly once, was called %d times.", _eval_shmget_data.status );
    } else {
        ++_result_grade;

        if ( IPC_KEY != _eval_shmget_data.key ) {
            eval_error("(S1) shmget() called with invalid key: Expected 0x%x, got 0x%x", IPC_KEY, _eval_shmget_data.key);
        } else {
            ++_result_grade;
        }
        if ( sizeof(DadosServidor) < _eval_shmget_data.size ) {
            eval_error("(S1) shmget() called with invalid size: Expected %d (max), got %d", sizeof(DadosServidor), _eval_shmget_data.size);
        } else {
            ++_result_grade;
        }
        if ( 0 != _eval_shmget_data.shmflg ) {
            eval_error("(S1) shmget() called with invalid shmflg: Expected 0, got %d", _eval_shmget_data.shmflg);
        } else {
            ++_result_grade;
        }
    }

    if ( 1 != _eval_shmat_data.status ) {
        eval_error("(S1) shmat() should have been called exactly once, was called %d times.", _eval_shmat_data.status );
    } else {
        if ( 0xD8140 != _eval_shmat_data.shmid) {
            eval_error("(S1) shmat called with wrong shmid");
        } else {
            ++_result_grade;
        }
        if ( NULL != _eval_shmat_data.shmaddr ) {
            eval_error("(S1) shmat called with wrong shmaddr");
        } else {
            ++_result_grade;
        }
        if ( 0 != _eval_shmat_data.shmflg ) {
            eval_error("(S1) shmat called with wrong shmflg");
        } else {
            ++_result_grade;
        }

        if ( db != &localDadosServidor ) {
            eval_error("(S1) db not initialized properly");
        } else {
            ++_result_grade;
        }
    }

    if ( 0xD8140 != _initShm_S1_data.ret ) {
        eval_error("(S1) invalid return value");
    } else {
        ++_result_grade;
    }
    question_setgrade( questions,"1.1", GRADE_EVAL * _result_grade/10 );

    char buffer[1024];
    snprintf( buffer, 1024, "(S1.1) %d", 0xD8140);
    if (eval_check_successlog( "(S1.1)", buffer )) {
        question_setgrade( questions,"1.1.1", GRADE_SUCCESS_ERROR );
    }

    eval_close_logs( "(S1)" );

    eval_info("Evaluating S1.2 - %s...", question_text(questions,"1.1"));
    eval_reset_except_stats();

    _eval_shmget_data.status = 0;
    _eval_shmget_data.action = 3; // error (ENOENT) on first try, inject on 2nd try

    _eval_shmget_data.key = 0;
    _eval_shmget_data.size = 0;
    _eval_shmget_data.shmflg = 0xFFFF;

    _eval_shmget_data.shmid = 0xD8140;

    _eval_shmat_data.status = 0;
    _eval_shmat_data.action = 1; // inject
    _eval_shmat_data.shmid = -1;
    _eval_shmat_data.shmaddr = &localDadosServidor;
    _eval_shmat_data.shmflg = 0xFFFF;

    unlink( FILE_DATABASE_USERS );
    unlink( FILE_DATABASE_PRODUCTS );

    EVAL_CATCH( initShm_S1() );
    _result_grade = 0;
    int _result_grade_log = 0;

    if ( _EXIT == _eval_env.stat ) {
        eval_error( "(S1) bad termination");
    } else {
        ++_result_grade;
    }

    if ( 2 != _eval_shmget_data.status ) {
        eval_error("(S1) shmget() should have been called exactly twice, was called %d times.", _eval_shmget_data.status );
    } else {
        ++_result_grade;

        if ( IPC_KEY != _eval_shmget_data.key ) {
            eval_error("(S1) shmget() called with invalid key: Expected 0x%x, got 0x%x", IPC_KEY, _eval_shmget_data.key);
        } else {
            ++_result_grade;
        }
        if ( sizeof(DadosServidor) < _eval_shmget_data.size ) {
            eval_error("(S1) shmget() called with invalid size: Expected %d (max), got %d", sizeof(DadosServidor), _eval_shmget_data.size);
        } else {
            ++_result_grade;
        }
        if ( ( _eval_shmget_data.shmflg & ( IPC_CREAT | 0600 ) ) == 0 ) {
            eval_error("(S1) shmget() called with invalid shmflg: Expected %d, got %d", ( IPC_CREAT | 0600 ), _eval_shmget_data.shmflg);
        } else {
            ++_result_grade;
        }
    }

    if (eval_check_errorlog( "(S1.1)", "(S1.1) " )) {
        ++_result_grade_log;
    }

    if (eval_check_successlog( "(S1.2)", "(S1.2) " )) {
        ++_result_grade_log;
    }

    snprintf( buffer, 1024, "(S1.3) %d", 0xD8140);
    if (eval_check_successlog( "(S1.3)", buffer )) {
        ++_result_grade_log;
    }

    if ( 1 != _eval_shmat_data.status ) {
        eval_error("(S1.4) shmat() should have been called exactly once, was called %d times.", _eval_shmat_data.status );
    } else {
        ++_result_grade;

        if ( 0xD8140 != _eval_shmat_data.shmid) {
            eval_error("(S1.4) shmat called with wrong shmid");
        } else {
            ++_result_grade;
        }
        if ( NULL != _eval_shmat_data.shmaddr ) {
            eval_error("(S1.4) shmat called with wrong shmaddr");
        } else {
            ++_result_grade;
        }
        if ( 0 != _eval_shmat_data.shmflg ) {
            eval_error("(S1.4) shmat called with wrong shmflg");
        } else {
            ++_result_grade;
        }

        if ( db != &localDadosServidor ) {
            eval_error("(S1.4) db not initialized properly");
        } else {
            ++_result_grade;
        }

        if (eval_check_successlog( "(S1.4)", "(S1.4) " )) {
            ++_result_grade_log;
        }

        // Check initialization
        for( int i = 0; i < MAX_USERS; i++ ) {
            if ( db->listUsers[i].nif != USER_NOT_FOUND ) {
                eval_error("(S1.4) db listUsers not initialized properly");
                break;
            } else {
                ++_result_grade;
            }
        }

        for( int i = 0; i < MAX_PRODUCTS; i++ ) {
            if ( db->listProducts[i].idProduto != PRODUCT_NOT_FOUND ) {
                eval_error("(S1.4) db listProducts not initialized properly");
                break;
            } else {
                ++_result_grade;
            }
        }

    }

    if (eval_check_errorlog( "(S1.5)", "(S1.5) " )) {
        ++_result_grade_log;
    }

    if ( RET_ERROR != _initShm_S1_data.ret ) {
        eval_error("(S1) invalid return value, should be RET_ERROR");
    } else {
        ++_result_grade;
    }

    question_setgrade( questions,"1.2", GRADE_EVAL * _result_grade/(11 + MAX_USERS + MAX_PRODUCTS) );
    question_setgrade( questions,"1.2.1", GRADE_SUCCESS_ERROR * _result_grade_log/5 );

    eval_close_logs( "(S1)" );

    eval_info("Evaluating S1.3 - %s...", question_text(questions,"1.1"));
    eval_reset_except_stats();

    _eval_shmget_data.status = 0;
    _eval_shmget_data.action = 3; // error (ENOENT) on first try, inject on 2nd try

    _eval_shmget_data.key = 0;
    _eval_shmget_data.size = 0;
    _eval_shmget_data.shmflg = 0xFFFF;

    _eval_shmget_data.shmid = 0xD8140;

    _eval_shmat_data.status = 0;
    _eval_shmat_data.action = 1; // inject
    _eval_shmat_data.shmid = -1;
    _eval_shmat_data.shmaddr = &localDadosServidor;
    _eval_shmat_data.shmflg = 0xFFFF;

    createTestDBUsers();
    createTestDBProducts();

    EVAL_CATCH( initShm_S1() );
    _result_grade = 0;
    _result_grade_log = 0;

    if ( _EXIT == _eval_env.stat ) {
        eval_error( "(S1) bad termination");
    } else {
        ++_result_grade;
    }

    if ( 0xD8140 != _initShm_S1_data.ret ) {
        eval_error("(S1) invalid return value");
    } else {
        ++_result_grade;
    }

    if (eval_check_errorlog( "(S1.1)", "(S1.1) " )) {
        ++_result_grade_log;
    }

    if (eval_check_successlog( "(S1.2)", "(S1.2) " )) {
        ++_result_grade_log;
    }

    snprintf( buffer, 1024, "(S1.3) %d", 0xD8140);
    if (eval_check_successlog( "(S1.3)", buffer )) {
        ++_result_grade_log;
    }

    if (eval_check_successlog( "(S1.4)", "(S1.4) " )) {
        ++_result_grade_log;
    }

    if (eval_check_successlog( "(S1.5)", "(S1.5) " )) {
        ++_result_grade_log;
    }

    if (eval_check_successlog( "(S1.6)", "(S1.6) " )) {
        ++_result_grade_log;
    }


    // Validate values
    if ( validateTestDBUsers() != 0 ) {
        eval_error("(S1.5) Users database not properly read into memory");
    } else {
        ++_result_grade;
    };

    if ( validateTestDBProducts() != 0 ) {
        eval_error("(S1.6) Products database not properly read into memory");
    } else {
        ++_result_grade;
    };

    question_setgrade( questions,"1.3", GRADE_EVAL * _result_grade/4 );
    question_setgrade( questions,"1.3.1", GRADE_SUCCESS_ERROR * _result_grade_log/6 );

    // Remove files
    removeTestDBUsers();
    removeTestDBProducts();

    eval_close_logs( "(S1)" );
    return eval_complete("(S1)");
}

/**
 * Evaluate S2
 **/

struct {
    int action;
    int ret;
} _initMsg_S2_data;

int initMsg_S2() {
    _initMsg_S2_data.ret = 0;
    _initMsg_S2_data.ret = _student_initMsg_S2();
    return _initMsg_S2_data.ret;
}

int eval_s2( ) {
    eval_info("Evaluating S2.1 - %s...", question_text(questions,"2.1"));
    eval_reset();

    _eval_msgget_data.status = 0;
    _eval_msgget_data.action = 1; // inject
    _eval_msgget_data.key = 0;
    _eval_msgget_data.msgflg = 0xFFFF;
    _eval_msgget_data.msqid = 0xD8140;

    _eval_msgctl_data.status = 0;
    _eval_msgctl_data.action = 1;

    EVAL_CATCH( initMsg_S2() );
    int _result_grade = 0;
    int _result_grade_log = 0;

    if ( _EXIT == _eval_env.stat ) {
        eval_error( "(S1) bad termination");
    } else {
        ++_result_grade;
    }

    if ( 2 != _eval_msgget_data.status ) {
        eval_error("(S2) msgget() should have been called exactly twice, was called %d times.", _eval_msgget_data.status );
    } else {
        ++_result_grade;

        if ( IPC_KEY != _eval_msgget_data.key ) {
            eval_error("(S2.1) msgget() called with invalid key: Expected 0x%x, got 0x%x", IPC_KEY, _eval_msgget_data.key);
        } else {
            ++_result_grade;
        }
        if ( ( _eval_msgget_data.msgflg & ( IPC_CREAT | 0600) ) == 0 ) {
            eval_error("(S2.1) msgget() called with invalid shmflg: Expected %d, got %d",
                ( _eval_msgget_data.msgflg & ( IPC_CREAT | 0600) ), _eval_msgget_data.msgflg);
        } else {
            ++_result_grade;
        }
    }

    if ( 1 != _eval_msgctl_data.status ) {
        eval_error("(S2.1) msgctl() should have been called exactly once, was called %d times.", _eval_msgctl_data.status );
    } else {
        ++_result_grade;
        if ( IPC_RMID != _eval_msgctl_data.cmd ) {
            eval_error("(S2.1) msgctl() called with wrong cmd");
        } else {
            ++_result_grade;
        }
    }

    if ( 0xD8140 != _initMsg_S2_data.ret ) {
        eval_error("(S2) Invalid return value");
    } else {
        ++_result_grade;
    }
    if (eval_check_successlog( "(S2.1)", "(S2.1) ")) {
        ++_result_grade_log;
    }

    char buffer[1024];
    snprintf( buffer, 1024, "(S2.2) %d", 0xD8140);
    if (eval_check_successlog( "(S2.2)", buffer )) {
        ++_result_grade_log;
    }
    question_setgrade( questions,"2.1", GRADE_EVAL * _result_grade/7 );
    question_setgrade( questions,"2.1.1", GRADE_SUCCESS_ERROR * _result_grade_log/2 );

    eval_close_logs( "(S2)" );

    eval_info("Evaluating S2.2 - %s...", question_text(questions,"2.2"));
    eval_reset_except_stats();

    _eval_msgget_data.status = 0;
    _eval_msgget_data.action = 4; // fail with ENOENT on 1st try, work on second
    _eval_msgget_data.key = 0;
    _eval_msgget_data.msgflg = 0xFFFF;
    _eval_msgget_data.msqid = 0xD8140;

    _eval_msgctl_data.status = 0;
    _eval_msgctl_data.action = 1;

    EVAL_CATCH( initMsg_S2() );
    _result_grade = 0;
    _result_grade_log = 0;

    if ( _NO_EXIT != _eval_env.stat ) {
        eval_error( "(S1) bad termination");
    } else {
        ++_result_grade;
    }

    if ( 2 != _eval_msgget_data.status ) {
        eval_error("(S2) msgget() should have been called exactly twice, was called %d times.", _eval_msgget_data.status );
    } else {
        ++_result_grade;

        if ( IPC_KEY != _eval_msgget_data.key ) {
            eval_error("(S2.1) msgget() called with invalid key: Expected 0x%x, got 0x%x", IPC_KEY, _eval_msgget_data.key);
        } else {
            ++_result_grade;
        }
        if ( ( _eval_msgget_data.msgflg & ( IPC_CREAT | 0600) ) == 0 ) {
            eval_error("(S2.1) msgget() called with invalid shmflg: Expected %d, got %d",
                ( _eval_msgget_data.msgflg & ( IPC_CREAT | 0600) ), _eval_msgget_data.msgflg);
        } else {
            ++_result_grade;
        }
    }

    if ( 0 != _eval_msgctl_data.status ) {
        eval_error("(S2.1) msgctl() should have been called 0 times, was called %d times.", _eval_msgctl_data.status );
    } else {
        ++_result_grade;
    }

    if (eval_check_successlog( "(S2.1)", "(S2.1) ")) {
        ++_result_grade_log;
    }

    if ( 0xD8140 != _initMsg_S2_data.ret ) {
        eval_error("(S2) Invalid return value");
    } else {
        ++_result_grade;
    }

    snprintf( buffer, 1024, "(S2.2) %d", 0xD8140);
    if (eval_check_successlog( "(S2.2)", buffer )) {
        ++_result_grade_log;
    }
    question_setgrade( questions,"2.2", GRADE_EVAL * _result_grade/6 );
    question_setgrade( questions,"2.2.1", GRADE_SUCCESS_ERROR * _result_grade_log/2 );

    eval_close_logs( "(S2)" );
    return eval_complete("(S2)");
}

/**
 * Evaluate S3
 **/

struct {
    int action;
    int ret;
} _initSem_S3_data;

int initSem_S3() {
    _initSem_S3_data.ret = 0;
    _initSem_S3_data.ret = _student_initSem_S3();
    return _initSem_S3_data.ret;
}

int eval_s3( ) {
    eval_info("Evaluating S3.1 - %s...", question_text(questions,"3.1"));
    eval_reset();

    // Existing semaphore array

    _eval_semget_data.status = 0;
    _eval_semget_data.action = 1; // inject
    _eval_semget_data.key = 0;
    _eval_semget_data.nsems = -123;
    _eval_semget_data.semflg = 0xFFFF;
    _eval_semget_data.semid = 0xD8140;

    _eval_semctl_data.status = 0;
    _eval_semctl_data.action = 2;

    initlog( &_data_log );

    EVAL_CATCH( initSem_S3() );
    int _result_grade = 0;
    int _result_grade_log = 0;

    if ( _EXIT == _eval_env.stat ) {
        eval_error( "(S3) bad termination");
    } else {
        ++_result_grade;
    }

    if ( 2 != _eval_semget_data.status ) {
        eval_error("(S3) semget() should have been called exactly twice, was called %d times.", _eval_semget_data.status );
    } else {
        ++_result_grade;

        if ( IPC_KEY != _eval_semget_data.key ) {
            eval_error("(S3.1) semget() called with invalid key: Expected 0x%x, got 0x%x", IPC_KEY, _eval_semget_data.key);
        } else {
            ++_result_grade;
        }

        if ( 3 != _eval_semget_data.nsems ) {
            eval_error("(S3.1) semget() called with invalid nsems");
        } else {
            ++_result_grade;
        }

        if ( ( _eval_semget_data.semflg & ( IPC_CREAT | 0600) ) == 0 ) {
            eval_error("(S3.1) semget() called with invalid semflg: Expected %d, got %d",
                ( _eval_semget_data.semflg & ( IPC_CREAT | 0600) ), _eval_semget_data.semflg);
        } else {
            ++_result_grade;
        }
    }

    if ( 4 != _eval_semctl_data.status ) {
        eval_error("(S3.1) semctl() should have been called 4 times, was called %d times.", _eval_semctl_data.status );
    } else {
        ++_result_grade;

        if ( findinlog( &_data_log, "semctl,%d,%d,%d", 0xD8140, 0, IPC_RMID ) < 0 ) {
            eval_error("(S3.1) Existing semaphore not removed");
        } else {
            ++_result_grade;
        }

        if ( findinlog( &_data_log, "semctl,%d,%d,%d", 0xD8140, 0, SETVAL ) < 0 ) {
            eval_error("(S3.1) Semaphore #0 not initialized");
        } else {
            ++_result_grade;
        }

        if ( findinlog( &_data_log, "semctl,%d,%d,%d", 0xD8140, 1, SETVAL ) < 0 ) {
            eval_error("(S3.1) Semaphore #1 not initialized");
        } else {
            ++_result_grade;
        }

        if ( findinlog( &_data_log, "semctl,%d,%d,%d", 0xD8140, 2, SETVAL ) < 0 ) {
            eval_error("(S3.1) Semaphore #2 not initialized");
        } else {
            ++_result_grade;
        }
    }

    if (eval_check_successlog( "(S3.1)", "(S3.1) ")) {
        ++_result_grade_log;
    }

    if ( 0xD8140 != _initSem_S3_data.ret ) {
        eval_error("(S3) Invalid return value");
    } else {
        ++_result_grade;
    }

    char buffer[1024];
    snprintf( buffer, 1024, "(S3.2) %d", 0xD8140);
    if (eval_check_successlog( "(S3.2)", buffer )) {
        ++_result_grade_log;
    }

    if (eval_check_successlog( "(S3.3)", "(S3.3) " )) {
        ++_result_grade_log;
    }

    question_setgrade( questions,"3.1", GRADE_EVAL * _result_grade/11 );
    question_setgrade( questions,"3.1.1", GRADE_SUCCESS_ERROR * _result_grade_log/3 );

    eval_close_logs( "(S3)" );

    eval_info("Evaluating S3.2 - %s...", question_text(questions,"2.2"));
    eval_reset_except_stats();

    // New semaphore array


    _eval_semget_data.status = 0;
    _eval_semget_data.action = 4; // fail with ENOENT on 1st try, work on second
    _eval_semget_data.key = 0;
    _eval_semget_data.nsems = -123;
    _eval_semget_data.semflg = 0xFFFF;
    _eval_semget_data.semid = 0xD8140;

    _eval_semctl_data.status = 0;
    _eval_semctl_data.action = 2;

    initlog( &_data_log );

    EVAL_CATCH( initSem_S3() );
    _result_grade = 0;
    _result_grade_log = 0;

    if ( _EXIT == _eval_env.stat ) {
        eval_error( "(S3) bad termination");
    } else {
        ++_result_grade;
    }

    if ( 2 != _eval_semget_data.status ) {
        eval_error("(S3) semget() should have been called exactly twice, was called %d times.", _eval_semget_data.status );
    } else {
        ++_result_grade;

        if ( IPC_KEY != _eval_semget_data.key ) {
            eval_error("(S3.1) semget() called with invalid key: Expected 0x%x, got 0x%x", IPC_KEY, _eval_semget_data.key);
        } else {
            ++_result_grade;
        }
        if ( ( _eval_semget_data.semflg & ( IPC_CREAT | 0600) ) == 0 ) {
            eval_error("(S3.1) semget() called with invalid semflg: Expected %d, got %d",
                ( _eval_semget_data.semflg & ( IPC_CREAT | 0600) ), _eval_semget_data.semflg);
        } else {
            ++_result_grade;
        }
    }

    if ( 3 != _eval_semctl_data.status ) {
        eval_error("(S3.1) semctl() should have been called 3 times, was called %d times.", _eval_semctl_data.status );
    } else {
        ++_result_grade;

        if ( findinlog( &_data_log, "semctl,%d,%d,%d", 0xD8140, 0, SETVAL ) < 0 ) {
            eval_error("(S3.1) Semaphore #0 not initialized");
        } else {
            ++_result_grade;
        }

        if ( findinlog( &_data_log, "semctl,%d,%d,%d", 0xD8140, 1, SETVAL ) < 0 ) {
            eval_error("(S3.1) Semaphore #1 not initialized");
        } else {
            ++_result_grade;
        }

        if ( findinlog( &_data_log, "semctl,%d,%d,%d", 0xD8140, 2, SETVAL ) < 0 ) {
            eval_error("(S3.1) Semaphore #2 not initialized");
        } else {
            ++_result_grade;
        }

    }
    if (eval_check_successlog( "(S3.1)", "(S3.1) ")) {
        ++_result_grade_log;
    }


    if ( 0xD8140 != _initSem_S3_data.ret ) {
        eval_error("(S3) Invalid return value");
    } else {
        ++_result_grade;
    }

    snprintf( buffer, 1024, "(S3.2) %d", 0xD8140);
    if (eval_check_successlog( "(S3.2)", buffer )) {
        ++_result_grade_log;
    }

    if (eval_check_successlog( "(S3.3)", "(S3.3) " )) {
        ++_result_grade_log;
    }
    question_setgrade( questions,"3.2", GRADE_EVAL * _result_grade/9 );
    question_setgrade( questions,"3.2.1", GRADE_SUCCESS_ERROR * _result_grade_log/3 );

    eval_close_logs( "(S3)" );

    return eval_complete("(S3)");
}


/**
 * Wrapper for triggerSignals_S4()
 **/
struct {
    int action;
    int ret;
} _triggerSignals_S4_data;

int triggerSignals_S4() {
    _triggerSignals_S4_data.ret = -1014;
    _triggerSignals_S4_data.ret = _student_triggerSignals_S4();
    return 0;
}

int eval_s4( ) {
    eval_info("Evaluating S4 - %s...", question_text(questions,"4"));
    eval_reset();

    initlog(&_data_log);
    _eval_signal_data.action = 2;

    EVAL_CATCH( triggerSignals_S4() );
    int _result_grade = 0;

    if ( _NO_EXIT != _eval_env.stat ) {
        eval_error( "(S4) bad termination");
    } else {
        ++_result_grade;
    }

    if ( RET_SUCCESS != _triggerSignals_S4_data.ret ) {
        eval_error("(S4) Bad return value");
    } else {
        ++_result_grade;
    }

    if ( findinlog( &_data_log, "signal,%d,%p", SIGINT, handleSignalSIGINT_S8 ) < 0 ) {
        eval_error("(S4) SIGINT not armed properly");
    } else {
        ++_result_grade;
    }

    if ( findinlog( &_data_log, "signal,%d,%p", SIGCHLD, handleSignalSIGCHLD_S9 ) < 0 ) {
        eval_error("(S4) SIGCHLD not armed properly");
    } else {
        ++_result_grade;
    }
    question_setgrade( questions,"4", GRADE_EVAL * _result_grade/4 );

    if (eval_check_successlog( "(S4)", "(S4) " )) {
        question_setgrade( questions,"4.1", GRADE_SUCCESS_ERROR );
    }

    eval_close_logs( "(S4)" );
    return eval_complete("(S4)");
}


/**
 * Wrapper for receiveClientLogin_S5()
 **/
struct {
    int action;
    MsgContent ret;
} _receiveClientLogin_S5_data;

MsgContent receiveClientLogin_S5() {
    MsgContent msg = {0};
    _receiveClientLogin_S5_data.ret = msg;
    _receiveClientLogin_S5_data.ret = _student_receiveClientLogin_S5();
    return _receiveClientLogin_S5_data.ret;
}

int eval_s5( ) {
    eval_info("Evaluating S5 - %s...", question_text(questions,"5.1"));
    eval_reset();

    MsgContent snd;

    snd.msgType = MSGTYPE_LOGIN;
    snd.msgData.infoLogin.nif = 123456;
    sprintf( snd.msgData.infoLogin.senha, "abc123" );
    snd.msgData.infoLogin.pidCliente = 7890123;

    _eval_msgrcv_data.status = 0;
    _eval_msgrcv_data.action = 2; // inject

    _eval_msgrcv_data.msqid = -1;
    _eval_msgrcv_data.msgtyp = -1;
    _eval_msgrcv_data.msgflg = -1;

    _eval_msgrcv_data.msgp = &snd;
    _eval_msgrcv_data.msgsz = sizeof(snd.msgData);

    msgId = 0x1324;

    EVAL_CATCH( receiveClientLogin_S5() );
    int _result_grade = 0;

    if ( _EXIT == _eval_env.stat ) {
        eval_error("(S5) bad termination");
    } else {
        ++_result_grade;
    }

    if ( 1 != _eval_msgrcv_data.status ) {
        eval_error("(S5) msgrcv() should have been called exactly onces, was called %d times.", _eval_msgrcv_data.status );
    } else {
        ++_result_grade;
        if ( msgId != _eval_msgrcv_data.msqid ) {
            eval_error("(S5) Invalid msqid in msgrcv() call.");
        } else {
            ++_result_grade;
        }
        if ( sizeof( snd.msgData ) != _eval_msgrcv_data.msgsz ) {
            eval_error("(S5) Invalid msgsz in msgrcv() call.");
        } else {
            ++_result_grade;
        }
        if ( MSGTYPE_LOGIN != _eval_msgrcv_data.msgtyp ) {
            eval_error("(S5) Invalid msgtyp in msgrcv() call.");
        } else {
            ++_result_grade;
        }
        if ( 0 != _eval_msgrcv_data.msgflg ) {
            eval_error("(S5) Invalid msgflg in msgrcv() call.");
        } else {
            ++_result_grade;
        }
    }

    MsgContent rcv = _receiveClientLogin_S5_data.ret;

    if (( snd.msgData.infoLogin.nif != rcv.msgData.infoLogin.nif ) ||
        ( strncmp( snd.msgData.infoLogin.senha, rcv.msgData.infoLogin.senha, 20 ) != 0 ) ||
        ( snd.msgData.infoLogin.pidCliente != rcv.msgData.infoLogin.pidCliente )) {
            eval_error("(S5) Invalid data returned");
    } else {
        ++_result_grade;
    }
    question_setgrade( questions,"5.1", GRADE_EVAL * _result_grade/7 );

    char buffer[1024];
    snprintf( buffer, 1024, "(S5) %d %s %d", snd.msgData.infoLogin.nif,
        snd.msgData.infoLogin.senha, snd.msgData.infoLogin.pidCliente );
    if (eval_check_successlog( "(S5)", buffer )) {
        question_setgrade( questions,"5.1.1", GRADE_SUCCESS_ERROR );
    }


    eval_close_logs( "(S5)" );

    eval_info("Evaluating S5 - %s...", question_text(questions,"5.2"));
    eval_reset_except_stats();

    _eval_msgrcv_data.status = 0;
    _eval_msgrcv_data.action = 3; // error

    EVAL_CATCH( receiveClientLogin_S5() );
    _result_grade = 0;

    if ( _EXIT == _eval_env.stat ) {
        eval_error("(S5) bad termination");
    } else {
        ++_result_grade;
    }

    if ( 1 != _eval_msgrcv_data.status ) {
        eval_error("(S5) msgrcv() should have been called exactly once, was called %d times.",
            _eval_msgrcv_data.status );
    } else {
        ++_result_grade;
        if ( msgId != _eval_msgrcv_data.msqid ) {
            eval_error("(S5) Invalid msqid in msgrcv() call.");
        } else {
            ++_result_grade;
        }
        if ( sizeof( snd.msgData ) != _eval_msgrcv_data.msgsz ) {
            eval_error("(S5) Invalid msgsz in msgrcv() call.");
        } else {
            ++_result_grade;
        }
        if ( MSGTYPE_LOGIN != _eval_msgrcv_data.msgtyp ) {
            eval_error("(S5) Invalid msgtyp in msgrcv() call.");
        } else {
            ++_result_grade;
        }
        if ( 0 != _eval_msgrcv_data.msgflg ) {
            eval_error("(S5) Invalid msgflg in msgrcv() call.");
        } else {
            ++_result_grade;
        }
    }

    MsgContent ret = _receiveClientLogin_S5_data.ret;

    if ( ret.msgData.infoLogin.nif != USER_NOT_FOUND ) {
        eval_error("(S5) Invalida data returned");
    } else {
        ++_result_grade;
    }
    question_setgrade( questions,"5.2", GRADE_EVAL * _result_grade/7 );

    if (eval_check_errorlog( "(S5)", "(S5) " )) {
        question_setgrade( questions,"5.2.1", GRADE_SUCCESS_ERROR );
    }

    eval_close_logs( "(S5)" );
    return eval_complete("(S5)");
}

/**
 * Wrapper for createServidorDedicado_S6()
 **/
struct {
    int action;
    int ret;
} _createServidorDedicado_S6_data;

int createServidorDedicado_S6() {
    _createServidorDedicado_S6_data.ret = -1234;
    _createServidorDedicado_S6_data.ret = _student_createServidorDedicado_S6();
    return _createServidorDedicado_S6_data.ret;
}

int eval_s6( ) {
    eval_info("Evaluating S6 - %s...", question_text(questions,"6.1"));
    eval_reset();

    _eval_wait_data.status = 0;
    _eval_wait_data.action = 1;

    // Testa filho
    _eval_fork_data.status = 0;
    _eval_fork_data.action = 1; // return 0

    EVAL_CATCH( createServidorDedicado_S6() );
    int _result_grade = 0;

    if ( _EXIT == _eval_env.stat ) {
        eval_error("(S6) bad termination");
    } else {
        ++_result_grade;
    }

    if ( 1 != _eval_fork_data.status ) {
        eval_error("(S6) Fork should have been called exactly once, was called %d times.", _eval_fork_data.status );
    } else {
        ++_result_grade;
        if ( 0 != _createServidorDedicado_S6_data.ret ) {
            eval_error("(S6) Invalid return value");
        } else {
            ++_result_grade;
        }
    }

    if ( 0 != _eval_wait_data.status ) {
        eval_error("(S6) wait() should not have been called");
    } else {
        ++_result_grade;
    }
    question_setgrade( questions,"6.1", GRADE_EVAL * _result_grade/4 );

    eval_close_logs( "(S6)" );

    // Testa pai
     eval_info("Evaluating S6 - %s...", question_text(questions,"6.2"));

    _eval_wait_data.status = 0;
    _eval_wait_data.action = 1;

    _eval_fork_data.status = 0;
    _eval_fork_data.action = 2; // return 1

    EVAL_CATCH( createServidorDedicado_S6() );
    _result_grade = 0;

    if ( _EXIT == _eval_env.stat ) {
        eval_error("(S6) bad termination");
    } else {
        ++_result_grade;
    }

    if ( 1 != _eval_fork_data.status ) {
        eval_error("(S6) Fork should have been called exactly once, was called %d times.", _eval_fork_data.status );
    } else {
        ++_result_grade;
        if ( 1 != _createServidorDedicado_S6_data.ret ) {
            eval_error("(S6) Invalid return value");
        } else {
            ++_result_grade;
        }
    }

    if ( 0 != _eval_wait_data.status ) {
        eval_error("(S6) wait() should not have been called");
    } else {
        ++_result_grade;
    }
    question_setgrade( questions,"6.2", GRADE_EVAL * _result_grade/4 );

    if (eval_check_successlog( "(S6)", "(S6) Servidor Dedicado: PID 1" )) {
        question_setgrade( questions,"6.2.1", GRADE_SUCCESS_ERROR );
    }

    eval_close_logs( "(S6)" );

    // Testa error
    eval_info("Evaluating S6 - %s...", question_text(questions,"6.3"));
    _eval_fork_data.status = 0;
    _eval_fork_data.action = 3; // error

    EVAL_CATCH( createServidorDedicado_S6() );
    _result_grade = 0;

    if ( _EXIT == _eval_env.stat ) {
        eval_error("(S6) bad termination");
    } else {
        ++_result_grade;
    }

    if ( 1 != _eval_fork_data.status ) {
        eval_error("(S6) Fork should have been called exactly once, was called %d times.", _eval_fork_data.status );
    } else {
        ++_result_grade;
        if ( -1 != _createServidorDedicado_S6_data.ret ) {
            eval_error("(S6) Invalid return value");
        } else {
            ++_result_grade;
        }
    }
    question_setgrade( questions,"6.3", GRADE_EVAL * _result_grade/3 );

    if (eval_check_errorlog("(S6)", "(S6) ")) {
        question_setgrade( questions,"6.3.1", GRADE_SUCCESS_ERROR );
    }

    eval_close_logs( "(S6)" );

    return eval_complete("(S6)");
}


struct {
    int status;
    int action;
    int ret;
} _shutdownAndExit_S7_data;


void shutdownAndExit_S7() {

    _shutdownAndExit_S7_data.status++;
    switch( _shutdownAndExit_S7_data.action ) {
    case(1):
        _eval_exit(RET_SUCCESS);
    default:
        _student_shutdownAndExit_S7();
    }
}

int eval_s7( ) {
    eval_info("Evaluating S7 - %s...", question_text(questions,"7.1"));
    eval_reset();

    _shutdownAndExit_S7_data.action = 0;

    // Evaluate S7.1
    _eval_kill_data.status = 0;
    _eval_kill_data.pid = -1;
    _eval_kill_data.sig = -1;
    _eval_kill_data.action = 3; // don't send signal, log call

    _eval_pause_data.status = 0;
    _eval_pause_data.action = 1;

    initlog( &_data_log );

    shmId = -1;
    semId = -1;
    msgId = -1;

    EVAL_CATCH( shutdownAndExit_S7() );
    int _result_grade_log = 0;

    if (eval_check_successlog("(S7)", "(S7) ")) {
        ++_result_grade_log;
    }

    if (eval_check_errorlog("(S7.1)", "(S7.1) ")) {
        ++_result_grade_log;
    }
    question_setgrade( questions,"7.1", GRADE_SUCCESS_ERROR * _result_grade_log/2 );

    eval_close_logs( "(S7)" );

    eval_info("Evaluating S7 - %s...", question_text(questions,"7.2"));
    eval_reset_except_stats();

    _shutdownAndExit_S7_data.action = 0;

    reset_dadosServidor();

    localDadosServidor.listUsers[0].nif = 123456789;
    localDadosServidor.listUsers[1].nif = 123456789;
    localDadosServidor.listUsers[2].nif = 123456789;
    localDadosServidor.listUsers[3].nif = 123456789;
    localDadosServidor.listUsers[4].nif = 123456789;
    localDadosServidor.listUsers[5].nif = 123456789;
    localDadosServidor.listUsers[2].pidServidorDedicado = 2;
    localDadosServidor.listUsers[5].pidServidorDedicado = 5;
    db = & localDadosServidor;

    _eval_kill_data.status = 0;
    _eval_kill_data.pid = -1;
    _eval_kill_data.sig = -1;
    _eval_kill_data.action = 3; // don't send signal, log call

    _eval_pause_data.status = 0;
    _eval_pause_data.action = 1;

    initlog( &_data_log );

    shmId = 0x1324;
    semId = -1;
    msgId = -1;

    EVAL_CATCH( shutdownAndExit_S7() );
    int _result_grade = 0;
    _result_grade_log = 0;

    if (eval_check_successlog("(S7)", "(S7) ")) {
        ++_result_grade_log;
    }

    if ( _EXIT != _eval_env.stat ) {
        if ( 2 == _eval_env.stat ) {
            eval_info("(S7) Timeout because of S7.3, see below");
            ++_result_grade;
        } else {
            eval_error("(S7) bad termination" );
        }
    } else {
        if ( RET_SUCCESS != _eval_exit_data.status ) {
            eval_error("(S7) Invalid exit status");
        } else {
            ++_result_grade;
        }
    }
    if (eval_check_successlog("(S7.1)", "(S7.1) ")) {
        ++_result_grade_log;
    }


    // 7.2
    if ( 2 != _eval_kill_data.status ) {
        eval_error("(S7) kill() should have been called exactly %d times, was called %d times.",
            2, _eval_kill_data.status );
    } else {
        ++_result_grade;

        // Active server #1 (2)
        if ( findinlog( &_data_log, "kill,%d,%d", 2, SIGUSR1 ) < 0 ) {
            eval_error("(S7.2) Incorrect or missing signal to active dedicated server");
        } else {
            ++_result_grade;
        }

        // Active server #2 (5)
        if ( findinlog( &_data_log, "kill,%d,%d", 5, SIGUSR1 ) < 0 ) {
            eval_error("(S7.2) Incorrect or missing signal to active dedicated server");
        } else {
            ++_result_grade;
        }
    }
    if (eval_check_successlog("(S7.2)", "(S7.2) ")) {
        ++_result_grade_log;
    }


    // 7.3
    eval_info( "S7.3 was removed from project");
    if (eval_check_successlog("(S7.3)", "(S7.3) ")) {
        ++_result_grade_log;
    }


    if (eval_check_successlog("(S7.4)", "(S7.4) ")) {
        ++_result_grade_log;
    }


    // CC: Não podemos avaliar assim, porque se os IDs são todos -1, então é porque não foram criados pelo servidor
    // if (eval_check_errorlog("(S7.5)", "(S7.5) ")) {
    //     question_setgrade( questions,"@.1", GRADE_SUCCESS_ERROR );
    // }
    // Unable to mark SHM for deletion
    // if (eval_check_errorlog("(S7.5)", "(S7.5) ")) {
    //     question_setgrade( questions,"@.1", GRADE_SUCCESS_ERROR );
    // }
    // Unable to delete semaphores
    // if (eval_check_errorlog("(S7.5)", "(S7.5) ")) {
    //     question_setgrade( questions,"@.1", GRADE_SUCCESS_ERROR );
    // }
    // Unable to delete message queue
    question_setgrade( questions,"7.2", GRADE_EVAL * _result_grade/4 );
    question_setgrade( questions,"7.2.1", GRADE_SUCCESS_ERROR * _result_grade_log/5 );

    eval_close_logs( "(S7)" );

    // Test remaining questions with no active servers
    eval_info("Evaluating S7 - %s...", question_text(questions,"7.3"));
    eval_reset_except_stats();

    reset_dadosServidor();

    localDadosServidor.listProducts[0].idProduto = 123456789;
    localDadosServidor.listProducts[1].idProduto = 123456789;
    localDadosServidor.listProducts[2].idProduto = 123456789;
    localDadosServidor.listProducts[3].idProduto = 123456789;
    localDadosServidor.listProducts[4].idProduto = 123456789;
    localDadosServidor.listProducts[5].idProduto = 123456789;
    localDadosServidor.listProducts[2] = test_product(2);
    localDadosServidor.listProducts[5] = test_product(5);

    _eval_kill_data.status = 0;
    _eval_kill_data.pid = -1;
    _eval_kill_data.sig = -1;
    _eval_kill_data.action = 2; // don't send signal, just capture data

    _eval_pause_data.status = 0;
    _eval_pause_data.action = 1;


    shmId = 0x1324;
    semId = 0x2435;
    msgId = 0x3546;

    _eval_shmget_data.status = 0;
    _eval_shmget_data.action = 1; // inject
    _eval_shmget_data.key = 0;
    _eval_shmget_data.size = 0;
    _eval_shmget_data.shmflg = 0xFFFF;
    _eval_shmget_data.shmid = 0x1324;

    _eval_msgctl_data.status = 0;
    _eval_msgctl_data.action = 1;
    _eval_msgctl_data.msqid = -1;
    _eval_msgctl_data.cmd = -1;
    _eval_msgctl_data.buf = NULL;

    _eval_semctl_data.status = 0;
    _eval_semctl_data.action = 1;

    _eval_semctl_data.semnum = -1;
    _eval_semctl_data.semid = -1;
    _eval_semctl_data.cmd = -1;
    _eval_semctl_data.arg.array = NULL;

    _eval_shmctl_data.status = 0;
    _eval_shmctl_data.action = 1;
    _eval_shmctl_data.shmid = -1;
    _eval_shmctl_data.cmd = -1;

    initlog( &_data_log );

    EVAL_CATCH( shutdownAndExit_S7() );
    _result_grade = 0;
    _result_grade_log = 0;

    if (eval_check_successlog("(S7)", "(S7) ")) {
        ++_result_grade_log;
    }

    if ( _NO_EXIT == _eval_env.stat ) {
        eval_error("(S7) bad termination");
    } else {
        ++_result_grade;
        if ( RET_SUCCESS != _eval_exit_data.status ) {
            eval_error("(S7) Invalid exit status");
        } else {
            ++_result_grade;
        }
    }
    if (eval_check_successlog("(S7.1)", "(S7.1) ")) {
        ++_result_grade_log;
    }

    if ( 0 != _eval_kill_data.status ) {
        eval_error("(S7) kill() should have been called exactly %d times, was called %d times.",
            0, _eval_kill_data.status );
    } else {
        ++_result_grade;
    }

    // Server termination message
    if (eval_check_successlog("(S7.2)", "(S7.2) ")) {
        ++_result_grade_log;
    }


    if (eval_check_successlog("(S7.3)", "(S7.3) ")) {
        ++_result_grade_log;
    }


    // Check FILE_DATABASE_PRODUCTS
    FILE *f = fopen( FILE_DATABASE_PRODUCTS, "r" );
    if ( f ) {
        ++_result_grade;

        Produto p;
        size_t size;

        int tests[] = {2,5};
        int i;

        for( i = 0; i < 2; i++ ) {
            size = fread( &p, sizeof(Produto), 1, f );
            if ( size < 1 ) {
                eval_error("(S7.4) Error reading file %s", FILE_DATABASE_PRODUCTS );
                break;
            } else {
                ++_result_grade;
                Produto t = test_product( tests[i] );
                if ( ( p.idProduto != t.idProduto ) ||
                    ( strncmp( p.nomeProduto, t.nomeProduto, 40 ) != 0 ) ||
                    ( strncmp( p.categoria, t.categoria, 40 ) != 0 ) ||
                    ( p.preco != t.preco ) ||
                    ( p.stock != t.stock ) ) {
                    eval_error("(S7.4) Bad or missing data in file %s", FILE_DATABASE_PRODUCTS );
                    break;
                } else {
                    ++_result_grade;
                }
            }
        }

        // Check if additional data is on file
        if ( i == 2 ) {
            char s; fread( &s, sizeof(char), 1, f );
            if( ! feof( f ) ) {
                eval_error("(S7.4) File has too much data");
            } else {
                ++_result_grade;
            }
        }

        fclose(f);
        unlink(FILE_DATABASE_PRODUCTS);
    } else {
        eval_error("(S7.4) Unable to open file %s for testing", FILE_DATABASE_PRODUCTS );
    }
    if (eval_check_successlog("(S7.4)", "(S7.4) ")) {
        ++_result_grade_log;
    }

    if ( 1 != _eval_shmctl_data.status ) {
        eval_error("(S7.5) shmctl() should have been called exactly once, was called %d times.",
            _eval_shmctl_data.status );
    } else {
        ++_result_grade;
        if ( shmId != _eval_shmctl_data.shmid ) {
            eval_error("(S7) shmctl() called with wrong shmid");
        } else {
            ++_result_grade;
        }

        if ( IPC_RMID != _eval_shmctl_data.cmd ) {
            eval_error("(S7) shmctl() called with wrong cmd");
        } else {
            ++_result_grade;
        }

        if ( NULL != _eval_shmctl_data.buf ) {
            eval_error("(S7) shmctl() called with wrong buf");
        } else {
            ++_result_grade;
        }

        if (eval_check_successlog("(S7.5)", "(S7.5) ")) {
            ++_result_grade_log;
        }
    }

    if ( 1 != _eval_semctl_data.status ) {
        eval_error("(S7) semctl() should have been called exactly once, was called %d times.",
            _eval_semctl_data.status );
    } else {
        ++_result_grade;

        if ( semId != _eval_semctl_data.semid ) {
            eval_error("(S7) semctl() called with wrong semid");
        } else {
            ++_result_grade;
        }

        if ( IPC_RMID != _eval_semctl_data.cmd ) {
            eval_error("(S7) semctl() called with wrong cmd");
        } else {
            ++_result_grade;
        }

        if (eval_check_successlog("(S7.5)", "")) {
            ++_result_grade_log;
        }

    }

    if ( 1 != _eval_msgctl_data.status ) {
        eval_error("(S7) msgctl() should have been called exactly once, was called %d times.",
            _eval_msgctl_data.status );
    } else {
        ++_result_grade;

        if ( msgId != _eval_msgctl_data.msqid ) {
            eval_error("(S7) msgctl() called with wrong shmid");
        } else {
            ++_result_grade;
        }

        if ( IPC_RMID != _eval_msgctl_data.cmd ) {
            eval_error("(S7) msgctl() called with wrong cmd");
        } else {
            ++_result_grade;
        }

        if ( NULL != _eval_msgctl_data.buf ) {
            eval_error("(S7) msgctl() called with wrong buf");
        } else {
            ++_result_grade;
        }

        if (eval_check_successlog("(S7.5)", "")) {
            ++_result_grade_log;
        }
    }
    question_setgrade( questions,"7.3", GRADE_EVAL * _result_grade/20 );
    question_setgrade( questions,"7.3.1", GRADE_SUCCESS_ERROR * _result_grade_log/8 );

    eval_close_logs( "(S7)" );

    return eval_complete("(S7)");
}

void handleSignalSIGINT_S8(int sinalRecebido) {
    _student_handleSignalSIGINT_S8( sinalRecebido );
}


int eval_s8( ) {
    eval_info("Evaluating S8 - %s...", question_text(questions,"8"));
    eval_reset();

    _shutdownAndExit_S7_data.action = 1;
    _shutdownAndExit_S7_data.status = 0;

    EVAL_CATCH( handleSignalSIGINT_S8( SIGINT ) );
    int _result_grade = 0;

    eval_info("Evaluating S7 - %s...", question_text(questions,"7.3"));

    if ( _NO_EXIT == _eval_env.stat ) {
        eval_error("(S8) bad termination");
    } else {
        ++_result_grade;
        if ( RET_SUCCESS != _eval_exit_data.status ) {
            eval_error("(S8) Invalid exit status");
        } else {
            ++_result_grade;
        }
    }

    if ( 1 != _shutdownAndExit_S7_data.status ) {
        eval_error("(S8) shutdownAndExit_S7() should have been called exactly once");
    } else {
        ++_result_grade;
    }
    question_setgrade( questions,"8", GRADE_EVAL * _result_grade/3 );

    if (eval_check_successlog("(S8)", "(S8) ")) {
        question_setgrade( questions,"8.1", GRADE_SUCCESS_ERROR );
    }

    _shutdownAndExit_S7_data.action = 0;
    _shutdownAndExit_S7_data.status = 0;

    eval_close_logs( "(S8)" );
    return eval_complete("(S8)");
}

void handleSignalSIGCHLD_S9(int sinalRecebido) {
    _student_handleSignalSIGCHLD_S9( sinalRecebido );
}


int eval_s9( ) {
    eval_info("Evaluating S9 - %s...", question_text(questions,"9"));
    eval_reset();

    _eval_wait_data.status = 0;
    _eval_wait_data.stat_loc = (void *) -1;
    _eval_wait_data.action = 1;

    EVAL_CATCH( handleSignalSIGCHLD_S9( SIGCHLD ) );
    int _result_grade = 0;

    if ( _EXIT == _eval_env.stat ) {
        eval_error("(S9) bad termination");
    } else {
        ++_result_grade;
    }

    if ( 1 != _eval_wait_data.status ) {
        eval_error("(S9) wait() should have been called exactly once");
    } else {
        ++_result_grade;
    }

    // if ( NULL != _eval_wait_data.stat_loc ) {
    //     eval_error("(S9) wait() called with invalid stat_loc");
    // }
    question_setgrade( questions,"9", GRADE_EVAL * _result_grade/2 );

    if (eval_check_successlog("(S9)", "(S9) ")) {
        question_setgrade( questions,"9.1", GRADE_SUCCESS_ERROR );
    }

    eval_close_logs( "(S9)" );
    return eval_complete("(S9)");
}


/**
 * Wrapper for triggerSignals_SD10()
 **/
struct {
    int action;
    int ret;
} _triggerSignals_SD10_data;

int triggerSignals_SD10() {
    _triggerSignals_SD10_data.ret = -1024;
    _triggerSignals_SD10_data.ret = _student_triggerSignals_SD10();
    return _triggerSignals_SD10_data.ret;
}

int eval_sd10( ) {
    eval_info("Evaluating SD10 - %s...", question_text(questions,"10"));
    eval_reset();

    initlog(&_data_log);
    _eval_signal_data.action = 2;

    EVAL_CATCH( triggerSignals_SD10() );
    int _result_grade = 0;

    if ( _EXIT == _eval_env.stat ) {
        eval_error( "(SD10) bad termination");
    } else {
        ++_result_grade;
    }

    if ( RET_SUCCESS != _triggerSignals_SD10_data.ret ) {
        eval_error("(SD10) Bad return value");
    } else {
        ++_result_grade;
    }

    if ( findinlog( &_data_log, "signal,%d,%p", SIGUSR1, handleSignalSIGUSR1_SD18 ) < 0 ) {
        eval_error("(SD10) SIGUSR1 not armed properly");
    } else {
        ++_result_grade;
    }

    if ( findinlog( &_data_log, "signal,%d,%p", SIGINT, SIG_IGN ) < 0 ) {
        eval_error("(SD10) SIGINT not ignored properly");
    } else {
        ++_result_grade;
    }
    question_setgrade( questions,"10", GRADE_EVAL * _result_grade/4 );

    if (eval_check_successlog( "(SD10)", "(SD10) " )) {
        question_setgrade( questions,"10.1", GRADE_SUCCESS_ERROR );
    }

    eval_close_logs( "(SD10)" );
    return eval_complete("(SD10)");
}

struct {
    int action;
    int ret;
} _validateRequest_SD11_data;

int validateRequest_SD11(Login request) {
    _validateRequest_SD11_data.ret = -1024;
    _validateRequest_SD11_data.ret = _student_validateRequest_SD11( request );
    return _validateRequest_SD11_data.ret;
}

int eval_sd11( ) {
    eval_info("Evaluating SD11 - %s...", question_text(questions,"11.1"));
    eval_reset();

    Login r;
    r.pidCliente = 1;

    EVAL_CATCH( validateRequest_SD11( r ) );
    int _result_grade = 0;

    if ( _EXIT == _eval_env.stat ) {
        eval_error("(SD11) bad termination");
    } else {
        ++_result_grade;
    }

    if ( RET_SUCCESS != _validateRequest_SD11_data.ret ) {
        eval_error("(SD11) Invalid return value");
    } else {
        ++_result_grade;
    }
    question_setgrade( questions,"11.1", GRADE_EVAL * _result_grade/2 );

    if (eval_check_successlog("(SD11)", "(SD11) ")) {
        question_setgrade( questions,"11.1.1", GRADE_SUCCESS_ERROR );
    }

    eval_close_logs( "(SD11)" );

    eval_info("Evaluating SD11 - %s...", question_text(questions,"11.2"));
    eval_reset_except_stats();

    r.pidCliente = -1;

    EVAL_CATCH( validateRequest_SD11( r ) );
    _result_grade = 0;

    if ( _EXIT == _eval_env.stat ) {
        eval_error("(SD11) bad termination");
    } else {
        ++_result_grade;
    }

    if ( RET_ERROR != _validateRequest_SD11_data.ret ) {
        eval_error("(SD11) Invalid return value");
    } else {
        ++_result_grade;
    }
    question_setgrade( questions,"11.2", GRADE_EVAL * _result_grade/2 );

    if (eval_check_errorlog("(SD11)", "(SD11) ")) {
        question_setgrade( questions,"11.2.1", GRADE_SUCCESS_ERROR );
    }

    eval_close_logs( "(SD11)" );
    return eval_complete("(SD11)");
}

/**
 * Wrapper for searchUserDB_SD12()
 **/
struct {
    int action;
    int ret;
} _searchUserDB_SD12_data;

int searchUserDB_SD12(Login request) {
    _searchUserDB_SD12_data.ret = -1234;
    _searchUserDB_SD12_data.ret = _student_searchUserDB_SD12(request);
    return _searchUserDB_SD12_data.ret;
}

int eval_sd12( ) {
    eval_info("Evaluating SD12 - %s...", question_text(questions,"12.1"));
    eval_reset();

    reset_dadosServidor();
    for( int i = 0; i < MAX_USERS; i++ )
        localDadosServidor.listUsers[i] = test_user(i);
    db = & localDadosServidor;

    _eval_msgsnd_data.status = 0;
    _eval_msgsnd_data.action = 4; // ignore

    int idx = 6;

    EVAL_CATCH( searchUserDB_SD12( test_user(idx) ) );
    int _result_grade = 0;

    if ( _EXIT == _eval_env.stat ) {
        eval_error("(SD12) bad termination");
    } else {
        ++_result_grade;
    }

    if ( idx != _searchUserDB_SD12_data.ret ) {
        eval_error("(SD12) Invalid return value");
    } else {
        ++_result_grade;
    }

    if ( 0 != _eval_msgsnd_data.status ) {
        eval_error("(SD12) msgsnd should not have been called");
    } else {
        ++_result_grade;
    }
    question_setgrade( questions,"12.1", GRADE_EVAL * _result_grade/3 );

    char buffer[1024];
    snprintf( buffer, 1024, "(SD12.1) %d", idx);
    if (eval_check_successlog( "(SD12.1)", buffer )) {
        question_setgrade( questions,"12.1.1", GRADE_SUCCESS_ERROR );
    }

    eval_close_logs( "(SD12)" );

    // Bad nif
    eval_info("Evaluating SD12 - %s...", question_text(questions,"12.2"));
    eval_reset_except_stats();

    reset_dadosServidor();
    for( int i = 0; i < MAX_USERS; i++ )
        localDadosServidor.listUsers[i] = test_user(i);
    db = & localDadosServidor;

    _eval_msgsnd_data.status = 0;
    _eval_msgsnd_data.action = 1; // capture
    msgId = 0x1324;

    idx = 6;
    Login l =  test_user(idx);
    l.nif = -124356;
    l.pidCliente = 123546;
    msg.msgData.infoLogin.pidCliente = l.pidCliente;

    EVAL_CATCH( searchUserDB_SD12( l ) );
    _result_grade = 0;
    int _result_grade_log = 0;

    if (eval_check_errorlog("(SD12.1)","(SD12.1) ")) {
        ++_result_grade_log;
    }

    if ( _EXIT == _eval_env.stat ) {
        eval_error("(SD12) bad termination");
    } else {
        ++_result_grade;
    }

    if ( USER_NOT_FOUND != _searchUserDB_SD12_data.ret ) {
        eval_error("(SD12) Invalid return value");
    } else {
        ++_result_grade;
    }

    if ( 1 != _eval_msgsnd_data.status ) {
        eval_error("(SD12) msgsnd should have been exactly once");
    } else {
        ++_result_grade;
        MsgContent * msg;

        if ( 0x1324 != _eval_msgsnd_data.msqid ) {
            eval_error("(SD12) msgsnd() called with invalid msqid");
        } else {
            ++_result_grade;
        }
        if ( sizeof(msg->msgData) != _eval_msgsnd_data.msgsz ) {
            eval_error("(SD12) msgsnd() called with invalid msgsz");
        } else {
            ++_result_grade;
        }

        if ( 0 != _eval_msgsnd_data.msgflg ) {
            eval_error("(SD12) msgsnd() called with invalid msgflg");
        } else {
            ++_result_grade;
        }

        msg = _eval_msgsnd_data.msgp;
        if ( msg ) {
            ++_result_grade;
            if ( msg -> msgType != l.pidCliente ) {
                eval_error("(SD12) bad message type");
            } else {
                ++_result_grade;
            }
            if ( msg -> msgData.infoLogin.pidServidorDedicado != -1 ) {
                eval_error("(SD12) bad message data");
            } else {
                ++_result_grade;
            }
        } else {
            eval_error("(SD12) msgsnd() called with invalid msgp");
        }
    }

    if (eval_check_successlog("(SD12.2)", "(SD12.2) ")) {
        ++_result_grade_log;
    }
    question_setgrade( questions,"12.2", GRADE_EVAL * _result_grade/9 );
    question_setgrade( questions,"12.2.1", GRADE_SUCCESS_ERROR * _result_grade_log/2 );

    eval_close_logs( "(SD12)" );

    // Bad nif
    eval_info("Evaluating SD12 - %s...", question_text(questions,"12.3"));
    eval_reset_except_stats();

    reset_dadosServidor();
    for( int i = 0; i < MAX_USERS; i++ )
        localDadosServidor.listUsers[i] = test_user(i);
    db = & localDadosServidor;

    _eval_msgsnd_data.status = 0;
    _eval_msgsnd_data.action = 1; // capture
    msgId = 0x1324;

    idx = 6;
    l =  test_user(idx);
    sprintf(l.senha,"wrong!");
    l.pidCliente = 132456;
    msg.msgData.infoLogin.pidCliente = l.pidCliente;

    EVAL_CATCH( searchUserDB_SD12( l ) );
    _result_grade = 0;
    _result_grade_log = 0;

    if (eval_check_errorlog("(SD12.1)","(SD12.1) ")) {
        ++_result_grade_log;
    }

    if ( _EXIT == _eval_env.stat ) {
        eval_error("(SD12) bad termination");
    } else {
        ++_result_grade;
    }

    if ( USER_NOT_FOUND != _searchUserDB_SD12_data.ret ) {
        eval_error("(SD12) Invalid return value");
    } else {
        ++_result_grade;
    }

    if ( 1 != _eval_msgsnd_data.status ) {
        eval_error("(SD12) msgsnd should have been exactly once");
    } else {
        ++_result_grade;
        MsgContent * msg;

        if ( 0x1324 != _eval_msgsnd_data.msqid ) {
            eval_error("(SD12) msgsnd() called with invalid msqid");
        } else {
            ++_result_grade;
        }
        if ( sizeof(msg->msgData) != _eval_msgsnd_data.msgsz ) {
            eval_error("(SD12) msgsnd() called with invalid msgsz");
        } else {
            ++_result_grade;
        }

        if ( 0 != _eval_msgsnd_data.msgflg ) {
            eval_error("(SD12) msgsnd() called with invalid msgflg");
        } else {
            ++_result_grade;
        }

        msg = _eval_msgsnd_data.msgp;
        if ( msg ) {
            ++_result_grade;

            if ( msg -> msgType != l.pidCliente ) {
                eval_error("(SD12) bad message type");
            } else {
                ++_result_grade;
            }
            if ( msg -> msgData.infoLogin.pidServidorDedicado != -1 ) {
                eval_error("(SD12) bad message data");
            } else {
                ++_result_grade;
            }
        } else {
            eval_error("(SD12) msgsnd() called with invalid msgp");
        }
    }

    if (eval_check_successlog("(SD12.2)", "(SD12.2) ")) {
        ++_result_grade_log;
    }
    question_setgrade( questions,"12.3", GRADE_EVAL * _result_grade/9 );
    question_setgrade( questions,"12.3.1", GRADE_SUCCESS_ERROR * _result_grade_log/2 );

    eval_close_logs( "(SD12)" );

    return eval_complete("(SD12)");
}

/**
 * Wrapper for reserveUserDB_SD13()
 **/
struct {
    int action;
    int ret;
} _reserveUserDB_SD13_data;

int reserveUserDB_SD13(int indexClient, int pidCliente) {
    _reserveUserDB_SD13_data.ret = -1234;
    _reserveUserDB_SD13_data.ret = _student_reserveUserDB_SD13(indexClient, pidCliente);
    return _reserveUserDB_SD13_data.ret;
}

int eval_sd13( ) {
    eval_info("Evaluating SD13 - %s...", question_text(questions,"13"));
    eval_reset();

    _eval_semctl_data.status = 0;
    _eval_semctl_data.action = 2;
    _eval_semop_data.action = 1;
    _eval_semop_data.status = 0;

    reset_dadosServidor();
    db = &localDadosServidor;

    int indexClient = 5;
    int pidCliente = 0x1324;

    EVAL_CATCH( reserveUserDB_SD13( indexClient, pidCliente ) );
    int _result_grade = 0;

    if ( _EXIT == _eval_env.stat ) {
        eval_error("(SD13) bad termination");
    } else {
        ++_result_grade;
    }

    if ( RET_SUCCESS != _reserveUserDB_SD13_data.ret ) {
        eval_error("(SD13) Invalid return value");
    } else {
        ++_result_grade;
    }

    if ( ( db -> listUsers[indexClient].pidServidorDedicado != getpid() ) ||
         ( db -> listUsers[indexClient].pidCliente != pidCliente ) ) {
        eval_error("(SD13) Database not updated properly");
    } else {
        ++_result_grade;
    }
    question_setgrade( questions,"13", GRADE_EVAL * _result_grade/3 );

    if (eval_check_successlog("(SD13)","(SD13) ")) {
        question_setgrade( questions,"13.1", GRADE_SUCCESS_ERROR );
    }

    eval_close_logs( "(SD13)" );

    return eval_complete("(SD13)");
}

/**
 * Wrapper for sendProductList_SD14()
 **/
struct {
    int action;
    int ret;
} _sendProductList_SD14_data;

int sendProductList_SD14(int indexClient, int pidCliente) {
    _sendProductList_SD14_data.ret = -1234;
    _sendProductList_SD14_data.ret = _student_sendProductList_SD14(indexClient, pidCliente);
    return _sendProductList_SD14_data.ret;
}

int eval_sd14( ) {
    eval_info("Evaluating SD14 - %s...", question_text(questions,"14"));
    eval_reset();

    _eval_semctl_data.status = 0;
    _eval_semctl_data.action = 2;
    _eval_semop_data.action = 1;
    _eval_semop_data.status = 0;

    reset_dadosServidor();
    db = &localDadosServidor;

    localDadosServidor.listProducts[0].idProduto = 123456789;
    localDadosServidor.listProducts[1].idProduto = 123456789;
    localDadosServidor.listProducts[2].idProduto = 123456789;
    localDadosServidor.listProducts[3].idProduto = 123456789;
    localDadosServidor.listProducts[4].idProduto = 123456789;
    localDadosServidor.listProducts[5].idProduto = 123456789;
    localDadosServidor.listProducts[6].idProduto = 123456789;
    localDadosServidor.listProducts[7].idProduto = 123456789;
    localDadosServidor.listProducts[8].idProduto = 123456789;
    localDadosServidor.listProducts[4] = test_product(4);
    localDadosServidor.listProducts[8] = test_product(8);

    int indexClient = 5;
    localDadosServidor.listUsers[indexClient] = test_user( indexClient );
    int pidCliente = 0x1324;

    // Prepare a queue to receive the messages
    msgId = msgget( IPC_KEY, IPC_CREAT |  0600 );
    if ( msgId < 0 ) {
        eval_error( "(*critical*) Unable to create exclusive message queue for testing, key = 0x%X\n", IPC_KEY );
        eval_error( "(*critical*) Error was %s, aborting...\n", strerror(errno));
        exit(1);
    }

    _eval_msgsnd_data.status = 0;
    _eval_msgsnd_data.action = 0;

    EVAL_CATCH( sendProductList_SD14( indexClient, pidCliente ) );
    int _result_grade = 0;
    int _result_grade_log = 0;

    if ( _EXIT == _eval_env.stat ) {
        eval_error("(SD14) bad termination");
    } else {
        ++_result_grade;
    }

    if ( RET_SUCCESS != _sendProductList_SD14_data.ret ) {
        eval_error("(SD14) Invalid return value");
    } else {
        ++_result_grade;
    }

    if ( 3 != _eval_msgsnd_data.status ) {
        eval_error("(SD14) Invalid number of messages sent");
    } else {
        ++_result_grade;

        // SD14.2

        MsgContent msg;

        if ( msgId != _eval_msgsnd_data.msqid ) {
            eval_error("(SD14) msgsnd() called with wrong msqid");
        } else {
            ++_result_grade;
        }

        if ( sizeof(msg.msgData) != _eval_msgsnd_data.msgsz ) {
            eval_error("(SD14) msgsnd() called with wrong msgsz");
        } else {
            ++_result_grade;
        }

        if ( 0 != _eval_msgsnd_data.msgflg ) {
            eval_error("(SD14) msgsnd() called with wrong msgflg");
        } else {
            ++_result_grade;
        }

        ssize_t size;

        localDadosServidor.listUsers[indexClient].pidServidorDedicado = getpid();

        size = msgrcv( msgId, &msg, sizeof(msg.msgData), 0, IPC_NOWAIT );
        if ( size > 0 ) {
            ++_result_grade;

            if ( msg.msgType != pidCliente ) {
                eval_error("(SD14) invalid message type (1)");
            } else {
                ++_result_grade;
            }

            if ( compare_product( msg.msgData.infoProduto, test_product(4)) ) {
                eval_error("(SD14) invalid message data - product (1)");
            } else {
                ++_result_grade;
            }

            if ( strncmp( msg.msgData.infoLogin.nome, localDadosServidor.listUsers[indexClient].nome, 52 )) {
                eval_error("(SD14) invalid message data - user (1).nome");
            } else {
                ++_result_grade;
            }
            if ( msg.msgData.infoLogin.saldo != localDadosServidor.listUsers[indexClient].saldo) {
                eval_error("(SD14) invalid message data - user (1).saldo");
            } else {
                ++_result_grade;
            }
            // if ( compare_user( msg.msgData.infoLogin, localDadosServidor.listUsers[indexClient]) ) {
            //     eval_error("(SD14) invalid message data - user (1)");
            // }
        }

        size = msgrcv( msgId, &msg, sizeof(msg.msgData), 0, IPC_NOWAIT );
        if ( size > 0 ) {
            ++_result_grade;

            if ( msg.msgType != pidCliente ) {
                eval_error("(SD14) invalid message type (2)");
            } else {
                ++_result_grade;
            }

            if ( compare_product( msg.msgData.infoProduto, test_product(8)) ) {
                eval_error("(SD14) invalid message data (2)");
            } else {
                ++_result_grade;
            }

            if ( strncmp( msg.msgData.infoLogin.nome, localDadosServidor.listUsers[indexClient].nome, 52 )) {
                eval_error("(SD14) invalid message data - user (1).nome");
            } else {
                ++_result_grade;
            }
            if ( msg.msgData.infoLogin.saldo != localDadosServidor.listUsers[indexClient].saldo) {
                eval_error("(SD14) invalid message data - user (1).saldo");
            } else {
                ++_result_grade;
            }
            // if ( compare_user( msg.msgData.infoLogin, localDadosServidor.listUsers[indexClient]) ) {
            //     eval_error("(SD14) invalid message data - user (1)");
            // }
        }

        // SD14.3

        size = msgrcv( msgId, &msg, sizeof(msg.msgData), 0, IPC_NOWAIT );
        if ( size > 0 ) {
            ++_result_grade;

            if ( msg.msgType != pidCliente ) {
                eval_error("(SD14) invalid message type (end message)");
            } else {
                ++_result_grade;
            }
            if ( msg.msgData.infoProduto.idProduto != FIM_LISTA_PRODUTOS ) {
                eval_error("(SD14) invalid message content (end message)");
            } else {
                ++_result_grade;
            }
        }
    }

    // Success client message infoLogin prepared
    if (eval_check_successlog("(SD14.1)","(SD14.1) ")) {
        ++_result_grade_log;
    }

    // Product list sent to client
    if (eval_check_successlog("(SD14.2)","(SD14.2) ")) {
        ++_result_grade_log;
    }

    // Product list termination sent to client
    if (eval_check_successlog("(SD14.3)","(SD14.3) ")) {
        ++_result_grade_log;
    }
    question_setgrade( questions,"14", GRADE_EVAL * _result_grade/19 );
    question_setgrade( questions,"14.1", GRADE_SUCCESS_ERROR * _result_grade_log/3 );


    eval_close_logs( "(SD14)" );

    // Remove temporary message queue
    if ( msgctl( msgId, IPC_RMID, NULL ) == -1 ) {
        eval_error( "(*critical*) Unable to remove test message queue, key = 0x%X\n", IPC_KEY );
        eval_error( "(*critical*) Error was %s, aborting...\n", strerror(errno));
        exit(1);
    }

    return eval_complete("(SD14)");
}


/**
 * Wrapper for receiveClientOrder_SD15()
 **/
struct {
    int action;
    MsgContent ret;
} _receiveClientOrder_SD15_data;

MsgContent receiveClientOrder_SD15() {
    MsgContent msg = {0};
    _receiveClientOrder_SD15_data.ret = msg;
    _receiveClientOrder_SD15_data.ret = _student_receiveClientOrder_SD15();
    return _receiveClientOrder_SD15_data.ret;
}

int eval_sd15( ) {
    eval_info("Evaluating SD15 - %s...", question_text(questions,"15.1"));
    eval_reset();

    MsgContent snd = {0};
    snd.msgType = getpid();
    snd.msgData.infoLogin.nif = 123456;
    snd.msgData.infoLogin.pidCliente = 456789;
    snd.msgData.infoProduto.idProduto = 123;

    _eval_msgrcv_data.status = 0;
    _eval_msgrcv_data.action = 2; // inject

    _eval_msgrcv_data.msqid = -1;
    _eval_msgrcv_data.msgtyp = -1;
    _eval_msgrcv_data.msgflg = -1;

    _eval_msgrcv_data.msgp = &snd;
    _eval_msgrcv_data.msgsz = sizeof(snd.msgData);

    msgId = 0x1324;


    EVAL_CATCH( receiveClientOrder_SD15( ) );
    int _result_grade = 0;

    if ( _EXIT == _eval_env.stat ) {
        eval_error("(SD15) bad termination");
    } else {
        ++_result_grade;
    }

    if ( 1 != _eval_msgrcv_data.status ) {
        eval_error("(SD15) msgrcv() should have been called exactly once");
    } else {
        ++_result_grade;
        if ( msgId != _eval_msgrcv_data.msqid ) {
            eval_error("(SD15) msgrcv() called with invalid msqid");
        } else {
            ++_result_grade;
        }

        if ( getpid() != _eval_msgrcv_data.msgtyp ) {
            eval_error("(SD15) msgrcv() called with invalid msgtyp");
        } else {
            ++_result_grade;
        }

        if ( 0 != _eval_msgrcv_data.msgflg ) {
            eval_error("(SD15) msgrcv() called with invalid msgflg");
        } else {
            ++_result_grade;
        }

        MsgContent rcv = _receiveClientOrder_SD15_data.ret;

        if ( getpid() != rcv.msgType ) {
            eval_error("(SD15) invalid message type on return value");
        } else {
            ++_result_grade;
        }

        if ( ( rcv.msgData.infoLogin.nif != 123456 ) ||
            ( rcv.msgData.infoLogin.pidCliente != 456789 ) ||
            ( rcv.msgData.infoProduto.idProduto != 123 ) ) {
            eval_error("(SD15) invalid return value");
        } else {
            ++_result_grade;
        }
    }
    question_setgrade( questions,"15.1", GRADE_EVAL * _result_grade/7 );


    if (eval_check_successlog("(SD15)","(SD15) ")) {
        question_setgrade( questions,"15.1.1", GRADE_SUCCESS_ERROR );
    }

    eval_close_logs( "(SD15)" );

    eval_info("Evaluating SD15 - %s...", question_text(questions,"15.2"));
    eval_reset_except_stats();


    _eval_msgrcv_data.status = 0;
    _eval_msgrcv_data.action = 3; // error

    _eval_msgrcv_data.msqid = -1;
    _eval_msgrcv_data.msgtyp = -1;
    _eval_msgrcv_data.msgflg = -1;

    msgId = 0x1234;


    EVAL_CATCH( receiveClientOrder_SD15( ) );
    _result_grade = 0;

    if ( _EXIT == _eval_env.stat ) {
        eval_error("(SD15) bad termination");
    } else {
        ++_result_grade;
    }

    if ( 1 != _eval_msgrcv_data.status ) {
        eval_error("(SD15) msgrcv() should have been called exactly once");
    } else {
        ++_result_grade;
        if ( msgId != _eval_msgrcv_data.msqid ) {
            eval_error("(SD15) msgrcv() called with invalid msqid");
        } else {
            ++_result_grade;
        }

        if ( getpid() != _eval_msgrcv_data.msgtyp ) {
            eval_error("(SD15) msgrcv() called with invalid msgtyp");
        } else {
            ++_result_grade;
        }

        if ( 0 != _eval_msgrcv_data.msgflg ) {
            eval_error("(SD15) msgrcv() called with invalid msgflg");
        } else {
            ++_result_grade;
        }

        MsgContent rcv = _receiveClientOrder_SD15_data.ret;
        if ( ( rcv.msgData.infoLogin.nif != USER_NOT_FOUND ) ) {
            eval_error("(SD15) invalid return value");
        } else {
            ++_result_grade;
        }
    }
    question_setgrade( questions,"15.2", GRADE_EVAL * _result_grade/6 );

    if (eval_check_errorlog("(SD15)","(SD15) ")) {
        question_setgrade( questions,"15.2.1", GRADE_SUCCESS_ERROR );
    }

    eval_close_logs( "(SD15)" );
    return eval_complete("(SD15)");
}

/**
 * Wrapper for sendPurchaseAck_SD16()
 **/
struct {
    int action;
    int ret;
} _sendPurchaseAck_SD16_data;

int sendPurchaseAck_SD16( int idProduto, int pidCliente ) {
    _sendPurchaseAck_SD16_data.ret = -1234;
    _sendPurchaseAck_SD16_data.ret = _student_sendPurchaseAck_SD16( idProduto, pidCliente );
    return _sendPurchaseAck_SD16_data.ret;
}

int eval_sd16( ) {
    // Normal sale
    eval_info("Evaluating SD16 - %s...", question_text(questions,"16.1"));
    eval_reset();

    _eval_msgsnd_data.status = 0;
    _eval_msgsnd_data.action = 1; // capture

    _eval_msgsnd_data.msqid = -1;
    _eval_msgsnd_data.msgflg = -1;

    msgId = 0x1234;

    initlog(&_data_log);

    semId = 0x2345;

    _eval_semctl_data.status = 0;
    _eval_semctl_data.action = 2;

    _eval_semop_data.action = 2;
    _eval_semop_data.status = 0;

    reset_dadosServidor();
    db = &localDadosServidor;

    localDadosServidor.listProducts[0].idProduto = 123456789;
    localDadosServidor.listProducts[1].idProduto = 123456789;
    localDadosServidor.listProducts[2].idProduto = 123456789;
    localDadosServidor.listProducts[3].idProduto = 123456789;
    localDadosServidor.listProducts[4].idProduto = 123456789;
    int idProduto = 5;
    localDadosServidor.listProducts[5] = test_product(5);
    localDadosServidor.listProducts[5].stock = 2;
    localDadosServidor.listProducts[5].idProduto = 5;
    int pidCliente = 123456;

    EVAL_CATCH( sendPurchaseAck_SD16( idProduto, pidCliente ) );
    int _result_grade = 0;
    int _result_grade_log = 0;

    if ( _EXIT == _eval_env.stat ) {
        eval_error("(SD16) bad termination");
    } else {
        ++_result_grade;
    }

    if ( 2 != _eval_semop_data.status ) {
        eval_error("(SD16) semop() should have been called exactly twice");
    } else {
        ++_result_grade;

        int lower = findinlog( &_data_log, "semop,%d,%d,%d,%d", semId,SEM_PRODUCTS,-1,0);
        int raise = findinlog( &_data_log, "semop,%d,%d,%d,%d", semId,SEM_PRODUCTS,+1,0);

        if ( lower < 0 || raise < 0 ) {
            eval_error("(SD16) semaphores not used properly");
        } else {
            ++_result_grade;
        }
        if ( raise < lower ) {
            eval_error("(SD16) semaphores not used properly");
        } else {
            ++_result_grade;
        }
    }

    if ( 1 != _eval_msgsnd_data.status ) {
        eval_error("(SD16) msgsnd() should have been called exactly once");
    } else {
        ++_result_grade;

        MsgContent * msg = (MsgContent *) _eval_msgsnd_data.msgp;

        if ( msgId != _eval_msgsnd_data.msqid ) {
            eval_error("(SD16) msgsnd() called with wrong msqid");
        } else {
            ++_result_grade;
        }
        if ( 0 != _eval_msgsnd_data.msgflg ) {
            eval_error("(SD16) msgsnd() called with wrong msgflg");
        } else {
            ++_result_grade;
        }

        if ( sizeof(msg -> msgData) != _eval_msgsnd_data.msgsz ) {
            eval_error("(SD16) msgsnd() called with wrong msgsz");
        } else {
            ++_result_grade;
        }

        if ( msg ) {
            ++_result_grade;

            if ( pidCliente != msg -> msgType ) {
                eval_error("(SD16) Wrong message type");
            } else {
                ++_result_grade;
            }

            if ( msg -> msgData.infoProduto.idProduto != PRODUTO_COMPRADO ) {
                eval_error("(SD16) Wrong messsage content");
            } else {
                ++_result_grade;
            }
        } else {
            eval_error("(SD16) msgsnd() called with bad msgp");
        }

        if ( 1 != localDadosServidor.listProducts[idProduto].stock ) {
            eval_error("(SD16) stock not updated correctly");
        } else {
            ++_result_grade;
        }
    }

    if (eval_check_successlog("(SD16.2)","(SD16.2) ")) {
        ++_result_grade_log;
    }

    if (eval_check_successlog("(SD16.3)","(SD16.3) ")) {
        ++_result_grade_log;
    }
    question_setgrade( questions,"16.1", GRADE_EVAL * _result_grade/12 );
    question_setgrade( questions,"16.1.1", GRADE_SUCCESS_ERROR * _result_grade_log/2 );

    eval_close_logs( "(SD16)" );

    // Out of stock

    eval_info("Evaluating SD16 - %s...", question_text(questions,"16.2"));
    eval_reset_except_stats();

    semId = 0x21345;

    _eval_semctl_data.status = 0;
    _eval_semctl_data.action = 2;

    _eval_semop_data.action = 1;
    _eval_semop_data.status = 0;

    _eval_msgsnd_data.status = 0;
    _eval_msgsnd_data.action = 1; // capture

    _eval_msgsnd_data.msqid = -1;
    _eval_msgsnd_data.msgflg = -1;

    msgId = 0x1324;


    reset_dadosServidor();
    db = &localDadosServidor;

    localDadosServidor.listProducts[0].idProduto = 123456789;
    localDadosServidor.listProducts[1].idProduto = 123456789;
    localDadosServidor.listProducts[2].idProduto = 123456789;
    localDadosServidor.listProducts[3].idProduto = 123456789;
    localDadosServidor.listProducts[4].idProduto = 123456789;
    idProduto = 5;
    localDadosServidor.listProducts[5] = test_product(5);
    localDadosServidor.listProducts[5].idProduto = idProduto;
    localDadosServidor.listProducts[5].stock = 0;
    pidCliente = 123456;

    EVAL_CATCH( sendPurchaseAck_SD16( idProduto, pidCliente ) );
    _result_grade = 0;
    _result_grade_log = 0;

    if ( _EXIT == _eval_env.stat ) {
        eval_error("(SD16) bad termination");
    } else {
        ++_result_grade;
    }

    if ( 1 != _eval_msgsnd_data.status ) {
        eval_error("(SD16) msgsnd() should have been called exactly once");
    } else {
        ++_result_grade;

        MsgContent * msg = (MsgContent *) _eval_msgsnd_data.msgp;

        if ( msgId != _eval_msgsnd_data.msqid ) {
            eval_error("(SD16) msgsnd() called with wrong msqid");
        } else {
            ++_result_grade;
        }
        if ( 0 != _eval_msgsnd_data.msgflg ) {
            eval_error("(SD16) msgsnd() called with wrong msgflg");
        } else {
            ++_result_grade;
        }

        if ( sizeof(msg -> msgData) != _eval_msgsnd_data.msgsz ) {
            eval_error("(SD16) msgsnd() called with wrong msgsz");
        } else {
            ++_result_grade;
        }

        if ( msg ) {
            ++_result_grade;

            if ( pidCliente != msg -> msgType ) {
                eval_error("(SD16) Wrong message type");
            } else {
                ++_result_grade;
            }

            if ( msg -> msgData.infoProduto.idProduto != PRODUTO_NAO_COMPRADO ) {
                eval_error("(SD16) Wrong messsage content");
            } else {
                ++_result_grade;
            }
        } else {
            eval_error("(SD16) msgsnd() called with bad msgp");
        }

        if ( 0 != localDadosServidor.listProducts[idProduto].stock ) {
            eval_error("(SD16) stock updated incorrectly");
        } else {
            ++_result_grade;
        }
    }

    if (eval_check_errorlog("(SD16.2)","(SD16.2) ")) {
        ++_result_grade_log;
    }

    if (eval_check_successlog("(SD16.3)","(SD16.3) ")) {
        ++_result_grade_log;
    }
    question_setgrade( questions,"16.2", GRADE_EVAL * _result_grade/9 );
    question_setgrade( questions,"16.2.1", GRADE_SUCCESS_ERROR * _result_grade_log/2 );

    eval_close_logs( "(SD16)" );

    // Not found

    eval_info("Evaluating SD16 - %s...", question_text(questions,"16.3"));
    eval_reset_except_stats();

    semId = 0x2345;

    _eval_semctl_data.status = 0;
    _eval_semctl_data.action = 2;
    _eval_semop_data.action = 1;
    _eval_semop_data.status = 0;

    _eval_msgsnd_data.status = 0;
    _eval_msgsnd_data.action = 1; // capture

    _eval_msgsnd_data.msqid = -1;
    _eval_msgsnd_data.msgflg = -1;

    msgId = 0x1234;

    reset_dadosServidor();
    db = &localDadosServidor;

    localDadosServidor.listProducts[0].idProduto = 123456789;
    localDadosServidor.listProducts[1].idProduto = 123456789;
    localDadosServidor.listProducts[2].idProduto = 123456789;
    localDadosServidor.listProducts[3].idProduto = 123456789;
    localDadosServidor.listProducts[4].idProduto = 123456789;
    idProduto = 5;
    localDadosServidor.listProducts[5] = test_product(5);
    localDadosServidor.listProducts[5].idProduto = 1;
    localDadosServidor.listProducts[5].stock = 2;
    pidCliente = 123456;

    EVAL_CATCH( sendPurchaseAck_SD16( idProduto, pidCliente ) );
    _result_grade = 0;
    _result_grade_log = 0;

    if ( _EXIT == _eval_env.stat ) {
        eval_error("(SD16) bad termination");
    } else {
        ++_result_grade;
    }

    if ( 1 != _eval_msgsnd_data.status ) {
        eval_error("(SD16) msgsnd() should have been called exactly once");
    } else {
        ++_result_grade;

        MsgContent * msg = (MsgContent *) _eval_msgsnd_data.msgp;

        if ( msgId != _eval_msgsnd_data.msqid ) {
            eval_error("(SD16) msgsnd() called with wrong msqid");
        } else {
            ++_result_grade;
        }
        if ( 0 != _eval_msgsnd_data.msgflg ) {
            eval_error("(SD16) msgsnd() called with wrong msgflg");
        } else {
            ++_result_grade;
        }

        if ( sizeof(msg -> msgData) != _eval_msgsnd_data.msgsz ) {
            eval_error("(SD16) msgsnd() called with wrong msgsz");
        } else {
            ++_result_grade;
        }

        if ( msg ) {
            ++_result_grade;

            if ( pidCliente != msg -> msgType ) {
                eval_error("(SD16) Wrong message type");
            } else {
                ++_result_grade;
            }

            if ( msg -> msgData.infoProduto.idProduto != PRODUTO_NAO_COMPRADO ) {
                eval_error("(SD16) Wrong messsage content");
            } else {
                ++_result_grade;
            }
        } else {
            eval_error("(SD16) msgsnd() called with bad msgp");
        }
    }

    if (eval_check_errorlog("(SD16.2)","(SD16.2) ")) {
        ++_result_grade_log;
    }

    if (eval_check_successlog("(SD16.3)","(SD16.3) ")) {
        ++_result_grade_log;
    }
    question_setgrade( questions,"16.3", GRADE_EVAL * _result_grade/8 );
    question_setgrade( questions,"16.3.1", GRADE_SUCCESS_ERROR * _result_grade_log/2 );

    eval_close_logs( "(SD16)" );

    // Cancelled purchase
    eval_info("Evaluating SD16 - %s...", question_text(questions,"16.4"));
    eval_reset_except_stats();

    _eval_msgsnd_data.status = 0;
    _eval_msgsnd_data.action = 1; // capture

    _eval_msgsnd_data.msqid = -1;
    _eval_msgsnd_data.msgflg = -1;

    msgId = 0x1324;

    reset_dadosServidor();
    db = &localDadosServidor;

    pidCliente = 132456;

    EVAL_CATCH( sendPurchaseAck_SD16( PRODUCT_NOT_FOUND, pidCliente ) );
    _result_grade = 0;
    _result_grade_log = 0;

    if ( _EXIT == _eval_env.stat ) {
        eval_error("(SD16) bad termination");
    } else {
        ++_result_grade;
    }

    if ( 1 != _eval_msgsnd_data.status ) {
        eval_error("(SD16) msgsnd() should have been called exactly once");
    } else {
        ++_result_grade;

        MsgContent * msg = (MsgContent *) _eval_msgsnd_data.msgp;

        if ( msgId != _eval_msgsnd_data.msqid ) {
            eval_error("(SD16) msgsnd() called with wrong msqid");
        } else {
            ++_result_grade;
        }
        if ( 0 != _eval_msgsnd_data.msgflg ) {
            eval_error("(SD16) msgsnd() called with wrong msgflg");
        } else {
            ++_result_grade;
        }

        if ( sizeof(msg -> msgData) != _eval_msgsnd_data.msgsz ) {
            eval_error("(SD16) msgsnd() called with wrong msgsz");
        } else {
            ++_result_grade;
        }

        if ( msg ) {
            ++_result_grade;

            if ( pidCliente != msg -> msgType ) {
                eval_error("(SD16) Wrong message type");
            } else {
                ++_result_grade;
            }

            if ( msg -> msgData.infoProduto.idProduto != PRODUTO_NAO_COMPRADO ) {
                eval_error("(SD16) Wrong messsage content");
            } else {
                ++_result_grade;
            }
        } else {
            eval_error("(SD16) msgsnd() called with bad msgp");
        }
    }

    free( _eval_msgsnd_data.msgp );

    if (eval_check_errorlog("(SD16.1)","(SD16.1) ")) {
        ++_result_grade_log;
    }

    if (eval_check_successlog("(SD16.3)","(SD16.3) ")) {
        ++_result_grade_log;
    }
    question_setgrade( questions,"16.4", GRADE_EVAL * _result_grade/8 );
    question_setgrade( questions,"16.4.1", GRADE_SUCCESS_ERROR * _result_grade_log/2 );

    eval_close_logs( "(SD16)" );

    return eval_complete("(SD16)");
}

struct {
    int status;
    int action;
    int ret;
} _shutdownAndExit_SD17_data;


void shutdownAndExit_SD17() {

    _shutdownAndExit_SD17_data.status++;
    switch( _shutdownAndExit_SD17_data.action ) {
    case(1):
        _eval_exit(RET_SUCCESS);
    default:
        _student_shutdownAndExit_SD17();
    }
}

int eval_sd17( ) {
    eval_info("Evaluating SD17 - %s...", question_text(questions,"17"));
    eval_reset();

    _eval_semctl_data.status = 0;
    _eval_semctl_data.action = 2;
    _eval_semop_data.action = 1;
    _eval_semop_data.status = 0;

    indexClient = 2;

    reset_dadosServidor();
    db = &localDadosServidor;

    localDadosServidor.listUsers[indexClient].pidServidorDedicado = 123456;
    localDadosServidor.listUsers[indexClient].pidCliente = 78901;

    _shutdownAndExit_SD17_data.action = 0;

    EVAL_CATCH( shutdownAndExit_SD17( ) );
    int _result_grade = 0;
    int _result_grade_log = 0;

    if ( _EXIT != _eval_env.stat ) {
        eval_error("(SD17) bad termination");
    } else {
        ++_result_grade;
        if ( RET_SUCCESS != _eval_exit_data.status ) {
            eval_error("(SD17) bad exit status");
        } else {
            ++_result_grade;
        }
    }

    if (eval_check_successlog("(SD17)","(SD17) Shutdown Servidor Dedicado")) {
        ++_result_grade_log;
    }

    if ( localDadosServidor.listUsers[indexClient].pidServidorDedicado >= 0) {
        eval_error("(SD17.1) pidServidorDedicado not cleared");
    } else {
        ++_result_grade;
    }
    if ( localDadosServidor.listUsers[indexClient].pidCliente >= 0 ) {
        eval_error("(SD17.1) pidCliente not cleared");
    } else {
        ++_result_grade;
    }

    if (eval_check_successlog("(SD17.1)","(SD17.1) ")) {
        ++_result_grade_log;
    }
    question_setgrade( questions,"17", GRADE_EVAL * _result_grade/4 );
    question_setgrade( questions,"17.1", GRADE_SUCCESS_ERROR * _result_grade_log/2 );

    eval_close_logs( "(SD17)" );

    return eval_complete("(SD17)");

}

void handleSignalSIGUSR1_SD18(int sinalRecebido) {
    _student_handleSignalSIGUSR1_SD18( sinalRecebido );
}

int eval_sd18( ) {
    eval_info("Evaluating SD18 - %s...", question_text(questions,"18"));
    eval_reset();

    reset_dadosServidor();
    db = &localDadosServidor;
    indexClient = 2;
    localDadosServidor.listUsers[indexClient].pidCliente = 78901;
    msg.msgData.infoLogin.pidCliente = localDadosServidor.listUsers[indexClient].pidCliente;

    _shutdownAndExit_SD17_data.status = 0;
    _shutdownAndExit_SD17_data.action = 1;

    _eval_kill_data.status = 0;
    _eval_kill_data.pid = -1;
    _eval_kill_data.sig = -1;
    _eval_kill_data.action = 2; // don't send signal, just capture data

    EVAL_CATCH( handleSignalSIGUSR1_SD18( SIGUSR1 ) );
    int _result_grade = 0;

    if ( _EXIT != _eval_env.stat ) {
        eval_error("(SD18) bad termination");
    } else {
        ++_result_grade;
        if ( RET_SUCCESS != _eval_exit_data.status ) {
            eval_error("(SD18) bad exit status");
        } else {
            ++_result_grade;
        }
    }

    if ( 1 != _eval_kill_data.status ) {
        eval_error("(SD18) kill() should have been called exactly once");
    } else {
        ++_result_grade;

        if ( _eval_kill_data.pid != localDadosServidor.listUsers[indexClient].pidCliente ) {
            eval_error("(SD18) kill() called with wrong pid");
        } else {
            ++_result_grade;
        }
        if ( _eval_kill_data.sig != SIGUSR2 ) {
            eval_error("(SD18) kill() called with wrong sig");
        } else {
            ++_result_grade;
        }
    }
    question_setgrade( questions,"18", GRADE_EVAL * _result_grade/5 );

    if (eval_check_successlog("(SD18)","(SD18) ")) {
        question_setgrade( questions,"18.1", GRADE_SUCCESS_ERROR );
    }


    if ( 1 != _shutdownAndExit_SD17_data.status ) {
        eval_error("(SD18) shutdownAndExit_SD17() should have been called exactly once");
    }

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
    nerr += eval_s9();
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