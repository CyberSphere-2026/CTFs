FROM ubuntu:22.04

# Install some libs
RUN apt-get update && apt-get -y upgrade && apt install patchelf

# create ctf-user
RUN groupadd -r ctf && useradd -r -g ctf ctf
COPY quotes /home/ctf/
COPY launch.sh /home/ctf/
COPY ynetd /home/ctf/
COPY flag.txt /home/ctf
COPY ld-linux-x86-64.so.2 /home/ctf
COPY libc.so.6 /home/ctf

#set some proper permissions
#RUN pip install inputimeout 
RUN chown -R root:ctf /home/ctf
RUN chmod 750 /home/ctf/quotes
RUN chmod 750 /home/ctf/ynetd
RUN chmod 750 /home/ctf/launch.sh
RUN chmod 750 /home/ctf/flag.txt


RUN patchelf \
  --set-interpreter /home/ctf/ld-linux-x86-64.so.2 \
  --set-rpath /home/ctf \
  /home/ctf/quotes


USER ctf
EXPOSE 5153

CMD /home/ctf/ynetd -p 5153 /home/ctf/launch.sh
