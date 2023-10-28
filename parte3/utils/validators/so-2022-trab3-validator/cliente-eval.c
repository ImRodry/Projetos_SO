#define _CLIENTE 1
#include "common.h"
#include "so_utils.h"
#include <fcntl.h>
#include <sys/stat.h>
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

// Global variables
int msgId = RET_ERROR;                      // Variável que tem o ID da Message Queue
MsgContent msg;                             // Variável que serve para todas as mensagens trocadas entre Cliente e Servidor


// Question list
question_t questions[] = {
    {"1.1",   "Message queue does not exist", 0.0 },
    {"1.1.1",   "Validate log so_success or so_error", 0.0 },
    {"1.2",   "Message queue exists", 0.0 },
    {"1.2.1",   "Validate log so_success or so_error", 0.0 },

    {"2",     "Arm signal handlers", 0.0 },
    {"2.1",   "Validate log so_success or so_error", 0.0 },

    {"3",     "Get login data", 0.0 },
    {"3.1",   "Validate log so_success or so_error", 0.0 },

    {"4.1",   "Send login message (ok)", 0.0 },
    {"4.1.1",   "Validate log so_success or so_error", 0.0 },
    {"4.2",   "Send login message (fail)", 0.0 },
    {"4.2.1",   "Validate log so_success or so_error", 0.0 },

    {"5",     "Set timer", 0.0 },
    {"5.1",   "Validate log so_success or so_error", 0.0 },

    {"6.1",    "Bad login", 0.0 },
    {"6.1.1",   "Validate log so_success or so_error", 0.0 },
    {"6.2",    "Good login", 0.0 },
    {"6.2.1",   "Validate log so_success or so_error", 0.0 },

    {"7.1",   "Get product id", 0.0 },
    {"7.1.1",   "Validate log so_success or so_error", 0.0 },
    {"7.2",   "Get product id (bad input)", 0.0 },
    {"7.2.1",   "Validate log so_success or so_error", 0.0 },

    {"8.1",   "Send client order message (ok)", 0.0 },
    {"8.1.1",   "Validate log so_success or so_error", 0.0 },
    {"8.2",   "Send client order message (fail)", 0.0 },
    {"8.2.1",   "Validate log so_success or so_error", 0.0 },

    {"9.1",   "Purchase ok", 0.0 },
    {"9.1.1",   "Validate log so_success or so_error", 0.0 },
    {"9.2",   "Purchase failed", 0.0 },
    {"9.2.1",   "Validate log so_success or so_error", 0.0 },

    {"10",     "Handle SIGUSR2", 0.0 },
    {"10.1",   "Validate log so_success or so_error", 0.0 },

    {"11",     "Handle SIGINT", 0.0 },
    {"11.1",   "Validate log so_success or so_error", 0.0 },

    {"12",     "Handle SIGALRM", 0.0 },
    {"12.1",   "Validate log so_success or so_error", 0.0 },

    {"---", "_end_",0.0}
};


/**
 * Wrapper for initMsg_C1
 *
 */

struct {
    int action;
    int result;
} _initMsg_C1;

int initMsg_C1() {
    _initMsg_C1.result = -1;
    _initMsg_C1.result = _student_initMsg_C1();
    return _initMsg_C1.result;
}

/**
 * Evaluate C1
 */
int eval_c1( ) {
    eval_info("Evaluating C1.1 - %s...", question_text(questions,"1.1"));
    eval_reset();

    _eval_msgget_data.status = 0;
    _eval_msgget_data.action = 1; // Return dummy queue ID
    _eval_msgget_data.msgflg = -1;
    _eval_msgget_data.msqid = 0xD1840;

    EVAL_CATCH( initMsg_C1() );
    int _result_grade = 0;

    if ( _EXIT == _eval_env.stat ) {
        eval_error( "(C1) bad termination");
    } else {
        ++_result_grade;
    }

    if ( 1 != _eval_msgget_data.status ) {
        eval_error("(C1.1) mssget() should have been called exactly once, was called %d times.", _eval_msgget_data.status );
    } else {
        ++_result_grade;

        if( IPC_KEY != _eval_msgget_data.key ) {
            eval_error( "(C1) invalid IPC key");
        } else {
            ++_result_grade;
        }

        if( 0 != _eval_msgget_data.msgflg ) {
            eval_error( "(C1) invalid msgflg");
        } else {
            ++_result_grade;
        }

        if ( 0xD1840 != _eval_msgget_data.ret ) {
            eval_error( "(C1) invalid value returned");
        } else {
            ++_result_grade;
        }

        if (eval_check_successlog( "(C1)", "(C1) " )) {
            question_setgrade( questions,"1.1.1", GRADE_SUCCESS_ERROR );
        }
    }
    question_setgrade( questions,"1.1", GRADE_EVAL * _result_grade/5 );

    eval_info("Evaluating C1.2 - %s...", question_text(questions,"1.2"));
    eval_reset_except_stats();

    _eval_msgget_data.status = 0;
    _eval_msgget_data.action = 2; // Return error
    _eval_msgget_data.msgflg = -1;
    _eval_msgget_data.msqid = 0xD1840;

    EVAL_CATCH( initMsg_C1() );
    _result_grade = 0;
    if ( _EXIT == _eval_env.stat ) {
        eval_error( "(C1) bad termination");
    } else {
        ++_result_grade;
    }

    if ( 1 != _eval_msgget_data.status ) {
        eval_error("(C1.1) mssget() should have been called exactly once, was called %d times.", _eval_msgget_data.status );
    } else {
        ++_result_grade;

        if( IPC_KEY != _eval_msgget_data.key ) {
            eval_error( "(C1) invalid IPC key");
        } else {
            ++_result_grade;
        }

        if( 0 != _eval_msgget_data.msgflg ) {
            eval_error( "(C1) invalid msgflg");
        } else {
            ++_result_grade;
        }

        if ( RET_ERROR != _eval_msgget_data.ret ) {
            eval_error( "(C1) invalid value returned");
        } else {
            ++_result_grade;
        }

        if (eval_check_errorlog( "(C1)", "(C1) " )) {
            question_setgrade( questions,"1.2.1", GRADE_SUCCESS_ERROR );
        }
    }
    question_setgrade( questions,"1.2", GRADE_EVAL * _result_grade/5 );

    eval_close_logs( "(C1)" );
    return eval_complete("(C1)");
}

