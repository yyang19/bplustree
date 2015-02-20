lcov --capture -b ./obj --directory ./obj --output-file coverage.info
genhtml coverage.info --output-directory out
#/local/yyang/tools/lcov/bin/lcov --capture -b ./ --directory ./ --output-file coverage.info
#/local/yyang/tools/lcov/bin/genhtml coverage.info --output-directory out
