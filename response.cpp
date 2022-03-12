#include "response.hpp"

/*A server which receives an entity-body with a transfer-coding it does
   not understand SHOULD return 501 (Unimplemented), and close the
   connection. A server MUST NOT send transfer-codings to an HTTP/1.0
   client.*/

/* FileParser tmp definition */
FileParser::FileParser(void) : rawFile(), status(false) { }

FileParser::FileParser(const FileParser& other) : rawFile(other.rawFile), status(other.status) { }

FileParser::FileParser(const std::string& filePath, bool _status) { 
	status = _status;

	std::ifstream file(filePath.c_str());
	if (!file.is_open()) {
		throw StatusLine(500, REASON_500, "FileParser: error trying to get body file");
	}
	rawFile = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
	file.close();
}

FileParser::~FileParser() { }

std::string FileParser::getRequestFile(void) const {
	return rawFile;
}

int FileParser::getRequestFileSize(void) const {
	return rawFile.size();
}

/* ------------------------ COPLIEN FORM ----------------------- */

Response::Response() : _autoIndex(false) {}

//Response::Response(Request* req, const StatusLine& staLine/*, const std::vector<Server*> infoVirServs*/) :
//	/*_infoVirServs(infoVirServs),*/ _req(req), _staLine(staLine), _autoIndex(false) {}

Response::Response(const Response& c) : 
	/*_infoVirServs(c._infoVirServs),*/ _req(c._req), _staLine(c._staLine), _buffer(c._buffer),
    _autoIndex(c._autoIndex) {}

Response::~Response() {}

Response& Response::operator=(Response a)
{
	swap(a, *this);
	return *this;
}

/* --------------------------- SETTERS ------------------------- */

void Response::setRequest(Request* req)
{
	_req = req;
}

void Response::setStatusLine(const StatusLine& staLine)
{
	_staLine = staLine;
}
/*
void Response::setInfoVirtualServs(const std::vector<Server*> infoVirServs)
{
	_infoVirServs = infoVirServs;
}*/


/* --------------------------- GETTERS ------------------------- */

const StatusLine& Response::getStatusLine() const
{
	return _staLine;
}

int Response::getCode() const
{
	return _staLine.getCode();
}

const std::string& Response::getBuffer() const
{
	return _buffer;
}


/* --------------------------- METHODS ------------------------- */

void Response::clear()
{
	_req->clear();
	_staLine.clear();
	_buffer.clear();
	_autoIndex = false;
}

void Response::fillBuffer(Request* req, const Location& loc, const StatusLine& sl)
{
	_req = req;
	_loc = loc;
	_staLine = sl;

	if (_staLine.getCode() >= 400)
		return fillError(_staLine);

	try
	{
		std::string hostName(_req->getHeaders().find("Host")->second); 		// Keeping only host name and removing port
		hostName = hostName.substr(0, hostName.find(':')); // * tal vez no sea necesario *

        if (!_loc.get_return_uri().empty())        // Doing an HTTP redirection (301) if redirect field filled in matched location block
        {
			std::cout << "[debug] return uri: " << _loc.get_return_uri() << "\n";
            std::string redirectedUri = _req->getPath();            // Replacing location name in the URI with the redirect string set in config file
            replaceLocInUri(redirectedUri, _loc.get_return_uri());

            // Replacing previous requested URI with redirected URI for next client request
            // (Location header in 301 response will be set with this URI)
            _req->setPath(redirectedUri);

            throw StatusLine(301, REASON_301, "http redirection");
        }
		std::string realUri = reconstructFullURI(_req->getMethod(), _req->getPath());    // Modifying URI with root and index directive if any, checking for the allowed methods
		std::string *cgiName = getCgiExecutableName(realUri);     					    // Checking if the targeted file is a CGI based on his extension

        // Execute the appropriate method and fills the response buffer with status line + 
        // headers + body (if any). If an error occurs during this process, it will throw 
        // a StatusLine object with the appropriate error code.
		if (cgiName && (_req->getMethod() == GET || _req->getMethod() == POST))
			execCgi(realUri, cgiName);
		else if (_req->getMethod() == GET || _req->getMethod() == HEAD)
            execGet(realUri);
		else if (_req->getMethod() == POST)
            execPost(realUri);
		else if (_req->getMethod() == DELETE)
            execDelete(realUri);
		else
			throw (StatusLine(400, REASON_400, "request method do not exist")); /* doble check de method */
	}
	catch (const StatusLine& errorStaLine)	// If an error occured during the reponse creation
	{
		fillError(errorStaLine);
	}
}


