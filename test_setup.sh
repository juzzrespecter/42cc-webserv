#!/bin/bash

TESTER=tester
CGI_PATH=YoupiBanane/cgi-bin

if [ ! -d "./YoupiBanane" ]; then 
    mkdir -pv YoupiBanane/{nop,Yeah,cgi-bin}
    touch YoupiBanane/youpi.bad_extension \
	  YoupiBanane/youpi.bla \
	  YoupiBanane/nop/youpi.bad_extension \
	  YoupiBanane/nop/other.pouic \
	  YoupiBanane/Yeah/not_happy.bad_extension
    if [ $(uname) == "Linux" ]; then
	wget -O $PWD/$CGI_PATH/ubuntu_cgi_tester https://projects.intra.42.fr/uploads/document/document/6716/ubuntu_cgi_tester
	chmod +x $CGI_PATH/ubuntu_cgi_tester
    else
	wget -O $PWD/$CGI_PATH/cgi_tester https://projects.intra.42.fr/uploads/document/document/6717/cgi_tester
	chmod +x $CGI_PATH/cgi_tester
    fi
fi

if [ ! -f $TESTER ]; then
    if [ $(uname) == "Linux" ]; then
	wget -O $PWD/$TESTER https://projects.intra.42.fr/uploads/document/document/6718/ubuntu_tester
    else
	wget -O $PWD/$TESTER https://projects.intra.42.fr/uploads/document/document/6715/tester
    fi
    chmod +x $TESTER
fi

if [ ! -f webserver.conf ]; then
    cat <<EOF > tester.conf
server {
       listen 8080 ;

       accept_method GET ;
       cgi_pass bla ./YoupiBanane/cgi-bin/cgi_tester ;
       location /put_test/ {
                accept_method PUT ;
                accept_upload /upload_files/ ;
       }
       location /post_body/ {
                accept_method POST ;
                client_max_body_size 100 ;
       }
       location /directory/ {
                error_page youpi.bad_extension ;
                alias ./YoupiBanane/ ;
       }
}
EOF
fi

make
./webserver tester.conf &
./tester http://localhost:8080
