#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

#define ICMP_ECHO_REQUEST_SIZE 8

void display(char *buffer, int length) {
    for (int i = 0; i < length; i+=2) {
        // Affiche deux octets à la fois, en hexadécimal
        printf("%02x%02x ", (unsigned char)buffer[i], (unsigned char)buffer[i + 1]);
        
        // Ajoute un retour à la ligne tous les 16 octets
        if ((i + 1) % 16 == 15) {
            printf("\n");
        }
    }

    // Ajoute un retour à la ligne si la dernière ligne n'était pas complète
    if (length % 16 != 0) {
        printf("\n");
    }
}

// Remplit l'en-tête IP avec les valeurs spécifiées
// Note that the ip_off and ip_len fields are in host byte order. (man ip on macOS)
void fill_ip_header(struct ip *ip_header, size_t payload_size, u_short id, u_char ttl, const char *src_ip, const char *dest_ip) {
    memset(ip_header, 0, sizeof(struct ip));

    ip_header->ip_hl = sizeof(struct ip) / 4;
    ip_header->ip_v = 4;
    ip_header->ip_tos = 0;
    ip_header->ip_len = sizeof(struct ip) + ICMP_ECHO_REQUEST_SIZE + payload_size;
    ip_header->ip_id = htons(id);
    ip_header->ip_off = 0;
    ip_header->ip_ttl = ttl;
    ip_header->ip_p = IPPROTO_ICMP;
    ip_header->ip_sum = htons(0);
    ip_header->ip_src.s_addr = inet_addr(src_ip);
    ip_header->ip_dst.s_addr = inet_addr(dest_ip);
}

// Remplit l'en-tête ICMP avec les valeurs spécifiées
void fill_icmp_header(struct icmp *icmp_header, u_short id, u_short sequence) {
    memset(icmp_header, 0, sizeof(struct icmp));

    icmp_header->icmp_type = ICMP_ECHO;
    icmp_header->icmp_code = 0;
    icmp_header->icmp_cksum = htons(0);
    icmp_header->icmp_id = htons(id);
    icmp_header->icmp_seq = htons(sequence);
}

short checksum(char *addr, int count) {
    unsigned long sum = 0;
    uint16_t *ptr = (uint16_t *)addr;

    // Additionne les mots de 16 bits
    while (count > 1) {
        sum += *ptr++;
        count -= 2;
    }

    // Traite l'octet restant pour les paquets de taille impaire
    if (count > 0) {
        sum += (*(unsigned char *)ptr) << 8;
    }

    // Réduit le dépassement
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    // Retourne l'inverse du résultat
    return (short)~sum;
}

void update_icmp_checksum(struct icmp *icmp_header, size_t payload_size) {
    // Taille totale de l'ICMP (en-tête + payload)
    size_t icmp_total_size = ICMP_ECHO_REQUEST_SIZE + payload_size;

    // Calcul de la somme de contrôle
    icmp_header->icmp_cksum = checksum((char *)icmp_header, icmp_total_size);
}

void update_ip_checksum(struct ip *ip_header) {
    ip_header->ip_sum = checksum((char *)ip_header, ip_header->ip_hl * 4);
}

int send_packet(char *packet, int packet_size, const char *dest_ip) {
    int sock;
    struct sockaddr_in dest_addr;

    // Créer un socket brut
    sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (sock < 0) {
        perror("Socket error");
        return -1;
    }

    // Configurer l'adresse de destination
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(0);  // Pas de port pour les paquets bruts
    dest_addr.sin_addr.s_addr = inet_addr(dest_ip);  // Adresse IP de destination


    // Désactiver l'ajout automatique d'entête IP par le noyau (important sur macOS)
    int one = 1;
    if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0) {
        perror("setsockopt error");
        close(sock);
        return -1;
    }
    

    // Envoyer le paquet
    if (sendto(sock, packet, packet_size, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
        perror("Send failed");
        close(sock);
        return -1;
    }

    printf("Paquet envoyé à %s\n", dest_ip);

    // Fermer le socket
    close(sock);
    return 0;
}

int main() {
    // Définition des paramètres
    const char *src_ip = "192.168.0.1";
    const char *dest_ip = "192.168.0.2";
    size_t payload_size = 0;
    u_short id_ip = 0x1234;
    u_char ttl = 100;
    u_short id_icmp = 0x5678;
    u_short icmp_sequence = 1;

    // Création de l'en-tête IP
    struct ip ip_header;
    fill_ip_header(&ip_header, payload_size, id_ip, ttl, src_ip, dest_ip);

    // Création de l'en-tête ICMP
    struct icmp icmp_header;
    fill_icmp_header(&icmp_header, id_icmp, icmp_sequence);

    // Création du paquet
    size_t packet_size = sizeof(struct ip) + ICMP_ECHO_REQUEST_SIZE + payload_size;
    char packet[packet_size];
    memcpy(packet, &ip_header, sizeof(struct ip));
    memcpy(packet + sizeof(struct ip), &icmp_header, ICMP_ECHO_REQUEST_SIZE); // On copie uniquement la partie de l'en-tête ICMP utile pour un ECHO REQUEST

    // Affichage du contenu du buffer avant mise à jour des sommes de contrôle
    // display(packet, sizeof(packet));

    // Mise à jour de la somme de contrôle ICMP
    update_icmp_checksum((struct icmp *)(packet + sizeof(struct ip)), payload_size);

    // Mise à jour de la somme de contrôle IP
    update_ip_checksum((struct ip *)packet);

    // Affichage du contenu du buffer après mise à jour des sommes de contrôle
    // display(packet, sizeof(packet));

    // Envoi du paquet
    send_packet(packet, packet_size, dest_ip);
    
    return 0;
}