/**
 * Wrapper for triggerSignals_C2
 *
 */
struct {
    int action;
    int result;
} _triggerSignals_C2_data;

int triggerSignals_C2() {
    _triggerSignals_C2_data.result = -1024;
    _triggerSignals_C2_data.result = _student_triggerSignals_C2();
    return 0;
}

/**
 * Evaluate C2
 */
int eval_c2( ) {
    eval_info("Evaluating C2 - %s...", question_text(questions,"2"));
    eval_reset();

    initlog(&_data_log);
    _eval_signal_data.action = 2;

    EVAL_CATCH( triggerSignals_C2() );
    int _result_grade = 0;

    if ( _EXIT == _eval_env.stat || _SUCCESS != _triggerSignals_C2_data.result ) {
        eval_error("(C2) function exited or returned error when test should be successful");
    } else {
        ++_result_grade;
    }

    if ( findinlog( &_data_log, "signal,%d,%p", SIGUSR2, trataSinalSIGUSR2_C10 ) < 0 ) {
        eval_error("(C2) SIGUSR2 not armed properly");
    } else {
        ++_result_grade;
    }

    if ( findinlog( &_data_log, "signal,%d,%p", SIGINT, trataSinalSIGINT_C11 ) < 0 ) {
        eval_error("(C2) SIGINT not armed properly");
    } else {
        ++_result_grade;
    }

    if ( findinlog( &_data_log, "signal,%d,%p", SIGALRM, trataSinalSIGALRM_C12 ) < 0 ) {
        eval_error("(C2) SIGALRM not armed properly");
    } else {
        ++_result_grade;
    }
    question_setgrade( questions,"2", GRADE_EVAL * _result_grade/4 );

    if (eval_check_successlog( "(C2)", "(C2) " )) {
        question_setgrade( questions,"2.1", GRADE_SUCCESS_ERROR );
    }

    eval_close_logs( "(C2)" );
    return eval_complete("(C2)");
}

/**
 * Wrapper for getDadosPedidoUtilizador_C3()
 **/
struct {
    int action;
    MsgContent msg;
} _getDadosPedidoUtilizador_C3_data;

MsgContent getDadosPedidoUtilizador_C3() {
    memset( (void *) &_getDadosPedidoUtilizador_C3_data.msg, 0, sizeof(MsgContent) );
    _getDadosPedidoUtilizador_C3_data.msg = _student_getDadosPedidoUtilizador_C3();
    return _getDadosPedidoUtilizador_C3_data.msg;
}

/**
 * Evaluate C3
 */
int eval_c3( ) {
    eval_info("Evaluating C3 - %s...", question_text(questions,"3"));
    eval_reset();

    const int nif = 987654321;
    const char senha[] = "a1b2c3";

    FILE* ft;
    ft = fopen(FILE_STDIN,"w");
    fprintf(ft,"%d\n", nif );
    fprintf(ft,"%s\n", senha );
    for(int i = 0; i < 5; i++ ) fprintf(ft,"\n" );
    fclose(ft);

    unlink( FILE_STDOUT );

    // Redirect standard I/O to files
    EVAL_CATCH_IO( getDadosPedidoUtilizador_C3(), FILE_STDIN, FILE_STDOUT );
    int _result_grade = 0;

    if ( _EXIT == _eval_env.stat ) {
        eval_error( "(C3) bad termination");
    } else {
        ++_result_grade;
    }

    // Remove I/O files
    unlink( FILE_STDIN );
    unlink( FILE_STDOUT );

    if ( nif != _getDadosPedidoUtilizador_C3_data.msg.msgData.infoLogin.nif ) {
        eval_error( "(C3) invalid msg.msgData.infoLogin.nif" );
    } else {
        ++_result_grade;
    }

    if ( strncmp( senha, _getDadosPedidoUtilizador_C3_data.msg.msgData.infoLogin.senha, strlen(senha) ) ) {
        eval_error( "(C3) invalid msg.msgData.infoLogin.senha");
    } else {
        ++_result_grade;
    }

    if ( getpid() != _getDadosPedidoUtilizador_C3_data.msg.msgData.infoLogin.pidCliente ) {
        eval_error( "(C3) invalid msg.msgData.infoLogin.pidCliente");
    } else {
        ++_result_grade;
    }
    question_setgrade( questions,"3", GRADE_EVAL * _result_grade/4 );

    char buffer[1024];
    snprintf( buffer, 1024, "(C3) %d %s %d", nif, senha, getpid());
    if (eval_check_successlog( "(C3)", buffer )) {
        question_setgrade( questions,"3.1", GRADE_SUCCESS_ERROR );
    }

    eval_close_logs( "(C3)" );
    return eval_complete("(C3)");
}

