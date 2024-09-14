FROM ubuntu:20.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \ 
    apt-get install -y wget cmake build-essential git python3 python3-pip && \
    apt-get clean

RUN git clone https://github.com/emscripten-core/emsdk.git /emsdk && \
    cd /emsdk && \
    ./emsdk install latest && \
    ./emsdk activate latest && \
    echo "source /emsdk/emsdk_env.sh" >> ~/.bashrc

ENV PATH="/emsdk/emsdk_env.sh:${PATH}"

WORKDIR /app

COPY . /app

RUN /bin/bash -c "source /emsdk/emsdk_env.sh && \
    cd client && \
    emcc ../src/emscripten_main.cpp ../src/chip8.cpp ../src/gui.cpp \
    -I ../include -s ALLOW_MEMORY_GROWTH=1 -s ASSERTIONS=2 \
    -s USE_SDL=2 -s WASM=1 -s SAFE_HEAP=1 -s DISABLE_EXCEPTION_CATCHING=0 \
    -s EXPORTED_FUNCTIONS=_main,_load,_stop -s EXPORTED_RUNTIME_METHODS=ccall,cwrap \
    --no-heap-copy --preload-file ../roms --shell-file shell.html -o chip8.html"

FROM nginx:alpine

COPY --from=builder /app/client /usr/share/nginx/html

EXPOSE 80

CMD ["nginx", "-g", "daemon off;"]