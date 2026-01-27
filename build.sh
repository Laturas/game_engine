gcc main.c -o afterhours.exe \
  -g3 -O0 \
  -I libs/include \
  libs/lib/libraylib.a \
  -lm -lpthread -ldl -lrt -lX11

./afterhours.exe