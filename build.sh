gcc main.c -o afterhours.exe \
  -g3 -O0 \
  -Wall -Wextra -Wpedantic \
  -I libs/include \
  libs/lib/linux/libraylib.a \
  -lm -lpthread -ldl -lrt -lX11

./afterhours.exe