#pragma once
#include <LibSL.h>
#include <thread>

#include "asio.hpp"
using asio::ip::tcp;

struct script_error{
    int code;
    int line;
    std::string msg;
};

struct Updater{
    int val;
};

//JH warning: this object size shouldn't vary at runtime
struct tcp_header{
    int buffer_size;

};

class Messaging{

    Messaging(){
        service = new asio::io_service();
        sock = new tcp::socket(*service);
        server_callback = nullptr;
        client_callback = nullptr;
        isInitiated = false;
    }
    bool isInitiated;
    std::thread client_thread;
    std::thread server_thread;

    tcp::socket* sock;
    asio::io_service* service;
    void (*server_callback)(Updater& up);
    void (*client_callback)(script_error& up);


    //send a header which contain the size of the next buffer sent.
    void send_header(size_t buffsize)throw (std::system_error);
    void handle_error(int len, asio::error_code& asio_err)throw (std::system_error);

    //receive an expected header
    int receive_header(tcp_header& head)throw (std::system_error);


    static void iceSLServer(){
        Updater up;
        while(Messaging::getInstance().receive_update(up) > 0){
            std::cerr << "updated!" << std::endl;
            std::cerr << up.val << std::endl;
        }
    }

    static void client(){
        script_error er;
        while(Messaging::getInstance().receive_error(er) > 0){
            std::cerr << er.msg << std::endl;
        }
    }


public:
    static Messaging& getInstance( ){
        static Messaging mes;
        return mes;
    }
    void initClient(void (*onUpdate)(script_error&));
    void initServer(void (*onUpdate)(Updater&));

    int receive_update( Updater& up) throw (std::system_error);
    void send_update(Updater& up);

    void send_error(script_error& err);
    int receive_error(script_error& err)throw (std::system_error);

    void spawn_client();
    void end_client();

    void spawn_server();
    void end_server();

};

