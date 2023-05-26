FROM gcc:12.2.0
COPY . /db36_c
WORKDIR /db36_c
RUN make -j