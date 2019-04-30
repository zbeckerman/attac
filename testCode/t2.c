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
    WSADATA info;               /* Winsock 情報用構造体   */
    struct hostent *host;       /* 接続するホスト情報     */
    struct sockaddr_in addr;    /* ホストの IP アドレス   */
    u_long inaddr;              /* インターネットアドレス */
    SOCKET client;              /* クライアントソケット   */
    int res;                    /* 関数の返し値           */
    char buffer[1024];          /* 読み出し用バッファ     */

    /* 送信リクエストとヘッダ */
    char request[] = "GET " URL_PATH " HTTP/1.0\r\n"
                     "User-Agent: " AGENT_NAME "\r\n"
                     "Host: " HOST_NAME "\r\n"
                     "Connection: close\r\n"
                     "\r\n";

    /* Winsock の初期化 (バージョン 1.1 を要求) */
    res = WSAStartup(0x0101, &info);
    if(res == SOCKET_ERROR) error("Winsock initialize.");

    /* ホスト情報の取得 */
    host = gethostbyname(HOST_NAME);
    if(host == NULL)    error("Invalid host name.");
    if(host->h_addr_list == NULL || host->h_addr_list[0] == NULL){
        error("Invalid host address.");
    }

    /* ホストのアドレスを取得する (すでにネットワークバイト順序) */
    inaddr = *(u_long*)(host->h_addr_list[0]);

    /* アドレスの設定 */
    addr.sin_family = AF_INET;          /* インターネットアドレス */
    addr.sin_port = htons(SERVER_PORT); /* ポート (変換して代入)  */
    addr.sin_addr = *(struct in_addr*)&inaddr;  /* アドレス設定 */
    memset(addr.sin_zero, 0x00, 8);     /* パディング   */

    /* ストリームソケットの構築 */
    client = socket(AF_INET, SOCK_STREAM, 0);
    if(client == INVALID_SOCKET)    error("Socket creation.");

    /* ホストと接続 */
    res = connect(client, (struct sockaddr*)&addr, sizeof(addr));
    if(res == SOCKET_ERROR) error("Connection.");

    /* データを出力 */
    res = send(client, request, strlen(request), 0);
    if(res == SOCKET_ERROR) error("Data Sending.");

    /* データの入力 */
    for(/* */;;){
        res = recv(client, buffer, sizeof(buffer), 0);
        if(res == SOCKET_ERROR) error("Data Reading.");
        if(res == 0)    break;
        fwrite(buffer, res, 1, stdout);
    }

    /* ソケットの破棄 */
    res = closesocket(client);
    if(res == SOCKET_ERROR) error("In closing socket.");

    /* Winsock 終了処理 */
    res = WSACleanup();
    if(res == SOCKET_ERROR) error("Cleanup.");

    return EXIT_SUCCESS;
}

void error(const char* msg){
    printf("Winsock Error[%d]: %s\n", h_errno, msg);
    exit(EXIT_FAILURE);
    return;
}
