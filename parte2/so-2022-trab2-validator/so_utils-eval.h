/******************************************************************************
 ** ISCTE-IUL: Trabalho prático 3 de Sistemas Operativos
 ** Avaliação automática
 ******************************************************************************/
#ifndef __UTILS_EVAL_H__
#define __UTILS_EVAL_H__

#undef DEBUG_MODE
#define DEBUG_MODE 0

#include "so_utils.h"
#include "eval.h"

/**
 * Disable warning: ‘%s’ directive output may be truncated
 * which may be triggered by the sucesso() and erro() macros
 **/
#ifdef __GNUC__
#if __GNUC__ > 7
#pragma GCC diagnostic ignored "-Wformat-truncation"
#pragma GCC diagnostic push
#endif
#endif


#undef so_success
#undef so_error

#ifdef _EVAL_DEBUG

#define so_success(passo,fmt, ...) do {\
    printf( BACK_GREEN "@SUCCESS {" passo "}" GREEN " [" fmt "]" NO_COLOUR "\n", ## __VA_ARGS__);\
    snprintf( newline( &_success_log ), LOGLINE,"(" passo ") " fmt, ## __VA_ARGS__); \
} while(0)

#define so_error(passo,fmt, ...) do {\
    printf( BACK_BOLD_RED "@ERROR {" passo "}" RED " [" fmt "]" NO_COLOUR "\n", ## __VA_ARGS__); if (errno) perror("");\
    snprintf( newline( &_error_log ), LOGLINE,"(" passo ") " fmt, ## __VA_ARGS__); \
} while(0)

#else  // _EVAL_DEBUG

#define so_success(passo,fmt, ...) do {\
    snprintf( newline( &_success_log ), LOGLINE,"(" passo ") " fmt, ## __VA_ARGS__); \
} while(0)

#define so_error(passo,fmt, ...) do {\
    snprintf( newline( &_error_log ), LOGLINE,"(" passo ") " fmt, ## __VA_ARGS__); \
} while(0)

#endif // _EVAL_DEBUG


/**
 * The replacement versions of exit_on_error() and exit_on_null() work the same way
 * but silence the ouput
 */

#undef so_exit_on_error
#define so_exit_on_error(status, errorMsg) do { \
    if (status < 0) { \
        exit(-1); \
    } \
} while (0)

#undef so_exit_on_null
#define so_exit_on_null(status, errorMsg) do { \
    if (NULL == status) { \
        exit(-1); \
    } \
} while (0)

#undef so_rand
#define so_rand() 1340000000  //(RAND_MAX/2)

#endif