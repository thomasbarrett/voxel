#ifndef BROWSER_H
#define BROWSER_H

extern "C" int get_pid();
extern "C" void send(void *data, size_t size);
extern "C" float random();
#endif /* BROWSER_H */