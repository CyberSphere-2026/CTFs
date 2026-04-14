
from ubuntu@sha256:fecdb5b9bf8874108b4fc23fac669b10968e54c450382b20d7db92565e7655ed AS app

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -yq --no-install-recommends \
                qemu-system-x86 \
        && rm -rf /var/lib/apt/lists/*

FROM pwn.red/jail

COPY --from=app / /srv
COPY run.sh /srv/app/run
COPY initramfs.cpio.gz bzImage /srv/app/
RUN chmod 755 /srv/app/run

ENV JAIL_TIME=400 JAIL_MEM=256M JAIL_CPU=500 JAIL_PIDS=10

RUN chmod +x /srv/app/run


