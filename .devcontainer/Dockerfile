FROM --platform=linux/amd64 ubuntu:20.04

ENV TZ=Asia/Seoul LANG=ko_KR.UTF-8 LANGUAGE=ko_KR.UTF-8

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
    locales tzdata build-essential gdb vim curl git wget \
    cmake make gcc g++ sudo \
    && locale-gen ko_KR.UTF-8 && update-locale LANG=ko_KR.UTF-8 \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

RUN useradd -m -s /bin/bash jungle && \
    echo 'jungle ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers && \
    usermod -aG sudo jungle

USER jungle
WORKDIR /home/jungle

CMD ["/bin/bash"]