#include <mictcp.h>
#include <api/mictcp_core.h>

/*
 * Permet de créer un socket entre l’application et MIC-TCP
 * Retourne le descripteur du socket ou bien -1 en cas d'erreur
 */
int mic_tcp_socket(start_mode sm)
{
   printf("[MIC-TCP] Appel de la fonction: ");  printf(__FUNCTION__); printf("\n");
   mic_tcp_sock sock;
   sock.fd = 0;
   initialize_components(sm); /* Appel obligatoire */
   set_loss_rate(0);
   return 0;
}

/*
 * Permet d’attribuer une adresse à un socket.
 * Retourne 0 si succès, et -1 en cas d’échec
 */
int mic_tcp_bind(int socket, mic_tcp_sock_addr addr)
{
   printf("[MIC-TCP] Appel de la fonction: ");  printf(__FUNCTION__); printf("\n");
   return 0;
}

/*
 * Met le socket en état d'acceptation de connexions
 * Retourne 0 si succès, -1 si erreur
 */
int mic_tcp_accept(int socket, mic_tcp_sock_addr* addr)
{
    printf("[MIC-TCP] Appel de la fonction: ");  printf(__FUNCTION__); printf("\n");
    return 0;
}

/*
 * Permet de réclamer l’établissement d’une connexion
 * Retourne 0 si la connexion est établie, et -1 en cas d’échec
 */
int mic_tcp_connect(int socket, mic_tcp_sock_addr addr)
{
    printf("[MIC-TCP] Appel de la fonction: ");  printf(__FUNCTION__); printf("\n");
    return 0;
}

/*
 * Permet de réclamer l’envoi d’une donnée applicative
 * Retourne la taille des données envoyées, et -1 en cas d'erreur
 */
int pe = 0;

int mic_tcp_send (int mic_sock, char* mesg, int mesg_size)
{
    printf("[MIC-TCP] Appel de la fonction: "); printf(__FUNCTION__); printf("\n");
        mic_tcp_pdu pdu;

        //remplir le payload
        pdu.payload.size = mesg_size;
        pdu.payload.data = mesg;
        pdu.header.seq_num = pe; // DT.nseq <-- pe

        mic_tcp_pdu pk;
        mic_tcp_sock_addr addr_pk;

        addr_pk.ip_addr = "localhost";
        addr_pk.ip_addr_size = strlen(addr_pk.ip_addr) + 1;
        addr_pk.port = htons(API_CS_Port);

        // activation timer
        int result = IP_send(pdu,addr_pk);
        int time = IP_recv(&pk, &addr_pk, 10);
        pe = (pe+1)%2;
        while (pe != pk.header.ack_num || time == -1){
            result = IP_send(pdu,addr_pk);
            time = IP_recv(&pk, &addr_pk, 1000);
        }

    return result;
}

/*
 * Permet à l’application réceptrice de réclamer la récupération d’une donnée
 * stockée dans les buffers de réception du socket
 * Retourne le nombre d’octets lu ou bien -1 en cas d’erreur
 * NB : cette fonction fait appel à la fonction app_buffer_get()
 */

int pa = 0;

int mic_tcp_recv (int socket, char* mesg, int max_mesg_size)
{
    printf("[MIC-TCP] Appel de la fonction: "); printf(__FUNCTION__); printf("\n");
    int result = -1;
    mic_tcp_pdu pdu;
    pdu.payload.size = max_mesg_size;
    pdu.payload.data = mesg;
    result = app_buffer_get(pdu.payload);
    return result;
}

/*
 * Permet de réclamer la destruction d’un socket.
 * Engendre la fermeture de la connexion suivant le modèle de TCP.
 * Retourne 0 si tout se passe bien et -1 en cas d'erreur
 */
int mic_tcp_close (int socket)
{
    printf("[MIC-TCP] Appel de la fonction : "); printf(__FUNCTION__); printf("\n");
    return 0;
}

/*
 * Traitement d’un PDU MIC-TCP reçu (mise à jour des numéros de séquence
 * et d'acquittement, etc.) puis insère les données utiles du PDU dans
 * le buffer de réception du socket. Cette fonction utilise la fonction
 * app_buffer_put().
 */



void process_received_PDU(mic_tcp_pdu pdu, mic_tcp_sock_addr addr)
{
    printf("[MIC-TCP] Appel de la fonction: "); printf(__FUNCTION__); printf("\n");
    mic_tcp_pdu ACK_PDU;
    ACK_PDU.header.ack = '1';
    if (pdu.header.seq_num == pa){
        app_buffer_put(pdu.payload);
        pa = (pa + 1)%2;
        ACK_PDU.header.ack_num = pa;
        IP_send(ACK_PDU, addr);
    }
    else{
        ACK_PDU.header.ack_num = pa;
        IP_send(ACK_PDU, addr);
    }
}
