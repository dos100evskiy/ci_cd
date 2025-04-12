# FROM ubuntu:latest

# # # Copy the .deb package into the container
# # COPY ci_cd.deb /app/

# # Install dependencies, fix broken package installs, and clean up
# RUN apt-get update && \
#     apt-get install -y dpkg && \
#     dpkg -i /app/ci_cd.deb || apt-get -f install -y && \
#     rm -rf /var/lib/apt/lists/*

# # Run the executable using its absolute path
# CMD ["/fib", "10"]
FROM ubuntu:latest

# Установка компилятора
RUN apt-get update && \
    apt-get install -y g++ make && \
    rm -rf /var/lib/apt/lists/*

# Копируем исходники
COPY . /app
WORKDIR /app

# Компилируем
RUN make || g++ main.cpp -o fib

# Копируем бинарник в /usr/local/bin
RUN cp fib /usr/local/bin/ && \
    chmod +x /usr/local/bin/fib

# Проверяем, что файл на месте
RUN ls -la /usr/local/bin/fib

# Явно указываем путь к бинарнику
CMD ["/usr/local/bin/fib", "10"]