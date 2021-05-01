TargetName = slow

build: clean
	g++ ./slowjs/*.cpp -o ${TargetName} -std=c++17 

clean:
	rm -rf ./${TargetName}