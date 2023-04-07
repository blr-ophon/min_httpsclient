CC = gcc
OPT = -O2
CFLAGS = -std=gnu99 -g -Wall -Wextra -pedantic $(OPT)

INCLUDES= -I./include -I./lib/min_httpclient
LIBHHL := ./lib/min_httpclient/hhl.so
LIBRARIES= -L. ${LIBHHL} -lcrypto -lssl

CFILES_DIR := ./src
BUILD_DIR := ./build

CFILES := $(wildcard ./src/*.c)
OBJECTS := $(CFILES:$(CFILES_DIR)/%.c=$(BUILD_DIR)/%.o)
EXEC := ./bin/min_https


${EXEC}: ${OBJECTS} ${LIBHHL} certs
	mkdir -p $(dir $@)
	$(CC) ${CFLAGS} ${INCLUDES} ${OBJECTS} -o $@ ${LIBRARIES}

${LIBHHL}:
	cd ./lib/min_httpclient && make 
	cd ..

${BUILD_DIR}/%.o: ${CFILES_DIR}/%.c
	mkdir -p $(dir $@)
	$(CC) ${CFLAGS} ${INCLUDES} -c $< -o $@ 

certs:
	mkdir -p ./certs
	cd ./certs && curl --etag-compare etag.txt --etag-save etag.txt --remote-name https://curl.se/ca/cacert.pem

clean:
	rm -rf ${OBJECTS} 
	rm -rf ${LIBHHL} 
	rm -rf ${EXEC}
	rm -rf ./certs

testrun: ${EXEC}
	$^ http://www.example.com

debug: ${EXEC}
	cgdb --args $^ http://www.example.com
