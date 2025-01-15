cd build/tests

if [ -z "$1" ]; then
    for test in ./test_*; do
        $test
    done
else
    ./$1
fi
