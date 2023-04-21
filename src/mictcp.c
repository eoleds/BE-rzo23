#include <mictcp.h>
#include <api/mictcp_core.h>

/*
 * Permet de créer un socket entre l’application et MIC-TCP
 * Retourne le descripteur du socket ou bien -1 en cas d'erreur
 */
int mic_tcp_socket(start_mode sm)
{
   printf("[MIC-TCP] Appel de la fonction: ");  printf(__FUNCTION__); printf("\n");
   int result = -1;
   mic_tcp_sock sock;
   sock.fd = 0;
   result = initialize_components(sm); /* Appel obligatoire */
   set_loss_rate(0);
   return result;
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
 * Met le socket en état d'acceptation de connexion
 * Retourne 0 si succès, -1 si erreur
 */

//Variable booléenne pour savoir si la connexion est bien établie
int connexion_etablie = 0;

int mic_tcp_accept(int socket, mic_tcp_sock_addr* addr)
{
    printf("[MIC-TCP] Appel de la fonction: ");  printf(__FUNCTION__); printf("\n");
    while(!connexion_etablie){   
        //attente de l'etablissement de connexion
    }
    return 0;
}

/*
 * Permet de réclamer l’établissement d’une connexion
 * Retourne 0 si la connexion est établie, et -1 en cas d’échec
 */

// on définit le prochain paquet à emettre (PE)
int pe = 0;
int mic_tcp_connect(int socket, mic_tcp_sock_addr addr)
{
    printf("[MIC-TCP] Appel de la fonction: ");  printf(__FUNCTION__); printf("\n");
    
    // On déclare + initialise le PDU SYN
    mic_tcp_pdu pdu = {0};
    pdu.header.syn = 1;
    pdu.header.ack = 0;

    // On associe puis update le numéro du prochain paquet à émettre
    pdu.header.seq_num = pe;
    pe = (pe+1)%2;

    pdu.payload.data = "";
    pdu.payload.size = 0;

    mic_tcp_pdu rec = {0}; //pdu qui va recevoir


    int result;
    int time_;

    //Variable booléenne pour savoir si on a reçu le SYN_ACK qu'on attend
    int syn_ack_recu = 0;

    while (syn_ack_recu == 0){
        result = IP_send(pdu, addr);
        time_ = IP_recv(&rec, &addr, 10);
        //Si le timer n'a pas expiré et que le PDU recu est bien un SYN_ACK
        if (time_ != -1 && rec.header.syn == 1 && rec.header.ack == 1){
            //On déclare qu'on l'a bien reçu pour sortir de la boucle
            syn_ack_recu = 1;
        }
    }

    //On initialise l'ACK à envoyer
    mic_tcp_pdu ACK = {0};
    ACK.header.syn = 0;
    ACK.header.ack = 1;
    ACK.header.seq_num = pe;
    ACK.payload.data = "";
    ACK.payload.size = 0;

    // On envoie l'ACK
    IP_send(ACK, addr);
    printf("Connexion établie\n");
    return 0;
}

/*
 * Permet de réclamer l’envoi d’une donnée applicative
 * Retourne la taille des données envoyées, et -1 en cas d'erreur
 */

//On compte les paquets envoyés et reçus
float paquets_env;
float paquets_rec;

//Taux de perte qu'on s'autorise
float taux_perte = 0.5;

int mic_tcp_send (int mic_sock, char* mesg, int mesg_size)
{
    printf("[MIC-TCP] Appel de la fonction: "); printf(__FUNCTION__); printf("\n");
        mic_tcp_pdu pdu;
        //remplir le payload
        pdu.payload.size = mesg_size;
        pdu.payload.data = mesg;
        pdu.header.seq_num = pe; // DT.nseq <-- pe

        //PDU de l'ACK à recevoir
        mic_tcp_pdu pk = {0};
        mic_tcp_sock_addr addr = {0}, addr_pk = {0};

        addr.ip_addr = "localhost";
        addr.ip_addr_size = strlen(addr.ip_addr) + 1;
        addr.port = htons(API_CS_Port);

        // activation timer
        int env ;//= IP_send(pdu,addr_pk);
        int rec ;//= IP_recv(&pk, &addr_pk, 10);

        //On s'apprête à envoyer un paquet
        paquets_env++;

        pe = (pe+1)%2;
        int envoi_a_faire = 1;
        while (envoi_a_faire){
            env = IP_send(pdu,addr);
            //On considère d'abord l'envoi réussi
            envoi_a_faire = 0;
            //On attend de recevoir un ack
            rec = IP_recv(&pk, &addr_pk, 10);

            //Si on n'a rien rçu :
            if(rec == -1){
                if (paquets_rec / paquets_env < (1.0 - taux_perte)){
                    envoi_a_faire = 1; //Perte non tolérable, on a besoin de renvoyer
                }
                // Sinon, on ne fait rien <-> on 'perd' le paquet
            }
            else{
                if (pk.header.ack_num == pe){ //Le paquet reçu est celui qu'on a envoyé
                    paquets_rec++;
                }
                else{
                    //Ce n'est pas le bon ; est-ce que la perte du paquet envoyé serait tolérable ?
                    if(paquets_rec / paquets_env < (1.0 - taux_perte)){ //Non
                        envoi_a_faire = 1; //On renvoie
                    }
                    //Sinon, on s'autorise à perdre le paquet
                }
            }
        }

    return env;
}

/*
 * Permet à l’application réceptrice de réclamer la récupération d’une donnée
 * stockée dans les buffers de réception du socket
 * Retourne le nombre d’octets lu ou bien -1 en cas d’erreur
 * NB : cette fonction fait appel à la fonction app_buffer_get()
 */

// On définit le prochain paquet attendu (PA)
int pa = 0;

int mic_tcp_recv (int socket, char* mesg, int max_mesg_size)
{
    printf("[MIC-TCP] Appel de la fonction: "); printf(__FUNCTION__); printf("\n");
    int result = -1;
    //On crée un PDU qu'on remplit des informations qu'on a reçues (data + size)
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
    mic_tcp_pdu ACK_PDU = {0};
    ACK_PDU.header.ack = 1;
    //On traite le cas de reception d'un SYN (-> envoi d'un SYN_ACK)
    if (pdu.header.syn == 1 && pdu.header.ack == 0){
        // On créé un PDU SYN_ACK qu'on initialise
        mic_tcp_pdu SYN_ACK_PDU = {0};
        SYN_ACK_PDU.header.ack = 1;
        SYN_ACK_PDU.header.syn = 1;
        SYN_ACK_PDU.payload.data = "";
        SYN_ACK_PDU.payload.size = 0;

        // On update le num du prochain paquet a emettre
        pa = (pa + 1)%2;
        SYN_ACK_PDU.header.seq_num = pa;

        // On envoie le SYN_ACK
        IP_send(SYN_ACK_PDU, addr);

    // Le cas de reception ACK (-> etablissement de connexion)
    }else if (pdu.header.syn == 0 && pdu.header.ack == 1){
        connexion_etablie = 1;

    // Le cas de reception d'une data (-> update de PA + envoi ACK)
    }else if (pdu.header.seq_num == pa){
        app_buffer_put(pdu.payload);
        pa = (pa + 1)%2;
        ACK_PDU.header.ack_num = pa;
        IP_send(ACK_PDU, addr);
    }else{
        ACK_PDU.header.ack_num = pa;
        IP_send(ACK_PDU, addr);
    }
}
