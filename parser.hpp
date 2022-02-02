#ifndef __PARSER_HPP__
#define __PARSER_HPP__

/* directives can only accept one declaration in a block */

#define N_TOKEN_TYPE 5
#define N_LOCATION_DIR 9
#define N_SERVER_DIR 11
#define N_VALID_DIR 12

#include <string>
#include <cctype>
#include <iostream>
#include <fstream>
#include <sstream>
#include <deque>
#include <vector>
#include <algorithm>

 enum directive_flag_t {
            ERROR_PAGE,
            BODY_SIZE,
            ROOT,
            AUTOINDEX,
            INDEX,
            RETURN,
            CGI_PASS,
            ACCEPT_METHOD,
            ACCEPT_UPLOAD,
            LISTEN,
            SERVER_NAME,
            LOCATION
};

class Parser {
    public:
        struct location_block_t {
            std::string uri;
            std::vector<std::string> directive[N_LOCATION_DIR];

            location_block_t(void);
            location_block_t(const std::string& uri_);
            location_block_t(const location_block_t& other);
            location_block_t& operator=(const location_block_t& other);
        };

        struct server_block_t {
            std::vector<location_block_t> location;
            std::vector<std::string>      directive[N_SERVER_DIR];

            server_block_t(void);
            server_block_t(const server_block_t& other);
            server_block_t& operator=(const server_block_t& other);
        };

        typedef std::string::size_type  size_type;
        typedef std::vector<server_block_t> server_vector;

    private:
        enum token_flag_t {
            CBO,
            CBC,
            SEMICOLON,
            SPACE,
            WORD,
            NON_VALID_CHAR
        };
        typedef size_t (Parser::*tokenize_options)(int);
        typedef bool (Parser::*directive_parse_table)(void) const;

        enum quotes_flags_e { ON, OFF };
        struct token_t {
            token_flag_t type;
            std::string  token;

            token_t(token_flag_t type_, const std::string& token_);
            token_t(const token_t& other);
        };


        std::deque<token_t> tok_lst;
        std::string         raw_input;

        static const std::string directive[N_VALID_DIR];
        static directive_parse_table directive_options[N_VALID_DIR];

        bool    is_number(const std::string& str, int n_max = 0, int n_min = 0) const;
        bool    is_addr(const std::string& addr) const;

        bool    is_word(const token_t& tok) const;
        bool    is_semicolon(const token_t& tok) const;
        bool    is_cbo(const token_t& tok) const;
        bool    is_cbc(const token_t& tok) const;

        bool    syntax_directive_max_body_size(void) const;
        bool    syntax_directive_rrue(void) const; // root, return, upload, error_page
        bool    syntax_directive_autoindex(void) const;
        bool    syntax_directive_index(void) const;
        bool    syntax_directive_return(void) const;
        bool    syntax_directive_cgi_pass(void) const;
        bool    syntax_directive_accept_method(void) const;
        bool    syntax_directive_listen(void) const;
        bool    syntax_directive_server_name(void) const;
        bool    syntax_directive_location(void) const;        

        bool    syntax_directive_duplicate(void) const;

        directive_flag_t syntax_directive(void); // change names for clarity
        location_block_t syntax_location_block(const std::string& uri);
        server_block_t   syntax_server_block(void);

        const token_t& current(void) const;
        bool           empty(void) const;
        token_t        peek(size_type pos = 1) const;
        void           next(void);

        token_flag_t  tokenize_id(char c) const;

        std::string tokenize_word_clean_token(const std::string& token) const;

        size_type  tokenize_curly_bracket(int pos);
        size_type  tokenize_semicolon(int pos);
        size_type  tokenize_word(int pos);
        size_type  tokenize_space(int pos);

        void   read_config_file(const std::string& path);
        void   tokenize(void);
        
    public:
        Parser(void);
        Parser(const Parser& other);
        ~Parser();

        Parser& operator=(const Parser& other);

        server_vector    parse(const std::string& config_path);
};

#endif // __PARSER_HPP__
