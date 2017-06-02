#include "remote_handler.h"
#include <thread>
#include <LibSL.h>
using asio::ip::tcp;
using namespace  std;



//send a header which contain the size of the next buffer sent.
void Messaging::send_header(size_t buffsize)throw (std::system_error)
{
    tcp_header head;
    head.buffer_size = buffsize;
    sock->send(asio::buffer(&head.buffer_size,sizeof(head.buffer_size)));
}

void Messaging::handle_error(int len, asio::error_code& asio_err)throw (std::system_error)
{
    if(len  == 0) {
        if(asio_err == asio::error::eof){
            cerr << "disconnected" << endl;
        }
        cerr << asio_err.message() << endl;
    }
}

//receive an expected header
int Messaging::receive_header(tcp_header& head)throw (std::system_error)
{

    asio::error_code asio_err;
    std::vector<int> size(1);
    int len = sock->read_some(asio::buffer(size),asio_err);
    handle_error(len,asio_err);
    if(len < 1)return len;
    head.buffer_size = size[0];
    return len;
}

//init a client
void Messaging::initClient(void (*onUpdate)(script_error&))
{
    tcp::endpoint ep(asio::ip::address::from_string("127.0.0.1"), 50500);
    asio::error_code asio_err;
    sock->connect(ep,asio_err);
    handle_error(0,asio_err);
    client_callback = onUpdate;
    isInitiated = true;

}

void Messaging::initServer(void (*onUpdate)(Updater&))
{
    tcp::acceptor a(*service, tcp::endpoint(asio::ip::address::from_string("127.0.0.1"), 50500));
    asio::error_code asio_err;
    a.accept(*sock,asio_err);
    handle_error(0,asio_err);
    server_callback = onUpdate;
    isInitiated = true;

}

int Messaging::receive_update( Updater& up) throw (std::system_error)
{
    tcp_header head;
    int len = receive_header(head);
    if(len < 1)return len;
    if(head.buffer_size > 512)return;
    asio::error_code asio_err;
    std::vector<int> size(1);
    len = sock->read_some(asio::buffer(size),asio_err);
    handle_error(len,asio_err);
    if(len < 1)return len;

    up.val = size[0];
    (*server_callback)(up);
    return len;
}

void Messaging::send_update(Updater& up)
{
    try{
        asio::error_code asio_err;
        send_header(sizeof(int));
        sock->send(asio::buffer(&up.val,sizeof(up.val)),asio::socket_base::message_do_not_route,asio_err);
    }catch (std::system_error er){
        cerr << "socket disconnected" << endl;
    }

}

void Messaging::send_error(script_error& err)
{
    try{
        int size = sizeof(char)*err.msg.size();
        send_header(size);
        sock->send(asio::buffer(&err.code,sizeof(err.code)));
        sock->send(asio::buffer(&err.line,sizeof(err.line)));
        std::vector<char> err2char;
        for(int i = 0; i < err.msg.size(); i++){
            err2char.push_back(err.msg[i]);
        }
        sock->send(asio::buffer(err2char));
    }catch (std::system_error er){
        cerr << "socket disconnected" << endl;
    }

}

int Messaging::receive_error(script_error& err)throw (std::system_error)
{
    asio::error_code asio_err;
    std::vector<int> size(1);
    tcp_header head;
    int len = receive_header(head);
    if(len < 1)return len;

    len = sock->read_some(asio::buffer(size),asio_err);
    handle_error(len,asio_err);
    if(len < 1)return len;

    err.code = size[0];
    len = sock->read_some(asio::buffer(size),asio_err);
    handle_error(len,asio_err);
    if(len < 1)return len;

    err.line = size[0];
    std::vector<char> msg(head.buffer_size);
    len = sock->read_some(asio::buffer(msg),asio_err);
    handle_error(len,asio_err);
    if(len < 1)return len;
    std::string s;
    for(int i = 0; i < head.buffer_size; i++){
        s.push_back(msg[i]);
    }
    err.msg = s;
    (*client_callback)(err);

    return len;
}

void Messaging::spawn_client(){
    client_thread = std::thread(&Messaging::getInstance().client);
}

void Messaging::end_client(){
    client_thread.join();
}

void Messaging::spawn_server(){
    server_thread = std::thread(&Messaging::getInstance().iceSLServer);
}

void Messaging::end_server(){
    server_thread.join();
}


