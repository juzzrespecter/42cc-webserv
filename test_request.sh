#!/bin/zsh

REQ="GET / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 56\r\nAccept: ni idea de que es esto\r\nTransfer-Encoding: chunked\r\n\r\n"
echo -e $REQ > test_request_msg
