#include <iostream>
#include <unordered_map>
#include <thread>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <atomic>
#include <mutex>
#include <chrono>

using namespace std;

unordered_map<int, long long> memo;
atomic<int> total_requests{0};
atomic<chrono::duration<double>> total_duration{chrono::duration<double>::zero()};
mutex metrics_mutex;

long long fib(int n) {
    if(n == 0 || n == 1) return n;
    if(memo.find(n) != memo.end()) return memo[n];
    memo[n] = fib(n-1) + fib(n-2);
    return memo[n];
}

string generate_metrics() {
    stringstream metrics;
    metrics << "# HELP fib_calculations_total Total number of Fibonacci calculations\n";
    metrics << "# TYPE fib_calculations_total counter\n";
    metrics << "fib_calculations_total " << total_requests.load() << "\n";
    
    metrics << "# HELP fib_calculation_duration_seconds Total time spent calculating Fibonacci\n";
    metrics << "# TYPE fib_calculation_duration_seconds counter\n";
    metrics << "fib_calculation_duration_seconds " << total_duration.load().count() << "\n";
    
    return metrics.str();
}

void handle_client(int client_socket) {
    char buffer[1024] = {0};
    read(client_socket, buffer, 1024);
    
    string response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: text/plain; version=0.0.4\r\n";
    response += "Connection: close\r\n\r\n";
    response += generate_metrics();
    
    send(client_socket, response.c_str(), response.size(), 0);
    close(client_socket);
}

void start_metrics_server() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    int opt = 1;
    int port = 8080;
    
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    
    cout << "Metrics server started on port " << port << endl;
    
    while (true) {
        int addrlen = sizeof(address);
        if ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            continue;
        }
        
        handle_client(client_socket);
    }
}

int main(int argc, char* argv[]) {
    thread metrics_thread(start_metrics_server);
    metrics_thread.detach();
    
    if(argc != 2) {
        cerr << "ERROR: Use exactly one argument" << endl;
        return 0;
    }

    int n = stoi(argv[1]);
    if(n > 92 || n < 0) {
        cerr << "ERROR: Argument must be between 0 and 92" << endl;
        return 0;
    }

    auto start = chrono::high_resolution_clock::now();
    long long result = fib(n);
    auto end = chrono::high_resolution_clock::now();
    
    chrono::duration<double> duration = end - start;
    
    {
        lock_guard<mutex> lock(metrics_mutex);
        total_requests++;
        total_duration.store(total_duration.load() + duration);
    }
    
    cout << "Fibonacci(" << n << ") = " << result << endl;
    cout << "Calculation took " << duration.count() << " seconds" << endl;
    
    while (true) {
        this_thread::sleep_for(chrono::seconds(1));
    }
    
    return 0;
}