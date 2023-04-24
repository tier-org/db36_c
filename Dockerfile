FROM gcc:10.4.0
COPY . /db36_c
WORKDIR /db36_c
RUN make -j