CC=g++
bin=Jarvis
src=Jarvis.cc
lib=-ljsoncpp -lcurl -lcrypto -lpthread
INCLUDE=-I./speech

$(bin):$(src)
	$(CC) -o $@ $^ $(lib) -std=c++11 $(INCLUDE) #-static
.PHONY:clean
clean:
	rm -f $(bin)
