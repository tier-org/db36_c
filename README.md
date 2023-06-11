# Extreme Cardinality Database

DB36 is a database for high cardinality data. Current stage is WIP, please **do not use it**, for any questions write an email to adastreamer@gmail.com.


# Build

```
make -j

#
# to recompile: clean and build
#
make recompile
```

# Environment

```
docker run -it -v $PWD:/db36_c ubuntu bash
```

# Tests

```
./bin/db36_test ./data/file 10 4 4 1
```

# Some useful/debug commands

```
#
# Monitor sys calls
#
while true; do pid=$(pgrep 'tests/benchmark' | head -1); if [[ -n "$pid" ]]; then strace  -s 2000 -vvtf -p "$pid"; break; fi; done

#
# cached pages for the file
#
fincore data/file
dd if=data/file iflag=nocache count=0
```

# Docker build/run

```
docker build . -t db36

docker run -it --rm -v $PWD/data:/db36_c/data --memory-swappiness=0 db36 ./bin/db36_test ./data/file 32 32 32 100000

#
# test build environment
#
docker run -it --rm -v $PWD:/db36_c gcc:12.2.0 bash

```