/* ----------------------- PRIVATE METHODS --------------------- */

void Response::execCgi(const std::string& realUri, std::string* cgiName)
{
    struct stat st;
    if (stat(realUri.c_str(), &st) == -1)
	{
        throw StatusLine(404, REASON_404, "fillCgi method: " + realUri);
	}

	Body cgiRep;
	CGI cgi(&cgiRep, _req, realUri, *cgiName);
	
	cgi.executeCGI();
	
	fillStatusLine(_staLine);
	fillServerHeader();
	fillDateHeader();
	fillContentlengthHeader(convertNbToString(cgiRep.getSize()));

	_buffer += cgiRep.getBody();
	
	delete cgiName;	
}

void Response::fillContentlengthHeader(const std::string& size) 
{
	_buffer += "Content-Length: " + size + CRLF;
}

void Response::fillServerHeader() 
{
	_buffer += "Server: webserv\r\n";
}

void Response::fillDateHeader() 
{
	time_t now = time(0);			 // current date and time on the current system

	char* date_time = ctime(&now); 	// convert now to string form

	std::vector<std::string> date = splitWithSep(date_time, ' ');	// Splitting date line and removing '\n'

	date.back().resize(4);

	// Formating header date.
	// ctime format = Thu May 20 14:33:40 2021 >> to header date format : Thu, 20 May 2021 12:16:42 GMT
	_buffer += "Date: " + date[0] + ", " + date[2] + " " + date[1] + " " + date[4] + " " + date[3] + " GMT" + CRLF;
}

