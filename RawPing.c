#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

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
void fill_ip_header(struct ip *ip_header, size_t payload_size, u_short id, u_char ttl, const char *src_ip, const char *dest_ip) {
    memset(ip_header, 0, sizeof(struct ip));

    ip_header->ip_hl = sizeof(struct ip) / 4;
    ip_header->ip_v = 4;
    ip_header->ip_tos = 0;
    ip_header->ip_len = htons(sizeof(struct ip) + payload_size);
    ip_header->ip_id = htons(id);
    ip_header->ip_off = htons(0);
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

int main() {
    return 0;
}