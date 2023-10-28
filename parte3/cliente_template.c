/******************************************************************************
 ** ISCTE-IUL: Trabalho prático 3 de Sistemas Operativos
 **
 ** Aluno: Nº:       Nome:
 ** Nome do Módulo: cliente.c v2.1 (https://moodle22.iscte-iul.pt/mod/forum/discuss.php?d=6187)
 ** Descrição/Explicação do Módulo:
 **
 **
 ******************************************************************************/
#include "common.h"
// #define SO_HIDE_DEBUG   // Uncomment this line to hide all @DEBUG statements
#include "so_utils.h"

/* Variáveis globais */
int msgId = RET_ERROR;                      // Variável que tem o ID da Message Queue
MsgContent msg;                             // Variável que serve para todas as mensagens trocadas entre Cliente e Servidor

/* Protótipos de funções. Os alunos não devem alterar estes protótipos. */
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

/**
 * Main: Processamento do processo Cliente
 *       Não é suposto que os alunos alterem nada na função main()
 * @return int Exit value (RET_SUCCESS | RET_ERROR)
 */
int main() {
    so_exit_on_error((msgId = initMsg_C1()), "C1");
    so_exit_on_error(triggerSignals_C2(), "C2");
    msg = getDadosPedidoUtilizador_C3();
    so_exit_on_error(msg.msgData.infoLogin.nif, "C3");
    so_exit_on_error(sendClientLogin_C4(msg), "C4");
    configuraTemporizador_C5(MAX_ESPERA);
    so_exit_on_error(receiveProductList_C6(), "C6");
    int idProduto = getIdProdutoUtilizador_C7();
    so_exit_on_error(sendClientOrder_C8(idProduto, msg.msgData.infoLogin.pidServidorDedicado), "C8");
    receivePurchaseAck_C9();
    so_exit_on_error(-1, "ERRO: O cliente nunca devia chegar a este passo");
    return RET_ERROR;
}

/******************************************************************************
 * FUNÇÕES IPC MESSAGE QUEUES (cópia de /home/so/reference/msg-operations.c )
 *****************************************************************************/

/**
 * @brief Internal Private Function, not to be used by the students.
 */
int __msgGet( int msgFlags ) {
    int msgId = msgget( IPC_KEY, msgFlags );
    if ( msgId < 0 ) {
        so_debug( "Could not create/open the Message Queue with key=0x%x", IPC_KEY );
    } else {
        so_debug( "Using the Message Queue with key=0x%x and id=%d", IPC_KEY, msgId );
    }
    return msgId;
}

/**
 * @brief Creates an IPC Message Queue NON-exclusively, associated with the IPC_KEY, of an array of size multiple of sizeof(Aluno)
 *
 * @return int msgId. In case of error, returns -1
 */
int msgCreate() {
    return __msgGet( IPC_CREAT | 0600 );
}

/**
 * @brief Opens an already created IPC Message Queue associated with the IPC_KEY
 *
 * @return int msgId. In case of error, returns -1
 */
int msgGet() {
    return __msgGet( 0 );
}

/**
 * @brief Removes the IPC Message Queue associated with the IPC_KEY
 *
 * @param msgId IPC MsgID
 * @return int 0 if success or if the Message Queue already did not exist, or -1 if the Message Queue exists and could not be removed
 */
int msgRemove( int msgId ) {
    // Ignore any errors here, as this is only to check if the Message Queue exists and remove it
    if ( msgId > 0 ) {
        // If the Message Queue with IPC_KEY already exists, remove it
        int result = msgctl( msgId, IPC_RMID, 0 );
        if ( result < 0) {
            so_debug( "Could not remove the Message Queue with key=0x%x and id=%d", IPC_KEY, msgId );
        } else {
            so_debug( "Removed the Message Queue with key=0x%x and id=%d", IPC_KEY, msgId );
        }
        return result;
    }
    return 0;
}

/**
 * @brief Sends the passed message to the IPC Message Queue associated with the IPC_KEY
 *
 * @param msgId IPC MsgID
 * @param msg Message to be sent
 * @param msgType Message Type (or Address)
 * @return int success
 */
int msgSend(int msgId, MsgContent msg, int msgType) {
    msg.msgType = msgType;
    int result = msgsnd(msgId, &msg, sizeof(msg.msgData), 0);
    if (result < 0) {
        so_debug( "Could not send the Message to the Message Queue with key=0x%x and id=%d", IPC_KEY, msgId );
    } else {
        so_debug( "Sent the Message to the Message Queue with key=0x%x and id=%d", IPC_KEY, msgId );
    }
    return result;
}

/**
 * @brief Reads a message of the passed tipoMensagem from the IPC Message Queue associated with the IPC_KEY
 *
 * @param msgId IPC MsgID
 * @param msg Pointer to the Message to be filled
 * @param msgType Message Type (or Address)
 * @return int Number of bytes read, or -1 in case of error
 */
