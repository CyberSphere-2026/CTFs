FROM ubuntu:22.04
RUN apt-get update && apt-get install -y attr && rm -rf /var/lib/apt/lists/*
WORKDIR /challenge
COPY flag .
COPY enc .
RUN setfattr -n user.key -v "dummy_key" flag && \
    setfattr -n user.nonce -v "dummy_nonce" flag
CMD ["echo", "hello world"]
