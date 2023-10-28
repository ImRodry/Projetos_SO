s/#include "so_utils.h"/#include "so_utils-eval.h"/
s/int main(/int _student_main(/
s/^int shmId = RET_ERROR; /extern int shmId; /
s/^int msgId = RET_ERROR; /extern int msgId; /
s/^int semId = RET_ERROR; /extern int semId; /
s/^MsgContent msg; /extern MsgContent msg; /
s/^DadosServidor *db; /extern DadosServidor *db; /
s/^int indexClient; /extern int indexClient; /
s/int initShm_S1(/ int _student_initShm_S1(/
s/int initMsg_S2(/ int _student_initMsg_S2(/
s/int initSem_S3(/ int _student_initSem_S3(/
s/int triggerSignals_S4(/ int _student_triggerSignals_S4(/
s/MsgContent receiveClientLogin_S5(/ MsgContent _student_receiveClientLogin_S5(/
s/int createServidorDedicado_S6(/ int _student_createServidorDedicado_S6(/
s/void shutdownAndExit_S7(/ void _student_shutdownAndExit_S7(/
s/void handleSignalSIGINT_S8(/ void _student_handleSignalSIGINT_S8(/
s/void handleSignalSIGCHLD_S9(/ void _student_handleSignalSIGCHLD_S9(/
s/int triggerSignals_SD10(/ int _student_triggerSignals_SD10(/
s/int validateRequest_SD11(/ int _student_validateRequest_SD11(/
s/int searchUserDB_SD12(/ int _student_searchUserDB_SD12(/
s/int reserveUserDB_SD13(/ int _student_reserveUserDB_SD13(/
s/int sendProductList_SD14(/ int _student_sendProductList_SD14(/
s/MsgContent receiveClientOrder_SD15(/ MsgContent _student_receiveClientOrder_SD15(/
s/int sendPurchaseAck_SD16(/ int _student_sendPurchaseAck_SD16(/
s/void shutdownAndExit_SD17(/ void _student_shutdownAndExit_SD17(/
s/void handleSignalSIGUSR1_SD18(/ void _student_handleSignalSIGUSR1_SD18(/
