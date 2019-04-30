#!/usr/bin/perl

use Socket;
use IO::Handle;

$| = 1;

$server_port = 23;

socket(ME, PF_INET, SOCK_STREAM, getprotobyname('tcp'));
setsockopt(ME, SOL_SOCKET, SO_REUSEADDR, 1);
setsockopt(ME, SOL_SOCKET, 1, 1);
$my_addr = sockaddr_in(2002, INADDR_ANY);
bind(ME, $my_addr) || die "Couldn't bind to port : $!\n";
listen(ME, SOMAXCONN) || die "Couldn't listen : $!\n";
accept(CLIENT, ME);

socket(SERVER, PF_INET, SOCK_STREAM, getprotobyname('tcp'));
setsockopt(SERVER, SOL_SOCKET, 1, 1);
#$iaddr = inet_aton("bbs.fament.com");
$iaddr = inet_aton("peanut.millersville.edu");
$paddr = sockaddr_in($server_port, $iaddr);
connect(SERVER, $paddr);

print "Got Connection!\n";

open S, ">server.txt";

S->autoflush(1);
SERVER->autoflush(1);
CLIENT->autoflush(1);

if (fork())
{
  while (1)
  {
    if (read(SERVER, $buf, 1) == 0)
    {
      last;
    }
    if ($buf eq "\0")
    {
      print S "s - 0\n";
    }
    else
    {
      print S "s - " . ord($buf) . "\n";
    }
    print CLIENT $buf;
  }
}
else
{
  while (1)
  {
    if (read(CLIENT, $buf, 1) == 0)
    {
      last;
    }
    if ($buf eq "\0")
    {
      print S "c - 0\n";
    }
    else
    {
      print S "c - " . ord($buf) . "\n";
    }
    print SERVER $buf;
  }
}

close CLIENT;
close SERVER;
close S;
