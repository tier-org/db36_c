FROM gcc:9.4.0
COPY . /db36_c
WORKDIR /db36_c
RUN make -j