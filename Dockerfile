FROM ubuntu:latest

# Copy the .deb package into the container
COPY ci_cd.deb /app/

# Install dependencies, fix broken package installs, and clean up
RUN apt-get update && \
    apt-get install -y dpkg && \
    dpkg -i /app/ci_cd.deb || apt-get -f install -y && \
    rm -rf /var/lib/apt/lists/*

# Run the executable using its absolute path
CMD ["/fib", "10"]