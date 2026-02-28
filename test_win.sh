gcc test.c -o tests.exe \
  -g3 -O0 \
  -Wall -Wextra -Wpedantic \
  -I libs/include \
  libs/lib/windows/libraylib.a \
  -lm -lpthread -ldl -lgdi32 -lwinmm

./tests.exe