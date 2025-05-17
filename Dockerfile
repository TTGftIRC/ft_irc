FROM debian:bullseye

RUN apt update && apt install -y \ 
clang \
make \
netcat-openbsd \
valgrind \
&& apt-get clean && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . /app

CMD [ "bash" ]