int msgReceive(int msgId, MsgContent* msg, int msgType) {
    int result = msgrcv(msgId, msg, sizeof(msg->msgData), msgType, 0);
    if ( result < 0) {
        so_debug( "Could not Receive the Message of Type %d from the Message Queue with key=0x%x and id=%d", msgType, IPC_KEY, msgId );
    } else {
        so_debug( "Received a Message of Type %d from the Message Queue with key=0x%x and id=%d, with %d bytes", msgType, IPC_KEY, msgId, result );
    }
    return result;
}

/******************************************************************************
 * MÓDULO CLIENTE
 *****************************************************************************/

/**
 *  O módulo Cliente é responsável pela interação com o utilizador. Após o login do utilizador,
 *  este poderá realizar atividades durante o tempo da sessão. Assim, definem-se as seguintes
 *  tarefas a desenvolver:
 */

/**
 * @brief C1    Abre a Message Queue (MSG) do projeto, que tem a KEY IPC_KEY. Deve assumir que a
 *              fila de mensagens já foi criada pelo Servidor. Em caso de erro, dá so_error e
 *              termina o Cliente. Caso contrário dá so_success <msgId>.
 *
 * @return int RET_ERROR em caso de erro, ou a msgId em caso de sucesso
 */
int initMsg_C1() {
    int result = RET_ERROR;    // Por omissão retorna erro
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno

    so_debug("> [@return:%d]", result);
    return result;
}

/**
 * @brief C2    Arma e trata os sinais SIGUSR2 (ver C10), SIGINT (ver C11), e SIGALRM (ver C12).
 *              Em caso de qualquer erro a armar os sinais, dá so_error e termina o Cliente.
 *              Caso contrário, dá so_success.
 *
 * @return int Sucesso (RET_SUCCESS | RET_ERROR)
 */
int triggerSignals_C2() {
    int result = RET_ERROR;    // Por omissão retorna erro
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno

    so_debug("> [@return:%d]", result);
    return result;
}

/**
 * @brief C3    Pede ao utilizador que preencha os dados referentes à sua autenticação (NIF e
 *              Senha), criando um elemento do tipo Login com essas informações, e preenchendo
 *              também o campo pidCliente com o PID do seu próprio processo Cliente. Os restantes
 *              campos da estrutura Login não precisam ser preenchidos.
 *              Em caso de qualquer erro, dá so_error e termina o Cliente.
 *              Caso contrário dá so_success <nif> <senha> <pidCliente>;
 *              Convenciona-se que se houver problemas, esta função coloca nif = USER_NOT_FOUND;
 *
 * @return Login Elemento com os dados preenchidos. Se nif==USER_NOT_FOUND,
 *               significa que o elemento é inválido
 */
MsgContent getDadosPedidoUtilizador_C3() {
    MsgContent msg;
    msg.msgData.infoLogin.nif = USER_NOT_FOUND;   // Por omissão retorna erro
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno

    so_debug("> [@return nif:%d, senha:%s, pidCliente:%d]", msg.msgData.infoLogin.nif,
                                msg.msgData.infoLogin.senha, msg.msgData.infoLogin.pidCliente);
    return msg;
}

/**
 * @brief C4    Envia uma mensagem do tipo MSGTYPE_LOGIN para a MSG com a informação recolhida do
 *              utilizador. Em caso de erro, dá so_error. Caso contrário, dá so_success.
 *
 * @return int Sucesso (RET_SUCCESS | RET_ERROR)
 */
int sendClientLogin_C4(MsgContent msg) {
    int result = RET_ERROR;    // Por omissão retorna erro
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno

    so_debug("> [@return:%d]", result);
    return result;
}

/**
 * @brief C5    Configura um alarme com o valor de tempoEspera segundos (ver C12), e dá
 *              so_success "Espera resposta em <tempoEspera> segundos" (não usa sleep!);
 *
 * @param tempoEspera o tempo em segundos que queremos pedir para marcar o timer do SO (i.e., MAX_ESPERA)
 */
void configuraTemporizador_C5(int tempoEspera) {
    so_debug("< [@param tempoEspera:%d]", tempoEspera);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno

    so_debug(">");
}

