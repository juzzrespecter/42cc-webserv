# webserv
<br/>
<br/>
<p align="center">
  <i>"I predict the Internet, (...) will soon go <br/>
    spectacularly supernova and in 1996 catastrophically collapse."</i>
</p>
<br/>
<br/>
Part of 42 common core cursus: develop a simple HTTP-compilant web server, fully compatible with a
web navigator. 
<br/>
<br/>
Partially based in nginx, handles GET, POST, PUT, HEAD and DELETE requests.<br/>
Configuration file options:
<ul>
<li>listen: sets up port and host of virtual server</li>
<li>error_page: sets up default 404 error page option</li> 
<li>client_max_body_size: sets payload limit size for a request</li>
<li>location: sets routes for specific configuration inside a virtual server</li>
<li>accept_method: list of accepted methods</li>
<li>return: sets up page redirection</li>
<li>root: sets up root directory file search</li>
<li>alias: sets alias for root directory in a route</li>
<li>autoindex: sets ON | OFF file listing on a directory</li>
<li>index: sets index page searching for a directory</li>
<li>accept_cgi: sets up list of allowed scripts and path to binary script execution</li>
<li>accept_upload: sets a default upload directory used with a PUT request</li>
  </ul>
<br/>

Made by [@danrodri](https://profile.intra.42.fr/users/danrodri/), [@fgomez-s](https://profile.intra.42.fr/users/fgomez-s)
## usage
```
make && ./webserver [path_to_conf_file]
```

Project passed with [125/100] mark ✅😎

   
