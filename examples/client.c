#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 65432

int main(int argc, char const *argv[]) {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from C client";
    char buffer[1024] = {0};
    
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\\n Ошибка создания сокета \\n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\\n Неверный адрес / Адрес не поддерживается \\n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\\n Ошибка подключения \\n");
        return -1;
    }
    
    send(sock, hello, strlen(hello), 0);
    printf("Сообщение 'Hello' отправлено\\n");
    
    read(sock, buffer, 1024);
    printf("Ответ от сервера: %s\\n", buffer);
    
    return 0;
}