/**
 * Wrapper for sendClientLogin_C4()
 **/
struct {
    int action;
    int result;
} _sendClientLogin_C4_data;


int sendClientLogin_C4(MsgContent msg) {
    _sendClientLogin_C4_data.result = -1234;
    _sendClientLogin_C4_data.result = _student_sendClientLogin_C4( msg );
    return _sendClientLogin_C4_data.result;
}

/**
 * Evaluate C4
 */
int eval_c4( ) {
    eval_info("Evaluating C4.1 - %s...", question_text(questions,"4.1"));
    eval_reset();

    _eval_msgsnd_data.status = 0;
    _eval_msgsnd_data.action = 1; // capture
    _eval_msgsnd_data.msgp = NULL;

    msgId = 0xD1840;

    MsgContent login = {0};
    login.msgType = -1;
    login.msgData.infoLogin.nif = 987654321;
    login.msgData.infoLogin.pidCliente = 132546;
    strncpy( login.msgData.infoLogin.senha, "a1b2c3", 19 );

    EVAL_CATCH( sendClientLogin_C4( login ) );
    int _result_grade = 0;

    if ( _EXIT == _eval_env.stat || RET_ERROR == _sendClientLogin_C4_data.result ) {
        eval_error("(C4) function exited or returned error when test should be successful");
    } else {
        ++_result_grade;

        if ( 1 != _eval_msgsnd_data.status ) {
            eval_error ("(C4) msgsnd() should have been called exactly once, was called %d times.", _eval_msgsnd_data.status );
        } else {
            ++_result_grade;

            if ( msgId != _eval_msgsnd_data.msqid ) {
                eval_error("(C4) Invalid msqid argument used on msgsnd()");
            } else {
                ++_result_grade;
            }

            MsgContent *m;

            if ( _eval_msgsnd_data.msgsz != sizeof(m -> msgData) ) {
                eval_error("(C4) Invalid msgsz argument used on msgsnd()");
            } else {
                ++_result_grade;
            }

            m = _eval_msgsnd_data.msgp;

            if ( m == NULL ) {
                eval_error("(C4) Invalid msgp argument in msgsnd()" );
            } else {
                ++_result_grade;
            }

            if ( _eval_msgsnd_data.msgsz == sizeof(m -> msgData) && m ) {
                if ( MSGTYPE_LOGIN != m -> msgType ) {
                    eval_error("(C4) Invalid .msgType");
                } else {
                    ++_result_grade;
                }

                if ( login.msgData.infoLogin.nif != m -> msgData.infoLogin.nif ) {
                    eval_error("(C4) Invalid msgData.infoLogin.nif");
                } else {
                    ++_result_grade;
                }

                if ( login.msgData.infoLogin.pidCliente != m -> msgData.infoLogin.pidCliente ) {
                    eval_error("(C4) Invalid msgData.infoLogin.pidCliente");
                } else {
                    ++_result_grade;
                }

                if ( strncmp( login.msgData.infoLogin.senha, m -> msgData.infoLogin.senha, 19) != 0  ) {
                    eval_error("(C4) Invalid .msgData.infoLogin.senha");
                } else {
                    ++_result_grade;
                }
            }
        }
        question_setgrade( questions,"4.1", GRADE_EVAL * _result_grade/9 );

        if (eval_check_successlog( "(C4)", "(C4) " )) {
            question_setgrade( questions,"4.1.1", GRADE_SUCCESS_ERROR );
        }
    }

    if ( _eval_msgsnd_data.msgp ) {
        free( _eval_msgsnd_data.msgp );
        _eval_msgsnd_data.msgp = NULL;
    }

    eval_close_logs( "(C4)" );

    eval_info("Evaluating C4.2 - %s...", question_text(questions,"4.2"));
    eval_reset_except_stats();

    _eval_msgsnd_data.status = 0;
    _eval_msgsnd_data.action = 2; // error
    _eval_msgsnd_data.msgp = NULL;

    EVAL_CATCH( sendClientLogin_C4( login ) );
    _result_grade = 0;

    if ( _EXIT == _eval_env.stat ) {
        eval_error( "(C4) bad termination");
    } else {
        ++_result_grade;
        if ( RET_ERROR != _sendClientLogin_C4_data.result ) {
            eval_error("(C4) Invalid return value, should be RET_ERROR");
        } else {
            ++_result_grade;
        }

        if (eval_check_errorlog( "(C4)", "(C4) " )) {
            question_setgrade( questions,"4.2.1", GRADE_SUCCESS_ERROR );
        }
    }
    question_setgrade( questions,"4.2", GRADE_EVAL * _result_grade/2 );

    eval_close_logs( "(C4)" );
    return eval_complete("(C4)");
}

/**
 * Wrapper for configuraTemporizador_C5
 *
 */
void configuraTemporizador_C5(int tempoEspera) {
    _student_configuraTemporizador_C5( tempoEspera );
}

/**
 * Evaluate C5
 */
