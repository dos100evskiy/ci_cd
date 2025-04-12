FROM ubuntu:latest

RUN apt-get update && \
    apt-get install -y g++ make && \
    rm -rf /var/lib/apt/lists/*

COPY . /app
WORKDIR /app

RUN g++ -O2 -std=c++11 -pthread main.cpp -o fib

EXPOSE 8080

CMD ["/app/fib", "10"]S