/**
 * Protoypes for the functions implemented by students and their wrappers
 **/

#if defined( _CLIENTE )

/* Protótipos de funções */
int existsFifoServidor_C1(char *);                   // C1: Função a ser implementada pelos alunos
int triggerSignals_C2();                             // C2: Função a ser implementada pelos alunos
Login getDadosPedidoUtilizador_C3();                 // C3: Função a ser implementada pelos alunos
int buildNomeFifo(char *, int, char *, int, char *); // Função a ser implementada pelos alunos
int createFifo_C4(char *);                           // C4: Função a ser implementada pelos alunos
int writeRequest_C5(Login, char *);                  // C5: Função a ser implementada pelos alunos
void configuraTemporizador_C6(int);                  // C6: Função a ser implementada pelos alunos
Login readAckLogin_C7(char *);                       // C7: Função a ser implementada pelos alunos
int validateAutenticacaoServidor_C8(Login);          // C8: Função a ser implementada pelos alunos
int sleepRandomTime_C9();                            // C9: Função a ser implementada pelos alunos
int writeFimSessao_C10(char *, int, Login);          // C10: Função a ser implementada pelos alunos
void deleteFifoAndExit_C11();                        // C11: Função a ser implementada pelos alunos
void trataSinalSIGUSR2_C12(int);                     // C12: Função a ser implementada pelos alunos
void trataSinalSIGINT_C13(int);                      // C13: Função a ser implementada pelos alunos
void trataSinalSIGALRM_C14(int);                     // C14: Função a ser implementada pelos alunos

int _student_existsFifoServidor_C1(char *);
int _student_triggerSignals_C2();
Login _student_getDadosPedidoUtilizador_C3();
int _student_buildNomeFifo(char *, int, char *, int, char *);
int _student_createFifo_C4(char *);
int _student_writeRequest_C5(Login, char *);
void _student_configuraTemporizador_C6(int);
Login _student_readAckLogin_C7(char *);
int _student_validateAutenticacaoServidor_C8(Login);
int _student_sleepRandomTime_C9();
int _student_writeFimSessao_C10(char *, int, Login);
void _student_deleteFifoAndExit_C11();                 
void _student_trataSinalSIGUSR2_C12(int);
void _student_trataSinalSIGINT_C13(int);
void _student_trataSinalSIGALRM_C14(int);

#elif defined( _SERVIDOR )

/* Protótipos de funções */
int existsDB_S1(char *);                // S1:   Função a ser implementada pelos alunos
int createFifo_S2(char *);              // S2:   Função a ser implementada pelos alunos
int triggerSignals_S3();                // S3:   Função a ser implementada pelos alunos
Login readRequest_S4(char *);           // S4:   Função a ser implementada pelos alunos
int createServidorDedicado_S5();        // S5:   Função a ser implementada pelos alunos
void deleteFifoAndExit_S6();            // S6:   Função a ser implementada pelos alunos
void trataSinalSIGINT_S7(int);          // S7:   Função a ser implementada pelos alunos
void trataSinalSIGCHLD_S8(int);         // S8:   Função a ser implementada pelos alunos
int triggerSignals_SD9();               // SD9:  Função a ser implementada pelos alunos
int validaPedido_SD10(Login);           // SD10:  Função a ser implementada pelos alunos
int buildNomeFifo(char *, int, char *, int, char *); // Função a ser implementada pelos alunos
int procuraUtilizadorBD_SD11(Login, char *, Login *); // SD11: Função a ser implementada pelos alunos
int reservaUtilizadorBD_SD12(Login *, char *, int, Login);  // SD12: Função a ser implementada pelos alunos
int createFifo_SD13();                  // SD13: Função a ser implementada pelos alunos
int sendAckLogin_SD14(Login, char *);   // SD14: Função a ser implementada pelos alunos
int readFimSessao_SD15(char *);         // SD15: Função a ser implementada pelos alunos
int terminaSrvDedicado_SD16(Login *, char *, int);  // SD16: Função a ser implementada pelos alunos
void deleteFifoAndExit_SD17();          // SD17: Função a ser implementada pelos alunos
void trataSinalSIGUSR1_SD18(int);       // SD18: Função a ser implementada pelos alunos                

int _student_existsDB_S1(char *);
int _student_createFifo_S2(char *);
int _student_triggerSignals_S3();
Login _student_readRequest_S4(char *);
int _student_createServidorDedicado_S5();
void _student_deleteFifoAndExit_S6();
void _student_trataSinalSIGINT_S7(int);
void _student_trataSinalSIGCHLD_S8(int);
int _student_triggerSignals_SD9();
int _student_validaPedido_SD10(Login);
int _student_buildNomeFifo(char *, int, char *, int, char *);
int _student_procuraUtilizadorBD_SD11(Login, char *, Login *);
int _student_reservaUtilizadorBD_SD12(Login *, char *, int, Login);
int _student_createFifo_SD13();
int _student_sendAckLogin_SD14(Login, char *);
int _student_readFimSessao_SD15(char *);
int _student_terminaSrvDedicado_SD16(Login *, char *, int);
void _student_deleteFifoAndExit_SD17();
void _student_trataSinalSIGUSR1_SD18(int);

#else

#error Either _CLIENTE or _SERVIDOR must be defined

#endif