int eval_c5( ) {
    eval_info("Evaluating C5 - %s...", question_text(questions,"5"));
    eval_reset();

    _eval_alarm_data.status = 0;
    _eval_alarm_data.action = 1;

    int tempoEspera = 1114;

    EVAL_CATCH( configuraTemporizador_C5(tempoEspera) );
    int _result_grade = 0;

    if ( _EXIT == _eval_env.stat ) {
        eval_error( "(C5) bad termination");
    } else {
        ++_result_grade;
    }

    if ( 1 != _eval_alarm_data.status ) {
        eval_error("(C5) alarm() should have been called exactly once");
    } else {
        ++_result_grade;
        if ( _eval_alarm_data.seconds != tempoEspera ) {
            eval_error("(C5) alarm() was not called with the correct parameters");
        } else {
            ++_result_grade;
        }

        char buffer[1024];
        snprintf( buffer, 1024, "(C5) Espera resposta em %d segundos", tempoEspera );
        if (eval_check_successlog( "(C5)", buffer )) {
            question_setgrade( questions,"5.1", GRADE_SUCCESS_ERROR );
        }
    }
    question_setgrade( questions,"5", GRADE_EVAL * _result_grade/3 );

    eval_close_logs( "(C5)" );
    return eval_complete("(C5)");
}

/**
 * Wrapper for receiveProductList_C6()
 */
struct {
    int action;
    int result;
} _receiveProductList_C6_data;


int receiveProductList_C6( ) {
    _receiveProductList_C6_data.result = -1234;
    _receiveProductList_C6_data.result = _student_receiveProductList_C6( );
    return _receiveProductList_C6_data.result;
}

/**
 * Evaluate C6
 */
