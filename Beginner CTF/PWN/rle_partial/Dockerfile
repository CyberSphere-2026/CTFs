FROM ubuntu:22.04

# Install only required tools
RUN apt-get update && \
    apt-get install -y --no-install-recommends socat patchelf && \
    rm -rf /var/lib/apt/lists/*

# Create user
RUN groupadd -r ctf && useradd -r -g ctf ctf

WORKDIR /home/ctf

# Copy files
COPY ld-linux-x86-64.so.2 libc.so.6 bad_dna flag.txt message.txt ./

# Permissions
RUN chown -R root:ctf /home/ctf && \
    chmod 750 * 

# Patch binary
RUN patchelf --set-interpreter /home/ctf/ld-linux-x86-64.so.2 bad_dna && \
    patchelf --set-rpath /home/ctf bad_dna

USER ctf

EXPOSE 5151

# Clean network exposure (no scripts, no ynetd)
CMD ["socat", "TCP-LISTEN:5151,reuseaddr,fork", "EXEC:./bad_dna,stderr"]
