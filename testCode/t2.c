#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define AGENT_NAME "HJPSampleCode/1.0"
#define HOST_NAME  "www.mars.dti.ne.jp"
#define URL_PATH   "/~torao/program/socket/program.html"
#define SERVER_PORT 80

void error(const char* msg);

int main(void){
    WSADATA info;               /* Winsock ���p�\����   */
    struct hostent *host;       /* �ڑ�����z�X�g���     */
    struct sockaddr_in addr;    /* �z�X�g�� IP �A�h���X   */
    u_long inaddr;              /* �C���^�[�l�b�g�A�h���X */
    SOCKET client;              /* �N���C�A���g�\�P�b�g   */
    int res;                    /* �֐��̕Ԃ��l           */
    char buffer[1024];          /* �ǂݏo���p�o�b�t�@     */

    /* ���M���N�G�X�g�ƃw�b�_ */
    char request[] = "GET " URL_PATH " HTTP/1.0\r\n"
                     "User-Agent: " AGENT_NAME "\r\n"
                     "Host: " HOST_NAME "\r\n"
                     "Connection: close\r\n"
                     "\r\n";

    /* Winsock �̏����� (�o�[�W���� 1.1 ��v��) */
    res = WSAStartup(0x0101, &info);
    if(res == SOCKET_ERROR) error("Winsock initialize.");

    /* �z�X�g���̎擾 */
    host = gethostbyname(HOST_NAME);
    if(host == NULL)    error("Invalid host name.");
    if(host->h_addr_list == NULL || host->h_addr_list[0] == NULL){
        error("Invalid host address.");
    }

    /* �z�X�g�̃A�h���X���擾���� (���łɃl�b�g���[�N�o�C�g����) */
    inaddr = *(u_long*)(host->h_addr_list[0]);

    /* �A�h���X�̐ݒ� */
    addr.sin_family = AF_INET;          /* �C���^�[�l�b�g�A�h���X */
    addr.sin_port = htons(SERVER_PORT); /* �|�[�g (�ϊ����đ��)  */
    addr.sin_addr = *(struct in_addr*)&inaddr;  /* �A�h���X�ݒ� */
    memset(addr.sin_zero, 0x00, 8);     /* �p�f�B���O   */

    /* �X�g���[���\�P�b�g�̍\�z */
    client = socket(AF_INET, SOCK_STREAM, 0);
    if(client == INVALID_SOCKET)    error("Socket creation.");

    /* �z�X�g�Ɛڑ� */
    res = connect(client, (struct sockaddr*)&addr, sizeof(addr));
    if(res == SOCKET_ERROR) error("Connection.");

    /* �f�[�^���o�� */
    res = send(client, request, strlen(request), 0);
    if(res == SOCKET_ERROR) error("Data Sending.");

    /* �f�[�^�̓��� */
    for(/* */;;){
        res = recv(client, buffer, sizeof(buffer), 0);
        if(res == SOCKET_ERROR) error("Data Reading.");
        if(res == 0)    break;
        fwrite(buffer, res, 1, stdout);
    }

    /* �\�P�b�g�̔j�� */
    res = closesocket(client);
    if(res == SOCKET_ERROR) error("In closing socket.");

    /* Winsock �I������ */
    res = WSACleanup();
    if(res == SOCKET_ERROR) error("Cleanup.");

    return EXIT_SUCCESS;
}

void error(const char* msg){
    printf("Winsock Error[%d]: %s\n", h_errno, msg);
    exit(EXIT_FAILURE);
    return;
}