int eval_c6( ) {
    eval_info("Evaluating C6 - %s...", question_text(questions,"6.1"));
    eval_reset();

    _eval_msgrcv_data.status = 0;
    _eval_msgrcv_data.action = 2; // inject

    _eval_msgrcv_data.msqid = -1;
    _eval_msgrcv_data.msgtyp = -1;
    _eval_msgrcv_data.msgflg = -1;

    // Bad login
    MsgContent snd;
    snd.msgType = getpid();
    snd.msgData.infoLogin.pidServidorDedicado = -1;
    strncpy(snd.msgData.infoLogin.nome, "User 2", 52);
    snd.msgData.infoLogin.saldo = 132;
    _eval_msgrcv_data.msgp = &snd;
    _eval_msgrcv_data.msgsz = sizeof(snd.msgData);

    msgId = 0xD1840;

    EVAL_CATCH( receiveProductList_C6() );
    int _result_grade = 0;
    int _result_grade1 = 0;

    // Evaluate C6
    // Evaluate C6: SUCCESS
    if ( 1 != _eval_msgrcv_data.status ) {
        eval_error("(C6) msgrcv() should have been called exactly once, was called %d times.", _eval_msgrcv_data.status );
    } else {
        ++_result_grade;
        if ( msgId != _eval_msgrcv_data.msqid ) {
            eval_error("(C6) Argumento msgid inválido no msgrcv()");
        } else {
            ++_result_grade;
        }

        if ( sizeof(snd.msgData) != _eval_msgrcv_data.msgsz ) {
            eval_error("(C6) Argumento msgsz inválido no msgrcv()");
        } else {
            ++_result_grade;
        }

        if ( getpid() != _eval_msgrcv_data.msgtyp ) {
            eval_error("(C6) Argumento msgtyp inválido no msgrcv()");
        } else {
            ++_result_grade;
        }

        char buffer[1024];
        snprintf( buffer, 1024, "(C6) %s %d %d", snd.msgData.infoLogin.nome, snd.msgData.infoLogin.saldo, -1);
        if (eval_check_successlog("(C6)", buffer )) {
            ++_result_grade1;
        }
    }

    // Evaluate C6.1
    if ( 1 != _eval_alarm_data.status ) {
        eval_error("(C6.1) alarm() should have been called exactly once, was called %d times", _eval_alarm_data.status );
    } else {
        ++_result_grade;
        if ( 0 != _eval_alarm_data.seconds ) {
            eval_error("(C6.1) alarm() called with wrong parameters");
        } else {
            ++_result_grade;
        }
    }

    // Evaluate C6.2
    // Evaluate C6.2: ERROR
    if ( _NO_EXIT == _eval_env.stat && RET_SUCCESS == _receiveProductList_C6_data.result ) {
        eval_error("(C6.2) receiveProductList_C6 returned success when pidServidorDedicado == -1");
    } else {
        ++_result_grade;
        if (eval_check_errorlog("(C6.2)", "(C6.2) " )) {
            ++_result_grade1;
        }
    }
    question_setgrade( questions,"6.1", GRADE_EVAL * _result_grade/7 );
    question_setgrade( questions,"6.1.1", GRADE_SUCCESS_ERROR * _result_grade1/2 );

    // Evaluate C6.2: SUCCESS
    // We will send the messages to the message queue first
    msgId = msgget( IPC_KEY, IPC_CREAT | 0600 );
    if ( msgId < 0 ) {
        eval_error( "(*critical*) Unable to create exclusive message queue for testing, key = 0x%X\n", IPC_KEY );
        eval_error( "(*critical*) Error was %s, aborting...\n", strerror(errno));
        exit(1);
    }
    snd.msgType = getpid();
    snd.msgData.infoLogin.pidServidorDedicado = 132456;
    const int nprods = 3;

    // Send list of products
    for( int i = 0; i < nprods; i ++ ) {
        snd.msgData.infoProduto.idProduto = 10 + i;
        snprintf( snd.msgData.infoProduto.nomeProduto, 39, "Produto #%d", 10 + i );
        snprintf( snd.msgData.infoProduto.categoria, 39, "Categoria #%d", 20 + i );
        snd.msgData.infoProduto.preco = 30 + i;

        if ( msgsnd( msgId, &snd, sizeof(snd.msgData), 0 ) == -1 ) {
            eval_error( "(*critical*) Unable to send test message to queue, key = 0x%X\n", IPC_KEY );
            eval_error( "(*critical*) Error was %s, aborting...\n", strerror(errno));
            exit(1);
        }
    }

    // Send end of list
    snd.msgData.infoProduto.idProduto = FIM_LISTA_PRODUTOS;
    if ( msgsnd( msgId, &snd, sizeof( snd.msgData ), 0 ) == -1 ) {
        eval_error( "(*critical*) Unable to send test message to queue, key = 0x%X\n", IPC_KEY );
        eval_error( "(*critical*) Error was %s, aborting...\n", strerror(errno));
        exit(1);
    }

    // Now eval C6 with login ok
    eval_info("Evaluating C6 - %s...", question_text(questions,"6.2"));
    eval_reset_except_stats();

    _eval_alarm_data.status = 0;
    _eval_alarm_data.action = 1; // ignore
    _eval_alarm_data.seconds = -1234;

    _eval_msgrcv_data.status = 0;
    _eval_msgrcv_data.action = 0; // call msgrcv()

    _eval_msgrcv_data.msqid = -1;
    _eval_msgrcv_data.msgtyp = -1;
    _eval_msgrcv_data.msgflg = -1;

    EVAL_CATCH_IO( receiveProductList_C6(), FILE_DEVNULL, FILE_STDOUT );
    _result_grade = 0;
    _result_grade1 = 0;

    // C6
    if (eval_check_successlog("(C6)", "(C6) " )) {
        ++_result_grade1;
    }

    // C6.1 was already validated

    // C6.2
    if ( _EXIT == _eval_env.stat || RET_ERROR == _receiveProductList_C6_data.result ) {
        eval_error("(C6.2) receiveProductList_C6 terminated or returned RET_ERROR when authentication valid");
    } else {
        ++_result_grade;
        if ( nprods + 1 != _eval_msgrcv_data.status ) {
            eval_error("(C6) msgrcv() should have been called exactly %d times, was called %d times.",
                nprods + 1, _eval_msgrcv_data.status );
        } else {
            ++_result_grade;
            if ( msgId != _eval_msgrcv_data.msqid ) {
                eval_error("(C6) Argumento msgid inválido no msgrcv()");
            } else {
                ++_result_grade;
            }

            if ( sizeof(snd.msgData) != _eval_msgrcv_data.msgsz ) {
                eval_error("(C6) Argumento msgsz inválido no msgrcv()");
            } else {
                ++_result_grade;
            }

            if ( getpid() != _eval_msgrcv_data.msgtyp ) {
                eval_error("(C6) Argumento msgtyp inválido no msgrcv()");
            } else {
                ++_result_grade;
            }

            // Check output file
            chmod( FILE_STDOUT, 0600 );
            FILE *f = fopen( FILE_STDOUT, "r" );
            if ( !f ) {
                eval_error( "(*critical*) Unable to open output file %s\n", FILE_STDOUT );
                eval_error( "(*critical*) Error was %s, aborting...\n", strerror(errno));
                exit(1);
            }

            // Test header
            if ( eval_fstrncmp( f, "Lista de Produtos Disponíveis", 1024 ) != 0 ) {
                eval_error("(C6) Expected \"%s\" on stdout", "Lista de Produtos Disponíveis");
            } else {
                ++_result_grade;
            }
            for( int i = 0; i < nprods; i ++ ) {
                char buffer[1024];

                so_fgets( buffer, sizeof(buffer), f ); // Reads from STDOUT "uma linha de texto com a indicação de idProduto, Nome, Categoria e Preço"
                so_debug( "%s", buffer);

                char str[1024];
                snprintf(str, 1024, "%d", 10 + i);
                if (NULL == strstr(buffer, str)) {
                    eval_error("(C6.4) Expected idProduto \"%s\" on stdout", str);
                } else {
                    ++_result_grade;
                }
                snprintf(str, 1024, "Produto #%d", 10 + i);
                if (NULL == strstr(buffer, str)) {
                    eval_error("(C6.4) Expected nomeProduto \"%s\" on stdout", str);
                } else {
                    ++_result_grade;
                }
                snprintf(str, 1024, "Categoria #%d", 20 + i);
                if (NULL == strstr(buffer, str)) {
                    eval_error("(C6.4) Expected categoria \"%s\" on stdout", str);
                } else {
                    ++_result_grade;
                }
                snprintf(str, 1024, "%d", 30 + i );
                if (NULL == strstr(buffer, str)) {
                    eval_error("(C6.4) Expected preco \"%s\" on stdout", str);
                } else {
                    ++_result_grade;
                }

                // snprintf( buffer, 1023, "%d Produto #%d Categoria #%d %d",
                //     10 + i, 10 + i, 20 + i, 30 + i );
                // if ( eval_fstrncmp( f, buffer, 1024 ) != 0 ) {
                //     eval_error("(C6) Expected \"%s\" on stdout", buffer);
                // }
            }
            if (eval_check_successlog("(C6.3)", "(C6.3) " )) {
                ++_result_grade1;
            }
            fclose(f);
        }
    }
    question_setgrade( questions,"6.2", GRADE_EVAL * _result_grade/18 );
    question_setgrade( questions,"6.2.1", GRADE_SUCCESS_ERROR * _result_grade1/2 );

    // Remove temporary message queue
    if ( msgctl( msgId, IPC_RMID, NULL ) == -1 ) {
        eval_error( "(*critical*) Unable to remove test message queue, key = 0x%X\n", IPC_KEY );
        eval_error( "(*critical*) Error was %s, aborting...\n", strerror(errno));
        exit(1);
    }

    // Remove output file
    unlink( FILE_STDOUT );

    eval_close_logs( "(C6)" );
    return eval_complete("(C6)");
}

