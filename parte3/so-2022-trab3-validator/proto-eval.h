/**
 * Protoypes for the functions implemented by students and their wrappers
 **/

#if defined( _CLIENTE )

/* Protótipos de funções */
int initMsg_C1();                           // C1:  Função a ser completada pelos alunos (definida abaixo)
int triggerSignals_C2();                    // C2:  Função a ser completada pelos alunos (definida abaixo)
MsgContent getDadosPedidoUtilizador_C3();   // C3:  Função a ser completada pelos alunos (definida abaixo)
int sendClientLogin_C4(MsgContent);         // C4:  Função a ser completada pelos alunos (definida abaixo)
void configuraTemporizador_C5(int);         // C5:  Função a ser completada pelos alunos (definida abaixo)
int receiveProductList_C6();                // C6:  Função a ser completada pelos alunos (definida abaixo)
int getIdProdutoUtilizador_C7();            // C7:  Função a ser completada pelos alunos (definida abaixo)
int sendClientOrder_C8(int, int);           // C8:  Função a ser completada pelos alunos (definida abaixo)
void receivePurchaseAck_C9();               // C9:  Função a ser completada pelos alunos (definida abaixo)
void trataSinalSIGUSR2_C10(int);            // C10: Função a ser completada pelos alunos (definida abaixo)
void trataSinalSIGINT_C11(int);             // C11: Função a ser completada pelos alunos (definida abaixo)
void trataSinalSIGALRM_C12(int);            // C12: Função a ser completada pelos alunos (definida abaixo)

int _student_initMsg_C1();
int _student_triggerSignals_C2();
MsgContent _student_getDadosPedidoUtilizador_C3();
int _student_sendClientLogin_C4(MsgContent);
void _student_configuraTemporizador_C5(int);
int _student_receiveProductList_C6();
int _student_getIdProdutoUtilizador_C7();
int _student_sendClientOrder_C8(int, int);
void _student_receivePurchaseAck_C9();
void _student_trataSinalSIGUSR2_C10(int);
void _student_trataSinalSIGINT_C11(int);
void _student_trataSinalSIGALRM_C12(int);


#elif defined( _SERVIDOR )

/* Protótipos de funções */
int initShm_S1();                       // S1:   Função a ser completada pelos alunos (definida abaixo)
int initMsg_S2();                       // S2:   Função a ser completada pelos alunos (definida abaixo)
int initSem_S3();                       // S3:   Função a ser completada pelos alunos (definida abaixo)
int triggerSignals_S4();                // S4:   Função a ser completada pelos alunos (definida abaixo)
MsgContent receiveClientLogin_S5();     // S5:   Função a ser completada pelos alunos (definida abaixo)
int createServidorDedicado_S6();        // S6:   Função a ser completada pelos alunos (definida abaixo)
void shutdownAndExit_S7();              // S7:   Função a ser completada pelos alunos (definida abaixo)
void handleSignalSIGINT_S8(int);        // S8:   Função a ser completada pelos alunos (definida abaixo)
void handleSignalSIGCHLD_S9(int);       // S9:   Função a ser completada pelos alunos (definida abaixo)
int triggerSignals_SD10();              // SD10: Função a ser completada pelos alunos (definida abaixo)
int validateRequest_SD11(Login);        // SD11: Função a ser completada pelos alunos (definida abaixo)
int searchUserDB_SD12(Login);           // SD12: Função a ser completada pelos alunos (definida abaixo)
int reserveUserDB_SD13(int, int);       // SD13: Função a ser completada pelos alunos (definida abaixo)
int sendProductList_SD14(int, int);     // SD14: Função a ser completada pelos alunos (definida abaixo)
MsgContent receiveClientOrder_SD15();   // SD15: Função a ser completada pelos alunos (definida abaixo)
int sendPurchaseAck_SD16(int, int);     // SD16: Função a ser completada pelos alunos (definida abaixo)
void shutdownAndExit_SD17();            // SD17: Função a ser completada pelos alunos (definida abaixo)
void handleSignalSIGUSR1_SD18(int);     // SD18: Função a ser completada pelos alunos (definida abaixo)

int _student_initShm_S1();
int _student_initMsg_S2();
int _student_initSem_S3();
int _student_triggerSignals_S4();
MsgContent _student_receiveClientLogin_S5();
int _student_createServidorDedicado_S6();
void _student_shutdownAndExit_S7();
void _student_handleSignalSIGINT_S8(int);
void _student_handleSignalSIGCHLD_S9(int);
int _student_triggerSignals_SD10();
int _student_validateRequest_SD11(Login);
int _student_searchUserDB_SD12(Login);
int _student_reserveUserDB_SD13(int, int);
int _student_sendProductList_SD14(int, int);
MsgContent _student_receiveClientOrder_SD15();
int _student_sendPurchaseAck_SD16(int, int);
void _student_shutdownAndExit_SD17();
void _student_handleSignalSIGUSR1_SD18(int);

#else

#error Either _CLIENTE or _SERVIDOR must be defined

#endif