/* st_mtimespec no portable (#define st_mtime st_mtimespec.tv_sec) */
void Response::fillLastModifiedHeader(const char* uri)
{
	struct stat infFile;

	if (stat(uri, &infFile) == -1)
		throw StatusLine(404, REASON_404, "fillLastModifiedHeader method");

	struct tm* lm = localtime(&infFile.st_mtime);

	const std::string day[7] = { "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
	const std::string mon[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

	_buffer += "Last-Modified: " + day[lm->tm_wday - 1] + ", " + convertNbToString(lm->tm_mday) + " " + mon[lm->tm_mon] + " " 
			+ convertNbToString(lm->tm_year + 1900) + " " + convertNbToString(lm->tm_hour) + ":" + 
			convertNbToString(lm->tm_min) + ":" + convertNbToString(lm->tm_sec) + " GMT" + CRLF;
}

void Response::fillLocationHeader(const std::string& redirectedUri)
{
    _buffer += "Location:" + redirectedUri + CRLF;
}

void Response::fillStatusLine(const StatusLine& staLine)
{
	_buffer = "HTTP/1.1 " + convertNbToString(staLine.getCode()) + " " + staLine.getReason();
	if (!staLine.getAdditionalInfo().empty())
		_buffer += " (" + staLine.getAdditionalInfo() + ")";
	_buffer += CRLF;
}

void print_Loc(const Location& _loc)
{
	std::cout << "root: " << _loc.get_root() << "\n";
	for (std::vector<std::string>::const_iterator it = _loc.get_methods().begin(); it != _loc.get_methods().end(); it++)
		std::cout << "methods: " << *it << "\n";
		
	for (std::vector<std::string>::const_iterator it = _loc.get_index().begin(); it != _loc.get_index().end(); it++)
		std::cout << "index: " << *it << "\n";
}

void Response::replaceLocInUri(std::string& uri, const std::string& root)
{
	std::cout << "[debug] uri antes: " << uri << "\n";
	if (root[root.size() - 1] == '/')
		uri.erase(0, 1); // borra '/' sobrante
		
	uri.insert(0, root);
	std::cout << "[debug] uri despues: " << uri << "\n";
}

std::string Response::addIndex(const std::string& uri, const std::vector<std::string>& indexs)
{
	struct stat infFile;
	std::cout << "uri pre index: " << uri << "\n";
	for (std::vector<std::string>::const_iterator it = indexs.begin(); it != indexs.end(); ++it)
	{
		// Add each index to the uri
		std::string uriWithIndex(uri + *it);

		// And then try to access the URI resulting from this concatenation
		if (!stat(uriWithIndex.c_str(), &infFile))
			return uriWithIndex;
	}
	std::cout << "uri pos index: " << uri << "\n";
	if (_loc.get_autoindex()) {
		_autoIndex = true;
		return uri;
	}
	throw StatusLine(403, REASON_403, "trying to access a directory addIndex method");
}

void Response::checkMethods(int method, const std::vector<std::string>& methodsAllowed) const
{
	std::string tab[NB_METHODS] = { "GET", "HEAD", "POST", "DELETE" };

	for (std::vector<std::string>::const_iterator it = methodsAllowed.begin();
			it != methodsAllowed.end(); ++it)
		if (!it->compare(tab[method]))
			return ;

	throw StatusLine(405, REASON_405, "checkMethods method");
}

std::string Response::reconstructFullURI(int method, std::string uri)
{
	bool fileExist = true;
	struct stat infFile;

	// Replacing the part of the URI that matched with the root path if there is one existing
	if (!_loc.get_root().empty() && !(method == POST && !_loc.get_upload_path().empty()))
		replaceLocInUri(uri, _loc.get_root());

	else if (method == POST && !_loc.get_upload_path().empty()){ /* before need to be checked CGI */
		std::string fullUploadPath(_loc.get_upload_path());
		replaceLocInUri(fullUploadPath, _loc.get_root());
		replaceLocInUri(uri, fullUploadPath);
	}
	// If no root in location block, or root doesn't start with a '.', need to add it to find the file using
	// relative path
	if (!uri.empty() && uri[0] == '/')
		uri.insert(uri.begin(), '.');

	// Checking if the path after root substitution is correct, and if it's a directory trying
	// to add indexs. Case POST method, no 404 because it can create the file.
	if (stat(uri.c_str(), &infFile) == -1 && !(fileExist = false) && method != POST)
		throw StatusLine(404, REASON_404, "reconstructFullURI method: " + uri);

    // Case we match a directory and an autoindex isn't set. We try all the possible indexs, if none
    // works addIndex throw a 403 error StatusLine object
	if (fileExist && S_ISDIR(infFile.st_mode))
	{
		if (!uri.empty() && uri.at(uri.size() - 1) != '/') // redirect directories not ended in '/'
		{
			_req->setPath(_req->getPath() + "/");
			throw StatusLine(301, REASON_301, "redirect " + uri + " to complete directory uri");
		}
		if (!(method == DELETE))
		{
			uri = addIndex(uri, _loc.get_index());
		}
	}
	checkMethods(method, _loc.get_methods());

	return uri;
}

void Response::fillError(const StatusLine& sta)
{    
    if (sta.getCode() != _staLine.getCode())
    {
	    _staLine = sta;
	}
	
	_buffer.clear();
	
	// Filling buffer with error code + some basic headers
	fillStatusLine(sta);
	fillServerHeader();
	fillDateHeader();

    // Case HTTP redirection
    if (_staLine.getCode() == 301)
	{
        fillLocationHeader(_req->getPath());
	}

	// Value of host header field in request
	const std::string* hostField = &_req->getHeaders().find("host")->second;
	const std::string hostValue(hostField->substr(0, hostField->find(':')));

	// Looking in each virtual server names if one match host header field value, if
    // not using default server


	std::string pathError;
	std::string errorFile;
	std::string errorCodeHTML = "/" + convertNbToString(sta.getCode()) + ".html";

    // Custom error pages if set in config file
	if (_staLine.getCode() != 404 || _loc.get_error_page().empty()) {
		errorFile = errorPage(sta);
		fillContentTypeHeader();
	} 
	else
	{
        // Adding relative file access if not well filled in config file
		std::string pathError = _req->getPath().substr(0, _req->getPath().rfind('/')) + "/" + _loc.get_error_page();
		replaceLocInUri(pathError, _loc.get_root());
		std::cerr << "[debug] path error: " << pathError << "\n";
		struct stat infFile;
		if (stat(pathError.c_str(), &infFile) == -1) /* check if pathError file is readable */{
			std::cerr << strerror(errno) << "\n";
			errorFile = errorPage(sta);
			fillContentTypeHeader();
		}
		else {
			FileParser body(pathError.c_str(), true);
			errorFile = body.getRequestFile();
			fillContentTypeHeader(getResourceExtension(pathError));
		}
	}

    // Filling buffer

	//fillContentlengthHeader(convertNbToString(body.getRequestFileSize()));
	fillContentlengthHeader(convertNbToString(errorFile.size()));
	_buffer += CRLF + errorFile;
}

void Response::postToFile(const std::string& uri)
{
	std::fstream postFile;

	postFile.open(uri.c_str(), std::ios_base::out | std::ios_base::trunc);

	if (!postFile.is_open()) {
		std::cerr << "[DEBUG] " <<  strerror(errno) << "\n";
		std::cout << "[DEBUG] trying to create file in " << uri.c_str() << "\n";
		throw (StatusLine(500, REASON_500, "failed to open file in post method"));
	}
	postFile << _req->getBody().getBody();
}

bool Response::isResourceAFile(const std::string& uri) const {
	return (!uri.empty() && uri[uri.size() - 1] == '/');
}

void Response::execGet(const std::string& realUri)
{
    // Storing status line and some headers in buffer
    fillStatusLine(_staLine);
    fillServerHeader();
    fillDateHeader();
    
	if (!isResourceAFile(realUri)) {
		FileParser body(realUri.c_str(), true); // CAHNGER

        // Setting size after storing the body in FileParser object, then setting Last-Modified header
        fillContentlengthHeader(convertNbToString(body.getRequestFileSize()));
        fillLastModifiedHeader(realUri.c_str());

        // For GET, writing the body previously stored to the buffer
        if (_req->getMethod() == GET) {
			fillContentTypeHeader(getResourceExtension(realUri));
            _buffer += CRLF + body.getRequestFile();
		}
		std::cout << _buffer << "\n";
		return ;
	}
	if (_autoIndex == false) { // may be redundant code
		throw StatusLine(403, REASON_403, "requested a directory with autoindex set off");
	}
	if (_autoIndex == true)  {
		std::string autoIndexPage;
        autoIndexDisplayer(realUri, autoIndexPage);
		fillContentTypeHeader();
        fillContentlengthHeader(convertNbToString(autoIndexPage.size()));
        _buffer += CRLF + autoIndexPage;
	}
}

void Response::execPost(const std::string& realUri)
{
    // Need to create file so changing code 200 ("OK") to 201 ("created")
    struct stat infoFile;
    if (stat(realUri.c_str(), &infoFile) == -1)
        _staLine = StatusLine(201, REASON_201, "POST error");
    
    // Creating a new file or appending to existing file post request payload. If 
    // opening failed, throws StatusLine object with error 500 (internal error)
    postToFile(realUri);
    
    // Storing status line and some headers in buffer
    fillStatusLine(_staLine);
    fillServerHeader();
    fillDateHeader();
}

void Response::execDelete(const std::string& realUri)
{
    if (remove(realUri.c_str()))
        throw (StatusLine(500, REASON_500, "remove function failed in DELETE method"));

    // Storing status line and some headers in buffer
    fillStatusLine(_staLine);
    fillServerHeader();
    fillDateHeader();    
}

void Response::autoIndexDisplayer(const std::string& realUri, std::string& autoIndexPage) {
	autoIndexPage.append("<html>\n<head><title>Index of " + realUri + "</title></head>\n");
	autoIndexPage.append("<body><h1>Index of " + realUri + "</h1><hr><br><pre><ul>");

	DIR* 			dir_ptr;
	struct dirent*	dir_s;

	std::vector<std::string> file_list;
	dir_ptr = opendir(realUri.c_str());
	if (!dir_ptr) {
		throw StatusLine(500, REASON_500, "autoindex: could not open directory");
	}
	while ((dir_s = readdir(dir_ptr)) != NULL) {
		file_list.push_back(dir_s->d_name);
	}
	closedir(dir_ptr);

	for (std::vector<std::string>::iterator it = file_list.begin(); it != file_list.end(); it++) {
		struct stat info;
		std::string filePath = realUri + *it;

		if (stat(filePath.c_str(), &info) == -1) {
			StatusLine(500, REASON_500, "autoindex: could not open file");
		}
		std::string timeStamp = asctime(localtime(&info.st_mtime));

		timeStamp.erase(--timeStamp.end());
		std::string fileName = (it->size() > 10) ? it->substr(0, 15) + "." : *it;
		std::string spaceFirst = std::string("                    ").substr(fileName.size());
		std::string fileSize = convertNbToString(info.st_size);
		std::string spaceSecond = std::string("          ").substr(fileSize.size());

		autoIndexPage.append("<li><a href=\"" + *it + "\">" + fileName + "</a>" 
			+ spaceFirst + fileSize + spaceSecond + timeStamp + "</li>\n");
	}
	autoIndexPage.append("</ul></pre><br></body></html>");
}

std::vector<std::string> Response::splitWithSep(const std::string& str, char dlm) {
	std::vector<std::string> tab;
	std::stringstream strStream(str.c_str());
	std::string token;

	while (std::getline(strStream, token, dlm)) {
		tab.push_back(token);
	}
	return tab;
}

std::string* Response::getCgiExecutableName(const std::string& uri) {
	typedef std::vector<std::pair<std::string, std::string> >::const_iterator cgiIterator;
	std::string fileExt(getResourceExtension(uri));

	for (cgiIterator it = _loc.get_cgi_pass().begin(); it != _loc.get_cgi_pass().end(); it++) {
		if (!fileExt.compare(it->first)) {
			return new std::string(fileExt);
		}
	}
	return NULL;
}

std::string Response::errorPage(const StatusLine& sl) {
	std::string errorPage;

	errorPage.append("<!DOCTYPE><html><head><title>Webserv</title></head><body><h1>Webserv</h1><hr><p>");
	errorPage.append(sl.getReason());
	errorPage.append("</p></body></html>");
	return errorPage;
}

/* overload for webserver-generated pages */
void Response::fillContentTypeHeader(void) {
	_buffer.append("Content-Type: text/html");
	_buffer.append(CRLF);
}

void Response::fillContentTypeHeader(const std::string& fileExt) {
	std::string contentTypeValue = "application/octet-stream"; // default value for unrecognized file extesions

	_buffer.append("Content-Type: ");
	if (fileExt.empty()) {
		_buffer.append(contentTypeValue + CRLF);
		return ;		
	}

	/* content-type should be set according to accept header values; if
	   accept header is present in request and does not allow response uri file format,
	   server should send 406 code error, but this behaviour is optional */
	const std::string type[4] = {"audio", "image", "text", "video"};

	const std::string audio_subtype[1] = {"mpeg"};
	const std::string audio_fileext[1] = {"mp3"};

	const std::string image_subtype[5] = {"gif", "jpeg", "jpeg", "png", "tiff"};
	const std::string image_fileext[5] = {"gif", "jpg",  "jpeg", "png", "tiff"};

	const std::string text_subtype[5] = {"css", "csv", "html", "plain", "xml"};
	const std::string text_fileext[5] = {"css", "csv", "html", "txt",   "xml"};

	const std::string video_subtype[5] = {"mpeg", "mp4", "quicktime", "x-flv", "webm"};
	const std::string video_fileext[5] = {"mpeg", "mp4", "mov",       "flv",   "webm"};

	const std::string* subtype_arr[4] = {audio_subtype, image_subtype, text_subtype, video_subtype};
	const std::string* fileext_arr[4] = {audio_fileext, image_fileext, text_fileext, video_fileext};

	int subtype_id[4] = {1, 5, 5, 5};

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < subtype_id[i]; j++) {
			if (!fileext_arr[i][j].compare(fileExt)) {
				contentTypeValue = type[i] + "/" + subtype_arr[i][j];
				break ;
			}
		}
	}
	_buffer.append(contentTypeValue + CRLF);
}

std::string Response::getResourceExtension(const std::string& uri) const {
	std::string ext("");
	size_t		slash_pos = uri.rfind('/');

	if (slash_pos == std::string::npos) {
		return ext;
	}
	size_t		dot_pos = uri.find('.', slash_pos);

	if (dot_pos == std::string::npos) {
		return ext;
	}
	ext = uri.substr(dot_pos + 1);
	return ext;
}

/* --------------- NON-MEMBER FUNCTION OVERLOADS --------------- */

void swap(Response& a, Response& b)
{
	std::swap(a._req, b._req);
	swap(a._staLine, b._staLine);
	std::swap(a._buffer, b._buffer);
    std::swap(a._autoIndex, b._autoIndex);
}