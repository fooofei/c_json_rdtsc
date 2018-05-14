#!/bin/bash
echo "Testing rapidJSON API - start"

# test rapidJSON directly
./test_cpp &> test_cpp.txt

# test rapidJSON using C calls
./test_c &> test_c.txt

# print that functionality is being tested
printf "Testing functionality -"

# diff the files and make sure that all the shit's the same
diff test_cpp.txt test_c.txt &> diff.txt
if [ -s diff.txt ]
	then
		echo " FAILED"
	else
		echo " PASSED"
fi

# delete the text files
rm *.txt

# print that memory is being tested
printf "Testing memory leaks  - "

# print the memory usage to make sure there are no leaks
valgrind ./test_c 2> out.txt > out.txt
cat out.txt | grep "total heap usage"
rm out.txt

# print that the testing os complete
echo "Testing rapidJSON API - end"