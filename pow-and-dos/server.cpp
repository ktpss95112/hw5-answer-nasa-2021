// this source file is not released
// an ip address can have only one connection at a time

#include <iostream>
#include <string>
#include <tuple>
#include <thread>
#include <functional>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <mutex>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>


#define TIMEOUT_PER_CONNECTION 90


struct Conn {
    int client_fd;
    std::string ipstr;
    int port;
};


std::tuple<std::string, int> get_ip_port_from_socket (int sockfd) {

    struct sockaddr_storage addr;
    socklen_t len = sizeof(addr);
    char ipstr[INET6_ADDRSTRLEN];
    int port;
    if (getpeername(sockfd, (struct sockaddr*)&addr, &len) == -1) {
        std::cerr << "Error on getpeername, errno = " << errno << ": " << std::strerror(errno) << std::endl;
        return std::make_tuple("", -1);
    }

    if (addr.ss_family == AF_INET) {
        struct sockaddr_in *s = (struct sockaddr_in *)&addr;
        port = ntohs(s->sin_port);
        inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
    } else { // AF_INET6
        struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
        port = ntohs(s->sin6_port);
        inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
    }

    return std::make_tuple(ipstr, port);
}


std::mutex connection_status_mutex;
std::unordered_map<std::string, bool> connection_status;
bool connection_exists (Conn conn) {
    bool ret;
    connection_status_mutex.lock();
    ret = !(connection_status.find(conn.ipstr) == connection_status.end() || !connection_status.at(conn.ipstr));
    connection_status_mutex.unlock();
    return ret;
}
void connection_add (Conn conn) {
    connection_status_mutex.lock();
    connection_status[conn.ipstr] = true;
    connection_status_mutex.unlock();
}
void connection_del (Conn conn) {
    connection_status_mutex.lock();
    connection_status.erase(conn.ipstr);
    connection_status_mutex.unlock();
}


Conn accept_connection (int socket_fd) {
    sockaddr_storage client_address;
    socklen_t addr_len = sizeof(client_address);

    int client_fd;
    if ((client_fd = accept(socket_fd, (sockaddr *)&client_address, &addr_len)) == -1 ) {
        // TODO: error handling
        std::cerr << "Error on accept, errno = " << errno << ": " << std::strerror(errno) << std::endl;
        std::exit(EXIT_FAILURE);
    }

    auto [client_ip_str, client_port] = get_ip_port_from_socket(client_fd);
    Conn conn {client_fd, client_ip_str, client_port};

    if (connection_exists(conn)) {
        std::cerr << "[warn] connection from " << client_ip_str << ":" << client_port << " already exists, drop connection" << std::endl;
        if (write(client_fd, "You can only create one connection simultaneously!\n", 52) != 52) {} // to surpress compiler's warning
        close(client_fd);
        return Conn{-1, "", -1};
    }
    connection_add(conn);

    std::cerr << "[info] accept connection from " << client_ip_str << ":" << client_port << std::endl;

    return conn;
}


void handle_conn (Conn conn) {
    // fork
    pid_t pid;
    bool wait_success = false;
    if ((pid = fork()) < 0) {
        std::cerr << "Error on fork(), errno = " << errno << " : " << std::strerror(errno) << std::endl;
        goto END_CONNECTION;
    }
    else if (pid == 0) {
        dup2(conn.client_fd, STDIN_FILENO);
        dup2(conn.client_fd, STDOUT_FILENO);
        if (execlp("./main.py", "./main.py", (char *)nullptr) == -1) {
            std::cerr << "error on execlp(), errno = " << errno << " : " << std::strerror(errno) << std::endl;
            goto END_CONNECTION;
        }
    }

    // wait
    for (int i = 0; i < TIMEOUT_PER_CONNECTION; ++i) {
        sleep(1);
        if (int status, ret = waitpid(pid, &status, WNOHANG); ret == -1) {
            std::cerr << "Error on waitpid(), errno = " << errno << " : " << std::strerror(errno) << std::endl;
            goto END_CONNECTION;
        }
        else if (ret > 0) {
            wait_success = true;
            break;
        }
    }

    // kill
    if (!wait_success) {
        if (kill(pid, SIGTERM) == -1) {
            std::cerr << "Error on kill(), errno = " << errno << " : " << std::strerror(errno) << std::endl;
            goto END_CONNECTION;
        }
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            std::cerr << "Error on kill(), errno = " << errno << " : " << std::strerror(errno) << std::endl;
            goto END_CONNECTION;
        }
    }

END_CONNECTION:
    connection_del(conn);
    close(conn.client_fd);
    std::cout << "Connection from " << conn.ipstr << ":" << conn.port << " closed." << std::endl;
}


int main (int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: ./server bind_addr port\n"
                  << "    If you don't know what bind_addr is, just leave it '0.0.0.0'\n"
                  << "\n"
                  << "Example:\n"
                  << "    ./server 0.0.0.0 8888\n";
        std::exit(EXIT_FAILURE);
    }

    std::string bind_addr {argv[1]};
    std::string port {argv[2]};

    // setup server
    int socket_fd;
    {
        addrinfo hints;
        addrinfo *res;
        std::memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;

        std::string port_str = port;

        if (int status = getaddrinfo(bind_addr.c_str(), port_str.c_str(), &hints, &res); status != 0) {
            std::cerr << "Error on getaddrinfo: " << gai_strerror(status) << std::endl;
            exit(EXIT_FAILURE);
        }

        if ((socket_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1 ) {
            std::cerr << "Error on creating socket, errno = " << errno << ": " << std::strerror(errno) << std::endl;
            std::exit(EXIT_FAILURE);
        }

        if (int optval = 1; setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1 ) {
            std::cerr << "Error on setsockopt, errno = " << errno << ": " << std::strerror(errno) << std::endl;
            std::exit(EXIT_FAILURE);
        }

        if (bind(socket_fd, res->ai_addr, res->ai_addrlen) == -1) {
            std::cerr << "Error on bind, errno = " << errno << ": " << std::strerror(errno) << std::endl;
            std::exit(EXIT_FAILURE);
        }

        if (listen(socket_fd, 128) == -1) {
            std::cerr << "Error on listen, errno = " << errno << ": " << std::strerror(errno) << std::endl;
            std::exit(EXIT_FAILURE);
        }

        std::cerr << "Listening on " << bind_addr << ":" << port << " ... " << std::endl;

        freeaddrinfo(res);
    }

    // accept connection
    {
        while (1) {
            Conn conn = accept_connection(socket_fd);
            if (conn.client_fd == -1) continue;
            std::thread{handle_conn, conn}.detach();
        }
    }

    return 0;
}
