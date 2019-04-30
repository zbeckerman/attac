#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include <math.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/utsname.h>

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define DIFF_TIME(x,y)  (x.tv_sec + x.tv_usec*1.E-06 - y.tv_sec - y.tv_usec*1.E-06) 

#define BUFLEN 		10*1024		/* each read can handle up to 10KB	*/
#define	MAX_PROTOCOLLEN	50
#define MAX_REQUESTLEN	MAX_PROTOCOLLEN + MAXHOSTNAMELEN + MAXPATHLEN + 100
#define MAX_DATELEN     26      /* length of date string        */
#define IP4_ADDRESSLEN  16      /* length of IPv4 address string*/

void 	parse_url	(char*, char*, char*, int*, char*);

/******************************************************
*
*	main()					
*
******************************************************/
int main (int argc, char *argv[])
{
	char	line[]="--------------------------------------------------";
	char	*p;
 	int 	n, port, sockfd, ns, n_reads;
 	long 	bytes, first_bytes, rest_bytes, total_bytes;
	double  norm_10K;
	double	dns_sec, connect_sec, first_sec, rest_sec, get_sec, total_sec, sec, bw;
 	char 	*url, *user_agent, request[MAX_REQUESTLEN], protocol[MAX_PROTOCOLLEN], 
		path[MAXPATHLEN], date[MAX_DATELEN], server_IP[IP4_ADDRESSLEN],
		server_name[MAXHOSTNAMELEN], client_name[MAXHOSTNAMELEN], buf[BUFLEN],
		response_hdr[BUFLEN];
		
 	struct 	hostent 	*host_info;
	struct  in_addr  	in;
 	struct 	sockaddr_in 	addr;
 	struct 	timeval 	*timeout, time0, time1, time2;
	time_t			timep;
 	fd_set 			Rset;

        /* Check command line */
        if (argc < 2) {
	  printf("Usage: %s url\n", argv[0]);
	  exit(1);
	}  


	/* Parse URL */
 	url = argv[1];
 	parse_url(url, protocol, server_name, &port, path);
	user_agent = "Mozilla/4.0 http_get";
	/* Prepare HTTP request */
	sprintf(request, "GET %s HTTP/1.1\r\nHost: %s\r\n", path, server_name);
	  /* User-agent */
	strcat(request, "User-Agent: ");
	strcat(request, user_agent);
	strcat(request, "\r\n");
 	strcat(request, "Accept: */*\r\n");
 	strcat(request, "Connection: close\r\n"); 
	strcat(request, "\r\n");
	printf("%s\nREQUEST\n\n%s%s\n", line, request, line);

        /* DNS lookup for server IP address */
 	gettimeofday(&time0, NULL);
	host_info = gethostbyname(server_name);
	if (!host_info) {
	  printf("Error during address lookup\n"); 
	  exit(0);
	}  
 	gettimeofday(&time1, NULL);
	memcpy(&in, host_info->h_addr_list[0], sizeof(in));
	strcpy(server_name, host_info->h_name);
	strcpy(server_IP,   inet_ntoa(in));
	dns_sec = DIFF_TIME(time1, time0);

        /* Write output */
	gethostname(client_name, MAXHOSTNAMELEN);
	timep = time0.tv_sec;
	strcpy(date, ctime(&timep));
	date[strlen(date)-1]='\0';
 	printf("\nDate:   \t\t%s\n", date);
	printf("Client: \t\t%s\n", client_name);
	printf("Server canonical name:\t%s\n", server_name);
	printf("Server IP Address:    \t%s\n",	server_IP);
	printf("Server TCP Port:    \t%d\n",	port);	
	printf("Object:     \t\t%s\n\n",	path);
 	printf ("DNS lookup:   \t\t%.3f sec\n",	dns_sec);	
	
        /* Prepare socket information */
	bzero(&addr, sizeof(addr));
 	addr.sin_family = AF_INET;
 	addr.sin_port = htons(port);
        memcpy(&addr.sin_addr, host_info->h_addr_list[0], sizeof(struct in_addr));
 
        /* Open socket and connect */
 	sockfd = socket(AF_INET, SOCK_STREAM, 0);
 	gettimeofday(&time0, NULL);
 	connect(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
 	gettimeofday(&time1, NULL);
	connect_sec = DIFF_TIME(time1, time0);
        printf("Connect:      \t\t%.3f sec\n", connect_sec);

        /* Send request and wait for response */
 	FD_ZERO(&Rset);
 	FD_SET(sockfd, &Rset);
 	gettimeofday(&time0, NULL);
 	write(sockfd, request, strlen(request));
	ns = select(sockfd+1, &Rset, NULL, NULL, timeout);
 	gettimeofday(&time1, NULL);
 
        /* Read reply */
	if (!ns) 
	{
          printf ("Timeout waiting for object (%g sec)\n\n",sec);
 	  close(sockfd);
	  exit(-1);
	}
 	total_bytes = n_reads = 0;
 	for (;;)
	{
		if (!n_reads)
		{		
			/* First packet - contains response header */ 			
			/* Latency = time to receive first packet */
			bytes = read(sockfd, response_hdr, BUFLEN);
			first_bytes = bytes;
			if (!bytes) break;
		} 
		else
		{
			/* Remaining packets */ 			
			bytes = read(sockfd, buf, BUFLEN);
			if (!bytes) break;
		}
 		total_bytes += bytes;
		n_reads++;
	}
 	gettimeofday(&time2, NULL);
	first_sec = DIFF_TIME(time1, time0);
	rest_sec = DIFF_TIME(time2, time1);
	get_sec = first_sec + rest_sec;
	total_sec = dns_sec + connect_sec + get_sec;
	rest_bytes = total_bytes - first_bytes;

	/* Close socket */
 	close(sockfd);


	/* Print timing results */ 
	if (get_sec) bw = 0.001*total_bytes/get_sec;
	else bw = 0;
	if (rest_bytes) norm_10K = connect_sec + first_sec + (rest_sec * 10000/rest_bytes);
	else norm_10K = 0;

 	printf("GET latency:  \t\t%.3f sec  for  %ld bytes", first_sec, first_bytes);
	if (first_sec) printf(", \tBW= %.3f KB/s", .001*first_bytes/first_sec);
	printf("\n");

	rest_bytes = total_bytes - first_bytes;
 	printf("GET remainder:\t\t%.3f sec  for  %ld bytes", rest_sec, rest_bytes);
	if (rest_sec) printf(", \tBW= %.3f KB/s", .001*rest_bytes/rest_sec);
	printf("\n");

 	printf("GET total:    \t\t%.3f sec  for  %ld bytes", get_sec, total_bytes);
	if (get_sec) printf(", \tBW= %.3f KB/s", .001*total_bytes/get_sec);
	printf("\n");

 	printf("\n\nTotal time:   \t\t%.3f sec\n\n", total_sec);

	/* Print response header */
	for (n=0, p=response_hdr+4;  n<first_bytes;  p++)
		if (*(p-3)=='\r' && *(p-2)=='\n' && *(p-1)=='\r' && *p=='\n') break;
	*(p-1) = '\0';
	printf ("\n%s\nRESPONSE HEADER\n\n%s\n%s\n", line, response_hdr, line);
	exit(0);
}


/* parse_url() */
void parse_url(char *url, char *protocol, char *host, int *port, char *path)
{
	int	n;
	char 	*t, *slash, *colon;
	char	delim[] = "://";

 	/* Protocol */
	t = url;
 	if (!(colon = strstr(t, delim)))  strcpy (protocol,"http");
	else 
	{
		n = (int)(colon-t);
 		strncpy (protocol, t, n);
		protocol[n+1] ='\0';
 		t = colon + strlen(delim);
 	}
	
 	/* Host */
 	if (!(slash = strchr(t, '/')))  
	{
		strcpy(path, "/");
 		strcpy(host, t);
	}
	else 
	{
 		strcpy (path, slash);
		*slash = '\0';
		strcpy (host, t);
		*slash = '/';
	}
	
 	/* Port */
 	if (!(colon= strchr (host,':'))) *port = 80;
 	else *port = atoi(colon + 1);
}
