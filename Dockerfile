FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive
ENV WINEDEBUG=-all

RUN apt-get update \
    && apt-get install -y --no-install-recommends wine g++-mingw-w64-x86-64 \
    && mkdir -p /usr/local/lib/todo \
    && find /usr -type f \( -name 'libgcc_s_seh-1.dll' -o -name 'libstdc++-6.dll' -o -name 'libwinpthread-1.dll' \) -exec cp -n -t /usr/local/lib/todo {} + \
    && apt-get purge -y g++-mingw-w64-x86-64 \
    && apt-get autoremove -y \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace

COPY bin/todo.exe /usr/local/lib/todo/todo.exe

RUN printf '#!/bin/sh\nexec wine /usr/local/lib/todo/todo.exe "$@"\n' > /usr/local/bin/todo \
    && chmod +x /usr/local/bin/todo

CMD ["/bin/bash"]