/**
 * Wrapper for getIdProdutoUtilizador_C7()
 */
struct {
    int action;
    int result;
} _getIdProdutoUtilizador_C7_data;

int getIdProdutoUtilizador_C7() {
    _getIdProdutoUtilizador_C7_data.result = -1234;
    _getIdProdutoUtilizador_C7_data.result = _student_getIdProdutoUtilizador_C7();
    return _getIdProdutoUtilizador_C7_data.result;
}

/**
 * Evaluate C7
 */
int eval_c7( ) {
    eval_info("Evaluating C7 - %s...", question_text(questions,"7.1"));
    eval_reset();

    const int produto = 10;

    FILE* ft;
    ft = fopen(FILE_STDIN,"w");
    fprintf(ft,"%d\n", produto );

    fclose(ft);

    // Redirect standard I/O to files
    EVAL_CATCH_IO( getIdProdutoUtilizador_C7(), FILE_STDIN, FILE_DEVNULL );
    int _result_grade = 0;

    if ( _EXIT == _eval_env.stat ) {
        eval_error( "(C7) bad termination");
    } else {
        ++_result_grade;
    }

    // Remove I/O files
    unlink( FILE_STDIN );

    if ( produto != _getIdProdutoUtilizador_C7_data.result ) {
        eval_error( "(C7) invalid idProduto" );
    } else {
        ++_result_grade;
    }
    question_setgrade( questions,"7.1", GRADE_EVAL * _result_grade/2 );

    char buffer[1024];
    snprintf( buffer, 1024, "(C7) %d", produto);
    if (eval_check_successlog( "(C7)", buffer )) {
        question_setgrade( questions,"7.1.1", GRADE_SUCCESS_ERROR );
    }

    eval_close_logs( "(C7)" );

    // Check with bad input
    eval_info("Evaluating C7 - %s...", question_text(questions,"7.2"));
    eval_reset_except_stats();

    ft = fopen(FILE_STDIN,"w");
    fprintf(ft,"abc\n", produto );
    fclose(ft);

    // Redirect standard I/O to files
    EVAL_CATCH_IO( getIdProdutoUtilizador_C7(), FILE_STDIN, FILE_DEVNULL );
    _result_grade = 0;

    if ( _EXIT == _eval_env.stat ) {
        eval_error( "(C7.1) bad termination");
    } else {
        ++_result_grade;
    }

    // Remove I/O files
    unlink( FILE_STDIN );

    if ( PRODUCT_NOT_FOUND != _getIdProdutoUtilizador_C7_data.result ) {
        eval_error( "(C7) invalid idProduto, expected PRODUCT_NOT_FOUND" );
    } else {
        ++_result_grade;
    }
    question_setgrade( questions,"7.2", GRADE_EVAL * _result_grade/2 );

    if (eval_check_errorlog( "(C7)", "(C7) " )) {
        question_setgrade( questions,"7.2.1", GRADE_SUCCESS_ERROR );
    }

    eval_close_logs( "(C7)" );
    return eval_complete("(C7)");
}

/**
 * Wrapper for sendClientOrder_C8( int, int )
 */
struct {
    int action;
    int result;
} _sendClientOrder_C8_data;


int sendClientOrder_C8(int idProduto, int pidServidorDedicado) {
    _sendClientOrder_C8_data.result = -1234;
    _sendClientOrder_C8_data.result = _student_sendClientOrder_C8( idProduto, pidServidorDedicado );
    return _sendClientOrder_C8_data.result;
}

/**
 * Evaluate C8
 */
