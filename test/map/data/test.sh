echo 'one'
cd one
g++ code.cpp
./a.out > output.txt
diff output.txt answer.txt
cd ..

echo 'two'
cd two
g++ code.cpp
./a.out > output.txt
diff output.txt answer.txt
cd ..

echo 'three'
cd three
g++ code.cpp
./a.out > output.txt
diff output.txt answer.txt
cd ..

echo 'four'
cd four
g++ code.cpp
./a.out > output.txt
diff output.txt answer.txt
cd ..

echo 'five'
cd five
g++ code.cpp
./a.out > output.txt
diff output.txt answer.txt
cd ..

echo 'six'
cd six
g++ code.cpp
./a.out > output.txt
diff output.txt answer.txt
cd ..

echo 'seven'
cd seven
g++ code.cpp
./a.out > output.txt
diff output.txt answer.txt
cd ..