/**
 * @brief C6    Lê da Message Queue uma mensagem cujo tipo é o PID deste processo Cliente.
 *              Se houver erro, dá so_error e termina o Cliente.
 *              Caso contrário, dá so_success <nome> <saldo> <pidServidorDedicado>.
 *        C6.1  “Desliga” o alarme configurado em C5.
 *        C6.2  Valida se o resultado da autenticação do Servidor Dedicado foi sucesso ou não
 *              (convenciona-se que se a autenticação não tiver sucesso, o campo
 *              pidServidorDedicado==-1). No caso de insucesso, dá so_error, e termina o Cliente.
 *              Senão, escreve no STDOUT a frase “Lista de Produtos Disponíveis:”.
 *        C6.3  Extrai da mensagem recebida o Produto especificado.
 *              Se o campo idProduto tiver o valor FIM_LISTA_PRODUTOS, convencionou-se que
 *              significa que não há mais produtos a listar, então dá so_success
 *              e retorna sucesso (vai para C7).
 *        C6.4  Mostra no STDOUT uma linha de texto com a indicação de idProduto, Nome,
 *              Categoria e Preço.
 *        C6.5  Lê da Message Queue uma nova mensagem cujo tipo é o PID deste processo Cliente.
 *              Se houver erro, dá so_error e termina o Cliente.
 *              Caso contrário, volta ao passo C6.3.
 *
 * @return int Sucesso (RET_SUCCESS | RET_ERROR)
 */
int receiveProductList_C6() {
    int result = RET_ERROR;    // Por omissão retorna erro
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno

    so_debug("> [@return:%d]", result);
    return result;
}

/**
 * @brief C7    Pede ao utilizador que indique qual o idProduto (número) que deseja adquirir.
 *              Não necessita validar se o valor inserido faz parte da lista apresentada.
 *              Em caso de qualquer erro, dá so_error e retorna PRODUCT_NOT_FOUND.
 *              Caso contrário dá so_success <idProduto>, e retorna esse idProduto.
 *
 * @return int em caso de erro retorna RET_ERROR. Caso contrário, retorna um idProduto
 */
int getIdProdutoUtilizador_C7() {
    int result = RET_ERROR;    // Por omissão retorna erro
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno

    so_debug("> [@return:%d]", result);
    return result;
}

/**
 * @brief C8    Envia uma mensagem cujo tipo é o PID do Servidor Dedicado para a MSG com a
 *              informação do idProduto recolhida do utilizador. Em caso de erro, dá so_error.
 *              Caso contrário, dá so_success.
 *
 * @param idProduto O produto pretendido pelo Utilizador
 * @return int Sucesso (RET_SUCCESS | RET_ERROR)
 */
int sendClientOrder_C8(int idProduto, int pidServidorDedicado) {
    int result = RET_ERROR;    // Por omissão retorna erro
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno

    so_debug("> [@return:%d]", result);
    return result;
}

/**
 * @brief C9    Lê da MSG uma mensagem cujo tipo é o PID deste processo Cliente, com a resposta
 *              final do Servidor Dedicado. Em caso de erro, dá so_error. Caso contrário, dá
 *              so_success <idProduto>. Se o campo idProduto for PRODUTO_COMPRADO, escreve no
 *              STDOUT “Pode levantar o seu produto”. Caso contrário, escreve no STDOUT
 *              “Ocorreu um problema na sua compra. Tente novamente”.
 *              Em ambos casos, termina o Cliente.
 */
void receivePurchaseAck_C9() {
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno

    so_debug(">");
    exit(RET_SUCCESS);
}

/**
 * @brief C10   O sinal armado SIGUSR2 serve para o Servidor Dedicado indicar que o servidor
 *              está em modo shutdown. Se o Cliente receber esse sinal, dá so_success e
 *              termina o Cliente.
 *
 * @param sinalRecebido nº do Sinal Recebido (preenchido pelo SO)
 */
void trataSinalSIGUSR2_C10(int sinalRecebido) {
    so_debug("< [@param sinalRecebido:%d]", sinalRecebido);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno

    so_debug(">");
}

/**
 * @brief C11   O sinal armado SIGINT serve para que o utilizador possa cancelar o pedido do
 *              lado do Cliente, usando o atalho <CTRL+C>. Se receber esse sinal (do utilizador
 *              via Shell), o Cliente dá so_success "Shutdown Cliente", e termina o Cliente.
 *
 * @param sinalRecebido nº do Sinal Recebido (preenchido pelo SO)
 */
void trataSinalSIGINT_C11(int sinalRecebido) {
    so_debug("< [@param sinalRecebido:%d]", sinalRecebido);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno

    so_debug(">");
}

/**
 * @brief C12   O sinal armado SIGALRM serve para que, se o Cliente em C6 esperou mais do que
 *              MAX_ESPERA segundos sem resposta, o Cliente dá so_error "Timeout Cliente", e
 *              termina o Cliente.
 *
 * @param sinalRecebido nº do Sinal Recebido (preenchido pelo SO)
 */
void trataSinalSIGALRM_C12(int sinalRecebido) {
    so_debug("< [@param sinalRecebido:%d]", sinalRecebido);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno

    so_debug(">");
}