int eval_c8( ) {
    eval_info("Evaluating C8.1 - %s...", question_text(questions,"8.1"));
    eval_reset();

    _eval_msgsnd_data.status = 0;
    _eval_msgsnd_data.action = 1; // capture
    _eval_msgsnd_data.msgp = NULL;

    msgId = 0xD1840;

    int idProduto = 132;
    int pidServidorDedicado = 4657;

    EVAL_CATCH( sendClientOrder_C8( idProduto, pidServidorDedicado ) );
    int _result_grade = 0;

    if ( _EXIT == _eval_env.stat || RET_ERROR == _sendClientOrder_C8_data.result ) {
        eval_error("(C8) function exited or returned error when test should be successful");
    } else {
        ++_result_grade;
        if ( 1 != _eval_msgsnd_data.status ) {
            eval_error ("(C8) msgsnd() should have been called exactly once, was called %d times.", _eval_msgsnd_data.status );
        } else {
            ++_result_grade;

            if ( msgId != _eval_msgsnd_data.msqid ) {
                eval_error("(C8) Invalid msqid argument used on msgsnd()");
            } else {
                ++_result_grade;
            }

            MsgContent *m = _eval_msgsnd_data.msgp;

            if ( m == NULL ) {
                eval_error("(C8) Invalid msgp argument in msgsnd()" );
            } else {
                ++_result_grade;
                if ( _eval_msgsnd_data.msgsz != sizeof(m -> msgData)) {
                    eval_error("(C8) Invalid msgsz argument used on msgsnd()");
                } else {
                    ++_result_grade;
                    if ( pidServidorDedicado != m -> msgType ) {
                        eval_error("(C8) Invalid .msgType");
                    } else {
                        ++_result_grade;
                    }

                    if ( idProduto != m -> msgData.infoProduto.idProduto ) {
                        eval_error("(C8) Invalid msgData.infoProduto.idProduto");
                    } else {
                        ++_result_grade;
                    }
                }
            }
        }
        question_setgrade( questions,"8.1", GRADE_EVAL * _result_grade/7 );

        if (eval_check_successlog( "(C8)", "(C8) " )) {
            question_setgrade( questions,"8.1.1", GRADE_SUCCESS_ERROR );
        }

    }

    if ( _eval_msgsnd_data.msgp ) {
        free( _eval_msgsnd_data.msgp );
        _eval_msgsnd_data.msgp = NULL;
    }

    eval_close_logs( "(C8)" );

    eval_info("Evaluating C8.2 - %s...", question_text(questions,"8.2"));
    eval_reset_except_stats();

    _eval_msgsnd_data.status = 0;
    _eval_msgsnd_data.action = 2; // error
    _eval_msgsnd_data.msgp = NULL;

    EVAL_CATCH( sendClientOrder_C8( idProduto, pidServidorDedicado ) );
    _result_grade = 0;

    if ( _EXIT == _eval_env.stat ) {
        eval_error( "(C8) bad termination");
    } else {
        ++_result_grade;
        if ( RET_ERROR != _sendClientOrder_C8_data.result ) {
            eval_error("(C8) Invalid return value, should be RET_ERROR");
        } else {
            ++_result_grade;
        }
    }
    question_setgrade( questions,"8.2", GRADE_EVAL * _result_grade/2 );
    if (eval_check_errorlog( "(C8)", "(C8) " )) {
        question_setgrade( questions,"8.2.1", GRADE_SUCCESS_ERROR );
    }

    eval_close_logs( "(C8)" );
    return eval_complete("(C8)");
}


void receivePurchaseAck_C9( ) {
    _student_receivePurchaseAck_C9( );
}

/**
 * Evaluate C9
 */
int eval_c9( ) {
    eval_info("Evaluating C9 - %s...", question_text(questions,"9.1"));
    eval_reset();

    _eval_msgrcv_data.status = 0;
    _eval_msgrcv_data.action = 2; // inject

    _eval_msgrcv_data.msqid = -1;
    _eval_msgrcv_data.msgtyp = -1;
    _eval_msgrcv_data.msgflg = -1;

    // Purchase ok
    MsgContent snd;
    snd.msgType = getpid();
    snd.msgData.infoProduto.idProduto = PRODUTO_COMPRADO;
    _eval_msgrcv_data.msgp = &snd;
    _eval_msgrcv_data.msgsz = sizeof(snd.msgData);

    msgId = 0xD1840;

    EVAL_CATCH_IO( receivePurchaseAck_C9(), FILE_DEVNULL, FILE_STDOUT );
    int _result_grade = 0;

    if ( _EXIT != _eval_env.stat ) {
        eval_error( "(C9) should have finished with exit()");
    } else {
        ++_result_grade;
        if ( 0 != _eval_exit_data.status ) {
            eval_error("(C9) Invalid exit value, should be 0");
        } else {
            ++_result_grade;
        }

        if ( 1 != _eval_msgrcv_data.status ) {
            eval_error("(C9) msgrcv() should have been called exactly once, was called %d times", _eval_msgrcv_data.status );
        } else {
            ++_result_grade;
            if ( msgId != _eval_msgrcv_data.msqid ) {
                eval_error("(C9) Argumento msgid inválido no msgrcv()");
            } else {
                ++_result_grade;
            }

            if ( sizeof(snd.msgData) != _eval_msgrcv_data.msgsz ) {
                eval_error("(C9) Argumento msgsz inválido no msgrcv()");
            } else {
                ++_result_grade;
            }

            if ( getpid() != _eval_msgrcv_data.msgtyp ) {
                eval_error("(C9) Argumento msgtyp inválido no msgrcv()");
            } else {
                ++_result_grade;
            }

            // Check output file
            chmod( FILE_STDOUT, 0600 );
            FILE *f = fopen( FILE_STDOUT, "r" );
            if ( !f ) {
                eval_error( "(*critical*) Unable to open output file %s\n", FILE_STDOUT );
                eval_error( "(*critical*) Error was %s, aborting...\n", strerror(errno));
                exit(1);
            }

            // Test header
            if ( eval_fstrncmp( f, "Pode levantar o seu produto", 1024 ) != 0 ) {
                eval_error("(C9) Expected \"%s\" on stdout", "Pode levantar o seu produto");
            } else {
                ++_result_grade;
            }
            fclose(f);
        }
        question_setgrade( questions,"9.1", GRADE_EVAL * _result_grade/7 );

        char buffer[1024];
        snprintf( buffer, 1024, "(C9) %d", PRODUTO_COMPRADO );
        if (eval_check_successlog( "(C9)", buffer )) {
            question_setgrade( questions,"9.1.1", GRADE_SUCCESS_ERROR );
        }
    }

    unlink( FILE_STDOUT );

    eval_close_logs( "(C9)" );

    eval_info("Evaluating C9 - %s...", question_text(questions,"9.2"));
    eval_reset_except_stats();

    _eval_msgrcv_data.status = 0;
    _eval_msgrcv_data.action = 2; // inject

    _eval_msgrcv_data.msqid = -1;
    _eval_msgrcv_data.msgtyp = -1;
    _eval_msgrcv_data.msgflg = -1;

    // Purchase failed
    snd.msgType = getpid();
    snd.msgData.infoProduto.idProduto = PRODUTO_NAO_COMPRADO;
    _eval_msgrcv_data.msgp = &snd;
    _eval_msgrcv_data.msgsz = sizeof(snd.msgData);

    EVAL_CATCH_IO( receivePurchaseAck_C9(), FILE_DEVNULL, FILE_STDOUT );
    _result_grade = 0;

    if ( _EXIT != _eval_env.stat ) {
        eval_error( "(C9) should have finished with exit()");
    } else {
        ++_result_grade;
        if ( 0 != _eval_exit_data.status ) {
            eval_error("(C9) Invalid exit value, should be 0");
        } else {
            ++_result_grade;
        }

        // Check output file
        chmod( FILE_STDOUT, 0600 );
        FILE *f = fopen( FILE_STDOUT, "r" );
        if ( !f ) {
            eval_error( "(*critical*) Unable to open output file %s\n", FILE_STDOUT );
            eval_error( "(*critical*) Error was %s, aborting...\n", strerror(errno));
            exit(1);
        }

        // Test header
        if ( eval_fstrncmp( f, "Ocorreu um problema na sua compra. Tente novamente", 1024 ) != 0 ) {
            eval_error("(C6) Expected \"%s\" on stdout", "Ocorreu um problema na sua compra. Tente novamente");
        } else {
            ++_result_grade;
        }
        fclose(f);


    }
    question_setgrade( questions,"9.2", GRADE_EVAL * _result_grade/3 );
    if (eval_check_successlog( "(C9)", "(C9) " )) {
        question_setgrade( questions,"9.2.1", GRADE_SUCCESS_ERROR );
    }

    unlink( FILE_STDOUT );

    eval_close_logs( "(C9)" );
    return eval_complete("(C9)");

}

