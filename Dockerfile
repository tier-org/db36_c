FROM gcc:9.5.0
COPY . /db36_c
WORKDIR /db36_c
RUN make -j