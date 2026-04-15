
from ubuntu@sha256:72297848456d5d37d1262630108ab308d3e9ec7ed1c3286a32fe09856619a782 AS app

#RUN apt-get update && \
#	apt-get install -y gdb gdbserver socat


#FROM pwn.red/jail

#COPY --from=app / /srv

COPY main /srv/app/run
COPY flag.txt libc.so.6* ld-linux-x86-64.so.2* /srv/app/

RUN chmod +x /srv/app/run

#CMD socat -T 30 TCP-LISTEN:5000,reuseaddr,fork EXEC:/srv/app/run