/**
 * Wrapper for trataSinalSIGUSR2_C10
 *
 */
void trataSinalSIGUSR2_C10(int sinalRecebido) {
    _student_trataSinalSIGUSR2_C10( sinalRecebido );
};

/**
 * Evaluate C10
 */
int eval_c10( ) {
    eval_info("Evaluating C10 - %s...", question_text(questions,"10"));
    eval_reset();

    EVAL_CATCH( trataSinalSIGUSR2_C10( SIGUSR2 ) );

    if ( _EXIT != _eval_env.stat ) {
        eval_error( "(C10) bad termination, should have called exit");
    } else {
        question_setgrade( questions,"10", GRADE_EVAL );
    }
    if (eval_check_successlog( "(C10)", "(C10) " )) {
        question_setgrade( questions,"10.1", GRADE_SUCCESS_ERROR );
    }

    eval_close_logs( "(C10)" );
    return eval_complete("(C10)");
}

/**
 * Wrapper for trataSinalSIGINT_C11
 *
 */
void trataSinalSIGINT_C11(int sinalRecebido) {
    _student_trataSinalSIGINT_C11( sinalRecebido );
};

/**
 * Evaluate C11
 */
int eval_c11( ) {
    eval_info("Evaluating C11 - %s...", question_text(questions,"11"));
    eval_reset();

    EVAL_CATCH( trataSinalSIGINT_C11( SIGINT ) );

    if ( _EXIT != _eval_env.stat ) {
        eval_error( "(C11) bad termination, should have called exit");
    } else {
        question_setgrade( questions,"11", GRADE_EVAL );
    }

    if (eval_check_successlog( "(C11)", "(C11) Shutdown Cliente" )) {
        question_setgrade( questions,"11.1", GRADE_SUCCESS_ERROR );
    }


    eval_close_logs( "(C11)" );
    return eval_complete("(C11)");
}

/**
 * Wrapper for trataSinalSIGALRM_C12
 *
 */
void trataSinalSIGALRM_C12(int sinalRecebido) {
    _student_trataSinalSIGALRM_C12( sinalRecebido );
};

/**
 * Evaluate C12
 */
int eval_c12( ) {
    eval_info("Evaluating C12 - %s...", question_text(questions,"12"));
    eval_reset();

    EVAL_CATCH( trataSinalSIGALRM_C12( SIGALRM ) );

    if ( _EXIT != _eval_env.stat ) {
        eval_error( "(C12) bad termination, should have called exit");
    } else {
        question_setgrade( questions,"12", GRADE_EVAL );
    }

    if (eval_check_errorlog( "(C12)", "(C12) Timeout Cliente" )) {
        question_setgrade( questions,"12.1", GRADE_SUCCESS_ERROR );
    }

    eval_close_logs( "(C12)" );
    return eval_complete("(C12)");
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